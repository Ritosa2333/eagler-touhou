import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { pathToFileURL } from 'node:url';
const root = path.resolve(import.meta.dirname, '..'), workspace = path.dirname(root);
const directory = path.join(root, 'artifacts/bullet-rectangle-boundaries');
fs.mkdirSync(directory, { recursive: true });
const compilerTemp = path.join(directory, 'compiler-temp');
fs.mkdirSync(compilerTemp, { recursive: true });
const python = emscripten(root).python;
const compiler = emscripten(root).compiler;
const output = path.join(directory, 'core.mjs');
const built = spawnSync(python, [compiler, 'tests/fixtures/bullet-rectangle-boundaries.cpp',
  '-std=c++20', '-O2', '-o', output, '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
  '-sEXPORTED_FUNCTIONS=["_rectangle_collision"]'], {
  cwd: root, encoding: 'utf8', env: { ...process.env,
    EM_CACHE: path.join(root, 'artifacts/emscripten-cache'), EMSDK_PYTHON: python,
    EMCC_TEMP_DIR: compilerTemp,
    PATH: path.dirname(python) + path.delimiter + process.env.PATH,
  },
});
assert.equal(built.status, 0, built.stdout + '\n' + built.stderr);
const create = (await import(pathToFileURL(output))).default;
const native = JSON.parse(fs.readFileSync(new URL('fixtures/bullet-rectangle-boundaries-original.json', import.meta.url)));
test('ordinary bullet hit/graze borders match actual cached Player rects at equality and adjacent float32s', async () => {
  assert.equal(native.originalExeSha256, '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  const core = await create();
  for (const row of native.rows)
    assert.equal(core._rectangle_collision(row.type, ...row.position, ...row.player, row.playerHit),
      row.nativeReturn, JSON.stringify(row));
  assert.deepEqual([...new Set(native.rows.map(row => row.nativeReturn))].sort(), [0, 1, 2]);
});
