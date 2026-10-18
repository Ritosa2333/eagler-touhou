import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {SourceTextModule,SyntheticModule,createContext} from 'node:vm';

// Executes the complete production shell, its actual message/lifecycle
// listeners and real host/keyboard helpers. Native, startup branding and the
// MP session seam are mocks: this is reception/ownership evidence, not a
// browser/native gameplay or physical touch latency test. The only source
// substitution is the same TH11_BUILD_INFO variant stamp used by packaging.
const runtime=resolve(dirname(fileURLToPath(import.meta.url)),'../../th11_web/sdl-runtime');
const sourcePath=process.env.TH11_SHELL_UNDER_TEST||resolve(runtime,'shell.mjs');
const source=readFileSync(sourcePath,'utf8');
const stamp="/*TH11_BUILD_INFO*/{version:'development-incomplete',completeGame:false,multiplayer:false}";
assert(source.includes(stamp),'Fixture must stamp the actual TH11 build marker');
const hash=value=>createHash('sha256').update(value).digest('hex');
console.info(JSON.stringify({schema:'th11-managed-touch-source/1',shell:sourcePath,shellSha256:hash(source),
  hostSha256:hash(readFileSync(resolve(runtime,'eagler-host.mjs'))),
  keyboardSha256:hash(readFileSync(resolve(runtime,'directory-keyboard.mjs'))),
  testSha256:hash(readFileSync(fileURLToPath(import.meta.url))),
  native:false,browser:false,network:false}));
