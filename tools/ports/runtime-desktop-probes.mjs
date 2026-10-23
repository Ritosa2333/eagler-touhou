import assert from 'node:assert/strict';
import {createHash,randomUUID} from 'node:crypto';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve,extname,dirname} from 'node:path';
import {execFileSync} from 'node:child_process';
import {build} from '../../launcher/node_modules/esbuild/lib/main.js';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {buildProductionPlayerHostRenderer,productionPlayerStylesheetLinks,readLauncherPublicAsset} from './launcher-player-host-scaffold.mjs';
import {MUSIC_PCM_PROBE_SOURCE} from './music-browser-helpers.mjs';
import {collectDesktopSfxWindow,DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE,mapDesktopSfxStartJournal,validateDesktopSfxInputWindow} from './runtime-desktop-audio.mjs';
import {createStageRecorder,DesktopStartupPreconditionError,readDesktopHostEvents,recordDesktopGameplayReadiness,recordDesktopStartupEvidenceIssue,requiredAudioTitleReadinessEvidence,requiredDesktopReadinessEvidence,requiredDesktopStartupStages,summarizeDesktopStartupEvidence} from './runtime-desktop-stage.mjs';
import {incompleteGameplayPrecondition,incompleteTh09TouchPrecondition,correlateInputSfxWindow,correlateSfxWindow} from './runtime-desktop-readiness.mjs';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {createFixtureResponseProbe} from './runtime-fixture-site.mjs';
import {assessFocusRewriteForTarget,identifyPlaywrightSources,launchFocusControlledChromium} from '../browser/cdp-focus-gateway.mjs';
import {classifyAudioLifecyclePath,compareNativeBrowserTargets,desktopCheckAdaptationIds,desktopCheckScope,desktopExecutionBrowserMode,inspectAudioContextsBeforeGesture,inspectAudioGestureWindow,inspectMinimizedKeyboardClear,inspectNativeBackgroundEvents,inspectNativeFireStimulus,inspectNativeForegroundIdentity,inspectNativePageFocus,inspectPostCleanupForeground,inspectViewportScreenshotClip,selectDesktopExecutionMode,selectNativePageTarget,summarizeDesktopCheckScope} from './runtime-desktop-lifecycle.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {assessRuntimeTouchRestart,captureRuntimeRestartBoundary,compareRuntimeRestartIdentity,createLauncherRestartPress} from './runtime-touch-restart.mjs';
import {assessRestartKeyboardForwarding,focusRestartKeyboardHost} from './runtime-touch-restart-keyboard.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,assertControlledScene,assertControlledSceneStimulus,assertControlledSceneRequest,assertControlledScenePauseBoundary,assertControlledSceneArmed,validationSceneRequest} from '../../tests/adaptation/adapters/scene.mjs';
import {touchMovementContract} from '../../tests/adaptation/adapters/movement.mjs';
import {readTouchMotionBoundary,readTouchMotionTrace,decodeTouchMotionTrace} from './touch-motion-contract.mjs';
import {runTouchMotionSceneGroup} from './touch-motion-scenes.mjs';
import {touchActionsRequest,readTouchActionState} from '../../tests/adaptation/adapters/actions.mjs';
import {runTouchActionSceneGroup} from './touch-action-scenes.mjs';
import {runTouchDoubleTapBombScene} from './touch-double-tap-scenes.mjs';
import {runTouchBatch} from './touch-batch-runtime.mjs';
import {runTouchUnlimitedResultSceneGroup} from './touch-unlimited-result-scenes.mjs';
import {controlRunBoundary,runBoundaryRequest} from '../../tests/adaptation/adapters/run-boundaries.mjs';
import {deathbombRequest,readDeathbombState,requestDeathbombHit} from '../../tests/adaptation/adapters/deathbomb.mjs';
import {runTouchDeathbombScene} from './touch-deathbomb-scenes.mjs';
import {dialogueTouchSceneRequest,readDialogueTouchWords,normalizeDialogueTouchManifest,normalizeDialogueTouchObservation,requestDialogueTouchMessage} from '../../tests/adaptation/adapters/dialogue.mjs';
import {runTouchDialogueFreshConfirmScene} from './touch-dialogue-confirm-scenes.mjs';
import {runOriginalMenuInputScene} from './original-menu-input-scenes.mjs';
import {runLoadedDialogueSkipScene} from './loaded-dialogue-skip-scenes.mjs';
import {readOriginalMenuState} from '../../tests/adaptation/adapters/menu.mjs';
import {runLauncherTouchScenes} from './launcher-touch-scenes.mjs';
import {runLauncherInputRuntime} from './launcher-input-runtime.mjs';
import {sendOwnedPhysicalKey} from './keyboard-scene-input.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {callNativeReplayFile} from '../../tests/adaptation/adapters/replay-files.mjs';
import {controlReplayTrace,readReplayTrace,readReplayInputReceipt} from '../../tests/adaptation/adapters/replay-trace.mjs';
import {runReplayRecordingGroup} from './replay-recording-runner.mjs';
import {gamepadProbeBundle,prepareVirtualGamepad,bindProtocolHostGamepad} from './gamepad-probe.mjs';
import {deriveSfxLatencyContract,readSfxLatencySceneState,resolveSfxLatencyContract,resolveSfxLatencyScenes,sfxLatencySceneRequest} from '../../tests/adaptation/adapters/audio.mjs';
import {assertQueueTailMutationIsRejected,classifySfxLatencyEvidence,sfxLatencyProbeInitSource} from './sfx-latency-contract.mjs';
import {inspectDesktopTerminalErrors} from './runtime-desktop-lifecycle.mjs';
import {inspectDesktopInputCleanup} from './runtime-desktop-lifecycle.mjs';
import {createHeadlessLifecycleDiagnostics} from './headless-lifecycle-diagnostics.mjs';

const argument=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const outputArgument=argument('output');
if(!outputArgument)throw new Error('Required: --output=ignored-report-directory');
const output=resolve(outputArgument),root=resolve(import.meta.dirname,'../..');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const nativeFocusState=page=>page.evaluate(()=>({visibilityState:document.visibilityState,hidden:document.hidden,hasFocus:document.hasFocus()}));
async function waitForNativeFocus(page,timeoutMs=5000){
  const started=Date.now();let state=null,samples=0,lastError=null;
  do{
    try{state=await nativeFocusState(page);samples++;if(inspectNativePageFocus(state).ready)return{ready:true,state,samples,durationMs:Date.now()-started};}
    catch(error){lastError=String(error.message||error);}
    await page.waitForTimeout(100).catch(()=>{});
  }while(Date.now()-started<timeoutMs);
  const inspected=inspectNativePageFocus(state);
  return{ready:false,state,samples,durationMs:Date.now()-started,missing:inspected.missing,error:lastError};
}
async function readPageTargetWindow(browserCdp,page,{title}={}){
  const url=page.url(),response=await browserCdp.send('Target.getTargets');
  const selected=selectNativePageTarget({targetInfos:response.targetInfos??[],url:title===undefined?url:undefined,title});
  if(selected.status!=='matched')return{status:'incomplete',reason:selected.reason,url,matchCount:selected.matchCount};
  const target=selected.target;
  try{
    const window=await browserCdp.send('Browser.getWindowForTarget',{targetId:target.targetId});
    const bounds=await browserCdp.send('Browser.getWindowBounds',{windowId:window.windowId});
    if(!Number.isSafeInteger(window.windowId))return{status:'incomplete',target,reason:'Browser.getWindowForTarget returned no native window ID.'};
    return{status:'matched',target,window:{windowId:window.windowId,bounds:window.bounds??null,currentBounds:bounds.bounds??null}};
  }catch(error){return{status:'incomplete',target,url,reason:`Native Browser window identity unavailable: ${error.message}`};}
}
const playwrightSourceSignature=source=>JSON.stringify({entry:source.entry,coreEntry:source.coreEntry,packageRoot:source.packageRoot,
  packageName:source.packageName,version:source.version,entryIdentity:source.entryIdentity,coreEntryIdentity:source.coreEntryIdentity,packageIdentity:source.packageIdentity,
  files:Object.fromEntries(Object.entries(source.files).map(([name,file])=>[name,{path:file.path,bytes:file.bytes,sha256:file.sha256}]))});
const audioProbeSource=`${MUSIC_PCM_PROBE_SOURCE}\n${DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE}`;

