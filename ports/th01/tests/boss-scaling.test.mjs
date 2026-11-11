import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';

const binary = await readFile(new URL('../web/game.wasm', import.meta.url));
const counts = [1, 2, 3, 4, 6, 8];
const baseline = [
  [800, 864, 920, 976, 1080, 1160],
  [1000, 1080, 1160, 1240, 1400, 1520],
  [1160, 1280, 1384, 1480, 1680, 1840],
];

test('stage five boss health follows the existing player and orb scaling', async () => {
  for (let players = 1; players <= 3; players++) {
    for (const [index, count] of counts.entries()) {
      const core = (await WebAssembly.instantiate(binary, {})).instance.exports;
      core.th_init(players, count, 123, 0);
      core.th_stage_command(14, 0, 5, 0, 0, 0);
      assert.equal(core.th_get(4, 0), count);
      assert.equal(core.th_get(60, 0), 0);
      assert.equal(core.th_get(61, 0), baseline[players - 1][index], `${players} players, ${count} orbs`);
    }
  }
});
