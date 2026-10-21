export async function collectDesktopSfxWindow({capture,operation,finish}){
 const failures=[];
 let capturePromise;
 try{capturePromise=Promise.resolve(capture());}catch(error){capturePromise=Promise.reject(error);}
 const captureOutcome=capturePromise.then(value=>({ok:true,value}),error=>({ok:false,error}));
 let operationOutcome;
 try{operationOutcome={ok:true,value:await operation()};}catch(error){operationOutcome={ok:false,error};}
 const captured=await captureOutcome;
 let finished;
 try{finished={ok:true,value:await finish()};}catch(error){finished={ok:false,error};}
 if(!operationOutcome.ok)failures.push(operationOutcome.error);
 if(!captured.ok)failures.push(captured.error);
 if(!finished.ok)failures.push(finished.error);
 if(failures.length===1)throw failures[0];
 if(failures.length>1)throw new AggregateError(failures,'SFX input window collection and cleanup failed');
 return{firing:captured.value,inputCapture:finished.value};
}

export function validateDesktopSfxHistoryReadiness(readiness,{sessionId,contextIndex,sampleRate,historyTapIds}={}){
 const bad=reason=>({valid:false,reason});
 if(!readiness||readiness.available!==true||readiness.status!=='ready'||readiness.reason!==null||
   readiness.source!=='shared-audioworklet-history-status'||readiness.sessionId!==sessionId||
   readiness.contextIndex!==contextIndex||readiness.sampleRate!==sampleRate||
   !Number.isFinite(readiness.elapsedMs)||readiness.elapsedMs<0||!Number.isSafeInteger(readiness.pollCount)||readiness.pollCount<1||
   !Array.isArray(readiness.checks)||readiness.checks.length!==readiness.pollCount||!Array.isArray(readiness.taps)||readiness.taps.length<1)
  return bad('Rendered SFX history readiness is missing, malformed, or belongs to another input session.');
 const arm=readiness.arm,ready=readiness.ready;
 if(arm?.state!=='running'||ready?.state!=='running'||!Number.isFinite(arm.currentTime)||!Number.isSafeInteger(arm.contextFrame)||
   arm.contextFrame!==Math.floor(arm.currentTime*sampleRate)||!Number.isFinite(ready.currentTime)||!Number.isSafeInteger(ready.contextFrame)||
   ready.contextFrame!==Math.floor(ready.currentTime*sampleRate)||ready.contextFrame<=arm.contextFrame||
   !Number.isSafeInteger(readiness.barrierFrame)||readiness.barrierFrame<=arm.contextFrame||ready.contextFrame<readiness.barrierFrame)
  return bad('SDL3 audio clock did not advance from the armed frame through the readiness observation.');
 const barrierBasis=readiness.barrierBasis,basisIds=new Set(Array.isArray(barrierBasis)?barrierBasis.map(row=>row?.tapId):[]);
 if(!Array.isArray(barrierBasis)||barrierBasis.length!==readiness.taps.length||basisIds.size!==barrierBasis.length||barrierBasis.some(row=>
   !Number.isSafeInteger(row?.tapId)||!Number.isSafeInteger(row?.firstFrame)||!Number.isSafeInteger(row?.lastProcessStartFrame)||
   !Number.isSafeInteger(row?.lastProcessEndFrame)||!Number.isFinite(row?.contextTime)||!Number.isSafeInteger(row?.contextFrame)||
   row.contextFrame!==Math.floor(row.contextTime*sampleRate)||row.contextFrame>ready.contextFrame||row.lastProcessEndFrame<=arm.contextFrame||
   row.firstFrame>row.lastProcessStartFrame||row.lastProcessStartFrame>=row.lastProcessEndFrame)||
   Math.max(...barrierBasis.map(row=>row.lastProcessEndFrame))!==readiness.barrierFrame)
  return bad('The fixed readiness barrier is not backed by each tap’s first observed post-arm render quantum.');
 const finalCheck=readiness.checks.at(-1);
 if(readiness.checks.some(check=>check?.barrierFrame!==null&&check?.barrierFrame!==readiness.barrierFrame)||
   finalCheck?.barrierFrame!==readiness.barrierFrame||!Number.isSafeInteger(finalCheck?.readyFrame)||finalCheck.readyFrame<readiness.barrierFrame||
   !Number.isSafeInteger(finalCheck?.minRenderedThroughFrame)||finalCheck.minRenderedThroughFrame<readiness.barrierFrame||
   finalCheck?.barrierCoveredTapCount!==readiness.taps.length||finalCheck?.reason!==null)
  return bad('The final readiness poll did not retain the fixed rendered-frame barrier.');
 const tapIds=new Set(),historyIds=new Set();
 for(const tap of readiness.taps){
  if(!Number.isSafeInteger(tap?.tapId)||tap.tapId<0||tapIds.has(tap.tapId)||!Number.isSafeInteger(tap.historyHandleId)||tap.historyHandleId<1||
    historyIds.has(tap.historyHandleId)||tap.available!==true||tap.contextIndex!==contextIndex||tap.contextState!=='running'||tap.sampleRate!==sampleRate||
    !Number.isFinite(tap.contextTime)||!Number.isSafeInteger(tap.contextFrame)||tap.contextFrame!==Math.floor(tap.contextTime*sampleRate)||
    tap.contextFrame>ready.contextFrame||!Number.isSafeInteger(tap.firstFrame)||!Number.isSafeInteger(tap.renderedThroughFrame)||
    !Number.isSafeInteger(tap.processCount)||tap.processCount<1||!Number.isSafeInteger(tap.lastProcessStartFrame)||
    !Number.isSafeInteger(tap.lastProcessEndFrame)||tap.firstFrame>tap.lastProcessStartFrame||tap.lastProcessStartFrame>=tap.lastProcessEndFrame||
    tap.lastProcessEndFrame!==tap.renderedThroughFrame||tap.renderedThroughFrame<readiness.barrierFrame)
   return bad('A shared SFX history tap lacks a real render quantum covering the observed SDL3 clock frame.');
  tapIds.add(tap.tapId);historyIds.add(tap.historyHandleId);
 }
 if(basisIds.size!==tapIds.size||[...basisIds].some(id=>!tapIds.has(id)))
  return bad('The fixed barrier basis does not identify every armed history tap exactly once.');
 if(!Array.isArray(historyTapIds)||historyTapIds.length!==tapIds.size||historyTapIds.some(id=>!tapIds.has(id)))
  return bad('The readiness taps do not match every history handle armed for this Runtime input session.');
 return{valid:true,reason:null};
}

