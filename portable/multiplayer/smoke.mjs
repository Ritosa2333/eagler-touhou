import createModule from '/th10-sdl.mjs';
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
