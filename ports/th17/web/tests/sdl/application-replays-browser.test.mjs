import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,readdirSync} from 'node:fs';import {writeFile} from 'node:fs/promises';import {resolve} from 'node:path';
import {core,memory,string,root,report,sha} from '../cpp/helpers.mjs';import {launchBrowser} from '../../../../../tools/browser/launch.mjs';import {applicationServer,applicationPage,inspectApplication} from './application-fixture.mjs';

import {expectedReplay} from './replay-playback-fixture.mjs';

test('Full application playback agrees with the native-verified coupled core for all original demonstration inputs',{timeout:900000},async()=>{
 const server=await applicationServer();let browser;const observations=[];
 try{browser=await launchBrowser();const page=await applicationPage(browser,server);for(const name of(process.env.TH17_APP_REPLAY?process.env.TH17_APP_REPLAY.split(','):['demo1.rpy','demo2.rpy','demo3.rpy'])){
  const expected=await expectedReplay(name);await page.evaluate(({name,stage})=>{fixture._application_close();initialize();fixture.FS.writeFile('/save/replay/th17_01.rpy',fixture.FS.readFile('/assets/'+name));const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('th17_01.rpy\0'),p);if(!fixture._application_test_replay(p,stage))throw Error('Replay startup failed');fixture._free(p);waitFor(()=>state()[0]===1);},expected);
  const actual=await page.evaluate(count=>{const output=[];while(world()[0]===0)step();output.push([...world().slice(0,15),...world().slice(16,18)]);for(let i=1;i<count;i++){step();output.push([...world().slice(0,15),...world().slice(16,18)]);}return output;},expected.frames.length);
  const mismatch=actual.findIndex((v,i)=>v.some((x,j)=>x!==expected.frames[i][j]));if(mismatch>=0){const info={name,stage:expected.stage,frame:mismatch,expected:expected.frames[mismatch],actual:actual[mismatch],previousExpected:expected.frames[mismatch-1],previousActual:actual[mismatch-1]};await writeFile(resolve(root,'artifacts/browser/application-replay-mismatch.json'),JSON.stringify(info,null,2));assert.fail(JSON.stringify(info));}
  observations.push({name,stage:expected.stage,frames:actual.length,stateSha256:sha(JSON.stringify(actual))});await inspectApplication(page,'original-'+name.replace('.rpy',''),observations);
 }
 report('sdl-application-replays',{passed:true,completeGame:false,wasmSha256:sha(readFileSync(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),observations,scope:'Three original demonstration inputs drive the complete application, including its fresh-run construction, deferred scene entry, production RunInput and draw/audio services. Every frame compares coupled player position/life/stocks/deaths, scoring/graze/chapter, enemy count and game RNG to StageGameplay previously checked against original native callbacks. This supplements that native comparison; it is not a complete original-game longplay or physical-mobile test.'});
 }finally{await browser?.close();await server.close();}
});
