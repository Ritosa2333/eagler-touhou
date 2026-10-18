// Emscripten/WASM boundary for the real Runtime Shell keyboard browser test.
// This fixture replaces only the generated native module import; it never
// installs input listeners or a keyboard bridge.
let scanCodes = {};
try { ({scanCodes} = await import('./keyboard.mjs')); } catch {}

const eventLog = [];
const held = new Set();
const decoder = new TextDecoder();
let memory;
let nextPointer = 4096;

function record(code, down) {
  if (!code) return;
  if (down) held.add(code);
  else held.delete(code);
  eventLog.push({code, down: !!down});
}

function codeForScan(scan) {
  return Object.keys(scanCodes).find(code => scanCodes[code] === scan) || `scan:${scan}`;
}

function cString(pointer) {
  const bytes = new Uint8Array(memory.buffer);
  let end = pointer;
  while (end < bytes.length && bytes[end] !== 0) end++;
  return decoder.decode(bytes.subarray(pointer, end));
}

function createFS() {
  const entries = new Map([['/', {kind: 'dir'}]]);
  const normalize = path => {
    const parts = String(path).replaceAll('\\', '/').split('/').filter(Boolean);
    const result = [];
    for (const part of parts) {
      if (part === '.') continue;
      if (part === '..') result.pop();
      else result.push(part);
    }
    return '/' + result.join('/');
  };
  const parent = path => {
    const value = normalize(path);
    return value.slice(0, value.lastIndexOf('/')) || '/';
  };
  const fs = {
    mkdir(path) {
      path = normalize(path);
      if (entries.has(path)) throw Error('File exists: ' + path);
      entries.set(path, {kind: 'dir'});
    },
    mkdirTree(path) {
      const target = normalize(path);
      let current = '';
      for (const part of target.split('/').filter(Boolean)) {
        current += '/' + part;
        if (!entries.has(current)) entries.set(current, {kind: 'dir'});
      }
    },
    mount(_type, _options, path) {
      fs.mkdirTree(path);
      return {mount: {}};
    },
    syncfs(_populate, callback) { queueMicrotask(() => callback?.(null)); },
    writeFile(path, value) {
      path = normalize(path);
      fs.mkdirTree(parent(path));
      const bytes = typeof value === 'string' ? new TextEncoder().encode(value) : new Uint8Array(value);
      entries.set(path, {kind: 'file', bytes: bytes.slice(), mtime: new Date()});
    },
    readFile(path) {
      const entry = entries.get(normalize(path));
      if (!entry || entry.kind !== 'file') throw Error('No such file: ' + path);
      return entry.bytes.slice();
    },
    analyzePath(path) { return {exists: entries.has(normalize(path))}; },
    stat(path) {
      const entry = entries.get(normalize(path));
      if (!entry) throw Error('No such file: ' + path);
      return {mode: entry.kind === 'dir' ? 0x4000 : 0x8000, size: entry.bytes?.length || 0, mtime: entry.mtime || new Date()};
    },
    isDir(mode) { return (mode & 0xf000) === 0x4000; },
    isFile(mode) { return (mode & 0xf000) === 0x8000; },
    readdir(path) {
      path = normalize(path);
      if (!entries.has(path)) throw Error('No such directory: ' + path);
      const prefix = path === '/' ? '/' : path + '/';
      const children = new Set(['.', '..']);
      for (const key of entries.keys()) {
        if (!key.startsWith(prefix) || key === path) continue;
        const child = key.slice(prefix.length).split('/')[0];
        if (child) children.add(child);
      }
      return [...children];
    },
    unlink(path) {
      path = normalize(path);
      if (!entries.delete(path)) throw Error('No such file: ' + path);
    },
    symlink(source, target) {
      target = normalize(target);
      fs.mkdirTree(parent(target));
      entries.set(target, {kind: 'file', bytes: new TextEncoder().encode(String(source)), mtime: new Date()});
    },
    rename(from, to) {
      from = normalize(from); to = normalize(to);
      const entry = entries.get(from);
      if (!entry) throw Error('No such file: ' + from);
      fs.mkdirTree(parent(to)); entries.set(to, entry); entries.delete(from);
    },
  };
  return fs;
}

function makeCore() {
  memory = new WebAssembly.Memory({initial: 256});
  const fs = createFS();
  const module = {};
  const core = new Proxy(module, {
    get(target, property) {
      if (property === 'then') return undefined;
      if (property === 'memory') return memory;
      if (property === 'FS') return fs;
      if (property === 'IDBFS') return module.IDBFS;
      if (property === 'HEAPU8') return new Uint8Array(memory.buffer);
      if (property === 'HEAP32') return new Int32Array(memory.buffer);
      if (property === 'HEAPU32') return new Uint32Array(memory.buffer);
      if (property === 'allocate' || property === 'malloc' || property === '_malloc' || property === 'graphics_allocate') {
        return size => { const pointer = nextPointer; nextPointer += Math.max(8, Number(size) || 8); return pointer; };
      }
      if (['deallocate', 'free', '_free', 'release', 'graphics_free'].includes(property)) return () => {};
      if (property === 'sdl_key') return (pointer, down) => record(cString(pointer), down);
      if (property === '_th09_key' || property === '_th15_key' || property === 'th11_key') {
        return (scan, down) => record(codeForScan(scan), down);
      }
      if (['sdl_keys_clear', '_th09_keys_clear', 'th11_keys_clear', '_th15_keys_clear'].includes(property)) {
        return () => { held.clear(); eventLog.push({clear: true}); };
      }
      if (property in target) return target[property];
      if (typeof property === 'symbol') return undefined;
      return () => 1;
    },
    set(target, property, value) { target[property] = value; return true; },
  });
  Object.assign(module, {
    memory,
    FS: fs,
    IDBFS: {},
    SDL3: {audioContext: {state: 'running', resume: async () => {}}},
    allocate: size => { const pointer = nextPointer; nextPointer += Math.max(8, Number(size) || 8); return pointer; },
    deallocate() {},
  });
  window.__eaglerNativeKeyboardSink = {
    events: eventLog,
    held: () => [...held].sort(),
  };
  return core;
}

export default async function createModule(options = {}) {
  const core = makeCore();
  if (typeof options.instantiateWasm === 'function') {
    const original = WebAssembly.instantiateStreaming;
    WebAssembly.instantiateStreaming = async () => ({instance: {exports: core}, module: {}});
    try {
      await options.instantiateWasm({}, () => {});
    } finally {
      WebAssembly.instantiateStreaming = original;
    }
  }
  return core;
}
