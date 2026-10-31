import test from 'node:test';import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';import {resolve} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {releaseServer} from '../../../th09_web/scripts/release-server.mjs';
import {runtimeFrame,rpc} from './launcher-fixture.mjs';import {root,report} from '../cpp/helpers.mjs';

test('Accepted production site starts, plays, pauses and returns with title music on desktop and mobile',{timeout:900000},async()=>{
 const site=resolve(root,process.env.TH17_BROWSER_SITE||'artifacts/sdl-release/site'),manifest=JSON.parse(await readFile(resolve(site,'manifest.json'),'utf8'));
 assert.equal(manifest.completeGame,true);assert.equal(manifest.development,false);const publicUrl=process.env.TH17_PUBLIC_TEST_URL,server=publicUrl?null:await releaseServer({root:site,port:0}),origin=publicUrl||server.url;let browser;const observations=[];
 try{
  if(publicUrl){const response=await fetch(new URL('manifest.json',origin),{headers:{'Cache-Control':'no-cache'}});assert.equal(response.status,200);const remote=await response.json();assert.equal(remote.game,'th17');assert.equal(remote.development,false);assert.equal(remote.version,manifest.version);assert.equal(remote.execution.sha256,manifest.execution.sha256);}
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  for(const mobile of publicUrl?[true]:[false,true]){
   const context=await browser.newContext({viewport:mobile?{width:960,height:540}:{width:1280,height:960},hasTouch:mobile,isMobile:mobile,deviceScaleFactor:mobile?2:1,...(mobile?{userAgent:'Mozilla/5.0 (Linux; Android 14; Pixel 8) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Mobile Safari/537.36'}:{})}),page=await context.newPage(),errors=[];
   const transfers=new Map(),failedRequests=[];page.on('response',r=>{if(/\/packages\/|\/runtime\/th17\//.test(r.url()))transfers.set(r.request(),{url:r.url(),status:r.status(),respondedAt:Date.now()});});page.on('requestfinished',r=>{const v=transfers.get(r);if(v)v.finishedAt=Date.now();});page.on('requestfailed',r=>failedRequests.push({url:r.url(),error:r.failure()?.errorText}));
   page.on('pageerror',e=>errors.push(String(e)));await page.addInitScript(mobile=>{localStorage.setItem('eagler-touhou-game-options-v1-th17',JSON.stringify({music:'midi',musicPreferenceExplicit:true,options:{touchEnabled:mobile,touchMovementMode:'touch',renderScale:2}}));localStorage.setItem('eagler-touch-help-seen-v8','1');window.runtimeEvents=[];window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'&&e.data.game==='th17')runtimeEvents.push(e.data);});},mobile);
   await page.goto(origin,{waitUntil:'domcontentloaded',timeout:publicUrl?120000:30000});await page.locator('#changelogConfirm').click();await page.locator('button[data-game="th17"]').click();await page.locator('#launch').click();
   try{
    // A fresh public profile downloads the complete original resource package.
    // Fail immediately on a runtime error; retain transfer state on a timeout.
    await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='first-frame'||e.event==='error'),null,{timeout:publicUrl?300000:150000});
    assert.deepEqual(await page.evaluate(()=>runtimeEvents.filter(e=>e.event==='error')),[],'Runtime startup errors');
   }catch(error){
    const diagnostic={origin,manifestVersion:manifest.version,errors,transfers:[...transfers.values()],failedRequests,visibleText:await page.locator('body').innerText(),events:await page.evaluate(()=>runtimeEvents.slice(-16)),frames:page.frames().map(f=>f.url())};
    const path=resolve(root,'artifacts/browser/production-startup-'+(publicUrl?'public-':'')+(mobile?'mobile':'desktop'));await writeFile(path+'.json',JSON.stringify(diagnostic,null,2)+'\n');await page.screenshot({path:path+'.png'});throw error;
   }
   const runtime=runtimeFrame(page);assert.ok(runtime);
   assert.equal(await page.evaluate(()=>getComputedStyle(document.body,'::before').display),'none','Launcher texture must not cover the game');
   const waitTicks=async n=>{const start=await runtime.evaluate(()=>Module._th17_frame());await runtime.waitForFunction(({start,n})=>Module._th17_frame()>=start+n,{start,n},{timeout:150000});};
   const key=async code=>{await rpc(page,'keyboard',{code,down:true});await waitTicks(3);await rpc(page,'keyboard',{code,down:false});await waitTicks(3);};
   const audio=()=>runtime.evaluate(()=>{const c=Module,p=c._th17_audio_statistics()/4;return Array.from(c.HEAPU32.subarray(p,p+11));});
   await waitTicks(160);assert.equal(await runtime.evaluate(()=>Module._th17_phase()),0);assert.equal((await audio())[5],0,'Title track');
   const health=await page.evaluate(()=>runtimeEvents.filter(e=>e.event==='frame-health').at(-1));assert.ok(health&&Object.hasOwn(health,'gpuMs'),'Asynchronous GPU timing is wired into the real runtime');assert.ok(health.gpuMs===null||Number.isFinite(health.gpuMs)&&health.gpuMs>=0);
   assert.equal(await runtime.evaluate(()=>typeof Module._th17_probe_state),'undefined');assert.equal(await runtime.evaluate(()=>typeof Module._application_state),'undefined');assert.deepEqual(await runtime.locator('canvas').evaluate(c=>[c.width,c.height]),[1280,960]);
   for(let screen=0;screen<4;screen++){await key('KeyZ');if(screen<3)await waitTicks(55);}
   await runtime.waitForFunction(()=>Module._th17_phase()===1,null,{timeout:150000});await waitTicks(165);const gameAudio=await audio();assert.equal(gameAudio[0],1);assert.equal(gameAudio[3],0);assert.equal(gameAudio[5],1,'Stage one track');assert.ok(gameAudio[2]>0,'PCM was queued');
   if(mobile){await page.locator('#touchBomb').tap();await page.locator('#touchEscape').tap();}else await key('Escape');
   await runtime.waitForFunction(()=>Module._th17_phase()===2,null,{timeout:60000});await waitTicks(40);await page.screenshot({path:resolve(root,'artifacts/browser/production-'+(mobile?'mobile':'desktop')+'-pause.png')});
   await key('Escape');await runtime.waitForFunction(()=>Module._th17_phase()===1,null,{timeout:60000});await waitTicks(35);await key('Escape');await waitTicks(40);await key('KeyQ');
   await runtime.waitForFunction(()=>Module._th17_phase()===0,null,{timeout:150000});await waitTicks(100);const returnedAudio=await audio();assert.equal(returnedAudio[5],0,'Title music restored');
   await rpc(page,'sync');const files=(await rpc(page,'list')).files;assert.ok(files.some(f=>f.path==='scoreth17.dat'));assert.ok(files.some(f=>f.path==='th17.cfg'));
   const fatal=await page.evaluate(()=>runtimeEvents.filter(e=>e.event==='error'));assert.deepEqual(fatal,[]);assert.deepEqual(errors,[]);observations.push({mobile,canvas:[1280,960],gameAudio,returnedAudio,files});await context.close();
  }
  report(publicUrl?'production-launcher-public':'production-launcher',{passed:true,origin,manifestVersion:manifest.version,wasmSha256:manifest.execution.sha256,observations,scope:'Actual accepted production Wasm, final launcher and allowlisted HTTP server with CSP. Real RAF, desktop/mobile viewport start/selection/gameplay, SDL PCM queue, pause/resume, Q return, title BGM restoration and player-file persistence. Public mode uses the HTTPS tunnel and mobile viewport. Developer probes are absent. Chromium software rendering does not measure phone GPU performance.'});
 }finally{await browser?.close();if(server){server.netplay.close();server.server.closeAllConnections();await new Promise(r=>server.server.close(r));}}
});
