// TH128 import-package producer + validator (source candidate, package-v2).
//
// Envelope: eagler-touhou/package/1, game/target "th128", resource-only package
// (no `runtime` key: the web page supplies the Runtime, exactly like the TH12
// reference package). TH128's own data layout is used; TH12's dataLayout value
// and TH12 payloads are never copied.
//
// Scope of this module: build a deterministic Package ZIP + extracted folder
// from an explicit input manifest, and validate such a descriptor/package
// without touching the network. It does not install into a Package Store and
// does not claim upstream Launcher support: at the pinned upstream commit
// `isGameId` has no "th128" entry, so `allowedGameIds: ["th128"]` marks an
// extension target and MUST NOT be reported as a canonical upstream import PASS.
//
// No third-party dependencies: STORE-only ZIP writing and CRC32 are hand-rolled;
// reading supports STORE only (the writer never emits DEFLATE).

import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

export const PACKAGE_DESCRIPTOR_SCHEMA = 'eagler-touhou/package/1';
export const RUNTIME_PROTOCOL = 'eagler-touhou/1';
export const TH128_GAME_ID = 'th128';
export const DATA_LAYOUT_SCHEMA = 'th128-package-data-layout/1';
export const DEFAULT_TARGET_PREFIX = '/assets/';
export const DEFAULT_SUPPORTED_LANGUAGES = Object.freeze(['zh', 'ja']);
export const DEFAULT_SUPPORTED_AUDIO_FORMATS = Object.freeze(['wav']);

const ID_PATTERN = /^[a-z0-9][a-z0-9._:-]{0,127}$/i;
const REVISION_PATTERN = ID_PATTERN;
const SHA256_PATTERN = /^[0-9a-f]{64}$/;
const DATA_LAYOUT_PATTERN = /^sha256-[0-9a-f]{64}$/;
const FORBIDDEN_TARGET_SEGMENTS = new Set([
  'saves', 'save', 'config', 'configs', 'replays', 'replay',
  'logs', 'log', 'profiles', 'profile', 'credentials', 'secrets',
  'test-results', 'acceptance',
]);
const FORBIDDEN_SOURCE_EXTENSIONS = new Set(['.log', '.rpy', '.cfg', '.ini', '.exe', '.dll']);

export class PackageError extends Error {
  constructor(code, message) {
    super(`${code}: ${message}`);
    this.name = 'PackageError';
    this.code = code;
  }
}

const fail = (code, message) => {
  throw new PackageError(code, message);
};

const isPlainObject = (value) =>
  typeof value === 'object' && value !== null && !Array.isArray(value);

// ---------- primitives ----------

export function sha256Hex(bytes) {
  return crypto.createHash('sha256').update(bytes).digest('hex');
}

export function canonicalJson(value, spaces = 2) {
  const walk = (node) => {
    if (Array.isArray(node)) return node.map(walk);
    if (isPlainObject(node)) {
      const out = {};
      for (const key of Object.keys(node).sort()) out[key] = walk(node[key]);
      return out;
    }
    return node;
  };
  return `${JSON.stringify(walk(value), null, spaces)}\n`;
}

const CRC32_TABLE = (() => {
  const table = new Uint32Array(256);
  for (let n = 0; n < 256; n += 1) {
    let c = n;
    for (let k = 0; k < 8; k += 1) c = (c & 1) ? (0xedb88320 ^ (c >>> 1)) : (c >>> 1);
    table[n] = c >>> 0;
  }
  return table;
})();

export function crc32(bytes) {
  let crc = 0xffffffff;
  for (const byte of bytes) crc = CRC32_TABLE[(crc ^ byte) & 0xff] ^ (crc >>> 8);
  return (crc ^ 0xffffffff) >>> 0;
}

// ---------- path / identity rules ----------

