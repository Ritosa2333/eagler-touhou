import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {validateRafDispatchDelayOptions,installRafDispatchDelay} from './raf-dispatch-delay.mjs';
import {startPerformance} from './performance-driver.mjs';

let assertions=0;const groups=[];
const equal=(a,b,m)=>{++assertions;assert.equal(a,b,m);};
const deep=(a,b,m)=>{++assertions;assert.deepEqual(a,b,m);};
const throws=(f,expected)=>{++assertions;assert.throws(f,expected);};
const test=(name,run)=>groups.push({name,run});

// A manually serviced realm. No real RAF or timeout is called by this suite.
// Timestamp and arrival are independent inputs, so retaining the old real RAF
// stamp cannot accidentally pass because the fake clock supplied the same value.
function realm(){
 let clockMs=100,frame=299,rafHandle=40,timerHandle=800;
 const rafs=new Map(),timers=new Map();
 const calls={request:[],cancel:[],set:[],clear:[],clock:0,frame:0};
 const failures={request:null,cancel:null,set:null};
 const cancellationResult={kind:'original cancel result'};
 const clock={timeOrigin:1700000000000,now(){equal(this,clock);++calls.clock;return clockMs;}};
 const scope={
  performance:clock,
  requestAnimationFrame(callback,...rest){
   calls.request.push({receiver:this,callback,rest});
   if(failures.request)throw failures.request;
   if(typeof callback!=='function')throw TypeError('original callback validation');
   const handle=++rafHandle;rafs.set(handle,callback);return handle;
  },
  cancelAnimationFrame(handle,...rest){
   calls.cancel.push({receiver:this,handle,rest});
   if(failures.cancel)throw failures.cancel;
   rafs.delete(handle);return cancellationResult;
  },
  setTimeout(callback,delay,...rest){
   calls.set.push({receiver:this,callback,delay,rest});
   if(failures.set)throw failures.set;
   const handle=++timerHandle;timers.set(handle,{callback,due:clockMs+delay});return handle;
  },
  clearTimeout(handle){calls.clear.push({receiver:this,handle});timers.delete(handle);}
 };
 const originals={request:scope.requestAnimationFrame,cancel:scope.cancelAnimationFrame};
 return {scope,clock,calls,failures,rafs,timers,originals,cancellationResult,
  setNow:value=>{clockMs=value;},setFrame:value=>{frame=value;},
  observedFrame:()=>{++calls.frame;return frame;},
  fireRaf(handle,stamp,receiver=scope,...rest){
   const callback=rafs.get(handle);if(!callback)throw Error('Missing fake RAF '+handle);
   rafs.delete(handle);return Reflect.apply(callback,receiver,[stamp,...rest]);
  },
  fireTimer(handle){
   const timer=timers.get(handle);if(!timer)throw Error('Missing fake timer '+handle);
   timers.delete(handle);clockMs=Math.max(clockMs,timer.due);return timer.callback();
  }
 };
}
function install(r,extra={}){
 return installRafDispatchDelay({delayMs:600,targetFrame:300,
  observedFrame:r.observedFrame,scope:r.scope,...extra});
}

test('disabled is a complete poison-realm bypass and finish is idempotent',()=>{
 const poison=new Proxy({}, {get(){throw Error('disabled realm read');},
  set(){throw Error('disabled realm write');},getOwnPropertyDescriptor(){throw Error('disabled descriptor read');}});
 for(const args of [{scope:poison},{delayMs:0,scope:poison,observedFrame:()=>{throw Error('frame read');}}]){
  const helper=installRafDispatchDelay(args),report=helper.finish();
  equal(report.schema,'th10-raf-dispatch-delay/1');equal(report.enabled,false);equal(report.valid,true);
  equal(report.installed,false);equal(report.restored,false);equal(report.event,null);
  deep([report.heldCount,report.releasedCount,report.cancelledHeldCount,report.pendingAfterRestore],[0,0,0,0]);
  equal(report.timeOrigin,null);equal(report.installedAtMs,null);equal(report.finishedAtMs,null);
  equal(helper.finish(),report);deep(report.errors,[]);
 }
});