const settle=()=>new Promise(resolve=>setImmediate(resolve));
const gate=()=>{let release;const promise=new Promise(resolve=>{release=resolve;});return {promise,release};};
async function until(predicate,label){
  for(let i=0;i<60;++i){if(predicate())return;await settle();}
  assert.fail('Did not reach '+label);
}
function events(extra={}){
  const listeners=new Map();
  return {...extra,addEventListener(name,callback){if(!listeners.has(name))listeners.set(name,[]);listeners.get(name).push(callback);},
    dispatchEvent(event){for(const callback of listeners.get(event.type)||[])callback(event);return true;}};
}
async function harness({multiplayer=true,holdInitialization=false,holdData=false,epoch=23}={}){
  const acknowledgments=[],notifications=[],calls=[],errors=[];
  const canvas=events({focus(){},getBoundingClientRect:()=>({left:80,top:0,width:640,height:480})});
  const nodes={canvas,'#error':{textContent:''},'#loading':{textContent:''}};
  const document=events({hidden:false,hasFocus:()=>false,documentElement:{style:{}},body:events({style:{}}),
    querySelector:selector=>nodes[selector]??null});
  const location=new URL('http://localhost/runtime/th11/multiplayer/th11.html?runtimeEpoch='+epoch+'&managedData=1&gameGeneration=g1');
  const window=events({}),frame={isConnected:true,contentDocument:document,contentWindow:window};
  window.frameElement=frame;window.document=document;
  const initialization=gate(),data=gate();if(!holdInitialization)initialization.release();if(!holdData)data.release();
  const parent={location,postMessage(message,origin){
    assert.equal(origin,location.origin);const copy=JSON.parse(JSON.stringify(message));
    if(typeof copy.request==='string'){
      assert(!acknowledgments.some(old=>old.request===copy.request),'Duplicate command ACK');
      acknowledgments.push(copy);
    }else notifications.push(copy);
  },__eaglerPrepareManagedRuntimeDataV1:async request=>{
    assert.equal(request.game,'th11');assert.equal(request.epoch,epoch);calls.push(['data']);
    await data.promise;return {buffer:new ArrayBuffer(32)};
  }};
  let syncBlock=null,resourceBlock=null,launchBlock=null,sequence=0,session;
  const native={fire:false,focus:false,x:0,y:0,bomb:0,escape:0,bombPulses:0,escapePulses:0,
    initializations:0,starts:0,updates:0};
  const cancel=()=>{native.focus=false;native.x=native.y=0;calls.push(['cancel']);};
  const memory={buffer:new ArrayBuffer(65536)};
  const core={memory,
    sdl_touch:(...args)=>calls.push(['touch',...args]),sdl_touch_cancel:cancel,
    sdl_touch_options:(...args)=>calls.push(['options',...args]),sdl_touch_gestures(){},sdl_touch_mode(){},
    sdl_touch_controls(fire,focus,bomb,escape,x,y){
      calls.push(['controls',fire,focus,bomb,escape,x,y]);native.fire=!!fire;native.focus=!!focus;native.x=x;native.y=y;
      if(bomb!==native.bomb)++native.bombPulses;if(escape!==native.escape)++native.escapePulses;
      native.bomb=bomb;native.escape=escape;
    },
    th11_key:(...args)=>calls.push(['key',...args]),th11_keys_clear:cancel,
    th11_initialize(){++native.initializations;cancel();return 1;},
    th11_prepare_loading:()=>1,th11_draw_loading:()=>1,th11_always_hitbox(){},th11_music_enabled(){},
    th11_phase:()=>0,th11_return_title:()=>1,th11_error:()=>0,th11_save_scores:()=>1,
    th11_loop_start(){++native.starts;calls.push(['start']);},th11_loop_stop:()=>calls.push(['stop']),
    sdl_loop_pause:value=>calls.push(['pause',value]),th11_audio_close:()=>calls.push(['audio-close'])
  };
  const files=new Map();
  const FS={mkdirTree(){},mount(){},symlink(){},unlink:path=>files.delete(path),stat:()=>({mode:1,size:1}),
    isFile:()=>true,readdir:()=>[],writeFile:(path,bytes)=>files.set(path,bytes),
    readFile:path=>files.get(path)||new Uint8Array(4),
    syncfs(_populate,callback){calls.push(['sync']);const wait=syncBlock?.promise??Promise.resolve();wait.then(()=>callback(null));}};
  const Module={FS,IDBFS:{},HEAPU8:new Uint8Array(memory.buffer),SDL3:{audioContext:{state:'running',async resume(){this.state='running';}}}};
  const fetch=async input=>{
    const url=new URL(input,location);
    if(url.pathname.endsWith('/held-font.bin')){calls.push(['resource']);if(resourceBlock)await resourceBlock.promise;}
    return {ok:true,json:async()=>url.pathname.endsWith('/manifest.json')?
      {game:'th11',builtAt:'2026-10-13',execution:{sha256:'a'.repeat(64)}}:
      {schema:'eagler-sdl-resources/1',game:'th11',resources:[]},
      arrayBuffer:async()=>new Uint8Array([1,2,3,4]).buffer};
  };
  const context=createContext({window,parent,document,location,innerWidth:800,innerHeight:480,
    navigator:{},URL,URLSearchParams,TextEncoder,TextDecoder,Uint8Array,Uint32Array,Int32Array,ArrayBuffer,
    performance:{now:()=>3000,mark(){}},fetch,
    console:{log(){},warn(){},debug(){},error:value=>errors.push(String(value))},
    queueMicrotask,requestAnimationFrame:callback=>queueMicrotask(()=>callback(3000)),
    setInterval:()=>1,clearInterval(){},setTimeout:callback=>{queueMicrotask(callback);return 1;},clearTimeout(){},
    WebAssembly:{instantiateStreaming:async()=>({instance:{exports:core},module:{}})}});
  const mocks={
    './th11-sdl.mjs':{default:async options=>{
      await initialization.promise;
      await new Promise(ready=>options.instantiateWasm({},()=>ready()));
      return Module;
    }},
    './startup-branding.mjs':{installStartupBranding:async()=>{},finishStartupAnimation:async()=>{}},
    './motion-replay.mjs':{validateMotionReplay:()=>true},
    './multiplayer.mjs':{createMultiplayerRuntime:({getOptions})=>{
      let active=false,failed=false;
      session={
        async launch(){active=true;failed=false;calls.push(['mp-launch']);if(launchBlock)await launchBlock.promise;return {runLoop:true};},
        acceptInput:()=>active&&!failed&&!getOptions().netplaySpectator,
        fail(){failed=true;},keyboard:()=>false,validateOptions(){},applyOptions(){},afterAudioResume(){},
        flush:()=>calls.push(['replay-save']),frame(){},stop(){active=false;},inspect(){}
      };return session;
    }}
  };
  const modules=new Map();
  const dependency=async name=>{
    if(modules.has(name))return modules.get(name);
    let module;
    if(name==='./direct-input-bridge.mjs')
      module=new SourceTextModule(readFileSync(resolve(runtime,'../../third_party/eagler-common/browser/direct-input-bridge.mjs'),'utf8'),{context,identifier:name});
    else if(['./directory-keyboard.mjs','./eagler-host.mjs','./keyboard.mjs'].includes(name))
      module=new SourceTextModule(readFileSync(resolve(runtime,name),'utf8'),{context,identifier:resolve(runtime,name)});
    else{
      const values=mocks[name];assert(values,'Unexpected shell dependency '+name);
      module=new SyntheticModule(Object.keys(values),function(){for(const [key,value] of Object.entries(values))this.setExport(key,value);},{context,identifier:name});
    }
    const linked=Promise.resolve(module.link(dependency)).then(()=>module);modules.set(name,linked);return linked;
  };
  const stamped=source.replace(stamp,"/*TH11_BUILD_INFO*/{version:'fixture',completeGame:true,multiplayer:"+multiplayer+"}");
  const shell=new SourceTextModule(stamped,{context,identifier:sourcePath,
    importModuleDynamically:async name=>{const module=await dependency(name);if(module.status==='linked')await module.evaluate();return module;}});
  await shell.link(dependency);await shell.evaluate();
  const send=(fields,overrides={})=>{
    const request='r'+(++sequence),message={protocol:'eagler-touhou/1',game:'th11',epoch,request,...fields};
    window.dispatchEvent({type:'message',source:parent,origin:location.origin,data:message,...overrides});return request;
  };
  const ack=request=>acknowledgments.find(message=>message.request===request);
  const request=async fields=>{const id=send(fields);await until(()=>ack(id),'ACK '+fields.command);assert.equal(ack(id).ok,true,ack(id).error);return ack(id);};
  return {core,Module,native,calls,errors,notifications,document,window,frame,parent,send,ack,request,
    releaseInitialization:initialization.release,releaseData:data.release,
    async ready(){initialization.release();data.release();await until(()=>notifications.some(message=>message.event==='ready')||errors.length,'shell ready');assert.deepEqual(errors,[]);},
    async start(extra={}){
      await this.ready();await request({command:'configure',language:'lang_ja',music:'none',
        options:{touchEnabled:true,touchMovementMode:'touch',...(multiplayer?{netplayMode:'lan'}:{}),...extra}});
      await request({command:'launch'});
    },
    blockSync(){const blocked=gate();syncBlock=blocked;return ()=>{syncBlock=null;blocked.release();};},
    blockResource(){const blocked=gate();resourceBlock=blocked;return ()=>{resourceBlock=null;blocked.release();};},
    blockLaunch(){const blocked=gate();launchBlock=blocked;return ()=>{launchBlock=null;blocked.release();};},
    failSession(){session.fail();}
  };
}
const direct={command:'direct-touch',type:'down',id:9,x:.5,y:.5};
const controls=(bombSerial=0,extra={})=>({command:'touch-controls',controls:{fireEnabled:true,focusEnabled:true,
  joystickX:16384,joystickY:-16384,bombSerial,escapeSerial:0,...extra}});
