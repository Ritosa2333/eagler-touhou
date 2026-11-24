// 普通版与多人版共用同一份语义 GLES 渲染后端（portable/sdl）。两侧的文件
// 集合与逐字节内容必须一致：任何一侧单独改动都会让两种模式的绘制语义分叉。
import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {join} from 'node:path';

const single = fileURLToPath(new URL('../portable/sdl/', import.meta.url));
const multiplayer = fileURLToPath(new URL('../../th13-mp/portable/sdl/', import.meta.url));

function inventory(root, prefix = '') {
  return readdirSync(root, {withFileTypes: true}).flatMap(entry => {
    const path = join(root, entry.name), name = prefix + entry.name;
    if (entry.isDirectory()) return inventory(path, name + '/');
    assert.ok(entry.isFile(), `portable/sdl 只应包含普通文件：${name}`);
    return [[name, createHash('sha256').update(readFileSync(path)).digest('hex')]];
  }).sort(([a], [b]) => a < b ? -1 : a > b ? 1 : 0);
}

test('portable/sdl is byte-identical between th13 and th13-mp', t => {
  if (!existsSync(multiplayer)) return t.skip('未检出 ports/th13-mp');
  const expected = inventory(single);
  assert.ok(expected.length > 0);
  assert.deepEqual(inventory(multiplayer), expected);
});
