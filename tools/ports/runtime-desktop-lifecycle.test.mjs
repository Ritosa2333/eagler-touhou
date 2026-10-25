import assert from 'node:assert/strict';
import test from 'node:test';
import {restoreNativeForeground} from './runtime-desktop-lifecycle.mjs';
import {inspectDesktopInputCleanup,inspectDesktopTerminalErrors} from './runtime-desktop-lifecycle.mjs';
import {inspectResumedKeyboardMovement,shouldAssessResumedMovement} from './runtime-desktop-lifecycle.mjs';

test('window restore rejects residual movement even when both published input snapshots are neutral',()=>{
  const hidden={capabilities:{restartFrame:'enemy'},frames:{enemy:10},status:{stage:0},input:{value:0},position:{x:212,y:384}};
  const restored={...hidden,frames:{enemy:11}};
  const inspect=(state=restored)=>inspectResumedKeyboardMovement({hidden,restored:state,neutralInputValue:0});
  assert.equal(inspect().status,'passed');
  assert.equal(inspect({...restored,position:{x:216,y:384}}).status,'failed');
  assert.equal(inspect({...restored,input:{value:128}}).status,'failed');
  assert.equal(inspect({...restored,frames:{enemy:10}}).status,'incomplete');
  assert.equal(inspect({...restored,status:{stage:1}}).status,'incomplete');
  assert.equal(inspect({...restored,position:null}).status,'incomplete');
  assert.equal(shouldAssessResumedMovement(hidden,hidden),false);
  assert.equal(shouldAssessResumedMovement(hidden,restored),true);
  const nextStage={...restored,status:{stage:1},frames:{enemy:0}};
  assert.equal(shouldAssessResumedMovement(hidden,nextStage),true,'a reset stage clock must reach assessment before timeout');
  assert.equal(inspect(nextStage).status,'incomplete');
  assert.equal(shouldAssessResumedMovement(hidden,{...restored,frames:{}}),true);
});
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

