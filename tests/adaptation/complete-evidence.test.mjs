import assert from 'node:assert/strict';
import test from 'node:test';
import {createHash} from 'node:crypto';
import {assessCompleteEvidence} from './complete-evidence.mjs';
import {assessDriverReport} from './results.mjs';
import {assessMinimalRuntimeJourney} from '../../tools/ports/minimal-runtime-browser.mjs';
import {resolvePracticeAdapter} from './adapters/registry.mjs';
import {TH08_PRACTICE_FRESH_CONFIG} from './adapters/titles/th08-practice.mjs';

const HASH='a'.repeat(64),DATA={bytes:10,sha256:HASH};
const product={game:'th08',variant:'normal'};
const envelope=(id,observation,game=product.game,adaptationIds=[id])=>({
  game,variant:'normal',adaptationIds,observation,
});
const clone=value=>structuredClone(value);
function boundary(id,generationId,game=product.game){
  const before={generationId,revision:'rev-1',files:{'game-data':{objectId:'data-object',revision:HASH.slice(0,16),storageMode:'arraybuffer'}},
    objects:{'data-object':{...DATA}}};
  return envelope(id,{before:clone(before),after:clone(before),dataRead:{game,epoch:1,generation:generationId,
    fileId:'game-data',...DATA},expectedData:{...DATA},writes:[]},game);
}
function uiImport(id,game=product.game){
  return envelope(id,{path:`C:/artifacts/${game}.zip`,source:'Launcher #gamePackageImport UI',store:'IndexedDB Package Store',
    generation:{generationId:'gen-online',revision:'rev-1',gameVersion:'original',fileId:'game-data',target:`/${game}.data`,
      ...DATA,declaredBytes:DATA.bytes,declaredSha256:DATA.sha256}},game);
}
function negativeBoundary(id,generationId='gen-final',game=product.game){
  const actual=boundary(id,generationId,game).observation;
  actual.writes=[{operation:'transaction',database:'eagler-touhou-package-store-v1',stores:['objects'],mode:'readwrite'}];
  return envelope(id,{probe:{database:'eagler-touhou-package-store-v1',store:'objects',mode:'readwrite'},
    actual,status:'ignored-by-assessor',rejectedByUnchangedAssertion:true},game);
}
function packageImportRefs(id,game=product.game){
  return{
    '/uiImport':uiImport(id,game),
    '/resourceBoundaries/online':boundary(id,'gen-online',game),
    '/resourceBoundaries/offline':boundary(id,id==='RUNTIME-05'?'gen-final':'gen-offline',game),
    '/resourceBoundaries/onlineEnd':boundary(id,'gen-online',game),
    '/resourceBoundaries/finalOnlineEnd':boundary(id,'gen-final',game),
    '/resourceBoundaryNegative':negativeBoundary(id,'gen-final',game),
  };
}
function runtimeObservation(generationId,online,game=product.game){
  const params=new URLSearchParams({hosted:'1',managedData:'1',gameGeneration:generationId,runtimeVariant:'normal',runtimeEpoch:'1'});
  const dataRead={game,epoch:1,generation:generationId,fileId:'game-data',...DATA};
  return{frameUrl:`https://127.0.0.1/runtime/${game}.html?${params}`,query:Object.fromEntries(params),
    ready:{protocol:'eagler-touhou/1',game,epoch:1,event:'ready'},
    first:{protocol:'eagler-touhou/1',game,epoch:1,event:'first-frame'},
    pageOnline:online,workerControlled:true,dataRead,dataReads:[clone(dataRead)]};
}
function postflight(id,game=product.game){
  return envelope(id,{privateData:{...DATA},offlinePackage:{status:'ignored-by-assessor',data:{...DATA},
    package:{path:`C:/artifacts/${game}.zip`,archiveBytes:100,archiveSha256:'b'.repeat(64)}},
    normalRuntimePackage:{game,variant:'normal',entry:`${game}.html`,wasmSha256:'c'.repeat(64),manifestSha256:'d'.repeat(64)}},game);
}
function offlineCheck(id,refs,game=product.game){
  const entry={id,complete:true,status:'passed',evidenceRefs:Object.keys(refs)};
  const driver={schema:'eagler-touhou/offline-runtime-browser/1',scope:{game,variant:'normal'},checks:[entry]};
  for(const [pointer,value] of Object.entries(refs)){
    const parts=pointer.slice(1).split('/');let target=driver;
    for(const part of parts.slice(0,-1))target=target[part]??=(Object.create(null));
    target[parts.at(-1)]=value;
  }
  return{driver,entry};
}
function diagnosticPositive(lanes=['presentation']){
  const labels=['title-idle','title-animate','menu-motion','negative-control','negative-recovered',
    'stage1-motion','stage1-negative','stage1-recovered','pause-menu'];
  const reports=labels.map((label,index)=>{
    const positive=!['negative-control','stage1-negative'].includes(label);
    return{label,tick:index+10,previousTick:index+9,consecutive:true,gate:true,
      purityStatus:positive?'pass':'fail',purity:positive,worldFrozen:label==='pause-menu',valid:true,
      interpolationReady:true,stateAuthority:{complete:true,missingDomains:[],invalidDomains:[],
        unmappedGroups:[],uncoveredFields:[]},objects:[{key:`object-${index}`}],stateChanges:[]};
  });
  const entry={id:'DISPLAY-03',lane:'presentation',complete:true,status:'passed',
    observation:envelope('DISPLAY-03',{kind:'presentation-lab-native-capture',
      artifact:{path:'C:/captures/capture.json',bytes:100,sha256:'e'.repeat(64)},
      build:{schema:'th08/presentation-lab-build/1',commit:'test-commit',wasm:'f'.repeat(64),loader:'1'.repeat(64),
        sourceDigest:'2'.repeat(64),toolDigest:'3'.repeat(64),toolSources:{'analyzer.mjs':'4'.repeat(64)},
        instrumented:true,dataAvailable:true},reports,errors:[]})};
  const driver={schema:'eagler-touhou/diagnostic-browser/1',scope:{game:'th08',variant:'normal',lanes},
    targets:[{game:'th08',variant:'normal',checks:[entry]}]};
  return{driver,entry};
}
function rawReplay(){
  const payload={version:'2.6.1.0',game:'th08',mode:1,stage:0,life:4,bomb:3,power:96,gauge:10000,
    score:1234560,graze:23,point_total:234,point_stage:345,time:321,value:43210,night:4,familiar:17,rank:10,rankLock:true};
  const json=Buffer.from(JSON.stringify(payload)),length=(12+json.length+3)&~3,bytes=Buffer.alloc(0x68+length);
  bytes.writeUInt32LE(0x50523854,0);bytes.writeUInt16LE(6,4);bytes.writeUInt32LE(0x68,12);
  bytes.write('USER',0x68,'ascii');bytes.writeUInt32LE(length,0x6c);bytes.write('PRAC',0x70,'ascii');json.copy(bytes,0x74);
  return bytes;
}

