// TH128 import-package producer/validator, DAT + OGG layout (package-v3 candidate).
//
// Layout follows the pinned upstream TH15 shape instead of a /assets file dump:
//   games/th128/th128.dat            -> /th128.dat
//   games/th128/music/ogg/<n>.ogg    -> /music/<n>.ogg
//   shared/<font>.ttc|otf|ttf        -> /<font>
//   games/th128/language/<c>.zip     -> /__eagler/language/<c>.zip
// Envelope stays eagler-touhou/package/1 with game/target "th128"; components.ogg
// is the canonical TH15 music component shape. TH15 payload bytes are never
// copied and TH15's dataLayout is never reused.
//
// dataLayout follows the pinned upstream mount-contract convention:
//   sha256(JSON.stringify({files:[{filename:"/th128.dat"}]}))
// (the same formula that yields TH15's sha256-a09d77ec… constant).
//
// No runtime, no install, no network. The writer emits DEFLATE for DAT/language
// entries and STORE for OGG; readers here accept STORE and DEFLATE.

import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import zlib from 'node:zlib';

export const PACKAGE_DESCRIPTOR_SCHEMA = 'eagler-touhou/package/1';
export const RUNTIME_PROTOCOL = 'eagler-touhou/1';
export const TH128_GAME_ID = 'th128';
export const TH128_DATA_FILE_ID = 'game-data';

const ID_PATTERN = /^[a-z0-9][a-z0-9._:-]{0,127}$/i;
const SHA256_PATTERN = /^[0-9a-f]{64}$/;
const DATA_LAYOUT_PATTERN = /^sha256-[0-9a-f]{64}$/;
const OGG_MAGIC = Buffer.from('OggS', 'ascii');
const ZIP_MAGICS = [Buffer.from([0x50, 0x4b, 0x03, 0x04]), Buffer.from([0x50, 0x4b, 0x05, 0x06])];
const FONT_EXTENSIONS = new Set(['.ttc', '.otf', '.ttf']);
const FORBIDDEN_SEGMENTS = new Set(['saves', 'save', 'config', 'configs', 'replays', 'replay', 'logs', 'log', 'profiles', 'profile', 'credentials', 'secrets']);

export class PackageError extends Error {
  constructor(code, message) {
    super(`${code}: ${message}`);
    this.name = 'PackageError';
    this.code = code;
  }
}
const fail = (code, message) => { throw new PackageError(code, message); };
const isPlainObject = (v) => typeof v === 'object' && v !== null && !Array.isArray(v);

export const sha256Hex = (bytes) => crypto.createHash('sha256').update(bytes).digest('hex');
export const startsWithMagic = (bytes, magic) => Buffer.from(bytes).subarray(0, magic.length).equals(Buffer.from(magic));
export const isZipBytes = (bytes) => ZIP_MAGICS.some((magic) => startsWithMagic(bytes, magic));
export const isOggBytes = (bytes) => startsWithMagic(bytes, OGG_MAGIC);

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

// ---------- layout identity ----------

export function th128DataLayout() {
  // Runtime mount-contract identity: the "/th128.dat" mount itself, not the DAT contents.
  // The DAT bytes are pinned separately through files['game-data'].sha256 / expectedDatSha256.
  return `sha256-${sha256Hex(Buffer.from(JSON.stringify({ files: [{ filename: '/th128.dat' }] }), 'utf8'))}`;
}

/** The same formula applied to TH15's mount must reproduce the pinned upstream constant. */
export function layoutForDataFileName(filename) {
  return `sha256-${sha256Hex(Buffer.from(JSON.stringify({ files: [{ filename }] }), 'utf8'))}`;
}

export const TH15_PINNED_DATA_LAYOUT = 'sha256-a09d77ec0b3f9a3477f003011c8625924438bd5a05b71c911711186f01b19af3';

// ---------- source layout contract ----------