export const DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE=`(()=>{
 const unavailableJournal=Object.freeze({available:false,sessionId:null,events:Object.freeze([]),drops:0,unattributed:0,errors:1,
  stopHookAvailable:false,stopHookError:'AudioScheduledSourceNode.stop is unavailable',closeBoundary:null});
 if(window===window.top){window.__eaglerDesktopAudioProbe={available:false,contexts:[],events:[],capture:()=>({clock:null,captures:[],voices:{available:false,voiceCount:0,voices:[]}}),voiceSnapshot:()=>({available:false,voiceCount:0,voices:[]}),startJournalSnapshot:()=>unavailableJournal,summary:()=>({contexts:[],tapCount:0,errors:[]})};return;}
 const shared=window.__eaglerMusicPcmProbe;
 if(!shared){window.__eaglerDesktopAudioProbe={available:false,contexts:[],events:[],capture:()=>({clock:null,captures:[],voices:{available:false,voiceCount:0,voices:[]}}),voiceSnapshot:()=>({available:false,voiceCount:0,voices:[]}),startJournalSnapshot:()=>unavailableJournal,summary:()=>({contexts:[],tapCount:0,errors:['shared PCM probe unavailable']})};return;}
 const events=[],observedContexts=new WeakSet(),eventLimit=64,inputLimit=32,startJournalLimit=64,
  rateAutomationCounts=new WeakMap(),trackedRateParameters=new WeakMap();
 let inputSession=null,eventDrops=0,nextInputSession=0,startHookAvailable=false,startHookError=null,stopHookAvailable=false,stopHookError=null;
 let rateParameterObserverAvailable=false;
 const runtimeFor=game=>{const mount=window.__eaglerDesktopAudioRuntimeMount;
  if(mount){if(mount.game!==game||!mount.runtime)return{runtime:null,module:null};
   const runtime=window[mount.runtime.global];return{runtime,module:runtime?.[mount.runtime.module]??null};}
  const runtime=window['__'+game+'Runtime'];return{runtime,module:runtime?.Module??window.Module};};
 const startJournalSnapshot=(session=inputSession)=>{
  if(!session)return Object.freeze({available:startHookAvailable&&stopHookAvailable,sessionId:null,game:null,code:null,active:false,
   inputEvents:Object.freeze([]),events:Object.freeze([]),drops:0,unattributed:0,errors:(startHookError?1:0)+(stopHookError?1:0),
   stopHookAvailable,stopHookError,closeBoundary:null,historyReadiness:null});
  const journal=session.startJournal;
   return Object.freeze({available:startHookAvailable&&stopHookAvailable&&session.historyReady&&journal.errors===0,sessionId:session.sessionId,game:session.game,code:session.code,active:session.active,
    historyReady:session.historyReady,historyError:session.historyError??null,inputEvents:Object.freeze(session.events.slice()),events:Object.freeze(journal.events.slice()),drops:journal.drops,
    unattributed:journal.unattributed,errors:journal.errors+(startHookError?1:0)+(stopHookError?1:0),stopHookAvailable,stopHookError,
    closeBoundary:session.closeBoundary??null,historyReadiness:session.historyReadiness??null});
 };
 const findSourceInterval=(session,node)=>session?.startJournal.intervals.find(interval=>interval.node===node&&!interval.finalized)??null;
 const noteRateMutation=parameter=>{const intervals=trackedRateParameters.get(parameter);if(intervals)for(const interval of intervals)if(!interval.ended)interval.rateChanged=true;};
 const findPropertyOwner=(object,name)=>{for(let owner=object;owner;owner=Object.getPrototypeOf(owner))if(Object.getOwnPropertyDescriptor(owner,name))return owner;return null;};
 const installRateParameterObserver=()=>{
  const prototype=window.AudioParam?.prototype;if(!prototype)return false;
  const requiredMethods=['setValueAtTime','linearRampToValueAtTime','exponentialRampToValueAtTime','setTargetAtTime','setValueCurveAtTime',
   'cancelScheduledValues'];
  let valid=true;
  for(const name of [...requiredMethods,'cancelAndHoldAtTime']){
   const owner=findPropertyOwner(prototype,name),descriptor=owner&&Object.getOwnPropertyDescriptor(owner,name);
   if(!descriptor){if(requiredMethods.includes(name))valid=false;continue;}
   if(typeof descriptor.value!=='function')continue;
   try{Object.defineProperty(owner,name,{...descriptor,value:function(...args){const result=Reflect.apply(descriptor.value,this,args);
     rateAutomationCounts.set(this,(rateAutomationCounts.get(this)??0)+1);noteRateMutation(this);return result;}});}
   catch(_){valid=false;}
  }
  const valueOwner=findPropertyOwner(prototype,'value'),valueDescriptor=valueOwner&&Object.getOwnPropertyDescriptor(valueOwner,'value');
  if(!valueDescriptor||typeof valueDescriptor.get!=='function'||typeof valueDescriptor.set!=='function'||valueDescriptor.configurable!==true)valid=false;
  else try{Object.defineProperty(valueOwner,'value',{...valueDescriptor,get:function(){return Reflect.apply(valueDescriptor.get,this,[]);},
    set:function(value){const result=Reflect.apply(valueDescriptor.set,this,[value]);noteRateMutation(this);return result;}});}
   catch(_){valid=false;}
  return valid;
 };
 rateParameterObserverAvailable=installRateParameterObserver();
 const sourceIntervalEnd=(node,args,startContextTime,sampleRate)=>{
  const unavailable=reason=>({available:false,rateParametersAvailable:false,endFrame:null,reason,computedPlaybackRate:null,contentDuration:null,offset:null,requestedDuration:null});
  const bufferDuration=Number.isFinite(node?.buffer?.duration)?node.buffer.duration:null,offset=args?.[1]===undefined?0:args[1],requestedDuration=args?.[2],
   playbackRate=Number.isFinite(node?.playbackRate?.value)?node.playbackRate.value:null,
   detune=Number.isFinite(node?.detune?.value)?node.detune.value:null,
   scheduledStart=Number.isFinite(args?.[0])&&args[0]>startContextTime?args[0]:startContextTime;
  if(!rateParameterObserverAvailable)return unavailable('AudioParam automation and direct-write observation is unavailable');
  if(!node?.playbackRate||!node?.detune||!Number.isFinite(bufferDuration)||bufferDuration<0||!Number.isFinite(sampleRate)||sampleRate<=0||
    !Number.isFinite(scheduledStart)||!Number.isFinite(offset)||offset<0||offset>bufferDuration||
    (requestedDuration!==undefined&&(!Number.isFinite(requestedDuration)||requestedDuration<0))||
    !Number.isFinite(playbackRate)||playbackRate<=0||!Number.isFinite(detune))return unavailable('Source timing or rate parameters are not statically bounded');
  if((rateAutomationCounts.get(node.playbackRate)??0)>0||(rateAutomationCounts.get(node.detune)??0)>0)
   return unavailable('Source rate parameters have scheduled automation before start');
  const computedPlaybackRate=playbackRate*Math.pow(2,detune/1200);
  if(!Number.isFinite(computedPlaybackRate)||computedPlaybackRate<=0)return unavailable('Computed source playback rate is invalid');
  if(node?.loop===true&&requestedDuration===undefined)return{available:false,rateParametersAvailable:true,endFrame:null,reason:'Looped source requires an observed finite stop or start duration',
   computedPlaybackRate,contentDuration:null,offset,requestedDuration:requestedDuration??null};
  const availableContent=bufferDuration-offset,contentDuration=requestedDuration===undefined?availableContent:
    node?.loop===true?requestedDuration:Math.min(requestedDuration,availableContent),
   lengthSeconds=contentDuration/computedPlaybackRate;
  if(!Number.isFinite(lengthSeconds)||lengthSeconds<0)return unavailable('Source content duration cannot be converted to an output interval');
  return{available:true,rateParametersAvailable:true,endFrame:Math.round(scheduledStart*sampleRate)+Math.ceil(lengthSeconds*sampleRate),reason:null,
   computedPlaybackRate,contentDuration,offset,requestedDuration:requestedDuration??null};
 };
 const recordSuccessfulStart=(node,args,context,contextIndex,sampleRate,startContextTime,observedAtContextTime,game,module)=>{
  const session=inputSession;if(!session?.active||session.game!==game)return;
  const pool=module?.eaglerSfxOutput;if(!(pool?.voices instanceof Map)){session.startJournal.unattributed++;return;}
  const owners=[];for(const [id,voice] of pool.voices)if(voice?.node===node)owners.push({id,voice});
  if(!owners.length)return;
  const journal=session.startJournal;
  if(owners.length!==1){journal.unattributed++;return;}
  const {id,voice}=owners[0],merge=voice?.merge??null,
   scheduledStartContextTime=Number.isFinite(args?.[0])&&args[0]>startContextTime?args[0]:startContextTime;
  const taps=shared.taps.filter(tap=>tap.context===context&&tap.__eaglerMusicPcmSourceNode===merge);
  const identity=(source,role)=>source?Object.freeze({...shared.identifyNode(source,role)}):null;
  const tap=taps.length===1?taps[0]:null,history=tap?session.histories.get(tap.__eaglerMusicPcmTapId):null;
  const startFrame=Number.isFinite(scheduledStartContextTime)&&Number.isFinite(sampleRate)?Math.round(scheduledStartContextTime*sampleRate):null;
  const sourceInterval=sourceIntervalEnd(node,args,scheduledStartContextTime,sampleRate),endFrame=sourceInterval.endFrame;
  if(!history)journal.errors++;
  if(!Number.isSafeInteger(startFrame)||(endFrame!==null&&(!Number.isSafeInteger(endFrame)||endFrame<startFrame)))journal.errors++;
  const interval={node,merge,tap,history,startFrame,endFrame,naturalEndFrame:endFrame,scheduledStopFrame:null,
   stopFrames:[],stopDrops:0,finalized:false,ended:false,endedObservationAvailable:false,rateChanged:false,sourceIntervalAvailable:sourceInterval.available,
   sourceIntervalReason:sourceInterval.reason,rateParametersAvailable:sourceInterval.rateParametersAvailable,
   computedPlaybackRate:sourceInterval.computedPlaybackRate,contentDuration:sourceInterval.contentDuration,
   offset:sourceInterval.offset,requestedDuration:sourceInterval.requestedDuration,sessionId:session.sessionId};
  for(const parameter of [node.playbackRate,node.detune]){if(!parameter)continue;const tracked=trackedRateParameters.get(parameter)??new Set();tracked.add(interval);trackedRateParameters.set(parameter,tracked);}
  if(typeof node.addEventListener==='function'){
   interval.endedListener=()=>{interval.ended=true;interval.endedContextTime=Number.isFinite(context.currentTime)?context.currentTime:null;
    for(const parameter of [node.playbackRate,node.detune])trackedRateParameters.get(parameter)?.delete(interval);};
   try{node.addEventListener('ended',interval.endedListener,{once:true});interval.endedObservationAvailable=true;}
   catch(_){interval.sourceIntervalAvailable=false;interval.sourceIntervalReason='Source ended event could not be observed';}
  }else{interval.sourceIntervalAvailable=false;interval.sourceIntervalReason='Source ended event observation is unavailable';}
  const row=Object.freeze({sessionId:session.sessionId,sequence:journal.nextSequence++,sessionSequence:session.nextSequence++,game,voiceId:String(id),
   contextIndex,sampleRate,startCallContextTime:startContextTime,startContextTime:scheduledStartContextTime,observedAtContextTime,
   voiceStarted:Number.isFinite(voice?.started)?voice.started:null,
   sourceNodeIdentity:identity(node,'runtime-sfx-buffer-source'),
   outputSourceIdentity:identity(merge,'runtime-sfx-voice-output'),
   outputTapIndex:taps.length===1?taps[0].__eaglerMusicPcmTapId:null,tapMatchCount:taps.length,
   bufferSampleRate:Number.isFinite(node?.buffer?.sampleRate)?node.buffer.sampleRate:null,
   offset:sourceInterval.offset,requestedDuration:sourceInterval.requestedDuration,contentDuration:sourceInterval.contentDuration,
   playbackRate:Number.isFinite(node?.playbackRate?.value)?node.playbackRate.value:null,
   detune:Number.isFinite(node?.detune?.value)?node.detune.value:null,computedPlaybackRate:sourceInterval.computedPlaybackRate,
   sourceIntervalAvailable:interval.sourceIntervalAvailable,sourceIntervalReason:interval.sourceIntervalReason,
   startFrame,endFrame,tapHistoryId:history?.handleId??null});
  if(journal.events.length<startJournalLimit){journal.events.push(row);journal.intervals.push(interval);}
  else journal.drops++;
 };
 const installStartJournal=()=>{
  const prototype=window.AudioBufferSourceNode?.prototype,descriptor=prototype&&Object.getOwnPropertyDescriptor(prototype,'start');
  if(!descriptor||typeof descriptor.value!=='function'){startHookError='AudioBufferSourceNode.start is unavailable';return;}
  const original=descriptor.value;
  const wrapped=function(...args){
   const session=inputSession;
   let pending=null;
   if(session?.active){
    try{
     const {module}=runtimeFor(session.game),context=module?.SDL3?.audioContext;
     if(context&&this?.context===context){
      pending={game:session.game,module,context,contextIndex:shared.contexts.indexOf(context),sampleRate:context.sampleRate,
       startContextTime:context.currentTime};
     }
    }catch(_){session.startJournal.errors++;}
   }
   const result=Reflect.apply(original,this,args);
   if(pending){
    try{recordSuccessfulStart(this,args,pending.context,pending.contextIndex,pending.sampleRate,pending.startContextTime,
      pending.context.currentTime,pending.game,pending.module);}
    catch(_){const active=inputSession;if(active?.active&&active.game===pending.game)active.startJournal.errors++;}
   }
   return result;
  };
  try{Object.defineProperty(prototype,'start',{...descriptor,value:wrapped});startHookAvailable=true;}
  catch(error){startHookError=String(error?.message||error);}
 };
 installStartJournal();
 const installStopJournal=()=>{
  let owner=window.AudioBufferSourceNode?.prototype,descriptor=null;
  while(owner){descriptor=Object.getOwnPropertyDescriptor(owner,'stop');if(descriptor)break;owner=Object.getPrototypeOf(owner);}
  if(!descriptor||typeof descriptor.value!=='function'){stopHookError='AudioScheduledSourceNode.stop owner or method is unavailable';return;}
  const original=descriptor.value;
  const wrapped=function(...args){const result=Reflect.apply(original,this,args);
   const interval=[...sessions].map(session=>findSourceInterval(session,this)).find(Boolean);
   if(interval){const context=interval.node.context,rate=context?.sampleRate,now=context?.currentTime,when=Number.isFinite(args[0])&&args[0]>0?Math.max(now,args[0]):now;
    if(Number.isFinite(when)&&Number.isFinite(rate)){const stopFrame=Math.round(when*rate);interval.stopFrames.push(stopFrame);
     if(interval.stopFrames.length>8){interval.stopFrames.shift();interval.stopDrops++;}
     if(!interval.ended){interval.scheduledStopFrame=stopFrame;
      interval.endFrame=interval.naturalEndFrame===null?stopFrame:Math.min(interval.naturalEndFrame,stopFrame);
      if(interval.rateParametersAvailable&&interval.endedObservationAvailable){interval.sourceIntervalAvailable=true;interval.sourceIntervalReason=null;}}}}
   return result;};
  try{Object.defineProperty(owner,'stop',{...descriptor,value:wrapped});stopHookAvailable=true;}
  catch(error){stopHookError=String(error?.message||error);}
 };
 const sessions=new Set();
 installStopJournal();
 const observeContexts=()=>{for(const context of shared.contexts){if(observedContexts.has(context))continue;observedContexts.add(context);
  context.addEventListener('statechange',()=>{const event={type:'state',state:context.state,currentTime:context.currentTime,contextIndex:shared.contexts.indexOf(context)};
   if(events.length<eventLimit)events.push(event);else eventDrops++;});}};
 const clockFor=game=>{const {module}=runtimeFor(game),context=module?.SDL3?.audioContext??null;
  observeContexts();if(!context)return{available:false,contextIndex:null,state:null,sampleRate:null,currentTime:null,source:'SDL3.audioContext'};
  const contextIndex=shared.contexts.indexOf(context),state=context.state,sampleRate=context.sampleRate,currentTime=context.currentTime;
  return{available:contextIndex>=0&&Number.isFinite(sampleRate)&&Number.isFinite(currentTime),contextIndex,state,sampleRate,currentTime,source:'SDL3.audioContext'};};
 const waitForHistoryReadiness=async(session,armClock)=>{
  const timeoutMs=1000,pollIntervalMs=20,maxPolls=32,startedAt=Date.now(),armFrame=armClock?.available&&Number.isFinite(armClock.currentTime)&&
   Number.isFinite(armClock.sampleRate)?Math.floor(armClock.currentTime*armClock.sampleRate):null,checks=[];
  const base={source:'shared-audioworklet-history-status',sessionId:session.sessionId,contextIndex:session.contextIndex,sampleRate:session.sampleRate,
   arm:{state:armClock?.state??null,currentTime:armClock?.currentTime??null,contextFrame:armFrame},barrierFrame:null,barrierBasis:null,
   elapsedMs:0,pollCount:0,checks,taps:[]};
  let barrierFrame=null,barrierBasis=null;
  const incomplete=(reason,taps=[])=>({...base,barrierFrame,barrierBasis,available:false,status:'incomplete',reason,elapsedMs:Date.now()-startedAt,pollCount:checks.length,
   checks:checks.slice(),taps});
  if(armClock?.available!==true||!Number.isSafeInteger(armFrame)||armClock.state!=='running'||armClock.contextIndex!==session.contextIndex||armClock.sampleRate!==session.sampleRate)
   return incomplete('SDL3 audio clock was not running with the armed history context.');
  if(session.histories.size===0)return incomplete('No shared SFX output histories were armed.');
  let lastTaps=[],lastReason='No AudioWorklet render quantum was observed after history arm.';
  for(let attempt=1;attempt<=maxPolls&&Date.now()-startedAt<timeoutMs;attempt++){
   const replies=await Promise.all([...session.histories].map(async([tapId,history])=>{
    let status;try{status=await history.status();}catch(error){status={available:false,reason:String(error?.message||error)};}
    const clock=clockFor(session.game),contextFrame=clock.available&&clock.sampleRate>0?Math.floor(clock.currentTime*clock.sampleRate):null;
    return{tapId,historyHandleId:history.handleId,status,contextState:clock.state,contextIndex:clock.contextIndex,sampleRate:clock.sampleRate,
     contextTime:clock.currentTime,contextFrame};
   }));
   const readyClock=clockFor(session.game),readyFrame=readyClock.available&&readyClock.sampleRate>0?Math.floor(readyClock.currentTime*readyClock.sampleRate):null;
   lastTaps=replies.map(row=>({tapId:row.tapId,historyHandleId:row.historyHandleId,available:row.status?.available===true,
    reason:row.status?.reason??null,firstFrame:row.status?.firstFrame??null,renderedThroughFrame:row.status?.renderedThroughFrame??null,
    processCount:row.status?.processCount??null,lastProcessStartFrame:row.status?.lastProcessStartFrame??null,
    lastProcessEndFrame:row.status?.lastProcessEndFrame??null,currentFrame:row.status?.currentFrame??null,gaps:row.status?.gaps??null,
    gapDrops:row.status?.gapDrops??null,overwrittenFrames:row.status?.overwrittenFrames??null,contextState:row.contextState,
    contextIndex:row.contextIndex,sampleRate:row.sampleRate,contextTime:row.contextTime,contextFrame:row.contextFrame}));
   const responseValid=row=>row.status?.available===true&&row.status.type==='status'&&Number.isSafeInteger(row.status.processCount)&&row.status.processCount>=0;
   const statusValid=row=>responseValid(row)&&row.status.processCount>=1&&Number.isSafeInteger(row.status.firstFrame)&&
    Number.isSafeInteger(row.status.renderedThroughFrame)&&row.status.processCount>=1&&Number.isSafeInteger(row.status.lastProcessStartFrame)&&
    Number.isSafeInteger(row.status.lastProcessEndFrame)&&row.status.lastProcessEndFrame===row.status.renderedThroughFrame&&
    row.status.lastProcessStartFrame<row.status.lastProcessEndFrame&&row.status.firstFrame<=row.status.lastProcessStartFrame;
   const responsesValid=replies.every(responseValid),statusesValid=replies.every(statusValid),identityValid=readyClock.available&&readyClock.state==='running'&&
    readyClock.contextIndex===session.contextIndex&&readyClock.sampleRate===session.sampleRate&&Number.isSafeInteger(readyFrame)&&
    replies.every(row=>row.contextState==='running'&&row.contextIndex===session.contextIndex&&row.sampleRate===session.sampleRate&&Number.isSafeInteger(row.contextFrame));
   const progressed=Number.isSafeInteger(readyFrame)&&readyFrame>armFrame;
   const processed=replies.filter(statusValid).length;
   const everyTapRenderedAfterArm=statusesValid&&replies.every(row=>row.status.lastProcessEndFrame>armFrame);
   if(barrierFrame===null&&everyTapRenderedAfterArm){
    barrierBasis=replies.map(row=>({tapId:row.tapId,firstFrame:row.status.firstFrame,lastProcessStartFrame:row.status.lastProcessStartFrame,
     lastProcessEndFrame:row.status.lastProcessEndFrame,contextTime:row.contextTime,contextFrame:row.contextFrame}));
    barrierFrame=Math.max(...barrierBasis.map(row=>row.lastProcessEndFrame));
   }
   const tapsCoverBarrier=barrierFrame!==null&&statusesValid&&replies.every(row=>row.status.renderedThroughFrame>=barrierFrame);
   const clockReachedBarrier=Number.isSafeInteger(readyFrame)&&barrierFrame!==null&&readyFrame>=barrierFrame;
   const clockCovered=tapsCoverBarrier&&clockReachedBarrier;
   lastReason=!responsesValid?'A shared AudioWorklet history status response is missing or malformed.':
    !statusesValid?'No AudioWorklet render quantum was observed after history arm.':
    !identityValid?'The SDL3 audio clock or a history tap changed identity or stopped running.':
    !progressed?'The SDL3 audio clock has not advanced beyond the history-arm frame.':
    barrierFrame===null?'No common post-arm AudioWorklet render boundary was observed.':
    !tapsCoverBarrier?'The shared history taps have not rendered through the fixed post-arm barrier frame.':
    !clockReachedBarrier?'The SDL3 audio clock has not reached the fixed post-arm render barrier.':null;
   checks.push({poll:attempt,elapsedMs:Date.now()-startedAt,armFrame,readyFrame,barrierFrame,renderedTapCount:processed,
    barrierCoveredTapCount:barrierFrame!==null?replies.filter(row=>statusValid(row)&&row.status.renderedThroughFrame>=barrierFrame).length:0,
    minRenderedThroughFrame:statusesValid?Math.min(...replies.map(row=>row.status.renderedThroughFrame)):null,
    reason:lastReason});
   if(!responsesValid)return incomplete(lastReason,lastTaps);
   if(!identityValid)return incomplete(lastReason,lastTaps);
   if(progressed&&clockCovered){
    return{...base,barrierFrame,barrierBasis,available:true,status:'ready',reason:null,ready:{state:readyClock.state,currentTime:readyClock.currentTime,contextFrame:readyFrame},
     elapsedMs:Date.now()-startedAt,pollCount:checks.length,checks:checks.slice(),taps:lastTaps};
   }
   if(Date.now()-startedAt>=timeoutMs||attempt===maxPolls)break;
   await new Promise(resolve=>window.setTimeout(resolve,pollIntervalMs));
  }
  if(!lastTaps.length)lastReason='No SDL3 audio clock or Worklet status was available during history readiness.';
  else if(lastReason===null)lastReason='The bounded rendered-history readiness wait ended before clock coverage was established.';
  return incomplete(lastReason,lastTaps);
 };
 const voiceSnapshot=(game,clock=null)=>{const {module}=runtimeFor(game),pool=module?.eaglerSfxOutput;
  if(!(pool?.voices instanceof Map))return{available:false,voiceCount:0,voices:[]};
  const voices=[];for(const [id,voice] of pool.voices){const source=voice?.merge??null,context=source?.context??null;
   const matches=shared.taps.filter(tap=>tap.context===context&&tap.__eaglerMusicPcmSourceNode===source);
   const buffer=voice?.buffer,started=Number.isFinite(voice?.started)?voice.started:null;
   voices.push({id:String(id),playing:voice?.playing===true,node:!!voice?.node,started,
    sourceNodeIdentity:voice?.node?shared.identifyNode(voice.node,'runtime-sfx-buffer-source'):null,
    outputSourceIdentity:source?shared.identifyNode(source,'runtime-sfx-voice-output'):null,
    volume:Number.isFinite(voice?.volume)?voice.volume:null,contextIndex:context?shared.contexts.indexOf(context):null,
    contextSampleRate:Number.isFinite(context?.sampleRate)?context.sampleRate:null,
    outputTapIndex:matches.length===1?matches[0].__eaglerMusicPcmTapId:null,
    tapMatchCount:matches.length,buffer:buffer?{duration:Number.isFinite(buffer.duration)?buffer.duration:null,
     sampleRate:Number.isFinite(buffer.sampleRate)?buffer.sampleRate:null,length:Number.isFinite(buffer.length)?buffer.length:null,
     channels:Number.isFinite(buffer.numberOfChannels)?buffer.numberOfChannels:null}:null});}
  return{available:true,voiceCount:voices.length,contextIndex:clock?.contextIndex??null,voices};};
 const capture=(game)=>{const clock=clockFor(game);if(!clock.available)return{clock,captures:[],voices:voiceSnapshot(game,clock)};
  const context=shared.contexts[clock.contextIndex],snapshot={context,state:clock.state,sampleRate:clock.sampleRate,currentTime:clock.currentTime};
  const captures=shared.capture(snapshot,{retainSamples:true}).filter(row=>row.contextIndex===clock.contextIndex).map(row=>({tapIndex:row.tapId,contextIndex:row.contextIndex,
   contextState:row.contextState,sampleRate:row.sampleRate,currentTime:row.currentTime,role:row.role,sourceIdentity:row.sourceIdentity,
   samples:row.pcmSamples,peakAbs:row.peakAbs,rms:row.rms,nonZero:row.nonZero}));
  return{clock,captures,voices:voiceSnapshot(game,clock)};};
 const prepareSessionHistories=async(session,module,context)=>{
  const pool=module?.eaglerSfxOutput;if(!(pool?.voices instanceof Map))return{available:false,reason:'Runtime SFX voice pool is unavailable before input'};
  const aggregateLimitBytes=32*1024*1024,perTapLimitFrames=1048576,specs=[],usedTaps=new Set();let totalBytes=0,overPerTap=false,unsafeCapacity=false;
  const voices=[...pool.voices];
  for(const [id,voice] of voices){const merge=voice?.merge,buffer=voice?.buffer,taps=shared.taps.filter(tap=>tap.context===context&&tap.__eaglerMusicPcmSourceNode===merge);
   if(!merge||!buffer||taps.length!==1)return{available:false,reason:'SFX voice '+String(id)+' has no unique shared output tap'};
   const speed=Number.isFinite(voice.speed)&&voice.speed>0?voice.speed:1,duration=Number.isFinite(buffer.duration)?buffer.duration:null;
   if(duration===null||duration<0)return{available:false,reason:'SFX voice '+String(id)+' has no source duration'};
   const capacityFrames=Math.ceil((duration/speed+2)*context.sampleRate),bytes=capacityFrames*4;
   if(!Number.isSafeInteger(capacityFrames)||capacityFrames<=0||!Number.isSafeInteger(bytes)||bytes<=0)unsafeCapacity=true;
   if(capacityFrames>perTapLimitFrames)overPerTap=true;
   totalBytes+=bytes;
   const tap=taps[0],tapId=tap.__eaglerMusicPcmTapId;if(usedTaps.has(tapId))
    return{available:false,reason:'Multiple Runtime SFX voices share one output tap, so their PCM intervals cannot be isolated'};usedTaps.add(tapId);
   const route=shared.outputRouteSnapshot(merge);if(!route.known||route.drops!==0)return{available:false,reason:'SFX voice '+String(id)+' has no complete production output-route history'};
   specs.push({id,merge,tap,tapId,capacityFrames,bytes});
  }
  if(!specs.length)return{available:false,reason:'Runtime SFX pool has no output histories to arm'};
  if(unsafeCapacity||overPerTap||!Number.isSafeInteger(totalBytes)||totalBytes>aggregateLimitBytes){
   return{available:false,reason:'SFX history budget exceeded: voices='+voices.length+' requiredBytes='+String(totalBytes)+
    ' aggregateLimitBytes='+aggregateLimitBytes+' perTapLimitFrames='+perTapLimitFrames+(overPerTap?' perTapLimitExceeded=true':'')+(unsafeCapacity?' unsafeCapacity=true':'')};
  }
  const allocated=[];
  for(const spec of specs){const prepared=await shared.prepareTapHistory(spec.tap,{capacityFrames:spec.capacityFrames});
   if(!prepared?.available){for(const history of allocated)try{history.close?.();}catch(_){}
    return{available:false,reason:prepared?.reason??('SFX voice '+String(spec.id)+' history tap did not arm')};}
   allocated.push(prepared.handle);session.histories.set(spec.tapId,prepared.handle);session.historySources.set(spec.tapId,spec.merge);
  }
  return{available:true,taps:[...session.histories.keys()],capacityBytes:totalBytes};
 };
 const beginInputWindow=async(game,code)=>{if(inputSession?.active)throw Error('A desktop SFX input window is already active.');
  const sessionId=++nextInputSession;
  const clock=clockFor(game);
  const {module}=runtimeFor(game),context=module?.SDL3?.audioContext??null;
  inputSession={active:true,historyReady:false,historyError:null,historyReadiness:null,sessionId,nextSequence:1,game,code,context,contextIndex:clock.contextIndex,sampleRate:clock.sampleRate,closeBoundary:null,
   events:[],drops:0,histories:new Map(),historySources:new Map(),historyQueries:new Map(),
   startJournal:{events:[],intervals:[],drops:0,unattributed:0,errors:clock.available?0:1,nextSequence:1}};sessions.add(inputSession);
  let history={available:false,reason:'SDL3 audio context is unavailable'};
  try{if(clock.available&&context)history=await prepareSessionHistories(inputSession,module,context);}
  catch(error){history={available:false,reason:String(error?.message||error)};}
  let historyReadiness={available:false,status:'incomplete',reason:'SFX output histories were not armed.',source:'shared-audioworklet-history-status',sessionId,
   contextIndex:clock.contextIndex,sampleRate:clock.sampleRate,arm:null,ready:null,elapsedMs:0,pollCount:0,checks:[],taps:[]};
  if(history.available){const armClock=clockFor(game);historyReadiness=await waitForHistoryReadiness(inputSession,armClock);}
  if(history.available&&!historyReadiness.available)history={...history,available:false,reason:historyReadiness.reason};
  inputSession.historyReady=history.available;inputSession.historyError=history.reason??null;inputSession.historyReadiness=historyReadiness;
  if(!history.available)inputSession.startJournal.errors++;
  return{code,sessionId,clock,historyReady:history.available,historyError:history.reason??null,historyTaps:history.taps??[],historyCapacityBytes:history.capacityBytes??null,
   historyReadiness};};
 const endInputWindow=async()=>{if(!inputSession)return{available:false,events:[],drops:0};const session=inputSession;session.active=false;
  const context=session.context,closeContextIndex=context?shared.contexts.indexOf(context):-1,closeContextTime=context?.currentTime,closeSampleRate=context?.sampleRate;
  const closeFrame=Number.isFinite(closeContextTime)&&Number.isFinite(closeSampleRate)&&closeSampleRate>0?Math.floor(closeContextTime*closeSampleRate):null;
  let closeReason=null;
  if(!context)closeReason='SDL3 audio context is missing at input-window close';
  else if(context.state!=='running')closeReason='SDL3 audio context is not running at input-window close';
  else if(closeContextIndex!==session.contextIndex||closeSampleRate!==session.sampleRate)closeReason='SDL3 audio context identity or sample rate changed before input-window close';
  else if(!Number.isSafeInteger(closeFrame))closeReason='SDL3 audio context close frame is unavailable';
  session.closeBoundary=Object.freeze({available:closeReason===null,reason:closeReason,contextIndex:closeContextIndex,sampleRate:closeSampleRate,
   contextTime:closeContextTime,closeFrame});
  const rows=session.startJournal.events.map((event,index)=>{const interval=session.startJournal.intervals[index],tap=interval?.tap,history=interval?.history,
    intervalEndFrame=interval?.endFrame??event.endFrame,
    baseIntervalAvailable=interval?.sourceIntervalAvailable===true&&!interval?.rateChanged,
    closeAllowsInterval=session.closeBoundary.available&&Number.isSafeInteger(intervalEndFrame)&&intervalEndFrame<=closeFrame,
    sourceIntervalAvailable=baseIntervalAvailable&&closeAllowsInterval,
    sourceIntervalReason=!baseIntervalAvailable?(interval?.rateChanged?'Source playbackRate or detune changed after start':interval?.sourceIntervalReason??null):
     !session.closeBoundary.available?session.closeBoundary.reason:!closeAllowsInterval?'Source interval ends after the input-window close frame':null,
    startFrame=interval?.startFrame??event.startFrame,endFrame=sourceIntervalAvailable?intervalEndFrame:null;
   let pcmHistory=history?{available:false,reason:sourceIntervalAvailable?'SFX output history query is pending':'Source interval is incomplete; PCM history query was withheld'}:
    {available:false,reason:'SFX output history was not armed'},outputRoute={available:false,connected:null,reason:'production output route was not observed'};
   if(interval&&Number.isSafeInteger(startFrame)&&Number.isSafeInteger(endFrame)&&endFrame>=startFrame){
    outputRoute=shared.outputRouteRange(interval.merge,startFrame,endFrame);
    if(!history)pcmHistory={available:false,reason:'SFX output history handle is missing'};
    else if(endFrame===startFrame)pcmHistory={available:false,reason:'SFX output interval contains no rendered frames'};
    else if(!Number.isSafeInteger(endFrame))pcmHistory={available:false,reason:'SFX output interval has no finite end'};
    else pcmHistory={available:false,reason:'SFX output history query is pending'};
   }
   const row={event,interval,tap,history,startFrame,endFrame,intervalEndFrame,sourceIntervalAvailable,sourceIntervalReason,pcmHistory,outputRoute};
   if(history&&Number.isSafeInteger(startFrame)&&Number.isSafeInteger(endFrame)&&endFrame>startFrame){
    const entries=session.historyQueries.get(history)??[];entries.push(row);session.historyQueries.set(history,entries);
   }
   return row;
  });
  try{
   await Promise.all([...session.historyQueries].map(async([history,entries])=>{
    let response;try{response=await history.snapshotBatch(entries.map(row=>({fromFrame:row.startFrame,toFrame:row.endFrame})));}
    catch(error){response={available:false,reason:String(error?.message||error)};}
    if(response?.available!==true||!Array.isArray(response.results)||response.results.length!==entries.length){
     for(const row of entries)row.pcmHistory={available:false,reason:response?.reason??'SFX history batch response is incomplete'};return;}
    for(let index=0;index<entries.length;index++)entries[index].pcmHistory=response.results[index];
   }));
  }finally{for(const history of session.histories.values())history.close();}
  const finalized=rows.map(({event,interval,tap,history,startFrame,endFrame,intervalEndFrame,sourceIntervalAvailable,sourceIntervalReason,pcmHistory,outputRoute})=>{
   const row=Object.freeze({...event,startFrame,endFrame,intervalEndFrame,closeFrame,sourceIntervalAvailable,sourceIntervalReason,
    computedPlaybackRate:interval?.computedPlaybackRate??event.computedPlaybackRate,contentDuration:interval?.contentDuration??event.contentDuration,
    requestedDuration:interval?.requestedDuration??event.requestedDuration,ended:interval?.ended===true,
    stopEventCount:interval?.stopFrames.length??0,stopDrops:interval?.stopDrops??0,pcmHistory,outputRoute,historyTapIdentity:tap?Object.freeze({tapId:tap.__eaglerMusicPcmTapId,
    analyserNodeId:shared.identifyNode(tap,'runtime-sfx-output-tap').nodeId,sourceNodeId:interval?.merge?shared.identifyNode(interval.merge,'runtime-sfx-voice-output').nodeId:null,
    historyHandleId:history?.handleId??null}):null});if(interval)interval.finalized=true;return row;
  });
  const journal=startJournalSnapshot(session);
  const sfxStartJournal=Object.freeze({...journal,available:journal.available&&session.historyReady&&session.closeBoundary.available,
   events:Object.freeze(finalized),historyTapCount:session.histories.size,historyError:session.historyError,closeBoundary:session.closeBoundary});
  const result={available:true,game:session.game,code:session.code,sessionId:session.sessionId,historyReady:session.historyReady,
   historyError:session.historyError,historyReadiness:session.historyReadiness,historyTaps:Object.freeze([...session.histories.keys()]),events:Object.freeze(session.events.slice()),drops:session.drops,sfxStartJournal};
  inputSession=null;sessions.delete(session);return result;};
 const recordInput=event=>{if(!inputSession?.active||event.code!==inputSession.code)return;
  const clock=clockFor(inputSession.game),row={sessionSequence:inputSession.nextSequence++,type:event.type,code:event.code,isTrusted:event.isTrusted===true,repeat:event.repeat===true,
   eventTimeStamp:Number.isFinite(event.timeStamp)?event.timeStamp:null,contextIndex:clock.contextIndex,sampleRate:clock.sampleRate,
   contextTime:clock.currentTime,contextState:clock.state};
  if(inputSession.events.length<inputLimit)inputSession.events.push(Object.freeze(row));else inputSession.drops++;};
 window.addEventListener('keydown',recordInput,true);window.addEventListener('keyup',recordInput,true);
 window.__eaglerDesktopAudioProbe={available:true,contexts:shared.contexts,taps:shared.taps,events,capture,clockFor,voiceSnapshot,startJournalSnapshot,
  beginInputWindow,endInputWindow,summary(){observeContexts();return{contexts:shared.contexts.map(context=>({state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime})),
   tapCount:shared.taps.length,errors:shared.errors.slice(),events:events.slice(),eventDrops};}};
})();`;

