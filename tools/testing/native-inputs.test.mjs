import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,relative,resolve} from 'node:path';
import {nativeEnvironmentPolicy,nativeGroupEnvironment,nativeInputPlan,nativeInputSnapshot,nativeInputSnapshotsEqual,validateNativeEnvironmentPolicy} from './native-inputs.mjs';

const th11Suites=[
  new URL('../../ports/th11/web/tests/cpp/oracle-suite.json',import.meta.url),
  new URL('../../ports/th11-mp/web/tests/cpp/oracle-suite.json',import.meta.url),
];
const th15Packages=[
  new URL('../../ports/th15/web/package.json',import.meta.url),
  new URL('../../ports/th15-mp/web/package.json',import.meta.url),
];
const launcherLock=new URL('../../launcher/package-lock.json',import.meta.url);

async function snapshotFixture(game) {
  const temporaryRoot=await mkdtemp(resolve(tmpdir(),`eagler-native-inputs-${game}-`));
  const root=resolve(temporaryRoot,'ports',game,'web'),suite={game};
  await mkdir(root,{recursive:true});
  const plan=nativeInputPlan(root,suite,{});
  const fixturePath=path=>{
    const absolute=resolve(root,path),inside=relative(temporaryRoot,absolute);
    assert.ok(inside&&!inside.startsWith('..')&&!inside.startsWith('/')&&!inside.includes(':'),
      `Native fixture input escapes its temporary repository: ${path}`);
    return absolute;
  };
  for (const item of plan.directories) await mkdir(fixturePath(item.path),{recursive:true});
  await mkdir(resolve(root,'node_modules/@koromix',plan.koffiBackendPackage),{recursive:true});
  for (const item of plan.files) {
    if (item.optional) continue;
    const path=fixturePath(item.path);
    await mkdir(dirname(path),{recursive:true});
    await writeFile(path,'native input fixture');
  }
  if (game==='th15') await writeFile(resolve(root,'node_modules/fflate/index.js'),'fixture package content A');
  return {root,suite,close:()=>rm(temporaryRoot,{recursive:true,force:true})};
}

test('native comparison identity includes its shared report writer and isolates fixture repositories',async()=>{
  const first=await snapshotFixture('th15'),second=await snapshotFixture('th15');
  try{
    const firstBefore=await nativeInputSnapshot(first.root,first.suite,{});
    const secondBefore=await nativeInputSnapshot(second.root,second.suite,{});
    const path='../../../tools/testing/oracle-report-writer.mjs';
    assert.ok(firstBefore.details.some(entry=>entry.path===path&&entry.kind==='file'));
    await writeFile(resolve(first.root,path),'changed report identity behavior');
    assert.equal(nativeInputSnapshotsEqual(firstBefore,await nativeInputSnapshot(first.root,first.suite,{})),false);
    assert.equal(nativeInputSnapshotsEqual(secondBefore,await nativeInputSnapshot(second.root,second.suite,{})),true);
  }finally{await first.close();await second.close();}
});

test('TH11 log-only oracle tracing is cleared from inherited native environments',async()=>{
  const policy=nativeEnvironmentPolicy('th11');
  assert.ok(policy.clearInherited.includes('TH11_ORACLE_TRACE'));
  assert.equal(policy.defaults.TH11_ORACLE_TRACE,null);
  for (const path of th11Suites) {
    const suite=JSON.parse(await readFile(path,'utf8'));
    const result=nativeGroupEnvironment(suite,suite.groups[0],{}, {TH11_ORACLE_TRACE:'1'});
    assert.equal(result.environment.TH11_ORACLE_TRACE,undefined);
    assert.equal(result.effective.TH11_ORACLE_TRACE,null);
    assert.ok(result.unsetEnvironment.includes('TH11_ORACLE_TRACE'));
  }
});

test('TH15 fflate pin and native snapshot are scoped to TH15',async()=>{
  for (const path of th15Packages) {
    const manifest=JSON.parse(await readFile(path,'utf8'));
    assert.equal(manifest.devDependencies.fflate,'0.8.3');
  }
  const lock=JSON.parse(await readFile(launcherLock,'utf8'));
  assert.equal(lock.packages['node_modules/fflate'].version,'0.8.3');
  const th15=await snapshotFixture('th15'),th09=await snapshotFixture('th09'),th11=await snapshotFixture('th11');
  try {
    const before=await nativeInputSnapshot(th15.root,th15.suite,{});
    await writeFile(resolve(th15.root,'node_modules/fflate/index.js'),'fixture package content B');
    const after=await nativeInputSnapshot(th15.root,th15.suite,{});
    assert.equal(nativeInputSnapshotsEqual(before,after),false);
    assert.ok(before.details.some(entry=>entry.path==='node_modules/fflate/index.js'));
    for (const fixture of [th09,th11]) {
      assert.ok(!nativeInputPlan(fixture.root,fixture.suite,{}).directories.some(entry=>entry.path==='node_modules/fflate'));
      const withoutFflate=await nativeInputSnapshot(fixture.root,fixture.suite,{});
      assert.ok(!withoutFflate.details.some(entry=>entry.path.startsWith('node_modules/fflate/')));
    }
  } finally {
    await Promise.all([th15.close(),th09.close(),th11.close()]);
  }
});

