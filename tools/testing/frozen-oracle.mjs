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

const testEventReporter=new URL('./oracle-test-reporter.mjs',import.meta.url).href;

function normalizedPath(path) {
  const value=resolve(path).replaceAll('\\','/');
  return process.platform==='win32'?value.toLowerCase():value;
}

/** Return the exact test-runner arguments used by candidate and maintenance oracle tasks. */
export function oracleTestRunnerArgs({files,eventsPath,concurrency=1}) {
  if (!Array.isArray(files)||!files.length||files.some(file=>typeof file!=='string'))
    throw new Error('Oracle test runner requires manifest files');
  if (typeof eventsPath!=='string'||!eventsPath) throw new Error('Oracle test event path is required');
  if (!Number.isSafeInteger(concurrency)||concurrency<1||concurrency>32)
    throw new Error('Oracle test concurrency must be a positive integer');
  return ['--test','--test-concurrency='+concurrency,
    '--test-reporter=spec','--test-reporter-destination=stdout',
    '--test-reporter='+testEventReporter,'--test-reporter-destination='+resolve(eventsPath),
    ...files];
}

const emptyCounts=()=>({tests:0,pass:0,fail:0,cancelled:0,skipped:0,todo:0});
const countFields=['tests','passed','failed','cancelled','skipped','todo'];

function numericSummary(counts) {
  if (!counts||typeof counts!=='object'||countFields.some(field=>!Number.isSafeInteger(counts[field])||counts[field]<0)) return null;
  return {tests:counts.tests,pass:counts.passed,fail:counts.failed,cancelled:counts.cancelled,
    skipped:counts.skipped,todo:counts.todo};
}

/** Validate Node test-runner events against each declared manifest file. */
export function assessOracleTestEvents(text,{root,files}) {
  const expected=new Map(files.map(file=>[normalizedPath(resolve(root,'tests/cpp',file)),file]));
  const perFile=new Map([...expected].map(([key,file])=>[key,{file,registered:false,completed:false,executedNonSkipTodo:0,
    observed:emptyCounts(),summary:null}]));
  const errors=[];
  let globalSummary=null;
  for (const [index,line] of text.split(/\r?\n/).entries()) {
    if (!line.trim()) continue;
    let event;
    try {event=JSON.parse(line);} catch {errors.push(`invalid event JSON at line ${index+1}`);continue;}
    if (!event||typeof event.type!=='string'||!event.data||typeof event.data!=='object') {
      errors.push(`invalid event shape at line ${index+1}`);continue;
    }
    const data=event.data;
    const fileKey=typeof data.file==='string'?normalizedPath(data.file):null;
    const fileEvidence=fileKey?perFile.get(fileKey):null;
    if (['test:enqueue','test:dequeue','test:start','test:complete','test:pass','test:fail','test:skip','test:todo','test:summary'].includes(event.type)&&
        fileKey&&!fileEvidence) errors.push(`unmanifested test event file ${data.file}`);
    if (event.type==='test:enqueue'&&fileEvidence&&data.type==='test'&&typeof data.name==='string'&&
        normalizedPath(data.name)===fileKey) fileEvidence.registered=true;
    if (event.type==='test:complete'&&fileEvidence&&typeof data.name==='string'&&
        normalizedPath(data.name)===fileKey&&data.details?.type==='test') fileEvidence.completed=true;
    if (event.type==='test:summary') {
      const counts=numericSummary(data.counts);
      if (!counts) {errors.push(`invalid test summary for ${data.file??'group'}`);continue;}
      if (fileEvidence) {
        if (fileEvidence.summary) errors.push(`duplicate test summary for ${fileEvidence.file}`);
        fileEvidence.summary=counts;
      } else if (!data.file) {
        if (globalSummary) errors.push('duplicate aggregate test summary');
        globalSummary=counts;
      } else errors.push(`summary is not bound to a manifest file: ${data.file}`);
    }
    if (!['test:pass','test:fail','test:skip','test:todo'].includes(event.type)||data.details?.type!=='test') continue;
    if (!fileEvidence) {errors.push(`test outcome has no manifest file at line ${index+1}`);continue;}
    if (typeof data.name==='string'&&normalizedPath(data.name)===fileKey) continue;
    const outcome=fileEvidence.observed;
    outcome.tests++;
    if (event.type==='test:skip'||data.skip===true) outcome.skipped++;
    else if (event.type==='test:todo'||data.todo===true) outcome.todo++;
    else if (event.type==='test:fail') {outcome.fail++;fileEvidence.executedNonSkipTodo++;}
    else {outcome.pass++;fileEvidence.executedNonSkipTodo++;}
  }
  const filesEvidence=[];
  for (const item of perFile.values()) {
    const {summary,observed}=item;
    if (!item.registered) errors.push(`manifest test file was not registered: ${item.file}`);
    if (!item.completed) errors.push(`manifest test file did not complete: ${item.file}`);
    if (!summary&&observed.tests>0) errors.push(`manifest test file has no file summary: ${item.file}`);
    if (item.executedNonSkipTodo===0) errors.push(`manifest test file executed no non-skip/non-todo test: ${item.file}`);
    if (summary) {
      if (summary.tests!==observed.tests||summary.pass!==observed.pass||summary.fail!==observed.fail||
          summary.skipped!==observed.skipped||summary.todo!==observed.todo)
        errors.push(`structured event totals disagree with file summary: ${item.file}`);
      if (summary.cancelled>0) errors.push(`manifest test file has cancelled tests: ${item.file}`);
    }
    filesEvidence.push({...item,...(summary??observed),observed});
  }
  const totals=emptyCounts();
  for (const item of filesEvidence) for (const field of Object.keys(totals)) totals[field]+=item[field]??0;
  if (!globalSummary) errors.push('missing aggregate test summary');
  else if (Object.keys(totals).some(field=>totals[field]!==globalSummary[field]))
    errors.push('aggregate test summary disagrees with manifest file summaries');
  return {valid:errors.length===0&&files.length>0&&totals.tests>0&&totals.pass===totals.tests&&totals.fail===0&&
      totals.cancelled===0&&totals.skipped===0&&totals.todo===0,
    counts:totals,files:filesEvidence,errors};
}

