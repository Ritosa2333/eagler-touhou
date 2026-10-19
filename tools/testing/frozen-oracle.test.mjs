import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync,spawnSync} from 'node:child_process';
import {mkdtemp,mkdir,writeFile,readFile,rm,symlink,unlink} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {runFrozenOracle,runOracleMaintenance} from './frozen-oracle.mjs';
import {acceptedReferenceSnapshot,acceptedReferenceSnapshotsEqual,loadOracleSuite,maintenanceOutputPath,preflightOracleInputs} from './oracle-suite.mjs';
import {oracleInputs,captureOracleInputs,assertOracleInputsUnchanged} from './oracle-inputs.mjs';
import {nativeEnvironmentPolicy,nativeInputPlan,nativeInputSnapshot,nativeInputSnapshotsEqual} from './native-inputs.mjs';

const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const inputHelper=readFile(resolve(dirname(fileURLToPath(import.meta.url)),'oracle-inputs.mjs'));

async function fixture() {
  const repo=await mkdtemp(resolve(tmpdir(),'eagler-frozen-oracle-'));
  const root=resolve(repo,'ports/th09/web'),core=Buffer.from([0,97,115,109,1,0,0,0]);
  const files={
    '.gitignore':'**/artifacts/\n.local/\nprivate/\n**/reference/assets/\n',
    'tools/testing/oracle-inputs.mjs':await inputHelper,
    'common/include/eagler/thprac/PracticeInput.hpp':'#pragma once\nstruct PracticeInput {};\n',
    'ports/th09/web/target.json':JSON.stringify({game:'th09'}),
    'ports/th09/web/package.json':JSON.stringify({type:'module',dependencies:{}}),
    'ports/th09/web/tests/cpp/oracle-suite.json':JSON.stringify({
      schema:'eagler-touhou/native-oracle-suite/1',owner:'th09',game:'th09',variant:'normal',
      discovery:{directory:'tests/cpp',pattern:'*.test.mjs'},nativeEnvironment:nativeEnvironmentPolicy('th09'),baseInputs:[],
      groups:[{id:'unit',requiredInputs:[]},{id:'private',requiredInputs:[
        {id:'replay',kind:'receipt',receiptId:'replay',inputKind:'file'},
        {id:'trace',kind:'file',path:'private/accepted.trace',sha256:digest(Buffer.from('accepted trace bytes'))},
      ]}],
      tests:[{file:'fixture.test.mjs',role:'candidate',group:'unit'},{file:'blocked.test.mjs',role:'candidate',group:'private'}],
      maintenance:[{id:'maintenance-check',kind:'test',file:'maintenance-check.mjs',outputRoot:'artifacts/validation/oracle-maintenance',requiredInputs:[]}],
    }),
    'ports/th09/web/scripts/cpp/build.mjs':'// fixture builder\n',
    'ports/th09/web/cpp/game/core.cpp':'int core() { return 7; }\n',
    'ports/th09/web/cpp/game/core.hpp':'int core();\n',
    'ports/th09/web/tests/cpp/core-exports.cpp':'int exported() { return 1; }\n',
    'ports/th09/web/tests/cpp/helper.mjs':'export function fixtureValue() { return 4; }\n',
    'ports/th09/web/tests/cpp/fixture.test.mjs':"import test from 'node:test';import assert from 'node:assert/strict';import {fixtureValue} from './helper.mjs';test('fixture helper is consumed',()=>assert.equal(fixtureValue(),4));\n",
    'ports/th09/web/tests/cpp/blocked.test.mjs':"import test from 'node:test';import assert from 'node:assert/strict';test('private input',()=>assert.equal(3*3,9));\n",
    'ports/th09/web/tests/cpp/maintenance-check.mjs':[
      "import test from 'node:test';import assert from 'node:assert/strict';import {mkdir,writeFile,readFile} from 'node:fs/promises';import {resolve} from 'node:path';",
      "test('maintenance writes only to its staging output',async()=>{const output=process.env.EAGLER_MAINTENANCE_OUTPUT;assert.ok(output);await mkdir(output,{recursive:true});const path=resolve(output,'receipt.json');await writeFile(path,JSON.stringify({schema:'fixture/1',passed:true}));assert.deepEqual(JSON.parse(await readFile(path,'utf8')),{schema:'fixture/1',passed:true});});",
    ].join('\n')+'\n',
  };
  for (const [path,contents] of Object.entries(files)) {
    const destination=resolve(repo,path);
    await mkdir(dirname(destination),{recursive:true});
    await writeFile(destination,contents);
  }
  await prepareNativeFixture(repo,root);
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

async function prepareNativeFixture(repo,root) {
  const plan=nativeInputPlan(root,{game:'th09'},{}),workspace=resolve(dirname(fileURLToPath(import.meta.url)),'../..');
  for (const item of plan.directories) {
    const directory=resolve(root,item.path);await mkdir(directory,{recursive:true});
    if (item.path.startsWith('node_modules/'))
      await writeFile(resolve(directory,'package.json'),JSON.stringify({name:item.path,version:'fixture'}));
  }
  const koffiBackend=resolve(root,'node_modules/@koromix',plan.koffiBackendPackage);
  await mkdir(koffiBackend,{recursive:true});
  await writeFile(resolve(koffiBackend,'package.json'),JSON.stringify({name:`@koromix/${plan.koffiBackendPackage}`,version:'fixture'}));
  for (const item of plan.files) {
    const destination=resolve(root,item.path);
    try {await readFile(destination);continue;} catch {}
    if (item.optional) continue;
    let source;
    if (item.path.startsWith('../../../tools/testing/'))
      source=resolve(workspace,'tools/testing',item.path.split('/').at(-1));
    else if (item.path.startsWith('../../../common/testkit/'))
      source=resolve(workspace,'common/testkit',item.path.slice('../../../common/testkit/'.length));
    else if (item.path.startsWith('../tools/replay-verifier/'))
      source=resolve(workspace,'ports/th09/tools/replay-verifier',item.path.slice('../tools/replay-verifier/'.length));
    else if (item.path==='sdl-runtime/motion-replay.mjs') source=resolve(workspace,'ports/th09/web',item.path);
    if (source) {
      try {await mkdir(dirname(destination),{recursive:true});await writeFile(destination,await readFile(source));continue;} catch {}
    }
    await mkdir(dirname(destination),{recursive:true});await writeFile(destination,'// native input fixture\n');
  }
}

test('frozen oracle rejects unclassified and skipped selections, then accepts a complete pass',async()=>{
  const f=await fixture();
  try {
    const file=resolve(f.root,'tests/cpp/fixture.test.mjs');
    await rm(file);
    await assert.rejects(loadOracleSuite(f.root),/Oracle test classification mismatch/);
    await writeFile(file,"import test from 'node:test';test.skip('missing input',()=>{});\n");
    const skipped=await runFrozenOracle({root:f.root,environment:{}});
    assert.equal(skipped.passed,false);assert.ok(skipped.skipped>0);
    assert.equal(skipped.groups.find(group=>group.id==='private').status,'blocked');
    assert.equal(skipped.groups.find(group=>group.id==='unit').status,'failed');
    await writeFile(file,"import test from 'node:test';import assert from 'node:assert/strict';test('fixture',()=>assert.equal(2+2,4));\n");
    const readyWhileBlocked=await runFrozenOracle({root:f.root,name:'ready-while-blocked',environment:{}});
    assert.equal(readyWhileBlocked.passed,false);
    assert.equal(readyWhileBlocked.groups.find(group=>group.id==='unit').status,'passed');
    assert.equal(readyWhileBlocked.groups.find(group=>group.id==='private').status,'blocked');
    assert.equal(readyWhileBlocked.tests,1);
    assert.equal(readyWhileBlocked.pass,1);
    const data=resolve(f.root,'private/replay.rpy'),receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(data),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    await writeFile(resolve(f.root,'private/accepted.trace'),'accepted trace bytes');
    await writeFile(receiptPath,JSON.stringify({schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('accepted replay bytes'))},
    }}));
    const result=await runFrozenOracle({root:f.root,environment:{EAGLER_ORACLE_INPUT_RECEIPT:receiptPath}});
    assert.equal(result.passed,true,JSON.stringify(result));
    assert.equal(result.tests,2);
    assert.equal(result.pass,2);
    assert.equal(result.groups[0].status,'passed');
    assert.equal(result.nativeInputsChanged,false);
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
    const capturedAddition=captureOracleInputs(f.root);
    await rm(resolve(f.root,'cpp/game/added.cpp'));
    assert.throws(()=>assertOracleInputsUnchanged(f.root,capturedAddition),/Oracle input inventory changed during build: sources/);
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
      "import {readFile,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('waits for helper mutation',async()=>{",
      "  await writeFile(resolve(process.env.EAGLER_ORACLE_ROOT,'artifacts/cpp/helper-started'),'started');",
      "  const path=resolve(process.env.EAGLER_ORACLE_ROOT,'../../../tools/testing/oracle-inputs.mjs');",
      "  const deadline=Date.now()+10000;",
      "  while(!(await readFile(path,'utf8')).includes('ORACLE_HELPER_MUTATED')&&Date.now()<deadline)",
      "    await new Promise(done=>setTimeout(done,20));",
      "  assert.match(await readFile(path,'utf8'),/ORACLE_HELPER_MUTATED/);",
      "});",
    ].join('\n')+'\n');
    const pending=runFrozenOracle({root:f.root});
    const marker=resolve(f.root,'artifacts/cpp/helper-started');let started=false;
    for(let i=0;i<500&&!started;i++){
      try{await readFile(marker);started=true;}catch{await new Promise(done=>setTimeout(done,20));}
    }
    assert.equal(started,true,'candidate started after the build preflight');
    await writeFile(helper,(await readFile(helper,'utf8'))+'\n// ORACLE_HELPER_MUTATED\n');
    const result=await pending;
    assert.equal(result.tests,1);
    assert.equal(result.pass,1);
    assert.equal(result.passed,false,JSON.stringify(result));
    assert.equal(result.nativeInputsChanged,true);
    assert.ok(result.nativeInputs.before.sha256!==result.nativeInputs.after.sha256);
  } finally {await f.close();}
});

