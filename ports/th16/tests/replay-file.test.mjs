// 0x448c10：demo*.rpy 解密 + LZSS + 段头解析
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join, resolve } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const monorepo = resolve(root, '../..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const demosDir = process.env.TH16_REPLAY_DIR ?? resolve(monorepo, 'artifacts/th16/dat');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/replay-file.json'), 'utf8'));
const need = { skip: !existsSync(join(demosDir, 'demo1.rpy')) };

function info(name) {
  const r = spawnSync(harness, ['replay-info', join(demosDir, name)], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  const lines = r.stdout.trim().split('\n');
  const head = lines[0].match(/^(.*) stages=(\d+)$/);
  assert.ok(head, lines[0]);
  const stages = lines.slice(1).map((line) => {
    const p = Object.fromEntries(line.split(/\s+/).slice(1).map((kv) => kv.split('=')));
    return {
      stage: Number(p.stage),
      seed: Number(p.seed),
      frames: Number(p.frames),
      input_nz: Number(p.input_nz),
      fps: Number(p.fps),
      first: Number(p.first),
      held: p.held,
    };
  });
  return { name: head[1], stage_count: Number(head[2]), stages };
}

test('replay-file: demo1..4 decode via 0x448c10', need, () => {
  for (const key of Object.keys(golden.demos)) {
    assert.deepEqual(info(`${key}.rpy`), golden.demos[key], key);
  }
});

test('replay-file: demo1 encode roundtrip preserves stages/inputs', need, () => {
  const r = spawnSync(harness, ['replay-roundtrip', join(demosDir, 'demo1.rpy')], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  assert.match(r.stdout, /^ok stages=1 /);
});

test('replay-file: record → write_slot → open (0x448400)', () => {
  const dir = '/tmp/th16-replay-write';
  spawnSync('rm', ['-rf', dir], { encoding: 'utf8' });
  spawnSync('mkdir', ['-p', dir], { encoding: 'utf8' });
  const r = spawnSync(harness, ['replay-record-save', dir, '3'], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  assert.match(r.stdout, /ok path=.*th16_03\.rpy/);
  assert.match(r.stdout, /name=PORTTEST/);
  assert.match(r.stdout, /frames=91/);  // 90 + terminator
});
