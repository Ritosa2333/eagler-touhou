import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, writeFileSync, rmSync, readFileSync, copyFileSync} from 'node:fs';
import {execFileSync} from 'node:child_process';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {captureRuntimeBuildSources, executeRuntimeSourceBuild, expectedRuntimeBuildSources,
  RUNTIME_BUILD_CONTROL_FILES, RUNTIME_BUILD_HELPER_FILES, runtimeBuildReceipt} from '../../launcher/lib/runtime-build-evidence.mjs';

const repository=resolve(import.meta.dirname,'../..');

async function fixture(operation) {
  const root = mkdtempSync(resolve(tmpdir(), 'eagler-build-evidence-'));
  try {
    execFileSync('git', ['init'], {cwd:root, stdio:'ignore', windowsHide:true});
    for (const path of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]) {
      const file = resolve(root, path); mkdirSync(dirname(file), {recursive:true}); copyFileSync(resolve(repository,path),file);
    }
    for(const [path,contents] of [['ports/th20/src/main.cpp','int main(){return 0;}\n'],
      ['common/include/input.hpp','#pragma once\n']]){
      const file=resolve(root,path);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,contents);
    }
    const plan = {game:'th20', variant:'normal', source:resolve(root,'ports/th20'), common:resolve(root,'common'),
      environment:{EAGLER_COMMON_NETPLAY_ROOT:resolve(root,'common')}, execution:resolve(root,'execution.wasm'), steps:[]};
    const compile = source => ({command:process.execPath, args:['-e',source], cwd:root});
    const wasm = `require('node:fs').writeFileSync('execution.wasm',Buffer.from([0,97,115,109,1,0,0,0]));`;
    await operation({root, plan, compile, wasm});
  } finally {
    assert.equal(dirname(resolve(root)), resolve(tmpdir())); rmSync(root, {recursive:true,force:true});
  }
}

test('successful build execution binds its actual output to inputs observed before compilation',()=>fixture(async ({root,plan,compile,wasm})=>{
  plan.steps = [compile(wasm)];
  const receipt = await executeRuntimeSourceBuild(plan, root);
  assert.equal(receipt.executionSha256, createHash('sha256').update(readFileSync(plan.execution)).digest('hex'));
  assert.equal(receipt.profile, 'production');
  assert.ok(receipt.sourceFiles.some(file=>file.path === 'ports/th20/src/main.cpp'));
  assert.ok(receipt.sourceFiles.some(file=>file.path === 'common/include/input.hpp'));
  assert.ok(receipt.sourceFiles.some(file=>file.path==='tools/testing/discovery.mjs'));
  for(const helper of RUNTIME_BUILD_HELPER_FILES)assert.ok(receipt.sourceFiles.some(file=>file.path===helper),helper);
  assert.ok(!receipt.sourceFiles.some(file=>/tools\/build\/(?:.*\.test\.mjs|README\.md)$/.test(file.path)));
  assert.deepEqual(receipt.sourceFiles,expectedRuntimeBuildSources('th20','normal',root));
}));

test('expected inventory uses workspace build configuration without an installed SDK',()=>fixture(async ({root,plan})=>{
  const expected=expectedRuntimeBuildSources('th20','normal',root);
  assert.deepEqual(expected,captureRuntimeBuildSources(plan,root));
  assert.ok(expected.some(file=>file.path==='common/include/input.hpp'));
  assert.ok(expected.some(file=>file.path==='ports/th20/src/main.cpp'));
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
  assert.throws(()=>expectedRuntimeBuildSources('th20','normal',root),/requires the selected owner native sources/);
}));

test('failed compilers and mixed source generations cannot produce successful build evidence',()=>fixture(async ({root,plan,compile,wasm})=>{
  plan.steps = [compile('process.exit(7)')];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /build failed \(7\)/);
  plan.steps = [compile(wasm + `require('node:fs').appendFileSync('ports/th20/src/main.cpp','changed');`)];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /Source changed during compilation/);
  plan.steps = [compile(wasm + `require('node:fs').appendFileSync('launcher/lib/source-layout.mjs','changed');`)];
  await assert.rejects(executeRuntimeSourceBuild(plan, root), /Source changed during compilation/);
}));

test('a no-op command cannot certify a stale WASM, while a verified incremental build can reuse it',()=>fixture(async ({root,plan,compile,wasm})=>{
  writeFileSync(plan.execution,Buffer.from([0,97,115,109,1,0,0,0]));
  plan.steps=[compile('process.exit(0)')];
  await assert.rejects(executeRuntimeSourceBuild(plan,root),/did not refresh/);
  rmSync(plan.execution);
  plan.steps=[compile(`if(!require('node:fs').existsSync('execution.wasm')){${wasm}}`)];
  const previousReceipt=await executeRuntimeSourceBuild(plan,root);
  const previousPlan=structuredClone(plan);
  assert.deepEqual(await executeRuntimeSourceBuild(plan,root,{previousReceipt,previousPlan}),previousReceipt);
  await assert.rejects(executeRuntimeSourceBuild({...plan,toolchain:{version:'new-sdk'}},root,{previousReceipt,previousPlan}),/did not refresh/);
  writeFileSync(resolve(root,'ports/th20/src/main.cpp'),'changed production source');
  await assert.rejects(executeRuntimeSourceBuild(plan,root,{previousReceipt,previousPlan}),/did not refresh/);
  await assert.rejects(executeRuntimeSourceBuild({...plan,steps:[]},root),/requires executable compiler steps/);
}));

test('missing and non-WASM outputs and sources outside the checkout are rejected',()=>fixture(async ({root,plan})=>{
  const inputs = captureRuntimeBuildSources(plan, root);
  assert.throws(()=>runtimeBuildReceipt(plan, root, inputs), /ENOENT/);
  writeFileSync(plan.execution, 'not a compiler output');
  assert.throws(()=>runtimeBuildReceipt(plan, root, inputs), /not WebAssembly/);
  assert.throws(()=>captureRuntimeBuildSources({...plan, source:dirname(root)},root), /inside the current repository/);
}));
