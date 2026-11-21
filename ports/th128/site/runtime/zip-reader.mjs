import { crc32 } from './crc32.mjs';

const SIG_EOCD = 0x06054b50;
const SIG_CENTRAL = 0x02014b50;
const SIG_LOCAL = 0x04034b50;
const MAX_COMMENT = 65535;

export class ZipError extends Error {
  constructor(code, message) {
    super(message ?? code);
    this.name = 'ZipError';
    this.code = code;
  }
}

function fail(code, message) {
  throw new ZipError(code, message);
}

function decodeName(bytes) {
  try {
    return new TextDecoder('utf-8', { fatal: true }).decode(bytes);
  } catch {
    return fail('ENTRY_NAME_ENCODING', 'entry name is not valid UTF-8');
  }
}

/**
 * Strict ZIP reader: single disk, no ZIP64, no encryption, no data descriptors,
 * STORE (0) or DEFLATE (8) only, central directory must end at EOCD, no trailing
 * bytes and no overlapping payload regions. Returns entry metadata with byte ranges.
 */
export function readZipDirectory(bytes) {
  if (!(bytes instanceof Uint8Array)) fail('INPUT_TYPE', 'zip input must be Uint8Array');
  if (bytes.length < 22) fail('TRUNCATED', 'file is too short to be a ZIP archive');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);

  let eocd = -1;
  const first = Math.max(0, bytes.length - (22 + MAX_COMMENT));
  for (let i = bytes.length - 22; i >= first; i -= 1) {
    if (view.getUint32(i, true) === SIG_EOCD) {
      eocd = i;
      break;
    }
  }
  if (eocd < 0) fail('EOCD_NOT_FOUND', 'end of central directory not found');

  const disk = view.getUint16(eocd + 4, true);
  const centralDisk = view.getUint16(eocd + 6, true);
  const diskEntries = view.getUint16(eocd + 8, true);
  const totalEntries = view.getUint16(eocd + 10, true);
  const centralSize = view.getUint32(eocd + 12, true);
  const centralOffset = view.getUint32(eocd + 16, true);
  const commentLength = view.getUint16(eocd + 20, true);

  if (eocd + 22 + commentLength !== bytes.length) fail('TRAILING_BYTES', 'archive has bytes after the EOCD record');
  if (disk !== 0 || centralDisk !== 0 || diskEntries !== totalEntries) fail('MULTI_DISK', 'multi-disk archives are not supported');
  if (totalEntries === 0xffff || centralSize === 0xffffffff || centralOffset === 0xffffffff) fail('ZIP64_UNSUPPORTED', 'ZIP64 archives are not supported');
  if (totalEntries === 0) fail('EMPTY_ARCHIVE', 'archive contains no entries');
  if (centralOffset + centralSize !== eocd) fail('CENTRAL_BOUNDS', 'central directory does not end at the EOCD record');

  const decoder = new TextDecoder('utf-8', { fatal: true });
  const entries = [];
  const ranges = [];
  let p = centralOffset;

  for (let index = 0; index < totalEntries; index += 1) {
    if (p + 46 > eocd) fail('TRUNCATED_CENTRAL', 'central directory entry is truncated');
    if (view.getUint32(p, true) !== SIG_CENTRAL) fail('BAD_CENTRAL_ENTRY', `bad central directory signature at ${p}`);
    const flags = view.getUint16(p + 8, true);
    const method = view.getUint16(p + 10, true);
    const entryCrc = view.getUint32(p + 16, true);
    const compressedSize = view.getUint32(p + 20, true);
    const uncompressedSize = view.getUint32(p + 24, true);
    const nameLength = view.getUint16(p + 28, true);
    const extraLength = view.getUint16(p + 30, true);
    const entryCommentLength = view.getUint16(p + 32, true);
    const localOffset = view.getUint32(p + 42, true);
    const next = p + 46 + nameLength + extraLength + entryCommentLength;
    if (next > eocd) fail('TRUNCATED_CENTRAL_NAME', 'central directory name/extra overruns the directory');

    if (flags & 0x0001) fail('ENCRYPTED_ENTRY', 'encrypted entries are not supported');
    if (flags & 0x0008) fail('DATA_DESCRIPTOR', 'streamed entries with data descriptors are not supported');
    if (method !== 0 && method !== 8) fail('METHOD_UNSUPPORTED', `compression method ${method} is not supported`);
    if (compressedSize === 0xffffffff || uncompressedSize === 0xffffffff || localOffset === 0xffffffff) fail('ZIP64_UNSUPPORTED', 'ZIP64 entry fields are not supported');
    if (localOffset + 30 > centralOffset) fail('LOCAL_BOUNDS', 'local header offset is outside the payload area');

    let name;
    try {
      name = decoder.decode(bytes.subarray(p + 46, p + 46 + nameLength));
    } catch {
      fail('ENTRY_NAME_ENCODING', 'entry name is not valid UTF-8');
    }
    if (name.length === 0) fail('EMPTY_NAME', 'entry name is empty');

    if (view.getUint32(localOffset, true) !== SIG_LOCAL) fail('BAD_LOCAL_HEADER', `bad local header signature for ${name}`);
    const localFlags = view.getUint16(localOffset + 6, true);
    const localMethod = view.getUint16(localOffset + 8, true);
    const localCrc = view.getUint32(localOffset + 14, true);
    const localCompressed = view.getUint32(localOffset + 18, true);
    const localUncompressed = view.getUint32(localOffset + 22, true);
    const localNameLength = view.getUint16(localOffset + 26, true);
    const localExtraLength = view.getUint16(localOffset + 28, true);
    if (localFlags !== flags || localMethod !== method) fail('LOCAL_CENTRAL_MISMATCH', `local header disagrees with central directory for ${name}`);
    if (localCrc !== entryCrc || localCompressed !== compressedSize || localUncompressed !== uncompressedSize) fail('LOCAL_CENTRAL_MISMATCH', `local sizes disagree with central directory for ${name}`);
    if (localNameLength !== nameLength) fail('LOCAL_CENTRAL_MISMATCH', `local name length disagrees for ${name}`);
    const dataOffset = localOffset + 30 + localNameLength + localExtraLength;
    const dataEnd = dataOffset + compressedSize;
    if (dataOffset > centralOffset || dataEnd > centralOffset) fail('PAYLOAD_OVERFLOW', `payload for ${name} overruns the central directory`);
    let localName;
    try {
      localName = decoder.decode(bytes.subarray(localOffset + 30, localOffset + 30 + localNameLength));
    } catch {
      fail('ENTRY_NAME_ENCODING', 'local entry name is not valid UTF-8');
    }
    if (localName !== name) fail('LOCAL_CENTRAL_MISMATCH', 'local header name disagrees with central directory');
    if (method === 0 && compressedSize !== uncompressedSize) fail('STORE_SIZE_MISMATCH', `stored entry ${name} has differing sizes`);

    entries.push({
      name,
      method,
      crc32: entryCrc,
      compressedSize,
      uncompressedSize,
      dataOffset,
      compressedBytes: bytes.subarray(dataOffset, dataEnd),
    });
    ranges.push([localOffset, dataEnd]);

    p = next;
  }

  if (p !== centralOffset + centralSize) fail('CENTRAL_SIZE_MISMATCH', 'central directory size disagrees with entries');
  ranges.sort((a, b) => a[0] - b[0]);
  for (let i = 1; i < ranges.length; i += 1) {
    if (ranges[i][0] < ranges[i - 1][1]) fail('OVERLAPPING_ENTRIES', 'archive entries overlap');
  }

  const names = new Set();
  for (const entry of entries) {
    if (names.has(entry.name)) fail('DUPLICATE_ENTRY', `duplicate archive entry ${entry.name}`);
    names.add(entry.name);
  }

  return { entries, eocdOffset: eocd };
}

