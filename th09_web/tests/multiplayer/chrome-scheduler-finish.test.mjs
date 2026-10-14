// Evaluate only the current harness finalizer in a VM with in-memory stubs.
// Never import/execute the harness, start Chrome, or touch trace artifacts.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {posix} from 'node:path';
import vm from 'node:vm';
import test from 'node:test';

const harnessUrl=new URL('./smoothness-browser.mjs',import.meta.url);
const source=readFileSync(harnessUrl,'utf8');
const begin=source.indexOf('async function finishSchedulerTrace(reason){');
const end=source.indexOf('\nif(headedControl)parameters.headedLaunch',begin);
assert(begin>=0&&end>begin,'Locate the real finalizer; do not copy its implementation');
const finalizer=source.slice(begin,end);
const hash=value=>createHash('sha256').update(value).digest('hex');
const identity={harnessSha256:hash(source),finalizerSha256:hash(finalizer)};
const deferred=()=>{let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return {promise,resolve,reject};};
const snapshot=side=>({version:1,enabled:true,side,status:'stopped',collectionValid:true,events:[{name:'OwnedSafeEvent',pid:side,ts:10}],errors:[]});
function fixture({enabled=true,controllers=[],writeFailureSide=null}={}){
 const files=new Map(),calls={stop:[],write:[],read:[]},firstWrite=deferred();
 const schedulerTraces=controllers.map((controller,side)=>({stop(reason){calls.stop.push({side,reason});return controller.stop(reason);}}));
 const context=vm.createContext({schedulerTrace:enabled,schedulerTraceFinish:undefined,schedulerTraceEvidence:undefined,browsers:[{},{}],schedulerTraces,out:'/virtual/owned-traces',label:'contract',resolve:posix.resolve,
  writeFileSync(file,text,options){calls.write.push(file);firstWrite.resolve(file);assert.equal(options.flag,'wx');assert(!files.has(file),'Exclusive output must never overwrite');if(writeFailureSide!==null&&file.includes('-P'+(writeFailureSide+1)+'-'))throw Error('private-write-sentinel');files.set(file,text);},
  statSync(file){calls.read.push({kind:'stat',file});assert(files.has(file));return {size:Buffer.byteLength(files.get(file))};},
  sha(file){calls.read.push({kind:'sha',file});assert(files.has(file));return hash(files.get(file));}
 });
 vm.runInContext(finalizer+'\nglobalThis.finish=finishSchedulerTrace;',context,{timeout:1000});
 return {context,finish:context.finish,files,calls,firstWrite:firstWrite.promise};
}

test('disabled finalizer does not read controllers, stop, or write/read artifacts',async t=>{
 t.diagnostic(JSON.stringify(identity));
 const f=fixture({enabled:false});
 f.context.browsers=new Proxy([],{get(){throw Error('disabled browser access');}});
 f.context.schedulerTraces=new Proxy([],{get(){throw Error('disabled controller access');}});
 assert.equal(await f.finish('capture-stop'),undefined);
 assert.equal(await f.finish('finally'),undefined);
 assert.equal(f.context.schedulerTraceFinish,undefined);
 assert.deepEqual(f.calls,{stop:[],write:[],read:[]});assert.equal(f.files.size,0);
});

test('fast stop rejection waits for slow peer, retains its artifact, and memoizes concurrent/repeated finish',async()=>{
 const slow=deferred();
 const f=fixture({controllers:[{stop:()=>Promise.reject(Error('private-stop-sentinel'))},{stop:()=>slow.promise}]});
 let settled=false;const first=f.finish('capture-stop').then(value=>{settled=true;return value;});
 const concurrent=f.finish('failure');
 await Promise.resolve();await Promise.resolve();
 assert.equal(settled,false);assert.equal(f.calls.stop.length,2);assert.equal(f.calls.write.length,0);
 slow.resolve(snapshot(1));const result=await first;
 assert.strictEqual(await concurrent,result);assert.strictEqual(await f.finish('finally'),result);
 assert.equal(result.length,2);assert.equal(result[0].side,0);assert.equal(result[0].status,'finalization-failed');assert.equal(result[0].collectionValid,false);
 assert.equal(result[0].errors[0].kind,'stop-or-artifact-write-failed');
 assert.equal(result[1].side,1);assert.equal(result[1].collectionValid,true);assert(result[1].artifact.path.endsWith('-P2-scheduler-trace.json'));
 assert.equal(Object.hasOwn(result[1],'events'),false);assert.equal(f.files.size,1);assert.equal(JSON.parse([...f.files.values()][0]).events.length,1);
 assert.equal(JSON.stringify(result).includes('private-stop-sentinel'),false);
 assert.equal(f.calls.stop.length,2);assert.equal(f.calls.write.length,1);
 assert(f.calls.stop.every(call=>call.reason==='capture-stop'));
});

