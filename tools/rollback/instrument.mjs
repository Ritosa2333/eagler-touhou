#!/usr/bin/env node
// GPL reference: apeirs/th11, 324fb143e78560ae34f5930a19048e34035eb2f8,
// portable/rollback/instrument-rollback.mjs. This implementation preserves the
// original operand order, journals bulk initialization, and checks range overlap.
import binaryen from 'binaryen';
import { readFileSync, writeFileSync, renameSync, rmSync } from 'node:fs';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

export const INSTRUMENTATION_MAGIC = 0x52424a31;
export const INSTRUMENTATION_SECTION = 'eagler.write-journal.v1';
export const DEFAULT_EXPORTS = Object.freeze({
  touch: 'eagler_rb_touch',
  config: 'eagler_rb_config_address',
  marker: 'eagler_rb_instrumented',
});

const b = binaryen;
// Emscripten can omit target_features. Enable its scalar instruction baseline;
// additionally supported features are enabled when encountered in the tree.
const BASE_FEATURES = b.Features.MutableGlobals | b.Features.NontrappingFPToInt |
  b.Features.BulkMemory | b.Features.BulkMemoryOpt | b.Features.SignExt;
const ALLOWED_FEATURES = BASE_FEATURES | b.Features.SIMD128 |
  b.Features.ReferenceTypes | b.Features.Multivalue | b.Features.TailCall;

const CHILD_FIELDS = new Map([
  ['Block', ['children']], ['If', ['condition', 'ifTrue', 'ifFalse']], ['Loop', ['body']],
  ['Break', ['condition', 'value']], ['Switch', ['condition', 'value']],
  ['Call', ['operands']], ['CallIndirect', ['target', 'operands']],
  ['LocalGet', []], ['LocalSet', ['value']], ['GlobalGet', []], ['GlobalSet', ['value']],
  ['Load', ['ptr']], ['Store', ['ptr', 'value']], ['Const', []], ['Unary', ['value']],
  ['Binary', ['left', 'right']], ['Select', ['ifTrue', 'ifFalse', 'condition']],
  ['Drop', ['value']], ['Return', ['value']], ['MemorySize', []], ['MemoryGrow', ['delta']],
  ['Nop', []], ['Unreachable', []], ['MemoryCopy', ['dest', 'source', 'size']],
  ['MemoryFill', ['dest', 'value', 'size']], ['MemoryInit', ['dest', 'offset', 'size']],
  ['DataDrop', []], ['SIMDExtract', ['vec']], ['SIMDReplace', ['vec', 'value']],
  ['SIMDShuffle', ['left', 'right']], ['SIMDTernary', ['a', 'b', 'c']],
  ['SIMDShift', ['vec', 'shift']], ['SIMDLoad', ['ptr']],
  ['RefNull', []], ['RefIsNull', ['value']], ['RefFunc', []], ['RefEq', ['left', 'right']],
  ['TableGet', ['index']], ['TableSet', ['index', 'value']], ['TableSize', []],
  ['TableGrow', ['value', 'delta']], ['TupleMake', ['operands']], ['TupleExtract', ['tuple']],
].filter(([name]) => b[`${name}Id`] !== undefined).map(([name, fields]) => [b[`${name}Id`], { name, fields }]));

const REJECTED = new Map([
  ['AtomicRMW', 'atomic memory operations'], ['AtomicCmpxchg', 'atomic memory operations'],
  ['AtomicWait', 'atomic memory operations'], ['AtomicNotify', 'atomic memory operations'],
  ['AtomicFence', 'atomic memory operations'], ['SIMDLoadStoreLane', 'SIMD lane memory operations'],
].filter(([name]) => b[`${name}Id`] !== undefined).map(([name, reason]) => [b[`${name}Id`], reason]));

function fail(message) { throw new Error(`rollback instrumentation: ${message}`); }