export async function defaultInflate(compressed, { signal } = {}) {
  if (typeof DecompressionStream !== 'function') fail('INFLATE_UNAVAILABLE', "this environment has no DecompressionStream('deflate-raw')");
  if (signal?.aborted) fail('ABORTED', 'import was cancelled');
  const stream = new Blob([compressed]).stream().pipeThrough(new DecompressionStream('deflate-raw'));
  const buffer = await new Response(stream).arrayBuffer();
  return new Uint8Array(buffer);
}

/** Inflates one entry and verifies its uncompressed size and CRC-32. */
export async function readEntryData(entry, { inflate = defaultInflate, signal } = {}) {
  if (signal?.aborted) fail('ABORTED', 'import was cancelled');
  let body;
  if (entry.method === 0) {
    body = entry.compressedBytes;
  } else {
    try {
      body = await inflate(entry.compressedBytes, { signal, entry });
    } catch (error) {
      if (error && typeof error.code === 'string') throw error;
      fail('ENTRY_INFLATE_FAILED', `cannot inflate ${entry.name}: ${error?.message ?? error}`);
    }
  }
  if (!(body instanceof Uint8Array)) fail('INFLATE_TYPE', `inflate returned a non-Uint8Array for ${entry.name}`);
  if (body.length !== entry.uncompressedSize) fail('ENTRY_SIZE_MISMATCH', `uncompressed size mismatch for ${entry.name}`);
  if (crc32(body) !== entry.crc32) fail('ENTRY_CRC_MISMATCH', `CRC-32 mismatch for ${entry.name}`);
  return body;
}
