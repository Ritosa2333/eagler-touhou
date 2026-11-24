import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {join} from 'node:path';
import {formatTargets, inlineJsLines, unguardedInlineJs} from '../tools/source-layout.mjs';

const single = fileURLToPath(new URL('../', import.meta.url));
const multiplayer = fileURLToPath(new URL('../../th13-mp/', import.meta.url));
const ports = [['th13', single], ['th13-mp', multiplayer]];
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

test('.clang-format is byte-identical between th13 and th13-mp', t => {
  if (!existsSync(multiplayer)) return t.skip('未检出 ports/th13-mp');
  assert.equal(digest(join(multiplayer, '.clang-format')), digest(join(single, '.clang-format')));
});

test('inline JS blocks sit inside clang-format off/on', t => {
  for (const [name, port] of ports) {
    if (name === 'th13-mp' && !existsSync(port)) return t.skip('未检出 ports/th13-mp');
    const files = formatTargets(port);
    assert.ok(files.length > 0, `${name} 应找到参与排版的源码`);
    const sites = files.reduce((sum, file) => sum + inlineJsLines(readFileSync(join(port, file), 'utf8')).length, 0);
    assert.ok(sites > 0, `${name} 应找到内联 JS`);
    assert.deepEqual(unguardedInlineJs(port, files), []);
  }
});
