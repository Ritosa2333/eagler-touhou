import test from 'node:test';
import assert from 'node:assert/strict';
import { mkdtempSync, readFileSync, writeFileSync, rmSync, existsSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { spawnSync } from 'node:child_process';
import b from 'binaryen';
import { instrumentWasm, verifyInstrumentedWasm, INSTRUMENTATION_MAGIC, INSTRUMENTATION_SECTION, DIRTY_BLOCK_ABI, SCALAR_HOOK_ABI } from './instrument.mjs';

const SCALAR_FUNCTIONS = `
  (func (export "s8") (param i32 i32) (i32.store8 (local.get 0) (local.get 1)))
  (func (export "s16") (param i32 i32) (i32.store16 align=1 (local.get 0) (local.get 1)))
  (func (export "s32") (param i32 i32) (i32.store align=1 (local.get 0) (local.get 1)))
  (func (export "s64") (param i32 i64) (i64.store align=1 (local.get 0) (local.get 1)))
  (func (export "s64n") (param i32 i64) (i64.store32 align=1 (local.get 0) (local.get 1)))
  (func (export "f32") (param i32 f32) (f32.store align=1 (local.get 0) (local.get 1)))
  (func (export "f64") (param i32 f64) (f64.store align=1 (local.get 0) (local.get 1)))
  (func (export "offset") (param i32 i64) (i64.store offset=7 align=1 (local.get 0) (local.get 1)))
`;
const BULK_FUNCTIONS = `
  (func (export "fill") (param i32 i32 i32) (memory.fill (local.get 0) (local.get 1) (local.get 2)))
  (func (export "copy") (param i32 i32 i32) (memory.copy (local.get 0) (local.get 1) (local.get 2)))
  (func (export "init") (param i32 i32 i32) (memory.init $data (local.get 0) (local.get 1) (local.get 2)))
  (func (export "drop") (data.drop $data))
`;

function wasm(text) {
  const module = b.parseText(text);
  try {
    module.setFeatures(b.Features.BulkMemory | b.Features.BulkMemoryOpt |
      b.Features.MutableGlobals | b.Features.SIMD128 | b.Features.Atomics | b.Features.MultiMemory | b.Features.Memory64);
    return module.emitBinary();
  } finally { module.dispose(); }
}

function fixture(functions, { importedMemory = false, exports = {}, data = true, dirty = false,
  dirtyAbi = DIRTY_BLOCK_ABI, dirtyAddress = 84, scalar = false, scalarAbi = SCALAR_HOOK_ABI,
  scalarDefinitions = null } = {}) {
  const names = { touch: 'eagler_rb_touch', marker: 'eagler_rb_instrumented', config: 'eagler_rb_config_address', ...exports };
  return wasm(`(module
    (import "env" "touch" (func $touch (param i32 i32)))
    (import "env" "mark" (func $mark (param i32 i32) (result i32)))
    ${importedMemory ? '(import "env" "memory" (memory 1)) (export "memory" (memory 0))' : '(memory (export "memory") 1)'}
    (export "${names.touch}" (func $touch))
    (func $config (export "${names.config}") (result i32) (i32.const 64))
    (func $marker (export "${names.marker}") (result i32) (i32.load (i32.const 40)))
    (func (export "read_marker_from_wasm") (result i32) (call $marker))
    ${dirty ? `(func (export "eagler_rb_dirty_config_address") (result i32) (i32.const ${dirtyAddress}))
      (func (export "eagler_rb_dirty_config_abi") (result i32) (i32.const ${dirtyAbi}))` : ''}
    ${scalar ? `(global $scalar_width (mut i32) (i32.const 0))
      (func (export "last_scalar_width") (result i32) (global.get $scalar_width))
      (func (export "eagler_rb_scalar_abi") (result i32) (i32.const ${scalarAbi}))
      ${scalarDefinitions ?? [1, 2, 4, 8, 16].map(width => `(func (export "eagler_rb_touch${width}") (param i32)
        (global.set $scalar_width (i32.const ${width}))
        (call $touch (local.get 0) (i32.const ${width})))`).join('\n')}` : ''}
    ${data ? '(data $data "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!?")' : ''}
    ${functions}
  )`);
}

async function running(bytes, { instrumented = true, options = {}, ranges = [[256, 64], [512, 64]] } = {}) {
  const built = instrumented ? await instrumentWasm(bytes, options) : { bytes };
  const trace = [];
  const originals = new Map();
  let memory;
  const imports = { env: {
    memory: new WebAssembly.Memory({ initial: 1 }),
    mark: (event, value) => { trace.push(['mark', event]); return value; },
    touch: (address, size) => {
      address >>>= 0; size >>>= 0;
      trace.push(['touch', address, size]);
      // A small independent journal oracle: save the tracked part of every
      // intersected 32-byte block before the first mutation, then restore it.
      const heap = new Uint8Array(memory.buffer);
      for (const [base, length] of ranges) {
        const low = Math.max(base, address), high = Math.min(base + length, address + size);
        for (let block = low - low % 32; block < high; block += 32) {
          const lo = Math.max(block, base), hi = Math.min(block + 32, base + length);
          if (lo >= hi || originals.has(lo)) continue;
          originals.set(lo, heap.slice(lo, hi));
          if (options.inlineDirtyBlocks && base === ranges[0][0]) {
            const metadata = new Uint32Array(memory.buffer, 84, 3);
            heap[metadata[0] + (block - base) / 32] = metadata[1];
          }
        }
      }
    },
  } };
  const { instance } = await WebAssembly.instantiate(built.bytes, imports);
  memory = instance.exports.memory;
  const heap = new Uint8Array(memory.buffer);
  for (let n = 128; n < 1024; n++) heap[n] = (n * 7 + 13) & 255;
  new Uint32Array(memory.buffer, 64, 5).set([1, ...ranges.flat()]);
  if (options.inlineDirtyBlocks) new Uint32Array(memory.buffer, 84, 3).set([4096, 1, DIRTY_BLOCK_ABI]);
  return {
    instance, f: instance.exports, heap, trace, originals, ...built,
    restore() { for (const [at, old] of originals) heap.set(old, at); },
    clear() { trace.length = 0; originals.clear(); },
    tracking(on) { new Uint32Array(memory.buffer, 64, 1)[0] = Number(on); },
  };
}

function sameTracked(actual, before, ranges = [[256, 64], [512, 64]]) {
  for (const [base, length] of ranges) assert.deepEqual(actual.slice(base, base + length), before.slice(base, base + length));
}

function sameExecutionMemory(actual, baseline) {
  // The instrumentation owns the marker word. Every other byte, including
  // untracked memory and untouched neighboring blocks, must match exactly.
  assert.equal(new DataView(actual.buffer).getUint32(40, true), INSTRUMENTATION_MAGIC);
  assert.equal(new DataView(baseline.buffer).getUint32(40, true), 0);
  assert.deepEqual(actual.slice(0, 40), baseline.slice(0, 40));
  assert.deepEqual(actual.slice(44), baseline.slice(44));
}

function exportedFunctionIndices(bytes) {
  const module = b.readBinary(bytes);
  try {
    const indices = new Map();
    for (let index = 0; index < module.getNumFunctions(); index++)
      indices.set(b.getFunctionInfo(module.getFunctionByIndex(index)).name, index);
    const exports = {};
    for (let index = 0; index < module.getNumExports(); index++) {
      const info = b.getExportInfo(module.getExportByIndex(index));
      if (info.kind === b.ExternalFunction) exports[info.name] = indices.get(info.value);
    }
    return exports;
  } finally { module.dispose(); }
}

test('scalar store widths, integer/floating types and Wasm marker execute correctly', async () => {
  const bytes = fixture(SCALAR_FUNCTIONS);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  for (const [name, address, value] of [
    ['s8', 256, 0x45], ['s16', 257, 0x1234], ['s32', 261, 0x12345678],
    ['s64', 269, 0x123456789abcdefn], ['s64n', 277, 0x123456789abcdefn],
    ['f32', 512, 1.25], ['f64', 519, -Math.PI],
  ]) { a.f[name](address, value); baseline.f[name](address, value); }
  sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace.map(x => x[2]), [1, 2, 4, 8, 4, 4, 8]);
  assert.equal(a.f.eagler_rb_instrumented(), INSTRUMENTATION_MAGIC);
  assert.equal(a.f.read_marker_from_wasm(), INSTRUMENTATION_MAGIC);
  assert.equal(baseline.f.read_marker_from_wasm(), 0);
  a.restore(); sameTracked(a.heap, before);
  const module = new WebAssembly.Module(a.bytes);
  const sections = WebAssembly.Module.customSections(module, INSTRUMENTATION_SECTION);
  assert.equal(sections.length, 1);
  assert.equal(JSON.parse(new TextDecoder().decode(sections[0])).stores, 8);
});