const DATA_SOURCE = 'games/th128/th128.dat';
const DATA_TARGET = '/th128.dat';
const MUSIC_SOURCE_PREFIX = 'games/th128/music/ogg/';
const MUSIC_TARGET_PREFIX = '/music/';
const LANGUAGE_SOURCE_PREFIX = 'games/th128/language/';
const LANGUAGE_TARGET_PREFIX = '/__eagler/language/';
const SHARED_SOURCE_PREFIX = 'shared/';

export function classifyEntry({ source, target }) {
  if (source === DATA_SOURCE && target === DATA_TARGET) return 'data';
  if (source.startsWith(MUSIC_SOURCE_PREFIX)) {
    const name = source.slice(MUSIC_SOURCE_PREFIX.length);
    if (!/^[A-Za-z0-9][A-Za-z0-9._-]*\.ogg$/.test(name)) fail('invalid-layout', `${source} is not an OGG music file name`);
    if (target !== `${MUSIC_TARGET_PREFIX}${name}`) fail('invalid-layout', `${source} must mount at ${MUSIC_TARGET_PREFIX}${name}`);
    return 'ogg';
  }
  if (source.startsWith(LANGUAGE_SOURCE_PREFIX)) {
    const name = source.slice(LANGUAGE_SOURCE_PREFIX.length);
    if (!/^[A-Za-z0-9][A-Za-z0-9._-]*\.zip$/.test(name)) fail('invalid-layout', `${source} is not a language ZIP`);
    if (target !== `${LANGUAGE_TARGET_PREFIX}${name}`) fail('invalid-layout', `${source} must mount at ${LANGUAGE_TARGET_PREFIX}${name}`);
    return 'language';
  }
  if (source.startsWith(SHARED_SOURCE_PREFIX)) {
    const name = source.slice(SHARED_SOURCE_PREFIX.length);
    if (!name || name.includes('/')) fail('invalid-layout', `${source} must be a single shared font file`);
    if (!FONT_EXTENSIONS.has(path.posix.extname(name).toLowerCase())) fail('invalid-layout', `${source} is not a shared font`);
    if (target !== `/${name}`) fail('invalid-layout', `${source} must mount at /${name}`);
    return 'shared';
  }
  return fail('invalid-layout', `${source} is outside the TH128 DAT/OGG package layout`);
}

const assertSafeMountTarget = (target, label) => {
  if (typeof target !== 'string' || !target.startsWith('/') || target.includes('\\') || target.includes('\0')) {
    fail('unsafe-target', `${label} target is not a safe absolute mount path`);
  }
  const parts = target.slice(1).split('/');
  if (parts.some((part) => part === '' || part === '.' || part === '..')) fail('unsafe-target', `${label} target is unsafe`);
  for (const part of parts) if (FORBIDDEN_SEGMENTS.has(part.toLowerCase())) fail('forbidden-payload', `${label} target contains ${part}`);
  return target;
};

const assertSafeSource = (source, label) => {
  if (typeof source !== 'string' || source.length === 0) fail('unsafe-source', `${label} source is empty`);
  if (source.includes('\\') || source.includes('\0') || source.startsWith('/') || /^[A-Za-z]:/.test(source)) {
    fail('unsafe-source', `${label} source is not a safe relative path`);
  }
  if (source.split('/').some((part) => part === '' || part === '.' || part === '..')) fail('unsafe-source', `${label} source is unsafe`);
  return source;
};

// ---------- descriptor validation ----------

