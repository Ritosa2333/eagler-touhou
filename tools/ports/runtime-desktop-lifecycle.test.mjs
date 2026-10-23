import assert from 'node:assert/strict';
import test from 'node:test';
import {inspectDesktopInputCleanup,inspectDesktopTerminalErrors} from './runtime-desktop-lifecycle.mjs';
import {backgroundOnlyOmittedChecks,backgroundOnlyRequiredChecks,classifyAudioLifecyclePath,compareNativeBrowserTargets,desktopAdaptationIdsForMode,desktopCheckAdaptationIds,desktopCheckScope,desktopExecutionBrowserMode,inputOnlyOmittedChecks,inputOnlyRequiredChecks,inspectAudioContextsBeforeGesture,inspectAudioGestureWindow,inspectMinimizedKeyboardClear,inspectNativeBackgroundEvents,inspectNativeFireStimulus,inspectNativeForegroundIdentity,inspectNativePageFocus,inspectPostCleanupForeground,inspectViewportScreenshotClip,selectDesktopExecutionMode,selectNativePageTarget,summarizeDesktopCheckScope} from './runtime-desktop-lifecycle.mjs';

test('input cleanup requires every live Runtime acknowledgement and preserves cleanup errors',()=>{
  const result={liveRuntime:true,keyboardCleared:true,touchCancelAcknowledged:true,touchControlsAcknowledged:true,sendErrors:[]};
  assert.equal(inspectDesktopInputCleanup({required:true,result}).status,'passed');
  for(const field of ['liveRuntime','keyboardCleared','touchCancelAcknowledged','touchControlsAcknowledged'])
    assert.equal(inspectDesktopInputCleanup({required:true,result:{...result,[field]:false}}).status,'failed',field);
  assert.equal(inspectDesktopInputCleanup({required:true,result:{...result,sendErrors:['touch-cancel rejected']}}).status,'failed');
  assert.equal(inspectDesktopInputCleanup({required:false}).status,'not-required','A Runtime not yet ready has no release claim');
  assert.equal(inspectDesktopInputCleanup({required:false,keyErrors:['KeyZ release failed']}).status,'failed');
  assert.equal(inspectDesktopInputCleanup({required:false,evaluationError:'Host page closed'}).status,'failed');
});

test('native lifecycle requires a real visible focused document before actions',()=>{
  assert.deepEqual(inspectNativePageFocus({visibilityState:'visible',hidden:false,hasFocus:true}),{
    ready:true,missing:[],state:{visibilityState:'visible',hidden:false,hasFocus:true}});
  const hidden=inspectNativePageFocus({visibilityState:'hidden',hidden:true,hasFocus:false});
  assert.equal(hidden.ready,false);assert.equal(hidden.missing.length,3);
  const missing=inspectNativePageFocus(null);
  assert.equal(missing.ready,false);assert.equal(missing.state,null);
});

test('headed foreground restoration keeps the same page and window and observes normal state',()=>{
  const before={status:'matched',target:{type:'page',targetId:'game'},window:{windowId:17,currentBounds:{windowState:'minimized'}}};
  const restored={status:'matched',target:{type:'page',targetId:'game'},window:{windowId:17,currentBounds:{windowState:'normal'}}};
  const result=inspectNativeForegroundIdentity({expected:before,observed:restored});
  assert.equal(result.ready,true);assert.equal(result.sameTarget,true);assert.equal(result.sameWindow,true);assert.equal(result.windowState,'normal');
  assert.equal(inspectNativeForegroundIdentity({expected:before,observed:{...restored,target:{type:'page',targetId:'other'}}}).ready,false);
  assert.equal(inspectNativeForegroundIdentity({expected:before,observed:{...restored,window:{windowId:18,currentBounds:{windowState:'normal'}}}}).ready,false);
  assert.equal(inspectNativeForegroundIdentity({expected:before,observed:{...restored,window:{windowId:17,currentBounds:{windowState:'minimized'}}}}).ready,false);
  assert.equal(inspectNativeForegroundIdentity({expected:before,observed:{status:'incomplete'}}).ready,false);
});

