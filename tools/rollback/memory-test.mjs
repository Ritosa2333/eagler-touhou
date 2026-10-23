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
  const artifactParent = join(root, 'artifacts/rollback');
  await mkdir(artifactParent, { recursive: true });
  const output = options.output || await mkdtemp(join(artifactParent, 'memory-test-'));
  await mkdir(output, { recursive: true });
  const modulePath = join(output, 'fixture.mjs');
  const exports = ['_eagler_write_journal_test', '_eagler_rb_touch', '_eagler_rb_config_address', '_eagler_rb_instrumented'];
  const args = [
    '-std=c++20', '-O2', '-UNDEBUG', '-I', join(root, 'common/include'),
    join(root, 'common/tests/write-journal-test.cpp'),
    join(root, 'common/src/netplay/WriteJournal.cpp'),
    join(root, 'common/src/netplay/SimulationHeap.cpp'),
    '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
    '-sALLOW_MEMORY_GROWTH=1', '-sINVOKE_RUN=0', '-sASSERTIONS=1',
    '-sINCOMING_MODULE_JS_API=["wasmBinary"]',
    `-sEXPORTED_FUNCTIONS=${JSON.stringify(exports)}`, '--no-entry', '-o', modulePath,
  ];

  console.log(`Compiling optimized C++ journal fixture: ${output}`);
  try {
    const result = await run(compiler, args, {
      cwd: root, env: { ...process.env, EM_CONFIG: process.env.EM_CONFIG || join(sdk, '.emscripten') },
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

const profiles = [], scenarios = [0, 1, 2, 3, 4, 5, 6];
for (const nativeFilter of [false, true]) {
  const name = nativeFilter ? 'native-hook' : 'wasm';
  const { bytes, report } = await instrumentWasm(raw, { nativeFilter });
  await verifyInstrumentedWasm(bytes);
  await writeFile(join(output, `fixture.${name}.wasm`), bytes);
  for (const scenario of scenarios) {
    const instance = await createModule({ wasmBinary: bytes });
    assert.equal(instance._eagler_rb_instrumented(), INSTRUMENTATION_MAGIC, 'patched marker is callable through the compiled loader');
    assert.equal(instance._eagler_write_journal_test(scenario), 1, `C++ journal ${name} scenario ${scenario}`);
  }
  profiles.push({ name, scenarios, report });
}
await writeFile(join(output, 'result.json'), `${JSON.stringify({ success: true, uninstrumentedRejected: true, profiles }, null, 2)}\n`);
console.log(`C++ Wasm journal fixture passed (raw rejection + ${scenarios.length} scenarios in both profiles). Artifacts: ${output}`);
}

try { await main(); }
catch (error) { console.error(error.stack || error); process.exitCode = 1; }
