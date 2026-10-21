import {createHash} from 'node:crypto';
import {isDeepStrictEqual} from 'node:util';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {resolveDiagnosticProfile} from './adapters/diagnostics.mjs';
import {resolvePracticeAdapter} from './adapters/registry.mjs';
import {assessPackageInstallationAuthority,classifyPackageAuthorityPrompt} from '../../tools/ports/offline-installation-authority.mjs';
import {inspectSourceReviewNow} from '../../tools/ports/adaptation-source-review.mjs';
import {assessMinimalRuntimeJourney} from '../../tools/ports/minimal-runtime-browser.mjs';
import {assessRuntimeInputEvidence} from './adapters/input-evidence.mjs';
import {assessPresentationEvidence} from './adapters/presentation-evidence.mjs';

const HASH=/^[a-f0-9]{64}$/u;
const REQUIRED_REFS=Object.freeze({
  'RUNTIME-05':[
    '/uiImport','/resourceBoundaries/online','/resourceBoundaries/onlineEnd',
    '/resourceBoundaries/finalOnlineEnd','/resourceBoundaries/offline','/resourceBoundaryNegative',
  ],
  'PACKAGE-01':[
    '/uiImport','/resourceBoundaries/online','/resourceBoundaries/offline',
  ],
  'PACKAGE-02':[
    '/offlinePackagePreflight','/uiImport','/onlineRuntime','/postflight',
  ],
  'PACKAGE-05':['/appShell','/offlineRuntime','/postflight'],
  'PACKAGE-06':['/packageAuthority','/uiImport','/onlineRuntime','/offlineRuntime'],
});
const CONTRACTS=Object.freeze({
  'BUILD-01':{kind:'source-owner',schema:'eagler-touhou/build-acceptance/1',owner:'root',variants:['normal','multiplayer']},
  'BUILD-02':{kind:'build-plan-toolchain',schema:'eagler-touhou/build-acceptance/1',owner:'root',variants:['normal','multiplayer']},
  'BUILD-03':{kind:'source-build-output',schema:'eagler-touhou/build-acceptance/1',owner:'root',variants:['normal','multiplayer']},
  'BUILD-04':{kind:'runtime-package-source-binding',schema:'eagler-touhou/build-acceptance/1',owner:'root',variants:['normal','multiplayer']},
  'BUILD-05':{kind:'minimal-runtime-installation',schema:'eagler-touhou/minimal-runtime-browser/1',owner:'root',variants:['normal']},
  'BUILD-06':{kind:'reviewed-source-provenance',schema:'eagler-touhou/adaptation-source-review-result/1',owner:'root',variants:['normal','multiplayer']},
  'VERIFY-05':{kind:'accepted-original-reference',schema:'eagler-touhou/adaptation-source-review-result/1',owner:'root',variants:['normal','multiplayer']},
  'DISPLAY-03':{kind:'presentation-purity',schema:'eagler-touhou/diagnostic-browser/1',owner:'target'},
  'RUNTIME-05':{kind:'package-store-boundary',schema:'eagler-touhou/offline-runtime-browser/1',owner:'root'},
  'PACKAGE-01':{kind:'installed-package-readability',schema:'eagler-touhou/offline-runtime-browser/1',owner:'root'},
  'PACKAGE-02':{kind:'offline-package-identity',schema:'eagler-touhou/offline-runtime-browser/1',owner:'root'},
  'PACKAGE-05':{kind:'offline-first-frame',schema:'eagler-touhou/offline-runtime-browser/1',owner:'root'},
  'PACKAGE-06':{kind:'package-installation-authority',schema:'eagler-touhou/offline-runtime-browser/1',owner:'root',variants:['normal']},
  'PRACTICE-05':{kind:'practice-replay-roundtrip',schema:'eagler-touhou/practice-browser/1',owner:'target'},
});

function fail(message){throw new Error(message);}
function requireValue(condition,message){if(!condition)fail(message);}
function record(value){return value!==null&&typeof value==='object'&&!Array.isArray(value);}
function nonempty(value){return typeof value==='string'&&value.trim().length>0;}
function positiveInt(value){return Number.isSafeInteger(value)&&value>0;}
function validDigest(value){return typeof value==='string'&&HASH.test(value);}
function idsContain(value,id){return Array.isArray(value)&&value.includes(id);}
function equal(actual,expected,message){requireValue(isDeepStrictEqual(actual,expected),message);}
function sameData(actual,expected,message){
  requireValue(record(actual)&&record(expected)&&positiveInt(actual.bytes)&&validDigest(actual.sha256)&&
    actual.bytes===expected.bytes&&actual.sha256===expected.sha256,message);
}
function supportsContractVariant(contract,product){
  const definition=PRODUCT_GAMES[product?.game];
  return !!definition&&(contract.variants??['normal']).includes(product?.variant)&&
    (product.variant!=='multiplayer'||typeof definition.multiplayerRuntime==='string');
}

function reportOwnsEntry(driver,entry,product,contract){
  const identityMatches=value=>value?.game===product.game&&value?.variant===product.variant;
  if(record(driver.scope)&&driver.scope.game!==undefined)
    requireValue(identityMatches(driver.scope),'Driver scope belongs to another product or variant');
  if(contract.owner==='root'){
    requireValue(Array.isArray(driver.checks)&&driver.checks.filter(item=>item?.id===entry.id).length===1&&
      driver.checks.includes(entry),'Claim is not the unique root check in this report');
    requireValue(identityMatches(driver.scope),'Root check has no matching driver product scope');
    requireValue(!Array.isArray(driver.targets)||driver.targets.length===0,
      'This report contract does not accept target-scoped checks');
    return;
  }
  requireValue(Array.isArray(driver.targets)&&driver.targets.length===1,
    'Expected one target-scoped report');
  const target=driver.targets[0];
  requireValue(identityMatches(target),'Target belongs to another product or variant');
  requireValue(Array.isArray(target.checks)&&target.checks.filter(item=>item?.id===entry.id).length===1&&
    target.checks.includes(entry),'Claim is not the unique target check in this report');
}

