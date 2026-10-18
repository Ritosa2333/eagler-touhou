import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdtemp,mkdir,writeFile,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {runFrozenOracle} from './frozen-oracle.mjs';
import {oracleInputs,captureOracleInputs,assertOracleInputsUnchanged} from './oracle-inputs.mjs';

const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const inputHelper=readFile(resolve(dirname(fileURLToPath(import.meta.url)),'oracle-inputs.mjs'));

async function fixture() {
  const repo=await mkdtemp(resolve(tmpdir(),'eagler-frozen-oracle-'));
  const root=resolve(repo,'ports/th09/web'),core=Buffer.from([0,97,115,109,1,0,0,0]);
  const files={
    '.gitignore':'artifacts/\n',
    'tools/testing/oracle-inputs.mjs':await inputHelper,
    'common/include/eagler/thprac/PracticeInput.hpp':'#pragma once\nstruct PracticeInput {};\n',
    'ports/th09/web/target.json':JSON.stringify({game:'th09'}),
    'ports/th09/web/scripts/cpp/build.mjs':'// fixture builder\n',
    'ports/th09/web/cpp/game/core.cpp':'int core() { return 7; }\n',
    'ports/th09/web/cpp/game/core.hpp':'int core();\n',
    'ports/th09/web/tests/cpp/core-exports.cpp':'int exported() { return 1; }\n',
    'ports/th09/web/tests/cpp/fixture.test.mjs':"import test from 'node:test';import assert from 'node:assert/strict';test('fixture',()=>assert.equal(2+2,4));\n",
  };
  for (const [path,contents] of Object.entries(files)) {
    const destination=resolve(repo,path);
    await mkdir(dirname(destination),{recursive:true});
    await writeFile(destination,contents);
  }
  const inputs=oracleInputs(root),sourceHashes={};
  for (const path of [...inputs.sources,...inputs.headers,...inputs.controlFiles])
    sourceHashes[path]=digest(await readFile(resolve(root,path)));
  const output=resolve(root,'artifacts/cpp');
  await mkdir(output,{recursive:true});
  await writeFile(resolve(output,'game-core-test.wasm'),core);
  await writeFile(resolve(output,'game-core-test-build.json'),JSON.stringify({
    kind:'th09-original-comparison-fixture',completeGame:false,...inputs,sha256:digest(core),sourceHashes,
  }));
  const git=(args)=>execFileSync('git',args,{cwd:repo,encoding:'utf8',windowsHide:true,stdio:'ignore'});
  git(['init']);git(['config','user.email','oracle-fixture@example.invalid']);git(['config','user.name','Oracle fixture']);
  git(['add','.']);git(['commit','-m','fixture']);
  return {repo,root,git,close:async()=>{assert.equal(dirname(repo),resolve(tmpdir()));await rm(repo,{recursive:true,force:true});}};
}

test('frozen oracle rejects empty and skipped selections, then accepts a complete pass',async()=>{
  const f=await fixture();
  try {
    const file=resolve(f.root,'tests/cpp/fixture.test.mjs');
    await rm(file);
    await assert.rejects(runFrozenOracle({root:f.root}),/No oracle tests selected/);
    await writeFile(file,"import test from 'node:test';test.skip('missing input',()=>{});\n");
    const skipped=await runFrozenOracle({root:f.root});
    assert.equal(skipped.passed,false);assert.ok(skipped.skipped>0);
    await writeFile(file,"import test from 'node:test';import assert from 'node:assert/strict';test('fixture',()=>assert.equal(2+2,4));\n");
    const result=await runFrozenOracle({root:f.root});
    assert.equal(result.passed,true,JSON.stringify(result)+'\n'+await readFile(result.log,'utf8'));
    assert.equal(result.tests,1);
    assert.equal(result.sourceTreeChanged,false);
  } finally {await f.close();}
});

test('frozen oracle rejects a newly added native source before running tests',async()=>{
  const f=await fixture();
  try {
    await writeFile(resolve(f.root,'cpp/game/added.cpp'),'int added() { return 8; }\n');
    await assert.rejects(runFrozenOracle({root:f.root}),/Native build inventory mismatch: sources/);
  } finally {await f.close();}
});

test('native build input capture rejects changed and newly added sources',async()=>{
  const f=await fixture();
  try {
    const captured=captureOracleInputs(f.root);
    await writeFile(resolve(f.root,'cpp/game/core.cpp'),'int core() { return 9; }\n');
    assert.throws(()=>assertOracleInputsUnchanged(f.root,captured),/Oracle input changed during build: cpp\/game\/core\.cpp/);
    const added=captureOracleInputs(f.root);
    await writeFile(resolve(f.root,'cpp/game/added.cpp'),'int added() { return 1; }\n');
    assert.throws(()=>assertOracleInputsUnchanged(f.root,added),/Oracle input inventory changed during build: sources/);
  } finally {await f.close();}
});

test('frozen oracle requires source hashes for every enumerated input',async()=>{
  const f=await fixture();
  try {
    const path=resolve(f.root,'artifacts/cpp/game-core-test-build.json');
    const manifest=JSON.parse(await readFile(path,'utf8'));
    delete manifest.sourceHashes[Object.keys(manifest.sourceHashes)[0]];
    await writeFile(path,JSON.stringify(manifest));
    await assert.rejects(runFrozenOracle({root:f.root}),/source hash key set mismatch/);
  } finally {await f.close();}
});

test('frozen oracle fails when an imported build helper changes during the test run',async()=>{
  const f=await fixture();
  try {
    const helper=resolve(f.repo,'tools/testing/oracle-inputs.mjs');
    const file=resolve(f.root,'tests/cpp/fixture.test.mjs');
    await writeFile(file,[
      "import test from 'node:test';",
      "import assert from 'node:assert/strict';",
      "import {readFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('waits for helper mutation',async()=>{",
      "  await new Promise(done=>setTimeout(done,700));",
      "  const path=resolve(process.env.EAGLER_ORACLE_ROOT,'../../../tools/testing/oracle-inputs.mjs');",
      "  assert.match(await readFile(path,'utf8'),/ORACLE_HELPER_MUTATED/);",
      "});",
    ].join('\n')+'\n');
    const pending=runFrozenOracle({root:f.root});
    await new Promise(done=>setTimeout(done,250));
    await writeFile(helper,(await readFile(helper,'utf8'))+'\n// ORACLE_HELPER_MUTATED\n');
    const result=await pending;
    assert.equal(result.tests,1);
    assert.equal(result.pass,1);
    assert.equal(result.passed,false,JSON.stringify(result));
    assert.equal(result.sourceTreeChanged,true);
    assert.ok(result.sourceChanges.includes('../../../tools/testing/oracle-inputs.mjs'));
  } finally {await f.close();}
});
