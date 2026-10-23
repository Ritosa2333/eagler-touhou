import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstatSync,readFileSync,statSync} from 'node:fs';
import {basename,dirname,isAbsolute,resolve} from 'node:path';
import {isDeepStrictEqual} from 'node:util';
import {buildPresentationPairedReport,readProductionPresentationEvidence} from '../../../tools/ports/presentation-paired-report.mjs';
import {assessPresentationVisualArtifacts} from './presentation-visual-artifacts.mjs';
import {identityDigest} from '../../../tools/testing/dependency-snapshot.mjs';
import {resolveDiagnosticProfile} from './diagnostics.mjs';

const IDS=new Set(['DISPLAY-01','DISPLAY-03','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-07','REPLAY-07','AUDIO-06','VERIFY-07']);
const SHA=/^[a-f0-9]{64}$/u;
const record=value=>value!==null&&typeof value==='object'&&!Array.isArray(value);
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const same=(left,right)=>isDeepStrictEqual(left,right);
function requireValue(condition,message){assert.ok(condition,message);}

function readIdentityBytes(identity,label){
  requireValue(record(identity)&&isAbsolute(identity.path)&&Number.isSafeInteger(identity.bytes)&&identity.bytes>0&&
    SHA.test(identity.sha256??''),`${label} needs an absolute path, byte count and SHA-256`);
  const before=lstatSync(identity.path);
  requireValue(before.isFile()&&!before.isSymbolicLink(),`${label} must be a regular non-symlink file`);
  const bytes=readFileSync(identity.path),after=statSync(identity.path);
  requireValue(bytes.length===identity.bytes&&digest(bytes)===identity.sha256,`${label} bytes differ from the retained identity`);
  requireValue(after.size===before.size&&after.mtimeMs===before.mtimeMs&&after.ctimeMs===before.ctimeMs,
    `${label} changed while being read`);
  return{bytes,identity:{path:resolve(identity.path),bytes:bytes.length,sha256:digest(bytes)}};
}
function readIdentityFile(identity,label){
  const {bytes,identity:verified}=readIdentityBytes(identity,label);let value;
  try{value=JSON.parse(bytes.toString('utf8'));}
  catch(error){throw new Error(`${label} is not valid JSON: ${error.message}`);}
  return{value,identity:verified};
}

function checkStatus(row,id){
  requireValue(record(row)&&row.id===id,`Paired report omits ${id}`);
  return row.status;
}

function validateProductBinding({driver,entry,product,profile,capability}){
  requireValue(product?.variant==='normal','Paired presentation evidence requires its registered normal product');
  requireValue(Array.isArray(driver.targets)&&driver.targets.length===1,
    'Paired presentation evidence requires exactly one selected target');
  const target=driver.targets[0];
  requireValue(target.game===product.game&&target.variant===product.variant&&typeof target.id==='string'&&target.id,
    'Paired target belongs to another product or variant');
  requireValue(target.id===driver.selection?.targetId&&target.id===driver.evidence?.targetId,
    'Paired target identity differs from the selected Runtime');
  requireValue(Array.isArray(target.checks)&&target.checks.filter(item=>item===entry).length===1,
    'Claim is not the unique check in the selected target');
  requireValue(driver.scope?.game===product.game&&driver.scope?.variant===product.variant&&
    driver.profile?.game===product.game&&driver.profile?.variant===product.variant&&
    driver.profile?.schema===profile.schema,
  'Paired report profile or scope belongs to another product');
  const lanes=driver.scope?.lanes;
  requireValue(Array.isArray(lanes)&&lanes.includes('presentation')&&new Set(lanes).size===lanes.length,
    'Paired report does not select one valid presentation lane');
  const requested=driver.scope?.pairedReplay;
  requireValue(requested?.requested===true&&requested.status==='ready'&&same(requested.capability,capability),
    'Paired report capability differs from the registered presentation profile');
  requireValue(driver.identity?.status==='stable'&&driver.identity.stable===true&&
    record(driver.identity.pre)&&record(driver.identity.post)&&
    driver.identity.pre.targetId===target.id&&driver.identity.post.targetId===target.id&&
    driver.identity.pre.fingerprint===driver.identity.post.fingerprint,
  'Paired report does not retain a stable pre/post product identity');
  const pre=readIdentityFile(driver.identity.pre,'Pre-run diagnostic identity');
  const post=readIdentityFile(driver.identity.post,'Post-run diagnostic identity');
  for(const [label,snapshot] of [['Pre-run',pre.value],['Post-run',post.value]]){
    requireValue(snapshot.game===product.game&&snapshot.variant===product.variant&&snapshot.targetId===target.id&&
      snapshot.target?.id===target.id&&snapshot.lanes?.includes('presentation')&&
      same(snapshot.pairedReplay,capability)&&same(snapshot.caseIdentity?.profile?.presentation?.pairedReplay,capability),
    `${label} identity does not bind the selected profile and paired Replay capability`);
    requireValue(identityDigest(snapshot)===driver.identity[label==='Pre-run'?'pre':'post'].fingerprint,
      `${label} identity fingerprint does not match its snapshot`);
    const pairedInput=snapshot.caseIdentity?.files?.pairedReplay;
    const selectedInput=driver.evidence?.diagnosticInputs?.pairedReplay;
    requireValue(record(pairedInput)&&record(selectedInput)&&pairedInput.bytes===selectedInput.bytes&&
      pairedInput.sha256===selectedInput.sha256&&resolve(pairedInput.path)===resolve(selectedInput.path),
    `${label} identity does not bind the selected Replay input bytes`);
  }
  requireValue(same(pre.value,post.value),'Diagnostic profile, inputs or target changed during the paired run');
  return target;
}

