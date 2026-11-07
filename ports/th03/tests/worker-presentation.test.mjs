import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {WorkerNetworkSimulation} from '../web/netplay/simulation-client.js';

test('worker presentation uses the next browser refresh without a second 60 Hz gate', async () => {
  const source = await readFile(new URL('../web/app.js', import.meta.url), 'utf8');
  assert.match(source, /present:\(\)=>emulator\.present\(\)/);
  const callbacks = [];
  const oldRaf = globalThis.requestAnimationFrame;
  globalThis.requestAnimationFrame = fn => (callbacks.push(fn), callbacks.length);
  try {
    const drawn = [], released = [];
    const client = Object.assign(Object.create(WorkerNetworkSimulation.prototype), {
      closed: false, preparedPresentation: true,
      onDraw: bitmap => drawn.push(bitmap),
      releaseFrame: frame => released.push(frame),
    });
    const oldDocument = globalThis.document;
    globalThis.document = {hidden: false};
    try {
      for (const bitmap of ['first', 'second']) {
        client.preparedFrame = {bitmap, staged: true};
        client.present();
        assert.equal(callbacks.length, 1);
        callbacks.shift()();
      }
      assert.deepEqual(drawn, ['first', 'second']);
      assert.equal(released.length, 2);
    } finally {globalThis.document = oldDocument;}
  } finally {globalThis.requestAnimationFrame = oldRaf;}
});
