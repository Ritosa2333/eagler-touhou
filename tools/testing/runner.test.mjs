import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, readFile, readdir, rm, writeFile} from 'node:fs/promises';
import {setTimeout as delay} from 'node:timers/promises';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {runTasks} from './runner.mjs';
import {acquireOutputLock} from './output-lock.mjs';

const task = (id, source, extra={}) => ({id, command:process.execPath,
  args:['-e', source], cwd:process.cwd(), timeoutMs:10000, ...extra});
async function fixture(operation) {
  const output = await mkdtemp(resolve(tmpdir(), 'eagler-test-runner-'));
  try {await operation(output);} finally {
    assert.equal(dirname(resolve(output)), resolve(tmpdir()));
    await rm(output, {recursive:true, force:true});
  }
}

test('a failed test cannot be hidden by a successful sibling and all selected results persist', () => fixture(async output => {
  const report = await runTasks([task('failure', 'console.error("real failure");process.exit(3)'),
    task('success', 'console.log("real success")')], {output});
  assert.equal(report.status, 'failed');
  assert.equal(report.results.length, 2);
  assert.equal(report.results.find(result=>result.id === 'failure').code, 3);
  assert.match(await readFile(resolve(output, 'failure.log'), 'utf8'), /real failure/);
  assert.deepEqual(JSON.parse(await readFile(resolve(output, 'report.json'))), report);
}));

test('timeouts, missing executables and a cancelled selection all fail', () => fixture(async output => {
  const report = await runTasks([task('timeout', 'setInterval(()=>{},1000)', {timeoutMs:400}),
    task('missing', '', {command:resolve(output, 'missing-executable')})], {output});
  assert.equal(report.status, 'failed');
  assert.equal(report.results.find(result=>result.id === 'timeout').status, 'timeout');
  assert.equal(report.results.find(result=>result.id === 'missing').status, 'failed');
  const controller = new AbortController(); controller.abort();
  const cancelled = await runTasks([task('cancelled', 'throw Error("must not run")')], {output, signal:controller.signal});
  assert.equal(cancelled.status, 'failed');
  assert.equal(cancelled.results[0].status, 'cancelled');
}));

test('shared resources serialize actual child processes', () => fixture(async output => {
  const lock = resolve(output, 'exclusive.lock');
  const source = `const fs=require('node:fs'); const path=${JSON.stringify(lock)};
    fs.writeFileSync(path,'owned',{flag:'wx'});setTimeout(()=>fs.unlinkSync(path),100);`;
  const report = await runTasks([task('first', source, {resources:['source-build']}),
    task('second', source, {resources:['source-build']})], {output, concurrency:2});
  assert.equal(report.status, 'passed');
  const [first, second] = report.results;
  assert.ok(Date.parse(second.startedAt) >= Date.parse(first.startedAt) + first.durationMs - 2);
}));

test('a log write failure fails and terminates the actual driver', () => fixture(async output => {
  await mkdir(resolve(output, 'broken-log.log'));
  const report = await runTasks([task('broken-log','console.log("started");setInterval(()=>{},1000)')], {output});
  assert.equal(report.status, 'failed');
  assert.match(report.results[0].error, /Test log failed/);
  assert.ok(report.results[0].durationMs < 9000);
}));

test('a result callback failure terminates and awaits active siblings before releasing evidence', () => fixture(async output => {
  const heartbeat=resolve(output,'callback-heartbeat');
  const fast=`const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};
    function wait(){if(!fs.existsSync(file))setTimeout(wait,10);}wait();`;
  const worker=`const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};let n=0;
    fs.writeFileSync(file,String(n));setInterval(()=>fs.writeFileSync(file,String(++n)),25);`;
  await assert.rejects(runTasks([task('fast',fast),task('worker',worker)], {output,concurrency:2,
    onResult:result=>{if(result.id==='fast')throw new Error('intentional result failure');}}), /intentional result failure/);
  const ended=await readFile(heartbeat,'utf8');
  await delay(200);
  assert.equal(await readFile(heartbeat,'utf8'),ended,'a sibling continued after the runner rejected');
  const report=JSON.parse(await readFile(resolve(output,'report.json')));
  assert.equal(report.status,'failed');assert.equal(report.error,'intentional result failure');
  assert.deepEqual(report.results.map(result=>[result.id,result.status]),[['fast','passed'],['worker','cancelled']]);
  const release=await acquireOutputLock(output);await release();
}));

test('empty selections, duplicate ids and invalid time budgets are rejected', async () => {
  await assert.rejects(runTasks([], {}), /No tests selected/);
  await assert.rejects(runTasks([task('same',''), task('same','')], {}), /duplicate/);
  await assert.rejects(runTasks([task('unbounded','', {timeoutMs:0})], {}), /timeout/);
});

test('child environment controls remove inherited scope selectors before applying the selected profile', () => fixture(async output => {
  const key='EAGLER_NATIVE_SCOPE_TEST',previous=process.env[key];
  process.env[key]='narrow-inherited-value';
  try {
    const report=await runTasks([
      task('cleared',`console.log(JSON.stringify(process.env[${JSON.stringify(key)}]??null))`),
      task('profiled',`console.log(JSON.stringify(process.env[${JSON.stringify(key)}]??null))`,
        {environment:{[key]:'full-suite-value'}}),
    ],{output,concurrency:1,environment:{[key]:'inherited-run-level-overlay'},unsetEnvironment:[key]});
    assert.equal(report.status,'passed');
    assert.match(await readFile(report.results[0].log,'utf8'),/null/);
    assert.match(await readFile(report.results[1].log,'utf8'),/full-suite-value/);
  } finally {
    if(previous===undefined)delete process.env[key];else process.env[key]=previous;
  }
}));

