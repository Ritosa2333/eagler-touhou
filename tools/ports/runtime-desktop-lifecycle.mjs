export function inspectNativePageFocus(state) {
  const missing=[];
  if(state?.visibilityState!=='visible')missing.push('document.visibilityState is not visible');
  if(state?.hidden!==false)missing.push('document.hidden is not false');
  if(state?.hasFocus!==true)missing.push('document.hasFocus() is not true');
  return {ready:missing.length===0,missing,state:state??null};
}

export function inspectViewportScreenshotClip({bounds,viewport}={}) {
  const missing=[];
  const validBounds=['x','y','width','height'].every(key=>Number.isFinite(bounds?.[key]));
  const validViewport=Number.isFinite(viewport?.width)&&Number.isFinite(viewport?.height)&&viewport.width>0&&viewport.height>0;
  if(!validBounds)missing.push('iframe bounding box must contain finite x, y, width and height values');
  if(validBounds&&(bounds.width<=0||bounds.height<=0))missing.push('iframe bounding box must have positive width and height');
  if(!validViewport)missing.push('page viewport must have positive finite width and height');
  if(validBounds&&validViewport&&bounds.width>0&&bounds.height>0&&
    (bounds.x<0||bounds.y<0||bounds.x+bounds.width>viewport.width||bounds.y+bounds.height>viewport.height))
    missing.push('iframe bounding box must fit entirely within the page viewport');
  const ready=missing.length===0;
  return{ready,clip:ready?{x:bounds.x,y:bounds.y,width:bounds.width,height:bounds.height}:null,
    bounds:bounds??null,viewport:viewport??null,missing};
}

export function inspectPostCleanupForeground({hostState,runtimeState,beforePage,afterPage,backgroundPageCloseStatus}={}) {
  const host=inspectNativePageFocus(hostState),runtime=inspectNativePageFocus(runtimeState),missing=[];
  const temporaryPageClosed=backgroundPageCloseStatus==='closed';
  if(!temporaryPageClosed)missing.push('temporary background page did not close cleanly');
  if(!host.ready)missing.push('host document is not natively visible and focused');
  if(!runtime.ready)missing.push('Runtime document is not natively visible and focused');
  const beforeTarget=beforePage?.target,afterTarget=afterPage?.target;
  const beforeWindowId=beforePage?.window?.windowId,afterWindowId=afterPage?.window?.windowId;
  const sameTarget=beforePage?.status==='matched'&&afterPage?.status==='matched'&&
    beforeTarget?.type==='page'&&afterTarget?.type==='page'&&beforeTarget.targetId===afterTarget.targetId;
  const sameWindow=Number.isSafeInteger(beforeWindowId)&&Number.isSafeInteger(afterWindowId)&&beforeWindowId===afterWindowId;
  if(!sameTarget)missing.push('game page target changed or could not be matched after cleanup');
  if(!sameWindow)missing.push('native Chrome window changed or could not be matched after cleanup');
  return {ready:missing.length===0,missing,temporaryPageClosed,host,runtime,pageIdentity:{sameTarget,sameWindow,
    beforeTargetId:beforeTarget?.targetId??null,afterTargetId:afterTarget?.targetId??null,
    beforeWindowId:Number.isSafeInteger(beforeWindowId)?beforeWindowId:null,
    afterWindowId:Number.isSafeInteger(afterWindowId)?afterWindowId:null,
    beforeStatus:beforePage?.status??'missing',afterStatus:afterPage?.status??'missing'}};
}

export function inspectNativeFireStimulus({forwardEvents=[],fromIndex=0,epoch,code='KeyZ',neutralInputValue,heldInputValue}={}) {
  const keyDown=(forwardEvents??[]).slice(fromIndex).find(item=>item?.code===code&&item.down===true&&item.epoch===epoch)??null;
  const bridgeAccepted=keyDown?.accepted===true;
  const nativeInputChanged=Number.isSafeInteger(neutralInputValue)&&neutralInputValue===0&&
    Number.isSafeInteger(heldInputValue)&&heldInputValue!==neutralInputValue;
  const missing=[];
  if(!keyDown)missing.push('no matching Host keyboard keydown was recorded');
  else if(!bridgeAccepted)missing.push('Host KeyboardInputBridge did not accept the fire key');
  if(!nativeInputChanged)missing.push('the Runtime native input observation did not consume the fire key');
  return {ready:missing.length===0,missing,code,epoch,bridgeAccepted,nativeInputChanged,
    nativeFireConsumed:bridgeAccepted&&nativeInputChanged,keyDown};
}

