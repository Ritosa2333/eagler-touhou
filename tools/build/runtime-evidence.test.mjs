import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, writeFileSync, rmSync, readFileSync, copyFileSync} from 'node:fs';
import {execFileSync} from 'node:child_process';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {captureRuntimeBuildSources, executeRuntimeSourceBuild, expectedRuntimeBuildSources,
  RUNTIME_BUILD_CONTROL_FILES, RUNTIME_BUILD_HELPER_FILES, expectedRuntimeBuildOutputs,
  runtimeBuildInputIdentity, runtimeBuildPlanIdentity, runtimeBuildReceipt} from '../../launcher/lib/runtime-build-evidence.mjs';
import {runtimePackageSourceInputsIdentity} from '../../launcher/lib/runtime-package-evidence.mjs';

const repository=resolve(import.meta.dirname,'../..');

async function fixture(operation) {
  const root = mkdtempSync(resolve(tmpdir(), 'eagler-build-evidence-'));
  try {
    execFileSync('git', ['init'], {cwd:root, stdio:'ignore', windowsHide:true});
    for (const path of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]) {
      const file = resolve(root, path); mkdirSync(dirname(file), {recursive:true}); copyFileSync(resolve(repository,path),file);
    }
    for(const [path,contents] of [['ports/th20/src/main.cpp','int main(){return 0;}\n'],
      ['ports/th20/src/A-native.cpp','int upper_native_fixture;\n'],
      ['ports/th20/src/!bang-native.cpp','int punctuation_native_fixture;\n'],
      ['ports/th20/src/_under-native.cpp','int underscore_native_fixture;\n'],
      ['common/include/input.hpp','#pragma once\n'],['ports/th20/.gitignore','web/artifacts/\n']]){
      const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,contents);
    }
    execFileSync('git',['add','--all'],{cwd:root,stdio:'ignore',windowsHide:true});
    const outputMap=expectedRuntimeBuildOutputs('th20','normal',root);
    const plan = {game:'th20', variant:'normal', source:resolve(root,'ports/th20'), common:resolve(root,'common'),
      environment:{EAGLER_COMMON_NETPLAY_ROOT:resolve(root,'common')}, execution:resolve(root,outputMap.execution.path), steps:[]};
    const compile = source => ({command:process.execPath, args:['-e',source], cwd:root});
    const wasm = `const f=require('node:fs');f.mkdirSync('ports/th20/web/artifacts/sdl3',{recursive:true});`+
      `f.writeFileSync('ports/th20/web/artifacts/sdl3/th20-sdl.wasm',Buffer.from([0,97,115,109,1,0,0,0]));`+
      `f.writeFileSync('ports/th20/web/artifacts/sdl3/th20-sdl.mjs','fixture loader');`+
      `f.writeFileSync('ports/th20/web/artifacts/sdl3/build.json','fixture manifest');`;
    await operation({root, plan, compile, wasm, outputMap});
  } finally {
    assert.equal(dirname(resolve(root)), resolve(tmpdir())); rmSync(root, {recursive:true,force:true});
  }
}

test('successful build execution binds its compiler outputs to inputs observed before compilation',()=>fixture(async ({root,plan,compile,wasm})=>{
  plan.steps = [compile(wasm)];
  const receipt = await executeRuntimeSourceBuild(plan, root);
  assert.equal(receipt.executionSha256, createHash('sha256').update(readFileSync(plan.execution)).digest('hex'));
  assert.equal(receipt.schema,'eagler-touhou/runtime-source-build/3');
  assert.equal(receipt.planSha256,runtimeBuildPlanIdentity(plan));
  assert.deepEqual(Object.keys(receipt.outputs),['execution','loader','buildManifest']);
  assert.equal(receipt.outputs.loader.sha256,createHash('sha256').update(readFileSync(resolve(root,receipt.outputs.loader.path))).digest('hex'));
  assert.equal(receipt.profile, 'production');
  assert.ok(receipt.sourceFiles.some(file=>file.path === 'ports/th20/src/main.cpp'));
  assert.ok(receipt.sourceFiles.some(file=>file.path === 'common/include/input.hpp'));
  assert.ok(receipt.sourceFiles.some(file=>file.path==='tools/testing/discovery.mjs'));
  for(const helper of RUNTIME_BUILD_HELPER_FILES)assert.ok(receipt.sourceFiles.some(file=>file.path===helper),helper);
  assert.ok(!receipt.sourceFiles.some(file=>/tools\/build\/(?:.*\.test\.mjs|README\.md)$/.test(file.path)));
  assert.deepEqual(receipt.sourceFiles,expectedRuntimeBuildSources('th20','normal',root));
}));