function buildEvidenceFixture({game='th08',variant='normal'}={}){
  const now='2026-10-21T00:00:00.000Z',later='2026-10-21T00:00:01.000Z';
  const digest=letter=>letter.repeat(64);
  const sourceInvocation={script:'launcher/scripts/build-runtime.mjs',startedAt:now,finishedAt:later,exitCode:0,
    argumentSha256:digest('a')};
  const packageInvocation={script:'launcher/scripts/package-runtime.mjs',startedAt:now,finishedAt:later,exitCode:0,
    argumentSha256:digest('b')};
  const currentSource={invocation:sourceInvocation,plan:{path:'artifacts/validation/source-builds/th08-normal.json',
    sha256:digest('c'),planSha256:digest('d'),game,variant,owner:game,source:`ports/${game}`,common:'common',
    toolchain:{version:'emsdk-34',compiler:'emcc',root:'toolchains/emsdk'},features:{},requiredSubmodules:[]},
    receipt:{path:'artifacts/validation/source-builds/th08-normal.runtime.json',sha256:digest('e'),
      schema:'eagler-touhou/runtime-source-build/3',sourceInputsSha256:digest('f'),sourceFilesSha256:digest('1'),
      executionSha256:digest('2'),planSha256:digest('d'),outputCount:3,sourceCount:44}};
  const target={game,variant,owner:game,sourceRoot:`ports/${game}`,buildPlan:currentSource.plan.path,
    buildReceipt:currentSource.receipt.path,packageReceipt:'artifacts/validation/runtime-packages/th08-normal.json',
    runtime:'build-eagler',entry:'th08.html',packageSha256:digest('3')};
  const currentPackage={invocation:packageInvocation,package:{path:target.packageReceipt,output:target.runtime,
    runtimeRoot:'D:/build-eagler',receiptSha256:digest('4'),schema:'eagler-touhou/runtime-package-build/1',
    packageSha256:target.packageSha256,wasmSha256:currentSource.receipt.executionSha256,manifestSha256:digest('5'),
    sourceBuildReceiptSha256:currentSource.receipt.sha256,sourceInputsSha256:currentSource.receipt.sourceInputsSha256,
    fileCount:12,entry:target.entry}};
  const rawStage=(stage,current,invocation,ids,runId)=>{
    const report={schema:'eagler-touhou/build-acceptance/1',runId,startedAt:now,finishedAt:later,status:'passed',passed:true,
      scope:{game,variant},stage,invocations:[invocation],
      observations:{[stage==='source-build'?'sourceBuild':'package']:current},
      checks:ids.map(id=>({id,status:'passed',complete:false,scope:`${game}/${variant}`,
        evidence:{game,variant,adaptationIds:[id],observation:{fixture:true}}}))};
    const rawJson=JSON.stringify(report,null,2)+'\n',bytes=Buffer.from(rawJson);
    return{report:{path:`${stage}.json`,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),rawJson,
      runId,startedAt:now,finishedAt:later,stage,invocation},current};
  };
  const sourceRaw=rawStage('source-build',currentSource,sourceInvocation,['BUILD-01','BUILD-02','BUILD-03'],'source-run');
  const packageRaw=rawStage('package',currentPackage,packageInvocation,['BUILD-04'],'package-run');
  const sourceBuild={producer:sourceRaw.report,current:sourceRaw.current};
  const packageStage={producer:packageRaw.report,current:packageRaw.current};
  const verification={mode:'revalidated-existing-stage-reports',checkedAt:'2026-10-21T00:00:00.500Z'};
  const revalidation={manifest:{path:'runtime.json',sha256:digest('6')},target,
    sourceBuild:{report:sourceBuild.producer,observation:sourceBuild.current},
    package:{report:packageStage.producer,observation:packageStage.current},verification};
  const checks=['BUILD-01','BUILD-02','BUILD-03','BUILD-04'].map(id=>({id,status:'passed',complete:true,
    scope:`${game}/${variant}`,evidence:{game,variant,adaptationIds:[id],observation:{target,sourceBuild,
      ...(id==='BUILD-04'?{package:packageStage}:{}),claim:{'BUILD-01':'selected-source-owner','BUILD-02':'current-build-plan-and-toolchain',
        'BUILD-03':'current-source-build-output','BUILD-04':'current-package-and-source-binding'}[id]}}}));
  const driver={schema:'eagler-touhou/build-acceptance/1',runId:'verify-run',startedAt:now,finishedAt:later,
    status:'passed',passed:true,stage:'verify-existing',scope:{game,variant},checks,observations:{revalidation}};
  return{driver,checks,target,sourceBuild,packageStage,currentSource,currentPackage};
}
function minimalRuntimePositive(){
  const product={game:'th08',variant:'normal'},build=buildEvidenceFixture();
  build.driver.stage='verify-existing';
  const checkout={schema:'eagler-touhou/build-acceptance/1',runId:'checkout-run',
    startedAt:'2026-10-20T23:59:00.000Z',finishedAt:'2026-10-20T23:59:01.000Z',status:'incomplete',passed:false,
    stage:'checkout',scope:product,observations:{checkout:{status:'passed',targetSourceRoot:'ports/th08',
      unexpectedOtherProductOwners:[],absentOtherProductOwners:['ports/th10']}}};
  const refs=packageImportRefs('RUNTIME-05'),offlineResult=offlineCheck('RUNTIME-05',refs);
  const offline=Object.assign(offlineResult.driver,{runId:'offline-run',startedAt:'2026-10-20T23:59:02.000Z',
    finishedAt:'2026-10-20T23:59:03.000Z',status:'passed',passed:true,
    runtimePackages:{normal:{packageSha256:build.target.packageSha256}},
    onlineRuntime:{observation:runtimeObservation('gen-final',true)}});
  const journey={checkout,build:build.driver,offline};
  const entry=assessMinimalRuntimeJourney({...product,...journey,assessEvidence:assessCompleteEvidence});
  const driver={schema:'eagler-touhou/minimal-runtime-browser/1',runId:'minimal-run',
    startedAt:'2026-10-20T23:59:00.000Z',finishedAt:'2026-10-21T00:00:00.000Z',status:'passed',passed:true,
    scope:product,checks:[entry]};
  return{driver,entry,product};
}
function practiceAssessorInput(){
  const capability=resolvePracticeAdapter('th08','normal'),replay=capability.replayRoundTrip;
  const raw=rawReplay(),config=replay.parse(raw),stage=config.config.stage;
  const digest=createHash('sha256').update(raw).digest('hex'),bytes=Array.from(raw),configCopy=structuredClone(config.config);
  const applied={score:123456,displayScore:123456,lives:4,bombs:3,power:96,gauge:10000,gaugeCopy:10000,
    graze:23,grazeStage:23,points:234,pointsStage:345,timeOrbs:321,totalTimeOrbs:321,pointValue:43210,
    clockTime:4,rank:10,rankMinimum:10,rankMaximum:10,familiarCount:17,familiarPending:true,
    stage,difficulty:1,character:2};
  return{
    recording:{epoch:1,frameBefore:7,frameAfter:11,saveSucceeded:true,idbfsSyncSucceeded:true,
      path:replay.savePath,rawBytes:bytes,sha256:digest,inputCapacityFrames:4},
    persisted:{rawBytes:[...bytes],freshEpochSha256:digest},
    import:{beforeLaunch:true,writeSucceeded:true,listed:true,path:replay.importPath('p05a'),rawBytes:[...bytes],
      sha256:digest,readSha256:digest},
    fresh:{epoch:5,configured:{...TH08_PRACTICE_FRESH_CONFIG}},
    menu:{candidate:configCopy},
    playback:{selectedGame:{...replay.selectionCase},initialSnapshot:{
      schema:'eagler-touhou/th08-practice-initial-snapshot/1',validMask:3,
      state:{stage,gameFlags:8,difficulty:replay.selectionCase.difficulty,character:replay.selectionCase.character,replay:true},
      run:configCopy,applied:{...applied},
      cursor:{stageMask:1<<stage,frame:0,inputCursor:0,timingCursor:0,inputBytes:6,timingBytes:2,inputEnd:6,timingEnd:2},
      source:'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs'},
      liveSnapshot:{state:{active:true,replay:true,inGame:true},run:configCopy,
        applied:{...applied,score:123466,displayScore:123466}},
      stream:{available:true,loaded:true,stage,gameFlags:8,frame:2,inputCursor:4,timingCursor:1,inputBytes:6,timingBytes:2}},
  };
}
function practicePositive(){
  const capability=resolvePracticeAdapter('th08','normal'),replay=capability.replayRoundTrip;
  const evidence=practiceAssessorInput();
  const result=replay.assess(evidence,{selectionCase:replay.selectionCase??capability.selectionCase});
  const entry={id:'PRACTICE-05',adaptationIds:['PRACTICE-05'],status:'observed',complete:true,
    observation:envelope('PRACTICE-05',{complete:true,result:{...result,observationInput:{
      schema:'eagler-touhou/practice-replay-input/1',format:replay.format,evidence}}})};
  const driver={schema:'eagler-touhou/practice-browser/1',scope:'practice scene',
    targets:[{game:'th08',variant:'normal',checks:[entry]}]};
  return{driver,entry};
}
function relabelProduct(value,from,to){
  if(Array.isArray(value))return value.map(item=>relabelProduct(item,from,to));
  if(!value||typeof value!=='object')return value;
  return Object.fromEntries(Object.entries(value).map(([key,child])=>[key,
    key==='game'&&child===from?to:key==='target'&&child===`/${from}.data`?`/${to}.data`:
      key==='entry'&&child===`${from}.html`?`${to}.html`:relabelProduct(child,from,to)]));
}

