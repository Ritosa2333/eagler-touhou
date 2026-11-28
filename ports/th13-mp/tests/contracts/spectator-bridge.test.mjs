import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';

const shell = readFileSync(new URL('../../web/sdl-runtime/shell.mjs', import.meta.url), 'utf8');
const source = readFileSync(new URL('../../../../common/browser/spectator-playback.mjs', import.meta.url), 'utf8');

test('embedded spectator playback bridge equals the shared source', () => {
  const begin = '// BEGIN GENERATED SPECTATOR PLAYBACK\n';
  const end = '// END GENERATED SPECTATOR PLAYBACK';
  const at = shell.indexOf(begin);
  assert.ok(at >= 0, 'shell.mjs 缺少观战生成块起点');
  const block = shell.slice(at + begin.length, shell.indexOf(end, at));
  const expected = '// Generated from eagler-common/browser/spectator-playback.mjs.\n' +
    source.replace('export function createSpectatorPlaybackBridge', 'function createSpectatorPlaybackBridge');
  assert.equal(block, expected);
});

test('shell routes the spectator-playback command to the bridge', () => {
  const routed = /case 'spectator-playback':[\s\S]{0,400}?spectatorPlayback\.command\(/;
  assert.match(shell, routed);
  assert.match(source, /\['pause','play','seek','live'\]/);
});
