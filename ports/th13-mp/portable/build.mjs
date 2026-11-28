import {sourceObjectPath} from '../../../tools/build/source-object.mjs';
import {printingRuntimeInputs, printRuntimeInputs, headerInputs} from '../../../tools/build/runtime-inputs.mjs';
import {compilerArguments} from '../../../tools/build/compiler-arguments.mjs';
import {spawn, spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,unlinkSync,statSync} from 'node:fs';
import {checkProcess, checkWasm} from './build-harness.mjs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const builtAt = new Date().toISOString();
const workspace = resolve(fileURLToPath(new URL('../', import.meta.url)));
const game = 'th13';
const root = resolve(workspace, 'web');
const captureBuild = process.env.TH13_WEB_CAPTURE === '1';
const out = resolve(root, 'artifacts', process.env.TH13_OUTPUT || (captureBuild ? 'sdl3-capture' : 'sdl3'));
const output = resolve(out, game + '-sdl.mjs');
const wasmOutput = output.replace('.mjs', '.wasm');
const manifest = resolve(out, 'build.json');
const invalidate = paths => { for (const path of paths) if (existsSync(path)) unlinkSync(path); };
if (!printingRuntimeInputs) {
  mkdirSync(out, {recursive: true});
  invalidate([manifest, output, wasmOutput]);
}

