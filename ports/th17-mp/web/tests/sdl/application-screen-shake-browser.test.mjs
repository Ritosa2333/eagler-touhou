import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdir,readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage} from './application-fixture.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';

test('Bomb and ECL screen shake reaches the actual application cameras and resets after drawing',{timeout:600000},async()=>{
 const wasmSha256=sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),server=await applicationServer(),observations=[],errors=[];let browser;let frames=0;
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});
 try{
  browser=await launchBrowser();
  for(const scale of [1,2]){
   const page=await applicationPage(browser,server,scale);page.on('pageerror',e=>errors.push(String(e)));
   for(let character=0;character<3;character++)for(let animal=0;animal<3;animal++){
    if(scale===1&&(character!==1||animal!==0))continue;
    await page.evaluate(({character,animal})=>{
     initialize();choose(0,5);choose(0,6);choose(character,7);fixture._application_title_select(animal);step(1);waitFor(()=>state()[0]===1);settle(90);
     window.shakeTrace=(ticks,held=0,pressed=0)=>{const c=fixture,p=c._malloc(ticks*16*4);try{if(!c._application_test_shake_frames(held,pressed,ticks,p))throw Error('Shake application failed '+decode(c._application_error()));const samples=Array.from({length:ticks},(_,i)=>Array.from(new Float32Array(c.HEAPU8.buffer,p+i*16*4,16)));for(const s of samples){if(!s.slice(0,2).every((v,i)=>v===s[4+i]))throw Error('Playfield camera dropped shake '+s);if(!s.slice(2,4).every((v,i)=>v===s[6+i]))throw Error('Interface camera dropped shake '+s);if(!s.slice(8,14).every(v=>v===0))throw Error('Fullscreen or completed draw retained shake '+s);}return samples;}finally{c._free(p);}};
    },{character,animal});
    assert.ok((await page.evaluate(()=>shakeTrace(1)))[0].slice(0,14).every(v=>v===0),'Idle scene has no shake');frames++;
    const samples=[];samples.push(...await page.evaluate(()=>shakeTrace(120,2,2)));frames+=120;
    if(character===1&&animal===0&&scale===2){
     await page.locator('canvas').screenshot({path:resolve(root,'artifacts/browser/shake-marisa-bomb.png')});
     await page.evaluate(()=>{step(0x100);waitFor(()=>pause()[1]===6);settle(10);});
     const paused=await page.evaluate(()=>shakeTrace(16));frames+=16;
     for(const s of paused)assert.ok(s.slice(0,14).every(v=>v===0),'Paused bomb leaves all drawing cameras still');
     await page.locator('canvas').screenshot({path:resolve(root,'artifacts/browser/shake-marisa-pause.png')});
     await page.evaluate(()=>{step(1);waitFor(()=>pause()[0]===0);});
    }
    for(let i=0;i<5;i++){samples.push(...await page.evaluate(()=>{fixture._application_test_protection(60000);return shakeTrace(120);}));frames+=120;}
    const moving=samples.filter(s=>s[0]||s[1]),maximum=Math.max(...samples.flatMap(s=>[Math.abs(s[0]),Math.abs(s[1])]));
    assert.ok(moving.length>0,character+'/'+animal+' bomb actually shakes');assert.equal(samples.at(-1)[14],0,'Expired bomb effects retire');assert.ok(samples.slice(-30).every(s=>s.slice(0,14).every(v=>v===0)),'Expired bomb has no residual shift');
    assert.equal(await page.evaluate(()=>fixture._application_test_nudge(8,6,6)),1);
    const nudge=await page.evaluate(()=>shakeTrace(16));frames+=16;assert.ok(nudge.some(s=>s[0]||s[1]),'ECL/Reimu nudge uses the same live camera path');assert.ok(nudge.slice(-4).every(s=>s.slice(0,14).every(v=>v===0)),'Nudge ends without residual shift');
    const graphics=await page.evaluate(()=>{const c=fixture,p=c._application_graphics_stats()>>>2;return Array.from(c.HEAPU32.subarray(p,p+17));});assert.equal(await page.evaluate(()=>document.querySelector('canvas').getContext('webgl2').getError()),0,'Actual WebGL rendering errors');assert.equal(graphics[7],0,'Retains WebGL full-buffer replacement');
    observations.push({scale,character,animal,frames:samples.length,movingFrames:moving.length,maximum,nudgeMovingFrames:nudge.filter(s=>s[0]||s[1]).length,graphics});
   }
   await page.close();
  }
  assert.deepEqual(errors,[]);
  report('sdl-application-screen-shake',{passed:true,wasmSha256,frames,observations,scope:'Actual SDL3/WebGL2 application: all nine character/animal bombs at 1280x960 plus Marisa at 640x480, normal bomb input, original per-layer Playfield and Interface camera displacement, unshifted fullscreen overlays, per-draw reset, natural effect retirement, steady pause/resume, and the common ECL/Reimu nudge path. Temporary draw probes observe actual renderer state without changing it. Native timing/RNG/sample parity is verified separately; this is not a physical phone GPU performance test.'});
 }finally{await browser?.close();await server.close();}
});
