import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..');
const output=path.join(root,'artifacts/stage-flow-tests');
fs.mkdirSync(output,{recursive:true});
const exe=path.join(output,'stage-flow.exe');
const compiler=process.env.TH12_NATIVE_CXX||'C:/mingw64/bin/g++.exe';
// Link fresh changed translation units against the existing same-runtime
// native library; this avoids rebuilding unrelated scene/graphics work.
const compiled=spawnSync(compiler,['-std=c++20','-O1','-Isrc',
  'tests/native/stage-flow.cpp','src/game/GameSession.cpp','src/game/StageFlow.cpp',
  'src/game/Dialogue.cpp','src/game/SpellHistory.cpp','src/wasm/Exports.cpp','artifacts/native-cpp/libth12_runtime.a',
  '-o',exe],{cwd:root,encoding:'utf8'});
assert.equal(compiled.status,0,compiled.stderr||compiled.stdout);
test('full-game transitions preserve resources before main birth and keep standalone behavior isolated',()=>{
  const r=spawnSync(exe,[],{cwd:root,encoding:'utf8'});
  assert.equal(r.status,0,r.stderr||r.stdout);
});
test('Continue resources match selected original constructor and continue arithmetic samples',()=>{
  const native=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/stage-flow-resources-original.json')));
  for(const n of native.continue){
    const r=spawnSync(exe,[String(n.stage),String(n.difficulty),String(n.beforeCount),'sample','sample'],{cwd:root,encoding:'utf8'});
    assert.equal(r.status,0,r.stderr||r.stdout);
    assert.deepEqual(r.stdout.trim().split(/\s+/).map(Number),[
      n.count,n.scoreStored,n.power,n.lives,n.bombs,n.piv,n.graze,
      n.lifeFragments,n.bombFragments,n.rank,n.extendThreshold,0],
      `stage${n.stage}/difficulty${n.difficulty}/continue${n.beforeCount}`);
    assert.deepEqual(n.inventory,[0,0,0,0,0]);
  }
  for(const n of native.stageRngBoundary){assert.equal(n.seed,n.recordedSeed);assert.equal(n.calls,0);}
});
