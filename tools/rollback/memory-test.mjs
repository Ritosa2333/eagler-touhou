#!/usr/bin/env node
// Exercise the real C++ allocator and journal after the production Wasm pass.
import assert from 'node:assert/strict';
import { execFile } from 'node:child_process';
import { mkdir, mkdtemp, readFile, writeFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { promisify } from 'node:util';
import { instrumentWasm, verifyInstrumentedWasm, INSTRUMENTATION_MAGIC } from './instrument.mjs';

const run = promisify(execFile);
const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
async function main() {
  const options = {};
  for (const arg of process.argv.slice(2)) {
    const match = /^--(emxx|output)=(.+)$/.exec(arg);
    if (!match) throw new Error('usage: memory-test.mjs [--emxx=/path/to/em++] [--output=/path/to/artifacts]');
    options[match[1]] = resolve(match[2]);
  }
  const sdk = process.env.EMSDK || join(root, 'toolchains/emsdk');
  const compiler = options.emxx || process.env.EAGLER_EMXX || join(sdk, 'upstream/emscripten/em++');
  const llvmNm = process.env.EAGLER_LLVM_NM || join(sdk, 'upstream/bin', process.platform === 'win32' ? 'llvm-nm.exe' : 'llvm-nm');
  const environment = { ...process.env, EM_CONFIG: process.env.EM_CONFIG || join(sdk, '.emscripten') };
  const artifactParent = join(root, 'artifacts/rollback');
  await mkdir(artifactParent, { recursive: true });
  const output = options.output || await mkdtemp(join(artifactParent, 'memory-test-'));
  await mkdir(output, { recursive: true });
  const backends = ['WriteJournal', 'SimulationHeap'];
  const backendSource = name => join(root, 'common/src/netplay', `${name}.cpp`);
  const inactiveIsolation = { success: false, profiles: [] };
  const inactiveLog = [];
  console.log('Checking that disabled journal sources define no Wasm symbols');
  try {
    for (const [name, definitions] of [['undefined', []], ['zero', ['-DEAGLER_WRITE_JOURNAL=0']]]) {
      const objects = [];
      for (const backend of backends) {
        const object = `inactive-${name}-${backend}.o`;
        const result = await run(compiler,
          ['-std=c++20', '-O2', ...definitions, '-c', backendSource(backend), '-o', join(output, object)],
          { cwd: root, env: environment, timeout: 180_000, maxBuffer: 8 * 1024 * 1024 });
        inactiveLog.push(`${object} compile\n${result.stdout}${result.stderr}`);
        const symbols = await run(llvmNm, ['--defined-only', '--format=posix', join(output, object)],
          { cwd: root, env: environment, timeout: 30_000, maxBuffer: 8 * 1024 * 1024 });
        inactiveLog.push(`${object} llvm-nm --defined-only\n${symbols.stdout}${symbols.stderr}`);
        assert.equal(symbols.stdout.trim(), '', `${object} must not define allocator, hook, mspace, or other symbols`);
        objects.push({ source: `common/src/netplay/${backend}.cpp`, object, definedSymbols: [] });
      }
      inactiveIsolation.profiles.push({ macro: name, objects });
    }
    inactiveIsolation.success = true;
  } catch (error) {
    inactiveLog.push(`${error.stdout || ''}${error.stderr || ''}${error.stack || error}`);
    throw error;
  } finally {
    await writeFile(join(output, 'inactive-compile.log'), `${inactiveLog.join('\n')}\n`);
    await writeFile(join(output, 'inactive-isolation.json'), `${JSON.stringify(inactiveIsolation, null, 2)}\n`);
  }
  console.log('Disabled journal isolation passed (4 real Wasm objects, zero defined symbols)');
  const modulePath = join(output, 'fixture.mjs');
  const exports = ['_eagler_write_journal_test', '_eagler_write_journal_trap', '_eagler_write_journal_abandon',
    '_eagler_rb_touch', '_eagler_rb_config_address', '_eagler_rb_dirty_config_address',
    '_eagler_rb_dirty_config_abi', '_eagler_rb_instrumented'];
  const args = [
    '-std=c++20', '-O2', '-UNDEBUG', '-DEAGLER_WRITE_JOURNAL=1', '-DEAGLER_WRITE_JOURNAL_TEST_HOOKS=1', '-I', join(root, 'common/include'),
    join(root, 'common/tests/write-journal-test.cpp'),
    ...backends.map(backendSource),
    '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
    '-sALLOW_MEMORY_GROWTH=1', '-sINVOKE_RUN=0', '-sASSERTIONS=1',
    '-sINCOMING_MODULE_JS_API=["wasmBinary"]',
    `-sEXPORTED_FUNCTIONS=${JSON.stringify(exports)}`, '--no-entry', '-o', modulePath,
  ];

  console.log(`Compiling optimized C++ journal fixture: ${output}`);
  try {
    const result = await run(compiler, args, {
      cwd: root, env: environment,
      timeout: 180_000, maxBuffer: 8 * 1024 * 1024,
    });
    await writeFile(join(output, 'compile.log'), result.stdout + result.stderr);
    if (result.stderr) process.stderr.write(result.stderr);
  } catch (error) {
    await writeFile(join(output, 'compile.log'), `${error.stdout || ''}${error.stderr || ''}${error.message}\n`);
    throw error;
  }

  const raw = await readFile(join(output, 'fixture.wasm'));
  const createModule = (await import(pathToFileURL(modulePath).href)).default;
  // A fresh instance prevents the failed raw initialization from sharing any
  // allocator state, roots, tags, or sticky failure flag with positive scenarios.
  const uninstrumented = await createModule({ wasmBinary: raw });
  assert.equal(uninstrumented._eagler_rb_instrumented(), 0, 'raw marker is the volatile C++ placeholder');
  assert.equal(uninstrumented._eagler_write_journal_test(-1), 0, 'raw Wasm must reject journal initialization');
  console.log('Uninstrumented module correctly rejects journal initialization');

const profiles = [], scenarios = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14], trapScopes = ['game', 'host'];
for (const [name, options] of [
  ['wasm', { nativeFilter: false }],
  ['native-hook', { nativeFilter: true }],
  ['native-scalar-hooks', { nativeFilter: true, scalarHooks: true }],
  ['native-inline-dirty', { nativeFilter: true, inlineDirtyBlocks: true }],
]) {
  const { bytes, report } = await instrumentWasm(raw, options);
  await verifyInstrumentedWasm(bytes, options);
  await writeFile(join(output, `fixture.${name}.wasm`), bytes);
  for (const scenario of scenarios) {
    const instance = await createModule({ wasmBinary: bytes });
    assert.equal(instance._eagler_rb_instrumented(), INSTRUMENTATION_MAGIC, 'patched marker is callable through the compiled loader');
    assert.equal(instance._eagler_write_journal_test(scenario), 1, `C++ journal ${name} scenario ${scenario}`);
  }
  for (const [hostScope, scope] of trapScopes.entries()) {
    const instance = await createModule({ wasmBinary: bytes });
    assert.throws(() => instance._eagler_write_journal_trap(hostScope),
      error => error instanceof WebAssembly.RuntimeError && /out of bounds/.test(error.message),
      `C++ journal ${name} ${scope} scope must trap in the real Wasm store`);
    assert.equal(instance._eagler_write_journal_abandon(hostScope), 1,
      `C++ journal ${name} ${scope} scope cannot reuse an abandoned transaction`);
  }
  profiles.push({ name, scenarios, trapScopes, report });
}
await writeFile(join(output, 'result.json'), `${JSON.stringify({ success: true, inactiveIsolation, uninstrumentedRejected: true, profiles }, null, 2)}\n`);
console.log(`C++ Wasm journal fixture passed (4 inactive objects + raw rejection + ${scenarios.length} scenarios + ${trapScopes.length} real trap cases in ${profiles.length} profiles). Artifacts: ${output}`);
}

try { await main(); }
catch (error) { console.error(error.stack || error); process.exitCode = 1; }