// Binaryen's JS API does not expose the number of memories. Read just the
// memory declarations first so a second, unexported memory cannot evade checks.
function inspectMemory(bytes) {
  const data = new Uint8Array(bytes);
  if (data.length < 8 || [...data.slice(0, 8)].join(',') !== '0,97,115,109,1,0,0,0') fail('expected a WebAssembly 1 binary');
  let at = 8;
  let end = data.length;
  const byte = () => { if (at >= end) fail('truncated WebAssembly section'); return data[at++]; };
  const u32 = () => {
    let value = 0;
    for (let n = 0; n < 5; n++) {
      const v = byte();
      if (n === 4 && (v & 0xf0)) fail('invalid u32 LEB128');
      value += (v & 0x7f) * 2 ** (7 * n);
      if (!(v & 0x80)) return value;
    }
    fail('invalid u32 LEB128');
  };
  const string = () => { const length = u32(); if (length > end - at) fail('truncated WebAssembly name'); at += length; };
  const valueType = () => {
    const type = byte();
    if (type === 0x63 || type === 0x64) {
      // Signed heap-type LEB128; its value is not needed for memory inspection.
      for (let n = 0; n < 5; n++) { if (!(byte() & 0x80)) return; }
      fail('invalid reference heap type');
    }
    if (![0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x70, 0x6f].includes(type)) fail(`unsupported import value type 0x${type.toString(16)}`);
  };
  const memories = [];
  const limits = memory => {
    const flags = u32();
    if (flags & 4) fail('memory64/table64 is not supported');
    if (flags & 2) fail('shared/atomic memory is not supported');
    if (flags & ~1) fail('unsupported memory/table limits');
    const initial = u32();
    const maximum = flags & 1 ? u32() : undefined;
    if (memory) memories.push({ initial, maximum });
  };
  while (at < data.length) {
    end = data.length;
    const section = byte();
    const length = u32();
    if (length > data.length - at) fail('truncated WebAssembly section payload');
    end = at + length;
    if (section === 2) {
      const count = u32();
      for (let n = 0; n < count; n++) {
        string(); string();
        switch (byte()) {
          case 0: u32(); break;
          case 1: valueType(); limits(false); break;
          case 2: limits(true); break;
          case 3: valueType(); byte(); break;
          case 4: byte(); u32(); break;
          default: fail('unsupported WebAssembly import kind');
        }
      }
      if (at !== end) fail('unexpected bytes in import section');
    } else if (section === 5) {
      const count = u32();
      for (let n = 0; n < count; n++) limits(true);
      if (at !== end) fail('unexpected bytes in memory section');
    }
    at = end;
  }
  if (memories.length !== 1) fail(`exactly one wasm32 memory is required (found ${memories.length})`);
  return memories[0];
}

function exportedFunction(module, name, params, result, imported = false) {
  const ref = module.getExport(name);
  if (!ref) fail(`missing export ${name}`);
  const ex = b.getExportInfo(ref);
  if (ex.kind !== b.ExternalFunction) fail(`${name} must export a function`);
  const fn = module.getFunction(ex.value);
  if (!fn) fail(`missing function for ${name}`);
  const info = b.getFunctionInfo(fn);
  if (b.expandType(info.params).join(',') !== params.join(',') || info.results !== result) fail(`invalid signature for ${name}`);
  if (!imported && (info.base || !info.body)) fail(`${name} must be defined in the module`);
  return { fn, info, name: ex.value };
}

function constantResult(body) {
  const info = b.getExpressionInfo(body);
  if (info.id === b.ConstId && info.type === b.i32) return info.value >>> 0;
  if (info.id === b.ReturnId && info.value) return constantResult(info.value);
  if (info.id === b.BlockId && info.children.length &&
      info.children.slice(0, -1).every(expr => b.getExpressionId(expr) === b.NopId)) return constantResult(info.children.at(-1));
  return null;
}

function markerWordAddress(body, initialBytes) {
  const info = b.getExpressionInfo(body);
  if (info.id === b.ReturnId && info.value) return markerWordAddress(info.value, initialBytes);
  if (info.id === b.BlockId && info.children.length &&
      info.children.slice(0, -1).every(expr => b.getExpressionId(expr) === b.NopId))
    return markerWordAddress(info.children.at(-1), initialBytes);
  if (info.id !== b.LoadId || info.type !== b.i32 || info.bytes !== 4 || info.isAtomic)
    fail('marker placeholder must load a static volatile i32 word');
  const base = constantResult(info.ptr);
  const address = base === null ? null : base + info.offset;
  if (address === null || address % 4 || address + 4 > initialBytes)
    fail('marker placeholder must load an aligned static word within initial memory');
  return address;
}