test('unknown complete claims and note placeholders fail closed',()=>{
  const unknown={id:'INPUT-01',complete:true,status:'passed',observation:{note:'checked'}};
  const result=assessCompleteEvidence({driver:{schema:'eagler-touhou/runtime-browser/1',checks:[unknown]},entry:unknown,product});
  assert.equal(result.available,false);assert.equal(result.valid,false);

  const known=diagnosticPositive();
  known.entry.observation=envelope('DISPLAY-03',{note:'checked'});
  const invalid=assessCompleteEvidence({driver:known.driver,entry:known.entry,product});
  assert.equal(invalid.available,true);assert.equal(invalid.valid,false);

  const normalizedOnly=practicePositive();
  delete normalizedOnly.entry.observation.observation.result.observationInput;
  const unavailable=assessCompleteEvidence({driver:normalizedOnly.driver,entry:normalizedOnly.entry,product});
  assert.equal(unavailable.available,false);assert.equal(unavailable.valid,false);
});

test('known IDs cannot borrow another driver, product mode, identity, or incomplete reference set',()=>{
  const presentation=diagnosticPositive();
  const wrongDriver=assessCompleteEvidence({driver:{...presentation.driver,schema:'eagler-touhou/practice-browser/1'},
    entry:presentation.entry,product});
  assert.equal(wrongDriver.available,false);assert.equal(wrongDriver.valid,false);

  const refs=packageImportRefs('PACKAGE-02');
  const missing=offlineCheck('PACKAGE-02',{'/offlinePackagePreflight':envelope('PACKAGE-02',{}),
    '/uiImport':refs['/uiImport'],'/onlineRuntime':envelope('PACKAGE-02',runtimeObservation('gen-online',true)),
    '/postflight':postflight('PACKAGE-02')});
  const missingResult=assessCompleteEvidence({driver:missing.driver,entry:missing.entry,product});
  assert.equal(missingResult.available,true);assert.equal(missingResult.valid,false);

  const wrongIdentity=practicePositive();
  wrongIdentity.entry.observation.variant='multiplayer';
  const identityResult=assessCompleteEvidence({driver:wrongIdentity.driver,entry:wrongIdentity.entry,product});
  assert.equal(identityResult.available,true);assert.equal(identityResult.valid,false);

  const mpProduct=assessCompleteEvidence({driver:presentation.driver,entry:presentation.entry,
    product:{game:'th08',variant:'multiplayer'}});
  assert.equal(mpProduct.available,false);assert.equal(mpProduct.valid,false);

  const unsupportedPresentation=assessCompleteEvidence({driver:presentation.driver,entry:presentation.entry,
    product:{game:'th06',variant:'normal'}});
  assert.equal(unsupportedPresentation.available,false);
  const practice=practicePositive();
  const unsupportedPractice=assessCompleteEvidence({driver:practice.driver,entry:practice.entry,
    product:{game:'th06',variant:'normal'}});
  assert.equal(unsupportedPractice.available,false);
});

