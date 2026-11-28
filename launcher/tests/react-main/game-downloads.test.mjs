import assert from 'node:assert/strict';
import {test} from 'vitest';
import {createGameDownloads, gameDownloadProgress} from '../../app/models/game-downloads.ts';

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
  const totals = {completed: 1, total: 3, completedBytes: 20, totalBytes: 60};
  started[0].onTotals(totals);
  assert.equal(downloads.getSnapshot().totals, totals);
  downloads.cancel('th08');
  started[0].resolve();
  await turn();
  started[0].onTotals(totals);
  assert.equal(downloads.getSnapshot().totals, null);
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
  assert.deepEqual(downloads.getSnapshot().failed, [game('th06', 'beauty')]);
});

const job = {game: 'th06', version: 'original'};
function receive(run, loaded, total) {
  run.onNetwork({active: [{kind: 'game', phase: 'receiving', loaded, total, startedAt: 10, updatedAt: 1010}]});
}

test('installation totals aggregate committed and in-flight bytes without claiming completion', async () => {
  const done = Promise.withResolvers();
  let run;
  const queue = createGameDownloads({download(_job, options) {run = options; return done.promise;}, onFailure: assert.fail});
  const progress = () => gameDownloadProgress(queue.getSnapshot());
  queue.enqueue(job);
  assert.equal(progress().percent, null);
  run.onTotals({completed: 0, total: 2, completedBytes: 0, totalBytes: 1000});
  receive(run, 200, 400);
  assert.deepEqual(progress(), {loaded: 200, total: 1000, percent: 20, speed: 200, eta: 4, phase: 'receiving'});
  receive(run, 400, 400);
  run.onNetwork({active: []});
  assert.equal(progress().phase, 'saving');
  assert.equal(progress().percent, 40);
  run.onTotals({completed: 1, total: 2, completedBytes: 400, totalBytes: 1000});
  receive(run, 300, 600);
  assert.equal(progress().percent, 70);
  run.onNetwork({active: []});
  run.onTotals({completed: 2, total: 2, completedBytes: 1000, totalBytes: 1000});
  assert.equal(progress().percent, 99);
  assert.equal(progress().phase, 'saving');
  done.resolve();
  await done.promise;
  assert.equal(queue.getSnapshot().active, null);
});

test('unknown totals are indeterminate and a failed final job is still retryable', async () => {
  const runs = [], failures = [];
  const queue = createGameDownloads({download(_job, run) {
    const done = Promise.withResolvers();
    runs.push({run, done});
    return done.promise;
  }, onFailure: value => failures.push(value)});
  queue.enqueue(job);
  runs[0].run.onTotals({completed: 0, total: 2, completedBytes: 0, totalBytes: null});
  receive(runs[0].run, 40, 100);
  const progress = gameDownloadProgress(queue.getSnapshot());
  assert.equal(progress.loaded, 40);
  assert.equal(progress.percent, null);
  assert.equal(progress.eta, null);
  runs[0].done.reject(new Error('test transfer failed'));
  await assert.rejects(runs[0].done.promise);
  assert.equal(queue.getSnapshot().active, null);
  assert.deepEqual(queue.getSnapshot().failed, [job]);
  assert.deepEqual(failures, [job]);
  queue.retry(job.game);
  assert.deepEqual(queue.getSnapshot().failed, []);
  assert.deepEqual(queue.getSnapshot().active, job);
  runs[1].done.resolve();
  await runs[1].done.promise;
  assert.equal(queue.getSnapshot().active, null);
});