function verifyClaimIdentity(entry,product,id,contract){
  requireValue(entry?.id===id,'Claim ID does not match the supported assessor');
  for(const field of ['checks','adaptationIds']){
    if(entry[field]===undefined)continue;
    requireValue(Array.isArray(entry[field])&&entry[field].length===1&&entry[field][0]===id,
      `Claim ${field} must identify only ${id}`);
  }
  requireValue(typeof product?.game==='string'&&supportsContractVariant(contract,product),
    'No complete-evidence contract is registered for this product variant');
}

function evidenceEnvelope(value,product,id,label){
  requireValue(record(value),`${label} must be a structured evidence envelope`);
  requireValue(value.game===product.game&&value.variant===product.variant,
    `${label} belongs to another product or variant`);
  requireValue(idsContain(value.adaptationIds,id),`${label} does not identify ${id}`);
  requireValue(record(value.observation),`${label} has no structured observation`);
  return value.observation;
}

function resolvePointer(report,pointer){
  requireValue(typeof pointer==='string'&&pointer.startsWith('/')&&!/~(?![01])/u.test(pointer),
    'Evidence reference is not a valid JSON pointer');
  const tokens=pointer.slice(1).split('/').map(token=>token.replaceAll('~1','/').replaceAll('~0','~'));
  let value=report;
  for(const token of tokens){
    requireValue(!['checks','scenarios','scenario','modes','locales','coverage','scope','selection'].includes(token),
      `Evidence reference points into claim metadata: ${pointer}`);
    requireValue(record(value)&&Object.hasOwn(value,token),`Missing required evidence reference ${pointer}`);
    value=value[token];
  }
  requireValue(record(value),`Evidence reference ${pointer} is not an evidence envelope`);
  return value;
}

function referencedObservations(driver,entry,product,id){
  const expected=REQUIRED_REFS[id];
  const actual=entry.evidenceRefs;
  requireValue(Array.isArray(actual)&&actual.length===expected.length&&
    new Set(actual).size===expected.length&&expected.every(pointer=>actual.includes(pointer)),
  `${id} must reference its complete declared evidence set`);
  return Object.fromEntries(expected.map(pointer=>[
    pointer,evidenceEnvelope(resolvePointer(driver,pointer),product,id,pointer),
  ]));
}

function checkImport(envelope,product,id){
  const observation=evidenceEnvelope(envelope,product,id,'Package import');
  requireValue(nonempty(observation.path)&&observation.path.toLowerCase().endsWith('.zip'),
    'Package import has no ZIP source');
  requireValue(observation.source==='Launcher #gamePackageImport UI'&&
    observation.store==='IndexedDB Package Store','Package import did not use the Launcher Package Store path');
  const generation=observation.generation;
  requireValue(record(generation)&&nonempty(generation.generationId)&&nonempty(generation.revision)&&
    generation.fileId==='game-data'&&generation.target===`/${product.game}.data`&&
    positiveInt(generation.bytes)&&validDigest(generation.sha256)&&
    generation.declaredBytes===generation.bytes&&generation.declaredSha256===generation.sha256,
  'Imported generation does not identify the selected DATA bytes');
  return{observation,generation};
}

function checkBoundary(envelope,product,id,label,expectedData,{generationId}={}){
  const observation=evidenceEnvelope(envelope,product,id,label);
  const {before,after,dataRead,writes}=observation;
  requireValue(record(before)&&record(after)&&record(before.files)&&record(after.files)&&
    record(before.objects)&&record(after.objects),'Package boundary is missing before/after store snapshots');
  equal(before,after,`${label} changed the installed generation or file references`);
  requireValue(Array.isArray(writes)&&writes.length===0,`${label} performed a Package Store write`);
  requireValue(nonempty(before.generationId)&&nonempty(before.revision)&&
    (!generationId||before.generationId===generationId),`${label} read another installed generation`);
  const file=before.files['game-data'];
  requireValue(record(file)&&nonempty(file.objectId)&&Object.hasOwn(before.objects,file.objectId),
    `${label} DATA file reference does not resolve to an object`);
  const object=before.objects[file.objectId];
  sameData(object,expectedData,`${label} DATA object differs from the expected bytes`);
  requireValue(record(dataRead)&&dataRead.game===product.game&&positiveInt(dataRead.epoch)&&
    dataRead.generation===before.generationId&&dataRead.fileId==='game-data',
  `${label} did not observe a Runtime DATA read from the selected generation`);
  sameData(dataRead,expectedData,`${label} Runtime DATA read differs from the expected bytes`);
  sameData(observation.expectedData,expectedData,`${label} has a different expected DATA identity`);
  return observation;
}