test('frozen oracle detects a consumed tests/cpp helper changing during the run',async()=>{
  const f=await fixture();
  try {
    const helper=resolve(f.root,'tests/cpp/helper.mjs');
    const file=resolve(f.root,'tests/cpp/fixture.test.mjs');
    await writeFile(helper,"export const fixtureValue='before';\n");
    await writeFile(file,[
      "import test from 'node:test';",
      "import assert from 'node:assert/strict';",
      "import {readFile,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "import {fixtureValue} from './helper.mjs';",
      "test('consumes helper while it is frozen',async()=>{",
      "  assert.equal(fixtureValue,'before');",
      "  await writeFile(resolve(process.env.EAGLER_ORACLE_ROOT,'artifacts/cpp/helper-started'),'started');",
      "  const path=resolve(process.env.EAGLER_ORACLE_ROOT,'tests/cpp/helper.mjs');",
      "  const deadline=Date.now()+10000;",
      "  while(!(await readFile(path,'utf8')).includes(\"fixtureValue='after'\")&&Date.now()<deadline)",
      "    await new Promise(done=>setTimeout(done,20));",
      "  assert.match(await readFile(path,'utf8'),/fixtureValue='after'/);",
      "});",
    ].join('\n')+'\n');
    const pending=runFrozenOracle({root:f.root});
    const marker=resolve(f.root,'artifacts/cpp/helper-started');let started=false;
    for(let i=0;i<500&&!started;i++){
      try{await readFile(marker);started=true;}catch{await new Promise(done=>setTimeout(done,20));}
    }
    assert.equal(started,true,'candidate imported the helper after native preflight');
    await writeFile(helper,"export const fixtureValue='after';\n");
    const result=await pending;
    assert.equal(result.tests,1);
    assert.equal(result.pass,1);
    assert.equal(result.passed,false,JSON.stringify(result));
    assert.equal(result.nativeInputsChanged,true);
  } finally {await f.close();}
});

