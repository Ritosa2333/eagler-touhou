import assert from 'node:assert/strict';
import {test} from 'vitest';
import {createGameDownloads} from '../../app/models/game-downloads.ts';

const game = (id, version = 'original') => ({game: id, version});
const turn = () => new Promise(resolve => queueMicrotask(resolve));
function fixture() {
  const started = [], failed = [];
  const downloads = createGameDownloads({
    download(job, options) {
      const {promise, resolve, reject} = Promise.withResolvers();
      started.push({job, ...options, resolve, reject});
      return promise;
    },
    onFailure(job) {failed.push(job);},
  });
  return {downloads, started, failed};
}

test('each manual selection runs once in FIFO order and progress stays with the active task', async () => {
  const {downloads, started} = fixture();
  const states = [];
  downloads.subscribe(() => states.push(downloads.getSnapshot().active?.game ?? null));
  downloads.enqueue(game('th06'));
  downloads.enqueue(game('th07'));
  downloads.enqueue(game('th06', 'beauty'));
  downloads.enqueue(game('th08'));
  assert.deepEqual(started.map(task => task.job.game), ['th06']);
  assert.deepEqual(downloads.getSnapshot().pending.map(task => task.game), ['th07', 'th08']);
  const progress = {stage: 'package', message: '', completed: 1, total: 3};
  started[0].onProgress(progress);
  assert.equal(downloads.getSnapshot().progress, progress);
  downloads.cancel('th08');
  started[0].resolve();
  await turn();
  started[0].onProgress(progress);
  assert.equal(downloads.getSnapshot().progress, null);
  assert.deepEqual(started.map(task => task.job.game), ['th06', 'th07']);
  assert.ok(!states.includes(null), 'the indicator stays present between queued jobs');
  started[1].resolve();
  await turn();
  assert.equal(downloads.getSnapshot().active, null);
});

test('cancelling keeps the next job waiting until the old installer stops', async () => {
  const {downloads, started, failed} = fixture();
  downloads.enqueue(game('th06'));
  downloads.enqueue(game('th07'));
  downloads.cancel('th06');
  assert.equal(started[0].signal.aborted, true);
  assert.equal(started.length, 1);
  started[0].reject(new Error('aborted'));
  await turn();
  assert.deepEqual(failed, []);
  assert.equal(downloads.getSnapshot().active.game, 'th07');
  downloads.dispose();
  assert.equal(started[1].signal.aborted, true);
  assert.equal(downloads.getSnapshot().active, null);
});

test('a failed job can be retried at the end of the queue while another game is running', async () => {
  const {downloads, started, failed} = fixture();
  downloads.enqueue(game('th06', 'beauty'));
  downloads.enqueue(game('th07'));
  started[0].reject(new Error('unavailable'));
  await turn();
  assert.equal(failed[0].game, 'th06');
  assert.equal(downloads.getSnapshot().failed.length, 1);
  downloads.retry('th06');
  assert.deepEqual(downloads.getSnapshot().pending, [game('th06', 'beauty')]);
  assert.deepEqual(downloads.getSnapshot().failed, []);
  started[1].resolve();
  await turn();
  assert.deepEqual(started[2].job, game('th06', 'beauty'));
  started[2].reject(new Error('still unavailable'));
  await turn();
  assert.equal(downloads.getSnapshot().active, null);
  assert.deepEqual(downloads.getSnapshot().failed, []);
});
