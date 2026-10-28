import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, writeFileSync, readFileSync, readdirSync, existsSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {checkProcess, checkWasm} from './build-harness.mjs';
const wasm = Buffer.from([0,97,115,109,1,0,0,0]);
function fixture(t) {
  const dir = mkdtempSync(resolve(tmpdir(), 'th13-build-test-'));
  t.after(() => rmSync(dir, {recursive: true, force: true}));
  const port = resolve(dir, 'ports/th13');
  const put = (name, text) => {const path = resolve(dir, name); mkdirSync(resolve(path, '..'), {recursive: true}); writeFileSync(path, text); return path;};
  for (const file of ['build.mjs', 'build-harness.mjs']) put('ports/th13/portable/' + file, readFileSync(new URL(file, import.meta.url)));
  for (const file of ['source-object.mjs', 'runtime-inputs.mjs', 'compiler-arguments.mjs']) put('tools/build/' + file, readFileSync(new URL('../../../tools/build/' + file, import.meta.url)));
  for (const name of ['game', 'platform', 'sdl']) put('ports/th13/web/cpp/' + name + '/a.cpp', '// synthetic');
  put('ports/th13/web/cpp/game/a.hpp', '// header');
  put('ports/th13/portable/sdl/Renderer.cpp', '// renderer');
  put('ports/th13/portable/sdl/backend.hpp', '// backend 1');
  put('ports/th13/tests/harness/harness.cpp', '// harness');
  const run = (script, env = {}, args = []) => spawnSync(process.execPath, [resolve(port, 'portable', script), ...args], {encoding: 'utf8', env: {...process.env, ...env}});
  return {dir, port, put, run};
}
test('spawn errors/null/signals/nonzero cannot pass; wasm validation', () => {
  for (const run of [{status: null}, {status: 0, signal: 'SIGTERM'}, {status: 0, error: Error('ENOENT')}, {status: 1}]) assert.throws(() => checkProcess(run));
  checkProcess({status: 0}); checkWasm(wasm);
  assert.throws(() => checkWasm(Buffer.from('stale')));
});
test('harness removes stale proof; successful fresh output has hashed manifest', t => {
  const f = fixture(t);
  const sdk = resolve(f.dir, 'sdk');
  const compiler = f.put('sdk/bin/clang++', '#!/usr/bin/env python3\nimport sys, pathlib\nif "--version" in sys.argv: print("synthetic compiler 1")\nelse: pathlib.Path(sys.argv[sys.argv.index("-o")+1]).write_bytes(bytes([0,97,115,109,1,0,0,0]))\n');
  // chmod through the existing system utility; no external packages.
  assert.equal(spawnSync('chmod', ['+x', compiler]).status, 0);
  let run = f.run('build-harness.mjs', {WASI_SDK_PATH: sdk});
  assert.equal(run.status, 0, run.stderr);
  const output = resolve(f.port, 'web/artifacts/wasi');
  const manifest = JSON.parse(readFileSync(resolve(output, 'build.json')));
  assert.equal(manifest.binary.sha256.length, 64);
  assert.equal(manifest.toolchain.sha256.length, 64);
  assert.ok(Object.keys(manifest.sources).some(p => p.endsWith('a.hpp')));
  assert.ok(manifest.flags.includes('-ffp-contract=off'));
  run = f.run('build-harness.mjs', {WASI_SDK_PATH: ''});
  assert.notEqual(run.status, 0);
  assert.equal(existsSync(resolve(output, 'build.json')), false);
  assert.equal(existsSync(resolve(output, 'th13-harness.wasm')), false);
  writeFileSync(compiler, '#!/usr/bin/env python3\nimport sys\nif "--version" in sys.argv: print("noop")\n');
  run = f.run('build-harness.mjs', {WASI_SDK_PATH: sdk});
  assert.notEqual(run.status, 0);
  assert.equal(existsSync(resolve(output, 'build.json')), false);
  assert.equal(existsSync(resolve(output, 'th13-harness.wasm')), false);
});
test('web cache invalidates on portable headers and compiler identity', t => {
  const f = fixture(t);
  const compilerText = version => `import sys, pathlib\nif '--version' in sys.argv: print('synthetic ${version}')\nelse:\n p=pathlib.Path(sys.argv[sys.argv.index('-o')+1]); p.write_bytes(bytes([0,97,115,109,1,0,0,0]))\n if p.suffix == '.mjs': p.with_suffix('.wasm').write_bytes(bytes([0,97,115,109,1,0,0,0]))\n`;
  f.put('sdk/upstream/emscripten/emcc.py', compilerText(1));
  const env = {EMSDK: resolve(f.dir, 'sdk'), TH_PYTHON: 'python3'};
  const build = () => { const run = f.run('build.mjs', env); assert.equal(run.status, 0, run.stderr); return JSON.parse(readFileSync(resolve(f.port, 'web/artifacts/sdl3/build.json'))); };
  const a = build();
  const objectDir = resolve(f.port, 'web/artifacts/sdl3/objects');
  // Cache keys are checked rather than relying on filesystem time resolution.
  const keys = () => readdirSync(objectDir).filter(p => p.endsWith('.key')).sort().map(p => p + readFileSync(resolve(objectDir, p), 'utf8')).join('\n');
  const k1 = keys();
  build(); assert.equal(keys(), k1);
  f.put('ports/th13/portable/sdl/backend.hpp', '// backend 2');
  const b = build(); const k2 = keys(); assert.notEqual(k2, k1);
  f.put('sdk/upstream/emscripten/emcc.py', compilerText(2));
  const c = build(); assert.notEqual(keys(), k2);
  assert.notDeepEqual(a.sourceFiles, b.sourceFiles);
  assert.notDeepEqual(b.toolchain, c.toolchain);
  const printed = f.run('build.mjs', env, ['--print-inputs']);
  assert.equal(printed.status, 0, printed.stderr);
  assert.ok(JSON.parse(printed.stdout).inputs['portable/sdl/backend.hpp']);
  assert.ok(c.linkFlags.includes('-sMODULARIZE=1'));
  const out = resolve(f.port, 'web/artifacts/sdl3');
  const failed = expected => {
    const run = f.run('build.mjs', {...env, ...expected.env});
    assert.notEqual(run.status, 0, run.stdout);
    assert.match(run.stderr, expected.error);
    for (const name of ['build.json', 'th13-sdl.wasm', 'th13-sdl.mjs']) assert.equal(existsSync(resolve(out, name)), false);
  };
  failed({env: {TH_PYTHON: resolve(f.dir, 'missing-python')}, error: /ENOENT/});
  // Cache misses may not recycle objects left by an earlier successful compiler.
  f.put('sdk/upstream/emscripten/emcc.py', "import sys\nif '--version' in sys.argv: print('noop')\n");
  failed({env: {}, error: /Missing fresh object/});
  f.put('sdk/upstream/emscripten/emcc.py', "import os, signal\nos.kill(os.getpid(), signal.SIGTERM)\n");
  failed({env: {}, error: /SIGTERM/});
});