test('native freeze ignores unrelated launcher and documentation edits',async()=>{
  const f=await fixture();
  try {
    await writeFile(resolve(f.root,'tests/cpp/fixture.test.mjs'),[
      "import test from 'node:test';",
      "import {mkdir,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('changes unrelated workspace files',async()=>{",
      "  const root=process.env.EAGLER_ORACLE_ROOT;",
      "  await mkdir(resolve(root,'../../../docs'),{recursive:true});",
      "  await writeFile(resolve(root,'../../../docs/unrelated.md'),'outside native closure');",
      "  await mkdir(resolve(root,'../../../launcher/tests'),{recursive:true});",
      "  await writeFile(resolve(root,'../../../launcher/tests/browser-only.mjs'),'outside native closure');",
      "});",
    ].join('\n')+'\n');
    const result=await runFrozenOracle({root:f.root,name:'unrelated-edits',environment:{}});
    assert.equal(result.groups.find(group=>group.id==='unit').status,'passed');
    assert.equal(result.nativeInputsChanged,false,JSON.stringify(result.nativeInputs));
    assert.equal(result.head.before,result.head.after);
  } finally {await f.close();}
});

test('complete native environment profile overrides inherited limits and rejects narrowed suite configuration',async()=>{
  const f=await fixture();
  try {
    await writeFile(resolve(f.root,'tests/cpp/fixture.test.mjs'),[
      "import test from 'node:test';",
      "import assert from 'node:assert/strict';",
      "test('uses the complete replay range',()=>{",
      "  assert.equal(process.env.TH09_REPLAY_LIMIT,'4294967295');",
      "  assert.equal(process.env.TH09_BATTLE_PAIR,undefined);",
      "  assert.equal(process.env.TH09_DEMO_CASE,undefined);",
      "});",
    ].join('\n')+'\n');
    const result=await runFrozenOracle({root:f.root,name:'scope-clears-inherited',environment:{
      TH09_REPLAY_LIMIT:'12',TH09_BATTLE_PAIR:'0,1',TH09_DEMO_CASE:'2',
    }});
    const unit=result.groups.find(group=>group.id==='unit');
    assert.equal(unit.status,'passed');
    assert.equal(unit.effectiveScope.TH09_REPLAY_LIMIT,'4294967295');
    assert.equal(unit.effectiveScope.TH09_BATTLE_PAIR,null);
    const path=resolve(f.root,'tests/cpp/oracle-suite.json'),suite=JSON.parse(await readFile(path,'utf8'));
    suite.groups[0].environment={TH09_REPLAY_LIMIT:'12'};
    await writeFile(path,JSON.stringify(suite));
    await assert.rejects(loadOracleSuite(f.root),/narrows the full profile/);
  } finally {await f.close();}
});

