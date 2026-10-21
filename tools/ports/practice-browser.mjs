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
import {createPracticeReplayObservationInput,practiceSnapshotRequest,readPracticeMenuRaw,readPracticeSnapshotRaw,
  syncPracticeIdbfsRaw} from '../../tests/adaptation/adapters/practice.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';

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
  sourceReadPractice,sourceWaitPracticeUntil,sourcePractice,caseRecord,record,stage,site,config,output,save}={}){
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
        const frame=capability.recordingFrame(next);return Number.isSafeInteger(frame)&&frame>frameBefore;
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
    const syncCall=await runtimeFrame.evaluate(syncPracticeIdbfsRaw,{runtime:titleAdapter.runtime});
    const synchronized=await stage('practice-replay-idbfs-sync-complete',{epoch:sourceEpoch},()=>
      runtimeFrame.evaluate(capability.sync.wait,{timeoutMs:30000,afterCalls:armed.calls}));
    const restored=await runtimeFrame.evaluate(capability.sync.restore);
    observerArmed=false;
    if(syncCall?.available!==true||syncCall.succeeded!==true||synchronized?.available!==true||synchronized.succeeded!==true||restored?.restored!==true)
      throw new Error(`Replay persistence boundary did not complete: ${JSON.stringify({syncCall,synchronized,restored})}`);
    const saved=await stage('practice-replay-shell-readback',{path:capability.savePath},()=>sourcePage.evaluate(path=>rpc('read',{path}),capability.savePath));
    const raw=replayBytes(saved,'Saved practice Replay'),digest=sha256(raw),parsed=capability.parse(raw);
    const recordingStage=capability.recordingStage(sourcePractice,parsed);
    const inputExpectation=capability.inputExpectation(raw,{stage:recordingStage,expectedSha256:digest});
    const inputCapacityFrames=capability.inputCapacityFrames(inputExpectation);
    if(!Number.isSafeInteger(inputCapacityFrames)||inputCapacityFrames<=0)throw new Error('Independent Replay parser returned no usable input capacity');
    const recording={epoch:sourceEpoch,frameBefore:before.frameBefore,frameAfter:before.frameAfter,saveSucceeded:true,
      idbfsSyncSucceeded:true,path:capability.savePath,sha256:digest,bytes:raw.byteLength,format:capability.format,
      parsed:parsed.config??parsed,inputCapacityFrames,stage:recordingStage};
    caseRecord.p05Recording=recording;await save();

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
    const native=await capability.enterMenu({page,frame,target,runtime:titleAdapter.runtime,practice,
      readRuntimeObservation,runtimeObservationRequest,stage,importPath,caseRecord,save,sourcePractice,epoch:freshEpoch});
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
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','variant']});
  assert.equal(option('variant')??'normal','normal','Practice menu driver requires an ordinary Runtime');
  assert.ok(option('manifest')&&option('game'),'Required: --manifest=private-inputs.json --game=explicit-selection');
  const config=JSON.parse(await readFile(resolve(option('manifest')),'utf8'));
  const selection=selectRuntimeTargets(config,{game:option('game'),variant:'normal'});
  const missingPracticeAdapters=selection.targets.filter(target=>PRODUCT_GAMES[target.game].features.thprac===true&&
    !resolveAdaptationAdapter(target.game,target.variant)?.practice);
  if(missingPracticeAdapters.length)throw new Error(`Selected practice products need a native practice adapter before browser launch: ${missingPracticeAdapters.map(target=>target.id).join(', ')}`);
  Object.assign(report,{selection:{games:selection.games,variant:'normal',expectedTargets:selection.expected},
    scope:'Ordinary Runtime | title-declared native practice selection and parameter consumption | native practice controls | locale screenshots for review | declared Replay roundtrip capabilities',
    coverage:{ids:Array.from(new Set(selection.games.flatMap(game=>practiceAdaptationIds(game,'normal')))),complete:false,
      scope:'Four isolated locale Runtime epochs per selected target. A registered title capability supplies its native menu path, ABI and parameter case. Replay roundtrip operations run when the selected capability declares them.'},targets:[]});
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
        const manifest=JSON.parse(await readVerifiedRuntimeFile(target,record.identity.runtime,'manifest.json'));
        record.feature=practiceBuildFeature(manifest,PRODUCT_GAMES[target.game]);
        if(!record.feature){
          if(PRODUCT_GAMES[target.game].features.thprac){
            record.inputIssue='practice-enabled-package-required';
            throw Error('Practice behavior lane requires a practice-enabled package; the selected valid package has practice disabled');
          }
          record.status='not-applicable';record.reason='This product does not support practice';continue;
        }
        for(const [epoch,enabled,locale] of [[1,true,'ja-JP'],[2,true,'en-US'],[3,true,'zh-CN'],[4,false,'en-US']]){
          const caseRecord={epoch,enabled,locale,status:'running'};
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
          const actualPracticeCase=Boolean(practiceCapability)&&enabled&&epoch===1;
          await stage('configure',{epoch,locale,enabled,actualPracticeCase},()=>page.evaluate(({sharedResources,enabled,locale,actualPracticeCase,practiceParams})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,
            thpracCatalog:[],options:{touchEnabled:false,alwaysHitbox:false,thpracEnabled:enabled,thpracLocale:locale,limitPresentationTo60:false,
              ...(actualPracticeCase?{thprac:{params:practiceParams}}:{})}}),{sharedResources,enabled,locale,actualPracticeCase,
                practiceParams:actualPracticeCase?practiceCapability.parameterCase:null}));
          await stage('launch',{epoch},()=>page.evaluate(()=>rpc('launch')));
          await stage('first-frame',{epoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='first-frame'&&event.epoch===epoch),epoch,{timeout:120000}));
          await responseProbe.flush();
          await frame.locator('canvas').first().focus();
          const observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
          const ready=state=>titleFlow.isGameplayReady(state,'practice');
          const waitUntil=async(label,predicate,timeoutMs=30000)=>{
            const started=Date.now();let state;
            do{state=await observe();if(predicate(state))return state;await page.waitForTimeout(75);}
            while(Date.now()-started<timeoutMs);
            throw new Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);
          };
          const press=async code=>{const key=code==='KeyZ'?'z':code;await page.keyboard.press(key,{delay:90});await page.waitForTimeout(140);};
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
              read:readPractice,waitUntil:waitPracticeUntil,press,waitForGameplay:()=>waitUntil('native practice gameplay ready',ready),
              parameterCase:practiceCapability.parameterCase,selectionCase:practiceCapability.selectionCase}),
              assess:snapshot=>practiceCapability.assessConsumption(snapshot,practiceCapability.parameterCase,practiceCapability.selectionCase)});
            caseRecord.practiceSnapshot=entered.practice;
            caseRecord.parameterConsumption=entered.parameterConsumption;
            caseRecord.featureInventory=entered.featureInventory;
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
          await page.keyboard.press('Backspace',{delay:100});
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
            await page.keyboard.press('Backspace',{delay:100});
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
                sourceWaitPracticeUntil:waitPracticeUntil,sourcePractice:caseRecord.practiceSnapshot,caseRecord,record,
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
        assert.equal(record.locales.length,4,'Every enabled locale and the disabled control must run');
        assert.ok(record.locales.every(item=>item.status==='passed'),'A practice locale or disabled control failed; see its saved native state');
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
        record.checks.push(...practiceAdaptationChecks({game:target.game,variant:target.variant,
          productSupported:PRODUCT_GAMES[target.game].features.thprac,feature:record.feature,locales:record.locales,
          finalErrorAudit:record.finalErrorAudit,
          practiceReplayRoundTrip:record.locales.find(item=>item.epoch===1)?.p05}));
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
  if(!report.passed&&report.finalErrorAudit.status==='failed')report.status='failed';
});
if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
