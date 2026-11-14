// Independent verification of captured production-room evidence. No browser
// control or game-state mutation. Run after formal-flow.mjs completes.
import assert from 'node:assert/strict';
import {readFile, writeFile, readdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';

export function compareAudit(...arguments_) {
  const frames = arguments_.pop(), histories = arguments_;
  assert(Number.isSafeInteger(frames) && frames > 0, 'Nonempty confirmed history');
  assert([2, 3].includes(histories.length), 'Every configured seat must be compared');
  for (const [seat, rows] of histories.entries()) {
    assert.equal(rows.length, frames, 'Confirmed history length for seat ' + seat);
    for (let frame = 0; frame < frames; frame++) {
      assert.equal(rows[frame].frame, frame, 'Missing/duplicate frame for seat ' + seat);
      for (const domain of ['core', 'full', 'host'])
        assert.match(rows[frame][domain] ?? '', /^[0-9a-f]{1,16}$/, 'Missing or malformed ' + domain);
      assert.equal(typeof rows[frame].domains, 'string', 'Required domain audit');
      assert.equal(rows[frame].inputs?.length, histories.length, 'All configured seat inputs');
      assert.deepEqual(histories[0][frame], rows[frame], 'Confirmed state/input at ' + frame + ' seat ' + seat);
    }
  }
}

const distribution = values => {
  const sorted = [...values].sort((a, b) => a - b);
  return sorted.length ? {count: sorted.length, min: sorted[0],
    p50: sorted[Math.floor(sorted.length * .5)], p95: sorted[Math.floor(sorted.length * .95)],
    p99: sorted[Math.floor(sorted.length * .99)], max: sorted.at(-1)} : {count: 0};
};

export function verifyTiming(chunks, audit, seat, configuration) {
  assert(configuration?.source, 'Recorded configuration source is required');
  assert.equal(configuration.inputDelayFrames, 0, 'Recorded default input delay');
  assert.equal(configuration.predictionLimitFrames, 12, 'Recorded prediction window');
  assert(Number.isInteger(configuration.adonisMode) && configuration.adonisMode !== 1,
    'Recorded rollback mode');
  assert.equal(configuration.localSeat, seat, 'Recorded local seat');
  assert([2, 3].includes(configuration.players), 'Recorded player count');
  const sampled = new Map(), applied = new Map(), sent = new Map(), confirmed = new Map();
  const reads = new Map(), hosted = new Map(), pendingDevices = [], deviceReads = [];
  const bufferedHosted = chunks.some(chunk => chunk.events.some(event => event.kind === 'read-hosted-physical'));
  const hostedQueues = new Map(), consumedHosted = new Map(), deviceSampleReads = [];
  const hostedEdgeQueueFrames = [];
  let hostedEdgesConsumed = 0, duplicateDeviceEvents = 0, clearedHostedEdges = 0;
  const readToSample = [], deviceToRead = [], applyToPresent = [], correctionSpans = [], simulationSpans = [];
  let pendingRead = null, pendingDevice = null, pendingTouchDevice = null;
  let touchObserved = 0, touchEvents = 0, hostedBaselineKnown = false;
  let mappedHostedReads = 0, firstHostedRead = true;
  const rawToButtons = raw => (raw & 0x8ff) | ((raw & 0x100) ? 0x8000 : 0) |
    ((raw & 0x80000) ? 0x1000 : 0) | ((raw & 0x200000) ? 0x2000 : 0) |
    ((raw & 0x10000) ? 0x4000 : 0) | ((raw & 0x200) && !(raw & 0x800) ? 0x200 : 0);
  const finiteBits = bits => {
    const bytes = new Uint32Array([bits]);
    return Number.isFinite(new Float32Array(bytes.buffer)[0]);
  };
  const simulated = new Map(), presents = [], corrections = [], frameOffsets = [];
  const sampleToApply = [], sampleToSend = [], sampleToSchedule = [], sampleToPresent = [];
  const sampleQueueFrames = [];
  let previousTime = 0, predictedApplications = 0, replayedApplications = 0;
  let cursor = 0, freshFrame = 0, pendingApply = null, correction = null;
  const frames = audit.length, inRange = frame => frame < frames;
  for (const chunk of chunks) {
    assert.equal(chunk.clock, 'steady-microseconds', 'Timing clock identity');
    assert.equal(chunk.overflow, 0, 'Timing capture overflow');
    for (const event of chunk.events) {
      assert(Number.isSafeInteger(event.us) && Number.isSafeInteger(event.frame), 'Integer timing identity');
      assert(event.frame >= 0 && event.us >= 0, 'Nonnegative timing identity');
      assert(event.us >= previousTime, 'Timing clock must be monotonic');
      previousTime = event.us;
      const {kind, frame} = event;
      if (correction?.complete) assert.equal(kind, 'corrected', 'Correction completion must be recorded');
      if (kind === 'device') {
        assert.equal(pendingDevice, null, 'Previous device event has a mapped scan');
        pendingDevice = event;
      } else if (kind === 'device-map') {
        assert(pendingDevice, 'Device mapping requires a physical event');
        const scan = event.detail >>> 1, down = !!(event.detail & 1);
        assert.equal(scan, pendingDevice.value, 'Device mapping retains its scan');
        assert.equal(down, !!pendingDevice.detail, 'Device mapping retains its press/release');
        assert.equal(frame, pendingDevice.frame, 'Device mapping retains its capture frontier');
        if (hosted.has(scan)) assert.equal(event.value, hosted.get(scan).mask,
          'A hosted scan retains its production button mapping');
        if (bufferedHosted) {
          const previous = hosted.get(scan)?.down ?? false;
          if (down !== previous) {
            const queue = hostedQueues.get(scan) ?? [];
            queue.push({device: pendingDevice, mask: event.value, down});
            hostedQueues.set(scan, queue);
          } else ++duplicateDeviceEvents;
        }
        hosted.set(scan, {mask: event.value, down});
        pendingDevices.push(pendingDevice);
        pendingDevice = null;
      } else if (kind === 'touch-device') {
        assert.equal(pendingTouchDevice, null, 'Previous touch event has its coordinate payload');
        assert([0, 1, 2].includes(event.value), 'Touch event type');
        pendingTouchDevice = event;
      } else if (kind === 'touch-device-xy') {
        assert(pendingTouchDevice, 'Touch event coordinates require their event');
        assert.equal(frame, pendingTouchDevice.frame, 'Touch event frame');
        assert(finiteBits(event.value) && finiteBits(event.detail), 'Finite touch event coordinates');
        pendingDevices.push(pendingTouchDevice);
        pendingTouchDevice = null;
        ++touchEvents;
      } else if (kind === 'device-clear') {
        hosted.clear();
        consumedHosted.clear();
        for (const queue of hostedQueues.values()) clearedHostedEdges += queue.length;
        hostedQueues.clear();
        hostedBaselineKnown = true;
      } else if (kind === 'read') {
        assert.equal(pendingRead, null, 'A physical read is sampled exactly once');
        assert.equal(frame, reads.size, 'Physical reads follow the fresh capture frontier');
        assert([0, 1, 2].includes(event.detail), 'Physical touch mode');
        pendingRead = {event, hosted: false, touch: false, physical: false, edges: new Set()};
      } else if (kind === 'read-hosted-edge') {
        assert(bufferedHosted, 'Buffered hosted edge requires physical snapshots');
        assert(pendingRead && !pendingRead.physical && !pendingRead.hosted,
          'Hosted edges follow the read and precede its snapshots');
        assert.equal(frame, pendingRead.event.frame, 'Hosted edge consumes the current sample frame');
        assert([0, 1].includes(event.detail), 'Hosted edge is a press or release');
        const scan = event.value;
        assert(!pendingRead.edges.has(scan), 'At most one hosted edge per key per read');
        const queue = hostedQueues.get(scan);
        assert(queue?.length, 'Hosted edge requires a pending device transition');
        const edge = queue.shift();
        assert.equal(!!event.detail, edge.down, 'Hosted edge consumes FIFO press/release order');
        assert(event.us >= edge.device.us, 'Device transition precedes hosted edge consumption');
        consumedHosted.set(scan, {mask: edge.mask, down: edge.down});
        pendingRead.edges.add(scan);
        deviceSampleReads.push({frame, deviceUs: edge.device.us});
        if (inRange(frame)) hostedEdgeQueueFrames.push(frame - edge.device.frame);
        ++hostedEdgesConsumed;
      } else if (kind === 'read-hosted-physical') {
        assert(bufferedHosted && pendingRead && !pendingRead.physical && !pendingRead.hosted,
          'One physical hosted snapshot before the consumed snapshot');
        assert.equal(frame, pendingRead.event.frame, 'Physical hosted snapshot frame');
        if (firstHostedRead) {
          hostedBaselineKnown ||= event.value === 0;
          firstHostedRead = false;
        }
        const expected = [...hosted.values()].reduce((mask, key) => mask | (key.down ? key.mask : 0), 0);
        if (hostedBaselineKnown)
          assert.equal(event.value, expected, 'Physical hosted snapshot reflects current device state');
        else assert.equal((event.value & expected) >>> 0, expected,
          'Known physical hosted keys survive the snapshot');
        for (const [scan, queue] of hostedQueues)
          assert(!queue.length || pendingRead.edges.has(scan),
            'Every pending key consumes one edge at the next read');
        pendingRead.physical = true;
      } else if (kind === 'read-hosted') {
        assert(pendingRead && !pendingRead.hosted, 'One hosted-key snapshot per physical read');
        assert.equal(frame, pendingRead.event.frame, 'Hosted-key read frame');
        assert.equal((event.value & pendingRead.event.value) >>> 0, event.value,
          'Aggregate physical read contains hosted buttons');
        if (bufferedHosted) assert(pendingRead.physical, 'Buffered read requires its physical hosted snapshot');
        if (!bufferedHosted && firstHostedRead) {
          hostedBaselineKnown ||= event.value === 0;
          firstHostedRead = false;
        }
        const expected = [...(bufferedHosted ? consumedHosted : hosted).values()]
          .reduce((mask, key) => mask | (key.down ? key.mask : 0), 0);
        if (hostedBaselineKnown) {
          assert.equal(event.value, expected, bufferedHosted
            ? 'Consumed hosted snapshot reflects buffered device edges'
            : 'Physical hosted read reflects received device events');
          ++mappedHostedReads;
        } else {
          assert.equal((event.value & expected) >>> 0, expected, 'Known held device events survive the read');
        }
        for (const device of pendingDevices.splice(0)) {
          assert.equal(device.frame, frame, 'Device event reaches its next due physical read');
          assert(event.us >= device.us, 'Device event precedes physical read');
          deviceReads.push({frame, deviceUs: device.us, readUs: pendingRead.event.us});
          if (!bufferedHosted || device.kind === 'touch-device')
            deviceSampleReads.push({frame, deviceUs: device.us});
          if (inRange(frame)) deviceToRead.push((pendingRead.event.us - device.us) / 1000);
        }
        pendingRead.hosted = true;
      } else if (kind === 'read-touch-xy') {
        assert(pendingRead && !pendingRead.touch, 'One analog payload per physical read');
        assert.equal(frame, pendingRead.event.frame, 'Analog read frame');
        assert(finiteBits(event.value) && finiteBits(event.detail), 'Finite physical analog coordinates');
        pendingRead.x = event.value; pendingRead.y = event.detail;
        pendingRead.touch = true;
      } else if (kind === 'sample') {
        assert(pendingRead?.hosted && pendingRead.touch, 'Saved sample requires its complete physical read');
        assert.equal(frame, pendingRead.event.frame, 'Sample consumes the current physical read');
        assert.equal(event.value, rawToButtons(pendingRead.event.value), 'Saved buttons match the production physical read');
        assert(event.us >= pendingRead.event.us, 'Physical read precedes saved sample');
        if (inRange(frame)) readToSample.push((event.us - pendingRead.event.us) / 1000);
        reads.set(frame, pendingRead);
        pendingRead = null;
        assert.equal(frame, sampled.size, 'Exactly one contiguous saved local sample per frame');
        sampled.set(frame, event);
        // This is a measured queue, separate from the configured frame offset.
        if (inRange(frame)) sampleQueueFrames.push(frame - freshFrame);
      } else if (kind === 'send' || kind === 'schedule') {
        assert(sampled.has(frame), 'Send requires a saved sample');
        assert(!sent.has(frame), 'Input scheduled once');
        assert.equal(event.value, sampled.get(frame).value, 'Scheduled/sent input matches saved sample');
        sent.set(frame, event);
      } else if (kind === 'apply') {
        assert.equal(pendingApply, null, 'Previous application must finish simulation');
        assert.equal(frame, cursor, 'Applications follow the simulation cursor');
        assert(sampled.has(frame), 'Apply requires saved local input');
        assert(sent.has(frame), 'Input is scheduled before application');
        assert.equal(event.value, sampled.get(frame).value, 'Replay/fresh apply uses the saved local input');
        const replay = !!(event.detail & 0x100);
        assert.equal(replay, !!correction, 'Replay application requires a measured restore');
        const mask = event.detail & 255;
        assert.equal(mask & (1 << seat), 0, 'The local input is never predicted');
        assert.equal(event.detail & ~(0x100 | ((1 << configuration.players) - 1)), 0,
          'Application prediction flags');
        if (replay) {
          assert(applied.has(frame), 'Replay only consumes previously applied history');
          if (inRange(frame)) replayedApplications++;
        } else {
          assert(!applied.has(frame), 'One initial application per frame');
          assert.equal(frame, freshFrame, 'Initial simulation advances once per captured frame');
          const capture = sampled.get(freshFrame);
          if (inRange(frame)) frameOffsets.push(frame - capture.frame);
          ++freshFrame;
          applied.set(frame, event);
        }
        if (inRange(frame) && mask) predictedApplications++;
        pendingApply = {...event};
      } else if (kind === 'apply-touch') {
        assert(pendingApply && !pendingApply.touch, 'Applied analog mode belongs to one granted frame');
        assert.equal(frame, pendingApply.frame, 'Applied analog mode frame');
        assert.equal(event.value, reads.get(frame).event.detail, 'Saved analog mode is used during fresh/replay simulation');
        pendingApply.touch = {mode: event.value};
      } else if (kind === 'apply-touch-xy') {
        assert(pendingApply?.touch && !pendingApply.touch.xy, 'Applied analog coordinates follow the granted mode');
        assert.equal(frame, pendingApply.frame, 'Applied analog coordinate frame');
        const read = reads.get(frame);
        assert.equal(event.value, read.x, 'Saved analog X is used during fresh/replay simulation');
        assert.equal(event.detail, read.y, 'Saved analog Y is used during fresh/replay simulation');
        pendingApply.touch.xy = true;
        if (inRange(frame) && pendingApply.touch.mode) ++touchObserved;
      } else if (kind === 'simulate') {
        assert(pendingApply, 'Simulation completion requires its application');
        assert.equal(frame, cursor, 'Simulation completion follows its application');
        assert.equal(frame, pendingApply.frame, 'Application/completion frame identity');
        assert.equal(event.value, pendingApply.value, 'Application/completion local input identity');
        assert.equal(event.detail, correction ? 1 : 0, 'Simulation completion replay identity');
        assert(pendingApply.touch?.xy, 'Simulation consumes the saved full analog payload');
        simulationSpans.push({start: pendingApply.us, end: event.us, replay: !!correction});
        simulated.set(frame, event);
        pendingApply = null;
        ++cursor;
        if (correction) {
          ++correction.replayed;
          assert(cursor <= correction.end, 'Replay cannot pass its saved forward frontier');
          correction.complete = cursor === correction.end;
        }
      } else if (kind === 'confirm') {
        assert.equal(frame, confirmed.size, 'Confirmations are contiguous and unique');
        assert(!confirmed.has(frame), 'Confirmation is unique');
        assert(simulated.has(frame) && frame < cursor, 'Confirmation follows completed, retained simulation');
        assert.equal(event.value, sampled.get(frame)?.value, 'Confirmation retains local input');
        confirmed.set(frame, event);
      } else if (kind === 'present') {
        assert.equal(pendingApply, null, 'Presentation follows completed simulation');
        assert.equal(correction, null, 'Presentation waits for the complete correction');
        assert.equal(frame, cursor - 1, 'Presentation identifies the actual simulation frontier');
        assert(frame >= (presents.at(-1)?.frame ?? 0), 'Presentation never rewinds the displayed frame');
        assert(simulated.has(frame), 'Presentation requires retained simulation');
        presents.push(event);
      } else if (kind === 'restore') {
        assert.equal(pendingApply, null, 'Restore cannot interrupt an application');
        assert(frame < cursor && frame >= confirmed.size, 'Restore only rewinds unconfirmed history');
        const distance = cursor - frame;
        assert.equal(event.value, distance, 'Restore depth recomputed from the simulation cursor');
        assert(distance <= configuration.predictionLimitFrames, 'Restore depth fits prediction window');
        if (!correction) correction = {end: cursor, replayed: 0, complete: false, started: event.us};
        assert(correction.end - frame <= configuration.predictionLimitFrames,
          'Restarted correction fits its original forward frontier');
        if (inRange(frame)) corrections.push(distance);
        for (let discarded = frame; discarded < cursor; ++discarded) simulated.delete(discarded);
        cursor = frame;
      } else if (kind === 'corrected') {
        assert(correction?.complete, 'Correction completion requires reaching its saved frontier');
        assert.equal(frame, correction.end - 1, 'Corrected frontier identity');
        assert.equal(event.value, correction.replayed, 'Corrected work matches saved-input replay count');
        correctionSpans.push({start: correction.started, end: event.us});
        correction = null;
      }
    }
  }
  assert.equal(pendingApply, null, 'No missing simulation completion at capture tail');
  assert.equal(pendingRead, null, 'No physical read is left unsampled');
  assert.equal(pendingDevice, null, 'No device event is missing its mapping');
  assert.equal(pendingTouchDevice, null, 'No touch event is missing its coordinates');
  assert.equal(reads.size, sampled.size, 'One physical read per saved sample');
  assert(!correction?.complete, 'No missing correction completion at capture tail');
  let presentIndex = 0;
  for (let frame = 0; frame < audit.length; frame++) {
    const sample = sampled.get(frame), apply = applied.get(frame), send = sent.get(frame);
    assert(sample && apply && send && confirmed.has(frame), 'Complete input lifecycle at ' + frame);
    assert.equal(audit[frame].frame, frame, 'Timing/audit frame identity');
    assert.equal(audit[frame].inputs.length, configuration.players, 'All configured input seats are audited');
    assert.equal(sample.value, audit[frame].inputs[seat], 'Confirmed input matches sampled frame N');
    assert.equal(confirmed.get(frame).value, sample.value, 'Confirmation retains local input');
    assert(send.us >= sample.us && apply.us >= send.us && confirmed.get(frame).us >= apply.us,
      'Sample/schedule/application/confirmation ordering');
    sampleToApply.push((apply.us - sample.us) / 1000);
    sampleToSchedule.push((send.us - sample.us) / 1000);
    if (send.kind === 'send') sampleToSend.push((send.us - sample.us) / 1000);
    while (presentIndex < presents.length &&
      (presents[presentIndex].us < apply.us || presents[presentIndex].frame < frame)) presentIndex++;
    assert(presentIndex < presents.length, 'First real presentation coverage at ' + frame);
    sampleToPresent.push((presents[presentIndex].us - sample.us) / 1000);
    applyToPresent.push((presents[presentIndex].us - apply.us) / 1000);
  }
  assert(sampleToSend.length > 0, 'Actual input packet submission must be observed');
  const mapping = distribution(frameOffsets);
  assert.equal(mapping.count, frames, 'Every initial application has a capture/simulation frame mapping');
  assert.equal(mapping.min, configuration.inputDelayFrames, 'Minimum observed frame offset');
  assert.equal(mapping.max, configuration.inputDelayFrames, 'Maximum observed frame offset');
  const preApplyCorrectionWall = [], preApplySimulationWork = [], preApplyOtherWork = [];
  const spanMeasure = spans => {
    const totals = [0];
    for (const span of spans) totals.push(totals.at(-1) + span.end - span.start);
    const through = time => {
      let low = 0, high = spans.length;
      while (low < high) {
        const middle = (low + high) >>> 1;
        if (spans[middle].end <= time) low = middle + 1;
        else high = middle;
      }
      return totals[low] + (low < spans.length ? Math.max(0, time - spans[low].start) : 0);
    };
    return (first, last) => (through(last) - through(first)) / 1000;
  };
  const correctionOverlap = spanMeasure(correctionSpans), simulationOverlap = spanMeasure(simulationSpans);
  for (let frame = 0; frame < frames; ++frame) {
    const first = sampled.get(frame).us, last = applied.get(frame).us;
    const correcting = correctionOverlap(first, last);
    const work = simulationOverlap(first, last);
    preApplyCorrectionWall.push(correcting);
    preApplySimulationWork.push(work);
    preApplyOtherWork.push((last - first) / 1000 - work);
  }
  const deviceToSample = deviceSampleReads.filter(event => inRange(event.frame))
    .map(event => (sampled.get(event.frame).us - event.deviceUs) / 1000);
  return {seat, frames, samples: frames, capturedSamplesTotal: sampled.size,
    configuration, observedCaptureToSimulationFrameOffset: mapping.max,
    captureToInitialSimulationFrameOffsets: mapping,
    sampledQueueFrames: distribution(sampleQueueFrames),
    predictedApplications, replayedApplications, corrections: corrections.length,
    correctionDistances: distribution(corrections),
    scheduledOnlyFrames: frames - sampleToSend.length, sampleToScheduleMs: distribution(sampleToSchedule),
    sampleToSendMs: distribution(sampleToSend), sampleToApplyMs: distribution(sampleToApply),
    sampleToPresentMs: distribution(sampleToPresent),
    physicalReadToSampleMs: distribution(readToSample), deviceToPhysicalReadMs: distribution(deviceToRead),
    deviceToSampleMs: distribution(deviceToSample), initialApplyToPresentMs: distribution(applyToPresent),
    hostedEdgeQueueFrames: distribution(hostedEdgeQueueFrames),
    preApplyCorrectionWallMs: distribution(preApplyCorrectionWall),
    preApplySimulationWorkMs: distribution(preApplySimulationWork),
    preApplyOtherWorkMs: distribution(preApplyOtherWork),
    latencyAcceptance: {status: 'NOT_ASSESSED', reason: 'No wall-time latency threshold is configured; queue and latency distributions are observations.'},
    deviceToSampleAcceptance: {status: mappedHostedReads === reads.size ? 'OBSERVED' : 'NOT_ASSESSED',
      physicalReads: reads.size, mappedHostedReads, eventsFollowedToRead: deviceReads.length,
      hostedInputMode: bufferedHosted ? 'BUFFERED_EDGES' : 'LEGACY_SNAPSHOT',
      hostedEdgesConsumed, duplicateDeviceEvents, clearedHostedEdges,
      pendingHostedEdges: [...hostedQueues.values()].reduce((count, queue) => count + queue.length, 0),
      eventsFollowedToSample: deviceSampleReads.length,
      analogApplications: touchObserved, touchEvents,
      analogCoverage: touchObserved ? 'EXERCISED' : 'UNEXERCISED',
      reason: 'Physical read, saved buttons and analog payload, and fresh/replay consumption are checked. Wall-time latency has no configured threshold.'}};
}

export function timingNegativeControls(chunks, audit, seat, configuration) {
  const events = chunks.flatMap(chunk => chunk.events), controls = [];
  const sample = events.find(event => event.kind === 'sample');
  const fresh = events.find(event => event.kind === 'apply' && !(event.detail & 0x100));
  const replay = events.find(event => event.kind === 'apply' && (event.detail & 0x100));
  const firstPresent = events.find(event => event.kind === 'present');
  const confirmation = events.find(event => event.kind === 'confirm' && event.frame === sample.frame);
  const check = (mutation, changed, available = true, config = configuration) => {
    if (!available) {
      controls.push({mutation, rejected: false, status: 'UNEXERCISED', reason: 'This capture does not exercise the mutated event.'});
      return;
    }
    let reason;
    try { verifyTiming([{clock: 'steady-microseconds', overflow: 0, events: changed}], audit, seat, config); }
    catch (error) { reason = error.message; }
    assert(reason, mutation + ' must fail');
    controls.push({mutation, rejected: true, reason});
  };
  const read = events.find(event => event.kind === 'read');
  const analog = events.find(event => event.kind === 'apply-touch-xy');
  const hostedRead = events.find(event => event.kind === 'read-hosted');
  const physicalHosted = events.find(event => event.kind === 'read-hosted-physical' && event.frame > 0)
    ?? events.find(event => event.kind === 'read-hosted-physical');
  const hostedEdge = events.find(event => event.kind === 'read-hosted-edge');
  const touchDevice = events.find(event => event.kind === 'touch-device-xy');
  if (physicalHosted) {
    check('missing-hosted-physical-snapshot', events.filter(event => event !== physicalHosted));
    check('wrong-hosted-physical-state', events.map(event => event === physicalHosted
      ? {...event, value: event.value ^ 1} : event));
  }
  if (hostedEdge) {
    check('missing-hosted-edge-consumption', events.filter(event => event !== hostedEdge));
    check('wrong-hosted-edge-order', events.map(event => event === hostedEdge
      ? {...event, detail: event.detail ^ 1} : event));
    check('duplicate-hosted-edge-consumption', events.flatMap(event => event === hostedEdge
      ? [event, {...event}] : [event]));
  }
  if (touchDevice) check('missing-touch-event-coordinates', events.filter(event => event !== touchDevice));
  check('missing-physical-read', events.filter(event => event !== read), !!read);
  check('duplicate-physical-read', events.flatMap(event => event === read ? [event, {...event}] : [event]), !!read);
  check('wrong-physical-buttons', events.map(event => event === read ? {...event, value: event.value ^ 1} : event), !!read);
  check('missing-analog-read', events.filter(event => event.kind !== 'read-touch-xy'));
  check('wrong-saved-analog-input', events.map(event => event === analog ? {...event, value: event.value ^ 1} : event), !!analog);
  check('wrong-hosted-read', events.map(event => event === hostedRead ? {...event, value: event.value ^ 1} : event), !!hostedRead);
  check('duplicate-sample', events.flatMap(event => event === sample ? [event, {...event}] : [event]));
  check('wrong-send-input', events.map(event => event.kind === 'send' || event.kind === 'schedule'
    ? {...event, value: event.value ^ 1} : event));
  check('no-actual-send', events.map(event => event.kind === 'send' ? {...event, kind: 'schedule'} : event));
  check('duplicate-initial-apply', events.flatMap(event => event === fresh ? [event, {...event}] : [event]));
  check('missing-simulation-completions', events.filter(event => event.kind !== 'simulate'));
  check('confirmation-before-application', events.flatMap(event => event === sample
    ? [event, {...confirmation, us: sample.us}] : event === confirmation ? [] : [event]));
  check('missing-presentation-tail', events.filter(event => event.kind !== 'present' || event === firstPresent),
    firstPresent.frame < audit.length - 1);
  check('wrong-saved-replay-input', events.map(event => event === replay
    ? {...event, value: event.value ^ 1} : event), !!replay);
  check('impossible-restore-depth', events.map(event => event.kind === 'restore'
    ? {...event, value: 999} : event), events.some(event => event.kind === 'restore'));
  check('missing-correction-completions', events.filter(event => event.kind !== 'corrected'),
    events.some(event => event.kind === 'corrected'));
  check('nonzero-configured-delay', events, true, {...configuration, inputDelayFrames: 2});
  check('wrong-configured-prediction-window', events, true, {...configuration, predictionLimitFrames: 99});
  return controls;
}

export async function verifyDirectory(directory, {prefix = false} = {}) {
  const dir = resolve(directory), identities = [];
  const read = async path => {
    const bytes = await readFile(path);
    identities.push({path: String(path), bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')});
    return bytes.toString('utf8');
  };
  const lines = async path => (await read(path)).trim().split('\n').filter(Boolean).map(JSON.parse);
  const report = JSON.parse(await read(dir + (prefix ? '/progress.json' : '/formal-smoke.json')));
  await read(new URL(import.meta.url));
  if (!prefix) {
    assert.equal(report.passed, true, 'Formal full flow passed');
    assert.equal(report.terminal.completed, true, 'Final resulting world is a clear');
    assert.equal(report.errors.length, 0, 'No browser/game errors');
  }
  const seatCount = report.current?.length || (await readdir(dir)).filter(name => /^audit-\d+\.ndjson$/.test(name)).length;
  assert([2, 3].includes(seatCount), 'Two or three player evidence');
  const audit = await Promise.all(Array.from({length: seatCount}, (_, seat) => lines(dir + `/audit-${seat}.ndjson`)));
  const frames = prefix ? Math.min(...audit.map(rows => rows.length)) : report.terminal.frames;
  if (prefix) audit.forEach((rows, seat) => { audit[seat] = rows.slice(0, frames); });
  compareAudit(...audit, frames);
  // Keep the entire event history: a prefix's samples can precede a replay and
  // its confirmation/presentation can arrive later. Only statistics use 0..N.
  const timing = await Promise.all(Array.from({length: seatCount}, (_, seat) => lines(dir + `/timing-${seat}.ndjson`)));
  const seats = timing.map((chunks, seat) => {
    const options = report.current?.[seat]?.runtime?.options;
    assert(options, 'Missing recorded runtime configuration for P' + (seat + 1));
    assert.equal(options.netplayMode, 'lan', 'Recorded production room mode');
    return verifyTiming(chunks, audit[seat], seat, {
      source: (prefix ? 'progress.json' : 'formal-smoke.json') + `:current[${seat}].runtime.options`,
      inputDelayFrames: options.netplayInputDelay, predictionLimitFrames: options.netplayPredictionLimit,
      adonisMode: options.netplayAdonisMode, localSeat: options.netplayPlayer, players: options.netplayPlayerCount,
    });
  });
  // Each negative control changes captured evidence, never the game or the
  // acceptance result. A comparator that misses any corruption must fail.
  assert(frames >= 3, 'Enough real frames for evidence negative controls');
  const controls = [];
  for (let seat = 1; seat < seatCount; ++seat) {
    for (const mutation of ['state', 'host', 'missing-frame', 'wrong-input']) {
      const changed = structuredClone(audit);
      if (mutation === 'state') changed[seat][1].full += '0';
      if (mutation === 'host') changed[seat][1].host += '0';
      if (mutation === 'missing-frame') changed[seat].splice(1, 1);
      if (mutation === 'wrong-input') changed[seat][1].inputs[seat] ^= 1;
      assert.throws(() => compareAudit(...changed, frames), undefined, mutation + ' must fail for seat ' + seat);
      controls.push({seat, mutation, rejected: true});
    }
  }
  const missingHost = structuredClone(audit);
  for (const rows of missingHost) for (const row of rows) delete row.host;
  assert.throws(() => compareAudit(...missingHost, frames), undefined, 'Shared missing host domain must fail');
  controls.push({mutation: 'missing-host-on-all-seats', rejected: true});
  const timingControls = timing.map((chunks, seat) => ({seat,
    controls: timingNegativeControls(chunks, audit[seat], seat, seats[seat].configuration)}));
  const unexercised = timingControls.flatMap(({seat, controls}) => controls
    .filter(control => !control.rejected).map(control => ({seat, mutation: control.mutation})));
  return {passed: unexercised.length === 0,
    scope: prefix ? 'Captured prefix only; not a full-flow pass' : 'Full production-room evidence',
    historyAndEventConsistencyPassed: true, latencyAcceptance: 'NOT_ASSESSED',
    frames, frameRange: {first: 0, endExclusive: frames}, evidenceInputs: identities,
    seats, negativeControls: controls, timingNegativeControls: timingControls,
    unexercisedTimingNegativeControls: unexercised};
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  const dir = process.argv[2], prefix = process.argv.includes('--prefix');
  const report = await verifyDirectory(dir, {prefix});
  await writeFile(resolve(dir, prefix ? 'prefix-evidence.json' : 'verified-evidence.json'), JSON.stringify(report, null, 2) + '\n');
  console.log(JSON.stringify(report));
  if (!report.passed) process.exitCode = 1;
}