// LLVM's noinline attribute does not survive every final wasm-opt inlining pass.
// Initialize the volatile backing word as well as replacing the exported body,
// so optimized callers that contain a copy of the load observe the same marker.
// Append the new active segment; existing passive segment indices stay intact.
function initializeMarkerWord(binary, address) {
  const uleb = value => {
    const out = [];
    do { const byte = value & 127; value = Math.floor(value / 128); out.push(byte | (value ? 128 : 0)); } while (value);
    return Uint8Array.from(out);
  };
  const sleb = value => {
    const out = [];
    value |= 0;
    for (;;) {
      const byte = value & 127; value >>= 7;
      const done = (value === 0 && !(byte & 64)) || (value === -1 && (byte & 64));
      out.push(byte | (done ? 0 : 128));
      if (done) return Uint8Array.from(out);
    }
  };
  const concat = (...parts) => {
    const result = new Uint8Array(parts.reduce((n, part) => n + part.length, 0));
    let at = 0;
    for (const part of parts) { result.set(part, at); at += part.length; }
    return result;
  };
  const readU32 = (data, at) => {
    let value = 0;
    for (let n = 0; n < 5 && at < data.length; n++) {
      const byte = data[at++]; value += (byte & 127) * 2 ** (n * 7);
      if (!(byte & 128)) return [value, at];
    }
    fail('invalid emitted section length');
  };
  const segment = concat(Uint8Array.of(0, 0x41), sleb(address),
    Uint8Array.of(0x0b, 4, 0x31, 0x4a, 0x42, 0x52));
  const chunks = [binary.subarray(0, 8)];
  let at = 8, foundData = false;
  while (at < binary.length) {
    const id = binary[at++];
    const [size, start] = readU32(binary, at);
    if (start + size > binary.length) fail('truncated emitted section');
    let payload = binary.subarray(start, start + size);
    at = start + size;
    if (id === 11) {
      const [count, first] = readU32(payload, 0);
      payload = concat(uleb(count + 1), payload.subarray(first), segment);
      foundData = true;
    } else if (id === 12) {
      const [count, end] = readU32(payload, 0);
      if (end !== payload.length) fail('invalid emitted data count');
      payload = uleb(count + 1);
    }
    chunks.push(Uint8Array.of(id), uleb(payload.length), payload);
  }
  if (!foundData) {
    const payload = concat(uleb(1), segment);
    chunks.push(Uint8Array.of(11), uleb(payload.length), payload);
  }
  return concat(...chunks);
}

function optionsWithDefaults(options) {
  const out = { ...DEFAULT_EXPORTS, ...options };
  for (const key of ['touch', 'marker']) if (typeof out[key] !== 'string' || !out[key]) fail(`invalid ${key} export name`);
  if (out.config !== null && (typeof out.config !== 'string' || !out.config)) fail('invalid config export name');
  return out;
}

function configureFeatures(module) {
  const original = module.getFeatures();
  if (original & ~ALLOWED_FEATURES) fail(`unsupported declared WebAssembly features: 0x${(original & ~ALLOWED_FEATURES).toString(16)}`);
  module.setFeatures(original | BASE_FEATURES);
}

function collectWrites(module, body, functionName) {
  const writes = [];
  const stack = [{ expr: body, depth: 0, source: 'body' }];
  while (stack.length) {
    const { expr, depth, source } = stack.pop();
    if (!expr) continue;
    const id = b.getExpressionId(expr);
    const rejected = REJECTED.get(id);
    if (rejected) fail(`${rejected} in ${functionName}`);
    const descriptor = CHILD_FIELDS.get(id);
    if (!descriptor) fail(`unsupported expression ${id} in ${functionName}`);
    let info;
    // Binaryen 132 has no getExpressionInfo adapter for childless instructions
    // such as unreachable. Inspect those through the common expression API.
    try { info = descriptor.fields.length ? b.getExpressionInfo(expr) : { id, type: b.getExpressionType(expr) }; }
    catch (error) { fail(`cannot inspect ${source} in ${functionName} (${expr}): ${error.message}`); }
    if ((info.id === b.StoreId || info.id === b.LoadId) && info.isAtomic) fail(`atomic memory operation in ${functionName}`);
    if (descriptor.name.startsWith('SIMD') || (info.id === b.StoreId && b.Store.getValueType(expr) === b.v128) ||
        info.type === b.v128) module.setFeatures(module.getFeatures() | b.Features.SIMD128);
    if (descriptor.name.startsWith('Ref') || descriptor.name.startsWith('Table')) module.setFeatures(module.getFeatures() | b.Features.ReferenceTypes);
    if (descriptor.name.startsWith('Tuple')) module.setFeatures(module.getFeatures() | b.Features.Multivalue);
    if (info.isReturn) module.setFeatures(module.getFeatures() | b.Features.TailCall);
    const write = [b.StoreId, b.MemoryCopyId, b.MemoryFillId, b.MemoryInitId].includes(info.id);
    if (write) writes.push({ expr, depth, kind: descriptor.name });
    for (const field of descriptor.fields) {
      const children = info[field];
      const source = `${descriptor.name}.${field}`;
      if (Array.isArray(children)) for (const child of children) stack.push({ expr: child, depth: depth + Number(write), source });
      else if (children) stack.push({ expr: children, depth: depth + Number(write), source });
    }
  }
  return writes;
}