export function validatePackageDescriptor(descriptor, options = {}) {
  const allowedGameIds = options.allowedGameIds ?? [TH128_GAME_ID];
  const supportedLanguages = new Set(options.supportedLanguages ?? ['zh', 'zh-hans', 'ja']);
  if (!isPlainObject(descriptor)) fail('invalid-descriptor', 'descriptor must be an object');
  if (descriptor.schema !== PACKAGE_DESCRIPTOR_SCHEMA) fail('invalid-descriptor', `schema must be ${PACKAGE_DESCRIPTOR_SCHEMA}`);
  if (typeof descriptor.game !== 'string' || !ID_PATTERN.test(descriptor.game)) fail('invalid-descriptor', 'game id is malformed');
  if (!allowedGameIds.includes(descriptor.game)) {
    fail('unsupported-game', `game "${descriptor.game}" is not a supported target (pinned upstream isGameId has no th128 entry, so this is a Runtime-extension target)`);
  }
  if (typeof descriptor.revision !== 'string' || !ID_PATTERN.test(descriptor.revision)) fail('invalid-revision', 'revision is malformed');
  if (descriptor.runtime !== undefined || descriptor.runtimes !== undefined) fail('runtime-not-allowed', 'a TH128 resource package must not carry a Runtime');
  if (!isPlainObject(descriptor.files) || Object.keys(descriptor.files).length === 0) fail('invalid-files', 'files must be a non-empty object');

  const kinds = new Map();
  const seenTargets = new Map();
  const seenSources = new Map();
  for (const id of Object.keys(descriptor.files)) {
    if (!ID_PATTERN.test(id)) fail('invalid-id', `${id} is not a safe file id`);
    const file = descriptor.files[id];
    if (!isPlainObject(file)) fail('invalid-files', `${id} must be an object`);
    if (typeof file.revision !== 'string' || !ID_PATTERN.test(file.revision)) fail('invalid-revision', `${id} revision is malformed`);
    assertSafeSource(file.source, id);
    assertSafeMountTarget(file.target, id);
    if (!Number.isSafeInteger(file.bytes) || file.bytes < 0) fail('invalid-bytes', `${id} bytes must be a non-negative integer`);
    if (typeof file.sha256 !== 'string' || !SHA256_PATTERN.test(file.sha256.toLowerCase())) fail('invalid-hash', `${id} sha256 is malformed`);
    file.sha256 = file.sha256.toLowerCase();
    if (seenTargets.has(file.target)) fail('duplicate-target', `${id} repeats target ${file.target}`);
    if (seenSources.has(file.source)) fail('duplicate-source', `${id} repeats source ${file.source}`);
    seenTargets.set(file.target, id);
    seenSources.set(file.source, id);
    kinds.set(id, classifyEntry({ source: file.source, target: file.target }));
  }

  const dataIds = Object.keys(descriptor.files).filter((id) => kinds.get(id) === 'data');
  if (dataIds.length !== 1) fail('invalid-data-file', 'exactly one games/th128/th128.dat -> /th128.dat entry is required');
  const dataId = dataIds[0];

  const requirement = descriptor.runtimeRequirement;
  if (!isPlainObject(requirement)) fail('invalid-runtime-requirement', 'runtimeRequirement is required');
  if (requirement.protocol !== RUNTIME_PROTOCOL) fail('invalid-runtime-requirement', `protocol must be ${RUNTIME_PROTOCOL}`);
  if (requirement.target !== descriptor.game) fail('invalid-runtime-requirement', 'runtimeRequirement.target must equal game');
  if (requirement.dataFile !== dataId) fail('invalid-runtime-requirement', `dataFile must name the ${DATA_TARGET} entry (${dataId})`);
  if (typeof requirement.dataLayout !== 'string' || !DATA_LAYOUT_PATTERN.test(requirement.dataLayout)) fail('invalid-data-layout', 'dataLayout must be sha256-<64 hex>');
  if (requirement.dataLayout !== th128DataLayout()) {
    fail('data-layout-mismatch', 'dataLayout must be the /th128.dat mount-contract identity');
  }
  if (requirement.dataLayout === TH15_PINNED_DATA_LAYOUT) fail('foreign-data-layout', 'TH15 dataLayout must never be reused');

  const base = descriptor.base;
  if (!isPlainObject(base) || !Array.isArray(base.files) || base.files.length === 0) fail('invalid-base', 'base.files must be a non-empty array');
  const seenBase = new Set();
  for (const id of base.files) {
    if (typeof id !== 'string' || !Object.hasOwn(descriptor.files, id)) fail('unknown-file', `base references unknown file ${String(id)}`);
    if (seenBase.has(id)) fail('duplicate-file', `base repeats ${id}`);
    seenBase.add(id);
  }
  if (!seenBase.has(dataId)) fail('invalid-base', 'base.files must include the DAT entry');
  for (const id of seenBase) {
    if (!['data', 'shared'].includes(kinds.get(id))) fail('invalid-base', `base must hold only the DAT and shared fonts, not ${id}`);
  }

  const components = descriptor.components;
  if (!isPlainObject(components)) fail('invalid-components', 'components is required (canonical ogg component)');
  const oggIds = Object.keys(descriptor.files).filter((id) => kinds.get(id) === 'ogg');
  const ogg = components.ogg;
  if (!isPlainObject(ogg) || ogg.type !== 'ogg') fail('invalid-components', 'components.ogg {type:"ogg",files:[…]} is required');
  if (ogg.loopMetadata !== undefined && !isPlainObject(ogg.loopMetadata)) fail('invalid-components', 'components.ogg.loopMetadata must be an object');
  if (!Array.isArray(ogg.files) || ogg.files.length !== oggIds.length) fail('invalid-components', 'components.ogg.files must list every OGG entry exactly once');
  const oggSet = new Set(ogg.files);
  if (oggSet.size !== ogg.files.length) fail('duplicate-file', 'components.ogg.files repeats an id');
  for (const id of oggIds) if (!oggSet.has(id)) fail('invalid-components', `components.ogg.files must include ${id}`);
  for (const id of ogg.files) {
    if (typeof id !== 'string' || !Object.hasOwn(descriptor.files, id)) fail('unknown-file', `components.ogg references unknown file ${String(id)}`);
    if (kinds.get(id) !== 'ogg') fail('invalid-components', `components.ogg may only hold OGG entries, not ${id}`);
  }

  const languageIds = Object.keys(descriptor.files).filter((id) => kinds.get(id) === 'language');
  if (components.language !== undefined) {
    if (languageIds.length === 0) fail('invalid-components', 'components.language without language payloads');
    const language = components.language;
    if (!isPlainObject(language) || language.type !== 'language' || !Array.isArray(language.entries) || language.entries.length === 0) {
      fail('invalid-components', 'components.language must be {type:"language",entries:[…]}');
    }
    const entryIds = new Set();
    for (const entry of language.entries) {
      if (!isPlainObject(entry)) fail('invalid-components', 'language entry must be an object');
      if (typeof entry.id !== 'string' || !ID_PATTERN.test(entry.id)) fail('invalid-id', 'language entry id is malformed');
      if (entryIds.has(entry.id)) fail('duplicate-entry', `language repeats ${entry.id}`);
      entryIds.add(entry.id);
      if (typeof entry.file !== 'string' || !Object.hasOwn(descriptor.files, entry.file)) fail('unknown-file', `language entry references unknown file ${String(entry.file)}`);
      if (kinds.get(entry.file) !== 'language') fail('invalid-components', `${entry.file} is not a language ZIP`);
      const code = String(entry.id).replace(/^lang[_-]/i, '').toLowerCase();
      if (!supportedLanguages.has(code)) fail('unsupported-language', `${entry.id} is not a shipped TH128 language (${[...supportedLanguages].join(', ')})`);
      if (!descriptor.files[entry.file].target.endsWith(`/${entry.id}.zip`) && !descriptor.files[entry.file].target.endsWith(`/${code}.zip`)) fail('unsupported-language', `${entry.file} target must encode language ${code}`);
      if (entry.title !== undefined && typeof entry.title !== 'string') fail('invalid-components', 'language entry title must be a string');
    }
    if (entryIds.size !== languageIds.length) fail('invalid-components', 'every language ZIP needs exactly one entry');
  } else if (languageIds.length > 0) {
    fail('invalid-components', 'language payloads require components.language entries');
  }
  return descriptor;
}