export function isSafeRelativeSource(value) {
  if (typeof value !== 'string' || value.length === 0) return false;
  if (value.includes('\\') || value.includes('\0')) return false;
  if (value.startsWith('/') || /^[A-Za-z]:/.test(value)) return false;
  return value.split('/').every((part) => part !== '' && part !== '.' && part !== '..');
}

export function isSafeMountTarget(value) {
  if (typeof value !== 'string' || !value.startsWith('/')) return false;
  if (value.includes('\\') || value.includes('\0')) return false;
  return value.slice(1).split('/').every((part) => part !== '' && part !== '.' && part !== '..');
}

function assertId(value, label) {
  if (typeof value !== 'string' || !ID_PATTERN.test(value)) fail('invalid-id', `${label} is not a safe file id`);
  return value;
}

function assertRevision(value, label) {
  if (typeof value !== 'string' || !REVISION_PATTERN.test(value)) fail('invalid-revision', `${label} revision is malformed`);
  return value;
}

function assertSha256(value, label) {
  if (typeof value !== 'string' || !SHA256_PATTERN.test(value.toLowerCase())) fail('invalid-hash', `${label} sha256 is malformed`);
  return value.toLowerCase();
}

function assertTargetShape(target, label, options) {
  if (!isSafeMountTarget(target)) fail('unsafe-target', `${label} target is not a safe absolute mount path`);
  const prefix = options.targetPrefix ?? DEFAULT_TARGET_PREFIX;
  if (prefix && !target.startsWith(prefix)) fail('unsafe-target', `${label} target leaves the ${prefix} mount`);
  const segments = target.toLowerCase().split('/');
  for (const segment of segments) {
    if (FORBIDDEN_TARGET_SEGMENTS.has(segment)) fail('forbidden-payload', `${label} target contains a non-package segment "${segment}"`);
  }
  return target;
}

function assertSourceShape(source, id, target) {
  if (!isSafeRelativeSource(source)) fail('unsafe-source', `${id} source is not a safe relative path`);
  if (source === target) fail('source-target-confusion', `${id} source and target are identical`);
  if (source.startsWith('assets/') && !source.startsWith('/assets/')) {
    // A relative source that mirrors the mount path is still fine; this branch
    // exists only to document that source and target live in different spaces.
  }
  const ext = path.posix.extname(source).toLowerCase();
  if (FORBIDDEN_SOURCE_EXTENSIONS.has(ext)) fail('forbidden-payload', `${id} source has a non-package extension ${ext}`);
  return source;
}

// ---------- descriptor validation ----------

export function dataLayoutOf(descriptor) {
  const material = {
    schema: DATA_LAYOUT_SCHEMA,
    dataFile: descriptor.runtimeRequirement?.dataFile ?? null,
    files: Object.keys(descriptor.files ?? {})
      .sort()
      .map((id) => ({ id, target: descriptor.files[id].target })),
  };
  return `sha256-${sha256Hex(Buffer.from(JSON.stringify(material), 'utf8'))}`;
}

