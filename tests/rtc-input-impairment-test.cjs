const assert = require('node:assert/strict');
const {installRtcInputImpairment} = require('../testkit/rtc-input-impairment.cjs');

let time = 0;
const jobs = new Map(), delivered = [];
let serial = 0;
class Channel {
  constructor(label) { this.label = label; this.readyState = 'open'; }
  send(data) { delivered.push({label: this.label, time, data: new Uint8Array(data)}); }
}
const env = {
  DataChannel: Channel,
  now: () => time,
  later: (fn, ms) => { const id = ++serial; jobs.set(id, {fn, due: time + ms}); return id; },
  cancel: id => jobs.delete(id),
};
const advance = value => {
  time = value;
  for (const [id, job] of [...jobs]) if (job.due <= time) { jobs.delete(id); job.fn(); }
};

const fixture = installRtcInputImpairment({
  inputLabel: 'th06-input', controlLabel: 'th06-control',
  oneWayMs: 50, jitterMs: 0, blackoutFrame: 900, blackoutMs: 1000,
}, env);
const fast = new Channel('th06-input'), control = new Channel('th06-control');
const packet = new Uint8Array(44);
packet[5] = 1;
new DataView(packet.buffer).setUint32(24, 900, true);

fast.send(packet);
assert.equal(fixture.stats.blackoutDropped, 1);
control.send(packet);
assert.equal(delivered.length, 0, 'reliable repair cannot bypass simulated network delay');
advance(49);
assert.equal(delivered.length, 0);
advance(50);
assert.equal(delivered.length, 1);
assert.equal(delivered[0].label, 'th06-control');
fast.send(packet);
assert.equal(fixture.stats.blackoutDropped, 2);
advance(1001);
fast.send(packet);
assert.equal(delivered.length, 1);
advance(1051);
assert.equal(delivered.length, 2);
assert.equal(delivered[1].label, 'th06-input');
assert.equal(fixture.stats.controlInputs, 1);
fixture.uninstall();
assert.equal(fixture.stats.queuedBytes, 0);
assert.equal(jobs.size, 0);
console.log('eagler-common RTC input impairment: PASS blackout/delayed-repair/cleanup');

time = 0; delivered.length = 0;
const spike = installRtcInputImpairment({
  inputLabel: 'th06-input', controlLabel: 'th06-control',
  oneWayMs: 50, jitterMs: 0, spikeFrame: 900, spikeMs: 1000, spikeDelayMs: 800,
}, env);
fast.send(packet); control.send(packet);
advance(50); assert.equal(delivered.length, 0, 'repair must see the latency spike too');
advance(850); assert.equal(delivered.length, 2);
assert.equal(spike.stats.spikeControlDelayed, 1);
fast.send(packet); // queued until 1700, after the impairment window ends
advance(1000); fast.send(packet);
advance(1050); assert.equal(delivered.length, 3, 'new packets recover without waiting for the old queue');
advance(1700); assert.equal(delivered.length, 4, 'late old packet remains deliverable');
assert.equal(spike.stats.spikeStartMs, 0);
assert.equal(spike.stats.spikeEndMs, 1000);
assert.equal(spike.stats.spikeLastDeliveryMs, 1700);
assert.equal(spike.stats.spikeDelayed, 3);
spike.uninstall(); assert.equal(jobs.size, 0); assert.equal(spike.stats.queuedBytes, 0);
assert.throws(() => installRtcInputImpairment({spikeMs: -1}, env), /invalid/);
console.log('eagler-common RTC input impairment: PASS spike/both-lanes/recovery/late-tail');
