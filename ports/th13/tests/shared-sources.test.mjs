// 普通版与多人版共用的源码必须两侧一致：portable/sdl 是同一份语义 GLES 渲染
// 后端，列出的运行时模块在两个目录各有一份拷贝。任何一侧单独改动都会让两种
// 模式的画面语义分叉。
import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {join} from 'node:path';

const single = fileURLToPath(new URL('../', import.meta.url));
const multiplayer = fileURLToPath(new URL('../../th13-mp/', import.meta.url));
const sharedRuntimeModules = ['context-attributes.mjs', 'keyboard.mjs'];

function inventory(root, prefix = '') {
  return readdirSync(root, {withFileTypes: true}).flatMap(entry => {
    const path = join(root, entry.name), name = prefix + entry.name;
    if (entry.isDirectory()) return inventory(path, name + '/');
    assert.ok(entry.isFile(), `只应包含普通文件：${name}`);
    return [[name, createHash('sha256').update(readFileSync(path)).digest('hex')]];
  }).sort(([a], [b]) => a < b ? -1 : a > b ? 1 : 0);
}

const digest = path => createHash('sha256').update(readFileSync(path)).digest('hex');

test('portable/sdl is byte-identical between th13 and th13-mp', t => {
  if (!existsSync(multiplayer)) return t.skip('未检出 ports/th13-mp');
  const expected = inventory(join(single, 'portable/sdl'));
  assert.ok(expected.length > 0);
  assert.deepEqual(inventory(join(multiplayer, 'portable/sdl')), expected);
});

test('shared runtime modules are byte-identical between th13 and th13-mp', t => {
  if (!existsSync(multiplayer)) return t.skip('未检出 ports/th13-mp');
  for (const name of sharedRuntimeModules) {
    const path = `web/sdl-runtime/${name}`;
    assert.equal(digest(join(multiplayer, path)), digest(join(single, path)), `${path} 两侧必须逐字节相同`);
  }
});
