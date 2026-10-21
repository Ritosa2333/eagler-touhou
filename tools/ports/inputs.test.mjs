import assert from 'node:assert/strict';
import {test} from 'node:test';
import {mkdirSync,mkdtempSync,rmSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {INPUT_INSPECTION_MAX_BUFFER_BYTES,INPUT_INSPECTION_TIMEOUT_MS,inspectInputs} from './inputs.mjs';

function inputFixture(t,source) {
  const repositoryRoot=mkdtempSync(resolve(tmpdir(),'eagler-input-inspection-'));
  const owner=resolve(repositoryRoot,'owner');
  mkdirSync(owner,{recursive:true});
  writeFileSync(resolve(owner,'build.mjs'),source);
  t.after(()=>rmSync(repositoryRoot,{recursive:true,force:true}));
  return {repositoryRoot,owner,target:{game:'th11',variant:'multiplayer',directory:'owner',
    recipe:{kind:'node',build:['build.mjs']},definition:{features:{},cache:{}}}};
}

test('input inspection has a finite production subprocess budget',()=>{
  assert.equal(INPUT_INSPECTION_TIMEOUT_MS,30_000);
});

test('input inspection reads the owner entrypoint JSON and removes inherited Node debug options',async t=>{
  const fixture=inputFixture(t,
    "console.log(JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',inputs:{'src/main.cpp':{sha256:'a'.repeat(64)}},nodeOptions:process.env.NODE_OPTIONS??null}));\n");
  const environment={...process.env,NODE_OPTIONS:'--inspect=127.0.0.1:0'};
  const result=await inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot,environment});
  assert.equal(result.schema,'eagler-touhou/runtime-inputs/1');
  assert.deepEqual(result.inputFiles,[resolve(fixture.owner,'src/main.cpp')]);
  assert.deepEqual(result.command,['node','build.mjs','--print-inputs']);
  assert.equal(result.nodeOptions,null);
});

test('input inspection preserves complete JSON output above the 64 KiB pipe range',async t=>{
  const fixture=inputFixture(t,
    "const padding='p'.repeat(70*1024);console.log(JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',inputs:{'src/main.cpp':{}},padding}));\n");
  const result=await inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot});
  assert.equal(result.padding.length,70*1024);
  assert.deepEqual(result.inputFiles,[resolve(fixture.owner,'src/main.cpp')]);
});

test('input inspection reports malformed JSON with target, command and configured timeout',async t=>{
  const fixture=inputFixture(t,"process.stdout.write('{\\\"schema\\\":');\n");
  await assert.rejects(inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot,timeoutMs:4321}),error=>{
    assert.match(error.message,/invalid JSON/);
    assert.match(error.message,/target=th11\/multiplayer/);
    assert.match(error.message,/\"--print-inputs\"/);
    assert.match(error.message,/timeoutMs=4321/);
    assert.match(error.message,/exitCode=0/);
    return true;
  });
});

test('input inspection reports child exit failures and captured stderr',async t=>{
  const fixture=inputFixture(t,"process.stderr.write('fixture failure');process.exit(23);\n");
  await assert.rejects(inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot,timeoutMs:5000}),error=>{
    assert.match(error.message,/Runtime input inspection failed/);
    assert.match(error.message,/target=th11\/multiplayer/);
    assert.match(error.message,/timeoutMs=5000/);
    assert.match(error.message,/exitCode=23/);
    assert.match(error.message,/fixture failure/);
    return true;
  });
});

test('input inspection terminates a controlled stalled child with SIGKILL evidence',{
  timeout:12000,
},async t=>{
  const fixture=inputFixture(t,"process.on('SIGTERM',()=>{});process.stdout.write('child started');setInterval(()=>{},1000);\n");
  const started=Date.now();
  await assert.rejects(inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot,timeoutMs:2000}),error=>{
    assert.match(error.message,/Runtime input inspection timed out/);
    assert.match(error.message,/target=th11\/multiplayer/);
    assert.match(error.message,/\"--print-inputs\"/);
    assert.match(error.message,/timeoutMs=2000/);
    assert.match(error.message,/signal=SIGKILL/);
    assert.match(error.message,/child started/);
    assert.equal(error.cause?.signal,'SIGKILL');
    return true;
  });
  assert.ok(Date.now()-started<6000,'the child is stopped within the configured bound');
});

test('input inspection distinguishes maxBuffer exhaustion from a timeout',async t=>{
  const fixture=inputFixture(t,`process.stdout.write('x'.repeat(${INPUT_INSPECTION_MAX_BUFFER_BYTES+1024}));setInterval(()=>{},1000);\n`);
  await assert.rejects(inspectInputs(fixture.target,{repositoryRoot:fixture.repositoryRoot,timeoutMs:10000}),error=>{
    assert.match(error.message,/exceeded the output limit/);
    assert.match(error.message,/ERR_CHILD_PROCESS_STDIO_MAXBUFFER/);
    assert.match(error.message,/maxBufferBytes=8388608/);
    assert.doesNotMatch(error.message,/Runtime input inspection timed out/);
    return true;
  });
});