function assessNegativeBoundary(envelope,product,id,expectedData,generationId){
  const observation=evidenceEnvelope(envelope,product,id,'Package Store negative control');
  requireValue(observation.probe?.database==='eagler-touhou-package-store-v1'&&
    observation.probe?.store==='objects'&&observation.probe?.mode==='readwrite',
  'Negative control did not attempt a Package Store write');
  const actual=observation.actual;
  requireValue(record(actual)&&record(actual.before)&&record(actual.after)&&
    isDeepStrictEqual(actual.before,actual.after)&&record(actual.before.files)&&record(actual.before.objects),
  'Negative control changed the installed Package Store snapshot');
  requireValue(Array.isArray(actual.writes)&&actual.writes.some(write=>write?.operation==='transaction'&&
    write?.database==='eagler-touhou-package-store-v1'&&write?.mode==='readwrite'&&
    Array.isArray(write?.stores)&&write.stores.includes('objects')),
  'Negative control has no observed write transaction');
  requireValue(observation.rejectedByUnchangedAssertion===true,
    'Negative control was not rejected by the unchanged-state assertion');
  requireValue(actual.before.generationId===generationId&&nonempty(actual.before.revision),
    'Negative control was not observed in the final installed generation');
  const file=actual.before.files['game-data'];
  requireValue(record(file)&&nonempty(file.objectId)&&Object.hasOwn(actual.before.objects,file.objectId),
    'Negative control DATA file reference does not resolve to an object');
  sameData(actual.before.objects[file.objectId],expectedData,'Negative control changed the installed DATA identity');
  requireValue(actual.dataRead?.generation===generationId&&actual.dataRead?.game===product.game&&
    actual.dataRead?.fileId==='game-data',
  'Negative control did not observe the selected Runtime DATA read');
  sameData(actual.expectedData,expectedData,'Negative control used another DATA identity');
  sameData(actual.dataRead,expectedData,'Negative control read another DATA identity');
}

function assessPresentation(observation,profile){
  const presentation=profile?.presentation;
  requireValue(profile?.lanes?.includes('presentation')&&presentation?.checkId==='DISPLAY-03',
    'No registered presentation capture contract for this product');
  requireValue(observation.kind==='presentation-lab-native-capture',
    'DISPLAY-03 is not a Presentation Lab native capture');
  requireValue(record(observation.artifact)&&nonempty(observation.artifact.path)&&
    positiveInt(observation.artifact.bytes)&&validDigest(observation.artifact.sha256),
  'DISPLAY-03 has no identified capture artifact');
  const build=observation.build;
  requireValue(record(build)&&build.schema===profile.buildIdentitySchema&&
    build.instrumented===true&&build.dataAvailable===true&&nonempty(build.commit)&&
    validDigest(build.wasm)&&validDigest(build.loader)&&validDigest(build.sourceDigest)&&
    validDigest(build.toolDigest)&&record(build.toolSources)&&Object.keys(build.toolSources).length>0,
  'DISPLAY-03 capture is not bound to an identified instrumented build');
  requireValue(Array.isArray(observation.errors)&&observation.errors.length===0,
    'DISPLAY-03 capture contains errors');
  const reports=observation.reports;
  const expectedLabels=[...presentation.requiredNormalLabels,...presentation.negativeLabels];
  requireValue(Array.isArray(reports)&&reports.length===expectedLabels.length,
    'DISPLAY-03 report set is incomplete');
  const labels=reports.map(report=>report?.label);
  requireValue(new Set(labels).size===expectedLabels.length&&
    expectedLabels.every(label=>labels.includes(label)),'DISPLAY-03 report labels are missing or duplicated');
  const positiveLabels=new Set(presentation.requiredNormalLabels);
  for(const report of reports){
    requireValue(positiveInt(report.tick)&&Number.isSafeInteger(report.previousTick)&&
      report.tick===report.previousTick+1&&report.consecutive===true&&report.gate===true,
    `${report.label} lacks a consecutive gated logical Tick`);
    requireValue(report.valid===true&&report.interpolationReady===true&&Array.isArray(report.objects)&&
      report.objects.length>0&&Array.isArray(report.stateChanges),
    `${report.label} lacks valid native presentation observations`);
    const authority=report.stateAuthority;
    requireValue(authority?.complete===true&&
      ['missingDomains','invalidDomains','unmappedGroups','uncoveredFields'].every(key=>
        Array.isArray(authority[key])&&authority[key].length===0),
    `${report.label} has incomplete state authority`);
    const expectedPass=positiveLabels.has(report.label);
    requireValue(report.purity===expectedPass&&report.purityStatus===(expectedPass?'pass':'fail'),
      `${report.label} purity result does not match its positive/negative-control role`);
    requireValue(report.worldFrozen===(report.label===presentation.pauseLabel),
      `${report.label} has an unexpected world-freeze result`);
  }
}

function checkRuntimeEvents(observation,product,label,{online,expectedData,generationId}={}){
  requireValue(record(observation)&&nonempty(observation.frameUrl)&&record(observation.query),
    `${label} has no Runtime launch observation`);
  let url;
  try{url=new URL(observation.frameUrl);}catch{fail(`${label} Runtime frame URL is invalid`);}
  requireValue(observation.query.hosted==='1'&&observation.query.managedData==='1'&&
    observation.query.runtimeVariant==='normal'&&
    url.searchParams.get('hosted')==='1'&&url.searchParams.get('managedData')==='1'&&
    url.searchParams.get('runtimeVariant')==='normal'&&
    url.searchParams.get('gameGeneration')===observation.query.gameGeneration,
  `${label} did not launch the selected managed normal Runtime`);
  requireValue(observation.query.gameGeneration===generationId,
    `${label} launched another package generation`);
  requireValue(observation.pageOnline===online&&observation.workerControlled===true,
    `${label} has the wrong network or Service Worker state`);
  const ready=observation.ready,first=observation.first;
  requireValue(ready?.protocol==='eagler-touhou/1'&&ready.game===product.game&&
    Number.isSafeInteger(ready.epoch)&&ready.event==='ready'&&
    first?.protocol===ready.protocol&&first.game===ready.game&&first.epoch===ready.epoch&&
    first.event==='first-frame',`${label} did not reach the first frame in the selected Runtime`);
  const dataRead=observation.dataRead;
  requireValue(dataRead?.game===product.game&&dataRead.epoch===ready.epoch&&
    dataRead.generation===generationId&&dataRead.fileId==='game-data',
  `${label} has no matching native DATA read`);
  sameData(dataRead,expectedData,`${label} Runtime read different DATA bytes`);
  requireValue(Array.isArray(observation.dataReads)&&observation.dataReads.length===1&&
    isDeepStrictEqual(observation.dataReads[0],dataRead),
  `${label} DATA read trace does not match its selected Runtime`);
}