test('no large serialization or artifact work precedes both stops, and one write failure stays isolated',async()=>{
 const slow=deferred();
 let serializationReads=0;const fast=snapshot(0),events=fast.events;
 Object.defineProperty(fast,'events',{enumerable:true,get(){++serializationReads;return events;}});
 const f=fixture({writeFailureSide:0,controllers:[{stop:()=>Promise.resolve(fast)},{stop:()=>slow.promise}]});
 let settled=false;const pending=f.finish('failure').then(value=>{settled=true;return value;});
 for(let i=0;i<16;++i)await Promise.resolve();
 assert.equal(settled,false);assert.equal(f.calls.stop.length,2);assert.equal(f.calls.write.length,0);
 assert.equal(serializationReads,0);assert.equal(f.calls.read.length,0);assert.equal(f.files.size,0);
 slow.resolve(snapshot(1));const result=await pending;
 assert(serializationReads>0);
 assert.equal(result[0].status,'finalization-failed');assert.equal(result[0].collectionValid,false);assert.equal(result[1].collectionValid,true);
 assert.equal(JSON.stringify(result).includes('private-write-sentinel'),false);
 assert.equal(f.files.size,1);assert.equal(f.calls.write.length,2);assert.equal(f.calls.read.length,2);
 assert.strictEqual(await f.finish('finally'),result);assert.equal(f.calls.stop.length,2);assert.equal(f.calls.write.length,2);assert.equal(f.calls.read.length,2);
});