export function validatePackageDescriptor(descriptor, options = {}) {
  const allowedGameIds = options.allowedGameIds ?? [TH128_GAME_ID];
  const supportedLanguages = new Set(options.supportedLanguages ?? DEFAULT_SUPPORTED_LANGUAGES);
  const supportedAudio = new Set(options.supportedAudioFormats ?? DEFAULT_SUPPORTED_AUDIO_FORMATS);
  const requireSha = options.requireSha !== false;
  const requireBytes = options.requireBytes !== false;

  if (!isPlainObject(descriptor)) fail('invalid-descriptor', 'descriptor must be an object');
  if (descriptor.schema !== PACKAGE_DESCRIPTOR_SCHEMA) fail('invalid-descriptor', `schema must be ${PACKAGE_DESCRIPTOR_SCHEMA}`);
  if (typeof descriptor.game !== 'string' || !ID_PATTERN.test(descriptor.game)) fail('invalid-descriptor', 'game id is malformed');
  if (!allowedGameIds.includes(descriptor.game)) {
    fail('unsupported-game', `game "${descriptor.game}" is not a supported import target (the pinned upstream isGameId has no th128 entry, so this is an extension target, not an upstream PASS)`);
  }
  assertRevision(descriptor.revision, 'descriptor');
  if (descriptor.gameVersion !== undefined && !['original', 'beauty'].includes(descriptor.gameVersion)) {
    fail('invalid-descriptor', 'gameVersion must be original or beauty');
  }
  if (descriptor.runtime !== undefined || descriptor.runtimes !== undefined) {
    fail('runtime-not-allowed', 'a TH128 resource package must not carry a Runtime');
  }

  if (!isPlainObject(descriptor.files) || Object.keys(descriptor.files).length === 0) {
    fail('invalid-files', 'files must be a non-empty object');
  }
  const targets = new Map();
  const sources = new Map();
  for (const id of Object.keys(descriptor.files)) {
    assertId(id, `files["${id}"]`);
    const file = descriptor.files[id];
    if (!isPlainObject(file)) fail('invalid-files', `${id} must be an object`);
    assertRevision(file.revision, id);
    assertTargetShape(file.target, id, options);
    assertSourceShape(file.source, id, file.target);
    if (requireBytes) {
      if (!Number.isSafeInteger(file.bytes) || file.bytes < 0) fail('invalid-bytes', `${id} bytes must be a non-negative integer`);
    } else if (file.bytes !== undefined && (!Number.isSafeInteger(file.bytes) || file.bytes < 0)) {
      fail('invalid-bytes', `${id} bytes must be a non-negative integer`);
    }
    if (requireSha || file.sha256 !== undefined) assertSha256(file.sha256, id);
    if (targets.has(file.target)) fail('duplicate-target', `${id} repeats target ${file.target} (also used by ${targets.get(file.target)})`);
    if (sources.has(file.source)) fail('duplicate-source', `${id} repeats source ${file.source}`);
    targets.set(file.target, id);
    sources.set(file.source, id);
  }

  const requirement = descriptor.runtimeRequirement;
  if (!isPlainObject(requirement)) fail('invalid-runtime-requirement', 'runtimeRequirement is required for a resource package');
  if (typeof requirement.protocol !== 'string' || requirement.protocol.length === 0) fail('invalid-runtime-requirement', 'protocol must be a non-empty string');
  if (requirement.protocol !== RUNTIME_PROTOCOL) fail('invalid-runtime-requirement', `protocol must be ${RUNTIME_PROTOCOL}`);
  if (requirement.target !== descriptor.game) fail('invalid-runtime-requirement', 'runtimeRequirement.target must equal game');
  if (typeof requirement.dataFile !== 'string' || !Object.hasOwn(descriptor.files, requirement.dataFile)) {
    fail('invalid-runtime-requirement', 'runtimeRequirement.dataFile must name a declared file');
  }
  if (typeof requirement.dataLayout !== 'string' || !DATA_LAYOUT_PATTERN.test(requirement.dataLayout)) {
    fail('invalid-data-layout', 'runtimeRequirement.dataLayout must be sha256-<64 hex>');
  }
  if (/^sha256-94ee7257/.test(requirement.dataLayout)) {
    fail('foreign-data-layout', 'TH12 dataLayout must never be reused for TH128');
  }
  const expectedLayout = dataLayoutOf(descriptor);
  if (requirement.dataLayout !== expectedLayout) {
    fail('data-layout-mismatch', `dataLayout does not describe this file list (expected ${expectedLayout})`);
  }

  const base = descriptor.base;
  if (!isPlainObject(base) || !Array.isArray(base.files) || base.files.length === 0) {
    fail('invalid-base', 'base.files must be a non-empty array');
  }
  const seenBase = new Set();
  for (const id of base.files) {
    if (typeof id !== 'string' || !Object.hasOwn(descriptor.files, id)) fail('unknown-file', `base references unknown file ${String(id)}`);
    if (seenBase.has(id)) fail('duplicate-file', `base repeats ${id}`);
    seenBase.add(id);
  }
  if (!seenBase.has(requirement.dataFile)) fail('invalid-base', 'base.files must include runtimeRequirement.dataFile');

  if (descriptor.components !== undefined) {
    if (!isPlainObject(descriptor.components)) fail('invalid-components', 'components must be an object');
    for (const componentId of Object.keys(descriptor.components)) {
      assertId(componentId, `components["${componentId}"]`);
      const component = descriptor.components[componentId];
      if (!isPlainObject(component)) fail('invalid-components', `${componentId} must be an object`);
      if (typeof component.type !== 'string' || !ID_PATTERN.test(component.type)) fail('invalid-components', `${componentId}.type is malformed`);
      if (component.type === 'ogg' && !supportedAudio.has('ogg')) {
        fail('unsupported-component', `${componentId} advertises OGG audio, which this runtime does not consume`);
      }
      if (component.files !== undefined) {
        if (!Array.isArray(component.files)) fail('invalid-components', `${componentId}.files must be an array`);
        for (const id of component.files) {
          if (typeof id !== 'string' || !Object.hasOwn(descriptor.files, id)) fail('unknown-file', `${componentId} references unknown file ${String(id)}`);
        }
      }
      if (component.entries !== undefined) {
        if (!Array.isArray(component.entries) || component.entries.length === 0) fail('invalid-components', `${componentId}.entries must be a non-empty array`);
        const entryIds = new Set();
        for (const entry of component.entries) {
          if (!isPlainObject(entry)) fail('invalid-components', `${componentId} entry must be an object`);
          assertId(entry.id, `${componentId} entry`);
          if (entryIds.has(entry.id)) fail('duplicate-entry', `${componentId} repeats entry ${entry.id}`);
          entryIds.add(entry.id);
          if (typeof entry.file !== 'string' || !Object.hasOwn(descriptor.files, entry.file)) fail('unknown-file', `${componentId} entry references unknown file ${String(entry.file)}`);
          if (entry.title !== undefined && typeof entry.title !== 'string') fail('invalid-components', `${componentId} entry title must be a string`);
          if (component.type === 'language') {
            const code = String(entry.id).replace(/^lang[_-]/i, '').toLowerCase();
            if (!supportedLanguages.has(code)) fail('unsupported-language', `${entry.id} is not a shipped TH128 language (${[...supportedLanguages].join(', ')})`);
            const target = descriptor.files[entry.file].target;
            if (!target.endsWith(`/${code}.zip`) && !target.includes(`/${code}/`)) {
              // The language payload must be mounted per language identity.
              fail('unsupported-language', `${entry.file} target does not encode language ${code}`);
            }
          }
        }
      }
    }
  }
  return descriptor;
}

