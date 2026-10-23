import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {copyFile, mkdir, mkdtemp, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname, join, relative, resolve} from 'node:path';
import {
  RUNTIME_PACKAGE_BUILD_SCHEMA, RUNTIME_PACKAGE_CONTROL_FILES, captureRuntimePackageControlInputs,
  captureRuntimePackageFiles, captureRuntimePackageInputs, createRuntimePackageReceipt,
  loadRuntimePackageConfig, resolveRuntimePackagePlan, runtimePackageIdentity,
} from '../lib/runtime-package-evidence.mjs';
import {RUNTIME_BUILD_CONTROL_FILES, RUNTIME_BUILD_HELPER_FILES, captureRuntimeBuildSources, expectedRuntimeBuildOutputs,
  runtimeBuildInputIdentity, runtimeBuildPlanIdentity, runtimeBuildReceipt, verifyRuntimeBuildOutputs} from '../lib/runtime-build-evidence.mjs';
import {verifyRuntimePackageBuildOutputs} from '../lib/runtime-package-evidence.mjs';
import {PRODUCT_GAMES} from '../lib/contracts/product-catalog.mjs';

const project = resolve(import.meta.dirname, '../..');
const variantCount=Object.values(PRODUCT_GAMES).reduce((total,product)=>total+1+Number(!!product.multiplayerRuntime),0);
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const identity = (name, bytes) => ({bytes: bytes.length, sha256: hash(bytes)});
const workspace = await mkdtemp(join(tmpdir(), 'runtime-package-evidence-'));