test('software input probes use headless Chromium and desktop background stays headed',()=>{
  assert.equal(desktopExecutionBrowserMode('input-only'),'headless');
  assert.equal(desktopExecutionBrowserMode('touch-only'),'headless');
  assert.equal(desktopExecutionBrowserMode('audio-latency-only'),'headless');
  assert.equal(desktopExecutionBrowserMode('background-only'),'headed');
  assert.equal(desktopExecutionBrowserMode('full-desktop'),'headed');
  assert.throws(()=>desktopExecutionBrowserMode('unknown'),/Unknown desktop execution mode/);
});

test('terminal error audit rejects errors arriving after the last functional check',()=>{
  const observations={errors:[],events:[],httpErrors:[],fixtureBase:'https://127.0.0.1:1234/'};
  assert.equal(inspectDesktopTerminalErrors(observations).ready,true);
  observations.events.push({protocol:'eagler-touhou/1',event:'error',message:'cleanup failed'});
  assert.equal(inspectDesktopTerminalErrors(observations).ready,false);
  observations.events=[];observations.errors.push({page:'late page exception'});
  assert.equal(inspectDesktopTerminalErrors(observations).ready,false);
  observations.errors=[];observations.httpErrors.push({url:'https://127.0.0.1:1234/late.wasm',status:404});
  assert.equal(inspectDesktopTerminalErrors(observations).ready,false);
});

test('iframe screenshot clips use only finite bounds fully inside the viewport',()=>{
  const valid=inspectViewportScreenshotClip({bounds:{x:0,y:10,width:960,height:720},viewport:{width:1100,height:820}});
  assert.equal(valid.ready,true);assert.deepEqual(valid.clip,{x:0,y:10,width:960,height:720});
  for(const bounds of [null,{x:0,y:0,width:0,height:10},{x:-1,y:0,width:10,height:10},
    {x:0,y:0,width:11,height:10},{x:0,y:0,width:10,height:10.1},{x:NaN,y:0,width:10,height:10}])
    assert.equal(inspectViewportScreenshotClip({bounds,viewport:{width:10,height:10}}).ready,false);
  assert.equal(inspectViewportScreenshotClip({bounds:{x:0,y:0,width:10,height:10},viewport:{width:0,height:10}}).ready,false);
});

test('post-cleanup foreground evidence binds both documents to the original page and window',()=>{
  const page={status:'matched',target:{type:'page',targetId:'game'},window:{windowId:17}};
  const visible={visibilityState:'visible',hidden:false,hasFocus:true};
  const ready=inspectPostCleanupForeground({hostState:visible,runtimeState:visible,beforePage:page,afterPage:{...page},backgroundPageCloseStatus:'closed'});
  assert.equal(ready.ready,true);assert.equal(ready.pageIdentity.sameTarget,true);assert.equal(ready.pageIdentity.sameWindow,true);
  const hidden=inspectPostCleanupForeground({hostState:visible,runtimeState:{...visible,hidden:true},beforePage:page,afterPage:{...page},backgroundPageCloseStatus:'closed'});
  assert.equal(hidden.ready,false);assert.ok(hidden.missing.some(item=>item.includes('Runtime document')));
  const moved=inspectPostCleanupForeground({hostState:visible,runtimeState:visible,beforePage:page,
    afterPage:{...page,target:{...page.target,targetId:'other'},window:{windowId:18}},backgroundPageCloseStatus:'closed'});
  assert.equal(moved.ready,false);assert.equal(moved.pageIdentity.sameTarget,false);assert.equal(moved.pageIdentity.sameWindow,false);
  const notClosed=inspectPostCleanupForeground({hostState:visible,runtimeState:visible,beforePage:page,afterPage:{...page},backgroundPageCloseStatus:'failed'});
  assert.equal(notClosed.ready,false);assert.equal(notClosed.temporaryPageClosed,false);
});

