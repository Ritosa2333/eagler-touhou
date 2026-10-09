import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, writeFileSync, readFileSync, existsSync, rmSync, readdirSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {
  resolvePackagePlan, validateBuildManifest, validateNativeCapabilities,
  createRuntimeManifest, packageEagler, MULTIPLAYER_EXPORTS, sha256,
} from '../package-eagler.mjs';

const revision = 'e02347fac98c9e599d30a6ffd7ef30756555b948';
const leb = value => {
  const bytes = [];
  do { const part = value & 127; value >>>= 7; bytes.push(part | (value ? 128 : 0)); } while (value);
  return bytes;
};
const string = value => [...leb(Buffer.byteLength(value)), ...Buffer.from(value)];
const section = (id, bytes) => [id, ...leb(bytes.length), ...bytes];
function wasm(names, importName = null) {
  const types = section(1, [1, 0x60, 0, 0]);
  const imports = importName ? section(2, [1, ...string('env'), ...string(importName), 0, 0]) : [];
  const functions = section(3, [...leb(names.length), ...names.map(() => 0)]);
  const exports = section(7, [...leb(names.length),
    ...names.flatMap((name, i) => [...string(name), 0, ...leb(i + (importName ? 1 : 0))])]);
  const code = section(10, [...leb(names.length), ...names.flatMap(() => [2, 0, 0x0b])]);
  return Buffer.from([0, 97, 115, 109, 1, 0, 0, 0, ...types, ...imports, ...functions, ...exports, ...code]);
}
function buildFor(bytes, multiplayer = false) {
  const sourceFiles = {'source.cpp': sha256('// source\n')};
  return {
    game: 'th11', profile: multiplayer ? 'multiplayer' : 'sdl3',
    variant: multiplayer ? 'multiplayer' : 'normal', diagnostic: false,
    version: 'test', builtAt: '2026-10-09T00:00:00.000Z', kind: 'cpp-sdl3',
    features: {thprac: false, languages: true, ...(multiplayer ? {multiplayer: true} : {})},
    ...(multiplayer ? {commonRevision: revision,
      netplay: {prediction: false, rollback: false, measuredStartup: true},
      runtimeSourceFiles: {'adonis-calibration.mjs': sha256('// calibration\n')}} : {}),
    sourceFiles, sourceDigest: sha256(JSON.stringify(sourceFiles)),
    sha256: sha256(bytes), loaderSha256: sha256('// loader\n'),
    exports: WebAssembly.Module.exports(new WebAssembly.Module(bytes)),
  };
}
function withFixture(multiplayer, body) {
  const root = mkdtempSync(resolve(tmpdir(), 'th11-package-'));
  const plan = resolvePackagePlan(root, multiplayer ? ['--multiplayer'] : []);
  const put = (name, value) => {
    const path = resolve(root, name); mkdirSync(dirname(path), {recursive: true}); writeFileSync(path, value);
  };
  const binary = wasm(['th11_initialize', ...(multiplayer ? MULTIPLAYER_EXPORTS : [])]);
  const build = buildFor(binary, multiplayer);
  const buildDir = 'th11_web/artifacts/' + plan.profile + '/';
  put('source.cpp', '// source\n');
  put(buildDir + 'build.json', JSON.stringify(build));
  put(buildDir + 'th11-sdl.wasm', binary);
  put(buildDir + 'th11-sdl.mjs', '// loader\n');
  put('th11_web/sdl-runtime/managed.html', '<html><head></head><body></body></html>');
  // Exercise the real variant-dependent imports, not a build-info-only stub.
  put('th11_web/sdl-runtime/shell.mjs',
    readFileSync(new URL('../../th11_web/sdl-runtime/shell.mjs', import.meta.url), 'utf8'));
  for (const name of ['startup-branding.mjs', 'managed.css', 'keyboard.mjs', 'directory-keyboard.mjs',
    'eagler-host.mjs', 'multiplayer.mjs', 'multiplayer.css']) put('th11_web/sdl-runtime/' + name, '// shell\n');
  put('portable/browser/motion-replay.mjs', '// replay\n');
  put('portable/browser/multiplayer-replay.mjs', '// mp replay\n');
  put('third_party/eagler-common/browser/adonis-calibration.mjs', '// calibration\n');
  for (const name of ['font0.bin', 'font1.bin', 'font2.bin', 'font3.bin', 'cp932.bin', 'blend4444.bin'])
    put('private-fonts/' + name, Buffer.from([0]));
  try {
    return body({root, plan, build, binary, put,
      package: () => packageEagler({root, args: multiplayer ? ['--multiplayer'] : [], fonts: resolve(root, 'private-fonts')})});
  } finally { rmSync(root, {recursive: true, force: true}); }
}

test('ordinary and MP plans use separate binary, object-profile and package directories', () => {
  const normal = resolvePackagePlan('/workspace/title'), mp = resolvePackagePlan('/workspace/title', ['--multiplayer']);
  assert.notEqual(normal.out, mp.out);
  assert.notEqual(normal.buildRoot, mp.buildRoot);
  assert.equal(mp.variant, 'multiplayer');
  assert.throws(() => resolvePackagePlan('/workspace/title', ['--multiplayer', '--thprac']), /separate/);
  assert.throws(() => resolvePackagePlan('/workspace/title', ['--multiplayer-fixtures']), /Diagnostic/);
});