test('native input freeze binds installed dependency junction targets and contents',async()=>{
  const f=await fixture();
  try {
    const dependency=resolve(f.root,'node_modules/@alexaltea/unicorn-js'),
      first=resolve(f.repo,'unicorn-install-a'),second=resolve(f.repo,'unicorn-install-b');
    await rm(dependency,{recursive:true,force:true});
    await mkdir(first,{recursive:true});await mkdir(second,{recursive:true});
    await writeFile(resolve(first,'unicorn.js'),'same installed bytes');
    await writeFile(resolve(second,'unicorn.js'),'same installed bytes');
    const linkType=process.platform==='win32'?'junction':'dir';
    await symlink(first,dependency,linkType);
    const suite=await loadOracleSuite(f.root),plan=nativeInputPlan(f.root,suite,{}),before=await nativeInputSnapshot(f.root,suite,{});
    await unlink(dependency);await symlink(second,dependency,linkType);
    const moved=await nativeInputSnapshot(f.root,suite,{});
    assert.equal(before.bytes,moved.bytes);
    assert.equal(nativeInputSnapshotsEqual(before,moved),false);
    assert.notEqual(before.sha256,moved.sha256);
    await mkdir(resolve(f.root,'tests/cpp/empty-helper-directory'));
    const addedDirectory=await nativeInputSnapshot(f.root,suite,{});
    assert.equal(nativeInputSnapshotsEqual(moved,addedDirectory),false);
    const backendPackage=resolve(f.root,'node_modules/@koromix',plan.koffiBackendPackage,'package.json');
    await writeFile(backendPackage,'{"version":"changed backend"}');
    const changedBackend=await nativeInputSnapshot(f.root,suite,{});
    assert.equal(nativeInputSnapshotsEqual(addedDirectory,changedBackend),false);
  } finally {await f.close();}
});