function checkPostflight(envelope,product,id){
  const observation=evidenceEnvelope(envelope,product,id,'Package postflight');
  const privateData=observation.privateData;
  sameData(privateData,privateData,'Postflight private DATA identity is invalid');
  const offlinePackage=observation.offlinePackage;
  requireValue(record(offlinePackage)&&record(offlinePackage.data)&&record(offlinePackage.package)&&
    nonempty(offlinePackage.package.path)&&positiveInt(offlinePackage.package.archiveBytes)&&
    validDigest(offlinePackage.package.archiveSha256),
  'Postflight has no identified offline package');
  sameData(offlinePackage.data,privateData,'Postflight package and private DATA differ');
  const runtime=observation.normalRuntimePackage;
  requireValue(record(runtime)&&runtime.game===product.game&&runtime.variant==='normal'&&
    runtime.entry===`${product.game}.html`&&validDigest(runtime.wasmSha256)&&validDigest(runtime.manifestSha256),
  'Postflight does not identify the packaged normal Runtime');
  return{privateData,offlinePackage,runtime};
}

function assessRuntime05(refs,product,id){
  const imported=checkImport(refs['/uiImport'].__envelope,product,id);
  const expectedData={bytes:imported.generation.bytes,sha256:imported.generation.sha256};
  for(const pointer of ['/resourceBoundaries/online','/resourceBoundaries/onlineEnd'])
    checkBoundary(refs[pointer].__envelope,product,id,pointer,expectedData,
      {generationId:imported.generation.generationId});
  const finalOnline=checkBoundary(refs['/resourceBoundaries/finalOnlineEnd'].__envelope,product,id,
    '/resourceBoundaries/finalOnlineEnd',expectedData);
  checkBoundary(refs['/resourceBoundaries/offline'].__envelope,product,id,
    '/resourceBoundaries/offline',expectedData,{generationId:finalOnline.before.generationId});
  assessNegativeBoundary(refs['/resourceBoundaryNegative'].__envelope,product,id,expectedData,
    finalOnline.before.generationId);
}

function assessPackage01(refs,product,id){
  const imported=checkImport(refs['/uiImport'].__envelope,product,id);
  const expectedData={bytes:imported.generation.bytes,sha256:imported.generation.sha256};
  checkBoundary(refs['/resourceBoundaries/online'].__envelope,product,id,'online package boundary',
    expectedData,{generationId:imported.generation.generationId});
  checkBoundary(refs['/resourceBoundaries/offline'].__envelope,product,id,'offline package boundary',expectedData);
}

function assessPackage02(refs,product,id){
  const preflight=refs['/offlinePackagePreflight'].__envelope;
  const pre=evidenceEnvelope(preflight,product,id,'Offline package preflight');
  const pkg=pre.package;
  requireValue(pre.game===product.game&&pre.source==='offline-package'&&record(pkg)&&
    nonempty(pkg.path)&&pkg.path.toLowerCase().endsWith('.zip')&&positiveInt(pkg.archiveBytes)&&
    validDigest(pkg.archiveSha256)&&nonempty(pkg.descriptorRevision)&&pkg.dataFileId==='game-data',
  'Offline package preflight does not identify the selected ZIP and DATA member');
  const expectedData={bytes:pkg.dataBytes,sha256:pkg.dataSha256};
  sameData(pre.expectedData,expectedData,'Offline package preflight DATA differs from its manifest');
  const imported=checkImport(refs['/uiImport'].__envelope,product,id);
  requireValue(imported.generation.revision===pkg.descriptorRevision&&
    imported.generation.fileId===pkg.dataFileId,'Launcher imported another package descriptor');
  sameData(imported.generation,expectedData,'Launcher imported different DATA bytes');
  const runtimeEnvelope=refs['/onlineRuntime'].__envelope;
  const runtime=evidenceEnvelope(runtimeEnvelope,product,id,'Online Runtime');
  checkRuntimeEvents(runtime,product,'Online Runtime',{
    online:true,expectedData,generationId:imported.generation.generationId,
  });
  const post=checkPostflight(refs['/postflight'].__envelope,product,id);
  sameData(post.privateData,expectedData,'Postflight private DATA differs from the selected package');
  requireValue(post.offlinePackage.package.archiveSha256===pkg.archiveSha256,
    'Postflight archive differs from the preflight package');
}

function assessPackage05(refs,product,id){
  const shell=evidenceEnvelope(refs['/appShell'].__envelope,product,id,'Installed App Shell');
  requireValue(nonempty(shell.scope)&&nonempty(shell.worker)&&shell.controlled===true&&
    shell.controlledBeforeImport===true,'Installed App Shell was not controlled before import');
  let shellUrl,workerUrl;
  try{shellUrl=new URL(shell.scope);workerUrl=new URL(shell.worker,shell.scope);}catch{fail('Installed App Shell URLs are invalid');}
  requireValue(shellUrl.protocol==='https:'&&workerUrl.origin===shellUrl.origin,
    'Installed App Shell Service Worker does not match its HTTPS origin');
  const post=checkPostflight(refs['/postflight'].__envelope,product,id);
  const runtime=evidenceEnvelope(refs['/offlineRuntime'].__envelope,product,id,'Offline Runtime');
  checkRuntimeEvents(runtime,product,'Offline Runtime',{
    online:false,expectedData:post.privateData,generationId:runtime.query?.gameGeneration,
  });
}

