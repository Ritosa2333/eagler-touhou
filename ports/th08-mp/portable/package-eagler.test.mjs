import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync, existsSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname, join, resolve} from 'node:path';
import test from 'node:test';
import {
  packageEagler, resolvePackagePlan, sha256,
} from './package-eagler.mjs';
import {TH08_PRESENTATION_LAB_EXPORTS} from './presentation-lab/native-abi.mjs';

function encodeUleb(value) {
  const bytes = [];
  do {
    let byte = value & 0x7f;
    value >>>= 7;
    if (value) byte |= 0x80;
    bytes.push(byte);
  } while (value);
  return bytes;
}

function makeWasm(exportNames) {
  const section = (id, payload) => [id, ...encodeUleb(payload.length), ...payload];
  const functionCount = exportNames.length;
  const typeSection = section(1, [1, 0x60, 0, 0]);
  const functionSection = section(3, [functionCount, ...Array(functionCount).fill(0)]);
  const exportPayload = [functionCount];
  exportNames.forEach((name, index) => {
    const nameBytes = Buffer.from(name);
    exportPayload.push(...encodeUleb(nameBytes.length), ...nameBytes, 0, ...encodeUleb(index));
  });
  const exportSection = section(7, exportPayload);
  const codePayload = [functionCount];
  for (let index = 0; index < functionCount; index++) codePayload.push(2, 0, 0x0b);
  const codeSection = section(10, codePayload);
  return Buffer.from([
    0, 0x61, 0x73, 0x6d, 1, 0, 0, 0,
    ...typeSection, ...functionSection, ...exportSection, ...codeSection,
  ]);
}

function makeFixture() {
  const root = mkdtempSync(join(tmpdir(), 'th08-package-eagler-'));
  const fonts = join(root, 'private-fonts');
  function write(path, bytes) {
    const absolute = resolve(root, path);
    mkdirSync(dirname(absolute), {recursive: true});
    writeFileSync(absolute, bytes);
    return absolute;
  }
  const source = Buffer.from('packager source identity');
  write('web/cpp/game/AnmRenderer.cpp', 'TH08 game marker');
  write('web/cpp/game/source.cpp', source);
  write('web/sdl-runtime/th08.html', '<!doctype html><head></head><body></body>');
  write('web/sdl-runtime/shell.mjs', 'export const shell = true;');
  write('web/sdl-runtime/startup-branding.mjs', 'export const branding = true;');
  write('web/sdl-runtime/eagler-host.mjs', 'export const host = true;');
  write('web/sdl-runtime/directory-keyboard.mjs', 'export const keyboard = true;');
  write('third_party/eagler-common/browser/adonis-calibration.mjs', 'export const calibration = true;');
  write('web/sdl-runtime/multiplayer-host.mjs', 'export const multiplayer = true;');
  write('web/sdl-runtime/resume-countdown.mjs', 'export const createResumeCountdown = () => () => {};');
  write('web/sdl-runtime/save-storage.mjs', 'export const storage = true;');
  write('web/sdl-runtime/practice.mjs', 'export const practice = true;');
  write('web/sdl-runtime/practice-config.mjs', 'export const practiceConfig = true;');
  write('web/sdl-runtime/practice-sections.mjs', 'export const practiceSections = true;');
  write('portable/browser/motion-replay.mjs', 'export const replay = true;');
  write('private-fonts/blend.bin', Buffer.from([1, 2, 3]));
  write('private-fonts/cp932.bin', Buffer.from([4, 5]));

  for (const {variant, profile, diagnostic, exports} of [
    {variant: 'normal', profile: 'sdl3', diagnostic: false, exports: []},
    {variant: 'multiplayer', profile: 'multiplayer', diagnostic: false, exports: []},
    {variant: 'normal', profile: 'presentation-lab', diagnostic: true, exports: TH08_PRESENTATION_LAB_EXPORTS},
  ]) {
    const buildRoot = resolve(root, 'web/artifacts', profile);
    const wasm = makeWasm(exports);
    const loader = Buffer.from('loader:' + profile);
    write(resolve(buildRoot, 'th08-sdl.wasm'), wasm);
    write(resolve(buildRoot, 'th08-sdl.mjs'), loader);
    write(resolve(buildRoot, 'browser-services.js'), '');
    write(resolve(buildRoot, 'build.json'), JSON.stringify({
      game: 'th08',
      builtAt: '2026-10-10T00:00:00.000Z',
      kind: 'cpp-sdl3',
      profile,
      variant,
      diagnostic,
      version: '3.4.1-sdl3',
      features: {thprac: true, languages: true, focusHitbox: false},
      architecture: {loop: 'test'},
      sourceFiles: {'web/cpp/game/source.cpp': sha256(source)},
      exports: exports.map(name => ({name, kind: 'function'})),
      sha256: sha256(wasm),
      loaderSha256: sha256(loader),
    }));
  }

  return {root, fonts, dispose: () => rmSync(root, {recursive: true, force: true})};
}