test('native freeze detects a private receipt rewrite while a ready group runs',async()=>{
  const f=await fixture();
  try {
    const data=resolve(f.root,'private/replay.rpy'),receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(data),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    const receipt={schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('accepted replay bytes'))},
    }};
    await writeFile(receiptPath,JSON.stringify(receipt));
    await writeFile(resolve(f.root,'tests/cpp/fixture.test.mjs'),[
      "import test from 'node:test';",
      "import {mkdir,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('waits while the receipt is rewritten',async()=>{",
      "  const marker=resolve(process.env.EAGLER_ORACLE_ROOT,'artifacts/cpp/receipt-started');",
      "  await mkdir(resolve(marker,'..'),{recursive:true});await writeFile(marker,'started');",
      "  await new Promise(done=>setTimeout(done,500));",
      "});",
    ].join('\n')+'\n');
    const pending=runFrozenOracle({root:f.root,name:'receipt-mutated',environment:{EAGLER_ORACLE_INPUT_RECEIPT:receiptPath}});
    const marker=resolve(f.root,'artifacts/cpp/receipt-started');let started=false;
    for(let i=0;i<100&&!started;i++){
      try{await readFile(marker);started=true;}catch{await new Promise(done=>setTimeout(done,20));}
    }
    assert.equal(started,true);
    await writeFile(receiptPath,JSON.stringify(receipt)+' ');
    const result=await pending;
    assert.equal(result.nativeInputsChanged,true);
    assert.equal(result.protectedInputsChanged,true);
    assert.equal(result.passed,false);
  } finally {await f.close();}
});

test('oracle preflight requires private input receipt hashes and detects input changes',async()=>{
  const f=await fixture();
  try {
    const data=resolve(f.root,'private/replay.rpy');
    await mkdir(dirname(data),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    const receipt={schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('accepted replay bytes'))},
    }};
    const receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(receiptPath,JSON.stringify(receipt));
    const suite=await loadOracleSuite(f.root),required=[{id:'replay',kind:'receipt',receiptId:'replay'}];
    const environment={EAGLER_ORACLE_INPUT_RECEIPT:receiptPath};
    const missing=await preflightOracleInputs(f.root,suite,required,{});
    assert.equal(missing.status,'blocked');
    assert.match(missing.blocked[0].reason,/EAGLER_ORACLE_INPUT_RECEIPT is required/);
    const initial=await preflightOracleInputs(f.root,suite,required,environment);
    assert.equal(initial.status,'ready');
    await writeFile(data,'mutated replay bytes');
    const changed=await preflightOracleInputs(f.root,suite,required,environment);
    assert.equal(changed.status,'blocked');
    assert.match(changed.blocked[0].reason,/Private input hash mismatch/);
    await writeFile(data,'accepted replay bytes');
    delete receipt.inputs.replay.sha256;
    await writeFile(receiptPath,JSON.stringify(receipt));
    const unhashed=await preflightOracleInputs(f.root,suite,required,environment);
    assert.equal(unhashed.status,'blocked');
    assert.match(unhashed.blocked[0].reason,/authoritative path\/hash/);
  } finally {await f.close();}
});

test('private receipt environment paths resolve to the same hashed input',async()=>{
  const f=await fixture();
  try {
    const data=resolve(f.root,'private/replay.rpy'),other=resolve(f.root,'private/alternate.rpy');
    await mkdir(dirname(data),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    await writeFile(other,'different consumer bytes');
    const receipt={schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',envPath:'private/../private/replay.rpy',
        sha256:digest(Buffer.from('accepted replay bytes'))},
    }};
    const receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(receiptPath,JSON.stringify(receipt));
    const suite=await loadOracleSuite(f.root),required=[{
      id:'replay',kind:'receipt',receiptId:'replay',inputKind:'file',pathEnv:'TH09_REPLAY_FILE',
    }],environment={EAGLER_ORACLE_INPUT_RECEIPT:receiptPath};
    const samePath=await preflightOracleInputs(f.root,suite,required,environment);
    assert.equal(samePath.status,'ready',JSON.stringify(samePath));
    assert.equal(samePath.envOverrides.TH09_REPLAY_FILE,data);

    receipt.inputs.replay.envPath='private/alternate.rpy';
    await writeFile(receiptPath,JSON.stringify(receipt));
    const differentPath=await preflightOracleInputs(f.root,suite,required,environment);
    assert.equal(differentPath.status,'blocked');
    assert.match(differentPath.blocked[0].reason,/envPath must resolve to its hashed path/);
  } finally {await f.close();}
});

