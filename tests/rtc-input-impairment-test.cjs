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
