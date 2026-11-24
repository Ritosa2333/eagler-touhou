import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');

test('Rng 常量与原版锚点约定保持登记', () => {
  const rng = readFileSync(resolve(port, 'web/cpp/game/Rng.hpp'), 'utf8');
  assert.ok(rng.includes('0x343FDu'), 'RNG 乘数应为 MSVC CRT 0x343FD');
  assert.ok(rng.includes('0x269EC3u'), 'RNG 增量应为 MSVC CRT 0x269EC3');
});

test('地址锚点表包含 th13 特有子系统（神灵/灵界）', () => {
  const map = readFileSync(resolve(port, 'web/cpp/rebuild/AddressMap.hpp'), 'utf8');
  assert.ok(map.includes('"spirit"'));
  assert.ok(map.includes('"ecl-vm"'));
});