test('strict option validation and driver rejection precede poison API access',()=>{
 deep(validateRafDispatchDelayOptions(),{delayMs:0,targetFrame:300});
 for(const value of [null,false,true,'0','600',-1,.5,1.5,1000.5,1001,NaN,Infinity,-Infinity]){
  throws(()=>validateRafDispatchDelayOptions({rafDispatchDelayMs:value}),TypeError);
  const poison=new Proxy({}, {get(){throw Error('API must not be read');}});
  throws(()=>startPerformance(poison,{rafDispatchDelayMs:value}),TypeError);
 }
 for(const value of [null,false,'300',-.1,-1,300.5,0xffffffff,NaN,Infinity])
  throws(()=>validateRafDispatchDelayOptions({rafDispatchDelayMs:1,rafDispatchDelayFrame:value}),TypeError);
 for(const delayMs of [0,1,600,1000])for(const targetFrame of [0,300,0xfffffffe])
  deep(validateRafDispatchDelayOptions({rafDispatchDelayMs:delayMs,rafDispatchDelayFrame:targetFrame}),{delayMs,targetFrame});
 const poison=new Proxy({}, {get(){throw Error('invalid closure must precede realm');}});
 throws(()=>installRafDispatchDelay({delayMs:1,observedFrame:42,scope:poison}),TypeError);
});

test('pre-target callback preserves native handle this args return and no timer',()=>{
 const r=realm(),helper=install(r),receiver={name:'real RAF receiver'},observations=[];
 function callback(...args){observations.push({receiver:this,args});return 123;}
 const handle=Reflect.apply(r.scope.requestAnimationFrame,r.scope,[callback,'extra request argument']);
 equal(handle,41);equal(r.calls.request[0].receiver,r.scope);deep(r.calls.request[0].rest,['extra request argument']);
 equal(r.fireRaf(handle,77.25,receiver,'extra callback argument'),123);
 deep(observations,[{receiver,args:[77.25,'extra callback argument']}]);
 equal(r.calls.set.length,0);equal(r.calls.frame,1);
 const report=helper.finish();equal(report.heldCount,0);equal(report.pendingBeforeRestore,0);
 equal(report.valid,true);equal(report.restored,true);
});

test('holds once and release retains original timestamp after unrelated timer service',()=>{
 const r=realm(),helper=install(r),receiver={name:'native RAF callback'},observations=[];
 r.setFrame(300);r.setNow(200);
 const handle=r.scope.requestAnimationFrame(function(...args){observations.push({receiver:this,args});return 987;});
 equal(handle,41);equal(r.fireRaf(handle,175.5,receiver,'retained'),undefined);
 equal(observations.length,0);equal(r.timers.size,1);equal(r.calls.set[0].delay,600);
 equal(r.calls.set[0].receiver,r.scope);const heldTimer=[...r.timers.keys()][0];
 let touch=0;const independent=r.scope.setTimeout(()=>{++touch;},8);
 r.fireTimer(independent);equal(touch,1);equal(observations.length,0);
 r.setNow(815);r.setFrame(307);
 equal(r.fireTimer(heldTimer),987);
 deep(observations,[{receiver,args:[175.5,'retained']}]);
 const next=r.scope.requestAnimationFrame(stamp=>observations.push({stamp}));
 equal(next,42);r.fireRaf(next,816);equal(r.timers.size,0);equal(observations[1].stamp,816);
 const report=helper.finish();
 deep([report.heldCount,report.releasedCount,report.cancelledHeldCount],[1,1,0]);
 deep(report.event,{handle:41,rafTimestamp:175.5,arrivedAtMs:200,heldAtMs:200,dueAtMs:800,
  releasedAtMs:815,frameAtHold:300,frameAtRelease:307,deliveredTimestamp:175.5,
  cancelledAtMs:null,cancelReason:null});
 equal(report.pendingAfterRestore,0);equal(report.restored,true);equal(report.valid,true);
 equal(report.timeOrigin,r.clock.timeOrigin);
});

test('other real RAFs continue while the single callback is held',()=>{
 const r=realm(),helper=install(r),seen=[];r.setFrame(300);
 const first=r.scope.requestAnimationFrame(stamp=>seen.push(['held',stamp]));
 const second=r.scope.requestAnimationFrame(stamp=>seen.push(['other',stamp]));
 r.fireRaf(first,90);r.fireRaf(second,91);deep(seen,[['other',91]]);
 equal(r.timers.size,1);r.fireTimer([...r.timers.keys()][0]);
 deep(seen,[['other',91],['held',90]]);equal(helper.finish().heldCount,1);
});

