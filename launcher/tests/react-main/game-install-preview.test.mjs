import assert from 'node:assert/strict';
import {test} from 'vitest';
import {createGameDownloads, gameDownloadProgress} from '../../app/models/game-downloads.ts';
import {createGameInstallPreview, isLocalInstallPreview} from '../../app/models/game-install-preview.ts';

test('simulation is opt-in on loopback development only', () => {
  for (const host of ['127.0.0.1', 'localhost', '[::1]', 'touhouonline.com']) {
    const url = new URL(`http://${host}/?game=th07&preview=install`);
    assert.equal(isLocalInstallPreview(url, true), host !== 'touhouonline.com');
    assert.equal(isLocalInstallPreview(url, false), false);
    url.searchParams.delete('preview');
    assert.equal(isLocalInstallPreview(url, true), false);
  }
});

test('the simulated 100 MiB transfer follows the normal queue at 5 MiB/s, with repeat and cancel', async () => {
  let timestamp = 0, interval;
  const owner = {
    performance: {now: () => timestamp},
    setInterval(callback) {interval = callback; return 1;},
    clearInterval() {interval = undefined;},
  };
  const advance = async ms => {
    timestamp += ms;
    interval?.();
    await Promise.resolve();
    await Promise.resolve();
  };
  const preview = createGameInstallPreview(owner);
  const queue = createGameDownloads({download: preview.download, onFailure: assert.fail});
  const first = {game:'th07', version:'original'}, second = {game:'th08', version:'original'};
  queue.enqueue(first);
  queue.enqueue(second);
  assert.deepEqual(queue.getSnapshot().pending, [second]);
  assert.equal(gameDownloadProgress(queue.getSnapshot()).percent, null);
  await advance(600);
  await advance(5000);
  const current = gameDownloadProgress(queue.getSnapshot());
  assert.equal(current.percent, 25);
  assert.equal(current.speed, 5 * 1024 ** 2);
  assert.equal(current.eta, 15);
  await advance(15000);
  assert.equal(preview.installed(first), true);
  assert.deepEqual(queue.getSnapshot().active, second);
  queue.cancel(second.game);
  await advance(1);
  assert.equal(preview.installed(second), false);
  assert.equal(queue.getSnapshot().active, null);
  assert.equal(interval, undefined);
  queue.enqueue(first);
  assert.equal(preview.installed(first), false);
  await advance(600);
  await advance(10000);
  assert.equal(gameDownloadProgress(queue.getSnapshot()).percent, 50);
  queue.dispose();
  assert.equal(interval, undefined);
});