test('foreground restoration waits for the owned native window and both real document focus states',async()=>{
  let time=0,read=0,restores=0;
  const page=state=>({status:'matched',target:{type:'page',targetId:'owned'},window:{windowId:12,currentBounds:{windowState:state}}});
  const focused={visibilityState:'visible',hidden:false,hasFocus:true};
  const io={expected:page('normal'),readIdentity:async()=>page(++read<4?'minimized':'normal'),
    restoreWindow:async id=>{assert.equal(id,12);restores++;},activatePage:async id=>assert.equal(id,'owned'),
    readHost:async()=>focused,readRuntime:async()=>focused,wait:async ms=>{time+=ms;},now:()=>time,timeoutMs:1000};
  const result=await restoreNativeForeground(io);
  assert.equal(result.ready,true);assert.ok(restores>0);assert.ok(result.samples.length>=3);
  assert.equal(result.samples.at(-1).identityCheck.windowState,'normal');
  assert.equal(result.samples.at(-2).identityCheck.windowState,'normal','A transient focus sample is not enough');
  read=0;time=0;
  const hidden=await restoreNativeForeground({...io,readRuntime:async()=>({...focused,hasFocus:false})});
  assert.equal(hidden.ready,false);assert.match(hidden.reason,/deadline/);
  restores=0;
  const changed=await restoreNativeForeground({...io,readIdentity:async()=>({...page('normal'),target:{type:'page',targetId:'unrelated'}})});
  assert.equal(changed.ready,false);assert.equal(restores,0,'Never restore a different page/window');
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

test('background-only mode requires native window, pointer cancellation, gamepad reconnect and audio evidence',()=>{
  assert.deepEqual(backgroundOnlyRequiredChecks,[
    'package-and-private-data-identity','window-minimize-foreground-and-release','touch-pointer-cancel-and-release',
    'gamepad-disconnect-and-reconnect','background-input-lifecycle-contract','background-tab-foreground-release',
    'background-audio-recovery','package-and-private-data-identity-postflight',
  ]);
  assert.ok(backgroundOnlyOmittedChecks.includes('shared-touch-limited'));
  assert.ok(backgroundOnlyOmittedChecks.includes('shared-touch-unlimited'));
  assert.ok(desktopAdaptationIdsForMode('background-only').includes('TOUCH-16'));
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

test('audio latency mode requires PCM timing, live negative control and native gameplay SFX',()=>{
  assert.equal(selectDesktopExecutionMode({audioLatencyOnly:true}),'audio-latency-only');
  for(const flags of [{inputOnly:true},{touchOnly:true},{backgroundOnly:true}])
    assert.throws(()=>selectDesktopExecutionMode({...flags,audioLatencyOnly:true}),/mutually exclusive/);
  const scope=desktopCheckScope('audio-latency-only');
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','audio-output-with-music-disabled','audio-none-contract','native-sfx-before-mixed-queue-tail',
    'negative-control-sfx-delayed-to-mixed-queue-tail','package-and-private-data-identity-postflight']);
  assert.ok(scope.omittedIds.includes('shared-touch-limited'));
  assert.deepEqual(desktopAdaptationIdsForMode('audio-latency-only'),['AUDIO-02']);
  assert.deepEqual(desktopAdaptationIdsForMode('audio-latency-only',{prerequisites:true}),['BUILD-04','PACKAGE-02']);
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
  assert.deepEqual(scope.requiredIds,['package-and-private-data-identity','launcher-touch-native-consumers','shared-launcher-touch-focus','shared-touch-restart','shared-touch-actions',
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
    ['REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06']);
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

// Exercise the production closure with native I/O substituted | no browser launch.
test('normal focused window focuses only its canvas and requires two stable observations',async()=>{
  const {readFile}=await import('node:fs/promises');
  const source=await readFile(new URL('./runtime-desktop-probes.mjs',import.meta.url),'utf8');
  const start=source.indexOf('const restoreForeground=async');
  const end=source.indexOf('const acquireForeground=',start);
  const declaration=source.slice(start,end);
  const identity={status:'matched',target:{type:'page',targetId:'owned'},window:{windowId:12,currentBounds:{windowState:'normal'}}};
  const visible={visibilityState:'visible',hidden:false,hasFocus:true};
  let focused=0,waits=0,native=0,activated=0,fallback=0,reads=0;
  const frame={locator:()=>({focus:async()=>{focused++;}})};
  const page={waitForTimeout:async()=>{waits++;},title:async()=> 'fixture',bringToFront:async()=>{activated++;}};
  let unstable=false;
  const bindings={expectedNativePageIdentity:identity,readPageTargetWindow:async()=>identity,
    inspectNativeForegroundIdentity,inspectNativePageFocus,cdp:{send:async()=>{activated++;}},page,frame,
    ensureFrame:()=>{},nativeFocusState:async target=>{reads++;return unstable&&reads===3?{...visible,hidden:true}:visible;},
    activateOwnedBrowserWindow:async()=>{native++;return{status:'requested'};},lifecycleController:{metadata:{processId:123}},
    restoreNativeForeground:async options=>{fallback++;await options.activatePage('owned');return{ready:false,reason:'focus unavailable'};}};
  const restore=new Function(...Object.keys(bindings),declaration+'return restoreForeground;')(...Object.values(bindings));
  const result=await restore({runtime:true});
  assert.equal(result.ready,true);assert.equal(result.samples.length,2);assert.equal(result.method,'existing-native-foreground');
  assert.equal(focused,1);assert.equal(waits,1);assert.equal(native,0);assert.equal(activated,0);assert.equal(fallback,0);
  reads=0;unstable=true;
  const failed=await restore({runtime:true});
  assert.equal(failed.ready,false);assert.equal(fallback,1);assert.equal(native,1);
  assert.equal(failed.passiveSamples.length,1,'A hidden Runtime does not satisfy the passive foreground path');
});

test('foreground PCM window rejects intervening Runtime hidden and Host blur even after recovery',async()=>{
  const {inspectForegroundPcmWindow:inspectRaw}=await import('./runtime-desktop-lifecycle.mjs');
  const hostSource={url:'https://fixture/host',timeOrigin:1000},runtimeSource={url:'https://fixture/runtime?epoch=1',timeOrigin:2000};
  const inspectForegroundPcmWindow=options=>inspectRaw({...options,
    hostEvents:(options.hostEvents??[]).map(event=>({source:hostSource,...event})),
    runtimeEvents:(options.runtimeEvents??[]).map(event=>({source:runtimeSource,...event}))});
  const sample=time=>({host:{source:hostSource,time,visibilityState:'visible',hidden:false,hasFocus:true,inputOwnerFocused:true},
    runtime:{source:runtimeSource,time:time+10000,visibilityState:'visible',hidden:false,hasFocus:false}});
  const samples=[sample(100),sample(200),sample(300)].map((value,index)=>({...value,phase:['before-capture','after-capture','after-keyup'][index]}));
  const baseline={samples,hostEvents:[],runtimeEvents:[{type:'blur',hidden:false,visibility:'visible',time:10150}]};
  assert.equal(inspectForegroundPcmWindow(baseline).ready,true,'Iframe blur while Host owns input is expected');
  const hidden=inspectForegroundPcmWindow({...baseline,runtimeEvents:[{type:'visibilitychange',hidden:true,time:10150},{type:'visibilitychange',hidden:false,time:10180}]});
  assert.equal(hidden.ready,false);assert.equal(hidden.runtimeInterruptions.length,1);
  const blur=inspectForegroundPcmWindow({...baseline,hostEvents:[{reason:'host-blur',time:150},{type:'focus',time:180}]});
  assert.equal(blur.ready,false);assert.equal(blur.hostInterruptions.length,1);
  assert.equal(inspectForegroundPcmWindow({...baseline,hostEvents:[{reason:'host-hidden',time:150}]}).ready,false);
  assert.equal(inspectForegroundPcmWindow({...baseline,runtimeEvents:[{type:'pagehide',time:10150}]}).ready,false);
  assert.equal(inspectForegroundPcmWindow({...baseline,hostEvents:[{reason:'host-blur',time:99}],runtimeEvents:[{hidden:true,time:10301}]}).ready,true,'Each document uses its own bounded clock interval');
  assert.equal(inspectForegroundPcmWindow({...baseline,samples:[samples[0],{...samples[1],host:{...samples[1].host,inputOwnerFocused:false}},samples[2]]}).ready,false);
  assert.equal(inspectForegroundPcmWindow({samples:[samples[0]]}).ready,false);
  assert.equal(inspectForegroundPcmWindow({...baseline,runtimeEvents:[{type:'focus',time:10150,source:hostSource}]}).ready,false,'Events from a different document cannot inherit Runtime timing');
  assert.equal(inspectForegroundPcmWindow({...baseline,samples:samples.map((value,index)=>index===1?{...value,runtime:{...value.runtime,source:{...runtimeSource,timeOrigin:3000}}}:value)}).ready,false,'Navigation changes the document clock identity');
  assert.equal(inspectRaw({...baseline,runtimeEvents:[{type:'focus',time:10150}]}).ready,false,'Missing event provenance is incomplete');

  for(const invalid of [samples.slice(0,2),[...samples,samples[2]],
    samples.map(({phase,...value})=>value),[samples[1],samples[0],samples[2]],
    [samples[0],{...samples[1],phase:'before-capture'},samples[2]]])
    assert.equal(inspectForegroundPcmWindow({...baseline,samples:invalid}).complete,false,'Boundary phases must appear exactly once in producer order');
  for(const document of ['host','runtime'])for(const time of [-1,100,50,NaN,Infinity]){
    const invalid=samples.map(value=>({...value,[document]:{...value[document]}}));
    invalid[1][document].time=document==='runtime'&&Number.isFinite(time)&&time>=0?time+10000:time;
    assert.equal(inspectForegroundPcmWindow({...baseline,samples:invalid}).ready,false,`${document} boundary time must strictly increase`);
  }

});