function requireArtifactSet(artifacts,capability){
  requireValue(record(artifacts)&&record(artifacts.runs),'Paired report omits its raw evidence artifacts');
  const names={
    acceptance:'presentation-acceptance.json',
    pair:`paired-replay-${capability.fixture}-result.json`,
    baseline:`paired-replay-${capability.fixture}-baseline.json`,
    controlledExtraDraw:`paired-replay-${capability.fixture}-controlled-extra-draw.json`,
    cadence144:`paired-replay-${capability.fixture}-cadence-144hz.json`,
  };
  const refs={acceptance:artifacts.acceptance,pair:artifacts.pair,
    baseline:artifacts.runs.baseline,controlledExtraDraw:artifacts.runs.controlledExtraDraw,
    cadence144:artifacts.runs.cadence144};
  const loaded={},evidenceDirectories=new Set();
  for(const [key,name] of Object.entries(names)){
    requireValue(basename(refs[key]?.path??'')===name,`Paired ${key} artifact is not the registered ${name}`);
    const result=readIdentityFile(refs[key],`Paired ${key} artifact`);
    loaded[key]=result.value;evidenceDirectories.add(dirname(result.identity.path));
  }
  requireValue(evidenceDirectories.size===1,'Paired raw artifacts do not share one evidence directory');
  requireValue(record(artifacts.input)&&isAbsolute(artifacts.input.path)&&basename(artifacts.input.path)===capability.inputFile&&
    Number.isSafeInteger(artifacts.input.bytes)&&artifacts.input.bytes>0&&SHA.test(artifacts.input.sha256??''),
  'Paired Replay input identity does not match the registered fixture');
  readIdentityBytes(artifacts.input,'Selected Replay input');
  return{...loaded,input:{path:resolve(artifacts.input.path),bytes:artifacts.input.bytes,sha256:artifacts.input.sha256}};
}

function runCheck(comparison,id){
  const row=comparison.checks?.find(item=>item?.id===id);
  requireValue(record(row),`Recomputed comparison omits ${id}`);
  return row;
}

