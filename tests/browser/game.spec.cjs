const { test,expect }=require('@playwright/test');
test('original Sunridge assets render, car selection works and WASM drives the car',async({page},info)=>{
  test.setTimeout(120000);
  await page.clock.install({time:new Date('2026-10-17T00:00:00Z')});
  const advance=async milliseconds=>{
    for(let elapsed=0;elapsed<milliseconds;elapsed+=100)await page.clock.fastForward(Math.min(100,milliseconds-elapsed));
  };
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  const external=[];page.on('request',request=>{if(!request.url().startsWith('http://127.0.0.1:8000/'))external.push(request.url());});
  await page.goto('/');
  await expect(page.locator('#drive-button')).toBeEnabled();
  await expect(page.locator('#load-status')).toContainText('Ready');
  // Hold simulation time between inputs so a slow CI machine cannot drive into
  // a barrier while Playwright waits for its next browser action.
  await page.clock.pauseAt(new Date('2026-10-17T00:10:00Z'));
  expect(await page.evaluate(()=>VelocityX.engine)).toBe('WebAssembly');
  expect((await page.evaluate(()=>VelocityX.getRenderInfo())).triangles).toBeGreaterThan(100000);
  await page.locator('[data-car="1"]').click();
  expect((await page.evaluate(()=>VelocityX.getState())).carType).toBe(1);
  await page.screenshot({path:info.outputPath('sunridge-menu.png')});
  await page.locator('#drive-button').click();
  await expect(page.locator('#overlay')).toBeHidden();
  await advance(3100);
  expect((await page.evaluate(()=>VelocityX.getState())).countdown).toBe(0);
  const initial=await page.evaluate(()=>VelocityX.getState());
  if(info.project.name==='mobile') {
    const gas=page.getByRole('button',{name:'Accelerate',exact:true});
    const bounds=await gas.boundingBox();
    await page.mouse.move(bounds.x+bounds.width/2,bounds.y+bounds.height/2);await page.mouse.down();
  } else await page.keyboard.down('w');
  await advance(1000);
  const moving=await page.evaluate(()=>VelocityX.getState());
  expect(moving.speed).toBeGreaterThan(1500);
  expect(Math.hypot(moving.x-initial.x,moving.y-initial.y)).toBeGreaterThan(500);
  if(info.project.name==='desktop') {
    await page.keyboard.down('Shift');
    await advance(150);
    expect((await page.evaluate(()=>VelocityX.getState())).boost).toBeLessThan(.99);
    await page.keyboard.up('Shift');
    await page.keyboard.down('d');
    await advance(200);
    expect(Math.abs((await page.evaluate(()=>VelocityX.getState())).yaw-initial.yaw)).toBeGreaterThan(.02);
    await page.keyboard.up('d');
  }
  if(info.project.name==='mobile')await page.mouse.up();else await page.keyboard.up('w');
  await page.screenshot({path:info.outputPath('sunridge-driving.png')});
  await page.locator('#pause-button').click();
  await expect(page.locator('#pause-menu')).toBeVisible();
  const paused=await page.evaluate(()=>VelocityX.getState());
  await advance(300);
  expect((await page.evaluate(()=>VelocityX.getState())).raceTime).toBe(paused.raceTime);
  await page.locator('#resume-button').click();
  await expect(page.locator('#pause-menu')).toBeHidden();
  const oldCamera=(await page.evaluate(()=>VelocityX.getState())).cameraMode;
  await page.locator('#camera-button').click();
  expect((await page.evaluate(()=>VelocityX.getState())).cameraMode).not.toBe(oldCamera);
  if(info.project.name==='mobile')await page.locator('#recover-button').click();
  else await page.keyboard.press('r');
  const recovered=await page.evaluate(()=>VelocityX.getState());
  expect(recovered.speed).toBe(0);expect(recovered.nextGate).toBe(moving.nextGate);
  await page.keyboard.down('w');
  await advance(500);
  expect((await page.evaluate(()=>VelocityX.getState())).speed).toBeGreaterThan(500);
  await page.evaluate(()=>window.dispatchEvent(new Event('blur')));
  await expect(page.locator('#pause-menu')).toBeVisible();
  const stoppedInputSpeed=(await page.evaluate(()=>VelocityX.getState())).speed;
  await page.locator('#resume-button').click();
  await advance(300);
  expect((await page.evaluate(()=>VelocityX.getState())).speed).toBeLessThan(stoppedInputSpeed-30);
  await page.keyboard.up('w');
  expect(errors).toEqual([]);
  expect(external).toEqual([]);
  expect(await page.evaluate(()=>{const event=new KeyboardEvent('keydown',{code:'KeyR',ctrlKey:true,cancelable:true});document.dispatchEvent(event);return event.defaultPrevented;})).toBe(false);
});

test('best lap survives reload and unavailable local storage does not block startup',async({page})=>{
  await page.goto('/');
  await page.evaluate(()=>localStorage.setItem('velocityx.sunridge.best.v1','42.5'));
  await page.reload();await expect(page.locator('#drive-button')).toBeEnabled();
  await expect(page.locator('#best-time')).toHaveText('00:42.500');
  await page.addInitScript(()=>Object.defineProperty(window,'localStorage',{get(){throw new Error('Storage disabled');}}));
  await page.reload();await expect(page.locator('#drive-button')).toBeEnabled();
});

test('missing WASM produces a visible error instead of an unresponsive start button',async({page})=>{
  await page.route('**/wasm/velocityx.wasm',route=>route.fulfill({status:404,body:'Not found'}));
  await page.goto('/');await expect(page.locator('#load-status')).toContainText('Could not load');
  await expect(page.locator('#drive-button')).toBeDisabled();
});

test('missing original car asset is reported',async({page})=>{
  await page.route('**/assets/SM_CoupeBody.json',route=>route.fulfill({status:404,body:'Not found'}));
  await page.goto('/');await expect(page.locator('#load-status')).toContainText('SM_CoupeBody');
  await expect(page.locator('#drive-button')).toBeDisabled();
});

test('WebGL initialization failure is reported',async({page})=>{
  await page.addInitScript(()=>{const original=HTMLCanvasElement.prototype.getContext;HTMLCanvasElement.prototype.getContext=function(type,...args){return /webgl/i.test(type)?null:original.call(this,type,...args);};});
  await page.goto('/');await expect(page.locator('#load-status')).toContainText('Error creating WebGL context');
  await expect(page.locator('#drive-button')).toBeDisabled();
});