test('inline dirty-block guard skips only already-saved scalar heap blocks', async () => {
  const bytes = fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS + `
    (func (export "vector") (param i32)
      (v128.store align=1 (local.get 0) (v128.const i32x4 1 2 3 4)))`, { dirty: true });
  const a = await running(bytes, { options: { nativeFilter: true, inlineDirtyBlocks: true } });
  const before = a.heap.slice();
  a.f.s32(256, 1); a.f.s64(260, 2n); a.f.f32(268, 3); a.f.f64(272, 4);
  assert.deepEqual(a.trace, [['touch', 256, 4]], 'one native hook for repeated writes to the saved block');
  a.f.s16(287, 5);
  assert.deepEqual(a.trace.at(-1), ['touch', 287, 2], 'a dirty first block never hides a clean second block');
  const count = a.trace.length;
  a.f.vector(280); a.f.s64(304, 6n);
  assert.equal(a.trace.length, count, 'both endpoints of the unaligned SIMD store are saved');
  a.f.s8(512, 7); a.f.s32(516, 8); a.f.s32(900, 9);
  assert.deepEqual(a.trace.slice(-3), [['touch', 512, 1], ['touch', 516, 4], ['touch', 900, 4]], 'root and host writes keep native rejection/clipping');
  a.f.fill(260, 10, 8); a.f.copy(264, 260, 4); a.f.init(268, 0, 4);
  assert.deepEqual(a.trace.slice(-3), [['touch', 260, 8], ['touch', 264, 4], ['touch', 268, 4]], 'bulk writes never bypass the hook');
  a.restore(); sameTracked(a.heap, before);
  assert.equal(a.report.inlineDirtyStores, 9);
  assert.equal(a.report.dirtyBlockAbi, DIRTY_BLOCK_ABI);
  assert.deepEqual(exportedFunctionIndices(a.bytes), exportedFunctionIndices(bytes));
  await verifyInstrumentedWasm(a.bytes, { nativeFilter: true, inlineDirtyBlocks: true });
});

