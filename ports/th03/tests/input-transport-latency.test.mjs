import {test} from 'node:test';
import assert from 'node:assert/strict';
import {InputTransport} from '../web/netplay/input-transport.js';

function setup() {
  let now = 0;
  const queues = [[], []], timers = [], received = [[], []];
  const peers = [0, 1].map(() => new InputTransport('test', {now: () => now,
    repeat: (fn, ms) => {timers.push(ms); return fn;}, cancel: () => {}}));
  for (let id = 0; id < 2; id++) {
    const lane = fast => ({label: fast ? 'test-fast' : 'test', ordered: !fast,
      maxRetransmits: fast ? 0 : null, maxPacketLifeTime: null, readyState: 'open', bufferedAmount: 0,
      send: data => queues[id ^ 1].push({data, fast}), close() {}});
    peers[id].bind(lane(false), false); peers[id].bind(lane(true), true);
    peers[id].onmessage = event => received[id].push(JSON.parse(event.data));
  }
  const deliver = id => {while (queues[id].length) {const {data, fast} = queues[id].shift(); peers[id].receive(data, fast);}};
  deliver(0); deliver(1); deliver(0);
  return {peers, queues, received, timers, deliver, time: value => {now = value;}, close: () => peers.forEach(p => p.close())};
}

test('lost last frame is retried within one 60 Hz tick without breaking control order', () => {
  const f = setup();
  try {
    assert.deepEqual(f.timers, [8, 8]);
    f.peers[0].send(JSON.stringify({type: 'frame', frame: 0}));
    assert.equal(f.queues[1].shift().fast, true); // drop first fast packet
    f.peers[0].send(JSON.stringify({type: 'control', value: 'after frame'}));
    f.deliver(1);
    assert.deepEqual(f.received[1], []); // fence waits for missing frame
    f.time(15); f.peers[0].pump(); assert.equal(f.queues[1].length, 0);
    f.time(16); f.peers[0].pump();
    const retry = f.queues[1][0];
    f.deliver(1); f.peers[1].receive(retry.data, true); // delayed duplicate
    assert.deepEqual(f.received[1], [{type:'frame',frame:0}, {type:'control',value:'after frame'}]);
    assert.equal(f.peers[0].transportStats.retransmitted, 1);
  } finally {f.close();}
});