// ---------- file-level validation ----------

export function validatePackageFiles(descriptor, options = {}) {
  validatePackageDescriptor(descriptor, options);
  const rootDir = options.rootDir;
  if (typeof rootDir !== 'string' || rootDir.length === 0) fail('invalid-root', 'rootDir is required to validate payload bytes');
  const root = path.resolve(rootDir);
  const problems = [];
  const checked = [];
  for (const id of Object.keys(descriptor.files).sort()) {
    const file = descriptor.files[id];
    const absolute = resolveSource(root, file.source, options);
    if (!absolute) {
      problems.push({ id, code: 'unsafe-source', message: 'source resolves outside the allowed root' });
      continue;
    }
    if (!fs.existsSync(absolute)) {
      problems.push({ id, code: 'missing-source', message: `missing ${file.source}` });
      continue;
    }
    const bytes = fs.readFileSync(absolute);
    const actualSha = sha256Hex(bytes);
    if (file.bytes !== undefined && bytes.length !== file.bytes) {
      problems.push({ id, code: 'size-mismatch', message: `${bytes.length} != declared ${file.bytes}` });
      continue;
    }
    if (file.sha256 !== undefined && actualSha !== file.sha256.toLowerCase()) {
      problems.push({ id, code: 'hash-mismatch', message: `sha256 ${actualSha} != declared ${file.sha256}` });
      continue;
    }
    checked.push({ id, bytes: bytes.length, sha256: actualSha });
  }
  return { ok: problems.length === 0, problems, checked };
}