// ---------- payload validation ----------

export function validatePackageFiles(descriptor, { rootDir, allowOutsideRoot = false, expectedDataMagic = null, expectedDatSha256 = null } = {}) {
  validatePackageDescriptor(descriptor);
  const root = path.resolve(rootDir ?? '.');
  const problems = [];
  const checked = [];
  for (const id of Object.keys(descriptor.files).sort()) {
    const file = descriptor.files[id];
    const absolute = path.resolve(root, file.source);
    const rel = path.relative(root, absolute);
    const inside = rel === '' || (!rel.startsWith('..') && !path.isAbsolute(rel));
    if (!inside && !allowOutsideRoot) { problems.push({ id, code: 'unsafe-source', message: 'source escapes the package root' }); continue; }
    if (!fs.existsSync(absolute)) { problems.push({ id, code: 'missing-source', message: `missing ${file.source}` }); continue; }
    const bytes = fs.readFileSync(absolute);
    if (bytes.length !== file.bytes) { problems.push({ id, code: 'size-mismatch', message: `${bytes.length} != ${file.bytes}` }); continue; }
    const actual = sha256Hex(bytes);
    if (actual !== file.sha256) { problems.push({ id, code: 'hash-mismatch', message: `${actual} != ${file.sha256}` }); continue; }
    const kind = classifyEntry({ source: file.source, target: file.target });
    if (kind === 'data') {
      if (isZipBytes(bytes)) { problems.push({ id, code: 'dat-is-zip', message: 'a ZIP renamed to .dat is not an original data archive' }); continue; }
      if (isOggBytes(bytes)) { problems.push({ id, code: 'dat-is-ogg', message: 'an OGG file renamed to .dat is not an original data archive' }); continue; }
      if (expectedDataMagic && !startsWithMagic(bytes, Buffer.from(expectedDataMagic, 'hex'))) {
        problems.push({ id, code: 'dat-magic-mismatch', message: `expected magic ${expectedDataMagic}` }); continue;
      }
      if (expectedDatSha256 && actual !== expectedDatSha256.toLowerCase()) {
        problems.push({ id, code: 'dat-hash-mismatch', message: 'DAT bytes differ from the MAIN-supplied identity' }); continue;
      }
    }
    if (kind === 'ogg' && !isOggBytes(bytes)) { problems.push({ id, code: 'not-ogg', message: 'music entry must start with OggS' }); continue; }
    if (kind === 'language' && !isZipBytes(bytes)) { problems.push({ id, code: 'not-zip', message: 'language entry must be a ZIP' }); continue; }
    checked.push({ id, kind, bytes: bytes.length, sha256: actual });
  }
  return { ok: problems.length === 0, problems, checked };
}

