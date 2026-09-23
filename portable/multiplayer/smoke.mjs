import createModule from '/th10-sdl.mjs';
import {allKeyboardInputs,reachedNativeStage} from '/replay-robot.mjs';
let core,wasmIdentity;
const assetIdentities={};
const Module=await createModule({canvas:document.getElementById('screen'),noInitialRun:true,
 instantiateWasm(imports,ready){return fetch('/th10-sdl.wasm').then(async response=>{
  if(!response.ok)throw Error('WASM fetch failed '+response.status);
  const bytes=await response.arrayBuffer();
  wasmIdentity=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),x=>x.toString(16).padStart(2,'0')).join('');
  const {instance,module}=await WebAssembly.instantiate(bytes,imports);
  core=instance.exports;ready(instance,module);return core;
 });}
});
if(core.sdl_files_variant(1)!==1)throw Error('Ordinary runtime rejected');
const buildResponse=await fetch('/build.json');
if(!buildResponse.ok)throw Error('MP test server must expose build identity; restart serve.mjs');
const buildIdentity=await buildResponse.json();
if(buildIdentity.sha256!==wasmIdentity)throw Error('Mixed build: loaded WASM disagrees with source inventory');
for(const path of ['/game','/fonts','/savesth10-multiplayer/jp/replay'])Module.FS.mkdirTree(path);
for(const path of ['/input/th10.dat','/fonts/msgothic.ttc','/fonts/blend.bin','/fonts/codepages.bin']){
 const response=await fetch(path);if(!response.ok)throw Error(path);
 const bytes=new Uint8Array(await response.arrayBuffer());
 assetIdentities[path]=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),x=>x.toString(16).padStart(2,'0')).join('');
 Module.FS.writeFile(path==='/input/th10.dat'?'/game/th10.dat':path,bytes);
}
function string(value,callback){const bytes=new TextEncoder().encode(value+'\0'),ptr=core.files_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);return callback(ptr);}finally{core.files_free(ptr);}}
string('#screen',core.sdl_canvas);core.sdl_music_enabled(0);
let app=0,seatCount=0;
const networkInput=core.files_allocate(20);
function networkError(){const pointer=core.multiplayer_network_error?.(app)||0;
 if(!pointer)return '';const bytes=new Uint8Array(core.memory.buffer);const end=bytes.indexOf(0,pointer);
 return new TextDecoder().decode(bytes.subarray(pointer,end<0?pointer+256:Math.min(end,pointer+256)));}
function transportStatus(){return Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_transport_status(app),15));}
function capturePeerInput(frame,buttons){const words=new Uint32Array(core.memory.buffer,networkInput,5);words.fill(0);words[0]=buttons>>>0;
 return !!core.multiplayer_capture_local(app,frame,networkInput,5);}
const status=()=>Array.from(new Int32Array(core.memory.buffer,core.multiplayer_status(app),44));
const netStatus=()=>Array.from(new Int32Array(core.memory.buffer,core.multiplayer_netplay_status(app),11));
const canonical=()=>{const words=Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_canonical_hashes(app),44));
 if(words[0]!==2)throw Error('Canonical schema 2 required');return words;};
