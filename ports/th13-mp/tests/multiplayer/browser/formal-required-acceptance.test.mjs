import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {assertRuntimeBuildBinding, evaluateRequiredAcceptance} from './formal-required-acceptance.mjs';
const sha=b=>createHash('sha256').update(b).digest('hex');
const base='/work/artifacts/delivery-start-browser-20261112';
const build='/work/artifacts/work-observer-mp-zero-interpolation-20261112/build.json';
const fixtureBuild='/work/eagler-touhou/ports/th13-mp/web/artifacts/hud-fixture-20261112/build.json';
// Reproduce the historical build's source world without making a claim about
// current sources. Exact paths and hashes are checked against its build record.
const sourceRoot='/work/eagler-touhou/ports/th13-mp/';
const historicalRoot='/work/artifacts/mp-hud-framebuffer-20261112/background-fixture/';
const historicalSources=[
  ...['GameHud.cpp','GameHud.hpp','GameHudDraw.cpp','GameHudFrame.cpp','RunScene.cpp','RunScene.hpp',
    'AnmDrawSchedule.cpp','AnmDrawSchedule.hpp','HudDrawData.hpp','AnmRenderer.cpp','AnmCoordinates.cpp',
    'AnmResource.cpp','AsciiText.cpp','AsciiFrame.cpp','SessionState.hpp'].map(name=>'web/cpp/game/'+name),
  'portable/sdl/Renderer.cpp','portable/sdl/Shaders.hpp','portable/sdl/UnormBlendProfile.hpp',
  'web/cpp/sdl/Application.cpp',
];
async function historicalReadBytes(path) {
  const original=String(path), relative=original.startsWith(sourceRoot) ? original.slice(sourceRoot.length) : null;
  if (!historicalSources.includes(relative)) return readFile(path);
  const manifest=JSON.parse(await readFile(fixtureBuild,'utf8'));
  const bytes=await readFile(historicalRoot+relative);
  assert.equal(sha(bytes),manifest.sourceFiles?.[original],`historical source hash: ${relative}`);
  return bytes;
}
async function realInput() {
  const runtimeManifestBytes=await readFile(build);
  const formalReport=JSON.parse(await readFile(base+'/positive-01/formal-smoke.json','utf8'));
  // The diagnostic predates retaining these fields; derive from its two
  // independently recorded frame/epoch rows for this unit test only.
  Object.assign(formalReport,{playerCount:2,frameGuids:['frame-1','frame-2'],documentEpochs:[2,2],
    runtimeBuild:{path:'runtime-build.json',sha256:sha(runtimeManifestBytes)},
    deliveryBoundary:{closed:true,settled:true,pendingRequests:0,pendingBodies:0}});
  return {runtimeManifestBytes,formalReport};
}
function changeManifest(input, edit) {
  const changed=JSON.parse(input.runtimeManifestBytes.toString());edit(changed);
  const bytes=Buffer.from(JSON.stringify(changed));
  return {...input,runtimeManifestBytes:bytes,formalReport:{...input.formalReport,
    runtimeBuild:{path:'runtime-build.json',sha256:sha(bytes)}}};
}

test('missing evidence and arbitrary claimed success never certify fidelity', async()=>{
  const result=await evaluateRequiredAcceptance({formalReport:{passed:true,closedDelivery:{passed:true}},
    runtimeManifestBytes:Buffer.from('{}'),readBytes:async()=>{throw Error('missing')}});
  assert.equal(result.runtime.status,'BLOCKED');assert.equal(result.hud.status,'BLOCKED');
  assert.equal(result.original.status,'BLOCKED');assert.equal(result.pixel.status,'BLOCKED');
  assert.equal(result.coverage.status,'BLOCKED');assert.equal(result.passed,false);
});

test('real captured delivery binds retained f927 manifest; HUD fixture remains bounded', async()=>{
  const input=await realInput();const binding=assertRuntimeBuildBinding({expectedFiles:input.formalReport.expectedFiles,runtimeManifestBytes:input.runtimeManifestBytes});
  assert.equal(binding.manifest.sha256,'f9274f8d78fa868b3de7ccd2bf69626b2ec64c30acc891afac425444cac5bd47');
  const current=await evaluateRequiredAcceptance(input);
  assert.equal(current.runtime.status,'PASS',current.runtime.reason);
  assert.equal(current.hud.status,'BLOCKED');
  assert.match(current.hud.reason,/HUD fixture\/runtime source mismatch/);
  const result=await evaluateRequiredAcceptance({...input,readBytes:historicalReadBytes});
  assert.equal(result.runtime.status,'PASS',result.runtime.reason);
  assert.equal(result.hud.status,'PASS',result.hud.reason);
  assert.match(result.hud.scope,/not whole-program equivalence/);
  assert.equal(result.original.status,'BLOCKED');assert.equal(result.pixel.status,'BLOCKED');
  assert.equal(result.coverage.status,'BLOCKED');assert.equal(result.tool.status,'NOT_EVALUATED');
  assert.equal(result.determinism.status,'NOT_EVALUATED');assert.equal(result.passed,false);
});

