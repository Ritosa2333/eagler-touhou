const EOCD_SIGNATURE = 0x06054b50;
const CENTRAL_SIGNATURE = 0x02014b50;
const LOCAL_SIGNATURE = 0x04034b50;
const MAX_EOCD_SEARCH = 0xffff + 22;
const textDecoder = new TextDecoder("utf-8", { fatal: true });

const CRC32_TABLE = (() => {
  const table = new Uint32Array(256);
  for (let n = 0; n < 256; n++) {
    let c = n;
    for (let k = 0; k < 8; k++) c = (c & 1) ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    table[n] = c >>> 0;
  }
  return table;
})();

async function deflateStream(part) {
  try {return part.stream().pipeThrough(new DecompressionStream('deflate-raw'));}
  catch {
    const {Inflate} = await import('fflate');
    let inflater;
    return part.stream().pipeThrough(new TransformStream({
      start(controller) {inflater = new Inflate(chunk => controller.enqueue(chunk));},
      transform(chunk) {inflater.push(chunk);},
      flush() {inflater.push(new Uint8Array(), true);},
    }));
  }
}

async function inflateRaw(blobPart, maxBytes) {
  const stream = await deflateStream(blobPart);
  const reader = stream.getReader();
  const chunks = [];
  let total = 0;
  try {
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      total += value.length;
      if (total > maxBytes) throw new Error('ZIP entry size mismatch');
      chunks.push(value);
    }
  } finally {
    await reader.cancel().catch(() => {});
    reader.releaseLock();
  }
  const bytes = new Uint8Array(total);
  let offset = 0;
  for (const chunk of chunks) { bytes.set(chunk, offset); offset += chunk.length; }
  return bytes;
}

// Returns the uncompressed bytes of one ZIP entry and verifies its CRC32 and
// declared length. Both STORE (method 0) and DEFLATE (method 8) are accepted so
// a Package ZIP can trade size for decompression at import time.
export async function readStoredZipEntry(blob, entry) {
  if (!(blob instanceof Blob) || !entry || !Number.isInteger(entry.dataOffset) ||
      !Number.isInteger(entry.compressedSize) || !Number.isInteger(entry.uncompressedSize)) {
    throw new Error("invalid stored ZIP entry verification request");
  }
  const part = blob.slice(entry.dataOffset, entry.dataOffset + entry.compressedSize);
  const bytes = entry.method === 8 ? await inflateRaw(part, entry.uncompressedSize) : new Uint8Array(await part.arrayBuffer());
  if (bytes.length !== entry.uncompressedSize) throw new Error(`${entry.name}: ZIP entry size mismatch`);
  let crc = 0xffffffff;
  for (const byte of bytes) crc = CRC32_TABLE[(crc ^ byte) & 0xff] ^ (crc >>> 8);
  const actual = (crc ^ 0xffffffff) >>> 0;
  if (actual !== (entry.crc32 >>> 0)) throw new Error(`${entry.name}: ZIP CRC32 mismatch`);
  return bytes;
}

export async function verifyStoredZipEntry(blob, entry) {
  await readStoredZipEntry(blob, entry);
  return true;
}

export async function readOriginalZipEntry(blob, entry, signal) {
  const part = blob.slice(entry.dataOffset, entry.dataOffset + entry.compressedSize);
  const stream = entry.method === 8 ? await deflateStream(part) : part.stream();
  let total = 0, crc = 0xffffffff;
  const checked = stream.pipeThrough(new TransformStream({
    transform(chunk, controller) {
      total += chunk.length;
      if (total > entry.uncompressedSize) throw Error(`${entry.name}: ZIP entry exceeds declared size`);
      for (const value of chunk) crc = CRC32_TABLE[(crc ^ value) & 0xff] ^ (crc >>> 8);
      if (entry.method === 8) controller.enqueue(chunk);
    },
    flush() {
      if (total !== entry.uncompressedSize) throw Error(`${entry.name}: ZIP entry size mismatch`);
      if (((crc ^ 0xffffffff) >>> 0) !== entry.crc32) throw Error(`${entry.name}: ZIP CRC32 mismatch`);
    },
  }), {signal});
  // Blob storage keeps hundreds of megabytes of BGM out of JS byte arrays.
  const inflated = await new Response(checked).blob();
  return entry.method === 8 ? inflated : part;
}

export function isSafeStoredZipName(name) {
  if (typeof name !== "string" || !name || name.includes("\\") || name.startsWith("/") || name.includes("\0")) return false;
  return !name.split("/").some(part => !part || part === "." || part === "..");
}

function assertStoredEntry(entry, allowDeflate) {
  if (entry.flags & 0x0001) throw new Error(`${entry.name}: encrypted ZIP entries are not supported`);
  if (entry.method !== 0 && !(allowDeflate && entry.method === 8)) {
    throw new Error(`${entry.name}: ZIP entry must use STORE (method 0)${allowDeflate ? " or DEFLATE (method 8)" : ""}, not compression method ${entry.method}`);
  }
  if (entry.method === 0 && entry.compressedSize !== entry.uncompressedSize) {
    throw new Error(`${entry.name}: STORE entry size mismatch`);
  }
}