function resolveSource(root, source, options) {
  const candidate = path.resolve(root, source);
  const rel = path.relative(root, candidate);
  const inside = rel === '' || (!rel.startsWith('..') && !path.isAbsolute(rel));
  if (!inside) {
    if (!options.allowOutsideRoot) return null;
  }
  return candidate;
}

// ---------- producer ----------

function normalizeInput(input) {
  if (!isPlainObject(input)) fail('invalid-input', 'input must be an object');
  if (!Array.isArray(input.files) || input.files.length === 0) fail('invalid-input', 'input.files must be a non-empty array');
  const files = input.files.map((entry, index) => {
    if (!isPlainObject(entry)) fail('invalid-input', `input.files[${index}] must be an object`);
    const { id, source, target, sourceFile } = entry;
    assertId(id, `input.files[${index}].id`);
    const record = { id, source, target, sourceFile };
    return record;
  });
  return {
    game: input.game ?? TH128_GAME_ID,
    revision: input.revision,
    base: input.base ?? null,
    dataFile: input.dataFile ?? null,
    components: input.components ?? null,
    title: input.title ?? null,
    files,
  };
}

/**
 * planPackage: read the explicit input files and return the descriptor that
 * would be produced. Never writes anything.
 */
export function planPackage({ input, rootDir, options = {} }) {
  const spec = normalizeInput(input);
  const root = path.resolve(rootDir ?? '.');
  const allowedGameIds = options.allowedGameIds ?? [TH128_GAME_ID];
  if (!allowedGameIds.includes(spec.game)) fail('unsupported-game', `game ${spec.game} is not an allowed target`);

  const files = {};
  const payloads = [];
  const seenTargets = new Set();
  const seenSources = new Set();
  for (const entry of spec.files) {
    if (seenTargets.has(entry.target)) fail('duplicate-target', `${entry.id} repeats target ${entry.target}`);
    if (seenSources.has(entry.source)) fail('duplicate-source', `${entry.id} repeats source ${entry.source}`);
    seenTargets.add(entry.target);
    seenSources.add(entry.source);
    assertTargetShape(entry.target, entry.id, options);
    assertSourceShape(entry.source, entry.id, entry.target);
    const absolute = resolveSource(root, entry.sourceFile, options);
    if (!absolute) fail('unsafe-source', `${entry.id} sourceFile escapes the allowed root`);
    if (!fs.existsSync(absolute) || !fs.statSync(absolute).isFile()) fail('missing-source', `${entry.id} sourceFile does not exist`);
    const bytes = fs.readFileSync(absolute);
    if (bytes.length === 0) fail('empty-source', `${entry.id} is empty`);
    const sha256 = sha256Hex(bytes);
    files[entry.id] = {
      source: entry.source,
      target: entry.target,
      revision: sha256.slice(0, 16),
      bytes: bytes.length,
      sha256,
    };
    payloads.push({ id: entry.id, bytes, sha256 });
  }

  const dataFile = spec.dataFile
    ?? (Object.hasOwn(files, 'game-data') ? 'game-data' : Object.keys(files).find((id) => files[id].target.endsWith('/manifest.json')))
    ?? fail('invalid-input', 'input must name a dataFile (or provide game-data / a manifest target)');
  const baseFiles = spec.base ?? Object.keys(files).sort();
  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA,
    game: spec.game,
    revision: spec.revision ?? sha256Hex(Buffer.from(JSON.stringify({ game: spec.game, files }), 'utf8')).slice(0, 16),
    title: spec.title ?? 'TH128 prepared assets',
    files,
    base: { files: baseFiles },
    runtimeRequirement: {
      protocol: RUNTIME_PROTOCOL,
      target: spec.game,
      dataFile,
      dataLayout: 'sha256-' + '0'.repeat(64),
    },
  };
  if (spec.components) descriptor.components = spec.components;
  descriptor.runtimeRequirement.dataLayout = dataLayoutOf(descriptor);
  validatePackageDescriptor(descriptor, options);
  return { descriptor, payloads, dataFile };
}

