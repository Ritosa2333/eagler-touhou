#!/usr/bin/env node
// 生成 rgba64_png.hpp：64×64、8-bit RGBA 的 PNG 字节数组，供 anm_resource_test 的
// png_decode_stb 使用。用法：node tests/anm/make-rgba64-png.mjs
import {deflateSync} from 'node:zlib';
import {writeFileSync} from 'node:fs';
import {dirname, join} from 'node:path';
import {fileURLToPath} from 'node:url';

const size = 64;
const stride = 1 + size * 4;  // 每行行首一个过滤字节（0 = None）
const raw = Buffer.alloc(stride * size);
for (let y = 0; y < size; y++) {
  for (let x = 0; x < size; x++) {
    const at = y * stride + 1 + x * 4;
    raw[at] = x * 4;
    raw[at + 1] = y * 4;
    raw[at + 2] = (x * y) & 255;
    raw[at + 3] = 255 - y * 3;
  }
}

const crcTable = Array.from({length: 256}, (_, n) => {
  let c = n;
  for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
  return c >>> 0;
});
function crc32(buffer) {
  let c = 0xffffffff;
  for (const byte of buffer) c = crcTable[(c ^ byte) & 255] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}
function chunk(type, data = Buffer.alloc(0)) {
  const length = Buffer.alloc(4);
  length.writeUInt32BE(data.length);
  const body = Buffer.concat([Buffer.from(type, 'latin1'), data]);
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(body));
  return Buffer.concat([length, body, crc]);
}

const header = Buffer.alloc(13);
header.writeUInt32BE(size, 0);
header.writeUInt32BE(size, 4);
header[8] = 8;   // 位深
header[9] = 6;   // 颜色类型：RGBA
const png = Buffer.concat([
  Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]),
  chunk('IHDR', header), chunk('IDAT', deflateSync(raw, {level: 9})), chunk('IEND'),
]);

const bytes = [];
for (let i = 0; i < png.length; i += 16)
  bytes.push(`    ${[...png.subarray(i, i + 16)].map(byte => `0x${byte.toString(16).padStart(2, '0')}`).join(',')},`);
const text = `// 由 make-rgba64-png.mjs 生成，勿手改。
#pragma once
#include "game/Types.hpp"

namespace th13test {

// 64×64、8-bit RGBA PNG；像素 (x, y) = (x*4, y*4, (x*y)&255, 255-y*3)。
inline constexpr th13::u8 rgba64_png[] = {
${bytes.join('\n')}
};

}  // namespace th13test
`;
const output = join(dirname(fileURLToPath(import.meta.url)), 'rgba64_png.hpp');
writeFileSync(output, text);
console.log(`${output}: ${png.length} bytes`);