test('DISPLAY-03 requires the profile-declared complete presentation set',()=>{
  const {driver,entry}=diagnosticPositive();
  assert.deepEqual(assessCompleteEvidence({driver,entry,product}),
    {available:true,valid:true,reason:null,kind:'presentation-purity'});
  const partial=diagnosticPositive();
  partial.entry.observation.observation.reports=partial.entry.observation.observation.reports.slice(0,3);
  assert.equal(assessCompleteEvidence({driver:partial.driver,entry:partial.entry,product}).valid,false);
  const combined=diagnosticPositive(['presentation','quick','daily']);
  assert.equal(assessCompleteEvidence({driver:combined.driver,entry:combined.entry,product}).valid,true);
  const duplicate=diagnosticPositive(['presentation','quick','presentation']);
  assert.equal(assessCompleteEvidence({driver:duplicate.driver,entry:duplicate.entry,product}).valid,false);
});

test('offline claims are shared across registered products and recompute boundary evidence',()=>{
  const th06='th06',th06Product={game:th06,variant:'normal'},p1=packageImportRefs('PACKAGE-01',th06);
  const refs=relabelProduct({'/uiImport':p1['/uiImport'],'/resourceBoundaries/online':p1['/resourceBoundaries/online'],
    '/resourceBoundaries/offline':p1['/resourceBoundaries/offline']},'th08',th06);
  const run=offlineCheck('PACKAGE-01',refs,th06);
  assert.equal(assessCompleteEvidence({driver:run.driver,entry:run.entry,product:th06Product}).valid,true);

  const runtime=packageImportRefs('RUNTIME-05');
  const changed=clone(runtime['/resourceBoundaries/online']);
  changed.observation.writes=[{operation:'put'}];
  const invalidRuntime=offlineCheck('RUNTIME-05',{'/uiImport':runtime['/uiImport'],
    '/resourceBoundaries/online':changed,'/resourceBoundaries/onlineEnd':runtime['/resourceBoundaries/onlineEnd'],
    '/resourceBoundaries/finalOnlineEnd':runtime['/resourceBoundaries/finalOnlineEnd'],
    '/resourceBoundaries/offline':runtime['/resourceBoundaries/offline'],
    '/resourceBoundaryNegative':runtime['/resourceBoundaryNegative']});
  assert.equal(assessCompleteEvidence({driver:invalidRuntime.driver,entry:invalidRuntime.entry,product}).valid,false);

  const rRuntime=packageImportRefs('RUNTIME-05');
  const rRefs={'/uiImport':rRuntime['/uiImport'],'/resourceBoundaries/online':rRuntime['/resourceBoundaries/online'],
    '/resourceBoundaries/onlineEnd':rRuntime['/resourceBoundaries/onlineEnd'],
    '/resourceBoundaries/finalOnlineEnd':rRuntime['/resourceBoundaries/finalOnlineEnd'],
    '/resourceBoundaries/offline':rRuntime['/resourceBoundaries/offline'],
    '/resourceBoundaryNegative':rRuntime['/resourceBoundaryNegative']};
  const rRun=offlineCheck('RUNTIME-05',rRefs);
  assert.equal(assessCompleteEvidence({driver:rRun.driver,entry:rRun.entry,product}).valid,true);

  const p2=packageImportRefs('PACKAGE-02');
  const packageRef=envelope('PACKAGE-02',{game:'th08',source:'offline-package',expectedData:{...DATA},
    package:{path:'C:/artifacts/th08.zip',archiveBytes:100,archiveSha256:'b'.repeat(64),
      descriptorRevision:'rev-1',dataFileId:'game-data',dataBytes:DATA.bytes,dataSha256:DATA.sha256}});
  const p2Run=offlineCheck('PACKAGE-02',{'/offlinePackagePreflight':packageRef,'/uiImport':p2['/uiImport'],
    '/onlineRuntime':envelope('PACKAGE-02',runtimeObservation('gen-online',true)),
    '/postflight':postflight('PACKAGE-02')});
  assert.equal(assessCompleteEvidence({driver:p2Run.driver,entry:p2Run.entry,product}).valid,true);

  const p5Run=offlineCheck('PACKAGE-05',{'/appShell':envelope('PACKAGE-05',{scope:'https://127.0.0.1/',
    worker:'https://127.0.0.1/app-shell-sw.js',controlled:true,controlledBeforeImport:true}),
    '/offlineRuntime':envelope('PACKAGE-05',runtimeObservation('gen-offline',false)),
    '/postflight':postflight('PACKAGE-05')});
  assert.equal(assessCompleteEvidence({driver:p5Run.driver,entry:p5Run.entry,product}).valid,true);
});

