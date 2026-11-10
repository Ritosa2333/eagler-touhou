// PBG and THA archive conventions match launcher/scripts/touhou_formats.py
// and the thtk reference in ports/th15/web/reference/tools/thtk.
const decoder = new TextDecoder('shift_jis');
const tableLimit = 8 * 1024 * 1024;
const u32 = (bytes, at) => new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(at, true);
const magic = (bytes, value) => [...value].every((c, i) => bytes[i] === c.charCodeAt(0));

function decrypt(input, key, step, block, limit = input.length) {
  const result = input.slice();
  const tail = input.length % block;
  const length = Math.min(input.length - (tail < block / 4 ? tail : 0) - (input.length & 1), Math.ceil(limit / block) * block);
  for (let start = 0; start < length; start += block) {
    const size = Math.min(block, length - start);
    let source = start;
    for (let parity = 1; parity >= 0; parity--) {
      for (let i = size - 2 + parity; i >= 0; i -= 2) {
        result[start + i] = input[source++] ^ (key & 255);
        key += step;
      }
    }
  }
  return result;
}

function lzss(bytes, size) {
  if (size <= 0 || size > tableLimit) throw Error('Archive table or BGM index exceeds the supported size');
  let bit = 0;
  function read(count) {
    if (bit + count > bytes.length * 8) throw Error('Truncated archive compression stream');
    let value = 0;
    while (count--) {
      value = (value << 1) | ((bytes[bit >> 3] >> (7 - (bit & 7))) & 1);
      bit++;
    }
    return value;
  }
  const ring = new Uint8Array(8192), output = new Uint8Array(size);
  let write = 0, head = 1;
  while (write < size) {
    if (read(1)) {
      output[write++] = ring[head++ & 8191] = read(8);
      continue;
    }
    const offset = read(13), length = read(4) + 3;
    if (!offset || write + length > size) throw Error('Invalid archive compression span');
    for (let i = 0; i < length; i++) output[write++] = ring[head++ & 8191] = ring[(offset + i) & 8191];
  }
  return output;
}

const earlyKeys = [
  [0x1b, 0x37, 0x40, 0x2800], [0x51, 0xe9, 0x40, 0x3000], [0xc1, 0x51, 0x80, 0x3200], [0x03, 0x19, 0x400, 0x7800],
  [0xab, 0xcd, 0x200, 0x2800], [0x12, 0x34, 0x80, 0x3200], [0x35, 0x97, 0x80, 0x2800], [0x99, 0x37, 0x400, 0x2000],
];
const lateKeys = [
  [0x1b, 0x73, 0x100, 0x3800], [0x12, 0x43, 0x200, 0x3e00], [0x35, 0x79, 0x400, 0x3c00], [0x03, 0x91, 0x80, 0x6400],
  [0xab, 0xdc, 0x80, 0x7000], [0x51, 0x9e, 0x100, 0x4000], [0xc1, 0x15, 0x400, 0x2c00], [0x99, 0x7d, 0x80, 0x4400],
];
const pbgzKeys = {
  M: [0x1b, 0x37, 0x40, 0x2000], T: [0x51, 0xe9, 0x40, 0x3000], A: [0xc1, 0x51, 0x1400, 0x2000], J: [0x03, 0x19, 0x1400, 0x7800],
  E: [0xab, 0xcd, 0x200, 0x1000], W: [0x12, 0x34, 0x400, 0x2800], '-': [0x35, 0x97, 0x80, 0x2800], '*': [0x99, 0x37, 0x400, 0x1000],
};

async function bytes(blob, start, end) {
  return new Uint8Array(await blob.slice(start, end).arrayBuffer());
}

export async function hasRetailDatHeader(blob) {
  const header = await bytes(blob, 0, 16);
  return header.length === 16 && (['PBG3', 'PBG4', 'PBGZ'].some(tag => magic(header, tag)) ||
    magic(decrypt(header, 0x1b, 0x37, 16), 'THA1'));
}

