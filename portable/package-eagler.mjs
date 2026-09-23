import {
  readFileSync, writeFileSync, mkdirSync, existsSync, readdirSync, rmSync,
} from 'node:fs';
import {resolve, dirname, relative, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {TH10_PRESENTATION_LAB_EXPORTS} from './presentation-lab/native-abi.mjs';

const SHA256 = /^[a-f0-9]{64}$/i;

export function sha256(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

export function resolvePackagePlan(root, args = []) {
  if (args.includes('--multiplayer-fixtures')) {
    throw Error('Multiplayer fixture binaries are diagnostic-only and cannot be packaged.');
  }
  const game = existsSync(resolve(root, 'th08_web/cpp/game/AnmRenderer.cpp')) ? 'th08' : 'th10';
  const presentationLab = args.includes('--presentation-lab');
  const multiplayer = args.includes('--multiplayer');
  if (presentationLab && multiplayer) {
    throw Error('Presentation Lab and multiplayer are separate build variants.');
  }
  if (multiplayer && game !== 'th10') {
    throw Error('The multiplayer Runtime package is only defined for TH10.');
  }

  const buildProfile = presentationLab ? 'presentation-lab' : multiplayer ? 'multiplayer' : 'sdl3';
  const variant = multiplayer ? 'multiplayer' : 'normal';
  const out = presentationLab
    ? resolve(root, 'artifacts/presentation-lab/runtime')
    : multiplayer ? resolve(root, 'build-eagler-multiplayer') : resolve(root, 'build-eagler');
  const buildRoot = resolve(root, game + '_web/artifacts', buildProfile);
  return {game, presentationLab, multiplayer, buildProfile, variant, out, buildRoot};
}

export function validateBuildManifest(build, {game, buildProfile, variant, presentationLab}) {
  if (build.game !== game || build.profile !== buildProfile || build.variant !== variant ||
      !!build.diagnostic !== presentationLab) {
    throw Error('Build profile or variant does not match requested package');
  }
  if (!SHA256.test(build.sha256 || '') || !SHA256.test(build.loaderSha256 || '')) {
    throw Error('Build manifest is missing a valid WASM or loader SHA-256');
  }
  if (!build.sourceFiles || typeof build.sourceFiles !== 'object' || Array.isArray(build.sourceFiles) ||
      Object.keys(build.sourceFiles).length === 0) {
    throw Error('Build manifest is missing its source identity inventory');
  }
}

export function createRuntimeManifest({game, build, presentationLab, multiplayer}) {
  const features = {
    thprac: build.features?.thprac === true,
    languages: build.features?.languages === true,
    focusHitbox: build.features?.focusHitbox === true,
    ...(multiplayer ? {multiplayer: true} : {}),
  };
  return {
    game,
    protocol: 'eagler-touhou/1',
    adapter: 'sdl3-eagler',
    profile: presentationLab ? 'presentation-lab' : multiplayer ? 'multiplayer' : 'production',
    ...(multiplayer ? {product: game + 'mp', variant: 'multiplayer'} : {}),
    version: build.version,
    features,
    music: ['ogg-stream', 'ogg-full', 'none', ...(game === 'th08' ? ['midi'] : [])],
    touchReplay: false,
    execution: {
      kind: build.kind,
      sha256: build.sha256,
      loaderSha256: build.loaderSha256,
      architecture: build.architecture,
    },
  };
}

export function packageEagler({
  root = resolve(import.meta.dirname, '..'),
  args = process.argv.slice(2),
  fonts = process.env.EAGLER_FONT_ROOT,
} = {}) {
  const plan = resolvePackagePlan(root, args);
  const {game, presentationLab, multiplayer, buildProfile, variant, out, buildRoot} = plan;
  if (!fonts) throw Error('Set EAGLER_FONT_ROOT to the private SDL-native font resource directory');

  const hash = sha256;
  const build = JSON.parse(readFileSync(resolve(buildRoot, 'build.json'), 'utf8'));
  validateBuildManifest(build, plan);
  const exported = new Set((build.exports || []).map(entry => entry.name));
  const auditExports = game === 'th10'
    ? TH10_PRESENTATION_LAB_EXPORTS
    : ['presentation_lab_freeze', 'presentation_lab_resume', 'presentation_lab_draw'];
  if (presentationLab) {
    for (const name of auditExports) {
      if (!exported.has(name)) throw Error('Diagnostic build is missing ' + name);
    }
  } else {
    for (const name of exported) {
      if (name.startsWith('presentation_lab_') || name.startsWith('audit_')) {
        throw Error('Production build contains diagnostic export ' + name);
      }
    }
  }

  for (const [name, expected] of Object.entries(build.sourceFiles)) {
    if (!SHA256.test(expected || '') || hash(readFileSync(resolve(root, name))) !== expected) {
      throw Error('Rebuild modified source: ' + name);
    }
  }

  const entry = game === 'th08' ? 'th08-modern.html' : 'th10.html';
  const fontNames = game === 'th08' ? ['msgothic.ttc', 'blend.bin', 'cp932.bin'] : ['blend.bin', 'codepages.bin'];
  const thprac = build.features?.thprac === true;
  const runtimeNames = [
    'shell.mjs', 'eagler-host.mjs', 'save-storage.mjs',
    ...(game === 'th10' ? ['practice-loader.mjs'] : []),
    ...(thprac ? ['practice.mjs', 'practice-config.mjs', 'practice-sections.mjs'] : []),
  ];
  const names = [
    entry, 'manifest.json', ...runtimeNames, 'motion-replay.mjs', ...(game==='th10'?['replay-file-policy.mjs']:[]),
    game + '-sdl.mjs', game + '-sdl.wasm', 'resources.json',
    ...fontNames.map(name => 'fonts/' + name),
  ];
  const allowed = new Set([...names, 'runtime-files.json']);
  function walk(dir) {
    return existsSync(dir)
      ? readdirSync(dir, {withFileTypes: true}).flatMap(entry =>
          entry.isDirectory() ? walk(resolve(dir, entry.name)) : [resolve(dir, entry.name)])
      : [];
  }
  const write = (name, bytes) => {
    const path = resolve(out, name);
    mkdirSync(dirname(path), {recursive: true});
    writeFileSync(path, bytes);
  };

  const shellRoot = resolve(root, game + '_web/sdl-runtime');
  const html = readFileSync(resolve(shellRoot, game + '.html'), 'utf8')
    .replace('<head>', '<head><meta name="eagler-data-provider" content="retail-memory">');
  const shellFiles = Object.fromEntries(runtimeNames.map(name => [name, readFileSync(resolve(shellRoot, name))]));
  const replayHelper = readFileSync(resolve(root, 'portable/browser/motion-replay.mjs'));
  const replayPolicy = game==='th10'?{'replay-file-policy.mjs':readFileSync(resolve(root,'portable/browser/replay-file-policy.mjs'))}:{};
  const binaries = {};
  for (const ext of ['mjs', 'wasm']) {
    const name = game + '-sdl.' + ext;
    const bytes = readFileSync(resolve(buildRoot, name));
    const expected = ext === 'wasm' ? build.sha256 : build.loaderSha256;
    if (hash(bytes) !== expected) throw Error('Build identity mismatch: ' + name);
    binaries[name] = bytes;
  }
  // Inspect the hash-verified actual module, not just its declared export list.
  // Validation happens before any output is written or old files are removed.
  const actualExports = WebAssembly.Module.exports(new WebAssembly.Module(binaries[game + '-sdl.wasm']));
  if (game === 'th10' && actualExports.some(entry => entry.name === 'practice_enable') !== thprac) {
    throw Error('THPrac feature declaration does not match the native capability');
  }
  for (const {name} of actualExports) {
    if (name.startsWith('mp_fixture_')) throw Error('Fixture mutation export cannot be packaged: ' + name);
    if (!presentationLab && (name.startsWith('presentation_lab_') || name.startsWith('audit_'))) {
      throw Error('Production binary contains diagnostic export ' + name);
    }
  }
  const resources = fontNames.map(name => {
    const bytes = readFileSync(resolve(fonts, name));
    return {name, bytes, entry: {path: '/fonts/' + name, url: './fonts/' + name, bytes: bytes.length}};
  });
  const manifest = createRuntimeManifest({game, build, presentationLab, multiplayer});
  const manifestBytes = Buffer.from(JSON.stringify(manifest, null, 2) + '\n');
  const resourcesBytes = Buffer.from(JSON.stringify({
    schema: 'eagler-sdl-resources/1', game, resources: resources.map(item => item.entry),
  }, null, 2) + '\n');
  const packagedBytes = {
    [entry]: Buffer.from(html),
    ...shellFiles,
    'motion-replay.mjs': replayHelper,
    ...replayPolicy,
    ...binaries,
    'manifest.json': manifestBytes,
    'resources.json': resourcesBytes,
    ...Object.fromEntries(resources.map(({name, bytes}) => ['fonts/' + name, bytes])),
  };

  if (game === 'th10') {
    rmSync(resolve(out, 'fonts/msgothic.ttc'), {force: true});
    rmSync(resolve(out, 'fonts/simhei.ttf'), {force: true});
  }
  for (const path of walk(out)) {
    if (!allowed.has(relative(out, path).split(sep).join('/'))) {
      throw Error('Unexpected file in output; select a clean output directory: ' + path);
    }
  }
  for (const [name, bytes] of Object.entries(packagedBytes)) write(name, bytes);
  const files = Object.fromEntries(names.map(name => {
    const bytes = readFileSync(resolve(out, name));
    return [name, {bytes: bytes.length, sha256: hash(bytes)}];
  }));
  write('runtime-files.json', Buffer.from(JSON.stringify({
    schema: 'eagler-touhou/runtime-directory/1', game, files,
  }, null, 2) + '\n'));

  const result = {game, out, variant, files: names.length, wasm: build.sha256};
  console.log(JSON.stringify(result, null, 2));
  return result;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  packageEagler();
}