test('MP attestation rejects rollback, prediction, wrong common pin and ordinary substitution', () => {
  const plan = resolvePackagePlan('/workspace/title', ['--multiplayer']);
  const build = buildFor(wasm(MULTIPLAYER_EXPORTS), true);
  validateBuildManifest(build, plan);
  for (const netplay of [
    {...build.netplay, prediction: true}, {...build.netplay, rollback: true},
    {...build.netplay, measuredStartup: false},
  ]) assert.throws(() => validateBuildManifest({...build, netplay}, plan), /pure delay/);
  assert.throws(() => validateBuildManifest({...build, commonRevision: '0'.repeat(40)}, plan), /pinned/);
  assert.throws(() => validateBuildManifest({...build, variant: 'normal'}, plan), /variant/);
  assert.throws(() => validateBuildManifest({...build, sourceDigest: '0'.repeat(64)}, plan), /inventory/);
});

test('actual WASM prevents export-list forgery, MP leakage and diagnostic shipping', () => {
  const normal = resolvePackagePlan('/workspace/title'), mp = resolvePackagePlan('/workspace/title', ['--multiplayer']);
  const mpBytes = wasm(MULTIPLAYER_EXPORTS), mpBuild = buildFor(mpBytes, true);
  validateNativeCapabilities(new WebAssembly.Module(mpBytes), mpBuild, mp);
  assert.throws(() => validateNativeCapabilities(new WebAssembly.Module(mpBytes),
    {...mpBuild, exports: []}, mp), /actual WASM/);
  assert.throws(() => validateNativeCapabilities(new WebAssembly.Module(mpBytes),
    buildFor(mpBytes), normal), /multiplayer/);
  const networkBytes = wasm(['th11_initialize'], 'emscripten_websocket_new');
  assert.throws(() => validateNativeCapabilities(new WebAssembly.Module(networkBytes),
    buildFor(networkBytes), normal), /transport/);
  const probeBytes = wasm(['th11_initialize', 'th11_probe_mutate']);
  assert.throws(() => validateNativeCapabilities(new WebAssembly.Module(probeBytes),
    buildFor(probeBytes), normal), /diagnostic/);
  const incomplete = wasm(MULTIPLAYER_EXPORTS.slice(1));
  assert.throws(() => validateNativeCapabilities(new WebAssembly.Module(incomplete),
    buildFor(incomplete, true), mp), /missing th11_mp_configure/);
});

test('package identity keeps base resources th11 and identifies the MP product explicitly', () => {
  const build = buildFor(wasm(MULTIPLAYER_EXPORTS), true);
  const manifest = createRuntimeManifest(build, resolvePackagePlan('/workspace/title', ['--multiplayer']));
  assert.equal(manifest.game, 'th11');
  assert.equal(manifest.product, 'th11mp');
  assert.equal(manifest.variant, 'multiplayer');
  assert.equal(manifest.features.thprac, false);
});

test('closed runtime inventories include the complete MP browser module closure only for MP', () => {
  for (const multiplayer of [false, true]) withFixture(multiplayer, fixture => {
    fixture.package();
    const manifest = JSON.parse(readFileSync(resolve(fixture.plan.out, 'manifest.json')));
    const inventory = JSON.parse(readFileSync(resolve(fixture.plan.out, 'runtime-files.json')));
    const shell = readFileSync(resolve(fixture.plan.out, 'shell.mjs'), 'utf8');
    assert.equal(manifest.game, 'th11');
    assert.match(shell, new RegExp('"multiplayer":' + multiplayer));
    for (const name of ['multiplayer.mjs', 'multiplayer.css', 'multiplayer-replay.mjs', 'adonis-calibration.mjs']) {
      assert.equal(Object.hasOwn(inventory.files, name), multiplayer);
      assert.equal(existsSync(resolve(fixture.plan.out, name)), multiplayer);
    }
    const multiplayerImport = /['"]\.\/multiplayer\.mjs['"]/;
    if (multiplayer) assert.match(shell, multiplayerImport);
    else assert.doesNotMatch(shell, multiplayerImport,
      'ordinary shell must not reference an undeclared MP module, even in a false branch');
    for (const [name, expected] of Object.entries(inventory.files)) {
      const bytes = readFileSync(resolve(fixture.plan.out, name));
      assert.equal(bytes.length, expected.bytes);
      assert.equal(sha256(bytes), expected.sha256);
    }
  });
});

test('bad hashes, stale sources, calibration and shell mismatches leave existing output untouched', () => {
  for (const kind of ['wasm', 'source', 'calibration', 'factory']) withFixture(true, fixture => {
    fixture.put('build-eagler-multiplayer/manifest.json', 'previous generation');
    if (kind === 'wasm') fixture.put('th11_web/artifacts/multiplayer/th11-sdl.wasm', wasm(['wrong']));
    if (kind === 'source') fixture.put('source.cpp', '// changed\n');
    if (kind === 'calibration') fixture.put('third_party/eagler-common/browser/adonis-calibration.mjs', '// changed\n');
    if (kind === 'factory') fixture.put('th11_web/sdl-runtime/shell.mjs',
      readFileSync(resolve(fixture.root, 'th11_web/sdl-runtime/shell.mjs'), 'utf8').replace('/*TH11_MULTIPLAYER_FACTORY*/', ''));
    assert.throws(() => fixture.package(), /identity mismatch|modified source|calibration module|factory marker/);
    assert.equal(readFileSync(resolve(fixture.plan.out, 'manifest.json'), 'utf8'), 'previous generation');
    assert.deepEqual(readdirSync(fixture.plan.out), ['manifest.json']);
  });
});

test('unexpected old output files are rejected without deleting user data', () => withFixture(true, fixture => {
  fixture.put('build-eagler-multiplayer/unrelated.txt', 'keep');
  assert.throws(() => fixture.package(), /Unexpected file/);
  assert.equal(readFileSync(resolve(fixture.plan.out, 'unrelated.txt'), 'utf8'), 'keep');
  assert.equal(existsSync(resolve(fixture.plan.out, 'manifest.json')), false);
}));