test('concurrent invocations cannot overwrite the same report directory',()=>fixture(async output=>{
  const release=await acquireOutputLock(output);
  try {await assert.rejects(runTasks([task('overlap','process.exit(0)')],{output}),/already owned/);}
  finally {await release();}
  assert.equal((await runTasks([task('next','process.exit(0)')],{output})).status,'passed');
}));

test('a failed prerequisite cancels dependent commands instead of reusing old build output', () => fixture(async output => {
  const report = await runTasks([task('configure','process.exit(1)'),
    task('build','console.log("must not run")')], {output, concurrency:1, failFast:true});
  assert.equal(report.status, 'failed');
  assert.deepEqual(report.results.map(result=>result.status), ['failed','cancelled']);
}));

test('a timed-out driver terminates its detached native descendants', () => fixture(async output => {
  const heartbeat = resolve(output, 'heartbeat');
  const descendant = `const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};
    fs.writeFileSync(file,String(Date.now()));setInterval(()=>fs.writeFileSync(file,String(Date.now())),30);`;
  const driver = `require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(descendant)}],
    {detached:true,stdio:'ignore'});setInterval(()=>{},1000);`;
  const report = await runTasks([task('nested-timeout', driver, {timeoutMs:1200})], {output});
  assert.equal(report.results[0].status, 'timeout');
  const after = await readFile(heartbeat, 'utf8');
  await delay(200);
  assert.equal(await readFile(heartbeat, 'utf8'), after, 'a descendant kept executing after its task completed');
}));

test('a successful driver cannot hide detached work that survives its exit', () => fixture(async output => {
  const heartbeat = resolve(output, 'leaked-heartbeat');
  const descendant = `const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};
    fs.writeFileSync(file,String(Date.now()));setInterval(()=>fs.writeFileSync(file,String(Date.now())),30);`;
  const driver = `const child=require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(descendant)}],
    {detached:true,stdio:'ignore'});child.unref();setTimeout(()=>process.exit(0),150);`;
  const report = await runTasks([task('leaked-driver', driver)], {output});
  assert.equal(report.status, 'failed');
  const after = await readFile(heartbeat, 'utf8');
  await delay(150);
  assert.equal(await readFile(heartbeat, 'utf8'), after);
}));

test('runTasks removes NODE_OPTIONS shard selectors from host, run, and task environments', {timeout: 120000}, async () => {
  const root=await mkdtemp(resolve(tmpdir(),'eagler-node-options-runner-'));
  const selector='--test-shard=1/2',saved=process.env.NODE_OPTIONS;
  try {
    const first=resolve(root,'one.test.mjs'),second=resolve(root,'two.test.mjs');
    await writeFile(first,`import test from 'node:test';import assert from 'node:assert/strict';import {writeFileSync} from 'node:fs';import {resolve} from 'node:path';const mark=()=>writeFileSync(resolve(process.env.RUNNER_TEST_MARKERS,'one.marker'),'one');test('one-a',()=>{mark();assert.equal(2,2)});test('one-b',()=>assert.deepEqual([1,2],[1,2]));\n`);
    await writeFile(second,`import test from 'node:test';import assert from 'node:assert/strict';import {writeFileSync} from 'node:fs';import {resolve} from 'node:path';const mark=()=>writeFileSync(resolve(process.env.RUNNER_TEST_MARKERS,'two.marker'),'two');test('two-a',()=>{mark();assert.equal('x','x')});test('two-b',()=>assert.ok(true));test('two-c',()=>assert.equal(4,2+2));\n`);
    const scenarios=[
      {id:'host',host:true},
      {id:'run-environment',environment:{NODE_OPTIONS:selector}},
      {id:'task-environment',taskEnvironment:{NODE_OPTIONS:selector}},
    ];
    for(const scenario of scenarios){
      if(scenario.host)process.env.NODE_OPTIONS=selector;else delete process.env.NODE_OPTIONS;
      const output=resolve(root,scenario.id),markers=resolve(output,'markers');await mkdir(markers,{recursive:true});
      const environment=scenario.environment??{};
      const taskEnvironment={RUNNER_TEST_MARKERS:markers,...scenario.taskEnvironment};
      const report=await runTasks([{id:'node-test-files',command:process.execPath,
        args:['--test','--test-reporter=spec',first,second],cwd:root,timeoutMs:30000,environment:taskEnvironment}],
        {output:resolve(output,'run'),concurrency:1,environment,unsetEnvironment:['NODE_TEST_CONTEXT']});
      assert.equal(report.status,'passed',scenario.id);
      assert.equal(report.results.length,1,scenario.id);
      assert.equal(report.results[0].status,'passed',scenario.id);
      const log=await readFile(report.results[0].log,'utf8');
      assert.match(log,/ℹ tests 5(?:\r?\n|$)/,`${scenario.id}: both files' five tests must execute`);
      assert.match(log,/ℹ pass 5(?:\r?\n|$)/,`${scenario.id}: all five tests must pass`);
      assert.match(log,/ℹ skipped 0(?:\r?\n|$)/,`${scenario.id}: no tests may be skipped`);
      assert.deepEqual((await readdir(markers)).sort(),['one.marker','two.marker'],`${scenario.id}: each test file must leave its run-local marker`);
    }
  } finally {
    if(saved===undefined)delete process.env.NODE_OPTIONS;else process.env.NODE_OPTIONS=saved;
    assert.equal(dirname(resolve(root)),resolve(tmpdir()));await rm(root,{recursive:true,force:true});
  }
});
