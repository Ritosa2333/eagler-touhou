// th16 WASI harness 构建：tests/harness/harness.cpp + web/cpp/game/*.cpp → web/artifacts/wasi/th16-harness.wasm。
// 逐 TU 对象缓存 + 链接（与 portable/build.mjs 同一缓存框架）；任一失败删除 [wasm, manifest]。
// 用法：WASI_SDK_PATH=/path/to/wasi-sdk node portable/build-harness.mjs
import {mkdirSync, readdirSync, readFileSync, writeFileSync, existsSync, unlinkSync, statSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {spawn, spawnSync} from 'node:child_process';
import {sourceObjectPath} from '../../../tools/build/source-object.mjs';
import {printRuntimeInputs, headerInputs} from '../../../tools/build/runtime-inputs.mjs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

export function checkProcess(run) {
  if (run.error || run.signal || run.status !== 0) throw Error('compiler failed: ' + (run.error?.message || run.signal || run.status));
}
export function checkWasm(bytes) {
  if (bytes.length < 8 || !bytes.subarray(0, 8).equals(Buffer.from([0, 97, 115, 109, 1, 0, 0, 0]))) throw Error('Invalid/missing fresh wasm output');
}

export async function buildHarness() {
  const workspace = resolve(fileURLToPath(new URL('../', import.meta.url)));
  const root = resolve(workspace, 'web');
  const out = resolve(process.env.TH16_HARNESS_OUT ?? resolve(root, 'artifacts', 'wasi'));
  mkdirSync(out, {recursive: true});
  const output = resolve(out, 'th16-harness.wasm');
  const manifest = resolve(out, 'build.json');
  for (const path of [output, manifest]) if (existsSync(path)) unlinkSync(path);
  const wasi = process.env.WASI_SDK_PATH ?? (existsSync(resolve(workspace, '../toolchains/wasi-sdk')) ? resolve(workspace, '../toolchains/wasi-sdk') : null);
  if (!wasi) throw Error('Set WASI_SDK_PATH to a wasi-sdk installation.');
  const clang = resolve(wasi, 'bin', 'clang++');

  const gameSources = readdirSync(resolve(root, 'cpp/game')).filter(n => n.endsWith('.cpp')).map(n => resolve(root, 'cpp/game/' + n));
  const sources = [...gameSources, resolve(workspace, 'tests/harness/harness.cpp')];
  const compileFlags = ['--target=wasm32-wasip1', '-O2', '-std=c++17', '-fno-exceptions', '-fno-rtti',
    '-ffp-contract=off', '-DTH16_WASI_HARNESS=1', '-I' + resolve(root, 'cpp')];
  const linkFlags = ['--target=wasm32-wasip1', '-O2', '-Wl,-z,stack-size=1048576'];
  const headers = headerInputs(resolve(root, 'cpp'), resolve(workspace, 'tests/harness'));
  await printRuntimeInputs({workspace, root, sources, headers, flags: [...compileFlags, ...linkFlags]});

  const version = spawnSync(clang, ['--version'], {encoding: 'utf8'});
  checkProcess(version);
  const sha = data => createHash('sha256').update(data).digest('hex');
  const toolchain = {path: clang, version: version.stdout.trim(), sha256: sha(readFileSync(clang))};
  const selfFiles = [fileURLToPath(import.meta.url)];
  const sourceFiles = [...new Set([...sources, ...headers, ...selfFiles])].sort();
  const inventory = Object.fromEntries(sourceFiles.map(path => [path, sha(readFileSync(path))]));
  const prefix = JSON.stringify([compileFlags, toolchain,
    Object.fromEntries(headers.map(p => [p, inventory[p]])), Object.fromEntries(selfFiles.map(p => [p, inventory[p]]))]);
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
    if (existsSync(object) && statSync(object).size > 0 && existsSync(object + '.key') && readFileSync(object + '.key', 'utf8') === key) return {object, reused: true};
    for (const path of [object, object + '.key']) if (existsSync(path)) unlinkSync(path);
    await runOne([...compileFlags, '-c', source, '-o', object]);
    if (!existsSync(object) || statSync(object).size === 0) throw Error('Missing fresh object: ' + object);
    writeFileSync(object + '.key', key);
    return {object, reused: false};
  }

  console.log('build th16 wasi harness');
  const results = [];
  for (const source of sources) results.push(await compile(source));
  const reused = results.filter(r => r.reused).length;
  try {
    await runOne([...linkFlags, ...results.map(r => r.object), '-o', output]);
    const bytes = readFileSync(output);
    checkWasm(bytes);
    for (const path of sourceFiles) if (sha(readFileSync(path)) !== inventory[path]) throw Error('Build input changed: ' + path);
    writeFileSync(manifest, JSON.stringify({schema: 'th16/harness-build/v1', builtAt: new Date().toISOString(),
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