const count=(h,name)=>h.calls.filter(call=>call[0]===name).length;

test('real shell: immediate carrier shares live dispatch and never queues/ACKs or advances simulation',async()=>{
 const h=await harness();await h.start();const bridge=h.window.__eaglerDirectInputBridge;
 assert.equal(bridge.schema,'eagler-touhou/direct-input/1');assert.equal(bridge.epoch,23);
 const submit=fields=>bridge.submit({protocol:'eagler-touhou/1',game:'th11',epoch:23,...fields});
 const release=h.blockSync();
 try{
  const pending=h.send({command:'sync'});await until(()=>count(h,'sync')>1,'blocked sync');
  const starts=h.native.starts,bombs=h.native.bombPulses;
  assert.equal(submit(direct),true);assert.equal(count(h,'touch'),1,'synchronous before returning to browser');
  assert.equal(submit(controls(1)),true);assert.equal(submit(controls(1)),true);
  assert.equal(h.native.bombPulses,bombs+1,'same action serial not replayed by bridge');
  assert.equal(submit({...direct,epoch:22}),false);assert.equal(submit({...direct,request:'not-an-input-rpc'}),false);
  assert.equal(submit({command:'launch'}),false);assert.equal(count(h,'touch'),1);
  assert.equal(h.ack(pending),undefined);assert.equal(h.ack('not-an-input-rpc'),undefined);
  assert.equal(h.native.starts,starts);assert.equal(h.native.updates,0);
 }finally{release();await settle();}
});
test('real shell: immediate input cannot re-arm hidden, spectator or retired documents',async()=>{
 for(const spectator of [false,true]){
  const h=await harness();await h.start({netplaySpectator:spectator});
  const submit=fields=>h.window.__eaglerDirectInputBridge.submit({protocol:'eagler-touhou/1',game:'th11',epoch:23,...fields});
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});
  const before=count(h,'touch'),bombs=h.native.bombPulses;
  assert.equal(submit(direct),true);assert.equal(submit(controls(71)),true);
  assert.equal(count(h,'touch'),before);assert.equal(h.native.bombPulses,bombs);
  h.document.hidden=false;
  assert.equal(submit(controls(71)),true);assert.equal(h.native.bombPulses,bombs,'hidden snapshot must not become a future edge');
  h.frame.isConnected=false;
  assert.equal(submit(direct),true);assert.equal(count(h,'touch'),before);
  h.frame.isConnected=true;assert.equal(submit(direct),true);assert.equal(count(h,'touch'),before,'retired document stays retired');
 }
});