export function validatePackageDirectory(packageDir, options = {}) {
  const root = path.resolve(packageDir);
  const descriptorPath = path.join(root, 'package.json');
  if (!fs.existsSync(descriptorPath)) fail('missing-descriptor', 'package.json is missing');
  const raw = fs.readFileSync(descriptorPath);
  if (raw[0] === 0xef && raw[1] === 0xbb && raw[2] === 0xbf) fail('invalid-descriptor', 'package.json must not carry a UTF-8 BOM');
  let descriptor;
  try { descriptor = JSON.parse(raw.toString('utf8')); } catch (error) { fail('invalid-descriptor', `package.json is not valid JSON: ${error.message}`); }
  validatePackageDescriptor(descriptor, options);
  return { descriptor, descriptorBytes: raw, ...validatePackageFiles(descriptor, { ...options, rootDir: root }) };
}

// ---------- producer ----------

export function planPackage({ input, rootDir, options = {} }) {
  if (!isPlainObject(input)) fail('invalid-input', 'input must be an object');
  if (!Array.isArray(input.files) || input.files.length === 0) fail('invalid-input', 'input.files must be a non-empty array');
  const game = input.game ?? TH128_GAME_ID;
  const allowedGameIds = options.allowedGameIds ?? [TH128_GAME_ID];
  if (!allowedGameIds.includes(game)) fail('unsupported-game', `game ${game} is not an allowed target`);
  const root = path.resolve(rootDir ?? '.');

  const files = {};
  const payloads = [];
  const kinds = {};
  for (const [index, entry] of input.files.entries()) {
    if (!isPlainObject(entry)) fail('invalid-input', `input.files[${index}] must be an object`);
    const { id, source, target, sourceFile } = entry;
    if (typeof id !== 'string' || !ID_PATTERN.test(id)) fail('invalid-id', `input.files[${index}].id is malformed`);
    assertSafeSource(source, id);
    assertSafeMountTarget(target, id);
    const kind = classifyEntry({ source, target });
    if (kind === 'data' && payloads.some((p) => kinds[p.id] === 'data')) fail('invalid-data-file', 'only one DAT entry is allowed');
    const absolute = path.resolve(root, sourceFile ?? source);
    const rel = path.relative(root, absolute);
    if ((rel.startsWith('..') || path.isAbsolute(rel)) && !options.allowOutsideRoot) fail('unsafe-source', `${id} sourceFile escapes the package root`);
    if (!fs.existsSync(absolute) || !fs.statSync(absolute).isFile()) fail('missing-source', `${id} sourceFile does not exist`);
    const bytes = fs.readFileSync(absolute);
    if (bytes.length === 0) fail('empty-source', `${id} is empty`);
    if (kind === 'data' && isZipBytes(bytes)) fail('dat-is-zip', `${id} looks like a ZIP; a renamed asset dump is not the original DAT`);
    if (kind === 'data' && isOggBytes(bytes)) fail('dat-is-ogg', `${id} looks like OGG, not the original DAT`);
    if (kind === 'ogg' && !isOggBytes(bytes)) fail('not-ogg', `${id} must start with OggS`);
    if (kind === 'language' && !isZipBytes(bytes)) fail('not-zip', `${id} must be a language ZIP`);
    const sha256 = sha256Hex(bytes);
    if (kind === 'data' && options.expectedDatSha256 && sha256 !== String(options.expectedDatSha256).toLowerCase()) {
      fail('dat-hash-mismatch', `${id} differs from the MAIN-supplied original DAT sha256`);
    }
    files[id] = { source, target, revision: sha256.slice(0, 16), bytes: bytes.length, sha256 };
    kinds[id] = kind;
    payloads.push({ id, kind, bytes, sha256 });
  }

  const dataId = Object.keys(files).find((id) => kinds[id] === 'data');
  if (!dataId) fail('invalid-data-file', `input must contain ${DATA_SOURCE} -> ${DATA_TARGET}`);
  const sharedIds = Object.keys(files).filter((id) => kinds[id] === 'shared').sort();
  const oggIds = Object.keys(files).filter((id) => kinds[id] === 'ogg').sort();
  const languageIds = Object.keys(files).filter((id) => kinds[id] === 'language').sort();

  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA,
    game,
    revision: input.revision ?? sha256Hex(Buffer.from(JSON.stringify({ game, files }), 'utf8')).slice(0, 16),
    title: input.title ?? 'TH128 original data and OGG music',
    files,
    base: { files: [dataId, ...sharedIds] },
    runtimeRequirement: { protocol: RUNTIME_PROTOCOL, target: game, dataFile: dataId, dataLayout: th128DataLayout() },
    components: { ogg: { type: 'ogg', files: oggIds } },
  };
  // MAIN keeps OGG loop metadata separately; carry it verbatim when supplied.
  if (input.loopMetadata !== undefined) {
    if (!isPlainObject(input.loopMetadata)) fail('invalid-input', 'loopMetadata must be an object');
    descriptor.components.ogg.loopMetadata = input.loopMetadata;
  }
  if (languageIds.length > 0) {
    if (!Array.isArray(input.languageEntries)) fail('invalid-input', 'language payloads require input.languageEntries');
    descriptor.components.language = { type: 'language', entries: input.languageEntries.map((entry) => ({ ...entry })) };
  }
  validatePackageDescriptor(descriptor, options);
  return { descriptor, payloads, dataId };
}