test('inline dirty-block guard preserves partial overlaps, inactive scopes and offset traps', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS, { dirty: true }),
    { options: { nativeFilter: true, inlineDirtyBlocks: true } });
  const before = a.heap.slice();
  for (const address of [254, 318, 510, 574]) a.f.s32(address, 0x12345678);
  assert.deepEqual(a.trace.map(row => row[1]), [254, 318, 510, 574]);
  a.restore(); sameTracked(a.heap, before);
  a.clear(); a.tracking(false);
  // The metadata address itself is valid, but its tag pointer deliberately is
  // not: an inactive HostScope must short-circuit before accessing any tags.
  new Uint32Array(a.heap.buffer, 84, 1)[0] = 0xffffffff;
  a.f.s32(260, 12); assert.deepEqual(a.trace, []);
  a.tracking(true);
  assert.throws(() => a.f.offset(0xfffffffc, 1n), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, [], 'overflowing memarg offsets do not wrap into the heap');
});

test('inline dirty-block guard follows new generations after restore and byte-tag wrap', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS, { dirty: true }),
    { options: { nativeFilter: true, inlineDirtyBlocks: true } });
  const before = a.heap.slice();
  const metadata = new Uint32Array(a.heap.buffer, 84, 3);
  for (let frame = 0; frame < 260; frame++) {
    const generation = frame % 255 + 1;
    if (generation === 1) a.heap.fill(0, metadata[0], metadata[0] + 2);
    metadata[1] = generation; a.clear();
    a.f.s32(260, frame + 1); a.f.s32(264, frame + 2);
    assert.deepEqual(a.trace, [['touch', 260, 4]], `exactly one first-write hook in generation ${generation}`);
    a.restore(); sameTracked(a.heap, before);
  }
});

test('inline guard rejects host writes in a heap-only backend but retains both boundary overlaps', async () => {
  const ranges = [[256, 64], [0, 0]];
  const a = await running(fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS, { dirty: true }),
    { options: { nativeFilter: true, inlineDirtyBlocks: true }, ranges });
  const before = a.heap.slice();
  for (const address of [128, 252, 320, 512, 900]) a.f.s32(address, 1234);
  assert.deepEqual(a.trace, [], 'non-overlapping scalar host writes are rejected without the hook');
  a.f.s32(254, 1234); a.f.s64(318, 1234n);
  assert.deepEqual(a.trace, [['touch', 254, 4], ['touch', 318, 8]], 'both partial heap edges reach clipping');
  a.f.fill(900, 0, 4);
  assert.deepEqual(a.trace.at(-1), ['touch', 900, 4], 'bulk rejection remains native');
  a.restore(); sameTracked(a.heap, before, ranges);
});

test('inline dirty-block guard requires an explicit compatible and disjoint backend ABI', async () => {
  const options = { nativeFilter: true, inlineDirtyBlocks: true };
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS), options), /missing export eagler_rb_dirty_config_address/);
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true, dirtyAbi: 1 }), options), /incompatible dirty-block ABI/);
  for (const dirtyAddress of [0, 85, 65532])
    await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true, dirtyAddress }), options), /aligned, static three-word/);
  for (const dirtyAddress of [60, 64, 80])
    await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true, dirtyAddress }), options), /overlap tracked-range/);
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true, dirtyAddress: 32 }), options), /overlap marker/);
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true }), { inlineDirtyBlocks: true }), /requires native-filter/);
  const baseline = await instrumentWasm(fixture(SCALAR_FUNCTIONS, { dirty: true }), { nativeFilter: true });
  await assert.rejects(verifyInstrumentedWasm(baseline.bytes, options), /report missing or incompatible/);
});

test('inline dirty guards retain nested operand order and recursive call-local state', async () => {
  const bytes = fixture(`
    (func $recurse (export "recurse") (param i32) (result i32)
      (if (result i32) (local.get 0)
        (then
          (i32.store (i32.add (i32.const 256) (i32.mul (local.get 0) (i32.const 4)))
            (call $recurse (i32.sub (local.get 0) (i32.const 1))))
          (i32.add (local.get 0) (i32.const 100)))
        (else (i32.const 99))))
    (func (export "nested")
      (i32.store (call $mark (i32.const 1) (i32.const 284))
        (block (result i32)
          (i64.store (call $mark (i32.const 2) (i32.const 286)) (i64.const 1234))
          (call $mark (i32.const 3) (i32.const 5678))))
      (i32.store (i32.const 512) (i32.load (i32.const 284))))`, { dirty: true });
  const options = { nativeFilter: true, inlineDirtyBlocks: true };
  const a = await running(bytes, { options }), baseline = await running(bytes, { instrumented: false, options });
  const before = a.heap.slice();
  assert.equal(a.f.recurse(4), baseline.f.recurse(4));
  a.f.nested(); baseline.f.nested();
  assert.deepEqual(a.heap.slice(128, 1024), baseline.heap.slice(128, 1024));
  assert.deepEqual(a.trace, [['touch', 260, 4], ['mark', 1], ['mark', 2],
    ['touch', 286, 8], ['mark', 3], ['touch', 512, 4]]);
  a.restore(); sameTracked(a.heap, before);
});