test('native fire stimulus requires accepted Host input and a non-neutral Runtime input sample',()=>{
  const events=[{code:'KeyZ',down:true,accepted:true,epoch:2}];
  const valid=inspectNativeFireStimulus({forwardEvents:events,epoch:2,neutralInputValue:0,heldInputValue:1});
  assert.equal(valid.ready,true);assert.equal(valid.nativeFireConsumed,true);
  assert.equal(inspectNativeFireStimulus({forwardEvents:[{...events[0],accepted:false}],epoch:2,neutralInputValue:0,heldInputValue:1}).ready,false);
  assert.equal(inspectNativeFireStimulus({forwardEvents:events,epoch:2,neutralInputValue:0,heldInputValue:0}).ready,false);
  assert.equal(inspectNativeFireStimulus({forwardEvents:events,epoch:1,neutralInputValue:0,heldInputValue:1}).ready,false);
});

test('background evidence requires a browser lifecycle observation',()=>{
  assert.deepEqual(inspectNativeBackgroundEvents({state:{visibilityState:'hidden',hidden:true},events:[]}),{
    visibilityHidden:true,visibilityChange:false,blur:false,observed:true});
  assert.equal(inspectNativeBackgroundEvents({state:{visibilityState:'visible',hidden:false},events:[]}).observed,false);
  assert.equal(inspectNativeBackgroundEvents({events:[{type:'visibilitychange',hidden:true}]}).visibilityChange,true);
  assert.equal(inspectNativeBackgroundEvents({events:[{type:'blur'}]}).blur,true);
});

test('minimized release requires accepted Host clear and neutral native input during the hidden interval',()=>{
  const state={visibilityState:'hidden',hidden:true,hasFocus:false};
  const lifecycle=[{type:'visibilitychange',hidden:true,epoch:2}];
  const host=[{reason:'host-hidden',accepted:true,time:120,epoch:2}];
  const valid=inspectMinimizedKeyboardClear({windowState:'minimized',nativeState:state,events:lifecycle,hostEvents:host,
    hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:0,neutralInputValue:0});
  assert.equal(valid.status,'passed');assert.equal(valid.acceptedClear.time,120);
  const hostDidNotClear=inspectMinimizedKeyboardClear({windowState:'minimized',nativeState:state,events:lifecycle,
    hostEvents:[{...host[0],accepted:false}],hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:0,neutralInputValue:0});
  assert.equal(hostDidNotClear.status,'incomplete');assert.equal(hostDidNotClear.hostClearAccepted,false);
  const clearOnlyAfterRestore=inspectMinimizedKeyboardClear({windowState:'minimized',nativeState:state,events:lifecycle,
    hostEvents:[{...host[0],time:170}],hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:0,neutralInputValue:0});
  assert.equal(clearOnlyAfterRestore.status,'incomplete');
  const nativeNotNeutral=inspectMinimizedKeyboardClear({windowState:'minimized',nativeState:state,events:lifecycle,hostEvents:host,
    hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:1,neutralInputValue:0});
  assert.equal(nativeNotNeutral.status,'failed');assert.equal(nativeNotNeutral.nativeNeutral,false);
  const nativeNotObserved=inspectMinimizedKeyboardClear({windowState:'minimized',nativeState:state,events:lifecycle,hostEvents:host,
    hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:null,neutralInputValue:0});
  assert.equal(nativeNotObserved.status,'incomplete');assert.equal(nativeNotObserved.inputObserved,false);
  assert.match(nativeNotObserved.missing[0],/could not be observed/);
  assert.equal(inspectMinimizedKeyboardClear({windowState:'normal',nativeState:state,events:lifecycle,hostEvents:host,
    hiddenStartAtMs:100,hiddenSampledAtMs:150,inputValue:0,neutralInputValue:0}).status,'incomplete');
});