test('real shell: input and ACK bypass a blocked IDB queue without simulation work',async()=>{
  const h=await harness();await h.start();const release=h.blockSync();
  try{
    const before=count(h,'sync'),pending=h.send({command:'sync'});
    await until(()=>count(h,'sync')>before,'blocked sync');
    const starts=h.native.starts;
    const key=h.send({command:'keyboard',code:'KeyZ',down:true}),down=h.send(direct),held=h.send(controls(1));
    await settle();
    for(const id of [key,down,held])assert(h.ack(id)?.ok,'Input ACK waited for IDB');
    assert.equal(count(h,'touch'),1);assert.equal(h.native.bombPulses,1);assert.equal(h.ack(pending),undefined);
    assert.equal(h.native.starts,starts);assert.equal(h.native.updates,0);
    await h.request({command:'touch-cancel'});
    assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);assert.equal(h.native.x,0);
    assert.equal(h.native.bombPulses,1);
  }finally{release();await settle();}
});
test('real shell: live resource transfer cannot delay touch or cancellation',async()=>{
  const h=await harness();await h.start();const release=h.blockResource();
  try{
    const pending=h.send({command:'resources',resources:[{path:'/fonts/probe.bin',url:'./held-font.bin',bytes:4}]});
    await until(()=>count(h,'resource')===1,'blocked resource');
    await h.request(direct);await h.request(controls(1));await h.request({command:'touch-cancel'});
    assert.equal(count(h,'touch'),1);assert.equal(h.native.bombPulses,1);assert.equal(h.ack(pending),undefined);
  }finally{release();await settle();}
});
test('real shell: early core and DATA initialization discard inputs immediately, consuming source serials',async()=>{
  for(const setup of [{holdInitialization:true},{holdData:true}]){
    const h=await harness(setup);
    if(setup.holdData)await until(()=>count(h,'data')===1,'DATA wait with core present');
    for(const message of [direct,controls(7),{command:'touch-cancel'}])await h.request(message);
    assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
    await h.start();await h.request(controls(7));assert.equal(h.native.bombPulses,0);
    await h.request(controls(8));assert.equal(h.native.bombPulses,1);
  }
});
test('real shell: launched flag alone does not accept input before async MP launch completes',async()=>{
  const h=await harness();await h.ready();
  await h.request({command:'configure',music:'none',options:{touchEnabled:true,netplayMode:'lan'}});
  const release=h.blockLaunch(),launch=h.send({command:'launch'});
  try{
    await until(()=>count(h,'mp-launch')===1,'MP launch wait');
    await h.request(direct);await h.request(controls(4));
    assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);assert.equal(h.ack(launch),undefined);
  }finally{release();await until(()=>h.ack(launch),'launch completion');}
  assert.equal(h.ack(launch).ok,true);
  await h.request(controls(4));assert.equal(h.native.bombPulses,0);
  await h.request(controls(5));assert.equal(h.native.bombPulses,1);
});
test('real shell: hidden and blur cleanup preserves fire and does not replay rejected actions',async()=>{
  const h=await harness();await h.start();await h.request(controls(1,{escapeSerial:1}));
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  const before=count(h,'controls');await h.request(direct);await h.request(controls(2,{escapeSerial:2}));
  assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),before);
  assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);assert.equal(h.native.x,0);
  h.document.hidden=false;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  await h.request(controls(2,{escapeSerial:2}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
  h.window.dispatchEvent({type:'blur'});assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);
  await h.request(controls(3,{escapeSerial:3}));assert.equal(h.native.bombPulses,2);assert.equal(h.native.escapePulses,2);
});
test('real shell: stop/relaunch retains TH11 native serial mapping and consumes stopped input',async()=>{
  const h=await harness();await h.start();await h.request(controls(1));
  const release=h.blockSync(),stopping=h.window.__th11Runtime.stop();
  try{
    await h.request(direct);await h.request(controls(2));assert.equal(count(h,'touch'),0);assert.equal(h.native.bombPulses,1);
  }finally{release();await stopping;}
  await h.request(controls(3));await h.request({command:'launch'});
  await h.request(controls(3));assert.equal(h.native.bombPulses,1);
  await h.request(controls(4));assert.equal(h.native.bombPulses,2);
});
test('real shell: spectator, disabled controls and failed sessions remain ineligible',async()=>{
  for(const extra of [{netplaySpectator:true},{touchEnabled:false}]){
    const h=await harness();await h.start(extra);await h.request(direct);await h.request(controls(5));
    assert.equal(count(h,'touch'),0);assert.equal(count(h,'controls'),0);
    await h.window.__th11Runtime.stop();await h.start();
    await h.request(controls(5));assert.equal(h.native.bombPulses,0);
    await h.request(controls(6));assert.equal(h.native.bombPulses,1);
    h.failSession();await h.request(direct);await h.request(controls(7));assert.equal(count(h,'touch'),0);assert.equal(h.native.bombPulses,1);
  }
});
test('real shell: Replay menu keeps its existing input contract without becoming a player',async()=>{
  const h=await harness();await h.start({netplayMode:undefined,replayViewer:true});
  await h.request(direct);await h.request(controls(1));assert.equal(count(h,'touch'),1);assert.equal(h.native.bombPulses,1);
});
test('real shell: pagehide, detach, replaced document and Window owner cannot revive touch',async()=>{
  for(const retiredBy of ['pagehide','detached','document','window']){
    const h=await harness();await h.start();await h.request(controls(1));
    if(retiredBy==='pagehide')h.window.dispatchEvent({type:'pagehide'});
    else if(retiredBy==='detached')h.frame.isConnected=false;
    else if(retiredBy==='document')h.frame.contentDocument={};
    else h.frame.contentWindow={};
    await h.request(direct);await h.request(controls(2));
    assert.equal(count(h,'touch'),0);assert.equal(h.native.focus,false);
    h.frame.isConnected=true;h.frame.contentDocument=h.document;h.frame.contentWindow=h.window;
    h.window.dispatchEvent({type:'pageshow'});await h.request(direct);assert.equal(count(h,'touch'),0,retiredBy);
  }
});
test('real shell: persisted BFCache resume retains the same owner without replaying actions or reviving retirement',async()=>{
  for(const outcome of ['resume','replaced','terminal']){
    const h=await harness();await h.start();await h.request(controls(1,{escapeSerial:1}));
    h.window.dispatchEvent({type:'pagehide',persisted:true});
    await h.request(direct);await h.request(controls(2,{escapeSerial:2}));
    assert.equal(count(h,'touch'),0);assert.equal(h.native.bombPulses,1);
    assert.equal(h.native.fire,true);assert.equal(h.native.focus,false);assert.equal(h.native.x,0);
    if(outcome==='replaced')h.frame.contentDocument={};
    if(outcome==='terminal')h.window.dispatchEvent({type:'pagehide',persisted:false});
    h.window.dispatchEvent({type:'pageshow',persisted:true});await settle();
    h.frame.contentDocument=h.document;
    await h.request(controls(2,{escapeSerial:2}));assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);
    await h.request(direct);assert.equal(count(h,'touch'),outcome==='resume'?1:0);
    await h.request(controls(3,{escapeSerial:3}));
    assert.equal(h.native.bombPulses,outcome==='resume'?2:1);assert.equal(h.native.escapePulses,outcome==='resume'?2:1);
  }
});
test('real shell: document retirement during awaited launch fences the continuation',async()=>{
  const h=await harness();await h.ready();
  await h.request({command:'configure',music:'none',options:{touchEnabled:true,netplayMode:'lan'}});
  const release=h.blockLaunch(),launch=h.send({command:'launch'});
  await until(()=>count(h,'mp-launch')===1,'MP launch wait');
  h.window.dispatchEvent({type:'pagehide'});release();
  await until(()=>h.ack(launch),'superseded launch ACK');
  assert.equal(h.ack(launch).ok,false);assert.equal(h.native.starts,0);
  await h.request(direct);assert.equal(count(h,'touch'),0);
});
test('real shell: source/origin/protocol/game/epoch rejection happens before serial state changes',async()=>{
  const h=await harness();await h.start();
  const message={protocol:'eagler-touhou/1',game:'th11',epoch:23,...controls(1)};
  for(const override of [{source:{}},{origin:'http://other.invalid'},
    {data:{...message,protocol:'wrong'}},{data:{...message,game:'th09'}},{data:{...message,epoch:24}}]){
    const id=h.send(controls(1),override);await settle();assert.equal(h.ack(id),undefined);
  }
  assert.equal(count(h,'controls'),0);await h.request(controls(1));assert.equal(h.native.bombPulses,1);
});
test('real shell: flat Host snapshots, uint32 wrap, axes and parent overlay focus remain valid',async()=>{
  const h=await harness({multiplayer:false});await h.start();
  assert.equal(h.document.hasFocus(),false,'A legitimate parent overlay owns focus');
  const c=controls(1,{escapeSerial:1,joystickX:50000,joystickY:-50000,touchSensitivity:200}).controls;
  await h.request({command:'touch-controls',...c});await h.request(direct);
  assert.equal(h.native.bombPulses,1);assert.equal(h.native.escapePulses,1);assert.equal(count(h,'touch'),1);
  assert.equal(h.native.x,32767);assert.equal(h.native.y,-32767);
  await h.request(controls(0xffffffff,{escapeSerial:0xffffffff}));
  await h.request(controls(0,{escapeSerial:0}));
  assert.equal(h.native.bombPulses,3);assert.equal(h.native.escapePulses,3);
});
test('real shell: letterbox and managed pointer owners share cancellation eligibility',async()=>{
  const h=await harness({multiplayer:false});await h.start();const target={setPointerCapture(){}};
  const pointer=(type,x)=>h.document.dispatchEvent({type,pointerType:'touch',pointerId:45,target,clientX:x,clientY:240,
    preventDefault(){},stopPropagation(){}});
  pointer('pointerdown',60);pointer('pointermove',70);assert.equal(count(h,'touch'),2);
  h.document.hidden=true;h.document.dispatchEvent({type:'visibilitychange'});
  pointer('pointermove',90);pointer('pointerdown',60);assert.equal(count(h,'touch'),2);
  h.document.hidden=false;h.document.dispatchEvent({type:'visibilitychange'});await settle();
  pointer('pointermove',90);assert.equal(count(h,'touch'),2);
  pointer('pointerdown',60);assert.equal(count(h,'touch'),3);
});