export async function readRetailArchive(blob, game) {
  const header = await bytes(blob, 0, 16);
  if (header.length !== 16) throw Error('Truncated game DAT');
  if (game === 'th06') {
    if (!magic(header, 'PBG3')) throw Error('Expected a TH06 PBG3 archive');
    return [];
  }
  const pbg4 = game === 'th07', pbgz = game === 'th08' || game === 'th09';
  let count, offset, size, table;
  if (pbg4) {
    if (!magic(header, 'PBG4')) throw Error('Expected a TH07 PBG4 archive');
    count = u32(header, 4); offset = u32(header, 8); size = u32(header, 12);
  } else if (pbgz) {
    if (!magic(header, 'PBGZ')) throw Error('Expected a PBGZ game archive');
    const meta = decrypt(header.subarray(4), 0x1b, 0x37, 12);
    count = u32(meta, 0) - 123456; offset = u32(meta, 4) - 345678; size = u32(meta, 8) - 567891;
  } else {
    const meta = decrypt(header, 0x1b, 0x37, 16);
    if (!magic(meta, 'THA1')) throw Error('Expected a THA1 game archive');
    size = u32(meta, 4) - 123456789;
    offset = blob.size - (u32(meta, 8) - 987654321);
    count = u32(meta, 12) - 135792468;
  }
  if (count <= 0 || count > 65535 || offset < 16 || blob.size - offset > tableLimit || offset >= blob.size)
    throw Error('Invalid game DAT directory');
  const packed = await bytes(blob, offset, blob.size);
  table = lzss(pbg4 ? packed : decrypt(packed, 0x3e, 0x9b, 0x80, pbgz ? 0x400 : packed.length), size);
  const entries = [];
  let cursor = 0;
  for (let i = 0; i < count; i++) {
    const end = table.indexOf(0, cursor);
    if (end <= cursor || end - cursor > 255) throw Error('Invalid game DAT filename');
    const nameBytes = table.subarray(cursor, end);
    const name = decoder.decode(nameBytes).replaceAll('\\', '/').toLowerCase();
    const keyIndex = nameBytes.reduce((sum, value) => sum + value, 0) & 7;
    cursor = pbg4 || pbgz ? end + 1 : (end + 4) & ~3;
    if (cursor + 12 > table.length) throw Error('Truncated game DAT directory');
    const start = u32(table, cursor), length = u32(table, cursor + 4);
    cursor += 12;
    if (start < 16 || start >= offset || start < (entries.at(-1)?.start ?? 16)) throw Error('Invalid game DAT entry offset');
    entries.push({name, start, length, keyIndex});
  }
  return entries.map((entry, index) => ({...entry, end: entries[index + 1]?.start ?? offset}));
}

export async function readRetailBgmIndex(blob, game, entries) {
  const entry = entries.find(item => item.name.split('/').at(-1) === 'thbgm.fmt');
  if (!entry || entry.end - entry.start > tableLimit || entry.length > tableLimit) throw Error('Game DAT has no usable thbgm.fmt');
  const packed = await bytes(blob, entry.start, entry.end);
  if (game === 'th07') return lzss(packed, entry.length);
  if (game === 'th08' || game === 'th09') {
    const content = lzss(packed, entry.length);
    if (!magic(content, 'edz')) return content;
    const type = String.fromCharCode(content[3]);
    let key = pbgzKeys[type];
    if (!key) throw Error('Unknown PBGZ resource key');
    if (game === 'th09' && 'AJW'.includes(type)) key = [...key.slice(0, 2), 0x400, 0x400];
    if (game === 'th09' && type === 'M') key = [...key.slice(0, 3), 0x2800];
    return decrypt(content.subarray(4), ...key);
  }
  const content = decrypt(packed, ...(game === 'th10' || game === 'th11' ? earlyKeys : lateKeys)[entry.keyIndex]);
  return content.length === entry.length ? content : lzss(content, entry.length);
}

export function parseBgmTracks(index, size) {
  const tracks = [];
  for (let at = 0; at + 52 <= index.length && index[at]; at += 52) {
    const name = decoder.decode(index.subarray(at, at + 16)).split('\0', 1)[0].toLowerCase();
    const offset = u32(index, at + 16), intro = u32(index, at + 24), length = u32(index, at + 28);
    const format = u32(index, at + 32), rate = u32(index, at + 36);
    if (!/^[a-z0-9_]+\.wav$/.test(name) || format !== 0x20001 || rate !== 44100 ||
        !length || length > 128 * 1024 * 1024 || (offset | intro | length) % 4 || intro >= length || offset + length > size || tracks.length >= 128)
      throw Error(`Invalid thbgm.fmt track: ${name}`);
    tracks.push({name: name.replace(/\.wav$/, '.ogg'), offset, length, intro});
  }
  if (!tracks.length || new Set(tracks.map(track => track.name)).size !== tracks.length) throw Error('Invalid or empty thbgm.fmt');
  return tracks;
}
