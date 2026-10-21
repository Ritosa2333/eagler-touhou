import {assertRuntimeObservation} from './runtime-observations.mjs';

const identityFields=['hostPageId','hostTimeOrigin','frameDocumentId','frameUrl','epoch',
  'runtimeId','moduleId','coreId','firstFrameId'];

/** Compare the same host, iframe document, mounted Runtime and original first-frame receipt. */
export function compareRuntimeRestartIdentity(before,after,{expectedEpoch}={}){
  const missing=[];
  for(const [label,value] of [['before',before],['after',after]]){
    for(const field of identityFields){
      if(value?.[field]===undefined||value?.[field]===null||value?.[field]===''||
        (field==='hostTimeOrigin'&&!Number.isFinite(value[field]))||
        (field==='epoch'&&!Number.isSafeInteger(value[field])))
        missing.push(`${label}.${field}`);
    }
    if(!Number.isSafeInteger(value?.firstFrameCount)||value.firstFrameCount<1)
      missing.push(`${label}.firstFrameCount`);
  }
  if(missing.length)return{status:'incomplete',missing,reason:'Runtime identity needs the same host, iframe document, mounted exports and first-frame event.'};
  const differences=identityFields.filter(field=>before[field]!==after[field]);
  if(before.firstFrameCount!==1||after.firstFrameCount!==1)
    differences.push('firstFrameCount');
  if(expectedEpoch!==undefined&&(before.epoch!==expectedEpoch||after.epoch!==expectedEpoch))
    differences.push('expectedEpoch');
  return differences.length?{status:'changed',missing:[],differences,
    reason:'Restart changed the host, Runtime document, mounted instance, epoch or original first-frame identity.'}:
    {status:'same',missing:[],differences:[]};
}

/** Replace only the declared first KeyR with the production Launcher action. */
export function createLauncherRestartPress({press,requestRestart}={}){
  if(typeof press!=='function'||typeof requestRestart!=='function')
    throw new TypeError('Launcher restart input needs press() and requestRestart() callbacks');
  let requested=false;const forwardedCodes=[];
  const send=async code=>{
    if(!requested){
      if(code!=='KeyR'){
        const error=new Error('The Launcher restart action must replace the title flow first KeyR input.');
        error.status='incomplete';error.evidence={capability:'restart-button',input:code,gameplayInputSent:false};
        throw error;
      }
      requested=true;
      return requestRestart();
    }
    forwardedCodes.push(code);
    return press(code);
  };
  return Object.freeze({press:send,wasRequested:()=>requested,forwardedCodes:()=>[...forwardedCodes]});
}

/**
 * Sample native Runtime state while a restart action/title flow is in flight.
 * The initial read arms the sampler before `operate` can send input. Rows are
 * scalar-only and bounded by the supplied observation deadline/cadence.
 */
export async function captureRuntimeRestartBoundary({observe,operate,frameKey,pausedFrame,
  isGameplayReady,shouldContinueSampling=()=>true,timeoutMs=30000,pollIntervalMs=75,
  wait=ms=>new Promise(resolve=>setTimeout(resolve,ms)),now=()=>Date.now()}={}){
  if(typeof observe!=='function'||typeof operate!=='function'||typeof isGameplayReady!=='function')
    throw new TypeError('Restart boundary capture needs observe(), operate() and isGameplayReady() callbacks');
  if(typeof frameKey!=='string'||!frameKey||!Number.isFinite(pausedFrame))
    throw new TypeError('Restart boundary capture needs a native frame key and finite paused frame');
  if(!Number.isFinite(timeoutMs)||timeoutMs<=0||!Number.isFinite(pollIntervalMs)||pollIntervalMs<=0)
    throw new RangeError('Restart boundary capture needs a finite positive timeout and polling interval');
  const sampleLimit=Math.ceil(timeoutMs/pollIntervalMs)+2;
  if(sampleLimit>512)throw new RangeError('Restart boundary sample budget exceeds the fixed 512-row limit');
  const startedAt=now(),samples=[],maxRetainedSamples=32;
  let initialSample=null,sampleCount=0,resetBoundary=null,sampleError=null,operationError=null,operationResult,stop=false;
  const compact=(state,operationStarted)=>{
    const frame=state?.frames?.[frameKey];
    const row={index:sampleCount,elapsedMs:Math.max(0,now()-startedAt),
      frame:Number.isFinite(frame)?frame:null,
      ready:isGameplayReady(state)===true,
      operationStarted,
      pauseActive:typeof state?.pause?.active==='boolean'?state.pause.active:null};
    if(sampleCount===0)initialSample=row;
    sampleCount++;
    if(samples.length===maxRetainedSamples)samples.shift();
    samples.push(row);
    return row;
  };
  const capture=async(operationStarted)=>{
    const state=await observe();
    const row=compact(state,operationStarted);
    if(operationStarted&&row.ready&&row.pauseActive===false&&Number.isFinite(row.frame)&&row.frame<pausedFrame){
      resetBoundary={...row,frameKey};
      return{reset:true,row};
    }
    return{reset:false,row};
  };
  // This read is deliberately complete before the operation callback starts.
  let initial;
  try{initial=await capture(false);}
  catch(error){return{status:'incomplete',reason:'Native Runtime observation failed before the restart operation began.',
    frameKey,pausedFrame,sampleCount,sampleLimit,initialSample,samples,samplesOmitted:sampleCount-samples.length,
    operationStarted:false,error:String(error?.stack||error)};}
  if(initial.row.pauseActive!==true||initial.row.frame!==pausedFrame)
    return{status:'incomplete',reason:'The Runtime left the native paused frame boundary before restart input began.',
      frameKey,pausedFrame,initialSample,sampleCount,sampleLimit,samples,samplesOmitted:sampleCount-samples.length,
      operationStarted:false};
  const sampleUntilBoundary=async()=>{
    const sampleStartedAt=now();
    try{
      while(!stop&&!resetBoundary&&sampleCount<sampleLimit&&now()-sampleStartedAt<timeoutMs){
        await wait(pollIntervalMs);
        if(stop)break;
        if((await capture(operationStarted)).reset)break;
      }
    }catch(error){sampleError=error;stop=true;}
  };
  // Start the next native read before scheduling the input/title-flow callback.
  let operationStarted=false;
  const samplingPromise=sampleUntilBoundary();
  const operationPromise=Promise.resolve().then(()=>{operationStarted=true;return operate();}).then(value=>{
    operationResult=value;
    try{if(shouldContinueSampling(value)!==true)stop=true;}
    catch(error){operationError=error;stop=true;}
  },error=>{
    operationError=error;stop=true;
  });
  await Promise.all([samplingPromise,operationPromise]);
  if(operationError&&sampleError)throw new AggregateError([operationError,sampleError],
    'Restart operation and native observation both failed');
  if(operationError)throw operationError;
  if(sampleError)return{status:'incomplete',reason:'Native Runtime sampling failed during the restart operation.',
    error:String(sampleError?.stack||sampleError),frameKey,pausedFrame,sampleCount,sampleLimit,initialSample,
    samples,samplesOmitted:sampleCount-samples.length,durationMs:Math.max(0,now()-startedAt),operationStarted,operationResult};
  const status=resetBoundary?'observed':'incomplete';
  return{status,...(!resetBoundary?{reason:'No gameplay-ready native frame below the paused boundary was captured.'}:{}),
    frameKey,pausedFrame,resetBoundary,sampleCount,sampleLimit,initialSample,samples,
    samplesOmitted:sampleCount-samples.length,durationMs:Math.max(0,now()-startedAt),operationStarted,operationResult};
}