// ---------- deterministic ZIP (DEFLATE for dat/language, STORE for ogg) ----------

const EOCD_SIGNATURE = 0x06054b50;
const CENTRAL_SIGNATURE = 0x02014b50;
const LOCAL_SIGNATURE = 0x04034b50;
const DOS_DATE = 0x0021;
const DOS_TIME = 0x0000;

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

const compressionForName = (name) => (/\.(dat|zip)$/i.test(name) ? 8 : 0);

export function buildPackageZip(entries) {
  const prepared = entries.map((entry) => {
    if (!isPlainObject(entry) || typeof entry.name !== 'string') fail('invalid-zip', 'entry needs a name');
    assertSafeSource(entry.name, entry.name);
    const raw = Buffer.from(entry.bytes ?? new Uint8Array());
    const method = compressionForName(entry.name);
    const body = method === 8 ? zlib.deflateRawSync(raw, { level: 9 }) : raw;
    return { name: Buffer.from(entry.name, 'ascii'), raw, body, method, crc: crc32(raw) };
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
    local.writeUInt16LE(entry.method, 8);
    local.writeUInt16LE(DOS_TIME, 10);
    local.writeUInt16LE(DOS_DATE, 12);
    local.writeUInt32LE(entry.crc, 14);
    local.writeUInt32LE(entry.body.length, 18);
    local.writeUInt32LE(entry.raw.length, 22);
    local.writeUInt16LE(entry.name.length, 26);
    local.writeUInt16LE(0, 28);
    chunks.push(local, entry.name, entry.body);
    const cd = Buffer.alloc(46);
    cd.writeUInt32LE(CENTRAL_SIGNATURE, 0);
    cd.writeUInt16LE(20, 4);
    cd.writeUInt16LE(20, 6);
    cd.writeUInt16LE(0, 8);
    cd.writeUInt16LE(entry.method, 10);
    cd.writeUInt16LE(DOS_TIME, 12);
    cd.writeUInt16LE(DOS_DATE, 14);
    cd.writeUInt32LE(entry.crc, 16);
    cd.writeUInt32LE(entry.body.length, 20);
    cd.writeUInt32LE(entry.raw.length, 24);
    cd.writeUInt16LE(entry.name.length, 28);
    cd.writeUInt16LE(0, 30);
    cd.writeUInt16LE(0, 32);
    cd.writeUInt16LE(0, 34);
    cd.writeUInt16LE(0, 36);
    cd.writeUInt32LE(0, 38);
    cd.writeUInt32LE(offset, 42);
    central.push(Buffer.concat([cd, entry.name]));
    offset += 30 + entry.name.length + entry.body.length;
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
  return { zip: Buffer.concat([...chunks, centralBytes, eocd]), methods: Object.fromEntries(prepared.map((e) => [e.name.toString(), e.method])) };
}

/** Read STORE/DEFLATE entries and verify CRC32 + sizes. */
export function readPackageZip(zipBytes) {
  const bytes = Buffer.from(zipBytes);
  if (bytes.length < 22) fail('invalid-zip', 'ZIP is too short');
  let eocd = -1;
  for (let i = bytes.length - 22; i >= 0; i -= 1) {
    if (bytes.readUInt32LE(i) === EOCD_SIGNATURE && i + 22 === bytes.length) { eocd = i; break; }
  }
  if (eocd < 0) fail('invalid-zip', 'ZIP end-of-central-directory record not found');
  const total = bytes.readUInt16LE(eocd + 10);
  const centralSize = bytes.readUInt32LE(eocd + 12);
  const centralOffset = bytes.readUInt32LE(eocd + 16);
  if (centralOffset + centralSize > bytes.length) fail('invalid-zip', 'ZIP central directory is out of bounds');
  const entries = new Map();
  let offset = centralOffset;
  for (let index = 0; index < total; index += 1) {
    if (offset + 46 > bytes.length || bytes.readUInt32LE(offset) !== CENTRAL_SIGNATURE) fail('invalid-zip', 'invalid central entry');
    const method = bytes.readUInt16LE(offset + 10);
    const crc = bytes.readUInt32LE(offset + 16);
    const compressedSize = bytes.readUInt32LE(offset + 20);
    const uncompressedSize = bytes.readUInt32LE(offset + 24);
    const nameLength = bytes.readUInt16LE(offset + 28);
    const extraLength = bytes.readUInt16LE(offset + 30);
    const commentLength = bytes.readUInt16LE(offset + 32);
    const localOffset = bytes.readUInt32LE(offset + 42);
    const name = bytes.subarray(offset + 46, offset + 46 + nameLength).toString('ascii');
    if (!name || name.includes('\\') || name.startsWith('/')) fail('unsafe-zip-name', name);
    if (entries.has(name)) fail('duplicate-zip-name', name);
    if (method !== 0 && method !== 8) fail('invalid-zip', `${name}: unsupported method ${method}`);
    if (bytes.readUInt32LE(localOffset) !== LOCAL_SIGNATURE) fail('invalid-zip', `${name}: invalid local header`);
    const localNameLength = bytes.readUInt16LE(localOffset + 26);
    const localExtraLength = bytes.readUInt16LE(localOffset + 28);
    const dataOffset = localOffset + 30 + localNameLength + localExtraLength;
    if (dataOffset + compressedSize > bytes.length) fail('invalid-zip', `${name}: data out of bounds`);
    const body = bytes.subarray(dataOffset, dataOffset + compressedSize);
    const raw = method === 8 ? zlib.inflateRawSync(body) : Buffer.from(body);
    if (raw.length !== uncompressedSize) fail('invalid-zip', `${name}: size mismatch`);
    if (crc32(raw) !== crc) fail('invalid-zip', `${name}: CRC32 mismatch`);
    entries.set(name, { bytes: raw, method, crc32: crc });
    offset += 46 + nameLength + extraLength + commentLength;
  }
  return entries;
}

export function createPackage({ input, rootDir, outDir, zipName = null, options = {} }) {
  const out = path.resolve(outDir);
  if (fs.existsSync(out) && fs.readdirSync(out).length > 0) fail('output-exists', `refusing to overwrite ${out}`);
  const plan = planPackage({ input, rootDir, options });
  const descriptorBytes = Buffer.from(canonicalJson(plan.descriptor), 'utf8');
  fs.mkdirSync(out, { recursive: true });
  const extracted = path.join(out, 'extracted');
  fs.mkdirSync(extracted, { recursive: true });
  fs.writeFileSync(path.join(extracted, 'package.json'), descriptorBytes);
  const entries = [{ name: 'package.json', bytes: descriptorBytes }];
  for (const payload of plan.payloads) {
    const file = plan.descriptor.files[payload.id];
    const destination = path.join(extracted, ...file.source.split('/'));
    fs.mkdirSync(path.dirname(destination), { recursive: true });
    fs.writeFileSync(destination, payload.bytes);
    entries.push({ name: file.source, bytes: payload.bytes });
  }
  const built = buildPackageZip(entries);
  const zipPath = path.join(out, zipName ?? `th128-dat-ogg-${plan.descriptor.revision}.zip`);
  fs.writeFileSync(zipPath, built.zip);
  return {
    descriptor: plan.descriptor,
    descriptorBytes,
    zipPath,
    zipBytes: built.zip.length,
    zipSha256: sha256Hex(built.zip),
    methods: built.methods,
    outDir: out,
    entries: entries.map((entry) => ({ name: entry.name, bytes: entry.bytes.length, sha256: sha256Hex(entry.bytes) })),
  };
}

export default {
  PACKAGE_DESCRIPTOR_SCHEMA,
  RUNTIME_PROTOCOL,
  TH128_GAME_ID,
  TH128_DATA_FILE_ID,
  TH15_PINNED_DATA_LAYOUT,
  PackageError,
  canonicalJson,
  crc32,
  sha256Hex,
  isZipBytes,
  isOggBytes,
  th128DataLayout,
  layoutForDataFileName,
  classifyEntry,
  validatePackageDescriptor,
  validatePackageFiles,
  validatePackageDirectory,
  planPackage,
  createPackage,
  buildPackageZip,
  readPackageZip,
};