test('native-filter delegates clipping while preserving tracking, bulk writes and traps', async () => {
  const bytes = fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS);
  const a = await running(bytes, { options: { nativeFilter: true } });
  const baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  for (const [name, args] of [['s32', [254, 0x12345678]], ['s64', [316, 0x123456789abcdefn]],
    ['s32', [900, 0x332211]], ['fill', [510, 0x47, 8]], ['init', [300, 1, 4]], ['copy', [570, 260, 8]]]) {
    a.f[name](...args); baseline.f[name](...args);
  }
  sameExecutionMemory(a.heap, baseline.heap);
  assert(a.trace.some(row => row[1] === 900), 'the native hook owns range rejection');
  a.restore(); sameTracked(a.heap, before);
  a.clear(); a.tracking(false); a.f.s32(260, 42); assert.deepEqual(a.trace, []);
  a.tracking(true); a.f.fill(256, 0, 0); assert.deepEqual(a.trace, []);
  assert.throws(() => a.f.offset(0xfffffffc, 1n), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, [], 'overflowing offsets never wrap into a native hook');
  assert.equal(a.report.rangeFilter, 'native-hook');
  assert.deepEqual(exportedFunctionIndices(a.bytes), exportedFunctionIndices(bytes));
  await assert.rejects(instrumentWasm(bytes, { nativeFilter: true, config: null }), /requires a configuration/);
});

test('fixed-width hooks preserve scalar widths, clipping, bulk writes and trapping offsets', async () => {
  const input = fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS + `
    (func (export "vector") (param i32)
      (v128.store align=1 (local.get 0) (v128.const i32x4 1 2 3 4)))`, { scalar: true });
  const options = { nativeFilter: true, scalarHooks: true };
  const a = await running(input, { options }), baseline = await running(input, { instrumented: false });
  const before = a.heap.slice();
  for (const [name, args, width] of [
    ['s8', [255, 0x7f], 1], ['s16', [287, 0x1234], 2], ['s32', [254, 0x12345678], 4],
    ['s64', [316, 0x123456789abcdefn], 8], ['s64n', [520, 0x123456789abcdefn], 4],
    ['f32', [260, 1.25], 4], ['f64', [566, 3.5], 8], ['vector', [280], 16], ['s32', [900, 42], 4],
  ]) {
    a.f[name](...args); baseline.f[name](...args);
    assert.equal(a.f.last_scalar_width(), width);
    assert.deepEqual(a.trace.at(-1), ['touch', args[0], width]);
  }
  for (const [name, args] of [['fill', [510, 0x47, 8]], ['init', [300, 1, 4]], ['copy', [570, 260, 8]]]) {
    a.f[name](...args); baseline.f[name](...args);
    assert.equal(a.f.last_scalar_width(), 4, 'bulk writes retain the generic two-argument hook');
  }
  sameExecutionMemory(a.heap, baseline.heap);
  a.restore(); sameTracked(a.heap, before);
  a.clear(); a.tracking(false); a.f.s32(260, 42); assert.deepEqual(a.trace, []);
  a.tracking(true); a.f.fill(256, 0, 0); assert.deepEqual(a.trace, []);
  assert.throws(() => a.f.offset(0xfffffffc, 1n), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, [], 'overflowing effective addresses do not reach a specialized hook');
  assert.equal(a.report.scalarHookStores, a.report.stores);
  assert.deepEqual(Object.keys(a.report.scalarHookWidths), ['1', '2', '4', '8', '16']);
  assert(Object.values(a.report.scalarHookWidths).every(count => count > 0));
  assert.deepEqual(exportedFunctionIndices(a.bytes), exportedFunctionIndices(input));
  assert.equal((await verifyInstrumentedWasm(a.bytes)).scalarHooks, true, 'bare verification discovers the profile');
});

test('fixed-width hooks preserve nested operand order and exclude every specialized hook body', async () => {
  const hooks = [1, 2, 4, 8, 16].map(width => `(func (export "eagler_rb_touch${width}") (param i32)
    (i32.store (i32.const 112) (i32.const ${width}))
    (call $touch (local.get 0) (i32.const ${width})))`).join('\n');
  const input = fixture(SCALAR_FUNCTIONS + `
    (func $recurse (export "recurse") (param i32) (result i32)
      (if (result i32) (local.get 0)
        (then (i32.store (i32.add (i32.const 256) (i32.mul (local.get 0) (i32.const 4)))
          (call $recurse (i32.sub (local.get 0) (i32.const 1))))
          (i32.add (local.get 0) (i32.const 100)))
        (else (i32.const 99))))
    (func (export "nested")
      (i32.store (call $mark (i32.const 1) (i32.const 284))
        (block (result i32)
          (i64.store (call $mark (i32.const 2) (i32.const 286)) (i64.const 1234))
          (call $mark (i32.const 3) (i32.const 5678)))))
    (func (export "vector") (param i32)
      (v128.store align=1 (local.get 0) (v128.const i32x4 1 2 3 4)))`,
    { scalar: true, scalarDefinitions: hooks });
  const options = { nativeFilter: true, scalarHooks: true };
  const a = await running(input, { options }), baseline = await running(input, { instrumented: false });
  const before = a.heap.slice();
  for (const [name, args, width] of [['s8', [260, 1], 1], ['s16', [260, 2], 2], ['s32', [260, 4], 4],
    ['s64', [260, 8n], 8], ['vector', [280], 16]]) {
    a.clear(); a.f[name](...args);
    assert.deepEqual(a.trace, [['touch', args[0], width]], 'hook-internal stores cannot recursively instrument themselves');
    assert.equal(new DataView(a.heap.buffer).getUint32(112, true), width);
    a.restore();
  }
  a.clear(); assert.equal(a.f.recurse(4), baseline.f.recurse(4));
  assert.deepEqual(a.trace, [260, 264, 268, 272].map(address => ['touch', address, 4]));
  a.trace.length = 0; a.f.nested(); baseline.f.nested();
  assert.deepEqual(a.trace, [['mark', 1], ['mark', 2], ['touch', 286, 8], ['mark', 3], ['touch', 284, 4]]);
  assert.deepEqual(a.heap.slice(128, 1024), baseline.heap.slice(128, 1024));
  a.restore(); sameTracked(a.heap, before);
});

