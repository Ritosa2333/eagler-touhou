import assert from 'node:assert/strict';
import {createHash,randomBytes} from 'node:crypto';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve,dirname} from 'node:path';
import {selectRuntimeTargets,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {practiceAdaptationChecks,practiceAdaptationIds,practiceBuildFeature,runPracticeGameplayStage} from './practice-observations.mjs';
import {createStageRecorder} from './runtime-desktop-stage.mjs';
import {auditBrowserFinalErrors} from '../browser/final-error-audit.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {createPracticeReplayObservationInput,practiceSnapshotRequest,readPracticeMenuRaw,readPracticeSnapshotRaw,readPracticeProgramRaw} from '../../tests/adaptation/adapters/practice.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {assertPracticeCapabilityEvidence} from '../../tests/adaptation/adapters/practice-capability-evidence.mjs';
import {installPracticeExitObserver,releasePracticeExitObserver} from './practice-exit-observer.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';
import {runPracticeTrainerBrowser} from './practice-trainer-browser.mjs';
import {assertPracticeTrainerMatrix} from './practice-trainer-scene.mjs';
import {runPracticeOptionsBrowser,assertPracticeInputOptions} from './practice-options-browser.mjs';
import {runPracticeBossBrowser,assertPracticeBossEffects} from './practice-boss-scene.mjs';
import {runPracticeDisplay,assertPracticeDisplay} from './practice-display-options.mjs';
import {runPracticeMathOptions,assertPracticeMathOptions} from './practice-math-options.mjs';
import {runPracticeKeyMonitorBrowser,assertPracticeKeyMonitor} from './practice-key-monitor.mjs';
import {preparePracticeDeathTitle,runPracticeLifecycleOption,assertPracticeLifecycleOption} from './practice-lifecycle-options.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed} from '../../tests/adaptation/adapters/scene.mjs';

const option=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
if(!option('output'))throw Error('Required: --output=directory');
const output=resolve(option('output')),repoRoot=resolve(import.meta.dirname,'../..');
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');

async function clickCanvasLogical(frame,page,x,y){
  const box=await frame.locator('canvas').first().boundingBox();
  if(!box||box.width<=0||box.height<=0)throw new Error('Native practice pointer target canvas is unavailable');
  const scale=Math.min(box.width/640,box.height/480);
  const point={x:box.x+(box.width-640*scale)/2+x*scale,y:box.y+(box.height-480*scale)/2+y*scale};
  await page.mouse.click(point.x,point.y);
  return{logical:{x,y},page:point,canvas:{x:box.x,y:box.y,width:box.width,height:box.height},scale};
}

function practiceProtocolInitScript({game,epoch}){
  if(window!==window.top)return;
  const pending=new Set();
  Object.defineProperty(window,'__flushPracticeProtocolCaptures',{configurable:true,value:async()=>{
    while(pending.size)await Promise.all([...pending]);
    if(window.__practiceProtocolCaptureError)throw new Error(window.__practiceProtocolCaptureError);
  }});
  addEventListener('message',event=>{
    const frame=document.querySelector('iframe'),data=event.data;
    if(event.origin!==location.origin||event.source!==frame?.contentWindow||data?.protocol!=='eagler-touhou/1'||
      data.game!==game||data.epoch!==epoch)return;
    if(data.event!=='error'&&data.event!=='abort'&&data.ok!==false)return;
    const operation=Promise.resolve(window.__capturePracticeProtocolFailure({...data,origin:event.origin}))
      .catch(error=>{window.__practiceProtocolCaptureError=String(error?.stack||error);});
    pending.add(operation);void operation.finally(()=>pending.delete(operation));
  });
}

async function bindPracticeProtocolCapture(page,game,epoch,record){
  await page.exposeBinding('__capturePracticeProtocolFailure',(source,event)=>{
    const samePage=source.page===page,mainFrame=source.frame===page.mainFrame();
    if(!samePage||!mainFrame){record.protocolErrors.push({type:'untrusted-binding-source',samePage,mainFrame,event});return;}
    record.protocolErrors.push(event);
  });
  await page.addInitScript(practiceProtocolInitScript,{game,epoch});
}

async function drainPracticeProtocolCapture(page){
  if(!page||page.isClosed())return;
  await page.evaluate(async()=>{
    if(typeof window.__flushPracticeProtocolCaptures==='function')await window.__flushPracticeProtocolCaptures();
  });
}

function replayBytes(value,label){
  assert.ok(value&&Array.isArray(value.bytes),`${label}: Shell did not return the Runtime file bytes`);
  assert.ok(value.bytes.every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255),`${label}: Shell returned invalid bytes`);
  return Uint8Array.from(value.bytes);
}

