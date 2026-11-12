import {test} from 'node:test';
import assert from 'node:assert/strict';
import {verifyTiming, timingNegativeControls} from './browser/formal-evidence.mjs';

const bits = value => new Uint32Array(new Float32Array([value]).buffer)[0];
const config = {source: 'synthetic production event order', inputDelayFrames: 0,
  predictionLimitFrames: 12, adonisMode: 2, localSeat: 0, players: 2};

function capture() {
  const events = [], audit = [];
  const add = (kind, frame, value = 0, detail = 0) => events.push({kind, frame, value, detail, us: events.length * 100});
  const read = (frame, buttons, mode = 0, x = 0, y = 0) => {
    add('read', frame, buttons, mode); add('read-hosted', frame, buttons);
    add('read-touch-xy', frame, bits(x), bits(y));
    add('sample', frame, buttons); add('send', frame, buttons);
  };
  const apply = (frame, buttons, replay = false, mode = 0, x = 0, y = 0) => {
    add('apply', frame, buttons, (replay ? 0x100 : 0) | 2);
    add('apply-touch', frame, mode); add('apply-touch-xy', frame, bits(x), bits(y));
    add('simulate', frame, buttons, +replay);
    if (!replay) add('present', frame);
  };
  read(0, 0); apply(0, 0); add('confirm', 0);
  add('device', 1, 44, 1); add('device-map', 1, 1, (44 << 1) | 1);
  add('touch-device', 1, 1, 2); add('touch-device-xy', 1, bits(.2), bits(.8));
  read(1, 1, 1, -45.25, 389.5); apply(1, 1, false, 1, -45.25, 389.5);
  add('restore', 1, 1);
  apply(1, 1, true, 1, -45.25, 389.5);
  add('corrected', 1, 1); add('present', 1); add('confirm', 1, 1);
  add('device', 2, 44, 0); add('device-map', 2, 1, 44 << 1);
  read(2, 0); apply(2, 0); add('confirm', 2);
  for (let frame = 0; frame < 3; ++frame) audit.push({frame, inputs: [frame === 1 ? 1 : 0, 0]});
  return {chunks: [{clock: 'steady-microseconds', overflow: 0, events}], audit};
}

test('physical reads, mapped events and saved analog payload survive correction', () => {
  const {chunks, audit} = capture(), before = structuredClone(chunks);
  const result = verifyTiming(chunks, audit, 0, config);
  assert.deepEqual(chunks, before, 'Read-only verifier preserves captured evidence');
  assert.equal(result.samples, 3);
  assert.equal(result.observedCaptureToSimulationFrameOffset, 0);
  assert.equal(result.deviceToSampleAcceptance.status, 'OBSERVED');
  assert.equal(result.deviceToSampleAcceptance.eventsFollowedToRead, 3);
  assert.equal(result.deviceToSampleAcceptance.analogApplications, 2);
  assert.equal(result.deviceToSampleMs.count, 3);
  assert.equal(result.preApplyCorrectionWallMs.count, 3);
  assert.equal(result.latencyAcceptance.status, 'NOT_ASSESSED');
});

test('physical read and analog corruption controls are all rejected', () => {
  const {chunks, audit} = capture();
  const controls = timingNegativeControls(chunks, audit, 0, config);
  assert(controls.length >= 18);
  assert(controls.every(control => control.rejected), JSON.stringify(controls));
  assert.throws(() => verifyTiming([{...chunks[0], events: chunks[0].events
    .filter(event => !['read', 'read-hosted', 'read-touch-xy', 'apply-touch', 'apply-touch-xy'].includes(event.kind))}], audit, 0, config),
  /physical read/);
  assert.throws(() => verifyTiming([{...chunks[0], events: chunks[0].events
    .map(event => event.kind === 'device-map' ? {...event, detail: event.detail ^ 1} : event)}], audit, 0, config),
  /press\/release/);
  assert.throws(() => verifyTiming([{...chunks[0], events: chunks[0].events
    .map(event => event.kind === 'read-touch-xy' ? {...event, value: 0x7fc00000} : event)}], audit, 0, config),
  /Finite/);
  const replayCompletion = chunks[0].events.find(event => event.kind === 'simulate' && event.detail === 1);
  assert.throws(() => verifyTiming([{...chunks[0], events: chunks[0].events
    .flatMap(event => event === replayCompletion
      ? [event, {...event, kind: 'present'}] : [event])}], audit, 0, config),
  /Correction completion must be recorded|Presentation waits for the complete correction/);
});