test('fixed-width hooks reject missing, incompatible, aliased and incorrectly typed native exports', async () => {
  const options = { nativeFilter: true, scalarHooks: true };
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS), options), /missing export eagler_rb_scalar_abi/);
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { scalar: true, scalarAbi: 2 }), options), /incompatible scalar-hook ABI/);
  const input = fixture(SCALAR_FUNCTIONS, { scalar: true });
  await assert.rejects(instrumentWasm(input, { scalarHooks: true }), /requires native-filter/);
  await assert.rejects(instrumentWasm(input, { ...options, config: null }), /requires a configuration/);
  await assert.rejects(instrumentWasm(input, { ...options, inlineDirtyBlocks: true }), /separate guard profiles/);
  await assert.rejects(instrumentWasm(input, { ...options, scalarTouch8: 'eagler_rb_touch4' }), /distinct backend functions/);
  const badTypes = [1, 2, 4, 8, 16].map(width => `(func (export "eagler_rb_touch${width}")
    (param ${width === 4 ? 'i64' : 'i32'}) (drop (local.get 0)))`).join('\n');
  await assert.rejects(instrumentWasm(fixture(SCALAR_FUNCTIONS, { scalar: true, scalarDefinitions: badTypes }), options), /signature/);
  const generic = await instrumentWasm(input, { nativeFilter: true });
  await assert.rejects(verifyInstrumentedWasm(generic.bytes, options), /report missing or incompatible/);
  const built = await instrumentWasm(input, options);
  await assert.rejects(verifyInstrumentedWasm(built.bytes, { config: null }), /requires a configuration export/);
});

test('every advertised scalar hook stays excluded in existing guard profiles', async () => {
  const hooks = [1, 2, 4, 8, 16].map(width => `(func (export "eagler_rb_touch${width}") (param i32)
    (i32.store (i32.const 112) (i32.const ${width}))
    (call $touch (local.get 0) (i32.const ${width})))`).join('\n');
  const input = fixture(SCALAR_FUNCTIONS, { scalar: true, dirty: true, scalarDefinitions: hooks });
  for (const options of [{}, { nativeFilter: true }, { nativeFilter: true, inlineDirtyBlocks: true }]) {
    const a = await running(input, { options });
    assert.equal(a.report.stores, 8, 'backend stores are not part of the game instrumentation count');
    for (const width of [1, 2, 4, 8, 16]) {
      a.clear(); a.f[`eagler_rb_touch${width}`](260);
      assert.deepEqual(a.trace, [['touch', 260, width]]);
      assert.equal(new DataView(a.heap.buffer).getUint32(112, true), width);
    }
  }
});

test('a scalar profile with only bulk writes retains generic hooks and verifies zero scalar stores', async () => {
  const options = { nativeFilter: true, scalarHooks: true };
  const a = await running(fixture(BULK_FUNCTIONS, { scalar: true }), { options });
  const before = a.heap.slice();
  a.f.fill(254, 23, 8); a.f.copy(510, 260, 8); a.f.init(570, 1, 8);
  assert.deepEqual(a.trace, [['touch', 254, 8], ['touch', 510, 8], ['touch', 570, 8]]);
  assert.equal(a.f.last_scalar_width(), 0);
  assert.equal(a.report.scalarHookStores, 0);
  assert.equal(a.report.stores, 0);
  assert.equal((await verifyInstrumentedWasm(a.bytes, options)).scalarHooks, true);
  a.restore(); sameTracked(a.heap, before);
});

test('SIMD v128 whole stores are journaled with all 16 bytes', async () => {
  const bytes = fixture(`(func (export "vector") (param i32)
    (v128.store align=1 (local.get 0) (v128.const i32x4 1 2 3 4)))`);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  a.f.vector(280); baseline.f.vector(280);
  sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace, [['touch', 280, 16]]);
  assert.deepEqual([...a.originals.keys()], [256, 288]);
  a.restore(); sameTracked(a.heap, before);
});