/** Reuse the Runtime's declared native observers to assess one keyboard or Launcher-button restart. */
export function assessRuntimeTouchRestart({before,paused,pauseStable,restarted,advanced,resetBoundary,
  beforeIdentity,afterIdentity,expectedEpoch,gameplayReady}={}){
  const key=before?.capabilities?.restartFrame;
  if(before?.capabilities?.pause!==true||!key||!Number.isFinite(before?.frames?.[key]))
    return{checkStatus:'incomplete',reason:'The Runtime must expose native pause state and a finite restart frame before restart.'};
  for(const [label,state] of [['paused',paused],['pauseStable',pauseStable],['restarted',restarted],['advanced',advanced]]){
    if(state?.capabilities?.pause!==true||!Number.isFinite(state?.frames?.[key]))
      return{checkStatus:'incomplete',reason:`The ${label} observation lacks native pause or restart-frame evidence.`};
  }
  if(gameplayReady?.restarted!==true||gameplayReady?.advanced!==true)
    return{checkStatus:'incomplete',reason:'Native gameplay readiness was not re-established after restart.'};
  if(resetBoundary?.frameKey!==key||resetBoundary?.operationStarted!==true||
    !Number.isSafeInteger(resetBoundary?.index)||resetBoundary.index<1||
    resetBoundary?.ready!==true||resetBoundary?.pauseActive!==false||
    !Number.isFinite(resetBoundary?.frame)||resetBoundary.frame>=paused.frames[key])
    return{checkStatus:'incomplete',reason:'A concurrent native sample did not capture gameplay readiness below the paused frame boundary.',
      resetBoundary:resetBoundary??null,pausedFrame:paused.frames[key]};
  const identity=compareRuntimeRestartIdentity(beforeIdentity,afterIdentity,{expectedEpoch});
  if(identity.status==='incomplete')return{checkStatus:'incomplete',reason:identity.reason,identity};
  if(identity.status!=='same')throw Error(identity.reason);

  assertRuntimeObservation(paused,'pause-active');
  assertRuntimeObservation(pauseStable,'pause-active');
  assertRuntimeObservation(pauseStable,'frame-frozen',{key,before:paused.frames[key]});
  assertRuntimeObservation(restarted,'pause-inactive');
  assertRuntimeObservation({game:before.game,capabilities:{restartFrame:key},frames:{[key]:resetBoundary.frame}},
    'restart-frame-reset',{before:paused.frames[key]});
  assertRuntimeObservation(advanced,'pause-inactive');
  assertRuntimeObservation(advanced,'frame-advanced',{key,before:restarted.frames[key]});
  return{checkStatus:'passed',restartFrame:key,before:before.frames[key],paused:paused.frames[key],
    resetBoundary,
    restarted:restarted.frames[key],advanced:advanced.frames[key],identity};
}
