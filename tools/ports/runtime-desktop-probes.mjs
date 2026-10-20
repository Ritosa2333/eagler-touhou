import assert from 'node:assert/strict';
import {createHash,randomUUID} from 'node:crypto';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {execFileSync} from 'node:child_process';
import {build} from '../../launcher/node_modules/esbuild/lib/main.js';
import {selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {readRuntimeObservation} from './runtime-observations.mjs';
import {createStageRecorder,requiredDesktopReadinessEvidence,requiredDesktopStartupStages,summarizeDesktopStartupEvidence} from './runtime-desktop-stage.mjs';
import {incompleteGameplayPrecondition,incompleteTh09TouchPrecondition,correlateTh09SfxWindow} from './runtime-desktop-readiness.mjs';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {createFixtureResponseProbe} from './runtime-fixture-site.mjs';
import {assessFocusRewriteForTarget,identifyPlaywrightSources,launchFocusControlledChromium} from '../browser/cdp-focus-gateway.mjs';
import {classifyAudioLifecyclePath,compareNativeBrowserTargets,desktopCheckScope,inspectAudioContextsBeforeGesture,inspectAudioGestureWindow,inspectMinimizedKeyboardClear,inspectNativeBackgroundEvents,inspectNativeFireStimulus,inspectNativePageFocus,inspectPostCleanupForeground,inspectViewportScreenshotClip,selectDesktopExecutionMode,selectNativePageTarget,summarizeDesktopCheckScope} from './runtime-desktop-lifecycle.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';

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
const audioProbeSource=`(()=>{
 if(!window.AudioContext||!window.AudioNode){window.__eaglerDesktopAudioProbe={contexts:[],taps:[],events:[{type:'unsupported'}],capture:()=>[],summary:()=>({contexts:[],tapCount:0})};return;}
 const probe={contexts:[],taps:[],events:[],capture(){
  const captures=[];
  for(const analyser of this.taps){
   const samples=new Float32Array(analyser.fftSize);analyser.getFloatTimeDomainData(samples);
   let peakAbs=0,squares=0,nonZero=0;for(const value of samples){const a=Math.abs(value);if(a>peakAbs)peakAbs=a;if(value!==0)nonZero++;squares+=value*value;}
   captures.push({tapIndex:this.taps.indexOf(analyser),contextState:analyser.context.state,sampleRate:analyser.context.sampleRate,currentTime:analyser.context.currentTime,
    samples:Array.from(samples),peakAbs,rms:Math.sqrt(squares/samples.length),nonZero});
  }
  return captures;
 },voiceSnapshot(){
  const runtime=window.__th09Runtime,module=runtime?.Module??window.Module,pool=module?.eaglerSfxOutput;
  if(!(pool?.voices instanceof Map))return{available:false,voiceCount:0,voices:[]};
  const voices=[];for(const [id,v] of pool.voices){
   const b=v?.buffer;voices.push({id:String(id),playing:v?.playing===true,node:!!v?.node,started:Number.isFinite(v?.started)?v.started:null,
    volume:Number.isFinite(v?.volume)?v.volume:null,buffer:b?{duration:Number.isFinite(b.duration)?b.duration:null,
     sampleRate:Number.isFinite(b.sampleRate)?b.sampleRate:null,length:Number.isFinite(b.length)?b.length:null,
     channels:Number.isFinite(b.numberOfChannels)?b.numberOfChannels:null}:null,
    outputTapIndex:this.taps.findIndex(tap=>tap.__eaglerDesktopSourceNode===v?.merge)});
  }
  return{available:true,voiceCount:voices.length,voices};
 },summary(){return{contexts:this.contexts.map(context=>({state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime})),tapCount:this.taps.length};}};
 const nativeConnect=AudioNode.prototype.connect,wrapped=new WeakSet();
 AudioNode.prototype.connect=function(destination,...args){
  if(destination===this.context.destination&&!wrapped.has(this)){
   wrapped.add(this);const context=this.context;if(!probe.contexts.includes(context)){probe.contexts.push(context);context.addEventListener('statechange',()=>probe.events.push({type:'state',state:context.state,time:context.currentTime}));}
   const analyser=context.createAnalyser();analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
   analyser.__eaglerDesktopSourceNode=this;
   const output=Number.isInteger(args[0])?args[0]:0;
   nativeConnect.call(this,analyser,output,0);nativeConnect.call(analyser,destination);
   probe.taps.push(analyser);probe.events.push({type:'output-tap',time:this.context.currentTime});return destination;
  }
  return nativeConnect.call(this,destination,...args);
 };
 window.__eaglerDesktopAudioProbe=probe;
})();`;

const report=await runWithReport({output,schema:'eagler-touhou/runtime-desktop-probes/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','variant'],flags:['suppress-touch-movement','background-only','input-only']});
  const suppressTouchMovement=process.argv.includes('--suppress-touch-movement');
  const backgroundOnly=process.argv.includes('--background-only');
  const inputOnly=process.argv.includes('--input-only');
  const executionMode=selectDesktopExecutionMode({backgroundOnly,inputOnly});
  const checkScope=desktopCheckScope(executionMode);
  if(suppressTouchMovement&&backgroundOnly)throw new Error('--background-only cannot be combined with --suppress-touch-movement.');
  const selectedGame=argument('game'),selectedVariant=argument('variant');
  if(!selectedGame||selectedVariant!=='normal')throw new Error('Required: --game=<explicit comma-separated games> --variant=normal');
  const manifestArgument=argument('manifest');if(!manifestArgument)throw new Error('Required: --manifest=<accepted private input manifest>');
  const manifestPath=resolve(manifestArgument);
  const config=JSON.parse(await readFile(manifestPath,'utf8'));
  const selection=selectRuntimeTargets(config,{game:selectedGame,variant:selectedVariant});
  const keyboardBundleResult=await build({
    stdin:{resolveDir:resolve(root,'launcher'),loader:'ts',contents:
      "export {KeyboardMapping,keyboardMappingSchema} from './src/launcher/keyboard-mapping.mts';export {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';"},
    bundle:true,write:false,format:'iife',globalName:'EaglerDesktopKeyboard',platform:'browser',target:'es2020'});
  const keyboardBundle=keyboardBundleResult.outputFiles[0].text;
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
    audioProbeSource:{path:'desktop-audio-probe.js',sha256:hash(audioProbeBytes)},
  };
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
    writeFile(resolve(output,probeArtifacts.audioProbeSource.path),audioProbeBytes),
  ]);
  const sourceSnapshotStart=sourceSnapshot(root);
  Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
    sourceSnapshotStart,manifest:manifestIdentity,
    probeArtifacts,
    selection:{games:selection.games,variant:selection.variant,expectedTargets:selection.expected},
    negativeControl:{enabled:suppressTouchMovement,kind:suppressTouchMovement?'suppress-touch-movement':null},
    executionMode,
    scope:backgroundOnly?'Headed full Chrome with current normal Runtime packages and private DATA. The probe performs the same Runtime launch and native gameplay preparation, then runs only package/DATA identity, real window-minimize lifecycle, same-window tab background/foreground lifecycle, background audio recovery, and postflight identity checks. It observes native target/window IDs and document focus state; Playwright focus emulation is audited and rewritten through a loopback CDP gateway. No synthetic lifecycle events are dispatched.':inputOnly?'Headed full Chrome with current normal Runtime packages and private DATA. This mode retains the full foreground gameplay input, touch, foreground SFX, pause, new-epoch and package/DATA identity checks. It omits only window-minimize, same-window tab-background and background-audio-recovery checks, which are recorded as omitted and do not count as passes.':'Headed Chromium on Windows with current normal Runtime packages and private DATA. Playwright keyboard events pass through the production KeyboardMapping and Runtime KeyboardInputBridge. A small host harness forwards host blur/document-hidden to KeyboardMapping.clear and keyboard-clear; this verifies Runtime release behavior without claiming the Launcher UI wiring. Simulated touch commands enter each Runtime package through its documented touch protocol while the native game loop runs. WebAudio sampling is attached inside each Runtime iframe to its actual output graph. No physical keyboard, touchscreen, controller, retail EXE comparison, Launcher UI, multiplayer session, or OGG track is claimed.',
    protocols:{keyboard:'eagler-touhou/keyboard-input/1',touch:'eagler-touhou/runtime-protocol top-level touch-controls fields',nativeObservation:'eagler-touhou/runtime-observation/1',audio:'Per-Runtime-realm AudioNode AnalyserNode tap immediately before AudioDestinationNode'},
    audioMode:'configure.music=none; this isolates actual sound-effect output from BGM. AUDIO-01 OGG track playback is outside this probe because the accepted input manifest supplies no music resource receipt.',
    requiredChecks:backgroundOnly?[
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
      'TOUCH-01..04 all four configured movement modes',
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
  report.requiredReadinessEvidence={...requiredDesktopReadinessEvidence};
  report.requiredCheckIds=checkScope.requiredIds;
  report.omittedChecks=checkScope.omittedIds.map(id=>({id,status:'omitted',reason:`Excluded by explicit --${executionMode} execution mode; this is not a pass.`}));
  const routeFiles=new Map();
  for(const [name,file] of Object.entries(config.shared??{}))routeFiles.set(`/shared/${name}`,resolve(file));
  routeFiles.set('/desktop-keyboard-mapping.js',keyboardBundleBytes);
  routeFiles.set('/desktop-audio-probe.js',audioProbeBytes);
  const packages=new Map(),dataIdentities=new Map();
  const hostHtml=target=>`<!doctype html><meta charset="utf-8"><title>${target.id} desktop Runtime probe</title>
  <style>html,body{margin:0;background:#202020;color:white}body{outline:none}iframe{width:960px;height:720px;border:0}</style>
  <script src="/desktop-keyboard-mapping.js"></script><script src="/desktop-audio-probe.js"></script><script>
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
  </script><iframe src="about:blank"></iframe><script>mountEpoch(1);</script>`;
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
    const file=routeFiles.get(url.pathname);if(file){const bytes=Buffer.isBuffer(file)?file:await readFile(file);reply(request,response,bytes,({'.mjs':'text/javascript','.js':'text/javascript','.css':'text/css','.json':'application/json'})[extname(url.pathname)]??'application/octet-stream');return;}
    reply(request,response,'Unknown desktop probe resource','text/plain',404);
  }catch(error){reply(request,response,String(error.message||error),'text/plain',500);}}, {protocol:'https',port:0,host:'127.0.0.1'});
  report.server={protocol:fixture.protocol,host:fixture.host,port:fixture.port,tlsIdentity:fixture.tlsIdentity};
  let browser;
  let lifecycleController;
  try{
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
    for(const target of selection.targets){
      const titleFlow=resolveTitleFlow({game:target.game,variant:target.variant});
      const record={game:target.game,variant:target.variant,id:target.id,status:'running',checks:[],stages:[],events:[],lifecycleEvents:[],errors:[],consoleErrors:[],httpErrors:[],audio:{},
        executionMode,requiredCheckIds:checkScope.requiredIds,
        requiredReadinessEvidence:{...requiredDesktopReadinessEvidence},
        omittedChecks:report.omittedChecks.map(item=>({...item}))};
      report.targets.push(record);await save();
      let context,page,frame,cdp,windowInfo,fixtureAudit,screenshotPageIdentity=null,activeEpoch=1,backgroundAudioEventStart=0,resetInputState=async()=>{};
      const stage=createStageRecorder(record,save);
      const requiredCheckIds=checkScope.requiredIds?new Set(checkScope.requiredIds):null;
      const check=async(id,ids,operation)=>{
        if(requiredCheckIds&&!requiredCheckIds.has(id)){
          if(!record.omittedChecks.some(item=>item.id===id))record.omittedChecks.push({id,adaptationIds:ids,status:'omitted',reason:`Excluded by --${executionMode}; this check is not counted as a pass.`});
          await save();return null;
        }
        const entry={id,adaptationIds:ids,status:'running'};record.checks.push(entry);await save();try{entry.observation=await operation();if(entry.observation?.checkStatus){entry.status=entry.observation.checkStatus;delete entry.observation.checkStatus;}else entry.status='passed';}
        catch(error){entry.status='failed';entry.error=String(error.stack||error);record.errors.push({check:id,error:entry.error});await save();await resetInputState().catch(()=>{});}await save();return entry;};
      const observe=()=>frame.evaluate(readRuntimeObservation,{game:target.game,variant:target.variant});
      const nativePeek=()=>frame.evaluate(game=>{const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module,core=runtime?.core??module,audioContexts={};for(const api of ['SDL','SDL2','SDL3']){try{const context=module?.[api]?.audioContext;if(context)audioContexts[api]={state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime,sharedWithHost:context===parent.__touhouAudioContext};}catch(error){audioContexts[api]={error:String(error)};}}return{canvas:document.querySelector('canvas')?{width:document.querySelector('canvas').width,height:document.querySelector('canvas').height}:null,audioContexts,audioProbe:window.__eaglerDesktopAudioProbe?.summary?.()??null,wasmRuntime:!!core};},target.game);
      const captureRuntimeScreenshot=async(label,path)=>{
        const evidence={label,path,status:'running',startedAt:new Date().toISOString()};
        try{
          if(!page||!cdp||!frame)throw Error('The page, native CDP session, or Runtime frame is unavailable.');
          const before=await readPageTargetWindow(cdp,page);evidence.pageBeforeRestore=before;
          if(before.status!=='matched'||before.target?.type!=='page'||!Number.isSafeInteger(before.window?.windowId))
            throw Error('The game page target and native window could not be identified before screenshot capture.');
          await cdp.send('Browser.setWindowBounds',{windowId:before.window.windowId,bounds:{windowState:'normal'}});
          await page.bringToFront();
          evidence.hostFocus=await waitForNativeFocus(page,5000);
          evidence.runtimeFocus=await waitForNativeFocus(frame,5000);
          const after=await readPageTargetWindow(cdp,page);evidence.pageAfterRestore=after;
          const samePage=after.status==='matched'&&after.target?.type==='page'&&after.target.targetId===before.target.targetId;
          const sameWindow=after.status==='matched'&&after.window?.windowId===before.window.windowId;
          evidence.identity={samePage,sameWindow,targetId:after.target?.targetId??null,windowId:after.window?.windowId??null};
          if(!samePage||!sameWindow)throw Error('Page target or native window identity changed during screenshot preparation.');
          if(!evidence.hostFocus.ready||!evidence.runtimeFocus.ready)throw Error('Host and Runtime documents did not both report native visible/focused state.');
          if(screenshotPageIdentity&&(screenshotPageIdentity.targetId!==after.target.targetId||screenshotPageIdentity.windowId!==after.window.windowId))
            throw Error('Screenshot page target/window does not match the previously captured game page.');
          screenshotPageIdentity??={targetId:after.target.targetId,windowId:after.window.windowId};
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
      const gameplayReadiness=state=>titleFlow.gameplayReadiness(state,'desktop');
      const gameplayReady=state=>gameplayReadiness(state).ready;
      const waitForGameplayReadiness=async(epoch,label,timeoutMs=5000)=>{
        const start=Date.now();let state,samples=0;
        do{state=await observe();samples++;if(gameplayReady(state))break;await page.waitForTimeout(75);}while(Date.now()-start<timeoutMs);
        const readiness=gameplayReadiness(state);
        record.gameplayReadinessByEpoch??=[];
        record.gameplayReadinessByEpoch.push({epoch,label,ready:readiness.ready,reasons:readiness.reasons,
          samples,durationMs:Date.now()-start,state});
        await save();
        return state;
      };
      const enterNativeGameplay=async(epoch,label=`native gameplay eligibility epoch ${epoch}`)=>{
        await titleFlow.enterGameplay({observe,press:async(code,holdMs=90)=>key(code,holdMs),
          waitUntil:(waitLabel,predicate,timeoutMs=30000)=>waitObservation(waitLabel,predicate,timeoutMs),
          lane:'desktop',epoch,label,timeoutMs:30000});
        let state=await waitForGameplayReadiness(epoch,label);
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
      const ensureFrame=()=>{const candidate=page.frames().find(item=>item.url().includes(`/runtime/${target.id}/`));if(!candidate)throw Error(`${target.id}: Runtime iframe is missing`);frame=candidate;return frame;};
      const flushFixtureAudit=async()=>{
        if(!fixtureAudit)return;
        try{await fixtureAudit.flush();}
        finally{record.fixtureResponses=fixtureAudit.responses;record.fixtureResponseErrors=fixtureAudit.errors;await save();}
      };
      const cleanupKeys=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','w','a','s','d','KeyZ','KeyX','ShiftLeft','ControlLeft','Escape','Enter'];
      resetInputState=async(epoch=activeEpoch)=>{
        if(!page)return;
        for(const code of cleanupKeys)await keyUp(code).catch(()=>{});
        await page.evaluate(async({game,epoch})=>{
          const runtimeFrame=document.querySelector('iframe')?.contentWindow,bridge=runtimeFrame?.__eaglerKeyboardInputBridge;
          window.__desktopKeyboardOwner?.clear();
          const keyboardCleared=bridge?.submit({protocol:'eagler-touhou/1',game,epoch,command:'keyboard-clear'})??false;
          const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100};
          try{send('keyboard-clear');send('touch-cancel');send('touch-controls',neutral);}catch{}
          const acknowledgements=await Promise.all([
            rpc('touch-cancel',{},750).then(()=>true).catch(()=>false),
            rpc('touch-controls',neutral,750).then(()=>true).catch(()=>false),
          ]);
          return{keyboardCleared,liveRuntime:!!bridge,touchCancelAcknowledged:acknowledgements[0],touchControlsAcknowledged:acknowledgements[1]};
        },{game:target.game,epoch}).catch(()=>{});
      };
      try{
        await check('package-and-private-data-identity',['BUILD-04','PACKAGE-02'],async()=>{const packageIdentity=await verifyRuntimePackage(target,{repoRoot:root});const dataIdentity=await verifyPrivateData(target.data);packages.set(target.id,packageIdentity);dataIdentities.set(target.id,dataIdentity);if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256);record.packageIdentity=packageIdentity;record.dataIdentity=dataIdentity;return{packageSha256:packageIdentity.packageSha256,sourceBuild:packageIdentity.sourceBuild,privateData:dataIdentity};});
        if(record.checks.at(-1).status!=='passed'){record.status='failed';record.finishedAt=new Date().toISOString();await save();continue;}
        cdp=await browser.newBrowserCDPSession();
        context=await browser.newContext({viewport:{width:1100,height:820}});
        await context.addInitScript({content:`if(window!==window.top){${audioProbeSource}window.__desktopRawKeyboardEvents=[];for(const name of ['keydown','keyup'])window.addEventListener(name,event=>window.__desktopRawKeyboardEvents.push({type:name,code:event.code,key:event.key,keyCode:event.keyCode,isTrusted:event.isTrusted,target:event.target?.tagName??null,time:performance.now()}),true);const type=()=>({desktopProbe:true,game:new URL(location.href).pathname.split('/')[2],epoch:Number(new URLSearchParams(location.search).get('runtimeEpoch'))});for(const name of ['blur','focus','visibilitychange','pagehide','pageshow']){const target=name==='visibilitychange'?document:window;target.addEventListener(name,()=>{try{parent.postMessage({...type(),type:name,visibility:document.visibilityState,hidden:document.hidden,time:performance.now()},location.origin);}catch{}},true);}}`});
        page=await context.newPage();page.on('pageerror',error=>record.errors.push({page:String(error)}));page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
        fixtureAudit=await createFixtureResponseProbe(page,{base:fixture.base,expectedResponses,output,label:target.id});
        record.fixtureResponseCapture=fixtureAudit.capture;
        record.fixtureResponses=fixtureAudit.responses;record.fixtureResponseErrors=fixtureAudit.errors;
        const hostUrl=`${fixture.base}/host/${target.id}`;
        await stage('host-navigation',{url:hostUrl,epoch:1},()=>page.goto(hostUrl));await flushFixtureAudit();
        await page.waitForFunction(()=>window.__eaglerDesktopAudioProbe&&window.__desktopKeyboardPreferences);await page.evaluate(()=>{document.body.tabIndex=-1;});
        const ready=await stage('ready',{epoch:1,event:'ready'},async()=>{const event=await waitRuntimeEvent('ready',1,'desktop-ready');await flushFixtureAudit();return event;});ensureFrame();
        assert.equal(ready.protocol,'eagler-touhou/1');assert.equal(ready.game,target.game);assert.equal(ready.epoch,1);
        await stage('keyboard-bridge',{epoch:1},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
        await page.evaluate(({game,epoch})=>window.__desktopInstallKeyboardMapping({game,epoch,layout:'arrows'}),{game:target.game,epoch:1});
        const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
        const configureEpoch=async(mode)=>page.evaluate(({sharedResources,options})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:true,touchMovementMode:'touch',thpracEnabled:false,limitPresentationTo60:false,...options}}),{sharedResources,options:{...(target.options??{}),touchEnabled:true,touchMovementMode:mode}});
        const launchEpoch=async(epoch,mode)=>{
          activeEpoch=epoch;
          await page.evaluate(({game,epoch})=>window.__desktopInstallKeyboardMapping({game,epoch,layout:'arrows'}),{game:target.game,epoch});
          await stage('configure',{epoch,mode},()=>configureEpoch(mode));
          await stage('launch',{epoch},()=>page.evaluate(()=>rpc('launch')));
          await stage('first-frame',{epoch,event:'first-frame'},async()=>{const event=await waitRuntimeEvent('first-frame',epoch,`desktop-first-frame-${epoch}`);await flushFixtureAudit();return event;});
          ensureFrame();await page.evaluate(()=>document.body.focus());
          return enterNativeGameplay(epoch);
        };
        await stage('configure',{epoch:1,mode:'touch'},()=>configureEpoch('touch'));
        await stage('launch',{epoch:1},()=>page.evaluate(()=>rpc('launch')));
        await stage('first-frame',{epoch:1,event:'first-frame'},async()=>{const event=await waitRuntimeEvent('first-frame',1,'desktop-first-frame');await flushFixtureAudit();return event;});
        await page.evaluate(()=>document.body.focus());
        const canvas=await nativePeek();assert.ok(canvas.canvas?.width>0&&canvas.canvas?.height>0);record.initial={ready,canvas,audioContexts:canvas.audioContexts,audioProbe:canvas.audioProbe};
        await captureRuntimeScreenshot('title',resolve(output,`${target.id}-title.png`));
        const started=await enterNativeGameplay(activeEpoch,'native gameplay eligibility epoch 1');
        record.gameplayStart=started;
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
        const nativeCheck=async(id,ids,operation)=>check(id,ids,async()=>{
          const eligibility=incompleteGameplayPrecondition(await observe(),target.game);
          if(eligibility)return{...eligibility,stage:`before-${id}`};
          return operation();
        });
        await nativeCheck('native-four-direction-arrows',['INPUT-01'],()=>directionCase('arrows',[
          ['ArrowUp','ArrowUp','y',-1],['ArrowDown','ArrowDown','y',1],['ArrowLeft','ArrowLeft','x',-1],['ArrowRight','ArrowRight','x',1]]));
        for(const route of ['iframe-runtime-listener','host-runtime-bridge'])await check(`negative-control-suppressed-${route}-keyboard-mapping`,['INPUT-01'],async()=>{
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
        await nativeCheck('native-key-clear-while-held',['INPUT-06'],async()=>{
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
            const sample=await frame.evaluate(game=>{const probe=window.__eaglerDesktopAudioProbe;return{
              captures:probe?.capture?.()??[],voices:game==='th09'?probe?.voiceSnapshot?.()??null:null};},target.game);
            const elapsedMs=Date.now()-start,localTaps=new Map();
            for(const capture of sample.captures){captures.push(capture);localTaps.set(capture.tapIndex,capture);}
            if(sample.voices)voiceSamples.push({elapsedMs,voices:sample.voices,localTaps});
            await page.waitForTimeout(25);
          }while(Date.now()<end);
          const captureWindowEndedAtMs=Date.now();
          const frames=captures.map(item=>item.samples),nonZeroSamples=captures.reduce((sum,item)=>sum+item.nonZero,0),peakAbs=captures.reduce((peak,item)=>Math.max(peak,item.peakAbs),0),rmsMax=captures.reduce((peak,item)=>Math.max(peak,item.rms),0);
          const pcmByTap=new Map();for(const capture of captures){const total=pcmByTap.get(capture.tapIndex)??{tapIndex:capture.tapIndex,captures:0,nonZeroSamples:0,peakAbs:0,rmsMax:0};
            total.captures++;total.nonZeroSamples+=capture.nonZero;total.peakAbs=Math.max(total.peakAbs,capture.peakAbs);total.rmsMax=Math.max(total.rmsMax,capture.rms);pcmByTap.set(capture.tapIndex,total);}
          let voiceDiagnostics=null;
          if(target.game==='th09'&&voiceSamples.length){
            const activations=[],activeObservations=[],previous=new Map(),observedTapIndexes=new Set();
            for(const sample of voiceSamples){
              for(const voice of sample.voices.voices){
                const key=voice.id,active=voice.playing||voice.node,prior=previous.get(key),tapPcm=sample.localTaps.get(voice.outputTapIndex);
                if(active&&(!prior?.active||prior.started!==voice.started))activations.push({elapsedMs:sample.elapsedMs,...voice,
                  pcm:tapPcm?{nonZero:tapPcm.nonZero,peakAbs:tapPcm.peakAbs,rms:tapPcm.rms}:null});
                if(active){activeObservations.push({elapsedMs:sample.elapsedMs,...voice,
                  pcm:tapPcm?{nonZero:tapPcm.nonZero,peakAbs:tapPcm.peakAbs,rms:tapPcm.rms}:null});
                  if(Number.isInteger(voice.outputTapIndex)&&voice.outputTapIndex>=0)observedTapIndexes.add(voice.outputTapIndex);}
                previous.set(key,{active,started:voice.started});
              }
            }
            const first=voiceSamples[0].voices,last=voiceSamples.at(-1).voices;
            voiceDiagnostics={available:first.available&&last.available,voiceCountStart:first.voiceCount,voiceCountEnd:last.voiceCount,
              snapshots:{start:first.voices,end:last.voices},activationEvents:activations,activeObservations,
              associatedOutputPcm:[...observedTapIndexes].sort((a,b)=>a-b).map(index=>pcmByTap.get(index)??{tapIndex:index,captures:0,nonZeroSamples:0,peakAbs:0,rmsMax:0})};
          }
          const samplePath=resolve(output,`${target.id}-${label}-webaudio.f32`);const flat=frames.flat(),buffer=Buffer.alloc(flat.length*4);flat.forEach((value,index)=>buffer.writeFloatLE(value,index*4));await writeFile(samplePath,buffer);
          const audioState=await nativePeek();
          return{durationMs,captureStartedAtMs:start,captureWindowEndedAtMs,captureFinishedAtMs:Date.now(),captureCount:captures.length,contextState:captures.at(-1)?.contextState??Object.values(audioState.audioContexts??{})[0]?.state??null,
            contextStates:captures.map(item=>item.contextState),sampleRate:captures.find(item=>item.sampleRate)?.sampleRate??null,
            tapCount:audioState.audioProbe?.tapCount??0,contexts:audioState.audioProbe?.contexts??[],nonZeroSamples,peakAbs,rmsMax,
            ...(voiceDiagnostics?{nativeSfx:voiceDiagnostics}:{}),samplePath,bytes:buffer.length,sha256:hash(buffer)};
        };
        await check('audio-output-with-music-disabled',['AUDIO-02'],async()=>{
          if(target.game!=='th09'){
            const eligibility=incompleteGameplayPrecondition(await observe(),target.game);
            if(eligibility)return{...eligibility,reason:'Native gameplay eligibility was not established for the audio sample.'};
          }
          const idle=await sampleAudio('idle',400);let firing,preKey,postKey;
          preKey=target.game==='th09'?await observe():null;
          if(target.game==='th09'){
            const eligibility=incompleteTh09TouchPrecondition(preKey);
            if(eligibility){record.audio={idle,audioContexts:(await nativePeek()).audioContexts,mode:'none',nativeInputWindow:{before:preKey,after:null}};
              return{checkStatus:'incomplete',...record.audio,reason:'TH09 native touch context or player readiness was not established before the firing sample.',eligibility:eligibility.eligibility};}
          }
          try{await keyDown('KeyZ');await page.waitForTimeout(20);firing=await sampleAudio('firing',1000);}
          finally{await keyUp('KeyZ');}
          postKey=target.game==='th09'?await observe():null;
          record.audio={idle,firing,audioContexts:(await nativePeek()).audioContexts,mode:'none',...(preKey?{nativeInputWindow:{before:preKey,after:postKey}}:{})};
          if(target.game==='th09'){
            const beforeEligibility=incompleteTh09TouchPrecondition(preKey),afterEligibility=incompleteTh09TouchPrecondition(postKey);
            if(beforeEligibility||afterEligibility)return{checkStatus:'incomplete',...record.audio,
              reason:'TH09 native touch context or player readiness was not established for the firing sample.',eligibility:{before:beforeEligibility?.eligibility??{ready:true,reasons:[]},after:afterEligibility?.eligibility??{ready:true,reasons:[]}}};
            const sfxCorrelation=correlateTh09SfxWindow(idle,firing);
            const nativeSfx=firing.nativeSfx,mappedActive=nativeSfx?.activeObservations?.filter(item=>item.outputTapIndex>=0)??[];
            if(mappedActive.length&&!nativeSfx.associatedOutputPcm?.some(item=>item.nonZeroSamples>0))
              assert.fail('TH09 native SFX voice playback reached a mapped WebAudio output tap, but captured PCM stayed zero');
            if(!sfxCorrelation.correlated)return{checkStatus:'incomplete',...record.audio,nativeSfxCorrelation:sfxCorrelation,
              reason:'TH09 gameplay eligibility was valid, but no new native SFX voice activation with non-zero PCM on its own output tap was correlated with the KeyZ firing window.'};
            assert.equal(firing.contextState,'running','The Runtime WebAudio context must be running');
            return{...record.audio,nativeSfxCorrelation:sfxCorrelation};
          }
          assert.equal(firing.contextState,'running','The Runtime WebAudio context must be running');assert.ok(firing.tapCount>0,'No real WebAudio destination path was observed');assert.ok(firing.nonZeroSamples>0&&firing.peakAbs>0,'No non-zero WebAudio PCM was captured while firing with BGM disabled');
          return record.audio;
        });
        const touchControls=async(fields={})=>{
          const controls={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100,...fields};
          if(suppressTouchMovement&&(controls.joystickX||controls.joystickY)){
            (record.suppressedTouchMovement??=[]).push({epoch:activeEpoch,command:'touch-controls',requested:{...controls}});
            controls.joystickX=controls.joystickY=0;
          }
          return page.evaluate(value=>rpc('touch-controls',value),controls);
        };
        const cancelTouch=()=>page.evaluate(()=>rpc('touch-cancel'));
        const directTouch=async(type,id,x,y)=>{
          if(suppressTouchMovement){(record.suppressedTouchMovement??=[]).push({epoch:activeEpoch,command:'direct-touch',type,id,x,y});return;}
          const point=await frame.locator('canvas').evaluate((canvas,{x,y})=>{const rect=canvas.getBoundingClientRect();return{x:(rect.left+x*rect.width)/innerWidth,y:(rect.top+y*rect.height)/innerHeight};},{x,y});
          return page.evaluate(value=>send('direct-touch',value),{type,id,...point});
        };
        const retireAndLaunch=async(epoch,mode)=>{
          const priorEpoch=activeEpoch;
          assert.ok(Number.isSafeInteger(epoch)&&epoch>priorEpoch,`Next Runtime epoch ${epoch} must be greater than active epoch ${priorEpoch}`);
          await resetInputState(priorEpoch);
          const prior=frame;await prior.goto('about:blank');await page.waitForTimeout(100);
          const pagehide=await page.evaluate(oldEpoch=>lifecycleEvents.some(event=>event.type==='pagehide'&&event.epoch===oldEpoch),priorEpoch);
          await stage('runtime-navigation',{priorEpoch,epoch},()=>page.evaluate(next=>mountEpoch(next),epoch));activeEpoch=epoch;
          await stage('ready',{epoch,event:'ready'},()=>waitRuntimeEvent('ready',epoch,`desktop-ready-${epoch}`));ensureFrame();
          await stage('keyboard-bridge',{epoch},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
          const state=await launchEpoch(epoch,mode);started=state;return{priorEpoch,epoch,pagehide,state};
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
        await check('simulated-touch-mode-touch',['TOUCH-01','TOUCH-05','TOUCH-06','TOUCH-15','TOUCH-16'],async()=>{
          const transition=await retireAndLaunch(2,'touch');
          if(!transition.pagehide)return{checkStatus:'incomplete',...transition,reason:'The prior Runtime did not emit pagehide before the fresh touch session.'};
          const evidence=await touchModeProbe('touch',2);return{...transition,...evidence};
        });
        await check('native-four-direction-wasd',['INPUT-02'],async()=>{
          const transition=await retireAndLaunch(3,'touch');
          if(!transition.pagehide)return{checkStatus:'incomplete',...transition,reason:'The prior Runtime epoch did not report pagehide before the fresh WASD session.'};
          const matrix=await directionCase('wasd',[
            ['w','ArrowUp','y',-1],['s','ArrowDown','y',1],['a','ArrowLeft','x',-1],['d','ArrowRight','x',1]]);
          return{...transition,...matrix};
        });
        for(const [epoch,mode] of [[4,'touch-unlimited'],[5,'joystick'],[6,'joystick-free']]){
          await check(`runtime-epoch-touch-mode-${mode}`,['TOUCH-02','TOUCH-03','TOUCH-04','RUNTIME-01'],async()=>{
            const transition=await retireAndLaunch(epoch,mode);assert.ok(transition.pagehide,`Prior Runtime epoch ${transition.priorEpoch} did not report pagehide`);
            const evidence=await touchModeProbe(mode,epoch);return{...transition,...evidence};
          });
        }
        if(target.game==='th09'){
          await check('touch-fire-focus-and-mixed-input',['TOUCH-05','TOUCH-06','TOUCH-15','INPUT-05'],async()=>{
            const precondition=await observe(),eligibility=incompleteTh09TouchPrecondition(precondition);
            if(eligibility)return{...eligibility,stage:'before-native-touch-fire'};
            const idle=await sampleAudio('touch-fire-idle',350);let touchFire;
            try{await touchControls({fireEnabled:true});touchFire=await sampleAudio('touch-fire-enabled',1000);}
            finally{await touchControls().catch(()=>{});await cancelTouch().catch(()=>{});}
            const postcondition=await observe(),postEligibility=incompleteTh09TouchPrecondition(postcondition);
            record.th09TouchFire={precondition,idle,touchFire,postcondition};
            if(postEligibility)return{checkStatus:'incomplete',...record.th09TouchFire,
              reason:'Native TH09 touch context or player readiness ended during the fire-enabled sample.',eligibility:postEligibility.eligibility};
            const sfxCorrelation=correlateTh09SfxWindow(idle,touchFire);
            const nativeSfx=touchFire.nativeSfx,mappedActive=nativeSfx?.activeObservations?.filter(item=>item.outputTapIndex>=0)??[];
            const hasNativePcm=Boolean(nativeSfx?.associatedOutputPcm?.some(item=>item.nonZeroSamples>0));
            if(mappedActive.length&&!hasNativePcm)
              assert.fail('TH09 native SFX voice playback reached a mapped WebAudio output tap, but captured PCM stayed zero');
            if(!sfxCorrelation.correlated)return{checkStatus:'incomplete',...record.th09TouchFire,nativeSfxCorrelation:sfxCorrelation,
              reason:'Touch fire ran in an eligible native gameplay context, but no new native SFX voice activation with non-zero PCM on its own output tap was observed.'};
            return{...record.th09TouchFire,nativeSfxCorrelation:sfxCorrelation};
          });
        }else{
          await nativeCheck('touch-fire-focus-and-mixed-input',['TOUCH-05','TOUCH-06','TOUCH-15','INPUT-05'],async()=>{
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
        await check('native-pause-state-and-freeze',['RUNTIME-04'],async()=>{
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
        await check('window-minimize-foreground-and-release',['RUNTIME-04','INPUT-06'],async()=>{
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
          hiddenInput=await waitObservation('Native keyboard input is neutral while the Chrome window is still minimized',state=>state.input.value===before.input.value,1400).catch(()=>null);
          boundsAtHiddenSample=await cdp.send('Browser.getWindowBounds',{windowId:windowInfo.windowId});
          const nativeState=await nativeFocusState(page),hiddenSampledAtMs=await page.evaluate(()=>performance.now());
          eventsWhileMinimized=await page.evaluate(index=>lifecycleEvents.slice(index),lifecycleStart);
          minimizedKeyboardClear=inspectMinimizedKeyboardClear({windowState:boundsAtHiddenSample.bounds?.windowState,nativeState,
            events:eventsWhileMinimized.filter(item=>item.epoch===activeEpoch),hostEvents:hostEventsWhileMinimized,
            hiddenStartAtMs,hiddenSampledAtMs,inputValue:hiddenInput?.input?.value,neutralInputValue:before.input.value});
          minimized={nativeState,windowState:boundsAtHiddenSample.bounds?.windowState,bounds:boundsAtHiddenSample.bounds,boundsInitially:boundsAfterMinimize.bounds,hiddenStartAtMs,hiddenSampledAtMs,
            hiddenInput,hostKeyboardLifecycle:hostEventsWhileMinimized,lifecycleEvents:eventsWhileMinimized,minimizedKeyboardClear,
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
        await check('background-tab-foreground-release',['RUNTIME-04','INPUT-06'],async()=>{
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
        await check('background-audio-recovery',['AUDIO-04'],async()=>{
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
        await check('iframe-exit-and-new-epoch',['RUNTIME-01','RUNTIME-04'],async()=>{
          const priorEpoch=activeEpoch,nextEpoch=priorEpoch+1;
          const prior=frame;await cancelTouch().catch(()=>{});await touchControls().catch(()=>{});await prior.goto('about:blank');await page.waitForTimeout(100);
          const pagehide=await page.evaluate(oldEpoch=>lifecycleEvents.some(event=>event.type==='pagehide'&&event.epoch===oldEpoch),priorEpoch);assert.ok(pagehide,'Runtime iframe teardown must emit pagehide observation');
          await stage('runtime-navigation',{epoch:nextEpoch},()=>page.evaluate(epoch=>mountEpoch(epoch),nextEpoch));
          await stage('ready',{epoch:nextEpoch,event:'ready'},()=>waitRuntimeEvent('ready',nextEpoch,`desktop-ready-${nextEpoch}`));ensureFrame();
          await stage('keyboard-bridge',{epoch:nextEpoch},()=>frame.waitForFunction(()=>Boolean(window.__eaglerKeyboardInputBridge),undefined,{timeout:30000}));
          const newState=await launchEpoch(nextEpoch,'touch');
          const staleRequest=`stale-${randomUUID()}`;await page.evaluate(({request,epoch})=>send('sync',{epoch,request}),{request:staleRequest,epoch:priorEpoch});await page.waitForTimeout(350);
          assert.equal(await page.evaluate(request=>events.some(event=>event.request===request),staleRequest),false,'An old epoch request must not be accepted after Runtime replacement');
          const newAudio=await nativePeek();
          return{oldEpoch:priorEpoch,newEpoch:nextEpoch,pagehideObserved:pagehide,staleRequestIgnored:true,newAudio,newState,audioContextSharingIsNotAssumed:true};
        });
        record.events=await page.evaluate(()=>events);record.lifecycleEvents=await page.evaluate(()=>lifecycleEvents);record.hostKeyboardLifecycle=await page.evaluate(()=>hostKeyboardLifecycle);record.keyboardForwardEvents=await page.evaluate(()=>keyboardForwardEvents);record.keyboardMapEvents=await page.evaluate(()=>keyboardMapEvents);
        await flushFixtureAudit();
        const sameOriginErrors=record.httpErrors.filter(event=>event.url.startsWith(fixture.base));
        assert.deepEqual(sameOriginErrors,[],'Local Runtime resources returned HTTP errors');
        assert.deepEqual(record.errors.filter(item=>item.page),[],'Browser page errors occurred');
      }catch(error){record.status='failed';record.failure=String(error.stack||error);record.failedAt=new Date().toISOString();await save();}
      finally{
        await resetInputState(activeEpoch);
        if(fixtureAudit){await flushFixtureAudit().catch(error=>{record.fixtureAuditFailure=String(error.stack||error);});await fixtureAudit.stop();}
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
          await check('package-and-private-data-identity-postflight',['BUILD-04','PACKAGE-02'],async()=>{
            const packageIdentity=await verifyRuntimePackage(target,{repoRoot:root}),dataIdentity=await verifyPrivateData(target.data);
            assert.deepEqual(packageIdentity,record.packageIdentity,'Runtime source, build plan, receipts or package changed during desktop probes');
            assert.deepEqual(dataIdentity,record.dataIdentity,'Private DATA changed during desktop probes');
            record.packageIdentityPostflight=packageIdentity;record.dataIdentityPostflight=dataIdentity;
            return{packageSha256:packageIdentity.packageSha256,sourceBuild:packageIdentity.sourceBuild,privateData:dataIdentity};
          });
        }
        const scopeSummary=checkScope.requiredIds?summarizeDesktopCheckScope(record.checks,checkScope):null;
        const startupEvidenceSummary=summarizeDesktopStartupEvidence({stages:record.stages,gameplayReadinessByEpoch:record.gameplayReadinessByEpoch});
        record.startupEvidenceSummary=startupEvidenceSummary;
        if(scopeSummary){record.requiredCheckSummary=scopeSummary;record.missingRequiredChecks=scopeSummary.missingRequired;}
        if(record.focusEmulationGateway?.errors?.length){record.failure??='The audited CDP focus gateway recorded protocol or transport errors.';record.gatewayErrors=record.focusEmulationGateway.errors;}
        record.status=record.failure||record.checks.some(item=>item.status==='failed')||scopeSummary?.status==='failed'||startupEvidenceSummary.status==='failed'?'failed':
          scopeSummary?.status==='incomplete'||startupEvidenceSummary.status!=='passed'?'incomplete':record.checks.every(item=>item.status==='passed')?'passed':'incomplete';
        if(page&&frame&&cdp)await captureRuntimeScreenshot('last',resolve(output,`${target.id}-last.png`));
        if(cdp){try{if(windowInfo)await cdp.send('Browser.setWindowBounds',{windowId:windowInfo.windowId,bounds:{windowState:'normal'}});}catch{}await cdp.detach().catch(()=>{});}
        await context?.close().catch(()=>{});record.finishedAt=new Date().toISOString();await save();
        console.log(`${target.id}: ${record.status.toUpperCase()} ${record.failure?.split('\n')[0]??record.checks.filter(item=>item.status!=='passed').map(item=>item.id+':'+item.status).join(', ')}`);
      }
    }
  }finally{
    try{
      if(lifecycleController){
        report.browserCleanup=await lifecycleController.close();
        report.focusEmulationGateway=report.browserCleanup.gateway;
      }else await browser?.close();
    }catch(error){
      report.browserCleanupFailure=String(error.stack||error);
      report.focusEmulationGateway=error.gatewaySnapshot??lifecycleController?.gatewaySnapshot?.()??null;
      for(const target of report.targets)if(target.status==='passed'){
        target.status='failed';target.failure='Browser CDP gateway reported an error during shutdown.';
      }
    }
    await fixture?.close();
    report.finishedAt=new Date().toISOString();report.sourceSnapshotEnd=sourceSnapshot(root);
    report.sourceStable=report.sourceSnapshotStart.sha256===report.sourceSnapshotEnd.sha256;
    const currentHarness=await readFile(new URL(import.meta.url));
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
    await save();
  }
});
if(report.status!=='passed')process.exitCode=1;
