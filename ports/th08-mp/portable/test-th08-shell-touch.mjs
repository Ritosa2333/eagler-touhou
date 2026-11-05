import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {SourceTextModule,SyntheticModule,createContext} from 'node:vm';

// Execute the complete shell ESM, including its actual message listener and
// async command queue. Only native/platform dependencies are mocked. Override
// TH08_SHELL_UNDER_TEST to run the queue regression against a frozen old shell.
const runtime=resolve(dirname(fileURLToPath(import.meta.url)),'../web/sdl-runtime');
const sourcePath=process.env.TH08_SHELL_UNDER_TEST||resolve(runtime,'shell.mjs');
const settle=()=>new Promise(resolve=>setImmediate(resolve));
const gate=()=>{let release;const promise=new Promise(resolve=>{release=resolve;});return {promise,release};};
async function until(predicate,label){
  for(let i=0;i<40;++i){if(predicate())return;await settle();}
  assert.fail('Did not reach '+label);
}
function events(extra={}){
  const listeners=new Map();
  return {...extra,addEventListener(name,callback){if(!listeners.has(name))listeners.set(name,[]);listeners.get(name).push(callback);},
    dispatchEvent(event){for(const callback of listeners.get(event.type)||[])callback(event);return true;}};
}
async function harness({multiplayer=true,holdInitialization=false}={}){
  const acknowledgments=[],notifications=[],calls=[],errors=[];
  const canvas=events({focus(){},getBoundingClientRect:()=>({left:80,top:0,width:640,height:480})});
  const nodes={canvas,'#screen':canvas,'#error':{textContent:''},'#loading':{textContent:''}};
  const document=events({hidden:false,documentElement:{style:{}},body:events({style:{}}),
    querySelector:selector=>nodes[selector]??null});
  const location=new URL('http://localhost/runtime/th08.html?runtimeEpoch=23&managedData=1&gameGeneration=g1&runtimeVariant='+(multiplayer?'multiplayer':'normal'));
  const window=events({}),frame={isConnected:true,contentDocument:document,contentWindow:window};
  window.frameElement=frame;
  const parent={location,document:{querySelector:()=>null},postMessage(message){
    const copy=JSON.parse(JSON.stringify(message));
    if(typeof copy.request==='string')acknowledgments.push(copy);else notifications.push(copy);
  },__eaglerPrepareManagedRuntimeDataV1:async()=>({buffer:new ArrayBuffer(32)})};
  let syncBlock=null,resourceBlock=null,audioBlock=null,sequence=0,nextPointer=1024;
  const initialization=gate();if(!holdInitialization)initialization.release();
  const native={fire:false,focus:false,x:0,y:0,bomb:0,escape:0,bombPulses:0,escapePulses:0,opens:0,reopens:0,initializations:0};
  const cancel=()=>{native.focus=false;native.x=native.y=0;calls.push(['cancel']);};
  const begin=()=>{cancel();native.bomb=native.escape=0;};
  const memory={buffer:new ArrayBuffer(65536)},heap=new Uint32Array(memory.buffer);
  const core={
    memory,allocate:bytes=>{const p=nextPointer;nextPointer+=bytes+8;return p;},deallocate(){},
    sdl_canvas(){},sdl_files_variant:()=>1,
    sdl_touch:(...args)=>calls.push(['touch',...args]),sdl_touch_cancel:cancel,
    sdl_touch_options:(...args)=>calls.push(['options',...args]),sdl_touch_gestures(){},sdl_touch_mode(){},sdl_touch_display(){},
    sdl_touch_controls(fire,focus,bomb,escape,x,y){
      calls.push(['controls',fire,focus,bomb,escape,x,y]);native.fire=!!fire;native.focus=!!focus;native.x=x;native.y=y;
      if(bomb!==native.bomb)++native.bombPulses;if(escape!==native.escape)++native.escapePulses;
      native.bomb=bomb;native.escape=escape;
    },
    sdl_key:(p,down)=>calls.push(['key',new TextDecoder().decode(new Uint8Array(memory.buffer,p).subarray(0,new Uint8Array(memory.buffer,p).indexOf(0))),down]),
    sdl_keys_clear:cancel,sdl_game_open(){++native.opens;begin();return 512;},sdl_game_close:cancel,
    sdl_prepare_loading:()=>1,sdl_draw_loading:()=>1,sdl_prepare_total:()=>0,sdl_prepare_next:()=>0,
    sdl_game_initialize(){++native.initializations;return 1;},
    sdl_loop_pause:value=>calls.push(['pause',value]),sdl_loop_start:()=>calls.push(['start']),sdl_loop_stop:()=>calls.push(['stop']),
    sdl_music_enabled(){},sdl_music_source(){},sdl_ogg_decode_mode(){},save(){},
    sdl_game_status:()=>64,sdl_stats:()=>128,sdl_audio_stats:()=>192,
    multiplayer_netplay_status:()=>320,multiplayer_replay_status:()=>256,multiplayer_replay_request:()=>0,
    multiplayer_replay_viewer:()=>1,multiplayer_replay_reopen(){++native.reopens;heap[256/4+9]=0;begin();return 512;},
    multiplayer_local_player_visibility(){},
  };
  const files=new Map([['/msgothic.ttc',new Uint8Array(1)]]);
  const FS={mkdirTree(){},mount(){},symlink(){},unlink:path=>files.delete(path),stat:()=>({mode:1,size:1}),
    isFile:()=>true,readdir:()=>[],writeFile:(path,bytes)=>files.set(path,bytes),readFile:path=>files.get(path)||new Uint8Array(4),
    syncfs(_populate,callback){calls.push(['sync']);const wait=syncBlock?.promise??Promise.resolve();wait.then(()=>callback(null));}};
  const Module={FS,IDBFS:{},SDL3:{audioContext:{state:'running',async resume(){
    calls.push(['audio-resume']);if(audioBlock)await audioBlock.promise;this.state='running';
  }}}};
  const fetch=async input=>{
    const url=new URL(input,location);
    if(url.pathname.endsWith('/held-font.bin')){calls.push(['resource']);if(resourceBlock)await resourceBlock.promise;}
    return {ok:true,json:async()=>url.pathname.endsWith('/manifest.json')?
      {builtAt:'2026-10-12',execution:{sha256:'a'.repeat(64)}}:{schema:'eagler-sdl-resources/1',game:'th08',resources:[]},
      arrayBuffer:async()=>new Uint8Array([1,2,3,4]).buffer};
  };
  const context=createContext({window,parent,document,location,innerWidth:800,innerHeight:480,
    navigator:{},URL,URLSearchParams,TextEncoder,TextDecoder,Uint8Array,Uint32Array,Int32Array,ArrayBuffer,
    performance:{now:()=>3000,mark(){}},indexedDB:{},fetch,
    console:{log(){},warn(){},debug(){},error:value=>errors.push(String(value))},
    queueMicrotask,requestAnimationFrame:callback=>queueMicrotask(()=>callback(3000)),
    setTimeout:callback=>{queueMicrotask(callback);return 1;},clearTimeout(){},
    CustomEvent:class {constructor(type,fields={}){this.type=type;Object.assign(this,fields);}},
    WebAssembly:{instantiateStreaming:async()=>({instance:{exports:core},module:{}})}});
  const mocks={
    './th08-sdl.mjs':{default:async options=>{await initialization.promise;await options.instantiateWasm({},()=>{});return Module;}},
    './practice.mjs':{createPractice:()=>({configure(){},clear(){},close(){},tick(){},key:()=>false,mouse(){}})},
    './motion-replay.mjs':{exportReplayName:path=>path,importReplayName:path=>path},
    './save-storage.mjs':{initializeSaveStorage:async({beforeMount,sync})=>{beforeMount();await sync(true);return {
      namespace:'/saves',root:()=>'/saves',relativeSave:path=>path};},migrateLegacySaves:async()=>{}},
    './multiplayer-host.mjs':{validateMultiplayerOptions:options=>options.netplayMode==='lan'?
      {seed:1234,adonisMode:0,spectator:!!options.netplaySpectator}:null,
      configureMultiplayer:async()=>{},updateNetworkDiagnostics(){},networkError:()=>''},
    './adonis-calibration.mjs':{createAdonisCalibration:()=>({stop(){},start(){},pump(){}})},
    './startup-branding.mjs':{installStartupBranding:async()=>{}},
  };
  const modules=new Map();
  const dependency=async name=>{
    if(modules.has(name))return modules.get(name);
    let module;
    if(name==='./directory-keyboard.mjs'||name==='./eagler-host.mjs')
      module=new SourceTextModule(readFileSync(resolve(runtime,name),'utf8'),{context,identifier:name});
    else{
      const values=mocks[name];assert(values,'Unexpected shell dependency '+name);
      module=new SyntheticModule(Object.keys(values),function(){for(const [key,value] of Object.entries(values))this.setExport(key,value);},{context,identifier:name});
    }
    const linked=Promise.resolve(module.link(dependency)).then(()=>module);modules.set(name,linked);return linked;
  };
  const shell=new SourceTextModule(readFileSync(sourcePath,'utf8'),{context,identifier:sourcePath,
    importModuleDynamically:async name=>{const module=await dependency(name);if(module.status==='linked')await module.evaluate();return module;}});
  await shell.link(dependency);await shell.evaluate();
  const send=(fields,overrides={})=>{
    const request='r'+(++sequence),data={protocol:'eagler-touhou/1',game:'th08',epoch:23,request,...fields};
    window.dispatchEvent({type:'message',source:parent,origin:location.origin,data,...overrides});return request;
  };
  const ack=request=>acknowledgments.find(message=>message.request===request);
  const request=async fields=>{const id=send(fields);await until(()=>ack(id),'ACK '+fields.command);assert.equal(ack(id).ok,true,ack(id).error);return ack(id);};
  return {core,Module,native,calls,errors,document,window,frame,parent,send,ack,request,
    async ready(){initialization.release();await until(()=>notifications.some(message=>message.event==='ready')||errors.length,'shell ready');assert.deepEqual(errors,[]);},
    async start(extra={}){
      await this.ready();await request({command:'configure',language:'lang_ja',music:'none',
        options:{touchEnabled:true,touchMovementMode:'touch',...(multiplayer?{netplayMode:'lan'}:{}),...extra}});
      await request({command:'launch'});
    },
    blockSync(){const blocked=gate();syncBlock=blocked;return ()=>{syncBlock=null;blocked.release();};},
    blockResource(){const blocked=gate();resourceBlock=blocked;return ()=>{resourceBlock=null;blocked.release();};},
    blockAudio(){const blocked=gate();audioBlock=blocked;Module.SDL3.audioContext.state='suspended';return ()=>{audioBlock=null;blocked.release();};},
    requestReplay(){heap[256/4+9]=1;Module.runtimeFinish(0,0);}
  };
}
const direct={command:'direct-touch',type:'down',id:9,x:.5,y:.5};
const controls=(bombSerial=0,extra={})=>({command:'touch-controls',controls:{fireEnabled:true,focusEnabled:true,
  joystickX:16384,joystickY:-16384,bombSerial,escapeSerial:0,...extra}});