test('package plans isolate ordinary, multiplayer and presentation-lab outputs', () => {
  const {root, dispose} = makeFixture();
  try {
    const normal = resolvePackagePlan(root, []);
    const multiplayer = resolvePackagePlan(root, ['--multiplayer']);
    const lab = resolvePackagePlan(root, ['--presentation-lab']);
    assert.equal(normal.buildRoot, resolve(root, 'web/artifacts/sdl3'));
    assert.equal(normal.out, resolve(root, 'build-eagler'));
    assert.equal(multiplayer.buildRoot, resolve(root, 'web/artifacts/multiplayer'));
    assert.equal(multiplayer.out, resolve(root, 'build-eagler-multiplayer'));
    assert.equal(multiplayer.variant, 'multiplayer');
    assert.notEqual(normal.out, multiplayer.out);
    assert.equal(lab.buildRoot, resolve(root, 'web/artifacts/presentation-lab'));
    assert.equal(lab.out, resolve(root, 'artifacts/presentation-lab/runtime'));
    assert.throws(() => resolvePackagePlan(root, ['--multiplayer', '--presentation-lab']), /separate build variants/);
  } finally {
    dispose();
  }
});

test('ordinary, multiplayer and diagnostic packages retain their manifests and file hashes', () => {
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
    assert.equal(multiplayerManifest.game, 'th08');
    assert.equal(multiplayerManifest.profile, 'multiplayer');
    assert.equal(multiplayerManifest.product, 'th08mp');
    assert.equal(multiplayerManifest.variant, 'multiplayer');
    assert.equal(multiplayerManifest.features.multiplayer, true);
    assert.equal(multiplayerManifest.execution.sha256,
      JSON.parse(readFileSync(resolve(root, 'web/artifacts/multiplayer/build.json'), 'utf8')).sha256);

    for (const directory of [normal.out, multiplayer.out, lab.out]) {
      const manifestBytes = readFileSync(resolve(directory, 'manifest.json'));
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
    // shell.mjs imports this statically in every variant that ships it; dropping
    // it from runtimeNames would 404 the whole shell at runtime.
    assert(existsSync(resolve(normal.out, 'resume-countdown.mjs')));
    assert(existsSync(resolve(multiplayer.out, 'resume-countdown.mjs')));
    assert(existsSync(resolve(lab.out, 'resume-countdown.mjs')));
    for (const name of ['practice.mjs', 'practice-config.mjs', 'practice-sections.mjs']) {
      assert(existsSync(resolve(multiplayer.out, name)), name);
    }
    const labManifest = JSON.parse(readFileSync(resolve(lab.out, 'manifest.json'), 'utf8'));
    assert.equal(labManifest.profile, 'presentation-lab');
    assert.equal(labManifest.features.multiplayer, undefined);
    assert.equal(labManifest.product, undefined);
  } finally {
    dispose();
  }
});

test('resume countdown overlay keeps the native frame budget', () => {
  const root = resolve(import.meta.dirname, '..');
  const overlay = readFileSync(resolve(root, 'web/sdl-runtime/resume-countdown.mjs'), 'utf8');
  const native = readFileSync(resolve(root, 'web/cpp/game/UiMenus.cpp'), 'utf8');
  const frames = overlay.match(/TOTAL_FRAMES = (\d+), FRAMES_PER_DIGIT = (\d+)/);
  const budget = native.match(/resume_countdown_frames\s*=\s*(\d+)/);
  assert(frames, 'overlay frame constants');
  assert(budget, 'native countdown budget');
  assert.equal(frames[1], budget[1], 'TOTAL_FRAMES must equal resume_countdown_frames');
  assert.equal(Number(frames[1]) % Number(frames[2]), 0, 'digit pacing must divide the frame budget');
});

test('packaging rejects build-variant, export-inventory and binary-hash mismatches before output', () => {
  const {root, fonts, dispose} = makeFixture();
  try {
    const buildPath = resolve(root, 'web/artifacts/multiplayer/build.json');
    const sourcePath = resolve(root, 'web/cpp/game/source.cpp');
    const build = JSON.parse(readFileSync(buildPath, 'utf8'));
    writeFileSync(buildPath, JSON.stringify({...build, variant: 'normal'}));
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /profile or variant/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);

    writeFileSync(buildPath, JSON.stringify(build));
    writeFileSync(buildPath, JSON.stringify({...build, exports: [{name: 'unexpected'}]}));
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /export inventory/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);

    writeFileSync(buildPath, JSON.stringify(build));
    writeFileSync(sourcePath, 'changed source');
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /Rebuild modified source/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);
    writeFileSync(sourcePath, 'packager source identity');

    writeFileSync(buildPath, JSON.stringify({...build, sha256: '0'.repeat(64)}));
    assert.throws(() => packageEagler({root, fonts, args: ['--multiplayer']}), /Build identity mismatch/);
    assert.equal(existsSync(resolve(root, 'build-eagler-multiplayer')), false);
  } finally {
    dispose();
  }
});