export function validateDesktopSfxInputWindow(window){
  const bad=reason=>({valid:false,reason});
  if(!window||window.source!=='SDL3.audioContext.currentTime'||!Number.isFinite(window.startContextTime)||
    !Number.isFinite(window.endContextTime)||window.endContextTime<window.startContextTime||
    !Number.isInteger(window.contextIndex)||window.contextIndex<0||!Number.isFinite(window.sampleRate)||window.sampleRate<=0)
    return bad('SDL3 audio clock boundaries are missing, malformed, or reversed.');
  const start=window.clockEvidence?.start,end=window.clockEvidence?.end;
  if(start?.available!==true||end?.available!==true||start.source!=='SDL3.audioContext'||end.source!=='SDL3.audioContext.currentTime'||
    start.contextIndex!==window.contextIndex||end.contextIndex!==window.contextIndex||
    start.sampleRate!==window.sampleRate||end.sampleRate!==window.sampleRate||start.currentTime!==window.startContextTime||
    end.currentTime!==window.endContextTime||start.state!=='running'||end.state!=='running'||end.currentTime<=start.currentTime)
    return bad('The action boundaries do not share one observed SDL3 audio context clock.');
  const capture=window.inputCapture;
  const release=window.releaseEvidence;
  if(release?.source!=='readRuntimeObservation.input'||!Number.isFinite(release.baselineValue)||
    !Number.isFinite(release.consumedValue)||release.consumedValue===release.baselineValue||
    !Number.isFinite(release.nativeValue)||release.nativeValue!==release.baselineValue||
    release.contextIndex!==window.contextIndex||release.currentTime!==window.endContextTime)
    return bad('The native input-clear observation is missing or does not match the action end clock.');
  const consumer=release.consumer;
  const snapshots=[consumer?.baseline,consumer?.held,consumer?.released];
  if(snapshots.some(snapshot=>snapshot?.available!==true||snapshot.complete!==true||snapshot.phase!=='consumer-run'||
    !Number.isSafeInteger(snapshot.sequence)||snapshot.sequence<=0||!Number.isSafeInteger(snapshot.stage)||
    !Number.isSafeInteger(snapshot.activePlayerShots)||snapshot.activePlayerShots<0||typeof snapshot.consumed?.fire!=='boolean'))
    return bad('Actual player-consumer snapshots are missing or incomplete.');
  const [baselineConsumer,heldConsumer,releasedConsumer]=snapshots;
  if(baselineConsumer.stage!==heldConsumer.stage||heldConsumer.stage!==releasedConsumer.stage||
    !(baselineConsumer.sequence<heldConsumer.sequence&&heldConsumer.sequence<releasedConsumer.sequence)||
    baselineConsumer.consumed.fire!==false||heldConsumer.consumed.fire!==true||releasedConsumer.consumed.fire!==false||
    heldConsumer.activePlayerShots<=baselineConsumer.activePlayerShots)
    return bad('Player-consumer snapshots do not prove a held fire action created a shot before release.');
  if(capture?.available!==true||capture.code!==window.code||capture.sessionId!==window.sessionId||!Array.isArray(capture.events)||capture.drops!==0)
    return bad('Trusted native input edge evidence is incomplete.');
  const journal=capture.sfxStartJournal;
  const readiness=validateDesktopSfxHistoryReadiness(window.historyReadiness,{sessionId:window.sessionId,contextIndex:window.contextIndex,
    sampleRate:window.sampleRate,historyTapIds:capture.historyTaps});
  const journalReadiness=validateDesktopSfxHistoryReadiness(journal?.historyReadiness,{sessionId:window.sessionId,contextIndex:window.contextIndex,
    sampleRate:window.sampleRate,historyTapIds:capture.historyTaps});
  if(!readiness.valid||!journalReadiness.valid||JSON.stringify(window.historyReadiness)!==JSON.stringify(journal.historyReadiness))
    return bad(readiness.reason??journalReadiness.reason??'Input and closed-session SFX history readiness records do not match.');
  const closeBoundary=journal?.closeBoundary;
  if(closeBoundary?.available!==true||closeBoundary.reason!==null||closeBoundary.contextIndex!==window.contextIndex||closeBoundary.sampleRate!==window.sampleRate||
    !Number.isFinite(closeBoundary.contextTime)||!Number.isSafeInteger(closeBoundary.closeFrame)||
    closeBoundary.closeFrame!==Math.floor(closeBoundary.contextTime*closeBoundary.sampleRate)||closeBoundary.contextTime<window.endContextTime)
    return bad('The input session has no valid synchronous SDL3 audio-context close-frame boundary.');
  const causalOrders=new Set(capture.events.map(event=>event?.sessionSequence));
  if(journal?.available!==true||journal.stopHookAvailable!==true||journal.sessionId!==window.sessionId||journal.game!==window.game||journal.code!==window.code||
    journal.drops!==0||journal.unattributed!==0||journal.errors!==0||!Array.isArray(journal.events)||
    journal.events.some(event=>event?.sessionId!==window.sessionId||event.game!==window.game||!event.voiceId||
      !Number.isSafeInteger(event.sessionSequence)||event.sessionSequence<=0||causalOrders.has(event.sessionSequence)||
      !Number.isFinite(event.startCallContextTime)||!Number.isFinite(event.startContextTime)||!Number.isFinite(event.observedAtContextTime)||
      event.observedAtContextTime<event.startCallContextTime||
      !Number.isFinite(event.voiceStarted)||
      event.contextIndex!==window.contextIndex||event.sampleRate!==window.sampleRate||!Number.isInteger(event.sourceNodeIdentity?.nodeId)||
      !Number.isInteger(event.outputSourceIdentity?.nodeId)||event.tapMatchCount!==1||!Number.isInteger(event.outputTapIndex)||event.outputTapIndex<0||
      event.sourceIntervalAvailable!==true||event.sourceIntervalReason!==null||!Number.isFinite(event.computedPlaybackRate)||event.computedPlaybackRate<=0||
      !Number.isSafeInteger(event.startFrame)||!Number.isSafeInteger(event.endFrame)||!Number.isSafeInteger(event.intervalEndFrame)||
      event.intervalEndFrame!==event.endFrame||event.endFrame<event.startFrame||
      event.endFrame>closeBoundary.closeFrame||event.closeFrame!==closeBoundary.closeFrame))
    return bad('The bounded SFX start journal is missing, overflowed, or cannot uniquely identify a Runtime voice and output tap.');
  for(const event of journal.events)causalOrders.add(event.sessionSequence);
  if(capture.events.some(event=>!Number.isSafeInteger(event?.sessionSequence)||event.sessionSequence<=0)||causalOrders.size!==capture.events.length+journal.events.length)
    return bad('Input edges and SFX starts do not have unique causal order within the session.');
  const events=capture.events.filter(event=>event.code===window.code);
  const downs=events.filter(event=>event.type==='keydown'&&event.isTrusted===true);
  const ups=events.filter(event=>event.type==='keyup'&&event.isTrusted===true);
  if(downs.length!==1||ups.length!==1)return bad('The input window does not contain one trusted keydown and keyup.');
  const [down]=downs,[up]=ups;
  const downIndex=events.indexOf(down),upIndex=events.indexOf(up);
  if(events.some(event=>event.isTrusted!==true)||down.contextIndex!==window.contextIndex||up.contextIndex!==window.contextIndex||
    down.sampleRate!==window.sampleRate||up.sampleRate!==window.sampleRate||!Number.isFinite(down.contextTime)||!Number.isFinite(up.contextTime)||
    !Number.isSafeInteger(down.sessionSequence)||!Number.isSafeInteger(up.sessionSequence)||up.sessionSequence<=down.sessionSequence||
    down.contextState!=='running'||up.contextState!=='running'||downIndex<0||upIndex<=downIndex||down.contextTime<window.startContextTime||
    down.contextTime<window.historyReadiness.ready.currentTime||
    up.contextTime<down.contextTime||up.contextTime>window.endContextTime)
    return bad('Trusted input edges cannot be ordered after rendered-history readiness on the SDL3 audio clock.');
  return{valid:true,down,up,consumer:{baseline:baselineConsumer,held:heldConsumer,released:releasedConsumer}};
}