export function inspectNativeBackgroundEvents({state,events=[]}={}) {
  const visibilityHidden=state?.visibilityState==='hidden'&&state?.hidden===true;
  const visibilityChange=events.some(event=>event?.type==='visibilitychange'&&event.hidden===true);
  const blur=events.some(event=>event?.type==='blur');
  return {visibilityHidden,visibilityChange,blur,observed:visibilityHidden||visibilityChange||blur};
}

export function inspectMinimizedKeyboardClear({windowState,nativeState,events=[],hostEvents=[],hiddenStartAtMs,hiddenSampledAtMs,inputValue,neutralInputValue}={}) {
  const background=inspectNativeBackgroundEvents({state:nativeState,events});
  const windowHidden=windowState==='minimized'&&background.observed;
  const acceptedClear=(hostEvents??[]).find(event=>
    (event?.reason==='host-blur'||event?.reason==='host-hidden')&&event.accepted===true&&
    Number.isFinite(event.time)&&Number.isFinite(hiddenStartAtMs)&&Number.isFinite(hiddenSampledAtMs)&&
    event.time>=hiddenStartAtMs&&event.time<=hiddenSampledAtMs)??null;
  const nativeNeutral=Number.isSafeInteger(inputValue)&&Number.isSafeInteger(neutralInputValue)&&inputValue===neutralInputValue&&neutralInputValue===0;
  const status=!windowHidden||!acceptedClear?'incomplete':nativeNeutral?'passed':'failed';
  return{ready:status==='passed',status,windowHidden,background,acceptedClear,hostClearAccepted:!!acceptedClear,
    nativeNeutral,inputValue:Number.isSafeInteger(inputValue)?inputValue:null,
    neutralInputValue:Number.isSafeInteger(neutralInputValue)?neutralInputValue:null,
    missing:!windowHidden?['native hidden/blur state was not established while the window was minimized']:
      !acceptedClear?['Host keyboard-clear was not accepted during the minimized hidden interval']:
        !nativeNeutral?['native input was not neutral while the window was still minimized']:[]};
}

export function inspectAudioContextsBeforeGesture(contexts=[]) {
  const observed=Array.isArray(contexts)?contexts:[];
  const ready=observed.length>0&&observed.every(item=>item?.state==='running');
  return{ready,contextCount:observed.length,states:observed.map(item=>item?.state??null),
    reason:ready?null:observed.length===0?'No Runtime AudioContext connected to the observed output tap was available before the fire gesture.':
      'Every Runtime AudioContext connected to the observed output tap must already be running before KeyZ.'};
}

export function inspectAudioGestureWindow({contextsBeforeGesture=[],stateEvents=[],capturedContextStates=[]}={}) {
  const preGesture=inspectAudioContextsBeforeGesture(contextsBeforeGesture);
  const observedCaptureStates=Array.isArray(capturedContextStates)?capturedContextStates:[];
  const captureStayedRunning=observedCaptureStates.length>0&&observedCaptureStates.every(state=>state==='running');
  const stateStable=(stateEvents??[]).length===0;
  const ready=preGesture.ready&&stateStable&&captureStayedRunning;
  const missing=[];
  if(!preGesture.ready)missing.push(preGesture.reason);
  if(!stateStable)missing.push('An AudioContext state transition occurred during the KeyZ capture window.');
  if(!captureStayedRunning)missing.push('The PCM capture did not observe a running state for every sampled output context.');
  return{ready,preGesture,captureStayedRunning,stateStable,capturedContextStates:observedCaptureStates,
    stateEvents:[...(stateEvents??[])],missing};
}

export function classifyAudioLifecyclePath({beforeContexts=[],afterContexts=[],stateEvents=[]}={}) {
  const beforeRunning=beforeContexts.length>0&&beforeContexts.every(item=>item?.state==='running');
  const afterRunning=afterContexts.length>0&&afterContexts.every(item=>item?.state==='running');
  const suspended=stateEvents.some(item=>item?.state==='suspended');
  const resumedAfterSuspend=stateEvents.some((item,index)=>item?.state==='suspended'&&
    stateEvents.slice(index+1).some(next=>next?.state==='running'));
  const path=resumedAfterSuspend?'suspended-resumed':beforeRunning&&afterRunning&&!suspended?'continuous-running':
    suspended?'suspended-without-observed-resume':'context-state-transition-unclassified';
  return{path,beforeRunning,afterRunning,suspended,resumedAfterSuspend,stateEvents};
}

export function compareNativeBrowserTargets({pageTarget,pageWindowId,otherTarget,otherWindowId}={}) {
  if(pageTarget?.type!=='page'||otherTarget?.type!=='page'||!pageTarget.targetId||!otherTarget.targetId||
    !Number.isSafeInteger(pageWindowId)||!Number.isSafeInteger(otherWindowId))
    return {status:'incomplete',sameWindow:null,reason:'Both page target IDs and native Browser window IDs are required.'};
  return {status:pageWindowId===otherWindowId?'matched':'different-window',sameWindow:pageWindowId===otherWindowId,
    pageTargetId:pageTarget.targetId,otherTargetId:otherTarget.targetId,pageWindowId,otherWindowId};
}

