import { spawnSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { fileURLToPath } from 'node:url';
import path from 'node:path';
const root = fileURLToPath(new URL('../', import.meta.url));
const windows = process.platform === 'win32';
const localSdk = path.join(root, '.local/emsdk/upstream/emscripten');
const compiler = process.env.EMXX || (existsSync(localSdk) ? path.join(localSdk, windows ? 'em++.exe' : 'em++') : 'em++');
const shell = windows && /\.(bat|cmd)$/i.test(compiler);
const version = spawnSync(compiler,['--version'],{cwd:root,encoding:'utf8',shell});
if(version.status!==0 || !/\b6\.0\.12\b/.test(version.stdout || '')) {
  console.error('This build requires Emscripten 6.0.12. Install and activate that SDK, or set EMXX to its compiler.');
  process.exit(1);
}
mkdirSync(path.join(root, 'VelocityX_Web/wasm'), { recursive: true });
const result = spawnSync(compiler, [
  'web-src/race.cpp', '-std=c++17', '-O3', '-Wall', '-Wextra', '-Werror', '--no-entry', '-sSTANDALONE_WASM=1',
  '-sFILESYSTEM=0', '-sSUPPORT_LONGJMP=0', '-sINITIAL_MEMORY=131072', '-sSTACK_SIZE=32768',
  '-sEXPORTED_FUNCTIONS=["_vx_reset","_vx_start","_vx_select","_vx_recover","_vx_update","_vx_state","_vx_verify"]',
  '-o', 'VelocityX_Web/wasm/velocityx.wasm'
], { cwd: root, stdio: 'inherit', shell });
if (result.error) console.error('Install Emscripten 6.0.12 and activate it first.', result.error.message);
if (result.status === 0) {
  const hash = file => createHash('sha256').update(readFileSync(path.join(root,file))).digest('hex');
  const textHash = file => createHash('sha256').update(readFileSync(path.join(root,file),'utf8').replace(/\r\n/g,'\n')).digest('hex');
  writeFileSync(path.join(root,'VelocityX_Web/wasm/manifest.json'), JSON.stringify({
    compiler:'Emscripten 6.0.12',sourceHash:textHash('web-src/race.cpp'),recipeHash:textHash('scripts/build-wasm.mjs'),
    wasmHash:hash('VelocityX_Web/wasm/velocityx.wasm')
  },null,2)+'\n');
}
process.exit(result.status ?? 1);