async function locateEocd(blob) {
  if (!(blob instanceof Blob) || blob.size < 22) throw new Error("invalid ZIP file");
  const tailLength = Math.min(blob.size, MAX_EOCD_SEARCH);
  const tailOffset = blob.size - tailLength;
  const tail = new Uint8Array(await blob.slice(tailOffset).arrayBuffer());
  const view = new DataView(tail.buffer, tail.byteOffset, tail.byteLength);
  for (let offset = tail.byteLength - 22; offset >= 0; offset--) {
    if (view.getUint32(offset, true) !== EOCD_SIGNATURE) continue;
    const commentLength = view.getUint16(offset + 20, true);
    if (offset + 22 + commentLength !== tail.byteLength) continue;
    const disk = view.getUint16(offset + 4, true);
    const centralDisk = view.getUint16(offset + 6, true);
    const entriesOnDisk = view.getUint16(offset + 8, true);
    const totalEntries = view.getUint16(offset + 10, true);
    const centralSize = view.getUint32(offset + 12, true);
    const centralOffset = view.getUint32(offset + 16, true);
    if (disk !== 0 || centralDisk !== 0 || entriesOnDisk !== totalEntries) throw new Error("multi-disk ZIP is not supported");
    if (totalEntries === 0xffff || centralSize === 0xffffffff || centralOffset === 0xffffffff) throw new Error("ZIP64 is not supported");
    if (centralOffset + centralSize > blob.size) throw new Error("ZIP central directory is out of bounds");
    return { totalEntries, centralSize, centralOffset };
  }
  throw new Error("ZIP end-of-central-directory record not found");
}

export async function parseStoredZip(blob, { allowDeflate = true, originalGame = false } = {}) {
  const eocd = await locateEocd(blob);
  if (originalGame && eocd.centralSize > 8 * 1024 * 1024) throw new Error('ZIP directory is too large');
  const centralBytes = new Uint8Array(await blob.slice(eocd.centralOffset, eocd.centralOffset + eocd.centralSize).arrayBuffer());
  const view = new DataView(centralBytes.buffer, centralBytes.byteOffset, centralBytes.byteLength);
  const entries = new Map();
  let offset = 0;
  for (let index = 0; index < eocd.totalEntries; index++) {
    if (offset + 46 > centralBytes.byteLength || view.getUint32(offset, true) !== CENTRAL_SIGNATURE) {
      throw new Error("invalid ZIP central directory entry");
    }
    const flags = view.getUint16(offset + 8, true);
    const method = view.getUint16(offset + 10, true);
    const crc32 = view.getUint32(offset + 16, true);
    const compressedSize = view.getUint32(offset + 20, true);
    const uncompressedSize = view.getUint32(offset + 24, true);
    const nameLength = view.getUint16(offset + 28, true);
    const extraLength = view.getUint16(offset + 30, true);
    const commentLength = view.getUint16(offset + 32, true);
    const localOffset = view.getUint32(offset + 42, true);
    if (compressedSize === 0xffffffff || uncompressedSize === 0xffffffff || localOffset === 0xffffffff) {
      throw new Error("ZIP64 entries are not supported");
    }
    const next = offset + 46 + nameLength + extraLength + commentLength;
    if (next > centralBytes.byteLength) throw new Error("ZIP central directory entry is truncated");
    const rawName = centralBytes.subarray(offset + 46, offset + 46 + nameLength);
    let name;
    try { name = textDecoder.decode(rawName); }
    catch (error) {
      if (!originalGame || (flags & 0x800)) throw error;
      // Legacy game ZIPs can contain Japanese or Chinese paths.
      try {name = new TextDecoder('shift_jis', {fatal: true}).decode(rawName);}
      catch {name = new TextDecoder('gb18030').decode(rawName);}
    }
    if (originalGame && name.endsWith('/') && !uncompressedSize && isSafeStoredZipName(name.slice(0, -1))) {
      offset = next;
      continue;
    }
    if (!isSafeStoredZipName(name)) throw new Error(`unsafe ZIP entry name: ${name}`);
    if (entries.has(name)) throw new Error(`duplicate ZIP entry: ${name}`);
    const entry = { name, flags, method, crc32, compressedSize, uncompressedSize, localOffset };
    assertStoredEntry(entry, allowDeflate);

    const localBytes = new Uint8Array(await blob.slice(localOffset, localOffset + 30).arrayBuffer());
    if (localBytes.byteLength !== 30) throw new Error(`${name}: truncated ZIP local header`);
    const localView = new DataView(localBytes.buffer, localBytes.byteOffset, localBytes.byteLength);
    if (localView.getUint32(0, true) !== LOCAL_SIGNATURE) throw new Error(`${name}: invalid ZIP local header`);
    if (localView.getUint16(8, true) !== method) throw new Error(`${name}: local/central compression method mismatch`);
    const localNameLength = localView.getUint16(26, true);
    const localExtraLength = localView.getUint16(28, true);
    const dataOffset = localOffset + 30 + localNameLength + localExtraLength;
    if (dataOffset + compressedSize > blob.size) throw new Error(`${name}: ZIP entry data is out of bounds`);
    entry.dataOffset = dataOffset;
    entries.set(name, entry);
    offset = next;
  }
  if (offset > centralBytes.byteLength) throw new Error("invalid ZIP central directory length");
  return entries;
}