async function readOracleTestEvents(path,options) {
  try {return assessOracleTestEvents(await readFile(path,'utf8'),options);}
  catch (error) {
    return {valid:false,counts:emptyCounts(),files:options.files.map(file=>({file,registered:false,completed:false,
      executedNonSkipTodo:0,...emptyCounts()})),errors:[`could not read Node test events: ${error.message}`]};
  }
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
export async function runFrozenOracle({root,name='full-suite',concurrency=2,timeoutMs=7200000,
  overallTimeoutMs=7200000,signal,environment=process.env}) {
  if (!validEvidenceName(name)) throw new Error('Invalid evidence name');
  if (!Number.isSafeInteger(concurrency)||concurrency<1||concurrency>4) throw new Error('Test concurrency must be 1..4');
  if (!Number.isSafeInteger(timeoutMs)||timeoutMs<1||!Number.isSafeInteger(overallTimeoutMs)||overallTimeoutMs<1)
    throw new Error('Oracle time budgets must be positive integer milliseconds');
  const directory=resolve(root,'artifacts/cpp/verification');
  await mkdir(directory,{recursive:true});
  const resultPath=resolve(directory,name+'-result.json'),runId=randomUUID(),startedAt=new Date().toISOString();
  const running={schema:'eagler-touhou/native-oracle-result/1',runId,status:'running',passed:false,exitCode:1,
    owner:basename(dirname(resolve(root))),name,startedAt,timeoutMs,overallTimeoutMs,terminalReason:null,
    baselineChanged:null,protectedInputsChanged:null};
  await writeJsonAtomic(resultPath,running,runId);
  const deadlineController=new AbortController();
  let deadlineExceeded=false;
  const deadlineTimer=setTimeout(()=>{deadlineExceeded=true;deadlineController.abort();},overallTimeoutMs);
  deadlineTimer.unref();
  const runSignal=signal?AbortSignal.any([signal,deadlineController.signal]):deadlineController.signal;
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
  const eventReports=new Map();
  const tasks=[];
  for (const group of runnable) {
    const eventsPath=resolve(directory,name,'node-events-'+group.id+'.jsonl');
    await mkdir(dirname(eventsPath),{recursive:true});
    eventReports.set(group.id,eventsPath);
    tasks.push({id:group.id,command:process.execPath,
      args:oracleTestRunnerArgs({files:group.tests.map(file=>resolve(root,'tests/cpp',file)),eventsPath,concurrency}),
      cwd:root,timeoutMs,environment:scopeForRun(suite,group,preflightById.get(group.id)?.envOverrides??{},environment,
        {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:core}).environment});
  }
  let taskReport=null;
  const nativePolicy=validateNativeEnvironmentPolicy(suite),baseEnvironment={...environment},
    unsetEnvironment=[...new Set([...nativePolicy.clearInherited,'NODE_TEST_CONTEXT'])];
  for (const key of unsetEnvironment) delete baseEnvironment[key];
  if (tasks.length) taskReport=await runTasks(tasks,{output:resolve(directory,name),concurrency:1,signal:runSignal,environment:{...baseEnvironment,
    NO_COLOR:'1',NODE_DISABLE_COLORS:'1',TH11_CORE_TEST_FILE:core,EAGLER_ORACLE_ROOT:root},
    unsetEnvironment,
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
    const eventReport=await readOracleTestEvents(eventReports.get(group.id),{root,files:group.tests});
    const counts=eventReport.counts,afterInputs=postflight.after,privateInputsChanged=postflight.protectedInputsChanged;
    const passed=task?.status==='passed'&&eventReport.valid&&!runSignal.aborted&&!privateInputsChanged;
    groups.push({id:group.id,status:passed?'passed':'failed',testFiles:group.tests.length,...counts,
      taskStatus:task?.status??'not-started',durationMs:task?.durationMs??0,exitCode:task?.code??null,log:task?.log??null,
      nodeEvents:eventReports.get(group.id)??null,testFileEvidence:eventReport.files,nodeEventErrors:eventReport.errors,
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
  const terminalReason=deadlineExceeded?'time-budget-exceeded':signal?.aborted?'cancelled':
    groups.some(group=>group.taskStatus==='timeout')?'task-timeout':
      groups.some(group=>group.taskStatus==='cancelled')?'cancelled':null;
  const passed=!terminalReason&&!runSignal.aborted&&groups.length>0&&groups.every(group=>group.status==='passed')&&counts.tests>0&&counts.pass===counts.tests&&
      counts.fail===0&&counts.cancelled===0&&counts.skipped===0&&counts.todo===0&&
      hash(await readFile(core))===coreSha256&&!testChanges.length&&!inventoryChanged&&!nativeInputsChanged&&!baselineChanged&&!protectedInputsChanged;
  const result={...running,status:passed?'passed':'failed',finishedAt:new Date().toISOString(),passed,exitCode:passed?0:1,
    terminalReason,
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
      terminalReason:deadlineExceeded?'time-budget-exceeded':signal?.aborted?'cancelled':null,
      error:{name:error.name??'Error',message:error.message??String(error)}};
    await writeJsonAtomic(resultPath,failed,runId);
    throw error;
  } finally {
    clearTimeout(deadlineTimer);
  }
}

/** Run one explicitly selected maintenance task into ignored staging only. */
export async function runOracleMaintenance({root,taskId,name,timeoutMs=7200000,signal,environment=process.env}) {
  if (!validEvidenceName(taskId)||!validEvidenceName(name)) throw new Error('Maintenance task and evidence name are required');
  const suite=await loadOracleSuite(root),task=suite.maintenance?.find(item=>item.id===taskId);
  if (!task) throw new Error(`Unknown maintenance task: ${taskId}`);
  const policy=validateNativeEnvironmentPolicy(suite),scope=nativeGroupEnvironment(suite,task,{},environment),
    output=maintenanceOutputPath(root,task,name),preflight=await preflightOracleInputs(root,suite,
      [...suite.baseInputs,...(task.requiredInputs??[])],scope.environment);
  if (preflight.status!=='ready') return {passed:false,status:'blocked',exitCode:1,task:taskId,output,blocked:preflight.blocked,inputs:preflight.inputs};
  if(task.kind!=='test')throw new Error(`Maintenance task must use the Node test runner: ${taskId}`);
  const inputFile=resolve(root,'tests/cpp',task.file),eventsPath=resolve(output,'logs','node-events.jsonl'),
    args=oracleTestRunnerArgs({files:[inputFile],eventsPath});
  const repository=gitRoot(root),headBefore=gitRevision(repository),evidence=await coreEvidence(root),
    nativeBefore=await nativeInputSnapshot(root,suite,environment),baselineBefore=await acceptedReferenceSnapshot(root,suite);
  assertBuildMatchesNativeSnapshot(evidence.manifest,nativeBefore);
  const taskScope=scopeForRun(suite,task,preflight.envOverrides,environment,
    {EAGLER_ORACLE_ROOT:root,TH11_CORE_TEST_FILE:evidence.core,EAGLER_MAINTENANCE_OUTPUT:output});
  const baseEnvironment={...environment},unsetEnvironment=[...new Set([...policy.clearInherited,'NODE_TEST_CONTEXT'])];
  for (const key of unsetEnvironment) delete baseEnvironment[key];
  const report=await runTasks([{id:taskId,command:process.execPath,args,cwd:root,timeoutMs,
    environment:taskScope.environment}],
  {output:resolve(output,'logs'),concurrency:1,environment:{...baseEnvironment,NO_COLOR:'1',NODE_DISABLE_COLORS:'1',
    TH11_CORE_TEST_FILE:evidence.core,EAGLER_ORACLE_ROOT:root},unsetEnvironment,
    signal,
    metadata:{schema:'eagler-touhou/oracle-maintenance/1',owner:suite.owner,task:taskId}});
  const taskResult=report.results[0],eventAssessment=await readOracleTestEvents(eventsPath,{root,files:[task.file]}),
    counts=eventAssessment.counts,
    after=await preflightOracleInputs(root,suite,[...suite.baseInputs,...(task.requiredInputs??[])],taskScope.environment),
    nativeAfter=await nativeInputSnapshot(root,suite,environment),nativeInputsChanged=!nativeInputSnapshotsEqual(nativeBefore,nativeAfter),
    headAfter=gitRevision(repository);
  let baselineAfter=null,baselineError=null;
  try {baselineAfter=await acceptedReferenceSnapshot(root,suite);} catch(error) {baselineError=error.message;}
  const baselineChanged=!acceptedReferenceSnapshotsEqual(baselineBefore,baselineAfter);
  const passed=report.status==='passed'&&taskResult.status==='passed'&&eventAssessment.valid&&after.status==='ready'&&
    inputSnapshotsEqual(preflight.inputs,after.inputs)&&!nativeInputsChanged&&!baselineChanged;
  const terminalReason=taskResult.status==='timeout'?'task-timeout':taskResult.status==='cancelled'?'cancelled':null;
  return {passed,status:passed?'passed':'failed',exitCode:passed?0:1,task:taskId,output,durationMs:report.durationMs,
    terminalReason,
    inputsBefore:preflight.inputs,inputsAfter:after.inputs,inputChanges:!inputSnapshotsEqual(preflight.inputs,after.inputs),
    nativeInputsChanged,nativeInputs:{before:nativeBefore,after:nativeAfter},head:{before:headBefore,after:headAfter},
    effectiveScope:taskScope.effective,baselineChanged,acceptedReference:{before:baselineBefore,after:baselineAfter,error:baselineError},
    ...counts,testFileEvidence:eventAssessment.files,nodeEventErrors:eventAssessment.errors,nodeEvents:eventsPath,
    report:resolve(output,'logs','report.json'),log:taskResult.log};
}