/**
 * createPackage: deterministic extracted folder + STORE-only ZIP.
 * Refuses to overwrite an existing output directory.
 */
export function createPackage({ input, rootDir, outDir, zipName = null, options = {} }) {
  if (typeof outDir !== 'string' || outDir.length === 0) fail('invalid-output', 'outDir is required');
  const out = path.resolve(outDir);
  if (fs.existsSync(out)) {
    const entries = fs.readdirSync(out);
    if (entries.length > 0) fail('output-exists', `refusing to overwrite existing output ${out}`);
  }
  const plan = planPackage({ input, rootDir, options });
  const descriptorBytes = Buffer.from(canonicalJson(plan.descriptor), 'utf8');

  fs.mkdirSync(out, { recursive: true });
  const extracted = path.join(out, 'extracted');
  fs.mkdirSync(extracted, { recursive: true });
  fs.writeFileSync(path.join(extracted, 'package.json'), descriptorBytes);

  const zipEntries = [{ name: 'package.json', bytes: descriptorBytes }];
  for (const payload of plan.payloads) {
    const file = plan.descriptor.files[payload.id];
    const destination = path.join(extracted, ...file.source.split('/'));
    fs.mkdirSync(path.dirname(destination), { recursive: true });
    fs.writeFileSync(destination, payload.bytes);
    zipEntries.push({ name: file.source, bytes: payload.bytes });
  }

  const zip = buildStoredZip(zipEntries);
  const zipPath = path.join(out, zipName ?? `th128-import-${plan.descriptor.revision}.zip`);
  fs.writeFileSync(zipPath, zip);
  return {
    descriptor: plan.descriptor,
    descriptorBytes,
    zipPath,
    zipBytes: zip.length,
    zipSha256: sha256Hex(zip),
    outDir: out,
    entries: zipEntries.map((entry) => ({ name: entry.name, bytes: entry.bytes.length, sha256: sha256Hex(entry.bytes) })),
  };
}

// ---------- deterministic STORE-only ZIP ----------

const EOCD_SIGNATURE = 0x06054b50;
const CENTRAL_SIGNATURE = 0x02014b50;
const LOCAL_SIGNATURE = 0x04034b50;
const DOS_DATE = 0x0021; // 1980-01-01
const DOS_TIME = 0x0000;

