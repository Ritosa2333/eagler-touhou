import test from 'node:test';import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';import {resolve} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage,inspectApplication} from './application-fixture.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';

test('Every real stage runs through its bosses and exit with production C++ draw/audio and shared WebGL2',{timeout:7200000},async()=>{
 const server=await applicationServer();let browser;const observations=[],summaries=[],errors=[],extraOnly=process.env.TH17_TEST_EXTRA_ONLY==='1';
 try{
  browser=await launchBrowser();const page=await applicationPage(browser,server);page.on('pageerror',e=>errors.push(String(e)));
  if(!extraOnly)await page.evaluate(()=>{choose(0,5);choose(1,6);choose(0,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);});
  for(const stage of extraOnly?[7]:[1,2,3,4,5,6,7]){
   // Unlock while away from the main menu, then re-enter it as the real cheat
   // flow does. Mutating records after Main was built leaves its test cursor's
   // disabled-Extra list intact and selects Practice instead.
   if(stage===7)await page.evaluate(()=>{fixture._application_close();initialize();choose(5,10);fixture._application_unlock();step(0x102);ready(1);settle(30);choose(1,5);choose(0,6);choose(0,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);});
   assert.equal((await page.evaluate(()=>state()))[2],stage);let ticks=0,maxEnemies=0,maxBullets=0,maxBatchMs=0;
   for(;ticks<120000;ticks+=120){
    const value=await page.evaluate(()=>{const begin=performance.now();if(!fixture._application_test_combat_batch(120))throw Error(decode(fixture._application_error()));const p=fixture._application_graphics_stats()/4;return{state:state(),world:world(),stats:Array.from(fixture.HEAPU32.subarray(p,p+17)),milliseconds:performance.now()-begin};});
    maxEnemies=Math.max(maxEnemies,value.world[14]);maxBullets=Math.max(maxBullets,value.world[15]);maxBatchMs=Math.max(maxBatchMs,value.milliseconds);assert.equal(value.stats[7],0,'GL errors in stage '+stage);assert.equal(value.state[14],0,'Protected real script must not enter game-over');
    if(ticks%6000===0){console.log(JSON.stringify({stage,ticks,state:value.state.slice(0,7),enemies:value.world[14],bullets:value.world[15]}));await inspectApplication(page,'full-stage-'+stage+'-tick-'+ticks,observations);}
    if(value.state[0]!==1||value.state[2]!==stage)break;
   }
   const state=await page.evaluate(()=>state());assert.ok(ticks<120000,'Real stage exits '+stage);assert.ok(maxEnemies>0,'Real enemies '+stage);
   if(stage<6)assert.equal(state[2],stage+1);else assert.equal(state[0],stage===6?2:0,'Normal ending / Extra result destination');
   const summary={stage,ticks:ticks+120,maxEnemies,maxBullets,maxBatchMs,destination:state};summaries.push(summary);console.log(JSON.stringify(summary));
   if(stage===6)report('sdl-application-full-stages-normal',{passed:true,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),summaries:[...summaries],observations:[...observations],scope:'Actual contiguous six-stage update/draw/audio run through real bosses to Ending. Explicit player protection; no native no-cheat replay or physical phone FPS claim.'});
  }
  assert.deepEqual(errors,[]);const wasmSha256=sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm')));
  if(extraOnly){report('sdl-application-full-stages-extra',{passed:true,wasmSha256,summaries,observations,scope:'Actual complete Extra update/draw/audio run. Explicit player protection; no native no-cheat replay or phone FPS claim.'});const normal=JSON.parse(await readFile(resolve(root,'artifacts/cpp/verification/sdl-application-full-stages-normal.json'),'utf8'));assert.equal(normal.passed,true);assert.equal(normal.wasmSha256,wasmSha256,'Resumed normal/Extra rendering build identity');summaries.unshift(...normal.summaries);observations.unshift(...normal.observations);}
  assert.deepEqual(summaries.map(s=>s.stage),[1,2,3,4,5,6,7]);report('sdl-application-full-stages',{passed:true,wasmSha256,summaries,observations,scope:'Normal six-stage contiguous run and Extra through the actual ECL bosses and exits, all update/draw/audio services and shared SDL3/WebGL2. Explicit invulnerability bypasses deaths; input is scripted, so this is not a native no-cheat replay-parity claim. No rendering is skipped. Browser-window presents are combined per 120 updates for test speed. Software GPU timings are not physical-phone frame rates.'});
 }finally{await browser?.close();await server.close();}
});
