export function inspectNativePageFocus(state) {
  const missing=[];
  if(state?.visibilityState!=='visible')missing.push('document.visibilityState is not visible');
  if(state?.hidden!==false)missing.push('document.hidden is not false');
  if(state?.hasFocus!==true)missing.push('document.hasFocus() is not true');
  return {ready:missing.length===0,missing,state:state??null};
}

export function inspectDesktopInputCleanup({required=false,result=null,keyErrors=[],evaluationError=null}={}) {
  const missing=[...keyErrors];
  if(evaluationError)missing.push(evaluationError);
  if(required){
    for(const field of ['liveRuntime','keyboardCleared','touchCancelAcknowledged','touchControlsAcknowledged'])
      if(result?.[field]!==true)missing.push(`input cleanup ${field} was not acknowledged`);
    missing.push(...(result?.sendErrors??[]));
  }
  return{status:missing.length?'failed':required?'passed':'not-required',required,result,keyErrors,evaluationError,missing};
}

export function inspectNativeForegroundIdentity({expected,observed,requireNormal=true}={}) {
  const missing=[];
  const valid=page=>page?.status==='matched'&&page.target?.type==='page'&&
    typeof page.target.targetId==='string'&&page.target.targetId.length>0&&Number.isSafeInteger(page.window?.windowId);
  if(!valid(expected))missing.push('the expected actual page target and native window identity are unavailable');
  if(!valid(observed))missing.push('the observed actual page target and native window identity are unavailable');
  const sameTarget=valid(expected)&&valid(observed)&&expected.target.targetId===observed.target.targetId;
  const sameWindow=valid(expected)&&valid(observed)&&expected.window.windowId===observed.window.windowId;
  if(valid(expected)&&valid(observed)&&!sameTarget)missing.push('the actual page target changed during foreground restoration');
  if(valid(expected)&&valid(observed)&&!sameWindow)missing.push('the native browser window changed during foreground restoration');
  const windowState=observed?.window?.currentBounds?.windowState??observed?.window?.bounds?.windowState??null;
  if(requireNormal&&windowState!=='normal')missing.push('the observed native browser window is not in normal state');
  return{ready:missing.length===0,missing,sameTarget,sameWindow,windowState,
    targetId:observed?.target?.targetId??null,windowId:Number.isSafeInteger(observed?.window?.windowId)?observed.window.windowId:null};
}

/** Restore only the owned window and wait for native bounds/focus to settle together. */
export async function restoreNativeForeground({expected,readIdentity,restoreWindow,activatePage,focusRuntime,
  readHost,readRuntime,wait,now=Date.now,timeoutMs=5000}={}){
  const started=now(),samples=[];
  let identity=await readIdentity(),owner=expected??identity,stable=0;
  do{
    const ownership=inspectNativeForegroundIdentity({expected:owner,observed:identity,requireNormal:false});
    if(!ownership.ready)return{ready:false,reason:'Owned page/window identity changed',samples,identity,ownership};
    const state=identity.window.currentBounds?.windowState??identity.window.bounds?.windowState;
    if(state!=='normal')await restoreWindow(identity.window.windowId);
    await activatePage(identity.target.targetId);
    if(focusRuntime)await focusRuntime();
    const host=await readHost(),runtime=readRuntime?await readRuntime():null;
    identity=await readIdentity();
    const identityCheck=inspectNativeForegroundIdentity({expected:owner,observed:identity});
    const hostFocus=inspectNativePageFocus(host),runtimeFocus=runtime?inspectNativePageFocus(runtime):null;
    const ready=identityCheck.ready&&hostFocus.ready&&(!readRuntime||runtimeFocus?.ready===true);
    samples.push({elapsedMs:now()-started,identity,identityCheck,host:hostFocus,runtime:runtimeFocus});
    stable=ready?stable+1:0;
    if(stable>=2)return{ready:true,samples,identity,identityCheck,host:hostFocus,runtime:runtimeFocus};
    if(!inspectNativeForegroundIdentity({expected:owner,observed:identity,requireNormal:false}).ready)
      return{ready:false,reason:'Owned page/window identity changed',samples,identity,identityCheck};
    await wait(100);
  }while(now()-started<timeoutMs);
  return{ready:false,reason:'Native window bounds and document focus did not settle before the deadline',
    samples,identity,...samples.at(-1)};
}