function assessPackage06(refs,product,id){
  const authority=evidenceEnvelope(refs['/packageAuthority'].__envelope,product,id,'Package installation authority');
  requireValue(authority.status==='passed'&&authority.coverageComplete===true,
    'PACKAGE-06 driver did not seal a complete installation authority observation');
  const input=authority.authorityInput;
  requireValue(record(input)&&input.schema==='eagler-touhou/package-installation-authority-input/1'&&
    input.game===product.game&&record(input.journal),
  'PACKAGE-06 has no retained Package Store journal input');
  const imported=checkImport(refs['/uiImport'].__envelope,product,id);
  const expectedData={bytes:imported.generation.bytes,sha256:imported.generation.sha256};
  requireValue(input.baseGenerationId===imported.generation.generationId&&
    input.baseRevision===imported.generation.revision,
  'PACKAGE-06 journal starts from another imported generation');

  const update=authority.update,fixture=authority.fixture;
  requireValue(record(update)&&update.status==='committed'&&record(update.base)&&record(update.current)&&
    update.base.generationId===imported.generation.generationId&&update.base.revision===imported.generation.revision&&
    nonempty(update.current.generationId)&&nonempty(update.current.revision)&&update.current.pendingGeneration===null,
  'PACKAGE-06 has no committed second Package Store generation');
  requireValue(record(fixture)&&fixture.game===product.game&&fixture.revision===update.current.revision&&
    record(fixture.descriptor)&&nonempty(fixture.descriptor.path)&&positiveInt(fixture.descriptor.bytes)&&
    validDigest(fixture.descriptor.sha256)&&record(fixture.catalog)&&fixture.catalog.path==='release-catalog.json'&&
    positiveInt(fixture.catalog.bytes)&&validDigest(fixture.catalog.sha256)&&record(fixture.sourceArchive)&&
    positiveInt(fixture.sourceArchive.bytes)&&validDigest(fixture.sourceArchive.sha256)&&
    Array.isArray(fixture.files)&&fixture.files.length>0&&fixture.files.every(file=>
      record(file)&&typeof file.source==='string'&&(file.source.startsWith('games/')||file.source.startsWith('shared/'))&&
      positiveInt(file.bytes)&&validDigest(file.sha256)),
  'PACKAGE-06 published fixture is not bound to verified descriptor and package bytes');
  requireValue(isDeepStrictEqual(update.sourceArchive,fixture.sourceArchive),
    'PACKAGE-06 committed update does not use the declared published fixture archive');

  const card=authority.gameCard,prompt=input.updatePrompt;
  requireValue(record(card)&&card.source==='Launcher game card after a real page reopen'&&
    ['启动游戏','Start game'].includes(card.launchLabel),
  'PACKAGE-06 has no reopened Launcher game-card result');
  requireValue(record(prompt)&&prompt.observed===true&&prompt.confirmed===true&&
    classifyPackageAuthorityPrompt(prompt)==='published-package-update-ready'&&
    nonempty(prompt.cancel)&&prompt.publishedRevision===update.current.revision&&
    prompt.currentGenerationAtPrompt===update.current.generationId&&
    prompt.currentRevisionAtPrompt===update.current.revision&&prompt.sourceAtPrompt==='local'&&
    prompt.pinnedRuntimeGenerationAtPrompt===imported.generation.generationId&&
    isDeepStrictEqual(authority.uiResults?.updatePrompt,prompt),
  'PACKAGE-06 production update-ready prompt is missing or not tied to its committed and pinned generations');

  checkRuntimeEvents(authority.initialRuntime,product,'Pinned pre-update Runtime',{
    online:true,expectedData,generationId:imported.generation.generationId,
  });
  requireValue(Number(authority.initialRuntime.query.runtimeEpoch)===prompt.pinnedRuntimeEpoch,
    'PACKAGE-06 update-ready prompt refers to another Runtime epoch');
  requireValue(authority.initialRuntime.ready.epoch===prompt.pinnedRuntimeEpoch,
    'PACKAGE-06 initial ready event refers to another Runtime epoch');

  const recomputed=assessPackageInstallationAuthority(input);
  requireValue(recomputed.available===true&&recomputed.valid===true,
    `PACKAGE-06 installation authority could not be recomputed: ${recomputed.reason??'unknown evidence gap'}`);
  const normalizedRecomputed=JSON.parse(JSON.stringify(recomputed));
  equal(authority.productionReads,normalizedRecomputed,
    'PACKAGE-06 reported store evidence differs from the retained read journal');

  const online=refs['/onlineRuntime'].__observation;
  const offline=refs['/offlineRuntime'].__observation;
  checkRuntimeEvents(online,product,'Online Runtime',{
    online:true,expectedData,generationId:update.current.generationId,
  });
  requireValue(Number(online.query.runtimeEpoch)>prompt.pinnedRuntimeEpoch,
    'PACKAGE-06 restart did not create a fresh Runtime epoch after the update-ready decision');
  requireValue(Number(online.query.runtimeEpoch)===online.ready.epoch&&Number(offline.query.runtimeEpoch)===offline.ready.epoch,
    'PACKAGE-06 Runtime URL and ready event epochs differ');
  for(const runtime of [authority.initialRuntime,online,offline])
    requireValue(Number(new URL(runtime.frameUrl).searchParams.get('runtimeEpoch'))===runtime.ready.epoch,
      'PACKAGE-06 actual iframe URL and ready event epochs differ');
  checkRuntimeEvents(offline,product,'Offline Runtime',{
    online:false,expectedData,generationId:update.current.generationId,
  });
  requireValue(input.updatedGenerationId===update.current.generationId&&input.updatedRevision===update.current.revision&&
    input.finalRuntimeGenerationId===online.query.gameGeneration&&
    input.offlineRuntimeGenerationId===offline.query.gameGeneration,
  'PACKAGE-06 journal identities do not match the updated online and offline Runtime generations');
  requireValue(isDeepStrictEqual(authority.uiResults?.gameCard,card)&&
    authority.uiResults?.onlineRuntime?.generationId===update.current.generationId&&
    authority.uiResults?.offlineRuntime?.generationId===update.current.generationId&&
    isDeepStrictEqual(authority.uiResults.onlineRuntime.dataRead,online.dataRead)&&
    isDeepStrictEqual(authority.uiResults.offlineRuntime.dataRead,offline.dataRead),
  'PACKAGE-06 UI Runtime receipts do not match the retained native DATA observations');
}

