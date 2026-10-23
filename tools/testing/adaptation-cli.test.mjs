import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {existsSync,mkdtempSync,readFileSync,rmSync} from 'node:fs';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';

const repository=resolve(import.meta.dirname,'../..');
const cli=resolve(repository,'tests/run.mjs');
const cleanTestInputs=()=>Object.fromEntries(Object.entries(process.env).filter(([name])=>!name.startsWith('EAGLER_TEST_')));

function runCli(root,name,args){
  const output=resolve(root,name);
  const result=spawnSync(process.execPath,[cli,'--profile=adaptation',...args,`--output=${output}`],{
    cwd:repository,env:cleanTestInputs(),encoding:'utf8',timeout:5000,windowsHide:true,
  });
  assert.equal(result.error,undefined,'tests/run.mjs must finish within the CLI test budget');
  const summaryPath=resolve(output,'summary.json');
  assert.ok(existsSync(summaryPath),'tests/run.mjs must seal summary.json');
  return{code:result.status,stdout:result.stdout,stderr:result.stderr,output,
    summary:JSON.parse(readFileSync(summaryPath,'utf8'))};
}

test('adaptation CLI keeps a selected title and mode isolated and blocks missing Runtime input',{
  timeout:9000,
},t=>{
  const root=mkdtempSync(join(tmpdir(),'adaptation-cli-'));
  t.after(()=>rmSync(root,{recursive:true,force:true}));

  const normal=runCli(root,'plan-normal',[
    '--game=th08','--variant=normal','--suite=runtime','--plan',
  ]);
  assert.equal(normal.code,0);
  assert.equal(normal.summary.status,'planned');
  assert.equal(normal.summary.passed,false);
  assert.deepEqual(normal.summary.selection.games,['th08']);
  assert.equal(normal.summary.selection.variant,'normal');
  assert.deepEqual(normal.summary.inputs,{});
  assert.deepEqual(normal.summary.cases.map(({game,variant,group})=>({game,variant,group})),[
    {game:'th08',variant:'normal',group:'runtime'},
  ]);

  const multiplayer=runCli(root,'plan-multiplayer',[
    '--game=th08','--variant=multiplayer','--suite=replay','--plan',
  ]);
  assert.equal(multiplayer.code,0);
  assert.deepEqual(multiplayer.summary.selection.games,['th08']);
  assert.equal(multiplayer.summary.selection.variant,'multiplayer');
  assert.deepEqual(multiplayer.summary.inputs,{});
  assert.deepEqual(multiplayer.summary.cases.map(({game,variant,group})=>({game,variant,group})),[
    {game:'th08',variant:'multiplayer',group:'replay'},
  ]);

  const both=runCli(root,'plan-both',[
    '--game=th08','--variant=both','--suite=replay','--plan',
  ]);
  assert.equal(both.code,0);
  assert.deepEqual(new Set(both.summary.cases.map(({game,variant,group})=>`${game}/${variant}/${group}`)),new Set([
    'th08/normal/replay','th08/multiplayer/replay',
  ]));

  const blocked=runCli(root,'run-no-inputs',[
    '--game=th08','--variant=normal','--suite=runtime',
  ]);
  assert.notEqual(blocked.code,0,'missing Runtime input must not return success');
  assert.equal(blocked.summary.schema,'eagler-touhou/adaptation-run/1');
  assert.ok(blocked.summary.runId);
  assert.ok(Number.isFinite(Date.parse(blocked.summary.finishedAt)),'blocked summary must be sealed');
  assert.equal(blocked.summary.passed,false);
  assert.equal(blocked.summary.status,'incomplete');
  assert.deepEqual(blocked.summary.inputs,{});
  assert.equal(blocked.summary.cases.length,1);
  assert.equal(blocked.summary.cases[0].status,'blocked');
  assert.equal(blocked.summary.cases[0].execution,'not-run');
  assert.equal(blocked.summary.cases[0].task,null);
  assert.deepEqual(blocked.summary.cases[0].blockedInputs.map(input=>input.name),[
    'EAGLER_TEST_RUNTIME_MANIFEST',
  ]);
  assert.equal(existsSync(resolve(blocked.output,'processes')),false,'missing input must block before a browser task starts');

  const fighting=runCli(root,'plan-fighting',[
    '--game=th075','--variant=normal','--suite=runtime','--plan',
  ]);
  assert.equal(fighting.code,0);assert.equal(fighting.summary.status,'planned');
  assert.equal(fighting.summary.cases[0].status,'blocked');
  assert.equal(fighting.summary.cases[0].task,null);
  assert.match(fighting.summary.cases[0].reason,/native observations and scenario flow/);

  const unknown=runCli(root,'unknown-game',['--game=th99','--variant=normal','--suite=runtime']);
  assert.notEqual(unknown.code,0);assert.equal(unknown.summary.status,'failed');
  assert.equal(unknown.summary.passed,false);assert.ok(unknown.summary.finishedAt);
  assert.match(unknown.summary.failure,/Unknown adaptation product th99/);
});
