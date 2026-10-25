import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash} from 'node:crypto';
import {mkdir,readFile,readdir,stat,writeFile} from 'node:fs/promises';
import {basename,dirname,isAbsolute,relative,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {dependencySnapshot,fileIdentity,identityDigest} from '../testing/dependency-snapshot.mjs';
import {buildDiagnosticServerEnvironment,inspectDiagnosticProduct,loadDiagnosticInputs} from './diagnostic-inputs.mjs';
import {DIAGNOSTIC_LANE_CHECKS,PAIRED_PRESENTATION_CHECKS,pairedPresentationCapability,
  resolveDiagnosticProfile} from '../../tests/adaptation/adapters/diagnostics.mjs';
import {buildPresentationPairedReport,readProductionPresentationEvidence} from './presentation-paired-report.mjs';
import {diagnosticCaseIdentity} from '../../tests/adaptation/identity.mjs';
import {recordDiagnosticReferenceChecks} from '../../tests/adaptation/adapters/diagnostic-reference-evidence.mjs';
import {assertDiagnosticCaptureBuild,finalizeDiagnosticReport,finalizeDiagnosticTarget,plannedDiagnosticChecks,recordDiagnosticLane} from './diagnostic-report.mjs';
import {stopDiagnosticChild,trackDiagnosticChild} from './diagnostic-lifecycle.mjs';
import {runTasks} from '../testing/runner.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const requestedOutput=arg('output');
if(!requestedOutput)throw new Error('Required: --output=fresh-directory');
const output=resolve(requestedOutput);
const repoRoot=resolve(import.meta.dirname,'../..');
const lanesAllowed=new Set(['presentation','quick','daily']);
const shaPattern=/^[a-f0-9]{64}$/;
const delay=ms=>new Promise(resolve=>setTimeout(resolve,ms));

try{
  const prior=await readdir(output);
  assert.deepEqual(prior,[],'--output must name a fresh empty directory');
}catch(error){if(error.code!=='ENOENT')throw error;}

function parseLanes(value){
  assert.ok(typeof value==='string'&&value.length,'Select one or more diagnostic lanes');
  const lanes=value.split(',');
  assert.ok(lanes.length&&lanes.every(lane=>lanesAllowed.has(lane)),'Lanes must be presentation, quick or daily');
  assert.equal(new Set(lanes).size,lanes.length,'Diagnostic lanes may not repeat');
  return lanes;
}

function repositoryFile(path,label){
  assert.ok(typeof path==='string'&&path.length&&!isAbsolute(path),`${label} must be a repository-relative path`);
  const absolute=resolve(repoRoot,path),outside=relative(repoRoot,absolute);
  assert.ok(outside!== '..'&&!outside.startsWith(`..${process.platform==='win32'?'\\':'/'}`)&&!isAbsolute(outside),
    `${label} leaves the repository`);
  return absolute;
}

async function requireFile(path,label){
  const info=await stat(path);
  assert.ok(info.isFile(),`${label} must be a file: ${path}`);
  return path;
}

async function requireDirectory(path,label){
  const info=await stat(path);
  assert.ok(info.isDirectory(),`${label} must be a directory: ${path}`);
  return path;
}

function validateProfileLanes(profile,lanes){
  for(const key of ['buildIdentitySchema','wasmFile','runtimeProfile'])
    assert.ok(typeof profile[key]==='string'&&profile[key].length,`Diagnostic profile is missing ${key}`);
  for(const key of ['font','data','inputRoot','snapshot'])
    assert.ok(typeof profile.envNames?.[key]==='string'&&profile.envNames[key].length,
      `Diagnostic profile is missing the ${key} environment name`);
  if(lanes.includes('presentation')){
    const contract=profile.presentation;
    assert.ok(contract&&typeof contract==='object'&&Array.isArray(contract.requiredNormalLabels)&&
      Array.isArray(contract.negativeLabels)&&Array.isArray(contract.partialChecks),
      'Diagnostic profile has no presentation assertion contract');
    for(const key of ['checkId','playerOwnerId','positionFieldId','audioMutationGroup','movementLabel',
      'negativeMovementLabel','recoveredMovementLabel','pauseLabel','scope'])
      assert.ok(contract[key]!==undefined,`Diagnostic presentation contract is missing ${key}`);
  }
  if(lanes.includes('quick')){
    assert.ok(typeof profile.quickCaptureSchema==='string'&&profile.quickCaptureSchema.length,
      'Diagnostic profile has no quick capture schema');
    assert.ok(Number.isSafeInteger(profile.quickFixtureCount)&&profile.quickFixtureCount>0,
      'Diagnostic profile has no positive quickFixtureCount');
    assert.ok(Array.isArray(profile.quickInputs)&&profile.quickInputs.length===profile.quickFixtureCount,
      'Diagnostic profile quickInputs must match quickFixtureCount');
    assert.equal(new Set(profile.quickInputs.map(item=>item.id)).size,profile.quickFixtureCount,
      'Diagnostic quick fixture ids must be unique');
    assert.equal(new Set(profile.quickInputs.map(item=>item.index)).size,profile.quickFixtureCount,
      'Diagnostic quick fixture indices must be unique');
    assert.equal(new Set(profile.quickInputs.map(item=>item.captureFile)).size,profile.quickFixtureCount,
      'Diagnostic quick capture filenames must be unique');
    assert.ok(Array.isArray(profile.quickPartialChecks),'Diagnostic profile has no quick partial-check list');
    assert.ok(profile.quickInputs.every((item,index)=>typeof item.id==='string'&&item.id&&item.index===index&&
      typeof item.file==='string'&&item.file.length&&typeof item.captureFile==='string'&&
      /^[A-Za-z0-9][A-Za-z0-9._-]*\.json$/.test(item.captureFile)),
    'Quick fixtures require ordered input identities and safe capture artifact names');
  }
  if(lanes.includes('daily')){
    assert.ok(typeof profile.dailyCaptureSchema==='string'&&profile.dailyCaptureSchema.length,
      'Diagnostic profile has no daily capture schema');
    assert.ok(Array.isArray(profile.dailyFixtures)&&profile.dailyFixtures.length>0,
      'Diagnostic profile has no daily fixtures');
    assert.ok(profile.dailyFixtures.every(item=>typeof item.captureFile==='string'&&
      /^[A-Za-z0-9][A-Za-z0-9._-]*\.json$/.test(item.captureFile)),
    'Daily fixtures require safe capture artifact names');
    assert.equal(new Set(profile.dailyFixtures.map(item=>item.captureFile)).size,profile.dailyFixtures.length,
      'Daily capture filenames must be unique');
    const keys=new Set(profile.dailyFixtures.map(item=>item.replayKey));
    assert.equal(keys.size,profile.dailyFixtures.length,'Daily replay keys must be unique');
    assert.ok(Array.isArray(profile.dailyGateReplayOptions),'Diagnostic profile has no daily gate replay options');
    assert.deepEqual(new Set(profile.dailyGateReplayOptions.map(item=>item.replayKey)),keys,
      'Daily gate options must select exactly the registered daily Replay inputs');
    assert.ok(profile.dailyGateReplayOptions.every(item=>typeof item.option==='string'&&item.option.startsWith('--')),
      'Daily gate replay options must declare CLI option names');
    assert.ok(Array.isArray(profile.dailyPartialChecks),'Diagnostic profile has no daily partial-check list');
  }
  for(const lane of lanes)assert.ok(typeof profile.laneScope?.[lane]==='string'&&profile.laneScope[lane].length,
    `Diagnostic profile has no ${lane} scope statement`);
  for(const lane of lanes){
    assert.ok(Array.isArray(DIAGNOSTIC_LANE_CHECKS[lane])&&DIAGNOSTIC_LANE_CHECKS[lane].length,
      `No numbered adaptation checks are mapped to diagnostic lane ${lane}`);
    if(lane==='presentation')assert.ok(DIAGNOSTIC_LANE_CHECKS[lane].includes(profile.presentation.checkId),
      'Presentation check ID is absent from the shared lane mapping');
  }
}

function profilePathKeys(lanes){
  const keys=['serverScript','buildReceipt','sourceRoot','presentationDirectory','runtimeDirectory'];
  if(lanes.includes('presentation'))keys.push('browserVerifier');
  if(lanes.includes('quick'))keys.push('quickCapture','quickGate','goldenManifest');
  if(lanes.includes('daily'))keys.push('dailyCapture','dailyGate','goldenManifest');
  return [...new Set(keys)];
}

async function resolveProfilePaths(profile,lanes){
  const paths={};
  for(const key of profilePathKeys(lanes)){
    const absolute=repositoryFile(profile[key],`Diagnostic profile ${key}`);
    if(['runtimeDirectory','sourceRoot','presentationDirectory'].includes(key))
      paths[`${key}Absolute`]=await requireDirectory(absolute,`Diagnostic profile ${key}`);
    else paths[`${key}Absolute`]=await requireFile(absolute,`Diagnostic profile ${key}`);
  }
  return paths;
}

async function createDiagnosticSnapshot({manifestPath,diagnosticsPath,game,variant,lanes,expectedTargetId,pairedReplay=null}){
  const runtimeConfig=JSON.parse(await readFile(manifestPath,'utf8'));
  const selection=selectRuntimeTargets(runtimeConfig,{game,variant}),target=selection.targets[0];
  assert.ok(target,`No Runtime target selected for ${game}/${variant}`);
  if(expectedTargetId!==undefined)assert.equal(target.id,expectedTargetId,'Selected Runtime target changed during diagnostic execution');
  const [caseIdentity,runtimeIdentity,dataIdentity,runtimeManifestIdentity,diagnosticsManifestIdentity]=await Promise.all([
    diagnosticCaseIdentity({root:repoRoot,manifestPath:diagnosticsPath,game,variant,lanes,pairedReplay}),
    verifyRuntimePackage(target,{repoRoot}),verifyPrivateData(target.data),
    fileIdentity(manifestPath),fileIdentity(diagnosticsPath),
  ]);
  const snapshot={game,variant,lanes,pairedReplay:pairedReplay??null,targetId:target.id,target:{id:target.id,game:target.game,variant:target.variant,
      runtime:target.runtime,entry:target.entry,data:target.data,packageSha256:target.packageSha256},runtimeManifest:{path:manifestPath,...runtimeManifestIdentity},
    diagnosticManifest:{path:diagnosticsPath,...diagnosticsManifestIdentity},runtimeIdentity,dataIdentity,caseIdentity};
  return{snapshot,fingerprint:identityDigest(snapshot)};
}

async function saveDiagnosticSnapshot(output,name,snapshot){
  const path=resolve(output,`${name}.json`);
  await writeFile(path,JSON.stringify(snapshot.snapshot,null,2)+'\n');
  return{path,...await fileIdentity(path),fingerprint:snapshot.fingerprint,targetId:snapshot.snapshot.targetId};
}

async function reserveLoopbackPort(){
  const server=createServer();
  await new Promise((accept,reject)=>{server.once('error',reject);server.listen(0,'127.0.0.1',accept);});
  const port=server.address().port;
  await new Promise((accept,reject)=>server.close(error=>error?reject(error):accept()));
  return port;
}

function validateLabIdentity(value,profile){
  assert.ok(value&&value.schema===profile.buildIdentitySchema&&value.instrumented===true,
    'Presentation Lab server did not return an instrumented build identity');
  assert.ok(typeof value.commit==='string'&&value.commit.length>0,'Presentation Lab build identity has no source commit');
  for(const field of ['wasm','loader','sourceDigest','toolDigest'])assert.match(value[field]??'',shaPattern,
    `Presentation Lab identity has no valid ${field} hash`);
  assert.ok(value.toolSources&&typeof value.toolSources==='object'&&!Array.isArray(value.toolSources)&&Object.keys(value.toolSources).length,
    'Presentation Lab build identity has no closed tool source list');
  for(const [path,digest] of Object.entries(value.toolSources))assert.match(digest,shaPattern,`Invalid tool source hash: ${path}`);
  return value;
}

async function validateLabPackage(runtimeDirectory,labBuild,profile){
  const inventory=JSON.parse(await readFile(resolve(runtimeDirectory,'runtime-files.json'),'utf8'));
  assert.equal(inventory.schema,'eagler-touhou/runtime-directory/1','Presentation Lab Runtime inventory schema is unsupported');
  const wasm=inventory.files?.[profile.wasmFile];
  assert.ok(wasm&&shaPattern.test(wasm.sha256??''),`Diagnostic Runtime inventory has no WASM identity for ${profile.wasmFile}`);
  assert.equal(wasm.sha256,labBuild.wasm,'Served Lab WASM does not match the packaged Lab Runtime');
  const manifest=JSON.parse(await readFile(resolve(runtimeDirectory,'manifest.json'),'utf8'));
  assert.equal(manifest.profile,profile.runtimeProfile,'Selected diagnostic Runtime has another profile identity');
  assert.equal(manifest.execution?.sha256,labBuild.wasm,'Diagnostic package manifest and served build identity disagree');
  const bytes=await readFile(resolve(runtimeDirectory,profile.wasmFile));
  assert.equal(createHash('sha256').update(bytes).digest('hex'),labBuild.wasm,'Packaged Lab WASM bytes changed');
  return{runtimeDirectory,inventorySchema:inventory.schema,wasmSha256:labBuild.wasm};
}

async function startLabServer({script,profile,product,output}){
  const port=await reserveLoopbackPort(),url=`http://127.0.0.1:${port}/`;
  const stdout=[],stderr=[];
  const outPath=resolve(output,'lab-server.stdout.log'),errPath=resolve(output,'lab-server.stderr.log');
  const {environment,clearedSnapshotEnvironment}=buildDiagnosticServerEnvironment(process.env,profile,product,port);
  const child=spawn(process.execPath,[script],{cwd:repoRoot,windowsHide:true,stdio:['ignore','pipe','pipe'],env:environment});
  child.stdout.setEncoding('utf8').on('data',chunk=>stdout.push(chunk));
  child.stderr.setEncoding('utf8').on('data',chunk=>stderr.push(chunk));
  const lifecycle=trackDiagnosticChild(child);
  const finishLogs=async()=>{
    await Promise.all([writeFile(outPath,stdout.join('')),writeFile(errPath,stderr.join(''))]);
  };
  let stopPromise=null;
  const stop=()=>stopPromise??=(async()=>{
    const outcome=await stopDiagnosticChild(child,{tracker:lifecycle});
    await finishLogs();
    return{...outcome,stdout:outPath,stderr:errPath};
  })();
  try{
    let lastError='server has not started';
    for(let elapsed=0;elapsed<30000;elapsed+=250){
      if(child.exitCode!==null||child.signalCode!==null)throw new Error(`Lab server exited before readiness (${child.exitCode??child.signalCode})`);
      try{
        const response=await fetch(new URL('build.json',url),{signal:AbortSignal.timeout(1000),cache:'no-store'});
        if(response.ok){const build=validateLabIdentity(await response.json(),profile);
          const packageIdentity=await validateLabPackage(profile.runtimeDirectoryAbsolute,build,profile);
          return{url,port,build,packageIdentity,clearedSnapshotEnvironment,stop};}
        lastError=`HTTP ${response.status}`;
      }catch(error){lastError=error.message;}
      await delay(250);
    }
    throw new Error(`Presentation Lab server did not publish build.json: ${lastError}`);
  }catch(error){
    const shutdown=await stop();
    if(!shutdown.exited||!shutdown.normal)
      throw new AggregateError([error,new Error(`Lab server cleanup failed: ${JSON.stringify(shutdown)}`)],
        'Presentation Lab setup failed and its server did not stop normally');
    throw error;
  }
}

function browserTask({id,script,url,evidenceRoot,timeoutMs,extraArgs=[]}){
  return{id,command:process.env.TH_PYTHON??process.env.PYTHON??(process.platform==='win32'?'python':'python3'),
    args:[script,'--url',url,'--output',evidenceRoot,...extraArgs],cwd:repoRoot,timeoutMs};
}

function nodeTask({id,script,args,timeoutMs}){
  return{id,command:process.execPath,args:[script,...args],cwd:repoRoot,timeoutMs};
}

function pythonTask({id,script,args,timeoutMs}){
  return{id,command:process.env.TH_PYTHON??process.env.PYTHON??(process.platform==='win32'?'python':'python3'),
    args:[script,...args],cwd:repoRoot,timeoutMs};
}

function allTaskResultsPassed(report){return report?.results?.length>0&&report.results.every(result=>result.status==='passed');}

async function readJsonArtifact(path){
  const before=await stat(path);assert.ok(before.isFile(),`Expected JSON evidence file: ${path}`);
  const bytes=await readFile(path),after=await stat(path);
  assert.equal(after.size,before.size,`Evidence file changed size while being read: ${path}`);
  assert.equal(after.mtimeMs,before.mtimeMs,`Evidence file changed while being read: ${path}`);
  assert.equal(after.ctimeMs,before.ctimeMs,`Evidence file metadata changed while being read: ${path}`);
  return{report:JSON.parse(bytes.toString('utf8')),
    identity:{path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}};
}

function assessPresentationCapture(capture,serverBuild,contract){
  assert.ok(capture&&Array.isArray(capture.reports),'browser-captures.json has no report rows');
  assert.deepEqual(capture.errors,[],'Presentation Lab browser recorded page errors');
  assert.deepEqual(capture.build,serverBuild,'Browser capture build identity differs from the served Lab build');
  const reports=capture.reports;
  assert.ok(reports.length>0,'Presentation Lab captured no windows');
  for(const row of reports){
    assert.equal(row.valid,true,`${row.label}: capture window is invalid`);
    assert.equal(row.stateAuthority?.complete,true,`${row.label}: authority mapping is incomplete`);
    assert.deepEqual(row.stateAuthority?.samples,[],`${row.label}: authority fields are unmapped`);
  }
  const normal=reports.filter(row=>row.negativeControl===false),negative=reports.filter(row=>row.negativeControl===true);
  const labels=new Map(reports.map(row=>[row.label,row]));
  const requiredNormal=contract.requiredNormalLabels;
  for(const label of requiredNormal)assert.ok(normal.some(row=>row.label===label),`Presentation capture is missing ${label}`);
  for(const row of normal)assert.equal(row.purityStatus,'pass',`${row.label}: normal draw changed authoritative state`);
  assert.deepEqual(negative.map(row=>row.label).sort(),[...contract.negativeLabels].sort(),
    'Both independent real draw-side negative controls are required');
  for(const row of negative){
    assert.equal(row.purityStatus,'fail',`${row.label}: purity negative control was not rejected`);
    assert.ok(row.stateChanges?.some(change=>change.group===contract.audioMutationGroup),
      `${row.label}: missing native audio mutation evidence`);
  }
  const positionStatus=label=>labels.get(label)?.objects?.find(object=>object.ownerId===contract.playerOwnerId)?.fields?.find(field=>field.id===contract.positionFieldId)?.status;
  assert.equal(positionStatus(contract.movementLabel),'interpolated','Positive movement window lacks interpolation evidence');
  assert.equal(positionStatus(contract.negativeMovementLabel),'missing-interpolation','Movement negative control was not detected');
  assert.equal(positionStatus(contract.recoveredMovementLabel),'interpolated','Recovered movement window did not return to interpolation');
  assert.deepEqual(labels.get(contract.movementLabel).traceBefore,labels.get(contract.recoveredMovementLabel).traceAfter,
    'Recovered window does not return to the original trace boundary');
  assert.equal(labels.get(contract.pauseLabel).worldFrozen,true,'Pause window did not freeze the game world');
  return{status:'passed',complete:true,normalWindows:normal.length,negativeControls:negative.map(row=>row.label),
    authorityComplete:true,namedStatePurity:'pass',scope:contract.scope};
}

function recordPairedPresentationChecks(target,acceptance,capabilityStatus,reason,artifacts=null){
  const checkIds=new Set(PAIRED_PRESENTATION_CHECKS);
  const byId=new Map((acceptance?.checks??[]).map(row=>[row.id,row]));
  const retained=target.checks.filter(row=>!checkIds.has(row.id));
  const records=PAIRED_PRESENTATION_CHECKS.map(id=>{
    const row=byId.get(id);
    if(!row)return{id,lane:'presentation-paired',status:capabilityStatus==='blocked'?'blocked':'incomplete',complete:false,
      machineComplete:false,scope:'paired presentation / named Replay window',
      observation:{game:target.game,variant:target.variant,adaptationIds:[id],observation:{reason:reason??`Paired report omitted ${id}`}}};
    return{id,lane:'presentation-paired',status:row.status,complete:row.complete===true,
      machineComplete:row.machineComplete===true,scope:'paired presentation / named Replay window',
      observation:{game:target.game,variant:target.variant,adaptationIds:[id],observation:{
        schema:acceptance.schema,scope:acceptance.scope,check:row,humanReview:acceptance.humanReview??null,
        artifacts}}};
  });
  target.checks=retained.concat(records);
}

async function presentationPairArtifacts(evidenceRoot,product,profile,serverBuild,productIdentities,productionReportPath){
  const capability=profile.presentation.pairedReplay;
  const resultPath=resolve(evidenceRoot,'paired-replay-'+capability.fixture+'-result.json');
  const acceptancePath=resolve(evidenceRoot,'presentation-acceptance.json');
  const baselinePath=resolve(evidenceRoot,'paired-replay-'+capability.fixture+'-baseline.json');
  const controlledPath=resolve(evidenceRoot,'paired-replay-'+capability.fixture+'-controlled-extra-draw.json');
  const cadence144Path=resolve(evidenceRoot,'paired-replay-'+capability.fixture+'-cadence-144hz.json');
  const [result,acceptance,baseline,controlled,cadence144]=await Promise.all([
    readJsonArtifact(resultPath),readJsonArtifact(acceptancePath),readJsonArtifact(baselinePath),
    readJsonArtifact(controlledPath),readJsonArtifact(cadence144Path)]);
  const inputPath=resolve(product.inputRoot,capability.inputFile);
  const inputIdentity={path:inputPath,...await fileIdentity(inputPath,'Paired presentation Replay input')};
  assert.deepEqual({bytes:inputIdentity.bytes,sha256:inputIdentity.sha256},
    {bytes:productIdentities.pairedReplay?.bytes,sha256:productIdentities.pairedReplay?.sha256},
    'Paired Replay input changed after the diagnostic identity snapshot');
  const expectedIds=[...PAIRED_PRESENTATION_CHECKS].sort();
  const actualIds=(acceptance.report.checks??[]).map(row=>row.id).sort();
  assert.deepEqual(actualIds,expectedIds,'Paired presentation acceptance has missing or duplicate check rows');
  assert.equal(result.report.schema,'eagler-presentation/replay-pair/1','Paired Replay report schema is unsupported');
  assert.equal(acceptance.report.schema,'eagler-presentation/acceptance/1','Paired acceptance schema is unsupported');
  assert.deepEqual(acceptance.report.scope,{game:profile.game,variant:profile.variant},
    'Paired presentation acceptance is bound to another product');
  assert.equal(result.report.fixture,capability.fixture,'Paired result names another fixture');
  assert.deepEqual(result.report.scope,{game:profile.game,variant:profile.variant},'Paired Replay result is bound to another product');
  assert.deepEqual(result.report.input,{bytes:inputIdentity.bytes,sha256:inputIdentity.sha256},
    'Served paired Replay bytes do not match the selected diagnostic input');
  const runs=[baseline.report,controlled.report,cadence144.report];
  for(const [index,run] of runs.entries()){
    const role=index===0?'baseline':'controlled-extra-draw';
    const label=`${capability.cadencesHz[index]} Hz`;
    assert.equal(run.role,role,`Saved ${label} Replay capture has another role`);
    assert.deepEqual(run.scope,{game:profile.game,variant:profile.variant},`Saved ${label} capture is bound to another product`);
    assert.deepEqual(run.replayIdentity,{bytes:inputIdentity.bytes,sha256:inputIdentity.sha256},
      `Saved ${label} capture used different Replay bytes`);
    assert.equal(run.scheduler?.rateHz,capability.cadencesHz[index],`Saved ${label} capture used another callback cadence`);
    assert.equal(run.scheduler?.source,'GameHost::frame',`Saved ${label} capture did not use the production frame callback`);
    assert.equal(run.identity?.build?.wasm,serverBuild.wasm,`Saved ${label} capture used another diagnostic WASM`);
    assert.equal(run.identity?.build?.sourceDigest,serverBuild.sourceDigest,`Saved ${label} capture used another source set`);
    assert.equal(run.identity?.build?.diagnostic,true,`Saved ${label} capture is not from the diagnostic Runtime`);
    assert.equal(run.identity?.data?.bytes,productIdentities.data?.bytes,`Saved ${label} capture used another DATA byte count`);
    assert.equal(run.identity?.data?.sha256,productIdentities.data?.sha256,`Saved ${label} capture used another DATA input`);
  }
  assert.equal(new Set(runs.map(run=>run.epoch)).size,3,'Each cadence capture must use a fresh Runtime epoch');
  const productionEvidence=productionReportPath?readProductionPresentationEvidence(productionReportPath):null;
  const recomputed=buildPresentationPairedReport({runs,requiredTicks:capability.ticks,ratesHz:capability.cadencesHz,
    scope:{game:profile.game,variant:profile.variant},expectedAuthorityGroups:capability.expectedAuthorityGroups,
    cadenceDifferenceGroups:capability.cadenceDifferenceGroups,fixture:capability.fixture,input:inputIdentity,productionEvidence});
  assert.ok(JSON.stringify(result.report)===JSON.stringify(recomputed),
    'Saved paired comparison does not reproduce from its retained native and PCM captures');
  for(const run of runs){
    assert.equal(run.capture?.requestedTicks,capability.ticks,'A cadence run requested another logical-tick extent');
    assert.equal(run.capture?.completedTicks,capability.ticks,'A cadence run did not retain every logical tick');
    assert.equal(run.samples?.length,capability.ticks,'A cadence run is missing raw Replay samples');
  }
  for(const row of acceptance.report.checks){
    assert.ok(['passed','failed','incomplete','blocked'].includes(row.status),`Invalid paired check status: ${row.id}`);
    assert.equal(typeof row.complete,'boolean',`Paired check has no complete flag: ${row.id}`);
    assert.equal(typeof row.machineComplete,'boolean',`Paired check has no machineComplete flag: ${row.id}`);
    assert.deepEqual(row.evidence?.adaptationIds,[row.id],`Paired check is not bound to its own adaptation ID: ${row.id}`);
  }
  return{status:acceptance.report.status,report:acceptance.report,
    acceptance:acceptance.identity,pair:result.identity,productionReport:productionEvidence?.report??null,
    display:Object.fromEntries(await Promise.all(Object.entries({observations:'presentation-observations.json',
      scan:'display05-pool-reuse-scan.json',titleMaterial:'material-display05-title-before.json',
      poolMaterial:'material-display05-pool-reuse-boundary.json',capture:'browser-captures.json',
      continuousMaterial:'material-continuous-motion.json',pauseMaterial:'material-pause-menu.json',
      resumedMaterial:'material-pause-resumed.json',materials:'visual-materials.json'})
      .map(async([key,name])=>[key,(await readJsonArtifact(resolve(evidenceRoot,name))).identity]))),
    runs:{baseline:baseline.identity,controlledExtraDraw:controlled.identity,cadence144:cadence144.identity},input:inputIdentity,
    capability:{fixture:capability.fixture,inputFile:capability.inputFile,ticks:capability.ticks,
      cadencesHz:[...capability.cadencesHz],cadenceDifferenceGroups:[...capability.cadenceDifferenceGroups]}};
}

async function runLane({lane,profile,server,product,productIdentities,targetId,output,manifestPath,pairedReplayRequested=false,pairedReplayBlockedReason=null}){
  const evidenceRoot=resolve(output,'evidence',lane),runnerRoot=resolve(output,'runner',lane);
  await mkdir(dirname(evidenceRoot),{recursive:true});
  await mkdir(dirname(runnerRoot),{recursive:true});
  let tasks=[];
  let productionReportPath=null;
  if(lane==='presentation'){
    const extraArgs=[];
    if(pairedReplayRequested&&!pairedReplayBlockedReason){
      const capability=profile.presentation.pairedReplay;
      const productionOutput=resolve(output,'production-runtime');
      productionReportPath=resolve(productionOutput,'report.json');
      tasks.push(nodeTask({id:'presentation-production-runtime',script:resolve(repoRoot,'tools/ports/runtime-browser.mjs'),
        args:[`--manifest=${manifestPath}`,`--game=${profile.game}`,'--variant=normal','--gamepad',`--output=${productionOutput}`,
          `--presentation-replay=${resolve(product.inputRoot,capability.inputFile)}`,`--presentation-ticks=${capability.ticks}`],timeoutMs:15*60*1000}));
      extraArgs.push('--production-report',productionReportPath);
      extraArgs.push('--paired-replay','--replay-fixture',capability.fixture,'--paired-ticks',String(capability.ticks));
      extraArgs.push('--paired-cadences',capability.cadencesHz.join(','),
        '--cadence-difference-groups',capability.cadenceDifferenceGroups.join(','),
        '--expected-authority-groups',String(capability.expectedAuthorityGroups));
    }
    tasks.push(browserTask({id:'presentation-browser',script:profile.browserVerifierAbsolute,url:server.url,
      evidenceRoot,timeoutMs:80*60*1000,extraArgs}));
  }else if(lane==='quick'){
     tasks=[
      pythonTask({id:'quick-capture',script:profile.quickCaptureAbsolute,
        args:['--fixtures',String(profile.quickFixtureCount),'--url',server.url,'--output',evidenceRoot],timeoutMs:60*60*1000}),
      nodeTask({id:'quick-gate',script:profile.quickGateAbsolute,args:['--capture-root',evidenceRoot,
        '--input-root',product.inputRoot,'--golden-root',dirname(profile.goldenManifestAbsolute),
        '--report',resolve(evidenceRoot,'gate-report.json')],timeoutMs:60*60*1000}),
    ];
  }else{
    tasks=[...profile.dailyFixtures.map(fixture=>pythonTask({id:`capture-${fixture.id}`,script:profile.dailyCaptureAbsolute,
      args:['--replay',product.replayFiles[fixture.replayKey],'--start-stage',String(fixture.stage),'--url',server.url,
        '--output',resolve(evidenceRoot,`${fixture.id}.json`)],timeoutMs:90*60*1000})),
      nodeTask({id:'daily-gate',script:profile.dailyGateAbsolute,args:['--capture-root',evidenceRoot,
        ...profile.dailyGateReplayOptions.flatMap(item=>[item.option,product.replayFiles[item.replayKey]]),
        '--golden-root',dirname(profile.goldenManifestAbsolute),'--report',resolve(evidenceRoot,'gate-report.json')],
        timeoutMs:90*60*1000})];
  }
  const run=await runTasks(tasks,{output:runnerRoot,concurrency:1,failFast:false,
    metadata:{game:profile.game,variant:profile.variant,lane}});
  const laneReport={lane,game:profile.game,variant:profile.variant,targetId,evidenceRoot,runnerReport:run,
    runnerArtifact:{path:resolve(runnerRoot,'report.json'),...await fileIdentity(resolve(runnerRoot,'report.json'))},
    productInputs:productIdentities,status:'failed',complete:false};
  if(lane==='presentation'){
    const task=run.results.find(item=>item.id==='presentation-browser');
    try{
      const capturePath=resolve(evidenceRoot,'browser-captures.json');
      const {report:capture,identity:captureIdentity}=await readJsonArtifact(capturePath);
      laneReport.observation={game:laneReport.game,variant:laneReport.variant,
        adaptationIds:DIAGNOSTIC_LANE_CHECKS.presentation,
        observation:{kind:'presentation-lab-native-capture',artifact:{path:capturePath,...captureIdentity},
          build:capture.build,reports:capture.reports,errors:capture.errors}};
      laneReport.scope=profile.presentation.scope;
       laneReport.capture=assessPresentationCapture(capture,server.build,profile.presentation);
      laneReport.browserCapturePath=capturePath;
      laneReport.status=task?.status==='passed'&&laneReport.capture.status==='passed'?'passed':'failed';
      laneReport.complete=laneReport.status==='passed';
      if(pairedReplayRequested){
        if(pairedReplayBlockedReason){
          laneReport.pairedReplay={status:'blocked',reason:pairedReplayBlockedReason,
            requiredChecks:PAIRED_PRESENTATION_CHECKS};
          laneReport.status='incomplete';laneReport.complete=false;
        }else{
          const pairArtifacts=await presentationPairArtifacts(evidenceRoot,product,profile,server.build,productIdentities,productionReportPath);
          laneReport.pairedReplay=pairArtifacts;
          laneReport.observation.pairedPresentation=pairArtifacts;
          const acceptance=pairArtifacts.report;
          laneReport.coverage={checks:acceptance.checks.map(row=>({id:row.id,status:row.status,complete:row.complete,
            machineComplete:row.machineComplete})),humanReview:acceptance.humanReview};
          if(acceptance.status==='failed')laneReport.status='failed';
          else if(acceptance.status!=='passed')laneReport.status='incomplete';
          laneReport.complete=acceptance.status==='passed'&&acceptance.complete===true;
        }
      }
    }catch(error){laneReport.error=String(error.message??error);}
    if(!laneReport.coverage?.checks)laneReport.coverage={checks:[{id:profile.presentation.checkId,status:laneReport.status,complete:laneReport.complete}],
      partial:profile.presentation.partialChecks};
  }else if(lane==='quick'){
    try{
      const capturePath=resolve(evidenceRoot,'results.json'),gatePath=resolve(evidenceRoot,'gate-report.json');
      const [{report:capture,identity:captureIdentity},{report:gate,identity:gateIdentity}]=await Promise.all([
        readJsonArtifact(capturePath),readJsonArtifact(gatePath)]);
      const fixtureRows=[];
      for(const fixture of profile.quickInputs){
        const rawCapturePath=resolve(evidenceRoot,fixture.captureFile);
        const {report:rawCapture,identity:rawCaptureIdentity}=await readJsonArtifact(rawCapturePath);
        const input=productIdentities.quickInputs.find(item=>item.id===fixture.id&&item.index===fixture.index);
        const suiteFixture=capture.fixtures?.find(row=>row.fixture===fixture.index);
        const comparison=gate.results?.find(row=>row.id===fixture.id);
        assert.ok(input,`${fixture.id}: selected input identity is missing`);
        assert.ok(suiteFixture,`${fixture.id}: quick capture suite omitted its fixture row`);
        assert.equal(rawCapture.schema,profile.quickCaptureSchema,`${fixture.id}: unexpected capture schema`);
        assert.equal(rawCapture.fixture,fixture.index,`${fixture.id}: raw capture belongs to another fixture`);
        assert.deepEqual(rawCapture.errors,[],`${fixture.id}: capture reports browser errors`);
        assertDiagnosticCaptureBuild(rawCapture.build,server.build);
        assert.equal(rawCapture.replay?.sha256,input.sha256,`${fixture.id}: captured Replay differs from the selected input`);
        assert.equal(rawCapture.replay?.bytes,input.bytes,`${fixture.id}: captured Replay size differs from the selected input`);
        assert.ok(Array.isArray(rawCapture.rows)&&rawCapture.rows.length>0,`${fixture.id}: native gameplay rows are missing`);
        assert.equal(suiteFixture.ticks,rawCapture.rows.length,`${fixture.id}: suite tick count differs from its native capture`);
        assert.equal(suiteFixture.complete,rawCapture.complete,`${fixture.id}: suite and raw capture completion differ`);
        assert.deepEqual(suiteFixture.errors,[],`${fixture.id}: suite metadata reports browser errors`);
        assert.ok(comparison,`${fixture.id}: immutable Replay gate omitted its comparison result`);
        fixtureRows.push({id:fixture.id,index:fixture.index,input:{path:input.path,bytes:input.bytes,sha256:input.sha256},
          capture:{path:rawCapturePath,...rawCaptureIdentity,replay:rawCapture.replay,build:rawCapture.build,complete:rawCapture.complete,
            stopReason:rawCapture.stopReason,terminalScene:rawCapture.terminalScene,
            rows:{count:rawCapture.rows.length,first:rawCapture.rows[0],last:rawCapture.rows.at(-1)}},
          comparison});
      }
      laneReport.observation={game:laneReport.game,variant:laneReport.variant,adaptationIds:DIAGNOSTIC_LANE_CHECKS.quick,
        observation:{kind:'quick-immutable-replay-comparison',capture:{path:capturePath,...captureIdentity,report:capture},
          fixtures:fixtureRows,gate:{path:gatePath,...gateIdentity,report:gate}}};
      laneReport.scope=profile.laneScope.quick;
      const expected=profile.quickInputs.map(row=>row.index);
      const expectedIds=profile.quickInputs.map(row=>row.id);
      assert.deepEqual(capture.fixtures?.map(row=>row.fixture),expected,
        `Quick capture must contain all ${profile.quickFixtureCount} profile-selected demos`);
      assert.ok(capture.fixtures.every(row=>row.complete===true&&row.errors?.length===0&&row.ticks>0),
        'Every quick Replay capture must reach its real terminal');
      assert.equal(gate.suite,'quick','Quick gate report lane identity mismatch');
      assert.equal(gate.complete,true,'Quick immutable Replay comparison is incomplete');
      assert.deepEqual(gate.expectedIds,expectedIds,'Quick gate has an unexpected fixture set');
      assert.deepEqual(gate.results?.map(row=>row.id),expectedIds,'Quick gate results do not match the selected fixture order');
      assert.ok(gate.results?.length===profile.quickFixtureCount&&gate.results.every(row=>row.status==='PASS'),
        'Every profile-selected quick Replay comparison must pass');
      laneReport.capturePath=resolve(evidenceRoot,'results.json');laneReport.gatePath=resolve(evidenceRoot,'gate-report.json');
      laneReport.gate={status:'passed',complete:true,fixtures:gate.results.map(row=>row.id)};
      laneReport.status=allTaskResultsPassed(run)?'passed':'failed';laneReport.complete=laneReport.status==='passed';
    }catch(error){laneReport.error=String(error.message??error);}
    laneReport.coverage={checks:[{id:DIAGNOSTIC_LANE_CHECKS.quick[0],status:laneReport.status,complete:laneReport.complete}],
      partial:profile.quickPartialChecks};
  }else{
    try{
      const gatePath=resolve(evidenceRoot,'gate-report.json');
      const {report:gate,identity:gateIdentity}=await readJsonArtifact(gatePath);
      const captureRows=[];
      for(const fixture of profile.dailyFixtures){
        const capturePath=resolve(evidenceRoot,fixture.captureFile);
        const {report:capture,identity:captureIdentity}=await readJsonArtifact(capturePath);
        assert.equal(capture.schema,profile.dailyCaptureSchema,`${fixture.id}: unexpected capture schema`);
        assert.deepEqual(capture.errors,[],`${fixture.id}: Replay capture has browser errors`);
        assertDiagnosticCaptureBuild(capture.build,server.build);
        const identity=productIdentities.dailyReplays[fixture.replayKey];
        assert.equal(capture.replay?.sha256,identity.sha256,`${fixture.id}: capture uses another Replay file`);
        assert.equal(capture.replay?.bytes,identity.bytes,`${fixture.id}: captured Replay size changed`);
        const comparison=gate.results?.find(row=>row.id===fixture.id);
        assert.ok(comparison,`${fixture.id}: immutable Replay gate omitted its comparison result`);
        captureRows.push({id:fixture.id,stage:fixture.stage,ticks:capture.rows?.ticks,sha256:capture.replay.sha256,
          capture:{path:capturePath,...captureIdentity,report:capture},comparison});
      }
      laneReport.observation={game:laneReport.game,variant:laneReport.variant,adaptationIds:DIAGNOSTIC_LANE_CHECKS.daily,
        observation:{kind:'daily-immutable-replay-comparison',captures:captureRows,
          gate:{path:gatePath,...gateIdentity,report:gate}}};
      laneReport.scope=profile.laneScope.daily;
      assert.equal(gate.suite,'daily','Daily gate report lane identity mismatch');
      assert.equal(gate.complete,true,'Daily immutable Replay comparison is incomplete');
      const expectedIds=profile.dailyFixtures.map(item=>item.id);
      assert.deepEqual(gate.expectedIds,expectedIds,'Daily gate has an unexpected fixture set');
      assert.deepEqual(gate.results?.map(row=>row.id),expectedIds,'Daily gate results do not match the selected fixture order');
      assert.ok(gate.results?.length===profile.dailyFixtures.length&&gate.results.every(row=>row.status==='PASS'),
        'Every daily Replay comparison must pass');
      laneReport.captures=captureRows;laneReport.gatePath=resolve(evidenceRoot,'gate-report.json');
      laneReport.gate={status:'passed',complete:true,fixtures:gate.results.map(row=>row.id)};
      laneReport.status=allTaskResultsPassed(run)?'passed':'failed';laneReport.complete=laneReport.status==='passed';
    }catch(error){laneReport.error=String(error.message??error);}
    laneReport.coverage={checks:[{id:DIAGNOSTIC_LANE_CHECKS.daily[0],status:laneReport.status,complete:laneReport.complete}],
      partial:profile.dailyPartialChecks};
  }
  if(laneReport.status!=='passed')laneReport.complete=false;
  return laneReport;
}

const result=await runWithReport({output,schema:'eagler-touhou/diagnostic-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['manifest','diagnostics','game','variant','lanes','output'],flags:['paired-replay']});
  const manifestPath=arg('manifest'),diagnosticsPath=arg('diagnostics'),game=arg('game'),variant=arg('variant');
  const pairedReplayRequested=process.argv.includes('--paired-replay');
  assert.ok(manifestPath,'Required: --manifest=RuntimeInput.json');
  assert.ok(diagnosticsPath,'Required: --diagnostics=privateDiagnostic.json');
  assert.ok(PRODUCT_GAMES[game],`Unsupported game ${game}`);
  assert.ok(['normal','multiplayer'].includes(variant),'--variant must be normal or multiplayer');
  const lanes=parseLanes(arg('lanes'));
  if(pairedReplayRequested)assert.ok(lanes.includes('presentation'),'--paired-replay requires --lanes including presentation');
  const profile=resolveDiagnosticProfile(game,variant);
  assert.ok(profile,`No diagnostic browser profile is registered for ${game}/${variant}`);
  validateProfileLanes(profile,lanes);
  const pairedCapability=pairedPresentationCapability(profile);
  let pairedReplayBlockedReason=pairedReplayRequested&&pairedCapability.status!=='ready'?pairedCapability.reason:null;
  let pairedReplay=pairedReplayRequested&&!pairedReplayBlockedReason?pairedCapability.capability:null;
  const plannedChecks=plannedDiagnosticChecks(lanes,DIAGNOSTIC_LANE_CHECKS);
  if(pairedReplayRequested)for(const id of PAIRED_PRESENTATION_CHECKS)
    if(!plannedChecks.some(check=>check.id===id))plannedChecks.push({id,lane:'presentation-paired',status:'not-run',complete:false});
  const targetRecord={game,variant,id:null,status:'not-run',passed:false,checks:plannedChecks};
  report.targets=[targetRecord];
  report.scope={game,variant,lanes,laneDescriptions:Object.fromEntries(lanes.map(lane=>[lane,profile.laneScope[lane]])),
    pairedReplay:pairedReplayRequested?{requested:true,status:pairedReplay?'ready':'blocked',capability:pairedReplay??null,
      requestedCapability:pairedCapability.capability??null,
      reason:pairedReplayBlockedReason}:null};
  report.coverage={complete:false,selectedChecks:targetRecord.checks.map(check=>check.id),pairedReplay:pairedReplayRequested};
  const runtimeConfig=JSON.parse(await readFile(resolve(manifestPath),'utf8'));
  const selection=selectRuntimeTargets(runtimeConfig,{game,variant});
  const target=selection.targets[0];
  targetRecord.id=target.id;
  const diagnostic=await loadDiagnosticInputs(diagnosticsPath,{game,variant});
  const product=diagnostic.product;
  const manifestData=resolve(target.data),sameDataPath=process.platform==='win32'?
    manifestData.toLowerCase()===product.data.toLowerCase():manifestData===product.data;
  assert.ok(sameDataPath,
    `${game}/${variant}: diagnostic DATA must be the same file selected by the Runtime manifest`);
  let pairedReplayInputIdentity=null;
  if(pairedReplay){
    try{
      pairedReplayInputIdentity=await fileIdentity(resolve(product.inputRoot,pairedReplay.inputFile),'Paired presentation Replay input');
    }catch(error){
      pairedReplayBlockedReason=`Selected paired Replay input is unavailable: ${error.message}`;
      pairedReplay=null;
      report.scope.pairedReplay.status='blocked';report.scope.pairedReplay.capability=null;
      report.scope.pairedReplay.reason=pairedReplayBlockedReason;
    }
  }
  const [runtimeIdentity,dataIdentity,productIdentities]=await Promise.all([
    verifyRuntimePackage(target,{repoRoot}),verifyPrivateData(target.data),
    inspectDiagnosticProduct(product,{quickInputs:profile.quickInputs,dailyFixtures:profile.dailyFixtures,lanes,
      pairedReplay}),
  ]);
  if(pairedReplay&&pairedReplayInputIdentity){
    assert.deepEqual({bytes:productIdentities.pairedReplay?.bytes,sha256:productIdentities.pairedReplay?.sha256},
      {bytes:pairedReplayInputIdentity.bytes,sha256:pairedReplayInputIdentity.sha256},
      'Paired Replay input changed while diagnostic inputs were inspected');
  }
  const profilePaths=await resolveProfilePaths(profile,lanes);
  const profileIdentity={};
  for(const key of profilePathKeys(lanes).filter(key=>!['sourceRoot','presentationDirectory','runtimeDirectory'].includes(key)))
    profileIdentity[key]=await fileIdentity(profilePaths[`${key}Absolute`]);
  const dependencies=await dependencySnapshot(repoRoot,[fileURLToPath(import.meta.url),
    'tests/adaptation/adapters/diagnostics.mjs']);
  const absoluteManifestPath=resolve(manifestPath),absoluteDiagnosticsPath=resolve(diagnosticsPath);
  Object.assign(report,{selection:{game,variant,targetId:target.id},
    evidence:{game,variant,targetId:target.id,runtime:runtimeIdentity,manifestData:dataIdentity,
      diagnosticInputs:productIdentities,labBuild:null,lanes:[]},
    profile:{schema:profile.schema,game,variant,files:profileIdentity,
      buildReceipt:profile.buildReceipt,sourceRoot:profile.sourceRoot,presentationDirectory:profile.presentationDirectory},dependencies});
  const preSnapshot=await createDiagnosticSnapshot({manifestPath:absoluteManifestPath,diagnosticsPath:absoluteDiagnosticsPath,
    game,variant,lanes,expectedTargetId:target.id,pairedReplay});
  await mkdir(resolve(output,'identity'),{recursive:true});
  report.identity={status:'captured',stable:false,pre:await saveDiagnosticSnapshot(resolve(output,'identity'),'pre',preSnapshot),post:null};
  await save();
  let server=null;
  try{
    try{server=await startLabServer({script:profilePaths.serverScriptAbsolute,profile:{...profile,...profilePaths},product,output});
      report.evidence.labBuild=server.build;report.evidence.labRuntime=server.packageIdentity;
      report.server={url:server.url,port:server.port,status:'ready',clearedSnapshotEnvironment:server.clearedSnapshotEnvironment};await save();}
    catch(error){report.server={status:'failed',error:String(error.stack||error)};}
    for(const lane of lanes){
      if(!server){
        const laneReport={lane,game,variant,targetId:target.id,status:'failed',complete:false,
          error:'Presentation Lab server was not ready'};
        report.evidence.lanes.push(laneReport);
        recordDiagnosticLane(targetRecord,lane,laneReport,{checkIds:DIAGNOSTIC_LANE_CHECKS[lane],
          primaryCheckId:lane==='presentation'?profile.presentation.checkId:DIAGNOSTIC_LANE_CHECKS[lane][0]});
        if(lane==='presentation'&&pairedReplayRequested)
          recordPairedPresentationChecks(targetRecord,null,pairedReplayBlockedReason?'blocked':'ready',laneReport.error);
        await save();continue;
      }
      try{
        const laneReport=await runLane({lane,profile:{...profile,...profilePaths},server,product,
          productIdentities,targetId:target.id,output,manifestPath:absoluteManifestPath,pairedReplayRequested,
          pairedReplayBlockedReason});
        report.evidence.lanes.push(laneReport);
        recordDiagnosticLane(targetRecord,lane,laneReport,{checkIds:DIAGNOSTIC_LANE_CHECKS[lane],
          primaryCheckId:lane==='presentation'?profile.presentation.checkId:DIAGNOSTIC_LANE_CHECKS[lane][0]});
        if(lane==='presentation'&&pairedReplayRequested){
          const paired=laneReport.pairedReplay;
          recordPairedPresentationChecks(targetRecord,paired?.report??null,
            paired?.status==='blocked'?'blocked':'ready',paired?.reason??laneReport.error,
            paired?{acceptance:paired.acceptance,pair:paired.pair,runs:paired.runs,input:paired.input,
              capability:paired.capability,display:paired.display,productionReport:paired.productionReport}:null);
        }
        await save();
      }catch(error){
        const laneReport={lane,game,variant,targetId:target.id,status:'failed',complete:false,error:String(error.stack||error)};
        report.evidence.lanes.push(laneReport);
        recordDiagnosticLane(targetRecord,lane,laneReport,{checkIds:DIAGNOSTIC_LANE_CHECKS[lane],
          primaryCheckId:lane==='presentation'?profile.presentation.checkId:DIAGNOSTIC_LANE_CHECKS[lane][0]});
        if(lane==='presentation'&&pairedReplayRequested)
          recordPairedPresentationChecks(targetRecord,null,pairedReplayBlockedReason?'blocked':'ready',laneReport.error);
        await save();
      }
    }
  }finally{
    if(server){
      try{
        report.server.shutdown=await server.stop();
        if(!report.server.shutdown.exited||!report.server.shutdown.normal)
          throw new Error(`Lab server did not stop normally: ${JSON.stringify(report.server.shutdown)}`);
      }
      catch(error){report.server.shutdownError=String(error.stack||error);targetRecord.error=`Lab server shutdown failed: ${error.message}`;}
      await save();
    }
    try{
      const postSnapshot=await createDiagnosticSnapshot({manifestPath:absoluteManifestPath,diagnosticsPath:absoluteDiagnosticsPath,
        game,variant,lanes,expectedTargetId:target.id,pairedReplay});
      report.identity.post=await saveDiagnosticSnapshot(resolve(output,'identity'),'post',postSnapshot);
      report.identity.stable=preSnapshot.fingerprint===postSnapshot.fingerprint;
      report.identity.status=report.identity.stable?'stable':'changed';
      if(!report.identity.stable){
        targetRecord.error='Selected Runtime, diagnostic inputs, Lab build, source files, tools or golden inputs changed during execution';
        for(const check of targetRecord.checks){check.status='failed';check.complete=false;check.error=targetRecord.error;}
      }
    }catch(error){
      report.identity.postError=String(error.stack||error);report.identity.status='failed';report.identity.stable=false;
      targetRecord.error=`Post-run diagnostic identity snapshot failed: ${error.message}`;
      for(const check of targetRecord.checks){check.status='failed';check.complete=false;check.error=targetRecord.error;}
    }
    recordDiagnosticReferenceChecks(report,{game,variant});
    finalizeDiagnosticTarget(targetRecord,report.evidence.lanes,lanes,
      report.identity.stable&&!report.server?.shutdownError);
    if(pairedReplayRequested&&targetRecord.checks.some(check=>PAIRED_PRESENTATION_CHECKS.includes(check.id)&&
      (check.complete!==true||check.status!=='passed'))){
      if(targetRecord.status!=='failed')targetRecord.status='incomplete';
      targetRecord.passed=false;
    }
    finalizeDiagnosticReport(report,targetRecord);
    await save();
  }
});

const passed=result.passed===true;
console.log(JSON.stringify({status:result.status,passed,game:result.selection?.game,variant:result.selection?.variant,
  lanes:result.evidence?.lanes?.map(lane=>({lane:lane.lane,status:lane.status,complete:lane.complete}))??[]}));
if(!passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
