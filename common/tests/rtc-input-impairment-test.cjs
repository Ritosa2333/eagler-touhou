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
packet.set([69,54,78,80,4,1,0,2]); // E6NP/v4 input header, empty input tail.
new DataView(packet.buffer).setUint32(24, 900, true);
new DataView(packet.buffer).setUint32(32, 0xffffffff, true);

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

function adonis(sender, kind = 2, players = 2) {
  const bytes = new Uint8Array(64), view = new DataView(bytes.buffer);
  const probe = kind === 2 || kind === 3;
  bytes.set([65,68,83,4,kind,sender,2,2]);
  view.setUint32(8, 1234, true); view.setUint32(16, 0x12345678, true);
  view.setUint32(20, 5678, true); view.setUint32(24, 0xffffffff, true);
  view.setUint32(28, probe ? 129 : 0, true);
  bytes[56] = players; bytes[57] = probe ? (sender + 1) % players : 255;
  if (kind === 4) [36000,120,0,20000,40000,30000].forEach((v, i) => view.setUint32(32+i*4, v, true));
  if (kind >= 5 && kind <= 8) [1,2,1].forEach((v, i) => view.setUint32(32+i*4, v, true));
  return bytes;
}

time = 0; delivered.length = 0;
const calibration = installRtcInputImpairment({
  inputLabel: 'th06-input', controlLabel: 'th06-control', oneWayMs: 50, jitterMs: 0,
  dropEvery: 1, blackoutFrame: 900, blackoutMs: 1000,
  spikeFrame: 900, spikeMs: 1000, spikeDelayMs: 800,
}, env);
for (const players of [2,3]) for (let sender = 0; sender < players; ++sender)
  for (const kind of [2,3]) fast.send(adonis(sender, kind, players));
assert.equal(delivered.length, 0, 'all seats delay both probe and echo');
assert.equal(jobs.size, 10);
assert.equal(calibration.stats.calibrationMatched, 10);
assert.equal(calibration.stats.calibrationProbes, 5);
assert.equal(calibration.stats.calibrationEchoes, 5);
assert.equal(calibration.stats.matched, 0, 'calibration cannot qualify gameplay impairment');
assert.equal(calibration.stats.dropped, 0, 'frame loss never applies to calibration');
assert.equal(calibration.stats.blackoutDropped, 0);
assert.equal(calibration.stats.spikeStartMs, null, 'automatic D is not frame 0xffffffff');
advance(49); assert.equal(delivered.length, 0);
advance(50); assert.equal(delivered.length, 10);
assert.ok(delivered.every(row => row.time === 50));
assert.equal(calibration.stats.calibrationSent, 10);
assert.equal(calibration.stats.calibrationDeliveredDelayTotalMs, 500);
assert.equal(calibration.stats.sent, 0);
for (const sender of [0,1]) for (const kind of [1,4,5,6,7,8,9,10,11]) control.send(adonis(sender, kind));
assert.equal(delivered.length, 28, 'ADS negotiation remains an explicitly bypassed control lane');
assert.equal(calibration.stats.calibrationControlBypassed, 18);
assert.equal(calibration.stats.calibrationControlPolicy, 'bypass');
assert.equal(calibration.stats.controlInputs, 0);
fast.send(packet);
assert.equal(calibration.stats.blackoutDropped, 1, 'ordinary input still owns frame impairment');
const backing = new Uint8Array(80); backing.set(adonis(1, 3), 8);
const subview = new DataView(backing.buffer, 8, 64), expected = backing.slice(8, 72);
fast.send(subview); backing.fill(0);
advance(100);
assert.deepEqual(delivered.at(-1).data, expected, 'queued probe/echo copies the actual view bytes');
assert.equal(calibration.stats.calibrationSent, 11, 'an active gameplay blackout/spike does not delay or drop ADS further');
assert.equal(calibration.stats.calibrationMaxDeliveredDelayMs, 50);
assert.equal(calibration.stats.spikeDelayed, 0);
fast.send(adonis(0)); calibration.uninstall();
assert.equal(calibration.stats.calibrationCanceled, 1);
assert.equal(calibration.stats.canceled, 0);
assert.equal(jobs.size, 0); assert.equal(calibration.stats.queuedBytes, 0);
console.log('eagler-common RTC input impairment: PASS ADS/probe-echo/all-seats/frame-isolation/control-policy/cleanup');

