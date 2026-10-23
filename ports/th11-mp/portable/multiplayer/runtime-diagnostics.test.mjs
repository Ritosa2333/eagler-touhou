import test from 'node:test';
import assert from 'node:assert/strict';
import {copyFileSync,mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname,join,resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {webcrypto} from 'node:crypto';

// Exercise the shipped runtime and its real pinned calibration helper. Only
// the native ABI, document and clock are fixtures; no runtime source rewriting.
// The package places these three modules together, so reproduce that layout.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'../..');
const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'../../common'));
const manifest={game:'th11',product:'th11mp',variant:'multiplayer',profile:'multiplayer',
 features:{multiplayer:true},execution:{sha256:'123456789abcdef0'.repeat(4)}};

test('runtime diagnostics preserve live lifecycle and explicit hash sample identity',async t=>{
 const directory=mkdtempSync(join(tmpdir(),'th11-runtime-diagnostics-'));
 const performanceDescriptor=Object.getOwnPropertyDescriptor(globalThis,'performance');
 const cryptoDescriptor=Object.getOwnPropertyDescriptor(globalThis,'crypto');
 let now=10;
 Object.defineProperty(globalThis,'performance',{configurable:true,value:{now:()=>now}});
 if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{configurable:true,value:webcrypto});
 t.after(()=>{
  if(performanceDescriptor)Object.defineProperty(globalThis,'performance',performanceDescriptor);
  else delete globalThis.performance;
  if(cryptoDescriptor)Object.defineProperty(globalThis,'crypto',cryptoDescriptor);
  else delete globalThis.crypto;
  rmSync(directory,{recursive:true,force:true});
 });
 for(const [source,name] of [
  [join(root,'web/sdl-runtime/multiplayer.mjs'),'multiplayer.mjs'],
  [join(commonRoot,'browser/adonis-calibration.mjs'),'adonis-calibration.mjs'],
  [join(root,'portable/browser/multiplayer-replay.mjs'),'multiplayer-replay.mjs'],
 ])copyFileSync(source,join(directory,name));
 const {createMultiplayerRuntime}=await import(pathToFileURL(join(directory,'multiplayer.mjs')).href);

 async function fixture(st,{synchronousExitStop=false,replay=false}={}){
  now=10;
  const memory={buffer:new ArrayBuffer(65536)},Module={HEAPU8:new Uint8Array(memory.buffer)};
  const net=new Uint32Array(memory.buffer,64,24),game=new Uint32Array(memory.buffer,256,56);
  const rollback=new Uint32Array(memory.buffer,512,24),viewer=new Uint32Array(memory.buffer,640,12);
  net[0]=1;net[3]=101;net[4]=100;net[8]=12;net[9]=1;net[12]=7;net[20]=1;net[21]=1;
  game[0]=1;game[1]=0x12345678;game[3]=1;game[4]=98;
  rollback[0]=1;rollback[1]=1;rollback[2]=1;
  viewer[0]=1;viewer[1]=1;
  const options=replay?{replayViewer:true}:{
   netplayMode:'lan',netplayUrl:'ws://127.0.0.1:18080/?room=diagnostics&run=1',
   netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1234,netplayDifficulty:3,
   netplayInputDelay:0,netplayInputDelayAuto:false,netplayPredictionReserve:2,
   netplayAdonisMode:0,netplayPredictionLimit:12,netplayChallengeMode:true,
   netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],
  };
  const target={
   document:{hidden:false,head:{append(){}},body:{append(){}},
    createElement(){return {setAttribute(){},remove(){}};}},
   __eaglerPeerTransport:{route:'rtc',disconnected:false},
  };
  const f={net,game,rollback,viewer,target,options,phase:1,reads:0,phaseReads:0,
   exits:[],errors:[],events:[],saves:0,loopStops:0,stops:0,networkPumps:0,configureWords:null};
  let allocation=2048;
  const core={
   memory,malloc(bytes){const pointer=allocation;allocation+=(bytes+7)&~7;return pointer;},free(){},
   th11_mp_status:()=>64,th11_mp_game_status(){++f.reads;return 256;},
   th11_mp_rollback_status:()=>512,th11_mp_replay_ui_status:()=>640,
   th11_mp_calibration_status:()=>768,
   th11_phase(){++f.phaseReads;return f.phase;},
   th11_mp_error:()=>0,th11_error:()=>0,
   th11_mp_configure(pointer,count){f.configureWords=Array.from(new Uint32Array(memory.buffer,pointer,count));return 1;},
   th11_mp_connect:()=>1,th11_mp_spectator_connect:()=>1,th11_mp_start:()=>1,
   th11_mp_replay_menu:()=>1,th11_mp_replay_validate:()=>1,
   th11_mp_replay_save(){++f.saves;return 1;},
   th11_mp_pump(){++f.networkPumps;return 1;},
   th11_mp_local_visibility(){},th11_mp_always_hitbox(){},
   th11_loop_stop(){++f.loopStops;},th11_mp_stop(){++f.stops;},
  };
  f.runtime=createMultiplayerRuntime({Module,core,getOptions:()=>options,getLanguage:()=>'en',manifest,
   emit:(type,payload)=>f.events.push({type,payload}),onError:error=>f.errors.push(error),
   onExit:argument=>{
    f.exits.push(argument);
    if(synchronousExitStop)f.runtime.stop();
    return Promise.resolve();
   },target});
  st.after(()=>f.runtime.stop());
  assert.deepEqual(await f.runtime.launch(),{runLoop:true});
  if(!replay){
   assert.equal(f.configureWords.length,22);
   assert.equal(f.configureWords[0],6);
   assert.equal(f.configureWords[7],0);
   assert.equal(f.configureWords[14],0);
  }
  f.frameAt=time=>{now=time;f.runtime.frame();return target.__eaglerNetplayDiagnostics;};
  return f;
 }

 await t.test('high-rate callbacks sample once per second and identify the sampled world',async st=>{
  const f=await fixture(st);
  const first=f.frameAt(10);
  assert.equal(f.reads,1);
  assert.equal(first.nativeHash,0x12345678);
  assert.equal(first.nativeHashFrame,98);
  assert.notEqual(first.nativeHashFrame,first.frame,'world frame is not the input frontier');
  assert.equal(first.nativeHashGeneration,7);
  assert.equal(first.nativeHashSampledAtMs,10);
  assert.equal(first.nativeHashAgeMs,0);
  assert.equal(first.nativeHashCorrecting,false);
  f.game[1]=0xabcdef01;f.game[4]=130;f.rollback[3]=1;
  for(let time=18;time<1010;time+=8)f.frameAt(time);
  assert.equal(f.reads,1,'120+ callbacks must not compute 120+ world hashes');
  const before=f.frameAt(1009);
  assert.equal(before.nativeHash,0x12345678);
  assert.equal(before.nativeHashAgeMs,999);
  assert.equal(before.nativeHashCorrecting,false,'sample-time correction state is immutable');
  assert.equal(before.rollback.correcting,true,'current correction state still updates each callback');
  const due=f.frameAt(1010);
  assert.equal(f.reads,2);
  assert.equal(due.nativeHash,0xabcdef01);
  assert.equal(due.nativeHashFrame,130);
  assert.equal(due.nativeHashSampledAtMs,1010);
  assert.equal(due.nativeHashAgeMs,0);
  assert.equal(due.nativeHashCorrecting,true);
  f.rollback[3]=0;
  const next=f.frameAt(1018);
  assert.equal(next.nativeHashCorrecting,true);
  assert.equal(next.rollback.correcting,false);
  assert.equal(next.nativeHashAgeMs,8);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('a normal retirement exits immediately between hash samples',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  // Leave the complete status deliberately stale to prove phase has its own
  // live ABI read. Real production th11_phase() does not hash the world.
  f.phase=4;f.net[13]=1;
  const terminal=f.frameAt(18);
  assert.equal(f.reads,1);
  assert.equal(f.phaseReads,1);
  assert.equal(f.saves,1);
  assert.deepEqual(f.exits,[{returnToMenu:false}]);
  assert.equal(terminal.retired,true);
  assert.equal(f.runtime.acceptInput(),false);
  f.frameAt(26);
  assert.equal(f.exits.length,1);
  assert.equal(f.saves,1);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('the real calibration disconnect callback reads live confirmed terminal phase',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  assert.equal(typeof f.target.__eaglerPeerTransport.onDisconnect,'function');
  f.phase=4;f.target.__eaglerPeerTransport.disconnected=true;
  f.target.__eaglerPeerTransport.onDisconnect();
  assert.equal(f.reads,1);
  assert.equal(f.phaseReads,1);
  assert.deepEqual(f.exits,[{returnToMenu:false}]);
  assert.equal(f.loopStops,0,'normal confirmed Return is not a disconnected live game');
  assert.equal(f.networkPumps,0);
  assert.equal(f.target.__eaglerPeerTransport.onDisconnect,null,'normal exit stops calibration hooks');
  assert.equal(f.runtime.acceptInput(),false);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('an unconfirmed terminal phase still pauses instead of claiming normal completion',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  f.phase=4;f.net[4]=0xffffffff;f.target.__eaglerPeerTransport.disconnected=true;
  f.target.__eaglerPeerTransport.onDisconnect();
  assert.equal(f.reads,1);
  assert.equal(f.exits.length,0);
  assert.equal(f.loopStops,1);
  assert.equal(f.networkPumps,1);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('new generation refreshes the hash immediately and reopens the input lifecycle',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  f.phase=4;f.net[13]=1;f.frameAt(18);
  assert.equal(f.runtime.acceptInput(),false);
  f.phase=1;f.net[13]=0;f.net[12]=8;f.net[3]=4;f.net[4]=2;
  f.game[1]=0x23456789;f.game[4]=3;f.rollback[3]=1;
  const restarted=f.frameAt(26);
  assert.equal(f.reads,2,'generation change samples without waiting 1000ms');
  assert.equal(restarted.generation,8);
  assert.equal(restarted.nativeHashGeneration,8);
  assert.equal(restarted.nativeHash,0x23456789);
  assert.equal(restarted.nativeHashFrame,3);
  assert.equal(restarted.nativeHashSampledAtMs,26);
  assert.equal(restarted.nativeHashAgeMs,0);
  assert.equal(restarted.nativeHashCorrecting,true);
  assert.equal(f.runtime.acceptInput(),true);
  assert.equal(typeof f.target.__eaglerPeerTransport.onDisconnect,'function');
  assert.deepEqual(f.errors,[]);
 });

 await t.test('public full status is fresh and independent of automatic diagnostic sampling',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  const original=f.runtime.gameStatus();
  assert.equal(original.length,56);
  assert.deepEqual(original,Array.from(f.game));
  f.game[1]=0xfedcba98;f.game[4]=900;f.game[55]=0x87654321;
  const fresh=f.runtime.gameStatus();
  assert.equal(f.reads,3);
  assert.deepEqual(fresh,Array.from(f.game));
  assert.equal(original[1],0x12345678,'public reads are independent copies');
  assert.equal(fresh[55],0x87654321,'full ABI tail remains available to correctness oracles');
  const diagnostic=f.frameAt(18);
  assert.equal(f.reads,3);
  assert.equal(diagnostic.nativeHash,0x12345678,'explicit reads do not move the automatic sample deadline');
  assert.equal(diagnostic.nativeHashAgeMs,8);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('stop and same-generation launch invalidate automatic samples',async st=>{
  const f=await fixture(st);
  f.frameAt(10);
  f.runtime.stop();
  assert.equal(f.runtime.active,false);
  f.frameAt(18);
  assert.equal(f.reads,1,'a stopped callback does no status work');
  f.game[1]=0x3456789a;f.game[4]=12;
  await f.runtime.launch();
  const restarted=f.frameAt(26);
  assert.equal(f.reads,2);
  assert.equal(restarted.nativeHash,0x3456789a);
  assert.equal(restarted.nativeHashGeneration,7);
  assert.equal(restarted.nativeHashFrame,12);
  assert.equal(restarted.nativeHashSampledAtMs,26);
  assert.equal(restarted.nativeHashAgeMs,0);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('synchronous exit cleanup cannot invalidate an in-progress diagnostic publication',async st=>{
  const f=await fixture(st,{synchronousExitStop:true});
  f.frameAt(10);
  f.phase=4;f.net[13]=1;
  const terminal=f.frameAt(18);
  assert.equal(f.runtime.active,false);
  assert.equal(f.reads,1);
  assert.equal(f.stops,1);
  assert.equal(terminal.nativeHash,0x12345678);
  assert.equal(terminal.nativeHashAgeMs,8);
  assert.equal(f.exits.length,1);
  assert.deepEqual(f.errors,[]);
 });

 await t.test('Replay viewer retains its live native UI termination independent of cached hash',async st=>{
  const f=await fixture(st,{replay:true});
  f.frameAt(10);
  assert.equal(f.runtime.mode,'replay-menu');
  f.viewer[1]=2;f.viewer[2]=10;f.viewer[4]=10;
  f.frameAt(18);
  assert.equal(f.runtime.mode,'replay');
  f.viewer[1]=4;
  f.frameAt(26);
  assert.equal(f.reads,1);
  assert.equal(f.exits.length,1);
  assert.equal(f.saves,0);
  f.frameAt(34);
  assert.equal(f.exits.length,1);
  assert.deepEqual(f.errors,[]);
 });
});