test('cancel held maps the native handle cancels timer and never holds a second time',()=>{
 const r=realm(),helper=install(r);r.setFrame(300);let invoked=0;
 const handle=r.scope.requestAnimationFrame(()=>{++invoked;});
 r.fireRaf(handle,91);const [timerId,timer]=[...r.timers.entries()][0];
 r.setNow(130);const receiver={name:'cancel receiver'};
 equal(Reflect.apply(r.scope.cancelAnimationFrame,receiver,[handle,'cancel extra']),r.cancellationResult);
 deep(r.calls.cancel.at(-1),{receiver,handle,rest:['cancel extra']});
 equal(r.calls.clear.at(-1).handle,timerId);equal(r.timers.size,0);
 timer.callback();equal(invoked,0);
 const next=r.scope.requestAnimationFrame(()=>{++invoked;});r.fireRaf(next,140);equal(invoked,1);
 const report=helper.finish();
 deep([report.heldCount,report.releasedCount,report.cancelledHeldCount],[1,0,1]);
 equal(report.event.cancelReason,'cancelAnimationFrame');equal(report.event.cancelledAtMs,130);
 equal(report.pendingBeforeRestore,0);equal(report.valid,true);
});

test('cancel pending and unknown preserve cancellation semantics and reject stale dispatch',()=>{
 const r=realm(),helper=install(r);let invoked=0;
 const handle=r.scope.requestAnimationFrame(()=>{++invoked;}),stale=r.rafs.get(handle);
 equal(r.scope.cancelAnimationFrame(handle),r.cancellationResult);
 Reflect.apply(stale,r.scope,[93]);equal(invoked,0);equal(r.calls.frame,0);
 equal(r.scope.cancelAnimationFrame(999),r.cancellationResult);equal(r.calls.cancel.at(-1).handle,999);
 const report=helper.finish();equal(report.heldCount,0);equal(report.pendingBeforeRestore,0);
 equal(report.valid,true);
});

test('finish cancels held and pending callbacks restores descriptors and ignores late callbacks',()=>{
 const r=realm(),beforeRequest=Object.getOwnPropertyDescriptor(r.scope,'requestAnimationFrame'),
  beforeCancel=Object.getOwnPropertyDescriptor(r.scope,'cancelAnimationFrame'),helper=install(r);
 r.setFrame(300);let invoked=0;
 const held=r.scope.requestAnimationFrame(()=>{++invoked;});r.fireRaf(held,95);
 const timer=[...r.timers.values()][0],pending=r.scope.requestAnimationFrame(()=>{++invoked;}),
  stale=r.rafs.get(pending),cachedRequest=r.scope.requestAnimationFrame;
 const report=helper.finish(),frozen=JSON.stringify(report),clocks=r.calls.clock;
 equal(report.pendingBeforeRestore,2);equal(report.pendingAfterRestore,0);
 equal(report.cancelledHeldCount,1);equal(report.event.cancelReason,'finish');
 equal(r.rafs.size,0);equal(r.timers.size,0);equal(report.restored,true);
 deep(Object.getOwnPropertyDescriptor(r.scope,'requestAnimationFrame'),beforeRequest);
 deep(Object.getOwnPropertyDescriptor(r.scope,'cancelAnimationFrame'),beforeCancel);
 timer.callback();Reflect.apply(stale,r.scope,[1000]);
 equal(invoked,0);equal(helper.finish(),report);equal(JSON.stringify(report),frozen);equal(r.calls.clock,clocks);
 const after=Reflect.apply(cachedRequest,r.scope,[()=>{++invoked;}]);
 r.fireRaf(after,1001);equal(invoked,1);equal(JSON.stringify(report),frozen);
});

test('finish inside released callback does not acquire a new clock or mutate afterward',()=>{
 const r=realm();let helper,report,serialized,clocks;
 helper=install(r);r.setFrame(300);
 const handle=r.scope.requestAnimationFrame(()=>{
  report=helper.finish();serialized=JSON.stringify(report);clocks=r.calls.clock;return 41;
 });
 r.fireRaf(handle,90);equal(r.fireTimer([...r.timers.keys()][0]),41);
 equal(report.releasedCount,1);equal(report.pendingBeforeRestore,0);
 equal(JSON.stringify(helper.finish()),serialized);equal(r.calls.clock,clocks);equal(report.restored,true);
});

test('foreign global replacement is not overwritten and makes evidence invalid',()=>{
 const r=realm(),helper=install(r),foreign=function(){return 'foreign';};
 r.scope.requestAnimationFrame=foreign;const report=helper.finish();
 equal(r.scope.requestAnimationFrame,foreign);equal(r.scope.cancelAnimationFrame,r.originals.cancel);
 equal(report.restored,false);equal(report.valid,false);
 deep(report.errors,['Global ownership lost: requestAnimationFrame']);
});

