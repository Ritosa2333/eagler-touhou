// TH11 ordinary and multiplayer builds have separate objects, binaries and attestations.
import {spawn, execFileSync} from 'node:child_process';
import {readFileSync, writeFileSync, readdirSync, mkdirSync, existsSync} from 'node:fs';
import {resolve, relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {responseFileContents} from './response-file.mjs';

const builtAt = new Date().toISOString();
const workspace = resolve(fileURLToPath(new URL('../', import.meta.url)));
const root = resolve(workspace, 'th11_web');
const multiplayer = process.argv.includes('--multiplayer');
const printPlan = process.argv.includes('--print-plan');
const thprac = process.env.TH_ENABLE_THPRAC === '1' || process.argv.includes('--thprac');
if (multiplayer && thprac) throw Error('TH11 multiplayer and THPrac are separate runtime variants.');
const profile = multiplayer ? 'multiplayer' : 'sdl3';
const variant = multiplayer ? 'multiplayer' : 'normal';
const out = resolve(root, 'artifacts', profile);
const commonRevision = 'e02347fac98c9e599d30a6ffd7ef30756555b948';
const netplayRoot = resolve(process.env.EAGLER_COMMON_ROOT || resolve(workspace, 'third_party/eagler-common'));
const sdk = process.env.EMSDK || [
  resolve(workspace, 'tools/emsdk'),
  resolve(workspace, '../toolchains/emsdk'),
  resolve(workspace, '../../toolchains/emsdk'),
].find(path => existsSync(resolve(path, '.emscripten')));
const emcc = sdk && ['install/emscripten/emcc.py', 'upstream/emscripten/emcc.py']
  .map(name => resolve(sdk, name)).find(existsSync);
if (!emcc && !printPlan) throw Error('TH11 build needs Emscripten; set EMSDK to the shared pinned SDK.');
const imgui = resolve(root, 'cpp/third_party/imgui');
const flags = [
  '-O2', '-g0', '-std=c++17', '-ffp-contract=off', '-fno-strict-aliasing',
  '-fno-exceptions', '-fno-rtti', '-DTH_NATIVE_PLATFORM=1', '-DTH_ENABLE_THCRAP=1',
  '-DTH11_DEVELOPMENT_HARNESS=0', '-DIMGUI_DISABLE_WIN32_FUNCTIONS',
  '-I' + imgui, '-I' + resolve(root, 'cpp'),
  ...(thprac ? ['-DTH_ENABLE_THPRAC=1'] : []),
  '--use-port=sdl3', '--use-port=sdl3_ttf',
];
if (multiplayer) flags.push('-DTH11_MULTIPLAYER=1', '-I' + resolve(root, 'cpp/multiplayer'),
  '-I' + resolve(netplayRoot, 'include'));
const files = dir => existsSync(dir) ? readdirSync(dir, {withFileTypes: true}).flatMap(entry =>
  entry.isDirectory() ? files(resolve(dir, entry.name)) :
    entry.name.endsWith('.cpp') ? [resolve(dir, entry.name)] : []) : [];
const source = [...files(resolve(root, 'cpp/game')), ...files(resolve(root, 'cpp/sdl'))];
const renderer = resolve(workspace, 'portable/sdl/Renderer.cpp');
source.push(renderer);
source.push(...['imgui.cpp', 'imgui_draw.cpp', 'imgui_freetype.cpp', 'imgui_tables.cpp', 'imgui_widgets.cpp']
  .map(name => resolve(imgui, name)));
if (multiplayer) {
  source.push(...files(resolve(root, 'cpp/multiplayer')));
  source.push(...['NetplayProtocol', 'NetplayCore', 'NetplaySession', 'WebSocketTransport',
    'BrowserPeerTransport', 'SessionChannel', 'InputReplay', 'RollbackJournal']
    .map(name => resolve(netplayRoot, 'src/netplay', name + '.cpp')));
}
const exported = [
  '_malloc', '_free', '_th11_validate_file', '_th11_save_scores', '_th11_save_replay',
  '_th11_initialize', '_th11_music_enabled', '_th11_audio_statistics', '_th11_return_title',
  '_th11_restart', '_th11_error', '_th11_frame', '_th11_phase', '_th11_pause', '_th11_resume',
  '_th11_loop_start', '_th11_loop_stop', '_th11_loop_pause', '_th11_key', '_th11_keys_clear',
  '_th11_always_hitbox', '_th11_touch', '_th11_touch_cancel', '_th11_touch_options', '_th11_touch_controls', '_th11_touch_stick',
];
if (thprac) exported.push('_th11_practice_configure');
// MP entry points carry EMSCRIPTEN_KEEPALIVE in their owning translation units.
const linkFlags = multiplayer ? ['-lwebsocket'] : [];
if (printPlan) {
  console.log(JSON.stringify({game: 'th11', profile, variant, outputDirectory: out,
    packageDirectory: resolve(workspace, multiplayer ? 'build-eagler-multiplayer' : 'build-eagler'),
    flags, linkFlags, sources: source.map(path => relative(workspace, path).replaceAll('\\', '/')),
    commonRevision: multiplayer ? commonRevision : null, prediction: false, rollback: false}, null, 2));
  process.exit(0);
}
if (multiplayer) {
  const actual = execFileSync('git', ['-C', netplayRoot, 'rev-parse', 'HEAD'], {encoding: 'utf8', windowsHide: true}).trim();
  if (actual !== commonRevision) throw Error('TH11 MP requires eagler-common ' + commonRevision + ', found ' + actual);
  execFileSync('git', ['-C', netplayRoot, 'diff', '--quiet', 'HEAD', '--'], {windowsHide: true});
}
mkdirSync(out, {recursive: true});
const env = {...process.env, EM_CONFIG: process.env.EM_CONFIG || resolve(sdk, '.emscripten'),
  EMSDK: sdk, EMCC_CORES: process.env.EMCC_CORES || '4'};
const python = process.env.TH_PYTHON || 'python';
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const observed = new Map();
function readObserved(path) {
  const bytes = readFileSync(path), digest = sha(bytes);
  if (observed.has(path) && observed.get(path) !== digest) throw Error('Source changed during build: ' + path);
  observed.set(path, digest);
  return bytes;
}
const headerFiles = dir => readdirSync(dir, {withFileTypes: true}).flatMap(entry =>
  entry.isDirectory() ? ((!multiplayer && (entry.name === 'multiplayer' || entry.name === 'netplay')) ? [] :
    headerFiles(resolve(dir, entry.name))) :
    /\.(h|hpp|inc)$/.test(entry.name) ? [resolve(dir, entry.name)] : []);
const headers = [...headerFiles(resolve(root, 'cpp')), ...headerFiles(resolve(workspace, 'portable/sdl')),
  ...headerFiles(resolve(workspace, 'portable/input')),
  ...(multiplayer ? headerFiles(resolve(netplayRoot, 'include')) : [])].sort();
const headerHash = sha(headers.map(path => path + sha(readObserved(path))).join('\n'));
readObserved(fileURLToPath(import.meta.url));
readObserved(resolve(workspace, 'portable/response-file.mjs'));
const runtimeSourceFiles = multiplayer ? {
  'adonis-calibration.mjs': sha(readObserved(resolve(netplayRoot, 'browser/adonis-calibration.mjs'))),
} : {};
async function run(args) {
  let command = args;
  if (process.platform === 'win32' && args.reduce((length, value) => length + value.length + 3, emcc.length) > 20000) {
    const contents = responseFileContents(args), path = resolve(out, 'arguments-' + sha(contents).slice(0, 20) + '.rsp.utf-8');
    writeFileSync(path, contents);
    command = ['@' + path];
  }
  await new Promise((accept, reject) => {
    const processHandle = spawn(python, [emcc, ...command], {cwd: root, env, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe']});
    let log = '';
    processHandle.stdout.on('data', bytes => { log += bytes; process.stdout.write(bytes); });
    processHandle.stderr.on('data', bytes => { log += bytes; process.stderr.write(bytes); });
    processHandle.on('error', reject);
    processHandle.on('exit', code => code ? reject(Error('emcc failed ' + code + '\n' + log)) : accept());
  });
}
const objects = resolve(out, 'objects');
mkdirSync(objects, {recursive: true});
const settings = JSON.stringify([flags, headerHash]);
async function compile(file) {
  const name = relative(workspace, file).replaceAll('\\', '_').replaceAll('/', '_').replaceAll(':', '_').replaceAll('.', '_');
  const object = resolve(objects, name + '.o'), key = sha(settings + sha(readObserved(file)));
  if (existsSync(object) && existsSync(object + '.key') && readFileSync(object + '.key', 'utf8') === key) return object;
  await run([...flags, '-c', file, '-o', object]);
  writeFileSync(object + '.key', key);
  return object;
}
console.log('Build th11 C++ / SDL3 / Emscripten (' + variant + ')');
const objectsBuilt = new Array(source.length);
const rendererIndex = source.indexOf(renderer);
objectsBuilt[rendererIndex] = await compile(renderer); // Populate the shared SDL port cache before parallel compilation.
let cursor = 0, done = 0;
const jobs = Math.max(1, Math.min(16, Number.parseInt(process.env.TH_BUILD_JOBS || '4', 10) || 4));
await Promise.all(Array.from({length: jobs}, async () => {
  while (cursor < source.length) {
    const index = cursor++;
    if (index !== rendererIndex) objectsBuilt[index] = await compile(source[index]);
    if (++done % 40 === 0) console.log(done + '/' + source.length + ' translation units');
  }
}));
const loader = resolve(out, 'th11-sdl.mjs');
await run([...flags, ...linkFlags, '--no-entry', '-sDEFAULT_TO_CXX=1', '-sMODULARIZE=1', '-sEXPORT_ES6=1',
  '-sENVIRONMENT=web,worker', '-sALLOW_MEMORY_GROWTH=1', '-sSTACK_SIZE=1048576', '-sINITIAL_MEMORY=134217728',
  '-sMAXIMUM_MEMORY=1073741824', '-sFILESYSTEM=1', '-lidbfs.js', '-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8',
  '-sINVOKE_RUN=0', '-sEXIT_RUNTIME=0', '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2',
  '-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0', '-sEXPORTED_FUNCTIONS=' + exported.join(','),
  ...objectsBuilt, '-o', loader]);
const wasm = readFileSync(loader.replace('.mjs', '.wasm')), module = new WebAssembly.Module(wasm);
for (const [path, expected] of observed) if (sha(readFileSync(path)) !== expected)
  throw Error('Source changed during compilation; refuse mixed build: ' + path);
const inventory = Object.fromEntries([...observed].sort(([a], [b]) => a.localeCompare(b))
  .map(([path, hash]) => [relative(workspace, path).replaceAll('\\', '/'), hash]));
const sdkMetadata = resolve(sdk, 'touhou-sdk.json');
const toolchain = existsSync(sdkMetadata) ? JSON.parse(readFileSync(sdkMetadata)) :
  {emsdkRoot: relative(workspace, sdk).replaceAll('\\', '/'), layout: 'external'};
const report = {
  builtAt, game: 'th11', kind: 'cpp-sdl3', profile, variant, diagnostic: false,
  version: multiplayer ? '1.0.0-mp1-sdl3' : '1.0.0-sdl3',
  features: {thprac, languages: true, focusHitbox: false, ...(multiplayer ? {multiplayer: true} : {})},
  ...(multiplayer ? {commonRevision, runtimeSourceFiles,
    netplay: {prediction: false, rollback: false, measuredStartup: true}} : {}),
  architecture: {loop: multiplayer ? 'cpp-fixed-60hz-confirmed-lockstep' : 'cpp-fixed-60hz-bounded-catchup',
    audio: 'cpp-miniaudio-vorbis-sdl3-stream', renderer: 'cpp-gles-semantic-batched',
    graphicsInterface: 'semantic-state-texture-matrix', vertexUpload: 'web-bufferData-direct-game-batches-cached-vao',
    files: 'sdl-io-idbfs', fonts: 'cpp-original-baked-glyphs-argb4444', input: 'cpp-sdl', launcher: 'eagler-touhou/1'},
  sdlVersion: '3.4.2', sources: source.map(path => relative(workspace, path).replaceAll('\\', '/')),
  sourceFiles: inventory, sourceDigest: sha(JSON.stringify(inventory)),
  sharedSources: ['Renderer.cpp', 'Renderer.hpp', 'Shaders.hpp', 'GraphicsState.hpp', 'AssetPixelFormat.hpp',
    'RenderCommands.hpp', 'LegacyGraphics.hpp', 'MotionTrack.hpp', 'TouchController.hpp'],
  bytes: wasm.length, sha256: sha(wasm), loaderSha256: sha(readFileSync(loader)),
  imports: WebAssembly.Module.imports(module), exports: WebAssembly.Module.exports(module), toolchain,
};
writeFileSync(resolve(out, 'build.json'), JSON.stringify(report, null, 2) + '\n');
console.log(JSON.stringify({game: 'th11', variant, bytes: wasm.length, sha256: report.sha256, output: loader}, null, 2));