const memoryStatus=()=>({heap:core.memory.buffer.byteLength,words:Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_memory_status(app),12))});
const enemyDebug=()=>Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_enemy_debug(app),81));
const lifecycle=()=>Array.from(new Int32Array(core.memory.buffer,core.multiplayer_lifecycle_status(app),12));
const audioStatus=()=>Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_audio_status(app),9));
const generationStatus=()=>Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_generation_status(app),9));
const replayStatus=()=>Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_replay_status(app),13));
const replayPath='/savesth10-multiplayer/jp/replay/th10_01.rpy';
let replayTrace=[],replayTraceLast=-1,robotPreparedFrame=-1,robotPreparedGeneration=-1;
function replayObservation(){const n=netStatus(),c=canonical();return {frame:n[3],state:status(),rng:c.slice(13,17),enemy:c[5],replay:replayStatus()};}
function observedTick(){const result=core.sdl_loop_tick(app,1/60,16);
 if(result||core.application_error(app))throw Error('Native Replay tick failed '+result+' '+core.application_error(app));
 const n=netStatus();if(n[3]>=0&&n[3]!==replayTraceLast){replayTraceLast=n[3];replayTrace.push(replayObservation());}
 return n;
}
function configure(words){
 const ptr=core.files_allocate(words.length*4);try{
  new Uint32Array(core.memory.buffer,ptr,words.length).set(words);
  if(!core.multiplayer_configure(app,ptr,words.length))throw Error('Session rejected');
 }finally{core.files_free(ptr);}
}
function commitInputs(buttons=[]){
 const ptr=core.files_allocate(seatCount*20);try{
  const inputs=new Uint32Array(core.memory.buffer,ptr,seatCount*5);inputs.fill(0);
  for(let seat=0;seat<seatCount;++seat)inputs[seat*5]=buttons[seat]||0;
  return !!core.multiplayer_commit_inputs(app,ptr,seatCount*5);
 }finally{core.files_free(ptr);}
}
window.multiplayerSmoke={
 async audioSeek(){
  if(app)throw Error('Audio seek probe must run before a game Application exists');
  core.sdl_music_enabled(1);
  const {checkReplaySeekAudio}=await import('/replay-audio-seek.mjs');
  return checkReplaySeekAudio(core,Module);
 },
 replayStatus,
 recordKeyboardBatch(count,stopStage=0){
  if(!Number.isInteger(count)||count<1||count>300)throw Error('Invalid bounded robot batch');
  const rows=[];
  for(let i=0;i<count;++i){
   const before=netStatus(),tape=replayStatus(),life=lifecycle(),generation=generationStatus()[1];
   if(generation!==0)throw Error('Robot exhausted native lives before its target');
   if(before[9]&&reachedNativeStage(status(),life,stopStage))break;
   if(Module.FS.analyzePath(replayPath).exists)break;
   // A single native call can finish loading and enter a simulation frame.
   // Prime that frame at startup AND every native stage boundary. Capture it
   // once even when several loading callbacks retain the same frontier; never
   // overwrite the agreed neutral boundary input with a later robot sample.
   if(before[1]&&(robotPreparedFrame!==before[2]||robotPreparedGeneration!==generation)){
    const inputs=before[9]&&life[1]===life[2]
     ?allKeyboardInputs(this.replayProbe(),this.replayControls()):new Array(seatCount).fill(0);
    const local=status()[2];
    if(inputs.length!==seatCount)throw Error('Robot observation is not active gameplay');
    if(!this.captureLocal(before[2],inputs[local]))throw Error('Robot local input rejected');
    for(let seat=0;seat<seatCount;++seat)if(seat!==local){
     const result=this.submitRemote(seat,before[2],inputs[seat]);
     if(![1,2,3,4].includes(result))throw Error('Robot remote input rejected '+result);
    }
    robotPreparedFrame=before[2];robotPreparedGeneration=generation;
   }
   const result=core.sdl_loop_tick(app,1/60,16);
   if(result||core.application_error(app))throw Error('Native robot tick failed '+JSON.stringify({result,error:core.application_error(app),before,after:netStatus(),life:lifecycle(),replay:replayStatus()}));
   const after=replayStatus();
   if(after[4]!==tape[4]){
    if(after[4]!==tape[4]+1)throw Error('Robot skipped archive frames');
    rows.push(replayObservation());
   }
  }
  return {rows,probe:this.replayProbe(),controls:this.replayControls(),net:netStatus(),replay:replayStatus(),state:status(),life:lifecycle(),stageReady:reachedNativeStage(status(),lifecycle(),stopStage),saved:Module.FS.analyzePath(replayPath).exists};
 },
 replayControls(){if(!core.mp_fixture_replay_controls)throw Error('Read-only control probe requires a fixture build');
  return Array.from(new Float32Array(core.memory.buffer,core.mp_fixture_replay_controls(app),16));},
 replayProbe(){if(!core.mp_fixture_replay_probe)throw Error('Read-only probe requires a fixture build');
  const pointer=core.mp_fixture_replay_probe(app),h=new Float32Array(core.memory.buffer,pointer,16);
  return Array.from(new Float32Array(core.memory.buffer,pointer,52+h[2]*7+h[3]*8+h[12]*4));},
 replaySeekBatch(){if(!core.mp_fixture_replay_seek_batch)throw Error('Seek batch test requires a fixture build');
  const result=core.mp_fixture_replay_seek_batch(app);if(result)throw Error('Native Replay seek failed '+result+' '+core.application_error(app));
  return replayObservation();},
 replayObservation,
 async replayRaf(through,escapeAfter=0){
  if(!Number.isInteger(through)||through<1||through>250000||
     !Number.isInteger(escapeAfter)||escapeAfter<0||escapeAfter>10000||!replayStatus()[8])
    throw Error('Invalid bounded Replay browser loop');
  const read=()=>({observation:replayObservation(),
    graphics:Array.from(new Uint32Array(core.memory.buffer,core.sdl_stats(),17)),
    audio:Array.from(new Uint32Array(core.memory.buffer,core.sdl_audio_stats(),12))});
  const initial=read(),samples=[],previousPrepare=Module.runtimePrepare,previousFinish=Module.runtimeFinish;
  let escaped=false,finished=false;
  return await new Promise((resolve,reject)=>{
   const finish=(error)=>{
    if(finished)return;finished=true;clearTimeout(timeout);
    core.sdl_loop_stop();this.key('Escape',false);
    Module.runtimePrepare=previousPrepare;Module.runtimeFinish=previousFinish;
    if(error)reject(error);else resolve({initial,samples,escaped,final:replayStatus(),life:lifecycle()});
   };
   const timeout=setTimeout(()=>finish(Error('Replay browser loop timed out')),240000);
   Module.runtimePrepare=()=>1;
   Module.runtimeFinish=(result)=>{
    try{
     if(result||core.application_error(app))throw Error('Replay browser loop failed '+result+' '+core.application_error(app));
     const sample=read();samples.push(sample);
     const tape=sample.observation.replay;
     if(escaped&&!tape[8]||!escapeAfter&&tape[4]>=through){finish();return;}
     if(escapeAfter&&!escaped&&samples.length>=escapeAfter){this.key('Escape',true);escaped=true;}
     if(samples.length>Math.ceil(through/4)+4000)throw Error('Replay browser callback bound exceeded');
    }catch(error){finish(error);}
   };
   core.sdl_loop_start(app);
  });
 },
 saveReplay(){if(!core.mp_fixture_save_replay)throw Error('Replay save test requires fixture build');return !!core.mp_fixture_save_replay(app);},
 replayBytes(){return Array.from(Module.FS.readFile(replayPath));},
 key(code,down){string(code,p=>core.sdl_key(p,down?1:0));},
 viewerStart(bytes){
  if(app)throw Error('Use a fresh instance for Replay playback');
  const data=new Uint8Array(bytes),ptr=core.files_allocate(data.length);
  try{new Uint8Array(core.memory.buffer,ptr,data.length).set(data);
   if(!core.multiplayer_replay_validate(ptr,data.length))throw Error('Invalid multiplayer Replay');
  }finally{core.files_free(ptr);}
  Module.FS.writeFile(replayPath,data);app=core.sdl_game_open(0,1234);
  if(!app)throw Error('Viewer initialization failed');replayTrace=[];replayTraceLast=-1;
  return replayStatus();
 },
 viewerTicks(count){for(let i=0;i<count;++i)observedTick();return {net:netStatus(),replay:replayStatus(),state:status()};},
 takeReplayTrace(){const result=replayTrace;replayTrace=[];return result;},
 savedFiles(){const result={};for(const directory of ['','/replay']){
  const root='/savesth10-multiplayer/jp'+directory;
  for(const name of Module.FS.readdir(root)){if(name==='.'||name==='..')continue;const path=root+'/'+name;
   if(Module.FS.isFile(Module.FS.stat(path).mode))result[(directory+'/'+name).replace(/^\//,'')]=Array.from(Module.FS.readFile(path));}
 }return result;},
 connect(relay){if(!core.multiplayer_connect)throw Error('Network Runtime required');return !!string(relay,p=>core.multiplayer_connect(app,p));},
 pollNetwork(){return !!core.multiplayer_network_poll(app);},
 transportStatus,
 networkError,
 peerUntilGeneration(generation){
  if(!core.multiplayer_network_poll(app))throw Error(networkError());
  if(transportStatus()[12]>=generation)return {net:netStatus(),transport:transportStatus()};
  const before=netStatus(),life=Array.from(new Int32Array(core.memory.buffer,core.multiplayer_lifecycle_status(app),9));
  if(before[9]&&before[1]&&life[1]===life[2]&&!capturePeerInput(before[2],0))throw Error('Generation input capture rejected');
  const result=core.sdl_loop_tick(app,1/60,16);
  if(result||core.application_error(app))throw Error('Native generation tick failed '+result+' '+core.application_error(app)+' '+networkError());
  return {net:netStatus(),transport:transportStatus()};
 },
 peerAdvance(target,buttons){
  if(!core.multiplayer_network_poll(app))throw Error(networkError());
  const before=netStatus();
  if(before[9]){
   if(before[2]>target)return {net:before,transport:transportStatus()};
   if(!capturePeerInput(before[2],buttons))throw Error('Local network capture rejected '+before[2]);
   // Stop at an exact confirmed comparison boundary without injecting remote
   // input or silently advancing another frame merely to finish a rollback.
   if(before[2]===target&&(before[4]===-1||before[4]<target))return {net:before,transport:transportStatus()};
  }
  const result=core.sdl_loop_tick(app,1/60,16);
  if(result||core.application_error(app))throw Error('Native peer tick failed '+result+' '+core.application_error(app)+' '+networkError());
  return {net:netStatus(),transport:transportStatus()};
 },
 identity(){return {game:'th10',variant:'multiplayer',fixtureBuild:!!core.mp_fixture_prepare,wasmSha256:wasmIdentity,
  sourceDigest:buildIdentity.sourceDigest,profile:buildIdentity.profile,assets:assetIdentities};},
 fixture(kind){if(!core.mp_fixture_prepare)throw Error('Fixture export absent from production Runtime');return !!core.mp_fixture_prepare(app,kind);},
 historyProfile(profile){if(app||!core.mp_fixture_score_history)throw Error('History setup requires a fresh diagnostic page');return !!core.mp_fixture_score_history(profile);},
 fixtureStatus(){if(!core.mp_fixture_status)throw Error('Fixture export absent');return Array.from(new Int32Array(core.memory.buffer,core.mp_fixture_status(app),18));},
 close(){if(app){core.sdl_game_close();app=0;}return true;},
 start(loadouts,local=0,difficulty=1,seed=1234){
  if(app)throw Error('Use a fresh page for a new run');
  app=core.sdl_game_open(0,seed);if(!app)throw Error('Native app creation failed');
  seatCount=loadouts.length;
  const words=[1,loadouts.length,local,difficulty,seed,...loadouts.flat()];while(words.length<11)words.push(0);
  configure(words);
  return status();
 },
 startNet(loadouts,local=0,difficulty=1,seed=1234,sessionLow=0x55667788,sessionHigh=0x11223344){
  if(app)throw Error('Use a fresh page for a new run');
  app=core.sdl_game_open(0,seed);if(!app)throw Error('Native app creation failed');
  seatCount=loadouts.length;
  const words=[2,loadouts.length,local,difficulty,seed,sessionLow>>>0,sessionHigh>>>0,...loadouts.flat()];
  while(words.length<13)words.push(0);configure(words);return netStatus();
 },
 sessionPacket(phase){const ptr=core.files_allocate(128);try{
  const size=core.multiplayer_session_build(app,phase,ptr,128);if(!size)throw Error('Session packet rejected');
  return Array.from(new Uint8Array(core.memory.buffer,ptr,size));
 }finally{core.files_free(ptr);}},
 applySession(bytes){const ptr=core.files_allocate(bytes.length);try{
  new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);return !!core.multiplayer_session_apply(app,ptr,bytes.length);
 }finally{core.files_free(ptr);}},
 markReady(){return !!core.multiplayer_session_mark_ready(app);},
 canStart(){return !!core.multiplayer_session_can_start(app);},
 netStatus,
 canonical,
 enemyDebug,
 lifecycle,
 audioStatus,
 generationStatus,
 memoryStatus,
 timedNetTick(){const start=performance.now();const result=core.sdl_loop_tick(app,1/60,16);
  const milliseconds=performance.now()-start;
  if(result||core.application_error(app))throw Error('Timed native tick failed '+result+' '+core.application_error(app));
  return {milliseconds,net:netStatus(),memory:memoryStatus()};},
 inputPacket(peer,frame,sequence=1,ack=0){const ptr=core.files_allocate(2048);try{
  const size=core.multiplayer_packet_build(app,peer,frame,sequence,ack,ptr,2048);
  if(!size)throw Error('Input packet rejected');
  return Array.from(new Uint8Array(core.memory.buffer,ptr,size));
 }finally{core.files_free(ptr);}},
 applyPacket(bytes){const ptr=core.files_allocate(bytes.length);try{
  new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);
  return core.multiplayer_packet_apply(app,ptr,bytes.length);
 }finally{core.files_free(ptr);}},
 captureLocal(frame,buttons=0){const ptr=core.files_allocate(20);try{
  const row=new Uint32Array(core.memory.buffer,ptr,5);row.fill(0);row[0]=buttons>>>0;
  return !!core.multiplayer_capture_local(app,frame,ptr,5);
 }finally{core.files_free(ptr);}},
 submitRemote(player,frame,buttons=0){const ptr=core.files_allocate(20);try{
  const row=new Uint32Array(core.memory.buffer,ptr,5);row.fill(0);row[0]=buttons>>>0;
  return core.multiplayer_submit_remote(app,player,frame,ptr,5);
 }finally{core.files_free(ptr);}},
 menuTicks(count,buttons=[]){for(let i=0;i<count;++i){
  if(!commitInputs(buttons))throw Error('Menu input rejected');
  const result=core.sdl_loop_tick(app,1/60,16);if(result||core.application_error(app))throw Error('Native tick failed '+result+' '+core.application_error(app));
 }return status();},
 netTicks(count){for(let i=0;i<count;++i){const result=core.sdl_loop_tick(app,1/60,16);if(result||core.application_error(app))throw Error('Native net tick failed '+result+' '+core.application_error(app));}return {status:status(),net:netStatus()};},
 ticks(count,buttons=[]){const ptr=core.files_allocate(seatCount*20);try{
  const inputs=new Uint32Array(core.memory.buffer,ptr,seatCount*5);
  inputs.fill(0);
  for(let seat=0;seat<seatCount;++seat)inputs[seat*5]=buttons[seat]||0;
  for(let i=0;i<count;++i){if(!core.multiplayer_commit_inputs(app,ptr,seatCount*5))throw Error('Input rejected');const result=core.sdl_loop_tick(app,1/60,16);if(result||core.application_error(app))throw Error('Native tick failed '+result+' '+core.application_error(app));}
 }finally{core.files_free(ptr);}return status();},
 status,
 nativeStatus(){return Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));},
};
