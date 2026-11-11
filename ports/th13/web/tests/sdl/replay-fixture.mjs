// Synthetic t13r input for browser regressions; no original replay bytes are stored.
export function replayFixture({frames = 1, spellId = -1, mode = 0, stage = 1} = {}) {
  const count = frames + 1;
  const plain = Buffer.alloc(0x74 + 0xc4 + count * 6 + Math.ceil(count / 30));
  plain.write('BROWSER', 0, 'ascii');
  plain[10] = mode;
  plain[0x58] = 1;
  plain.writeInt32LE(1, 0x64);
  plain.writeInt32LE(spellId, 0x70);
  const header = plain.subarray(0x74, 0x74 + 0xc4);
  header.writeUInt16LE(stage, 0);
  header.writeUInt16LE(123, 2);
  header.writeUInt32LE(count, 4);
  header.writeUInt32LE(count * 6 + Math.ceil(count / 30), 8);
  header.writeInt32LE(51200, 0x10);
  header.writeInt32LE(1, 0x20);
  header.writeInt32LE(spellId, 0x30);
  for (const [at, value] of [[0x38, 1000000], [0x3c, 1000000], [0x40, 25000000],
      [0x44, 100], [0x48, 400], [0x4c, 100], [0x50, 2], [0x5c, 2]])
    header.writeInt32LE(value, at);
  plain.fill(0xff, 0x74 + 0xc4 + frames * 6, 0x74 + 0xc4 + count * 6);
  const packed = [];
  let byte = 0, bits = 0;
  const put = bit => {
    byte = (byte << 1) | (bit & 1);
    if (++bits === 8) { packed.push(byte); byte = bits = 0; }
  };
  for (const value of plain) {
    put(1);
    for (let i = 7; i >= 0; --i) put(value >> i);
  }
  for (let i = 0; i < 14; ++i) put(0);
  if (bits) packed.push(byte << (8 - bits));
  const encrypt = (input, key, step, block) => {
    const out = Buffer.from(input);
    const remainder = input.length % block;
    const tail = remainder < block / 4 ? remainder : 0;
    let remaining = (input.length & ~1) - tail, pos = 0;
    while (remaining > 0) {
      const size = Math.min(remaining, block), base = pos;
      for (let source = base + size - 1; source >= base; source -= 2) {
        out[pos++] = input[source] ^ key; key = (key + step) & 255;
      }
      for (let source = base + size - 2; source >= base; source -= 2) {
        out[pos++] = input[source] ^ key; key = (key + step) & 255;
      }
      remaining -= size;
    }
    return out;
  };
  const body = encrypt(encrypt(Buffer.from(packed), 0x7d, 0x3a, 0x100), 0x5c, 0xe1, 0x400);
  const file = Buffer.alloc(0x24 + body.length);
  file.write('t13r', 0, 'ascii'); file.writeUInt16LE(2, 4);
  file.writeUInt32LE(file.length, 0x0c); file.writeUInt32LE(0x100, 0x10);
  file.writeUInt32LE(body.length, 0x1c); file.writeUInt32LE(plain.length, 0x20);
  body.copy(file, 0x24);
  return file;
}
