import test from 'node:test';
import assert from 'node:assert/strict';
import { createRoomBroadcaster } from '../src/room-broadcast.mjs';

test('room broadcasts isolate state, preserve events and retire cancelled callbacks', () => {
  const timers = new Map(), sent = [];
  let serial = 0;
  const broadcaster = createRoomBroadcaster({
    snapshot: room => ({ type: 'state', value: room.value }),
    send: (room, text) => sent.push([room, JSON.parse(text)]),
    changed() {},
    schedule: callback => { timers.set(++serial, callback); return serial; },
    cancel: timer => timers.delete(timer),
  });
  const first = { value: 1 }, second = { value: 10 };
  broadcaster.publish(first);
  const stale = timers.get(1);
  first.value = 2;
  broadcaster.publish(first);
  first.value = 3;
  broadcaster.publish(first);
  broadcaster.publish(first, { type: 'start' });
  broadcaster.publish(second);
  assert.deepEqual(sent.map(row => row[1]), [
    { type: 'state', value: 1 }, { type: 'start' }, { type: 'state', value: 10 },
  ]);
  stale();
  assert.deepEqual(sent.at(-1)[1], { type: 'state', value: 3 });
  first.value = 4;
  broadcaster.publish(first);
  const cancelled = timers.get(3);
  broadcaster.clear(first);
  broadcaster.publish(first);
  const count = sent.length;
  cancelled();
  stale();
  assert.equal(sent.length, count, 'retired timers cannot publish or clear a new window');
  broadcaster.clear(first);
  broadcaster.clear(second);
});
