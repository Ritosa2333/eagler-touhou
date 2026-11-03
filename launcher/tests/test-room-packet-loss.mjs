import {test} from 'vitest';
import assert from 'node:assert/strict';
import {createPacketLossTest} from '../src/launcher/room-packet-loss.mts';

function pair({drop = () => false, delay = () => 20, duplicate = false, controls = true} = {}) {
  let clock = 0, serial = 0; const tasks = new Map(), visible = [true, true], ready = [true, true], probes = [0, 0];
  const later = (fn, ms) => {tasks.set(++serial, {fn, at: clock + ms}); return serial;};
  const peers = [0, 1].map(side => createPacketLossTest({now: () => clock, later, cancel: id => tasks.delete(id), changed() {},
    visible: () => visible[side], ready: () => ready[side],
    control(value) {if (controls) later(() => peers[1-side].receiveControl(value), 8); return true;},
    probe(value) {
      const sequence = Number(value.trim().split(':').at(-1)); probes[side]++;
      if (!drop(side, sequence)) {later(() => peers[1-side].receiveProbe(value), delay(side, sequence)); if (duplicate) later(() => peers[1-side].receiveProbe(value), delay(side, sequence)+7);}
      return true;
    },
  }));
  const advance = (until = 20000) => {
    for (;;) {
      const next = [...tasks].sort((a,b) => a[1].at-b[1].at || a[0]-b[0])[0];
      if (!next || next[1].at > until) break;
      tasks.delete(next[0]); clock = next[1].at; next[1].fn();
    }
    clock = until;
  };
  return {peers, advance, visible, ready, probes, tasks};
}
test('manual paired sweep measures asymmetric loss including lost trailing probes', () => {
  const f = pair({drop: (side, sequence) => side === 0 ? sequence % 10 === 9 : sequence >= 285});
  assert.deepEqual(f.probes, [0,0], 'no traffic before manual start');
  f.peers[0].start(); f.advance();
  const [a,b] = f.peers.map(p => p.snapshot());
  assert.equal(a.state, 'complete'); assert.equal(b.state, 'complete');
  assert.deepEqual(f.probes, [300,300]);
  assert.deepEqual([a.outgoing.lost,a.incoming.lost,a.outgoing.percent,a.incoming.percent], [30,15,10,5]);
  assert.deepEqual(a.outgoing,b.incoming); assert.deepEqual(a.incoming,b.outgoing);
  assert.equal(f.tasks.size,0);
});
test('duplicates and reordered messages within the tail do not become losses', () => {
  const f = pair({delay: (_,n) => n%10===0 ? 200 : 20, duplicate: true}); f.peers[1].start(); f.advance();
  for(const p of f.peers) {const r=p.snapshot(); assert.equal(r.state,'complete'); assert.equal(r.incoming.percent,0); assert.equal(r.outgoing.percent,0); assert.ok(r.incoming.reordered>0);}
});
test('an opened probe path with no delivered datagrams can report genuine 100 percent message loss', () => {
  const f = pair({drop: side => side === 0}); f.peers[0].start(); f.advance();
  assert.equal(f.peers[0].snapshot().outgoing.percent,100); assert.equal(f.peers[0].snapshot().incoming.percent,0);
});
test('unavailable paths, missing peer support, visibility loss and disposal never invent a loss rate', () => {
  const unavailable = pair(); unavailable.ready[0]=false; unavailable.peers[0].start();
  assert.equal(unavailable.peers[0].snapshot().state,'unavailable'); assert.deepEqual(unavailable.probes,[0,0]);
  const old = pair({controls:false}); old.peers[0].start(); old.advance();
  assert.equal(old.peers[0].snapshot().state,'interrupted'); assert.equal(old.peers[0].snapshot().outgoing,undefined);
  const hidden = pair(); hidden.peers[0].start(); hidden.advance(700); hidden.visible[1]=false; hidden.advance();
  assert.ok(hidden.peers.every(p=>p.snapshot().state==='interrupted')); assert.equal(hidden.peers[0].snapshot().outgoing,undefined);
  const left = pair(); left.peers[0].start(); left.advance(700); left.peers[0].dispose(); left.advance();
  assert.ok(left.peers.every(p=>p.snapshot().state==='interrupted')); assert.equal(left.tasks.size,0);
});
test('both peers starting together terminates cleanly and a later retry succeeds', () => {
  const f = pair(); f.peers.forEach(p=>p.start()); f.advance();
  assert.ok(f.peers.every(p=>p.snapshot().state==='interrupted'));
  f.peers[0].start(); f.advance(40000); assert.ok(f.peers.every(p=>p.snapshot().state==='complete'));
});