const endpointBegin=source.indexOf('function beginCompletedEndpoint(');
const endpointEnd=source.indexOf('async function finishSchedulerTrace(reason){',endpointBegin);
assert(endpointBegin>=0&&endpointEnd>endpointBegin,'Locate actual endpoint functions');
const endpointFunctions=source.slice(endpointBegin,endpointEnd);
const completedAt=source.indexOf("hostProbe?.mark('formal-poll-complete');");
const collectAt=source.indexOf('await collectCompletedEndpoints();',completedAt);
const exportBegin=source.indexOf(' const results=await Promise.all(pages.map((p,side)=>p.evaluate(({endpoint,rafAnomaly})=>{',collectAt);
const exportEnd=source.indexOf('\n if(renderWork)',exportBegin);
assert(completedAt>=0&&collectAt>completedAt&&exportBegin>collectAt&&exportEnd>exportBegin,'Locate real completion/export/qualification path');
const exportBlock=source.slice(exportBegin,exportEnd);
const plain=value=>JSON.parse(JSON.stringify(value));
const flushMicrotasks=async()=>{for(let i=0;i<48;++i)await Promise.resolve();};
const rtcStats=()=>new Map([
 ['transport',{type:'transport',selectedCandidatePairId:'pair'}],
 ['pair',{type:'candidate-pair',state:'succeeded',nominated:true,localCandidateId:'local',remoteCandidateId:'remote',bytesSent:1024,bytesReceived:2048,currentRoundTripTime:.002}],
 ['local',{type:'local-candidate',candidateType:'host',protocol:'udp'}],
 ['remote',{type:'remote-candidate',candidateType:'host',protocol:'udp'}]
]);
function endpointFixture({traced=true,release=true,fallback=false,ended=[20100,20100],beginRejectSide=null}={}){
 const activity=[],drains=[deferred(),deferred()],drainDone=[false,false],pageStates=[];
 const f=fixture({enabled:traced,controllers:drains.map((drain,side)=>({stop(){activity.push('stop-'+side);return drain.promise;}}))});
 const large=side=>{assert(!traced||drainDone.every(Boolean),'Large sample export before both drains');activity.push('large-'+side);};
 const pages=[0,1].map(side=>{
  const rtc=deferred(),timers=new Map(),heap=new Uint32Array(128);for(let i=0;i<heap.length;++i)heap[i]=i+100;
  let now=20101,timerId=0;
  const state={stats:[1204,1204,1100,1110,1204,0,1,1204],hash:77,rtc,timers,heap};
  const samples=[[1,1,4,1200],[2,1,4,1201]];
  Object.defineProperty(samples,'toJSON',{value(){large(side);return this.slice();}});
  const measure={started:100,ended:ended[side],audioStart:10,samples,closed:[]};
  const play={connected:true,disconnected:false};
  const pc={connectionState:'connected',iceConnectionState:'connected',getStats(){activity.push('rtc-'+side);return rtc.promise;}};
  const peer={pc,inputDc:{readyState:'open',ordered:false,maxRetransmits:0,bufferedAmount:0},controlDc:{readyState:'open',ordered:true,maxRetransmits:null,bufferedAmount:0}};
  const transport=fallback?undefined:{peers:new Map([['peer-'+side,peer]])};
  const core={HEAPU32:heap,HEAP32:new Int32Array(heap.buffer),HEAPU8:new Uint8Array(heap.buffer),
   SDL3:{audioContext:{state:'running',currentTime:30}},
   _th09_loop_pause(on){assert.equal(on,1);activity.push('pause-'+side);},
   _th09_network_hash(){return state.hash;},_sdl_stats(){return 0;},_th09_probe_world(){return 128;}};
  const gl={RENDERER:0,getExtension(){return {UNMASKED_RENDERER_WEBGL:1};},getParameter(){return 'ANGLE (Intel test fixture)';}};
  const context=vm.createContext({core,measure,play,impairment:{matched:1000,sent:1000},document:{querySelector(){return {getContext(){assert(!traced||drainDone.every(Boolean));return gl;}};}},
   performance:{timeOrigin:1700000000000+side*10000,now:()=>now+=.1},
   readStats:()=>state.stats.slice(),confirmedHashTrace:[[120,77]],__eaglerNetplayTransport:fallback?'relay':'rtc',
   __eaglerNetplayPath:fallback?'relay':'direct',__eaglerPeerTransport:transport,
   setTimeout(callback,ms){assert.equal(ms,1500);const id=++timerId;timers.set(id,callback);return id;},clearTimeout(id){timers.delete(id);}
  });
  vm.runInContext('globalThis.window=globalThis;',context);
  Object.assign(state,{context,measure,play,pc,peer,transport,core,advance(ms){now+=ms;}});
  pageStates.push(state);
  return {async evaluate(fn,arg){
   if(beginRejectSide===side&&fn.name==='beginCompletedEndpoint')throw Error('begin-page-failure');
   context.__argument=arg===undefined?undefined:plain(arg);
   const value=await vm.runInContext('('+fn.toString()+')(__argument)',context,{timeout:1000});
   return value===undefined?undefined:plain(value);
  }};
 });
 Object.assign(f.context,{pages,release,endpointSnapshots:[],endpointTraceError:undefined,measurementEndpointQualification:undefined,teardownClean:undefined,
  retainedResults:undefined,hostProbe:undefined,limit:1200,fallback,expectedRtcPath:fallback?'any':'direct',rafAnomaly:false});
 vm.runInContext(endpointFunctions+'\nglobalThis.collect=collectCompletedEndpoints;globalThis.qualify=qualifyCompletedEndpoint;',f.context,{timeout:1000});
 const run=()=>vm.runInContext('(async()=>{await collectCompletedEndpoints();'+exportBlock+'\nreturn results;})()',f.context,{timeout:1000});
 const exportOnly=()=>vm.runInContext('(async()=>{'+exportBlock+'\nreturn results;})()',f.context,{timeout:1000});
 return {...f,activity,pageStates,run,exportOnly,
  resolveRtc(side,stats=rtcStats()){pageStates[side].rtc.resolve(stats);},
  resolveDrain(side){drainDone[side]=true;drains[side].resolve(snapshot(side));},
  cleanUpAfterEndpoint(side){const s=pageStates[side];s.advance(15500);s.stats.fill(0);s.hash=0;s.heap.fill(0);s.core.SDL3.audioContext.currentTime+=15.5;
   s.play.connected=false;s.play.disconnected=true;s.pc.connectionState='closed';s.peer.inputDc.readyState='closed';s.peer.controlDc.readyState='closed';
   s.transport?.peers.clear();s.measure.closed.push('Confirmed peer input stopped advancing');}
 };
}

