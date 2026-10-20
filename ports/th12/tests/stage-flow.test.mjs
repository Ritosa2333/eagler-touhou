import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {emscripten} from '../scripts/toolchain.mjs';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..');
const output=path.join(root,'artifacts/stage-flow-tests');
fs.mkdirSync(output,{recursive:true});
const exe=path.join(output,'stage-flow.mjs');
const runner=path.join(output,'run-stage-flow.mjs');
fs.writeFileSync(runner,"import create from './stage-flow.mjs'; await create({arguments:process.argv.slice(2)});\n");
const {python,compiler}=emscripten(root);
// Build the complete current runtime: no pre-existing native library or
// platform-specific C++ installation is needed by this behavioral fixture.
const compiled=spawnSync(python,[compiler,'-std=c++20','-O1','-Isrc','-DTH12_DIAGNOSTIC=0',
  'tests/native/stage-flow.cpp',...runtimeSources(root),'-o',exe,'-sENVIRONMENT=node',
  '-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576'],
  {cwd:root,encoding:'utf8',windowsHide:true,env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache')}});
assert.equal(compiled.status,0,compiled.error?.message||compiled.stderr||compiled.stdout||'Fixture compilation failed');
test('full-game transitions preserve resources before main birth and keep standalone behavior isolated',()=>{
  const r=spawnSync(process.execPath,[runner],{cwd:root,encoding:'utf8'});
  assert.equal(r.status,0,r.stderr||r.stdout);
  assert.match(r.stdout,/Atomic main birth/);
});
test('Continue resources match selected original constructor and continue arithmetic samples',()=>{
  const native=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/stage-flow-resources-original.json')));
  for(const n of native.continue){
    const r=spawnSync(process.execPath,[runner,String(n.stage),String(n.difficulty),String(n.beforeCount),'sample','sample'],{cwd:root,encoding:'utf8'});
    assert.equal(r.status,0,r.stderr||r.stdout);
    assert.deepEqual(r.stdout.trim().split(/\s+/).map(Number),[
      n.count,n.scoreStored,n.power,n.lives,n.bombs,n.piv,n.graze,
      n.lifeFragments,n.bombFragments,n.rank,n.extendThreshold,0],
      `stage${n.stage}/difficulty${n.difficulty}/continue${n.beforeCount}`);
    assert.deepEqual(n.inventory,[0,0,0,0,0]);
  }
  for(const n of native.stageRngBoundary){assert.equal(n.seed,n.recordedSeed);assert.equal(n.calls,0);}
});

test('retail post-boss MSG retires subtitles while Stage Clear keeps player and bullets moving',{skip:!process.env.TH12_TEST_ASSETS},()=>{
  const r=spawnSync(process.execPath,[runner,path.resolve(process.env.TH12_TEST_ASSETS)],{cwd:root,encoding:'utf8'});
  assert.equal(r.status,0,r.stderr||r.stdout);
});