test('a depth-three correction rejects an intermediate presentation before completion', () => {
  const events = [], audit = [];
  const add = (kind, frame, value = 0, detail = 0) =>
    events.push({kind, frame, value, detail, us: events.length * 100});
  const apply = (frame, replay = false) => {
    add('apply', frame, 0, replay ? 0x102 : 2);
    add('apply-touch', frame); add('apply-touch-xy', frame);
    add('simulate', frame, 0, +replay);
  };
  for (let frame = 0; frame < 4; ++frame) {
    add('read', frame); add('read-hosted', frame); add('read-touch-xy', frame);
    add('sample', frame); add('send', frame); apply(frame); add('present', frame);
    audit.push({frame, inputs: [0, 0]});
    if (frame === 0) add('confirm', 0);
  }
  add('restore', 1, 3);
  apply(1, true);
  const intermediateCompletion = events.at(-1);
  apply(2, true); apply(3, true);
  add('corrected', 3, 3); add('present', 3);
  for (let frame = 1; frame < 4; ++frame) add('confirm', frame);
  const chunks = [{clock: 'steady-microseconds', overflow: 0, events}];
  const result = verifyTiming(chunks, audit, 0, config);
  assert.equal(result.corrections, 1);
  assert.equal(result.correctionDistances.max, 3);
  assert.equal(result.replayedApplications, 3);
  const changed = events.flatMap(event => event === intermediateCompletion
    ? [event, {...event, kind: 'present'}] : [event]);
  // The first replayed frame has two frames left. This must reach the new
  // presentation guard, rather than the earlier completed-correction guard.
  assert.throws(() => verifyTiming([{...chunks[0], events: changed}], audit, 0, config),
    error => error.message.startsWith('Presentation waits for the complete correction\n') &&
      error.actual?.complete === false && error.actual?.replayed === 1 &&
      error.actual?.end === 4 && error.expected === null);
});

function bufferedTapCapture({cycle = false} = {}) {
  const events = [], audit = [];
  const add = (kind, frame, value = 0, detail = 0) =>
    events.push({kind, frame, value, detail, us: events.length * 100});
  const read = (frame, buttons, edge) => {
    add('read', frame, buttons);
    if (edge !== undefined) add('read-hosted-edge', frame, 44, edge);
    add('read-hosted-physical', frame, cycle && frame >= 2 ? 1 : 0);
    add('read-hosted', frame, buttons); add('read-touch-xy', frame);
    add('sample', frame, buttons); add('send', frame, buttons);
  };
  const apply = (frame, buttons, replay = false) => {
    add('apply', frame, buttons, replay ? 0x102 : 2);
    add('apply-touch', frame); add('apply-touch-xy', frame);
    add('simulate', frame, buttons, +replay);
  };
  read(0, 0); apply(0, 0); add('present', 0); add('confirm', 0);
  read(1, 0); apply(1, 0); add('present', 1);
  add('restore', 1, 1);
  add('device', 2, 44, 1); add('device-map', 2, 1, (44 << 1) | 1);
  // Repeated keydown never creates another logical press.
  add('device', 2, 44, 1); add('device-map', 2, 1, (44 << 1) | 1);
  add('device', 2, 44, 0); add('device-map', 2, 1, 44 << 1);
  if (cycle) {
    add('device', 2, 44, 1); add('device-map', 2, 1, (44 << 1) | 1);
  }
  read(2, 1, 1);
  apply(1, 0, true); add('corrected', 1, 1); add('present', 1);
  apply(2, 1); add('present', 2); add('confirm', 1); add('confirm', 2, 1);
  read(3, 0, 0); apply(3, 0); add('present', 3); add('confirm', 3);
  if (cycle) { read(4, 1, 1); apply(4, 1); add('present', 4); add('confirm', 4, 1); }
  for (let frame = 0; frame < (cycle ? 5 : 4); ++frame)
    audit.push({frame, inputs: [frame === 2 || frame === 4 ? 1 : 0, 0]});
  return {events, chunks: [{clock: 'steady-microseconds', overflow: 0, events}], audit};
}

