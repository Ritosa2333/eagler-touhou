import {test, beforeAll as before} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let broKeyBindingsFromAliases, BRO_DEFAULT_KEY_BINDINGS, wasdKeyboardAliases;
before(async()=>{
  const result=await build({stdin:{contents:`export {broKeyBindingsFromAliases,BRO_DEFAULT_KEY_BINDINGS} from './src/launcher/bro-key-bindings.mts';export {wasdKeyboardAliases} from './src/launcher/keyboard-preferences.mts';`,resolveDir:fileURLToPath(new URL('../../',import.meta.url))},bundle:true,write:false,format:'esm',platform:'node'});
  ({broKeyBindingsFromAliases,BRO_DEFAULT_KEY_BINDINGS,wasdKeyboardAliases}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64')));
});

test('BRO 未改键时下发原生默认键位', () => {
  for (const aliases of [null, undefined, {}, {KeyQ: 'KeyV'}]) {
    assert.deepEqual(broKeyBindingsFromAliases(aliases), BRO_DEFAULT_KEY_BINDINGS);
  }
});

test('网页 WASD 预设折算成 BRO 游戏键位', () => {
  const b = broKeyBindingsFromAliases(wasdKeyboardAliases);
  assert.equal(b.keys.up, 0x57);     // KeyW
  assert.equal(b.keys.left, 0x41);   // KeyA
  assert.equal(b.keys.down, 0x53);   // KeyS
  assert.equal(b.keys.right, 0x44);  // KeyD
  assert.equal(b.keys.shoot, 0x4A);  // KeyJ
  assert.equal(b.keys.spell, 0x4B);  // KeyK
  assert.equal(b.keys.special, 0x48);// KeyH
  // 网页没有对应动作的 keysys 项保持 BRO 默认。
  assert.equal(b.keysys.menu, 0x1B);
  assert.equal(b.keysys.repslow === undefined, true);
});

test('单个动作改键只改对应字段，非法映射被忽略', () => {
  const b = broKeyBindingsFromAliases({KeyJ: 'KeyZ', Unknown: 'ArrowUp'});
  assert.equal(b.keys.shoot, 0x4A);
  assert.equal(b.keys.up, 0x26);
});