function validateRunInputs({runs,capability,scope,input,driver}){
  requireValue(Array.isArray(runs)&&runs.length===3,'Three retained cadence run artifacts are required');
  const expectedRoles=['baseline','controlled-extra-draw','controlled-extra-draw'];
  const diagnosticInputs=driver.evidence?.diagnosticInputs?.pairedReplay;
  requireValue(record(diagnosticInputs)&&resolve(diagnosticInputs.path)===input.path&&
    diagnosticInputs.bytes===input.bytes&&diagnosticInputs.sha256===input.sha256,
  'Selected Replay bytes do not match the diagnostic input identity');
  const build=driver.evidence?.labBuild;
  requireValue(record(build)&&SHA.test(build.wasm??'')&&SHA.test(build.sourceDigest??''),
    'Paired run has no retained diagnostic Lab build identity');
  const targetData=driver.evidence?.manifestData;
  requireValue(record(targetData)&&Number.isSafeInteger(targetData.bytes)&&targetData.bytes>0&&SHA.test(targetData.sha256??''),
    'Paired run has no retained selected DATA identity');
  const epochs=new Set();
  for(let index=0;index<runs.length;index++){
    const run=runs[index],rate=capability.cadencesHz[index],label=`${rate} Hz`;
    requireValue(run?.schema==='eagler-presentation/replay-capture/1'||
      run?.schema==='eagler-presentation/replay-pair-run/1',`${label} raw Replay run has an unsupported schema`);
    requireValue(run.role===expectedRoles[index]&&run.fixture===capability.fixture&&same(run.scope,scope),
      `${label} raw Replay run belongs to another role, fixture or product`);
    requireValue(Number.isSafeInteger(run.epoch)&&run.epoch>0&&!epochs.has(run.epoch),
      `${label} raw Replay run must have a fresh Runtime epoch`);epochs.add(run.epoch);
    requireValue(run.scheduler?.rateHz===rate&&run.scheduler?.source==='GameHost::frame',
      `${label} raw Replay run lacks its registered production callback cadence`);
    requireValue(run.capture?.requestedTicks===capability.ticks&&run.capture?.completedTicks===capability.ticks&&
      run.samples?.length===capability.ticks,`${label} raw Replay run omits its complete logical-tick window`);
    requireValue(run.replayIdentity?.bytes===input.bytes&&run.replayIdentity?.sha256===input.sha256,
      `${label} raw Replay run consumed another input file`);
    requireValue(run.identity?.build?.game===scope.game&&run.identity.build.diagnostic===true&&
      run.identity.build.instrumented===true&&run.identity.build.wasm===build.wasm&&
      run.identity.build.sourceDigest===build.sourceDigest&&run.identity.build.toolDigest===build.toolDigest,
    `${label} raw Replay run belongs to another diagnostic build`);
    requireValue(run.identity?.data?.bytes===targetData.bytes&&run.identity.data.sha256===targetData.sha256,
      `${label} raw Replay run consumed another DATA file`);
    requireValue(run.readVerification?.schema==='eagler-presentation/read-only-observation/1'&&
      run.readVerification.repetitions===2&&run.readVerification.expectedAuthorityGroups===capability.expectedAuthorityGroups&&
      run.readVerification.authorityGroups===capability.expectedAuthorityGroups&&run.readVerification.status==='passed'&&
      Array.isArray(run.readVerification.issues)&&run.readVerification.issues.length===0,
    `${label} raw run does not retain the registered repeated-read evidence`);
  }
  requireValue(runs[0].identity?.build?.productionBuild?.available===true&&
    runs[0].identity.build.productionBuild.diagnostic===false&&
    runs[0].identity.build.productionBuild.sourceMatchesCurrent===true&&
    runs[0].identity.build.productionBuild.binaryMatchesDescriptor===true&&
    SHA.test(runs[0].identity.build.productionBuild.wasmSha256??'')&&
    runs[0].identity.build.productionBuild.wasmSha256!==runs[0].identity.build.wasm,
  'VERIFY-07 lacks an independent current production build identity');
  requireValue(runs.every(run=>same(run.identity.build.productionBuild,runs[0].identity.build.productionBuild)),
    'Production build identity differs between fresh Runtime epochs');
  return epochs;
}

