import assert from 'node:assert/strict';
import { execFileSync, spawnSync } from 'node:child_process';
import { resolve } from 'node:path';
import test from 'node:test';

const root = resolve(import.meta.dirname, '..');
const game = 'th08';
const script = resolve(root, 'portable/build.mjs');
const plan = (...args) => JSON.parse(execFileSync(process.execPath,
  [script, '--' + game, '--print-plan', ...args],
  { cwd: root, encoding: 'utf8', windowsHide: true }));

test('ordinary build excludes multiplayer implementation and shared netplay code', () => {
  const normal = plan();
  assert.equal(normal.variant, 'normal');
  assert.equal(normal.profile, 'sdl3');
  assert.equal(normal.outputDirectory, resolve(root, game + '_web/artifacts/sdl3'));
  assert(!normal.flags.some(flag => /TH_ENABLE_(MULTIPLAYER_GAMEPLAY|NETPLAY)/.test(flag)));
  assert(!normal.sources.some(source => /multiplayer|eagler-common\/src\/netplay/.test(source)));
  assert.deepEqual(normal.linkFlags, []);
});

test('multiplayer build has its own output, macros and pinned shared implementation', () => {
  const mp = plan('--multiplayer');
  assert.equal(mp.variant, 'multiplayer');
  assert.equal(mp.profile, 'multiplayer');
  assert.notEqual(mp.outputDirectory, plan().outputDirectory);
  assert(mp.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));
  assert(mp.flags.includes('-DTH_ENABLE_NETPLAY=1'));
  for (const component of ['NetplayCore', 'NetplayProtocol', 'NetplaySession',
    'RollbackJournal', 'BrowserPeerTransport', 'WebSocketTransport']) {
    const source = '../third_party/eagler-common/src/netplay/' + component + '.cpp';
    assert.equal(mp.sources.filter(item => item === source).length, 1, component);
  }
  assert(mp.linkFlags.includes('-lwebsocket'));
});

test('presentation diagnostics cannot be mixed into the multiplayer variant', () => {
  const result = spawnSync(process.execPath,
    [script, '--' + game, '--print-plan', '--multiplayer', '--presentation-lab'],
    { cwd: root, encoding: 'utf8', windowsHide: true });
  assert.notEqual(result.status, 0);
  assert.match(result.stderr, /separate build variants/);
});
