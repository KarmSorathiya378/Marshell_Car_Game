const { test } = require('node:test');
const assert = require('node:assert/strict');
const { readFileSync } = require('node:fs');
const { createHash } = require('node:crypto');
const path = require('node:path');
const vm = require('node:vm');
const root = path.resolve(__dirname,'..');
const read = file => readFileSync(path.join(root,file));
const bytes = read('VelocityX_Web/wasm/velocityx.wasm');
const hash = file => createHash('sha256').update(read(file)).digest('hex');
const textHash = file => createHash('sha256').update(read(file).toString().replace(/\r\n/g,'\n')).digest('hex');
async function engine() {
  const { instance } = await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:{proc_exit:code=>{throw new Error(`WASM exited ${code}`);}}});
  const api = instance.exports;api._initialize?.();api.vx_reset(0,0);
  return { api, state:()=>Array.from(new Float32Array(api.memory.buffer,api.vx_state(),24)) };
}
test('deployed WASM is compiled from the current source and build recipe',()=>{
  assert.equal(WebAssembly.validate(bytes),true);
  const manifest=JSON.parse(read('VelocityX_Web/wasm/manifest.json'));
  assert.equal(manifest.sourceHash,textHash('web-src/race.cpp'),'Run npm run build:wasm after editing C++');
  assert.equal(manifest.recipeHash,textHash('scripts/build-wasm.mjs'),'Rebuild after changing compiler flags');
  assert.equal(manifest.wasmHash,hash('VelocityX_Web/wasm/velocityx.wasm'));
});
test('browser scripts parse and all referenced local files and UI elements exist',()=>{
  const html=read('VelocityX_Web/index.html').toString();
  for(const [,source]of html.matchAll(/<script[^>]*src="([^"]+)"/g)) {
    assert.ok(source.startsWith('./'),'Runtime scripts must ship with the game');
    new vm.Script(read(path.join('VelocityX_Web',source)).toString());
  }
  const js=read('VelocityX_Web/app.js').toString();
  for(const [,id]of js.matchAll(/\$\('([^']+)'\)/g))assert.ok(html.includes(`id="${id}"`),`Missing UI element ${id}`);
  assert.ok(read('VelocityX_Web/style.css').length>0);
});
test('original coupe, glass and ridge meshes contain finite triangles and normals',()=>{
  for(const name of ['SM_CoupeBody','SM_CoupeGlass','SM_Ridge']) {
    const mesh=JSON.parse(read(`VelocityX_Web/assets/${name}.json`));
    assert.equal(mesh.name,name);assert.ok(mesh.positions.length>300);
    assert.equal(mesh.positions.length%9,0);assert.equal(mesh.positions.length,mesh.normals.length);
    assert.ok(mesh.positions.every(Number.isFinite));assert.ok(mesh.normals.every(Number.isFinite));
  }
});
test('WASM passes the native dynamics and race-order regressions',async()=>{
  const {api}=await engine();const failures=api.vx_verify();
  const labels=['acceleration','braking','reverse','steering','boost','skipped/backward checkpoints','duplicate checkpoints','recovery','three laps and best time','frame-rate independence'];
  for(let i=0;i<labels.length;i++)assert.equal(failures&(1<<i),0,labels[i]);
});
test('race requires Start and a three-second countdown before moving',async()=>{
  const {api,state}=await engine();
  for(let i=0;i<120;i++)api.vx_update(1/60,1,0,0,0,0);
  assert.equal(state()[3],0);assert.equal(state()[15],0);
  api.vx_start();
  for(let i=0;i<120;i++)api.vx_update(1/60,1,0,0,0,0);
  assert.ok(state()[10]>.9);assert.equal(state()[3],0);
  for(let i=0;i<180;i++)api.vx_update(1/60,1,0,0,0,0);
  assert.ok(state()[3]>3000);assert.ok(state()[1]>2500);
});
test('car selection locks after starting and reset preserves supplied best lap',async()=>{
  const {api,state}=await engine();
  api.vx_reset(2,42.5);assert.equal(state()[14],2);assert.equal(state()[8],42.5);
  api.vx_select(1);assert.equal(state()[14],1);
  api.vx_start();api.vx_select(0);assert.equal(state()[14],1);
  api.vx_reset(0,42.5);assert.equal(state()[15],0);assert.equal(state()[12],1);assert.equal(state()[13],1);assert.equal(state()[8],42.5);
});
test('recovery stops the car and never advances checkpoint progress',async()=>{
  const {api,state}=await engine();api.vx_start();
  for(let i=0;i<300;i++)api.vx_update(1/60,1,0,0,0,0);
  const gate=state()[13];api.vx_recover();
  assert.equal(state()[13],gate);assert.equal(state()[3],0);assert.equal(state()[19],0);assert.equal(state()[20],0);
});
test('invalid saved best times and long frame gaps remain bounded',async()=>{
  const {api,state}=await engine();api.vx_reset(99,NaN);
  assert.equal(state()[14],2);assert.equal(state()[8],0);
  api.vx_start();api.vx_update(Infinity,1,0,0,0,0);assert.equal(state()[10],3);
  api.vx_update(20,1,0,0,0,0);assert.ok(state()[10]>=2);
});
for(const preset of [0,1,2])test(`preset ${preset} can drive the whole circuit and finish all three laps`,async()=>{
  const {api,state}=await engine();api.vx_reset(preset,0);api.vx_start();
  // A simple steering controller drives through the real dynamics and barriers;
  // it does not move the car directly or inject checkpoint crossings.
  let s;
  for(let frame=0;frame<15000;frame++) {
    s=state();if(s[16])break;
    const angle=Math.atan2(s[1]/24000,s[0]/40000)+.07;
    const target=Math.atan2(24000*Math.sin(angle)-s[1],40000*Math.cos(angle)-s[0]);
    const error=Math.atan2(Math.sin(target-s[2]),Math.cos(target-s[2]));
    api.vx_update(1/60,s[3]<4500?1:0,s[3]>5000?1:0,Math.max(-1,Math.min(1,error*4)),0,0);
  }
  s=state();assert.equal(s[16],1);assert.equal(s[12],3);assert.equal(s[3],0);
  assert.ok(s[8]>30&&s[8]<90,'A full valid best lap should be recorded');
  assert.ok(s[6]>100&&s[6]<200,'Three actual laps must be driven');
});
