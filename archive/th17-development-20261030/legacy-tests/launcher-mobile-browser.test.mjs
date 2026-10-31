import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {root,report} from '../cpp/helpers.mjs';
import {launcherServer,runtimeFrame,rpc} from './launcher-fixture.mjs';

for(const profile of ['android','ios'])test('Mobile '+profile+' launcher DOM delivers touch, actions, pause and both resolutions',{timeout:900000},async()=>{
 const server=await launcherServer(),errors=[],observations=[];let browser;
 try{
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  const userAgent=profile==='android'?'Mozilla/5.0 (Linux; Android 14; Pixel 8) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Mobile Safari/537.36':'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Mobile/15E148 Safari/604.1';
  const context=await browser.newContext({viewport:{width:960,height:540},deviceScaleFactor:2,hasTouch:true,isMobile:true,userAgent});
  const page=await context.newPage();page.on('pageerror',e=>errors.push(String(e)));
  await page.addInitScript(()=>{
   if(!localStorage.getItem('eagler-touhou-game-options-v1-th17'))localStorage.setItem('eagler-touhou-game-options-v1-th17',JSON.stringify({music:'midi',musicPreferenceExplicit:true,options:{touchEnabled:true,touchMovementMode:'touch',touchFocusMode:'hold-button',touchSensitivity:100}}));
   localStorage.setItem('eagler-touch-help-seen-v8','1');
   window.runtimeEvents=[];window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'&&e.data.game==='th17')runtimeEvents.push(e.data);});
  });
  await page.goto(server.url);await page.locator('#changelogConfirm').tap();await page.locator('button[data-game="th17"]').tap();
  assert.match(await page.locator('#renderScaleValue').textContent(),/1280/);
  // Exercise the visible option as well as its saved value; the second launch
  // below checks that this controls the actual backing buffer, independent of DPR.
  await page.locator('#renderScaleToggle').tap();assert.match(await page.locator('#renderScaleValue').textContent(),/640/);
  await page.locator('#renderScaleToggle').tap();assert.match(await page.locator('#renderScaleValue').textContent(),/1280/);
  await page.locator('#launch').tap();
  await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='first-frame'),null,{timeout:150000});
  const runtime=runtimeFrame(page);assert.ok(runtime);await runtime.waitForFunction(()=>Module._th17_frame()>=145,null,{timeout:60000});
  await runtime.evaluate(()=>Module._th17_loop_stop());
  // Android dispatches directly to the game canvas. The iOS layout routes
  // through the host surface to keep fixed action buttons above the iframe.
  const surface=profile==='ios'?page.locator('#touchDirectSurface'):runtime.locator('canvas');
  const advance=n=>runtime.evaluate(n=>{const c=Module;if(!c._th17_probe_ticks(n)){const p=c._th17_error();throw Error(new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))));}const p=c._th17_probe_state();return Array.from(c.HEAP32.subarray(p/4,p/4+16));},n);
  const menu=async screen=>{for(let i=0;i<240;i++){const s=await advance(1);if(s[0]===0&&s[8]===screen&&s[9]===2)return s;}throw Error('Missing mobile menu '+screen);};
  const touchTap=async()=>{await surface.tap({position:{x:380,y:260}});await rpc(page,'list');return advance(5);};
  await menu(1);await touchTap();await menu(5);await touchTap();await menu(6);await touchTap();await menu(7);await touchTap();
  let s;for(let i=0;i<240;i++){s=await advance(1);if(s[0]===1&&s[5]>125)break;}assert.equal(s[0],1);assert.ok(s[5]>125);
  assert.ok(await surface.isVisible());for(const id of ['touchBomb','touchFire','touchFocus','touchEscape'])assert.ok(await page.locator('#'+id).isVisible(),id+' visible on phone');
  assert.deepEqual(await runtime.locator('canvas').evaluate(c=>[c.width,c.height]),[1280,960]);
  const player=()=>runtime.evaluate(()=>{const c=Module,p=c._th17_probe_player_state();return Array.from(new Float32Array(c.HEAPU8.buffer,p,10));});
  const world=()=>runtime.evaluate(()=>{const c=Module,p=c._th17_probe_world_state()/4;return Array.from(c.HEAPU32.subarray(p,p+16));});
  const cdp=await context.newCDPSession(page),b=await surface.boundingBox();assert.ok(b);
  const contact={id:1,x:b.x+b.width*.40,y:b.y+b.height*.65,radiusX:3,radiusY:3,force:1};
  const before=await player();await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[contact]});await rpc(page,'list');
  await cdp.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{...contact,x:contact.x+42,y:contact.y-19}]});await rpc(page,'list');
  await advance(12);const after=await player();assert.ok(after[0]-before[0]>8,'Finger drag changes x continuously');assert.ok(after[1]-before[1]<-4,'Finger drag changes y continuously');
  assert.ok(Math.abs((after[0]-before[0])/(after[1]-before[1]))>1.2,'Touch keeps a non-eight-way slope');
  await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});await rpc(page,'list');await advance(8);const released=await player();assert.equal(released[0],after[0]);assert.equal(released[1],after[1]);
  const focus=await page.locator('#touchFocus').boundingBox(),fp={id:2,x:focus.x+focus.width/2,y:focus.y+focus.height/2};
  await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[fp]});await rpc(page,'list');await advance(1);assert.ok((await player())[6]&8,'Held low-speed DOM button reaches game');
  await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});await rpc(page,'list');await advance(1);assert.equal((await player())[6]&8,0,'Releasing focus clears game input');
  await page.locator('#touchFire').tap();await rpc(page,'list');await advance(1);assert.equal((await player())[6]&1,0);await page.locator('#touchFire').tap();await rpc(page,'list');await advance(1);assert.ok((await player())[6]&1);
  const bombs=(await world())[10];await page.locator('#touchBomb').tap();await rpc(page,'list');await advance(5);assert.equal((await world())[10],bombs-1,'Visible B button consumes one bomb');
  await page.locator('#touchEscape').tap();await rpc(page,'list');s=await advance(40);assert.equal(s[14],1);const paused=await advance(20);assert.equal(paused[5],s[5]);
  await page.screenshot({path:resolve(root,'artifacts/browser/launcher-mobile-'+profile+'-pause.png')});
  await page.locator('#touchEscape').tap();await rpc(page,'list');s=await advance(40);assert.equal(s[14],0);
  await page.evaluate(async()=>{if(document.fullscreenElement)await document.exitFullscreen();});
  await page.setViewportSize({width:540,height:960});await rpc(page,'list');await advance(2);
  for(const id of ['touchBomb','touchFire','touchFocus','touchEscape']){const box=await page.locator('#'+id).boundingBox();assert.ok(box&&box.x>=0&&box.y>=0&&box.x+box.width<=542&&box.y+box.height<=962,id+' fits portrait viewport');}
  await page.screenshot({path:resolve(root,'artifacts/browser/launcher-mobile-'+profile+'-portrait.png')});
  observations.push({kind:'mobile-dom-input',profile,before,after,released,bombs,canvas:[1280,960],portrait:[540,960]});
  // Reload with a durable 640 setting after the normal player-file sync.
  await rpc(page,'sync');
  await page.evaluate(()=>{const key='eagler-touhou-game-options-v1-th17',v=JSON.parse(localStorage.getItem(key));v.options.renderScale=1;localStorage.setItem(key,JSON.stringify(v));});
  await page.reload();await page.locator('button[data-game="th17"]').tap();assert.match(await page.locator('#renderScaleValue').textContent(),/640/);await page.locator('#launch').tap();
  await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='first-frame'),null,{timeout:150000});
  const low=runtimeFrame(page);await low.waitForFunction(()=>Module._th17_frame()>=5,null,{timeout:60000});assert.deepEqual(await low.locator('canvas').evaluate(c=>[c.width,c.height]),[640,480]);
  assert.deepEqual(errors,[]);const manifest=JSON.parse(await readFile(resolve(server.site,'manifest.json'),'utf8'));
  report('sdl-launcher-mobile-'+profile,{passed:true,manifestVersion:manifest.version,wasmSha256:manifest.execution.sha256,observations,scope:'Chromium phone viewport/DPR and actual CDP touch events through the visible final launcher and its iframe. Native-speed arbitrary-angle movement, focus hold/release, firing, bombs, pause/resume, portrait resize and both render backing resolutions. Physical phone GPU performance remains device testing.'});
 }catch(e){await writeFile(resolve(root,'artifacts/browser/launcher-mobile-'+profile+'-errors.json'),JSON.stringify({errors,observations,error:String(e)},null,2));throw e;}
 finally{await browser?.close();await server.close();}
});
