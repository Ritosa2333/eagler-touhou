import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import vm from 'node:vm';
const source=await readFile(new URL('../browser/runtime-crash-reporter.js',import.meta.url),'utf8');
function fixture(game='th15',multiplayer=true) {
  const listeners=new Map(),shown=[],timers=[];
  const url=new URL(`https://test.example/runtime/${game}/${multiplayer?'multiplayer/':''}${'a'.repeat(64)}/${game}.html?room=private`);
  const world={console:{warn(){},error(){}},location:url,navigator:{userAgent:'test-browser',language:'zh-CN',languages:['zh-CN'],onLine:true},
    document:{currentScript:{src:new URL('runtime-crash-reporter.js',url).href},readyState:'loading',addEventListener(){},visibilityState:'visible'},
    performance:{memory:{usedJSHeapSize:100,totalJSHeapSize:200,jsHeapSizeLimit:1000}},
    innerWidth:400,innerHeight:800,devicePixelRatio:2,URL,URLSearchParams,TextDecoder,Uint8Array,Uint32Array,Int32Array,Float32Array,ArrayBuffer,Error,
    fetch:async()=>({ok:false}),setInterval:f=>timers.push(f),
    addEventListener:(event,callback)=>listeners.set(event,callback),
    __eaglerCrashReportHostV1:{show:value=>shown.push(value)}};
  world.window=world;world.parent=world;
  const context=vm.createContext(world);
  vm.runInContext(source,context);
  return {world,api:world.__eaglerCrashReporterV1,listeners,shown,timers,dispatch:data=>
    vm.runInContext('addEventFixture(window)',Object.assign(context,{addEventFixture:source=>
      listeners.get('message')({origin:url.origin,source,data})}))};
}
test('abort preserves stderr, state, memory and stacks without calling crashed WASM',()=>{
  const {world,api,shown}=fixture();let calls=0;
  const core={HEAPU8:new Uint8Array(4096),_th15_frame:()=>{++calls;return 88124;},_th15_phase:()=>1};
  api.bindCore(core,{build:{version:'test',execution:{sha256:'wasm-identity'}},context:()=>({stage:6,spell:'Pristine Lunacy',token:'private'})});
  world.console.error('Cannot enlarge memory');const before=calls;
  core.onAbort('Aborted()');api.sample();
  const report=api.report();
  assert.equal(calls,before);assert.equal(report.game,'th15');assert.equal(report.multiplayer,true);
  assert.equal(report.samples[0].frame,88124);assert.equal(report.samples[0].gameState.stage,6);
  assert.equal(report.wasmMemoryBytes,4096);assert.equal(report.logs[0].values[0],'Cannot enlarge memory');
  assert.match(report.errors[0].error.stack,/Aborted/);assert.equal(report.runtime.build.execution.sha256,'wasm-identity');
  assert.equal(report.samples[0].gameState.token,undefined);assert.ok(shown.length);
  assert.ok(!JSON.stringify(report).includes('room=private'));
});
test('warnings stay nonfatal; unhandled rejection and native errors retain original causes',()=>{
  const {world,api,listeners}=fixture('th12',false);
  world.console.warn('music not ready');assert.equal(api.report(),undefined);
  const error=new Error('Junko opening');listeners.get('unhandledrejection')({reason:error});
  api.fatal(error,{source:'native-fatal'});api.fatal(error,{source:'native-fatal'});
  assert.equal(api.report().errors.length,2);assert.equal(api.report().errors[0].error.stack,error.stack);
  assert.equal(api.report().game,'th12');assert.equal(api.report().multiplayer,false);
});
test('rolling history stays bounded and circular values serialize safely',()=>{
  const {api,world}=fixture();const cyclic={};cyclic.self=cyclic;
  api.bindCore({HEAPU8:new Uint8Array(1024)});
  for(let n=0;n<150;n++){world.console.error(n,cyclic);api.sample();}
  api.fatal('failure');const report=api.report();
  assert.equal(report.logs.length,100);assert.equal(report.samples.length,30);
  assert.match(JSON.stringify(report),/circular/);
});

