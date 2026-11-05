import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {targets,inspectInputs,root} from '../ports/inputs.mjs';

for(const game of ['th01','th02','th03','th04','th05'])test(`${game} source inspection needs no private resources or native compiler`,
  {skip:!existsSync(resolve(root,`ports/${game}/source-import.json`))},()=>{
  const target=targets({game,variant:'normal'})[0],report=inspectInputs(target);
  const owner=resolve(root,target.directory),baseline=JSON.parse(readFileSync(resolve(owner,'source-import.json'),'utf8'));
  assert.equal(report.schema,'eagler-touhou/runtime-inputs/1');
  assert.deepEqual(report.translationUnits,[],'managed assembly does not claim native compilation');
  for(const name of ['web/managed-shell.mjs','web/managed-input.mjs','scripts/build-eagler.mjs','source-import.json'])
    assert.ok(report.inputFiles.includes(resolve(owner,name)),`source identity covers ${name}`);
  assert.ok(report.inputFiles.includes(resolve(root,'common/browser/directory-keyboard.mjs')));
  for(const resource of baseline.resources)assert.ok(!report.inputFiles.includes(resolve(owner,'web',resource)),resource);
  assert.ok(report.compilerFlags.includes('native-core=external-precompiled'));
});