test('source build plan identity canonicalizes object keys and binds compiler steps',()=>{
  const first={schema:'eagler-touhou/source-build/1',game:'th20',sources:{old:'digest'},
    steps:[{command:'emcc',args:['-O2','source.cpp']}],toolchain:{version:'6.0.9',metadata:{b:2,a:1}}};
  const reordered={toolchain:{metadata:{a:1,b:2},version:'6.0.9'},steps:[{args:['-O2','source.cpp'],command:'emcc'}],
    sources:{new:'metadata'},game:'th20',schema:'different-record-version'};
  assert.equal(runtimeBuildPlanIdentity(first),runtimeBuildPlanIdentity(reordered),
    'record metadata and object property insertion order do not affect plan identity');
  reordered.steps[0].args=['-O0','source.cpp'];
  assert.notEqual(runtimeBuildPlanIdentity(first),runtimeBuildPlanIdentity(reordered),
    'compiler command flags are part of plan identity');
});

test('expected inventory uses workspace build configuration without an installed SDK',()=>fixture(async ({root,plan})=>{
  const expected=expectedRuntimeBuildSources('th20','normal',root);
  assert.deepEqual(expected,captureRuntimeBuildSources(plan,root));
  assert.ok(expected.some(file=>file.path==='common/include/input.hpp'));
  assert.ok(expected.some(file=>file.path==='ports/th20/src/main.cpp'));
  assert.ok(expected.some(file=>file.path==='ports/th20/src/A-native.cpp'));
  assert.ok(expected.some(file=>file.path==='ports/th20/src/!bang-native.cpp'));
  const comparePath=(a,b)=>a<b?-1:a>b?1:0;
  assert.deepEqual(expected.map(file=>file.path),expected.map(file=>file.path).sort(comparePath),
    'the source inventory uses stable code-point order for case and punctuation');
  const buildIdentity=runtimeBuildInputIdentity(plan,root);
  const packageIdentity=runtimePackageSourceInputsIdentity(expected,buildIdentity.requiredSubmodules);
  assert.equal(buildIdentity.sourceFilesSha256,packageIdentity.sourceFilesSha256,
    'source build and package receipts share one canonical inventory digest');
  for(const path of ['ports/th20/README.md','ports/th20/ADAPTATION.md','ports/th20/docs/guide.txt',
    'ports/th20/tests/probe.test.mjs','common/testkit/replay-verifier/replay-verifier.test.mjs']){
    const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,'documentation or test helper\n');
  }
  assert.ok(!expected.some(file=>file.path==='tools/build/runtime-evidence.test.mjs'));
  assert.ok(!expected.some(file=>file.path==='tools/build/README.md'));
  assert.ok(!expected.some(file=>/README|ADAPTATION|\/docs\/|\.test\./i.test(file.path)));
  assert.deepEqual(expectedRuntimeBuildSources('th20','normal',root),expected,
    'documentation and confirmed test-entry edits do not invalidate the compiler source receipt');
  writeFileSync(resolve(root,'common/include/input.hpp'),'#pragma once\n// production edit\n');
  assert.notDeepEqual(expectedRuntimeBuildSources('th20','normal',root),expected,
    'production source edits change the expected compiler source inventory');
  writeFileSync(resolve(root,'common/include/input.hpp'),'#pragma once\n');
  writeFileSync(resolve(root,RUNTIME_BUILD_HELPER_FILES[0]),'// builder helper edit\n');
  assert.notDeepEqual(expectedRuntimeBuildSources('th20','normal',root),expected,
    'real production build-helper edits change the expected compiler source inventory');
  writeFileSync(resolve(root,RUNTIME_BUILD_HELPER_FILES[0]),readFileSync(resolve(repository,RUNTIME_BUILD_HELPER_FILES[0])));
  rmSync(resolve(root,'ports/th20/src/main.cpp'));
  assert.throws(()=>expectedRuntimeBuildSources('th20','normal',root),/Tracked or discovered Runtime build input is missing/);
}));

test('failed compilers and mixed source generations cannot produce successful build evidence',()=>fixture(async ({root,plan,compile,wasm})=>{
  plan.steps = [compile('process.exit(7)')];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /build failed \(7\)/);
  plan.steps = [compile(wasm + `require('node:fs').appendFileSync('ports/th20/src/main.cpp','changed');`)];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /Source changed during compilation/);
  plan.steps = [compile(wasm + `require('node:fs').appendFileSync('launcher/lib/source-layout.mjs','changed');`)];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /Source changed during compilation/);
}));