try {
  const output = join(workspace, 'runtime');
  await mkdir(output);
  const payload = {
    'th06.html': Buffer.from('<html>fixture</html>'),
    'th06.js': Buffer.from('globalThis.fixture = true;'),
    'th06.wasm': Buffer.from([0, 97, 115, 109, 1, 0, 0, 0]),
    'manifest.json': Buffer.from(JSON.stringify({game: 'th06', variant: 'normal', protocol: 'eagler-touhou/1'})),
    'runtime-generation.json': Buffer.from('{"generation":"fixture-generation"}'),
  };
  for (const [name, bytes] of Object.entries(payload)) await writeFile(join(output, name), bytes);
  const inventory = {
    schema: 'eagler-touhou/runtime-directory/1', game: 'th06', variant: 'normal',
    files: Object.fromEntries(Object.entries(payload).map(([name, bytes]) => [name, identity(name, bytes)])),
  };
  await writeFile(join(output, 'runtime-files.json'), `${JSON.stringify(inventory, null, 2)}\n`);
  const captured = captureRuntimePackageFiles(output, {game: 'th06', variant: 'normal', entry: 'th06.html'});
  assert.deepEqual(Object.keys(captured.files).sort(), [...Object.keys(payload), 'runtime-files.json'].sort());
  assert.equal(captured.packageSha256,
    '02e9114ab77ccb79c114a8c385eddc5f1fc88d9c8f32f43cf49b7261ab9bfb7d',
    'package identity follows the published Runtime identity object and excludes descriptors');
  assert.equal(captured.packageSha256, runtimePackageIdentity({
    game: 'th06', variant: 'normal', entry: 'th06.html',
    files: Object.fromEntries(Object.entries(captured.files).reverse()),
  }), 'identity is independent of file-map insertion order');
  const originalFiles = captured.files;
  const receipt = createRuntimePackageReceipt({
    game: 'th06', variant: 'normal', entry: 'th06.html', packageSha256: captured.packageSha256,
    files: captured.files, sourceBuildReceiptSha256: '1'.repeat(64), sourceInputsSha256: '2'.repeat(64),
    packagerInputs: [{path: 'ports/th06/portable/package-eagler.mjs', sha256: '3'.repeat(64)}],
  });
  assert.equal(receipt.schema, RUNTIME_PACKAGE_BUILD_SCHEMA);
  assert.deepEqual(receipt.files, originalFiles, 'receipt preserves hashes for every closed package file, including inventory');
  const descriptorBytes = Buffer.from('{"generation":"changed-generation"}');
  await writeFile(join(output, 'runtime-generation.json'), descriptorBytes);
  inventory.files['runtime-generation.json'] = identity('runtime-generation.json', descriptorBytes);
  await writeFile(join(output, 'runtime-files.json'), `${JSON.stringify(inventory, null, 2)}\n`);
  const changedDescriptor = captureRuntimePackageFiles(output, {game: 'th06', variant: 'normal', entry: 'th06.html'});
  assert.equal(changedDescriptor.packageSha256, captured.packageSha256,
    'inventory and generation descriptors do not change the payload identity');
  assert.notDeepEqual(changedDescriptor.files, captured.files,
    'the complete receipt map still records descriptor byte changes');
  await writeFile(join(output, 'th06.data'), Buffer.from('private game data'));
  assert.throws(() => captureRuntimePackageFiles(output, {game: 'th06', variant: 'normal', entry: 'th06.html'}), /not closed/,
    'private DATA cannot leak into the Runtime code package');
  await rm(join(output, 'th06.data'));

  const config = loadRuntimePackageConfig(project);
  assert.deepEqual(Object.keys(config.games), Object.keys(PRODUCT_GAMES));
  const plans = Object.entries(config.games).flatMap(([game, value]) => Object.keys(value.variants).map(variant =>
    resolveRuntimePackagePlan(game, variant, project)));
  assert.equal(plans.length, variantCount, 'formal package plans cover every catalogued Runtime variant');
  assert.equal(plans.filter(plan => plan.kind === 'cmake').length, 4, 'th06/th07 use the CMake code-only packager');
  const sourceOutputOwners=new Map();
  for (const plan of plans) {
    assert.equal(plan.entry, `${plan.game}.html`);
    assert.ok(plan.output.startsWith(plan.source), `${plan.game}/${plan.variant} package output belongs to its source owner`);
    if (plan.kind === 'cmake') assert.deepEqual(plan.files, [`${plan.game}.html`, `${plan.game}.js`, `${plan.game}.wasm`]);
    else assert.ok(plan.controlFiles.includes(plan.recipe.script), `${plan.game}/${plan.variant} records its official packager`);
    const outputs=expectedRuntimeBuildOutputs(plan.game,plan.variant,project);
    assert.equal(relative(project,plan.sourceExecution).replaceAll('\\','/'),outputs.execution.path);
    for(const [role,output] of Object.entries(outputs)){
      assert.equal(sourceOutputOwners.has(output.path),false,
        `${plan.game}/${plan.variant}/${role} must not collide with ${sourceOutputOwners.get(output.path)}`);
      sourceOutputOwners.set(output.path,`${plan.game}/${plan.variant}/${role}`);
    }
  }
  const th09Normal=resolveRuntimePackagePlan('th09','normal',project);
  const th09Multiplayer=resolveRuntimePackagePlan('th09','multiplayer',project);
  assert.equal(th09Normal.sourceBuild.environment.TH09_OUTPUT,'artifacts/sdl-release');
  assert.equal(th09Multiplayer.sourceBuild.environment.TH09_OUTPUT,'artifacts/sdl-multiplayer');
  const th09OutputFixture=join(workspace,'th09-output-matrix');
  const normalOutputs=Object.entries(expectedRuntimeBuildOutputs('th09','normal',project));
  const multiplayerOutputs=Object.entries(expectedRuntimeBuildOutputs('th09','multiplayer',project));
  const normalSnapshot=new Map();
  for(const [role,{path}] of normalOutputs){
    const destination=join(th09OutputFixture,path),bytes=Buffer.from(`normal ${role}`);
    await mkdir(dirname(destination),{recursive:true});await writeFile(destination,bytes);normalSnapshot.set(path,bytes);
  }
  for(const [role,{path}] of multiplayerOutputs){
    const destination=join(th09OutputFixture,path);
    await mkdir(dirname(destination),{recursive:true});await writeFile(destination,Buffer.from(`multiplayer ${role}`));
  }
  for(const [path,bytes] of normalSnapshot)
    assert.deepEqual(await readFile(join(th09OutputFixture,path)),bytes,'the multiplayer output directory leaves normal source outputs unchanged');

  const fixture = join(workspace, 'fixture-repository');
  await mkdir(fixture);
  const copyPaths = new Set([
    ...RUNTIME_PACKAGE_CONTROL_FILES,
    ...RUNTIME_BUILD_CONTROL_FILES,
    ...RUNTIME_BUILD_HELPER_FILES,
    'launcher/config/runtime-packages.json',
    'launcher/config/runtime-builds.json',
    'launcher/config/workspace.json',
    'ports/th08/portable/package-eagler.mjs',
    'ports/th08/portable/presentation-lab/native-abi.mjs',
  ]);
  for (const path of copyPaths) {
    const destination = join(fixture, path);
    await mkdir(dirname(destination), {recursive: true});
    await copyFile(join(project, path), destination);
  }
  const fixtureSource = join(fixture, 'ports/th08');
  const fixtureCommon = join(fixture, 'common');
  await mkdir(join(fixtureSource, 'src'), {recursive: true});
  await mkdir(join(fixtureCommon, 'include/eagler'), {recursive: true});
  await writeFile(join(fixtureSource, 'src/minimal.cpp'), 'int fixture_source() { return 1; }\n');
  await writeFile(join(fixtureSource, '.gitignore'), 'web/artifacts/\n');
  await writeFile(join(fixtureCommon, 'include/eagler/minimal.hpp'), '#pragma once\n');
  execFileSync('git', ['init', '--quiet'], {cwd: fixture, windowsHide: true});
  execFileSync('git', ['add', '--all'], {cwd: fixture, windowsHide: true});
  const fixturePlan = resolveRuntimePackagePlan('th08', 'normal', fixture);
  const expectedOutputs=expectedRuntimeBuildOutputs('th08','normal',fixture);
  for(const [role,{path}] of Object.entries(expectedOutputs)){
    const bytes=role==='execution'?Buffer.from([0,97,115,109,1,0,0,0]):Buffer.from(`fresh ${role} fixture`);
    await mkdir(dirname(join(fixture,path)),{recursive:true});
    await writeFile(join(fixture,path),bytes);
  }
  const sourceBuildPlan = {
    game: 'th08', variant: 'normal', source: fixtureSource, common: fixtureCommon,
    execution: fixturePlan.sourceExecution,
    environment: {EAGLER_COMMON_NETPLAY_ROOT: fixtureCommon}, requiredSubmodules: [],
    steps:[{command:'fixture-compiler',args:['--release']}],features:{thcrap:true,thprac:true},
    toolchain:{version:'fixture-toolchain',compiler:'fixture-compiler'},
  };
  sourceBuildPlan.sources = runtimeBuildInputIdentity(sourceBuildPlan, fixture);
  const buildPlanPath = join(fixture, 'artifacts/validation/source-builds/th08-normal.json');
  await mkdir(dirname(buildPlanPath), {recursive: true});
  await writeFile(buildPlanPath, `${JSON.stringify(sourceBuildPlan, null, 2)}\n`);
  const sourceFiles=captureRuntimeBuildSources(sourceBuildPlan,fixture);
  const sourceReceipt=runtimeBuildReceipt(sourceBuildPlan,fixture,sourceFiles);
  assert.equal(sourceReceipt.planSha256,runtimeBuildPlanIdentity(sourceBuildPlan));
  await writeFile(join(dirname(buildPlanPath),'th08-normal.runtime.json'),`${JSON.stringify(sourceReceipt,null,2)}\n`);
  const sourceOutputDigests=verifyRuntimeBuildOutputs(sourceReceipt,'th08','normal',fixture);
  const packageCodeFiles={files:{
    'th08.html':{bytes:12,sha256:hash(Buffer.from('fresh entry'))},
    'th08-sdl.mjs':{bytes:Buffer.byteLength('fresh loader fixture'),sha256:hash(Buffer.from('fresh loader fixture'))},
    'th08-sdl.wasm':{bytes:8,sha256:sourceOutputDigests.execution.sha256},
  }};
  assert.equal(verifyRuntimePackageBuildOutputs(packageCodeFiles,fixturePlan,sourceOutputDigests),true);
  packageCodeFiles.files['th08-sdl.mjs']={bytes:Buffer.byteLength('stale loader fixture'),sha256:hash(Buffer.from('stale loader fixture'))};
  const recomputedPackageSha=runtimePackageIdentity({game:'th08',variant:'normal',entry:'th08.html',files:packageCodeFiles.files});
  assert.match(recomputedPackageSha,/^[a-f0-9]{64}$/);
  assert.throws(()=>verifyRuntimePackageBuildOutputs(packageCodeFiles,fixturePlan,sourceOutputDigests),/packaged loader is not the current compiler output/,
    'recomputing the package identity cannot certify a stale packaged loader');
  await writeFile(join(fixture,sourceOutputDigests.loader.path),'mutated source loader');
  assert.throws(()=>verifyRuntimeBuildOutputs(sourceReceipt,'th08','normal',fixture),/source Runtime output changed/,
    'the compiler output receipt also rejects a changed source loader');

  const originalCwd = process.cwd();
  try {
    process.chdir(tmpdir());
    const packageInputs = captureRuntimePackageInputs(fixturePlan, fixture);
    assert.ok(packageInputs.sourceFiles.some(file => file.path === 'ports/th08/src/minimal.cpp'));
    assert.ok(packageInputs.packagerInputs.some(file => file.path === 'ports/th08/portable/package-eagler.mjs'));
    assert.ok(packageInputs.packagerInputs.some(file => file.path === 'launcher/scripts/package-runtime.mjs'));
    assert.deepEqual(captureRuntimePackageControlInputs(fixturePlan, fixture), packageInputs.packagerInputs);
    sourceBuildPlan.steps[0].args=['--unrelated-output'];
    await writeFile(buildPlanPath,`${JSON.stringify(sourceBuildPlan,null,2)}\n`);
    assert.throws(()=>captureRuntimePackageInputs(fixturePlan,fixture),/plan identity does not match/,
      'changing the recorded compiler command invalidates package preparation');
  } finally {
    process.chdir(originalCwd);
  }
} finally {
  await rm(workspace, {recursive: true, force: true});
}

console.log(JSON.stringify({runtimePackageEvidence: 'PASS', variants: variantCount, schema: RUNTIME_PACKAGE_BUILD_SCHEMA}));