/** Instrument a linked wasm32 binary. No optimizer runs after inserting hooks. */
export async function instrumentWasm(input, options = {}) {
  const opts = optionsWithDefaults(options);
  const memory = inspectMemory(input);
  await b.ready;
  const module = b.readBinary(new Uint8Array(input));
  try {
    configureFeatures(module);
    const touch = exportedFunction(module, opts.touch, [b.i32, b.i32], b.none, true);
    const marker = exportedFunction(module, opts.marker, [], b.i32);
    if (constantResult(marker.info.body) === INSTRUMENTATION_MAGIC) fail('module is already instrumented');
    const config = opts.config === null ? null : exportedFunction(module, opts.config, [], b.i32);
    if (new Set([touch.name, marker.name, ...(config ? [config.name] : [])]).size !== (config ? 3 : 2)) fail('hook, config and marker must be distinct functions');
    const cfg = config ? constantResult(config.info.body) : null;
    if (config && (cfg === null || !cfg || cfg % 4 || cfg + 20 > memory.initial * 65536)) fail('config export must return an aligned, static five-word address within initial memory');
    const markerAddress = markerWordAddress(marker.info.body, memory.initial * 65536);
    if (config && markerAddress < cfg + 20 && markerAddress + 4 > cfg) fail('marker word must not overlap configuration');
    const excluded = new Set([touch.name, marker.name, config?.name]);
    const report = {
      schema: INSTRUMENTATION_SECTION, magic: INSTRUMENTATION_MAGIC,
      exports: { touch: opts.touch, config: opts.config, marker: opts.marker },
      configAddress: cfg, markerAddress, functions: 0, stores: 0, copies: 0, fills: 0, inits: 0, maxWriteDepth: 0,
    };
    const functions = [];
    for (let n = 0; n < module.getNumFunctions(); n++) functions.push(b.getFunctionInfo(module.getFunctionByIndex(n)));
    for (const info of functions) {
      if (info.base || !info.body || excluded.has(info.name)) continue;
      const writes = collectWrites(module, info.body, info.name);
      if (!writes.length) continue;
      const vars = [...info.vars];
      const params = b.expandType(info.params).length;
      const slots = new Map();
      const slot = (depth, purpose, type) => {
        const key = `${depth}:${purpose}:${type}`;
        if (!slots.has(key)) { slots.set(key, params + vars.length); vars.push(type); }
        return slots.get(key);
      };
      const get = (index, type = b.i32) => module.local.get(index, type);
      const c = value => module.i32.const(value | 0);
      const cfgLoad = word => module.i32.load(cfg + word * 4, 4, c(0));
      // Config ranges are valid non-wrapping wasm32 spans. Subtraction avoids
      // overflowing the end address of a store that crosses either boundary.
      const overlaps = (address, length, baseWord) => module.i32.and(
        module.i32.ne(cfgLoad(baseWord + 1), c(0)),
        module.i32.or(
          module.i32.lt_u(module.i32.sub(address(), cfgLoad(baseWord)), cfgLoad(baseWord + 1)),
          module.i32.and(module.i32.lt_u(address(), cfgLoad(baseWord)),
            module.i32.lt_u(module.i32.sub(cfgLoad(baseWord), address()), length()))));
      const guard = (address, length, originalPointer, offset = 0) => {
        let call = module.call(touch.name, [address(), length()], b.none);
        if (config) call = module.if(cfgLoad(0), module.if(
          module.i32.and(module.i32.ne(length(), c(0)),
            module.i32.or(overlaps(address, length, 1), overlaps(address, length, 3))), call));
        else call = module.if(module.i32.ne(length(), c(0)), call);
        // Wasm memarg offsets are added without wrapping. A trapping effective
        // address must not journal a different low address after i32 overflow.
        if (offset) call = module.if(module.i32.le_u(originalPointer(), c(0xffffffff - offset)), call);
        return call;
      };
      for (const { expr, depth, kind } of writes) {
        report.maxWriteDepth = Math.max(report.maxWriteDepth, depth + 1);
        const pointer = slot(depth, 'pointer', b.i32);
        if (kind === 'Store') {
          const valueType = b.Store.getValueType(expr);
          if (![b.i32, b.i64, b.f32, b.f64, b.v128].includes(valueType)) fail(`unsupported store type in ${info.name}`);
          const offset = b.Store.getOffset(expr);
          if (!Number.isSafeInteger(offset) || offset < 0 || offset > 0xffffffff) fail(`invalid wasm32 store offset in ${info.name}`);
          const value = slot(depth, 'value', valueType);
          const bytes = b.Store.getBytes(expr);
          const address = () => offset ? module.i32.add(get(pointer), c(offset)) : get(pointer);
          b.Store.setPtr(expr, module.local.tee(pointer, b.Store.getPtr(expr), b.i32));
          b.Store.setValue(expr, module.block(null, [
            module.local.set(value, b.Store.getValue(expr)),
            guard(address, () => c(bytes), () => get(pointer), offset),
            get(value, valueType),
          ], valueType));
          report.stores++;
        } else {
          const api = b[kind];
          const length = slot(depth, 'length', b.i32);
          api.setDest(expr, module.local.tee(pointer, api.getDest(expr), b.i32));
          api.setSize(expr, module.block(null, [
            module.local.set(length, api.getSize(expr)),
            guard(() => get(pointer), () => get(length), () => get(pointer)),
            get(length),
          ], b.i32));
          report[{ MemoryCopy: 'copies', MemoryFill: 'fills', MemoryInit: 'inits' }[kind]]++;
        }
      }
      // Locals are private to a call and a nesting depth, including recursive
      // calls. Reusing a depth across sibling stores keeps large functions small.
      module.removeFunction(info.name);
      module.addFunction(info.name, info.params, info.results, vars, info.body);
      report.functions++;
    }
    if (!(report.stores + report.copies + report.fills + report.inits)) fail('no memory writes were instrumented');
    b.Function.setBody(marker.fn, module.i32.const(INSTRUMENTATION_MAGIC));
    module.addCustomSection(INSTRUMENTATION_SECTION, new TextEncoder().encode(JSON.stringify(report)));
    if (!module.validate()) fail('instrumented module failed validation');
    const bytes = initializeMarkerWord(module.emitBinary(), markerAddress);
    await verifyInstrumentedWasm(bytes, opts);
    return { bytes, report };
  } finally { module.dispose(); }
}

