import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, readFile, rm} from 'node:fs/promises';
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
