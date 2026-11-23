// th16 网页运行时构建：C++ / SDL3 / Emscripten（GLES 3.0 ↔ WebGL2；对齐 ports/th13 构建约定）。
// 用法：EMSDK=/path/to/emsdk node portable/build.mjs [--print-inputs]
import {sourceObjectPath} from '../../../tools/build/source-object.mjs';
import {printingRuntimeInputs, printRuntimeInputs, headerInputs} from '../../../tools/build/runtime-inputs.mjs';
import {compilerArguments} from '../../../tools/build/compiler-arguments.mjs';
import {spawn, spawnSync} from 'node:child_process';
import {readFileSync, writeFileSync, readdirSync, mkdirSync, existsSync, unlinkSync, statSync} from 'node:fs';
import {checkProcess, checkWasm} from './build-harness.mjs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const builtAt = new Date().toISOString();
const workspace = resolve(fileURLToPath(new URL('../', import.meta.url)));
const game = 'th16';
const root = resolve(workspace, 'web');
const out = resolve(root, 'artifacts', process.env.TH16_OUTPUT || 'sdl3');
const output = resolve(out, game + '-sdl.mjs');
const wasmOutput = output.replace('.mjs', '.wasm');
const manifest = resolve(out, 'build.json');
const invalidate = paths => { for (const path of paths) if (existsSync(path)) unlinkSync(path); };
if (!printingRuntimeInputs) {
  mkdirSync(out, {recursive: true});
  // 只留本次构建产物：失败的构建不能留下旧二进制/旧凭证。
  invalidate([manifest, output, wasmOutput]);
}

const sdk = process.env.EMSDK ?? (existsSync(resolve(workspace, 'tools/emsdk')) ? resolve(workspace, 'tools/emsdk') : resolve(workspace, '../toolchains/emsdk'));
const emcc = [resolve(sdk, 'install/emscripten/emcc.py'), resolve(sdk, 'upstream/emscripten/emcc.py')].find(existsSync);
if (!emcc && !printingRuntimeInputs) throw Error('Set EMSDK to an installed Emscripten SDK; see docs/DEVELOPMENT.md.');
const env = {...process.env, EM_CONFIG: process.env.EM_CONFIG ?? resolve(sdk, '.emscripten'), EMSDK: sdk, EMCC_CORES: process.env.EMCC_CORES ?? '4'};
const python = process.env.TH_PYTHON ?? 'python3';
const run = (args) => new Promise((done, reject) => {
  const p = spawn(python, [emcc, ...compilerArguments(args, {compiler: emcc, output: out})], {cwd: root, env, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe']});
  let log = '';
  p.stdout.on('data', x => { log += x; process.stdout.write(x); });
  p.stderr.on('data', x => { log += x; process.stderr.write(x); });
  p.on('error', reject);
  p.on('close', (code, signal) => code !== 0 || signal ? reject(Error('emcc failed ' + (signal || code) + '\n' + log)) : done());
});

// -ffp-contract=off：th16 原版为 SSE2 标量单精度，禁 FMA 融合才能逐位对齐。
const common = ['-O2', '-g0', '-fno-strict-aliasing', '-ffp-contract=off',
  '-DTH_SDL3=1', '-DTH_NATIVE_PLATFORM=1', '--use-port=sdl3',
  '-DTH16_HAS_TTF=1', '--use-port=sdl3_ttf',  // 0x459240 字形光栅（字体文件由宿主装入 MEMFS）
  '-I' + resolve(root, 'cpp'), '-I' + resolve(workspace, '../../common/include')];
const flags = [...common, '-std=c++17', '-fno-exceptions', '-fno-rtti'];

const sources = [
  ...readdirSync(resolve(root, 'cpp/game')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/game/' + n),
  ...readdirSync(resolve(root, 'cpp/platform')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/platform/' + n),
  ...readdirSync(resolve(root, 'cpp/sdl')).filter(n => n.endsWith('.cpp')).map(n => 'cpp/sdl/' + n),
  '../portable/sdl/Renderer.cpp',  // 语义 GLES 渲染后端（与 th13/th15 同源）
];

const allHeaders = [
  ...headerInputs(resolve(root, 'cpp'), resolve(workspace, 'portable/sdl'), resolve(workspace, 'portable/input')),
  resolve(workspace, '../../common/include/eagler/audio/WebSfx.hpp'),  // 共享网页音效输出（AudioDevice.cpp）
];
await printRuntimeInputs({workspace, root, sources: sources.map(p => resolve(root, p)), headers: allHeaders, flags});
const version = spawnSync(python, [emcc, '--version'], {cwd: root, env, encoding: 'utf8'});
checkProcess(version);
const compilerIdentity = {emcc, emccSha256: createHash('sha256').update(readFileSync(emcc)).digest('hex'),
  version: version.stdout, python, config: existsSync(env.EM_CONFIG) ? createHash('sha256').update(readFileSync(env.EM_CONFIG)).digest('hex') : null};
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
let next = 0;
await Promise.all(Array.from({length: buildJobs}, async () => {
  while (next < sources.length) {
    const i = next++;
    outputs[i] = await compile(resolve(root, sources[i]));
  }
}));

const linkFlags = [...flags, '--emit-symbol-map', '-sDEFAULT_TO_CXX=1', '--no-entry',
  '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=web,worker',
  '-sALLOW_MEMORY_GROWTH=1', '-sSTACK_SIZE=1048576', '-sINITIAL_MEMORY=134217728',
  '-sMAXIMUM_MEMORY=1073741824', '-sINVOKE_RUN=0', '-sEXIT_RUNTIME=0',
  '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',
  '-lidbfs.js', '-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,UTF8ToString,HEAPU8',  // th16-dev.html / shell.mjs：th16.dat 进 MEMFS、存档挂 IDBFS、错误文本解码
  '-sEXPORTED_FUNCTIONS=_malloc,_free',
  ...outputs, '-o', output];
await run(linkFlags);
const wasm = readFileSync(wasmOutput);
checkWasm(wasm);
if (!statSync(output).size) throw Error('Missing fresh loader');
for (const path of sourceFiles) if (sha(readFileSync(path)) !== inventory[path]) throw Error('Build input changed: ' + path);
const module = new WebAssembly.Module(wasm);
// features / architecture / exports 供 portable/package-eagler.mjs 生成启动器 manifest.json（eagler-touhou/1）
const report = {builtAt, game, kind: 'cpp-sdl3', version: '1.0.0-sdl3',
  features: {thprac: false, languages: false, focusHitbox: false},
  architecture: {loop: 'cpp-frameclock-60hz-catchup', audio: 'miniaudio-sdl3', renderer: 'cpp-gles-semantic-batched',
    files: 'sdl-io-idbfs', fonts: 'sdl3-ttf', input: 'cpp-sdl-gamepad-hosted-keyboard', launcher: 'eagler-touhou/1'},
  sources, sourceFiles: inventory, bytes: wasm.length, sha256: sha(wasm),
  loaderSha256: sha(readFileSync(output)),
  imports: WebAssembly.Module.imports(module), exports: WebAssembly.Module.exports(module),
  flags, linkFlags, toolchain: {emsdkRoot: sdk, layout: 'external', ...compilerIdentity}};
writeFileSync(manifest, JSON.stringify(report, null, 2) + '\n');
console.log(JSON.stringify({game, bytes: wasm.length, sha256: report.sha256, output}, null, 2));