export function buildStoredZip(entries) {
  if (!Array.isArray(entries) || entries.length === 0) fail('invalid-zip', 'entries must be a non-empty array');
  const names = new Set();
  const prepared = entries.map((entry) => {
    if (!entry || typeof entry.name !== 'string' || !isSafeRelativeSource(entry.name)) {
      fail('unsafe-zip-name', `${String(entry?.name)} is not a safe ZIP entry name`);
    }
    if (names.has(entry.name)) fail('duplicate-zip-name', entry.name);
    names.add(entry.name);
    const bytes = Buffer.from(entry.bytes ?? new Uint8Array());
    return { name: Buffer.from(entry.name, 'ascii'), bytes, crc: crc32(bytes) };
  });
  prepared.sort((a, b) => {
    const ao = a.name.toString() === 'package.json' ? 0 : 1;
    const bo = b.name.toString() === 'package.json' ? 0 : 1;
    if (ao !== bo) return ao - bo;
    return a.name.toString() < b.name.toString() ? -1 : 1;
  });

  const chunks = [];
  const central = [];
  let offset = 0;
  for (const entry of prepared) {
    const local = Buffer.alloc(30);
    local.writeUInt32LE(LOCAL_SIGNATURE, 0);
    local.writeUInt16LE(20, 4);
    local.writeUInt16LE(0, 6);
    local.writeUInt16LE(0, 8);
    local.writeUInt16LE(DOS_TIME, 10);
    local.writeUInt16LE(DOS_DATE, 12);
    local.writeUInt32LE(entry.crc, 14);
    local.writeUInt32LE(entry.bytes.length, 18);
    local.writeUInt32LE(entry.bytes.length, 22);
    local.writeUInt16LE(entry.name.length, 26);
    local.writeUInt16LE(0, 28);
    chunks.push(local, entry.name, entry.bytes);

    const cd = Buffer.alloc(46);
    cd.writeUInt32LE(CENTRAL_SIGNATURE, 0);
    cd.writeUInt16LE(20, 4);
    cd.writeUInt16LE(20, 6);
    cd.writeUInt16LE(0, 8);
    cd.writeUInt16LE(0, 10);
    cd.writeUInt16LE(DOS_TIME, 12);
    cd.writeUInt16LE(DOS_DATE, 14);
    cd.writeUInt32LE(entry.crc, 16);
    cd.writeUInt32LE(entry.bytes.length, 20);
    cd.writeUInt32LE(entry.bytes.length, 24);
    cd.writeUInt16LE(entry.name.length, 28);
    cd.writeUInt16LE(0, 30);
    cd.writeUInt16LE(0, 32);
    cd.writeUInt16LE(0, 34);
    cd.writeUInt16LE(0, 36);
    cd.writeUInt32LE(0, 38);
    cd.writeUInt32LE(offset, 42);
    central.push(Buffer.concat([cd, entry.name]));
    offset += 30 + entry.name.length + entry.bytes.length;
  }
  const centralBytes = Buffer.concat(central);
  const eocd = Buffer.alloc(22);
  eocd.writeUInt32LE(EOCD_SIGNATURE, 0);
  eocd.writeUInt16LE(0, 4);
  eocd.writeUInt16LE(0, 6);
  eocd.writeUInt16LE(prepared.length, 8);
  eocd.writeUInt16LE(prepared.length, 10);
  eocd.writeUInt32LE(centralBytes.length, 12);
  eocd.writeUInt32LE(offset, 16);
  eocd.writeUInt16LE(0, 20);
  return Buffer.concat([...chunks, centralBytes, eocd]);
}

