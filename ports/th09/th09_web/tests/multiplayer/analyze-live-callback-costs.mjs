// Read-only analysis of an existing live-cost report. Does not launch a game.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import { fileURLToPath } from 'node:url';

const [input, output] = process.argv.slice(2);
assert(input && output, 'Usage: node analyze-live-callback-costs.mjs report.json new-analysis.json');
assert.notEqual(path.resolve(input), path.resolve(output), 'Never overwrite the raw report');
assert(!fs.existsSync(output), 'Use a new output name; retained evidence is immutable');
const bytes = fs.readFileSync(input);
const report = JSON.parse(bytes);
const sha = data => crypto.createHash('sha256').update(data).digest('hex');
const sum = values => values.reduce((total, value) => total + value, 0);
function stats(values) {
  const sorted = [...values].sort((a, b) => a - b);
  const percentile = p => sorted[Math.max(0, Math.ceil(sorted.length * p) - 1)] ?? null;
  return { count: sorted.length, total: sum(sorted), mean: sorted.length ? sum(sorted) / sorted.length : null,
    min: sorted[0] ?? null, p50: percentile(.5), p95: percentile(.95), p99: percentile(.99), max: sorted.at(-1) ?? null };
}
const formalStart = 180, formalEnd = report.parameters.limit;
function analyzeSide(result, side) {
  const probe = result.liveCallbackCosts, schema = probe.schema;
  assert(probe.valid && probe.attached && probe.errors.length === 0, 'Live probe validity');
  assert(!Object.values(probe.overflow).some(Boolean), 'No live probe overflow');
  assert.equal(schema.version, 1);
  assert.equal(schema.header.length, 6);
  const phaseStart = 6, counterStart = phaseStart + schema.phases.length * 3;
  const gaugeStart = counterStart + schema.counters.length;
  const width = gaugeStart + schema.gauges.length;
  const monotonicEnd = gaugeStart;
  const epoch = probe.frames[0].values[2];
  const validate = values => {
    assert.equal(values.length, width);
    assert.equal(values[0], 1); assert.equal(values[1], 1); assert.equal(values[2], epoch);
    assert.equal(values[3], schema.phases.length); assert.equal(values[4], schema.counters.length);
    assert.equal(values[5], schema.gauges.length);
    for (const v of values) assert(Number.isFinite(v), 'Finite snapshot');
    for (let i = phaseStart; i < monotonicEnd; ++i) assert(values[i] >= 0, 'Nonnegative cumulative value');
    assert.equal(values[counterStart + schema.counters.indexOf('timingErrors')], 0);
  };
  const decodeDelta = (previous, current) => {
    validate(previous.values); validate(current.values);
    const delta = current.values.map((v, i) => v - previous.values[i]);
    for (let i = phaseStart; i < monotonicEnd; ++i) assert(delta[i] >= -1e-6, 'Monotonic snapshot');
    const phases = Object.fromEntries(schema.phases.map((name, i) => [name, {
      calls: delta[phaseStart + i * 3], inclusiveMs: delta[phaseStart + i * 3 + 1],
      exclusiveMs: delta[phaseStart + i * 3 + 2] }]));
    const counters = Object.fromEntries(schema.counters.map((name, i) => [name, delta[counterStart + i]]));
    const gauges = Object.fromEntries(schema.gauges.map((name, i) => [name, current.values[gaugeStart + i]]));
    const exclusiveMs = sum(Object.values(phases).map(v => v.exclusiveMs));
    return { phases, counters, gauges, exclusiveMs,
      nativeOutsideFrameCallbackMs: exclusiveMs - phases.frameCallback.inclusiveMs };
  };
  const markIntervals = [];
  for (let i = 1; i < probe.marks.length; ++i) {
    const a = probe.marks[i - 1], b = probe.marks[i];
    markIntervals.push({ from: a.label, to: b.label, fromObservedAt: a.observedAt,
      toObservedAt: b.observedAt, observedElapsedMs: b.observedAt - a.observedAt, ...decodeDelta(a, b) });
  }
  const lastMark = probe.marks.at(-1), first = probe.frames[0];
  markIntervals.push({ from: lastMark.label, to: 'first-recorded-onGameFrame',
    fromObservedAt: lastMark.observedAt, toObservedAt: first.observedAt,
    observedElapsedMs: first.observedAt - lastMark.observedAt,
    firstCallbackWorkMs: first.callbackWorkMs, firstSampleIndex: first.sampleIndex,
    note: 'This interval includes startup and every completed unobserved callback; do not call it one callback.',
    ...decodeDelta(lastMark, first) });
  const intervals = probe.frames.slice(1).map((current, i) => {
    const previous = probe.frames[i], a = result.measure.samples[previous.sampleIndex],
      b = result.measure.samples[current.sampleIndex], da = result.measure.frameDetails[previous.sampleIndex],
      db = result.measure.frameDetails[current.sampleIndex], d = decodeDelta(previous, current);
    assert.equal(b[3], current.frame); assert.equal(db[0], current.frame);
    assert.equal(a[3], previous.frame); assert.equal(da[0], previous.frame);
    const worldStateSaves = db[1] - da[1], correctionDelta = b[5] - a[5], replayDelta = b[6] - a[6];
    const oneCallback = d.phases.frameCallback.calls === 1;
    const ordinary = oneCallback && current.frame === previous.frame + 1 &&
      d.counters.forwardTicks === 1 && d.counters.replayTicks === 0 &&
      d.counters.speculativeTicks === 0 && d.counters.restoredCheckpoints === 0 &&
      d.phases.checkpoint.calls === 0 && d.phases.restore.calls === 0 &&
      worldStateSaves === 0 && correctionDelta === 0 && replayDelta === 0;
    let category;
    if (!oneCallback) category = 'multiple-or-zero-callbacks';
    else if (d.counters.replayTicks || d.counters.restoredCheckpoints || correctionDelta || replayDelta)
      category = 'correction-or-replay';
    else if (worldStateSaves || d.counters.speculativeTicks || d.phases.checkpoint.calls)
      category = 'checkpoint-without-correction';
    else if (ordinary) category = 'ordinary-single-forward';
    else if (d.counters.forwardTicks > 1) category = 'multiple-forwards-without-checkpoint';
    else if (!d.counters.forwardTicks) category = 'no-forward';
    else category = 'other-single-callback';
    return { sampleIndex: current.sampleIndex, previousSampleIndex: previous.sampleIndex,
      frame: current.frame, previousFrame: previous.frame, confirmed: current.confirmed,
      measuredAt: current.measuredAt, observedAt: current.observedAt,
      observationGapMs: current.observedAt - previous.observedAt,
      callbackWorkMs: current.callbackWorkMs, rafTimestamp: b[13], rafGapMs: b[13] - a[13],
      worldStateSaves, correctionDelta, replayDelta, category, ordinary,
      formal: current.frame >= formalStart && current.frame < formalEnd,
      lastTickUpdateMs: db[2], lastTickDrawMs: db[3],
      callbackNativeMinusReportedMs: d.phases.frameCallback.inclusiveMs - current.callbackWorkMs,
      ...d };
  });
  const summarize = rows => ({
    intervals: rows.length, sampleIndices: rows.map(v => v.sampleIndex),
    callbackWorkMs: stats(rows.map(v => v.callbackWorkMs)),
    frameCallbackNativeMs: stats(rows.map(v => v.phases.frameCallback.inclusiveMs)),
    nativeOutsideFrameCallbackMs: stats(rows.map(v => v.nativeOutsideFrameCallbackMs)),
    callbackNativeMinusReportedMs: stats(rows.map(v => v.callbackNativeMinusReportedMs)),
    worldStateSaves: sum(rows.map(v => v.worldStateSaves)),
    phases: Object.fromEntries(schema.phases.map(name => [name, {
      calls: sum(rows.map(v => v.phases[name].calls)),
      inclusiveMs: stats(rows.map(v => v.phases[name].inclusiveMs)),
      exclusiveMs: stats(rows.map(v => v.phases[name].exclusiveMs)) }])),
    counters: Object.fromEntries(schema.counters.map(name => [name, sum(rows.map(v => v.counters[name]))]))
  });
  const formal = intervals.filter(v => v.formal), ordinary = formal.filter(v => v.ordinary);
  const categories = Object.fromEntries([...new Set(formal.map(v => v.category))].map(name =>
    [name, summarize(formal.filter(v => v.category === name))]));
  assert.equal(sum(Object.values(categories).map(v => v.intervals)), formal.length, 'All formal intervals retained');
  const chunks = [...new Set(ordinary.map(v => v.counters.pcmChunks))].sort((a, b) => a - b)
    .map(n => ({ pcmChunks: n, summary: summarize(ordinary.filter(v => v.counters.pcmChunks === n)) }));
  const maxRafIndex = intervals.reduce((best, v, i) => v.rafGapMs > intervals[best].rafGapMs ? i : best, 0);
  const long = formal.filter(v => v.callbackWorkMs > 50);
  const memory = { heapBytes: result.memoryBytes, renderer: result.renderer };
  const audioOutput = { nonzeroBlocks: result.audioOutput.nonzeroBlocks,
    sampledValues: result.audioOutput.sampledValues, maxAmplitude: result.audioOutput.maxAmplitude,
    overflow: result.audioOutput.overflow };
  return {
    side, configuredCpuRate: report.parameters.cpuThrottle.ratesBySide[side - 1],
    integrity: { valid: probe.valid, errors: probe.errors, overflow: probe.overflow,
      firstSampleIndex: first.sampleIndex, lastSampleIndex: probe.frames.at(-1).sampleIndex,
      observedFrames: probe.frames.length, intervalCount: intervals.length, timingErrors: 0,
      formalIntervalCount: formal.length, formalMatchesRawCallbacks:
        result.measure.samples.filter(v => v[3] >= formalStart && v[3] < formalEnd).length === formal.length },
    memory, audioOutput, startup: markIntervals, allObservedIntervals: summarize(intervals),
    formal: summarize(formal), categories, ordinaryPcmChunkGroups: chunks,
    longOrdinaryOver25SampleIndices: ordinary.filter(v => v.callbackWorkMs > 25).map(v => v.sampleIndex),
    formalCallbacksOver50: long,
    earlyAdvancingCallbacks: intervals.filter(v => v.frame > v.previousFrame).slice(0, 4),
    largestRafGapNeighborhood: intervals.slice(Math.max(0, maxRafIndex - 2), maxRafIndex + 3),
    largestOutsideCallbackIntervals: [...formal].sort((a, b) =>
      b.nativeOutsideFrameCallbackMs - a.nativeOutsideFrameCallbackMs).slice(0, 5),
    postLastObservationNativeDelta: decodeDelta(probe.frames.at(-1), { values: probe.finalValues }),
    intervalFields: [
      'sampleIndex', 'previousSampleIndex', 'frame', 'previousFrame', 'category',
      'callbackWorkMs', 'frameCallbackNativeMs', 'worldStateSaves', 'forwardTicks', 'replayTicks',
      'speculativeTicks', 'restoredCheckpoints', 'pcmChunks', 'pumpRefillCalls', 'gpuQueuedBatches',
      'publishedSfx', 'nativeOutsideFrameCallbackMs', 'rafGapMs', 'observationGapMs',
      ...schema.phases.map(v => v + '.exclusiveMs')
    ],
    intervals: intervals.map(v => [
      v.sampleIndex, v.previousSampleIndex, v.frame, v.previousFrame, v.category,
      v.callbackWorkMs, v.phases.frameCallback.inclusiveMs, v.worldStateSaves,
      v.counters.forwardTicks, v.counters.replayTicks, v.counters.speculativeTicks,
      v.counters.restoredCheckpoints, v.counters.pcmChunks, v.counters.pumpRefillCalls,
      v.counters.gpuQueuedBatches, v.counters.publishedSfx, v.nativeOutsideFrameCallbackMs,
      v.rafGapMs, v.observationGapMs, ...schema.phases.map(name => v.phases[name].exclusiveMs)
    ])
  };
}
function analyzeHost(host) {
  const byPid = new Map();
  for (const sample of host.processSamples) for (const [pid, name, seconds] of sample.rows) {
    const key = pid + ':' + name;
    if (!byPid.has(key)) byPid.set(key, { pid, name, readings: [] });
    byPid.get(key).readings.push({ atMs: sample.receivedAtMs, cpuSeconds: seconds });
  }
  const processes = [...byPid.values()].map(p => {
    const first = p.readings[0], last = p.readings.at(-1);
    const discontinuity = p.readings.some((v, i) => i && v.cpuSeconds < p.readings[i - 1].cpuSeconds);
    return { pid: p.pid, name: p.name, observations: p.readings.length, first, last,
      discontinuity, observedCpuSecondsDelta: discontinuity ? null : last.cpuSeconds - first.cpuSeconds };
  });
  const cpu = host.cpuSamples.filter(v => Number.isFinite(v.busyFraction));
  return { valid: host.valid, complete: host.complete, errors: host.errors, overflow: host.overflow,
    cpuBusyFraction: stats(cpu.map(v => v.busyFraction)),
    weightedBusyFraction: 1 - sum(cpu.map(v => v.deltaIdleMs)) / sum(cpu.map(v => v.deltaTotalMs)),
    processObservationWindow: 'Whole host record; positive deltas measure only first-to-last sightings. Exited tails and short processes can be missed.',
    processes: processes.filter(v => v.observedCpuSecondsDelta > 0 || /wasm-opt|clang|ninja|lld/.test(v.name)),
    marks: host.marks, limitations: host.limitations };
}
const evidence = {
  version: 1, input: path.resolve(input), inputSha256: sha(bytes),
  analyzer: fileURLToPath(import.meta.url), analyzerSha256: sha(fs.readFileSync(fileURLToPath(import.meta.url))),
  passed: report.passed, identity: report.identity, parameters: report.parameters,
  formalWindow: { startInclusive: formalStart, endExclusive: formalEnd,
    selection: 'Every reported callback whose current simulation frame is in the window. Adjacent raw observations supply deltas; no timing outliers are excluded.' },
  units: 'Elapsed milliseconds, not process CPU time; phase inclusive times overlap.',
  definitions: {
    ordinary: 'Exactly one completed frameCallback, one forward tick, frame advances by one, zero checkpoint calls, WorldState Save increment, speculative ticks, restore calls, restored checkpoints, replay ticks, reported corrections and reported replay increments.',
    zeroSaveBoundary: 'The live tick seals every created WorldState and clears the graphics checkpoint before returning. Seal unbinds first-write capture; an earlier speculative checkpoint does not remain bound in an ordinary zero-Save tick.',
    phases: 'Only exclusive phase values can be summed. Inclusive values are retained for their own scope, never added to nested child values.',
    outsideCallback: 'Sum of all phase-exclusive deltas minus frameCallback inclusive delta. Native networkPump also runs from JS between callbacks, so an observation interval can contain additional native work even with exactly one callback.',
    callbackCost: 'callbackWorkMs is the existing exported C++ callback duration. The native frameCallback scope includes a slightly wider diagnostic boundary; their difference is retained.',
    startup: 'before-open and after-open are not CPU throttled. The configured-rate mark precedes match start and the first recorded onGameFrame. First-recorded intervals can contain many completed callbacks.',
    audio: 'AudioMix includes all active BGM/SFX decoding, resampling and mixing; these coarse counters do not separate those sources. Last-pump gauges cannot describe earlier pumps in the interval.',
    selectionLimits: 'No V8 CPU profile was collected in this run. These are diagnostic build counters, not release acceptance, physical input latency, acoustic continuity or GPU execution durations.'
  },
  schema: report.results[0].liveCallbackCosts.schema,
  sides: report.results.map((result, i) => analyzeSide(result, i + 1)),
  host: analyzeHost(report.hostLoad)
};
fs.writeFileSync(output, JSON.stringify(evidence, null, 2) + '\n', { flag: 'wx' });
console.log(JSON.stringify({ output: path.resolve(output), inputSha256: evidence.inputSha256,
  analyzerSha256: evidence.analyzerSha256, passed: evidence.passed,
  sides: evidence.sides.map(v => ({ side: v.side, formalCallbacks: v.formal.intervals,
    ordinary: v.categories['ordinary-single-forward']?.intervals ?? 0,
    over50: v.formalCallbacksOver50.map(x => ({ sampleIndex: x.sampleIndex, frame: x.frame, ordinary: x.ordinary,
      callbackMs: x.callbackWorkMs, pcmChunks: x.counters.pcmChunks, audioMixMs: x.phases.audioMix.exclusiveMs })) })),
  hostValid: evidence.host.valid }, null, 2));