test('a quick hosted tap during correction survives as FIFO samples with separate physical latency', () => {
  const {events, chunks, audit} = bufferedTapCapture(), original = structuredClone(chunks);
  const result = verifyTiming(chunks, audit, 0, config);
  assert.deepEqual(chunks, original, 'Verification is read-only');
  assert.equal(result.samples, 4);
  assert.equal(result.replayedApplications, 1);
  assert.equal(result.deviceToSampleAcceptance.hostedInputMode, 'BUFFERED_EDGES');
  assert.equal(result.deviceToSampleAcceptance.hostedEdgesConsumed, 2);
  assert.equal(result.deviceToSampleAcceptance.duplicateDeviceEvents, 1);
  assert.equal(result.deviceToSampleAcceptance.pendingHostedEdges, 0);
  assert.equal(result.deviceToPhysicalReadMs.count, 3);
  assert.equal(result.deviceToSampleMs.count, 2);
  assert.equal(result.hostedEdgeQueueFrames.min, 0);
  assert.equal(result.hostedEdgeQueueFrames.max, 1);
  const release = events.find(event => event.kind === 'device' && event.detail === 0);
  const consumedSample = events.find(event => event.kind === 'sample' && event.frame === 3);
  const immediateSample = events.find(event => event.kind === 'sample' && event.frame === 2);
  assert.equal(result.deviceToSampleMs.max, (consumedSample.us - release.us) / 1000);
  assert(result.deviceToSampleMs.max > (immediateSample.us - release.us) / 1000,
    'Release latency belongs to its later consumption frame, not the arrival frontier');
  assert.equal(result.deviceToSampleAcceptance.status, 'OBSERVED');
  const controls = timingNegativeControls(chunks, audit, 0, config);
  for (const name of ['missing-hosted-edge-consumption', 'wrong-hosted-edge-order',
    'duplicate-hosted-edge-consumption', 'missing-hosted-physical-snapshot', 'wrong-hosted-physical-state'])
    assert(controls.find(control => control.mutation === name)?.rejected, name);
});

test('buffered hosted snapshots reject dropped, reordered and repeated edge consumption', () => {
  const {events, chunks, audit} = bufferedTapCapture();
  const first = events.find(event => event.kind === 'read-hosted-edge');
  const verify = changed => verifyTiming([{...chunks[0], events: changed}], audit, 0, config);
  assert.throws(() => verify(events.filter(event => event !== first)),
    /Every pending key consumes one edge/);
  assert.throws(() => verify(events.map(event => event === first ? {...event, detail: 0} : event)),
    /FIFO press\/release order/);
  assert.throws(() => verify(events.flatMap(event => event === first ? [event, {...event}] : [event])),
    /At most one hosted edge per key per read/);
  const physical = events.find(event => event.kind === 'read-hosted-physical' && event.frame === 2);
  assert.throws(() => verify(events.map(event => event === physical ? {...event, value: 1} : event)),
    /Physical hosted snapshot reflects current device state/);
});

test('down-up-down buffers preserve the intermediate release while the physical key remains down', () => {
  const {events, chunks, audit} = bufferedTapCapture({cycle: true});
  const result = verifyTiming(chunks, audit, 0, config);
  assert.equal(result.samples, 5);
  assert.equal(result.deviceToSampleAcceptance.hostedEdgesConsumed, 3);
  assert.equal(result.deviceToSampleAcceptance.pendingHostedEdges, 0);
  assert.equal(result.deviceToPhysicalReadMs.count, 4);
  assert.equal(result.deviceToSampleMs.count, 3);
  assert.equal(result.hostedEdgeQueueFrames.max, 2);
  const releaseSnapshot = events.find(event => event.kind === 'read-hosted' && event.frame === 3);
  assert.throws(() => verifyTiming([{...chunks[0], events: events.map(event =>
    event === releaseSnapshot ? {...event, value: 1} : event)}], audit, 0, config),
    /Aggregate physical read contains hosted buttons|Consumed hosted snapshot reflects buffered device edges/);
});
