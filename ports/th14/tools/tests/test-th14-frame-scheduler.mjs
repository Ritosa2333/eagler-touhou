import assert from 'node:assert/strict';
import test from 'node:test';
import {Th14AsyncRuntime} from '../../runtimes/th12/portable/sdl/Th14AsyncRuntime.mjs';

test('TH14 reserves the next refresh and never queues stale simulation frames', async () => {
  const frames = [], calls = [];
  let finish;
  const module = {ccall(name, _type, _types, args) {
    calls.push([name, ...args]);
    return new Promise(resolve => { finish = resolve; });
  }};
  const runtime = new Th14AsyncRuntime(module, {requestFrame(fn) { frames.push(fn); }});
  const refresh = time => {
    assert.equal(frames.length, 1);
    frames.shift()(time);
  };
  runtime.startFrames(5, 7);
  refresh(10);
  await Promise.resolve();
  assert.equal(runtime.pendingCallbacks, 1);
  refresh(27);
  refresh(44);
  assert.deepEqual(calls, [['th14_async_frame', 5, 10, 7]]);
  finish(1);
  await runtime.tail;
  refresh(61);
  await Promise.resolve();
  assert.deepEqual(calls.at(-1), ['th14_async_frame', 5, 61, 7]);
  finish(0);
  await runtime.tail;
  refresh(78);
  assert.equal(frames.length, 0);
  assert.equal(calls.length, 2);
  assert.equal(runtime.pendingCallbacks, 0);
});