test('both small endpoints start RTC before trace stop; pending RTC and a fast drain cannot trigger large export',async t=>{
 t.diagnostic(JSON.stringify({...identity,endpointFunctionsSha256:hash(endpointFunctions),exportBlockSha256:hash(exportBlock)}));
 const f=endpointFixture(),pending=f.run();await flushMicrotasks();
 assert.equal(f.calls.stop.length,2);assert.equal(f.calls.write.length,0);
 for(const side of [0,1]){
  assert(f.activity.indexOf('pause-'+side)<f.activity.indexOf('rtc-'+side));
  assert(f.activity.indexOf('rtc-'+side)<f.activity.indexOf('stop-0'));
  assert.equal(f.context.endpointSnapshots[side].begin.status,'captured');
  assert.equal(f.context.endpointSnapshots[side].completed,null);
 }
 f.resolveDrain(0);await flushMicrotasks();assert.equal(f.calls.write.length,0);assert(!f.activity.some(x=>x.startsWith('large-')));
 f.resolveRtc(0);f.resolveRtc(1);await flushMicrotasks();
 assert(f.context.endpointSnapshots.every(row=>row.completed?.routeEvidence?.selectedPath==='direct'));
 assert.equal(f.calls.write.length,0);assert(!f.activity.some(x=>x.startsWith('large-')));
 f.resolveDrain(1);const results=await pending;
 assert.equal(f.calls.write.length,2);assert.equal(results.length,2);
 assert(f.context.measurementEndpointQualification.every(row=>row.qualified));assert.equal(f.context.teardownClean,true);
 assert(f.pageStates.every(state=>state.timers.size===0));
});

test('real export keeps frozen native/RTC evidence when a later drain clears runtime and closes the peer',async()=>{
 const f=endpointFixture(),pending=f.run();await flushMicrotasks();f.resolveRtc(0);f.resolveRtc(1);await flushMicrotasks();
 const before=plain(f.context.endpointSnapshots[0].completed);assert.equal(before.routeEvidence.selectedPath,'direct');
 f.cleanUpAfterEndpoint(0);f.resolveDrain(0);f.resolveDrain(1);const results=await pending,r=results[0];
 assert.deepEqual(r.stats,before.stats);assert.equal(r.hash,77);assert.deepEqual(r.sdl,before.sdl);assert.equal(r.audio.advancedSeconds,20);
 assert.equal(r.routeEvidence.selectedPath,'direct');assert.equal(r.routeEvidence.peers[0].selected.state,'succeeded');
 assert.equal(r.measurementEndpoint.phase,'post-workload-pre-drain');assert.equal(r.measurementEndpoint.body.closed.length,0);
 assert.equal(r.measure.samples.length,2);assert.deepEqual(r.measure.closed,['Confirmed peer input stopped advancing']);
 assert(r.postDrain.observedAt>r.measurementEndpoint.routeEvidence.completedAt);assert.deepEqual(r.postDrain.stats,Array(8).fill(0));
 assert.equal(r.postDrain.peerCount,0);assert.equal(r.postDrain.connected,false);assert.equal(r.postDrain.closePrefixMatches,true);
 assert.deepEqual(r.postDrain.lateClosed,['Confirmed peer input stopped advancing']);assert.equal(r.postDrain.teardownClean,false);
 assert.equal(Object.hasOwn(r.postDrain,'selectedPath'),false);assert.equal(Object.hasOwn(r.postDrain,'routeEvidence'),false);
 assert(f.context.measurementEndpointQualification.every(row=>row.qualified));assert.equal(f.context.teardownClean,false);
});