test('maintenance output is confined to ignored staging',async()=>{
  const f=await fixture();
  try {
    const task={id:'archive-collect',outputRoot:'artifacts/validation/oracle-maintenance'};
    assert.equal(maintenanceOutputPath(f.root,task,'review-20261019'),
      resolve(f.root,'artifacts/validation/oracle-maintenance/archive-collect/review-20261019'));
    assert.throws(()=>maintenanceOutputPath(f.root,{...task,outputRoot:'tests/cpp'},'review-20261019'),
      /must be git-ignored/);
    assert.throws(()=>maintenanceOutputPath(f.root,task,'../../reference'),
      /safe identifiers/);
    const link=resolve(f.root,'artifacts/validation/oracle-maintenance');
    const outside=resolve(f.root,'private-target');
    await mkdir(dirname(link),{recursive:true});await mkdir(outside,{recursive:true});
    await symlink(outside,link,process.platform==='win32'?'junction':'dir');
    assert.throws(()=>maintenanceOutputPath(f.root,task,'linked-output'),/cannot cross a symbolic link/);
  } finally {await f.close();}
});

test('maintenance runs a counted Node test and rejects baseline mutation',async()=>{
  const f=await fixture();
  try {
    const result=await runOracleMaintenance({root:f.root,taskId:'maintenance-check',name:'maintenance-smoke'});
    assert.equal(result.passed,true,JSON.stringify(result)+'\n'+await readFile(result.log,'utf8'));
    assert.equal(result.status,'passed');assert.equal(result.exitCode,0);
    assert.equal(result.tests,1);assert.equal(result.pass,1,await readFile(result.log,'utf8'));
    assert.deepEqual(JSON.parse(await readFile(resolve(result.output,'receipt.json'),'utf8')),
      {schema:'fixture/1',passed:true});
  } finally {await f.close();}
});

test('candidate result detects ignored accepted-reference deletion and addition',async()=>{
  const f=await fixture();
  try {
    const reference=resolve(f.root,'reference/assets');
    await mkdir(reference,{recursive:true});
    await writeFile(resolve(reference,'accepted.bin'),'accepted bytes');
    await writeFile(resolve(f.root,'tests/cpp/fixture.test.mjs'),[
      "import test from 'node:test';",
      "import {mkdir,writeFile,rm} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('candidate cannot change accepted reference',async()=>{",
      "  const dir=resolve(process.env.EAGLER_ORACLE_ROOT,'reference/assets');",
      "  await rm(resolve(dir,'accepted.bin'));await writeFile(resolve(dir,'added.bin'),'unaccepted bytes');",
      "});",
    ].join('\n')+'\n');
    const data=resolve(f.root,'private/replay.rpy'),receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(data),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    await writeFile(resolve(f.root,'private/accepted.trace'),'accepted trace bytes');
    await writeFile(receiptPath,JSON.stringify({schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('accepted replay bytes'))},
    }}));
    const result=await runFrozenOracle({root:f.root,name:'baseline-mutation',environment:{EAGLER_ORACLE_INPUT_RECEIPT:receiptPath}});
    assert.equal(result.groups.every(group=>group.status==='passed'),true,JSON.stringify(result));
    assert.equal(result.nativeInputsChanged,false);
    assert.equal(result.baselineChanged,true);
    assert.equal(result.acceptedReference.before.files,1);
    assert.equal(result.acceptedReference.after.files,1);
    assert.notEqual(result.acceptedReference.before.sha256,result.acceptedReference.after.sha256);
    assert.equal(result.passed,false);
    assert.equal(result.exitCode,1);
  } finally {await f.close();}
});

test('candidate rechecks captured inputs from blocked groups after other groups run',async()=>{
  const f=await fixture();
  try {
    const data=resolve(f.root,'private/replay.rpy'),receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(data),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(data,'accepted replay bytes');
    await writeFile(receiptPath,JSON.stringify({schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('accepted replay bytes'))},
    }}));
    await writeFile(resolve(f.root,'tests/cpp/fixture.test.mjs'),[
      "import test from 'node:test';",
      "import {writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('mutates another blocked group input',async()=>{",
      "  await writeFile(resolve(process.env.EAGLER_ORACLE_ROOT,'private/replay.rpy'),'changed by ready group');",
      "});",
    ].join('\n')+'\n');
    const result=await runFrozenOracle({root:f.root,name:'blocked-input-mutation',environment:{EAGLER_ORACLE_INPUT_RECEIPT:receiptPath}});
    const unit=result.groups.find(group=>group.id==='unit'),privateGroup=result.groups.find(group=>group.id==='private');
    assert.equal(unit.status,'passed');
    assert.equal(privateGroup.status,'blocked');
    assert.equal(privateGroup.inputsBefore.length,1);
    assert.equal(privateGroup.protectedInputsChanged,true);
    assert.equal(result.protectedInputsChanged,true);
    assert.equal(result.passed,false);
  } finally {await f.close();}
});

