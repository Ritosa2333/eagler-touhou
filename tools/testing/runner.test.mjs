import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {mkdtemp, mkdir, readFile, readdir, rm, writeFile} from 'node:fs/promises';
import {setTimeout as delay} from 'node:timers/promises';
import {tmpdir} from 'node:os';
import {basename, resolve, dirname} from 'node:path';
import {runTasks} from './runner.mjs';
import {acquireOutputLock} from './output-lock.mjs';

const task = (id, source, extra={}) => ({id, command:process.execPath,
  args:['-e', source], cwd:process.cwd(), timeoutMs:10000, ...extra});
async function sha256File(path) {
  const hash=createHash('sha256');
  for await (const chunk of createReadStream(path)) hash.update(chunk);
  return hash.digest('hex');
}
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

test('runner and direct supervisor reject root-relative and drive-relative cleanup image paths', () => fixture(async output => {
  const invalidPaths=['\\helper.exe','/helper.exe','C:helper.exe'];
  for(let index=0;index<invalidPaths.length;index++){
    await assert.rejects(runTasks([task(`invalid-cleanup-${index}`,'process.exit(0)',{
      cleanupImages:[{path:invalidPaths[index],sha256:'a'.repeat(64)}]})],{output:resolve(output,`runner-${index}`)}),
    /drive-qualified or UNC-share image path/);
  }
  const python=process.env.TH_PYTHON??process.env.PYTHON??(process.platform==='win32'?'python':'python3');
  const supervisor=resolve(process.cwd(),'tools/testing/supervise.py');
  for(let index=0;index<invalidPaths.length;index++){
    const child=spawnSync(python,[supervisor,'--cleanup-report',resolve(output,`python-${index}.json`),
      '--cleanup-image',invalidPaths[index],'a'.repeat(64),'--',process.execPath,'-e','process.exit(0)'],
    {cwd:process.cwd(),encoding:'utf8',windowsHide:true});
    assert.equal(child.status,2,child.stderr||child.error?.message);
    assert.match(child.stderr,/drive-qualified or a complete UNC-share path/);
  }
}));

test('a task-declared Windows helper is stopped only after exact path and SHA-256 match', {skip:process.platform !== 'win32'}, () => fixture(async output => {
  const image={path:process.execPath,sha256:await sha256File(process.execPath)};
  const helper='setInterval(()=>{},1000);';
  const driver=`const child=require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(helper)}],
    {detached:true,stdio:'ignore',windowsHide:true});child.unref();setTimeout(()=>process.exit(0),150);`;
  const report=await runTasks([task('declared-helper',driver,{cleanupImages:[image]})],{output});
  const result=report.results[0],cleanup=result.cleanupImages;
  assert.equal(report.status,'passed');assert.equal(result.code,0);
  assert.deepEqual(report.cleanupDeclarations,[{id:'declared-helper',images:[image]}]);
  assert.equal(cleanup.status,'passed');assert.deepEqual(cleanup.declared,[image]);
  const terminated=cleanup.observed.find(item=>item.action==='terminated');
  assert.ok(terminated,'the declared live helper must be explicitly terminated');
  assert.equal(terminated.path.toLowerCase(),image.path.toLowerCase());
  assert.equal(terminated.sha256,image.sha256);
  assert.equal(terminated.exitCode,0xE0000001);
  assert.deepEqual(cleanup.remaining,[]);
}));