export function selectNativePageTarget({targetInfos=[],url,title}={}) {
  const matches=targetInfos.filter(item=>item?.type==='page'&&
    (typeof url==='string'&&item.url===url||typeof title==='string'&&item.title===title));
  if(matches.length!==1)return {status:'incomplete',target:null,matchCount:matches.length,
    reason:`Expected one native page target matching the observed URL or title; found ${matches.length}.`};
  const target=matches[0];
  if(!target.targetId)return {status:'incomplete',target:null,matchCount:1,reason:'The native page target has no target ID.'};
  return {status:'matched',target:{targetId:target.targetId,type:target.type,url:target.url,title:target.title??''},matchCount:1};
}

export const backgroundOnlyRequiredChecks=Object.freeze([
  'package-and-private-data-identity',
  'window-minimize-foreground-and-release',
  'background-tab-foreground-release',
  'background-audio-recovery',
  'package-and-private-data-identity-postflight',
]);

export const backgroundOnlyOmittedChecks=Object.freeze([
  'native-four-direction-arrows',
  'negative-control-suppressed-iframe-runtime-listener-keyboard-mapping',
  'negative-control-suppressed-host-runtime-bridge-keyboard-mapping',
  'native-key-clear-while-held',
  'audio-output-with-music-disabled',
  'simulated-touch-mode-touch',
  'native-four-direction-wasd',
  'touch-fire-focus-and-mixed-input',
  'native-pause-state-and-freeze',
  'iframe-exit-and-new-epoch',
]);

export const inputOnlyRequiredChecks=Object.freeze([
  'package-and-private-data-identity',
  'native-four-direction-arrows',
  'negative-control-suppressed-iframe-runtime-listener-keyboard-mapping',
  'negative-control-suppressed-host-runtime-bridge-keyboard-mapping',
  'native-key-clear-while-held',
  'audio-output-with-music-disabled',
  'simulated-touch-mode-touch',
  'native-four-direction-wasd',
  'touch-fire-focus-and-mixed-input',
  'native-pause-state-and-freeze',
  'iframe-exit-and-new-epoch',
  'package-and-private-data-identity-postflight',
]);

export const inputOnlyOmittedChecks=Object.freeze([
  'window-minimize-foreground-and-release',
  'background-tab-foreground-release',
  'background-audio-recovery',
]);

export function selectDesktopExecutionMode({backgroundOnly=false,inputOnly=false}={}){
  if(backgroundOnly&&inputOnly)throw new Error('--background-only and --input-only are mutually exclusive.');
  return backgroundOnly?'background-only':inputOnly?'input-only':'full-desktop';
}

export function desktopCheckScope(mode){
  if(mode==='background-only')return{requiredIds:[...backgroundOnlyRequiredChecks],omittedIds:[...backgroundOnlyOmittedChecks]};
  if(mode==='input-only')return{requiredIds:[...inputOnlyRequiredChecks],omittedIds:[...inputOnlyOmittedChecks]};
  if(mode==='full-desktop')return{requiredIds:null,omittedIds:[]};
  throw new RangeError(`Unknown desktop check scope ${String(mode)}`);
}

export function summarizeDesktopCheckScope(checks,{requiredIds=[],omittedIds=[]}={}) {
  const required=new Set(requiredIds),omitted=new Set(omittedIds),byId=new Map();
  const duplicates=[];
  for(const check of checks??[]){
    if(byId.has(check?.id))duplicates.push(check.id);
    else byId.set(check?.id,check);
  }
  const requiredResults=requiredIds.map(id=>({id,status:byId.get(id)?.status??'missing'}));
  const missingRequired=requiredResults.filter(item=>item.status==='missing').map(item=>item.id);
  const failedRequired=requiredResults.filter(item=>item.status==='failed').map(item=>item.id);
  const incompleteRequired=requiredResults.filter(item=>!['passed','failed','missing'].includes(item.status)).map(item=>item.id);
  const unexpectedChecks=(checks??[]).filter(item=>!required.has(item?.id)).map(item=>({id:item?.id,status:item?.status??'unknown',omitted:omitted.has(item?.id)}));
  const duplicateRequired=duplicates.filter(id=>required.has(id));
  const status=failedRequired.length||unexpectedChecks.length||duplicateRequired.length?'failed':missingRequired.length||incompleteRequired.length?'incomplete':'passed';
  return {status,requiredResults,missingRequired,failedRequired,incompleteRequired,unexpectedChecks,duplicateRequired,
    omittedIds:[...omittedIds]};
}