test('marker word survives inlined loads with and without existing data sections', async () => {
  for (const data of [false, true]) {
    const input = fixture(SCALAR_FUNCTIONS + `
      (func (export "inlined_marker") (result i32) (i32.load (i32.const 40)))`, { data });
    const a = await running(input), baseline = await running(input, { instrumented: false });
    assert.equal(baseline.f.inlined_marker(), 0);
    assert.equal(a.f.inlined_marker(), INSTRUMENTATION_MAGIC);
    assert.equal(a.f.eagler_rb_instrumented(), INSTRUMENTATION_MAGIC);
    assert.equal(a.report.markerAddress, 40);
    sameExecutionMemory(a.heap, baseline.heap);
  }
});

test('childless unreachable, nop and memory.size preserve control flow and traps', async () => {
  const input = fixture(`(func (export "probe") (param i32) (result i32)
    (nop)
    (if (local.get 0) (then (unreachable)))
    (i32.store (i32.const 260) (memory.size))
    (memory.size))`);
  const a = await running(input), baseline = await running(input, { instrumented: false });
  assert.equal(a.f.probe(0), 1); assert.equal(baseline.f.probe(0), 1);
  sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace, [['touch', 260, 4]]);
  a.clear(); const before = a.heap.slice();
  assert.throws(() => a.f.probe(1), WebAssembly.RuntimeError);
  assert.throws(() => baseline.f.probe(1), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, []); assert.deepEqual(a.heap, before);
});

test('function indices, exports, table targets and start survive local reconstruction', async () => {
  // Every original function is exported so the complete function-index map is
  // observable independently of Binaryen's regenerated internal names. Hooks,
  // non-writing helpers and writing functions deliberately alternate positions.
  const input = wasm(`(module
    (type $operation (func (param i32) (result i32)))
    (import "env" "touch" (func $notify (param i32 i32)))
    (import "env" "mark" (func $mark (param i32 i32) (result i32)))
    (export "notify" (func $notify)) (export "mark" (func $mark))
    (memory (export "memory") 1)
    (table (export "table") 3 funcref)
    (elem (i32.const 0) $first $pure $second)
    (func $first (export "first") (param i32) (result i32)
      (i32.store (i32.const 260) (local.get 0)) (i32.add (local.get 0) (i32.const 1)))
    (func $marker (export "eagler_rb_instrumented") (result i32) (i32.load (i32.const 40)))
    (func $pure (export "pure") (param i32) (result i32) (i32.mul (local.get 0) (i32.const 3)))
    (func $touch (export "eagler_rb_touch") (param i32 i32) (call $notify (local.get 0) (local.get 1)))
    (func $second (export "second") (param i32) (result i32)
      (i32.store (i32.const 516) (local.get 0)) (i32.add (local.get 0) (i32.const 2)))
    (func $config (export "eagler_rb_config_address") (result i32) (i32.const 64))
    (func $startup (export "startup") (i32.store (i32.const 96) (i32.const 12345)))
    (start $startup)
    (func $dispatch (export "indirect") (param i32 i32) (result i32)
      (call_indirect (type $operation) (local.get 0) (local.get 1))))`);
  const expected = exportedFunctionIndices(input);
  assert.deepEqual(Object.values(expected).sort((a, b) => a - b), [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
  const a = await running(input), baseline = await running(input, { instrumented: false });
  assert.deepEqual(exportedFunctionIndices(a.bytes), expected, 'external index symbol map stays exact');
  assert.equal(expected.eagler_rb_instrumented, 3);
  assert.equal(new DataView(a.heap.buffer).getUint32(96, true), 12345, 'original start function executes');
  assert.equal(a.f.table.get(0), a.f.first);
  assert.equal(a.f.table.get(1), a.f.pure);
  assert.equal(a.f.table.get(2), a.f.second);
  const before = a.heap.slice();
  for (const [slot, result] of [[0, 8], [1, 21], [2, 9]]) {
    assert.equal(a.f.indirect(7, slot), result);
    assert.equal(baseline.f.indirect(7, slot), result);
  }
  assert.deepEqual(a.trace, [['touch', 260, 4], ['touch', 516, 4]]);
  sameExecutionMemory(a.heap, baseline.heap);
  a.restore(); sameTracked(a.heap, before);
  assert.throws(() => a.f.indirect(7, 3), WebAssembly.RuntimeError);
});

test('tracking flag and both state spans filter writes without changing execution', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS));
  a.f.s32(128, 7);
  a.f.s32(576, 9);
  assert.deepEqual(a.trace, []);
  a.tracking(false); a.f.s32(264, 8); assert.deepEqual(a.trace, []);
  assert.equal(new DataView(a.heap.buffer).getUint32(264, true), 8);
  a.tracking(true); a.f.s32(264, 9); a.f.s32(520, 10);
  assert.deepEqual(a.trace, [['touch', 264, 4], ['touch', 520, 4]]);
});

test('writes starting outside a span journal every overlapping byte and block', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS));
  for (const address of [248, 249, 252, 255, 256, 280, 284, 287, 288, 312, 316, 319, 320]) {
    a.clear();
    const before = a.heap.slice();
    a.f.s64(address, 0x123456789abcdefn);
    const overlap = address < 320 && address + 8 > 256;
    assert.equal(a.trace.length, Number(overlap), `address ${address}`);
    a.restore(); sameTracked(a.heap, before);
  }
  for (const [address, length] of [[240, 64], [244, 13], [300, 40], [504, 9], [568, 16], [480, 128]]) {
    a.clear(); const before = a.heap.slice();
    a.f.fill(address, 0xab, length);
    assert.deepEqual(a.trace, [['touch', address, length]]);
    a.restore(); sameTracked(a.heap, before);
  }
});