time = 0; delivered.length = 0;
const headers = installRtcInputImpairment({inputLabel:'th06-input', controlLabel:'th06-control', oneWayMs:50, jitterMs:0}, env);
for (const mutate of [
  p => p[0] = 0, p => p[3] = 3, p => p[4] = 12, p => p[5] = 2,
  p => p[6] = 0, p => p[7] = 0, p => p[56] = 1, p => p[57] = p[5],
  p => p[28] = 0, p => p[28] = 130, p => p[60] = 4, p => p[58] = 1,
  p => p.fill(0,8,16), p => new DataView(p.buffer).setUint32(24, 10, true),
]) { const bytes = adonis(1); mutate(bytes); fast.send(bytes); }
fast.send(adonis(1).subarray(0,63));
const unrelated = new Uint8Array(64); unrelated[5] = 1; fast.send(unrelated);
for (const mutate of [p => p[0] = 0, p => p[1] = 0, p => p[2] = 0, p => p[3] = 0,
                      p => p[4] = 3, p => p[5] = 2, p => p[7] = 1, p => p[42] = 1]) {
  const bytes = packet.slice(); mutate(bytes); fast.send(bytes);
}
assert.equal(jobs.size, 0, 'malformed/other protocol packets bypass the input experiment');
assert.equal(headers.stats.matched, 0); assert.equal(headers.stats.calibrationMatched, 0);
assert.equal(headers.stats.calibrationControlBypassed, 0);
for (const title of ['6','7','8','9','A','T']) {
  const bytes = packet.slice(); bytes[1] = title.charCodeAt(0); fast.send(bytes);
}
assert.equal(headers.stats.matched, 6, 'real E?NP title profiles retain input delay');
const pureDelayProbe = adonis(1); pureDelayProbe[6] = 1; pureDelayProbe[7] = 0;
new DataView(pureDelayProbe.buffer).setUint32(24, 9, true); fast.send(pureDelayProbe);
assert.equal(headers.stats.calibrationMatched, 1, 'pure delay/manual D uses the same measured input lane');
headers.uninstall(); assert.equal(headers.stats.canceled, 6);
assert.equal(headers.stats.calibrationCanceled, 1); assert.equal(jobs.size, 0);

function inputDelays(withCalibration) {
  time = 0;
  const fixture = installRtcInputImpairment({inputLabel:'th06-input', controlLabel:'th06-control',
    oneWayMs:50, jitterMs:10, seed:7135}, env);
  const delays = [];
  for (let i = 0; i < 4; ++i) {
    if (withCalibration) fast.send(adonis(i % 2, i % 2 ? 3 : 2));
    fast.send(packet); delays.push(jobs.get(serial).due);
  }
  assert.ok([...jobs.values()].every(job => job.due >= 40 && job.due <= 60));
  fixture.uninstall(); assert.equal(jobs.size, 0); assert.equal(fixture.stats.queuedBytes, 0);
  return delays;
}
assert.deepEqual(inputDelays(true), inputDelays(false), 'calibration does not shift the gameplay jitter tape');

time = 0; delivered.length = 0;
const loss = installRtcInputImpairment({inputLabel:'th06-input', controlLabel:'th06-control',
  oneWayMs:50, jitterMs:0, dropEvery:2}, env);
fast.send(packet); fast.send(adonis(0)); fast.send(packet);
assert.equal(loss.stats.matched, 2); assert.equal(loss.stats.dropped, 1);
assert.equal(loss.stats.calibrationMatched, 1);
advance(50);
assert.equal(loss.stats.sent, 1); assert.equal(loss.stats.calibrationSent, 1);
loss.uninstall(); assert.equal(jobs.size, 0);
console.log('eagler-common RTC input impairment: PASS protocol-headers/independent-jitter/input-loss-sequence');
