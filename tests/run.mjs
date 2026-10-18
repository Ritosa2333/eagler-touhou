import {execFileSync} from 'node:child_process';
import {mkdir, writeFile, rename} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runTasks} from '../tools/testing/runner.mjs';
import {repository, suites, profiles} from './suites.mjs';
import {sourceSnapshot} from '../tools/testing/source-snapshot.mjs';
import {acquireOutputLock} from '../tools/testing/output-lock.mjs';

const options = {suite:[], profile:[], jobs:4, output:resolve(repository, 'artifacts/validation/tests')};
async function saveSummary(value) {
  const pending=resolve(options.output,'summary.pending.json');
  await writeFile(pending,JSON.stringify(value,null,2)+'\n');
  await rename(pending,resolve(options.output,'summary.json'));
}
for (const argument of process.argv.slice(2)) {
  if (argument === '--list') {options.list=true; continue;}
  const match = argument.match(/^--(suite|profile|jobs|output)=(.+)$/);
  if (!match) throw new Error(`Unknown test argument: ${argument}`);
  const [, name, value] = match;
  if (name === 'suite' || name === 'profile') options[name].push(...value.split(','));
  else options[name] = name === 'jobs' ? Number(value) : resolve(value);
}
const catalog = suites({output:options.output});
if (options.list) {
  console.log(JSON.stringify(Object.entries(catalog).map(([id, suite])=>({id, tier:suite.tier, scope:suite.scope})), null, 2));
} else {
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
  const release=await acquireOutputLock(options.output);
  try {
  const commit = execFileSync('git', ['rev-parse', 'HEAD'], {cwd:repository, encoding:'utf8', windowsHide:true}).trim();
  const source = sourceSnapshot(repository);
  const startedAt=new Date().toISOString();
  await saveSummary({schema:'eagler-touhou/test-selection/1',commit,source,startedAt,selected:[...selected],results:[],status:'running'});
  const controller = new AbortController();
  const cancel = () => controller.abort();
  process.once('SIGINT', cancel); process.once('SIGTERM', cancel);
  const results = [];
  try {
    for (const suite of planned) {
      if (suite.prepare) {
        const preparation=await runTasks(suite.prepare(), {output:resolve(options.output, `${suite.id}-prepare`),
          concurrency:1, failFast:true, signal:controller.signal, metadata:{commit,source,suite:suite.id}});
        if (preparation.status !== 'passed') {
          results.push({suite:suite.id,status:'failed',report:resolve(options.output, `${suite.id}-prepare`, 'report.json')});
          continue;
        }
      }
      const report = await runTasks(suite.tasks, {output:resolve(options.output, suite.id),
        concurrency:suite.sequential ? 1 : Math.min(suite.concurrency ?? 32,options.jobs), failFast:suite.sequential === true, signal:controller.signal,
        metadata:{commit, source, node:process.versions.node, platform:process.platform, suite:suite.id, scope:suite.scope},
        onResult:result=>console.log(`${result.id}: ${result.status} (${(result.durationMs/1000).toFixed(1)}s) ${result.log}`)});
      results.push({suite:suite.id, status:report.status, report:resolve(options.output, suite.id, 'report.json')});
    }
  } finally {
    process.removeListener('SIGINT', cancel); process.removeListener('SIGTERM', cancel);
    await mkdir(options.output, {recursive:true});
    const currentSource = sourceSnapshot(repository);
    const sourceChanged = currentSource.sha256 !== source.sha256;
    if (sourceChanged) {console.error('Source changed during testing; rerun the selection.'); process.exitCode=1;}
    await saveSummary({schema:'eagler-touhou/test-selection/1',
      commit,source,startedAt,finishedAt:new Date().toISOString(),currentSource, sourceChanged, selected:[...selected], results,
      status:!sourceChanged && results.length === selected.size && results.every(result=>result.status === 'passed') ? 'passed' : 'failed'});
  }
  if (results.length !== selected.size || results.some(result=>result.status !== 'passed')) process.exitCode=1;
  } finally {await release();}
}
