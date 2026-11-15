import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';

const project = fileURLToPath(new URL('../../', import.meta.url));
let work, relocateReplayPlayback;
before(async () => {
  work = await mkdtemp(resolve(project, '.cache/replay-relocation-'));
  const outfile = resolve(work, 'actual.mjs');
  await build({entryPoints: [resolve(project, 'app/services/replay-relocation.ts')], outfile, bundle: true, platform: 'node', format: 'esm', packages: 'external', logLevel: 'silent'});
  ({relocateReplayPlayback} = await import(pathToFileURL(outfile).href));
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
function fixture(relocation = true) {
  const calls = [], state = {epoch: 7, game: 'th10', runtimeVariant: 'normal', launched: true,
    runtimeInfo: {playback: {kind: 'replay', path: 'replay/a.rpy', frame: 456, paused: true, rate: 2, startStage: 3, ...(relocation ? {fileRelocation: 1} : {})}}};
  return {state, calls, runtime: {getSnapshot: () => state, async send(command, payload) {
    calls.push([command, payload]);
    if (payload.action === 'open' || payload.action === 'relocate') state.runtimeInfo.playback = {...state.runtimeInfo.playback, path: payload.target ?? payload.path};
    return {ok: true};
  }}};
}
test('supported relocation updates the active file without reopening, seeking or changing controls', async () => {
  const f = fixture(); await relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/a.rpy', 'replay/b.rpy');
  assert.deepEqual(f.calls, [['playback', {action: 'relocate', path: 'replay/a.rpy', target: 'replay/b.rpy'}]]);
});
test('older packages restore the same stage, frame, pause and rate through existing commands', async () => {
  const f = fixture(false); await relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/a.rpy', 'replay/b.rpy');
  assert.deepEqual(f.calls, [['playback', {action: 'pause'}], ['playback', {action: 'open', path: 'replay/b.rpy', stage: 3}],
    ['playback', {action: 'seek', value: 456}], ['playback', {action: 'pause'}], ['playback', {action: 'rate', value: 2}]]);
});
test('another file and idle players receive no relocation command', async () => {
  const f = fixture(); await relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/other.rpy', 'replay/b.rpy');
  f.state.launched = false; await relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/a.rpy', 'replay/b.rpy');
  assert.deepEqual(f.calls, []);
});
test('epoch or product changes never send the remaining fallback controls to a successor', async () => {
  const f = fixture(false), original = f.runtime.send;
  f.runtime.send = async (...args) => {const result = await original(...args); f.state.epoch++; return result;};
  await assert.rejects(relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/a.rpy', 'replay/b.rpy'), {name: 'AbortError', message: 'EAGLER_RUNTIME_SESSION_SUPERSEDED'});
  assert.equal(f.calls.length, 1);
  const wrongVariant = fixture();
  await assert.rejects(relocateReplayPlayback(wrongVariant.runtime, 'th10mp', 7, 'replay/a.rpy', 'replay/b.rpy'), {name: 'AbortError', message: 'EAGLER_RUNTIME_SESSION_SUPERSEDED'});
  assert.deepEqual(wrongVariant.calls, []);
});
test('selecting another recording interrupts the compatibility recovery', async () => {
  const f = fixture(false), original = f.runtime.send;
  f.runtime.send = async (...args) => {const result = await original(...args); f.state.runtimeInfo.playback = {...f.state.runtimeInfo.playback, path: 'replay/other.rpy'}; return result;};
  await assert.rejects(relocateReplayPlayback(f.runtime, 'th10', 7, 'replay/a.rpy', 'replay/b.rpy'), {name: 'AbortError'});
  assert.deepEqual(f.calls, [['playback', {action: 'pause'}]]);
});
