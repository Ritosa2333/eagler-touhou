// Produce the closed Runtime directory frozen by eagler-touhou.
// Validate the selected variant and the actual binary before writing any output.
import {readFileSync, writeFileSync, mkdirSync, existsSync, readdirSync} from 'node:fs';
import {resolve, dirname, relative, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const GAME = 'th11';
const SHA256 = /^[a-f0-9]{64}$/i;
const COMMON_REVISION = 'd81eda48917625ff1d3f7d156bbdb2fdd816fde7';
export const MULTIPLAYER_EXPORTS = Object.freeze([
  'configure', 'connect', 'spectator_connect', 'start', 'pump', 'status',
  'calibration_status', 'error', 'stop', 'replay_validate', 'replay_load',
  'replay_size', 'replay_data', 'replay_save', 'replay_seek', 'always_hitbox', 'game_status',
].map(name => 'th11_mp_' + name));

export const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

export function resolvePackagePlan(root, args = []) {
  for (const diagnostic of ['--multiplayer-fixtures', '--presentation-lab', '--diagnostic'])
    if (args.includes(diagnostic)) throw Error('Diagnostic binaries cannot be packaged as a TH11 Runtime.');
  const multiplayer = args.includes('--multiplayer');
  if (multiplayer && (args.includes('--thprac') || process.env.TH_ENABLE_THPRAC === '1'))
    throw Error('TH11 multiplayer and THPrac are separate runtime variants.');
  return {
    game: GAME, multiplayer, profile: multiplayer ? 'multiplayer' : 'sdl3',
    variant: multiplayer ? 'multiplayer' : 'normal',
    out: resolve(root, multiplayer ? 'build-eagler-multiplayer' : 'build-eagler'),
    buildRoot: resolve(root, 'th11_web/artifacts', multiplayer ? 'multiplayer' : 'sdl3'),
  };
}

export function validateBuildManifest(build, plan) {
  if (build?.game !== GAME || build.profile !== plan.profile || build.variant !== plan.variant ||
      build.diagnostic !== false)
    throw Error('Build profile or variant does not match requested TH11 package.');
  if (!Number.isFinite(Date.parse(build.builtAt)))
    throw Error('Rebuild Runtime with a valid build timestamp.');
  if (!SHA256.test(build.sha256 || '') || !SHA256.test(build.loaderSha256 || ''))
    throw Error('Build manifest is missing a valid WASM or loader SHA-256.');
  if (!build.sourceFiles || typeof build.sourceFiles !== 'object' ||
      Array.isArray(build.sourceFiles) || Object.keys(build.sourceFiles).length === 0 ||
      !SHA256.test(build.sourceDigest || '') ||
      sha256(JSON.stringify(build.sourceFiles)) !== build.sourceDigest)
    throw Error('Build manifest is missing a valid source identity inventory.');
  if (plan.multiplayer) {
    if (build.features?.multiplayer !== true || build.features?.thprac !== false ||
        build.netplay?.prediction !== false || build.netplay?.rollback !== false ||
        build.netplay?.measuredStartup !== true || build.commonRevision !== COMMON_REVISION)
      throw Error('TH11 MP requires the pinned shared protocol, measured startup and pure delay lockstep.');
  } else if (build.features?.multiplayer === true || build.commonRevision || build.netplay) {
    throw Error('Ordinary TH11 cannot package a multiplayer build.');
  }
}

export function validateNativeCapabilities(module, build, plan) {
  const exported = new Set(WebAssembly.Module.exports(module).map(entry => entry.name));
  const declared = new Set((build.exports || []).map(entry => entry.name));
  if (exported.size !== declared.size || [...exported].some(name => !declared.has(name)))
    throw Error('Build export attestation does not match the actual WASM.');
  for (const name of exported) {
    if (/^(presentation_lab_|audit_|th11_probe_|mp_fixture_|native_mp_test_)/.test(name))
      throw Error('Production binary contains a diagnostic export: ' + name);
  }
  const practice = exported.has('th11_practice_configure');
  if (practice !== (build.features?.thprac === true) ||
      (practice && !exported.has('sdl_thprac_mouse')))
    throw Error('THPrac declaration does not match the native capability.');
  if (plan.multiplayer) {
    for (const name of MULTIPLAYER_EXPORTS)
      if (!exported.has(name)) throw Error('Multiplayer binary is missing ' + name);
    if (practice) throw Error('TH11 MP cannot contain THPrac.');
  } else {
    if ([...exported].some(name => name.startsWith('th11_mp_')) ||
        WebAssembly.Module.imports(module).some(entry => /emscripten_websocket/.test(entry.name)))
      throw Error('Ordinary TH11 contains multiplayer entry points or transport imports.');
  }
}

export function createRuntimeManifest(build, plan) {
  return {
    game: GAME, protocol: 'eagler-touhou/1', adapter: 'sdl3-eagler',
    profile: plan.multiplayer ? 'multiplayer' : 'production',
    ...(plan.multiplayer ? {product: 'th11mp', variant: 'multiplayer'} : {}),
    builtAt: build.builtAt, version: build.version,
    features: {
      thprac: build.features?.thprac === true,
      languages: build.features?.languages === true,
      focusHitbox: build.features?.focusHitbox === true,
      ...(plan.multiplayer ? {multiplayer: true} : {}),
    },
    music: ['ogg-stream', 'ogg-full', 'none'], touchReplay: false,
    execution: {kind: build.kind, sha256: build.sha256,
      loaderSha256: build.loaderSha256, architecture: build.architecture},
  };
}

export function packageEagler({
  root = resolve(import.meta.dirname, '..'), args = process.argv.slice(2),
  fonts = process.env.EAGLER_FONT_ROOT,
  commonRoot = process.env.EAGLER_COMMON_ROOT || resolve(root, 'third_party/eagler-common'),
} = {}) {
  const plan = resolvePackagePlan(root, args), {multiplayer, buildRoot, out} = plan;
  if (args.includes('--print-plan')) { console.log(JSON.stringify(plan, null, 2)); return plan; }
  if (!fonts) throw Error('Set EAGLER_FONT_ROOT to the private SDL-native font resource directory.');
  const build = JSON.parse(readFileSync(resolve(buildRoot, 'build.json'), 'utf8'));
  validateBuildManifest(build, plan);
  for (const [name, expected] of Object.entries(build.sourceFiles))
    if (!SHA256.test(expected || '') || sha256(readFileSync(resolve(root, name))) !== expected)
      throw Error('Rebuild modified source: ' + name);

  const packaged = new Map();
  const add = (name, bytes) => packaged.set(name, Buffer.isBuffer(bytes) ? bytes : Buffer.from(bytes));
  const copy = (from, name) => add(name, readFileSync(from));
  for (const ext of ['mjs', 'wasm']) {
    const name = GAME + '-sdl.' + ext, bytes = readFileSync(resolve(buildRoot, name));
    if (sha256(bytes) !== (ext === 'wasm' ? build.sha256 : build.loaderSha256))
      throw Error('Build identity mismatch: ' + name);
    add(name, bytes);
  }
  validateNativeCapabilities(new WebAssembly.Module(packaged.get(GAME + '-sdl.wasm')), build, plan);

  const shellRoot = resolve(root, 'th11_web/sdl-runtime');
  const runtimeNames = ['startup-branding.mjs', 'shell.mjs', 'managed.css', 'keyboard.mjs',
    'directory-keyboard.mjs', 'eagler-host.mjs', ...(multiplayer ? ['multiplayer.mjs'] : [])];
  const html = readFileSync(resolve(shellRoot, 'managed.html'), 'utf8')
    .replace('<head>', '<head><meta name="eagler-data-provider" content="retail-memory">' +
      (multiplayer ? '<meta name="eagler-product" content="th11mp">' : ''));
  add('th11.html', html);
  for (const name of runtimeNames) {
    if (name === 'shell.mjs') {
      const shell = readFileSync(resolve(shellRoot, name), 'utf8');
      const marker = /\/\*TH11_BUILD_INFO\*\/\{[^;]*\}/;
      if (!marker.test(shell)) throw Error('Runtime shell is missing the build identity marker.');
      add(name, shell.replace(marker, '/*TH11_BUILD_INFO*/' +
        JSON.stringify({version: build.version, completeGame: true, multiplayer})));
    } else copy(resolve(shellRoot, name), name);
  }
  copy(resolve(root, 'portable/browser/motion-replay.mjs'), 'motion-replay.mjs');
  if (multiplayer) {
    copy(resolve(root, 'portable/browser/multiplayer-replay.mjs'), 'multiplayer-replay.mjs');
    const calibration = resolve(commonRoot, 'browser/adonis-calibration.mjs');
    const expected = build.runtimeSourceFiles?.['adonis-calibration.mjs'];
    if (!SHA256.test(expected || '') || sha256(readFileSync(calibration)) !== expected)
      throw Error('The calibration module does not match the pinned MP build.');
    copy(calibration, 'adonis-calibration.mjs');
  }
  const fontNames = ['font0.bin', 'font1.bin', 'font2.bin', 'font3.bin', 'cp932.bin', 'blend4444.bin'];
  const resources = fontNames.map(name => {
    const bytes = readFileSync(resolve(fonts, name));
    add('fonts/' + name, bytes);
    return {path: '/fonts/' + name, url: './fonts/' + name, bytes: bytes.length};
  });
  add('resources.json', JSON.stringify({schema: 'eagler-sdl-resources/1', game: GAME, resources}, null, 2) + '\n');
  add('manifest.json', JSON.stringify(createRuntimeManifest(build, plan), null, 2) + '\n');

  // Validation above is read-only. An incomplete or mismatched build cannot
  // overwrite the previous output directory or mutate its closed inventory.
  const allowed = new Set([...packaged.keys(), 'runtime-files.json']);
  const walk = dir => existsSync(dir) ? readdirSync(dir, {withFileTypes: true}).flatMap(entry =>
    entry.isDirectory() ? walk(resolve(dir, entry.name)) : [resolve(dir, entry.name)]) : [];
  for (const path of walk(out))
    if (!allowed.has(relative(out, path).split(sep).join('/')))
      throw Error('Unexpected file in output; select a clean output directory: ' + path);
  const write = (name, bytes) => {
    const path = resolve(out, name);
    mkdirSync(dirname(path), {recursive: true});
    writeFileSync(path, bytes);
  };
  for (const [name, bytes] of packaged) write(name, bytes);
  const files = Object.fromEntries([...packaged].map(([name, bytes]) =>
    [name, {bytes: bytes.length, sha256: sha256(bytes)}]));
  write('runtime-files.json', JSON.stringify({
    schema: 'eagler-touhou/runtime-directory/1', game: GAME, files,
  }, null, 2) + '\n');
  const result = {game: GAME, out, variant: plan.variant, files: packaged.size, wasm: build.sha256};
  console.log(JSON.stringify(result, null, 2));
  return result;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) packageEagler();
