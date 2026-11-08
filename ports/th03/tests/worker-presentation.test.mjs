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

test('a delayed newest frame is drawn while older frames yield to newer ones', () => {
  const original = globalThis.document;
  globalThis.document = {hidden:false};
  try {
    const drawn=[],released=[];
    const client=Object.assign(Object.create(WorkerNetworkSimulation.prototype),{
      closed:false,drawHandle:1,displayMetrics:{expired:0,lateDrawn:0,hiddenDiscarded:0,replaced:0},
      onDraw:bitmap=>drawn.push(bitmap),releaseFrame:(frame,discarded)=>released.push([frame.bitmap,discarded])
    });
    const old=()=>({bitmap:'late',producedAt:performance.timeOrigin+performance.now()-80});
    client.queuedFrame=old();client.draw();
    assert.deepEqual(drawn,['late']);assert.equal(client.displayMetrics.lateDrawn,1);
    client.queuedFrame=old();client.preparedFrame={bitmap:'new'};client.draw();
    assert.deepEqual(drawn,['late']);assert.equal(client.displayMetrics.expired,1);assert.deepEqual(released.at(-1),['late',true]);
    globalThis.document.hidden=true;client.preparedFrame=null;client.queuedFrame=old();client.draw();
    assert.equal(client.displayMetrics.hiddenDiscarded,1);
  } finally {globalThis.document=original;}
});
