import {execFileSync} from 'node:child_process';
import {createHash,randomUUID} from 'node:crypto';
import {readFile,mkdir,writeFile,rename} from 'node:fs/promises';
import {basename,dirname,resolve} from 'node:path';
import {runTasks} from './runner.mjs';
import {oracleInputs} from './oracle-inputs.mjs';
import {acceptedReferenceSnapshot,acceptedReferenceSnapshotsEqual,inputSnapshotsEqual,loadOracleSuite,
  maintenanceOutputPath,preflightOracleInputs,preflightOracleSuite} from './oracle-suite.mjs';
import {nativeGroupEnvironment,nativeInputSnapshot,nativeInputSnapshotsEqual,validateNativeEnvironmentPolicy} from './native-inputs.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const sorted=values=>[...values].sort();
const sameList=(actual,expected)=>Array.isArray(actual)&&JSON.stringify(sorted(actual))===JSON.stringify(sorted(expected));

function gitRoot(root) {
  return execFileSync('git',['rev-parse','--show-toplevel'],{cwd:root,encoding:'utf8',windowsHide:true}).trim();
}

function gitRevision(root) {
  try {return execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8',windowsHide:true}).trim();}
  catch {return null;}
}

function assertBuildMatchesNativeSnapshot(manifest,snapshot) {
  const files=new Map(snapshot.details.filter(entry=>entry.kind==='file').map(entry=>[entry.path,entry.sha256]));
  const mismatched=Object.entries(manifest.sourceHashes??{}).filter(([path,sha256])=>files.get(path)!==sha256).map(([path])=>path);
  if (mismatched.length) throw new Error(`Native snapshot does not match the built core receipt: ${mismatched.join(', ')}`);
}

function scopeForRun(suite,group,receiptOverrides,environment,runtime={}) {
  const scope=nativeGroupEnvironment(suite,group,receiptOverrides,environment);
  Object.assign(scope.environment,runtime);
  Object.assign(scope.effective,runtime);
  return scope;
}

async function buildInputs(root,manifest) {
  const target=JSON.parse(await readFile(resolve(root,'target.json'),'utf8'));
  if (!['th09','th11','th15'].includes(target.game)) throw new Error(`Unsupported oracle target: ${target.game}`);
  if (manifest.kind!==`${target.game}-original-comparison-fixture`||manifest.completeGame!==false)
    throw new Error('Core/build target identity mismatch');
  const expected=oracleInputs(root,{portableSdlHeaders:target.game==='th15'});
  for (const field of ['sources','headers','controlFiles']) {
    if (!sameList(manifest[field],expected[field])) throw new Error(`Native build inventory mismatch: ${field}`);
  }
  const expectedKeys=sorted([...expected.sources,...expected.headers,...expected.controlFiles]);
  if (!manifest.sourceHashes||typeof manifest.sourceHashes!=='object'||Array.isArray(manifest.sourceHashes)||
      JSON.stringify(sorted(Object.keys(manifest.sourceHashes)))!==JSON.stringify(expectedKeys))
    throw new Error('Build manifest source hash key set mismatch');
  const changed=[];
  for (const path of expectedKeys) {
    if (hash(await readFile(resolve(root,path)))!==manifest.sourceHashes[path]) changed.push(path);
  }
  return {expected,changed};
}

async function coreEvidence(root) {
  const core=resolve(root,'artifacts/cpp/game-core-test.wasm');
  const manifest=JSON.parse(await readFile(resolve(root,'artifacts/cpp/game-core-test-build.json'),'utf8'));
  const coreSha256=hash(await readFile(core));
  if (coreSha256!==manifest.sha256) throw new Error('Core/build manifest mismatch');
  const inputs=await buildInputs(root,manifest);
  if (inputs.changed.length) throw new Error(`Sources changed after build: ${inputs.changed.join(', ')}`);
  return {core,coreSha256,manifest,inputs};
}

function countsFromLog(text) {
  const count=key=>Number(text.match(new RegExp('(?:^|\\n)(?:ℹ )?'+key+' ([0-9.]+)'))?.[1]??0);
  return Object.fromEntries(['tests','pass','fail','cancelled','skipped','todo'].map(key=>[key,count(key)]));
}

function blockedInputsEqual(before=[],after=[]) {
  const normalize=items=>items.map(({input,reason})=>({input,reason})).sort((a,b)=>
    a.input.localeCompare(b.input)||a.reason.localeCompare(b.reason));
  return JSON.stringify(normalize(before))===JSON.stringify(normalize(after));
}

function validEvidenceName(name) { return typeof name==='string'&&/^[a-z0-9][a-z0-9-]*$/.test(name); }

async function writeJsonAtomic(path,value,runId) {
  const pending=`${path}.${runId}.pending`;
  await writeFile(pending,JSON.stringify(value,null,2)+'\n');
  await rename(pending,path);
}

/** Pure read-only preflight for a title's native candidate and private-input groups. */
export async function preflightFrozenOracle({root,environment=process.env}) {
  const suite=await loadOracleSuite(root);
  const evidence=await coreEvidence(root);
  const groups=await preflightOracleSuite(root,environment);
  const nativeInputs=await nativeInputSnapshot(root,suite,environment);
  assertBuildMatchesNativeSnapshot(evidence.manifest,nativeInputs);
  return {...groups,coreSha256:evidence.coreSha256,buildInputs:{sources:evidence.inputs.expected.sources.length,
    headers:evidence.inputs.expected.headers.length,controlFiles:evidence.inputs.expected.controlFiles.length,
    changed:evidence.inputs.changed},nativeInputs};
}

/** Execute only manifest-classified candidate groups. Missing private inputs block that group and never suppress ready groups. */
export async function runFrozenOracle({root,name='full-suite',concurrency=2,timeoutMs=7200000,environment=process.env}) {
  if (!validEvidenceName(name)) throw new Error('Invalid evidence name');
  if (!Number.isSafeInteger(concurrency)||concurrency<1||concurrency>4) throw new Error('Test concurrency must be 1..4');
  const directory=resolve(root,'artifacts/cpp/verification');
  await mkdir(directory,{recursive:true});
  const resultPath=resolve(directory,name+'-result.json'),runId=randomUUID(),startedAt=new Date().toISOString();
  const running={schema:'eagler-touhou/native-oracle-result/1',runId,status:'running',passed:false,exitCode:1,
    owner:basename(dirname(resolve(root))),name,startedAt,baselineChanged:null,protectedInputsChanged:null};
  await writeJsonAtomic(resultPath,running,runId);
  try {
  const suite=await loadOracleSuite(root),repository=gitRoot(root);
  const headBefore=gitRevision(repository);
  const suitePreflight=await preflightOracleSuite(root,environment);
  const evidence=await coreEvidence(root),core=evidence.core,coreSha256=evidence.coreSha256;
  const nativeBefore=await nativeInputSnapshot(root,suite,environment);
  assertBuildMatchesNativeSnapshot(evidence.manifest,nativeBefore);
  const preflightById=new Map(suitePreflight.groups.map(group=>[group.id,group]));
  const tests=suite.tests.filter(item=>item.role==='candidate');
  const testHashes=Object.fromEntries(await Promise.all(suite.discovered.map(async file=>
    [file,hash(await readFile(resolve(root,'tests/cpp',file)))])));
  await writeFile(resolve(directory,name+'-build.json'),JSON.stringify({kind:evidence.manifest.kind,completeGame:evidence.manifest.completeGame,
    game:suite.game,variant:suite.variant,coreSha256,tests:suite.tests,groups:suite.groups,maintenance:suite.maintenance??[],testHashes},null,2)+'\n');

  const baselineBefore=await acceptedReferenceSnapshot(root,suite);
  const runnable=suite.candidateGroups.filter(group=>preflightById.get(group.id)?.status==='ready');
  const tasks=runnable.map(group=>({id:group.id,command:process.execPath,
    args:['--test','--test-concurrency='+concurrency,'--test-reporter=spec',...group.tests.map(file=>resolve(root,'tests/cpp',file))],
    cwd:root,timeoutMs,environment:scopeForRun(suite,group,preflightById.get(group.id)?.envOverrides??{},environment,
      {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:core}).environment}));
  let taskReport=null;
  const nativePolicy=validateNativeEnvironmentPolicy(suite),baseEnvironment={...environment};
  for (const key of nativePolicy.clearInherited) delete baseEnvironment[key];
  if (tasks.length) taskReport=await runTasks(tasks,{output:resolve(directory,name),concurrency:1,environment:{...baseEnvironment,
    NO_COLOR:'1',NODE_DISABLE_COLORS:'1',TH11_CORE_TEST_FILE:core,EAGLER_ORACLE_ROOT:root},
    unsetEnvironment:nativePolicy.clearInherited,
    metadata:{schema:'eagler-touhou/native-oracle-groups/1',owner:suite.owner}});
  const taskById=new Map((taskReport?.results??[]).map(task=>[task.id,task]));
  const postflightCache=new Map(),postflightById=new Map();
  for (const group of suite.candidateGroups) {
    const initial=preflightById.get(group.id),groupEnvironment=scopeForRun(suite,group,initial?.envOverrides??{},environment,
      {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:core});
    const after=await preflightOracleInputs(root,suite,[...suite.baseInputs,...group.requiredInputs],groupEnvironment.environment,postflightCache);
    const protectedInputsChanged=!inputSnapshotsEqual(initial?.inputs??[],after.inputs)||!blockedInputsEqual(initial?.blocked??[],after.blocked);
    postflightById.set(group.id,{after,protectedInputsChanged});
  }
  const groups=[];
  for (const group of suite.candidateGroups) {
    const initial=preflightById.get(group.id),task=taskById.get(group.id),postflight=postflightById.get(group.id),
      effectiveScope=scopeForRun(suite,group,initial?.envOverrides??{},environment,
        {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:core}).effective;
    if (initial?.status!=='ready') {
      groups.push({id:group.id,status:'blocked',testFiles:group.tests.length,blocked:initial?.blocked??[],tests:0,pass:0,fail:0,
        cancelled:0,skipped:0,todo:0,inputsBefore:initial?.inputs??[],inputsAfter:postflight?.after.inputs??[],
        protectedInputsChanged:postflight?.protectedInputsChanged??true,effectiveScope});
      continue;
    }
    const text=task?.log?await readFile(task.log,'utf8'):'';
    const counts=countsFromLog(text),afterInputs=postflight.after,privateInputsChanged=postflight.protectedInputsChanged;
    const passed=task?.status==='passed'&&counts.tests>0&&counts.pass===counts.tests&&counts.fail===0&&
      counts.cancelled===0&&counts.skipped===0&&counts.todo===0&&!privateInputsChanged;
    groups.push({id:group.id,status:passed?'passed':'failed',testFiles:group.tests.length,...counts,
      durationMs:task?.durationMs??0,exitCode:task?.code??null,log:task?.log??null,
      inputsBefore:initial.inputs,inputsAfter:afterInputs.inputs,privateInputsChanged,protectedInputsChanged:privateInputsChanged,
      inputErrors:afterInputs.blocked,effectiveScope});
  }

  const testChanges=[];
  for (const file of suite.discovered) {
    try {if (hash(await readFile(resolve(root,'tests/cpp',file)))!==testHashes[file]) testChanges.push(file);}
    catch {testChanges.push(file);}
  }
  let nativeAfter=null,nativeSnapshotError=null;
  try {nativeAfter=await nativeInputSnapshot(root,suite,environment);} catch (error) {nativeSnapshotError=error.message;}
  const nativeInputsChanged=!nativeInputSnapshotsEqual(nativeBefore,nativeAfter);
  const headAfter=gitRevision(repository);
  let baselineAfter=null,baselineError=null;
  try {baselineAfter=await acceptedReferenceSnapshot(root,suite);} catch(error) {baselineError=error.message;}
  const baselineChanged=!acceptedReferenceSnapshotsEqual(baselineBefore,baselineAfter);
  let inventoryChanged=false;
  try {inventoryChanged=JSON.stringify((await loadOracleSuite(root)).discovered)!==JSON.stringify(suite.discovered);}
  catch {inventoryChanged=true;}
  const counts=Object.fromEntries(['tests','pass','fail','cancelled','skipped','todo'].map(key=>
    [key,groups.reduce((sum,group)=>sum+group[key],0)]));
  const protectedInputsChanged=groups.some(group=>group.protectedInputsChanged);
  const passed=groups.length>0&&groups.every(group=>group.status==='passed')&&counts.tests>0&&counts.pass===counts.tests&&
      counts.fail===0&&counts.cancelled===0&&counts.skipped===0&&counts.todo===0&&
      hash(await readFile(core))===coreSha256&&!testChanges.length&&!inventoryChanged&&!nativeInputsChanged&&!baselineChanged&&!protectedInputsChanged;
  const result={...running,status:passed?'passed':'failed',finishedAt:new Date().toISOString(),passed,exitCode:passed?0:1,
    owner:suite.owner,game:suite.game,variant:suite.variant,
    discoveredFiles:suite.discovered.length,candidateFiles:tests.length,maintenanceFiles:suite.tests.length-tests.length,
    testFiles:groups.filter(group=>group.status==='passed'||group.status==='failed').reduce((sum,group)=>sum+group.testFiles,0),
    ...counts,durationMs:taskReport?.durationMs??0,coreSha256,buildInputs:{sources:evidence.inputs.expected.sources.length,
      headers:evidence.inputs.expected.headers.length,controlFiles:evidence.inputs.expected.controlFiles.length},
    groups,testChanges,inventoryChanged,nativeInputsChanged,nativeInputs:{before:nativeBefore,after:nativeAfter,error:nativeSnapshotError},
    head:{before:headBefore,after:headAfter},baselineChanged,protectedInputsChanged,
    acceptedReference:{before:baselineBefore,after:baselineAfter,error:baselineError},
    report:taskReport?resolve(directory,name,'report.json'):null,
    buildRecord:resolve(directory,name+'-build.json')};
  await writeJsonAtomic(resultPath,result,runId);
  return result;
  } catch(error) {
    const failed={...running,status:'failed',finishedAt:new Date().toISOString(),passed:false,exitCode:1,
      error:{name:error.name??'Error',message:error.message??String(error)}};
    await writeJsonAtomic(resultPath,failed,runId);
    throw error;
  }
}

/** Run one explicitly selected maintenance task into ignored staging only. */
export async function runOracleMaintenance({root,taskId,name,timeoutMs=7200000,environment=process.env}) {
  if (!validEvidenceName(taskId)||!validEvidenceName(name)) throw new Error('Maintenance task and evidence name are required');
  const suite=await loadOracleSuite(root),task=suite.maintenance?.find(item=>item.id===taskId);
  if (!task) throw new Error(`Unknown maintenance task: ${taskId}`);
  const policy=validateNativeEnvironmentPolicy(suite),scope=nativeGroupEnvironment(suite,task,{},environment),
    output=maintenanceOutputPath(root,task,name),preflight=await preflightOracleInputs(root,suite,
      [...suite.baseInputs,...(task.requiredInputs??[])],scope.environment);
  if (preflight.status!=='ready') return {passed:false,status:'blocked',exitCode:1,task:taskId,output,blocked:preflight.blocked,inputs:preflight.inputs};
  if(task.kind!=='test')throw new Error(`Maintenance task must use the Node test runner: ${taskId}`);
  const inputFile=resolve(root,'tests/cpp',task.file),args=['--test','--test-reporter=spec',inputFile];
  const repository=gitRoot(root),headBefore=gitRevision(repository),evidence=await coreEvidence(root),
    nativeBefore=await nativeInputSnapshot(root,suite,environment),baselineBefore=await acceptedReferenceSnapshot(root,suite);
  assertBuildMatchesNativeSnapshot(evidence.manifest,nativeBefore);
  const taskScope=scopeForRun(suite,task,preflight.envOverrides,environment,
    {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:evidence.core,EAGLER_MAINTENANCE_OUTPUT:output});
  const baseEnvironment={...environment};
  for (const key of policy.clearInherited) delete baseEnvironment[key];
  const report=await runTasks([{id:taskId,command:process.execPath,args,cwd:root,timeoutMs,
    environment:taskScope.environment}],
  {output:resolve(output,'logs'),concurrency:1,environment:{...baseEnvironment,NO_COLOR:'1',NODE_DISABLE_COLORS:'1',
    TH11_CORE_TEST_FILE:evidence.core,EAGLER_ORACLE_ROOT:root},unsetEnvironment:policy.clearInherited,
    metadata:{schema:'eagler-touhou/oracle-maintenance/1',owner:suite.owner,task:taskId}});
  const taskResult=report.results[0],log=taskResult.log?await readFile(taskResult.log,'utf8'):'',counts=countsFromLog(log),
    after=await preflightOracleInputs(root,suite,[...suite.baseInputs,...(task.requiredInputs??[])],taskScope.environment),
    nativeAfter=await nativeInputSnapshot(root,suite,environment),nativeInputsChanged=!nativeInputSnapshotsEqual(nativeBefore,nativeAfter),
    headAfter=gitRevision(repository);
  let baselineAfter=null,baselineError=null;
  try {baselineAfter=await acceptedReferenceSnapshot(root,suite);} catch(error) {baselineError=error.message;}
  const baselineChanged=!acceptedReferenceSnapshotsEqual(baselineBefore,baselineAfter);
  const passed=report.status==='passed'&&taskResult.status==='passed'&&counts.tests>0&&counts.pass===counts.tests&&
    counts.fail===0&&counts.cancelled===0&&counts.skipped===0&&counts.todo===0&&after.status==='ready'&&
    inputSnapshotsEqual(preflight.inputs,after.inputs)&&!nativeInputsChanged&&!baselineChanged;
  return {passed,status:passed?'passed':'failed',exitCode:passed?0:1,task:taskId,output,durationMs:report.durationMs,
    inputsBefore:preflight.inputs,inputsAfter:after.inputs,inputChanges:!inputSnapshotsEqual(preflight.inputs,after.inputs),
    nativeInputsChanged,nativeInputs:{before:nativeBefore,after:nativeAfter},head:{before:headBefore,after:headAfter},
    effectiveScope:taskScope.effective,baselineChanged,acceptedReference:{before:baselineBefore,after:baselineAfter,error:baselineError},
    ...counts,report:resolve(output,'logs','report.json'),log:taskResult.log};
}