test('AUDIO-04 requires a running output AudioContext before the fire gesture',()=>{
  assert.deepEqual(inspectAudioContextsBeforeGesture([{state:'running'}]),{ready:true,contextCount:1,states:['running'],reason:null});
  assert.equal(inspectAudioContextsBeforeGesture([]).ready,false);
  assert.equal(inspectAudioContextsBeforeGesture([{state:'suspended'}]).ready,false);
  assert.equal(inspectAudioContextsBeforeGesture([{state:'running'},{state:'suspended'}]).ready,false);
});

test('background audio recovery path is classified separately from the foreground gesture',()=>{
  const resumed=classifyAudioLifecyclePath({beforeContexts:[{state:'running'}],afterContexts:[{state:'running'}],
    stateEvents:[{state:'suspended'},{state:'running'}]});
  assert.equal(resumed.path,'suspended-resumed');assert.equal(resumed.resumedAfterSuspend,true);
  const continuous=classifyAudioLifecyclePath({beforeContexts:[{state:'running'}],afterContexts:[{state:'running'}],stateEvents:[]});
  assert.equal(continuous.path,'continuous-running');
  const gestureOnly=classifyAudioLifecyclePath({beforeContexts:[{state:'suspended'}],afterContexts:[{state:'running'}],
    stateEvents:[{state:'running'}]});
  assert.equal(gestureOnly.path,'context-state-transition-unclassified');
  assert.equal(inspectAudioContextsBeforeGesture([{state:'suspended'}]).ready,false);
});

test('AUDIO-04 rejects suspension or resume between the pre-key snapshot and PCM capture',()=>{
  const ready=inspectAudioGestureWindow({contextsBeforeGesture:[{state:'running'}],capturedContextStates:['running','running'],stateEvents:[]});
  assert.equal(ready.ready,true);
  const resumedByGesture=inspectAudioGestureWindow({contextsBeforeGesture:[{state:'running'}],
    capturedContextStates:['suspended','running'],stateEvents:[{state:'suspended'},{state:'running'}]});
  assert.equal(resumedByGesture.ready,false);assert.equal(resumedByGesture.stateStable,false);
  const stoppedBeforeGesture=inspectAudioGestureWindow({contextsBeforeGesture:[{state:'suspended'}],
    capturedContextStates:['running'],stateEvents:[{state:'running'}]});
  assert.equal(stoppedBeforeGesture.ready,false);assert.ok(stoppedBeforeGesture.missing.some(item=>item.includes('already be running before KeyZ')));
});

test('same-window tab lifecycle compares real page targets and window IDs',()=>{
  const pageTarget={type:'page',targetId:'page-a'},otherTarget={type:'page',targetId:'page-b'};
  assert.deepEqual(compareNativeBrowserTargets({pageTarget,pageWindowId:10,otherTarget,otherWindowId:10}),{
    status:'matched',sameWindow:true,pageTargetId:'page-a',otherTargetId:'page-b',pageWindowId:10,otherWindowId:10});
  assert.equal(compareNativeBrowserTargets({pageTarget,pageWindowId:10,otherTarget,otherWindowId:11}).status,'different-window');
  assert.equal(compareNativeBrowserTargets({pageTarget,pageWindowId:10,otherTarget:{type:'service_worker',targetId:'sw'},otherWindowId:10}).status,'incomplete');
});

test('native target lookup must be unambiguous and use actual page targets',()=>{
  const page={targetId:'game',type:'page',url:'https://fixture.test/game',title:'game'};
  assert.deepEqual(selectNativePageTarget({targetInfos:[page,{targetId:'worker',type:'worker',url:page.url}],url:page.url}),{
    status:'matched',target:page,matchCount:1});
  assert.equal(selectNativePageTarget({targetInfos:[],url:page.url}).status,'incomplete');
  assert.equal(selectNativePageTarget({targetInfos:[page,{...page,targetId:'other'}],url:page.url}).status,'incomplete');
  assert.equal(selectNativePageTarget({targetInfos:[{...page,targetId:''}],url:page.url}).status,'incomplete');
});

