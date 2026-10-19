import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {webcrypto} from 'node:crypto';
import {SourceTextModule,SyntheticModule,createContext} from 'node:vm';

// Execute the complete shell, real message listener/queue and packaged input,
// save-sync, Replay-file and optional-Practice loaders. Native/platform calls,
// branding and the optional Practice UI are stand-ins. No game simulation is
// inferred from these tests. TH10_SHELL_UNDER_TEST accepts a frozen old shell
// for a real queue regression control; all unchanged helpers use this checkout.
const runtime=resolve(dirname(fileURLToPath(import.meta.url)),'../web/sdl-runtime');
const sourcePath=process.env.TH10_SHELL_UNDER_TEST||resolve(runtime,'shell.mjs');
const settle=()=>new Promise(resolve=>setImmediate(resolve));
const gate=()=>{let release;const promise=new Promise(resolve=>{release=resolve;});return {promise,release};};
async function until(predicate,label){
  const deadline=Date.now()+5000;
  while(Date.now()<deadline){if(predicate())return;await new Promise(resolve=>setTimeout(resolve,1));}
  assert.fail('Did not reach '+label);
}
function events(extra={}){
  const listeners=new Map();
  return {...extra,addEventListener(name,callback){if(!listeners.has(name))listeners.set(name,[]);listeners.get(name).push(callback);},
    dispatchEvent(event){for(const callback of listeners.get(event.type)||[])callback(event);return true;}};
}
const lanOptions={netplayMode:'lan',netplayUrl:'ws://localhost/netplay?room=TEST&run=run1',
  netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1234,netplayDifficulty:0,
  netplayLoadouts:[{character:0,shot:0},{character:1,shot:1}]};