test('PRACTICE-05 reruns its declared title assessor on retained source evidence',()=>{
  const {driver,entry}=practicePositive();
  assert.equal(assessCompleteEvidence({driver,entry,product}).valid,true);
  const truncated=practicePositive();
  truncated.entry.observation.observation.result.observationInput.evidence.recording.rawBytes[0]=0;
  const invalid=assessCompleteEvidence({driver:truncated.driver,entry:truncated.entry,product});
  assert.equal(invalid.available,true);assert.equal(invalid.valid,false);
  assert.match(invalid.reason,/Replay|roundtrip|PRACTICE/i);
});

test('BUILD-01 through BUILD-04 recompute current stage evidence and bind package to the same source receipt',()=>{
  const fixture=buildEvidenceFixture(),product={game:'th08',variant:'normal'};
  for(const entry of fixture.checks){
    const result=assessCompleteEvidence({driver:fixture.driver,entry,product});
    assert.equal(result.available,true,entry.id);assert.equal(result.valid,true,result.reason);
  }
  const assessed=assessDriverReport({game:'th08',variant:'normal',group:'build',checksMapped:
    ['BUILD-01','BUILD-02','BUILD-03','BUILD-04'],scenarioIds:['revalidate-current-runtime-build-and-package']},
  {code:0,status:'passed',startedAt:'2026-10-20T23:59:59.000Z'},fixture.driver);
  assert.equal(assessed.status,'passed');assert.ok(assessed.checks.every(check=>check.complete));

  const multiplayer=buildEvidenceFixture({game:'th08',variant:'multiplayer'}),mpProduct={game:'th08',variant:'multiplayer'};
  for(const entry of multiplayer.checks){
    const result=assessCompleteEvidence({driver:multiplayer.driver,entry,product:mpProduct});
    assert.equal(result.available,true,entry.id);assert.equal(result.valid,true,result.reason);
  }

  const mutation=(id,edit)=>{
    const entry=fixture.checks.find(item=>item.id===id),copy=clone(fixture.driver);
    const copied=copy.checks.find(item=>item.id===id);edit(copied,copy);
    return assessCompleteEvidence({driver:copy,entry:copied,product});
  };
  assert.equal(mutation('BUILD-03',entry=>{entry.complete=true;entry.evidence.observation.sourceBuild.producer.sha256='0'.repeat(64);}).valid,false,
    'changing a producer report identity without changing its raw bytes is rejected');
  assert.equal(mutation('BUILD-04',entry=>{entry.evidence.observation.package.current.package.sourceBuildReceiptSha256='9'.repeat(64);}).valid,false,
    'a package bound to a different source receipt is rejected');
  assert.equal(mutation('BUILD-02',entry=>{entry.evidence.observation.sourceBuild.producer.rawJson+=' ';}).valid,false,
    'the report byte hash is recalculated from retained raw bytes');
  assert.equal(mutation('BUILD-01',entry=>{entry.evidence.game='th09';}).valid,false,
    'a build claim cannot be relabelled to another product');
  assert.equal(mutation('BUILD-04',entry=>{entry.evidence.observation.package.producer.stage='source-build';}).valid,false,
    'a package claim cannot use a source-build stage report');
  const normalBorrow=assessCompleteEvidence({driver:fixture.driver,entry:fixture.checks[0],
    product:{game:'th08',variant:'multiplayer'}});
  assert.equal(normalBorrow.valid,false,'normal evidence cannot satisfy MP');
  const mpBorrow=assessCompleteEvidence({driver:multiplayer.driver,entry:multiplayer.checks[0],
    product:{game:'th08',variant:'normal'}});
  assert.equal(mpBorrow.valid,false,'MP evidence cannot satisfy normal');
  const unsupportedMode=assessCompleteEvidence({driver:multiplayer.driver,entry:multiplayer.checks[0],
    product:{game:'th20',variant:'multiplayer'}});
  assert.equal(unsupportedMode.available,false,'a product without an MP Runtime cannot borrow the build contract');
});