/** Parse a STORE-only ZIP and verify CRC32 + sizes for every entry. */
export function readStoredZip(zipBytes) {
  const bytes = Buffer.from(zipBytes);
  if (bytes.length < 22) fail('invalid-zip', 'ZIP is too short');
  let eocd = -1;
  for (let i = bytes.length - 22; i >= 0; i -= 1) {
    if (bytes.readUInt32LE(i) === EOCD_SIGNATURE && i + 22 === bytes.length) {
      eocd = i;
      break;
    }
  }
  if (eocd < 0) fail('invalid-zip', 'ZIP end-of-central-directory record not found');
  const total = bytes.readUInt16LE(eocd + 10);
  const centralSize = bytes.readUInt32LE(eocd + 12);
  const centralOffset = bytes.readUInt32LE(eocd + 16);
  if (total === 0xffff || centralSize === 0xffffffff || centralOffset === 0xffffffff) fail('invalid-zip', 'ZIP64 is not supported');
  if (centralOffset + centralSize > bytes.length) fail('invalid-zip', 'ZIP central directory is out of bounds');
  const entries = new Map();
  let offset = centralOffset;
  for (let index = 0; index < total; index += 1) {
    if (offset + 46 > bytes.length || bytes.readUInt32LE(offset) !== CENTRAL_SIGNATURE) fail('invalid-zip', 'invalid ZIP central directory entry');
    const method = bytes.readUInt16LE(offset + 10);
    const crc = bytes.readUInt32LE(offset + 16);
    const compressedSize = bytes.readUInt32LE(offset + 20);
    const uncompressedSize = bytes.readUInt32LE(offset + 24);
    const nameLength = bytes.readUInt16LE(offset + 28);
    const extraLength = bytes.readUInt16LE(offset + 30);
    const commentLength = bytes.readUInt16LE(offset + 32);
    const localOffset = bytes.readUInt32LE(offset + 42);
    if (method !== 0) fail('invalid-zip', `entry ${index} is not STORE (method ${method})`);
    if (compressedSize !== uncompressedSize) fail('invalid-zip', `entry ${index} STORE size mismatch`);
    const name = bytes.subarray(offset + 46, offset + 46 + nameLength).toString('ascii');
    if (!isSafeRelativeSource(name)) fail('unsafe-zip-name', name);
    if (entries.has(name)) fail('duplicate-zip-name', name);
    if (localOffset + 30 > bytes.length) fail('invalid-zip', `${name}: truncated local header`);
    if (bytes.readUInt32LE(localOffset) !== LOCAL_SIGNATURE) fail('invalid-zip', `${name}: invalid local header`);
    const localNameLength = bytes.readUInt16LE(localOffset + 26);
    const localExtraLength = bytes.readUInt16LE(localOffset + 28);
    const dataOffset = localOffset + 30 + localNameLength + localExtraLength;
    if (dataOffset + compressedSize > bytes.length) fail('invalid-zip', `${name}: entry data out of bounds`);
    const payload = bytes.subarray(dataOffset, dataOffset + compressedSize);
    if (payload.length !== uncompressedSize) fail('invalid-zip', `${name}: truncated entry`);
    if (crc32(payload) !== crc) fail('invalid-zip', `${name}: CRC32 mismatch`);
    entries.set(name, { bytes: Buffer.from(payload), crc32: crc, method });
    offset += 46 + nameLength + extraLength + commentLength;
  }
  return entries;
}

/** Validate a whole package directory (descriptor + extracted payloads). */
export function validatePackageDirectory(packageDir, options = {}) {
  const root = path.resolve(packageDir);
  const descriptorPath = path.join(root, 'package.json');
  if (!fs.existsSync(descriptorPath)) fail('missing-descriptor', 'package.json is missing');
  const descriptorBytes = fs.readFileSync(descriptorPath);
  if (descriptorBytes[0] === 0xef && descriptorBytes[1] === 0xbb && descriptorBytes[2] === 0xbf) {
    fail('invalid-descriptor', 'package.json must not carry a UTF-8 BOM');
  }
  let descriptor;
  try {
    descriptor = JSON.parse(descriptorBytes.toString('utf8'));
  } catch (error) {
    fail('invalid-descriptor', `package.json is not valid JSON: ${error.message}`);
  }
  validatePackageDescriptor(descriptor, options);
  const report = validatePackageFiles(descriptor, { ...options, rootDir: root });
  return { descriptor, descriptorBytes, ...report };
}

export default {
  PACKAGE_DESCRIPTOR_SCHEMA,
  RUNTIME_PROTOCOL,
  TH128_GAME_ID,
  DATA_LAYOUT_SCHEMA,
  PackageError,
  canonicalJson,
  crc32,
  sha256Hex,
  isSafeRelativeSource,
  isSafeMountTarget,
  dataLayoutOf,
  validatePackageDescriptor,
  validatePackageFiles,
  validatePackageDirectory,
  planPackage,
  createPackage,
  buildStoredZip,
  readStoredZip,
};