function assessPractice(observation,practice){
  const replay=practice?.replayRoundTrip;
  requireValue(replay?.supported===true&&nonempty(replay.format)&&typeof replay.assess==='function',
    'No registered native Replay roundtrip assessor for this product');
  const result=observation.result,input=result?.observationInput;
  requireValue(record(input)&&input.schema==='eagler-touhou/practice-replay-input/1'&&
    input.format===replay.format&&record(input.evidence),
  'PRACTICE-05 source assessor input does not match the registered Replay format');
  const selectionCase=replay.selectionCase??practice.selectionCase;
  const recomputed=replay.assess(input.evidence,{selectionCase});
  requireValue(record(recomputed)&&recomputed.complete===true&&recomputed.status==='observed'&&
    Array.isArray(recomputed.missing)&&recomputed.missing.length===0&&
    Array.isArray(recomputed.problems)&&recomputed.problems.length===0,
  'Native Replay roundtrip assessor did not recompute a complete result');
  const{observationInput,...reportedResult}=result;
  equal(reportedResult,recomputed,'PRACTICE-05 normalized result differs from the native Replay assessor result');
}

function checkProducerStage(driver,product,stage){
  const metadata=stage?.producer,current=stage?.current;
  requireValue(record(metadata)&&nonempty(metadata.path)&&positiveInt(metadata.bytes)&&validDigest(metadata.sha256)&&
    nonempty(metadata.rawJson)&&nonempty(metadata.runId)&&metadata.stage&&
    Number.isFinite(Date.parse(metadata.startedAt))&&Number.isFinite(Date.parse(metadata.finishedAt))&&
    Date.parse(metadata.finishedAt)>=Date.parse(metadata.startedAt),
  'Build producer report identity is incomplete');
  const raw=Buffer.from(metadata.rawJson,'utf8');
  requireValue(raw.length===metadata.bytes&&createHash('sha256').update(raw).digest('hex')===metadata.sha256,
    'Build producer report bytes do not match their recorded identity');
  let producer;
  try{producer=JSON.parse(metadata.rawJson);}catch{fail('Build producer report is not valid JSON');}
  requireValue(producer.schema==='eagler-touhou/build-acceptance/1'&&producer.stage===metadata.stage&&
    producer.runId===metadata.runId&&producer.startedAt===metadata.startedAt&&producer.finishedAt===metadata.finishedAt&&
    producer.status==='passed'&&producer.passed===true&&producer.scope?.game===product.game&&
    producer.scope?.variant===product.variant,
  'Build producer report is unsealed or belongs to another product, variant or stage');
  const expectedStage=metadata.stage==='source-build'?'sourceBuild':'package';
  requireValue(metadata.stage===stage.expectedStage&&record(current)&&
    isDeepStrictEqual(producer.observations?.[expectedStage],current)&&
    isDeepStrictEqual(metadata.invocation,producer.invocations?.length===1?producer.invocations[0]:null)&&
    current.invocation?.exitCode===0&&current.invocation?.script===
      (metadata.stage==='source-build'?'launcher/scripts/build-runtime.mjs':'launcher/scripts/package-runtime.mjs')&&
    Number.isFinite(Date.parse(current.invocation.startedAt))&&Number.isFinite(Date.parse(current.invocation.finishedAt))&&
    current.invocation.argumentSha256&&validDigest(current.invocation.argumentSha256),
  'Build producer report does not identify the revalidated official invocation and observation');
  const ids=metadata.stage==='source-build'?['BUILD-01','BUILD-02','BUILD-03']:['BUILD-04'];
  requireValue(Array.isArray(producer.checks)&&producer.checks.length===ids.length&&
    ids.every(id=>producer.checks.filter(claim=>claim?.id===id).length===1),
  'Build producer report omits or duplicates a stage check');
  for(const id of ids){
    const claim=producer.checks.find(item=>item.id===id);
    requireValue(claim.status==='passed'&&claim.complete===false&&claim.scope===`${product.game}/${product.variant}`&&
      claim.evidence?.game===product.game&&claim.evidence?.variant===product.variant&&
      isDeepStrictEqual(claim.evidence.adaptationIds,[id])&&record(claim.evidence.observation),
    `Build producer report has an invalid ${id} stage claim`);
  }
  return{metadata,current,producer};
}

