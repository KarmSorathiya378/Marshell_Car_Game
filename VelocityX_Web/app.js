/* Sunridge browser renderer. Gameplay runs in wasm/velocityx.wasm, compiled
 * from web-src/race.cpp. World layout and palette follow DriveCircuit.cpp and
 * create_drive_assets.py; the coupe and ridges are the original Blender assets. */
(() => {
  'use strict';
  const $ = id => document.getElementById(id);
  const names = ['APEX GT', 'VELOCE R', 'TITAN X'];
  const keys = new Set();
  let api, memory, scene, camera, renderer, car, sun, bodyMaterial, checkpointMarker;
  let ready = false, paused = false, panel = 'main-menu', cameraMode = 0, quality = 1, renderDirty = true;
  let lastFrame = 0, wheelSpin = 0, persistedBest = 0, audio, muted = false;
  let state, snapCamera = true, gamepadPrevious = [], transient = '', transientUntil = 0;
  const wheels = [];
  const bestKey = 'velocityx.sunridge.best.v1';
  const palette = {
    Asphalt: [[.045,.055,.066],.88,0], Sand: [[.34,.25,.15],.95,0], Rock: [[.26,.19,.14],.97,0],
    White: [[.8,.86,.88],.52,0], Red: [[.65,.035,.028],.34,.25], Concrete: [[.38,.43,.45],.85,0],
    Dark: [[.016,.024,.03],.38,.35], Cyan: [[.015,.7,.52],.35,.3,1.8], Trunk: [[.09,.065,.04],.95,0],
    Leaves: [[.08,.16,.095],.9,0], Paint: [[.02,.48,.43],.22,.65], Glass: [[.012,.026,.039],.13,.62],
    Rubber: [[.014,.017,.018],.94,0], Metal: [[.5,.55,.58],.2,.9],
    WhiteGlow: [[.7,.88,1],.3,0,5], RedGlow: [[1,.015,.005],.3,0,3]
  };
  const materials = {};
  const point = a => new THREE.Vector3(400 * Math.cos(a), 0, 240 * Math.sin(a));
  const tangent = a => new THREE.Vector3(-400 * Math.sin(a), 0, 240 * Math.cos(a)).normalize();
  const clamp = (n, lo, hi) => Math.max(lo, Math.min(hi, n));
  function readState() {
    const s = new Float32Array(memory.buffer, api.vx_state(), 24);
    return { x:s[0], y:s[1], yaw:s[2], speed:s[3], steering:s[4], boost:s[5], raceTime:s[6],
      lapTime:s[7], bestLap:s[8], lastLap:s[9], countdown:s[10], noticeTime:s[11], lap:s[12],
      nextGate:s[13], carType:s[14], started:!!s[15], finished:!!s[16], offroad:!!s[17], boosting:!!s[18], notice:s[21] };
  }
  function formatTime(seconds) {
    if (!(seconds > 0)) return '—:—.---';
    const ms = Math.floor(seconds * 1000);
    return `${Math.floor(ms / 60000).toString().padStart(2,'0')}:${Math.floor(ms / 1000 % 60).toString().padStart(2,'0')}.${(ms % 1000).toString().padStart(3,'0')}`;
  }
  function showPanel(id) {
    panel = id;
    $('overlay').hidden = !id;
    for (const name of ['main-menu','pause-menu','results']) $(name).hidden = name !== id;
    document.body.classList.toggle('racing', !id);
    if (id === 'pause-menu') $('resume-button').focus({ preventScroll:true });
    if (id === 'results') $('again-button').focus({ preventScroll:true });
  }
  function clearInput() {
    keys.clear();
    document.querySelectorAll('[data-key]').forEach(button => button.classList.remove('active'));
  }
  function start() {
    if (!ready || (state.started && !state.finished)) return;
    clearInput();
    api.vx_start(); state = readState(); paused = false;
    showPanel(null); startAudio();
    if (document.activeElement instanceof HTMLElement) document.activeElement.blur();
  }
  function setPaused(value) {
    if (!ready || !state.started || state.finished) return;
    paused = value; clearInput(); showPanel(value ? 'pause-menu' : null);
    $('pause-button').setAttribute('aria-label', value ? 'Resume game' : 'Pause game');
  }
  function restart(toMenu = false) {
    if (!ready) return;
    clearInput(); paused = false;
    api.vx_reset(state.carType, state.bestLap); state = readState(); snapCamera = true;
    showPanel('main-menu');
    if (!toMenu) start();
  }
  function selectCar(type) {
    if (!ready || state.started) return;
    api.vx_select(type); state = readState();
    renderDirty=true;
    bodyMaterial.color.setRGB(...palette[['Paint','Red','White'][type]][0]);
    document.querySelectorAll('[data-car]').forEach(button => {
      const selected = Number(button.dataset.car) === type;
      button.classList.toggle('selected', selected); button.setAttribute('aria-pressed', String(selected));
    });
  }
  function recover() {
    if (!ready || !state.started || state.finished || paused) return;
    api.vx_recover();state = readState();snapCamera = true;
  }
  function changeCamera() { cameraMode = (cameraMode + 1) % 3; snapCamera = true; }
  function notify(text) { transient = text; transientUntil = performance.now() + 2400; }
  function changeQuality() {
    if (!ready) return;
    quality = (quality + 1) % 3;
    renderer.setPixelRatio(Math.min(devicePixelRatio || 1, [1,1.5,2][quality]));
    renderer.shadowMap.enabled = quality > 0;
    renderDirty=true;
    notify(['PERFORMANCE','BALANCED','HIGH'][quality] + ' / GRAPHICS');
  }
  function startAudio() {
    try {
      if (!audio) {
        const Context = window.AudioContext || window.webkitAudioContext;
        if (!Context) return;
        const context = new Context();
        const oscillator = context.createOscillator(), gain = context.createGain();
        oscillator.setPeriodicWave(context.createPeriodicWave(new Float32Array(5), new Float32Array([0,1,.3,0,.13])));
        gain.gain.value = 0;oscillator.connect(gain);gain.connect(context.destination);oscillator.start();
        audio = { context, oscillator, gain };
      }
      audio.context.resume().catch(() => {});
    } catch { /* Audio availability never blocks driving. */ }
  }
  function updateAudio() {
    if (!audio || audio.context.state !== 'running') return;
    const now = audio.context.currentTime;
    const frequency = 45 + (Math.abs(state.speed) % 1800) / 1800 * 65 + Math.abs(state.speed) * .005;
    audio.oscillator.frequency.setTargetAtTime(frequency, now, .04);
    audio.gain.gain.setTargetAtTime(muted || paused || !state.started || state.finished ? 0 : .025 + Math.min(Math.abs(state.speed) / 120000, .04), now, .06);
  }
  function bindInput() {
    const handled = new Set(['KeyW','KeyA','KeyS','KeyD','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Space','ShiftLeft','ShiftRight','Enter','Escape','KeyP','KeyC','KeyR','F1','F5','Digit1','Digit2','Digit3']);
    document.addEventListener('keydown', event => {
      if(event.ctrlKey || event.metaKey || event.altKey) return;
      if (!handled.has(event.code)) return;
      event.preventDefault();keys.add(event.code);
      if (event.repeat || !ready) return;
      switch (event.code) {
        case 'Enter': if (paused) setPaused(false); else start(); break;
        case 'Escape': case 'KeyP': setPaused(!paused); break;
        case 'KeyC': changeCamera(); break;
        case 'KeyR': recover(); break;
        case 'F5': restart(); break;
        case 'F1': changeQuality(); break;
        case 'Digit1': case 'Digit2': case 'Digit3': selectCar(Number(event.code.at(-1))-1); break;
      }
    });
    document.addEventListener('keyup', event => { if (handled.has(event.code)) event.preventDefault(); keys.delete(event.code); });
    const loseFocus = () => { clearInput();if (ready && state.started && !state.finished) setPaused(true); };
    window.addEventListener('blur', loseFocus);
    document.addEventListener('visibilitychange', () => { if (document.hidden) loseFocus(); });
    document.querySelectorAll('[data-key]').forEach(button => {
      button.addEventListener('pointerdown', event => {
        event.preventDefault();button.setPointerCapture(event.pointerId);keys.add(button.dataset.key);button.classList.add('active');
      });
      const release = () => { keys.delete(button.dataset.key);button.classList.remove('active'); };
      button.addEventListener('pointerup', release);button.addEventListener('pointercancel', release);button.addEventListener('lostpointercapture', release);
    });
    document.querySelectorAll('[data-car]').forEach(button => button.addEventListener('click', () => selectCar(Number(button.dataset.car))));
    $('drive-button').addEventListener('click', start);
    $('resume-button').addEventListener('click', () => setPaused(false));
    $('restart-button').addEventListener('click', () => restart());
    $('again-button').addEventListener('click', () => restart());
    $('menu-button').addEventListener('click', () => restart(true));
    $('results-menu-button').addEventListener('click', () => restart(true));
    $('pause-button').addEventListener('click', () => setPaused(!paused));
    $('camera-button').addEventListener('click', changeCamera);
    $('recover-button').addEventListener('click', recover);
    $('sound-button').addEventListener('click', () => {
      muted = !muted;startAudio();$('sound-button').textContent = muted ? 'MUTED' : 'SOUND';
      $('sound-button').setAttribute('aria-pressed', String(muted));
    });
    $('fullscreen-button').addEventListener('click', async () => {
      try { if (document.fullscreenElement) await document.exitFullscreen(); else await document.documentElement.requestFullscreen(); }
      catch { notify('FULLSCREEN IS UNAVAILABLE IN THIS BROWSER'); }
    });
    if (!document.documentElement.requestFullscreen) $('fullscreen-button').hidden = true;
    function updateOrientation() {
      const overlay = $('orientation-overlay');
      if (!overlay) return;
      if (window.matchMedia && window.matchMedia('(orientation: portrait)').matches) {
        overlay.style.display = 'flex';
      } else {
        overlay.style.display = 'none';
      }
    }
    updateOrientation();
    window.addEventListener('resize', updateOrientation);
    window.addEventListener('orientationchange', updateOrientation);
    if (window.matchMedia) {
      const portraitMql = window.matchMedia('(orientation: portrait)');
      if (portraitMql.addEventListener) portraitMql.addEventListener('change', updateOrientation);
      else if (portraitMql.addListener) portraitMql.addListener(updateOrientation);
    }
  }
  function readInput() {
    const down = (...codes) => codes.some(code => keys.has(code));
    let throttle = down('KeyW','ArrowUp') ? 1 : 0, brake = down('KeyS','ArrowDown') ? 1 : 0;
    let steer = Number(down('KeyD','ArrowRight')) - Number(down('KeyA','ArrowLeft'));
    let hand = down('Space'), nitro = down('ShiftLeft','ShiftRight');
    const pad = navigator.getGamepads?.()?.find(p => p?.connected);
    if (pad) {
      throttle = Math.max(throttle,pad.buttons[7]?.value || 0);brake = Math.max(brake,pad.buttons[6]?.value || 0);
      if (Math.abs(pad.axes[0]) > .15) steer = pad.axes[0];
      hand ||= !!pad.buttons[1]?.pressed;nitro ||= !!pad.buttons[2]?.pressed;
      const pressed = index => pad.buttons[index]?.pressed && !gamepadPrevious[index];
      if (pressed(0)) { if (paused) setPaused(false); else start(); }
      if (pressed(9)) setPaused(!paused);
      if (pressed(8)) recover();if (pressed(11)) changeCamera();
      gamepadPrevious = pad.buttons.map(b => b.pressed);
    } else gamepadPrevious = [];
    return [throttle,brake,steer,Number(hand),Number(nitro)];
  }
  function material(name) {
    if (!materials[name]) {
      const [rgb,roughness,metalness,glow=0] = palette[name];
      const color = new THREE.Color(...rgb);
      materials[name] = new THREE.MeshStandardMaterial({color,roughness,metalness,
        emissive: glow ? color : 0, emissiveIntensity:glow});
    }
    return materials[name];
  }
  function geometry(data) {
    const mesh = new THREE.BufferGeometry();
    mesh.setAttribute('position',new THREE.Float32BufferAttribute(data.positions,3));
    mesh.setAttribute('normal',new THREE.Float32BufferAttribute(data.normals,3));
    mesh.computeBoundingSphere();return mesh;
  }
  // FRandomStream's LCG and mantissa conversion preserve the authored scene layout.
  function randomStream(seed) {
    const bits = new ArrayBuffer(4), integers = new Uint32Array(bits), floats = new Float32Array(bits);
    return (lo,hi) => {seed = (Math.imul(seed,196314165)+907633515)|0;integers[0] = 0x3f800000 | (seed & 0x7fffff);return lo+(hi-lo)*(floats[0]-1);};
  }
  function buildWorld(ridgeGeometry) {
    const box = new THREE.BoxGeometry(1,1,1);
    const cylinder = new THREE.CylinderGeometry(.5,.5,1,8);
    const cone = new THREE.ConeGeometry(.5,1,10);
    const batches = new Map(), transform = new THREE.Object3D();
    function place(name, pos, scale, yaw=0, shape=box) {
      const key = name + ':' + shape.uuid;
      if (!batches.has(key)) batches.set(key,{ name, shape, instances:[] });
      batches.get(key).instances.push({pos,scale,yaw});
    }
    place('Sand',[0,-1.4,0],[1500,2,1500]);
    // A continuous ribbon avoids coplanar seams between the native road boxes.
    const roadPositions=[],roadNormals=[],roadIndices=[];
    for(let i=0;i<=320;i++) {
      const a=i*2*Math.PI/320,p=point(a),t=tangent(a),n=new THREE.Vector3(-t.z,0,t.x);
      for(const side of [-1,1]) {roadPositions.push(p.x+n.x*side*12,0,p.z+n.z*side*12);roadNormals.push(0,1,0);}
      if(i<320) {const k=i*2;roadIndices.push(k,k+1,k+2,k+1,k+3,k+2);}
    }
    const roadGeometry=new THREE.BufferGeometry();
    roadGeometry.setAttribute('position',new THREE.Float32BufferAttribute(roadPositions,3));
    roadGeometry.setAttribute('normal',new THREE.Float32BufferAttribute(roadNormals,3));roadGeometry.setIndex(roadIndices);
    const roadMesh=new THREE.Mesh(roadGeometry,material('Asphalt'));roadMesh.receiveShadow=true;scene.add(roadMesh);
    for (let i=0;i<320;i++) {
      const a=i*2*Math.PI/320,b=(i+1)*2*Math.PI/320,p=point(a),q=point(b);
      const middle=p.clone().add(q).multiplyScalar(.5), t=q.clone().sub(p).normalize(), n=new THREE.Vector3(-t.z,0,t.x);
      const len=p.distanceTo(q), yaw=-Math.atan2(t.z,t.x);
      for (const side of [-1,1]) {
        for (const [name,offset,height,width,thickness] of [[i%2?'White':'Red',12.4,.02,1.2,.08],['White',11.4,.01,.12,.025],['Concrete',14.5,.6,.65,1.2]]) {
          place(name,[middle.x+n.x*side*offset,height,middle.z+n.z*side*offset],[len+.05,thickness,width],yaw);
        }
        if (i%4===0) place('Cyan',[middle.x+n.x*side*14.15,1.12,middle.z+n.z*side*14.15],[len,.12,.08],yaw);
      }
      if (i%3===0) place('White',[middle.x,.01,middle.z],[2.5,.025,.13],yaw);
    }
    for(let i=0;i<12;i++)for(let j=0;j<2;j++)place((i+j)%2?'White':'Dark',[400-11+i*2,.02,j*2],[2,.04,2]);
    for(const side of [-1,1])place('Dark',[400+side*13.9,5,0],[1,10,1]);
    place('Dark',[400,9.9,0],[29,2,1.8]);place('Cyan',[400,9,-.94],[28,.14,.08]);
    const random=randomStream(427);
    for(let i=0;i<150;i++) {
      const a=random(0,2*Math.PI),p=point(a),n=new THREE.Vector3(p.x/400,0,p.z/240).normalize();
      p.addScaledVector(n,random(65,220));const h=random(1800,7500);
      place('Rock',[p.x,h*.0035,p.z],[h*.026,h*.014,h*.022],-random(0,2*Math.PI),ridgeGeometry);
    }
    for(let i=0;i<350;i++) {
      const a=random(0,2*Math.PI),p=point(a),n=new THREE.Vector3(p.x/400,0,p.z/240).normalize();
      p.addScaledVector(n,i%3===0?-random(24,90):random(21,65));const s=random(.8,1.8);
      place('Trunk',[p.x,1.5*s,p.z],[.45*s,3*s,.45*s],0,cylinder);
      place('Leaves',[p.x,4*s,p.z],[3*s,6*s,3*s],0,cone);
      place('Leaves',[p.x,6*s,p.z],[2.2*s,4*s,2.2*s],0,cone);
    }
    for(let i=0;i<10;i++) {
      const p=point(.12+i*.018);p.x-=35;
      place('Dark',[p.x,3.3,p.z],[8,6.6,7]);place('Cyan',[p.x+4.1,4.8,p.z],[.08,.18,6]);place('White',[p.x,6.9,p.z],[9,.3,8]);
    }
    for(const {name,shape,instances} of batches.values()) {
      const mesh=new THREE.InstancedMesh(shape,material(name),instances.length);
      instances.forEach(({pos,scale,yaw},i) => {
        transform.position.set(...pos);transform.scale.set(...scale);transform.rotation.set(0,yaw,0);transform.updateMatrix();mesh.setMatrixAt(i,transform.matrix);
      });
      mesh.castShadow=!['Sand','Asphalt','White','Cyan'].includes(name);mesh.receiveShadow=true;
      mesh.instanceMatrix.needsUpdate=true;mesh.frustumCulled=false;scene.add(mesh);
    }
    const canvas=document.createElement('canvas');canvas.width=1024;canvas.height=100;
    const context=canvas.getContext('2d');context.fillStyle='#142732';context.fillRect(0,0,1024,100);
    context.fillStyle='#dcefee';context.font='48px Arial';context.textAlign='center';context.fillText('V E L O C I T Y   /   X',512,67);
    const sign=new THREE.Mesh(new THREE.PlaneGeometry(26,1.6),new THREE.MeshBasicMaterial({map:new THREE.CanvasTexture(canvas),side:THREE.DoubleSide}));
    sign.position.set(400,9.85,-.96);sign.rotation.y=Math.PI;scene.add(sign);
    checkpointMarker=new THREE.Group();
    const beacon=new THREE.Mesh(new THREE.ConeGeometry(.6,1.3,4),new THREE.MeshBasicMaterial({color:0x80e8d8}));
    beacon.rotation.z=Math.PI;beacon.position.y=5;checkpointMarker.add(beacon);scene.add(checkpointMarker);
  }
  function buildCar(bodyGeometry,glassGeometry) {
    car=new THREE.Group();scene.add(car);
    bodyMaterial=material('Paint').clone();
    const body=new THREE.Mesh(bodyGeometry,bodyMaterial);body.castShadow=true;body.receiveShadow=true;car.add(body);
    const cabin=new THREE.Mesh(glassGeometry,material('Glass'));cabin.castShadow=true;car.add(cabin);
    function part(name,position,scale,pitch=0) {
      const mesh=new THREE.Mesh(new THREE.BoxGeometry(...scale),material(name));
      mesh.position.set(...position);mesh.rotation.z=pitch;mesh.castShadow=true;car.add(mesh);return mesh;
    }
    part('Paint',[-.28,.87,0],[.8,.04,1.14]).material=bodyMaterial;
    part('Dark',[2.12,-.23,0],[.4,.09,2.03]);part('Dark',[-2.12,-.23,0],[.45,.15,1.95]);part('Dark',[-1.96,.8,0],[.55,.09,2.18],.087);
    for(const side of [-1,1]) {
      part('Dark',[-1.96,.5,side*.62],[.12,.6,.08]);part('Dark',[0,-.22,side*.95],[3.5,.12,.1]);
      part('WhiteGlow',[2.34,.12,side*.62],[.035,.1,.5]);part('RedGlow',[-2.27,.13,side*.57],[.035,.09,.6]);
      part('Dark',[1.51,.39,side*.24],[1.23,.025,.15],-.14);
    }
    const tire=new THREE.CylinderGeometry(.35,.35,.28,20);tire.rotateX(Math.PI/2);
    const rim=new THREE.CylinderGeometry(.235,.235,.295,12);rim.rotateX(Math.PI/2);
    for(let i=0;i<4;i++) {
      const pivot=new THREE.Group();pivot.position.set(i<2?1.37:-1.41,-.2,i%2?.96:-.96);car.add(pivot);
      const wheel=new THREE.Group();pivot.add(wheel);wheel.add(new THREE.Mesh(tire,material('Rubber')));wheel.add(new THREE.Mesh(rim,material('Metal')));
      wheels.push({pivot,wheel});
    }
  }
  function buildScene(assets) {
    scene=new THREE.Scene();scene.background=new THREE.Color(0x9eafb6);scene.fog=new THREE.FogExp2(0x9eafb6,.0016);
    renderer=new THREE.WebGLRenderer({antialias:true,powerPreference:'high-performance'});
    const debug=renderer.getContext().getExtension('WEBGL_debug_renderer_info');
    const software=debug && /swiftshader|llvmpipe|software/i.test(renderer.getContext().getParameter(debug.UNMASKED_RENDERER_WEBGL));
    quality=software || matchMedia('(pointer:coarse)').matches ? 0 : 1;
    renderer.domElement.className='scene';renderer.domElement.setAttribute('aria-label','3D view of Sunridge circuit');
    renderer.setPixelRatio(Math.min(devicePixelRatio||1,quality===0?1:1.5));renderer.setSize(innerWidth,innerHeight);
    renderer.outputEncoding=THREE.sRGBEncoding;renderer.toneMapping=THREE.ACESFilmicToneMapping;renderer.toneMappingExposure=1.12;
    renderer.shadowMap.enabled=quality>0;renderer.shadowMap.type=THREE.PCFSoftShadowMap;
    $('game').appendChild(renderer.domElement);
    renderer.domElement.addEventListener('webglcontextlost',event=>{event.preventDefault();fail(new Error('Graphics context lost. Reload the page to reconnect to your GPU.'));});
    camera=new THREE.PerspectiveCamera(70,innerWidth/innerHeight,.1,1800);
    scene.add(new THREE.HemisphereLight(0xc5e0ee,0x79614a,.85));
    sun=new THREE.DirectionalLight(0xffd3a2,2.1);sun.castShadow=true;sun.shadow.mapSize.set(1024,1024);
    Object.assign(sun.shadow.camera,{left:-55,right:55,top:55,bottom:-55,near:.5,far:350});sun.shadow.bias=-.0005;sun.shadow.normalBias=.06;
    scene.add(sun);scene.add(sun.target);
    buildWorld(geometry(assets[2]));buildCar(geometry(assets[0]),geometry(assets[1]));
    window.addEventListener('resize',()=>{camera.aspect=innerWidth/innerHeight;camera.updateProjectionMatrix();renderer.setSize(innerWidth,innerHeight);renderDirty=true;});
  }
  const targetCamera = { position:null, look:null };
  function updateScene(delta) {
    car.position.set(state.x/100,.57,state.y/100);car.rotation.y=-state.yaw;
    const speed=Math.abs(state.speed);
    wheelSpin+=state.speed*delta/35;
    wheels.forEach(({pivot,wheel},index)=>{pivot.rotation.y=index<2?-state.steering*.42:0;wheel.rotation.z=-wheelSpin;});
    const forward=new THREE.Vector3(Math.cos(state.yaw),0,Math.sin(state.yaw));
    if(!targetCamera.position) {targetCamera.position=new THREE.Vector3();targetCamera.look=new THREE.Vector3();}
    const distance=cameraMode===0?7.2:cameraMode===1?10.5:-1.95;
    const height=cameraMode===0?3.05:cameraMode===1?6.05:.95;
    targetCamera.position.copy(car.position).addScaledVector(forward,-distance);targetCamera.position.y+=height;
    targetCamera.look.copy(car.position).addScaledVector(forward,cameraMode===2?40:7);targetCamera.look.y+=.6;
    camera.position.lerp(targetCamera.position,snapCamera?1:1-Math.exp(-9*delta));camera.lookAt(targetCamera.look);
    const fov=70+speed/1500+(state.boosting?6:0);camera.fov+=(fov-camera.fov)*(1-Math.exp(-4*delta));camera.updateProjectionMatrix();
    sun.position.set(car.position.x-80,140,car.position.z-90);sun.target.position.copy(car.position);
    const gate=point((state.nextGate%16)*2*Math.PI/16);checkpointMarker.position.copy(gate);checkpointMarker.visible=state.started&&!state.finished;
    checkpointMarker.children[0].position.y=5+Math.sin(performance.now()*.002)*.4;
    snapCamera=false;
  }
  function updateHud() {
    const speed=Math.round(Math.abs(state.speed)*.036);
    $('speed').textContent=String(speed).padStart(3,'0');$('gear').textContent=state.speed<-10?'R':speed<1?'N':Math.min(6,Math.floor(Math.abs(state.speed)/1800)+1);
    $('car-name').textContent=names[state.carType];$('lap').innerHTML=`${state.lap} <em>/ 3</em>`;
    $('current-time').textContent=formatTime(state.lapTime);$('best-time').textContent=formatTime(state.bestLap);
    $('boost-fill').style.width=`${state.boost*100}%`;$('rpm-fill').style.width=`${Math.abs(state.speed)%1800/18}%`;
    $('checkpoint').textContent=`CHECKPOINT ${String(state.nextGate).padStart(2,'0')} / 16`;
    $('offroad').style.display=state.offroad&&state.started&&!state.finished&&!paused?'block':'none';
    const counting=state.started&&!state.finished&&!paused&&state.countdown>0;
    $('countdown').style.display=counting?'block':'none';$('countdown').textContent=Math.ceil(state.countdown);
    const message=performance.now()<transientUntil?transient:state.noticeTime>0?['','NEW PERSONAL BEST','LAP COMPLETE','RECOVERED AT LAST CHECKPOINT'][state.notice]:'';
    $('notice').textContent=message;$('notice').style.display=message?'block':'none';
    if(state.bestLap>0 && state.bestLap!==persistedBest) {
      persistedBest=state.bestLap;try{localStorage.setItem(bestKey,String(persistedBest));}catch{ /* Storage may be disabled. */ }
    }
    if(state.finished&&panel!=='results') {
      $('result-time').textContent=formatTime(state.raceTime);$('result-best').textContent=formatTime(state.bestLap);showPanel('results');clearInput();
    }
    const canvas=$('minimap'),ctx=canvas.getContext('2d'),w=canvas.width,h=canvas.height;
    ctx.clearRect(0,0,w,h);ctx.lineWidth=7;ctx.strokeStyle='#586d73';ctx.beginPath();ctx.ellipse(w/2,h/2,175,90,0,0,Math.PI*2);ctx.stroke();
    for(let i=0;i<16;i++) {const a=i*Math.PI*2/16;ctx.fillStyle='#9eb3b7';ctx.fillRect(w/2+175*Math.cos(a)-2,h/2+90*Math.sin(a)-2,4,4);}
    const a=(state.nextGate%16)*Math.PI*2/16;ctx.fillStyle='#ffc17e';ctx.fillRect(w/2+175*Math.cos(a)-5,h/2+90*Math.sin(a)-5,10,10);
    ctx.fillStyle='#83f4d9';ctx.beginPath();ctx.arc(w/2+state.x/40000*175,h/2+state.y/24000*90,6,0,Math.PI*2);ctx.fill();
  }
  function animate(now) {
    if(!ready)return;
    const delta=clamp((now-lastFrame)/1000,0,1);lastFrame=now;
    const input=readInput();
    if(!paused)api.vx_update(delta,...input);
    const wasFinished=state.finished;state=readState();
    if(state.started&&!paused&&!state.finished || renderDirty || snapCamera || wasFinished!==state.finished) {
      updateScene(delta);renderer.render(scene,camera);renderDirty=false;
    }
    updateHud();updateAudio();
    requestAnimationFrame(animate);
  }
  function fail(error) {
    ready=false;clearInput();if(audio)audio.gain.gain.value=0;
    console.error('Velocity X startup failed:',error);showPanel('main-menu');
    $('drive-button').disabled=true;$('drive-button').textContent='UNABLE TO START';
    $('load-status').textContent=`${error.message} Reload to try again. Use a browser with WebAssembly and WebGL enabled.`;
  }
  async function fetchAsset(url,kind='json') {
    const response=await fetch(url,{signal:AbortSignal.timeout(20000)});
    if(!response.ok)throw new Error(`Could not load ${url} (${response.status}).`);
    return kind==='json'?response.json():response.arrayBuffer();
  }
  async function boot() {
    try {
      if(!window.THREE)throw new Error('The bundled 3D renderer could not load.');
      if(!window.WebAssembly)throw new Error('WebAssembly is unavailable.');
      const bytes=await fetchAsset('./wasm/velocityx.wasm','bytes');
      const result=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:{proc_exit:code=>{throw new Error(`Racing engine exited (${code}).`);}}});
      api=result.instance.exports;memory=api.memory;api._initialize?.();
      if(api.vx_verify()!==0)throw new Error('The driving engine failed its integrity check.');
      $('load-status').textContent='Loading original coupe and Sunridge scenery…';
      const assets=await Promise.all(['SM_CoupeBody','SM_CoupeGlass','SM_Ridge'].map(name=>fetchAsset(`./assets/${name}.json`)));
      let best=0;try{best=Number(localStorage.getItem(bestKey))||0;}catch{ /* Private browsing can disable storage. */ }
      api.vx_reset(0,best);state=readState();persistedBest=state.bestLap;
      buildScene(assets);bindInput();ready=true;selectCar(0);updateScene(0);updateHud();renderer.render(scene,camera);
      $('drive-button').disabled=false;$('drive-button').textContent='ENTER / START YOUR ENGINE';
      $('load-status').textContent='Ready. Choose a car, then start your three-lap race.';
      // Read-only diagnostics for regression tests and browser troubleshooting.
      window.VelocityX=Object.freeze({engine:'WebAssembly',get ready(){return ready;},getState:()=>({...readState(),paused,cameraMode,quality}),getRenderInfo:()=>({calls:renderer.info.render.calls,triangles:renderer.info.render.triangles})});
      lastFrame=performance.now();requestAnimationFrame(animate);
    } catch(error) {fail(error);}
  }
  boot();
})();
