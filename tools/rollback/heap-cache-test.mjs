#!/usr/bin/env node
// Supplemental native allocator checks; real undo is covered by memory-test.mjs.
import assert from 'node:assert/strict';
import { execFile } from 'node:child_process';
import { createHash } from 'node:crypto';
import { mkdir, mkdtemp, readFile, writeFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { promisify } from 'node:util';

const run = promisify(execFile);
const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
async function main() {
  const options = {};
  for (const arg of process.argv.slice(2)) {
    const match = /^--(cxx|output)=(.+)$/.exec(arg);
    if (!match) throw Error('usage: heap-cache-test.mjs [--cxx=/path/to/c++] [--output=/new/directory]');
    options[match[1]] = match[2];
  }
  const compiler = options.cxx || process.env.CXX || 'c++';
  const parent = join(root, 'artifacts/rollback');
  await mkdir(parent, { recursive: true });
  const output = options.output ? resolve(options.output) : await mkdtemp(join(parent, 'heap-cache-native-'));
  if (options.output) await mkdir(output, { recursive: false });
  const binary = join(output, process.platform === 'win32' ? 'heap-cache-test.exe' : 'heap-cache-test');
  const sources = ['common/tests/simulation-heap-cache-test.cpp', 'common/src/netplay/SimulationHeap.cpp'];
  const args = ['-std=c++20', '-O2', '-UNDEBUG', '-DEAGLER_WRITE_JOURNAL=1',
    '-DEAGLER_WRITE_JOURNAL_TEST_HOOKS=1', ...sources.map(source => join(root, source)), '-o', binary];
  const evidence = { success: false, compiler, args, sources: [] };
  try {
    for (const source of [...sources, 'common/third_party/dlmalloc/dlmalloc.inc']) {
      const data = await readFile(join(root, source));
      evidence.sources.push({ source, bytes: data.length, sha256: createHash('sha256').update(data).digest('hex') });
    }
    const version = await run(compiler, ['--version'], { cwd: root, timeout: 30_000 });
    evidence.compilerVersion = version.stdout.trim();
    const built = await run(compiler, args, { cwd: root, timeout: 180_000, maxBuffer: 8 * 1024 * 1024 });
    await writeFile(join(output, 'compile.log'), built.stdout + built.stderr);
    const executed = await run(binary, [], { cwd: root, timeout: 30_000, maxBuffer: 8 * 1024 * 1024 });
    await writeFile(join(output, 'run.log'), executed.stdout + executed.stderr);
    assert.match(executed.stdout, /simulation heap cache: PASS \d+ uncached comparisons, 128 repeated hits each/);
    evidence.stdout = executed.stdout;
    evidence.success = true;
    console.log(executed.stdout.trim());
    console.log(`Native heap-cache evidence: ${output}`);
  } catch (error) {
    evidence.error = error.stack || String(error);
    await writeFile(join(output, 'failure.log'), `${error.stdout || ''}${error.stderr || ''}${evidence.error}\n`);
    throw error;
  } finally {
    await writeFile(join(output, 'result.json'), `${JSON.stringify(evidence, null, 2)}\n`);
  }
}

try { await main(); }
catch (error) { console.error(error.stack || error); process.exitCode = 1; }