function assessBuild(observation,driver,product,id){
  requireValue(driver.stage==='verify-existing'&&driver.status==='passed'&&driver.passed===true,
    'Build evidence was not sealed by the read-only revalidation stage');
  const verification=driver.observations?.revalidation;
  requireValue(record(verification)&&verification.verification?.mode==='revalidated-existing-stage-reports'&&
    Number.isFinite(Date.parse(verification.verification.checkedAt))&&record(verification.manifest)&&
    nonempty(verification.manifest.path)&&validDigest(verification.manifest.sha256),
  'Build revalidation does not identify its Runtime manifest and verification mode');
  const target=observation.target;
  requireValue(record(target)&&target.game===product.game&&target.variant===product.variant&&
    nonempty(target.owner)&&nonempty(target.sourceRoot)&&nonempty(target.buildPlan)&&nonempty(target.buildReceipt)&&
    nonempty(target.packageReceipt)&&nonempty(target.runtime)&&nonempty(target.entry)&&validDigest(target.packageSha256)&&
    isDeepStrictEqual(target,verification.target),
  'Build evidence target differs from the selected product or revalidation report');
  const source=checkProducerStage(driver,product,{...observation.sourceBuild,expectedStage:'source-build'});
  requireValue(isDeepStrictEqual(observation.sourceBuild,
    {producer:verification.sourceBuild?.report,current:verification.sourceBuild?.observation}),
    'Source build evidence differs from the revalidated producer report');
  const currentSource=source.current;
  requireValue(currentSource.plan?.game===product.game&&currentSource.plan?.variant===product.variant&&
    currentSource.plan?.source===target.sourceRoot&&currentSource.plan?.owner===target.owner&&
    currentSource.plan?.path===target.buildPlan&&validDigest(currentSource.plan?.sha256)&&
    validDigest(currentSource.plan?.planSha256)&&nonempty(currentSource.plan?.toolchain?.version)&&
    record(currentSource.plan?.features)&&Array.isArray(currentSource.plan?.requiredSubmodules)&&
    currentSource.receipt?.path===target.buildReceipt&&validDigest(currentSource.receipt?.sha256)&&
    validDigest(currentSource.receipt?.executionSha256)&&validDigest(currentSource.receipt?.sourceInputsSha256)&&
    currentSource.receipt?.planSha256===currentSource.plan.planSha256&&
    Number.isSafeInteger(currentSource.receipt?.outputCount)&&currentSource.receipt.outputCount>0&&
    Number.isSafeInteger(currentSource.receipt?.sourceCount)&&currentSource.receipt.sourceCount>0,
  'Current source build plan, toolchain or receipt is incomplete');
  if(id==='BUILD-01'){
    requireValue(observation.claim==='selected-source-owner'&&target.sourceRoot===currentSource.plan.source&&
      nonempty(currentSource.plan.owner),'BUILD-01 source ownership is not verified');
  }else if(id==='BUILD-02'){
    requireValue(observation.claim==='current-build-plan-and-toolchain'&&nonempty(currentSource.plan.common)&&
      currentSource.plan.toolchain.compiler!==undefined&&currentSource.plan.toolchain.root!==undefined,
    'BUILD-02 does not identify the current build plan and toolchain');
  }else if(id==='BUILD-03'){
    requireValue(observation.claim==='current-source-build-output'&&source.metadata.invocation.exitCode===0&&
      source.metadata.invocation.script==='launcher/scripts/build-runtime.mjs',
    'BUILD-03 has no successful official source-build invocation');
  }else{
    requireValue(id==='BUILD-04'&&observation.claim==='current-package-and-source-binding',
      'Unknown BUILD complete-evidence claim');
    const packageStage=checkProducerStage(driver,product,{...observation.package,expectedStage:'package'});
    requireValue(isDeepStrictEqual(observation.package,
      {producer:verification.package?.report,current:verification.package?.observation}),
      'Package evidence differs from the revalidated producer report');
    const currentPackage=packageStage.current.package;
    requireValue(record(currentPackage)&&currentPackage.path===target.packageReceipt&&
      currentPackage.output===target.runtime&&currentPackage.entry===target.entry&&
      currentPackage.packageSha256===target.packageSha256&&validDigest(currentPackage.receiptSha256)&&
      validDigest(currentPackage.wasmSha256)&&validDigest(currentPackage.manifestSha256)&&
      currentPackage.sourceBuildReceiptSha256===currentSource.receipt.sha256&&
      currentPackage.sourceInputsSha256===currentSource.receipt.sourceInputsSha256&&
      currentPackage.wasmSha256===currentSource.receipt.executionSha256&&
      packageStage.metadata.invocation.script==='launcher/scripts/package-runtime.mjs',
    'BUILD-04 package output is not bound to the revalidated source build');
  }
}

function assessMinimalRuntime(driver,entry,product){
  const observation=evidenceEnvelope(entry.evidence,product,'BUILD-05','BUILD-05');
  requireValue(entry.status==='passed'&&entry.complete===true,
    'BUILD-05 is not declared as a passed complete journey');
  const journey=observation.journey;
  requireValue(record(journey)&&record(journey.checkout)&&record(journey.build)&&record(journey.offline),
    'BUILD-05 does not retain the nested checkout, build and Launcher reports');
  const recomputed=assessMinimalRuntimeJourney({...journey,...product,assessEvidence:assessCompleteEvidence});
  equal(entry,recomputed,'BUILD-05 summary differs from the recomputed nested journey');
}