test('accepted-reference snapshot follows declared junctions and binds target identity',async()=>{
  const f=await fixture();
  try {
    const reference=resolve(f.root,'reference'),link=resolve(reference,'assets'),
      first=resolve(f.repo,'private-reference-a'),second=resolve(f.repo,'private-reference-b');
    await mkdir(reference,{recursive:true});
    await mkdir(first,{recursive:true});await mkdir(second,{recursive:true});
    await writeFile(resolve(first,'accepted.bin'),'same accepted bytes');
    await writeFile(resolve(second,'accepted.bin'),'same accepted bytes');
    const linkType=process.platform==='win32'?'junction':'dir';
    await symlink(first,link,linkType);
    const suite=await loadOracleSuite(f.root);
    suite.groups[0].requiredInputs.push({expectedPath:'reference/assets'});
    const before=await acceptedReferenceSnapshot(f.root,suite);
    assert.equal(before.rootKind,'directory');
    assert.equal(before.links.length,1);
    assert.ok(before.links[0].target);
    assert.ok(Number.isSafeInteger(before.bytes)&&before.bytes>=Buffer.byteLength('same accepted bytes'));
    assert.ok(Number.isSafeInteger(before.files)&&before.files>=1);
    assert.equal(acceptedReferenceSnapshotsEqual(before,await acceptedReferenceSnapshot(f.root,suite)),true,
      'An unchanged declared reference junction must remain a valid frozen input');
    await unlink(link);await symlink(second,link,linkType);
    const moved=await acceptedReferenceSnapshot(f.root,suite);
    assert.equal(before.links[0].sha256,moved.links[0].sha256);
    assert.notEqual(before.sha256,moved.sha256);
    assert.notEqual(before.links[0].target,moved.links[0].target);
    assert.equal(acceptedReferenceSnapshotsEqual(before,moved),false);
    await writeFile(resolve(second,'accepted.bin'),'changed accepted bytes');
    const changed=await acceptedReferenceSnapshot(f.root,suite);
    assert.equal(acceptedReferenceSnapshotsEqual(moved,changed),false);
  } finally {await f.close();}
});

test('maintenance baseline changes fail even when its Node test passes',async()=>{
  const f=await fixture();
  try {
    await writeFile(resolve(f.root,'tests/cpp/maintenance-check.mjs'),[
      "import test from 'node:test';",
      "import {mkdir,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('writes candidate baseline accidentally',async()=>{",
      "  const baseline=resolve(process.env.EAGLER_ORACLE_ROOT,'reference/assets');",
      "  await mkdir(baseline,{recursive:true});await writeFile(resolve(baseline,'leaked.bin'),'must be rejected');",
      "});",
    ].join('\n')+'\n');
    const result=await runOracleMaintenance({root:f.root,taskId:'maintenance-check',name:'baseline-control'});
    assert.equal(result.tests,1);assert.equal(result.pass,1);
    assert.equal(result.baselineChanged,true);
    assert.equal(result.passed,false);assert.equal(result.exitCode,1);
  } finally {await f.close();}
});