async function harness({multiplayer=true,holdInitialization=false,withPractice=false}={}){
  const acknowledgments=[],notifications=[],calls=[],errors=[];
  const canvas=events({focus(){},getBoundingClientRect:()=>({left:80,top:0,width:640,height:480})});
  const nodes={canvas,'#screen':canvas,'#error':{textContent:''},'#loading':{textContent:''}};
  const overlays=[];
  const document=events({hidden:false,hasFocus:()=>false,documentElement:{style:{}},
    body:events({style:{},appendChild:node=>overlays.push(node)}),
    createElement:()=>({style:{},setAttribute(){}}),querySelector:selector=>nodes[selector]??null});
  const location=new URL('http://localhost/runtime/th10.html?runtimeEpoch=23&managedData=1&gameGeneration=g1&runtimeVariant='+(multiplayer?'multiplayer':'normal'));
  const window=events({}),frame={isConnected:true,contentDocument:document,contentWindow:window};
  window.frameElement=frame;
  const parent={location,document:{querySelector:()=>null},postMessage(message){
    const copy=JSON.parse(JSON.stringify(message));
    if(typeof copy.request==='string')acknowledgments.push(copy);else notifications.push(copy);
  },__eaglerPrepareManagedRuntimeDataV1:async()=>({buffer:new ArrayBuffer(32)})};
  let syncBlock=null,resourceBlock=null,audioBlock=null,brandingBlock=null,configureBlock=null;
  let sequence=0,nextPointer=1024;
  const initialization=gate();if(!holdInitialization)initialization.release();
  const native={fire:false,focus:false,x:0,y:0,bomb:0,escape:0,bombPulses:0,escapePulses:0,
    opens:0,configs:[],practiceImports:0};
  const cancel=()=>{native.focus=false;native.x=native.y=0;calls.push(['cancel']);};
  const begin=()=>{cancel();native.bomb=native.escape=0;};
  const memory={buffer:new ArrayBuffer(65536)},heap=new Uint32Array(memory.buffer);
  const allocate=bytes=>{const p=nextPointer;nextPointer=(nextPointer+bytes+7)&~7;return p;};
  const core={
    memory,graphics_allocate:allocate,graphics_free(){},files_allocate:allocate,files_free(){},
    sdl_canvas(){},sdl_files_variant:()=>1,
    sdl_touch:(...args)=>calls.push(['touch',...args]),sdl_touch_cancel:cancel,
    sdl_touch_options:(...args)=>calls.push(['options',...args]),sdl_touch_gestures(){},sdl_touch_mode(){},
    sdl_touch_controls(fire,focus,bomb,escape,x,y){
      calls.push(['controls',fire,focus,bomb,escape,x,y]);native.fire=!!fire;native.focus=!!focus;native.x=x;native.y=y;
      if(bomb!==native.bomb)++native.bombPulses;if(escape!==native.escape)++native.escapePulses;
      native.bomb=bomb;native.escape=escape;
    },
    sdl_key:(p,down)=>calls.push(['key',new TextDecoder().decode(new Uint8Array(memory.buffer,p).subarray(0,new Uint8Array(memory.buffer,p).indexOf(0))),down]),
    sdl_keys_clear:cancel,sdl_game_open(){++native.opens;begin();return 512;},sdl_game_close:cancel,
    sdl_loop_pause:value=>calls.push(['pause',value]),sdl_loop_start:()=>calls.push(['start']),sdl_loop_stop:()=>calls.push(['stop']),
    sdl_music_enabled(){},sdl_ogg_decode_mode(){},
    application_error:()=>0,application_save:()=>calls.push(['save']),application_touch_display(){},application_original_options(){},
    sdl_game_status:()=>64,sdl_stats:()=>128,sdl_audio_stats:()=>192,
    multiplayer_netplay_status:()=>320,multiplayer_transport_status:()=>384,multiplayer_lifecycle_status:()=>448,
    multiplayer_replay_status:()=>256,multiplayer_replay_validate:()=>1,multiplayer_local_player_visibility(){},
    multiplayer_configure(_app,pointer,length){native.configs.push(Array.from(new Uint32Array(memory.buffer,pointer,length)));return 1;},
    multiplayer_connect:()=>1,multiplayer_spectator_connect:()=>1,
    ...(withPractice?{practice_enable(){}}:{}),
  };
  const files=new Map([['/msgothic.ttc',new Uint8Array(1)],['/unifont.otf',new Uint8Array(1)]]);
  const FS={mkdirTree(){},mount(){},symlink(){},unlink:path=>files.delete(path),stat:()=>({mode:1,size:1}),
    analyzePath:path=>({exists:files.has(path)}),isFile:()=>true,readdir:()=>[],
    writeFile:(path,bytes)=>files.set(path,bytes),readFile:path=>files.get(path)||new Uint8Array(4),
    rename(from,to){files.set(to,files.get(from));files.delete(from);},
    syncfs(_populate,callback){calls.push(['sync']);const wait=syncBlock?.promise??Promise.resolve();wait.then(()=>callback(null));}};
  const Module={FS,IDBFS:{},SDL3:{audioContext:{state:'running',async resume(){
    calls.push(['audio-resume']);if(audioBlock)await audioBlock.promise;this.state='running';
  }}}};
  const fetch=async input=>{
    const url=new URL(input,location);
    if(url.pathname.endsWith('/held-font.bin')){calls.push(['resource']);if(resourceBlock)await resourceBlock.promise;}
    return {ok:true,json:async()=>url.pathname.endsWith('/manifest.json')?
      {builtAt:'2026-10-12',execution:{sha256:'a'.repeat(64)}}:{schema:'eagler-sdl-resources/1',game:'th10',resources:[]},
      arrayBuffer:async()=>new Uint8Array([1,2,3,4]).buffer};
  };
  const context=createContext({window,parent,document,location,innerWidth:800,innerHeight:480,
    navigator:{},URL,URLSearchParams,TextEncoder,TextDecoder,Uint8Array,Uint32Array,Int32Array,ArrayBuffer,DataView,
    performance:{now:()=>3000,mark(){}},indexedDB:{},fetch,
    crypto:{subtle:{async digest(...args){calls.push(['configure-digest']);if(configureBlock)await configureBlock.promise;return webcrypto.subtle.digest(...args);}}},
    console:{log(){},warn(){},debug(){},error:value=>errors.push(String(value))},
    queueMicrotask,requestAnimationFrame:callback=>queueMicrotask(()=>callback(3000)),
    setTimeout:callback=>{queueMicrotask(callback);return 1;},clearTimeout(){},
    CustomEvent:class {constructor(type,fields={}){this.type=type;Object.assign(this,fields);}},
    WebAssembly:{instantiateStreaming:async()=>({instance:{exports:core},module:{}})}});
  const mocks={
    './th10-sdl.mjs':{default:async options=>{await initialization.promise;await options.instantiateWasm({},()=>{});return Module;}},
    './practice.mjs':{createPractice:()=>{++native.practiceImports;return {
      configure:options=>calls.push(['practice-configure',!!options.thpracEnabled]),
      clear:()=>calls.push(['practice-clear']),close:()=>calls.push(['practice-close']),tick:()=>calls.push(['practice-tick']),
      key:()=>false,mouse:message=>calls.push(['practice-mouse',message.type])};}},
    './save-storage.mjs':{initializeSaveStorage:async({beforeMount,sync})=>{beforeMount();await sync(true);return {
      namespace:'/saves',root:()=>'/saves',relativeSave:path=>path};},migrateLegacySaves:async()=>{}},
    './adonis-calibration.mjs':{createAdonisCalibration:()=>({
      stop:()=>calls.push(['calibration-stop']),start:()=>calls.push(['calibration-start']),pump(){}})},
    './startup-branding.mjs':{installStartupBranding:async()=>{calls.push(['branding']);if(brandingBlock)await brandingBlock.promise;}},
  };
  const sourceModules={
    './directory-keyboard.mjs':resolve(runtime,'directory-keyboard.mjs'),
    './eagler-host.mjs':resolve(runtime,'eagler-host.mjs'),
    './practice-loader.mjs':resolve(runtime,'practice-loader.mjs'),
    // package-eagler.mjs copies these sources into the immutable Runtime.
    './save-sync.mjs':resolve(runtime,'../../third_party/eagler-common/browser/save-sync.mjs'),
    './replay-file-policy.mjs':resolve(runtime,'../../portable/browser/replay-file-policy.mjs'),
    './motion-replay.mjs':resolve(runtime,'../../portable/browser/motion-replay.mjs'),
  };
  const modules=new Map();
  const dynamic=async name=>{const module=await dependency(name);if(module.status==='linked')await module.evaluate();return module;};
  const dependency=async name=>{
    if(modules.has(name))return modules.get(name);
    let module;
    if(sourceModules[name])
      module=new SourceTextModule(readFileSync(sourceModules[name],'utf8'),{context,identifier:name,importModuleDynamically:dynamic});
    else{
      const values=mocks[name];assert(values,'Unexpected shell dependency '+name);
      module=new SyntheticModule(Object.keys(values),function(){for(const [key,value] of Object.entries(values))this.setExport(key,value);},{context,identifier:name});
    }
    const linked=Promise.resolve(module.link(dependency)).then(()=>module);modules.set(name,linked);return linked;
  };
  const shell=new SourceTextModule(readFileSync(sourcePath,'utf8'),{context,identifier:sourcePath,importModuleDynamically:dynamic});
  await shell.link(dependency);await shell.evaluate();
  const send=(fields,overrides={})=>{
    const request='r'+(++sequence),data={protocol:'eagler-touhou/1',game:'th10',epoch:23,request,...fields};
    window.dispatchEvent({type:'message',source:parent,origin:location.origin,data,...overrides});return request;
  };
  const ack=request=>acknowledgments.find(message=>message.request===request);
  const request=async fields=>{const id=send(fields);await until(()=>ack(id),'ACK '+fields.command);assert.equal(ack(id).ok,true,ack(id).error);return ack(id);};
  return {core,Module,native,calls,errors,document,window,frame,parent,canvas,overlays,send,ack,request,
    async ready(){initialization.release();await until(()=>notifications.some(message=>message.event==='ready')||errors.length,'shell ready');assert.deepEqual(errors,[]);},
    async configure(extra={}){
      await this.ready();await request({command:'configure',language:'lang_ja',music:'none',
        options:{touchEnabled:true,touchMovementMode:'touch',...(multiplayer?lanOptions:{}),...extra}});
    },
    async start(extra={}){await this.configure(extra);await request({command:'launch'});},
    blockSync(){const blocked=gate();syncBlock=blocked;return ()=>{syncBlock=null;blocked.release();};},
    blockResource(){const blocked=gate();resourceBlock=blocked;return ()=>{resourceBlock=null;blocked.release();};},
    blockAudio(){const blocked=gate();audioBlock=blocked;Module.SDL3.audioContext.state='suspended';return ()=>{audioBlock=null;blocked.release();};},
    blockBranding(){const blocked=gate();brandingBlock=blocked;return ()=>{brandingBlock=null;blocked.release();};},
    blockConfigure(){const blocked=gate();configureBlock=blocked;return ()=>{configureBlock=null;blocked.release();};},
    replayState({playing=false,seeking=false,generation=0}={}){
      heap[256/4]=1;heap[256/4+2]=+playing;heap[256/4+5]=generation;heap[256/4+7]=120;
      heap[256/4+8]=1;heap[256/4+11]=+seeking;heap[256/4+12]=3;Module.runtimeFinish(0,0);
    }
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
test('real shell: live TH10 touch bypasses a blocked resource transfer in both variants',async()=>{
  for(const multiplayer of [false,true]){
    const h=await harness({multiplayer});await h.start();const release=h.blockResource();
    try{
      const pending=h.send({command:'resources',resources:[{path:'/fonts/probe.bin',url:'./held-font.bin',bytes:4}]});
      await until(()=>count(h,'resource')===1,'blocked resource');
      const request=h.send(direct);await settle();
      assert(h.ack(request)?.ok,'direct-touch ACK waited behind the resource queue');
      assert.equal(count(h,'touch'),1);assert.equal(h.ack(pending),undefined);
    }finally{release();await settle();}
  }
});
test('real shell: initialization and ready-before-launch discard input and consume source serials',async()=>{
  const h=await harness({holdInitialization:true});
  for(const message of [direct,controls(7),{command:'touch-cancel'}]){
    const request=h.send(message);await settle();assert(h.ack(request)?.ok);
  }
  assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
  await h.configure();await h.request(direct);await h.request(controls(7));assert.equal(count(h,'touch'),0);
  await h.request({command:'launch'});await h.request(controls(7));assert.equal(h.native.bombPulses,0);
  await h.request(controls(8));assert.equal(h.native.bombPulses,1);
});
test('real shell: branding, network configure and audio must settle before touch is accepted',async()=>{
  for(const [block,event] of [['blockBranding','branding'],['blockConfigure','configure-digest'],['blockAudio','audio-resume']]){
    const h=await harness();await h.configure();const release=h[block](),launch=h.send({command:'launch'});
    try{
      await until(()=>count(h,event)===1,'unfinished '+event);
      await h.request(direct);await h.request(controls(3));assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
      assert.equal(h.ack(launch),undefined);
    }finally{release();await until(()=>h.ack(launch),'launch completion');}
    assert(h.ack(launch).ok,h.ack(launch).error);
    await h.request(controls(3));assert.equal(h.native.bombPulses,0);
    await h.request(controls(4));assert.equal(h.native.bombPulses,1);
    await h.request(direct);assert.equal(count(h,'touch'),1);
  }
});
test('real shell: hidden input is dropped; cancel preserves fire and prevents stale action replay',async()=>{
  const h=await harness();await h.start();await h.request(controls(1));
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  const before=count(h,'controls');await h.request(direct);await h.request(controls(2));
  assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),before);
  assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);assert.equal(h.native.x,0);
  h.document.hidden=false;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  await h.request(controls(2,{focusEnabled:false,joystickX:0,joystickY:0}));assert.equal(h.native.bombPulses,1);
  await h.request(controls(3));assert.equal(h.native.bombPulses,2);
});
test('real shell: closing rejects input; new native sessions reset mapped serials and retain source serials',async()=>{
  const h=await harness();await h.start();await h.request(controls(1));
  const release=h.blockSync(),stopping=h.window.__th10Runtime.stop();
  try{await h.request(direct);await h.request(controls(2));assert.equal(count(h,'touch'),0);assert.equal(h.native.bombPulses,1);}
  finally{release();await stopping;}
  await h.request(direct);assert.equal(count(h,'touch'),0);
  await h.request({command:'launch'});
  await h.request(controls(2));assert.equal(h.native.bombPulses,1,'old source state became a new-session bomb');
  await h.request(controls(3));assert.equal(h.native.bombPulses,2,'first new-session press was eaten');
});
test('real shell: native Replay menus, playback and seek keep local controls without a JS reopen or serial reset',async()=>{
  const h=await harness();await h.start({netplayMode:undefined,replayViewer:true});
  await h.request(direct);await h.request(controls(1,{escapeSerial:1}));
  h.replayState({playing:true,generation:1});
  await h.request(controls(1,{escapeSerial:1}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
  h.replayState({playing:true,seeking:true,generation:2});
  await h.request(controls(1,{escapeSerial:2}));assert.equal(h.native.escapePulses,2);
  assert.equal(h.native.opens,1);assert.equal(h.overlays.length,1);assert.equal(h.overlays[0].hidden,false);
  h.replayState({generation:3});assert.equal(h.overlays[0].hidden,true);
  await h.request(direct);assert.equal(count(h,'touch'),2);assert.equal(h.native.opens,1);
});
test('real shell: spectators and disabled touch consume source serials without enabling native input',async()=>{
  for(const rejected of [{netplaySpectator:true,netplaySpectatorId:'viewer-1'},{touchEnabled:false}]){
    const h=await harness();await h.start(rejected);
    assert.equal(h.calls.filter(call=>call[0]==='options').at(-1)[1],false);
    await h.request(direct);await h.request(controls(5));assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
    await h.window.__th10Runtime.stop();await h.start({netplaySpectator:false,touchEnabled:true});
    await h.request(controls(5));assert.equal(h.native.bombPulses,0);
    await h.request(controls(6));assert.equal(h.native.bombPulses,1);
  }
});
test('real shell: pagehide, detached/replaced owners and fatal graphics failure cannot revive old touch',async()=>{
  for(const retiredBy of ['pagehide','detached','replaced','contextlost']){
    const h=await harness();await h.start();await h.request(controls(1));
    if(retiredBy==='pagehide')h.window.dispatchEvent({type:'pagehide'});
    else if(retiredBy==='detached')h.frame.isConnected=false;
    else if(retiredBy==='replaced')h.frame.contentDocument={};
    else h.canvas.dispatchEvent({type:'webglcontextlost',preventDefault(){}});
    await h.request(direct);await h.request(controls(2));
    assert.equal(count(h,'touch'),0);assert.equal(h.native.focus,false);
    h.frame.isConnected=true;h.frame.contentDocument=h.document;h.window.dispatchEvent({type:'pageshow'});
    await h.request(direct);assert.equal(count(h,'touch'),0,retiredBy);
  }
});
test('real shell: envelope validation precedes fast dispatch and source serial consumption',async()=>{
  const h=await harness();await h.start();
  for(const overrides of [{source:{}},{origin:'http://other.invalid'},
    {data:{...controls(1),protocol:'wrong',game:'th10',epoch:23,request:'bad'}},
    {data:{...controls(1),protocol:'eagler-touhou/1',game:'th09',epoch:23,request:'bad'}},
    {data:{...controls(1),protocol:'eagler-touhou/1',game:'th10',epoch:24,request:'bad'}}]){
    const id=h.send(controls(1),overrides);await settle();assert.equal(h.ack(id),undefined);
  }
  assert.equal(count(h,'controls'),0);await h.request(controls(1));assert.equal(h.native.bombPulses,1);
});
test('real shell: action serial changes, wrap, cancellation and host-owned focus preserve first presses',async()=>{
  const h=await harness({multiplayer:false});await h.start();
  assert.equal(h.document.hasFocus(),false);
  await h.request(controls(1,{escapeSerial:1}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
  h.window.dispatchEvent({type:'blur'});assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);
  await h.request(controls(1,{escapeSerial:1}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
  await h.request(controls(0xffffffff,{escapeSerial:0xffffffff}));
  await h.request(controls(0,{escapeSerial:0}));
  assert.equal(h.native.bombPulses,3);assert.equal(h.native.escapePulses,3);
});
test('real shell: direct touch remains an absolute target after viewport-to-canvas mapping',async()=>{
  const h=await harness({multiplayer:false});await h.start();
  await h.request(direct);
  await h.request({...direct,type:'move',x:.6});
  await h.request({...direct,type:'move',x:.6});
  assert.deepEqual(h.calls.filter(call=>call[0]==='touch'),
    [['touch',0,9,.5,.5],['touch',1,9,.625,.5],['touch',1,9,.625,.5]]);
});
test('real shell: letterbox gestures use the same lifecycle and do not resurrect an old pointer',async()=>{
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
test('real shell: optional Practice stays capability-based and preserves its command/tick/close lifecycle',async()=>{
  const plain=await harness({multiplayer:false});await plain.start({thpracEnabled:true});assert.equal(plain.native.practiceImports,0);
  const h=await harness({multiplayer:false,withPractice:true});await h.start({thpracEnabled:true});
  assert.equal(h.native.practiceImports,1);assert.equal(h.calls.filter(call=>call[0]==='practice-configure').at(-1)[1],true);
  await h.request({command:'thprac-mouse',type:'down'});await h.request(direct);h.Module.runtimeFinish(0,0);
  assert.equal(count(h,'practice-mouse'),1);assert.equal(count(h,'practice-tick'),1);assert.equal(count(h,'touch'),1);
  await h.window.__th10Runtime.stop();assert.equal(count(h,'practice-close'),1);
});
test('real shell: rollback modes normalize to native mode 0 and D0, including challenge ABI',async()=>{
  for(const mode of [0,2]){
    for(const challenge of [false,true]){
      const h=await harness();await h.start({netplayAdonisMode:mode,netplayInputDelay:4,netplayInputDelayAuto:true,netplayChallengeMode:challenge});
      const words=h.native.configs[0];assert.equal(words[0],challenge?5:3);assert.equal(words[7],0);
      assert.equal(h.window.__eaglerNetplayInputDelayFrames,0);assert.equal(count(h,'calibration-start'),0);
      if(challenge){assert.equal(words.length,22);assert.deepEqual(words.slice(14,17),[0,0,2]);assert.equal(words[21],1);}
      else assert.equal(words.length,14);
    }
  }
});
test('real shell: pure delay retains fixed/automatic timing and rejects a mixed policy',async()=>{
  for(const [delay,automatic] of [[4,false],[0,true]]){
    const h=await harness();await h.start({netplayAdonisMode:1,netplayInputDelay:delay,netplayInputDelayAuto:automatic});
    const words=h.native.configs[0];assert.equal(words[0],4);assert.equal(words[7],delay);
    assert.deepEqual(words.slice(14,17),[1,+automatic,2]);assert.equal(count(h,'calibration-start'),1);
  }
  const h=await harness();await h.configure({netplayAdonisMode:1,netplayInputDelay:4,netplayInputDelayAuto:true});
  const pending=h.send({command:'launch'});await until(()=>h.ack(pending),'invalid policy ACK');
  assert.equal(h.ack(pending).ok,false);assert.match(h.ack(pending).error,/measured timing policy/);
  assert.equal(h.native.configs.length,0);await h.request(direct);assert.equal(count(h,'touch'),0);
});
