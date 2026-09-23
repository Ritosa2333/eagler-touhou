import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync, existsSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname, join, resolve} from 'node:path';
import test from 'node:test';
import {
  packageEagler, resolvePackagePlan, sha256,
} from './package-eagler.mjs';
import {TH10_PRESENTATION_LAB_EXPORTS} from './presentation-lab/native-abi.mjs';

function makeFixture() {
  const root = mkdtempSync(join(tmpdir(), 'th10-package-eagler-'));
  const fonts = join(root, 'private-fonts');
  function write(path, bytes) {
    const absolute = resolve(root, path);
    mkdirSync(dirname(absolute), {recursive: true});
    writeFileSync(absolute, bytes);
    return absolute;
  }
  const source = Buffer.from('packager source identity');
  write('th10_web/cpp/game/source.cpp', source);
  write('th10_web/sdl-runtime/th10.html', '<!doctype html><head></head><body></body>');
  write('th10_web/sdl-runtime/shell.mjs', 'export const shell = true;');
  write('th10_web/sdl-runtime/eagler-host.mjs', 'export const host = true;');
  write('th10_web/sdl-runtime/save-storage.mjs', 'export const storage = true;');
  write('portable/browser/motion-replay.mjs', 'export const replay = true;');
  write(join('private-fonts', 'blend.bin'), Buffer.from([1, 2, 3]));
  write(join('private-fonts', 'codepages.bin'), Buffer.from([4, 5]));

  for (const {variant, profile, diagnostic, exports} of [
    {variant: 'normal', profile: 'sdl3', diagnostic: false, exports: []},
    {variant: 'multiplayer', profile: 'multiplayer', diagnostic: false, exports: []},
    {variant: 'normal', profile: 'presentation-lab', diagnostic: true, exports: TH10_PRESENTATION_LAB_EXPORTS},
  ]) {
    const buildRoot = resolve(root, 'th10_web/artifacts', profile);
    const wasm = Buffer.from('wasm:' + variant);
    const loader = Buffer.from('loader:' + variant);
    write(resolve(buildRoot, 'th10-sdl.wasm'), wasm);
    write(resolve(buildRoot, 'th10-sdl.mjs'), loader);
    write(resolve(buildRoot, 'browser-services.js'), '');
    write(resolve(buildRoot, 'build.json'), JSON.stringify({
      game: 'th10',
      kind: 'cpp-sdl3',
      profile,
      variant,
      diagnostic,
      version: '3.5.1-sdl3',
      features: {thprac: false, languages: true, focusHitbox: false},
      architecture: {loop: 'test'},
      sourceFiles: {'th10_web/cpp/game/source.cpp': sha256(source)},
      exports: exports.map(name => ({name})),
      sha256: sha256(wasm),
      loaderSha256: sha256(loader),
    }));
  }

  return {root, fonts, dispose: () => rmSync(root, {recursive: true, force: true})};
}

test('package plans keep normal, multiplayer and presentation-lab outputs separate', () => {
  const {root, dispose} = makeFixture();
  try {
    const normal = resolvePackagePlan(root, []);
    const multiplayer = resolvePackagePlan(root, ['--multiplayer']);
    const lab = resolvePackagePlan(root, ['--presentation-lab']);
    assert.equal(normal.buildRoot, resolve(root, 'th10_web/artifacts/sdl3'));
    assert.equal(normal.out, resolve(root, 'build-eagler'));
    assert.equal(multiplayer.buildRoot, resolve(root, 'th10_web/artifacts/multiplayer'));
    assert.equal(multiplayer.out, resolve(root, 'build-eagler-multiplayer'));
    assert.equal(multiplayer.variant, 'multiplayer');
    assert.notEqual(normal.out, multiplayer.out);
    assert.equal(lab.buildRoot, resolve(root, 'th10_web/artifacts/presentation-lab'));
    assert.equal(lab.out, resolve(root, 'artifacts/presentation-lab/runtime'));
    assert.throws(() => resolvePackagePlan(root, ['--multiplayer', '--presentation-lab']), /separate build variants/);
  } finally {
    dispose();
  }
});

test('normal and multiplayer packages emit isolated manifests with verified identities', () => {
  const {root, fonts, dispose} = makeFixture();
  try {
    const normal = packageEagler({root, fonts, args: []});
    const multiplayer = packageEagler({root, fonts, args: ['--multiplayer']});
    const lab = packageEagler({root, fonts, args: ['--presentation-lab']});
    assert.equal(normal.out, resolve(root, 'build-eagler'));
    assert.equal(multiplayer.out, resolve(root, 'build-eagler-multiplayer'));
    assert.equal(lab.out, resolve(root, 'artifacts/presentation-lab/runtime'));
    assert.notEqual(normal.out, multiplayer.out);

    const normalManifest = JSON.parse(readFileSync(resolve(normal.out, 'manifest.json'), 'utf8'));
    const multiplayerManifestBytes = readFileSync(resolve(multiplayer.out, 'manifest.json'));
    const multiplayerManifest = JSON.parse(multiplayerManifestBytes.toString('utf8'));
    assert.equal(normalManifest.profile, 'production');
    assert.equal(normalManifest.features.multiplayer, undefined);
    assert.equal(normalManifest.variant, undefined);
    assert.equal(multiplayerManifest.game, 'th10');
    assert.equal(multiplayerManifest.profile, 'multiplayer');
    assert.equal(multiplayerManifest.product, 'th10mp');
    assert.equal(multiplayerManifest.variant, 'multiplayer');
    assert.equal(multiplayerManifest.features.multiplayer, true);
    assert.equal(multiplayerManifest.execution.sha256,
      JSON.parse(readFileSync(resolve(root, 'th10_web/artifacts/multiplayer/build.json'), 'utf8')).sha256);

    for (const [directory, manifestBytes] of [
      [normal.out, readFileSync(resolve(normal.out, 'manifest.json'))],
      [multiplayer.out, multiplayerManifestBytes],
    ]) {
      const runtimeFiles = JSON.parse(readFileSync(resolve(directory, 'runtime-files.json'), 'utf8'));
      assert.deepEqual(runtimeFiles.files['manifest.json'], {
        bytes: manifestBytes.length,
        sha256: sha256(manifestBytes),
      });
      for (const [name, identity] of Object.entries(runtimeFiles.files)) {
        const bytes = readFileSync(resolve(directory, name));
        assert.equal(bytes.length, identity.bytes, name);
        assert.equal(sha256(bytes), identity.sha256, name);
      }
    }
    assert(existsSync(resolve(normal.out, 'save-storage.mjs')));
    assert(existsSync(resolve(multiplayer.out, 'save-storage.mjs')));
    const labManifest = JSON.parse(readFileSync(resolve(lab.out, 'manifest.json'), 'utf8'));
    assert.equal(labManifest.profile, 'presentation-lab');
    assert.equal(labManifest.features.multiplayer, undefined);
    assert.equal(labManifest.product, undefined);
  } finally {
    dispose();
  }
});

test('packaging rejects build-variant and artifact-hash mismatches before output', () => {
  const {root, fonts, dispose} = makeFixture();
  try {
    const buildPath = resolve(root, 'th10_web/artifacts/multiplayer/build.json');
    const build = JSON.parse(readFileSync(buildPath, 'utf8'));
    writeFileSync(buildPath, JSON.stringify({...build, variant: 'normal'}));
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /profile or variant/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);

    writeFileSync(buildPath, JSON.stringify({...build, sha256: '0'.repeat(64)}));
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /Build identity mismatch/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);
  } finally {
    dispose();
  }
});