test('wrong WASM, loader, manifest and delivery cannot pass binding', async()=>{
  const input=await realInput();
  for(const edit of [m=>m.sha256='0'.repeat(64),m=>m.loaderSha256='0'.repeat(64),m=>m.bytes++,m=>m.game='th15']) {
    const changed=changeManifest(input,edit);
    assert.throws(()=>assertRuntimeBuildBinding({expectedFiles:changed.formalReport.expectedFiles,runtimeManifestBytes:changed.runtimeManifestBytes}));
    const result=await evaluateRequiredAcceptance(changed);
    assert.equal(result.runtime.status,'BLOCKED');assert.equal(result.hud.status,'BLOCKED');
  }
  const changedExpected=structuredClone(input.formalReport);changedExpected.expectedFiles.find(f=>f.name==='th13-sdl.mjs').sha256='0'.repeat(64);
  assert.throws(()=>assertRuntimeBuildBinding({expectedFiles:changedExpected.expectedFiles,runtimeManifestBytes:input.runtimeManifestBytes}),/loader hash/);
  const oldRaw=JSON.parse(await readFile(base+'/negative-wrong-build-01/formal-smoke.json','utf8'));
  assert.throws(()=>assertRuntimeBuildBinding({expectedFiles:oldRaw.expectedFiles,runtimeManifestBytes:input.runtimeManifestBytes}),/WASM hash/);
  for(const edit of [
    r=>r.delivery[0].sha256='0'.repeat(64), r=>r.delivery[1].documentEpoch--,
    r=>r.delivery.pop(), r=>delete r.closedDelivery, r=>r.runtimeBuild.sha256='0'.repeat(64),
    r=>r.errors.push({message:'late response failure'}),r=>r.deliveryBoundary.pendingBodies=1,
    r=>r.deliveryBoundary.closed=false,
  ]) {
    const report=structuredClone(input.formalReport);edit(report);
    const result=await evaluateRequiredAcceptance({...input,formalReport:report});
    assert.equal(result.runtime.status,'BLOCKED');assert.equal(result.hud.status,'BLOCKED');
  }
});

test('runtime source, compiler and evidence source changes reject HUD qualification', async()=>{
  const input={...await realInput(),readBytes:historicalReadBytes};
  assert.equal((await evaluateRequiredAcceptance(input)).hud.status,'PASS','historical control must qualify');
  for(const edit of [m=>{const p=Object.keys(m.sourceFiles).find(p=>p.endsWith('/GameHud.cpp'));m.sourceFiles[p]='0'.repeat(64)},
    m=>m.flags.push('-DUNRELATED_VARIANT'),m=>m.toolchain.emccSha256='0'.repeat(64)]) {
    const result=await evaluateRequiredAcceptance(changeManifest(input,edit));
    assert.equal(result.runtime.status,'PASS');assert.equal(result.hud.status,'BLOCKED');
  }
  const result=await evaluateRequiredAcceptance({...input,readBytes:async path=>{
    const bytes=await historicalReadBytes(path);
    return String(path).endsWith('/web/cpp/game/GameHud.cpp') ? Buffer.concat([bytes,Buffer.from('changed')]) : bytes;
  }});
  assert.equal(result.runtime.status,'PASS');assert.equal(result.hud.status,'BLOCKED');
});

test('missing original, missing raw frame, tolerances and skipped coverage never certify pixel', async()=>{
  const input=await realInput();
  const result=await evaluateRequiredAcceptance({...input,comparison:{passed:true,ignore:['alpha'],tolerance:1,skipped:['stage2']},
    readBytes:async path=>{
      if(String(path).endsWith('/run04/original-index.jsonl'))throw Error('missing frame index');
      return readFile(path);
    }});
  assert.match(result.original.reason,/missing or invalid/);assert.equal(result.original.captureIntegrity,undefined);
  assert.equal(result.pixel.status,'BLOCKED');assert.equal(result.coverage.status,'BLOCKED');assert.equal(result.passed,false);
});