test('background-only mode requires identity and exactly its three lifecycle checks',()=>{
  assert.deepEqual(backgroundOnlyRequiredChecks,[
    'package-and-private-data-identity','window-minimize-foreground-and-release','background-tab-foreground-release',
    'background-audio-recovery','package-and-private-data-identity-postflight',
  ]);
  assert.ok(backgroundOnlyOmittedChecks.includes('shared-touch-limited'));
  assert.ok(backgroundOnlyOmittedChecks.includes('shared-touch-unlimited'));
  assert.ok(!desktopAdaptationIdsForMode('background-only').some(id=>id.startsWith('TOUCH-')),
    'Background checks observe keyboard release and audio recovery, not a touch lifecycle');
});

test('background-only scope never counts omitted checks as passes',()=>{
  const passed=backgroundOnlyRequiredChecks.map(id=>({id,status:'passed'}));
  assert.equal(summarizeDesktopCheckScope(passed,{requiredIds:backgroundOnlyRequiredChecks,omittedIds:backgroundOnlyOmittedChecks}).status,'passed');
  const missing=summarizeDesktopCheckScope(passed.slice(1),{requiredIds:backgroundOnlyRequiredChecks,omittedIds:backgroundOnlyOmittedChecks});
  assert.equal(missing.status,'incomplete');assert.equal(missing.missingRequired[0],backgroundOnlyRequiredChecks[0]);
  const accidentalPass=summarizeDesktopCheckScope([...passed,{id:backgroundOnlyOmittedChecks[0],status:'passed'}],
    {requiredIds:backgroundOnlyRequiredChecks,omittedIds:backgroundOnlyOmittedChecks});
  assert.equal(accidentalPass.status,'failed');assert.equal(accidentalPass.unexpectedChecks[0].omitted,true);
  const duplicate=summarizeDesktopCheckScope([...passed,passed[0]],{requiredIds:backgroundOnlyRequiredChecks,omittedIds:backgroundOnlyOmittedChecks});
  assert.equal(duplicate.status,'failed');assert.deepEqual(duplicate.duplicateRequired,[backgroundOnlyRequiredChecks[0]]);
});

test('input-only scope retains all input, pause, new-epoch, and foreground SFX checks',()=>{
  for(const id of ['window-minimize-foreground-and-release','background-tab-foreground-release','background-audio-recovery'])
    assert.ok(inputOnlyOmittedChecks.includes(id));
  assert.ok(inputOnlyRequiredChecks.includes('audio-output-with-music-disabled'));
  assert.ok(inputOnlyRequiredChecks.includes('native-pause-state-and-freeze'));
  assert.ok(inputOnlyRequiredChecks.includes('iframe-exit-and-new-epoch'));
  for(const id of ['shared-touch-limited','shared-touch-unlimited','shared-touch-joystick-free',
    'shared-touch-limited-restored','shared-touch-two-finger-focus'])assert.ok(inputOnlyRequiredChecks.includes(id),`${id} remains in the reusable input-only scene group`);
  const scope=desktopCheckScope('input-only');
  assert.deepEqual(scope.requiredIds,inputOnlyRequiredChecks);
  assert.deepEqual(scope.omittedIds,inputOnlyOmittedChecks);
  assert.ok(inputOnlyRequiredChecks.includes('runtime-epoch-touch-mode-joystick'));
  for(const id of ['runtime-epoch-touch-mode-touch-unlimited','runtime-epoch-touch-mode-joystick-free'])
    assert.ok(inputOnlyOmittedChecks.includes(id),'A mode probe excluded from required checks must be explicitly omitted');
  assert.equal(desktopCheckScope('full-desktop').requiredIds,null);
  assert.throws(()=>desktopCheckScope('unknown'),/Unknown desktop check scope/);
  assert.equal(selectDesktopExecutionMode({}),'full-desktop');
  assert.equal(selectDesktopExecutionMode({inputOnly:true}),'input-only');
  assert.equal(selectDesktopExecutionMode({backgroundOnly:true}),'background-only');
  assert.throws(()=>selectDesktopExecutionMode({backgroundOnly:true,inputOnly:true}),/mutually exclusive/);
});