export function desktopExecutionBrowserMode(mode){
  if(['input-only','touch-only','actions-only','replay-recording-only','audio-latency-only'].includes(mode))return'headless';
  if(['background-only','full-desktop'].includes(mode))return'headed';
  throw new RangeError(`Unknown desktop execution mode ${String(mode)}`);
}

export function inspectDesktopTerminalErrors({errors=[],events=[],httpErrors=[],fixtureBase}={}){
  const pageErrors=errors.filter(item=>item?.page);
  const protocolErrors=events.filter(event=>event?.protocol==='eagler-touhou/1'&&
    ['error','runtime-error','fatal-error'].includes(event.event));
  const localHttpErrors=httpErrors.filter(event=>typeof fixtureBase==='string'&&event?.url?.startsWith(fixtureBase));
  return{ready:!pageErrors.length&&!protocolErrors.length&&!localHttpErrors.length,pageErrors,protocolErrors,localHttpErrors};
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

export function inspectMinimizedKeyboardClear({windowState,nativeState,events=[],hostEvents=[],hiddenStartAtMs,hiddenSampledAtMs,inputValue,neutralInputValue,inputObservation}={}) {
  const background=inspectNativeBackgroundEvents({state:nativeState,events});
  const windowHidden=windowState==='minimized'&&background.observed;
  const acceptedClear=(hostEvents??[]).find(event=>
    (event?.reason==='host-blur'||event?.reason==='host-hidden')&&event.accepted===true&&
    Number.isFinite(event.time)&&Number.isFinite(hiddenStartAtMs)&&Number.isFinite(hiddenSampledAtMs)&&
    event.time>=hiddenStartAtMs&&event.time<=hiddenSampledAtMs)??null;
  const inputObserved=Number.isSafeInteger(inputValue),neutralObserved=Number.isSafeInteger(neutralInputValue)&&neutralInputValue===0;
  const nativeNeutral=inputObserved&&neutralObserved&&inputValue===neutralInputValue;
  const status=!windowHidden||!acceptedClear||!inputObserved||!neutralObserved?'incomplete':nativeNeutral?'passed':'failed';
  return{ready:status==='passed',status,windowHidden,background,acceptedClear,hostClearAccepted:!!acceptedClear,
    inputObserved,neutralObserved,inputObservation:inputObservation?{
      status:inputObservation.status??null,samples:Number.isSafeInteger(inputObservation.samples)?inputObservation.samples:null,
      validSamples:Number.isSafeInteger(inputObservation.validSamples)?inputObservation.validSamples:null,
      readErrors:Number.isSafeInteger(inputObservation.readErrors)?inputObservation.readErrors:null,
      durationMs:Number.isFinite(inputObservation.durationMs)?inputObservation.durationMs:null,
      lastReadError:typeof inputObservation.lastReadError==='string'?inputObservation.lastReadError:null,
    }:null,
    nativeNeutral,inputValue:Number.isSafeInteger(inputValue)?inputValue:null,
    neutralInputValue:Number.isSafeInteger(neutralInputValue)?neutralInputValue:null,
    missing:!windowHidden?['native hidden/blur state was not established while the window was minimized']:
      !acceptedClear?['Host keyboard-clear was not accepted during the minimized hidden interval']:
        !inputObserved||!neutralObserved?['Native input could not be observed while the window was still minimized']:
          !nativeNeutral?['native input was not neutral while the window was still minimized']:[]};
}

/** Stop waiting on a completed tick or a changed/missing world observation. */
export function shouldAssessResumedMovement(hidden,restored) {
  const clock=hidden?.capabilities?.restartFrame;
  return typeof clock!=='string'||!Number.isSafeInteger(hidden?.frames?.[clock])||
    hidden?.status?.stage!==restored?.status?.stage||restored?.capabilities?.restartFrame!==clock||
    !Number.isSafeInteger(restored?.frames?.[clock])||restored.frames[clock]>hidden.frames[clock];
}

/** Published neutrality and the next completed movement tick must agree. */
export function inspectResumedKeyboardMovement({hidden,restored,neutralInputValue,tolerance=.0001}={}) {
  const clock=hidden?.capabilities?.restartFrame;
  const tickObserved=typeof clock==='string'&&restored?.capabilities?.restartFrame===clock&&
    Number.isSafeInteger(hidden?.frames?.[clock])&&Number.isSafeInteger(restored?.frames?.[clock])&&
    restored.frames[clock]>hidden.frames[clock];
  const coordinatesObserved=['x','y'].every(axis=>Number.isFinite(hidden?.position?.[axis])&&Number.isFinite(restored?.position?.[axis]));
  const sameStage=Number.isSafeInteger(hidden?.status?.stage)&&hidden.status.stage===restored?.status?.stage;
  const available=tickObserved&&coordinatesObserved&&sameStage&&neutralInputValue===0&&Number.isFinite(tolerance)&&tolerance>=0;
  const released=hidden?.input?.value===neutralInputValue&&restored?.input?.value===neutralInputValue;
  const delta=coordinatesObserved?{x:restored.position.x-hidden.position.x,y:restored.position.y-hidden.position.y}:null;
  const stationary=!!delta&&Math.abs(delta.x)<=tolerance&&Math.abs(delta.y)<=tolerance;
  return{status:!available?'incomplete':released&&stationary?'passed':'failed',ready:available&&released&&stationary,
    tickObserved,coordinatesObserved,sameStage,clock:clock??null,
    hiddenFrame:hidden?.frames?.[clock]??null,restoredFrame:restored?.frames?.[clock]??null,
    released,stationary,delta,tolerance,
    reason:!available?'The resumed movement tick, same-stage positions or neutral input baseline were not observed.':
      !released?'Held native input survived clearing or reappeared after resume.':!stationary?'The first observed resumed movement consumed residual displacement.':null};
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

/** Per-document clocks remain separate | iframe blur is expected when Host body owns input. */
export function inspectForegroundPcmWindow({samples=[],hostEvents=[],runtimeEvents=[]}={}) {
  const missing=[];
  const phases=['before-capture','after-capture','after-keyup'];
  const complete=samples.length===phases.length&&samples.every((sample,index)=>sample?.phase===phases[index]&&
    ['host','runtime'].every(document=>Number.isFinite(sample?.[document]?.time)&&sample[document].time>=0&&
      (index===0||sample[document].time>samples[index-1]?.[document]?.time)));
  if(!complete)missing.push('Foreground PCM window boundary observations are incomplete.');
  const validSource=source=>typeof source?.url==='string'&&source.url.length>0&&Number.isFinite(source?.timeOrigin)&&source.timeOrigin>=0;
  const sameSource=(a,b)=>validSource(a)&&validSource(b)&&a.url===b.url&&a.timeOrigin===b.timeOrigin;
  const sourceStable=complete&&['host','runtime'].every(document=>samples.every(sample=>sameSource(sample[document]?.source,samples[0][document]?.source)));
  const eventSourcesValid=sourceStable&&hostEvents.every(event=>sameSource(event.source,samples[0].host.source))&&
    runtimeEvents.every(event=>sameSource(event.source,samples[0].runtime.source));
  if(!sourceStable||!eventSourcesValid)missing.push('Foreground PCM observations did not retain the same document and performance clock identities.');
  const hostVisibleFocused=complete&&samples.every(({host})=>host.visibilityState==='visible'&&host.hidden===false&&host.hasFocus===true&&host.inputOwnerFocused===true);
  const runtimeVisible=complete&&samples.every(({runtime})=>runtime.visibilityState==='visible'&&runtime.hidden===false);
  const inWindow=(event,document)=>Number.isFinite(event?.time)&&event.time>=samples[0]?.[document]?.time&&event.time<=samples.at(-1)?.[document]?.time;
  const hostInterruptions=hostEvents.filter(event=>inWindow(event,'host')&&(['host-blur','host-hidden'].includes(event.reason)||event.type==='blur'||event.hidden===true));
  const runtimeInterruptions=runtimeEvents.filter(event=>inWindow(event,'runtime')&&(event.hidden===true||event.visibility==='hidden'||event.type==='pagehide'));
  if(!hostVisibleFocused)missing.push('Host keyboard input owner was not visible and focused at every PCM window boundary.');
  if(!runtimeVisible)missing.push('Runtime document was not visible at every PCM window boundary.');
  if(hostInterruptions.length)missing.push('Host lost foreground input ownership during the PCM window.');
  if(runtimeInterruptions.length)missing.push('Runtime document became hidden or left during the PCM window.');
  return{ready:missing.length===0,evidenceRole:'foreground-environment-window',complete,sourceStable,eventSourcesValid,hostVisibleFocused,runtimeVisible,
    hostInterruptions,runtimeInterruptions,samples,hostEvents,runtimeEvents,missing};
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

const DESKTOP_CHECK_DECLARATIONS=Object.freeze([
  Object.freeze({id:'package-and-private-data-identity',role:'prerequisite',adaptationIds:['BUILD-04','PACKAGE-02'],modes:['background-only','input-only','touch-only','actions-only','replay-recording-only','audio-latency-only']}),
  Object.freeze({id:'launcher-touch-native-consumers',adaptationIds:['TOUCH-06','TOUCH-15','INPUT-05'],modes:['actions-only']}),
  Object.freeze({id:'shared-launcher-touch-focus',adaptationIds:['TOUCH-05','TOUCH-06'],modes:['actions-only']}),
  Object.freeze({id:'shared-touch-restart',adaptationIds:['TOUCH-18'],modes:['actions-only']}),
  Object.freeze({id:'shared-replay-recording-and-isolation',adaptationIds:['REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06'],modes:['replay-recording-only']}),
  Object.freeze({id:'shared-touch-actions',adaptationIds:['TOUCH-08','RUNTIME-04'],modes:['actions-only']}),
  Object.freeze({id:'shared-touch-double-tap-bomb',adaptationIds:['TOUCH-11'],modes:['actions-only']}),
  Object.freeze({id:'shared-touch-deathbomb',adaptationIds:['TOUCH-09'],modes:['actions-only']}),
  Object.freeze({id:'shared-touch-dialogue-confirm',adaptationIds:['TOUCH-13'],modes:['actions-only']}),
  Object.freeze({id:'shared-touch-unlimited-result',adaptationIds:['TOUCH-10'],modes:['actions-only']}),
  Object.freeze({id:'shared-original-menu-dialogue-input',adaptationIds:['INPUT-01','INPUT-04','INPUT-07'],modes:['input-only']}),
  Object.freeze({id:'native-keyboard-actions',adaptationIds:['INPUT-01'],modes:['input-only']}),
  Object.freeze({id:'shared-touch-menu-dialogue-gestures',adaptationIds:['TOUCH-12'],modes:['actions-only']}),
  Object.freeze({id:'shared-launcher-gamepad-mapping',adaptationIds:['INPUT-03'],modes:['input-only']}),
  Object.freeze({id:'shared-launcher-touch-layout',adaptationIds:['TOUCH-17'],modes:['actions-only']}),
  Object.freeze({id:'shared-launcher-touch-magnifier',adaptationIds:['TOUCH-19'],modes:['actions-only']}),
  Object.freeze({id:'native-four-direction-arrows',adaptationIds:['INPUT-01'],modes:['input-only']}),
  Object.freeze({id:'negative-control-suppressed-iframe-runtime-listener-keyboard-mapping',adaptationIds:['INPUT-01'],modes:['input-only']}),
  Object.freeze({id:'negative-control-suppressed-host-runtime-bridge-keyboard-mapping',adaptationIds:['INPUT-01'],modes:['input-only']}),
  Object.freeze({id:'native-key-clear-while-held',adaptationIds:['INPUT-06'],modes:['input-only']}),
  Object.freeze({id:'audio-output-with-music-disabled',adaptationIds:['AUDIO-02'],modes:['input-only','audio-latency-only']}),
  Object.freeze({id:'audio-none-contract',adaptationIds:['AUDIO-02'],modes:['audio-latency-only']}),
  Object.freeze({id:'native-sfx-before-mixed-queue-tail',adaptationIds:['AUDIO-02'],modes:['audio-latency-only']}),
  Object.freeze({id:'negative-control-sfx-delayed-to-mixed-queue-tail',adaptationIds:['AUDIO-02'],modes:['audio-latency-only']}),
  Object.freeze({id:'shared-touch-limited',adaptationIds:['TOUCH-01','TOUCH-05','TOUCH-14','TOUCH-16'],modes:['input-only','touch-only']}),
  Object.freeze({id:'shared-touch-unlimited',adaptationIds:['TOUCH-02','TOUCH-14','TOUCH-16'],modes:['input-only','touch-only']}),
  Object.freeze({id:'shared-touch-joystick-free',adaptationIds:['TOUCH-04','TOUCH-05','TOUCH-16'],modes:['input-only','touch-only']}),
  Object.freeze({id:'shared-touch-limited-restored',adaptationIds:['TOUCH-01','TOUCH-02','TOUCH-14','TOUCH-16'],modes:['input-only','touch-only']}),
  Object.freeze({id:'shared-touch-two-finger-focus',adaptationIds:['TOUCH-07','TOUCH-15'],modes:['input-only','touch-only']}),
  Object.freeze({id:'native-four-direction-wasd',adaptationIds:['INPUT-02'],modes:['input-only']}),
  Object.freeze({id:'shared-keyboard-layouts',adaptationIds:['INPUT-02'],modes:['input-only']}),
  Object.freeze({id:'shared-gamepad-native-input',adaptationIds:['INPUT-04'],modes:['input-only']}),
  Object.freeze({id:'touch-fire-focus-and-mixed-input',adaptationIds:['TOUCH-15','INPUT-05'],adaptationIdsByGame:{th09:[]},modes:['input-only']}),
  Object.freeze({id:'native-pause-state-and-freeze',adaptationIds:['RUNTIME-04'],modes:['input-only']}),
  Object.freeze({id:'iframe-exit-and-new-epoch',adaptationIds:['RUNTIME-01','RUNTIME-04'],modes:['input-only']}),
  Object.freeze({id:'window-minimize-foreground-and-release',adaptationIds:['RUNTIME-04','INPUT-06','TOUCH-16'],modes:['background-only']}),
  Object.freeze({id:'touch-pointer-cancel-and-release',adaptationIds:['TOUCH-16'],modes:['background-only']}),
  Object.freeze({id:'gamepad-disconnect-and-reconnect',adaptationIds:['INPUT-06'],modes:['background-only']}),
  Object.freeze({id:'background-input-lifecycle-contract',adaptationIds:['INPUT-06','TOUCH-16'],modes:['background-only']}),
  Object.freeze({id:'background-tab-foreground-release',adaptationIds:['RUNTIME-04','INPUT-06','TOUCH-16'],modes:['background-only']}),
  Object.freeze({id:'background-audio-recovery',adaptationIds:['AUDIO-04'],modes:['background-only']}),
  Object.freeze({id:'runtime-epoch-touch-mode-touch-unlimited',adaptationIds:['RUNTIME-01'],modes:[]}),
  Object.freeze({id:'runtime-epoch-touch-mode-joystick',adaptationIds:['RUNTIME-01','TOUCH-03'],modes:['input-only','touch-only']}),
  Object.freeze({id:'runtime-epoch-touch-mode-joystick-free',adaptationIds:['RUNTIME-01','TOUCH-04'],modes:[]}),
  Object.freeze({id:'package-and-private-data-identity-postflight',role:'prerequisite',adaptationIds:['BUILD-04','PACKAGE-02'],modes:['background-only','input-only','touch-only','actions-only','replay-recording-only','audio-latency-only']}),
]);
const DESKTOP_CHECK_BY_ID=new Map(DESKTOP_CHECK_DECLARATIONS.map(check=>[check.id,check]));
const DESKTOP_SELECTIVE_MODES=Object.freeze(['background-only','input-only','touch-only','actions-only','replay-recording-only','audio-latency-only']);

function checksForMode(mode){
  return DESKTOP_CHECK_DECLARATIONS.filter(check=>check.modes.includes(mode)).map(check=>check.id);
}

function omittedForMode(mode){
  const required=new Set(checksForMode(mode));
  return DESKTOP_CHECK_DECLARATIONS.filter(check=>!required.has(check.id))
    .map(check=>check.id);
}

export const backgroundOnlyRequiredChecks=Object.freeze(checksForMode('background-only'));
export const backgroundOnlyOmittedChecks=Object.freeze(omittedForMode('background-only'));
export const inputOnlyRequiredChecks=Object.freeze(checksForMode('input-only'));
export const inputOnlyOmittedChecks=Object.freeze(omittedForMode('input-only'));
export const touchOnlyRequiredChecks=Object.freeze(checksForMode('touch-only'));
export const touchOnlyOmittedChecks=Object.freeze(omittedForMode('touch-only'));

export function desktopCheckAdaptationIds(id,{game}={}){
  const declaration=DESKTOP_CHECK_BY_ID.get(id);
  if(!declaration)throw new RangeError(`Unknown desktop check ${String(id)}`);
  return [...(declaration.adaptationIdsByGame?.[game]??declaration.adaptationIds)];
}

export function desktopAdaptationIdsForMode(mode,{prerequisites=false,...options}={}){
  if(!DESKTOP_SELECTIVE_MODES.includes(mode))throw new RangeError(`Unknown desktop check scope ${String(mode)}`);
  const ids=[];
  for(const checkId of checksForMode(mode).filter(id=>(DESKTOP_CHECK_BY_ID.get(id).role==='prerequisite')===prerequisites))
    for(const adaptationId of desktopCheckAdaptationIds(checkId,options))
    if(!ids.includes(adaptationId))ids.push(adaptationId);
  return ids;
}

export function selectDesktopExecutionMode({backgroundOnly=false,inputOnly=false,touchOnly=false,actionsOnly=false,replayRecordingOnly=false,audioLatencyOnly=false}={}){
  const selected=[backgroundOnly&&'background-only',inputOnly&&'input-only',touchOnly&&'touch-only',actionsOnly&&'actions-only',replayRecordingOnly&&'replay-recording-only',audioLatencyOnly&&'audio-latency-only'].filter(Boolean);
  if(selected.length>1)throw new Error(`${selected.map(mode=>`--${mode}`).join(' and ')} are mutually exclusive.`);
  return selected[0]??'full-desktop';
}

function baseDesktopCheckScope(mode){
  if(mode==='background-only')return{requiredIds:[...backgroundOnlyRequiredChecks],omittedIds:[...backgroundOnlyOmittedChecks]};
  if(mode==='input-only')return{requiredIds:[...inputOnlyRequiredChecks],omittedIds:[...inputOnlyOmittedChecks]};
  if(mode==='touch-only')return{requiredIds:[...touchOnlyRequiredChecks],omittedIds:[...touchOnlyOmittedChecks]};
  if(mode==='audio-latency-only'||mode==='actions-only'||mode==='replay-recording-only')return{requiredIds:checksForMode(mode),omittedIds:omittedForMode(mode)};
  if(mode==='full-desktop')return{requiredIds:null,omittedIds:[]};
  throw new RangeError(`Unknown desktop check scope ${String(mode)}`);
}

/** Select assertions inside an existing scene group while retaining artifact gates. */
export function desktopCheckScope(mode,{adaptationIds=[],game}={}){
  const base=baseDesktopCheckScope(mode);
  if(!Array.isArray(adaptationIds))throw Error('Select distinct adaptation check IDs');
  if(!adaptationIds.length)return base;
  if(base.requiredIds===null)throw Error('Numbered check selection requires an explicit desktop scene mode');
  if(adaptationIds.some(id=>!/^([A-Z]+-)+\d+$/.test(id))||new Set(adaptationIds).size!==adaptationIds.length)
    throw Error('Select distinct adaptation check IDs');
  const supported=new Set(base.requiredIds.flatMap(id=>desktopCheckAdaptationIds(id,{game})));
  for(const id of adaptationIds)if(!supported.has(id))throw Error(`${id} is outside the ${mode} scene group`);
  const selected=new Set(adaptationIds);
  const requiredIds=base.requiredIds.filter(id=>id==='package-and-private-data-identity'||id==='package-and-private-data-identity-postflight'||
    desktopCheckAdaptationIds(id,{game}).some(check=>selected.has(check)));
  const required=new Set(requiredIds);
  return{requiredIds,omittedIds:DESKTOP_CHECK_DECLARATIONS.filter(check=>!required.has(check.id)).map(check=>check.id)};
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
/** Read-only journal of the selected parent's actual touch-control serial stream. */
export function installTouchControlSerialObserver({game,epoch}){
  const key='__eaglerAcceptanceTouchSerials',prior=globalThis[key];
  if(prior){if(prior.game!==game||prior.epoch!==epoch)throw Error('Touch serial observer belongs to another Runtime');return prior;}
  const receipt={game,epoch,bombSerial:0,escapeSerial:0,observations:0,source:'parent touch-controls MessageEvent'};
  globalThis[key]=receipt;
  addEventListener('message',event=>{
    const value=event.data;
    if(event.source!==parent||event.origin!==location.origin||value?.protocol!=='eagler-touhou/1'||
      value.game!==game||value.epoch!==epoch||value.command!=='touch-controls')return;
    for(const field of ['bombSerial','escapeSerial'])if(!Number.isSafeInteger(value[field])||value[field]<0||value[field]>0xffffffff)return;
    receipt.bombSerial=value.bombSerial;receipt.escapeSerial=value.escapeSerial;receipt.observations++;
  });
  return receipt;
}

/** Neutral controls preserve edge-stream counters; zero is a new edge after a nonzero serial. */
export function neutralTouchControlsFromReceipt(receipt,{game,epoch}){
  if(receipt?.game!==game||receipt?.epoch!==epoch||!Number.isSafeInteger(receipt.bombSerial)||receipt.bombSerial<0||
    !Number.isSafeInteger(receipt.escapeSerial)||receipt.escapeSerial<0)throw Error('Current Runtime touch serial receipt is missing');
  return{fireEnabled:false,focusEnabled:false,bombSerial:receipt.bombSerial,escapeSerial:receipt.escapeSerial,
    joystickX:0,joystickY:0,touchSensitivity:100};
}

/** Keep startup on its verified native page; background scenarios run afterward. */
export async function runNativeForegroundOperation({operation,readState,restore,pause,evidence,maxRestores=2}){
  let done=false;const outcome=Promise.resolve().then(operation).finally(()=>{done=true;});
  const monitor=(async()=>{
    while(!done){
      const state=await readState();evidence.samples.push(state);
      if(!done&&!state.ready){
        if(state.owned!==true)throw Error('Startup no longer owns its original native page and window');
        if(evidence.restores.length>=maxRestores)throw Error('Native foreground repeatedly changed during startup');
        const receipt=await restore();evidence.restores.push(receipt);
        if(receipt?.ready!==true)throw Error('The owned startup window could not regain native foreground');
      }
      if(!done)await pause();
    }
  })();
  try{return await Promise.race([outcome,monitor.then(()=>outcome)]);}
  finally{done=true;await monitor.catch(()=>{});}
}