test('a no-op command cannot certify stale outputs, while verified incremental builds can reuse them',()=>fixture(async ({root,plan,compile,wasm,outputMap})=>{
  for(const [role,{path}] of Object.entries(outputMap)){
    const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});
    writeFileSync(file,role==='execution'?Buffer.from([0,97,115,109,1,0,0,0]):`stale ${role}`);
  }
  for(const role of ['loader','buildManifest']){
    plan.steps=[compile(`require('node:fs').writeFileSync(${JSON.stringify(outputMap[role].path)},'non-execution refresh');`)];
    await assert.rejects(executeRuntimeSourceBuild(plan,root),/did not refresh code outputs/,
      `refreshing only ${role} cannot certify a stale execution WASM`);
  }
  plan.steps=[compile('process.exit(0)')];
  await assert.rejects(executeRuntimeSourceBuild(plan,root),/did not refresh/);
  for(const {path} of Object.values(outputMap))rmSync(resolve(root,path));
  const outputDirectory='ports/th20/web/artifacts/sdl3';
  plan.steps=[compile(`const f=require('node:fs');if(!f.existsSync(${JSON.stringify(outputMap.execution.path)})){${wasm}}`+
    `if(f.existsSync('${outputDirectory}/rebind.flag'))f.writeFileSync('${outputDirectory}/build.json','rebound build metadata');`+
    `if(f.existsSync('${outputDirectory}/loader-only.flag'))f.writeFileSync(${JSON.stringify(outputMap.loader.path)},'loader-only mutation');`)];
  const previousReceipt=await executeRuntimeSourceBuild(plan,root);
  const previousPlan=structuredClone(plan);
  assert.deepEqual(await executeRuntimeSourceBuild(plan,root,{previousReceipt,previousPlan}),previousReceipt);
  writeFileSync(resolve(root,outputDirectory,'rebind.flag'),'refresh metadata only');
  const rebound=await executeRuntimeSourceBuild(plan,root,{previousReceipt,previousPlan});
  assert.notEqual(rebound.outputs.buildManifest.sha256,previousReceipt.outputs.buildManifest.sha256,
    'a valid unchanged code build may rebind manifest metadata against its matching prior receipt');
  assert.equal(rebound.outputs.loader.sha256,previousReceipt.outputs.loader.sha256);
  assert.equal(rebound.outputs.execution.sha256,previousReceipt.outputs.execution.sha256);
  const reboundPlan=structuredClone(plan);
  rmSync(resolve(root,outputDirectory,'rebind.flag'));
  writeFileSync(resolve(root,outputDirectory,'loader-only.flag'),'mutate loader without refreshing WASM');
  const loaderBeforeMutation=readFileSync(resolve(root,outputMap.loader.path));
  const wasmBeforeLoaderMutation=readFileSync(resolve(root,outputMap.execution.path));
  await assert.rejects(executeRuntimeSourceBuild(plan,root,{previousReceipt:rebound,previousPlan:reboundPlan}),/did not refresh code outputs/,
    'a same-plan compiler step that changes only the loader cannot reuse the prior receipt');
  assert.equal(readFileSync(resolve(root,outputMap.loader.path),'utf8'),'loader-only mutation');
  assert.deepEqual(readFileSync(resolve(root,outputMap.execution.path)),wasmBeforeLoaderMutation,
    'the negative control changes only the loader and leaves execution WASM bytes untouched');
  rmSync(resolve(root,outputDirectory,'loader-only.flag'));
  writeFileSync(resolve(root,outputMap.loader.path),loaderBeforeMutation);
  await assert.rejects(executeRuntimeSourceBuild({...plan,toolchain:{version:'new-sdk'}},root,{previousReceipt,previousPlan}),/did not refresh/);
  await assert.rejects(executeRuntimeSourceBuild({...plan,toolchain:{version:'new-sdk'}},root,{previousReceipt:rebound,previousPlan:reboundPlan}),/did not refresh/);
  writeFileSync(resolve(root,'ports/th20/src/main.cpp'),'changed production source');
  await assert.rejects(executeRuntimeSourceBuild(plan,root,{previousReceipt:rebound,previousPlan:reboundPlan}),/did not refresh/);
  await assert.rejects(executeRuntimeSourceBuild({...plan,steps:[]},root),/requires executable compiler steps/);
}));

test('missing and non-WASM outputs and sources outside the checkout are rejected',()=>fixture(async ({root,plan,outputMap})=>{
  const inputs = captureRuntimeBuildSources(plan, root);
  assert.throws(()=>runtimeBuildReceipt(plan, root, inputs), /regular file/);
  for(const [role,{path}] of Object.entries(outputMap)){
    const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});
    writeFileSync(file,role==='execution'?'not a compiler output':'fixture output');
  }
  writeFileSync(plan.execution, 'not a compiler output');
  assert.throws(()=>runtimeBuildReceipt(plan, root, inputs), /not WebAssembly/);
  assert.throws(()=>captureRuntimeBuildSources({...plan, source:dirname(root)},root), /inside (?:the current repository|the workspace)/);
}));