const report=await runWithReport({output,schema:'eagler-touhou/runtime-desktop-probes/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','variant','check'],flags:['suppress-touch-movement','background-only','input-only','touch-only','actions-only','replay-recording-only','audio-latency-only']});
  const suppressTouchMovement=process.argv.includes('--suppress-touch-movement');
  const backgroundOnly=process.argv.includes('--background-only');
  const inputOnly=process.argv.includes('--input-only');
  const touchOnly=process.argv.includes('--touch-only');
  const actionsOnly=process.argv.includes('--actions-only');
  const replayRecordingOnly=process.argv.includes('--replay-recording-only');
  const controlledInput=inputOnly||touchOnly||actionsOnly;
  const audioLatencyOnly=process.argv.includes('--audio-latency-only');
  const executionMode=selectDesktopExecutionMode({backgroundOnly,inputOnly,touchOnly,actionsOnly,replayRecordingOnly,audioLatencyOnly});
  const headlessProbe=desktopExecutionBrowserMode(executionMode)==='headless';
  const readinessEvidence=audioLatencyOnly?requiredAudioTitleReadinessEvidence:requiredDesktopReadinessEvidence;
  const selectedChecks=argument('check')?.split(',')??[];
  const checkScope=desktopCheckScope(executionMode,{adaptationIds:selectedChecks,game:argument('game')});
  if(suppressTouchMovement&&backgroundOnly)throw new Error('--background-only cannot be combined with --suppress-touch-movement.');
  const selectedGame=argument('game'),selectedVariant=argument('variant');
  if(!selectedGame||selectedVariant!=='normal')throw new Error('Required: --game=<explicit comma-separated games> --variant=normal');
  const manifestArgument=argument('manifest');if(!manifestArgument)throw new Error('Required: --manifest=<accepted private input manifest>');
  const manifestPath=resolve(manifestArgument);
  const config=JSON.parse(await readFile(manifestPath,'utf8'));
  const selection=selectRuntimeTargets(config,{game:selectedGame,variant:selectedVariant});
  if(selectedChecks.length&&selection.games.length!==1)throw Error('Numbered desktop selection requires one explicit game');
  const keyboardBundleResult=await build({
    stdin:{resolveDir:resolve(root,'launcher'),loader:'ts',contents:
      "export {KeyboardMapping,keyboardMappingSchema} from './src/launcher/keyboard-mapping.mts';export {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';"},
    bundle:true,write:false,format:'iife',globalName:'EaglerDesktopKeyboard',platform:'browser',target:'es2020'});
  const keyboardBundle=keyboardBundleResult.outputFiles[0].text;
  const launcherTouchFixtureUrl=new URL('./launcher-touch-fixture.ts',import.meta.url);
  const touchBundleResult=await build({entryPoints:[fileURLToPath(launcherTouchFixtureUrl)],
    bundle:true,write:false,format:'iife',globalName:'EaglerLauncherTouchFixture',platform:'browser',target:'es2020'});
  const launcherTouchBundleBytes=Buffer.from(touchBundleResult.outputFiles[0].text);
  const playerHostRenderer=await buildProductionPlayerHostRenderer({
    bundlePath:resolve(output,'launcher-player-host-renderer.mjs'),builder:build});
  const playerHostMarkup=new Map(selection.games.map(game=>[game,playerHostRenderer.render(game)]));
  const harnessSource=await readFile(new URL(import.meta.url));
  const stageRecorderSource=await readFile(new URL('./runtime-desktop-stage.mjs',import.meta.url));
  const readinessSource=await readFile(new URL('./runtime-desktop-readiness.mjs',import.meta.url));
  const practiceReadinessSource=await readFile(new URL('./practice-observations.mjs',import.meta.url));
  const titleFlowSource=await readFile(new URL('../../tests/adaptation/adapters/title-flow.mjs',import.meta.url));
  const productCatalogSource=await readFile(new URL('../../common/contracts/product-catalog.mjs',import.meta.url));
  const fixtureServerSource=await readFile(new URL('../browser/fixture-server.mjs',import.meta.url));
  const fixtureResponseWatcherSource=await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url));
  const responseBodyCaptureSource=await readFile(new URL('../browser/response-body-capture.mjs',import.meta.url));
  const focusGatewaySource=await readFile(new URL('../browser/cdp-focus-gateway.mjs',import.meta.url));
  const lifecycleClassifierSource=await readFile(new URL('./runtime-desktop-lifecycle.mjs',import.meta.url));
  const browserLauncherSource=await readFile(new URL('../browser/launch.mjs',import.meta.url));
  const playwrightLoaderSource=await readFile(new URL('../browser/playwright.mjs',import.meta.url));
  const touchModules=[];
  const touchEntrypoints=['./touch-motion-contract.mjs','./touch-motion-scenes.mjs','./launcher-player-host-scaffold.mjs','./headless-lifecycle-diagnostics.mjs','../../tests/adaptation/adapters/touch-clock.mjs',
    '../../tests/adaptation/adapters/movement.mjs','../../tests/adaptation/adapters/scene.mjs',
    './sfx-latency-contract.mjs','../../tests/adaptation/adapters/audio.mjs',
    '../../launcher/public/styles.css','../../launcher/app/components/player/workspace-viewport.css',
    './music-browser-helpers.mjs','./runtime-desktop-audio.mjs',
    './touch-action-scenes.mjs','./launcher-touch-scenes.mjs','./launcher-touch-fixture.ts','./launcher-input-runtime.mjs',
    './original-menu-input-scenes.mjs','./loaded-dialogue-skip-scenes.mjs','../../tests/adaptation/adapters/menu.mjs',
    '../../tests/adaptation/adapters/actions.mjs',
    './replay-recording-runner.mjs','../../tests/adaptation/adapters/replay-trace.mjs',
    '../../tests/adaptation/adapters/replay-files.mjs','./gamepad-probe.mjs',
    '../../launcher/src/launcher/gamepad-mapping.mts','../../launcher/app/services/player-gamepad.ts',
    '../../launcher/tests/fixtures/virtual-gamepads.mjs'];
  const touchDependencies=await dependencySnapshot(root,[fileURLToPath(import.meta.url),
    ...touchEntrypoints.map(name=>fileURLToPath(new URL(name,import.meta.url)))]);
  for(const item of touchDependencies){
    const url=pathToFileURL(resolve(root,item.path)),bytes=await readFile(url);
    assert.equal(hash(bytes),item.sha256,'Adapter dependency changed while recording its identity');
    touchModules.push({source:url.href,path:`touch-${touchModules.length}.mjs`,sha256:hash(bytes),bytes});
  }
  const playwrightSources=await identifyPlaywrightSources({workspaceRoot:root});
  const playwrightSourceStartSignature=playwrightSourceSignature(playwrightSources);
  const playwrightCoreSourceBytes={};
  const playwrightCoreArtifacts={};
  await mkdir(resolve(output,'playwright-core-source'),{recursive:true});
  for(const [relative,identity] of Object.entries({
    'playwright-entry':playwrightSources.entryIdentity,
    'core-entry':playwrightSources.coreEntryIdentity,
    'package-manifest':playwrightSources.packageIdentity,
    ...Object.fromEntries(Object.entries(playwrightSources.files).map(([name,file])=>[name,file])),
  })){
    const bytes=await readFile(identity.path);
    assert.equal(bytes.length,identity.bytes,`Playwright source size changed while snapshotting ${identity.path}`);
    assert.equal(hash(bytes),identity.sha256,`Playwright source hash changed while snapshotting ${identity.path}`);
    const artifactPath=`playwright-core-source/${relative.replaceAll('/','_')}`;
    playwrightCoreSourceBytes[relative]=bytes;
    playwrightCoreArtifacts[relative]={path:artifactPath,sourcePath:identity.path,bytes:identity.bytes,sha256:identity.sha256};
  }
  const readerSource=readRuntimeObservation.toString();
  const readerSourceBytes=Buffer.from(`(${readerSource})\n`,'utf8');
  const keyboardBundleBytes=Buffer.from(keyboardBundle,'utf8');
  const audioProbeBytes=Buffer.from(audioProbeSource,'utf8');
  const manifestIdentity={path:manifestPath,sha256:hash(await readFile(manifestPath))};
  const probeArtifacts={
    harness:{path:'harness.mjs',sha256:hash(harnessSource)},
    stageRecorder:{path:'runtime-desktop-stage-recorder.mjs',sha256:hash(stageRecorderSource)},
    gameplayReadiness:{path:'runtime-desktop-readiness.mjs',sha256:hash(readinessSource)},
    practiceReadiness:{path:'practice-observations.mjs',sha256:hash(practiceReadinessSource)},
    titleFlow:{path:'tests/adaptation/adapters/title-flow.mjs',sha256:hash(titleFlowSource)},
    productCatalog:{path:'common/contracts/product-catalog.mjs',sha256:hash(productCatalogSource)},
    fixtureServer:{path:'fixture-server.mjs',sha256:hash(fixtureServerSource)},
    fixtureResponseWatcher:{path:'runtime-fixture-site.mjs',sha256:hash(fixtureResponseWatcherSource)},
    responseBodyCapture:{path:'response-body-capture.mjs',sha256:hash(responseBodyCaptureSource)},
    focusGateway:{path:'cdp-focus-gateway.mjs',sha256:hash(focusGatewaySource)},
    lifecycleClassifier:{path:'runtime-desktop-lifecycle.mjs',sha256:hash(lifecycleClassifierSource)},
    browserLauncher:{path:'browser-launch.mjs',sha256:hash(browserLauncherSource)},
    playwrightLoader:{path:'playwright-loader.mjs',sha256:hash(playwrightLoaderSource)},
    playwrightCore:{entry:playwrightSources.entry,coreEntry:playwrightSources.coreEntry,packageRoot:playwrightSources.packageRoot,
      packageName:playwrightSources.packageName,version:playwrightSources.version,entryIdentity:playwrightSources.entryIdentity,
      coreEntryIdentity:playwrightSources.coreEntryIdentity,packageIdentity:playwrightSources.packageIdentity,files:playwrightCoreArtifacts},
    injectedReader:{path:'runtime-observation-reader.js',serialization:'readRuntimeObservation.toString()',sha256:hash(readerSourceBytes)},
    keyboardBundle:{path:'desktop-keyboard-mapping.js',sha256:hash(keyboardBundleBytes)},
    launcherTouchBundle:{path:'desktop-launcher-touch.js',sha256:hash(launcherTouchBundleBytes)},
    launcherPlayerHostRenderer:{path:'launcher-player-host-renderer.mjs',sha256:hash(playerHostRenderer.bytes)},
    audioProbeSource:{path:'desktop-audio-probe.js',sha256:hash(audioProbeBytes)},
  };
  await Promise.all(Object.values(probeArtifacts).filter(item=>typeof item.path==='string')
    .map(item=>mkdir(dirname(resolve(output,item.path)),{recursive:true})));
  await Promise.all([
    writeFile(resolve(output,probeArtifacts.harness.path),harnessSource),
    writeFile(resolve(output,probeArtifacts.stageRecorder.path),stageRecorderSource),
    writeFile(resolve(output,probeArtifacts.gameplayReadiness.path),readinessSource),
    writeFile(resolve(output,probeArtifacts.practiceReadiness.path),practiceReadinessSource),
    writeFile(resolve(output,probeArtifacts.titleFlow.path),titleFlowSource),
    writeFile(resolve(output,probeArtifacts.productCatalog.path),productCatalogSource),
    writeFile(resolve(output,probeArtifacts.fixtureServer.path),fixtureServerSource),
    writeFile(resolve(output,probeArtifacts.fixtureResponseWatcher.path),fixtureResponseWatcherSource),
    writeFile(resolve(output,probeArtifacts.responseBodyCapture.path),responseBodyCaptureSource),
    writeFile(resolve(output,probeArtifacts.focusGateway.path),focusGatewaySource),
    writeFile(resolve(output,probeArtifacts.lifecycleClassifier.path),lifecycleClassifierSource),
    writeFile(resolve(output,probeArtifacts.browserLauncher.path),browserLauncherSource),
    writeFile(resolve(output,probeArtifacts.playwrightLoader.path),playwrightLoaderSource),
    ...Object.entries(playwrightCoreSourceBytes).map(([relative,bytes])=>writeFile(resolve(output,playwrightCoreArtifacts[relative].path),bytes)),
    writeFile(resolve(output,probeArtifacts.injectedReader.path),readerSourceBytes),
    writeFile(resolve(output,probeArtifacts.keyboardBundle.path),keyboardBundleBytes),
    writeFile(resolve(output,probeArtifacts.launcherTouchBundle.path),launcherTouchBundleBytes),
    writeFile(resolve(output,probeArtifacts.audioProbeSource.path),audioProbeBytes),
  ]);
  const sourceSnapshotStart=sourceSnapshot(root);
  await Promise.all(touchModules.map(({path,bytes})=>writeFile(resolve(output,path),bytes)));
  Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
    sourceSnapshotStart,manifest:manifestIdentity,
    probeArtifacts,
    touchModules:touchModules.map(({bytes,...identity})=>identity),
    selection:{games:selection.games,variant:selection.variant,expectedTargets:selection.expected,checks:selectedChecks},
    negativeControl:{enabled:suppressTouchMovement,kind:suppressTouchMovement?'suppress-touch-movement':null},
    executionMode,
    scope:actionsOnly?'Headless Chromium | shared Runtime touch protocol actions | actual native player consumers and current artifact identity':backgroundOnly?'Headed full Chrome with current normal Runtime packages and private DATA. The probe performs the same Runtime launch and native gameplay preparation, then runs only package/DATA identity, real window-minimize lifecycle, same-window tab background/foreground lifecycle, background audio recovery, and postflight identity checks. It observes native target/window IDs and document focus state; Playwright focus emulation is audited and rewritten through a loopback CDP gateway. No synthetic lifecycle events are dispatched.':inputOnly?'Headless Chromium with current normal Runtime packages and private DATA. Production DOM keyboard mapping and Runtime KeyboardInputBridge, documented touch protocol, native gameplay effects, foreground SFX, pause, new-epoch and package/DATA identity checks run together. Host and Runtime document visibility/focus are sampled as page state; native window foreground and background lifecycle are evaluated by the headed desktop-background mode.':'Headed Chromium on Windows with current normal Runtime packages and private DATA. Playwright keyboard events pass through the production KeyboardMapping and Runtime KeyboardInputBridge. A small host harness forwards host blur/document-hidden to KeyboardMapping.clear and keyboard-clear; this verifies Runtime release behavior without claiming the Launcher UI wiring. Simulated touch commands enter each Runtime package through its documented touch protocol while the native game loop runs. WebAudio sampling is attached inside each Runtime iframe to its actual output graph. No physical keyboard, touchscreen, controller, retail EXE comparison, Launcher UI, multiplayer session, or OGG track is claimed.',
    protocols:{keyboard:'eagler-touhou/keyboard-input/1',touch:'eagler-touhou/runtime-protocol top-level touch-controls fields',nativeObservation:'eagler-touhou/runtime-observation/1',audio:'Per-Runtime-realm AudioNode AnalyserNode tap immediately before AudioDestinationNode'},
    audioMode:'configure.music=none; this isolates actual sound-effect output from BGM. AUDIO-01 OGG track playback is outside this probe because the accepted input manifest supplies no music resource receipt.',
    requiredChecks:touchOnly?[
      'Current package and DATA identity before and after execution',
      'Shared limited, unlimited and free joystick movement | native keyboard speed reference | per-tick traces',
      '100/200/300 sensitivity | free angles | micro-movement | original speed cap | release',
    ]:backgroundOnly?[
      'BUILD-04/PACKAGE-02 package and private DATA identity preflight',
      'RUNTIME-04/INPUT-06 real minimized-window lifecycle and native input release',
      'RUNTIME-04/INPUT-06 same-window tab background/foreground lifecycle and native input release',
      'AUDIO-04 WebAudio output recovery after a real background transition',
      'BUILD-04/PACKAGE-02 package and private DATA identity postflight',
    ]:inputOnly?[
      'BUILD-04/PACKAGE-02 package and private DATA identity preflight',
      'INPUT-01 arrows up/down/left/right and input-negative controls',
      'INPUT-06 native keyup and clear while held',
      'AUDIO-02 foreground WebAudio PCM output with music disabled',
      'Shared limited/unlimited touch and free-direction joystick scenes | sensitivity | micro-movement | speed limit | release | two-finger focus/ownership',
      'INPUT-02 WASD up/down/left/right through production keyboard aliases',
      'TOUCH-05/06 fire and focus | TOUCH-15 mixed holders | TOUCH-16 cancel releases touch',
      'RUNTIME-04 pause and simulation freeze when observable',
      'RUNTIME-01 Runtime frame teardown and epoch replacement',
      'BUILD-04/PACKAGE-02 package and private DATA identity postflight',
    ]:['INPUT-01 arrows up/down/left/right','INPUT-02 WASD up/down/left/right through production keyboard aliases',
      'INPUT-06 native keyup and clear while held','TOUCH-01..04 all four configured movement modes','TOUCH-05/06 fire and focus','TOUCH-15 mixed holders','TOUCH-16 cancel releases touch',
      'RUNTIME-04 pause and simulation freeze when observable','RUNTIME-04 headed window minimize lifecycle observation',
      'INPUT-06/RUNTIME-04 real tab background/foreground and keyboard clear',
      'AUDIO-02 WebAudio PCM output with music disabled','RUNTIME-01 Runtime frame teardown and epoch replacement'],targets:[]});
  report.requiredStartupStages=[...requiredDesktopStartupStages];
  if(touchOnly)report.scope='Shared protocol movement checks in headless Chromium using real packaged WASM, DATA and completed native ticks; window and audio lifecycle belong to their dedicated scenes.';
  if(replayRecordingOnly){
    report.scope='Packaged normal Runtime | one production recording world | finite keyboard, gamepad and touch windows | independent file parsing | paired playback input isolation';
    report.requiredChecks=['Package and DATA identity before and after execution',
      'Actual recording input and independently parsed saved bytes for all declared routes',
      'Versioned pointer trajectory and actual playback application',
      'The same Replay bytes under baseline and each live-input perturbation'];
  }
  if(audioLatencyOnly){
    report.scope='Packaged normal Runtime | native title input and logical sound | production target-voice PCM | live delay-to-queue-tail negative control. Title scenes share one session; gameplay and BGM lifecycle have separate acceptance.';
    report.protocols.audio='Silent parallel AudioWorklet tap of the actual target WebSfx voice | event-time SDL mixed-queue tail';
    report.requiredChecks=['Package and private DATA identity before and after execution',
      'Native logical event, successful upload, direct route and actual target PCM before the measured mixed queue tail',
      'The same live target delayed to its measured queue tail must fail the unchanged classifier'];
  }
  report.requiredReadinessEvidence={...readinessEvidence};
  report.requiredCheckIds=checkScope.requiredIds;
  report.omittedChecks=checkScope.omittedIds.map(id=>({id,status:'omitted',reason:`Excluded by explicit --${executionMode} execution mode; this is not a pass.`}));
  const routeFiles=new Map();
  for(const [name,file] of Object.entries(config.shared??{}))routeFiles.set(`/shared/${name}`,resolve(file));
  routeFiles.set('/desktop-keyboard-mapping.js',keyboardBundleBytes);
  routeFiles.set('/desktop-launcher-touch.js',launcherTouchBundleBytes);
  routeFiles.set('/desktop-audio-probe.js',audioProbeBytes);
  routeFiles.set('/launcher-app/workspace-viewport.css',resolve(root,'launcher/app/components/player/workspace-viewport.css'));
  const packages=new Map(),dataIdentities=new Map();
  const hostHtml=target=>`<!doctype html><html><head><meta charset="utf-8"><title>${target.id} desktop Runtime probe</title>
  ${productionPlayerStylesheetLinks()}<style>html,body{margin:0;width:100%;height:100%;overflow:hidden;background:#202020;color:white}body{outline:none}</style>
  <script src="/desktop-keyboard-mapping.js"></script><script src="/desktop-launcher-touch.js"></script><script src="/desktop-audio-probe.js"></script><script>
   const selected=${JSON.stringify({id:target.id,game:target.game,entry:target.entry})};let epoch=1;
   window.events=[];window.lifecycleEvents=[];window.hostKeyboardLifecycle=[];window.keyboardForwardEvents=[];window.keyboardMapEvents=[];
   window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const response=await fetch('/data/'+selected.id);if(!response.ok)throw Error('Private DATA is unavailable');const buffer=await response.arrayBuffer();return{buffer,bytes:buffer.byteLength};};
   const runtimeUrl=n=>'/runtime/'+selected.id+'/'+selected.entry+'?hosted=1&managedData=1&gameGeneration=desktop-probe&runtimeEpoch='+n;
   const frame=()=>document.querySelector('iframe');
   window.send=(command,fields={})=>frame().contentWindow.postMessage({protocol:'eagler-touhou/1',game:selected.game,epoch,...{command},...fields},location.origin);
   window.rpc=(command,fields={},timeoutMs=120000)=>new Promise((resolve,reject)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout: '+command));},timeoutMs);function receive(event){if(event.origin!==location.origin||event.source!==frame().contentWindow||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?resolve(event.data):reject(Error(event.data.error||'Command rejected'));}addEventListener('message',receive);send(command,{...fields,request});});
   addEventListener('message',event=>{if(event.origin!==location.origin)return;if(event.data?.desktopProbe===true)lifecycleEvents.push(event.data);else if(event.source===frame()?.contentWindow)events.push(event.data);});
   window.mountEpoch=n=>{epoch=n;frame().src=runtimeUrl(n);};
   window.__desktopKeyboardPreferences=EaglerDesktopKeyboard.createKeyboardPreferences(localStorage);
   window.__desktopInstallKeyboardMapping=({epoch:n,layout})=>{
    const target=frame().contentWindow,game=selected.game,preferences=window.__desktopKeyboardPreferences;
    if(layout)preferences.selectLayout(layout);
    const owner=new EaglerDesktopKeyboard.KeyboardMapping(preferences.getSnapshot),context={target,game,epoch:n,launched:true,spectator:false};
    const port={schema:EaglerDesktopKeyboard.keyboardMappingSchema,protocol:'eagler-touhou/1',game,epoch:n,origin:location.origin,
      supports:code=>owner.supports(code),map:(event,down,source,profile)=>{const transitions=owner.map(event,down,source,profile,context);try{parent.keyboardMapEvents.push({code:event.code,key:event.key,down,source,profile,transitions:transitions?.map(item=>({...item}))??null,epoch:n,time:performance.now()});}catch{}return transitions;},clear:()=>owner.clear()};
    target.__eaglerKeyboardMapping=port;window.__desktopKeyboardOwner=owner;return{schema:port.schema,layout:preferences.getLayoutSnapshot()};
   };
   addEventListener('keydown',event=>forwardDesktopKey(event,true),true);
   addEventListener('keyup',event=>forwardDesktopKey(event,false),true);
   function forwardDesktopKey(event,down){
    const target=frame().contentWindow,port=target.__eaglerKeyboardMapping,bridge=target.__eaglerKeyboardInputBridge;
    if(!port||typeof port.map!=='function'||!bridge)return;
    if(!port.supports(event.code))return;
    const accepted=bridge.submit({protocol:'eagler-touhou/1',game:selected.game,epoch,command:'keyboard',down,code:event.code,key:event.key,keyCode:event.keyCode,location:event.location??0,inputSource:'physical',repeat:event.repeat});
    keyboardForwardEvents.push({code:event.code,key:event.key,down,accepted,bridgeActive:bridge.active?.()??false,epoch,time:performance.now()});
    if(accepted===true)event.preventDefault();
   }
   const clearHeldKeyboard=(reason)=>{const owner=window.__desktopKeyboardOwner;if(!owner)return;owner.clear();const runtimeFrame=frame().contentWindow,bridge=runtimeFrame.__eaglerKeyboardInputBridge;const accepted=bridge?.submit({protocol:'eagler-touhou/1',game:selected.game,epoch,command:'keyboard-clear'})??false;hostKeyboardLifecycle.push({reason,epoch,accepted,time:performance.now()});};
   addEventListener('blur',()=>clearHeldKeyboard('host-blur'));
   document.addEventListener('visibilitychange',()=>{if(document.hidden)clearHeldKeyboard('host-hidden');});
   window.__desktopSelectLayout=(layout)=>{const changed=window.__desktopKeyboardPreferences.selectLayout(layout);if(!changed)throw Error('Unknown keyboard layout '+layout);
    const target=frame().contentWindow,bridge=target.__eaglerKeyboardInputBridge;if(!bridge)throw Error('Runtime keyboard input bridge is unavailable');window.__desktopKeyboardOwner?.clear();
    const ok=bridge.submit({protocol:'eagler-touhou/1',game:selected.game,epoch,command:'keyboard-clear'});
    if(!ok)throw Error('Runtime rejected keyboard clear after layout change');return window.__desktopKeyboardPreferences.getLayoutSnapshot();};
  </script></head><body>${playerHostMarkup.get(target.game)??(()=>{throw Error(`No production Player scaffold for ${target.game}`);})()}<script>mountEpoch(1);</script></body></html>`;
  let fixture;
  const expectedResponses=new Map();
  const reply=(request,response,bytes,contentType,status=200)=>{
    const body=Buffer.isBuffer(bytes)?bytes:Buffer.from(bytes),url=new URL(request.url,fixture.base);
    if(status===200)expectedResponses.set(url.pathname+url.search,{bytes:body.length,sha256:hash(body)});
    response.writeHead(status,{'Content-Type':contentType,'Content-Length':body.length,'Cache-Control':'no-store'}).end(body);
  };
  fixture=await createFixtureServer(async(request,response)=>{try{
    const url=new URL(request.url,'https://127.0.0.1');
    if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
    if(url.pathname.startsWith('/host/')){const target=config.targets.find(item=>item.id===decodeURIComponent(url.pathname.slice('/host/'.length)));if(!target)throw Error('Unknown selected target');reply(request,response,hostHtml(target),'text/html');return;}
    const selectedTarget=config.targets.find(target=>url.pathname===`/data/${target.id}`);
    if(selectedTarget){const expected=dataIdentities.get(selectedTarget.id);if(!expected)throw Error(`${selectedTarget.id}: private DATA was not verified`);const bytes=await readFile(selectedTarget.data);assert.equal(bytes.length,expected.bytes,'Private DATA size changed during run');assert.equal(hash(bytes),expected.sha256,'Private DATA changed during run');reply(request,response,bytes,'application/octet-stream');return;}
    for(const target of selection.targets){const prefix=`/runtime/${target.id}/`;if(url.pathname.startsWith(prefix)){const identity=packages.get(target.id);assert.ok(identity,`${target.id}: package has not been identity-verified`);const tail=decodeURIComponent(url.pathname.slice(prefix.length));const bytes=await readVerifiedRuntimeFile(target,identity,tail);reply(request,response,bytes,({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(tail)]??'application/octet-stream');return;}}
    if(url.pathname.startsWith('/launcher-public/')){const asset=await readLauncherPublicAsset(url.pathname);if(!asset){reply(request,response,'Launcher public asset not found','text/plain',404);return;}reply(request,response,asset.bytes,asset.contentType);return;}
    const file=routeFiles.get(url.pathname);if(file){const bytes=Buffer.isBuffer(file)?file:await readFile(file);reply(request,response,bytes,({'.mjs':'text/javascript','.js':'text/javascript','.css':'text/css','.json':'application/json'})[extname(url.pathname)]??'application/octet-stream');return;}
    reply(request,response,'Unknown desktop probe resource','text/plain',404);
  }catch(error){reply(request,response,String(error.message||error),'text/plain',500);}}, {protocol:'https',port:0,host:'127.0.0.1'});
  report.server={protocol:fixture.protocol,host:fixture.host,port:fixture.port,tlsIdentity:fixture.tlsIdentity};
  let browser;
  let lifecycleController;
  const headlessLifecycleDiagnostics=[];
  let activeHeadlessLifecycleDiagnostics=null;
  let detachActiveHeadlessBrowserListener=null;
  try{
    if(headlessProbe){
      const {launchBrowser}=await import('../browser/launch.mjs');
      browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
        `--ignore-certificate-errors-spki-list=${fixture.tlsIdentity.spkiSha256}`]});
      report.browser={version:browser.version(),mode:inputOnly?'headless Chromium | production DOM input and native Runtime effects':audioLatencyOnly?'headless Chromium | native input and actual PCM timing':'headless Chromium | shared protocol movement',headless:true,
        foregroundEvidence:'Host and Runtime document visibility/focus only; native window lifecycle is checked by headed desktop-background.'};
    }else{
    const {loadPlaywright}=await import('../browser/playwright.mjs');
    const {chromium}=await loadPlaywright();
    lifecycleController=await launchFocusControlledChromium({chromium,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${fixture.tlsIdentity.spkiSha256}`]});
    browser=lifecycleController.browser;
    report.browser={...lifecycleController.metadata,version:browser.version(),mode:'headed full Chrome via public launchServer and loopback CDP gateway',headless:false,
      certificateTrust:'Chromium trusts only the ephemeral fixture certificate SPKI pin for this run.',
      playwrightCore:{packageName:playwrightSources.packageName,version:playwrightSources.version,entry:playwrightSources.entry,
        coreEntry:playwrightSources.coreEntry,packageRoot:playwrightSources.packageRoot},
      focusEmulationPolicy:'Controller Emulation.setFocusEmulationEnabled enabled=true commands are audited and forwarded with enabled=false; Chrome receives the request and generates lifecycle events natively.'};
    }
    for(const target of selection.targets){
      const titleFlow=audioLatencyOnly?null:resolveTitleFlow({game:target.game,variant:target.variant});
      const sfxContract=audioLatencyOnly?resolveSfxLatencyContract(target.game,target.variant):null;
      const audioRuntimeMount=resolveAdaptationAdapter(target.game,target.variant)?.runtime??null;
      const record={game:target.game,variant:target.variant,id:target.id,status:'running',checks:[],stages:[],events:[],lifecycleEvents:[],errors:[],consoleErrors:[],httpErrors:[],audio:{},
        executionMode,requiredCheckIds:checkScope.requiredIds,
        requiredReadinessEvidence:{...readinessEvidence},
        omittedChecks:report.omittedChecks.map(item=>({...item}))};
      report.targets.push(record);await save();
      let context,page,frame,cdp,windowInfo,fixtureAudit,screenshotPageIdentity=null,expectedNativePageIdentity=null,activeEpoch=1,backgroundAudioEventStart=0,resetInputState=async()=>{};
      let detachHeadlessContext=null,detachHeadlessPage=null;
      let activeQueuedMovementPolicy=null;
      let controlledSceneArmedEpoch=null;
      let ordinaryRunBoundaryFixture=false;
      const headlessLifecycle=headlessProbe?createHeadlessLifecycleDiagnostics({targetId:target.id}):null;
      if(headlessLifecycle){
        detachActiveHeadlessBrowserListener?.();
        activeHeadlessLifecycleDiagnostics=headlessLifecycle;
        detachActiveHeadlessBrowserListener=headlessLifecycle.attachBrowser(browser);
        headlessLifecycle.setPhase('target-record-created',{activeEpoch});
      }
      const runLifecyclePhase=async(phase,operation,{activeEpoch:phaseEpoch=activeEpoch,plannedEpoch=null,ownedTeardownPhase=null}={})=>{
        if(!headlessLifecycle)return operation();
        headlessLifecycle.setPhase(phase,{activeEpoch:phaseEpoch,plannedEpoch,ownedTeardownPhase});
        try{
          const result=await operation();
          headlessLifecycle.setPhase(`${phase}:complete`,{activeEpoch:phaseEpoch,plannedEpoch,ownedTeardownPhase});
          return result;
        }catch(error){
          headlessLifecycle.setPhase(`${phase}:failed`,{activeEpoch:phaseEpoch,plannedEpoch});
          throw error;
        }
      };
      const recordHeadlessLifecycleDiagnostics=()=>{
        if(!headlessLifecycle)return;
        const snapshot=headlessLifecycle.snapshot();
        record.headlessLifecycleDiagnostics=snapshot;
        if(snapshot.unexpectedEvents.length){
          const event=snapshot.unexpectedEvents[0];
          record.headlessLifecycleErrors=snapshot.unexpectedEvents;
          record.failure??=`Unexpected headless browser lifecycle event ${event.event} during ${event.phase} at Runtime epoch ${event.activeEpoch??'unknown'}.`;
          if(record.status==='running'||record.status==='passed'||record.status==='incomplete')record.status='failed';
        }
      };
      if(headlessLifecycle)headlessLifecycleDiagnostics.push({record,diagnostics:headlessLifecycle,recordDiagnostics:recordHeadlessLifecycleDiagnostics});
      const stageRecorder=createStageRecorder(record,save);
      const stage=(name,details,operation)=>{
        const stageEpoch=Number.isSafeInteger(details?.epoch)?details.epoch:activeEpoch;
        const plannedEpoch=Number.isSafeInteger(details?.priorEpoch)&&Number.isSafeInteger(details?.epoch)?details.epoch:null;
        return runLifecyclePhase(`stage:${name}`,()=>stageRecorder(name,details,operation),{activeEpoch:stageEpoch,plannedEpoch});
      };
      const requiredCheckIds=checkScope.requiredIds?new Set(checkScope.requiredIds):null;
      const check=async(id,operation)=>{
        const adaptationIds=desktopCheckAdaptationIds(id,{game:target.game});
        if(requiredCheckIds&&!requiredCheckIds.has(id)){
          if(!record.omittedChecks.some(item=>item.id===id))record.omittedChecks.push({id,adaptationIds,status:'omitted',reason:`Excluded by --${executionMode}; this check is not counted as a pass.`});
          await save();return null;
        }
        const entry={id,adaptationIds,status:'running'};record.checks.push(entry);await save();try{entry.observation=await operation();if(entry.observation?.checkStatus){entry.status=entry.observation.checkStatus;delete entry.observation.checkStatus;}else entry.status='passed';
          if(['shared-original-menu-dialogue-input','shared-touch-menu-dialogue-gestures','shared-launcher-gamepad-mapping','shared-launcher-touch-layout','shared-launcher-touch-magnifier'].includes(id))
            entry.complete=entry.status==='passed'&&entry.observation?.complete===true;}
        catch(error){entry.status='failed';entry.error=String(error.stack||error);if(error.evidence||error.touchMotionEvidence||error.touchActionEvidence)entry.observation=error.evidence??error.touchMotionEvidence??error.touchActionEvidence;record.errors.push({check:id,error:entry.error});await save();if(!actionsOnly)await resetInputState().catch(()=>{});}await save();return entry;};
      const observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
      const observeWithAudioClock=()=>frame.evaluate(readRuntimeObservation,{...runtimeObservationRequest(target),includeAudioClock:true});
      const nativePeek=()=>frame.evaluate(({game,runtimeMount})=>{const declared=runtimeMount&&runtimeMount.game===game?runtimeMount:null;
        const runtime=declared?window[declared.runtime.global]:window[`__${game}Runtime`];
        const module=declared?runtime?.[declared.runtime.module]??null:runtime?.Module??window.Module;
        const core=declared?runtime?.[declared.runtime.core]??module:runtime?.core??module,audioContexts={};
        for(const api of ['SDL','SDL2','SDL3']){try{const context=module?.[api]?.audioContext;if(context)audioContexts[api]={state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime,sharedWithHost:context===parent.__touhouAudioContext};}catch(error){audioContexts[api]={error:String(error)};}}
        return{canvas:document.querySelector('canvas')?{width:document.querySelector('canvas').width,height:document.querySelector('canvas').height}:null,audioContexts,
          audioProbe:window.__eaglerDesktopAudioProbe?.summary?.()??null,wasmRuntime:!!core};},{game:target.game,runtimeMount:audioRuntimeMount?{game:target.game,runtime:audioRuntimeMount}:null});
      const captureRuntimeScreenshot=async(label,path)=>{
        const evidence={label,path,status:'running',startedAt:new Date().toISOString()};
        try{
          if(!page||!cdp||!frame)throw Error('The page, native CDP session, or Runtime frame is unavailable.');
          evidence.kind=headlessProbe?'headless-page-visibility':'native-window-foreground';
          if(!headlessProbe){
            const before=await readPageTargetWindow(cdp,page);evidence.pageBeforeRestore=before;
            const beforeIdentity=inspectNativeForegroundIdentity({expected:expectedNativePageIdentity,observed:before,requireNormal:false});
            evidence.identityBeforeRestore=beforeIdentity;
            if(!beforeIdentity.ready)throw Error(`The original native page/window identity was not present before screenshot capture: ${beforeIdentity.missing.join('; ')}`);
            await cdp.send('Browser.setWindowBounds',{windowId:before.window.windowId,bounds:{windowState:'normal'}});
          }else{
            evidence.pageForegroundClaim='headless document visibility and focus';
            evidence.hostFocusBeforeRestore=await nativeFocusState(page);
            evidence.runtimeFocusBeforeRestore=await nativeFocusState(frame);
          }
          await page.bringToFront();
          await frame.locator('canvas').focus();
          evidence.hostFocus=await waitForNativeFocus(page,5000);
          evidence.runtimeFocus=await waitForNativeFocus(frame,5000);
          if(!evidence.hostFocus.ready||!evidence.runtimeFocus.ready)throw Error('Host and Runtime documents did not both report native visible/focused state.');
          if(!headlessProbe){
            const after=await readPageTargetWindow(cdp,page);evidence.pageAfterRestore=after;
            evidence.identity=inspectNativeForegroundIdentity({expected:expectedNativePageIdentity,observed:after,requireNormal:true});
            if(!evidence.identity.ready)throw Error(`Page/window identity or normal window state changed during screenshot preparation: ${evidence.identity.missing.join('; ')}`);
            if(screenshotPageIdentity&&(screenshotPageIdentity.targetId!==after.target.targetId||screenshotPageIdentity.windowId!==after.window.windowId))
              throw Error('Screenshot page target/window does not match the previously captured game page.');
            screenshotPageIdentity??={targetId:after.target.targetId,windowId:after.window.windowId};
          }
          const bounds=await page.locator('iframe').boundingBox();
          const viewport=await page.evaluate(()=>({width:window.innerWidth,height:window.innerHeight}));
          evidence.clip=inspectViewportScreenshotClip({bounds,viewport});
          if(!evidence.clip.ready)throw Error(`Runtime iframe screenshot clip is outside or invalid for the viewport: ${evidence.clip.missing.join('; ')}`);
          await page.screenshot({path,clip:evidence.clip.clip});
          evidence.status='captured';
        }catch(error){evidence.status='unavailable';evidence.error=String(error.stack||error);}
        evidence.finishedAt=new Date().toISOString();record.screenshots??=[];record.screenshots.push(evidence);await save();return evidence;
      };
      const waitObservation=async(label,predicate,timeoutMs=30000)=>{const start=Date.now();let state,samples=0;do{state=await observe();samples++;if(predicate(state)){record.preconditions??=[];record.preconditions.push({label,samples,durationMs:Date.now()-start,state});return state;}await page.waitForTimeout(75);}while(Date.now()-start<timeoutMs);throw Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);};
      const waitObservationWithAudioClock=async(label,predicate,timeoutMs=30000)=>{const start=Date.now();let state,samples=0;do{state=await observeWithAudioClock();samples++;if(predicate(state)){record.preconditions??=[];record.preconditions.push({label,samples,durationMs:Date.now()-start,state});return state;}await page.waitForTimeout(75);}while(Date.now()-start<timeoutMs);throw Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);};
      const sampleMinimizedNativeInput=async(neutralValue,timeoutMs=1400)=>{
        const start=Date.now();let state=null,inputValue=null,samples=0,validSamples=0,readErrors=0,lastReadError=null;
        do{
          try{
            state=await observe();samples++;
            const value=state?.input?.value;
            if(Number.isSafeInteger(value)){
              validSamples++;inputValue=value;
              if(value===neutralValue)return{status:'neutral',state,inputValue,samples,validSamples,readErrors,
                durationMs:Date.now()-start,lastReadError};
            }
          }catch(error){readErrors++;lastReadError=String(error.stack||error);}
          await page.waitForTimeout(75);
        }while(Date.now()-start<timeoutMs);
        return{status:inputValue===null?'unobserved':'non-neutral',state,inputValue,samples,validSamples,readErrors,
          durationMs:Date.now()-start,lastReadError};
      };
      const gameplayReadiness=state=>titleFlow.gameplayReadiness(state,'desktop');
      const gameplayReady=state=>gameplayReadiness(state).ready;
      const waitForGameplayReadiness=async(epoch,label,{purpose,timeoutMs=5000}={})=>{
        if(purpose!=='startup'&&purpose!=='transition')throw new TypeError('Gameplay readiness call must declare startup or transition purpose');
        const start=Date.now();let state,samples=0;
        do{state=await observe();samples++;if(gameplayReady(state))break;await page.waitForTimeout(75);}while(Date.now()-start<timeoutMs);
        const readiness=gameplayReadiness(state);
        recordDesktopGameplayReadiness(record,{purpose,epoch,label,readiness,samples,durationMs:Date.now()-start,state});
        await save();
        return state;
      };
      const enterNativeGameplay=async(epoch,label=`native gameplay eligibility epoch ${epoch}`)=>{
        const controlledSceneRequired=controlledInput&&!ordinaryRunBoundaryFixture&&validationSceneRequest(target).contract;
        if(ordinaryRunBoundaryFixture){
          const request=await frame.evaluate(controlRunBoundary,runBoundaryRequest({...target,action:'arm'}));
          assert.equal(request.accepted,true,'The ordinary-run boundary fixture must arm before title navigation');
          (record.runBoundaryArms??=[]).push({epoch,request});
        }
        if(controlledSceneRequired&&controlledSceneArmedEpoch!==epoch){
          const armed=await frame.evaluate(controlValidationScene,
            validationSceneRequest({...target,action:'arm'}));
          (record.controlledSceneByEpoch??=[]).push({epoch,request:armed,acceptance:null,receipts:[]});await save();
          assertControlledSceneArmed(armed);controlledSceneArmedEpoch=epoch;
        }
        await titleFlow.enterGameplay({observe,press:async(code,holdMs=90)=>key(code,holdMs),
          waitUntil:(waitLabel,predicate,timeoutMs=30000)=>waitObservation(waitLabel,predicate,timeoutMs),
          lane:'desktop',epoch,label,timeoutMs:30000});
        let state=await waitForGameplayReadiness(epoch,label,{purpose:'startup'});
        if(controlledSceneRequired){
          if(!gameplayReady(state))throw Error(`The pre-armed controlled input world did not reach native gameplay readiness: ${JSON.stringify(gameplayReadiness(state))}`);
          const receipt=await frame.evaluate(controlValidationScene,validationSceneRequest(target));
          const entry=record.controlledSceneByEpoch?.find(item=>item.epoch===epoch);
          if(!entry)throw Error('Controlled input world has no pre-navigation arm record for this Runtime epoch');
          entry.acceptance=receipt;await save();
          assertControlledScene(receipt);
        }
        if(titleFlow.needsControlWarmup&&gameplayReady(state)){
          await key('ControlLeft',250);state=await observe();
        }
        return state;
      };
      const waitRuntimeEvent=async(eventName,epoch,label,timeoutMs=120000)=>{await page.waitForFunction(({eventName,epoch})=>window.events.some(event=>event?.event===eventName&&event.epoch===epoch),{eventName,epoch},{timeout:timeoutMs});return page.evaluate(({eventName,epoch})=>window.events.find(event=>event?.event===eventName&&event.epoch===epoch),{eventName,epoch});};
      const playwrightKey=code=>/^Key[A-Z]$/.test(code)?code.slice(3).toLowerCase():code==='ShiftLeft'?'Shift':code==='ControlLeft'?'Control':code;
      const heldKeys=new Set();
      const keyDown=async code=>{await page.keyboard.down(playwrightKey(code));heldKeys.add(code);};
      const keyUp=async code=>{if(!heldKeys.has(code))return;try{await page.keyboard.up(playwrightKey(code));}finally{heldKeys.delete(code);}};
      const key=async(code,holdMs=90)=>{try{await keyDown(code);await page.waitForTimeout(holdMs);}finally{await keyUp(code);}await page.waitForTimeout(100);};
      const readNativeTouchBoundary=()=>readTouchMotionBoundary({
        readTrace:()=>frame.evaluate(readTouchMotionTrace,runtimeObservationRequest(target)),
        readObservation:activeQueuedMovementPolicy?
          ()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target)):undefined,
        policy:activeQueuedMovementPolicy,
      });
      const touchSceneKey=async(code,down)=>{
        const clockBoundary=await frame.evaluate(game=>globalThis.__eaglerTouchMotionClock?.game===game,
          target.game);
        const readBridge=()=>frame.evaluate(code=>{
          const bridge=window.__eaglerKeyboardInputBridge;
          return bridge?{active:bridge.active?.()===true,held:bridge.keyDown?.(code)===true}:null;
        },code);
        const physical=await sendOwnedPhysicalKey({heldKeys,code,down,readBridge,dispatch:async(code,down)=>{
          const start=await page.evaluate(()=>keyboardMapEvents.length);
          if(down)await page.keyboard.down(playwrightKey(code));else await page.keyboard.up(playwrightKey(code));
          const receipt={start,code,down,epoch:activeEpoch};
          await page.waitForFunction(({start,code,down,epoch})=>keyboardMapEvents.slice(start).some(row=>
            row.epoch===epoch&&row.source==='physical'&&row.code===code&&row.down===down),receipt,{timeout:1500});
          return page.evaluate(({start,code,down,epoch})=>keyboardMapEvents.slice(start).find(row=>
            row.epoch===epoch&&row.source==='physical'&&row.code===code&&row.down===down),receipt);
        }});
        const boundary=await readNativeTouchBoundary();
        if(!boundary.trace.available)throw Error(`Cannot establish keyboard release boundary: ${boundary.trace.reason}`);
        return{acknowledged:true,source:'Runtime KeyboardInputBridge',...physical,
          nativeTraceTotal:boundary.trace.total,stableTraceBoundary:clockBoundary&&boundary.stableTraceBoundary,
          nextMovementInputBoundary:boundary.nextMovementInputBoundary};
      };
      const ensureFrame=()=>{const candidate=page.frames().find(item=>item.url().includes(`/runtime/${target.id}/`));if(!candidate)throw Error(`${target.id}: Runtime iframe is missing`);frame=candidate;return frame;};
      const acquireForeground=(epoch,stageName,{runtime=false}={})=>stage(stageName,{epoch,
        focusKind:headlessProbe?'headless-page-visibility':'native-window-foreground'},async()=>{
        const focusSample=async target=>{try{return await nativeFocusState(target);}catch(error){return{readError:String(error.message||error)};}};
        const hostBefore=await focusSample(page),runtimeBefore=runtime?await focusSample(frame):null;
        let identityBefore=null,identityAfter=null,identityCheck=null;
        if(!headlessProbe){
          identityBefore=await readPageTargetWindow(cdp,page);
          const expected=expectedNativePageIdentity??identityBefore;
          const preRestoreIdentity=inspectNativeForegroundIdentity({expected,observed:identityBefore,requireNormal:false});
          assert.ok(preRestoreIdentity.ready,`Cannot restore foreground because the original page/window identity is unavailable: ${preRestoreIdentity.missing.join('; ')}`);
          await cdp.send('Browser.setWindowBounds',{windowId:identityBefore.window.windowId,bounds:{windowState:'normal'}});
        }
        await page.bringToFront();
        if(runtime){ensureFrame();await frame.locator('canvas').focus();}
        const host=await waitForNativeFocus(page,5000),runtimeState=runtime?await waitForNativeFocus(frame,5000):null;
        if(!headlessProbe){
          identityAfter=await readPageTargetWindow(cdp,page);
          identityCheck=inspectNativeForegroundIdentity({expected:expectedNativePageIdentity??identityBefore,observed:identityAfter,requireNormal:true});
        }
        const proof={epoch,stage:stageName,kind:headlessProbe?'headless-page-visibility':'native-window-foreground',
          before:{host:hostBefore,runtime:runtimeBefore},after:{host,runtime:runtimeState},
          nativeWindow:headlessProbe?{status:'not-claimed',reason:'software input execution observes page state without asserting a desktop window'}:
            {before:identityBefore,after:identityAfter,identityCheck}};
        (record.startupForeground??=[]).push(proof);await save();
        if(!headlessProbe)assert.equal(identityCheck.ready,true,`Foreground restore must retain the same actual page and native window in normal state: ${identityCheck.missing.join('; ')}`);
        if(!headlessProbe&&epoch===1&&stageName==='host-focus-before-ready'&&!host.ready)
          throw new DesktopStartupPreconditionError('The initial native browser document is not visible and focused; desktop lifecycle operations have not started.');
        assert.equal(host.ready,true,headlessProbe?'Host document must be visible and focused before the next startup stage':'Host document must be natively visible and focused before the next startup stage');
        if(runtime)assert.equal(runtimeState.ready,true,headlessProbe?'Runtime document must be visible and focused before the next startup stage':'Runtime document must be natively visible and focused before the next startup stage');
        if(!headlessProbe){
          if(!expectedNativePageIdentity){
            expectedNativePageIdentity={status:'matched',target:{type:'page',targetId:identityAfter.target.targetId},
              window:{windowId:identityAfter.window.windowId}};
            record.nativePageIdentity={targetId:identityAfter.target.targetId,windowId:identityAfter.window.windowId};
          }
        }
        return proof;
      });
      const flushFixtureAudit=async()=>{
        if(!fixtureAudit)return;
        try{await fixtureAudit.flush();}
        finally{record.fixtureResponses=fixtureAudit.responses;record.fixtureResponseErrors=fixtureAudit.errors;await save();}
      };
      const cleanupKeys=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','w','a','s','d','KeyZ','KeyX','ShiftLeft','ControlLeft','Escape','Enter'];
      const resetInputStateCore=async(epoch=activeEpoch)=>{
        if(!page)return;
        const required=record.stages.some(item=>item.stage==='keyboard-bridge'&&item.epoch===epoch&&item.status==='passed');
        const keyErrors=[];
        for(const code of cleanupKeys)await keyUp(code).catch(error=>keyErrors.push(`${code}: ${error.message}`));
        let result=null,evaluationError=null;
        if(required)try{result=await page.evaluate(async({game,epoch})=>{
          const runtimeFrame=document.querySelector('iframe')?.contentWindow,bridge=runtimeFrame?.__eaglerKeyboardInputBridge;
          window.__desktopKeyboardOwner?.clear();
          const keyboardCleared=bridge?.submit({protocol:'eagler-touhou/1',game,epoch,command:'keyboard-clear'})??false;
          const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
          const sendErrors=[];
          try{send('keyboard-clear');send('touch-cancel');send('touch-controls',neutral);}
          catch(error){sendErrors.push(String(error.message||error));}
          const acknowledgements=await Promise.all([
            rpc('touch-cancel',{},750).then(()=>true).catch(()=>false),
            rpc('touch-controls',neutral,750).then(()=>true).catch(()=>false),
          ]);
          return{keyboardCleared,liveRuntime:!!bridge,touchCancelAcknowledged:acknowledgements[0],touchControlsAcknowledged:acknowledgements[1],sendErrors};
        },{game:target.game,epoch});}catch(error){evaluationError=String(error.message||error);}
        const cleanup={epoch,...inspectDesktopInputCleanup({required,result,keyErrors,evaluationError})};
        (record.inputCleanup??=[]).push(cleanup);
        if(cleanup.status==='failed')record.failure??=`Input cleanup failed: ${cleanup.missing.join('; ')}`;
        await save();return cleanup;
      };
      resetInputState=async(epoch=activeEpoch,phase='resetInputState')=>
        runLifecyclePhase(phase,()=>resetInputStateCore(epoch),{activeEpoch:epoch});
      try{
        await check('package-and-private-data-identity',async()=>{const packageIdentity=await verifyRuntimePackage(target,{repoRoot:root});const dataIdentity=await verifyPrivateData(target.data);packages.set(target.id,packageIdentity);dataIdentities.set(target.id,dataIdentity);if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256);record.packageIdentity=packageIdentity;record.dataIdentity=dataIdentity;return{packageSha256:packageIdentity.packageSha256,sourceBuild:packageIdentity.sourceBuild,privateData:dataIdentity};});
        if(record.checks.at(-1).status!=='passed'){record.status='failed';record.finishedAt=new Date().toISOString();await save();continue;}
        cdp=await browser.newBrowserCDPSession();
        context=await runLifecyclePhase('target-context-create',()=>browser.newContext({viewport:{width:1100,height:820}}),{activeEpoch});
        if(headlessLifecycle)detachHeadlessContext=headlessLifecycle.attachContext(context);
        if(replayRecordingOnly||actionsOnly||inputOnly)await prepareVirtualGamepad(context,await gamepadProbeBundle());
        if(audioLatencyOnly){
          assert.ok(sfxContract,`No SFX latency adapter for ${target.game}/${target.variant}`);
          const sources={};record.sfxProductionSources=[];
          for(const [name,path] of Object.entries(sfxContract.sourcePaths)){
            const bytes=await readFile(resolve(root,path));sources[name]=bytes.toString('utf8');
            const artifactPath=`${target.id}-sfx-source-${name}.txt`;
            await writeFile(resolve(output,artifactPath),bytes);
            record.sfxProductionSources.push({path,artifactPath,bytes:bytes.length,sha256:hash(bytes)});
          }
          record.sfxContract=deriveSfxLatencyContract(target.game,target.variant,sources);
          await context.addInitScript({content:`if(window!==window.top){${sfxLatencyProbeInitSource(sfxContract)}}`});
        }
        const runtimeMountInit=audioRuntimeMount?`window.__eaglerDesktopAudioRuntimeMount=${JSON.stringify({game:target.game,variant:target.variant,runtime:audioRuntimeMount})};`:'';
        await context.addInitScript({content:`if(window!==window.top){${runtimeMountInit}${audioProbeSource}window.__desktopRawKeyboardEvents=[];for(const name of ['keydown','keyup'])window.addEventListener(name,event=>window.__desktopRawKeyboardEvents.push({type:name,code:event.code,key:event.key,keyCode:event.keyCode,isTrusted:event.isTrusted,target:event.target?.tagName??null,time:performance.now()}),true);const type=()=>({desktopProbe:true,game:new URL(location.href).pathname.split('/')[2],epoch:Number(new URLSearchParams(location.search).get('runtimeEpoch'))});for(const name of ['blur','focus','visibilitychange','pagehide','pageshow']){const target=name==='visibilitychange'?document:window;target.addEventListener(name,()=>{try{parent.postMessage({...type(),type:name,visibility:document.visibilityState,hidden:document.hidden,time:performance.now()},location.origin);}catch{}},true);}}`});
        page=await runLifecyclePhase('target-page-create',()=>context.newPage(),{activeEpoch});
        if(headlessLifecycle)detachHeadlessPage=headlessLifecycle.attachPage(page);
        page.on('pageerror',error=>record.errors.push({page:String(error)}));page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
        fixtureAudit=await createFixtureResponseProbe(page,{base:fixture.base,expectedResponses,output,label:target.id});
        record.fixtureResponseCapture=fixtureAudit.capture;
        record.fixtureResponses=fixtureAudit.responses;record.fixtureResponseErrors=fixtureAudit.errors;
        const hostUrl=`${fixture.base}/host/${target.id}`;
        await stage('host-navigation',{url:hostUrl,epoch:1},()=>page.goto(hostUrl));await flushFixtureAudit();
        await page.waitForFunction(()=>window.__eaglerDesktopAudioProbe&&window.__desktopKeyboardPreferences);await page.evaluate(()=>{document.body.tabIndex=-1;});
        await acquireForeground(1,'host-focus-before-ready');
        const ready=await stage('ready',{epoch:1,event:'ready'},async()=>{const event=await waitRuntimeEvent('ready',1,'desktop-ready');await flushFixtureAudit();return event;});ensureFrame();
        assert.equal(ready.protocol,'eagler-touhou/1');assert.equal(ready.game,target.game);assert.equal(ready.epoch,1);
        await stage('keyboard-bridge',{epoch:1},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
        await page.evaluate(({game,epoch})=>window.__desktopInstallKeyboardMapping({game,epoch,layout:'arrows'}),{game:target.game,epoch:1});
        const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
        const configureEpoch=async(mode,focusMode='hold-button',extraOptions={})=>{
          const options={alwaysHitbox:false,thpracEnabled:false,limitPresentationTo60:false,...(target.options??{}),
            touchEnabled:true,touchMovementMode:mode,touchFocusMode:focusMode,doubleTapBombEnabled:false,...extraOptions};
          const acknowledgement=await page.evaluate(({sharedResources,options})=>rpc('configure',{
            language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,thpracCatalog:[],options}),{sharedResources,options});
          const receipt={epoch:activeEpoch,options,acknowledgement,source:'Runtime Shell configure command | before launch'};
          (record.launchOptionsByEpoch??=[]).push(receipt);return receipt;
        };
        const prepareForeground=epoch=>acquireForeground(epoch,'foreground',{runtime:true});
        const prepareLaunchForeground=epoch=>acquireForeground(epoch,'launch-foreground',{runtime:true});
        const launchEpoch=async(epoch,mode,focusMode,extraOptions={},preparation={})=>{
          activeEpoch=epoch;
          await page.evaluate(({game,epoch})=>window.__desktopInstallKeyboardMapping({game,epoch,layout:'arrows'}),{game:target.game,epoch});
          await prepareForeground(epoch);
          await stage('configure',{epoch,mode,focusMode},()=>configureEpoch(mode,focusMode,extraOptions));
          await prepareLaunchForeground(epoch);
          await stage('launch',{epoch},()=>page.evaluate(()=>rpc('launch')));
          await stage('first-frame',{epoch,event:'first-frame'},async()=>{const event=await waitRuntimeEvent('first-frame',epoch,`desktop-first-frame-${epoch}`);await flushFixtureAudit();return event;});
          ensureFrame();await page.evaluate(()=>document.body.focus());
          return preparation.titleOnly?observe():enterNativeGameplay(epoch);
        };
        await prepareForeground(1);
        await stage('configure',{epoch:1,mode:'touch'},()=>configureEpoch('touch',actionsOnly?'toggle-button':'hold-button'));
        await prepareLaunchForeground(1);
        await stage('launch',{epoch:1},()=>page.evaluate(()=>rpc('launch')));
        await stage('first-frame',{epoch:1,event:'first-frame'},async()=>{const event=await waitRuntimeEvent('first-frame',1,'desktop-first-frame');await flushFixtureAudit();return event;});
        await page.evaluate(()=>document.body.focus());
        const canvas=await nativePeek();assert.ok(canvas.canvas?.width>0&&canvas.canvas?.height>0);record.initial={ready,canvas,audioContexts:canvas.audioContexts,audioProbe:canvas.audioProbe};
        await captureRuntimeScreenshot('title',resolve(output,`${target.id}-title.png`));
        if(audioLatencyOnly){
          const readAudioScene=()=>frame.evaluate(readSfxLatencySceneState,sfxLatencySceneRequest(target));
          const waitAudioScene=async(expected,timeout=15000)=>{
            const start=Date.now();let state;
            do{state=await readAudioScene();
              if(state.available&&Object.entries(expected).every(([key,value])=>state[key]===value))return state;
              await page.waitForTimeout(25);
            }while(Date.now()-start<timeout);
            const error=new Error(`Native audio scene did not become ready: ${JSON.stringify({expected,state})}`);
            error.evidence={expected,state};throw error;
          };
          const audioScenes=resolveSfxLatencyScenes(sfxContract);
          const firstScene=audioScenes.positive;
          const prepared=await waitAudioScene({...firstScene.before,invalid:0,loading:0,bridgeActive:true,audioState:'running'});
          record.audioTitleReadinessByEpoch=[{epoch:activeEpoch,ready:true,reasons:[],state:prepared}];await save();
          const runtimeMapping=sfxLatencySceneRequest(target).contract.runtime;
          record.sfxInstallation=await frame.evaluate(async({mapping,probeGlobal})=>{
            const runtime=globalThis[mapping.global];return globalThis[probeGlobal].install(runtime?.[mapping.module]);
          },{mapping:runtimeMapping,probeGlobal:sfxContract.probeGlobal});
          assert.equal(record.sfxInstallation.installed,true);
          assert.deepEqual(record.sfxInstallation.targets.map(row=>row.handle).sort((a,b)=>a-b),[...sfxContract.targetHandles].sort((a,b)=>a-b));
          let positiveTransitionObserved=false;
          for(const scene of [audioScenes.positive,audioScenes.negative])await check(scene.checkId,async()=>{
            if(scene.role==='negative-control'&&!positiveTransitionObserved)return{checkStatus:'incomplete',
              reason:'The positive scene did not establish the native state required by the same-voice negative control',
              state:await readAudioScene(),partial:true};
            const before=await waitAudioScene({...scene.before,invalid:0,loading:0,bridgeActive:true,audioState:'running'});
            await frame.waitForFunction(({mapping})=>{
              const runtime=globalThis[mapping.global],pool=runtime?.[mapping.module]?.eaglerSfxOutput;
              return pool?.voices instanceof Map&&[...pool.voices.values()].every(voice=>!voice.playing&&!voice.node);
            },{mapping:runtimeMapping},{timeout:15000});
            await frame.locator('canvas').focus();
            const opened=await frame.evaluate(({probeGlobal,inputFilter,handle,mutation})=>{
              const probe=globalThis[probeGlobal],session=probe.begin();probe.armInputCapture(inputFilter);
              if(mutation)probe.armQueueTailMutation(handle);return session;
            },{...sfxContract,mutation:scene.role==='negative-control'});
            let snapshot,sourceLead,after;
            try{
              await key(scene.input,45);
              await frame.waitForFunction(({probeGlobal,sessionId})=>{
                const snapshot=globalThis[probeGlobal].snapshot();
                return snapshot.sessionId===sessionId&&snapshot.observations.length>0;
              },{probeGlobal:sfxContract.probeGlobal,sessionId:opened.session.id},{timeout:15000});
              snapshot=await frame.evaluate(probeGlobal=>globalThis[probeGlobal].snapshot(),sfxContract.probeGlobal);
              const events=snapshot.nativeTrace?.events?.filter(row=>row.logicalIndex===sfxContract.logicalIndex&&row.handle===sfxContract.handle)??[];
              if(events.length===1)sourceLead=await frame.evaluate(({probeGlobal,eventSerial})=>globalThis[probeGlobal].measureSourceLead(eventSerial),
                {probeGlobal:sfxContract.probeGlobal,eventSerial:events[0].serial});
              after=await waitAudioScene(scene.after);
              if(scene.role==='positive')positiveTransitionObserved=true;
              const observations=snapshot.observations.filter(row=>row.eventSerial===events[0]?.serial&&row.sessionId===snapshot.sessionId);
              const input={contract:record.sfxContract,trace:snapshot.nativeTrace,session:snapshot.session,
                observation:observations.length===1?observations[0]:null,sourceLead};
              const evidence={scene:scene.name,role:scene.role,targetVoice:scene.targetVoice,
                inputPath:'browser iframe keydown | production native Shell listener',before,after,
                opened,snapshot,sourceLead,partial:true};
              if(scene.role==='negative-control'){
                try{const rejected=assertQueueTailMutationIsRejected({...input,mutation:snapshot.mutation});
                  return{...evidence,negativeControl:{mutation:snapshot.mutation,rejectedByUnchangedClassifier:rejected}};
                }catch(error){error.evidence=evidence;throw error;}
              }
              const verdict=classifySfxLatencyEvidence(input);
              return{...evidence,verdict,checkStatus:verdict.status};
            }catch(error){
              const failureState=await readAudioScene().catch(readError=>({readError:String(readError)}));
              error.evidence={...error.evidence,scene:scene.name,before,after,failureState,opened,snapshot: snapshot??await frame.evaluate(
                probeGlobal=>globalThis[probeGlobal]?.snapshot(),sfxContract.probeGlobal).catch(()=>null),sourceLead};
              throw error;
            }finally{
              await keyUp(scene.input).catch(()=>{});
              await frame.evaluate(probeGlobal=>globalThis[probeGlobal]?.restoreQueueTailMutation(),sfxContract.probeGlobal).catch(()=>{});
            }
          });
        }else{
        let started=await enterNativeGameplay(activeEpoch,'native gameplay eligibility epoch 1');
        record.gameplayStart=started;
        const touchControls=async(fields={})=>{
          const controls={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100,...fields};
          if(suppressTouchMovement&&(controls.joystickX||controls.joystickY)){
            (record.suppressedTouchMovement??=[]).push({epoch:activeEpoch,command:'touch-controls',requested:{...controls}});
            controls.joystickX=controls.joystickY=0;
          }
          const acknowledgement=await page.evaluate(value=>rpc('touch-controls',value,5000),controls);
          return{...acknowledgement,acknowledged:acknowledgement?.ok===true};
        };
        const cancelTouch=()=>page.evaluate(()=>rpc('touch-cancel'));
        const directTouch=async(type,id,x,y)=>{
          if(suppressTouchMovement){(record.suppressedTouchMovement??=[]).push({epoch:activeEpoch,command:'direct-touch',type,id,x,y});return;}
          const point=await frame.locator('canvas').evaluate((canvas,{x,y})=>{const rect=canvas.getBoundingClientRect();return{x:(rect.left+x*rect.width)/innerWidth,y:(rect.top+y*rect.height)/innerHeight};},{x,y});
          const acknowledgement=await page.evaluate(value=>rpc('direct-touch',value,5000),{type,id,...point});
          return{...acknowledgement,acknowledged:acknowledgement?.ok===true};
        };
        const nativeTouchBatch=async operations=>{
          const installed=await frame.evaluate(()=>Boolean(window.__eaglerTouchBatch));
          if(!installed){
            const content=`(()=>{const readers={controlTouchMotionClock:${controlTouchMotionClock.toString()},readTouchActionState:${readTouchActionState.toString()},controlValidationScene:${controlValidationScene.toString()},readRuntimeObservation:${readRuntimeObservation.toString()}};window.__eaglerTouchBatch={run:${runTouchBatch.toString()},readers};})()`;
            await frame.addScriptTag({content});
          }
          return page.evaluate(async args=>{
            const fixture=document.querySelector('iframe').contentWindow.__eaglerTouchBatch;
            return fixture.run(args,fields=>rpc('direct-touch',fields,5000),fixture.readers);
          },{operations,clockRequest:touchClockRequest({...target,action:'tick'}),actionRequest:touchActionsRequest(target),
            sceneRequest:validationSceneRequest(target),observationRequest:runtimeObservationRequest(target)});
        };
        const runSharedTouch=async(mode,{focusMode='hold-button',scenarioSet='all'}={})=>{
          const movementContract=touchMovementContract(target.game,target.variant);
          const requiresQueuedMovementPolicy=movementContract.inputBoundary==='queued-for-next-movement';
          if(!['queued-for-next-movement','synchronous-consumption'].includes(movementContract.inputBoundary)){
            const reason=`Native movement input boundary is not verified for ${target.game}/${target.variant}`;
            record.touchTraceUnavailable=reason;return{checkStatus:'incomplete',reason,epoch:activeEpoch,movementContract};
          }
          const queuedMovementPolicy=requiresQueuedMovementPolicy?movementContract.keyboardReleasePolicy:null;
          if(requiresQueuedMovementPolicy&&!queuedMovementPolicy){
            const reason='Strict touch motion has no native queued-movement release policy for this Runtime target';
            record.touchTraceUnavailable=reason;return{checkStatus:'incomplete',reason,epoch:activeEpoch};
          }
          activeQueuedMovementPolicy=queuedMovementPolicy;
          const readTrace=async()=>{
            const boundary=await readNativeTouchBoundary();
            return boundary.trace.available?{available:true,...boundary.trace,
              stableTraceBoundary:boundary.stableTraceBoundary,
              nextMovementInputBoundary:boundary.nextMovementInputBoundary}:boundary.trace;
          };
          const initial=await readTrace();
          if(!initial.available){record.touchTraceUnavailable=initial.reason;return{checkStatus:'incomplete',reason:initial.reason,epoch:activeEpoch};}
          if(initial.abiVersion!==2){
            const reason=`Strict touch motion requires native ABI v2; package exposes ABI v${initial.abiVersion??'unknown'}`;
            record.touchTraceUnavailable=reason;return{checkStatus:'incomplete',reason,epoch:activeEpoch};
          }
          if(initial.localSeat!==0||initial.executionKind!==0){
            const reason='Strict touch motion requires the native local-seat 0 forward-execution stream';
            record.touchTraceUnavailable=reason;return{checkStatus:'incomplete',reason,epoch:activeEpoch};
          }
          if(queuedMovementPolicy&&initial.nextMovementInputBoundary?.available!==true){
            const reason=initial.nextMovementInputBoundary?.reason??'Strict touch motion requires the declared native queued-movement observation';
            record.touchTraceUnavailable=reason;return{checkStatus:'incomplete',reason,epoch:activeEpoch};
          }
          const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));
          if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:
            'Strict keyboard release checks require the native movement clock to pause between acknowledged key transitions',
            epoch:activeEpoch,clock};
          try{
            const controlledScene=controlledInput&&validationSceneRequest(target).contract?{
              request:record.controlledSceneByEpoch?.find(item=>item.epoch===activeEpoch)?.request??null,
              acceptance:assertControlledScene(await frame.evaluate(controlValidationScene,
                validationSceneRequest(target))),receipts:[]}:null;
            if(controlledScene&&!controlledScene.request)throw Error('Strict input scene has no pre-navigation native arm request');
            let previousScene=controlledScene?.acceptance;
            const evidence=await runTouchMotionSceneGroup({readTrace,controls:touchControls,
              pointer:(type,id,x,y)=>type==='cancel'?cancelTouch():directTouch(type,id,x,y),
              key:touchSceneKey,
              cancel:cancelTouch,
              wait:async milliseconds=>{
                if(clock.mode==='native-manual'){
                  await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));
                  if(controlledScene){
                    const receipt=assertControlledScene(await frame.evaluate(controlValidationScene,
                      validationSceneRequest(target)),previousScene);
                    controlledScene.receipts.push(receipt);previousScene=receipt;
                  }
                }
                else await page.waitForTimeout(milliseconds);
              }},{mode,focusMode,scenarioSet,inputSpace:{width:640,height:480},precision:movementContract.precision,
              inputCommand:movementContract.inputCommand,inputBoundary:movementContract.inputBoundary,
              inputBoundarySource:movementContract.inputBoundarySource,queuedMovementPolicy});
            return{...evidence,epoch:activeEpoch,input:'runtime-touch-protocol',clock,movementContract,controlledScene};
          }finally{
            await resetInputState(activeEpoch);
            if(clock.mode==='native-manual')record.touchClock??=[];
            if(clock.mode==='native-manual')record.touchClock.push(await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'})));
          }
        };
        await check('shared-touch-limited',()=>runSharedTouch(1));
        if(actionsOnly)await check('launcher-touch-native-consumers',async()=>{
          const request=touchActionsRequest(target),movement=touchMovementContract(target.game,target.variant);
          if(!request.contract||movement.keyboardReleasePolicy?.maxMovementTicks!==1)
            return{checkStatus:'incomplete',reason:'Declare the native action observer and one-update input queue for the DOM scene'};
          const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));
          if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Production DOM input scenes require the native manual clock',clock};
          let controlledScene,previous,failure;
          try{
            controlledScene={acceptance:assertControlledScene(await frame.evaluate(controlValidationScene,validationSceneRequest(target))),receipts:[]};
            previous=controlledScene.acceptance;
            const fixture=await page.evaluate(value=>EaglerLauncherTouchFixture.install(value),{game:target.game,epoch:activeEpoch});
            await bindProtocolHostGamepad(page,target.game,activeEpoch);
            const evidence=await runLauncherTouchScenes({
              pointer:value=>page.evaluate(value=>EaglerLauncherTouchFixture.pointer(value),value),
              cancel:()=>page.evaluate(()=>EaglerLauncherTouchFixture.cancel()),
              key:touchSceneKey,observe,readActions:()=>frame.evaluate(readTouchActionState,request),
              gamepad:async pressed=>{
                await page.evaluate(pressed=>EaglerAcceptanceInputs.virtualButton(acceptancePad,0,pressed?1:0),pressed);
                await frame.waitForFunction(pressed=>Boolean(window.__eaglerGamepadMapping?.sample?.(0)&1)===pressed,
                  pressed,{timeout:2000});
              },
              step:async ticks=>{for(let n=0;n<ticks;n++){
                await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));
                const receipt=assertControlledScene(await frame.evaluate(controlValidationScene,validationSceneRequest(target)),previous);
                controlledScene.receipts.push(receipt);previous=receipt;
              }},
            },{fireMode:PRODUCT_GAMES[target.game].touchFire.mode});
            return{...evidence,fixture,epoch:activeEpoch,clock,controlledScene};
          }catch(error){failure=error;throw error;}
          finally{
            const cleanupErrors=[];
            for(const [operation,perform] of [
              ['dispose-controllers',()=>page.evaluate(()=>{
                window.acceptanceGamepadService?.dispose?.();delete window.acceptanceGamepadService;
                EaglerLauncherTouchFixture.dispose();
              })],
              ['release-inputs',()=>resetInputState(activeEpoch)],
              ['restore-clock',async()=>{record.touchClock??=[];
                record.touchClock.push(await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'})));}],
            ])try{await perform();}catch(error){cleanupErrors.push({operation,error:String(error)});}
            if(cleanupErrors.length){
              if(failure)failure.cleanupErrors=cleanupErrors;
              else throw Error(`Launcher touch scene cleanup failed: ${JSON.stringify(cleanupErrors)}`);
            }
          }
        });
        if(actionsOnly)await check('shared-touch-actions',async()=>{
          const request=touchActionsRequest(target);
          if(!request.contract)return{checkStatus:'incomplete',reason:'Register the actual native player-consumer action observer'};
          const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));
          if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Native action scenes require the production manual clock',clock};
          let controlledScene,previousScene,previousPause,evidence,failure;
          try{
            controlledScene={acceptance:assertControlledScene(await frame.evaluate(controlValidationScene,validationSceneRequest(target))),receipts:[]};
            previousScene=controlledScene.acceptance;previousPause=(await observe()).pause;
            evidence=await runTouchActionSceneGroup({controls:touchControls,directTouch,observe,
              readActions:()=>frame.evaluate(readTouchActionState,request),
              step:async ticks=>{
                assert.ok(Number.isInteger(ticks)&&ticks>0&&ticks<=600,'Action step count is outside the declared scene budget');
                for(let n=0;n<ticks;n++){
                  await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));
                  const receipt=await frame.evaluate(controlValidationScene,validationSceneRequest(target));
                  const pause=(await observe()).pause;
                  if(pause?.active===true)
                    assertControlledScenePauseBoundary(receipt,previousScene,{before:previousPause,after:pause});
                  else assertControlledScene(receipt,previousScene);
                  previousPause=pause;
                  controlledScene.receipts.push(receipt);previousScene=receipt;
                }
              }},{inputMasks:request.contract.inputMasks,escapePulseTicks:request.contract.escapePulseTicks});
            return{...evidence,complete:false,epoch:activeEpoch,clock,controlledScene,
              scope:'Runtime touch protocol | actual player-consumer actions | Launcher button DOM has its own integration scope'};
          }catch(error){failure=error;throw error;}
          finally{
            const cleanup=evidence?.cleanupControls??failure?.touchActionCleanupControls;
            if(cleanup)await touchControls(cleanup);
            record.touchClock??=[];
            record.touchClock.push(await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'})));
          }
        });
        if(actionsOnly)await check('shared-touch-restart',async()=>{
          if(!audioRuntimeMount)return{checkStatus:'incomplete',reason:'The selected Runtime has no declared mount identity for restart evidence'};
          const initial=await observe(),restartFrame=initial.capabilities?.restartFrame;
          if(!initial.capabilities?.pause||!restartFrame||!Number.isFinite(initial.frames?.[restartFrame]))
            return{checkStatus:'incomplete',reason:'The selected Runtime does not expose native pause and restart-frame observations'};
          const focusRestartHost=async(source)=>{
            const focus=await focusRestartKeyboardHost({bringToFront:()=>page.bringToFront(),
              focusHostBody:()=>page.evaluate(()=>document.body.focus()),
              readFocus:()=>page.evaluate(()=>{
                const runtimeFrame=document.querySelector('iframe'),active=document.activeElement;
                return{documentHasFocus:document.hasFocus(),visibilityState:document.visibilityState,hidden:document.hidden,
                  activeElementTag:active?.tagName??null,activeElementIsBody:active===document.body,
                  activeElementIsRuntimeFrame:active===runtimeFrame};
              })});
            record.restartKeyboardFocus??=[];
            record.restartKeyboardFocus.push({source,epoch:activeEpoch,...focus});await save();
            return focus;
          };
          const focusIssue=(focus,source)=>focus.status==='ready'?null:
            {checkStatus:'incomplete',reason:`${source} could not place the visible host document body at the physical keyboard target`,focus};
          let fixture=null,failure;
          try{
          fixture=await page.evaluate(({game,epoch})=>
            EaglerLauncherTouchFixture.install({game,epoch}),{game:target.game,epoch:activeEpoch});
          const captureIdentity=()=>page.evaluate(({epoch,mount})=>{
            const frame=document.querySelector('iframe'),runtimeWindow=frame?.contentWindow;
            if(!frame||!runtimeWindow)return null;
            const tokens=window.__eaglerTouchRestartIdentity??(window.__eaglerTouchRestartIdentity={ids:new WeakMap(),next:0});
            const id=value=>{
              if(!value||(typeof value!=='object'&&typeof value!=='function'))return null;
              let token=tokens.ids.get(value);
              if(!token){token=`identity-${++tokens.next}`;tokens.ids.set(value,token);}
              return token;
            };
            const runtime=runtimeWindow[mount.global],module=runtime?.[mount.module]??null,core=runtime?.[mount.core]??null;
            const firstFrames=(window.events??[]).filter(event=>event?.event==='first-frame'&&event.epoch===epoch);
            return{hostPageId:id(window),hostTimeOrigin:performance.timeOrigin,frameDocumentId:id(frame.contentDocument),
              frameUrl:runtimeWindow.location.href,epoch:Number(new URLSearchParams(runtimeWindow.location.search).get('runtimeEpoch')),
              runtimeId:id(runtime),moduleId:id(module),coreId:id(core),firstFrameId:id(firstFrames[0]??null),
              firstFrameCount:firstFrames.length};
          },{epoch:activeEpoch,mount:audioRuntimeMount});
          const runRestart=async({source}={})=>{
            const pauseInputFocus=await focusRestartHost(`${source}-pause-input`),pauseFocusIssue=focusIssue(pauseInputFocus,source);
            if(pauseFocusIssue)return pauseFocusIssue;
            const before=await waitForGameplayReadiness(activeEpoch,`${source} restart precondition`,{purpose:'transition'});
            if(!gameplayReady(before))return{checkStatus:'incomplete',reason:`${source} restart did not begin in native gameplay`};
            if(before.capabilities?.restartFrame!==restartFrame||!Number.isFinite(before.frames?.[restartFrame])||
              before.capabilities?.pause!==true)
              return{checkStatus:'incomplete',reason:'Native pause and restart-frame observations are not available before input'};
            const beforeIdentity=await captureIdentity();
            if(!beforeIdentity)return{checkStatus:'incomplete',reason:'Could not identify the current Runtime iframe'};
            const identityPreflight=compareRuntimeRestartIdentity(beforeIdentity,beforeIdentity,{expectedEpoch:activeEpoch});
            if(identityPreflight.status==='incomplete')return{checkStatus:'incomplete',reason:identityPreflight.reason,identity:identityPreflight};
            if(identityPreflight.status!=='same')throw Error(identityPreflight.reason);
            await key('Escape');
            const paused=await waitObservation(`${source} native pause`,state=>state.pause?.active===true,10000);
            if(paused.capabilities?.pause!==true||paused.pause?.active!==true||
              !Number.isFinite(paused.frames?.[restartFrame]))
              return{checkStatus:'incomplete',reason:'The native paused snapshot lacks its restart-frame boundary.',before,paused};
            await page.waitForTimeout(250);
            const pauseStable=await observe();
            if(pauseStable.capabilities?.pause!==true||pauseStable.pause?.active!==true||
              !Number.isFinite(pauseStable.frames?.[restartFrame]))
              return{checkStatus:'incomplete',reason:'The stable paused snapshot lacks its native restart frame.',before,paused,pauseStable};
            const restartInputFocus=await focusRestartHost(`${source}-restart-input`),restartFocusIssue=focusIssue(restartInputFocus,source);
            if(restartFocusIssue)return{...restartFocusIssue,before,paused,pauseStable};
            const restartKeyIndex=await page.evaluate(()=>window.keyboardForwardEvents.length);
            const physicalFlowCodes=[];
            const physicalFlowPress=async code=>{physicalFlowCodes.push(code);return key(code);};
            let buttonEvidence=null;
            const press=source==='launcher-touch-button'?createLauncherRestartPress({press:physicalFlowPress,
              requestRestart:async()=>{
                const id=73000+activeEpoch;
                const down=await page.evaluate(id=>EaglerLauncherTouchFixture.pointer({control:'restart',type:'pointerdown',id}),id);
                const up=await page.evaluate(id=>EaglerLauncherTouchFixture.pointer({control:'restart',type:'pointerup',id}),id);
                buttonEvidence={down,up};return buttonEvidence;
              }}):null;
            const restartCapture=await captureRuntimeRestartBoundary({observe,frameKey:restartFrame,
              pausedFrame:paused.frames[restartFrame],isGameplayReady:gameplayReady,
              timeoutMs:30000,pollIntervalMs:75,wait:ms=>page.waitForTimeout(ms),now:()=>performance.now(),
              shouldContinueSampling:result=>result?.inputValid===true,
              operate:async()=>{
                const flowResult=await titleFlow.restartFromPause({press:press?.press??physicalFlowPress,
                  waitUntil:waitObservation,epoch:activeEpoch});
                return{flowResult,inputValid:press?press.wasRequested():physicalFlowCodes[0]==='KeyR'};
              }});
            const flowResult=restartCapture.operationResult?.flowResult;
            if(restartCapture.operationStarted!==true)return{checkStatus:'incomplete',
              reason:restartCapture.reason??'Native observation did not arm at the paused frame before restart input.',
              paused,pauseStable,restartCapture};
            if(press&&!restartCapture.operationResult?.inputValid)return{checkStatus:'incomplete',
              reason:'The registered title flow did not request its initial R through the Launcher button',paused,flowResult,restartCapture};
            if(!press&&!restartCapture.operationResult?.inputValid)return{checkStatus:'incomplete',
              reason:'The independent keyboard baseline did not begin with its registered physical R input',paused,flowResult,physicalFlowCodes,restartCapture};
            if(restartCapture.status!=='observed')return{checkStatus:'incomplete',
              reason:restartCapture.reason??`${source} restart did not capture a gameplay-ready native reset boundary`,
              paused,pauseStable,flowResult,restartCapture};
            const restarted=await waitForGameplayReadiness(activeEpoch,`${source} gameplay ready after restart`,
              {purpose:'transition',timeoutMs:15000});
            if(!gameplayReady(restarted))return{checkStatus:'incomplete',reason:`${source} restart never returned to native gameplay`,
              paused,pauseStable,restarted,flowResult,restartCapture};
            const advanced=await waitObservation(`${source} gameplay frame advances after restart`,state=>
              gameplayReady(state)&&Number.isFinite(state.frames?.[restartFrame])&&state.frames[restartFrame]>restarted.frames[restartFrame],10000);
            const afterIdentity=await captureIdentity();
            const capturedKeyboardEvents=await page.evaluate(start=>window.keyboardForwardEvents.slice(start),restartKeyIndex);
            const expectedPhysicalCodes=press?press.forwardedCodes():physicalFlowCodes;
            const keyboardDelivery=assessRestartKeyboardForwarding({events:capturedKeyboardEvents,epoch:activeEpoch,
              expectedCodes:expectedPhysicalCodes});
            if(keyboardDelivery.status!=='passed'){
              const error=new Error(source==='keyboard-r'?
                'The keyboard baseline must deliver its registered title-flow inputs physically':
                'Only confirmation inputs after the Launcher action may use the physical keyboard');
              error.evidence={keyboardDelivery,pauseInputFocus,restartInputFocus,capturedKeyboardEvents};throw error;
            }
            const keyboardEvents=keyboardDelivery.events;
            const evidence=assessRuntimeTouchRestart({before,paused,pauseStable,restarted,advanced,
              resetBoundary:restartCapture.resetBoundary,
              beforeIdentity,afterIdentity,expectedEpoch:activeEpoch,
              gameplayReady:{restarted:gameplayReady(restarted),advanced:gameplayReady(advanced)}});
            return{checkStatus:evidence.checkStatus,source,evidence,fixture,flowResult,keyboardEvents,keyboardDelivery,
              pauseInputFocus,restartInputFocus,button:buttonEvidence,
              before,paused,pauseStable,restartCapture,restarted,advanced,beforeIdentity,afterIdentity};
          };
            const keyboardBaseline=await runRestart({source:'keyboard-r'});
            if(keyboardBaseline.checkStatus==='incomplete')return keyboardBaseline;
            const touchButton=await runRestart({source:'launcher-touch-button'});
            if(touchButton.checkStatus==='incomplete')return{checkStatus:'incomplete',reason:touchButton.reason,keyboardBaseline,touchButton};
            const betweenRuns=compareRuntimeRestartIdentity(keyboardBaseline.beforeIdentity,touchButton.beforeIdentity,
              {expectedEpoch:activeEpoch});
            if(betweenRuns.status==='incomplete')return{checkStatus:'incomplete',reason:betweenRuns.reason,keyboardBaseline,touchButton,betweenRuns};
            if(betweenRuns.status!=='same')throw Error(betweenRuns.reason);
            return{checkStatus:'passed',complete:true,keyboardBaseline,touchButton,betweenRuns,
              scope:'Independent physical KeyR baseline and production Launcher Restart DOM action | native frame reset and same Runtime epoch'};
          }catch(error){failure=error;throw error;}
          finally{
            const cleanupErrors=[];
            for(const [operation,perform] of [
              ['dispose-touch-fixture',()=>page.evaluate(()=>EaglerLauncherTouchFixture.dispose())],
              ['release-inputs',()=>resetInputState(activeEpoch)],
              ['release-identity-tokens',()=>page.evaluate(()=>{delete window.__eaglerTouchRestartIdentity;})],
            ])try{await perform();}catch(error){cleanupErrors.push({operation,error:String(error)});}
            if(cleanupErrors.length){if(failure)failure.cleanupErrors=cleanupErrors;
              else throw Error(`Touch restart cleanup failed: ${JSON.stringify(cleanupErrors)}`);}
          }
        });
        const mappedSceneSend=async(operation,menu)=>{
          if(operation.source==='touch'){
            const startedAtMs=await frame.evaluate(()=>performance.now());
            const receipt=await directTouch(operation.type,operation.id,operation.x,operation.y);
            const completedAtMs=await frame.evaluate(()=>performance.now());
            return{...receipt,source:'touch',type:operation.type,id:operation.id,startedAtMs,completedAtMs};
          }
          if(operation.source==='keyboard'){
            const code=menu.keys[operation.action];assert.ok(code,'The title has no declared keyboard action');
            await(operation.down?keyDown(code):keyUp(code));
          }else if(operation.source==='gamepad'){
            const button=menu.gamepad[operation.action];assert.ok(Number.isSafeInteger(button),'The title has no declared mapped gamepad action');
            await page.evaluate(({button,down})=>EaglerAcceptanceInputs.virtualButton(acceptancePad,button,down?1:0),{button,down:operation.down});
          }else throw new Error('Unknown shared menu input source');
          return{acknowledged:true,source:operation.source,action:operation.action,down:operation.down};
        };
        const finishInputScene=async(operations,failure)=>{
          const cleanupErrors=[];
          for(const [operation,run] of operations)try{await run();}catch(error){cleanupErrors.push({operation,error:String(error.stack||error)});}
          if(!cleanupErrors.length)return;
          if(failure){failure.cleanupErrors=cleanupErrors;failure.evidence={...failure.evidence,cleanupErrors};return;}
          const error=new Error('Input scene cleanup or gameplay recovery failed');error.evidence={cleanupErrors};throw error;
        };
        const inputFrameBinding=async readNative=>{
          const url=frame.url(),hostEpoch=Number(new URL(url).searchParams.get('runtimeEpoch'));
          assert.equal(hostEpoch,activeEpoch,'Input observer is attached to another Host frame epoch');
          const nativeRuntime=(await readNative()).runtime;
          assert.equal(frame.url(),url,'Input observer frame changed while binding its native identity');
          assert.ok(nativeRuntime,'Input observer did not return its native Runtime identity');
          return{url,hostEpoch,nativeRuntime};
        };
        const menuInputTrial=async ingress=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant),menu=adapter?.originalMenu;
          if(!menu)return{checkStatus:'incomplete',reason:'Register original-menu state and input semantics'};
          const transition=await retireAndLaunch(activeEpoch+1,'touch','hold-button',{}, {titleOnly:true});
          const clockRequest=touchClockRequest({...target,action:'tick'});
          let clock=null,failure=null;
          try{
            if(ingress==='gamepad')await bindProtocolHostGamepad(page,target.game,activeEpoch);
            clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Original-menu scene requires the production manual clock'};
            const readTitle=()=>frame.evaluate(readOriginalMenuState,{contract:{...menu,runtime:adapter.runtime}});
            const frameBinding=await inputFrameBinding(readTitle);
            const result=await runOriginalMenuInputScene({
              readTitle,
              waitPhysical:async requestedMs=>{const startedAtMs=performance.now();await new Promise(resolve=>setTimeout(resolve,requestedMs));return{requestedMs,startedAtMs,completedAtMs:performance.now()};},
              send:operation=>mappedSceneSend(operation,menu),
              step:async ticks=>{for(let i=0;i<ticks;i++)await frame.evaluate(controlTouchMotionClock,clockRequest);},
            },{...menu,ingress,inputMasks:{...menu.inputMasks,...(ingress==='touch'?{cancel:menu.touchCancelMask}:{})}});
            const frameBindingAfter=await inputFrameBinding(readTitle);
            assert.deepEqual(frameBindingAfter,frameBinding,'Original menu replaced its native Runtime during input');
            return{...result,ingress,epoch:activeEpoch,transition,clock,frameBinding,frameBindingAfter};
          }catch(error){failure=error;throw error;}finally{
            await finishInputScene([
              ['mapped-device-release',()=>page.evaluate(()=>{window.acceptanceGamepadService?.dispose();window.acceptanceGamepadService=null;
                if(window.acceptancePad)acceptancePad.buttons.forEach((_,index)=>EaglerAcceptanceInputs.virtualButton(acceptancePad,index,0));})],
              ['native-input-reset',()=>resetInputState(activeEpoch)],
              ['native-clock-end',async()=>{if(clock?.mode==='native-manual')await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'});}],
              ['gameplay-readiness',()=>enterNativeGameplay(activeEpoch)],
            ],failure);
          }
        };
        const dialogueInputTrial=async(ingress,{shortTap=false}={})=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant),menu=adapter?.originalMenu;
          const {contract}=dialogueTouchSceneRequest(target);
          if(!contract||!menu)return{checkStatus:'incomplete',reason:'Register loaded-dialogue and original-menu input semantics'};
          const transition=await retireAndLaunch(activeEpoch+1,'touch','hold-button');
          const clockRequest=touchClockRequest({...target,action:'tick'}),sceneRequest=validationSceneRequest(target);
          let clock=null,previous=null,failure=null;
          const controlledScene={acceptance:null,timeline:[]};
          const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
          try{
            if(ingress==='gamepad')await bindProtocolHostGamepad(page,target.game,activeEpoch);
            clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Loaded-dialogue scene requires the production manual clock'};
            previous=assertControlledScene(await frame.evaluate(controlValidationScene,sceneRequest));controlledScene.acceptance=previous;
            const io={controls:touchControls,
              readManifest:async()=>{
                const manifest=normalizeDialogueTouchManifest(await frame.evaluate(readDialogueTouchWords,{contract,operation:'manifest'}),contract);
                if(!manifest.available)return manifest;
                const observation=normalizeDialogueTouchObservation(await frame.evaluate(readDialogueTouchWords,{contract,operation:'observation'}),contract);
                if(!observation.available)return observation;assert.deepEqual(manifest.runtime,observation.runtime);
                return{...manifest,fixture:observation.fixture,fixtureSource:contract.observation.source};
              },
              observe:async()=>normalizeDialogueTouchObservation(await frame.evaluate(readDialogueTouchWords,{contract,operation:'observation'}),contract),
              requestMessage:async entry=>{
                if(!adapter.controlledScene?.requestEffects?.dialogue)return frame.evaluate(requestDialogueTouchMessage,{contract,entry});
                const before=await frame.evaluate(controlValidationScene,sceneRequest);
                assert.deepEqual(before,previous,'Dialogue request lost the last controlled-world boundary');
                const result=await frame.evaluate(requestDialogueTouchMessage,{contract,entry});
                const after=await frame.evaluate(controlValidationScene,sceneRequest);
                controlledScene.timeline.push({kind:'request',before,after,effects:adapter.controlledScene.requestEffects.dialogue,result});
                assertControlledSceneRequest(after,before,{kind:'dialogue'});previous=after;return result;
              },
              readActions:()=>frame.evaluate(readTouchActionState,touchActionsRequest(target)),
              directTouchBatch:nativeTouchBatch,
              send:operation=>mappedSceneSend(operation,menu),
              waitPhysical:async ms=>{
                const startedAtMs=await frame.evaluate(()=>performance.now());await page.waitForTimeout(ms);
                const completedAtMs=await frame.evaluate(()=>performance.now());return{startedAtMs,completedAtMs};
              },
              step:async ticks=>{for(let i=0;i<ticks;i++){
                await frame.evaluate(controlTouchMotionClock,clockRequest);
                const next=await frame.evaluate(controlValidationScene,sceneRequest);controlledScene.timeline.push({kind:'tick',receipt:next});
                assertControlledSceneStimulus(next,previous,{kind:'dialogue'});previous=next;
              }},
            };
            const options={ingress,expected:{variant:target.variant,stage:contract.stage,touchContext:contract.touchContext,gameplayTouchContext:contract.gameplayTouchContext},
              waitOpcode:contract.waitOpcode,consumerLatencyTicks:contract.consumerLatencyTicks,consumerLatencySource:contract.consumerLatencySource,
              inputMasks:{fire:adapter.actions.inputMasks.fire,confirm:contract.observation.confirmMask,skip:contract.skipMask},
              pointer:{id:9511,point:{x:.5,y:.75}},gesture:shortTap?contract.gesture:contract.skipGesture,
              controls:{neutral,heldFire:{...neutral,fireEnabled:true}},
              sources:{manifest:contract.manifest.source,consumer:contract.sources.confirmConsumer},
              budgets:{baselineTicks:8,openTicks:90,dialogueOpenTicks:90,waitTicks:60,skipTicks:8,releaseTicks:8,source:contract.sources.confirmConsumer}};
            const frameBinding=await inputFrameBinding(io.observe);
            const result=shortTap?await runTouchDialogueFreshConfirmScene(io,options):await runLoadedDialogueSkipScene(io,options);
            const frameBindingAfter=await inputFrameBinding(io.observe);
            assert.deepEqual(frameBindingAfter,frameBinding,'Dialogue input replaced its native Runtime during the trial');
            return{...result,ingress,kind:shortTap?'short-confirm':'hold-skip',epoch:activeEpoch,transition,clock,frameBinding,frameBindingAfter,controlledScene};
          }catch(error){failure=error;throw error;}finally{
            await finishInputScene([
              ['mapped-device-release',()=>page.evaluate(()=>{window.acceptanceGamepadService?.dispose();window.acceptanceGamepadService=null;
                if(window.acceptancePad)acceptancePad.buttons.forEach((_,index)=>EaglerAcceptanceInputs.virtualButton(acceptancePad,index,0));})],
              ['native-input-reset',()=>resetInputState(activeEpoch)],
              ['native-clock-end',async()=>{if(clock?.mode==='native-manual')await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'});}],
            ],failure);
          }
        };
        const collectInputTrials=async tasks=>{
          const trials=[];
          for(const task of tasks)try{trials.push(await task());}catch(error){trials.push({checkStatus:'failed',reason:String(error.stack||error),evidence:error.evidence??null});}
          try{const restored=await retireAndLaunch(activeEpoch+1,'touch','hold-button');trials.push({checkStatus:'passed',kind:'restore-gameplay',epoch:activeEpoch,transition:restored});}
          catch(error){trials.push({checkStatus:'failed',kind:'restore-gameplay',reason:String(error.stack||error),evidence:error.evidence??null});}
          const failed=trials.find(t=>t.checkStatus==='failed'),missing=trials.find(t=>t.checkStatus!=='passed');
          return{checkStatus:failed?'failed':missing?'incomplete':'passed',complete:!missing,trials,reason:failed?.reason??missing?.reason??null};
        };
        if(inputOnly)await check('shared-original-menu-dialogue-input',()=>collectInputTrials([
          ()=>menuInputTrial('keyboard'),()=>menuInputTrial('gamepad'),()=>dialogueInputTrial('keyboard'),()=>dialogueInputTrial('gamepad'),
        ]));
        if(actionsOnly)await check('shared-touch-menu-dialogue-gestures',()=>collectInputTrials([
          ()=>menuInputTrial('touch'),()=>dialogueInputTrial('touch',{shortTap:true}),()=>dialogueInputTrial('touch'),
        ]));
        const launcherInputTrial=async kind=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant);
          if(!adapter?.actions||!adapter.movement)return{checkStatus:'incomplete',reason:'Register the native action consumer and movement owner'};
          const transition=await retireAndLaunch(activeEpoch+1,'touch','hold-button');
          const clockRequest=touchClockRequest({...target,action:'tick'}),sceneRequest=validationSceneRequest(target);
          let clock=null,previous=null,failure=null;
          const traceSource='native-touch-trace-v2';
          const controlledScene={acceptance:null,receipts:[]};
          try{
            clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Launcher input scene requires the production manual clock'};
            previous=assertControlledScene(await frame.evaluate(controlValidationScene,sceneRequest));
            controlledScene.acceptance=previous;
            const result=await runLauncherInputRuntime({kind,page,frame,context,target:{...target,epoch:activeEpoch},
              contract:{game:target.game,variant:target.variant,inputMasks:adapter.actions.inputMasks,movement:adapter.movement,
                gamepad:{mappingSeat:0,nativeSeat:0,nativeSeatSource:`${target.game}/${target.variant} | registered normal P1 input owner`,actions:{
                  shoot:{first:{control:{kind:'button',index:0}},second:{control:{kind:'axis',index:1,rest:0,value:1}}},
                  right:{first:{control:{kind:'axis',index:0,rest:0,value:1}},second:{control:{kind:'button',index:2}}},
                }},
                layout:{storageKey:'eagler-touhou-touch-layout-v1',control:'fire',
                  viewports:{landscape:{width:960,height:640},portrait:{width:640,height:960}},
                  profiles:{landscape:{fire:{x:.32,y:.38,scale:1.25,priority:1}},portrait:{fire:{x:.68,y:.62,scale:1.4,priority:1}}}},
                magnifier:{traceSource,pointerId:19471,pinch:{pointerIds:[19472,19473],
                  start:[{x:.4,y:.5},{x:.6,y:.5}],end:[{x:.3,y:.5},{x:.7,y:.5}]},
                  gesture:{start:{x:.5,y:.5},end:{x:.515625,y:.5}}}},
              step:async ticks=>{for(let i=0;i<ticks;i++){
                await frame.evaluate(controlTouchMotionClock,clockRequest);
                const next=await frame.evaluate(controlValidationScene,sceneRequest);controlledScene.receipts.push(next);assertControlledScene(next,previous);previous=next;
              }},
              readActions:()=>frame.evaluate(readTouchActionState,touchActionsRequest(target)),observe,
              readTrace:async()=>{
                const boundary=await readNativeTouchBoundary(),trace=boundary.trace;
                if(!trace.available)return trace;
                const sample=trace.samples.at(-1);
                if(trace.abiVersion!==2||!sample)return{available:false,reason:'No native forward touch trace sample'};
                return{available:true,complete:true,source:traceSource,sequence:sample.sequence,target:{x:sample.targetX,y:sample.targetY},raw:sample};
              },
            });
            return{...result,complete:result.complete??result.checkStatus==='passed',epoch:activeEpoch,transition,clock,controlledScene};
          }catch(error){failure=error;throw error;}finally{
            await finishInputScene([
              ['native-input-reset',()=>resetInputState(activeEpoch)],
              ['native-clock-end',async()=>{if(clock?.mode==='native-manual')await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'});}],
            ],failure);
          }
        };
        if(inputOnly)await check('shared-launcher-gamepad-mapping',()=>launcherInputTrial('gamepad'));
        if(actionsOnly)await check('shared-launcher-touch-layout',()=>launcherInputTrial('layout'));
        if(actionsOnly)await check('shared-launcher-touch-magnifier',()=>launcherInputTrial('magnifier'));
        if(actionsOnly)await check('shared-touch-double-tap-bomb',async()=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant);
          if(!adapter?.doubleTapBomb)return{checkStatus:'incomplete',reason:'Register the production double-tap gesture contract'};
          const trials=[];
          for(const enabled of [false,true]){
            const transition=await retireAndLaunch(activeEpoch+1,'touch','toggle-button',{doubleTapBombEnabled:enabled});
            const launch=record.launchOptionsByEpoch.find(item=>item.epoch===activeEpoch);
            const actionRequest=touchActionsRequest(target),sceneRequest=validationSceneRequest(target),
              clockRequest=touchClockRequest({...target,action:'tick'}),observationRequest=runtimeObservationRequest(target);
            const clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Double-tap scenes require the native manual clock',clock,trials};
            try{
              let previous=assertControlledScene(await frame.evaluate(controlValidationScene,sceneRequest));
              const directTouchBatch=async operations=>{
                const result=await nativeTouchBatch(operations);
                if(result.available!==true)return result;
                assertControlledScene(result.worldBefore);
                for(const receipt of result.worldReceipts){assertControlledScene(receipt,previous);previous=receipt;}
                return result;
              };
              const result=await runTouchDoubleTapBombScene({controls:touchControls,directTouchBatch,observe,
                readActions:()=>frame.evaluate(readTouchActionState,actionRequest),
                step:async ticks=>{for(let n=0;n<ticks;n++){
                  await frame.evaluate(controlTouchMotionClock,clockRequest);
                  const receipt=await frame.evaluate(controlValidationScene,sceneRequest);
                  assertControlledScene(receipt,previous);previous=receipt;
                }}},{inputMasks:adapter.actions.inputMasks,gesture:adapter.doubleTapBomb,
                launchOptions:{doubleTapBombEnabled:launch.options.doubleTapBombEnabled,source:launch.source,epoch:launch.epoch},
                geometry:{primary:{id:9417,start:{x:.5,y:.75},move:{x:.58,y:.75},bombMove:{x:.51,y:.70}},
                  secondary:{id:9427,point:{x:.3,y:.45}},movementEpsilon:.0001},
                budgets:{baselineTicks:8,movementSettlingTicks:2,movementSettlingSource:adapter.movement.inputBoundarySource,
                  triggerTicks:4,pulseDrainTicks:5}});
              trials.push({epoch:activeEpoch,transition,clock,...result});
              if(result.checkStatus!=='passed')return{checkStatus:result.checkStatus,reason:result.reason,trials};
            }finally{
              record.touchClock??=[];
              record.touchClock.push(await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'}));
            }
          }
          return{checkStatus:'passed',complete:true,trials,
            scope:'Actual startup enable/disable | single tap | secondary double-tap | one native Bomb | primary movement ownership'};
        });
        if(actionsOnly)await check('shared-touch-deathbomb',async()=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant);
          if(!adapter?.deathbomb)return{checkStatus:'incomplete',reason:'Register a real projectile collision and original Deathbomb window observer'};
          const trials=[];
          for(const [source,timing] of [['keyboard','last-window'],['touch','last-window'],['touch','late']]){
            const transition=await retireAndLaunch(activeEpoch+1,'touch','hold-button');
            const request=deathbombRequest(target),actionRequest=touchActionsRequest(target),sceneRequest=validationSceneRequest(target),
              clockRequest=touchClockRequest({...target,action:'tick'});
            const clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Deathbomb scenes require the native manual clock',clock,trials};
            let previous=assertControlledScene(await frame.evaluate(controlValidationScene,sceneRequest));
            try{
              const result=await runTouchDeathbombScene({controls:touchControls,directTouch,
                key:touchSceneKey,readState:()=>frame.evaluate(readDeathbombState,request),
                readActions:()=>frame.evaluate(readTouchActionState,actionRequest),
                requestHit:()=>frame.evaluate(requestDeathbombHit,request),
                step:async ticks=>{for(let n=0;n<ticks;n++){
                  await frame.evaluate(controlTouchMotionClock,clockRequest);
                  const receipt=await frame.evaluate(controlValidationScene,sceneRequest);
                  assertControlledSceneStimulus(receipt,previous,{kind:'single-projectile'});previous=receipt;
                }}},{source,timing,contract:{...adapter.deathbomb,inputMasks:adapter.actions.inputMasks},
                budgets:{eligibilityTicks:600,movementSettleTicks:8,collisionTicks:8,deathbombWindowTicks:64,
                  recoveryTicks:120,lateDrainTicks:120,movementEpsilon:.0001}});
              trials.push({source,timing,epoch:activeEpoch,transition,clock,...result});
            }catch(error){trials.push({source,timing,epoch:activeEpoch,checkStatus:'failed',error:String(error.stack??error),
              evidence:error.evidence??error.touchDeathbombEvidence??null});}
            finally{
              await resetInputState(activeEpoch);
              record.touchClock??=[];
              record.touchClock.push(await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'}));
            }
          }
          const failed=trials.find(item=>item.checkStatus==='failed'),incomplete=trials.find(item=>item.checkStatus!=='passed');
          return{checkStatus:failed?'failed':incomplete?'incomplete':'passed',complete:!incomplete,trials,
            reason:failed?.error??incomplete?.reason??null,
            scope:'Tagged production projectile | keyboard and touch at the last original window tick | late pulse through respawn | held movement'};
        });
        if(actionsOnly)await check('shared-touch-dialogue-confirm',async()=>{
          const request=dialogueTouchSceneRequest(target),contract=request.contract;
          if(!contract)return{checkStatus:'incomplete',reason:'Register the real modal consumer, loaded script manifest and confirmation gesture'};
          const transition=await retireAndLaunch(activeEpoch+1,'touch','hold-button');
          const clockRequest=touchClockRequest({...target,action:'tick'}),sceneRequest=validationSceneRequest(target);
          const clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
          if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Modal input scenes require the native manual clock',clock};
          try{
            let previous=assertControlledScene(await frame.evaluate(controlValidationScene,sceneRequest));
            const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
            const result=await runTouchDialogueFreshConfirmScene({controls:touchControls,
              readManifest:async()=>{
                const manifest=normalizeDialogueTouchManifest(await frame.evaluate(readDialogueTouchWords,{contract,operation:'manifest'}),contract);
                if(manifest.available!==true)return manifest;
                const observation=normalizeDialogueTouchObservation(await frame.evaluate(readDialogueTouchWords,{contract,operation:'observation'}),contract);
                if(observation.available!==true)return observation;
                assert.deepEqual(manifest.runtime,observation.runtime,'Loaded script and fixture receipts must belong to the same stopped Runtime');
                return{...manifest,fixture:observation.fixture,fixtureSource:contract.observation.source};
              },
              observe:async()=>normalizeDialogueTouchObservation(await frame.evaluate(readDialogueTouchWords,{contract,operation:'observation'}),contract),
              readActions:()=>frame.evaluate(readTouchActionState,touchActionsRequest(target)),
              requestMessage:entry=>frame.evaluate(requestDialogueTouchMessage,{contract,entry}),
              directTouchBatch:nativeTouchBatch,
              step:async ticks=>{for(let n=0;n<ticks;n++){
                await frame.evaluate(controlTouchMotionClock,clockRequest);
                const receipt=await frame.evaluate(controlValidationScene,sceneRequest);
                assertControlledSceneStimulus(receipt,previous,{kind:'dialogue'});previous=receipt;
              }}},{expected:{variant:target.variant,stage:contract.stage,touchContext:contract.touchContext},waitOpcode:contract.waitOpcode,
              consumerLatencyTicks:contract.consumerLatencyTicks,consumerLatencySource:contract.consumerLatencySource,
              inputMasks:{fire:resolveAdaptationAdapter(target.game,target.variant).actions.inputMasks.fire,confirm:contract.observation.confirmMask},
              pointer:{id:9487,point:{x:.5,y:.75}},gesture:contract.gesture,
              controls:{neutral,heldFire:{...neutral,fireEnabled:true}},
              budgets:{baselineTicks:8,dialogueOpenTicks:120,waitTicks:60,source:contract.sources.confirmConsumer}});
            return{...result,complete:result.checkStatus==='passed',epoch:activeEpoch,transition,clock,
              scope:'Real ECL-referenced loaded dialogue | held gameplay Fire | no-touch contrast | one fresh modal confirm edge'};
          }finally{
            record.touchClock??=[];
            record.touchClock.push(await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'}));
          }
        });
        if(actionsOnly)await check('shared-touch-unlimited-result',async()=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant),boundary=adapter?.runBoundaries;
          if(!boundary||!adapter.parseReplayMetadata)return{checkStatus:'incomplete',reason:'Register ordinary run/Result boundaries and the independent saved Replay metadata parser'};
          const clockRequest=touchClockRequest({...target,action:'tick'});
          let clock=null;
          ordinaryRunBoundaryFixture=true;
          try{
            const transition=await retireAndLaunch(activeEpoch+1,'touch-unlimited','hold-button');
            clock=await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'begin'});
            if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Ordinary run/Result scenes require the native manual clock',clock};
            const sources=boundary.sources;
            const storageIdentity={origin:new URL(hostUrl).origin,saveRoot:adapter.replay.opener.saveRoot,
              context:'fresh non-persistent BrowserContext owned by this test process'};
            const readState=async()=>{
              const state=await frame.evaluate(controlRunBoundary,runBoundaryRequest(target));
              return{...state,game:target.game,variant:target.variant,runtimeEpoch:activeEpoch,storageIdentity,
                identitySource:sources.runtimeIdentity,runIdSource:sources.runId,markerSource:sources.runMarker};
            };
            const initial=await readState();
            if(initial.available!==true)return{checkStatus:'incomplete',reason:initial.reason,initial,transition};
            assert.equal(initial.armed,true,'The ordinary-run fixture must be explicitly armed');
            assert.equal(initial.controlledWorld,false,'Result/Replay uses an ordinary recording, outside the write-locked quiet scene');
            const preparation={available:true,status:'ready',game:target.game,variant:target.variant,
              normal:initial.normal,isolatedStorage:true,runtimeEpoch:activeEpoch,storageIdentity,
              runId:initial.runId,identitySource:sources.runtimeIdentity,
              startReceipt:{available:true,status:gameplayReady(transition.state)?'passed':'incomplete',state:transition.state}};
            const geometry=boundary.touchGeometry;
            if(!geometry)return{checkStatus:'incomplete',reason:'Ordinary touch fixture has no title coordinate declaration'};
            let anchor=null,pointerHeld=false;
            const setNeutralTarget=async()=>{
              await cancelTouch();await touchControls();anchor=null;pointerHeld=false;
              return{available:true,accepted:true,source:'Runtime touch-cancel and neutral touch-controls RPC'};
            };
            const io={readState,setNeutralTarget,
              setUnlimitedTarget:async({target:point,requestId})=>{
                if(!pointerHeld){anchor=(await readState()).playerPosition;
                  await directTouch('down',9457,geometry.start.x,geometry.start.y);pointerHeld=true;}
                await directTouch('move',9457,geometry.start.x+(point.x-anchor.x)/geometry.width,
                  geometry.start.y+(point.y-anchor.y)/geometry.height);
                return{available:true,accepted:true,target:point,requestId,source:'Runtime direct-touch RPC'};
              },
              step:async()=>({available:true,steps:1,native:await frame.evaluate(controlTouchMotionClock,clockRequest)}),
              readTouchTrace:async({afterSequence})=>{
                const raw=await frame.evaluate(readTouchMotionTrace,runtimeObservationRequest(target));
                if(!raw.available)return raw;
                const trace=decodeTouchMotionTrace(raw.raw),state=await readState();
                const first=trace.samples[0]?.sequence??trace.total+1;
                return{available:true,nextSequence:trace.total+1,firstAvailableSequence:first,
                  dropped:afterSequence!==null&&afterSequence+1<first,
                  rows:trace.samples.filter(row=>afterSequence!==null&&row.sequence>afterSequence).map(row=>({
                    sequence:row.sequence,runId:state.runId,runtimeEpoch:activeEpoch,storageIdentity,stage:state.stage,
                    mode:row.mode===2?'unlimited':row.mode===1?'limited':'neutral',gameplayTick:Boolean(row.flags&1),
                    before:{x:row.beforeX,y:row.beforeY},target:{x:row.targetX,y:row.targetY},after:{x:row.afterX,y:row.afterY},
                    source:sources.touchTrace,stageBinding:'Current native stage with the production clock stopped'}))};
              },
              requestStageTransition:async identity=>({...await frame.evaluate(controlRunBoundary,
                runBoundaryRequest({...target,...identity,action:'stage'})),source:sources.stageTransition}),
              requestResultTransition:async identity=>({...await frame.evaluate(controlRunBoundary,
                runBoundaryRequest({...target,...identity,action:'result'})),source:sources.resultTransition}),
              saveReplay:async({slot,name})=>{
                const saved=await frame.evaluate(callNativeReplayFile,{runtime:adapter.runtime,
                  contract:{...adapter.replay.finalizer,slot,name},action:'finalize'});
                return{available:saved.status!=='incomplete',saved:saved.status==='passed',...saved,source:sources.replayFinalizer};
              },
              readReplayBytes:async({path})=>{
                const result=await page.evaluate(path=>rpc('read',{path},10000),path);
                return{available:true,bytes:Buffer.from(result.bytes),storageIdentity};
              }};
            const result=await runTouchUnlimitedResultSceneGroup(io,{preparation,maxWaitTicks:120,
              contract:{game:target.game,variant:target.variant,sources,
                nonzeroTarget:state=>({x:state.playerPosition.x+geometry.probeDelta.x,y:state.playerPosition.y+geometry.probeDelta.y}),
                inspectReplay:(bytes,options)=>({available:true,...adapter.parseReplayMetadata(bytes,options)})}});
            return{...result,complete:result.checkStatus==='passed',epoch:activeEpoch,clock,transition,
              scope:'Actual unlimited movement | run marker across production stage boundary | Result high-score field | independently decoded saved Replay'};
          }finally{
            try{
              if(clock?.mode==='native-manual'){
                record.touchClock??=[];
                record.touchClock.push(await frame.evaluate(controlTouchMotionClock,{...clockRequest,action:'end'}));
              }
            }finally{ordinaryRunBoundaryFixture=false;}
          }
        });
        if(replayRecordingOnly)await check('shared-replay-recording-and-isolation',async()=>{
          const adapter=resolveAdaptationAdapter(target.game,target.variant),semantics=adapter?.replay;
          if(!semantics||!adapter.clock||!adapter.parseReplayFrames)return{checkStatus:'incomplete',
            reason:'Register production Replay semantics, the manual clock and an independent file parser'};
          const traceRequest={runtime:adapter.runtime,contract:semantics.nativeTrace};
          const clock=await frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'begin'}));
          if(clock.mode!=='native-manual')return{checkStatus:'incomplete',reason:'Replay windows require the production manual clock',clock};
          const releaseAll=async()=>{
            for(const code of [...heldKeys])await keyUp(code);
            await cancelTouch();await touchControls();
            await page.evaluate(()=>{if(window.acceptancePad)for(let bit=0;bit<acceptancePad.buttons.length;bit++)
              EaglerAcceptanceInputs.virtualButton(acceptancePad,bit,0);});
          };
          const manualStep=()=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'tick'}));
          const controlTrace=async({enabled,reset})=>{
            const receipt=await frame.evaluate(controlReplayTrace,{...traceRequest,enabled:Number(enabled),reset:Number(reset)});
            if(receipt.status==='passed'){
              assert.equal(receipt.enabled,Number(enabled));assert.equal(receipt.reset,Number(reset));
              return{...receipt,enabled,reset};
            }
            return receipt;
          };
          try{
            await page.evaluate(()=>document.body.focus());
            await bindProtocolHostGamepad(page,target.game,activeEpoch);
            const result=await runReplayRecordingGroup({semantics,stage:0,outputDir:resolve(output,`${target.id}-replay-scenes`),io:{
              prepare:async()=>{await releaseAll();return{status:'passed',clock,epoch:activeEpoch,world:'normal-production-gameplay'};},
              controlTrace,readTrace:({afterSequence})=>frame.evaluate(readReplayTrace,{...traceRequest,afterSequence}),
              readInputReceipt:probe=>frame.evaluate(readReplayInputReceipt,{...traceRequest,receiptId:probe.ingress.receipt}),
              step:manualStep,
              key:async action=>{
                if(action.pressed)await keyDown(action.code);else await keyUp(action.code);
                await frame.waitForFunction(({code,pressed})=>window.__eaglerKeyboardInputBridge?.keyDown?.(code)===pressed,
                  {code:action.code,pressed:action.pressed},{timeout:2000});
              },
              gamepad:action=>page.evaluate(({bit,pressed})=>{
                if(!window.acceptancePad||!Number.isSafeInteger(bit)||bit<0||bit>=acceptancePad.buttons.length)
                  throw Error('The declared virtual gamepad button is unavailable');
                EaglerAcceptanceInputs.virtualButton(acceptancePad,bit,pressed?1:0);
              },action),
              controls:action=>touchControls({fireEnabled:action.enabled}),
              directTouch:event=>directTouch(event.action,event.id,event.x,event.y),
              finalize:()=>frame.evaluate(callNativeReplayFile,{runtime:adapter.runtime,contract:semantics.finalizer,action:'finalize'}),
              readFile:async path=>Uint8Array.from((await page.evaluate(path=>rpc('read',{path}),path)).bytes),
              openPlayback:async({bytes,sha256,stage:requestedStage})=>{
                const selectedStage=requestedStage??0,path=semantics.finalizer.path;
                await releaseAll();
                await page.evaluate(({path,bytes})=>rpc('write',{path,bytes}),{path,bytes:Array.from(bytes)});
                const persisted=Uint8Array.from((await page.evaluate(path=>rpc('read',{path}),path)).bytes);
                assert.equal(hash(persisted),sha256,'Imported Replay bytes differ before native playback');
                const opened=await frame.evaluate(callNativeReplayFile,{runtime:adapter.runtime,contract:semantics.opener,
                  action:'open',stage:selectedStage,path});
                if(opened.status!=='passed')return opened;
                const postOpen=Uint8Array.from((await page.evaluate(path=>rpc('read',{path}),path)).bytes);
                assert.equal(hash(postOpen),sha256,'Replay bytes changed across the production native opener');
                return{status:'passed',sha256:hash(postOpen),stage:selectedStage,
                  fileIdentity:{source:'Shell read before and after the declared native opener',before:hash(persisted),after:hash(postOpen)},
                  nativeOpen:opened,bootstrapOwner:'shared Replay trace scene'};
              },
              titleParser:{parseFrames:adapter.parseReplayFrames,parseExtension:adapter.parseReplayExtension},releaseAll,
            }});
            return{...result,checkStatus:result.status,complete:false,epoch:activeEpoch,clock,
              scope:'Finite production recording and same-file input isolation | natural Replay endpoint has its own acceptance'};
          }finally{
            const cleanup=[];
            for(const [operation,perform] of [
              ['release-inputs',releaseAll],
              ['disable-replay-trace',()=>controlTrace({enabled:false,reset:false})],
              ['dispose-gamepad',()=>page.evaluate(()=>{window.acceptanceGamepadService?.dispose?.();delete window.acceptanceGamepadService;})],
              ['resume-production-clock',()=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action:'end'}))],
            ]){
              try{
                const receipt=await perform();
                if(receipt?.status&&receipt.status!=='passed')throw Error(receipt.reason??`${operation} was not acknowledged`);
                cleanup.push({operation,status:'passed',receipt:receipt??null});
              }catch(error){cleanup.push({operation,status:'failed',error:String(error)});}
            }
            record.replayCleanup=cleanup;
            if(cleanup.some(item=>item.status==='failed')){
              const error=Error('Replay scene cleanup failed');error.evidence={cleanup};throw error;
            }
          }
        });
        if(gameplayReady(started))await captureRuntimeScreenshot('gameplay',resolve(output,`${target.id}-gameplay.png`));
        const directionCase=async(layout,entries)=>{
          const selectionState=await page.evaluate(layout=>window.__desktopSelectLayout(layout),layout);assert.equal(selectionState.selected,layout);
          if(layout==='wasd')await page.evaluate(()=>document.body.focus());else await frame.locator('canvas').focus();
          const initial=await observe();
          const initialEligibility=incompleteGameplayPrecondition(initial,target.game);
          if(initialEligibility)return{...initialEligibility,layout,selection:selectionState};
          const results=[];
          for(const [physical,canonical,axis,sign] of entries){
            const before=await observe();
            const eligibility=incompleteGameplayPrecondition(before,target.game);
            if(eligibility)return{...eligibility,layout,selection:selectionState,directions:results,
              reason:`Native gameplay eligibility ended before ${physical}; the remaining matrix was not attributed to keyboard mapping.`};
            let held,released,settled;const settleIntervalMs=180;
            try{
              try{await keyDown(physical);held=await waitObservation(`${layout}/${physical} native movement`,value=>value.capabilities.input&&value.input.value!==before.input.value&&
                gameplayReady(value)&&(axis==='x'?(value.position.x-before.position.x)*sign>0:(value.position.y-before.position.y)*sign>0),2500);}
              finally{await keyUp(physical);}
              released=await waitObservation(`${layout}/${physical} keyup releases native input`,value=>value.input.value===before.input.value,3000);
              await page.waitForTimeout(settleIntervalMs);settled=await observe();
              assert.equal(settled.input.value,before.input.value,`${physical}: native input must stay released for ${settleIntervalMs} ms`);
              assert.ok(Math.abs(settled.position[axis]-released.position[axis])<0.001,
                `${physical}: position must stay settled for ${settleIntervalMs} ms after keyup`);
              assert.ok((held.position[axis]-before.position[axis])*sign>0,`${physical} must move along ${axis} in the expected direction`);
              results.push({physical,canonical,axis,sign,before,held,released,settled,settleIntervalMs});
            }catch(error){
              const finalState=await observe().catch(()=>null),finalEligibility=incompleteGameplayPrecondition(finalState,target.game);
              if(finalEligibility)return{...finalEligibility,layout,selection:selectionState,directions:results,
                interruptedDirection:physical,actionError:String(error.stack||error)};
              throw error;
            }
          }
          return{layout,selection:selectionState,directions:results};
        };
        const nativeCheck=async(id,operation)=>check(id,async()=>{
          const eligibility=incompleteGameplayPrecondition(await observe(),target.game);
          if(eligibility)return{...eligibility,stage:`before-${id}`};
          return operation();
        });
        await nativeCheck('native-four-direction-arrows',()=>directionCase('arrows',[
          ['ArrowUp','ArrowUp','y',-1],['ArrowDown','ArrowDown','y',1],['ArrowLeft','ArrowLeft','x',-1],['ArrowRight','ArrowRight','x',1]]));
        for(const route of ['iframe-runtime-listener','host-runtime-bridge'])await check(`negative-control-suppressed-${route}-keyboard-mapping`,async()=>{
          const initial=await observe(),eligibility=incompleteGameplayPrecondition(initial,target.game);
          if(eligibility)return{...eligibility,route,stage:'before-negative-control'};
          await resetInputState(activeEpoch);const neutralStart=await waitObservation('Native keyboard clear returns to neutral before mapping negative control',state=>state.input.value===0,2000);
          await page.waitForTimeout(180);const before=await observe();
          assert.equal(before.input.value,neutralStart.input.value,'Native input must remain neutral after cleanup before the mapping negative control');
          const bridgeState=await frame.evaluate(()=>{const bridge=window.__eaglerKeyboardInputBridge,port=window.__eaglerKeyboardMapping;return{active:bridge?.active?.()??false,hasPort:!!port,protocol:port?.protocol,game:port?.game,epoch:port?.epoch,origin:port?.origin,frameOrigin:location.origin};});
          assert.equal(bridgeState.active,true,'The Runtime keyboard bridge must recognize the production mapping port');
          if(route==='iframe-runtime-listener')await frame.locator('canvas').focus();else await page.evaluate(()=>document.body.focus());
          const rawBefore=await frame.evaluate(()=>window.__desktopRawKeyboardEvents?.length??0),hostBefore=await page.evaluate(()=>keyboardForwardEvents.length);
          await frame.evaluate(()=>{const mapping=window.__eaglerKeyboardMapping;if(!mapping||typeof mapping.map!=='function')throw Error('Production keyboard mapping port is unavailable');window.__desktopSavedKeyboardMap=mapping.map;window.__desktopSuppressedMapCalls=0;mapping.map=()=>{window.__desktopSuppressedMapCalls++;return[];};});
          const physical=before.position.x>320?'ArrowLeft':'ArrowRight',sign=physical==='ArrowRight'?1:-1;let suppressed,mapCalls,rawAfter,hostAfter;
          try{await keyDown(physical);await page.waitForTimeout(180);suppressed=await observe();}
          finally{
            await keyUp(physical);
            try{mapCalls=await frame.evaluate(()=>window.__desktopSuppressedMapCalls);rawAfter=await frame.evaluate(()=>window.__desktopRawKeyboardEvents?.length??0);hostAfter=await page.evaluate(()=>keyboardForwardEvents.length);}
            finally{await frame.evaluate(()=>{const mapping=window.__eaglerKeyboardMapping;if(!mapping||typeof window.__desktopSavedKeyboardMap!=='function')throw Error('Saved keyboard map is unavailable');mapping.map=window.__desktopSavedKeyboardMap;delete window.__desktopSavedKeyboardMap;delete window.__desktopSuppressedMapCalls;});}
          }
          assert.ok(mapCalls>0,'The selected keyboard event route must reach the deliberately suppressed production mapping');
          if(route==='iframe-runtime-listener')assert.ok(rawAfter>rawBefore,'The iframe-native keyboard listener must receive the real browser key event');
          else assert.ok(hostAfter>hostBefore,'The host keyboard bridge must deliver the real browser key event to the Runtime');
          const suppressedEligibility=incompleteGameplayPrecondition(suppressed,target.game);
          if(suppressedEligibility)return{...suppressedEligibility,injectedFault:'KeyboardMapping.map returns no transitions while the production Runtime bridge is active',route,mapCalls,bridgeState,rawEvents:{before:rawBefore,after:rawAfter},hostEvents:{before:hostBefore,after:hostAfter}};
          assert.equal(suppressed.input.value,before.input.value,'A deliberately suppressed keyboard mapping must not change native input');
          assert.ok(Math.abs(suppressed.position.x-before.position.x)<0.001,'A deliberately suppressed keyboard mapping must not move the player');
          const restoredBefore=await observe(),restoredEligibility=incompleteGameplayPrecondition(restoredBefore,target.game);
          if(restoredEligibility)return{...restoredEligibility,injectedFault:'KeyboardMapping.map returns no transitions while the production Runtime bridge is active',route,mapCalls,bridgeState,
            rawEvents:{before:rawBefore,after:rawAfter},hostEvents:{before:hostBefore,after:hostAfter}};
          await keyDown(physical);let restored;
          try{restored=await waitObservation(`restored ${route} mapping reaches native input`,state=>state.input.value!==restoredBefore.input.value&&(state.position.x-restoredBefore.position.x)*sign>0,2500);}
          catch(error){const finalState=await observe().catch(()=>null),finalEligibility=incompleteGameplayPrecondition(finalState,target.game);
            if(finalEligibility)return{...finalEligibility,injectedFault:'KeyboardMapping.map returns no transitions while the production Runtime bridge is active',route,mapCalls,bridgeState,
              restoredBefore,actionError:String(error.stack||error),rawEvents:{before:rawBefore,after:rawAfter},hostEvents:{before:hostBefore,after:hostAfter}};
            throw error;
          }finally{await keyUp(physical);}
          return{injectedFault:'KeyboardMapping.map returns no transitions while the production Runtime bridge is active',route,bridgeState,neutralStart,before,suppressed,restoredBefore,restored,restorationVerified:true,suppressionIntervalMs:180,rawEvents:{before:rawBefore,after:rawAfter},hostEvents:{before:hostBefore,after:hostAfter}};
        });
        await nativeCheck('native-key-clear-while-held',async()=>{
          await resetInputState(activeEpoch);await page.evaluate(()=>window.__desktopSelectLayout('wasd'));
          const neutralStart=await waitObservation('Native keyboard clear returns to neutral input',state=>state.input.value===0,2000);
          await page.waitForTimeout(180);const before=await observe();assert.equal(before.input.value,neutralStart.input.value,'Native input must remain neutral after keyboard cleanup');
          let held,cleared,neutral,afterKeyup;
          const directionKey=before.position.x>320?'a':'d';
          try{
            await keyDown(directionKey);held=await waitObservation(`WASD ${directionKey.toUpperCase()} held before clear`,state=>state.input.value!==before.input.value&&(state.position.x-before.position.x)*(directionKey==='d'?1:-1)>0,3000);
            cleared=await page.evaluate(({game,epoch})=>{const runtimeFrame=document.querySelector('iframe').contentWindow;return runtimeFrame.__eaglerKeyboardInputBridge.submit({protocol:'eagler-touhou/1',game,epoch,command:'keyboard-clear'});},{game:target.game,epoch:activeEpoch});assert.equal(cleared,true);
            await page.waitForTimeout(180);neutral=await observe();assert.equal(neutral.input.value,before.input.value,'keyboard-clear must clear the mapped key at native input');
            await keyUp(directionKey);afterKeyup=await observe();assert.equal(afterKeyup.input.value,before.input.value,'keyup after clear must not reintroduce the key');
          }finally{await keyUp(directionKey);await resetInputState(activeEpoch);}
          return{neutralStart,before,held,clearAccepted:cleared,neutral,afterKeyup,clearIntervalMs:180,directionKey};
        });
        const sampleAudio=async(label,durationMs)=>{
          const captures=[],voiceSamples=[],start=Date.now(),end=start+durationMs;do{
            const sample=await frame.evaluate(game=>window.__eaglerDesktopAudioProbe?.capture?.(game)??
              {clock:null,captures:[],voices:{available:false,voiceCount:0,voices:[]}},target.game);
            const elapsedMs=Date.now()-start,localTaps=new Map();
            for(const capture of sample.captures){captures.push(capture);localTaps.set(capture.tapIndex,capture);}
            voiceSamples.push({elapsedMs,clock:sample.clock,voices:sample.voices,localTaps});
            await page.waitForTimeout(25);
          }while(Date.now()<end);
          const captureWindowEndedAtMs=Date.now();
          const frames=captures.map(item=>item.samples),nonZeroSamples=captures.reduce((sum,item)=>sum+item.nonZero,0),peakAbs=captures.reduce((peak,item)=>Math.max(peak,item.peakAbs),0),rmsMax=captures.reduce((peak,item)=>Math.max(peak,item.rms),0);
          const pcmByTap=new Map();for(const capture of captures){const total=pcmByTap.get(capture.tapIndex)??{tapIndex:capture.tapIndex,captures:0,nonZeroSamples:0,peakAbs:0,rmsMax:0};
            total.captures++;total.nonZeroSamples+=capture.nonZero;total.peakAbs=Math.max(total.peakAbs,capture.peakAbs);total.rmsMax=Math.max(total.rmsMax,capture.rms);pcmByTap.set(capture.tapIndex,total);}
          let voiceDiagnostics=null;
          if(voiceSamples.length){
            const activations=[],activeObservations=[],previous=new Map(),observedTapIndexes=new Set();
            for(let captureSequence=0;captureSequence<voiceSamples.length;captureSequence++){
              const sample=voiceSamples[captureSequence];
              for(const voice of sample.voices?.voices??[]){
                const key=voice.id,active=voice.playing||voice.node,prior=previous.get(key),tapPcm=sample.localTaps.get(voice.outputTapIndex);
                const clockEvidence={contextIndex:sample.clock?.contextIndex??null,contextSampleRate:sample.clock?.sampleRate??null,
                  observedAtContextTime:sample.clock?.currentTime??null,captureSequence};
                if(active&&(!prior?.active||prior.started!==voice.started))activations.push({elapsedMs:sample.elapsedMs,...clockEvidence,...voice,
                  pcm:tapPcm?{nonZero:tapPcm.nonZero,peakAbs:tapPcm.peakAbs,rms:tapPcm.rms}:null});
                if(active){activeObservations.push({elapsedMs:sample.elapsedMs,...clockEvidence,...voice,
                  pcm:tapPcm?{nonZero:tapPcm.nonZero,peakAbs:tapPcm.peakAbs,rms:tapPcm.rms}:null});
                  if(Number.isInteger(voice.outputTapIndex)&&voice.outputTapIndex>=0)observedTapIndexes.add(voice.outputTapIndex);}
                previous.set(key,{active,started:voice.started});
              }
            }
            const first=voiceSamples[0].voices,last=voiceSamples.at(-1).voices;
            voiceDiagnostics={available:voiceSamples.every(sample=>sample.voices?.available===true&&Array.isArray(sample.voices.voices)),
              clockSource:'SDL3.audioContext.currentTime',clockStart:voiceSamples[0].clock??null,clockEnd:voiceSamples.at(-1).clock??null,
              voiceCountStart:first?.voiceCount??null,voiceCountEnd:last?.voiceCount??null,
              snapshots:{start:first?.voices??null,end:last?.voices??null},activationEvents:activations,activeObservations,
              associatedOutputPcm:[...observedTapIndexes].sort((a,b)=>a-b).map(index=>pcmByTap.get(index)??{tapIndex:index,captures:0,nonZeroSamples:0,peakAbs:0,rmsMax:0})};
          }
          const samplePath=resolve(output,`${target.id}-${label}-webaudio.f32`);const flat=frames.flat(),buffer=Buffer.alloc(flat.length*4);flat.forEach((value,index)=>buffer.writeFloatLE(value,index*4));await writeFile(samplePath,buffer);
          const audioState=await nativePeek();
          return{durationMs,captureStartedAtMs:start,captureWindowEndedAtMs,captureFinishedAtMs:Date.now(),captureCount:captures.length,
            contextState:captures.at(-1)?.contextState??Object.values(audioState.audioContexts??{})[0]?.state??null,
            clockSource:'SDL3.audioContext.currentTime',clockStart:voiceSamples[0]?.clock??null,clockEnd:voiceSamples.at(-1)?.clock??null,
            contextStates:captures.map(item=>item.contextState),sampleRate:captures.find(item=>item.sampleRate)?.sampleRate??null,
            tapCount:audioState.audioProbe?.tapCount??0,contexts:audioState.audioProbe?.contexts??[],nonZeroSamples,peakAbs,rmsMax,
            ...(voiceDiagnostics?{nativeSfx:voiceDiagnostics}:{}),samplePath,bytes:buffer.length,sha256:hash(buffer)};
        };
        await check('audio-output-with-music-disabled',async()=>{
          await resetInputState(activeEpoch);
          const actionRequest=touchActionsRequest(target);
          if(!actionRequest.contract)return{checkStatus:'incomplete',mode:'none',
            reason:'Register the actual native player-consumer action observer before attributing a firing sound.'};
          const waitAction=async(label,predicate,timeoutMs=2500)=>{
            const start=Date.now();let action;
            do{
              action=await frame.evaluate(readTouchActionState,actionRequest);
              if(!action.available)return action;
              if(action.complete&&action.phase==='consumer-run'&&predicate(action))return action;
              await page.waitForTimeout(25);
            }while(Date.now()-start<timeoutMs);
            throw Error(`${label}: actual player-consumer condition not reached: ${JSON.stringify(action)}`);
          };
          const preflight=await observe(),eligibility=incompleteGameplayPrecondition(preflight,target.game);
          if(eligibility){record.audio={audioContexts:(await nativePeek()).audioContexts,mode:'none',nativeInputWindow:{before:preflight,held:null,after:null}};
            return{checkStatus:'incomplete',...record.audio,reason:'Native gameplay and input eligibility were not established before the firing sample.',eligibility:eligibility.eligibility};}
          const idle=await sampleAudio('idle',400);
          const preKey=await waitObservation('Native input returns to neutral before the firing sample',state=>state.input.value===0,2500);
          const preKeyEligibility=incompleteGameplayPrecondition(preKey,target.game);
          if(preKeyEligibility){record.audio={idle,audioContexts:(await nativePeek()).audioContexts,mode:'none',nativeInputWindow:{before:preKey,held:null,after:null}};
            return{checkStatus:'incomplete',...record.audio,reason:'Native gameplay eligibility ended before the firing sample.',eligibility:preKeyEligibility.eligibility};}
          const baselineConsumer=await waitAction('Neutral player-consumer before firing',action=>action.consumed.fire===false);
          if(!baselineConsumer.available)return{checkStatus:'incomplete',idle,mode:'none',reason:baselineConsumer.reason};
          await frame.locator('canvas').focus();
          const inputFocus=await frame.evaluate(()=>({focused:document.hasFocus(),
            canvasActive:document.activeElement===document.querySelector('canvas'),
            visibilityState:document.visibilityState,hidden:document.hidden}));
          if(!inputFocus.focused||!inputFocus.canvasActive||inputFocus.hidden||inputFocus.visibilityState!=='visible')
            return{checkStatus:'incomplete',idle,mode:'none',inputFocus,
              reason:'The Runtime canvas did not acquire visible document focus before the trusted fire-key capture.'};
          const inputStart=await frame.evaluate(({game,code})=>window.__eaglerDesktopAudioProbe?.beginInputWindow?.(game,code)??null,
            {game:target.game,code:'KeyZ'});
          if(!inputStart?.clock?.available||!Number.isSafeInteger(inputStart.sessionId)||inputStart.historyReady!==true){await frame.evaluate(()=>window.__eaglerDesktopAudioProbe?.endInputWindow?.());
            return{checkStatus:'incomplete',idle,mode:'none',sfxHistory:{ready:inputStart?.historyReady===true,error:inputStart?.historyError??null,
              taps:inputStart?.historyTaps??[],capacityBytes:inputStart?.historyCapacityBytes??null,readiness:inputStart?.historyReadiness??null},
              reason:inputStart?.historyError??inputStart?.historyReadiness?.reason??'The SDL3 audio clock, input-session identity, or rendered post-output PCM history was unavailable before sending the real fire key.'};}
          const waitForSfxStart=async(timeoutMs=2500)=>{
            const startedAt=Date.now();let snapshot;
            do{
              snapshot=await frame.evaluate(()=>window.__eaglerDesktopAudioProbe?.startJournalSnapshot?.()??null);
              if(snapshot?.available!==true||snapshot.sessionId!==inputStart.sessionId||snapshot.game!==target.game||snapshot.code!=='KeyZ')
                return{status:'incomplete',snapshot,reason:'The Runtime SFX start journal is unavailable or belongs to another input session.'};
              if(snapshot.drops!==0||snapshot.unattributed!==0||snapshot.errors!==0||!Array.isArray(snapshot.inputEvents)||!Array.isArray(snapshot.events))
                return{status:'incomplete',snapshot,reason:'The bounded Runtime SFX start journal dropped or could not attribute an event.'};
              const downs=snapshot.inputEvents.filter(event=>event.type==='keydown'&&event.code==='KeyZ'&&event.isTrusted===true);
              if(downs.length>1)return{status:'incomplete',snapshot,reason:'The Runtime SFX session contains multiple trusted fire-key downs.'};
              if(downs.length===1){
                const starts=snapshot.events.filter(event=>event.contextIndex===downs[0].contextIndex&&event.startContextTime>=downs[0].contextTime);
                if(starts.length){
                  if(starts.some(event=>event.tapMatchCount!==1||!Number.isInteger(event.outputTapIndex)||event.outputTapIndex<0||
                    !Number.isInteger(event.sourceNodeIdentity?.nodeId)||!event.voiceId))
                    return{status:'incomplete',snapshot,reason:'A Runtime start occurred but its voice and output tap identity were not unique.'};
                  return{status:'observed',snapshot,firstSequence:starts[0].sequence,startCount:starts.length};
                }
              }
              await page.waitForTimeout(10);
            }while(Date.now()-startedAt<timeoutMs);
            return{status:'timeout',snapshot,reason:'No Runtime SFX start was journaled before the bounded fire-key wait ended.'};
          };
          let held,firing,postKey,inputCapture,actionError,heldConsumer,releasedConsumer,sfxStartWait;
          try{
            await keyDown('KeyZ');
            heldConsumer=await waitAction('Player-consumer creates a shot for the held fire key',action=>
              action.sequence>baselineConsumer.sequence&&action.stage===baselineConsumer.stage&&
              action.consumed.fire===true&&action.activePlayerShots>baselineConsumer.activePlayerShots);
            assert.equal(heldConsumer.available,true,'The actual player-consumer observer disappeared while firing');
            held=await waitObservation('Native fire input remains held after shot creation',state=>state.input.value!==preKey.input.value,2500);
            sfxStartWait=await waitForSfxStart();
          }catch(error){actionError=error;}
          finally{await keyUp('KeyZ').catch(()=>{});}
          ({firing,inputCapture}=await collectDesktopSfxWindow({
            capture:()=>sampleAudio('firing',1000),
            operation:async()=>{
              releasedConsumer=await waitAction('Player-consumer releases the fire key',action=>
                action.sequence>(heldConsumer?.sequence??baselineConsumer.sequence)&&action.stage===baselineConsumer.stage&&action.consumed.fire===false);
              postKey=await waitObservationWithAudioClock('Native fire input releases to its prior value',
                state=>state.input.value===preKey.input.value,2500);
            },
            finish:()=>frame.evaluate(()=>window.__eaglerDesktopAudioProbe?.endInputWindow?.()??null),
          }));
          if(actionError)throw actionError;
          const startClock=inputStart.clock,endClock=postKey.audioClock;
          firing.actionWindow={source:'SDL3.audioContext.currentTime',game:target.game,code:'KeyZ',sessionId:inputStart.sessionId,
            startContextTime:startClock.currentTime,endContextTime:endClock?.currentTime,
            contextIndex:startClock.contextIndex,sampleRate:startClock.sampleRate,inputCapture,inputFocus,
            sfxStartWait,historyReadiness:inputStart.historyReadiness,
            clockEvidence:{start:startClock,end:endClock},releaseEvidence:{source:'readRuntimeObservation.input',
              baselineValue:preKey.input.value,consumedValue:held.input.value,nativeValue:postKey.input.value,
              contextIndex:endClock?.contextIndex,currentTime:endClock?.currentTime,
              consumer:{baseline:baselineConsumer,held:heldConsumer,released:releasedConsumer}}};
          const inputWindowValidation=validateDesktopSfxInputWindow(firing.actionWindow);
          firing.actionWindow.validation=inputWindowValidation;
          const postEligibility=incompleteGameplayPrecondition(postKey,target.game);
          record.audio={idle,firing,audioContexts:(await nativePeek()).audioContexts,mode:'none',
            nativeInputWindow:{before:preKey,held,after:postKey,consumedValue:held.input.value,releasedValue:postKey.input.value}};
          if(!inputWindowValidation.valid)return{checkStatus:'incomplete',...record.audio,
            reason:inputWindowValidation.reason};
          if(postEligibility)return{checkStatus:'incomplete',...record.audio,
            reason:'Native gameplay eligibility was lost before the firing sample completed.',eligibility:postEligibility.eligibility};
          if(sfxStartWait?.status==='incomplete')return{checkStatus:'incomplete',...record.audio,
            reason:sfxStartWait.reason,sfxStartWait};
          const startEvidence=mapDesktopSfxStartJournal(inputCapture,firing.nativeSfx);
          firing.nativeSfx={...firing.nativeSfx,sampledActivationEvents:firing.nativeSfx?.activationEvents??[],
            activationEvents:startEvidence.activationEvents,activeObservations:startEvidence.activeObservations,
            sourceIntervals:startEvidence.sourceIntervals,
            startJournalEvidence:{available:startEvidence.available,reason:startEvidence.reason,sessionId:startEvidence.sessionId,
              drops:startEvidence.drops,unattributed:startEvidence.unattributed,errors:startEvidence.errors}};
          record.audio.firing=firing;
          if(!startEvidence.available)return{checkStatus:'incomplete',...record.audio,nativeSfxStartEvidence:startEvidence,
            reason:startEvidence.reason};
          const sfxCorrelation=correlateInputSfxWindow(idle,firing);
          if(sfxCorrelation.status==='failed')assert.fail(sfxCorrelation.reason);
          if(sfxCorrelation.status==='incomplete')return{checkStatus:'incomplete',...record.audio,nativeSfxCorrelation:sfxCorrelation,
            reason:sfxCorrelation.reason};
          assert.equal(firing.contextState,'running','The Runtime WebAudio context must be running');
          assert.ok(firing.tapCount>0,'No real WebAudio destination path was observed');
          return{...record.audio,nativeSfxCorrelation:sfxCorrelation};
        });
        async function retireAndLaunch(epoch,mode,focusMode,extraOptions={},preparation={}){
          const priorEpoch=activeEpoch;
          assert.ok(Number.isSafeInteger(epoch)&&epoch>priorEpoch,`Next Runtime epoch ${epoch} must be greater than active epoch ${priorEpoch}`);
          await resetInputState(priorEpoch,'retireAndLaunch.resetInputState');
          const prior=frame;
          await runLifecyclePhase('retireAndLaunch.frame-goto-about-blank',()=>prior.goto('about:blank'),{activeEpoch:priorEpoch,plannedEpoch:epoch});
          await runLifecyclePhase('retireAndLaunch.wait-after-retire',()=>page.waitForTimeout(100),{activeEpoch:priorEpoch,plannedEpoch:epoch});
          const pagehide=await page.evaluate(oldEpoch=>lifecycleEvents.some(event=>event.type==='pagehide'&&event.epoch===oldEpoch),priorEpoch);
          await stage('runtime-navigation',{priorEpoch,epoch},()=>page.evaluate(next=>mountEpoch(next),epoch));activeEpoch=epoch;
          headlessLifecycle?.setPhase('retireAndLaunch.epoch-mounted',{activeEpoch:epoch,plannedEpoch:epoch});
          await runLifecyclePhase('retireAndLaunch.acquire-host-focus',()=>acquireForeground(epoch,'host-focus-before-ready'),{activeEpoch:epoch,plannedEpoch:epoch});
          await stage('ready',{epoch,event:'ready'},()=>waitRuntimeEvent('ready',epoch,`desktop-ready-${epoch}`));ensureFrame();
          await stage('keyboard-bridge',{epoch},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
          const state=await runLifecyclePhase('retireAndLaunch.launchEpoch',()=>launchEpoch(epoch,mode,focusMode,extraOptions,preparation),{activeEpoch:epoch});started=state;return{priorEpoch,epoch,pagehide,state};
        };
        const touchModeProbe=async(mode,epoch)=>{
          const precondition=await observe();
          const preconditionIssue=incompleteGameplayPrecondition(precondition,target.game);
          if(preconditionIssue)return{...preconditionIssue,mode,epoch};
          await resetInputState(epoch);await page.waitForTimeout(180);
          const start=await observe(),eligibility=incompleteGameplayPrecondition(start,target.game);
          if(eligibility)return{...eligibility,mode,epoch};
          const baseInput=start.input.value,actions=[];
          const touchNeutral=value=>gameplayReady(value)&&value.input.value===baseInput&&
            (target.game!=='th09'||value.motionSample?.enabled===false);
          try{
          if(mode==='touch'||mode==='touch-unlimited'){
            await cancelTouch();await touchControls();
            const before=await observe(),sign=before.position.x>192?-1:1;
            await directTouch('down',100+epoch,.5,.75);await directTouch('move',100+epoch,.5+sign*.08,.75);
            let moved;try{moved=await waitObservation(`${mode} drag native movement`,value=>
              gameplayReady(value)&&(value.position.x-before.position.x)*sign>0,3000);}finally{await directTouch('up',100+epoch,.5+sign*.08,.75);await cancelTouch();}
            const neutral=await waitObservation(`${mode} drag cancel releases input`,touchNeutral,2500);
            actions.push({kind:'drag',before,moved,neutral,sign,deltaX:moved.position.x-before.position.x});
          }else{
            const vectors=[[-1,0],[1,0],[0,-1],[0,1],[-1,-1],[1,1],[-1,1],[1,-1]];
            for(const [x,y] of vectors){
              await touchControls();await waitObservation(`${mode} neutral before ${x},${y}`,touchNeutral,1500);
              const before=await observe();await touchControls({joystickX:x*32767,joystickY:y*32767});
              let held;try{held=await waitObservation(`${mode} joystick ${x},${y} native movement`,value=>
                gameplayReady(value)&&(x===0||((value.position.x-before.position.x)*x>0))&&(y===0||((value.position.y-before.position.y)*y>0)),2000);}
              finally{await touchControls();}
              const released=await waitObservation(`${mode} joystick release ${x},${y}`,touchNeutral,1500);
              actions.push({kind:'joystick-direction',x,y,before,held,released});
            }
            if(mode==='joystick-free'){
              const magnitudes=[];
              for(const ratio of [.4,1]){
                await touchControls();const before=await observe();await touchControls({joystickX:Math.round(32767*ratio)});await page.waitForTimeout(240);const after=await observe();
                await touchControls();await waitObservation(`joystick-free ${ratio} release`,touchNeutral,1500);
                magnitudes.push({ratio,before,after,deltaX:Math.abs(after.position.x-before.position.x)});
              }
              assert.ok(magnitudes[0].deltaX>0&&magnitudes[1].deltaX>magnitudes[0].deltaX,
                'Free joystick input magnitude must produce proportionally ordered native displacement');actions.push({kind:'magnitude',values:magnitudes});
            }
          }
          await cancelTouch();await touchControls();
          const neutral=await waitObservation(`${mode} end neutral`,touchNeutral,2000);
          return{mode,epoch,start,actions,neutral};
          }catch(error){
            const finalState=await observe().catch(()=>null),finalEligibility=incompleteGameplayPrecondition(finalState,target.game);
            if(finalEligibility)return{...finalEligibility,mode,epoch,start,actions,actionError:String(error.stack||error)};
            throw error;
          }finally{await resetInputState(epoch);}
        };
        await check('native-four-direction-wasd',async()=>{
          await resetInputState(activeEpoch);
          const matrix=await directionCase('wasd',[
            ['w','ArrowUp','y',-1],['s','ArrowDown','y',1],['a','ArrowLeft','x',-1],['d','ArrowRight','x',1]]);
          return{epoch:activeEpoch,reusedRuntime:true,...matrix};
        });
        for(const [epoch,mode] of [[4,'touch-unlimited'],[5,'joystick'],[6,'joystick-free']]){
          const modeChecks=[`runtime-epoch-touch-mode-${mode}`,
            ...(mode==='touch-unlimited'?['shared-touch-unlimited']:mode==='joystick-free'?['shared-touch-joystick-free']:[])];
          if(requiredCheckIds&&!modeChecks.some(id=>requiredCheckIds.has(id))){
            for(const id of modeChecks)await check(id,async()=>{throw Error('An omitted mode must not launch a Runtime');});
            continue;
          }
          let transition;
          if(mode==='touch-unlimited')await check('shared-touch-unlimited',async()=>{
            if(record.touchTraceUnavailable)return{checkStatus:'incomplete',reason:record.touchTraceUnavailable};
            transition=await retireAndLaunch(epoch,mode);
            assert.ok(transition.pagehide,`Prior Runtime epoch ${transition.priorEpoch} did not report pagehide`);
            return{transition,...await runSharedTouch(2)};
          });
          if(mode==='joystick-free')await check('shared-touch-joystick-free',async()=>{
            if(record.touchTraceUnavailable)return{checkStatus:'incomplete',reason:record.touchTraceUnavailable};
            transition=await retireAndLaunch(epoch,mode);
            assert.ok(transition.pagehide,`Prior Runtime epoch ${transition.priorEpoch} did not report pagehide`);
            return{transition,...await runSharedTouch('joystick-free')};
          });
          await check(`runtime-epoch-touch-mode-${mode}`,async()=>{
            transition??=await retireAndLaunch(epoch,mode);assert.ok(transition.pagehide,`Prior Runtime epoch ${transition.priorEpoch} did not report pagehide`);
            const evidence=await touchModeProbe(mode,epoch);return{...transition,...evidence};
          });
        }
        await check('shared-touch-limited-restored',async()=>{
          if(record.touchTraceUnavailable)return{checkStatus:'incomplete',reason:record.touchTraceUnavailable};
          const transition=await retireAndLaunch(activeEpoch+1,'touch');
          assert.ok(transition.pagehide,'The previous movement configuration must be retired before restoring limited touch');
          return{transition,restoredUnlimited:false,...await runSharedTouch(1)};
        });
        await check('shared-touch-two-finger-focus',async()=>{
          if(record.touchTraceUnavailable)return{checkStatus:'incomplete',reason:record.touchTraceUnavailable};
          const transition=await retireAndLaunch(activeEpoch+1,'touch','two-finger');
          assert.ok(transition.pagehide,'The prior focus configuration must be retired before two-finger focus');
          return{transition,...await runSharedTouch(1,{focusMode:'two-finger',scenarioSet:'input-ownership'})};
        });
        if(target.game==='th09'){
          await check('touch-fire-focus-and-mixed-input',async()=>{
            const precondition=await observe(),eligibility=incompleteTh09TouchPrecondition(precondition);
            if(eligibility)return{...eligibility,stage:'before-native-touch-fire'};
            const idle=await sampleAudio('touch-fire-idle',350);let touchFire,touchCapture,touchStartAtMs,touchStateAcceptedAtMs;
            try{
              touchCapture=sampleAudio('touch-fire-enabled',1000);touchStartAtMs=Date.now();
              await touchControls({fireEnabled:true});touchStateAcceptedAtMs=Date.now();touchFire=await touchCapture;
            }finally{
              await touchControls().catch(()=>{});await cancelTouch().catch(()=>{});
              if(touchCapture&&!touchFire)touchFire=await touchCapture;
            }
            if(touchFire&&Number.isFinite(touchStartAtMs)&&Number.isFinite(touchStateAcceptedAtMs))
              touchFire.actionWindow={startElapsedMs:touchStartAtMs-touchFire.captureStartedAtMs,
                endElapsedMs:touchStateAcceptedAtMs-touchFire.captureStartedAtMs+150};
            const postcondition=await observe(),postEligibility=incompleteTh09TouchPrecondition(postcondition);
            record.th09TouchFire={precondition,idle,touchFire,postcondition};
            if(postEligibility)return{checkStatus:'incomplete',...record.th09TouchFire,
              reason:'Native TH09 touch context or player readiness ended during the fire-enabled sample.',eligibility:postEligibility.eligibility};
            const sfxCorrelation=correlateSfxWindow(idle,touchFire);
            if(sfxCorrelation.status==='failed')assert.fail(sfxCorrelation.reason);
            if(sfxCorrelation.status==='incomplete')return{checkStatus:'incomplete',...record.th09TouchFire,nativeSfxCorrelation:sfxCorrelation,
              reason:sfxCorrelation.reason};
            return{...record.th09TouchFire,nativeSfxCorrelation:sfxCorrelation};
          });
        }else{
          await nativeCheck('touch-fire-focus-and-mixed-input',async()=>{
            await cancelTouch();await touchControls();await keyUp('KeyZ').catch(()=>{});await keyUp('ShiftLeft').catch(()=>{});
            try{
              const calibration=record.keyboardCalibration={};
              const neutralStart=await waitObservation('Native input neutral before keyboard calibration',state=>state.input.value===0,2000);
              await page.waitForTimeout(180);const neutral=await observe();assert.equal(neutral.input.value,neutralStart.input.value,'Native input must stay neutral before keyboard calibration');calibration.neutral=neutral;
              await keyDown('KeyZ');let keyFire;try{keyFire=await waitObservation('Keyboard fire capture',state=>state.input.value!==neutral.input.value,2000);}finally{await keyUp('KeyZ');}
              calibration.keyFire=keyFire;const fireReleasedStart=await waitObservation('Keyboard fire release returns to neutral',state=>state.input.value===neutral.input.value,2000);
              await page.waitForTimeout(180);const fireReleased=await observe();assert.equal(fireReleased.input.value,neutral.input.value,'Fire input must remain released before focus calibration');calibration.fireReleased={start:fireReleasedStart,stable:fireReleased};
              const fireMask=keyFire.input.value^neutral.input.value;assert.notEqual(fireMask,0,'KeyZ must have a readable native fire effect');
              await keyDown('ShiftLeft');let keyFocus;try{keyFocus=await waitObservation('Keyboard focus capture',state=>state.input.value!==neutral.input.value,2000);}finally{await keyUp('ShiftLeft');}
              calibration.keyFocus=keyFocus;const focusReleasedStart=await waitObservation('Keyboard focus release returns to neutral',state=>state.input.value===neutral.input.value,2000);
              await page.waitForTimeout(180);const focusReleasedNative=await observe();assert.equal(focusReleasedNative.input.value,neutral.input.value,'Focus input must remain released before touch calibration');calibration.focusReleased={start:focusReleasedStart,stable:focusReleasedNative};
              const focusMask=keyFocus.input.value^neutral.input.value;assert.notEqual(focusMask,0,'Shift must have a readable native focus effect');
              await touchControls({fireEnabled:true});const touchFire=await waitObservation('Touch fire effect',state=>(state.input.value&fireMask)===fireMask,2500);
              await touchControls({focusEnabled:true});const touchFocus=await waitObservation('Touch focus effect',state=>(state.input.value&focusMask)===focusMask,2500);
              await touchControls();const focusReleased=await waitObservation('Touch focus release',state=>(state.input.value&focusMask)===0,2500);
              await touchControls({fireEnabled:true});await keyDown('KeyZ');const both=await waitObservation('Mixed key and touch fire',state=>(state.input.value&fireMask)===fireMask,2000);
              await keyUp('KeyZ');const touchOnly=await waitObservation('Keyboard release preserves touch fire',state=>(state.input.value&fireMask)===fireMask,2000);
              await keyDown('KeyZ');await touchControls();const keyboardOnly=await waitObservation('Touch release preserves keyboard fire',state=>(state.input.value&fireMask)===fireMask,2000);
              await cancelTouch();await touchControls();const afterCancel=await waitObservation('Touch cancel leaves keyboard holder',state=>(state.input.value&fireMask)===fireMask,2000);
              await keyUp('KeyZ');const allReleased=await waitObservation('Both fire holders released',state=>(state.input.value&fireMask)===0,2500);
              return{neutral,keyFire,fireMask,keyFocus,focusMask,touchFire,touchFocus,focusReleased,both,touchOnly,keyboardOnly,afterCancel,allReleased};
            }finally{await keyUp('KeyZ');await keyUp('ShiftLeft');await resetInputState(activeEpoch);}
          });
        }
        await check('native-pause-state-and-freeze',async()=>{
          const before=await observe();
          if(before.capabilities.pause!==true)return{checkStatus:'incomplete',reason:'No native pause observer is available for the active epoch.',precondition:before};
          const eligibility=incompleteGameplayPrecondition(before,target.game);
          if(eligibility)return{...eligibility,stage:'before-pause-input'};
          await key('Escape',90);const paused=await waitObservation('native pause state',state=>state.pause?.active===true,4000);assert.equal(paused.phase?.name,'paused');
          let frozen=null,worldClock=null,worldFreezeVerified=false,controlTickEvidence=null;
          if(Number.isFinite(paused.frames.simulation)){
            worldClock='simulation';await page.waitForTimeout(400);frozen=await observe();assert.equal(frozen.frames.simulation,paused.frames.simulation,'World simulation must freeze while paused');worldFreezeVerified=true;
          }else if(target.game==='th08'&&Number.isFinite(paused.worldProgress?.counters?.stage)&&Number.isFinite(paused.worldProgress?.counters?.enemySceneFrame)&&Number.isFinite(paused.position?.x)&&Number.isFinite(paused.position?.y)){
            worldClock='th08.stage+enemySceneFrame+playerPosition';await page.waitForTimeout(400);frozen=await observe();
            assert.equal(frozen.worldProgress?.counters?.stage,paused.worldProgress.counters.stage,'Stage must remain stable while paused');
            assert.equal(frozen.worldProgress?.counters?.enemySceneFrame,paused.worldProgress.counters.enemySceneFrame,'Enemy scene simulation must remain stable while paused');
            assert.equal(frozen.position.x,paused.position.x,'Player X must remain stable while paused');assert.equal(frozen.position.y,paused.position.y,'Player Y must remain stable while paused');
            controlTickEvidence={paused:paused.worldProgress.counters.controlTick,frozen:frozen.worldProgress?.counters?.controlTick,interpretation:'Recorded separately; it is not used as a world simulation clock'};worldFreezeVerified=true;
          }
          await key('Escape',90);await waitObservation('native unpause state',state=>state.pause?.active===false,4000);
          return{...(!worldFreezeVerified?{checkStatus:'incomplete'}:{}),paused,frozen,worldClock,worldFreezeVerified,controlTickEvidence,reason:worldFreezeVerified?null:'No authoritative world-progress observation is available; application tick is not used as a substitute'};
        });
        let backgroundTabEvidence;
        await check('window-minimize-foreground-and-release',async()=>{
          await resetInputState(activeEpoch);
          const before=await waitObservation('Native keyboard input is neutral before window minimize',state=>state.input.value===0,2000),eligibility=incompleteGameplayPrecondition(before,target.game);
          if(eligibility)return{...eligibility,stage:'before-window-minimize'};
          await page.bringToFront();const nativeFocusBefore=await waitForNativeFocus(page);
          const nativePage=await readPageTargetWindow(cdp,page);
          if(!nativeFocusBefore.ready)return{checkStatus:'incomplete',before,nativeFocusBefore,nativePage,reason:'The game page did not have native visible and focused state before minimizing.'};
          if(nativePage.status!=='matched')return{checkStatus:'incomplete',before,nativeFocusBefore,nativePage,reason:'The game page could not be matched to one native Chrome target and window.'};
          windowInfo=nativePage.window;
          let held,minimized,focused,events,hostEvents,released,afterKeyup,boundsAfterMinimize,boundsAfterRestore,nativeFocusAfter,
            boundsAtHiddenSample,hiddenInput,eventsWhileMinimized,hostEventsWhileMinimized,minimizedKeyboardClear;
          const lifecycleStart=await page.evaluate(()=>lifecycleEvents.length),hostLifecycleStart=await page.evaluate(()=>hostKeyboardLifecycle.length);
          try{
          const directionKey=before.position.x>320?'ArrowLeft':'ArrowRight',sign=directionKey==='ArrowRight'?1:-1;
          await keyDown(directionKey);held=await waitObservation(`direction held before window minimize (${directionKey})`,state=>state.input.value!==before.input.value&&(state.position.x-before.position.x)*sign>0,3000);
          const hiddenStartAtMs=await page.evaluate(()=>performance.now());
          await cdp.send('Browser.setWindowBounds',{windowId:windowInfo.windowId,bounds:{windowState:'minimized'}});await page.waitForTimeout(650);
          boundsAfterMinimize=await cdp.send('Browser.getWindowBounds',{windowId:windowInfo.windowId});
          await page.waitForFunction(index=>hostKeyboardLifecycle.slice(index).some(item=>(item.reason==='host-blur'||item.reason==='host-hidden')&&item.accepted===true),
            hostLifecycleStart,{timeout:1400}).catch(()=>{});
          hostEventsWhileMinimized=await page.evaluate(index=>hostKeyboardLifecycle.slice(index),hostLifecycleStart);
          const minimizedInputObservation=await sampleMinimizedNativeInput(before.input.value,1400);
          hiddenInput=minimizedInputObservation.state;
          boundsAtHiddenSample=await cdp.send('Browser.getWindowBounds',{windowId:windowInfo.windowId});
          const nativeState=await nativeFocusState(page),hiddenSampledAtMs=await page.evaluate(()=>performance.now());
          eventsWhileMinimized=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);
          minimizedKeyboardClear=inspectMinimizedKeyboardClear({windowState:boundsAtHiddenSample.bounds?.windowState,nativeState,
            events:eventsWhileMinimized.filter(item=>item.epoch===activeEpoch),hostEvents:hostEventsWhileMinimized,
            hiddenStartAtMs,hiddenSampledAtMs,inputValue:minimizedInputObservation.inputValue,
            neutralInputValue:before.input.value,inputObservation:minimizedInputObservation});
          minimized={nativeState,windowState:boundsAtHiddenSample.bounds?.windowState,bounds:boundsAtHiddenSample.bounds,boundsInitially:boundsAfterMinimize.bounds,hiddenStartAtMs,hiddenSampledAtMs,
            hiddenInput,minimizedInputObservation,hostKeyboardLifecycle:hostEventsWhileMinimized,lifecycleEvents:eventsWhileMinimized,minimizedKeyboardClear,
            lifecycle:inspectNativeBackgroundEvents({state:nativeState,events:eventsWhileMinimized.filter(item=>item.epoch===activeEpoch)})};
          await cdp.send('Browser.setWindowBounds',{windowId:windowInfo.windowId,bounds:{windowState:'normal'}});await page.bringToFront();
          nativeFocusAfter=await waitForNativeFocus(page);boundsAfterRestore=await cdp.send('Browser.getWindowBounds',{windowId:windowInfo.windowId});
          focused=await observe();events=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);hostEvents=await page.evaluate(index=>hostKeyboardLifecycle.slice(index),hostLifecycleStart);released=focused.input.value===before.input.value;
          await keyUp(directionKey);afterKeyup=await observe();
          }catch(error){const finalState=await observe().catch(()=>null),finalEligibility=incompleteGameplayPrecondition(finalState,target.game);
            if(finalEligibility)return{...finalEligibility,before,nativeFocusBefore,nativePage,held,minimized,focused,events,hostEvents,actionError:String(error.stack||error)};
            throw error;
          }finally{await keyUp(before.position.x>320?'ArrowLeft':'ArrowRight');await resetInputState(activeEpoch);}
          const activeEvents=events.filter(item=>item.epoch===activeEpoch),lifecycle=inspectNativeBackgroundEvents({state:minimized.nativeState,events:activeEvents});
          const backgroundVisibilityReached=lifecycle.visibilityHidden||lifecycle.visibilityChange,
            focusEventReached=lifecycle.blur||hostEvents.some(item=>item.reason==='host-blur'),
            minimizeEvidence={before,nativeFocusBefore,nativePage,held,minimized,focused,released,afterKeyup,boundsAfterMinimize,boundsAfterRestore,minimizedKeyboardClear,
              nativeFocusAfter,lifecycleEvents:events,hostKeyboardLifecycle:hostEvents,backgroundVisibilityReached,focusEventReached,
              directionKey:before.position.x>320?'ArrowLeft':'ArrowRight'};
          if(minimized.windowState!=='minimized')return{checkStatus:'incomplete',...minimizeEvidence,reason:'Chrome did not report the owned window in minimized state.'};
          if(!nativeFocusAfter?.ready)return{checkStatus:'incomplete',...minimizeEvidence,reason:'The restored game page did not regain native visible and focused state.'};
          if(!minimizeEvidence.focusEventReached&&!minimizeEvidence.backgroundVisibilityReached)
            return{checkStatus:'incomplete',...minimizeEvidence,reason:'Chromium minimized the window without hiding the document or blurring the Runtime'};
          if(minimizedKeyboardClear.status==='incomplete')return{checkStatus:'incomplete',...minimizeEvidence,
            reason:minimizedKeyboardClear.missing.join('; ')};
          if(minimizedKeyboardClear.status==='failed')return{checkStatus:'failed',...minimizeEvidence,
            reason:minimizedKeyboardClear.missing.join('; ')};
          assert.ok(released,'Foreground lifecycle must clear held native input');assert.equal(afterKeyup.input.value,before.input.value);
          return minimizeEvidence;
        });
        await check('background-tab-foreground-release',async()=>{
          const initial=await observe(),eligibility=incompleteGameplayPrecondition(initial,target.game);
          if(eligibility)return{...eligibility,stage:'before-background-tab'};
          await page.bringToFront();const nativeFocusBefore=await waitForNativeFocus(page);
          const gamePageTarget=await readPageTargetWindow(cdp,page);
          if(!nativeFocusBefore.ready)return{checkStatus:'incomplete',initial,nativeFocusBefore,gamePageTarget,reason:'The game page did not have native visible and focused state before switching tabs.'};
          if(gamePageTarget.status!=='matched')return{checkStatus:'incomplete',initial,nativeFocusBefore,gamePageTarget,reason:'The game page could not be matched to one native Chrome target and window.'};
          await resetInputState(activeEpoch);const neutralStart=await waitObservation('Native keyboard input is neutral before tab switch',state=>state.input.value===0,2000);
          await page.waitForTimeout(180);const before=await observe();assert.equal(before.input.value,neutralStart.input.value,'Native input must remain neutral before tab switch');
          const directionKey=before.position.x>320?'ArrowLeft':'ArrowRight',sign=directionKey==='ArrowRight'?1:-1;
          await page.evaluate(()=>document.body.focus());const lifecycleStart=await page.evaluate(()=>lifecycleEvents.length),hostLifecycleStart=await page.evaluate(()=>hostKeyboardLifecycle.length);
          backgroundAudioEventStart=await frame.evaluate(()=>window.__eaglerDesktopAudioProbe?.events?.length??0);const audioBefore=await nativePeek();
          let held,movementDuringHold,hidden,events,hostEvents,restored,afterReturn,afterKeyup,audioAfterReturn,backgroundPageTarget,windowComparison;
          let backgroundPage,backgroundPageClose={status:'not-created'};const backgroundTitle=`eagler-desktop-background-${randomUUID()}`;
          try{
            await keyDown(directionKey);held=await waitObservation(`native direction input held before background tab (${directionKey})`,state=>state.input.value!==before.input.value,3000);
            movementDuringHold=Number.isFinite(held.position?.x)&&Number.isFinite(before.position?.x)?(held.position.x-before.position.x)*sign>0:null;
            backgroundPage=await context.newPage();await backgroundPage.setContent(`<!doctype html><title>${backgroundTitle}</title><body>background tab target</body>`);
            backgroundPageTarget=await readPageTargetWindow(cdp,backgroundPage,{title:backgroundTitle});
            if(backgroundPageTarget.status!=='matched')return{checkStatus:'incomplete',nativeFocusBefore,gamePageTarget,backgroundPageTarget,before,held,
              reason:'The second page could not be matched to one native Chrome page target and window.'};
            windowComparison=compareNativeBrowserTargets({pageTarget:gamePageTarget.target,pageWindowId:gamePageTarget.window.windowId,
              otherTarget:backgroundPageTarget.target,otherWindowId:backgroundPageTarget.window.windowId});
            if(windowComparison.status!=='matched')return{checkStatus:'incomplete',nativeFocusBefore,gamePageTarget,backgroundPageTarget,windowComparison,before,held,
              reason:'The second page did not belong to the same native Chrome window as the game page.'};
            await backgroundPage.bringToFront();
            const backgroundState=await (async()=>{
              const started=Date.now();let state=null,samples=0;
              do{
                state=await nativeFocusState(page);samples++;
                const lifecycleEventsNow=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);
                if(state.hidden||state.visibilityState==='hidden'||state.hasFocus===false||lifecycleEventsNow.some(item=>item.epoch===activeEpoch&&item.type==='blur'))
                  return{state,samples,durationMs:Date.now()-started};
                await page.waitForTimeout(100);
              }while(Date.now()-started<5000);
              return{state,samples,durationMs:Date.now()-started};
            })();
            const backgroundEvents=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);
            hidden={...backgroundState,backgroundTarget:backgroundPageTarget,windowComparison,
              bounds:(await cdp.send('Browser.getWindowBounds',{windowId:backgroundPageTarget.window.windowId})).bounds,
              lifecycle:inspectNativeBackgroundEvents({state:backgroundState.state,events:backgroundEvents.filter(item=>item.epoch===activeEpoch)})};
            await page.bringToFront();const nativeFocusAfter=await waitForNativeFocus(page,5000);
            restored={...(await nativeFocusState(page)),focus:nativeFocusAfter,
              bounds:(await cdp.send('Browser.getWindowBounds',{windowId:gamePageTarget.window.windowId})).bounds};
            events=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);hostEvents=await page.evaluate(index=>hostKeyboardLifecycle.slice(index),hostLifecycleStart);
            audioAfterReturn=await nativePeek();
            const backgroundAudioEvents=await frame.evaluate(index=>window.__eaglerDesktopAudioProbe?.events?.slice(index)??[],backgroundAudioEventStart);
            const backgroundAudioLifecycle=classifyAudioLifecyclePath({beforeContexts:audioBefore.audioProbe?.contexts??[],
              afterContexts:audioAfterReturn.audioProbe?.contexts??[],stateEvents:backgroundAudioEvents.filter(item=>item.type==='state')});
            const activeEvents=events.filter(item=>item.epoch===activeEpoch),lifecycle=inspectNativeBackgroundEvents({state:hidden.state,events:activeEvents}),
              backgroundVisibilityReached=lifecycle.visibilityHidden||lifecycle.visibilityChange,
              focusEventReached=lifecycle.blur||hostEvents.some(item=>item.reason==='host-blur'),
              hostClearReached=hostEvents.some(item=>(item.reason==='host-blur'||item.reason==='host-hidden')&&item.accepted===true);
            const lifecycleEvidence={neutralStart,before,nativeFocusBefore,gamePageTarget,backgroundPageTarget,windowComparison,held,movementDuringHold,hidden,restored,audioBefore,audioAfterReturn,
              backgroundAudioLifecycle,backgroundAudioEvents,
              lifecycleEvents:events,hostKeyboardLifecycle:hostEvents,backgroundVisibilityReached,focusEventReached,hostClearReached,directionKey,
              browserPages:context.pages().map(item=>({url:item.url(),isGamePage:item===page,isBackgroundPage:item===backgroundPage}))};
            backgroundTabEvidence=lifecycleEvidence;
            if(!restored.focus?.ready)return{checkStatus:'incomplete',...lifecycleEvidence,reason:'The game page did not regain native visible and focused state after tab activation.'};
            if(!backgroundVisibilityReached&&!focusEventReached)return{checkStatus:'incomplete',...lifecycleEvidence,reason:'Switching to a second browser page produced no hidden or blur lifecycle event'};
            if(!hostClearReached)return{checkStatus:'incomplete',...lifecycleEvidence,reason:'A browser background event occurred without the host keyboard clear path accepting it'};
            afterReturn=await waitObservation('Host blur clears held native input before explicit keyup',state=>state.input.value===before.input.value,2500);
            await keyUp(directionKey);afterKeyup=await waitObservation('Held native keyboard input stays released after keyup',state=>state.input.value===before.input.value,2500);
          }catch(error){const finalState=await observe().catch(()=>null),finalEligibility=incompleteGameplayPrecondition(finalState,target.game);
            if(finalEligibility)return{...finalEligibility,neutralStart,before,nativeFocusBefore,gamePageTarget,backgroundPageTarget,windowComparison,held,movementDuringHold,hidden,restored,events,hostEvents,audioBefore,audioAfterReturn,
              actionError:String(error.stack||error)};
            throw error;
          }finally{
            await keyUp(directionKey).catch(()=>{});
            if(backgroundPage){try{await backgroundPage.close();backgroundPageClose={status:'closed'};}
              catch(error){backgroundPageClose={status:'failed',error:String(error.stack||error)};}}
            await page.bringToFront().catch(()=>{});await resetInputState(activeEpoch);
          }
          Object.assign(backgroundTabEvidence,{afterReturn,afterKeyup});
          assert.equal(afterReturn.input.value,before.input.value,'Host blur/hidden lifecycle must clear native held input before explicit keyup');
          const hostFocusAfterCleanup=await waitForNativeFocus(page,5000),runtimeFocusAfterCleanup=await waitForNativeFocus(frame,5000),pageAfterCleanup=await readPageTargetWindow(cdp,page);
          const postCleanupForeground=inspectPostCleanupForeground({hostState:hostFocusAfterCleanup.state,runtimeState:runtimeFocusAfterCleanup.state,
            beforePage:gamePageTarget,afterPage:pageAfterCleanup,backgroundPageCloseStatus:backgroundPageClose.status});
          backgroundTabEvidence.postCleanupForeground={...postCleanupForeground,hostFocus:hostFocusAfterCleanup,runtimeFocus:runtimeFocusAfterCleanup,
            pageTargetAfterCleanup:pageAfterCleanup,audio:await nativePeek(),lifecycleEvents:await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart)};
          if(!postCleanupForeground.ready)return{checkStatus:'incomplete',...backgroundTabEvidence,
            reason:'After closing the temporary tab, the game page did not retain native foreground state on its original Chrome target and window.'};
          return backgroundTabEvidence;
        });
        await check('background-audio-recovery',async()=>{
          if(!backgroundTabEvidence?.backgroundVisibilityReached||!backgroundTabEvidence.postCleanupForeground?.ready)
            return{checkStatus:'incomplete',backgroundTabEvidence,reason:'A hidden-to-visible transition and post-cleanup native foreground state on the original game page were not both established'};
          const gameplay=await observe(),gameplayIssue=incompleteGameplayPrecondition(gameplay,target.game);
          if(gameplayIssue)return{...gameplayIssue,backgroundTabEvidence,gameplay,reason:'Native gameplay eligibility was not established after foreground cleanup'};
          await resetInputState(activeEpoch);
          const neutralStart=await waitObservation('Native gameplay input is neutral before AUDIO-04 fire',state=>state.input.value===0,2000);
          await page.waitForTimeout(100);const neutral=await observe();
          if(neutral.input.value!==0)return{checkStatus:'incomplete',backgroundTabEvidence,neutralStart,neutral,
            reason:'The Runtime did not reach a neutral native input baseline before the foreground audio stimulus'};
          const hostFocus=await waitForNativeFocus(page,1000),runtimeFocus=await waitForNativeFocus(frame,1000),pageTarget=await readPageTargetWindow(cdp,page);
          const foreground=inspectPostCleanupForeground({hostState:hostFocus.state,runtimeState:runtimeFocus.state,
            beforePage:backgroundTabEvidence.gamePageTarget,afterPage:pageTarget,backgroundPageCloseStatus:backgroundTabEvidence.postCleanupForeground.temporaryPageClosed?'closed':'failed'});
          if(!foreground.ready)return{checkStatus:'incomplete',backgroundTabEvidence,foreground:{...foreground,hostFocus,runtimeFocus,pageTarget},neutral,
            reason:'The fresh pre-stimulus focus or native game page identity check was not satisfied'};
          const foregroundIdle=await sampleAudio('foreground-idle',400);
          if(foregroundIdle.captureCount<=0||foregroundIdle.tapCount<=0)
            return{checkStatus:'incomplete',backgroundTabEvidence,foreground,neutral,foregroundIdle,
              reason:'The Runtime WebAudio output tap did not provide a foreground baseline capture'};
          if(foregroundIdle.nonZeroSamples>0)
            return{checkStatus:'incomplete',backgroundTabEvidence,foreground,neutral,foregroundIdle,
              reason:'Foreground PCM was already active before KeyZ, so this capture cannot isolate the fire stimulus'};
          const audioBeforeFire=await nativePeek();
          const audioPreKey=await frame.evaluate(()=>{const probe=window.__eaglerDesktopAudioProbe;return{
            contexts:probe?.summary?.().contexts??[],eventCount:probe?.events?.length??0,time:performance.now()};});
          const audioContextGate=inspectAudioContextsBeforeGesture(audioPreKey.contexts);
          if(!audioContextGate.ready)return{checkStatus:'incomplete',backgroundTabEvidence,foreground,neutral,foregroundIdle,
            audioBeforeFire,audioPreKey,audioContextGate,audioLifecyclePath:backgroundTabEvidence.backgroundAudioLifecycle?.path??'unclassified',
            reason:'The Runtime AudioContext was not already running before the KeyZ gesture; this window cannot establish background audio continuity/recovery.'};
          const keyboardForwardStart=await page.evaluate(()=>keyboardForwardEvents.length),keyboardMapStart=await page.evaluate(()=>keyboardMapEvents.length);
          const audioEventsBefore=audioPreKey.eventCount;
          let keyDownEvent=null,held=null,heldError=null,heldObservedAtMs=null,foregroundAudio=null;
          const capturePromise=sampleAudio('foreground',1000).catch(error=>({captureError:String(error.stack||error)}));
          let afterCapture,afterKeyup;
          try{
            await keyDown('KeyZ');
            await page.waitForFunction(({index,epoch})=>keyboardForwardEvents.slice(index).some(item=>item.code==='KeyZ'&&item.down===true&&item.epoch===epoch),
              {index:keyboardForwardStart,epoch:activeEpoch},{timeout:1500}).catch(()=>{});
            keyDownEvent=await page.evaluate(({index,epoch})=>keyboardForwardEvents.slice(index).find(item=>item.code==='KeyZ'&&item.down===true&&item.epoch===epoch)??null,
              {index:keyboardForwardStart,epoch:activeEpoch});
            if(keyDownEvent?.accepted===true){
              try{held=await waitObservation('Native input consumes KeyZ during the foreground audio capture',state=>state.input.value!==neutral.input.value,900);
                heldObservedAtMs=Date.now();}
              catch(error){heldError=String(error.message||error);}
            }
            foregroundAudio=await capturePromise;afterCapture=await observe();
          }finally{await keyUp('KeyZ');}
          afterKeyup=await waitObservation('Native input returns to neutral after foreground KeyZ release',state=>state.input.value===neutral.input.value,2000).catch(async()=>observe());
          const forwardEvents=await page.evaluate(index=>keyboardForwardEvents.slice(index),keyboardForwardStart);
          const mappingEvents=await page.evaluate(index=>keyboardMapEvents.slice(index),keyboardMapStart);
          const fireStimulus=inspectNativeFireStimulus({forwardEvents,epoch:activeEpoch,neutralInputValue:neutral.input.value,
            heldInputValue:held?.input?.value});
          const heldObservedBeforeCaptureFinished=!!held&&Number.isFinite(heldObservedAtMs)&&
            Number.isFinite(foregroundAudio?.captureStartedAtMs)&&Number.isFinite(foregroundAudio?.captureWindowEndedAtMs)&&
            heldObservedAtMs>=foregroundAudio.captureStartedAtMs&&heldObservedAtMs<=foregroundAudio.captureWindowEndedAtMs;
          const nativeInputWindow={neutral:{simulationFrame:neutral.frames?.simulation,inputValue:neutral.input.value},
            held:held?{simulationFrame:held.frames?.simulation,inputValue:held.input.value}:null,
            afterCapture:{simulationFrame:afterCapture?.frames?.simulation,inputValue:afterCapture?.input?.value},
            afterKeyup:{simulationFrame:afterKeyup?.frames?.simulation,inputValue:afterKeyup?.input?.value},
            heldObservedBeforeCaptureFinished,heldError};
          const allAudioEvents=await frame.evaluate(index=>window.__eaglerDesktopAudioProbe?.events?.slice(index)??[],backgroundAudioEventStart);
          const stimulusEvents=allAudioEvents.slice(Math.max(0,audioEventsBefore-backgroundAudioEventStart));
          const stimulusStateChanges=stimulusEvents.filter(item=>item.type==='state').map(item=>({state:item.state,time:item.time}));
          const audioGestureWindow=inspectAudioGestureWindow({contextsBeforeGesture:audioPreKey.contexts,stateEvents:stimulusStateChanges,
            capturedContextStates:foregroundAudio.contextStates??[]});
          const afterAudio=await nativePeek(),audioLifecyclePath=backgroundTabEvidence.backgroundAudioLifecycle?.path??'context-state-transition-unclassified';
          const captureOverlappedNativeFire=nativeInputWindow.heldObservedBeforeCaptureFinished;
          const evidence={backgroundTabEvidence,foreground:{...foreground,hostFocus,runtimeFocus,pageTarget},neutralStart,neutral,foregroundIdle,
            audioBeforeFire,audioPreKey,audioContextGate,audioGestureWindow,nativeInputWindow,fireStimulus,keyboardForwardEvents:forwardEvents,keyboardMapEvents:mappingEvents,
            audioEvents:stimulusEvents,stimulusStateChanges,audioLifecyclePath,foregroundAudio,afterAudio,
            captureStartedBeforeKeyDown:true,keyDownStartedAfterAudioContextGate:true,captureOverlappedNativeFire};
          if(foregroundAudio?.captureError)return{checkStatus:'incomplete',...evidence,reason:'The Runtime audio capture did not complete'};
          if(!audioGestureWindow.ready)return{checkStatus:'incomplete',...evidence,
            reason:audioGestureWindow.missing.join('; ')};
          if(!fireStimulus.ready||!captureOverlappedNativeFire)
            return{checkStatus:'incomplete',...evidence,reason:'A bridge-accepted KeyZ with native input consumption was not captured during the foreground PCM window'};
          if(afterKeyup?.input?.value!==neutral.input.value)
            return{checkStatus:'incomplete',...evidence,reason:'Native input did not return to neutral after the foreground audio stimulus'};
          if(foregroundAudio.captureCount<=0||foregroundAudio.tapCount<=0)
            return{checkStatus:'incomplete',...evidence,reason:'The Runtime WebAudio output tap did not provide PCM captures'};
          if(foregroundAudio.contextState!=='running'||foregroundAudio.nonZeroSamples===0)
            return{checkStatus:'failed',...evidence,reason:'A bridge-accepted, natively consumed KeyZ produced no PCM on the live Runtime WebAudio output tap'};
          return{...evidence,foregroundOutputVerified:true};
        });
        await check('iframe-exit-and-new-epoch',async()=>{
          const priorEpoch=activeEpoch,nextEpoch=priorEpoch+1;
          const prior=frame;await cancelTouch().catch(()=>{});await touchControls().catch(()=>{});await prior.goto('about:blank');await page.waitForTimeout(100);
          const pagehide=await page.evaluate(oldEpoch=>lifecycleEvents.some(event=>event.type==='pagehide'&&event.epoch===oldEpoch),priorEpoch);assert.ok(pagehide,'Runtime iframe teardown must emit pagehide observation');
          await stage('runtime-navigation',{epoch:nextEpoch},()=>page.evaluate(epoch=>mountEpoch(epoch),nextEpoch));
          await acquireForeground(nextEpoch,'host-focus-before-ready');
          await stage('ready',{epoch:nextEpoch,event:'ready'},()=>waitRuntimeEvent('ready',nextEpoch,`desktop-ready-${nextEpoch}`));ensureFrame();
          await stage('keyboard-bridge',{epoch:nextEpoch},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
          const newState=await launchEpoch(nextEpoch,'touch');
          const staleRequest=`stale-${randomUUID()}`;await page.evaluate(({request,epoch})=>send('sync',{epoch,request}),{request:staleRequest,epoch:priorEpoch});await page.waitForTimeout(350);
          assert.equal(await page.evaluate(request=>events.some(event=>event.request===request),staleRequest),false,'An old epoch request must not be accepted after Runtime replacement');
          const newAudio=await nativePeek();
          return{oldEpoch:priorEpoch,newEpoch:nextEpoch,pagehideObserved:pagehide,staleRequestIgnored:true,newAudio,newState,audioContextSharingIsNotAssumed:true};
        });
        }
        record.events=await page.evaluate(()=>events);record.lifecycleEvents=await page.evaluate(()=>lifecycleEvents);record.hostKeyboardLifecycle=await page.evaluate(()=>hostKeyboardLifecycle);record.keyboardForwardEvents=await page.evaluate(()=>keyboardForwardEvents);record.keyboardMapEvents=await page.evaluate(()=>keyboardMapEvents);
        await flushFixtureAudit();
        const sameOriginErrors=record.httpErrors.filter(event=>event.url.startsWith(fixture.base));
        assert.deepEqual(sameOriginErrors,[],'Local Runtime resources returned HTTP errors');
        assert.deepEqual(record.errors.filter(item=>item.page),[],'Browser page errors occurred');
      }catch(error){
        if(error instanceof DesktopStartupPreconditionError){record.status='incomplete';record.incompleteReason=String(error.message);}
        else{record.status='failed';record.failure=String(error.stack||error);record.failedAt=new Date().toISOString();}
        await save();
      }
      finally{
        headlessLifecycle?.setPhase('target-finally-readback',{activeEpoch});
        if(page){
          try{
            Object.assign(record,await page.evaluate(readDesktopHostEvents));
            record.protocolErrors=record.events.filter(event=>event?.protocol==='eagler-touhou/1'&&
              ['error','runtime-error','fatal-error'].includes(event.event));
            if(record.protocolErrors.length)record.failure??='The Runtime reported a protocol error.';
          }catch(error){record.terminalHostReadFailure=String(error.stack||error);
            record.failure??='The Host event ledger could not be captured before browser cleanup.';}
        }
        if(frame){
          try{record.terminalRuntime=await nativePeek();
            record.terminalRuntime.document=await frame.evaluate(()=>({hidden:document.hidden,hasFocus:document.hasFocus(),
              userActivation:{isActive:navigator.userActivation?.isActive,hasBeenActive:navigator.userActivation?.hasBeenActive},
              loading:document.querySelector('#loading')?.textContent??null,error:document.querySelector('#error')?.textContent??null}));
            if(audioLatencyOnly)record.terminalSfxProbe=await frame.evaluate(probeGlobal=>globalThis[probeGlobal]?.snapshot(),sfxContract.probeGlobal);
          }catch(error){record.terminalRuntimeReadFailure=String(error.stack||error);}
        }
        await resetInputState(activeEpoch,'target-finally.resetInputState');
        if(fixtureAudit){
          await flushFixtureAudit().catch(error=>{record.fixtureAuditFailure=String(error.stack||error);record.failure??='The final fixture response audit could not be captured.';});
          await fixtureAudit.stop().catch(error=>{record.fixtureAuditFailure??=String(error.stack||error);record.failure??='The fixture response observer did not stop cleanly.';});
        }
        if(lifecycleController)record.focusEmulationGateway=lifecycleController.gatewaySnapshot();
        if(lifecycleController&&page&&cdp){
          try{
            const actualPage=await readPageTargetWindow(cdp,page),targetId=actualPage.target?.targetId??null;
            const focusRewriteProof=assessFocusRewriteForTarget(record.focusEmulationGateway,targetId);
            record.focusEmulationTargetAudit={actualPage,proof:focusRewriteProof};
            if(!focusRewriteProof.ready){
              const message=`CDP focus rewrite proof failed for the actual page target: ${focusRewriteProof.reason}`;
              record.failure??=message;record.errors.push({check:'focus-emulation-target-audit',error:message});
            }
          }catch(error){
            const message=`Could not audit CDP focus rewrites against the actual page target: ${error.message}`;
            record.failure??=message;record.errors.push({check:'focus-emulation-target-audit',error:message});
          }
        }
        else if(lifecycleController){
          const message='The actual Playwright page target was unavailable for CDP focus rewrite auditing.';
          record.failure??=message;record.errors.push({check:'focus-emulation-target-audit',error:message});
        }
        if(record.packageIdentity&&record.dataIdentity){
          await check('package-and-private-data-identity-postflight',async()=>{
            const packageIdentity=await verifyRuntimePackage(target,{repoRoot:root}),dataIdentity=await verifyPrivateData(target.data);
            assert.deepEqual(packageIdentity,record.packageIdentity,'Runtime source, build plan, receipts or package changed during desktop probes');
            assert.deepEqual(dataIdentity,record.dataIdentity,'Private DATA changed during desktop probes');
            record.packageIdentityPostflight=packageIdentity;record.dataIdentityPostflight=dataIdentity;
            return{packageSha256:packageIdentity.packageSha256,sourceBuild:packageIdentity.sourceBuild,privateData:dataIdentity};
          });
        }
        const scopeSummary=checkScope.requiredIds?summarizeDesktopCheckScope(record.checks,checkScope):null;
        const startupEvidenceSummary=summarizeDesktopStartupEvidence({stages:record.stages,
          readinessByEpoch:audioLatencyOnly?record.audioTitleReadinessByEpoch:record.gameplayReadinessByEpoch,readinessEvidence});
        record.startupEvidenceSummary=startupEvidenceSummary;
        recordDesktopStartupEvidenceIssue(record,startupEvidenceSummary);
        if(scopeSummary){record.requiredCheckSummary=scopeSummary;record.missingRequiredChecks=scopeSummary.missingRequired;}
        if(record.focusEmulationGateway?.errors?.length){record.failure??='The audited CDP focus gateway recorded protocol or transport errors.';record.gatewayErrors=record.focusEmulationGateway.errors;}
        record.status=record.failure||record.checks.some(item=>item.status==='failed')||scopeSummary?.status==='failed'||startupEvidenceSummary.status==='failed'?'failed':
          scopeSummary?.status==='incomplete'||startupEvidenceSummary.status!=='passed'?'incomplete':record.checks.every(item=>item.status==='passed')?'passed':'incomplete';
        if(page&&frame&&cdp)await captureRuntimeScreenshot('last',resolve(output,`${target.id}-last.png`));
        if(page){
          try{Object.assign(record,await page.evaluate(readDesktopHostEvents));}
          catch(error){record.terminalHostReadFailure=String(error.stack||error);record.failure??='The final Host ledger could not be captured.';}
          record.terminalErrorAudit=inspectDesktopTerminalErrors({...record,fixtureBase:fixture.base});
          record.protocolErrors=record.terminalErrorAudit.protocolErrors;
          if(!record.terminalErrorAudit.ready)record.failure??='The final browser or Runtime protocol audit reported an error.';
          if(record.failure)record.status='failed';
        }
        if(cdp){try{if(windowInfo)await cdp.send('Browser.setWindowBounds',{windowId:windowInfo.windowId,bounds:{windowState:'normal'}});}catch{}await cdp.detach().catch(()=>{});}
        if(context)await runLifecyclePhase('target-context-close',()=>context.close(),{activeEpoch,ownedTeardownPhase:'target-context-close'}).catch(()=>{});
        recordHeadlessLifecycleDiagnostics();
        if(record.headlessLifecycleErrors?.length)record.status='failed';
        detachHeadlessPage?.();detachHeadlessContext?.();
        record.finishedAt=new Date().toISOString();await save();
        console.log(`${target.id}: ${record.status.toUpperCase()} ${record.failure?.split('\n')[0]??record.checks.filter(item=>item.status!=='passed').map(item=>item.id+':'+item.status).join(', ')}`);
      }
    }
  }finally{
    try{
      if(lifecycleController){
        activeHeadlessLifecycleDiagnostics?.setPhase('run-browser-close',{ownedTeardownPhase:'run-browser-close'});
        report.browserCleanup=await lifecycleController.close();
        report.focusEmulationGateway=report.browserCleanup.gateway;
      }else{
        activeHeadlessLifecycleDiagnostics?.setPhase('run-browser-close',{ownedTeardownPhase:'run-browser-close'});
        await browser?.close();
      }
    }catch(error){
      report.browserCleanupFailure=String(error.stack||error);
      report.focusEmulationGateway=error.gatewaySnapshot??lifecycleController?.gatewaySnapshot?.()??null;
      for(const target of report.targets)if(target.status==='passed'){
        target.status='failed';target.failure='Browser CDP gateway reported an error during shutdown.';
      }
    }
    for(const item of headlessLifecycleDiagnostics)item.recordDiagnostics();
    if(headlessLifecycleDiagnostics.length){
      report.headlessLifecycleDiagnostics=headlessLifecycleDiagnostics.map(({diagnostics})=>diagnostics.snapshot());
      detachActiveHeadlessBrowserListener?.();
      for(const {diagnostics} of headlessLifecycleDiagnostics)diagnostics.detachAll();
      activeHeadlessLifecycleDiagnostics=null;detachActiveHeadlessBrowserListener=null;
    }
    await fixture?.close();
    report.finishedAt=new Date().toISOString();report.sourceSnapshotEnd=sourceSnapshot(root);
    report.sourceStable=report.sourceSnapshotStart.sha256===report.sourceSnapshotEnd.sha256;
    const currentHarness=await readFile(new URL(import.meta.url));
    report.touchModulesStable=true;
    for(const module of report.touchModules){
      module.postflightSha256=hash(await readFile(new URL(module.source)));
      module.stable=module.sha256===module.postflightSha256;
      if(!module.stable)report.touchModulesStable=false;
    }
    report.probeArtifacts.harnessPostflightSha256=hash(currentHarness);
    report.probeArtifacts.harnessStable=report.probeArtifacts.harness.sha256===report.probeArtifacts.harnessPostflightSha256;
    const currentStageRecorder=await readFile(new URL('./runtime-desktop-stage.mjs',import.meta.url));
    report.probeArtifacts.stageRecorderPostflightSha256=hash(currentStageRecorder);
    report.probeArtifacts.stageRecorderStable=report.probeArtifacts.stageRecorder.sha256===report.probeArtifacts.stageRecorderPostflightSha256;
    const currentReadiness=await readFile(new URL('./runtime-desktop-readiness.mjs',import.meta.url));
    report.probeArtifacts.gameplayReadinessPostflightSha256=hash(currentReadiness);
    report.probeArtifacts.gameplayReadinessStable=report.probeArtifacts.gameplayReadiness.sha256===report.probeArtifacts.gameplayReadinessPostflightSha256;
    const currentFixtureServer=await readFile(new URL('../browser/fixture-server.mjs',import.meta.url));
    report.probeArtifacts.fixtureServerPostflightSha256=hash(currentFixtureServer);
    report.probeArtifacts.fixtureServerStable=report.probeArtifacts.fixtureServer.sha256===report.probeArtifacts.fixtureServerPostflightSha256;
    const currentFixtureWatcher=await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url));
    report.probeArtifacts.fixtureResponseWatcherPostflightSha256=hash(currentFixtureWatcher);
    report.probeArtifacts.fixtureResponseWatcherStable=report.probeArtifacts.fixtureResponseWatcher.sha256===report.probeArtifacts.fixtureResponseWatcherPostflightSha256;
    const currentResponseBodyCapture=await readFile(new URL('../browser/response-body-capture.mjs',import.meta.url));
    report.probeArtifacts.responseBodyCapturePostflightSha256=hash(currentResponseBodyCapture);
    report.probeArtifacts.responseBodyCaptureStable=report.probeArtifacts.responseBodyCapture.sha256===report.probeArtifacts.responseBodyCapturePostflightSha256;
    const currentFocusGateway=await readFile(new URL('../browser/cdp-focus-gateway.mjs',import.meta.url));
    report.probeArtifacts.focusGatewayPostflightSha256=hash(currentFocusGateway);
    report.probeArtifacts.focusGatewayStable=report.probeArtifacts.focusGateway.sha256===report.probeArtifacts.focusGatewayPostflightSha256;
    const currentLifecycleClassifier=await readFile(new URL('./runtime-desktop-lifecycle.mjs',import.meta.url));
    report.probeArtifacts.lifecycleClassifierPostflightSha256=hash(currentLifecycleClassifier);
    report.probeArtifacts.lifecycleClassifierStable=report.probeArtifacts.lifecycleClassifier.sha256===report.probeArtifacts.lifecycleClassifierPostflightSha256;
    const currentBrowserLauncher=await readFile(new URL('../browser/launch.mjs',import.meta.url));
    report.probeArtifacts.browserLauncherPostflightSha256=hash(currentBrowserLauncher);
    report.probeArtifacts.browserLauncherStable=report.probeArtifacts.browserLauncher.sha256===report.probeArtifacts.browserLauncherPostflightSha256;
    const currentPlaywrightLoader=await readFile(new URL('../browser/playwright.mjs',import.meta.url));
    report.probeArtifacts.playwrightLoaderPostflightSha256=hash(currentPlaywrightLoader);
    report.probeArtifacts.playwrightLoaderStable=report.probeArtifacts.playwrightLoader.sha256===report.probeArtifacts.playwrightLoaderPostflightSha256;
    const currentPlaywrightSources=await identifyPlaywrightSources({workspaceRoot:root});
    report.probeArtifacts.playwrightCore.postflight={entry:currentPlaywrightSources.entry,coreEntry:currentPlaywrightSources.coreEntry,
      packageRoot:currentPlaywrightSources.packageRoot,packageName:currentPlaywrightSources.packageName,version:currentPlaywrightSources.version,
      entryIdentity:currentPlaywrightSources.entryIdentity,coreEntryIdentity:currentPlaywrightSources.coreEntryIdentity,packageIdentity:currentPlaywrightSources.packageIdentity,
      files:currentPlaywrightSources.files};
    report.probeArtifacts.playwrightCore.stable=playwrightSourceStartSignature===playwrightSourceSignature(currentPlaywrightSources);
    const manifestPostflightSha256=hash(await readFile(manifestPath));
    report.manifestPostflight={path:manifestPath,sha256:manifestPostflightSha256,stable:manifestPostflightSha256===manifestIdentity.sha256};
    report.status=report.browserCleanupFailure||!report.probeArtifacts.harnessStable||!report.probeArtifacts.stageRecorderStable||!report.probeArtifacts.gameplayReadinessStable||!report.probeArtifacts.fixtureServerStable||!report.probeArtifacts.fixtureResponseWatcherStable||!report.probeArtifacts.responseBodyCaptureStable||!report.probeArtifacts.focusGatewayStable||!report.probeArtifacts.lifecycleClassifierStable||!report.probeArtifacts.browserLauncherStable||!report.probeArtifacts.playwrightLoaderStable||!report.probeArtifacts.playwrightCore.stable||!report.manifestPostflight.stable?'failed':report.targets.length!==selection.expected.length?'incomplete':report.targets.every(target=>target.status==='passed')?'passed':report.targets.some(target=>target.status==='failed')?'failed':'incomplete';
    report.workspaceSnapshotChanged=!report.sourceStable;
    if(!report.sourceStable)report.workspaceSnapshotNote='Full-workspace snapshot is contextual. Selected production source, build receipts, package bytes, and private DATA are verified independently before and after each title.';
    if(!report.probeArtifacts.harnessStable)report.harnessDrift='The test harness changed while this run was executing.';
    if(!report.probeArtifacts.stageRecorderStable)report.stageRecorderDrift='The desktop startup stage recorder changed while this run was executing.';
    if(!report.probeArtifacts.gameplayReadinessStable)report.gameplayReadinessDrift='The desktop gameplay readiness classifier changed while this run was executing.';
    if(!report.probeArtifacts.fixtureServerStable)report.fixtureServerDrift='The ephemeral fixture server implementation changed while this run was executing.';
    if(!report.probeArtifacts.fixtureResponseWatcherStable)report.fixtureResponseWatcherDrift='The browser response identity watcher changed while this run was executing.';
    if(!report.probeArtifacts.responseBodyCaptureStable)report.responseBodyCaptureDrift='The dedicated Chromium response-body capture implementation changed while this run was executing.';
    if(!report.probeArtifacts.focusGatewayStable)report.focusGatewayDrift='The loopback CDP focus gateway changed while this run was executing.';
    if(!report.probeArtifacts.lifecycleClassifierStable)report.lifecycleClassifierDrift='The native browser lifecycle classifier changed while this run was executing.';
    if(!report.probeArtifacts.browserLauncherStable)report.browserLauncherDrift='The browser launch helper changed while this run was executing.';
    if(!report.probeArtifacts.playwrightLoaderStable)report.playwrightLoaderDrift='The Playwright loader changed while this run was executing.';
    if(!report.probeArtifacts.playwrightCore.stable)report.playwrightCoreDrift='The resolved Playwright core package or its browser control sources changed while this run was executing.';
    if(!report.manifestPostflight.stable)report.manifestDrift='The accepted Runtime input manifest changed while this run was executing.';
    report.passed=report.status==='passed';
    if(!report.touchModulesStable){report.status='failed';report.passed=false;}
    await save();
  }
});
if(report.status!=='passed')process.exitCode=1;
