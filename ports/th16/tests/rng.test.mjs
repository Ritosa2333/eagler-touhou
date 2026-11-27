// RNG 行为还原：C++ Rng 与 Unicorn 直接执行原版 0x402b70/0x402be0/0x402c70/0x402cb0/0x402cf0 的 golden 逐值对照。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const golden = JSON.parse(readFileSync(resolve(here, 'golden/rng.json'), 'utf8'));
const run = (...args) => execFileSync(harness, args, {encoding: 'utf8'}).trim().split('\n').map(l => l.split(' ').map(Number));

for (const seq of golden.sequences) {
  test(`step32 seed ${seq.seed} matches th16.exe 0x402be0`, () => {
    const ours = run('rng', String(seq.seed), String(seq.step32.length));
    assert.deepEqual(ours, seq.step32);
  });
  test(`step16 seed ${seq.seed} matches th16.exe 0x402b70`, () => {
    const ours = run('rng16', String(seq.seed), String(seq.step16.length));
    assert.deepEqual(ours, seq.step16);
  });
  test(`float variants seed ${seq.seed} match 0x402c70/0x402cb0/0x402cf0 bit-exactly`, () => {
    const ours = run('rng-float', String(seq.seed), String(seq.unit.length));
    const expected = seq.unit.map((u, i) => [u[0], seq.signed_unit[i][0], seq.angle[i][0]]);
    assert.deepEqual(ours, expected);
  });
}