test('zero-length and empty tracked spans do not call the journal', async () => {
  const a = await running(fixture(BULK_FUNCTIONS));
  a.f.fill(256, 7, 0); a.f.copy(512, 256, 0); a.f.init(256, 64, 0);
  a.f.fill(65536, 0, 0); a.f.copy(65536, 65536, 0); a.f.init(65536, 64, 0);
  assert.deepEqual(a.trace, []);
  new Uint32Array(a.heap.buffer, 64, 5).set([1, 256, 0, 512, 0]);
  a.f.fill(255, 0, 30); assert.deepEqual(a.trace, []);
});

test('copy, fill, passive memory.init and overlapping copy match original Wasm', async () => {
  const bytes = fixture(BULK_FUNCTIONS);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  for (const [name, args] of [
    ['fill', [251, 0xaa, 16]], ['init', [275, 5, 24]],
    ['copy', [287, 275, 20]], ['copy', [512, 288, 30]], ['init', [508, 0, 16]],
  ]) { a.f[name](...args); baseline.f[name](...args); }
  sameExecutionMemory(a.heap, baseline.heap);
  assert.equal(a.report.copies, 1); assert.equal(a.report.fills, 1); assert.equal(a.report.inits, 1);
  a.restore(); sameTracked(a.heap, before);
  a.f.drop(); baseline.f.drop();
  assert.throws(() => a.f.init(256, 0, 1), WebAssembly.RuntimeError);
  assert.throws(() => baseline.f.init(256, 0, 1), WebAssembly.RuntimeError);
});

test('nested scalar store evaluates original pointer and value before its hook', async () => {
  const bytes = fixture(`
    (func (export "nested")
      (i32.store
        (call $mark (i32.const 1) (i32.const 260))
        (block (result i32)
          (i32.store
            (call $mark (i32.const 2) (i32.const 520))
            (call $mark (i32.const 3) (i32.const 1234)))
          (call $mark (i32.const 4) (i32.const 5678)))))`);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  a.f.nested(); baseline.f.nested();
  sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace, [['mark', 1], ['mark', 2], ['mark', 3], ['touch', 520, 4], ['mark', 4], ['touch', 260, 4]]);
  assert.equal(a.report.maxWriteDepth, 2);
});

test('nested bulk writes in source and length operands keep the outer destination', async () => {
  const bytes = fixture(`
    (func (export "nested")
      (memory.copy
        (call $mark (i32.const 1) (i32.const 260))
        (block (result i32)
          (memory.fill (call $mark (i32.const 2) (i32.const 516)) (i32.const 170) (i32.const 4))
          (call $mark (i32.const 3) (i32.const 520)))
        (block (result i32)
          (memory.init $data (call $mark (i32.const 4) (i32.const 532)) (i32.const 0) (i32.const 4))
          (call $mark (i32.const 5) (i32.const 4)))))`);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  a.f.nested(); baseline.f.nested();
  sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace, [['mark', 1], ['mark', 2], ['touch', 516, 4], ['mark', 3], ['mark', 4], ['touch', 532, 4], ['mark', 5], ['touch', 260, 4]]);
  a.restore(); sameTracked(a.heap, before);
});

test('recursive calls and mixed scalar/bulk operand nesting keep independent locals', async () => {
  const bytes = fixture(`
    (func $recursive (export "recursive") (param $n i32) (result i32)
      (if (result i32) (local.get $n)
        (then
          (i32.store (i32.add (i32.const 256) (i32.mul (local.get $n) (i32.const 4)))
            (call $recursive (i32.sub (local.get $n) (i32.const 1))))
          (i32.add (local.get $n) (i32.const 100)))
        (else (i32.const 99))))
    (func (export "mixed")
      (memory.fill (i32.const 512)
        (block (result i32)
          (i64.store (i32.const 280) (i64.const 123456))
          (i32.store (i32.const 288) (i32.const 777))
          (i32.const 24))
        (block (result i32)
          (i32.store (i32.const 292) (i32.const 888))
          (i32.const 8))))`);
  const a = await running(bytes), baseline = await running(bytes, { instrumented: false });
  const before = a.heap.slice();
  assert.equal(a.f.recursive(4), baseline.f.recursive(4));
  a.f.mixed(); baseline.f.mixed(); sameExecutionMemory(a.heap, baseline.heap);
  assert.deepEqual(a.trace.slice(-4), [['touch', 280, 8], ['touch', 288, 4], ['touch', 292, 4], ['touch', 512, 8]]);
  a.restore(); sameTracked(a.heap, before);
});

test('unaligned memarg offsets cover crossing blocks and retain overflow traps', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS));
  const before = a.heap.slice();
  a.f.offset(277, 123456789n);
  assert.deepEqual(a.trace, [['touch', 284, 8]]);
  assert.deepEqual([...a.originals.keys()], [256, 288]);
  a.restore(); sameTracked(a.heap, before); a.clear();
  assert.throws(() => a.f.offset(0xffffffff, 2n), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, []);
  assert.deepEqual(a.heap, before);
});