test('audio latency mode requires actual PCM timing and its live negative control without gameplay checks',()=>{
  assert.equal(selectDesktopExecutionMode({audioLatencyOnly:true}),'audio-latency-only');
  for(const flags of [{inputOnly:true},{touchOnly:true},{backgroundOnly:true}])
    assert.throws(()=>selectDesktopExecutionMode({...flags,audioLatencyOnly:true}),/mutually exclusive/);
  const scope=desktopCheckScope('audio-latency-only');
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','native-sfx-before-mixed-queue-tail',
    'negative-control-sfx-delayed-to-mixed-queue-tail','package-and-private-data-identity-postflight']);
  assert.ok(scope.omittedIds.includes('shared-touch-limited'));
  assert.deepEqual(desktopAdaptationIdsForMode('audio-latency-only'),['BUILD-04','PACKAGE-02','AUDIO-02']);
  const checks=scope.requiredIds.map(id=>({id,status:'passed'}));
  assert.equal(summarizeDesktopCheckScope(checks,scope).status,'passed');
  assert.equal(summarizeDesktopCheckScope(checks.filter(item=>!item.id.startsWith('negative-control-')),scope).status,'incomplete');
  assert.equal(summarizeDesktopCheckScope(checks.map(item=>item.id.startsWith('negative-control-')?{...item,status:'failed'}:item),scope).status,'failed');
});

test('desktop check declarations own both adaptation IDs and per-mode eligibility',()=>{
  assert.deepEqual(desktopCheckAdaptationIds('shared-touch-limited'),['TOUCH-01','TOUCH-05','TOUCH-14','TOUCH-16']);
  assert.deepEqual(desktopCheckAdaptationIds('runtime-epoch-touch-mode-joystick'),['RUNTIME-01','TOUCH-03']);
  assert.deepEqual(desktopCheckAdaptationIds('touch-fire-focus-and-mixed-input',{game:'th09'}),[]);
  const inputIds=desktopAdaptationIdsForMode('input-only',{game:'th06'});
  assert.ok(inputIds.includes('TOUCH-01')&&inputIds.includes('TOUCH-02')&&inputIds.includes('TOUCH-04'));
  assert.ok(inputIds.includes('TOUCH-03'),'The directional joystick mode probe is required in input-only');
  const th09InputIds=desktopAdaptationIdsForMode('input-only',{game:'th09'});
  assert.ok(!th09InputIds.includes('INPUT-05'),'TH09 fire sampling does not test mixed input ownership');
  assert.throws(()=>desktopCheckAdaptationIds('unknown-check'),/Unknown desktop check/);
});

test('touch-only runs the shared movement contract with identity gates and excludes unrelated scenes',()=>{
  assert.equal(selectDesktopExecutionMode({touchOnly:true}),'touch-only');
  for(const flags of [{touchOnly:true,inputOnly:true},{touchOnly:true,backgroundOnly:true}])
    assert.throws(()=>selectDesktopExecutionMode(flags),/mutually exclusive/);
  const scope=desktopCheckScope('touch-only');
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','shared-touch-limited',
    'shared-touch-unlimited','shared-touch-joystick-free','shared-touch-limited-restored','shared-touch-two-finger-focus','runtime-epoch-touch-mode-joystick','package-and-private-data-identity-postflight']);
  assert.ok(scope.omittedIds.includes('audio-output-with-music-disabled'));
  assert.equal(summarizeDesktopCheckScope(scope.requiredIds.map(id=>({id,status:'passed'})),scope).status,'passed');
  assert.equal(summarizeDesktopCheckScope(scope.requiredIds.filter(id=>id!=='shared-touch-limited').map(id=>({id,status:'passed'})),scope).status,'incomplete');
});

