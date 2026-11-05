// Test-only, one-shot delay of an ARRIVED real RAF callback. This never calls a
// game tick, changes its timestamp, blocks the thread, or starts a polling loop.
export function validateRafDispatchDelayOptions(options={}){
 const delayMs=options.rafDispatchDelayMs===undefined?0:options.rafDispatchDelayMs;
 const targetFrame=options.rafDispatchDelayFrame===undefined?300:options.rafDispatchDelayFrame;
 if(typeof delayMs!=='number'||!Number.isInteger(delayMs)||
    (delayMs!==0&&(delayMs<1||delayMs>1000)))
  throw TypeError('rafDispatchDelayMs must be 0 or an integer in [1,1000]');
 if(!Number.isInteger(targetFrame)||targetFrame<0||targetFrame>=0xffffffff)
  throw TypeError('rafDispatchDelayFrame must be an absolute valid u32 game frame');
 return {delayMs,targetFrame};
}

export function installRafDispatchDelay({delayMs=0,targetFrame=300,observedFrame,scope=globalThis}={}){
 ({delayMs,targetFrame}=validateRafDispatchDelayOptions({
  rafDispatchDelayMs:delayMs,rafDispatchDelayFrame:targetFrame}));
 const report={schema:'th10-raf-dispatch-delay/1',enabled:delayMs!==0,delayMs,targetFrame,
  valid:true,errors:[],installed:false,restored:false,heldCount:0,releasedCount:0,
  cancelledHeldCount:0,pendingBeforeRestore:0,pendingAfterRestore:0,
  timeOrigin:null,clockScope:'the instrumented Window performance.now(), relative to timeOrigin',
  installedAtMs:null,finishedAtMs:null,event:null};
 // Deliberately before even reading RAF, timers, performance or observedFrame.
 if(delayMs===0)return {finish:()=>report};
 if(typeof observedFrame!=='function')throw TypeError('observedFrame must be a function');
 const request=scope.requestAnimationFrame,cancel=scope.cancelAnimationFrame;
 const setTimer=scope.setTimeout,clearTimer=scope.clearTimeout,clock=scope.performance;
 if([request,cancel,setTimer,clearTimer,clock?.now].some(x=>typeof x!=='function'))
  throw TypeError('RAF delay requires this realm RAF, timeout and performance APIs');
 const now=()=>Reflect.apply(clock.now,clock,[]);
 report.timeOrigin=clock.timeOrigin;report.installedAtMs=now();
 if(!Number.isFinite(report.timeOrigin)||!Number.isFinite(report.installedAtMs))
  throw TypeError('RAF delay requires a finite performance time origin and clock');
 const descriptors=new Map(['requestAnimationFrame','cancelAnimationFrame'].map(
  name=>[name,Object.getOwnPropertyDescriptor(scope,name)]));
 const pending=new Map();let active=true,used=false,finished=false;
 function fault(message){
  report.valid=false;if(report.errors.length<16)report.errors.push(String(message).slice(0,256));
 }
 function readTime(){
  try{const t=now();if(Number.isFinite(t))return t;}catch{}
  fault('Non-finite or unavailable performance clock');return null;
 }
 function readFrame(){
  try{const f=observedFrame();if(Number.isInteger(f)&&f>=0&&f<0xffffffff)return f;}catch{}
  fault('Unavailable or invalid observed completed frame');return null;
 }
 function deliver(entry,receiver,args){
  pending.delete(entry.handle);entry.state='delivered';
  return Reflect.apply(entry.callback,receiver,args);
 }
 function delayedRequest(callback,...rest){
  if(!active||typeof callback!=='function')return Reflect.apply(request,this,[callback,...rest]);
  const entry={handle:null,callback,state:'pending',timer:null};
  const proxy=function(...args){
   if(!active||entry.state!=='pending'||pending.get(entry.handle)!==entry)return;
   if(!used){
    const frame=readFrame();
    if(frame!==null&&frame>=targetFrame){
     used=true;
     const arrived=readTime();
     if(arrived!==null){
      const timestamp=args[0];
      if(typeof timestamp!=='number'||!Number.isFinite(timestamp))
       fault('Real RAF supplied a non-finite timestamp');
      const event=report.event={handle:entry.handle,rafTimestamp:timestamp,
       arrivedAtMs:arrived,heldAtMs:arrived,dueAtMs:arrived+delayMs,
       releasedAtMs:null,frameAtHold:frame,frameAtRelease:null,
       deliveredTimestamp:null,cancelledAtMs:null,cancelReason:null};
      const receiver=this;entry.state='held';++report.heldCount;
      try{
       entry.timer=Reflect.apply(setTimer,scope,[()=>{
        if(!active||entry.state!=='held'||pending.get(entry.handle)!==entry)return;
        entry.timer=null;event.releasedAtMs=readTime();event.frameAtRelease=readFrame();
        event.deliveredTimestamp=args[0];++report.releasedCount;
        // All diagnostic state is final BEFORE calling the original callback:
        // it may synchronously stop the native loop and finish this observer.
        return deliver(entry,receiver,args);
       },delayMs]);
       return;
      }catch(error){
       fault('Unable to schedule held RAF: '+String(error));
       event.cancelReason='timer-scheduling-failed';
      }
     }
    }
   }
   return deliver(entry,this,args);
  };
  entry.handle=Reflect.apply(request,this,[proxy,...rest]);
  if(pending.has(entry.handle))fault('Browser reused an outstanding RAF handle');
  pending.set(entry.handle,entry);
  return entry.handle;
 }
 function discard(entry,reason){
  if(entry.state==='held'){
   if(entry.timer!==null)Reflect.apply(clearTimer,scope,[entry.timer]);
   entry.timer=null;++report.cancelledHeldCount;
   report.event.cancelledAtMs=readTime();report.event.cancelReason=reason;
  }
  entry.state='cancelled';pending.delete(entry.handle);
 }
 function delayedCancel(handle,...rest){
  // Preserve original cancellation validation, this, return and exception.
  const result=Reflect.apply(cancel,this,[handle,...rest]);
  const entry=pending.get(handle);
  if(active&&entry)discard(entry,'cancelAnimationFrame');
  return result;
 }
 function restore(name,wrapper){
  if(scope[name]!==wrapper){fault('Global ownership lost: '+name);return false;}
  const descriptor=descriptors.get(name);
  if(descriptor)Object.defineProperty(scope,name,descriptor);
  else if(!Reflect.deleteProperty(scope,name))throw Error('Cannot delete installed '+name);
  return scope[name]===(name==='requestAnimationFrame'?request:cancel);
 }
 try{
  if(!Reflect.set(scope,'requestAnimationFrame',delayedRequest)||scope.requestAnimationFrame!==delayedRequest)
   throw Error('Cannot install requestAnimationFrame delay');
  if(!Reflect.set(scope,'cancelAnimationFrame',delayedCancel)||scope.cancelAnimationFrame!==delayedCancel)
   throw Error('Cannot install cancelAnimationFrame delay');
  report.installed=true;
 }catch(error){
  active=false;
  for(const [name,wrapper] of [['requestAnimationFrame',delayedRequest],['cancelAnimationFrame',delayedCancel]]){
   try{if(scope[name]===wrapper)restore(name,wrapper);}catch{}
  }
  throw error;
 }
 function finish(){
  if(finished)return report;
  finished=true;active=false;report.finishedAtMs=readTime();
  report.pendingBeforeRestore=pending.size;
  for(const entry of [...pending.values()]){
   try{
    if(entry.state==='pending')Reflect.apply(cancel,scope,[entry.handle]);
    discard(entry,'finish');
   }catch(error){
    fault('Pending RAF cleanup failed: '+String(error));
    // Even if the browser cancellation API fails, stale proxies cannot call
    // the closed module because active is already false.
    entry.state='cancelled';pending.delete(entry.handle);
   }
  }
  report.pendingAfterRestore=pending.size;
  let restored=true;
  for(const [name,wrapper] of [['requestAnimationFrame',delayedRequest],['cancelAnimationFrame',delayedCancel]]){
   try{if(!restore(name,wrapper))restored=false;}
   catch(error){restored=false;fault('Global restoration failed: '+name+': '+String(error));}
  }
  report.restored=restored;
  if(!restored)report.valid=false;
  return report;
 }
 return {finish};
}