const sdk = process.env.EMSDK ?? (existsSync(resolve(workspace, 'tools/emsdk')) ? resolve(workspace, 'tools/emsdk') : resolve(workspace, '../toolchains/emsdk'));
const emcc = [resolve(sdk, 'install/emscripten/emcc.py'), resolve(sdk, 'upstream/emscripten/emcc.py')].find(existsSync);
if (!emcc && !printingRuntimeInputs) throw Error('Set EMSDK to an installed Emscripten SDK; see docs/adaptation/build.md.');
const env = {...process.env, EM_CONFIG: process.env.EM_CONFIG ?? resolve(sdk, '.emscripten'), EMSDK: sdk, EMCC_CORES: process.env.EMCC_CORES ?? '4'};
const python = process.env.TH_PYTHON ?? 'python';
const run = (args) => new Promise((done, reject) => {
  const p = spawn(python, [emcc, ...compilerArguments(args, {compiler: emcc, output: out})], {cwd: root, env, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe']});
  let log = '';
  p.stdout.on('data', x => { log += x; process.stdout.write(x); });
  p.stderr.on('data', x => { log += x; process.stderr.write(x); });
  p.on('error', reject);
  p.on('close', (code, signal) => code !== 0 || signal ? reject(Error('emcc failed ' + (signal || code) + '\n' + log)) : done());
});

const common = ['-O2', '-g0', '-Wall', '-Wextra', '-fno-strict-aliasing', '-ffp-contract=off',
  '-DTH_SDL3=1', '-DTH_NATIVE_PLATFORM=1', '--use-port=sdl3', '--use-port=sdl3_ttf',
  '-I' + resolve(root, 'cpp'),
  '-I' + resolve(workspace, '../../common/include'),
  '-I' + resolve(root, 'cpp/multiplayer')];
const flags = [...common, '-std=c++17', '-fno-exceptions', '-fno-rtti'];
flags.push('-ffile-prefix-map=' + workspace + '=' + resolve(workspace, '../th13'));
flags.push('-DTH13_MULTIPLAYER=1');
if (captureBuild) flags.push('-DTH13_DRAW_CAPTURE=1');

const sources = [
  ...readdirSync(resolve(root, 'cpp/game')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/game/' + n),
  ...readdirSync(resolve(root, 'cpp/platform')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/platform/' + n),
  ...readdirSync(resolve(root, 'cpp/multiplayer')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/multiplayer/' + n),
  ...readdirSync(resolve(workspace, '../../common/src/netplay'))
    .filter(n => n.endsWith('.cpp') && !n.startsWith('Native'))
    .map(n => '../../../common/src/netplay/' + n),
  ...readdirSync(resolve(root, 'cpp/sdl')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/sdl/' + n),
  '../portable/sdl/Renderer.cpp',
];

const allHeaders = [
  ...headerInputs(resolve(root, 'cpp'), resolve(workspace, 'portable/sdl'),
                  resolve(workspace, 'portable/input'),
                  resolve(workspace, '../../common/include/eagler/input')),
  ...(existsSync(resolve(workspace, '../../common/include/eagler/netplay'))
      ? headerInputs(resolve(workspace, '../../common/include/eagler/netplay')) : []),
  resolve(workspace, '../../common/include/eagler/text/RuntimeFontRaster.hpp'),
  resolve(workspace, '../../common/include/eagler/audio/WebSfx.hpp'),
];
await printRuntimeInputs({workspace, root, sources: sources.map(p => resolve(root, p)), headers: allHeaders, flags});
const version = spawnSync(python, [emcc, '--version'], {cwd: root, env, encoding: 'utf8'});
checkProcess(version);
const compilerIdentity = {emcc, emccSha256: createHash('sha256').update(readFileSync(emcc)).digest('hex'),
  version: version.stdout, python, config: existsSync(env.EM_CONFIG) ? createHash('sha256').update(readFileSync(env.EM_CONFIG)).digest('hex') : null};
const clangCandidates = [resolve(sdk, 'upstream/bin/clang++'), resolve(sdk, 'install/bin/clang++')];
compilerIdentity.backends = Object.fromEntries(clangCandidates.filter(existsSync).map(p => [p, createHash('sha256').update(readFileSync(p)).digest('hex')]));
const sha = x => createHash('sha256').update(x).digest('hex');
const sourceFiles = [...new Set([...sources.map(p => resolve(root, p)), ...allHeaders,
  fileURLToPath(import.meta.url), fileURLToPath(new URL('./build-harness.mjs', import.meta.url)),
  ...['source-object.mjs', 'runtime-inputs.mjs', 'compiler-arguments.mjs'].map(p =>
    fileURLToPath(new URL('../../../tools/build/' + p, import.meta.url)))])].sort();
const inventory = Object.fromEntries(sourceFiles.map(p => [p, sha(readFileSync(p))]));
const prefix = JSON.stringify([flags, compilerIdentity,
  Object.fromEntries(allHeaders.sort().map(p => [p, inventory[p]])),
  sourceFiles.filter(p => p.endsWith('.mjs')).map(p => [p, inventory[p]])]);

const objects = resolve(out, 'objects');
if (!printingRuntimeInputs) mkdirSync(objects, {recursive: true});
async function compile(source) {
  const object = sourceObjectPath(objects, workspace, source);
  const key = createHash('sha256').update(prefix).update(readFileSync(source)).digest('hex');
  if (existsSync(object) && statSync(object).size > 0 && existsSync(object + '.key') && readFileSync(object + '.key', 'utf8') === key) return object;
  invalidate([object, object + '.key']);
  await run([...flags, '-c', source, '-o', object]);
  if (!existsSync(object) || statSync(object).size === 0) throw Error('Missing fresh object: ' + object);
  writeFileSync(object + '.key', key);
  return object;
}

console.log('Build ' + game + ' C++ / SDL3 / Emscripten');
const buildJobs = Math.max(1, Math.min(16, Number.parseInt(process.env.TH_BUILD_JOBS || '4', 10) || 4));
const outputs = new Array(sources.length);
let next = 0, done = 0;
await Promise.all(Array.from({length: buildJobs}, async () => {
  while (next < sources.length) {
    const i = next++;
    outputs[i] = await compile(resolve(root, sources[i]));
    if (++done % 20 === 0) console.log(done + '/' + sources.length + ' translation units');
  }
}));

const linkFlags = [...flags, '--emit-symbol-map', '-sDEFAULT_TO_CXX=1', '--no-entry',
  '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=web,worker',
  '-sALLOW_MEMORY_GROWTH=1', '-sSTACK_SIZE=1048576', '-sINITIAL_MEMORY=67108864',
  '-sMAXIMUM_MEMORY=1073741824', '-sINVOKE_RUN=0', '-sEXIT_RUNTIME=0',
  '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',
  '-sEXPORTED_RUNTIME_METHODS=FS,UTF8ToString,HEAP32,HEAPU8,HEAPU32,allocateUTF8',
  '-sEXPORTED_FUNCTIONS=_malloc,_free',
  ...outputs, '-o', output];
await run(linkFlags);
const wasm = readFileSync(wasmOutput);
checkWasm(wasm);
if (!statSync(output).size) throw Error('Missing fresh loader');
for (const path of sourceFiles) if (sha(readFileSync(path)) !== inventory[path]) throw Error('Build input changed: ' + path);
const report = {builtAt, game, kind: 'cpp-sdl3', version: '0.1.0-sdl3-skeleton',
  sources, sourceFiles: inventory, bytes: wasm.length, sha256: sha(wasm),
  loaderSha256: sha(readFileSync(output)),
  flags, linkFlags, toolchain: {emsdkRoot: sdk, layout: 'external', ...compilerIdentity}};
writeFileSync(manifest, JSON.stringify(report, null, 2) + '\n');
console.log(JSON.stringify({game, bytes: wasm.length, sha256: report.sha256, output}, null, 2));