test('BUILD-05 recomputes the nested checkout, BUILD-01..04 and RUNTIME-05 evidence',()=>{
  const fixture=minimalRuntimePositive();
  const result=assessCompleteEvidence(fixture);
  assert.deepEqual(result,{available:true,valid:true,reason:null,kind:'minimal-runtime-installation'});
  const normalized=assessDriverReport({game:'th08',variant:'normal',group:'single-game',
    checksMapped:['BUILD-05'],scenarioIds:['single-game-checkout-and-launcher']},
  {code:0,status:'passed',startedAt:'2026-10-20T23:58:59.000Z'},fixture.driver);
  assert.equal(normalized.status,'passed');assert.equal(normalized.checks[0].complete,true);
});

test('BUILD-05 rejects a bare completion flag, missing nested payload or mismatched Runtime generation',()=>{
  const fixture=minimalRuntimePositive();
  const bare={id:'BUILD-05',status:'passed',complete:true,
    evidence:{game:'th08',variant:'normal',adaptationIds:['BUILD-05'],observation:{note:'checked'}}};
  const noJourney={...fixture.driver,checks:[bare]};
  assert.equal(assessCompleteEvidence({driver:noJourney,entry:bare,product:fixture.product}).valid,false);

  const missing=minimalRuntimePositive();delete missing.entry.evidence.observation.journey;
  assert.equal(assessCompleteEvidence(missing).valid,false);

  const wrongGeneration=minimalRuntimePositive();
  wrongGeneration.entry.evidence.observation.journey.offline.runtimePackages.normal.packageSha256='9'.repeat(64);
  const invalid=assessCompleteEvidence(wrongGeneration);
  assert.equal(invalid.available,true);assert.equal(invalid.valid,false);
});