test('RTC failures retain explicit browser cause fields and channel queues without signaling secrets',()=>{
  const {api,world}=fixture('th12',true);
  world.__eaglerPeerTransport={route:'rtc',received:[],receivedHead:0,
    channelEvents:[{peer:1,kind:'control',event:'error',error:{name:'OperationError',message:'User-Initiated Abort',
      errorDetail:'sctp-failure',sctpCauseCode:12},channel:{readyState:'closing',bufferedAmount:8192}}],
    peers:new Map([[1,{pc:{connectionState:'connected',iceConnectionState:'connected',sctp:{state:'closed'},
      localDescription:{sdp:'private-sdp'},configuration:{iceServers:[{credential:'private-turn'}]}},
      inputDc:{readyState:'open',bufferedAmount:0},controlDc:{readyState:'closing',bufferedAmount:8192},
      adaptive:{snapshot:()=>({pendingControl:2,pendingBytes:160})}}]])};
  api.fatal(new Error('RTC peer P2 control channel error'));
  const report=api.report();
  assert.equal(report.transport.channelEvents[0].error.sctpCauseCode,12);
  assert.equal(report.transport.channelEvents[0].error.errorDetail,'sctp-failure');
  assert.equal(report.transport.peers[0].sctpState,'closed');
  assert.equal(report.transport.peers[0].control.bufferedAmount,8192);
  assert.equal(report.transport.peers[0].route.pendingControl,2);
  assert(!JSON.stringify(report).includes('private-sdp'));assert(!JSON.stringify(report).includes('private-turn'));
});
test('TH12 native gameplay getters wait until a session exists',()=>{
  const {api}=fixture('th12',false);let reads=0,session=null;
  const core={HEAPU8:new Uint8Array(1024),_th12_hud:()=>{++reads;return 0;}};
  api.bindCore(core,{context:()=>({session})});assert.equal(reads,0);
  session={stage:1};api.sample();assert.equal(reads,1);
});
test('TH12 reports its eight-word rollback ABI independently of TH15',()=>{
  const {api}=fixture('th12',true),heap=new Uint8Array(256);
  new Uint32Array(heap.buffer,16,8).set([1,7,42,120,1,118,130,1]);
  api.bindCore({HEAPU8:heap,_th12_mp_rollback_status:()=>16});
  api.fatal(new Error('report-only fixture'));
  const state=api.report().samples.at(-1).rollback;
  assert.equal(state.corrections,7);assert.equal(state.replayedFrames,42);
  assert.equal(state.committedFrames,120);assert.equal(state.frontier,130);
  assert.equal(state.requestedFrame,118);assert.equal(state.correcting,true);
  assert(!Object.hasOwn(state,'presentationFps'));
});
test('TH14 records Extra and rollback state before abort, and omits Relay configuration',()=>{
  const {api,world,dispatch,timers}=fixture('th14',true),heap=new Uint8Array(256);
  new Uint32Array(heap.buffer,16,6).set([15637,15636,523,2182,510,12]);
  new Uint32Array(heap.buffer,48,5).set([1,133,429,640,212]);
  let reads=0;
  api.bindCore({HEAPU8:heap,_th11_stage:()=>{++reads;return 7;},_th11_frame:()=>7811,
    _th11_phase:()=>1,_th11_dynamic_allocs:()=>128,_th14_checkpoint_bytes:()=>96,
    _th14_room_rollback:()=>16,_th14_battle_diagnostics:()=>48},{autoSample:false,hookAbort:false});
  timers[0]();
  assert.equal(reads,1);
  world.__eaglerNetplayLanActive=true;world.__eaglerNetplayLanFrame=15639;world.__eaglerNetplayLanConfirmed=15637;
  dispatch({protocol:'eagler-touhou/1',command:'configure',
    options:{netplayPlayerCount:3,netplayUrl:'wss://relay.example/?room=private',iceServers:[{urls:'turn:private'}],external:'private'}});
  world.console.error('connection failed wss://relay.example/?room=private');
  api.fatal(new Error('allocation failed'),{source:'wasm-abort'});
  api.sample();
  const report=api.report(),state=report.samples.at(-1);
  assert.equal(reads,1);
  assert.equal(report.game,'th14');
  assert.equal(state.stage,7);
  assert.equal(state.wasmAllocatedBytes,128);
  assert.equal(state.checkpointArenaBytes,96);
  assert.equal(state.rollback.confirmedFrame,15636);
  assert.equal(state.battle.spellId,133);
  assert.equal(report.lastFrame.frame,15639);
  assert.equal(report.events[0].options.netplayPlayerCount,3);
  assert(!JSON.stringify(report).includes('private'));
});
