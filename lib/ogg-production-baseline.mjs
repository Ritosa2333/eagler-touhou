import { createHash } from "node:crypto";

const OGG_CRC_POLYNOMIAL = 0x04c11db7;

function oggCrc(bytes, start, end) {
  let crc = 0;
  for (let index = start; index < end; index += 1) {
    crc = (crc ^ (bytes[index] << 24)) >>> 0;
    for (let bit = 0; bit < 8; bit += 1) {
      crc = crc & 0x80000000
        ? (((crc << 1) ^ OGG_CRC_POLYNOMIAL) >>> 0)
        : ((crc << 1) >>> 0);
    }
  }
  return crc >>> 0;
}

export function pinOggSerial(payload, serial) {
  if (!Number.isInteger(serial) || serial < 0 || serial > 0xffffffff) {
    throw new Error(`invalid OGG stream serial: ${serial}`);
  }
  const bytes = Buffer.from(payload);
  let offset = 0;
  while (offset < bytes.length) {
    if (offset + 27 > bytes.length || bytes.subarray(offset, offset + 4).toString("ascii") !== "OggS") {
      throw new Error(`invalid OGG page at byte ${offset}`);
    }
    const segments = bytes[offset + 26];
    const headerEnd = offset + 27 + segments;
    if (headerEnd > bytes.length) throw new Error(`truncated OGG segment table at byte ${offset}`);
    let bodyBytes = 0;
    for (let index = offset + 27; index < headerEnd; index += 1) bodyBytes += bytes[index];
    const pageEnd = headerEnd + bodyBytes;
    if (pageEnd > bytes.length) throw new Error(`truncated OGG page at byte ${offset}`);
    bytes.writeUInt32LE(serial, offset + 14);
    bytes.writeUInt32LE(0, offset + 22);
    bytes.writeUInt32LE(oggCrc(bytes, offset, pageEnd), offset + 22);
    offset = pageEnd;
  }
  return bytes;
}

export function assertOggProductionBaseline(payload, expected, label = "OGG output") {
  if (!expected || !Number.isInteger(expected.bytes) || !/^[a-f0-9]{64}$/i.test(expected.sha256 || "")) {
    throw new Error(`invalid production OGG baseline for ${label}`);
  }
  const bytes = Buffer.from(payload);
  const sha256 = createHash("sha256").update(bytes).digest("hex");
  if (bytes.length !== expected.bytes || sha256 !== expected.sha256.toLowerCase()) {
    throw new Error(`${label} does not match production OGG baseline (${bytes.length} bytes, ${sha256})`);
  }
  return Object.freeze({ bytes: bytes.length, sha256 });
}