test('inherited global methods are restored by removing only the observers own properties',()=>{
 const r=realm(),prototype={requestAnimationFrame:r.originals.request,cancelAnimationFrame:r.originals.cancel};
 delete r.scope.requestAnimationFrame;delete r.scope.cancelAnimationFrame;
 Object.setPrototypeOf(r.scope,prototype);const helper=install(r);
 equal(Object.hasOwn(r.scope,'requestAnimationFrame'),true);
 const report=helper.finish();equal(report.restored,true);
 equal(Object.hasOwn(r.scope,'requestAnimationFrame'),false);
 equal(Object.hasOwn(r.scope,'cancelAnimationFrame'),false);
 equal(r.scope.requestAnimationFrame,r.originals.request);
});

test('partial installation rolls back the owned request method',()=>{
 const r=realm();
 Object.defineProperty(r.scope,'cancelAnimationFrame',{value:r.originals.cancel,writable:false,configurable:false});
 throws(()=>install(r),/Cannot install cancelAnimationFrame/);
 equal(r.scope.requestAnimationFrame,r.originals.request);equal(r.scope.cancelAnimationFrame,r.originals.cancel);
 equal(r.rafs.size,0);equal(r.timers.size,0);
});

test('original request cancel and callback exceptions preserve exact identity',()=>{
 const r=realm(),helper=install(r),requestError=new Error('request failure');
 r.failures.request=requestError;throws(()=>r.scope.requestAnimationFrame(()=>{}),e=>e===requestError);
 r.failures.request=null;const handle=r.scope.requestAnimationFrame(()=>{});
 const cancelError=new Error('cancel failure');r.failures.cancel=cancelError;
 throws(()=>r.scope.cancelAnimationFrame(handle),e=>e===cancelError);
 equal(r.rafs.has(handle),true);r.failures.cancel=null;r.scope.cancelAnimationFrame(handle);
 throws(()=>r.scope.requestAnimationFrame(null),/original callback validation/);
 const callbackError=new Error('original callback failure');
 const immediate=r.scope.requestAnimationFrame(()=>{throw callbackError;});
 throws(()=>r.fireRaf(immediate,91),e=>e===callbackError);
 r.setFrame(300);const held=r.scope.requestAnimationFrame(()=>{throw callbackError;});
 r.fireRaf(held,92);throws(()=>r.fireTimer([...r.timers.keys()][0]),e=>e===callbackError);
 const report=helper.finish();equal(report.pendingBeforeRestore,0);equal(report.releasedCount,1);equal(report.valid,true);
});

test('invalid observed frames and timer scheduling failures cannot falsely pass injection',()=>{
 const r=realm(),helper=install(r);let invoked=0;
 r.setFrame(0xffffffff);const invalid=r.scope.requestAnimationFrame(()=>{++invoked;});
 r.fireRaf(invalid,90);equal(invoked,1);equal(r.timers.size,0);
 r.setFrame(300);r.failures.set=new Error('timer unavailable');
 const failed=r.scope.requestAnimationFrame(()=>{++invoked;});r.fireRaf(failed,91);
 equal(invoked,2);const report=helper.finish();
 equal(report.valid,false);equal(report.heldCount,1);equal(report.releasedCount,0);
 equal(report.event.cancelReason,'timer-scheduling-failed');equal(report.pendingAfterRestore,0);
 equal(report.errors.length,2);equal(report.restored,true);
});

test('startup clock validation makes no global replacements or timers',()=>{
 for(const change of [r=>{r.clock.timeOrigin=NaN;},r=>{r.setNow(Infinity);}]){
  const r=realm();change(r);throws(()=>install(r),/finite performance/);
  equal(r.scope.requestAnimationFrame,r.originals.request);equal(r.scope.cancelAnimationFrame,r.originals.cancel);
  equal(r.calls.set.length,0);equal(r.calls.request.length,0);
 }
});

const results=[];
for(const {name,run} of groups){
 const before=assertions;
 try{run();results.push({name,passed:true,assertions:assertions-before});}
 catch(error){results.push({name,passed:false,assertions:assertions-before,
  error:String(error?.stack??error).slice(0,3000)});}
}
const sourceHashes=Object.fromEntries(['raf-dispatch-delay.mjs','performance-driver.mjs','check-raf-dispatch-delay.mjs'].map(
 name=>[name,createHash('sha256').update(readFileSync(new URL(name,import.meta.url))).digest('hex')]));
const passed=results.every(result=>result.passed);
console.log(JSON.stringify({schema:'th10-raf-dispatch-delay-unit/1',passed,groups:results.length,
 assertions,scope:'manually dispatched fake realm only; no live browser or native game acceptance',sourceHashes,results},null,2));
if(!passed)process.exitCode=1;