test('trapping operands do not reach an outer hook or change its target', async () => {
  const a = await running(fixture(`(func (export "trap")
    (i32.store (i32.const 260) (i32.div_s (i32.const 1) (i32.const 0))))`));
  const before = a.heap.slice();
  assert.throws(() => a.f.trap(), WebAssembly.RuntimeError);
  assert.deepEqual(a.trace, []); assert.deepEqual(a.heap, before);
});

test('imported memory and custom hook names use the same verified contract', async () => {
  const names = { touch: 'touch_custom', marker: 'marker_custom', config: 'config_custom' };
  const a = await running(fixture(SCALAR_FUNCTIONS, { importedMemory: true, exports: names }), { options: names });
  a.f.s32(260, 77);
  assert.deepEqual(a.trace, [['touch', 260, 4]]);
  assert.equal(a.f.marker_custom(), INSTRUMENTATION_MAGIC);
  assert.equal((await verifyInstrumentedWasm(a.bytes, names)).magic, INSTRUMENTATION_MAGIC);
});

test('no-config mode calls a hook for every nonempty write and preserves nested operands', async () => {
  const a = await running(fixture(SCALAR_FUNCTIONS + BULK_FUNCTIONS), { options: { config: null } });
  a.f.s32(128, 33); a.f.fill(700, 4, 4); a.f.fill(700, 4, 0);
  assert.deepEqual(a.trace, [['touch', 128, 4], ['touch', 700, 4]]);
  assert.equal(a.report.configAddress, null);
});

test('missing markers, duplicate instrumentation and uninstrumented binaries are rejected', async () => {
  const input = fixture(SCALAR_FUNCTIONS);
  await assert.rejects(verifyInstrumentedWasm(input), /marker missing or incorrect/);
  const once = await instrumentWasm(input);
  await assert.rejects(instrumentWasm(once.bytes), /already instrumented/);
  await assert.rejects(instrumentWasm(input, { marker: 'absent' }), /missing export/);
  await assert.rejects(instrumentWasm(fixture('(func (export "read") (result i32) (i32.const 1))')), /no memory writes/);
});

test('memory64, shared/atomic memory, multiple memories and SIMD lanes fail closed', async () => {
  const rejected = [
    ['(module (memory i64 1))', /memory64/],
    ['(module (memory 1 2 shared))', /shared\/atomic/],
    ['(module (memory 1) (memory 1))', /exactly one/],
  ];
  for (const [text, pattern] of rejected) await assert.rejects(instrumentWasm(wasm(text)), pattern);
  const lane = fixture(`(func (export "lane") (v128.store8_lane 0 (i32.const 256) (v128.const i32x4 1 2 3 4)))`);
  await assert.rejects(instrumentWasm(lane), /SIMD lane memory/);
});

test('configuration must be an aligned constant address, not a dynamic calculation', async () => {
  for (const [body, reason] of [
    ['(i32.const 65)', /aligned/],
    ['(i32.const 65524)', /within initial memory/],
    ['(i32.load (i32.const 48))', /static/],
  ]) {
    const source = `(module
      (memory 1)
      (func (export "eagler_rb_touch") (param i32 i32))
      (func (export "eagler_rb_instrumented") (result i32) (i32.const 0))
      (func (export "eagler_rb_config_address") (result i32) ${body})
      (func (i32.store (i32.const 256) (i32.const 1))))`;
    await assert.rejects(instrumentWasm(wasm(source)), reason);
  }
});

test('marker placeholder must expose an independent static volatile word', async () => {
  for (const [body, reason] of [
    ['(i32.const 0)', /load a static volatile/],
    ['(i32.load (i32.load (i32.const 40)))', /aligned static word/],
    ['(i32.load (i32.const 41))', /aligned static word/],
    ['(i32.load (i32.const 65536))', /within initial memory/],
    ['(i32.load (i32.const 68))', /overlap configuration/],
  ]) {
    const source = `(module
      (memory 1)
      (func (export "eagler_rb_touch") (param i32 i32))
      (func (export "eagler_rb_instrumented") (result i32) ${body})
      (func (export "eagler_rb_config_address") (result i32) (i32.const 64))
      (func (i32.store (i32.const 256) (i32.const 1))))`;
    await assert.rejects(instrumentWasm(wasm(source)), reason);
  }
});

test('CLI atomically writes a verified artifact and rejects in-place/unmarked input', () => {
  const directory = mkdtempSync(join(tmpdir(), 'rollback-instrument-test-'));
  try {
    const input = join(directory, 'input.wasm'), output = join(directory, 'output.wasm');
    writeFileSync(input, fixture(SCALAR_FUNCTIONS));
    const cli = fileURLToPath(new URL('./instrument.mjs', import.meta.url));
    const run = args => spawnSync(process.execPath, [cli, ...args], { encoding: 'utf8' });
    const result = run([input, output]);
    assert.equal(result.status, 0, result.stderr);
    assert.equal(JSON.parse(result.stdout).stores, 8);
    assert.equal(WebAssembly.validate(readFileSync(output)), true);
    assert.equal(run(['--verify', output]).status, 0);
    assert.notEqual(run(['--verify', input]).status, 0);
    assert.notEqual(run([input, input]).status, 0);
    const missing = join(directory, 'must-not-exist.wasm');
    assert.notEqual(run([input, missing, '--marker=missing']).status, 0);
    assert.equal(existsSync(missing), false);
  } finally { rmSync(directory, { recursive: true, force: true }); }
});
