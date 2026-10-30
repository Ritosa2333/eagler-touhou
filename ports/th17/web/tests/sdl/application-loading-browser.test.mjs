import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdir,readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage} from './application-fixture.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';

test('Original loading artwork is presented for all fresh entrances and retries without consuming gameplay or replay frames',{timeout:600000},async()=>{
 const server=await applicationServer(),observations=[],errors=[];let browser;
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});
 try{
  browser=await launchBrowser();
  for(const scale of [1,2]){
   const page=await applicationPage(browser,server,scale);page.on('pageerror',e=>errors.push(String(e)));
   await page.evaluate(()=>{
    window.loading=()=>{const p=fixture._application_loading_state()>>>2;return Array.from(fixture.HEAP32.subarray(p,p+8));};
    window.loadingPixels=()=>{const c=fixture,p=c._application_pixels();let purple=0,white=0,red=0,black=0;for(let y=0;y<480;y++)for(let x=0;x<640;x++){const i=p+(y*640+x)*4,b=c.HEAPU8[i],g=c.HEAPU8[i+1],r=c.HEAPU8[i+2];if(x<400&&y<350){if(r>g*1.3&&b>g*1.3&&r>20&&b>20)purple++;if(r+g+b<12)black++;}if(x>360&&y>340){if(r>180&&g>180&&b>180)white++;if(r>70&&r>g*2&&r>b*1.5)red++;}}return{purple,white,red,black};};
    window.waitLoading=()=>waitFor(()=>loading()[0]===1);
    window.checkFrozenLoading=()=>{
     const initial=world().slice(0,20),recording=fixture._application_test_recording_frames(),replay=fixture._application_test_replay_stage();let ticks=0;
     while(loading()[0]&&loading()[2]<loading()[3]-1){step();ticks++;if(JSON.stringify(world().slice(0,20))!==JSON.stringify(initial))throw Error('Loading advanced gameplay '+world()+' initial='+initial);if(fixture._application_test_recording_frames()!==recording||fixture._application_test_replay_stage()!==replay)throw Error('Loading advanced replay');}
     return {ticks,loading:loading(),pixels:loadingPixels(),state:state(),recording};
    };
    window.restartRun=()=>{step(0x100);waitFor(()=>pause()[1]===6);step(0x10);step(1);if(!state()[12]||!fixture._application_test_replay_stage()){waitFor(()=>pause()[1]===7&&pause()[6]>=30);step(0x10);step(1);}waitLoading();};
   });
   const observe=async kind=>{
    const observed=await page.evaluate(()=>checkFrozenLoading());
    const screenshot=resolve(root,`artifacts/browser/loading-${scale}-${kind}.png`);await page.locator('canvas').screenshot({path:screenshot});
    observations.push({scale,kind,...observed,screenshotSha256:sha(await readFile(screenshot))});
    assert.ok(observed.pixels.purple>70000,kind+' original purple background covers the game');
    assert.ok(observed.pixels.white>100,kind+' white original Now Loading text visible');
    assert.ok(observed.pixels.red>40,kind+' original red footsteps visible');
    await page.evaluate(()=>{waitFor(()=>!loading()[0]);settle(120);});
    const stats=await page.evaluate(()=>{const p=fixture._application_graphics_stats()>>>2;return Array.from(fixture.HEAPU32.subarray(p,p+17));});assert.equal(stats[7],0,kind+' WebGL errors');
   };
   await page.evaluate(()=>{choose(0,5);choose(0,6);choose(0,7);fixture._application_title_select(0);step(1);waitLoading();});await observe('start');
   await page.evaluate(()=>restartRun());await observe('restart');
   assert.equal((await page.evaluate(()=>state()))[2],1);
   await page.evaluate(()=>{step(0x100);waitFor(()=>pause()[1]===6);step(0x10000);waitLoading();});await observe('return-title');
   assert.equal((await page.evaluate(()=>state()))[0],0);
   await page.evaluate(()=>{fixture._application_close();initialize();fixture._application_unlock();choose(2,5);choose(0,6);choose(1,7);choose(1,8);fixture._application_title_select(3);step(1);waitLoading();});await observe('practice');
   assert.equal((await page.evaluate(()=>state()))[2],4);
   await page.evaluate(()=>{fixture._application_close();initialize();fixture._application_unlock();choose(3,18);choose(0,19);choose(0,20);fixture._application_title_select(0);step(1);waitLoading();});await observe('spell-practice');
   await page.evaluate(()=>{fixture._application_close();initialize();fixture._application_unlock();choose(1,5);choose(0,6);choose(0,7);fixture._application_title_select(0);step(1);waitLoading();});await observe('extra');
   assert.equal((await page.evaluate(()=>state()))[2],7);
   await page.evaluate(()=>{fixture._application_close();initialize();fixture.FS.writeFile('/save/replay/th17_01.rpy',fixture.FS.readFile('/assets/demo1.rpy'));const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('th17_01.rpy\0'),p);if(!fixture._application_test_replay(p,3))throw Error('Original replay startup failed');fixture._free(p);waitLoading();});await observe('original-replay');
   await page.evaluate(()=>{step(0x100);waitFor(()=>pause()[1]===6);step(0x10);step(1);waitLoading();});await observe('replay-restart');
   await page.close();
  }
  assert.deepEqual(errors,[]);
  report('sdl-application-loading',{passed:true,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),observations,scope:'Actual SDL3/WebGL2 application at 640x480 and 1280x960: normal start/restart, return to title, Stage Practice, Spell Practice, Extra, native replay and replay retry. Screenshots and GPU pixels require original patterned background, loading logo and animal footsteps. Loading must leave gameplay counters, positions, stocks, game/visual RNG and recording clocks frozen. Browser rendering is not a physical phone GPU test.'});
 }finally{await browser?.close();await server.close();}
});
