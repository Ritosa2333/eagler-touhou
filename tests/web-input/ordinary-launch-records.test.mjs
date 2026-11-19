import assert from 'node:assert/strict';
import {webcrypto} from 'node:crypto';
import test from 'node:test';
import {functionSource} from './play-harness.mjs';

// Keep the read open across a scheduled frame in the production launch and tick paths.
function launch({profile=null,carryToken=0,playback=null}={}){
 const calls=[];
 let frame=0,release,owner=true,record=false;
 const observation=new Promise(resolve=>{release=()=>resolve(profile&&{profile,revision:7});});
 const module={
  _th128_world_records_restore(pointer,length){
   calls.push(['restore',frame,pointer,length]);
   if(frame!==0)return 0;
   record=true;return 1;
  },
  _th128_world_replay_begin(seed){calls.push(['begin',seed]);return record||carryToken?1:0;},
  _th128_world_replay_clock:()=>1,
  _th128_world_state:()=>'{}',
  _th128_world_tick(){calls.push(['tick',++frame]);return 1;},
  _th128_world_error:()=> 'ordinary profile restore requires a fresh natural session',
  UTF8ToString:value=>value,
 };
 const state={module,active:false,paused:true,worldMode:false,replayPlayback:playback,
  nativeScoreStore:{observe(){calls.push(['observe']);return observation;}}};
 const selection={seed:34772};
 const noop=()=>{};
 const context={state,module,selection,carryToken,shots:true,extraRun:null,thprac:null,
  pendingAudio:null,preparedEnding:null,preparedTextRasters:null,stage:{id:1},progress:0,
  terminalStageIds:new Set(),signal:new AbortController().signal,window:{},
  crypto:{randomUUID:()=>webcrypto.randomUUID()},performance,find:()=>({}),
  stale:()=>!owner,clock:{reset:noop},thpracLogicClock:{reset:noop},
  resetHighFrameratePresentation:noop,stopAudio:noop,disposeOriginalStageTextRasters:noop,
  verifyFreshExtraOwner:noop,
  nativeMainMemory:(bytes,apply)=>apply(9,bytes.length),
  pendingOriginalEndingRequest:()=>null,drainGameSounds:noop,drainMusicControl:noop,
 };
 const production=functionSource('checkWorld').replace(/\r\n/g,'\n');
 const start=production.indexOf('        if(extraRun){\n          const assets=[];');
 const end=production.indexOf('        if(state.stageIntroLoaded&&state.stageMusic)',start);
 assert.ok(start>=0&&end>start,'Missing production World handoff');
 const [finish,advanceSerial]=new Function(...Object.keys(context),
  `return [async()=>{${production.slice(start,end)}},async ${functionSource('advanceSerial')}];`
 )(...Object.values(context));
 const schedule=()=>state.active&&!state.paused?advanceSerial():undefined;
 return{calls,state,finish,schedule,release,cancel:()=>{owner=false;}};
}

test('a scheduled frame waits for ordinary profile restore and native recording at frame zero',async()=>{
 const run=launch({profile:new Uint8Array(52232)}),finished=run.finish();
 await run.schedule();
 run.release();await finished;
 assert.deepEqual(run.calls,[['observe'],['restore',0,9,52232],['begin',34772]]);
 assert.equal(run.state.nativeScoreRevision,7);
 await run.schedule();
 assert.deepEqual(run.calls.at(-1),['tick',1]);
});

test('a cancelled launch does not restore a late profile or publish a World',async()=>{
 const run=launch(),finished=run.finish();
 run.cancel();run.release();await finished;
 assert.deepEqual(run.calls,[['observe']]);
 assert.equal(run.state.active,false);
 assert.equal(run.state.worldMode,false);
});

test('a fresh profile, carried ordinary record and native playback keep distinct entry paths',async()=>{
 const fresh=launch(),finished=fresh.finish();fresh.release();await finished;
 assert.deepEqual(fresh.calls,[['observe'],['restore',0,0,0],['begin',34772]]);
 assert.equal(fresh.state.nativeScoreRevision,0);
 const carry=launch({carryToken:1});await carry.finish();
 assert.deepEqual(carry.calls,[['begin',65536]]);
 let bound=false;
 const replay=launch({playback:{bindNativeStage(){bound=true;}}});await replay.finish();
 assert.equal(bound,true);
 assert.deepEqual(replay.calls,[]);
});