export function mapDesktopSfxStartJournal(inputCapture,sampledNativeSfx){
  const journal=inputCapture?.sfxStartJournal;
  const unavailable=reason=>({available:false,reason,sourceIntervals:[],activationEvents:[],activeObservations:[]});
  const closeBoundary=journal?.closeBoundary;
  if(inputCapture?.available!==true||!Number.isSafeInteger(inputCapture.sessionId)||!inputCapture.game||!inputCapture.code||
    journal?.available!==true||journal.sessionId!==inputCapture.sessionId||journal.game!==inputCapture.game||journal.code!==inputCapture.code||
    journal.stopHookAvailable!==true||closeBoundary?.available!==true||closeBoundary.reason!==null||!Number.isInteger(closeBoundary.contextIndex)||closeBoundary.contextIndex<0||
    !Number.isFinite(closeBoundary.sampleRate)||closeBoundary.sampleRate<=0||!Number.isFinite(closeBoundary.contextTime)||
    !Number.isSafeInteger(closeBoundary.closeFrame)||closeBoundary.closeFrame!==Math.floor(closeBoundary.contextTime*closeBoundary.sampleRate)||
    !validateDesktopSfxHistoryReadiness(journal.historyReadiness,{sessionId:inputCapture.sessionId,contextIndex:closeBoundary.contextIndex,
      sampleRate:closeBoundary.sampleRate,historyTapIds:inputCapture.historyTaps}).valid||
    journal.historyReady!==true||!Number.isInteger(journal.historyTapCount)||journal.historyTapCount<1||
    !Array.isArray(journal.events)||journal.drops!==0||journal.unattributed!==0||journal.errors!==0)
    return unavailable('The SFX start journal is missing records or cannot attribute every Runtime voice.');
  const observationsList=Array.isArray(sampledNativeSfx?.activeObservations)?sampledNativeSfx.activeObservations:[];
  const inputEvents=inputCapture.events.filter(event=>event.code===inputCapture.code);
  const downs=inputEvents.filter(event=>event.type==='keydown'&&event.isTrusted===true);
  const ups=inputEvents.filter(event=>event.type==='keyup'&&event.isTrusted===true);
  if(inputEvents.some(event=>event.isTrusted!==true)||downs.length!==1||ups.length!==1||
    !Number.isSafeInteger(downs[0]?.sessionSequence)||!Number.isSafeInteger(ups[0]?.sessionSequence)||
    ups[0].sessionSequence<=downs[0].sessionSequence)
    return unavailable('The SFX journal cannot be ordered between one trusted keydown and keyup.');
  const [down]=downs,[up]=ups;
  const causalOrders=new Set();
  for(const event of inputEvents){
    if(!Number.isSafeInteger(event.sessionSequence)||event.sessionSequence<=0||causalOrders.has(event.sessionSequence))
      return unavailable('Input edges do not have unique causal order within the SFX session.');
    causalOrders.add(event.sessionSequence);
  }
  const sourceIntervals=[],activationEvents=[],activeObservations=[];
  const sequences=new Set();
  for(const event of journal.events){
    if(event?.sessionId!==journal.sessionId||event.game!==journal.game||!event?.voiceId||!Number.isSafeInteger(event.sequence)||
      event.sequence<=0||sequences.has(event.sequence)||!Number.isSafeInteger(event.sessionSequence)||event.sessionSequence<=0||
      causalOrders.has(event.sessionSequence)||
      !Number.isInteger(event.contextIndex)||event.contextIndex<0||
      !Number.isFinite(event.sampleRate)||event.sampleRate<=0||!Number.isFinite(event.startCallContextTime)||!Number.isFinite(event.startContextTime)||
      event.contextIndex!==closeBoundary.contextIndex||event.sampleRate!==closeBoundary.sampleRate||
      !Number.isFinite(event.observedAtContextTime)||event.observedAtContextTime<event.startCallContextTime||
      !Number.isFinite(event.voiceStarted)||!Number.isInteger(event.sourceNodeIdentity?.nodeId)||
      !Number.isInteger(event.outputSourceIdentity?.nodeId)||event.tapMatchCount!==1||!Number.isInteger(event.outputTapIndex)||event.outputTapIndex<0||
      event.sourceIntervalAvailable!==true||event.sourceIntervalReason!==null||!Number.isFinite(event.computedPlaybackRate)||event.computedPlaybackRate<=0||
      !Number.isSafeInteger(event.startFrame)||!Number.isSafeInteger(event.endFrame)||!Number.isSafeInteger(event.intervalEndFrame)||
      event.intervalEndFrame!==event.endFrame||event.endFrame<event.startFrame||
      event.endFrame>closeBoundary.closeFrame||event.closeFrame!==closeBoundary.closeFrame||
      !Number.isSafeInteger(event.stopDrops)||event.stopDrops!==0||
      event.startFrame!==Math.round(event.startContextTime*event.sampleRate)||!Number.isSafeInteger(event.tapHistoryId)||
      event.historyTapIdentity?.tapId!==event.outputTapIndex||event.historyTapIdentity?.sourceNodeId!==event.outputSourceIdentity.nodeId||
      event.historyTapIdentity?.historyHandleId!==event.tapHistoryId||!event.pcmHistory||typeof event.pcmHistory.available!=='boolean'||
      !event.outputRoute||typeof event.outputRoute.available!=='boolean')
      return unavailable('A journaled SFX start lacks a unique Runtime output source, bounded PCM interval, or route identity.');
    sequences.add(event.sequence);
    causalOrders.add(event.sessionSequence);
    const sourceRow={id:event.voiceId,started:event.startContextTime,startCallContextTime:event.startCallContextTime,
      voiceStarted:event.voiceStarted,playing:true,node:true,
      contextIndex:event.contextIndex,contextSampleRate:event.sampleRate,outputTapIndex:event.outputTapIndex,tapMatchCount:event.tapMatchCount,
      sourceNodeIdentity:event.sourceNodeIdentity,outputSourceIdentity:event.outputSourceIdentity,startJournalSequence:event.sequence,
      sessionSequence:event.sessionSequence,observedAtContextTime:event.observedAtContextTime,sourceStartFrame:event.startFrame,
      sourceEndFrame:event.endFrame,sourceIntervalEndFrame:event.intervalEndFrame,sourceIntervalAvailable:event.sourceIntervalAvailable,sourceIntervalReason:event.sourceIntervalReason,
      pcmHistory:event.pcmHistory,outputRoute:event.outputRoute,historyTapIdentity:event.historyTapIdentity,tapHistoryId:event.tapHistoryId,
      bufferSampleRate:event.bufferSampleRate,offset:event.offset,requestedDuration:event.requestedDuration,contentDuration:event.contentDuration,
      playbackRate:event.playbackRate,detune:event.detune,computedPlaybackRate:event.computedPlaybackRate,
      stopEventCount:event.stopEventCount??null,stopDrops:event.stopDrops??null,ended:event.ended===true};
    sourceIntervals.push(sourceRow);
    if(event.sessionSequence<=down.sessionSequence||event.sessionSequence>=up.sessionSequence)continue;
    const observations=observationsList.filter(item=>item.id===event.voiceId&&item.started===event.voiceStarted&&
      item.outputTapIndex===event.outputTapIndex&&item.sourceNodeIdentity?.nodeId===event.sourceNodeIdentity.nodeId&&
      item.outputSourceIdentity?.nodeId===event.outputSourceIdentity.nodeId&&item.contextIndex===event.contextIndex&&
      item.contextSampleRate===event.sampleRate);
    const pcm=observations.find(item=>item.pcm)?.pcm??null;
    activationEvents.push({...sourceRow,captureSequence:observations[0]?.captureSequence??null,pcm});
    for(const observation of observations)activeObservations.push({...observation,started:event.startContextTime,voiceStarted:event.voiceStarted});
  }
  return{available:true,reason:null,sessionId:journal.sessionId,drops:journal.drops,unattributed:journal.unattributed,errors:journal.errors,
    sourceIntervals,activationEvents,activeObservations};
}
