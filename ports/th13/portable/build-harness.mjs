import {mkdirSync, readdirSync, readFileSync, writeFileSync, existsSync, unlinkSync, statSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {spawn} from 'node:child_process';
import {sourceObjectPath} from '../../../tools/build/source-object.mjs';
import {printingRuntimeInputs, printRuntimeInputs, headerInputs} from '../../../tools/build/runtime-inputs.mjs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

export function checkProcess(run) {
  if (run.error || run.signal || run.status !== 0) throw Error('clang++ failed: ' + (run.error?.message || run.signal || run.status));
}
export function checkWasm(bytes) {
  if (bytes.length < 8 || !bytes.subarray(0, 8).equals(Buffer.from([0,97,115,109,1,0,0,0]))) throw Error('Invalid/missing fresh wasm output');
}

export async function buildHarness() {
const workspace = resolve(process.env.TH13_HARNESS_WORKSPACE ?? fileURLToPath(new URL('../', import.meta.url)));
const root = resolve(workspace, 'web');
const out = resolve(process.env.TH13_HARNESS_OUT ?? resolve(root, 'artifacts', 'wasi'));
mkdirSync(out, {recursive: true});

const output = resolve(out, 'th13-harness.wasm');
const manifest = resolve(out, 'build.json');
for (const path of [output, manifest]) if (existsSync(path)) unlinkSync(path);
const wasi = process.env.WASI_SDK_PATH;
if (!wasi) throw Error('Set WASI_SDK_PATH to a wasi-sdk installation.');
const clang = resolve(wasi, 'bin', 'clang++');

const gameSources = readdirSync(resolve(root, 'cpp/game')).filter(n => n.endsWith('.cpp')).map(n => resolve(root, 'cpp/game/' + n));
const main = resolve(workspace, 'tests/harness/harness.cpp');
const sources = [...gameSources, main];

const compileFlags = [
  '--target=wasm32-wasip1', '-O2', '-std=c++17', '-Wall', '-Wextra',
  '-fno-exceptions', '-fno-rtti',
  '-ffp-contract=off', '-DTH13_WASI_HARNESS=1', '-DTH13_DRAW_CAPTURE=1',
  '-I' + resolve(root, 'cpp'),
];
const linkFlags = [
  '--target=wasm32-wasip1', '-O2',
  '-Wl,-z,stack-size=1048576',
];
await printRuntimeInputs({workspace, root, sources,
  headers: headerInputs(resolve(root, 'cpp'), resolve(workspace, 'tests/harness')), flags: [...compileFlags, ...linkFlags]});

const version = spawnSync(clang, ['--version'], {encoding: 'utf8'});
checkProcess(version);
const toolchain = {path: clang, version: version.stdout.trim(), sha256: createHash('sha256').update(readFileSync(clang)).digest('hex')};
const sha = data => createHash('sha256').update(data).digest('hex');
const headers = headerInputs(resolve(root, 'cpp'), resolve(workspace, 'tests/harness'));
const selfFiles = [fileURLToPath(import.meta.url)];
const sourceFiles = [...new Set([...sources, ...headers, ...selfFiles])].sort();
const inventory = Object.fromEntries(sourceFiles.map(path => [path, sha(readFileSync(path))]));
const prefix = JSON.stringify([compileFlags, toolchain,
  Object.fromEntries(headers.map(p => [p, inventory[p]])),
  Object.fromEntries(selfFiles.map(p => [p, inventory[p]]))]);

const objects = resolve(out, 'objects');
mkdirSync(objects, {recursive: true});
const runOne = args => new Promise((done, reject) => {
  const p = spawn(clang, args, {stdio: ['ignore', 'pipe', 'pipe']});
  let log = '';
  p.stdout.on('data', x => { log += x; });
  p.stderr.on('data', x => { log += x; process.stderr.write(x); });
  p.on('error', reject);
  p.on('close', (code, signal) => code !== 0 || signal ? reject(Error('clang++ failed ' + (signal || code) + '\n' + log)) : done());
});
async function compile(source) {
  const object = sourceObjectPath(objects, workspace, source);
  const key = createHash('sha256').update(prefix).update(readFileSync(source)).digest('hex');
  if (existsSync(object) && statSync(object).size > 0 && existsSync(object + '.key') && readFileSync(object + '.key', 'utf8') === key)
    return {object, key, reused: true};
  for (const path of [object, object + '.key']) if (existsSync(path)) unlinkSync(path);
  await runOne([...compileFlags, '-c', source, '-o', object]);
  if (!existsSync(object) || statSync(object).size === 0) throw Error('Missing fresh object: ' + object);
  writeFileSync(object + '.key', key);
  return {object, key, reused: false};
}

console.log('build th13 wasi harness (incremental)');
const buildJobs = Math.max(1, Math.min(32, Number.parseInt(process.env.TH_BUILD_JOBS || '16', 10) || 16));
const results = new Array(sources.length);
let next = 0, done = 0, reused = 0;
await Promise.all(Array.from({length: buildJobs}, async () => {
  while (next < sources.length) {
    const i = next++;
    results[i] = await compile(sources[i]);
    if (results[i].reused) reused++;
    if (++done % 20 === 0) console.log(done + '/' + sources.length + ' translation units');
  }
}));
console.log('objects reused ' + reused + '/' + sources.length);

try {
  await runOne([...linkFlags, ...results.map(r => r.object), '-o', output]);
  const bytes = readFileSync(output);
  checkWasm(bytes);
  for (const path of sourceFiles) if (sha(readFileSync(path)) !== inventory[path]) throw Error('Build input changed: ' + path);
  writeFileSync(manifest, JSON.stringify({schema: 'th13/harness-build/v1', builtAt: new Date().toISOString(),
    output, sources: inventory, compileFlags, linkFlags, toolchain,
    cache: {objectsReused: reused, objectsTotal: sources.length},
    binary: {sha256: sha(bytes), bytes: bytes.length}}, null, 2) + '\n');
  console.log(JSON.stringify({output, manifest, reused}, null, 2));
} catch (error) {
  for (const path of [output, manifest]) if (existsSync(path)) unlinkSync(path);
  throw error;
}
}
if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) await buildHarness();