/** Verify the marker in the final binary, including after an optional wasm-opt. */
export async function verifyInstrumentedWasm(input, options = {}) {
  const opts = optionsWithDefaults(options);
  inspectMemory(input);
  await b.ready;
  const module = b.readBinary(new Uint8Array(input));
  try {
    const marker = exportedFunction(module, opts.marker, [], b.i32);
    if (constantResult(marker.info.body) !== INSTRUMENTATION_MAGIC) fail('instrumentation marker missing or incorrect');
    exportedFunction(module, opts.touch, [b.i32, b.i32], b.none, true);
    if (opts.config !== null) exportedFunction(module, opts.config, [], b.i32);
    return { marker: opts.marker, magic: INSTRUMENTATION_MAGIC };
  } finally { module.dispose(); }
}

async function main(args) {
  const options = {};
  const positional = [];
  let verify = false;
  for (const arg of args) {
    if (arg === '--verify') verify = true;
    else if (arg === '--no-config') options.config = null;
    else if (/^--(touch|config|marker)=.+/.test(arg)) { const at = arg.indexOf('='); options[arg.slice(2, at)] = arg.slice(at + 1); }
    else if (arg.startsWith('-')) fail(`unknown option ${arg}`);
    else positional.push(arg);
  }
  if (positional.length !== (verify ? 1 : 2)) fail('usage: instrument.mjs input.wasm output.wasm [--touch=name --config=name --marker=name] | --verify input.wasm');
  const input = readFileSync(positional[0]);
  if (verify) { process.stdout.write(`${JSON.stringify(await verifyInstrumentedWasm(input, options))}\n`); return; }
  if (resolve(positional[0]) === resolve(positional[1])) fail('input and output paths must differ');
  const { bytes, report } = await instrumentWasm(input, options);
  const temporary = `${positional[1]}.${process.pid}.tmp`;
  try { writeFileSync(temporary, bytes, { flag: 'wx' }); renameSync(temporary, positional[1]); }
  finally { rmSync(temporary, { force: true }); }
  process.stdout.write(`${JSON.stringify(report)}\n`);
}

if (process.argv[1] && pathToFileURL(resolve(process.argv[1])).href === import.meta.url) {
  try { await main(process.argv.slice(2)); }
  catch (error) { process.stderr.write(`${error.message}\n`); process.exitCode = 1; }
}