function assessReviewedSource(driver,entry,product,id){
  const observation=evidenceEnvelope(entry.evidence,product,id,id);
  requireValue(entry.status==='passed'&&entry.complete===true,
    `${id} is not declared as a passed complete review`);
  const checks=driver.checks?.map(item=>item?.id);
  requireValue(Array.isArray(checks)&&checks.length>0&&new Set(checks).size===checks.length&&
    checks.every(value=>['BUILD-06','VERIFY-05'].includes(value)),
  'Source review report has an incomplete or duplicate check set');
  const inputIdentity=observation.inputIdentity;
  requireValue(record(inputIdentity)&&record(inputIdentity.manifest)&&nonempty(inputIdentity.manifest.path)&&
    validDigest(inputIdentity.manifest.sha256)&&positiveInt(inputIdentity.manifest.bytes)&&
    Array.isArray(inputIdentity.files)&&inputIdentity.files.length>0,
  `${id} does not retain the reviewed input file identities`);
  equal(driver.inputs,inputIdentity,`${id} inputs are not bound to its reviewed evidence`);
  const recomputed=inspectSourceReviewNow({manifest:inputIdentity.manifest.path,game:product.game,
    variant:product.variant,checks});
  equal(recomputed.inputs,inputIdentity,`${id} reviewed files changed or are absent from its input identity list`);
  const expected=recomputed.checks.find(item=>item.id===id);
  requireValue(expected,'The selected review check was not recomputed');
  equal(entry.evidence,expected.evidence,`${id} evidence differs from its accepted source and original-reference records`);
}

/** Recompute only complete claims whose shared or registered title evidence is supported here. */
export function assessCompleteEvidence({driver,entry,product}={}){
  const runtimeInput=assessRuntimeInputEvidence({driver,entry,product});
  if(runtimeInput.available)return runtimeInput;
  const presentation=assessPresentationEvidence({driver,entry,product});
  if(presentation.available)return presentation;
  const id=entry?.id;
  const contract=CONTRACTS[id];
  if(!contract)return{available:false,valid:false,reason:`No complete-evidence assessor for ${id??'unknown claim'}`,kind:null};
  if(driver?.schema!==contract.schema)return{
    available:false,valid:false,
    reason:`No ${contract.kind} assessor for driver ${driver?.schema??'unknown'}`,
    kind:contract.kind,
  };
  if(typeof product?.game!=='string'||!supportsContractVariant(contract,product))return{
    available:false,valid:false,reason:`No ${contract.kind} assessor for ${product?.game??'unknown'}/${product?.variant??'unknown'}`,
    kind:contract.kind,
  };
  const diagnosticProfile=id==='DISPLAY-03'?resolveDiagnosticProfile(product.game,'normal'):null;
  if(id==='DISPLAY-03'&&(!diagnosticProfile?.lanes?.includes('presentation')||
    diagnosticProfile.presentation?.checkId!=='DISPLAY-03'))return{
    available:false,valid:false,reason:`No registered presentation capture contract for ${product.game}/normal`,kind:contract.kind,
  };
  const adaptation=id==='PRACTICE-05'?resolvePracticeAdapter(product.game,'normal'):null;
  if(id==='PRACTICE-05'&&(!adaptation?.replayRoundTrip||adaptation.replayRoundTrip.supported!==true||
    typeof adaptation.replayRoundTrip.assess!=='function'))return{
    available:false,valid:false,reason:`No native Replay roundtrip assessor for ${product.game}/normal`,kind:contract.kind,
  };
  if(id==='PRACTICE-05'&&!record(entry?.observation?.observation?.result?.observationInput))return{
    available:false,valid:false,reason:'PRACTICE-05 report omits the native Replay roundtrip assessor input',kind:contract.kind,
  };
  try{
    verifyClaimIdentity(entry,product,id,contract);
    reportOwnsEntry(driver,entry,product,contract);
    if(id==='BUILD-05'){
      requireValue(!entry.evidenceRefs||entry.evidenceRefs.length===0,
        'BUILD-05 requires its retained nested journey inline');
      assessMinimalRuntime(driver,entry,product);
    }else if(id==='BUILD-06'||id==='VERIFY-05'){
      requireValue(!entry.evidenceRefs||entry.evidenceRefs.length===0,
        `${id} requires its recomputed reviewed input identity inline`);
      assessReviewedSource(driver,entry,product,id);
    }else if(id.startsWith('BUILD-')){
      requireValue(!entry.evidenceRefs||entry.evidenceRefs.length===0,
        'BUILD checks require their inline revalidated stage evidence');
      assessBuild(evidenceEnvelope(entry.evidence,product,id,'Build acceptance'),driver,product,id);
    }else if(id==='DISPLAY-03'){
      const lanes=driver.scope?.lanes;
      requireValue(Array.isArray(lanes)&&lanes.includes('presentation')&&new Set(lanes).size===lanes.length&&
        lanes.every(lane=>diagnosticProfile.lanes.includes(lane))&&
        entry.lane==='presentation','DISPLAY-03 must be scoped to the presentation lane');
      requireValue(!entry.evidenceRefs||entry.evidenceRefs.length===0,
        'DISPLAY-03 requires its inline native-capture observation');
      assessPresentation(evidenceEnvelope(entry.observation,product,id,'DISPLAY-03'),diagnosticProfile);
    }else if(id==='PRACTICE-05'){
      requireValue(!entry.evidenceRefs||entry.evidenceRefs.length===0,
        'PRACTICE-05 requires its inline native Replay observation');
      assessPractice(evidenceEnvelope(entry.observation,product,id,'PRACTICE-05'),adaptation);
    }else{
      const refs=referencedObservations(driver,entry,product,id);
      const envelopes=Object.fromEntries(Object.entries(refs).map(([pointer,observation])=>[
        pointer,{__envelope:resolvePointer(driver,pointer),__observation:observation},
      ]));
      if(id==='RUNTIME-05')assessRuntime05(envelopes,product,id);
      else if(id==='PACKAGE-01')assessPackage01(envelopes,product,id);
      else if(id==='PACKAGE-02')assessPackage02(envelopes,product,id);
      else if(id==='PACKAGE-05')assessPackage05(envelopes,product,id);
      else assessPackage06(envelopes,product,id);
    }
    return{available:true,valid:true,reason:null,kind:contract.kind};
  }catch(error){
    return{available:true,valid:false,reason:error?.message??String(error),kind:contract.kind};
  }
}