test('a declared image with the wrong SHA-256 leaves the helper as a failing descendant', {skip:process.platform !== 'win32'}, () => fixture(async output => {
  const heartbeat=resolve(output,'wrong-hash-heartbeat');
  const helper=`const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};function beat(){fs.writeFileSync(file,String(Date.now()));setTimeout(beat,30)}beat();`;
  const driver=`const child=require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(helper)}],
    {detached:true,stdio:'ignore',windowsHide:true});child.unref();setTimeout(()=>process.exit(0),150);`;
  const report=await runTasks([task('wrong-helper-hash',driver,{cleanupImages:[{path:process.execPath,sha256:'0'.repeat(64)}]})],{output});
  const result=report.results[0],cleanup=result.cleanupImages;
  assert.equal(report.status,'failed');assert.equal(result.code,1);
  assert.equal(cleanup.status,'failed-descendants');
  assert.ok(cleanup.observed.some(item=>item.action==='sha256-mismatch'&&item.path.toLowerCase()===process.execPath.toLowerCase()));
  assert.ok(cleanup.remaining.some(item=>item.path?.toLowerCase()===process.execPath.toLowerCase()));
  const after=await readFile(heartbeat,'utf8');await delay(150);
  assert.equal(await readFile(heartbeat,'utf8'),after,'the failed task Job must still terminate the unapproved helper');
}));

test('a matching executable basename at another path does not authorize cleanup', {skip:process.platform !== 'win32'}, () => fixture(async output => {
  const heartbeat=resolve(output,'wrong-path-heartbeat');
  const image={path:resolve(output,'shadow',basename(process.execPath)),sha256:await sha256File(process.execPath)};
  const helper=`const fs=require('node:fs');const file=${JSON.stringify(heartbeat)};function beat(){fs.writeFileSync(file,String(Date.now()));setTimeout(beat,30)}beat();`;
  const driver=`const child=require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(helper)}],
    {detached:true,stdio:'ignore',windowsHide:true});child.unref();setTimeout(()=>process.exit(0),150);`;
  const report=await runTasks([task('wrong-helper-path',driver,{cleanupImages:[image]})],{output});
  const result=report.results[0],cleanup=result.cleanupImages;
  assert.equal(report.status,'failed');assert.equal(result.code,1);
  assert.ok(cleanup.observed.some(item=>item.action==='not-declared'&&item.path.toLowerCase()===process.execPath.toLowerCase()));
  assert.deepEqual(cleanup.observed.filter(item=>item.action==='terminated'),[]);
  const after=await readFile(heartbeat,'utf8');await delay(150);
  assert.equal(await readFile(heartbeat,'utf8'),after,'the Job must still end an image with an unapproved full path');
}));

test('a nonzero task exit stays failed after its declared helper is cleaned', {skip:process.platform !== 'win32'}, () => fixture(async output => {
  const image={path:process.execPath,sha256:await sha256File(process.execPath)};
  const helper='setInterval(()=>{},1000);';
  const driver=`const child=require('node:child_process').spawn(process.execPath,['-e',${JSON.stringify(helper)}],
    {detached:true,stdio:'ignore',windowsHide:true});child.unref();setTimeout(()=>process.exit(7),150);`;
  const report=await runTasks([task('failed-main-with-helper',driver,{cleanupImages:[image]})],{output});
  const result=report.results[0];
  assert.equal(report.status,'failed');assert.equal(result.status,'failed');assert.equal(result.code,7);
  assert.equal(result.cleanupImages.commandExitCode,7);assert.equal(result.cleanupImages.status,'command-failed');
  assert.ok(result.cleanupImages.observed.some(item=>item.action==='terminated'));
}));

test('Linux hard-fails task-declared helper cleanup instead of running without Job identity checks', {skip:process.platform !== 'linux'}, () => fixture(async output => {
  const marker=resolve(output,'must-not-run');
  const report=await runTasks([task('unsupported-helper',`require('node:fs').writeFileSync(${JSON.stringify(marker)},'ran')`,{
    cleanupImages:[{path:'C:\\BuildTools\\bin\\helper.exe',sha256:'a'.repeat(64)}]})],{output});
  assert.equal(report.status,'failed');assert.equal(report.results[0].status,'failed');
  assert.equal(report.results[0].code,1);
  assert.equal(report.results[0].cleanupImages.status,'unsupported-platform');
  await assert.rejects(readFile(marker),{code:'ENOENT'});
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