test('relay and disabled tracing use the same bounded endpoint path and retain DEV audit copies',async()=>{
 const f=endpointFixture({traced:false,release:false,fallback:true});const results=await f.run();
 assert.deepEqual(f.activity,['pause-0','pause-1','large-0','large-1']);assert.equal(f.calls.stop.length,0);assert.equal(f.files.size,0);
 assert(f.context.measurementEndpointQualification.every(row=>row.qualified));assert.equal(f.context.teardownClean,true);
 for(const result of results){assert.equal(result.measurementEndpoint.phase,'post-workload-pre-export');assert.equal(result.audited.length,48);assert.equal(result.audited[0],132);assert.equal(result.routeEvidence.selectedPath,'relay');}
});

for(const failure of ['unfinished','query-reject','query-timeout','missing-pair','peer-removed','pc-replaced','owner-replaced','pc-disconnected','input-closed','control-closed','begin-evaluate-reject']){
 test('endpoint failure remains ineligible and drains both traces: '+failure,async()=>{
  const f=endpointFixture({ended:failure==='unfinished'?[null,20100]:[20100,20100],beginRejectSide:failure==='begin-evaluate-reject'?0:null});
  const pending=f.run();await flushMicrotasks();assert.equal(f.calls.stop.length,2);
  const state=f.pageStates[0];
  if(failure==='query-reject')state.rtc.reject(Error('stats-rejected'));
  else if(failure==='query-timeout'){assert.equal(state.timers.size,1);for(const callback of [...state.timers.values()])callback();}
  else{
   if(failure==='peer-removed')state.transport.peers.clear();
   if(failure==='pc-replaced')state.peer.pc={...state.pc};
   if(failure==='owner-replaced')state.context.core={...state.core};
   if(failure==='pc-disconnected')state.pc.connectionState='disconnected';
   if(failure==='input-closed')state.peer.inputDc.readyState='closed';
   if(failure==='control-closed')state.peer.controlDc.readyState='closed';
   f.resolveRtc(0,failure==='missing-pair'?new Map():rtcStats());
  }
  f.resolveRtc(1);f.resolveDrain(0);f.resolveDrain(1);
  const results=await pending,qualification=plain(f.context.measurementEndpointQualification);
  assert.equal(qualification[0].qualified,false);assert.equal(qualification[1].qualified,true);
  assert.equal(f.calls.write.length,2);assert.equal(results[0].measure.samples.length,2);assert.equal(results[1].stats[0],1204);
  if(failure==='query-timeout')assert.match(results[0].routeEvidence.peers[0].error,/RTC stats timeout/);
  if(failure==='query-reject')assert.match(results[0].routeEvidence.peers[0].error,/stats-rejected/);
  if(failure==='begin-evaluate-reject')assert(f.context.endpointSnapshots[0].error);
  assert(f.pageStates.every(s=>s.timers.size===0));
 });
}

test('missing collected RTC endpoint never falls back to current live route or a second getStats query',async()=>{
 const f=endpointFixture({traced:false}),pending=f.context.collect();await flushMicrotasks();f.resolveRtc(0);f.resolveRtc(1);await pending;
 assert(f.context.endpointSnapshots[0].completed.routeEvidence);f.context.endpointSnapshots[0].completed=null;
 const callsBefore=f.activity.filter(x=>x.startsWith('rtc-')).length,results=await f.exportOnly();
 assert.equal(results[0].routeEvidence,null);assert.equal(f.context.measurementEndpointQualification[0].qualified,false);
 assert.equal(f.context.measurementEndpointQualification[1].qualified,true);
 assert.equal(f.activity.filter(x=>x.startsWith('rtc-')).length,callsBefore);
});