test('native snapshot tracks compiled game files and consumed SDL headers without sweeping SDL',async()=>{
  const th09=nativeInputPlan('C:/fixture/ports/th09/web',{game:'th09'},{}),
    th11=nativeInputPlan('C:/fixture/ports/th11/web',{game:'th11'},{}),
    th15=nativeInputPlan('C:/fixture/ports/th15/web',{game:'th15'},{});
  assert.ok(th09.directories.some(item=>item.path==='cpp/game'));
  assert.ok(!th09.directories.some(item=>item.path==='cpp'));
  assert.deepEqual(th11.files.filter(item=>item.path.startsWith('cpp/sdl/')).map(item=>item.path),
    ['cpp/sdl/ThcrapLayout.hpp']);
  assert.deepEqual(th15.files.filter(item=>item.path.startsWith('cpp/sdl/')).map(item=>item.path),
    ['cpp/sdl/PresentationCamera.hpp','cpp/sdl/PresentationUv.hpp']);

  const fixture=await snapshotFixture('th11');
  try {
    const game=resolve(fixture.root,'cpp/game');
    await writeFile(resolve(game,'core.cpp'),'int core() { return 1; }\n');
    await writeFile(resolve(game,'core.hpp'),'int core();\n');
    const before=await nativeInputSnapshot(fixture.root,fixture.suite,{});

    const unrelated=resolve(fixture.root,'cpp/sdl/unrelated-backend.cpp');
    await mkdir(dirname(unrelated),{recursive:true});
    await writeFile(unrelated,'platform backend outside native test closure');
    const withUnrelatedSdl=await nativeInputSnapshot(fixture.root,fixture.suite,{});
    assert.equal(nativeInputSnapshotsEqual(before,withUnrelatedSdl),true);

    await writeFile(resolve(fixture.root,'cpp/sdl/ThcrapLayout.hpp'),'changed layout input');
    const changedConsumedHeader=await nativeInputSnapshot(fixture.root,fixture.suite,{});
    assert.equal(nativeInputSnapshotsEqual(withUnrelatedSdl,changedConsumedHeader),false);

    await writeFile(resolve(game,'core.cpp'),'int core() { return 2; }\n');
    const changedGameSource=await nativeInputSnapshot(fixture.root,fixture.suite,{});
    assert.equal(nativeInputSnapshotsEqual(changedConsumedHeader,changedGameSource),false);

    await writeFile(resolve(game,'added.cpp'),'int added() { return 3; }\n');
    const addedGameSource=await nativeInputSnapshot(fixture.root,fixture.suite,{});
    assert.equal(nativeInputSnapshotsEqual(changedGameSource,addedGameSource),false);

    await rm(resolve(game,'core.hpp'));
    const deletedGameHeader=await nativeInputSnapshot(fixture.root,fixture.suite,{});
    assert.equal(nativeInputSnapshotsEqual(addedGameSource,deletedGameHeader),false);
  } finally { await fixture.close(); }
});

const owners=['th09','th11','th11-mp','th15','th15-mp'];
test('full-native policy clears NODE_OPTIONS and records an unselected effective value for every owner',async()=>{
  for(const owner of owners){
    const suitePath=new URL(`../../ports/${owner}/web/tests/cpp/oracle-suite.json`,import.meta.url);
    const suite=JSON.parse(await readFile(suitePath,'utf8'));
    const policy=nativeEnvironmentPolicy(suite.game);
    assert.ok(policy.clearInherited.includes('NODE_OPTIONS'),owner);
    assert.ok(suite.nativeEnvironment.clearInherited.includes('NODE_OPTIONS'),owner);
    assert.doesNotThrow(()=>validateNativeEnvironmentPolicy(suite),owner);
    const resolved=nativeGroupEnvironment(suite,null,{}, {NODE_OPTIONS:'--test-shard=1/2'});
    assert.equal(resolved.environment.NODE_OPTIONS,undefined,owner);
    assert.equal(resolved.effective.NODE_OPTIONS,null,owner);
    assert.ok(resolved.unsetEnvironment.includes('NODE_OPTIONS'),owner);
  }
});
