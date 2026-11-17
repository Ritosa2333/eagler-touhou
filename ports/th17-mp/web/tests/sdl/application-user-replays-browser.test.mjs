import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {core,memory,root,report,sha} from '../cpp/helpers.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage,inspectApplication} from './application-fixture.mjs';
import {expectedReplay} from './replay-playback-fixture.mjs';

test('User supplied long replays preserve every stage through the complete browser application',{timeout:3600000},async()=>{
 const paths=(process.env.TH17_USER_REPLAYS??[
  resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）/th17_01.rpy'),
  resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）/Th17_ud0134.rpy')].join('|')).split('|');
 const wasmSha256=sha(readFileSync(resolve(root,process.env.TH17_APPLICATION_RUNTIME||'artifacts/sdl-application','th17-application.wasm')));
 const server=await applicationServer(),observations=[],errors=[];let browser;
 try{
  browser=await launchBrowser();const page=await applicationPage(browser,server);page.on('pageerror',e=>errors.push(String(e)));
  for(const path of paths){
   const bytes=readFileSync(path),c=await core(),r=c.replay_create(),p=c.allocate(bytes.length),entry=c.allocate(0x158),stages=[];
   try{memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(r,p,bytes.length),1);for(let stage=1;stage<=7;stage++)if(c.replay_initial_state(r,stage,entry))stages.push(stage);}finally{c.deallocate(entry);c.deallocate(p);c.replay_delete(r);}
   for(const stage of stages.filter(s=>!process.env.TH17_USER_REPLAY_STAGES||process.env.TH17_USER_REPLAY_STAGES.split(',').map(Number).includes(s))){
    const expected=await expectedReplay(path,stage);
    await page.evaluate(({bytes,stage})=>{fixture._application_close();initialize();fixture.FS.writeFile('/save/replay/th17_01.rpy',new Uint8Array(bytes));const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('th17_01.rpy\0'),p);if(!fixture._application_test_replay(p,stage))throw Error('Replay startup failed');fixture._free(p);waitFor(()=>state()[0]===1);}, {bytes:Array.from(bytes),stage});
    let checked=0,completionFrame=null;
    await page.evaluate(()=>{while(world()[0]===0)step();});
    while(checked<expected.frames.length){
     const count=Math.min(240,expected.frames.length-checked),rows=await page.evaluate(({count,first})=>{
      const output=[];if(first)output.push([...world().slice(0,15),...world().slice(16,18)]);
      const n=count-(first?1:0),p=fixture._malloc(n*24*4);
      try{if(n&&!fixture._application_test_replay_batch(n,p))throw Error(decode(fixture._application_error()));for(let i=0;i<n;i++){const v=Array.from(fixture.HEAP32.subarray((p>>>2)+i*24,(p>>>2)+(i+1)*24));output.push([...v.slice(0,15),...v.slice(16,18)]);}}finally{fixture._free(p);}return output;
     },{count,first:checked===0});
     for(let i=0;i<rows.length;i++){
      const frame=checked+i,actual=rows[i],wanted=expected.frames[frame];
      if(stage<6&&frame===expected.frames.length-1&&actual[0]===0){
       // Application consumes the next-stage destination in this same frame;
       // the isolated scene intentionally leaves that destination to its host.
       assert.deepEqual(actual.slice(1,13),wanted.slice(1,13),'Retained player/stocks/score at stage exit');
       assert.equal((await page.evaluate(()=>state()))[2],stage+1,'Real next-stage scene constructed');
      }else if(stage===6&&actual[0]!==wanted[0]){
       // The complete application opens RetryPause at the native ending gate;
       // the isolated scene has no pause host and advances its idle counters.
       // Stocks, score, positions and RNG must still match throughout the tail.
       const current=await page.evaluate(()=>({pause:pause(),world:world()}));
       assert.equal(current.pause[0],1,'Final replay opens Pause, not a death/result error');
       completionFrame??=frame;assert.deepEqual(actual.slice(1),wanted.slice(1),'Ending tail gameplay state');
       assert.equal(actual[0],current.world[0],'Paused ending counter remains fixed');
      }else assert.deepEqual(actual,wanted,basename(path)+' stage '+stage+' frame '+frame);
     }checked+=count;
    }
    observations.push({name:basename(path),replaySha256:sha(bytes),stage,frames:checked,completionFrame,endingMenuFrames:completionFrame===null?0:checked-completionFrame,stateSha256:sha(JSON.stringify(expected.frames))});
    console.log(JSON.stringify(observations.at(-1)));await inspectApplication(page,'user-'+basename(path).replace('.rpy','')+'-stage'+stage,observations);
   }
  }
  assert.deepEqual(errors,[]);report('sdl-application-user-replays',{passed:true,wasmSha256,observations,scope:'Selected user replay stages in the complete SDL3/WebGL2 application, unmodified input and no protection/forced completion. Every gameplay frame compared to StageGameplay; final native-style RetryPause freezes the application clock while the isolated core advances its idle ending tail, whose other checked fields must still match. Audio command service runs but this parity fixture has no output device. Independent native comparisons and physical-phone performance are separate checks.'});
 }finally{await browser?.close();await server.close();}
});