/** Shared save, persistence, fresh-epoch import and assessment sequence. Titles own only codecs and native menu navigation. */
async function runPracticeReplayRoundTrip({browser,context,sourcePage,sourceResponseProbe,target,titleAdapter,practice,
  sourceReadPractice,sourceWaitPracticeUntil,sourceWaitForPracticeInput,sourcePress,sourcePractice,caseRecord,record,stage,site,config,output,save}={}){
  const capability=practice.replayRoundTrip;
  if(capability?.supported!==true)throw new Error('Title has no supported practice Replay roundtrip adapter');
  const sourceEpoch=caseRecord.epoch,freshEpoch=Math.max(...record.locales.map(item=>item.epoch))+1;
  let page=null,responseProbe=null,observerArmed=false,sourceStopped=false;
  try{
    const before=await stage('practice-replay-recording-boundary',{epoch:sourceEpoch},async()=>{
      const snapshot=await sourceReadPractice();
      const frameBefore=capability.recordingFrame(snapshot);
      if(!Number.isSafeInteger(frameBefore))throw new Error('Title Replay adapter did not expose a native recording frame');
      await sourceWaitPracticeUntil('practice Replay recording tick',next=>{
        if(practice.lifecycle&&(!next.state.inGame||!next.state.active||next.state.replay))throw Error('Practice lifecycle recording left the declared live Practice phase');
        const frame=capability.recordingFrame(next);return Number.isSafeInteger(frame)&&frame>frameBefore&&frame>=(practice.lifecycle?.minimumRecordingFrames??0);
      },15000);
      const after=await sourceReadPractice(),frameAfter=capability.recordingFrame(after);
      if(!Number.isSafeInteger(frameAfter)||frameAfter<=frameBefore)throw new Error('Native gameplay did not advance after the Replay recording boundary');
      return{frameBefore,frameAfter};
    });
    const armed=await stage('practice-replay-idbfs-observer-arm',{epoch:sourceEpoch},()=>
      sourcePage.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`))?.evaluate(capability.sync.arm));
    if(armed?.available!==true||armed.armed!==true)throw new Error(`Title IDBFS observer unavailable: ${armed?.reason??'invalid arm receipt'}`);
    observerArmed=true;
    const finalized=await stage('practice-replay-production-finalizer',{epoch:sourceEpoch,format:capability.format},()=>{
      const runtimeFrame=sourcePage.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));
      assert.ok(runtimeFrame,'Practice Replay Runtime frame is missing');
      return runtimeFrame.evaluate(capability.finalizer,{game:target.game,variant:target.variant,...capability.finalizerArgs});
    });
    if(finalized?.available!==true||finalized.finalized!==true)throw new Error(`Production Replay finalizer failed: ${finalized?.reason??'no successful save receipt'}`);
    const runtimeFrame=sourcePage.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));
    const synchronized=await stage('practice-replay-idbfs-sync-complete',{epoch:sourceEpoch},()=>
      runtimeFrame.evaluate(capability.sync.wait,{timeoutMs:30000,afterFinalizer:finalized.afterFinalizer}));
    const restored=await runtimeFrame.evaluate(capability.sync.restore);
    observerArmed=false;
    if(synchronized?.available!==true||synchronized.succeeded!==true||restored?.restored!==true)
      throw new Error(`Replay persistence boundary did not complete: ${JSON.stringify({finalized,synchronized,restored})}`);
    const saved=await stage('practice-replay-shell-readback',{path:capability.savePath},()=>sourcePage.evaluate(path=>rpc('read',{path}),capability.savePath));
    const raw=replayBytes(saved,'Saved practice Replay'),digest=sha256(raw),parsed=capability.parse(raw);
    const recordingStage=capability.recordingStage(sourcePractice,parsed);
    const inputExpectation=capability.inputExpectation(raw,{stage:recordingStage,expectedSha256:digest});
    const inputCapacityFrames=capability.inputCapacityFrames(inputExpectation);
    if(!Number.isSafeInteger(inputCapacityFrames)||inputCapacityFrames<=0)throw new Error('Independent Replay parser returned no usable input capacity');
    const recording={epoch:sourceEpoch,frameBefore:before.frameBefore,frameAfter:before.frameAfter,saveSucceeded:true,
      idbfsSyncSucceeded:true,path:capability.savePath,sha256:digest,bytes:raw.byteLength,format:capability.format,
      parsed:parsed.config??parsed,inputCapacityFrames,stage:recordingStage};
    recording.rawBytes=Array.from(raw);recording.persistence={finalizer:finalized,observer:synchronized,restored};caseRecord.p05Recording=recording;await save();

    if(practice.lifecycle){
      caseRecord.practiceLifecycle={epoch:sourceEpoch,appIdentity:await runtimeFrame.evaluate(runtime=>globalThis[runtime.global][runtime.app],titleAdapter.runtime),recording:{path:capability.savePath,sha256:digest,inputCapacityFrames}};
      const flow=resolveTitleFlow({game:target.game,variant:target.variant});
      assert.equal(typeof sourcePress,'function');const press=sourcePress;
      const observe=()=>runtimeFrame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
      const waitRuntime=async(label,predicate,timeoutMs=15000)=>{const started=Date.now();let current;do{current=await observe();if(predicate(current))return current;await sourcePage.waitForTimeout(75);}while(Date.now()-started<timeoutMs);throw Error(`${label}: ${JSON.stringify(current)}`);};
      const lifecycleResult=await practice.lifecycle.run({read:sourceReadPractice,waitUntil:sourceWaitPracticeUntil,waitForPracticeInput:sourceWaitForPracticeInput,readRuntime:observe,readInitial:()=>runtimeFrame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:titleAdapter.runtime,contract:practice,kind:'gameplayInitial'})),
        readIdentity:()=>runtimeFrame.evaluate(({runtime,epoch})=>({epoch,app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),{runtime:titleAdapter.runtime,epoch:sourceEpoch}),waitRuntime,press,
        restartFromPause:()=>flow.restartFromPause({press,waitUntil:waitRuntime,epoch:sourceEpoch}),
        enterOrdinary:()=>flow.enterGameplay({observe,waitUntil:waitRuntime,press,lane:'practice',epoch:sourceEpoch}),
        enterReplay:()=>capability.enterMenu({page:sourcePage,frame:runtimeFrame,target,runtime:titleAdapter.runtime,practice,
          readRuntimeObservation,runtimeObservationRequest,stage,press,importPath:capability.savePath,caseRecord:caseRecord.practiceLifecycle,save,sourcePractice,epoch:sourceEpoch,systemSlot:true,captureInitial:true}),
        replayConfig:parsed.config,record:caseRecord.practiceLifecycle,stage,save});
      assert.ok(lifecycleResult?.status==='observed'&&lifecycleResult.complete===true,'Practice lifecycle must have complete observed evidence before P05 continues');
      const appAfter=await runtimeFrame.evaluate(runtime=>globalThis[runtime.global][runtime.app],titleAdapter.runtime);
      assert.equal(appAfter,caseRecord.practiceLifecycle.appIdentity,'Lifecycle replaced the native GameApplication');
      caseRecord.practiceLifecycle.sameApplication=true;await save();
    }

    await sourceResponseProbe?.flush();
    if(sourceResponseProbe){for(const error of sourceResponseProbe.errors??[])record.responseErrors.push({epoch:sourceEpoch,error});
      await sourceResponseProbe.stop();sourceStopped=true;}
    await drainPracticeProtocolCapture(sourcePage);
    if(!sourcePage.isClosed())await sourcePage.close();

    page=await context.newPage();
    await bindPracticeProtocolCapture(page,target.game,freshEpoch,record);
    responseProbe=await site.watchPage(page,{output,label:`${target.id}-practice-replay-${freshEpoch}`});
    page.on('pageerror',error=>record.errors.push(String(error)));
    page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
    page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
    page.on('requestfailed',request=>record.requestFailures.push({url:request.url(),error:request.failure()?.errorText??null}));
    await stage('practice-replay-fresh-host-navigation',{epoch:freshEpoch},()=>page.goto(site.hostUrl(target,freshEpoch)));
    await stage('practice-replay-fresh-runtime-ready',{epoch:freshEpoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='ready'&&event.epoch===epoch),freshEpoch,{timeout:120000}));
    const hostFrame=await page.locator('iframe').elementHandle(),frame=await hostFrame.contentFrame();assert.ok(frame);
    const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
    await stage('practice-replay-fresh-configure',{epoch:freshEpoch},()=>page.evaluate(({sharedResources})=>rpc('configure',{
      language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,thpracCatalog:[],
      options:{touchEnabled:false,alwaysHitbox:false,thpracEnabled:true,thpracLocale:'ja-JP',limitPresentationTo60:false}}),{sharedResources}));
    const suffix=Array.from(randomBytes(4),byte=>'abcdefghijklmnopqrstuvwxyz0123456789'[byte%36]).join('');
    const importPath=capability.importPath(suffix);
    if(capability.isUserImportPath(importPath)!==true||importPath===capability.savePath)
      throw new Error('Title Replay adapter did not choose a distinct valid user-import path');
    const imported=await stage('practice-replay-official-prelaunch-import',{epoch:freshEpoch,path:importPath,beforeLaunch:true},async()=>{
      const bytes=Array.from(raw);
      const write=await page.evaluate(({path,bytes})=>rpc('write',{path,bytes}),{path:importPath,bytes});
      const readback=await page.evaluate(path=>rpc('read',{path}),importPath);
      const listed=await page.evaluate(()=>rpc('list'));
      assert.equal(write?.ok,true,'Official Shell did not acknowledge the Replay import');
      assert.deepEqual(readback.bytes,bytes,'Shell import readback changed Replay bytes');
      assert.ok(listed.files.some(file=>file.path===importPath),'Official Shell did not list imported Replay');
      return{beforeLaunch:true,writeSucceeded:write?.ok!==false,path:importPath,rawBytes:Uint8Array.from(readback.bytes),
        sha256:digest,readSha256:sha256(Uint8Array.from(readback.bytes)),listed:true};
    });
    await stage('practice-replay-fresh-launch',{epoch:freshEpoch},()=>page.evaluate(()=>rpc('launch')));
    await page.waitForFunction(epoch=>events.some(event=>event.event==='first-frame'&&event.epoch===epoch),freshEpoch,{timeout:120000});
    await responseProbe.flush();
    await frame.locator('canvas').first().focus();
    const persistedResult=await page.evaluate(path=>rpc('read',{path}),capability.savePath);
    const persistedBytes=replayBytes(persistedResult,'Fresh-epoch persisted Replay');
    const replayUiInput=practice.uiSnapshot?.keyboard?createPracticeUiInput({page,frame,adapter:titleAdapter}):null;
    if(replayUiInput)caseRecord.replayUiInput=replayUiInput.evidence;
    const replayPress=replayUiInput?replayUiInput.press:async key=>{await page.keyboard.press(key,{delay:90});await page.waitForTimeout(140);};
    const native=await capability.enterMenu({page,frame,target,runtime:titleAdapter.runtime,practice,
      readRuntimeObservation,runtimeObservationRequest,stage,press:replayPress,importPath,caseRecord,save,sourcePractice,epoch:freshEpoch});
    const evidence={recording:{...recording,rawBytes:raw},persisted:{rawBytes:persistedBytes,freshEpochSha256:sha256(persistedBytes)},
      import:imported,fresh:{epoch:freshEpoch,configured:native.initial?.configured},menu:{candidate:native.candidate},
      playback:{selectedGame:native.selectedGame,initialSnapshot:native.initialSnapshot,liveSnapshot:native.liveSnapshot,stream:native.stream}};
    const assessment=capability.assess(evidence,{selectionCase:capability.selectionCase??practice.selectionCase});
    const assessmentWithInput={...assessment,observationInput:createPracticeReplayObservationInput({format:capability.format,evidence})};
    caseRecord.p05=assessmentWithInput;await save();
    if(assessmentWithInput?.status!=='observed'||assessmentWithInput.complete!==true)
      throw new Error(`Native Replay roundtrip did not satisfy the title contract: ${JSON.stringify(assessmentWithInput)}`);
    return{assessment:assessmentWithInput,page,responseProbe};
  }catch(error){
    if(observerArmed){
      const runtimeFrame=sourcePage.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));
      try{await runtimeFrame?.evaluate(capability.sync.restore);}catch{}
    }
    if(responseProbe){
      try{await responseProbe.flush();}catch(flushError){record.responseErrors.push({epoch:freshEpoch,phase:'p05-error-flush',error:String(flushError.stack||flushError)});}
      try{for(const item of responseProbe.errors??[])record.responseErrors.push({epoch:freshEpoch,error:item});await responseProbe.stop();}catch{}
    }
    if(page&&!page.isClosed())try{await drainPracticeProtocolCapture(page);await page.close();}catch{}
    if(!sourceStopped&&sourceResponseProbe){try{await sourceResponseProbe.stop();}catch{}}
    if(!sourcePage.isClosed())try{await sourcePage.close();}catch{}
    throw error;
  }
}


const result=await runWithReport({output,schema:'eagler-touhou/practice-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','variant','packages','artwork-dir'],flags:['launcher-ui','launcher-ui-only','parameters-only','effects-only','options-only','boss-only','key-monitor-only','gameplay-options-only','lifecycle-options-only','math-options-only','display-options-only']});
  assert.equal(option('variant')??'normal','normal','Practice menu driver requires an ordinary Runtime');
  assert.ok(option('manifest')&&option('game'),'Required: --manifest=private-inputs.json --game=explicit-selection');
  const config=JSON.parse(await readFile(resolve(option('manifest')),'utf8'));
  const launcherOnly=process.argv.includes('--launcher-ui-only'),launcherUi=launcherOnly||process.argv.includes('--launcher-ui');
  const parametersOnly=process.argv.includes('--parameters-only');assert.ok(!parametersOnly||!launcherUi,'Parameter matrix uses its own native consumer scope');
  const effectsOnly=process.argv.includes('--effects-only');assert.ok(!effectsOnly||!parametersOnly&&!launcherUi,'Trainer consumers use their own native scope');
  const optionsOnly=process.argv.includes('--options-only');assert.ok(!optionsOnly||!effectsOnly&&!parametersOnly&&!launcherUi,'Advanced consumers use their own native scope');
  const bossOnly=process.argv.includes('--boss-only');assert.ok(!bossOnly||!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Boss consumers use their own native scope');
  const keyMonitorOnly=process.argv.includes('--key-monitor-only');assert.ok(!keyMonitorOnly||!bossOnly&&!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Key monitor uses its own native scope');
  const gameplayOptionsOnly=process.argv.includes('--gameplay-options-only');assert.ok(!gameplayOptionsOnly||!keyMonitorOnly&&!bossOnly&&!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Advanced gameplay uses its own native scope');
  const lifecycleOptionsOnly=process.argv.includes('--lifecycle-options-only');assert.ok(!lifecycleOptionsOnly||!gameplayOptionsOnly&&!keyMonitorOnly&&!bossOnly&&!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Lifecycle options use their own native scope');
  const mathOnly=process.argv.includes('--math-options-only');assert.ok(!mathOnly||!lifecycleOptionsOnly&&!gameplayOptionsOnly&&!keyMonitorOnly&&!bossOnly&&!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Math options use their own native scope');
  const displayOnly=process.argv.includes('--display-options-only');assert.ok(!displayOnly||!mathOnly&&!lifecycleOptionsOnly&&!gameplayOptionsOnly&&!keyMonitorOnly&&!bossOnly&&!effectsOnly&&!optionsOnly&&!parametersOnly&&!launcherUi,'Display tools use their own native scope');
  if(launcherUi)assert.ok(option('packages')&&option('artwork-dir'),'Launcher Practice UI requires --packages and --artwork-dir');
  const launcherPackages=launcherUi?JSON.parse(await readFile(resolve(option('packages')),'utf8')):null;
  const selection=selectRuntimeTargets(config,{game:option('game'),variant:'normal'});
  const missingPracticeAdapters=selection.targets.filter(target=>PRODUCT_GAMES[target.game].features.thprac===true&&
    !resolveAdaptationAdapter(target.game,target.variant)?.practice);
  if(missingPracticeAdapters.length)throw new Error(`Selected practice products need a native practice adapter before browser launch: ${missingPracticeAdapters.map(target=>target.id).join(', ')}`);
  Object.assign(report,{selection:{games:selection.games,variant:'normal',expectedTargets:selection.expected},
    launcherUiOnly:launcherOnly,parametersOnly,effectsOnly,optionsOnly,bossOnly,keyMonitorOnly,gameplayOptionsOnly,lifecycleOptionsOnly,mathOnly,displayOnly,
    scope:launcherOnly?'Real Launcher | Japanese Practice setting, Host capability and native gameplay menu':
      'Ordinary Runtime | title-declared native practice selection and parameter consumption | native practice controls | locale screenshots for review | declared Replay roundtrip capabilities',
    coverage:{ids:effectsOnly||optionsOnly||bossOnly||keyMonitorOnly||gameplayOptionsOnly||lifecycleOptionsOnly||mathOnly||displayOnly?['PRACTICE-08']:parametersOnly?['PRACTICE-04']:launcherOnly?['PRACTICE-01','PRACTICE-02']:Array.from(new Set(selection.games.flatMap(game=>practiceAdaptationIds(game,'normal')))),complete:false,
      scope:launcherOnly?'Three real Launcher settings scenes. Native locale, parameter and Replay scenarios are not executed by this invocation.':
        'Four isolated locale Runtime epochs per selected target. A registered title capability supplies its native menu path, ABI and parameter case. Replay roundtrip operations run when the selected capability declares them.'},targets:[]});
  report.harnessSources=await dependencySnapshot(repoRoot,['tools/ports/practice-browser.mjs']);
  for(const source of report.harnessSources){
    const destination=resolve(output,'harness-source',...source.path.split('/'));
    await mkdir(dirname(destination),{recursive:true});
    await writeFile(destination,await readFile(resolve(repoRoot,source.path)));
  }
  const site=await createRuntimeFixtureSite({repoRoot,targets:selection.targets,shared:config.shared,protocol:'https'});
  report.transport={base:site.base,protocol:site.protocol,tlsIdentity:site.tlsIdentity};
  report.sharedInputs=await site.verifyShared();
  let browser;
  try{
    const {launchBrowser}=await import('../browser/launch.mjs');
    browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${site.tlsIdentity.spkiSha256}`]});
    report.browser=browser.version();
    for(const target of selection.targets){
      const titleFlow=resolveTitleFlow({game:target.game,variant:target.variant});
      const titleAdapter=resolveAdaptationAdapter(target.game,target.variant);
      const practiceCapability=titleAdapter?.practice;
      const record={id:target.id,game:target.game,variant:target.variant,checks:[],errors:[],consoleErrors:[],httpErrors:[],requestFailures:[],protocolErrors:[],responseErrors:[],teardownErrors:[],locales:[]};
      report.targets.push(record);await save();
      const stage=createStageRecorder(record,save);
      let context,responseProbe,page;
      try{
        record.identity=await stage('identity-preflight',{},()=>site.verify(target));
        const manifestBytes=await readVerifiedRuntimeFile(target,record.identity.runtime,'manifest.json'),manifest=JSON.parse(manifestBytes);
        const planBytes=await readFile(resolve(repoRoot,record.identity.runtime.sourceBuild.buildPlan));
        record.buildFeatureEvidence={};
        for(const [name,bytes] of [['manifest',manifestBytes],['plan',planBytes]]){
          const path=resolve(output,`${target.id}-practice-${name}.json`);await writeFile(path,bytes);
          record.buildFeatureEvidence[name]={path,bytes:bytes.length,sha256:sha256(bytes)};
        }
        record.feature=practiceBuildFeature(manifest,PRODUCT_GAMES[target.game]);
        if(!record.feature){
          if(PRODUCT_GAMES[target.game].features.thprac){
            record.inputIssue='practice-enabled-package-required';
            throw Error('Practice behavior lane requires a practice-enabled package; the selected valid package has practice disabled');
          }
          record.status='not-applicable';record.reason='This product does not support practice';continue;
        }
        if(parametersOnly)assert.ok(practiceCapability.parameterMatrix?.length&&practiceCapability.assessParameterMatrix&&practiceCapability.programSnapshot,'Register complete parameter consumers before running the matrix');
        if(effectsOnly)assert.ok(practiceCapability.trainer&&titleAdapter.clock&&titleAdapter.deathbomb&&titleAdapter.controlledScene,'Register trainer consumers and the guarded production arena');
        if(optionsOnly)assert.ok(practiceCapability.options&&titleAdapter.controlledScene&&titleAdapter.clock,'Register native option controls and production consumers');
        if(bossOnly)assert.ok(practiceCapability.bossEffects&&titleAdapter.clock,'Register actual Boss damage and clock consumers');
        if(keyMonitorOnly)assert.ok(practiceCapability.options?.keyMonitor&&titleAdapter.clock&&titleAdapter.controlledScene,'Register actual key record, APS and CSV consumers');
        if(gameplayOptionsOnly)assert.ok(practiceCapability.options?.gameplay?.cases.length&&practiceCapability.trainer&&titleAdapter.clock&&titleAdapter.controlledScene&&titleAdapter.deathbomb,'Register advanced gameplay consumers and collision arena');
        if(lifecycleOptionsOnly)assert.ok(practiceCapability.options?.lifecycle&&titleAdapter.clock&&titleAdapter.controlledScene&&titleAdapter.deathbomb,'Register native lifecycle option consumers');
        const runs=displayOnly?practiceCapability.options.display.cases.map((row,index)=>[index+1,true,'en-US',{displayEnabled:row.enabled}]):mathOnly?[[1,true,'en-US']]:lifecycleOptionsOnly?practiceCapability.options.lifecycle.cases.map((row,index)=>[index+1,true,'en-US',{lifecycleCase:row}]):gameplayOptionsOnly?practiceCapability.options.gameplay.cases.map((trial,index)=>[index+1,true,'en-US',{trainerTrial:trial,params:{...practiceCapability.parameterCase,...trial.params}}]):keyMonitorOnly?[[1,true,'en-US']]:bossOnly?[[1,true,'en-US',{params:{...practiceCapability.parameterCase,...practiceCapability.bossEffects.parameters}}]]:optionsOnly?[[1,true,'en-US']]:effectsOnly?practiceCapability.trainer.cases.map((trial,index)=>[index+1,true,'en-US',{trainerTrial:trial}]):parametersOnly?practiceCapability.parameterMatrix.map((row,index)=>[index+1,true,'ja-JP',row]):[[1,true,'ja-JP'],[2,true,'en-US'],[3,true,'zh-CN'],[4,false,'en-US']];
        if(!launcherOnly)for(const [epoch,enabled,locale,parameter] of runs){
          const caseRecord={epoch,enabled,locale,status:'running',...(parameter?{parameterId:parameter.id}:{})};
          record.locales.push(caseRecord);await save();
          try{
          context=await browser.newContext({viewport:{width:1100,height:820}});
          await context.addInitScript(()=>{
            if(window===window.top)return;
            window.__practiceBrowserEvents=[];
            addEventListener('eagler-thprac-menu',event=>window.__practiceBrowserEvents.push({open:event.detail?.open,at:performance.now()}));
          });
          page=await context.newPage();
          await bindPracticeProtocolCapture(page,target.game,epoch,record);
          responseProbe=await site.watchPage(page,{output,label:`${target.id}-${epoch}`});
          caseRecord.responseIntegrity={responses:responseProbe.responses,errors:responseProbe.errors,capture:responseProbe.capture};
          page.on('pageerror',error=>record.errors.push(String(error)));
          page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
          page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
          page.on('requestfailed',request=>record.requestFailures.push({url:request.url(),error:request.failure()?.errorText??null}));
          await stage('host-navigation',{epoch,locale,enabled},()=>page.goto(site.hostUrl(target,epoch)));
          await responseProbe.flush();
          await stage('runtime-ready',{epoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='ready'&&event.epoch===epoch),epoch,{timeout:120000}));
          const element=await page.locator('iframe').elementHandle(),frame=await element.contentFrame();assert.ok(frame);
          const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
          const actualPracticeCase=Boolean(practiceCapability)&&enabled&&(displayOnly||mathOnly||lifecycleOptionsOnly||gameplayOptionsOnly||bossOnly||optionsOnly||effectsOnly||parametersOnly||epoch===1),parameterCase=parameter?.params??practiceCapability?.parameterCase;
          const configuration={language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,
            thpracCatalog:[],options:{touchEnabled:false,alwaysHitbox:false,thpracEnabled:enabled,thpracLocale:locale,limitPresentationTo60:false,
              ...(actualPracticeCase?{thprac:{params:parameterCase}}:{})}};
          caseRecord.startup={configuration};
          caseRecord.startup.configured=await stage('configure',{epoch,locale,enabled,actualPracticeCase},
            ()=>page.evaluate(configuration=>rpc('configure',configuration),configuration));
          caseRecord.startup.launched=await stage('launch',{epoch},()=>page.evaluate(()=>rpc('launch')));
          await stage('first-frame',{epoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='first-frame'&&event.epoch===epoch),epoch,{timeout:120000}));
          caseRecord.startup.hostEvents=await page.evaluate(()=>events.map(event=>({...event})));
          caseRecord.startup.firstFrame={menu:await frame.evaluate(readPracticeMenuRaw,{runtime:titleAdapter.runtime}),
            snapshot:await frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:titleAdapter.runtime,contract:practiceCapability})),
            identity:await frame.evaluate(({runtime,epoch})=>({epoch,app:globalThis[runtime.global][runtime.app],
              documentTimeOrigin:performance.timeOrigin,url:location.href}),{runtime:titleAdapter.runtime,epoch})};
          await save();
          await responseProbe.flush();
          await frame.locator('canvas').first().focus();
          if(lifecycleOptionsOnly&&parameter.lifecycleCase.kind==='one-key-death')caseRecord.disabledTitle=await preparePracticeDeathTitle({page,frame,target,adapter:titleAdapter,
            save:async e=>{caseRecord.disabledTitle=e;await save();}});
          if(effectsOnly||optionsOnly||keyMonitorOnly||gameplayOptionsOnly||lifecycleOptionsOnly||mathOnly||displayOnly)caseRecord.trainerArm=assertControlledSceneArmed(await frame.evaluate(controlValidationScene,validationSceneRequest({...target,action:'arm'})));
          if(parametersOnly)caseRecord.exitObserver=await frame.evaluate(installPracticeExitObserver,{runtime:titleAdapter.runtime});
          const observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
          const ready=state=>titleFlow.isGameplayReady(state,'practice');
          const waitUntil=async(label,predicate,timeoutMs=30000)=>{
            const started=Date.now();let state;
            do{state=await observe();if(predicate(state))return state;await page.waitForTimeout(75);}
            while(Date.now()-started<timeoutMs);
            throw new Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);
          };
          const uiInput=enabled&&practiceCapability?.uiSnapshot?.keyboard?createPracticeUiInput({page,frame,adapter:titleAdapter}):null;
          if(uiInput)caseRecord.practiceInput=uiInput.evidence;
          const press=uiInput?uiInput.press:async code=>{const key=code==='KeyZ'?'z':code;await page.keyboard.press(key,{delay:90});await page.waitForTimeout(140);};
          const readPractice=async()=>practiceCapability.snapshot.decode(await frame.evaluate(readPracticeSnapshotRaw,
            practiceSnapshotRequest({runtime:titleAdapter.runtime,contract:practiceCapability})));
          const waitPracticeUntil=async(label,predicate,timeoutMs=30000)=>{
            const started=Date.now();let state;
            do{state=await readPractice();if(predicate(state))return state;await page.waitForTimeout(75);}
            while(Date.now()-started<timeoutMs);
            throw new Error(`${label}: native PracticeSnapshot condition not reached: ${JSON.stringify(state)}`);
          };
          if(actualPracticeCase){
            const entered=await runPracticeGameplayStage({record:caseRecord,stage,epoch,ready,perform:()=>practiceCapability.enterMenu({
              read:readPractice,waitUntil:waitPracticeUntil,press,waitForPracticeInput:uiInput?.waitForPracticeInput,waitForGameplay:()=>waitUntil('native practice gameplay ready',ready),
              readInitial:()=>frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:titleAdapter.runtime,contract:practiceCapability,kind:'gameplayInitial'})),
              readIdentity:()=>frame.evaluate(({runtime,epoch})=>({epoch,app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),{runtime:titleAdapter.runtime,epoch}),
              parameterCase,selectionCase:practiceCapability.selectionCase}),
              assess:(snapshot,observed)=>practiceCapability.assessConsumption(snapshot,parameterCase,practiceCapability.selectionCase,observed)});
            caseRecord.practiceSnapshot=entered.practice;
            caseRecord.parameterConsumption=entered.parameterConsumption;
            caseRecord.featureInventory=entered.featureInventory;
            if(displayOnly){
              caseRecord.displayOptions=await runPracticeDisplay({page,frame,target,adapter:titleAdapter,arm:caseRecord.trainerArm,enabled:parameter.displayEnabled,output,repoRoot,save:async e=>{caseRecord.displayOptions=e;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(mathOnly){
              caseRecord.mathOptions=await runPracticeMathOptions({page,frame,target,adapter:titleAdapter,arm:caseRecord.trainerArm,output,repoRoot,save:async e=>{caseRecord.mathOptions=e;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(lifecycleOptionsOnly){
              caseRecord.lifecycleKind=parameter.lifecycleCase.kind;
              caseRecord.lifecycleOption=await runPracticeLifecycleOption({page,frame,target,adapter:titleAdapter,kind:caseRecord.lifecycleKind,arm:caseRecord.trainerArm,disabledTitle:caseRecord.disabledTitle,
                save:async e=>{caseRecord.lifecycleOption=e;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(keyMonitorOnly){
              caseRecord.keyMonitor=await runPracticeKeyMonitorBrowser({page,frame,target,adapter:titleAdapter,arm:caseRecord.trainerArm,output,
                save:async evidence=>{caseRecord.keyMonitor=evidence;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(bossOnly){
              caseRecord.bossEffects=await runPracticeBossBrowser({page,frame,target,adapter:titleAdapter,press,
                save:async evidence=>{caseRecord.bossEffects=evidence;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(optionsOnly){
              caseRecord.options=await runPracticeOptionsBrowser({page,frame,target,adapter:titleAdapter,arm:caseRecord.trainerArm,press,
                save:async evidence=>{caseRecord.options=evidence;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(effectsOnly||gameplayOptionsOnly){
              caseRecord.trainerId=parameter.trainerTrial.id;
              caseRecord.trainer=await runPracticeTrainerBrowser({page,frame,target,adapter:titleAdapter,trial:parameter.trainerTrial,
                arm:caseRecord.trainerArm,press,save:async evidence=>{caseRecord.trainer=evidence;await save();}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
            if(parametersOnly){
              caseRecord.parameterRaw=await frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:titleAdapter.runtime,contract:practiceCapability}));
              caseRecord.practiceSnapshot=practiceCapability.snapshot.decode(caseRecord.parameterRaw);
              caseRecord.programRaw=await frame.evaluate(readPracticeProgramRaw,{runtime:titleAdapter.runtime,contract:{programSnapshot:practiceCapability.programSnapshot}});
              caseRecord.status='passed';await responseProbe.flush();await save();continue;
            }
          }else{
            caseRecord.gameplay=await stage('enter-gameplay',{epoch},async()=>{
              const state=await titleFlow.enterGameplay({observe,waitUntil,lane:'practice',epoch,label:'Practice overlay requires actual gameplay',press});
              assert.ok(ready(state),'Practice overlay requires actual gameplay before opening');
              return state;
            });
          }
          const menu=async()=>{
            const state=await frame.evaluate(readPracticeMenuRaw,{runtime:titleAdapter.runtime});
            if(state?.available!==true)throw new Error(`Declared practice Runtime module is unavailable: ${state?.reason??'invalid observation'}`);
            const{available,...observation}=state;return observation;
          };
          const waitForMenu=async(expectedOpen,timeoutMs=5000)=>{
            const started=Date.now();let state;
            do{state=await menu();if(state.open===expectedOpen)return state;await page.waitForTimeout(50);}
            while(Date.now()-started<timeoutMs);
            throw new Error(`Native practice menu did not become ${expectedOpen?'open':'closed'}: ${JSON.stringify(state)}`);
          };
          const before=await menu();caseRecord.before=before;await save();
          assert.equal(before.open,false,'Fresh gameplay inherited an open practice overlay');
          await press('Backspace');
          if(enabled)await waitForMenu(true);
          else await page.waitForTimeout(500);
          const opened=await menu();caseRecord.opened=opened;await save();
          const screenshot=`${target.id}-${enabled?locale:'disabled'}-practice.png`;
          await stage('menu-screenshot',{epoch},()=>page.screenshot({path:resolve(output,screenshot),timeout:15000}));
          caseRecord.screenshot=screenshot;await save();
          assert.equal(opened.open,enabled,'Practice toggle does not match built/configured feature');
          assert.equal(opened.options?.thpracEnabled,enabled,'Native UI options did not receive the enabled state');
          assert.equal(opened.options?.thpracLocale,locale,'Native UI options did not receive the configured locale');
          if(enabled){
            assert.ok(opened.events.some(event=>event.open===true),'Native practice owner did not publish menu opening');
            if(actualPracticeCase&&practiceCapability.exerciseControls){
              const changed=await practiceCapability.exerciseControls({read:readPractice,press,
                clickPointer:(x,y)=>clickCanvasLogical(frame,page,x,y),
                waitForChange:async(read,predicate,timeoutMs)=>{
                  const started=Date.now();let state;
                  do{state=await read();if(predicate(state))return state;await page.waitForTimeout(75);}
                  while(Date.now()-started<timeoutMs);
                  return state;
                }});
              caseRecord.p06=changed.result;
              caseRecord.featureInventory=changed.featureInventory;
              await save();
            }
            await press('Backspace');
            await waitForMenu(false);
          }
          const closed=await menu();caseRecord.closed=closed;await save();
          assert.equal(closed.open,false,'Practice menu failed to close');
          if(enabled)assert.ok(closed.events.slice(opened.events.length).some(event=>event.open===false),
            'Native practice owner did not publish menu closing');
          if(actualPracticeCase&&practiceCapability.replayRoundTrip?.supported===true){
            try{
              const roundTrip=await runPracticeReplayRoundTrip({browser,context,sourcePage:page,sourceResponseProbe:responseProbe,
                target,titleAdapter,practice:practiceCapability,sourceReadPractice:readPractice,
                sourceWaitPracticeUntil:waitPracticeUntil,sourceWaitForPracticeInput:uiInput?.waitForPracticeInput,sourcePress:press,sourcePractice:caseRecord.practiceSnapshot,caseRecord,record,
                stage,site,config,output,save});
              caseRecord.p05=roundTrip.assessment;page=roundTrip.page;responseProbe=roundTrip.responseProbe;
            }catch(error){
              if(!caseRecord.p05)caseRecord.p05={status:'failed',complete:false,
                missing:[],problems:[String(error.stack||error)],scope:'recorded practice values restored by the native Replay menu in a fresh Runtime epoch'};
              else caseRecord.p05Failure=String(error.stack||error);
              await save();throw error;
            }
          }
          Object.assign(caseRecord,{status:'passed',closed,
            localeReview:'Screenshot records displayed glyphs and labels; native locale correctness requires image review'});
          await responseProbe.flush();
          }catch(error){Object.assign(caseRecord,{status:'failed',error:String(error.stack||error)});}
          finally{
            if(parametersOnly&&page&&!page.isClosed())try{
              caseRecord.runtimeEvents=await page.evaluate(()=>events);
              const endingFrame=page.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));
              if(endingFrame)caseRecord.exitEvidence=await endingFrame.evaluate(releasePracticeExitObserver);
            }catch(error){record.teardownErrors.push({epoch,phase:'practice-completion-observer.restore',error:String(error)});}
            if(responseProbe){
              try{await responseProbe.flush();}catch(error){record.responseErrors.push({epoch,phase:'flush-before-stop',error:String(error.stack||error)});}
              try{await responseProbe.stop();}catch(error){record.teardownErrors.push({epoch,phase:'response-probe.stop',error:String(error.stack||error)});}
              try{await responseProbe.flush();}catch(error){record.responseErrors.push({epoch,phase:'flush-after-stop',error:String(error.stack||error)});}
              for(const error of responseProbe.errors??[])record.responseErrors.push({epoch,error});
            }
            try{await drainPracticeProtocolCapture(page);}catch(error){record.teardownErrors.push({epoch,phase:'protocol-capture.drain',error:String(error.stack||error)});}
            if(page&&!page.isClosed())try{await page.close();}catch(error){record.teardownErrors.push({epoch,phase:'page.close',error:String(error.stack||error)});}
            if(context)try{await context.close();}catch(error){record.teardownErrors.push({epoch,phase:'context.close',error:String(error.stack||error)});}
            responseProbe=null;context=null;page=null;await save();
          }
        }
        assert.deepEqual(record.errors,[],'Browser errors');assert.deepEqual(record.consoleErrors,[],'Runtime errors');assert.deepEqual(record.httpErrors,[],'HTTP errors');
        if(!launcherOnly){
          assert.equal(record.locales.length,runs.length,'Every declared native parameter or locale scene must run');
          assert.ok(record.locales.every(item=>item.status==='passed'),'A declared Practice scene failed; see its saved native state');
        }
        if(launcherUi){
          const {runLauncherPracticeJourney}=await import('./launcher-practice-journey.mjs');
          record.launcherPractice=await runLauncherPracticeJourney({root:repoRoot,output:resolve(output,'launcher-ui-'+target.game),target,
            identity:record.identity.runtime,dataIdentity:record.identity.data,shared:config.shared,artwork:resolve(option('artwork-dir')),packageMap:launcherPackages,
            launchBrowser:({tlsIdentity})=>launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',`--ignore-certificate-errors-spki-list=${tlsIdentity.spkiSha256}`]})});
          assert.equal(record.launcherPractice.status,'observed','Real Launcher Practice settings failed; inspect the retained journey');
          assert.equal(record.launcherPractice.scenes.length,3);assert.ok(record.launcherPractice.scenes.every(scene=>scene.passed===true));
        }
        record.status='passed';
      }catch(error){record.status='failed';record.error=String(error.stack||error);
        const active=record.locales.find(item=>item.status==='running');if(active)Object.assign(active,{status:'failed',error:record.error});}
      finally{
        if(responseProbe){
          try{await responseProbe.flush();}catch(error){record.responseErrors.push({phase:'target-flush-before-stop',error:String(error.stack||error)});}
          try{await responseProbe.stop();}catch(error){record.teardownErrors.push({phase:'target-response-probe.stop',error:String(error.stack||error)});}
          try{await responseProbe.flush();}catch(error){record.responseErrors.push({phase:'target-flush-after-stop',error:String(error.stack||error)});}
          for(const error of responseProbe.errors??[])record.responseErrors.push({error});
        }
        try{await drainPracticeProtocolCapture(page);}catch(error){record.teardownErrors.push({phase:'target-protocol-capture.drain',error:String(error.stack||error)});}
        if(page&&!page.isClosed())try{await page.close();}catch(error){record.teardownErrors.push({phase:'target-page.close',error:String(error.stack||error)});}
        if(context)try{await context.close();}catch(error){record.teardownErrors.push({phase:'target-context.close',error:String(error.stack||error)});}
        responseProbe=null;context=null;page=null;
        try{record.postflight=await site.verify(target);}catch(error){record.status='failed';record.postflightError=String(error.stack||error);}
        record.finalErrorAudit=auditBrowserFinalErrors({pageErrors:record.errors,consoleErrors:record.consoleErrors,
          httpErrors:[...record.httpErrors,...record.requestFailures],protocolErrors:record.protocolErrors,
          responseErrors:record.responseErrors,teardownErrors:record.teardownErrors});
        if(record.finalErrorAudit.status==='failed'){
          record.preFinalAuditStatus=record.status;record.status='failed';
          record.error??=`Final browser error audit failed: ${record.finalErrorAudit.failedChannels.join(', ')}`;
        }
        if(displayOnly){
          let assessment;try{assert.deepEqual(record.locales.map(row=>row.displayOptions?.enabled),practiceCapability.options.display.cases.map(row=>row.enabled));
            const license=await practiceCapability.options.display.loadLicense(repoRoot);assessment=await Promise.all(record.locales.map(async row=>assertPracticeDisplay(row.displayOptions,practiceCapability.options,{license,images:await Promise.all(row.displayOptions.images.map(image=>readFile(image.path)))})));
          }catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,scope:'Actual bullet overlay geometry and license text submission',evidenceRefs:['/locales'],observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:assessment??{complete:false}}});
        }else if(mathOnly){
          let assessment;try{assert.equal(record.locales.length,1);const e=record.locales[0].mathOptions;
            assessment=assertPracticeMathOptions(e,practiceCapability.options,{reference:await practiceCapability.options.math.loadReference(repoRoot),
              ...Object.fromEntries(await Promise.all(['original','imported','exported'].map(async name=>[name,await readFile(e.files[name].path)])))});
          }catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,scope:'Actual CPU math consumers and native file import/export',evidenceRefs:['/locales'],observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:assessment??{complete:false}}});
        }else if(lifecycleOptionsOnly){
          let assessment;try{assert.deepEqual(record.locales.map(row=>row.lifecycleKind),practiceCapability.options.lifecycle.cases.map(row=>row.kind));
            assessment=record.locales.map(row=>assertPracticeLifecycleOption(row.lifecycleOption,practiceCapability.options,row.lifecycleKind));
          }catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,
            scope:'Native automatic restart and one-key death with disabled title control',evidenceRefs:['/locales'],
            observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:{cases:assessment??[],complete:false}}});
        }else if(keyMonitorOnly){
          let assessment;try{assert.equal(record.locales.length,1);const e=record.locales[0].keyMonitor;
            assessment=assertPracticeKeyMonitor(e,practiceCapability.options,{csvBytes:await readFile(e.csv.path)});
          }catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,
            scope:'Native keyboard monitor, 60-tick APS and actual CSV download',evidenceRefs:['/locales'],
            observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:assessment??{complete:false}}});
        }else if(bossOnly){
          let assessment;try{assert.equal(record.locales.length,1);assessment=assertPracticeBossEffects(record.locales[0].bossEffects,practiceCapability.bossEffects);}catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,
            scope:'Actual Boss damage, life and timeout consumers',evidenceRefs:['/locales'],
            observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:assessment??{complete:false}}});
        }else if(optionsOnly){
          let assessment;try{assert.equal(record.locales.length,1);assessment=assertPracticeInputOptions(record.locales[0].options,practiceCapability.options);}catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,
            scope:'Advanced native controls | physical input filters and player consumers',evidenceRefs:['/locales'],
            observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],observation:assessment??{complete:false}}});
        }else if(effectsOnly||gameplayOptionsOnly){
          let assessment;try{assessment=assertPracticeTrainerMatrix(record.locales,gameplayOptionsOnly?{...practiceCapability.trainer,cases:practiceCapability.options.gameplay.cases}:practiceCapability.trainer);}catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-08',status:record.status==='passed'?'passed':'failed',complete:false,
            scope:'Actual trainer collision, resources and Tracker counters | declared cases; Advanced and boss effects use separate consumer obligations',
            evidenceRefs:['/locales'],observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-08'],
              observation:assessment??{complete:false}}});
        }else if(parametersOnly){
          let assessment;try{assessment=practiceCapability.assessParameterMatrix(record.locales);}catch(error){record.status='failed';record.error??=String(error.stack||error);}
          record.checks.push({id:'PRACTICE-04',status:record.status==='passed'?'passed':'failed',complete:record.status==='passed'&&assessment?.complete===true,
            scope:'Native Practice parameter classes | frozen resource and ECL consumers',observation:{game:target.game,variant:target.variant,adaptationIds:['PRACTICE-04'],observation:assessment??null}});
        }else record.checks.push(...practiceAdaptationChecks({game:target.game,variant:target.variant,
          productSupported:PRODUCT_GAMES[target.game].features.thprac,feature:record.feature,locales:record.locales,
          finalErrorAudit:record.finalErrorAudit,
          practiceReplayRoundTrip:record.locales.find(item=>item.epoch===1)?.p05,launcherPractice:record.launcherPractice}));
        if(record.status==='passed'&&record.launcherPractice){
          const entry=record.checks.find(check=>check.id==='PRACTICE-01');
          try{
            entry.observation.observation.buildIdentity=assertPracticeCapabilityEvidence({target:record,product:{game:target.game,variant:target.variant}});
            entry.status='passed';entry.complete=true;entry.scope='Current compiled package | published feature, real Host settings, native owner and UI capability matrix';
          }catch(error){entry.status='failed';entry.complete=false;entry.error=String(error.stack||error);record.status='failed';record.error??=entry.error;}
        }
        await save();console.log(`${target.id}: ${record.status.toUpperCase()} ${record.error?.split('\n')[0]??''}`);
      }
    }
  }finally{
    report.teardownErrors=[];
    if(browser)try{await browser.close();}catch(error){report.teardownErrors.push({phase:'browser.close',error:String(error.stack||error)});}
    if(site)try{await site.close();}catch(error){report.teardownErrors.push({phase:'fixture.close',error:String(error.stack||error)});}
    report.httpFailures=site.httpFailures;
  }
  report.sharedInputsPostflight=await site.verifyShared();
  assert.deepEqual(await dependencySnapshot(repoRoot,['tools/ports/practice-browser.mjs']),report.harnessSources,
    'Practice browser dependency closure changed during acceptance');
  report.finalErrorAudit=auditBrowserFinalErrors({
    pageErrors:report.targets.flatMap(target=>target.errors),
    consoleErrors:report.targets.flatMap(target=>target.consoleErrors),
    httpErrors:[...report.targets.flatMap(target=>[...target.httpErrors,...target.requestFailures]),...site.httpFailures],
    protocolErrors:report.targets.flatMap(target=>target.protocolErrors),
    responseErrors:report.targets.flatMap(target=>target.responseErrors),
    teardownErrors:[...report.teardownErrors,...report.targets.flatMap(target=>target.teardownErrors)],
  });
  report.passed=report.targets.length===selection.expected.length&&report.targets.every(target=>['passed','not-applicable'].includes(target.status))&&site.httpFailures.length===0&&report.finalErrorAudit.status==='passed';
  if(parametersOnly){report.scope='Native Practice parameter classes | frozen resources, timelines and ECL instructions';
    report.coverage.scope='All declared parameter cases in separate cold Runtime epochs';report.coverage.complete=report.passed;}
  if(effectsOnly){report.scope='Native trainer consumers | actual collisions, resources and Tracker';
    report.coverage.scope='Declared trainer cases; full PRACTICE-08 also requires every Advanced and boss function';}
  if(displayOnly){report.scope='Native display tools | real bullet geometry and license rendering';report.coverage.scope='Bullet overlay on/off and about license';}
  if(mathOnly){report.scope='Native CPU math options | real normalization and table import/export';report.coverage.scope='RSQRT CPU and custom table';}
  if(optionsOnly){report.scope='Native Advanced options | actual physical inputs and player consumers';report.coverage.scope='Declared input option cases';}
  if(bossOnly){report.scope='Native Practice Boss | actual damage, life and timeout';report.coverage.scope='F5 and U in one real Boss world';}
  if(keyMonitorOnly){report.scope='Native key monitor | recorded native input, rolling APS and actual CSV';report.coverage.scope='Keyboard monitor and export';}
  if(gameplayOptionsOnly){report.scope='Native Advanced gameplay options | real collision and resource consumers';report.coverage.scope='Declared advanced gameplay cases';}
  if(lifecycleOptionsOnly){report.scope='Native Advanced lifecycle options | actual restart and death';report.coverage.scope='Automatic retry and one-key death';}
  if(!report.passed&&report.finalErrorAudit.status==='failed')report.status='failed';
});
if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
