import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {runTasks} from '../tools/testing/runner.mjs';
import {repository, suites, profiles, trackedFiles} from './suites.mjs';
import {sourceSnapshot} from '../tools/testing/source-snapshot.mjs';
import {acquireOutputLock} from '../tools/testing/output-lock.mjs';
import {runWithReport} from '../tools/testing/cli-report.mjs';
import {assertNoSameProfileTestDuplicates,buildExecutionOwnership,summarizeExecutionOwnership} from '../tools/testing/execution-ownership.mjs';
import {freshAdaptationOutput,runAdaptationVerification} from './adaptation/execution.mjs';

const options = {suite:[], profile:[], jobs:4, output:resolve(repository, 'artifacts/validation/tests')};
const argumentsForRun=process.argv.slice(2);
const outputHint=argumentsForRun.find(argument=>/^--output=.+$/.test(argument));
if(outputHint)options.output=resolve(outputHint.slice('--output='.length));
let argumentError;
try{for (const argument of argumentsForRun) {
  if (argument === '--list') {options.list=true; continue;}
  if (argument === '--ownership') {options.ownership=true; continue;}
  if (argument === '--plan') {options.planOnly=true; continue;}
  const match = argument.match(/^--(suite|profile|jobs|output|game|variant|check|inputs|resume|rerun-failed)=(.+)$/);
  if (!match) throw new Error(`Unknown test argument: ${argument}`);
  const [, name, value] = match;
  if (name === 'suite' || name === 'profile') options[name].push(...value.split(','));
  else if(['game','variant','check'].includes(name))options[name]=value;
  else if(name==='rerun-failed')options.rerunFailed=resolve(value);
  else options[name] = name === 'jobs' ? Number(value) : resolve(value);
}}catch(error){argumentError=error;}
if(options.profile.includes('adaptation')){
  try{
    if(argumentError)throw argumentError;
    if(options.profile.length!==1||options.list)throw Error('Adaptation selection requires only --profile=adaptation');
    const report=await runAdaptationVerification({root:repository,...options,
      output:outputHint?options.output:freshAdaptationOutput(repository),games:options.game?.split(','),checks:options.check?.split(','),
      groups:options.suite.length?options.suite:undefined,
      onResult:item=>console.log(`${item.id}: ${item.status} ${item.report?.path??''}`)});
    console.log(JSON.stringify({status:report.status,counts:report.counts,executed:report.executed,retained:report.retained,
      coverageComplete:report.coverage?.complete,summary:resolve(report.output,'summary.json')}));
    if(report.failure)console.error(report.failure);
    if(!report.passed&&report.status!=='planned')process.exitCode=1;
  }catch(error){console.error(error.message);process.exitCode=1;}
} else if ((options.list||options.ownership)&&!argumentError) {
  const catalog=suites({output:options.output});
  if(options.ownership){
    const profileSelections=Object.fromEntries(Object.entries(profiles).map(([name,ids])=>[name,ids]));
    const ownership=await buildExecutionOwnership({root:repository,catalog,profileSelections,files:trackedFiles(repository)});
    assertNoSameProfileTestDuplicates(ownership);
    if(ownership.managedOrphans.length)throw new Error(`Executable tests lost their declared owner: ${ownership.managedOrphans.map(item=>item.file).join(', ')}`);
    console.log(JSON.stringify(summarizeExecutionOwnership(ownership),null,2));
  }else console.log(JSON.stringify(Object.entries(catalog).map(([id, suite])=>({id, tier:suite.tier, scope:suite.scope})), null, 2));
} else {
  const release=await acquireOutputLock(options.output);
  try {
  const report=await runWithReport({output:options.output,filename:'summary.json',schema:'eagler-touhou/test-selection/1'},async(summary,save)=>{
  if(argumentError)throw argumentError;
  if(['game','variant','check','inputs','resume','rerunFailed','planOnly'].some(name=>options[name]!==undefined))
    throw Error('Adaptation selectors require --profile=adaptation');
  const catalog=suites({output:options.output});
  if (!options.suite.length && !options.profile.length) options.profile.push('quick');
  const selected = new Set(options.suite);
  for (const profile of options.profile) {
    if (!profiles[profile]) throw new Error(`Unknown test profile: ${profile}`);
    profiles[profile].forEach(id=>selected.add(id));
  }
  if (!selected.size) throw new Error('No suites selected');
  for (const id of selected) if (!catalog[id]) throw new Error(`Unknown test suite: ${id}`);
  // Resolve every input before starting: incomplete selections cannot pass.
  const planned = [...selected].map(id=>({id, ...catalog[id], tasks:catalog[id].tasks()}));
  const profileSelections={...Object.fromEntries(options.profile.map(name=>[name,profiles[name]])),selection:[...selected]};
  const ownership=await buildExecutionOwnership({root:repository,catalog,profileSelections,files:trackedFiles(repository)});
  assertNoSameProfileTestDuplicates(ownership);
  if(ownership.managedOrphans.length)throw new Error(`Executable tests lost their declared owner: ${ownership.managedOrphans.map(item=>item.file).join(', ')}`);
  const commit = execFileSync('git', ['rev-parse', 'HEAD'], {cwd:repository, encoding:'utf8', windowsHide:true}).trim();
  const source = sourceSnapshot(repository);
  Object.assign(summary,{commit,source,selected:[...selected],results:[]});
  await save();
  const controller = new AbortController();
  const cancel = () => controller.abort();
  process.once('SIGINT', cancel); process.once('SIGTERM', cancel);
  const results = summary.results;
  try {
    for (const suite of planned) {
      if (suite.prepare) {
        const preparation=await runTasks(suite.prepare(), {output:resolve(options.output, `${suite.id}-prepare`),
          concurrency:1, failFast:true, signal:controller.signal, metadata:{commit,source,runId:summary.runId,suite:suite.id}});
        if (preparation.status !== 'passed') {
          results.push({suite:suite.id,status:'failed',report:resolve(options.output, `${suite.id}-prepare`, 'report.json')});
          await save();
          continue;
        }
      }
      const report = await runTasks(suite.tasks, {output:resolve(options.output, suite.id),
        concurrency:suite.sequential ? 1 : Math.min(suite.concurrency ?? 32,options.jobs), failFast:suite.failFast??suite.sequential===true, signal:controller.signal,
        metadata:{commit, source,runId:summary.runId, node:process.versions.node, platform:process.platform, suite:suite.id, scope:suite.scope},
        onResult:result=>console.log(`${result.id}: ${result.status} (${(result.durationMs/1000).toFixed(1)}s) ${result.log}`)});
      results.push({suite:suite.id, status:report.status, report:resolve(options.output, suite.id, 'report.json')});
      await save();
    }
  } finally {
    process.removeListener('SIGINT', cancel); process.removeListener('SIGTERM', cancel);
    const currentSource = sourceSnapshot(repository);
    const sourceChanged = currentSource.sha256 !== source.sha256;
    if (sourceChanged) {console.error('Source changed during testing; rerun the selection.'); process.exitCode=1;}
    Object.assign(summary,{currentSource,sourceChanged,
      status:!sourceChanged && results.length === selected.size && results.every(result=>result.status === 'passed') ? 'passed' : 'failed'});
    summary.passed=summary.status==='passed';
    await save();
  }
  });
  if(report.failure)console.error(report.failure);
  if(!report.passed)process.exitCode=1;
  } finally {await release();}
}
