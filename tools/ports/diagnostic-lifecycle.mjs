import {spawn} from 'node:child_process';

function boundedWait(promise,timeoutMs){
  return new Promise(resolve=>{
    let settled=false;
    const timer=setTimeout(()=>{
      if(settled)return;settled=true;resolve(null);
    },timeoutMs);
    promise.then(value=>{
      if(settled)return;settled=true;clearTimeout(timer);resolve(value);
    },error=>{
      if(settled)return;settled=true;clearTimeout(timer);resolve({kind:'observer-error',error:String(error?.stack??error)});
    });
  });
}

/** Observe close and spawn errors once, so later bounded waits share one event source. */
export function trackDiagnosticChild(child){
  let outcome=null,resolveOutcome;
  const completed=new Promise(resolve=>{resolveOutcome=resolve;});
  const errors=[];
  const settle=value=>{if(outcome)return;outcome=value;resolveOutcome(value);};
  child.once('close',(code,signal)=>settle({kind:'close',code,signal}));
  child.on('error',error=>{
    const message=String(error?.stack??error);
    errors.push(message);
    if(child.pid===undefined)settle({kind:'spawn-error',error:message});
  });
  return{wait:timeoutMs=>outcome?Promise.resolve(outcome):boundedWait(completed,timeoutMs),
    get outcome(){return outcome;},get errors(){return [...errors];}};
}

/** Run the Windows process-tree helper without leaving its error event unhandled or waiting forever. */
export async function terminateDiagnosticProcessTree(pid,{spawnProcess=spawn,timeoutMs=2000}={}){
  let helper;
  try{helper=spawnProcess('taskkill.exe',['/pid',String(pid),'/T','/F'],{windowsHide:true,stdio:'ignore'});}
  catch(error){return{started:false,error:String(error?.stack??error)};}
  const tracked=trackDiagnosticChild(helper);
  let outcome=await tracked.wait(timeoutMs);
  if(outcome?.kind==='spawn-error')return{started:false,error:outcome.error};
  if(outcome?.kind==='close')return{started:true,closed:true,code:outcome.code,signal:outcome.signal};
  let killError=null;
  try{helper.kill('SIGKILL');}catch(error){killError=String(error?.stack??error);}
  outcome=await tracked.wait(timeoutMs);
  if(outcome?.kind==='spawn-error')return{started:false,error:outcome.error};
  if(outcome?.kind==='close')return{started:true,closed:true,code:outcome.code,signal:outcome.signal,escalated:true};
  return{started:true,closed:false,escalated:true,killError,errors:tracked.errors};
}

function describeClose(outcome,{escalated=false,errors=[]}={}){
  const normal=(outcome.code===null&&outcome.signal==='SIGTERM')||
    (outcome.code===0&&outcome.signal===null);
  return{exited:true,normal,code:outcome.code,signal:outcome.signal,escalated,errors};
}

/** Stop an owned server and distinguish an observed exit from a process that outlived both bounds. */
export async function stopDiagnosticChild(child,{tracker=trackDiagnosticChild(child),platform=process.platform,
  graceMs=4000,finalMs=4000,terminateTree=terminateDiagnosticProcessTree}={}){
  if(tracker.outcome?.kind==='spawn-error')
    return{exited:true,normal:false,spawned:false,error:tracker.outcome.error,errors:tracker.errors};
  if(tracker.outcome?.kind==='close')return describeClose(tracker.outcome,{errors:tracker.errors});

  const alreadyExited=child.exitCode!==null&&child.exitCode!==undefined||
    child.signalCode!==null&&child.signalCode!==undefined;
  let signalError=null,killHelper=null,escalated=false;
  if(!alreadyExited){
    try{child.kill('SIGTERM');}catch(error){signalError=String(error?.stack??error);}
  }
  let outcome=await tracker.wait(graceMs);
  if(outcome?.kind==='close')return describeClose(outcome,{errors:tracker.errors});
  if(outcome?.kind==='spawn-error')return{exited:true,normal:false,spawned:false,error:outcome.error,errors:tracker.errors};

  escalated=true;
  if(!alreadyExited&&platform==='win32'){
    try{killHelper=await terminateTree(child.pid);}
    catch(error){killHelper={started:false,error:String(error?.stack??error)};}
  }else if(!alreadyExited){
    try{child.kill('SIGKILL');}catch(error){signalError=signalError??String(error?.stack??error);}
  }
  outcome=await tracker.wait(finalMs);
  if(outcome?.kind==='close')return{...describeClose(outcome,{escalated,errors:tracker.errors}),signalError,killHelper};
  if(outcome?.kind==='spawn-error')return{exited:true,normal:false,spawned:false,error:outcome.error,errors:tracker.errors};
  return{exited:false,normal:false,escalated,signalError,killHelper,errors:tracker.errors};
}