test('numbered selection runs the matching native scene and both identity gates',()=>{
  const scope=desktopCheckScope('input-only',{adaptationIds:['TOUCH-03'],game:'th08'});
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','runtime-epoch-touch-mode-joystick','package-and-private-data-identity-postflight']);
  assert.ok(scope.omittedIds.includes('shared-touch-limited'));
  assert.ok(scope.omittedIds.includes('audio-output-with-music-disabled'));
  assert.equal(summarizeDesktopCheckScope(scope.requiredIds.slice(1).map(id=>({id,status:'passed'})),scope).status,'incomplete');
  assert.throws(()=>desktopCheckScope('input-only',{adaptationIds:['TOUCH-06'],game:'th08'}),/outside/);
  assert.throws(()=>desktopCheckScope('input-only',{adaptationIds:['TOUCH-03','TOUCH-03']}),/distinct/);
});

test('action-only scope requires native action effects and both identity gates',()=>{
  assert.equal(selectDesktopExecutionMode({actionsOnly:true}),'actions-only');
  assert.equal(desktopExecutionBrowserMode('actions-only'),'headless');
  assert.throws(()=>selectDesktopExecutionMode({actionsOnly:true,inputOnly:true}),/mutually exclusive/);
  const scope=desktopCheckScope('actions-only');
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','launcher-touch-native-consumers','shared-touch-restart','shared-touch-actions',
    'shared-touch-double-tap-bomb','shared-touch-deathbomb','shared-touch-dialogue-confirm','shared-touch-unlimited-result',
    'shared-touch-menu-dialogue-gestures','shared-launcher-touch-layout','shared-launcher-touch-magnifier',
    'package-and-private-data-identity-postflight']);
  const identities=scope.requiredIds.filter(id=>!['shared-touch-actions','shared-touch-restart'].includes(id))
    .map(id=>({id,status:'passed'}));
  assert.equal(summarizeDesktopCheckScope(identities,scope).status,'incomplete');
  assert.equal(summarizeDesktopCheckScope([...identities,{id:'shared-touch-actions',status:'failed'}],scope).status,'failed');
  const touchRestart=desktopCheckScope('actions-only',{adaptationIds:['TOUCH-18'],game:'th08'});
  assert.ok(touchRestart.requiredIds.includes('shared-touch-restart'));
  assert.ok(!touchRestart.requiredIds.includes('shared-touch-actions'));
});

test('finite Replay mode owns recording and isolation while excluding movement and audio scenes',()=>{
  assert.equal(selectDesktopExecutionMode({replayRecordingOnly:true}),'replay-recording-only');
  assert.equal(desktopExecutionBrowserMode('replay-recording-only'),'headless');
  const scope=desktopCheckScope('replay-recording-only');
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','shared-replay-recording-and-isolation','package-and-private-data-identity-postflight']);
  assert.deepEqual(desktopAdaptationIdsForMode('replay-recording-only',{game:'th08'}),
    ['BUILD-04','PACKAGE-02','REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06']);
  assert.ok(scope.omittedIds.includes('shared-touch-limited'));
});

test('input-only omitted background checks remain unexpected and cannot count toward required results',()=>{
  const input= inputOnlyRequiredChecks.map(id=>({id,status:'passed'}));
  assert.equal(summarizeDesktopCheckScope(input,{requiredIds:inputOnlyRequiredChecks,omittedIds:inputOnlyOmittedChecks}).status,'passed');
  const withBackground=[...input,{id:'background-audio-recovery',status:'passed'}];
  const summary=summarizeDesktopCheckScope(withBackground,{requiredIds:inputOnlyRequiredChecks,omittedIds:inputOnlyOmittedChecks});
  assert.equal(summary.status,'failed');
  assert.deepEqual(summary.unexpectedChecks,[{id:'background-audio-recovery',status:'passed',omitted:true}]);
  const missing=summarizeDesktopCheckScope(input.slice(1),{requiredIds:inputOnlyRequiredChecks,omittedIds:inputOnlyOmittedChecks});
  assert.equal(missing.status,'incomplete');
  assert.equal(missing.missingRequired[0],'package-and-private-data-identity');
});