const count=(h,name)=>h.calls.filter(call=>call[0]===name).length;

test('real shell: touch and ACK bypass a blocked filesystem command queue',async()=>{
  const h=await harness();await h.start();const release=h.blockSync();
  try{
    const syncBefore=count(h,'sync'),pending=h.send({command:'sync'});
    await until(()=>count(h,'sync')>syncBefore,'blocked sync');
    const key=h.send({command:'keyboard',code:'KeyZ',down:true}),down=h.send(direct),held=h.send(controls(1));
    await settle();
    assert(h.ack(key)?.ok,'existing keyboard bypass regressed');
    assert(h.ack(down)?.ok,'direct-touch ACK waited behind the filesystem queue');
    assert(h.ack(held)?.ok,'touch-controls ACK waited behind the filesystem queue');
    assert.equal(count(h,'touch'),1);assert.equal(h.native.bombPulses,1);assert.equal(h.ack(pending),undefined);
    const cancel=h.send({command:'touch-cancel'});await settle();assert(h.ack(cancel)?.ok);
    assert.equal(h.native.focus,false);assert.equal(h.native.x,0);assert.equal(h.native.fire,true);
  }finally{release();await settle();}
});
test('real shell: live normal Runtime touch bypasses a blocked resource transfer',async()=>{
  const h=await harness({multiplayer:false});await h.start();const release=h.blockResource();
  try{
    h.send({command:'resources',resources:[{path:'/fonts/probe.bin',url:'./held-font.bin',bytes:4}]});
    await until(()=>count(h,'resource')===1,'blocked resource');
    const request=h.send(direct);await settle();
    assert(h.ack(request)?.ok,'direct-touch ACK waited behind the resource queue');assert.equal(count(h,'touch'),1);
  }finally{release();await settle();}
});
test('real shell: initialization and unfinished launch discard input immediately without replaying serial pulses',async()=>{
  const h=await harness({holdInitialization:true});
  for(const message of [direct,controls(7),{command:'touch-cancel'}]){
    const request=h.send(message);await settle();assert(h.ack(request)?.ok);
  }
  assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
  await h.ready();await h.request({command:'configure',music:'none',options:{touchEnabled:true,netplayMode:'lan'}});
  const release=h.blockAudio(),launch=h.send({command:'launch'});
  try{
    await until(()=>count(h,'audio-resume')===1,'unfinished audio resume');
    const dropped=h.send(direct);await settle();assert(h.ack(dropped)?.ok);assert.equal(count(h,'touch'),0);
  }finally{release();await until(()=>h.ack(launch),'launch completion');}
  await h.request(controls(7));assert.equal(h.native.bombPulses,0);
  await h.request(controls(8));assert.equal(h.native.bombPulses,1,'first eligible new bomb must survive');
  await h.request(direct);assert.equal(count(h,'touch'),1);
});
test('real shell: hidden input is discarded; native transient cleanup preserves fire and consumes rejected serials',async()=>{
  const h=await harness();await h.start();await h.request(controls(1));
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  const before=count(h,'controls');await h.request(direct);await h.request(controls(2));
  assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),before);
  assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);assert.equal(h.native.x,0);
  h.document.hidden=false;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  await h.request(controls(2,{focusEnabled:false,joystickX:0,joystickY:0}));assert.equal(h.native.bombPulses,1);
  await h.request(controls(3));assert.equal(h.native.bombPulses,2);
});
test('real shell: closing input is dropped, and a fresh session preserves the source baseline but resets native serial mapping',async()=>{
  const h=await harness();await h.start();await h.request(controls(1));
  const release=h.blockSync(),stopping=h.window.__th08Runtime.stop();
  try{await h.request(direct);await h.request(controls(2));assert.equal(count(h,'touch'),0);assert.equal(h.native.bombPulses,1);}
  finally{release();await stopping;}
  await h.request(direct);assert.equal(count(h,'touch'),0);
  await h.request({command:'launch'});
  await h.request(controls(2));assert.equal(h.native.bombPulses,1,'old source snapshot must not become a new-session bomb');
  await h.request(controls(3));assert.equal(h.native.bombPulses,2,'first new-session press must not be eaten');
});
test('real shell: Replay menu keeps touch, while pending reopen drops input and rebases only native serials',async()=>{
  const h=await harness();await h.start({netplayMode:undefined,replayViewer:true});await h.request(direct);await h.request(controls(1));
  const release=h.blockSync(),syncBefore=count(h,'sync');h.send({command:'sync'});
  await until(()=>count(h,'sync')>syncBefore,'replay queue blocker');
  h.requestReplay();
  try{
    await h.request({...direct,type:'move'});await h.request(controls(2));
    assert.equal(count(h,'touch'),1);assert.equal(h.native.bombPulses,1);assert.equal(h.native.focus,false);
  }finally{release();await until(()=>h.native.reopens===1&&h.native.initializations===2,'Replay reopen');await settle();}
  await h.request(controls(2));assert.equal(h.native.bombPulses,1);
  await h.request(controls(3));assert.equal(h.native.bombPulses,2);
  await h.request(direct);assert.equal(count(h,'touch'),2);
});
test('real shell: spectator rejection still consumes source serials before a later playable session',async()=>{
  const h=await harness();await h.start({netplaySpectator:true});
  await h.request(direct);await h.request(controls(5));assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
  await h.window.__th08Runtime.stop();await h.start({netplaySpectator:false});
  await h.request(controls(5));assert.equal(h.native.bombPulses,0);
  await h.request(controls(6));assert.equal(h.native.bombPulses,1);
});
test('real shell: pagehide and a replaced document owner cannot revive old touch',async()=>{
  for(const retiredBy of ['pagehide','detached','replaced']){
    const h=await harness();await h.start();await h.request(controls(1));
    if(retiredBy==='pagehide')h.window.dispatchEvent({type:'pagehide'});
    else if(retiredBy==='detached')h.frame.isConnected=false;
    else h.frame.contentDocument={};
    await h.request(direct);await h.request(controls(2));
    assert.equal(count(h,'touch'),0);assert.equal(h.native.focus,false);
    h.frame.isConnected=true;h.frame.contentDocument=h.document;h.window.dispatchEvent({type:'pageshow'});
    await h.request(direct);assert.equal(count(h,'touch'),0,retiredBy);
  }
});
test('real shell: source/origin/protocol/game/epoch validation remains before realtime dispatch',async()=>{
  const h=await harness();await h.start();
  for(const overrides of [{source:{}},{origin:'http://other.invalid'},
    {data:{...direct,protocol:'wrong',game:'th08',epoch:23,request:'bad'}},
    {data:{...direct,protocol:'eagler-touhou/1',game:'th09',epoch:23,request:'bad'}},
    {data:{...direct,protocol:'eagler-touhou/1',game:'th08',epoch:24,request:'bad'}}]){
    const id=h.send(direct,overrides);await settle();assert.equal(h.ack(id),undefined);
  }
  assert.equal(count(h,'touch'),0);await h.request(direct);assert.equal(count(h,'touch'),1);
});
test('real shell: first valid bomb/escape and uint32 source wrap remain real presses',async()=>{
  const h=await harness({multiplayer:false});await h.start();
  await h.request(controls(1,{escapeSerial:1}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
  await h.request(controls(0xffffffff,{escapeSerial:0xffffffff}));
  await h.request(controls(0,{escapeSerial:0}));
  assert.equal(h.native.bombPulses,3);assert.equal(h.native.escapePulses,3);
});

test('real shell: letterbox pointers share lifecycle eligibility and cannot revive a cancelled gesture',async()=>{
  const h=await harness({multiplayer:false});await h.start();
  const target={setPointerCapture(){}};
  const pointer=(type,clientX)=>h.document.dispatchEvent({type,pointerType:'touch',pointerId:45,target,clientX,clientY:240,
    preventDefault(){},stopPropagation(){}});
  pointer('pointerdown',60);pointer('pointermove',70);assert.equal(count(h,'touch'),2);
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});
  pointer('pointermove',90);pointer('pointerdown',60);assert.equal(count(h,'touch'),2);
  h.document.hidden=false;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  pointer('pointermove',90);assert.equal(count(h,'touch'),2);
  pointer('pointerdown',60);assert.equal(count(h,'touch'),3);
});

