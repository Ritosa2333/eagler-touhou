import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdtemp,rm,mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';

test('ordinary Runtime entry rejects MP before reading private inputs or opening a browser',async()=>{
  const temporary=await mkdtemp(resolve(tmpdir(),'runtime-lane-'));
  try{
    for(const variant of ['multiplayer','both']){
      const output=resolve(temporary,variant);
      const result=spawnSync(process.execPath,[resolve(import.meta.dirname,'runtime-browser.mjs'),
        `--manifest=${resolve(temporary,'missing-private-inputs.json')}`,`--output=${output}`,
        '--game=th08',`--variant=${variant}`],{encoding:'utf8',windowsHide:true,timeout:15000});
      assert.equal(result.status,1,result.stderr);assert.match(result.stderr,/requires --variant=normal/);
      assert.match(result.stderr,/room-browser\.mjs/);assert.doesNotMatch(result.stderr,/ENOENT|browserType\.launch/);
      const report=JSON.parse(await readFile(resolve(output,'report.json')));
      assert.equal(report.passed,false);assert.equal(report.status,'failed');assert.ok(report.finishedAt);
    }
  }finally{await rm(temporary,{recursive:true,force:true});}
});

test('malformed manifest replaces an older passing report in both desktop browser entries',async()=>{
  const temporary=await mkdtemp(resolve(tmpdir(),'browser-stale-report-'));
  try{
    const manifest=resolve(temporary,'bad.json');await writeFile(manifest,'{invalid');
    for(const entry of ['runtime','room']){
      const output=resolve(temporary,entry);await mkdir(output);
      await writeFile(resolve(output,'report.json'),JSON.stringify({status:'passed',passed:true,runId:'old'}));
      const argumentsForEntry=entry==='runtime'?['--variant=normal']:['--url=http://127.0.0.1:9/','--players=2','--route=rtc'];
      const child=spawnSync(process.execPath,[resolve(import.meta.dirname,`${entry}-browser.mjs`),
        `--manifest=${manifest}`,`--output=${output}`,'--game=th08',...argumentsForEntry],
      {encoding:'utf8',windowsHide:true,timeout:15000});
      assert.equal(child.status,1,child.stderr);assert.match(child.stderr,/SyntaxError/);
      const report=JSON.parse(await readFile(resolve(output,'report.json')));
      assert.equal(report.status,'failed');assert.equal(report.passed,false);assert.notEqual(report.runId,'old');
      assert.ok(report.finishedAt);assert.match(report.failure,/SyntaxError/);
    }
  }finally{await rm(temporary,{recursive:true,force:true});}
});

test('a misspelled or duplicated scenario cannot retain an old passing browser report',async()=>{
  const temporary=await mkdtemp(resolve(tmpdir(),'browser-invalid-scenario-'));
  try{
    for(const [entry,extra,expected] of [
      ['room',['--lifecyle'],/Unknown option/],
      ['room',['--spectator=false'],/does not accept a value/],
      ['runtime',['--game=th11'],/Duplicate option/],
    ]){
      const output=await mkdtemp(resolve(temporary,entry+'-'));
      await writeFile(resolve(output,'report.json'),JSON.stringify({status:'passed',passed:true,runId:'old'}));
      const child=spawnSync(process.execPath,[resolve(import.meta.dirname,entry+'-browser.mjs'),
        '--manifest='+resolve(temporary,'does-not-exist.json'),'--output='+output,'--game=th08',...extra],
      {encoding:'utf8',windowsHide:true,timeout:15000});
      assert.equal(child.status,1,child.stderr);assert.match(child.stderr,expected);
      assert.doesNotMatch(child.stderr,/ENOENT|browserType\.launch/);
      const report=JSON.parse(await readFile(resolve(output,'report.json')));
      assert.equal(report.passed,false);assert.equal(report.status,'failed');assert.notEqual(report.runId,'old');
      assert.deepEqual(report.targets,[]);assert.ok(report.finishedAt);
    }
  }finally{assert.ok(temporary.startsWith(resolve(tmpdir(),'browser-invalid-scenario-')));await rm(temporary,{recursive:true,force:true});}
});
