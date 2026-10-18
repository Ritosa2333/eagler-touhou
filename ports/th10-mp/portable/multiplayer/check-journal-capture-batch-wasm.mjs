import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {appendFileSync, existsSync, mkdirSync, readFileSync, renameSync, writeFileSync} from 'node:fs';
import {dirname, relative, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';

// Compiles the real Emscripten implementation, then runs bounded Node fixtures.
// No title/browser process is started. Never run alongside the title build.
// --output paths are relative to the source root unless already absolute.
const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const usage = [
  'node portable/multiplayer/check-journal-capture-batch-wasm.mjs',
  '  [--output artifacts/.../receipt.json] [--sdk PATH] [--python EXE]',
  '  [--print-plan]',
  'Defaults: EMSDK / the shared local SDK; TH_PYTHON / EMSDK_PYTHON / python.',
].join('\n');
const options = {};
for (let index = 2; index < process.argv.length; ++index) {
  const key = process.argv[index];
  if (key === '--help') { console.log(usage); process.exit(0); }
  if (key === '--print-plan') { options.printPlan = true; continue; }
  if (!['--output', '--sdk', '--python'].includes(key) || index + 1 >= process.argv.length)
    throw Error('Invalid argument: ' + key + '\n' + usage);
  options[key.slice(2)] = process.argv[++index];
}
const stamp = new Date().toISOString().replace(/[-:.]/g, '');
const output = resolve(root, options.output ||
  'artifacts/rollback-smoothness/journal-capture-batch-wasm/' + stamp + '/receipt.json');
const out = dirname(output);
const buildDir = resolve(out, 'build');
const loader = resolve(buildDir, 'journal-capture-batch-probe.mjs');
const wasmPath = loader.replace(/\.mjs$/, '.wasm');
const common = resolve(root, 'third_party/eagler-common');
const probe = resolve(root, 'tests/rollback-journal-capture-batch-wasm-probe.cpp');
const journal = resolve(common, 'src/netplay/RollbackJournal.cpp');
const sourcePaths = [
  fileURLToPath(import.meta.url), probe,
  resolve(root, 'tests/rollback-journal-capture-batch-test.cpp'),
  journal, resolve(common, 'include/eagler/netplay/RollbackJournal.hpp'),
];
const sha = value => createHash('sha256').update(value).digest('hex');
const normalized = path => relative(root, path).replaceAll('\\', '/');
const sourceHashes = () => Object.fromEntries(sourcePaths.map(path => [normalized(path), sha(readFileSync(path))]));
const errorInfo = error => ({
  name: String(error?.name || 'Error'),
  message: String(error?.message || error).slice(0, 4096),
  stack: String(error?.stack || '').slice(0, 8192),
});

function compilerPlan() {
  const explicit = options.sdk || process.env.EMSDK;
  const candidates = explicit ? [resolve(explicit)] : [
    resolve(root, 'tools/emsdk'),
    resolve(root, '../toolchains/emsdk'),
    resolve(root, '../../toolchains/emsdk'),
    ...(process.env.EAGLER_WORKSPACE ? [resolve(process.env.EAGLER_WORKSPACE, 'toolchains/emsdk')] : []),
  ];
  for (const sdk of candidates) {
    const emcc = ['install/emscripten/emcc.py', 'upstream/emscripten/emcc.py']
      .map(name => resolve(sdk, name)).find(existsSync);
    if (!emcc) continue;
    const config = process.env.EM_CONFIG || resolve(sdk, '.emscripten');
    if (!existsSync(config)) throw Error('Emscripten config does not exist: ' + config);
    const python = options.python || process.env.TH_PYTHON || process.env.EMSDK_PYTHON || 'python';
    // Match portable/build.mjs's optimization and numeric flags. Do not add
    // a Safari version, bulk-memory switch, exception shim, or fake copy import.
    const argv = [
      emcc, '-O2', '-g0', '-fno-strict-aliasing', '-ffp-contract=off',
      '-std=c++17', '-fno-exceptions', '-fno-rtti', '-I' + resolve(common, 'include'),
      '-sDEFAULT_TO_CXX=1', '--no-entry', '-sMODULARIZE=1', '-sEXPORT_ES6=1',
      '-sENVIRONMENT=node', '-sALLOW_MEMORY_GROWTH=1', '-sSTACK_SIZE=1048576',
      '-sINITIAL_MEMORY=16777216', '-sMAXIMUM_MEMORY=134217728',
      '-sFILESYSTEM=0', '-sINVOKE_RUN=0', '-sEXIT_RUNTIME=0',
      probe, journal, '-o', loader,
    ];
    const settings = resolve(dirname(emcc), 'src/settings.js');
    const safari = existsSync(settings)
      ? /var\s+MIN_SAFARI_VERSION\s*=\s*(\d+)/.exec(readFileSync(settings, 'utf8')) : null;
    return {
      sdk, emcc, python, argv, config,
      sdkDefaultMinSafariVersion: safari ? Number(safari[1]) : null,
      env: {...process.env, EMSDK: sdk, EM_CONFIG: config, EMCC_CORES: process.env.EMCC_CORES || '1'},
      recordedEnv: {
        EMSDK: sdk, EM_CONFIG: config, EMCC_CORES: process.env.EMCC_CORES || '1',
        EMCC_CFLAGS: process.env.EMCC_CFLAGS || '',
      },
    };
  }
  throw Error('Local Emscripten SDK not found; set --sdk or EMSDK. No download is attempted.');
}

const receipt = {
  schema: 1, probe: 'real-emscripten-journal-capture-batch', status: 'RUNNING',
  startedAt: new Date().toISOString(), sourceRoot: root, output,
  node: {executable: process.execPath, version: process.version, platform: process.platform},
  differencesFromTitleBuild: [
    'Dedicated resource-free C++ probe and unchanged common RollbackJournal.cpp only.',
    'Node environment instead of web,worker; diagnostic exports; no SDL/title/filesystem.',
    '16 MiB initial / 128 MiB maximum memory instead of 64 MiB / 1 GiB; same 1 MiB stack.',
    'No Safari or WASM memory-feature override; inherit the selected SDK and EMCC_CFLAGS.',
    'Host copyWithin observer forwards each call to the original native method; never substitutes copies.',
    'Each deliberate OOB failure uses a fresh WASM instance; only raw memory is read after the failure.',
    'Correctness evidence only; durations are not a performance comparison.',
  ],
  fixtures: [],
};
function saveReceipt() {
  mkdirSync(out, {recursive: true});
  const temporary = output + '.tmp';
  writeFileSync(temporary, JSON.stringify(receipt, null, 2) + '\n');
  renameSync(temporary, output);
}

async function compile(plan) {
  mkdirSync(buildDir, {recursive: true});
  const logPath = resolve(out, 'compile.log');
  writeFileSync(logPath, '');
  const started = Date.now();
  const result = {command: plan.python, argv: plan.argv, env: plan.recordedEnv,
    log: logPath, status: 'RUNNING', stdoutTail: '', stderrTail: ''};
  receipt.build = result;
  saveReceipt();
  console.log('BUILD real Emscripten journal probe');
  await new Promise((done, reject) => {
    const child = spawn(plan.python, plan.argv, {
      cwd: root, env: plan.env, windowsHide: true, shell: false,
      stdio: ['ignore', 'pipe', 'pipe'],
    });
    let bytes = 0, reason = null, completed = false;
    const timer = setTimeout(() => {
      reason = Error('Probe compilation exceeded 180 seconds');
      child.kill();
    }, 180000);
    const collect = (key, chunk) => {
      result[key] = (result[key] + chunk.toString()).slice(-8192);
      const remaining = Math.max(0, 256 * 1024 - bytes);
      if (remaining) appendFileSync(logPath, chunk.subarray(0, remaining));
      bytes += chunk.length;
      if (bytes > 256 * 1024 && !reason) {
        reason = Error('Probe compiler output exceeded the 256 KiB limit');
        child.kill();
      }
    };
    child.stdout.on('data', chunk => collect('stdoutTail', chunk));
    child.stderr.on('data', chunk => collect('stderrTail', chunk));
    child.on('error', error => {
      if (completed) return;
      completed = true; clearTimeout(timer); reject(error);
    });
    child.on('close', (code, signal) => {
      if (completed) return;
      completed = true; clearTimeout(timer);
      result.exitCode = code; result.signal = signal; result.elapsedMs = Date.now() - started;
      result.outputBytes = bytes;
      if (reason || code !== 0) reject(reason || Error('Probe compiler exit code ' + code));
      else done();
    });
  });
  result.status = 'PASS';
}

const statNames = ['scopes', 'deferredBlocks', 'deferredBytes', 'copiedBlocks',
  'copiedBytes', 'flushes', 'batchImports', 'maxBlocksPerFlush'];
const statusNames = ['schema', 'frameOpen', 'scopeOpen', 'pendingBlocks', 'pendingBytes',
  'failed', 'bytes', 'blocks', 'frames', 'heapBytes', 'restoredFrame', 'arenaGrowths'];
function words(fixture, pointer, count) {
  assert(Number.isInteger(pointer) && pointer >= 0 && pointer + count * 4 <= fixture.memory.buffer.byteLength);
  const view = new DataView(fixture.memory.buffer);
  return Array.from({length: count}, (_, index) => view.getUint32(pointer + index * 4, true));
}
function bytesAt(fixture, pointer, count) {
  assert(pointer >= 0 && count >= 0 && pointer + count <= fixture.memory.buffer.byteLength);
  return Buffer.from(new Uint8Array(fixture.memory.buffer, pointer, count));
}
function liveStats(fixture) {
  const view = new DataView(fixture.memory.buffer);
  return Object.fromEntries(statNames.map((name, index) => {
    const value = view.getBigUint64(fixture.description.stats + index * 8, true);
    assert(value <= BigInt(Number.MAX_SAFE_INTEGER));
    return [name, Number(value)];
  }));
}
function invoke(fixture, name, ...args) {
  assert(!fixture.poisoned, 'Never reenter a WASM instance after its trapped C++ call');
  try { return fixture.module['_journal_batch_' + name](...args); }
  catch (error) { fixture.poisoned = true; throw error; }
}
function status(fixture) {
  const pointer = invoke(fixture, 'probe_status') >>> 0;
  assert.equal(pointer, fixture.description.status);
  const values = words(fixture, pointer, statusNames.length);
  const result = Object.fromEntries(statusNames.map((name, index) =>
    [name, values[index]]));
  assert.equal(result.schema, 1);
  return result;
}
function checkClosed(fixture) {
  const state = status(fixture), stats = liveStats(fixture);
  assert.equal(state.scopeOpen, 0); assert.equal(state.pendingBlocks, 0);
  assert.equal(state.pendingBytes, 0); assert.equal(state.failed, 0);
  assert.equal(stats.deferredBlocks, stats.copiedBlocks);
  assert.equal(stats.deferredBytes, stats.copiedBytes);
  assert.equal(stats.batchImports, stats.flushes);
  return {state, stats};
}
function expectedStats(fixture, expected) {
  const actual = liveStats(fixture);
  for (const [name, value] of Object.entries(expected)) assert.equal(actual[name], value, name);
  return actual;
}

// Trace only: every operation executes the original built-in copyWithin.
// It observes actual destination bytes as well as the journal's import counts.
function observeCopies(fixture, action) {
  const prototype = Uint8Array.prototype;
  const previous = Object.getOwnPropertyDescriptor(prototype, 'copyWithin');
  const original = prototype.copyWithin;
  const firstBuffer = fixture.memory.buffer;
  const observation = {result: null, error: null, copies: [], totalCopies: 0, totalBytes: 0, dropped: 0};
  const indexOf = (value, length, fallback) => {
    const number = value === undefined ? fallback : Math.trunc(Number(value));
    return number < 0 ? Math.max(length + number, 0) : Math.min(number, length);
  };
  Object.defineProperty(prototype, 'copyWithin', {
    configurable: true, writable: true, value: function(target, start, end) {
      const buffer = this.buffer, length = this.length, offset = this.byteOffset;
      const relevant = buffer === firstBuffer || buffer === fixture.memory.buffer;
      const to = indexOf(target, length, 0), from = indexOf(start, length, 0);
      const finish = indexOf(end, length, length);
      const count = Math.max(0, Math.min(finish - from, length - to));
      const result = Reflect.apply(original, this, arguments);
      if (relevant) {
        ++observation.totalCopies; observation.totalBytes += count;
        if (observation.copies.length < 256) {
          // Validate at the copy itself: an arena-growth copy may target an
          // old arena that is freed before the enclosing export returns.
          const destination = Buffer.from(buffer, offset + to, count);
          const source = Buffer.from(buffer, offset + from, count);
          observation.copies.push({to: offset + to, from: offset + from, bytes: count,
            currentHeap: buffer === fixture.memory.buffer,
            exactAtCopy: destination.equals(source), destinationSha256: sha(destination)});
        } else ++observation.dropped;
      }
      return result;
    },
  });
  try { observation.result = action(); }
  catch (error) { observation.error = error; }
  finally {
    if (previous) Object.defineProperty(prototype, 'copyWithin', previous);
    else delete prototype.copyWithin;
  }
  return observation;
}
function successful(observation, value = 1) {
  if (observation.error) throw observation.error;
  assert.equal(observation.result, value);
  return observation;
}
function checkPrefixCopies(fixture, copies, ranges, original, retained = true) {
  assert.equal(copies.length, ranges.length);
  return ranges.map((range, index) => {
    const copy = copies[index];
    assert.equal(copy.from, fixture.description.data + range.offset);
    assert.equal(copy.bytes, range.bytes);
    assert.equal(copy.currentHeap, true);
    assert.equal(copy.exactAtCopy, true);
    const expected = original.subarray(range.offset, range.offset + range.bytes);
    assert.equal(copy.destinationSha256, sha(expected));
    if (retained) assert(bytesAt(fixture, copy.to, copy.bytes).equals(expected));
    return {...copy, retainedDestinationChecked: retained, exact: true};
  });
}

let factory, compiledWasm;
async function fresh(name, stateBytes = null, maxBytes = 16384, maxBlocks = 16) {
  const fixture = {name, memory: null, module: null, poisoned: false, logs: [], logLinesDropped: 0};
  const log = (kind, line) => {
    if (fixture.logs.length < 32) fixture.logs.push({kind, text: String(line).slice(0, 1024)});
    else ++fixture.logLinesDropped;
  };
  fixture.module = await factory({
    noInitialRun: true, print: line => log('stdout', line), printErr: line => log('stderr', line),
    instantiateWasm(imports, receive) {
      const instance = new WebAssembly.Instance(compiledWasm, imports);
      const memories = Object.values(instance.exports).filter(value => value instanceof WebAssembly.Memory);
      assert.equal(memories.length, 1);
      fixture.memory = memories[0];
      receive(instance, compiledWasm);
      return instance.exports;
    },
  });
  assert(fixture.memory instanceof WebAssembly.Memory);
  if (stateBytes !== null) {
    const pointer = invoke(fixture, 'probe_create', stateBytes, maxBytes, maxBlocks) >>> 0;
    const data = words(fixture, pointer, 9);
    assert.equal(data[0], 1); assert.equal(data[2], stateBytes);
    assert.equal(data[4], 64); assert.equal(data[6], 6); assert.equal(data[8], statusNames.length);
    assert.equal(data[3] % 8, 0);
    fixture.description = {data: data[1], bytes: data[2], stats: data[3], marks: data[5], status: data[7]};
    checkClosed(fixture);
  }
  return fixture;
}
async function runCase(name, action) {
  const entry = {name, status: 'RUNNING'};
  receipt.fixtures.push(entry); saveReceipt();
  const started = Date.now();
  try {
    Object.assign(entry, await action());
    entry.status = 'PASS'; entry.elapsedMs = Date.now() - started;
    saveReceipt(); console.log('PASS ' + name);
  } catch (error) {
    entry.status = 'FAIL'; entry.elapsedMs = Date.now() - started;
    entry.error = errorInfo(error); saveReceipt(); throw error;
  }
}

async function directedSuite() {
  const f = await fresh('directed-suite');
  const observation = successful(observeCopies(f, () => invoke(f, 'run_directed_suite')), 0);
  assert(observation.totalCopies > 0);
  const pass = f.logs.map(line => line.text).join('\n').match(/PASS \((\d+) checks\)/);
  assert(pass, 'The included full-byte directed suite must report its checks');
  return {checks: Number(pass[1]), actualCopyWithinCalls: observation.totalCopies,
    actualCopyWithinBytes: observation.totalBytes, logs: f.logs, logLinesDropped: f.logLinesDropped};
}

async function pendingGrowthAndThresholds() {
  const f = await fresh('pending-growth-thresholds', 8192);
  const original = bytesAt(f, f.description.data, f.description.bytes);
  assert.equal(invoke(f, 'probe_begin_frame', 0), 1);
  assert.equal(invoke(f, 'probe_touch', f.description.data, 1100), 1);
  invoke(f, 'probe_fill', 0, 1100, 0x81);
  assert.equal(invoke(f, 'probe_begin_capture'), 1);
  const windowStart = bytesAt(f, f.description.data, f.description.bytes);
  const large = [{offset: 1537, bytes: 1024}, {offset: 3073, bytes: 1025}];
  for (const range of [large[0], {offset: 4501, bytes: 1023}, large[1], {offset: 7603, bytes: 37}])
    assert.equal(invoke(f, 'probe_touch', f.description.data + range.offset, range.bytes), 1);
  assert.equal(invoke(f, 'probe_touch', f.description.data, 1100), 1);
  assert.equal(invoke(f, 'probe_touch', f.description.data + 1537, 1024), 1);
  const before = expectedStats(f, {scopes: 1, deferredBlocks: 2, deferredBytes: 2049,
    copiedBlocks: 0, copiedBytes: 0, flushes: 0, batchImports: 0, maxBlocksPerFlush: 0});
  assert.equal(status(f).pendingBlocks, 2);
  const oldBuffer = f.memory.buffer, oldView = new Uint8Array(oldBuffer), oldBytes = oldBuffer.byteLength;
  const grownBytes = invoke(f, 'probe_grow', 1) >>> 0;
  assert(grownBytes > oldBytes);
  assert.notEqual(f.memory.buffer, oldBuffer);
  assert.equal(oldBuffer.byteLength, 0); assert.equal(oldView.byteLength, 0);
  assert.equal(f.memory.buffer.byteLength, grownBytes);
  assert(bytesAt(f, f.description.data, f.description.bytes).equals(windowStart));
  const end = successful(observeCopies(f, () => invoke(f, 'probe_end_capture')));
  assert.equal(end.dropped, 0);
  const copied = checkPrefixCopies(f, end.copies, large, original);
  const after = expectedStats(f, {scopes: 1, deferredBlocks: 2, deferredBytes: 2049,
    copiedBlocks: 2, copiedBytes: 2049, flushes: 1, batchImports: 1, maxBlocksPerFlush: 2});
  checkClosed(f);
  for (const range of [...large, {offset: 4501, bytes: 1023}, {offset: 7603, bytes: 37}])
    invoke(f, 'probe_fill', range.offset, range.bytes, 0x39);
  const outside = successful(observeCopies(f, () => invoke(f, 'probe_touch', f.description.data + 5701, 1500)));
  assert.equal(outside.copies.length, 1);
  invoke(f, 'probe_fill', 5701, 1500, 0xd9);
  assert.equal(invoke(f, 'probe_touch', f.description.data + 5701, 1500), 1);
  assert.deepEqual(liveStats(f), after);
  assert.equal(invoke(f, 'probe_end_frame'), 1);
  assert.equal(invoke(f, 'probe_undo', 0), 1);
  assert(bytesAt(f, f.description.data, f.description.bytes).equals(original));
  checkClosed(f);
  return {before, after, oldHeapBytes: oldBytes, grownHeapBytes: grownBytes,
    oldViewDetached: true, copied, outsideImmediateCopies: outside.copies,
    fullByteUndo: true, originalSha256: sha(original)};
}

async function arenaGrowthAndReuse() {
  const size = 720 * 1024 + 31, a = 140 * 1024 + 3, b = 160 * 1024 + 5, c = 310 * 1024 + 7;
  const all = [{offset: 0, bytes: 3}, {offset: 3, bytes: a}, {offset: 3 + a, bytes: b},
    {offset: 3 + a + b, bytes: c}, {offset: 3 + a + b + c, bytes: size - 3 - a - b - c}];
  const order = [all[1], all[0], all[2], all[4], all[3]];
  const largeOrder = order.filter(range => range.bytes >= 1024);
  const f = await fresh('arena-growth-reuse', size, size, 5);
  const original = bytesAt(f, f.description.data, size);
  const rounds = [];
  for (let frame = 0; frame < 2; ++frame) {
    assert.equal(invoke(f, 'probe_begin_frame', frame), 1);
    assert.equal(invoke(f, 'probe_begin_capture'), 1);
    const capture = successful(observeCopies(f, () => {
      for (const range of order)
        assert.equal(invoke(f, 'probe_touch', f.description.data + range.offset, range.bytes), 1);
      assert.equal(invoke(f, 'probe_touch', f.description.data + all[2].offset, all[2].bytes), 1);
      return invoke(f, 'probe_end_capture');
    }));
    assert.equal(capture.dropped, 0);
    const sourceCopies = capture.copies.filter(copy =>
      copy.bytes >= 1024 && copy.from >= f.description.data &&
      copy.from + copy.bytes <= f.description.data + size);
    const copied = checkPrefixCopies(f, sourceCopies, largeOrder, original, false);
    const checked = checkClosed(f);
    assert.equal(checked.state.arenaGrowths, 3);
    assert.equal(checked.state.bytes, size); assert.equal(checked.state.blocks, 5);
    const expected = {scopes: frame + 1, deferredBlocks: 4 * (frame + 1),
      deferredBytes: (size - 3) * (frame + 1), copiedBlocks: 4 * (frame + 1),
      copiedBytes: (size - 3) * (frame + 1), flushes: frame === 0 ? 3 : 4,
      batchImports: frame === 0 ? 3 : 4, maxBlocksPerFlush: frame === 0 ? 2 : 4};
    expectedStats(f, expected);
    invoke(f, 'probe_fill', 0, size, 0x52 + frame);
    assert.equal(invoke(f, 'probe_end_frame'), 1);
    assert.equal(invoke(f, 'probe_undo', frame), 1);
    assert(bytesAt(f, f.description.data, size).equals(original));
    rounds.push({frame, state: checked.state, stats: checked.stats, actualSourceCopies: copied,
      otherCopyWithinCalls: capture.copies.filter(copy => !sourceCopies.includes(copy)),
      fullByteUndo: true});
  }
  return {rounds, arenaReusedWithoutGrowth: true, originalSha256: sha(original)};
}

async function outOfBoundsOne(size, batch) {
  const f = await fresh('oob-' + size + (batch ? '-batch' : '-original'), 8192);
  const original = bytesAt(f, f.description.data, f.description.bytes);
  const ranges = [{offset: 17, bytes: 1024}, {offset: 2051, bytes: 1025}];
  assert.equal(invoke(f, 'probe_begin_frame', 0), 1);
  if (batch) assert.equal(invoke(f, 'probe_begin_capture'), 1);
  const prepare = successful(observeCopies(f, () => {
    for (const range of ranges)
      assert.equal(invoke(f, 'probe_touch', f.description.data + range.offset, range.bytes), 1);
    return 1;
  }));
  const before = liveStats(f), beforeState = status(f);
  assert.equal(beforeState.pendingBlocks, batch ? 2 : 0);
  assert.equal(beforeState.pendingBytes, batch ? 2049 : 0);
  const heapBytes = f.memory.buffer.byteLength;
  // Capacity was allocated by the valid prefix. The bad Touch cannot grow
  // memory, accidentally making this old end-of-memory address valid.
  const address = heapBytes - (size === 1 ? 0 : 8);
  const failure = observeCopies(f, () => invoke(f, 'probe_touch', address, size));
  assert(failure.error, 'Out-of-bounds Touch must fail before returning');
  assert(f.poisoned);
  if (size >= 1024) {
    assert(failure.error instanceof RangeError);
    assert.equal(failure.error.message, 'rollback copy outside WASM memory');
  } else assert(failure.error instanceof WebAssembly.RuntimeError);
  // Never call status/end/undo or any other export on this instance now.
  const marks = words(f, f.description.marks, 6);
  const after = liveStats(f);
  assert.equal(f.memory.buffer.byteLength, heapBytes);
  assert.equal(marks[0], 1); assert.equal(marks[1], 3); assert.equal(marks[2], 2);
  assert.equal(marks[3], address); assert.equal(marks[4], size); assert.equal(marks[5], 0xffffffff);
  const expected = batch
    ? {scopes: 1, deferredBlocks: 2, deferredBytes: 2049, copiedBlocks: 2,
       copiedBytes: 2049, flushes: 1, batchImports: 1, maxBlocksPerFlush: 2}
    : Object.fromEntries(statNames.map(name => [name, 0]));
  assert.deepEqual(after, expected);
  const prefixCopies = batch ? failure.copies : prepare.copies;
  if (batch) assert.equal(prepare.copies.length, 0);
  else assert.equal(failure.copies.length, 0);
  assert.equal(prepare.dropped + failure.dropped, 0);
  const copied = checkPrefixCopies(f, prefixCopies, ranges, original);
  assert(bytesAt(f, f.description.data, f.description.bytes).equals(original));
  return {mode: batch ? 'batch' : 'original-immediate', requestedBytes: size, address,
    heapBytes, beforeState, before, after, marks, exception: errorInfo(failure.error),
    failedInsideTouch: true, touchDidNotReturn: true, acceptedPrefixExact: copied,
    sourceUnchanged: true, noPostFailureWasmCalls: true, instanceDiscarded: true};
}

try {
  const plan = compilerPlan();
  if (options.printPlan) {
    console.log(JSON.stringify({root, output, command: plan.python, argv: plan.argv,
      env: plan.recordedEnv, sdkDefaultMinSafariVersion: plan.sdkDefaultMinSafariVersion,
      differencesFromTitleBuild: receipt.differencesFromTitleBuild}, null, 2));
  } else {
    receipt.sources = sourceHashes();
    receipt.toolchain = {sdk: plan.sdk, emcc: plan.emcc, emccSha256: sha(readFileSync(plan.emcc)),
      sdkDefaultMinSafariVersion: plan.sdkDefaultMinSafariVersion};
    saveReceipt();
    await compile(plan);
    assert.deepEqual(sourceHashes(), receipt.sources, 'Sources changed during compilation');
    const wasm = readFileSync(wasmPath);
    compiledWasm = await WebAssembly.compile(wasm);
    receipt.binary = {wasm: wasmPath, bytes: wasm.length, wasmSha256: sha(wasm),
      loader, loaderSha256: sha(readFileSync(loader)), imports: WebAssembly.Module.imports(compiledWasm),
      exports: WebAssembly.Module.exports(compiledWasm)};
    ({default: factory} = await import(pathToFileURL(loader).href));
    assert.equal(typeof factory, 'function');
    saveReceipt();
    await runCase('included-directed-suite', directedSuite);
    await runCase('pending-memory-growth-and-1023-1024-1025', pendingGrowthAndThresholds);
    await runCase('arena-growth-drain-and-reuse', arenaGrowthAndReuse);
    for (const size of [1, 1024, 1025]) {
      await runCase('out-of-bounds-' + size, async () => {
        const original = await outOfBoundsOne(size, false);
        const batch = await outOfBoundsOne(size, true);
        assert.equal(batch.exception.name, original.exception.name);
        assert.equal(batch.exception.message, original.exception.message);
        return {original, batch, exceptionMatchesOriginal: true};
      });
    }
    assert.deepEqual(sourceHashes(), receipt.sources, 'Sources changed during probe execution');
    receipt.status = 'PASS'; receipt.completedAt = new Date().toISOString();
    receipt.summary = {passedFixtures: receipt.fixtures.length,
      realEmscriptenBatch: true, copiedBytesPreserved: true, freshHeapAfterGrowth: true,
      originalFailurePhasePreserved: true, noPostFailureWasmCalls: true};
    saveReceipt();
    console.log('PASS journal capture batch WASM probe: ' + output);
  }
} catch (error) {
  receipt.status = 'FAIL'; receipt.completedAt = new Date().toISOString();
  receipt.error = errorInfo(error);
  if (receipt.build?.status === 'RUNNING') receipt.build.status = 'FAIL';
  try { saveReceipt(); } catch (writeError) {
    console.error('Could not save probe receipt: ' + String(writeError));
  }
  console.error('FAIL journal capture batch WASM probe: ' + String(error?.message || error));
  console.error('Receipt: ' + output);
  process.exitCode = 1;
}