function recompute({driver,entry,product,profile,capability}){
  requireValue(entry.id&&IDS.has(entry.id)&&entry.lane==='presentation-paired',
    'Paired presentation claim is not registered in the paired lane');
  requireValue(entry.status==='passed'&&entry.machineComplete===true,
    `${entry.id} is not declared as a machine-complete passed claim`);
  const target=validateProductBinding({driver,entry,product,profile,capability});
  const envelope=entry.observation;
  requireValue(record(envelope)&&envelope.game===product.game&&envelope.variant===product.variant&&
    Array.isArray(envelope.adaptationIds)&&envelope.adaptationIds.length===1&&envelope.adaptationIds[0]===entry.id,
  `${entry.id} observation is not uniquely bound to this product`);
  const observation=envelope.observation;
  requireValue(record(observation)&&observation.schema==='eagler-presentation/acceptance/1'&&
    same(observation.scope,{game:product.game,variant:product.variant})&&
    record(observation.check)&&observation.check.id===entry.id,
  `${entry.id} does not retain the paired acceptance row`);
  requireValue(entry.status===observation.check.status&&entry.complete===observation.check.complete&&
    entry.machineComplete===observation.check.machineComplete,
  `${entry.id} report flags differ from its retained acceptance row`);
  requireValue(same(entry, target.checks.find(item=>item.id===entry.id)),`${entry.id} differs from its target check`);
  const artifacts=requireArtifactSet(observation.artifacts,capability);
  const input=driver.evidence?.diagnosticInputs?.pairedReplay;
  requireValue(record(input)&&resolve(input.path)===artifacts.input.path&&input.bytes===artifacts.input.bytes&&
    input.sha256===artifacts.input.sha256,`${entry.id} Replay input identity differs from the selected diagnostic manifest`);
  validateRunInputs({runs:[artifacts.baseline,artifacts.controlledExtraDraw,artifacts.cadence144],capability,
    scope:{game:product.game,variant:product.variant},input:artifacts.input,driver});
  const runs=[artifacts.baseline,artifacts.controlledExtraDraw,artifacts.cadence144];
  if(['DISPLAY-04','DISPLAY-05','DISPLAY-06'].includes(entry.id))return assessPresentationVisualArtifacts({id:entry.id,
    refs:observation.artifacts.display,acceptance:artifacts.acceptance,observation,entry,capability,runs});
  // Reopen the ordinary browser report and every hash-bound raw capture.
  const productionEvidence=observation.artifacts.productionReport?
    readProductionPresentationEvidence(observation.artifacts.productionReport):null;
  if(productionEvidence){
    requireValue(same(productionEvidence.scope,{game:product.game,variant:product.variant}), 'Production capture belongs to another product');
    requireValue(same(productionEvidence.build,driver.evidence.runtime)&&same(productionEvidence.data,driver.evidence.manifestData),
      'Production capture differs from the selected package or DATA');
  }
  const recomputed=buildPresentationPairedReport({runs,requiredTicks:capability.ticks,ratesHz:capability.cadencesHz,
    scope:{game:product.game,variant:product.variant},expectedAuthorityGroups:capability.expectedAuthorityGroups,
    cadenceDifferenceGroups:capability.cadenceDifferenceGroups,fixture:capability.fixture,input:artifacts.input,productionEvidence});
  const result=artifacts.pair,acceptance=artifacts.acceptance;
  requireValue(result?.schema==='eagler-presentation/replay-pair/1'&&same(result.scope,observation.scope)&&
    result.fixture===capability.fixture&&same(result.input,{bytes:artifacts.input.bytes,sha256:artifacts.input.sha256}),
  'Saved paired comparison belongs to another product or Replay fixture');
  requireValue(same(result,recomputed),'Saved paired comparison differs from independently recomputed native and PCM evidence');
  const expected=runCheck(recomputed,entry.id);
  requireValue(expected.status==='passed'&&expected.machineComplete===true,
    `${entry.id} lacks complete independently recomputed machine evidence`);
  const acceptedCheck=acceptance.checks?.find(row=>row?.id===entry.id);
  requireValue(acceptance.schema==='eagler-presentation/acceptance/1'&&same(acceptance.scope,observation.scope)&&
    checkStatus(acceptedCheck,entry.id)===expected.status&&acceptedCheck.complete===expected.complete&&
    acceptedCheck.machineComplete===expected.machineComplete&&same(acceptedCheck,observation.check),
  `${entry.id} accepted claim differs from independently recomputed raw evidence`);
  requireValue(Array.isArray(acceptedCheck.evidence?.sources)&&acceptedCheck.evidence.sources.length===1&&
    same(acceptedCheck.evidence.sources[0],expected.evidence),`${entry.id} accepted source differs from the recomputed claim`);
  requireValue(observation.humanReview?.status==='pending','Machine evidence must not manufacture a human display review');
  return expected;
}

/** Recompute paired presentation machine claims from the retained raw Replay files. */
export function assessPresentationEvidence({driver,entry,product}={}){
  const id=entry?.id;
  if(!IDS.has(id))return{available:false,valid:false,reason:`No paired-presentation assessor for ${id??'unknown claim'}`,kind:null};
  if(id==='DISPLAY-03'&&entry?.lane!=='presentation-paired')
    return{available:false,valid:false,reason:null,kind:'paired-presentation'};
  if(driver?.schema!=='eagler-touhou/diagnostic-browser/1')
    return{available:false,valid:false,reason:`No paired-presentation assessor for driver ${driver?.schema??'unknown'}`,kind:'paired-presentation'};
  const kind='paired-presentation';
  try{
    const profile=resolveDiagnosticProfile(product?.game,product?.variant);
    const capability=profile?.presentation?.pairedReplay;
    requireValue(profile&&capability&&capability.schema==='eagler-presentation/replay-pair-capability/3',
      `No registered paired presentation profile for ${product?.game??'unknown'}/${product?.variant??'unknown'}`);
    requireValue(Array.isArray(capability.checks)&&capability.checks.includes(id),
      `${id} is not included by the registered paired presentation profile`);
    recompute({driver,entry,product,profile,capability});
    return{available:true,valid:true,reason:null,kind};
  }catch(error){
    return{available:true,valid:false,reason:error?.message??String(error),kind};
  }
}