test('oracle CLI accepts explicit modes and rejects duplicate mode flags',async()=>{
  const f=await fixture();
  try {
    const cli=resolve(dirname(fileURLToPath(import.meta.url)),'oracle-cli.mjs');
    const cleanEnvironment={...process.env};delete cleanEnvironment.EAGLER_ORACLE_INPUT_RECEIPT;
    const preflight=spawnSync(process.execPath,[cli,'--root='+f.root,'--mode=preflight'],
      {cwd:f.repo,encoding:'utf8',windowsHide:true,env:cleanEnvironment});
    assert.equal(preflight.status,1,preflight.stderr);
    const preflightResult=JSON.parse(preflight.stdout);
    assert.equal(preflightResult.groups.find(group=>group.id==='unit').status,'ready');
    assert.equal(preflightResult.groups.find(group=>group.id==='private').status,'blocked');
    const maintenance=spawnSync(process.execPath,[cli,'--root='+f.root,'--mode=maintenance','--task=maintenance-check',
      '--name=cli-smoke'],{cwd:f.repo,encoding:'utf8',windowsHide:true,env:cleanEnvironment});
    assert.equal(maintenance.status,0,maintenance.stderr+'\n'+maintenance.stdout);
    assert.equal(JSON.parse(maintenance.stdout).passed,true);
    const duplicate=spawnSync(process.execPath,[cli,'--root='+f.root,'--mode=preflight','--mode=candidate'],
      {cwd:f.repo,encoding:'utf8',windowsHide:true,env:cleanEnvironment});
    assert.notEqual(duplicate.status,0);
    assert.match(duplicate.stderr,/Invalid oracle argument/);
  } finally {await f.close();}
});

test('candidate start replaces an old green result with a failed build-preflight report',async()=>{
  const f=await fixture();
  try {
    const resultPath=resolve(f.root,'artifacts/cpp/verification/full-suite-result.json');
    await mkdir(dirname(resultPath),{recursive:true});
    await writeFile(resultPath,JSON.stringify({passed:true}));
    const build=resolve(f.root,'artifacts/cpp/game-core-test-build.json');
    const manifest=JSON.parse(await readFile(build,'utf8'));manifest.sha256='0'.repeat(64);
    await writeFile(build,JSON.stringify(manifest));
    await assert.rejects(runFrozenOracle({root:f.root}),/Core\/build manifest mismatch/);
    const result=JSON.parse(await readFile(resultPath,'utf8'));
    assert.equal(result.status,'failed');assert.equal(result.passed,false);assert.equal(result.exitCode,1);
    assert.ok(result.runId);assert.notEqual(result.runId,undefined);
    assert.equal(result.baselineChanged,null);
    assert.match(result.error.message,/Core\/build manifest mismatch/);
  } finally {await f.close();}
});

test('frozen oracle invalidates a passing candidate if a private input changes during execution',async()=>{
  const f=await fixture();
  try {
    const data=resolve(f.root,'private/replay.rpy'),receiptPath=resolve(f.root,'.local/oracle-inputs.json');
    await mkdir(dirname(data),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
    await writeFile(data,'before run');
    await writeFile(resolve(f.root,'private/accepted.trace'),'accepted trace bytes');
    await writeFile(receiptPath,JSON.stringify({schema:'eagler-touhou/oracle-private-inputs/1',owner:'th09',inputs:{
      replay:{kind:'file',path:'private/replay.rpy',sha256:digest(Buffer.from('before run'))},
    }}));
    await writeFile(resolve(f.root,'tests/cpp/blocked.test.mjs'),[
      "import test from 'node:test';",
      "import {mkdir,writeFile} from 'node:fs/promises';",
      "import {resolve} from 'node:path';",
      "test('private input remains present',async()=>{",
      "  const marker=resolve(process.env.EAGLER_ORACLE_ROOT,'artifacts/cpp/private-started');",
      "  await mkdir(resolve(marker,'..'),{recursive:true});await writeFile(marker,'started');",
      "  await new Promise(done=>setTimeout(done,650));",
      "});",
    ].join('\n')+'\n');
    const pending=runFrozenOracle({root:f.root,environment:{EAGLER_ORACLE_INPUT_RECEIPT:receiptPath}});
    const marker=resolve(f.root,'artifacts/cpp/private-started');let started=false;
    for(let i=0;i<100&&!started;i++){
      try{await readFile(marker);started=true;}catch{await new Promise(done=>setTimeout(done,20));}
    }
    assert.equal(started,true,'private candidate started');
    await writeFile(data,'changed during execution');
    const result=await pending,group=result.groups.find(item=>item.id==='private');
    assert.equal(result.passed,false);
    assert.equal(group.status,'failed');
    assert.equal(group.tests,1);assert.equal(group.pass,1);
    assert.equal(group.privateInputsChanged,true);
  } finally {await f.close();}
});
