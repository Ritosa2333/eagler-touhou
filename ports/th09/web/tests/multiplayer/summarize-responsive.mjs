// Summarize existing reports only. This script never builds or launches a browser.
// Run from the repository root: node web/tests/multiplayer/summarize-responsive.mjs
import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const workspace = fileURLToPath(new URL('../../../', import.meta.url));
const reportDirectory = 'web/artifacts/multiplayer-tests/';
const outputPath = 'docs/multiplayer/rollback-responsive-evidence.json';
const manifest = [];
const requestedExtensions = new Set(process.argv.slice(2));
for (const flag of requestedExtensions) assert(['--include-progress', '--include-host-abba', '--include-paired', '--include-paired-controls', '--include-paired-active', '--include-network-burst'].includes(flag), 'Unknown evidence extension: ' + flag);
const sha256 = value => createHash('sha256').update(value).digest('hex');
const jsonHash = value => sha256(JSON.stringify(value));
const sum = values => values.reduce((total, value) => total + value, 0);
const hashPattern = /^[a-f0-9]{64}$/;
const build = {
  J: 'b525102519c98ac5e3d4cd3bbcd840fa6e77eb1f0bf9196e3ba2cb2a8e94547b',
  L: '08cfc57ef71686ba53338b6e853f23030928f40a999863b31d94ce0b592621f1',
  M: '535c358231dc21ada7c2f9df910c1fb0d505cff41612bc79478205afee3993cf'
};

function array(value, name, length) {
  assert(Array.isArray(value), name + ': expected an array');
  if (length !== undefined) assert.equal(value.length, length, name + ': length');
  return value;
}
function finite(value, name) {
  assert(Number.isFinite(value), name + ': expected a finite number');
  return value;
}
function same(values, name) {
  assert(values.length > 0, name + ': no values');
  for (const value of values.slice(1)) assert.deepEqual(value, values[0], name);
  return values[0];
}
function close(actual, expected, name) {
  finite(actual, name);
  finite(expected, name);
  assert(Math.abs(actual - expected) <= 1e-5, name + ': numeric mismatch');
}
function report(name, expected = 'pass') {
  const path = name.startsWith('web/') ? name : reportDirectory + name;
  const bytes = readFileSync(resolve(workspace, path)); // Missing reports are fatal.
  const data = JSON.parse(bytes.toString('utf8').replace(/^\uFEFF/, ''));
  assert(data && typeof data === 'object' && !Array.isArray(data), path + ': report object');
  if (expected === 'pass') {
    assert.equal(data.passed, true, path + ': passed');
    if (data.errors !== undefined) assert.deepEqual(data.errors, [], path + ': errors');
  } else if (expected === 'build') {
    assert(hashPattern.test(data.sha256), path + ': frozen build SHA-256');
    assert(Number.isSafeInteger(data.bytes) && data.bytes > 0, path + ': frozen build size');
    assert(data.sources && Object.keys(data.sources).length > 0, path + ': source manifest');
  } else {
    assert.equal(expected, 'coverage-failure', path + ': unknown expectation');
    assert.equal(typeof data.error, 'string', path + ': expected retained failure');
    assert(data.error.includes('Sustained requested-depth recovery not exercised'),
      path + ': failure has changed; review before summarizing');
  }
  manifest.push({ path, sha256: sha256(bytes), bytes: bytes.length, expected });
  return data;
}
function distribution(values) {
  values.forEach(value => finite(value, 'distribution sample'));
  const sorted = [...values].sort((a, b) => a - b);
  const count = sorted.length;
  const total = sum(sorted);
  const at = q => count ? sorted[Math.floor(count * q)] : null;
  return {
    count, total, mean: count ? total / count : null,
    p50: at(.5), p95: at(.95), p99: at(.99), max: count ? sorted[count - 1] : null,
    over25ms: values.filter(x => x > 25).length,
    over50ms: values.filter(x => x > 50).length,
    over100ms: values.filter(x => x > 100).length,
    over250ms: values.filter(x => x > 250).length
  };
}

const sdlNames = [
  'calls', 'batches', 'uploadBytes', 'readBytes', 'frames', 'presentations',
  'bufferReplacements', 'bufferSubUpdates', 'vertexUploadBytes', 'directBytes',
  'copiedBytes', 'layoutSetups', 'textureBinds', 'framebufferBinds',
  'programCompiles', 'genericBatches', 'resamples'
];
function sdlCounters(value) {
  const result = Array.isArray(value)
    ? Object.fromEntries(array(value, 'SDL counters', sdlNames.length).map((v, i) => [sdlNames[i], v]))
    : value;
  assert(result && typeof result === 'object', 'SDL counters missing');
  for (const name of sdlNames) {
    finite(result[name], 'SDL.' + name);
    assert(result[name] >= 0, 'negative SDL counter');
  }
  return result;
}
function audioEvidence(peer) {
  const output = peer.audioOutput;
  const blocks = array(output.blocks, 'audio output blocks');
  assert(blocks.length > 0, 'no sampled audio output');
  for (const block of blocks) array(block, 'audio block', 5).forEach(v => finite(v, 'audio block'));
  const nonzero = blocks.filter(block => block[4] > 1e-6).length;
  assert.equal(nonzero, output.nonzeroBlocks, 'audio nonzero block count');
  assert(nonzero > 0, 'audio output is entirely zero');
  assert.equal(output.overflow, false, 'audio probe overflow');
  const gaps = blocks.slice(1).map((block, i) => block[0] - blocks[i][0]);
  assert(gaps.every(value => value >= 0), 'audio observation time went backwards');
  return {
    context: peer.audio,
    blocks: blocks.length, nonzeroBlocks: nonzero, zeroBlocks: blocks.length - nonzero,
    sampledValues: output.sampledValues, maxAmplitude: output.maxAmplitude,
    overflow: output.overflow,
    generatedBlockSeconds: sum(blocks.map(block => block[3])),
    blockDurationSeconds: [...new Set(blocks.map(block => block[3]))],
    observedCallbackGapMs: distribution(gaps),
    scope: 'Sampled existing SDL ScriptProcessor output. These counts do not establish acoustic continuity or decoder CPU cost. Audio and game elapsed timestamps have different origins.'
  };
}
function makeRows(peer) {
  const samples = array(peer.measure.samples, 'callback samples');
  const details = array(peer.measure.frameDetails, 'frame details', samples.length);
  assert(samples.length > 1, 'insufficient callback samples');
  let frontier = samples[0][3];
  const rows = [];
  for (let i = 0; i < samples.length; ++i) {
    const sample = array(samples[i], 'callback sample', 14);
    sample.forEach(value => finite(value, 'callback sample'));
    assert(details[i].length >= 2, 'missing WorldState capture counter');
    assert.equal(details[i][0], sample[3], 'frameDetails/sample alignment');
    finite(details[i][1], 'WorldState capture counter');
    frontier = Math.max(frontier, sample[3]);
    if (!i) continue;
    const before = samples[i - 1];
    const row = {
      i, frame: sample[3], frontier, beforeFrame: before[3],
      frameDelta: sample[3] - before[3],
      corrections: sample[5] - before[5],
      resimulated: sample[6] - before[6],
      inputCaptures: sample[7] - before[7],
      saves: details[i][1] - details[i - 1][1],
      submissions: sample[11] - before[11],
      workMs: sample[2], ok: sample[1], phase: sample[12],
      observedElapsedMs: sample[0], rafTimestampMs: sample[13],
      confirmedPublishedBoundary: sample[4]
    };
    for (const key of ['corrections', 'resimulated', 'inputCaptures', 'saves', 'submissions']) {
      assert(row[key] >= 0, key + ': counter decreased');
    }
    row.category = row.corrections || row.resimulated || row.frameDelta < 0
      ? 'correction_or_replay'
      : row.frameDelta > 0 ? (row.saves ? 'forward_with_save' : 'forward_no_save')
        : 'no_forward';
    rows.push(row);
  }
  return rows;
}
function aggregate(rows) {
  return {
    callbackWorkMs: distribution(rows.map(row => row.workMs)),
    frameAdvance: sum(rows.map(row => row.frameDelta)),
    corrections: sum(rows.map(row => row.corrections)),
    resimulated: sum(rows.map(row => row.resimulated)),
    saves: sum(rows.map(row => row.saves)),
    inputCaptures: sum(rows.map(row => row.inputCaptures)),
    submissions: sum(rows.map(row => row.submissions)),
    waitCallbacks: rows.filter(row => row.ok === 2).length
  };
}
function ordinary(row, saves = 0) {
  return row.frameDelta === 1 && row.corrections === 0 && row.resimulated === 0 &&
    row.saves === saves && row.submissions === 1 && row.phase === 1;
}
function endpoint(peer, index) {
  if (index < 0 || index >= peer.measure.samples.length) return null;
  const s = peer.measure.samples[index];
  const observedAbsoluteMs = peer.measure.started + s[0];
  return {
    sampleIndex: index, frame: s[3], ok: s[1],
    reportedCallbackWorkMs: s[2], correctionsTotal: s[5], resimulatedTotal: s[6],
    inputCapturesTotal: s[7], savesTotal: peer.measure.frameDetails[index][1],
    submissionCounter: s[11], phase: s[12],
    rafTimestampMs: s[13],
    legacyObservedElapsedMs: s[0], legacyObservedAbsoluteMs: observedAbsoluteMs,
    rafToLegacyObservationMs: observedAbsoluteMs - s[13]
  };
}
function extremeIntervals(peer, rows, selected) {
  const segments = [];
  let previous = selected[0];
  for (const current of selected.slice(1)) {
    const before = peer.measure.samples[previous.i];
    const after = peer.measure.samples[current.i];
    if (after[11] === before[11]) continue;
    const work = rows.slice(previous.i, current.i); // rows[i - 1] represents sample i.
    segments.push({
      from: endpoint(peer, previous.i), to: endpoint(peer, current.i),
      rafGapMs: after[13] - before[13], legacyObservationGapMs: after[0] - before[0],
      intervalWork: aggregate(work)
    });
    previous = current;
  }
  assert(segments.length > 0, 'no new renderer submissions in selected interval');
  const maxWork = [...selected].sort((a, b) => b.workMs - a.workMs)[0];
  return {
    largestNewSubmissionRafIntervals: [...segments].sort((a, b) => b.rafGapMs - a.rafGapMs).slice(0, 3),
    largestNewSubmissionLegacyObservationIntervals: [...segments]
      .sort((a, b) => b.legacyObservationGapMs - a.legacyObservationGapMs).slice(0, 3),
    largestReportedCallback: {
      before: endpoint(peer, maxWork.i - 1), current: endpoint(peer, maxWork.i),
      after: endpoint(peer, maxWork.i + 1), category: maxWork.category
    }
  };
}
function frontierBands(peer, rows) {
  const cuts = [180, 450, 720, 990, 1260, 1530, 1800];
  return cuts.slice(1).map((end, i) => {
    const start = cuts[i];
    const a = rows.findIndex(row => row.frontier >= start);
    const b = rows.findIndex(row => row.frontier >= end);
    assert(a >= 0 && b > a, 'missing logical frontier band ' + start + '..' + end);
    const elapsedMs = rows[b].observedElapsedMs - rows[a].observedElapsedMs;
    assert(elapsedMs > 0, 'frontier band elapsed time');
    return {
      requestedFrom: start, requestedTo: end,
      from: endpoint(peer, rows[a].i), to: endpoint(peer, rows[b].i),
      actualFrontierAdvance: rows[b].frontier - rows[a].frontier, elapsedMs,
      logicHz: (rows[b].frontier - rows[a].frontier) * 1000 / elapsedMs,
      work: aggregate(rows.slice(a + 1, b + 1))
    };
  });
}
function peerEvidence(peer, rawSummary) {
  assert.equal(peer.route, 'relay', 'expected real relay route');
  assert.equal(peer.audio.state, 'running', 'audio context not running');
  array(peer.measure.closed, 'connection closes');
  assert.deepEqual(peer.measure.closed, [], 'connection closed during run');
  array(peer.lifecycle, 'lifecycle');
  const rows = makeRows(peer);
  const selected = rows.filter(row => row.frame >= 180 && row.frame < 1800);
  assert.equal(selected.length, rawSummary.callbacks, 'selected callback count');
  const total = aggregate(selected);
  close(total.callbackWorkMs.total, rawSummary.callbackWorkMs.total, 'callback work total');
  close(total.callbackWorkMs.p99, rawSummary.callbackWorkMs.p99, 'callback work p99');
  const categories = Object.fromEntries([
    'no_forward', 'forward_no_save', 'forward_with_save', 'correction_or_replay'
  ].map(name => [name, aggregate(selected.filter(row => row.category === name))]));
  assert.equal(sum(Object.values(categories).map(group => group.callbackWorkMs.count)),
    selected.length, 'callback groups must partition all selected samples');
  const normalNoSave = selected.filter(row => ordinary(row));
  const normalWithSave = selected.filter(row => ordinary(row, 1));
  const ordinaryByFrame = new Map(normalNoSave.map(row => [row.frame, row]));
  assert.equal(ordinaryByFrame.size, normalNoSave.length, 'duplicate ordinary frontier frame');
  const sdl = sdlCounters(peer.sdl);
  const startSdl = sdlCounters(peer.measure.sdlStart);
  const measuredSdl = sdlCounters(peer.sdlMeasured);
  for (const name of sdlNames) assert.equal(sdl[name] - startSdl[name], measuredSdl[name], 'SDL measured ' + name);
  return {
    ordinaryByFrame,
    evidence: {
      finalStats: peer.stats, finalHash: peer.hash, finalAudit: peer.audited,
      route: peer.route, renderer: peer.renderer, memoryBytes: peer.memoryBytes,
      lifecycle: peer.lifecycle, notices: peer.measure.notices, closed: peer.measure.closed,
      measurement: {
        rawCallbacks: rows.length + 1, selectedCallbacks: selected.length,
        firstSelected: endpoint(peer, selected[0].i), lastSelected: endpoint(peer, selected.at(-1).i),
        total, callbackWorkFractionOfObservedElapsed: total.callbackWorkMs.total / rawSummary.elapsedMs,
        categories,
        strictSingleFrameNoSave: aggregate(normalNoSave),
        strictSingleFrameWithSave: aggregate(normalWithSave),
        normalNoSaveAtOrBehindReportedBoundary: selected.filter(row =>
          row.category === 'forward_no_save' && row.confirmedPublishedBoundary >= row.frame).length,
        normalSaveAheadOfReportedBoundary: selected.filter(row =>
          row.category === 'forward_with_save' && row.confirmedPublishedBoundary < row.frame).length,
        logicalFrontierBands: frontierBands(peer, rows),
        extremes: extremeIntervals(peer, rows, selected)
      },
      sdlTotal: sdl, sdlAtFirst180Crossing: startSdl, sdlMeasured: measuredSdl,
      audio: audioEvidence(peer),
      loadProbe: { samples: peer.measure.loads.length, available: peer.measure.loads.length > 0 },
      impairment: {
        matched: peer.impairment.matched, sent: peer.impairment.sent,
        dropped: peer.impairment.dropped, pending: peer.impairment.pending,
        peakPending: peer.impairment.peakPending, burstPackets: peer.impairment.burstPackets,
        bursts: peer.impairment.bursts, lanes: peer.impairment.lanes
      }
    }
  };
}

const parameters = {
  limit: 1800, delay: 39, jitter: 5, slow: 4, dropEvery: 0, release: true,
  profile: false, fallback: true, inputMode: 'tape', audioOutput: true,
  responsiveness: false, burstAt: 0, burstFor: 0, burstDelay: 0
};
const runs = ['a1', 'b1', 'b2', 'a2'].map(id => {
  const raw = report('rollback4x-responsive-release-' + id + '-report.json');
  const candidate = id[0] === 'a' ? 'J' : 'M';
  assert.equal(raw.identity.wasm, build[candidate], id + ': frozen WASM identity');
  assert.deepEqual(raw.parameters, parameters, id + ': measurement parameters');
  assert.equal(raw.confirmedChecks, 15, id + ': confirmed checks');
  assert.equal(raw.finalComparable, false, id + ': final comparison scope changed');
  array(raw.results, id + ': peers', 2);
  array(raw.summary, id + ': dual-peer summary', 2);
  const peers = raw.results.map((peer, index) => peerEvidence(peer, raw.summary[index]));
  return { id, candidate, raw, peers };
});
for (const field of ['harness', 'relay', 'shell', 'audioProbe']) {
  same(runs.map(run => run.raw.identity[field]), 'cross-run ' + field);
}
same(runs.map(run => run.raw.browsers), 'cross-run browser versions');
same([runs[0].raw.identity, runs[3].raw.identity], 'frozen J A1/A2 identity');
same([runs[1].raw.identity, runs[2].raw.identity], 'frozen M B1/B2 identity');
for (const run of runs) {
  for (const value of Object.values(run.raw.identity)) assert(hashPattern.test(value), 'identity SHA-256');
}
const confirmedTraces = runs.flatMap(run => run.raw.results.map(peer =>
  array(peer.confirmedHashes, 'confirmed hash trace').slice(0, 15)));
const commonConfirmed = same(confirmedTraces, 'all eight confirmed hash traces');
assert.deepEqual(commonConfirmed.map(pair => pair[0]), Array.from({ length: 15 }, (_, i) => (i + 1) * 120),
  'canonical checkpoint frame coverage');
const tapes = [0, 1].map(peer => {
  const values = runs.map(run => {
    const result = run.raw.results[peer];
    const tape = array(result.measure.inputTape, 'wire input tape').slice(0, 1800);
    assert.equal(tape.length, 1800, 'wire input tape incomplete');
    assert.equal(jsonHash(tape), result.inputTapeSha256, 'wire tape SHA-256');
    return tape;
  });
  const tape = same(values, 'same peer wire inputs across all four runs');
  return { peer: peer + 1, frames: tape.length, sha256: jsonHash(tape), fullValuesCompared: true };
});
function matchedOrdinary(peer, comparisonRuns = runs) {
  const maps = comparisonRuns.map(run => run.peers[peer].ordinaryByFrame);
  const frames = [...maps[0].keys()].filter(frame => maps.every(map => map.has(frame)));
  if (peer === 1) assert(frames.length > 0, 'no common P2 exact ordinary frames');
  return {
    peer: peer + 1, count: frames.length,
    firstFrame: frames.length ? Math.min(...frames) : null,
    lastFrame: frames.length ? Math.max(...frames) : null,
    available: frames.length > 0,
    limitation: frames.length ? null : 'No exact ordinary frame qualifies in all four runs for this peer; no matched timing comparison is available.',
    frames, frameListSha256: jsonHash(frames),
    reportedWholeCallbackMs: comparisonRuns.map((run, i) => ({
      run: run.id, ...distribution(frames.map(frame => maps[i].get(frame).workMs))
    }))
  };
}

const touch = report('rollback4x-responsive-input-touch-01-report.json');
assert.equal(touch.wasm, build.L, 'touch candidate identity');
assert.equal(touch.parameters.limit, 900, 'touch frame coverage');
assert.equal(touch.result.framebuffersEqual, true, 'touch framebuffer equality');
same(array(touch.result.hashes, 'touch hashes', 3), 'touch three-world hash');
same(array(touch.result.audits, 'touch audit', 3), 'touch three-world audit');
assert.equal(touch.result.audits[0].length, 48, 'touch audit field coverage');
array(touch.result.summary, 'touch peer summary', 2).forEach(summary => {
  assert.equal(summary[0], 900, 'touch frontier complete');
  assert.equal(summary[4], 900, 'touch once-only capture count');
});
assert.equal(touch.result.tape.length, 900, 'touch tape length');
assert.equal(jsonHash(touch.result.tape), touch.tapeSha256, 'touch exact tape hash');
assert.equal(touch.result.replay.length, touch.replayBytes, 'touch Replay size');
assert.equal(sha256(Buffer.from(touch.result.replay)), touch.replaySha256, 'touch Replay hash');
assert(touch.result.motionSamples[1] > 0, 'touch motion samples missing');

const deep = report('rollback4x-responsive-input-deep8-frontier-01-report.json');
assert.equal(deep.wasm, build.L, 'deep rollback candidate identity');
assert.equal(deep.parameters.deliveryClock, 'frontier', 'deep gate requires monotonic delivery frontier');
assert.equal(deep.parameters.fixedDelay, 8, 'deep transport depth');
assert.equal(deep.parameters.inputDelay, 0, 'deep local input delay');
assert.equal(deep.parameters.forceCorrection, true, 'deep force-correction workload');
assert.equal(deep.parameters.captureTrace, true, 'deep input capture trace');
assert.equal(deep.result.frames, 1800, 'deep frame coverage');
assert.equal(deep.result.framebuffersEqual, true, 'deep framebuffer equality');
same(array(deep.result.hashes, 'deep hashes', 3), 'deep three-world hash');
same(array(deep.result.audited, 'deep audits', 3), 'deep three-world audit');
assert.equal(deep.result.audited[0].length, 48, 'deep audit field coverage');
array(deep.result.summary, 'deep summary', 2).forEach(summary => {
  assert.equal(summary[0], 1800, 'deep frontier complete');
  assert.equal(summary[4], 1800, 'deep once-only capture count');
});
array(deep.result.costs, 'deep cost summaries', 2).forEach(cost => {
  assert(cost.requestedDeepCalls > 100, 'sustained requested depth gate');
  assert.equal(cost.maxRequestedDepth, 8, 'actual restore depth gate');
});
array(deep.result.captures, 'deep capture traces', 2).forEach(trace => {
  array(trace, 'deep peer capture trace', 1800);
  assert.deepEqual(trace.map(row => row[1]), Array.from({ length: 1800 }, (_, i) => i),
    'each frontier input must be captured exactly once');
});
assert(deep.result.partialRecoveryCalls.every(value => value > 0), 'cross-callback recovery absent');
assert(deep.result.preRecoveryCaptures.every(value => value > 0), 'capture before recovery absent');

function sourcePin(component, suffix, expected) {
  const found = Object.entries(component.sources).filter(([path]) =>
    path.replaceAll('\\', '/').endsWith(suffix));
  assert.equal(found.length, 1, 'component source identity missing: ' + suffix);
  assert(hashPattern.test(found[0][1]), 'component source SHA-256');
  if (expected) assert.equal(found[0][1], expected, 'review changed fixture before reusing its coverage: ' + suffix);
  return { path: found[0][0], sha256: found[0][1] };
}
function componentEvidence(component) {
  assert(hashPattern.test(component.wasmSha256), 'component WASM SHA-256');
  assert(component.sources && Object.keys(component.sources).length > 0, 'component sources');
  assert(component.headers && Object.keys(component.headers).length > 0, 'component headers');
  return {
    passed: component.passed, scope: component.scope, wasmSha256: component.wasmSha256,
    compiler: component.compiler, flags: component.flags, sources: component.sources,
    headerCount: Object.keys(component.headers).length,
    recordedHeaderSetSha256: jsonHash(component.headers),
    note: 'Build inputs are those recorded in this component report, not a claim that the current workspace or frozen production releases share these inputs.'
  };
}
const image = report('image-resample-report.json');
const imageFixture = sourcePin(image, '/tests/multiplayer/image-resample.cpp',
  '4197190afa029f28c0316c50b9227cced74490f059c41f4d9fcf90bf95fc5abb');
const session = report('session-report.json');
const sessionFixture = sourcePin(session, '/tests/multiplayer/session.cpp',
  '198cc70dc667085f3c245b89158a0a4b74ab887948a96f138440b97baccb26a6');
sourcePin(session, '/cpp/multiplayer/RollbackSession.cpp');

const textRender = report('rollback4x-responsive-text-render-01-report.json');
assert.deepEqual(textRender.identity.wasm, [build.J, build.M], 'production render comparison identities');
assert.equal(textRender.frames, 7200, 'production render frame coverage');
array(textRender.checks, 'production render checkpoints', 73);
const renderChecks = textRender.checks.map(pair => {
  array(pair, 'render checkpoint pair', 2);
  for (const key of ['frame', 'imageSha256', 'hash', 'session', 'phases']) {
    assert.deepEqual(pair[0][key], pair[1][key], 'render checkpoint equality: ' + key);
  }
  assert(hashPattern.test(pair[0].imageSha256), 'PNG identity');
  return Object.fromEntries(['frame', 'imageSha256', 'hash', 'session', 'phases'].map(key => [key, pair[0][key]]));
});
assert(renderChecks.every((check, i) => !i || check.frame > renderChecks[i - 1].frame),
  'render checkpoint order');
assert.equal(renderChecks.at(-1).frame, 7200, 'render last frame');
assert.equal(renderChecks.at(-1).hash, 763548183, 'render final reference hash');
assert.deepEqual(renderChecks.at(-1).phases, [1, 3], 'render results phase coverage');

const retainedFailures = [
  {
    name: 'rollback4x-responsive-input-deep8-01-failure.json',
    interpretation: 'Coverage gate required more than 100 complete eight-frame recoveries plus a fresh tick within one callback. The new recovery budget deliberately permits partial callbacks; this gate stopped the run before the later reference oracle.'
  },
  {
    name: 'rollback4x-responsive-input-deep8-02-failure.json',
    interpretation: 'The requested-depth gate had been corrected, but this earlier delivery clock followed the rewindable world frame. One peer recorded only 98 qualifying requests. The accepted frontier-clock fixture above uses a monotonic input-delivery frontier. This failure did not establish a correctness mismatch.'
  }
].map(entry => ({ ...entry, rawFailure: report(entry.name, 'coverage-failure') }));


// Extension sets are explicit and all-or-nothing. The original eleven reports
// above remain mandatory regardless of flags; an absent requested extension fails.
const frozen = {
  N: '17b20ffcb71be9bb8d15b7faa22295d61bcc1ef26655f4272b8d2619c8e71a35',
  P: 'de1760ac76f3aaa8cd49c9f267f37f85e86c70d6bf251679a5c93fefacca9b68',
  PInitialProfile: 'afe4ff5664e5341f5230faee1e1fdee7ca2db51053305243e2e2e33572d4b258',
  PFinalProfile: '253adae67aad58822146086abaafa453bdb7b6893531e1aaab18fd4ae9bc56d6',
  hostHarness: '42e6ceae2ed5211e4ea4a979a1996523c2370cd45659c011139193b0a2352e62',
  hostProbe: '35a08e970abddeba9f02b2ba0250013029a6eab8dc9f67cfe0c757a3b96e323b'
};
const progressFiles = {
  NRelease: 'web/artifacts/rollback4x-responsive-network-release/build.json',
  PInitialProfile: 'web/artifacts/rollback4x-responsive-vm-profile/build.json',
  PFinalProfile: 'web/artifacts/rollback4x-responsive-vm-final-profile/build.json',
  PFinalRelease: 'web/artifacts/rollback4x-responsive-vm-final-release/build.json',
  PWorld: reportDirectory + 'rollback4x-responsive-vm-world-01-report.json',
  PDeep: reportDirectory + 'rollback4x-responsive-vm-deep8-01-report.json',
  PRender: reportDirectory + 'rollback4x-responsive-vm-render-01-report.json',
  probeSmoke: reportDirectory + 'rollback4x-responsive-probe-smoke-01-report.json',
  hostSmoke: reportDirectory + 'host-load-probe-smoke-report.json',
  frameSchedule: reportDirectory + 'frame-schedule-report.json'
};
function buildEvidence(path, expected) {
  const data = report(path, 'build');
  assert.equal(data.sha256, expected, path + ': expected frozen build');
  return {
    path, kind: data.kind, builtAt: data.builtAt, completeGame: data.completeGame,
    sha256: data.sha256, bytes: data.bytes,
    imports: data.imports.length, exports: data.exports.length,
    sourceCount: Object.keys(data.sources).length, sourceSetSha256: jsonHash(data.sources),
    relevantSources: Object.fromEntries(Object.entries(data.sources).filter(([name]) =>
      /\/(WorldState|RollbackSession|Application|ImageResample)\.(cpp|hpp)$/.test(name)))
  };
}
function percentages(values) {
  const { count, mean, p50, p95, p99, max } = distribution(values);
  return { count, mean, p50, p95, p99, max, unit: 'percent' };
}
function hostEvidence(host) {
  assert(host && typeof host === 'object', 'requested host record missing');
  const cpu = array(host.cpuSamples, 'host CPU samples');
  const snapshots = array(host.processSamples, 'host process snapshots');
  const initial = cpu.filter(sample => sample.kind === 'initial' || sample.busyFraction === null);
  const cpuIntervals = cpu.filter(sample => Number.isFinite(sample.busyFraction) && !sample.counterDiscontinuity);
  const processRows = new Map();
  const excluded = { failedSnapshotPairs: 0, overflowedSnapshotPairs: 0, nonpositiveTimePairs: 0, negativeCpuDeltas: 0 };
  const roles = new Map();
  for (const entry of host.knownProcesses ?? []) {
    if (!roles.has(entry.pid)) roles.set(entry.pid, []);
    roles.get(entry.pid).push(entry.role);
  }
  const sourceTime = snapshot => finite(snapshot.sourceUtcMs, 'process source UTC time');
  for (const snapshot of snapshots) {
    for (const row of array(snapshot.rows, 'process snapshot rows')) {
      array(row, 'process counter tuple', 3);
      const [pid, name, lifetimeCpuSeconds] = row;
      finite(pid, 'process PID'); finite(lifetimeCpuSeconds, 'process lifetime CPU');
      assert.equal(typeof name, 'string', 'process name');
      const key = pid + ':' + name;
      if (!processRows.has(key)) processRows.set(key, {
        pid, name, appearances: 0, firstSourceUtcMs: sourceTime(snapshot), lastSourceUtcMs: sourceTime(snapshot),
        firstObservedLifetimeCpuSeconds: lifetimeCpuSeconds, lastObservedLifetimeCpuSeconds: lifetimeCpuSeconds,
        consecutiveIntervals: 0, observedIntervalMs: 0, cpuDeltaSeconds: 0,
        maxCoreEquivalents: null, knownRoles: roles.get(pid) ?? []
      });
      const record = processRows.get(key);
      ++record.appearances; record.lastSourceUtcMs = sourceTime(snapshot);
      record.lastObservedLifetimeCpuSeconds = lifetimeCpuSeconds;
    }
  }
  for (let i = 1; i < snapshots.length; ++i) {
    const before = snapshots[i - 1], after = snapshots[i];
    if (before.failed || after.failed) { ++excluded.failedSnapshotPairs; continue; }
    if (before.sourceRowOverflow || before.retainedRowOverflow || after.sourceRowOverflow || after.retainedRowOverflow) {
      ++excluded.overflowedSnapshotPairs; continue;
    }
    const elapsedMs = sourceTime(after) - sourceTime(before);
    if (elapsedMs <= 0) { ++excluded.nonpositiveTimePairs; continue; }
    const previous = new Map(before.rows.map(row => [row[0] + ':' + row[1], row[2]]));
    for (const [pid, name, lifetime] of after.rows) {
      const key = pid + ':' + name;
      if (!previous.has(key)) continue; // Never count a first-seen lifetime value.
      const delta = lifetime - previous.get(key);
      if (delta < 0) { ++excluded.negativeCpuDeltas; continue; }
      const record = processRows.get(key);
      ++record.consecutiveIntervals; record.observedIntervalMs += elapsedMs;
      record.cpuDeltaSeconds += delta;
      record.maxCoreEquivalents = Math.max(record.maxCoreEquivalents ?? 0, delta * 1000 / elapsedMs);
    }
  }
  const candidates = [...processRows.values()].map(record => ({
    ...record,
    cpuDeltaSeconds: record.consecutiveIntervals ? record.cpuDeltaSeconds : null,
    meanCoreEquivalents: record.observedIntervalMs ? record.cpuDeltaSeconds * 1000 / record.observedIntervalMs : null
  })).sort((a, b) => (b.cpuDeltaSeconds ?? -1) - (a.cpuDeltaSeconds ?? -1) || a.pid - b.pid);
  const compilerPattern = /^(?:clang(?:\+\+)?|g\+\+|gcc|cc1plus|wasm-opt|wasm-ld|lld|ninja|cmake|emcc|em\+\+)$/i;
  const compilerNames = [...new Set(candidates.filter(row => compilerPattern.test(row.name)).map(row => row.name))].sort();
  const marks = array(host.marks, 'host marks');
  const segments = marks.slice(1).map((mark, i) => {
    const previous = marks[i];
    const contained = cpuIntervals.filter(sample =>
      sample.atMs <= mark.atMs && sample.atMs - sample.intervalMs >= previous.atMs);
    return {
      from: previous, to: mark,
      fullyContainedCpuIntervals: percentages(contained.map(sample => sample.busyFraction * 100)),
      note: 'Only complete CPU sampling intervals inside these marks; boundary-spanning intervals remain in the full-window summary.'
    };
  });
  return {
    valid: host.valid, complete: host.complete, stopped: host.stopped,
    startedAtUtcMs: host.startedAtUtcMs, endedAtUtcMs: host.endedAtUtcMs,
    scope: host.scope, limitations: host.limitations,
    cpuIntervalMs: host.cpuIntervalMs, processIntervalMs: host.processIntervalMs,
    initialOrUnavailableCpuSamples: initial.length, cpuSamples: cpu.length,
    fullWindowBusyPercent: percentages(cpuIntervals.map(sample => sample.busyFraction * 100)),
    processSnapshots: snapshots.length,
    processSnapshotCounts: snapshots.map(snapshot => ({
      sourceUtcMs: snapshot.sourceUtcMs, sourceDurationMs: snapshot.sourceDurationMs,
      matchingCount: snapshot.matchingCount, retainedRows: snapshot.rows.length,
      byName: snapshot.rows.reduce((counts, row) => ({ ...counts, [row[1]]: (counts[row[1]] ?? 0) + 1 }), {}),
      failed: snapshot.failed, sourceRowOverflow: snapshot.sourceRowOverflow, retainedRowOverflow: snapshot.retainedRowOverflow
    })),
    candidates, compilerOrBuildToolNamesObserved: compilerNames,
    consecutiveProcessCpuMethod: 'Same PID and name in adjacent successful, untruncated snapshots; nonnegative CPU-counter differences only. First appearances and gaps do not contribute lifetime CPU. PID reuse cannot be completely ruled out without a creation-time field.',
    excludedProcessComparisonPairs: excluded,
    firstSeenLifetimeCpuIgnored: true,
    limits: host.limits, overflow: host.overflow, errors: host.errors,
    marks, segmentsBetweenMarks: segments,
    gameSamplesExcludedByHostLoad: 0,
    interpretation: 'valid means sampling integrity, not a stable or uncontended machine. This is the full host observation window, not solely simulation frames 180..1799. Compiler sightings do not establish the cause of any particular stall or all timing differences.'
  };
}
function pairedHostPhaseEvidence(raw) {
  const host = raw.hostLoad, marks = array(host.marks, 'paired host marks');
  function mark(label) {
    const found = marks.filter(value => value.label === label);
    assert.equal(found.length, 1, 'unique paired host phase mark ' + label);
    finite(found[0].utcMs, 'paired phase UTC'); finite(found[0].atMs, 'paired phase monotonic time');
    return found[0];
  }
  const checkpoint = raw.parameters.checkpointBlocks > 0;
  const definitions = [ ['base', 'paired-base-start', checkpoint ? 'paired-checkpoint-start' : 'paired-workload-complete'] ];
  if (checkpoint) definitions.push(['checkpoint', 'paired-checkpoint-start', 'paired-workload-complete']);
  const phases = definitions.map(([kind, fromLabel, toLabel]) => {
    const from = mark(fromLabel), to = mark(toLabel);
    assert(to.atMs > from.atMs && to.utcMs > from.utcMs, 'positive paired phase window');
    const blocks = raw.blocks.filter(block => block.kind === kind);
    assert(blocks.length > 0, 'paired phase has blocks');
    for (const block of blocks) assert(block.startedAtUtcMs >= from.utcMs && block.endedAtUtcMs <= to.utcMs, 'paired block contained in phase marks');
    const cpu = host.cpuSamples.filter(sample => Number.isFinite(sample.busyFraction) && !sample.counterDiscontinuity &&
      sample.atMs - sample.intervalMs >= from.atMs && sample.atMs <= to.atMs);
    const snapshots = host.processSamples.map((sample, index) => ({ sample, index }))
      .filter(({ sample }) => sample.sourceUtcMs >= from.utcMs && sample.sourceUtcMs <= to.utcMs);
    for (let i = 1; i < snapshots.length; ++i) {
      assert.equal(snapshots[i].index, snapshots[i - 1].index + 1, 'phase snapshots retain original adjacency');
      assert(snapshots[i].sample.sourceUtcMs >= snapshots[i - 1].sample.sourceUtcMs, 'phase process source time order');
    }
    const scoped = hostEvidence({ ...host, cpuSamples: cpu, processSamples: snapshots.map(value => value.sample), marks: [] });
    const total = cpu.reduce((sum, sample) => sum + finite(sample.deltaTotalMs, 'phase CPU total counter delta'), 0);
    const idle = cpu.reduce((sum, sample) => sum + finite(sample.deltaIdleMs, 'phase CPU idle counter delta'), 0);
    return {
      kind, from, to, wallClockWindowMs: to.utcMs - from.utcMs,
      blocks: blocks.length, firstBlockIndex: blocks[0].index, lastBlockIndex: blocks.at(-1).index,
      cpu: { fullyContainedIntervals: cpu.length, busyPercent: scoped.fullWindowBusyPercent,
        counterWeightedBusyPercent: total ? 100 * (total - idle) / total : null,
        summedDeltaTotalMs: total, summedDeltaIdleMs: idle,
        logicalCpuCounts: [...new Set(cpu.map(sample => sample.logicalCpus))] },
      processes: { sourceSnapshotIndices: snapshots.map(value => value.index), snapshotCount: snapshots.length,
        candidates: scoped.candidates, compilerOrBuildToolNamesObserved: scoped.compilerOrBuildToolNamesObserved,
        excludedComparisonPairs: scoped.excludedProcessComparisonPairs, firstSeenLifetimeCpuIgnored: true }
    };
  });
  return { phases,
    method: 'Phase marks bracket whole base/checkpoint workload, including oracle, page activation and harness work outside native timers. CPU intervals must be completely contained in the monotonic phase window. Process snapshot sourceUtcMs must lie inside the UTC phase window and original adjacency is retained; only adjacent successful, untruncated, same-PID/name nonnegative differences contribute CPU seconds. Boundary-crossing pairs and first-seen lifetime counters are not assigned to a phase.',
    interpretation: 'Host validity is sampling integrity, not a quiet machine. Compiler activity is an observed confound, not attribution of a particular game pair or native stage. No game pair is excluded and no base-ratio timing correction is performed.'
  };
}
function renderEvidence(raw, identities, label) {
  assert.deepEqual(raw.identity.wasm, identities, label + ': frozen render identities');
  assert.equal(raw.frames, 7200, label + ': render frames');
  array(raw.checks, label + ': checks', 73);
  const checks = raw.checks.map(pair => {
    array(pair, label + ': pair', 2);
    for (const key of ['frame', 'imageSha256', 'hash', 'session', 'phases', 'sdl']) {
      assert.deepEqual(pair[0][key], pair[1][key], label + ': exact ' + key);
    }
    return Object.fromEntries(['frame', 'imageSha256', 'hash', 'session', 'phases'].map(key => [key, pair[0][key]]));
  });
  assert.equal(checks.at(-1).frame, 7200, label + ': final frame');
  assert.equal(checks.at(-1).hash, 763548183, label + ': final hash');
  assert.deepEqual(checks.at(-1).phases, [1, 3], label + ': results phase');
  return {
    passed: true, scope: raw.scope, identity: raw.identity, frames: raw.frames,
    checkpointCount: checks.length, checks,
    all17SdlFieldsEqualAtEveryCheckpoint: true,
    finalSdl: raw.checks.at(-1).map(check => sdlCounters(check.sdl)),
    interpretation: 'Public spectator feed/tick exports and exact PNG/hash/session comparison. No live rollback, input-latency or timed-throughput conclusion.'
  };
}
function smokeEvidence(raw) {
  assert.equal(raw.identity.wasm, build.M, 'probe smoke frozen M');
  assert.equal(raw.identity.harness, '9114b424bcdac93bb0341519a383e6bfc1a47acb4916b428cc6c5f53d6c571cb', 'probe smoke harness');
  assert.equal(raw.parameters.slow, 1, 'probe smoke is not a 4x run');
  assert.equal(raw.parameters.rafDiagnostics, true, 'probe smoke RAF diagnostics');
  assert.equal(raw.parameters.inputMode, 'wall-clock', 'probe smoke input scope');
  assert.equal(raw.parameters.responsiveness, true, 'probe smoke response observations');
  const specifications = [
    { from: 1406, to: 1662, waitFrom: 1407, waitTo: 1660, frame: 424, boundary: 416, dispatch: 1429 },
    { from: 1420, to: 1668, waitFrom: 1422, waitTo: 1667, frame: 425, boundary: 417, dispatch: 1434 }
  ];
  return {
    passed: raw.passed, identity: raw.identity, parameters: raw.parameters, scope: raw.scope,
    originalDualPeerSummary: raw.summary, hostLoadRecorded: !!raw.hostLoad,
    peers: raw.results.map((peer, index) => {
      const spec = specifications[index], samples = peer.measure.samples, diagnostics = peer.rafDiagnostics;
      assert.equal(diagnostics.valid, true, 'RAF smoke integrity');
      assert.equal(diagnostics.overflow, false, 'RAF smoke overflow');
      assert.equal(peer.responsiveness.valid, true, 'response smoke integrity');
      const waits = samples.slice(spec.waitFrom, spec.waitTo + 1);
      assert(waits.every(sample => sample[1] === 2 && sample[3] === spec.frame && sample[4] === spec.boundary),
        'smoke waiting interval has changed');
      for (const column of [5, 6, 7]) same(waits.map(sample => sample[column]), 'smoke frozen counters');
      same(peer.measure.frameDetails.slice(spec.waitFrom, spec.waitTo + 1).map(row => row[1]), 'smoke frozen Save counter');
      const frameHooks = diagnostics.frames.filter(frame =>
        frame.sampleBefore >= spec.from && frame.sampleBefore <= spec.to && frame.sampleAfter === frame.sampleBefore + 1);
      assert.equal(frameHooks.length, spec.to - spec.from + 1, 'one frame hook per smoke sample');
      const requests = frameHooks.map(frame => diagnostics.requests.find(request => request.index === frame.rafIndex));
      assert(requests.every(request => request && request.frameCalls === 1 && request.pauseAtEntry === 0 && request.pauseAtExit === 0),
        'smoke game RAF continued without a pause request');
      const dispatch = diagnostics.requests.find(request => request.index === spec.dispatch);
      assert(dispatch && dispatch.enteredAt - dispatch.requestedAt > 390, 'separate dispatch pause missing');
      const remoteCapture = raw.results[1 - index].responsiveness.captures.find(capture => capture.frame === spec.boundary);
      assert(remoteCapture, 'counterpart had already captured the boundary input');
      assert.deepEqual(peer.measure.networkPacing, [], 'M smoke has no N pacing export');
      const { intents, captures, ...response } = peer.responsiveness;
      return {
        peer: index + 1, lifecycle: peer.lifecycle, audio: audioEvidence(peer),
        rafDiagnostics: {
          version: diagnostics.version, clock: diagnostics.clock, scope: diagnostics.scope,
          valid: diagnostics.valid, counts: diagnostics.counts, limits: diagnostics.limits,
          overflow: diagnostics.overflow, hookErrors: diagnostics.hookErrors
        },
        response: { ...response, intentCount: intents.length, captureCount: captures.length },
        stall: {
          from: endpoint(peer, spec.from), to: endpoint(peer, spec.to),
          submissionRafGapMs: samples[spec.to][13] - samples[spec.from][13],
          recordedGameRafCalls: requests.length, recordedFrameHooks: frameHooks.length,
          internalRafEntryGapMs: distribution(requests.slice(1).map((request, i) => request.enteredAt - requests[i].enteredAt)),
          waiting: {
            from: endpoint(peer, spec.waitFrom), to: endpoint(peer, spec.waitTo),
            callbacks: waits.length, legacyObservationSpanMs: waits.at(-1)[0] - waits[0][0],
            callbackWorkMs: distribution(waits.map(sample => sample[2])),
            frameAndCountersConstant: true
          },
          counterpartBoundaryCapture: { peer: 2 - index, ...remoteCapture },
          earlierDispatchPause: { ...dispatch, requestedToEnteredMs: dispatch.enteredAt - dispatch.requestedAt },
          maxInjectedSendCompletionDelayMs: raw.summary[index].deliveredDelayMs.max,
          networkPacingReadoutAvailable: false
        }
      };
    }),
    interpretation: 'The roughly 394 ms RAF dispatch pause and the later roughly 1.6 s interval with continued RAF/WASM waits are distinct. The missing progress was not a 1.6 s C++ tick or an input-generation deadlock. The counterpart boundary input was observed at the wire hook, but the old report has no per-packet sequence/ACK/fingerprint correlation. It cannot place that specific missing-input packet after WS.send; the unresolved interval is counterpart wire observation to local native application. Global send-delay maxima cannot replace packet correlation.',
    scopeLimit: 'CPU1 frozen M smoke with wall-clock synthetic input, not formal N/P performance or hardware input-to-photon evidence.'
  };
}
function progressEvidence() {
  const builds = {
    NRelease: buildEvidence(progressFiles.NRelease, frozen.N),
    PInitialProfile: buildEvidence(progressFiles.PInitialProfile, frozen.PInitialProfile),
    PFinalProfile: buildEvidence(progressFiles.PFinalProfile, frozen.PFinalProfile),
    PFinalRelease: buildEvidence(progressFiles.PFinalRelease, frozen.P)
  };
  const world = report(progressFiles.PWorld);
  assert.equal(world.wasm, frozen.PInitialProfile, 'world gate uses initial P diagnostic build');
  assert.equal(world.dense, false, 'P world sparse gate');
  assert.equal(world.result.frames, 1800, 'P world frames');
  assert.equal(world.result.checkpoints, 270, 'P world checkpoints');
  for (const key of ['poolBoundary', 'bulletBoundary', 'recordingBoundary']) assert.equal(world.result[key], 1, 'P world ' + key);
  const pDeep = report(progressFiles.PDeep);
  assert.equal(pDeep.wasm, frozen.PFinalProfile, 'P deep final diagnostic build');
  assert.deepEqual(pDeep.parameters, deep.parameters, 'P deep same controlled workload');
  assert.equal(pDeep.harnessSha256, deep.harnessSha256, 'P deep same frontier harness');
  assert.equal(pDeep.result.frames, 1800, 'P deep frames');
  assert.equal(pDeep.result.framebuffersEqual, true, 'P deep three-world PNG');
  same(pDeep.result.hashes, 'P deep three-world hash');
  same(pDeep.result.audited, 'P deep three-world audit');
  assert.deepEqual(pDeep.result.hashes, deep.result.hashes, 'P/L final network hashes');
  assert.deepEqual(pDeep.result.audited, deep.result.audited, 'P/L final audits');
  for (const cost of pDeep.result.costs) {
    assert(cost.requestedDeepCalls > 100 && cost.maxRequestedDepth === 8, 'P deep requested-depth coverage');
  }
  for (const trace of pDeep.result.captures) assert.deepEqual(trace.map(row => row[1]),
    Array.from({ length: 1800 }, (_, i) => i), 'P deep once-only capture');
  assert(pDeep.result.partialRecoveryCalls.every(value => value > 0), 'P deep cross-callback recovery');
  const { samples, captures, ...deepSummary } = pDeep.result;
  const rendering = report(progressFiles.PRender);
  const smoke = report(progressFiles.probeSmoke);
  const hostSmoke = report(progressFiles.hostSmoke);
  assert.equal(hostSmoke.failure, null, 'host smoke failure');
  assert(Object.values(hostSmoke.checks).every(value => value === true), 'host self-test checks');
  const schedule = report(progressFiles.frameSchedule);
  sourcePin(schedule, '/tests/multiplayer/frame-schedule.cpp',
    'a5f21959532b17e25110507bf7b08b2f168252ef03866ab532ee94d08728c7d6');
  return {
    requested: true, mandatoryFiles: Object.values(progressFiles), builds,
    policies: {
      N: 'Only continuous actual-input shortage: F=max(next,replayEnd), B=max(0,F-actualExclusive). Start slowing above B=4, target scale 2 at B>=7; increase by at most 1/50ms, recover by at most 1/150ms. Ready/rollback/expectsInput gating, update after receiving, no new input-delay frames or change to 60Hz logic/eight-frame prediction limit.',
      P: 'Complete 676-byte wasm32 AnmVm first writes at fixed slot*5+part identities; 64 VMs per lazy 43264-byte page, shared whole-slot/per-part bitmap, retained allocation per physical history record. Per-tick checkpoints remain. Journal Undo precedes VM restore, then owning undo; combined 32MiB/8192 first-write budget and separate retained-page capacity.',
      releaseGuard: 'Production release defines TH09_DEVELOPMENT_HARNESS=0; DEV overlap scans are excluded even though the release does not define NDEBUG. Diagnostic builds retain these checks.'
    },
    PInitialWorld: { ...world, report: progressFiles.PWorld, interpretation: 'Initial P profile build; not final release timing evidence.' },
    PFinalDeep8: {
      passed: true, report: progressFiles.PDeep, wasm: pDeep.wasm, scope: pDeep.scope,
      harnessSha256: pDeep.harnessSha256, parameters: pDeep.parameters, result: deepSummary,
      captures: captures.map(trace => ({ count: trace.length, sha256: jsonHash(trace), oncePerFrontier: true })),
      interpretation: 'Final P diagnostic build; reference/hash/audit/PNG and replay-harness assertions passed. Controlled frontier transport, not wall-clock relay or a throughput comparison.'
    },
    PFinalProductionRender: { report: progressFiles.PRender, ...renderEvidence(rendering, [build.M, frozen.P], 'M/P production render') },
    latestFrameScheduleComponent: { report: progressFiles.frameSchedule, ...componentEvidence(schedule) },
    latestSessionAndImageComponents: { sessionReport: reportDirectory + 'session-report.json', imageReport: reportDirectory + 'image-resample-report.json', note: 'Required baseline sections record the latest component reports and their own source hashes; frozen J/M performance identities are unchanged.' },
    MProbeSmoke: { report: progressFiles.probeSmoke, ...smokeEvidence(smoke) },
    hostProbeSelfTest: {
      report: progressFiles.hostSmoke, passed: true, scope: hostSmoke.scope, identity: hostSmoke.identity,
      observedWindowMs: hostSmoke.observedWindowMs, checks: hostSmoke.checks,
      host: hostEvidence(hostSmoke.hostLoad),
      interpretation: 'Host-only 5511ms self-test, independent of M smoke and the formal host ABBA. It did not launch a compiler or establish uncontended host conditions.'
    }
  };
}
function hostAbbaEvidence() {
  const ids = ['a1', 'b1', 'b2', 'a2'];
  const expectedParameters = { ...parameters, rafDiagnostics: false, hostLoad: true };
  const comparison = ids.map(id => {
    const path = reportDirectory + 'rollback4x-responsive-vm-host-' + id + '-report.json';
    const raw = report(path);
    const candidate = id[0] === 'a' ? 'N' : 'P';
    assert.equal(raw.identity.wasm, frozen[candidate], id + ': host ABBA WASM');
    assert.equal(raw.identity.harness, frozen.hostHarness, id + ': host ABBA harness');
    assert.equal(raw.identity.hostLoadProbe, frozen.hostProbe, id + ': host probe identity');
    assert.deepEqual(raw.parameters, expectedParameters, id + ': host ABBA parameters');
    assert.equal(raw.confirmedChecks, 15, id + ': host confirmed checks');
    assert.equal(raw.finalComparable, false, id + ': host live final scope');
    array(raw.results, 'host peers', 2); array(raw.summary, 'host peer summaries', 2);
    return { id, candidate, path, raw,
      peers: raw.results.map((peer, i) => peerEvidence(peer, raw.summary[i])),
      host: hostEvidence(raw.hostLoad) };
  });
  for (const field of ['harness', 'relay', 'shell', 'audioProbe', 'hostLoadProbe']) {
    same(comparison.map(run => run.raw.identity[field]), 'host cross-run ' + field);
  }
  same([comparison[0].raw.identity, comparison[3].raw.identity], 'host frozen N pair');
  same([comparison[1].raw.identity, comparison[2].raw.identity], 'host frozen P pair');
  same(comparison.map(run => run.raw.browsers), 'host cross-run browser versions');
  const hashes = same(comparison.flatMap(run => run.raw.results.map(peer =>
    array(peer.confirmedHashes, 'host canonical trace').slice(0, 15))), 'host eight canonical traces');
  assert.deepEqual(hashes.map(pair => pair[0]), Array.from({ length: 15 }, (_, i) => 120 * (i + 1)), 'host checkpoint coverage');
  const tapeEvidence = [0, 1].map(peer => {
    const values = comparison.map(run => {
      const result = run.raw.results[peer], tape = result.measure.inputTape.slice(0, 1800);
      assert.equal(tape.length, 1800, 'host tape completeness');
      assert.equal(jsonHash(tape), result.inputTapeSha256, 'host wire tape SHA');
      return tape;
    });
    const common = same(values, 'host exact tape values for peer ' + (peer + 1));
    return { peer: peer + 1, frames: common.length, sha256: jsonHash(common), fullValuesCompared: true };
  });
  return {
    requested: true, mandatoryFiles: comparison.map(run => run.path),
    order: ['N', 'P', 'P', 'N'], parameters: expectedParameters,
    harness: frozen.hostHarness, performanceBenefit: 'unconfirmed',
    gameSamplesExcludedByHostLoad: 0,
    interpretation: 'Keep this HOST_LOAD group separate from historical J/M. All four runs and host records are retained; host.valid is not a stability or exclusion gate. Observed compiler/build-tool processes establish an environment confound, not the cause of a specific stall or every difference.',
    crossRunChecks: { identities: true, browsers: true, perPeerTapes: tapeEvidence, eightConfirmedTracesEqual: true, confirmedCheckpoints: hashes, liveFinalHashesCompared: false },
    exactOrdinaryFrameComparisons: [matchedOrdinary(0, comparison), matchedOrdinary(1, comparison)],
    runs: comparison.map(run => ({
      run: run.id, candidate: run.candidate, report: run.path, passed: run.raw.passed,
      identity: run.raw.identity, parameters: run.raw.parameters, browsers: run.raw.browsers,
      scope: run.raw.scope, originalPresentationMeasurement: run.raw.presentationMeasurement,
      sameFrame: run.raw.sameFrame, finalComparable: run.raw.finalComparable,
      originalDualPeerSummary: run.raw.summary,
      peers: run.peers.map((peer, i) => ({ peer: i + 1, ...peer.evidence })),
      host: run.host
    }))
  };
}

function pairedEvidence(kind = 'mp') {
  assert(['mp', 'mm', 'mm-active', 'mp-active'].includes(kind), 'unknown paired diagnostic');
  const active = kind.endsWith('-active'), control = kind.startsWith('mm');
  const checkpointCount = control ? 0 : active ? 80 : 200;
  const blockCount = 40 + checkpointCount, finalFrame = 180 + 8 * blockCount;
  const paths = { mp: 'rollback4x-responsive-vm-paired-4x-01-report.json', mm: 'rollback4x-responsive-paired-control-mm-01-report.json', 'mm-active': 'rollback4x-responsive-paired-control-mm-active-01-report.json', 'mp-active': 'rollback4x-responsive-vm-paired-active-4x-01-report.json' };
  const path = reportDirectory + paths[kind];
  const raw = report(path);
  const expectedIdentity = {
    harness: active ? 'a95f3fcc32ebd5cdeb8f3d103f67311cd88aaaa400a493aac29dc4f6d4e89457' : control ? '70678067bca5f7ab98611351a4623c273f8db9318469d662b74aa0b6e15f9c93' : '7fbdc9e3a231c51ac0b957c9d972e99e035ac57b0d10dbbb8d1e59ac5bd64aec',
    presentationHarness: '23fa2220d9a4b027d743d02b056ead0d7fb759fdf87f8ad6880b4765fcda8a2a',
    presentationServer: 'd72f89dd7574fe3fec3c940cdbf8aef1b30488c48f936257a597cfae24c14802',
    browserLauncher: '76174aa7d19ae02159fb7ac6b484c26bdd5832b69aaddf661e4a95c51313ffed',
    hostLoadProbe: frozen.hostProbe,
    'A.wasm': '3bb69fa3a5ae733834c7b34bc3091c5f8e4d6370ac408e484ed5ab020fb49a35',
    'A.loader': 'dbb2af609470690e80160cd72accb78cfef07606e38fae1d80c02db8df725f28',
    'A.manifest': '66cc5ef7426b9f8f9823459857d8b22f08a7fb04ca7ae3363c562944cff62391',
    'B.wasm': control ? '3bb69fa3a5ae733834c7b34bc3091c5f8e4d6370ac408e484ed5ab020fb49a35' : frozen.PFinalProfile,
    'B.loader': control ? 'dbb2af609470690e80160cd72accb78cfef07606e38fae1d80c02db8df725f28' : '74ee6506e0705b237c4c3fb781bc6321e47310ddc8877ba39414e27480a37d4d',
    'B.manifest': control ? '66cc5ef7426b9f8f9823459857d8b22f08a7fb04ca7ae3363c562944cff62391' : '5e85bf7ad4d0b86a409f713fa8c67b56dfb1c58ff64791ac62e6877a331d4ae1'
  };
  assert.deepEqual(raw.identity, expectedIdentity, 'paired frozen identities');
  assert.deepEqual(raw.finalIdentity, raw.identity, 'paired end-of-run frozen identities');
  assert.equal(raw.correctnessPassed, true, 'paired correctness');
  assert.equal(raw.performanceGate, null, 'paired is not a performance acceptance gate');
  assert.deepEqual(raw.parameters, {
    cpuRate: 4, hardwareRequested: true, hostLoad: true, ...(active ? { activatePage: true } : {}), warmupFrames: 180,
    baseBlocks: 40, checkpointBlocks: checkpointCount,
    ...(control || active ? { workload: control ? 'base-only-control' : 'single-checkpoint' } : {}), ticksPerBlock: 8,
    firstBaseFrame: 180, firstCheckpointFrame: control ? null : 500, finalFrameExclusive: finalFrame,
    order: 'AB for even measured block indices; BA for odd indices; no concurrent game work',
    inputPolicy: 'Unchanged pc-cost-browser deterministic input sequence, openProbe(0,1,2,3)',
    renderPolicy: 'Semantic Draw every tick; submit the eighth tick; unchanged probe geometry policy',
    checkpointPolicy: control ? 'No checkpoint capture, restore or resimulation; base-only control' : 'One sparse checkpoint per eight-tick block, restore once, resimulate the same eight ticks'
  }, 'paired parameters');
  const basicStages = ['baseEightTicks'];
  const directStages = ['capture', 'forwardEightTicks', 'restore', 'resimEightTicks'];
  const checkpointStages = [...directStages, 'correction', 'totalMeasured'];
  const stages = [...basicStages, ...(control ? [] : checkpointStages)];
  const crossChecks = { networkHash: true, audit: true, png: true };
  const restoreChecks = { denseStateHash: true, png: true, networkHash: true, audit: true };
  for (const [id, entry] of Object.entries(raw.builds)) {
    assert(['A', 'B'].includes(id), 'paired build id');
    for (const field of ['wasm', 'loader', 'manifest']) assert.equal(entry[field], raw.identity[id + '.' + field], 'paired build ' + field);
    finite(raw.pageMetadata[id].timeOriginMs, 'paired page time origin');
    assert.equal(raw.finalStatus[id].cursor, finalFrame, 'paired final cursor');
    assert.deepEqual(raw.finalStatus[id].testErrors, [], 'paired final test errors');
  }
  assert.deepEqual(Object.keys(raw.builds).sort(), ['A', 'B'], 'paired two builds');
  let warmupFrontier = 0;
  for (const [i, block] of array(raw.warmupBlocks, 'paired warmup', 23).entries()) {
    assert.equal(block.index, i, 'warmup index');
    assert.equal(block.firstFrame, warmupFrontier, 'warmup first frame');
    assert.equal(block.count, Math.min(8, 180 - warmupFrontier), 'warmup count');
    assert.deepEqual(block.order, i % 2 ? ['B', 'A'] : ['A', 'B'], 'warmup order');
    warmupFrontier += block.count;
    assert.equal(block.frontiers.A, warmupFrontier, 'warmup A frontier');
    assert.equal(block.frontiers.B, warmupFrontier, 'warmup B frontier');
  }
  assert.equal(warmupFrontier, 180, 'complete warmup');
  function checkOracle(oracle, label) {
    finite(oracle.denseStateHash, label + ' dense hash');
    finite(oracle.networkHash, label + ' network hash');
    array(oracle.audit, label + ' audit', 48).forEach(value => assert(Number.isSafeInteger(value), label + ' audit integer'));
    assert(hashPattern.test(oracle.png.sha256) && oracle.png.bytes > 0, label + ' PNG');
  }
  const blocks = array(raw.blocks, 'paired measured blocks', blockCount);
  const conditions = [];
  if (active) {
    for (const block of raw.warmupBlocks) for (const id of ['A', 'B']) conditions.push({ kind: 'warmup', index: block.index, build: id, ...block.pageConditions[id] });
    for (const block of blocks) for (const id of ['A', 'B']) conditions.push({ kind: block.kind, index: block.index, build: id, ...block.builds[id].pageCondition });
    for (const condition of conditions) assert(typeof condition.visibilityState === 'string' && typeof condition.hasFocus === 'boolean', 'recorded page condition schema');
  }
  for (const [i, block] of blocks.entries()) {
    const checkpoint = i >= 40, frame = 180 + i * 8;
    const activeStages = checkpoint ? checkpointStages : basicStages;
    const timedStages = checkpoint ? directStages : basicStages;
    assert.equal(block.index, i, 'paired block index');
    assert.equal(block.kind, checkpoint ? 'checkpoint' : 'base', 'paired block kind');
    assert.equal(block.firstFrame, frame, 'paired block frame');
    assert.equal(block.endFrameExclusive, frame + 8, 'paired block end');
    assert.deepEqual(block.order, i % 2 ? ['B', 'A'] : ['A', 'B'], 'paired measured order');
    assert.deepEqual(block.pairedChecks.forward, crossChecks, 'cross-build forward checks');
    for (const key of ['networkHash', 'audit', 'png'])
      assert.deepEqual(block.builds.A.forwardOracle[key], block.builds.B.forwardOracle[key], 'cross-build forward oracle ' + key);
    for (const id of ['A', 'B']) {
      const row = block.builds[id];
      for (const key of ['index', 'kind', 'firstFrame', 'endFrameExclusive'])
        assert.equal(row[key], block[key], 'paired page/block ' + key);
      assert.deepEqual(Object.keys(row.costMs).sort(), [...activeStages].sort(), 'paired phase cost keys');
      assert.deepEqual(Object.keys(row.timedIntervals).sort(), [...timedStages].sort(), 'paired timer keys');
      let previousEnd = finite(row.startedAtMs, 'paired row start');
      for (const stage of timedStages) {
        const interval = row.timedIntervals[stage];
        const start = finite(interval.startedAtMs, 'paired timer start');
        const end = finite(interval.endedAtMs, 'paired timer end');
        assert(start >= previousEnd && end >= start && end <= row.endedAtMs, 'paired timers are ordered and contained');
        close(row.costMs[stage], end - start, 'paired elapsed duration');
        previousEnd = end;
      }
      checkOracle(row.forwardOracle, 'paired forward');
      if (checkpoint) {
        checkOracle(row.resimOracle, 'paired resimulation');
        assert.deepEqual(row.forwardOracle, row.resimOracle, 'within-build complete recorded oracle');
        assert.deepEqual(row.restoreChecks, restoreChecks, 'within-build restore gates');
        close(row.costMs.correction, row.costMs.capture + row.costMs.restore + row.costMs.resimEightTicks, 'paired correction definition');
        close(row.costMs.totalMeasured, row.costMs.correction + row.costMs.forwardEightTicks, 'paired total includes original forward');
        assert(Number.isSafeInteger(row.checkpointBytesAtCapture) && row.checkpointBytesAtCapture > 0, 'paired capture bytes');
      } else {
        assert.equal(row.resimOracle, null, 'base has no resimulation');
        assert.equal(row.restoreChecks, null, 'base has no restore checks');
        assert.equal(row.checkpointBytesAtCapture, null, 'base has no checkpoint');
      }
    }
    if (checkpoint) {
      assert.deepEqual(block.pairedChecks.resim, crossChecks, 'cross-build resimulation checks');
      assert.deepEqual(block.pairedChecks.restored, { A: restoreChecks, B: restoreChecks }, 'paired restore gates');
      for (const key of ['networkHash', 'audit', 'png'])
        assert.deepEqual(block.builds.A.resimOracle[key], block.builds.B.resimOracle[key], 'cross-build resimulation ' + key);
    } else {
      assert.equal(block.pairedChecks.resim, null, 'base cross resim absent');
      assert.equal(block.pairedChecks.restored, null, 'base restored absent');
    }
    for (const stage of activeStages) close(block.pairedDeltaMs[stage], block.builds.B.costMs[stage] - block.builds.A.costMs[stage], 'paired B-A');
    const [first, second] = block.order.map(id => block.evaluationIntervals[id]);
    assert(first.endedAtUtcMs <= second.startedAtUtcMs, 'paired page evaluations are sequential');
  }
  function validateStats(recorded, values, label) {
    const expected = distribution(values);
    assert.equal(recorded.count, expected.count, label + ' count');
    for (const field of ['total', 'p50', 'p95', 'p99', 'max']) close(recorded[field], expected[field], label + ' ' + field);
    close(recorded.min, Math.min(...values), label + ' min');
  }
  for (const stage of stages) {
    const relevant = blocks.filter(block => Object.hasOwn(block.pairedDeltaMs, stage));
    for (const id of ['A', 'B']) validateStats(raw.summary.builds[id][stage], relevant.map(block => block.builds[id].costMs[stage]), 'paired summary ' + id + ' ' + stage);
    validateStats(raw.summary.pairedDeltaMs[stage], relevant.map(block => block.pairedDeltaMs[stage]), 'paired delta summary ' + stage);
    for (const order of ['AB', 'BA']) validateStats(raw.summary.pairedDeltaByOrder[order][stage],
      relevant.filter(block => block.order.join('') === order).map(block => block.pairedDeltaMs[stage]), 'paired order summary ' + order + ' ' + stage);
  }
  return {
    requested: true, comparison: control ? 'same-build M/M base-only control' : 'M/P phase-cost diagnostic', activatePage: active, mandatoryFiles: [path], report: path,
    passed: true, correctnessPassed: true, performanceGate: null, performanceBenefit: 'unconfirmed',
    scope: raw.scope, measurementNotes: raw.measurementNotes,
    identity: raw.identity, finalIdentity: raw.finalIdentity, builds: raw.builds,
    parameters: raw.parameters, browser: raw.browser, pageMetadata: raw.pageMetadata,
    startedAtUtcMs: raw.startedAtUtcMs, endedAtUtcMs: raw.endedAtUtcMs, finalStatus: raw.finalStatus,
    coverage: { warmupTicks: 180, warmupBlocks: 23, baseBlocks: 40, checkpointBlocks: checkpointCount, measuredBlocks: blockCount, firstFrame: 180, finalFrameExclusive: finalFrame,
      allCrossBuildGates: true, withinBuildRestoreGates: control ? null : true,
      summaryRecomputedFromEveryPair: true, timedIntervalsVerified: true, gamePairsExcluded: 0,
      rawBlockSha256: jsonHash(blocks),
      pngProof: 'Generating harness compared complete PNG strings; retained hashes and byte lengths are rechecked here. Address-sensitive dense hashes are compared within each build only.' },
    originalPhaseAndOrderSummary: raw.summary,
    pairExtremesByStage: Object.fromEntries(stages.map(stage => {
      const relevant = blocks.filter(block => Object.hasOwn(block.pairedDeltaMs, stage));
      const compact = block => ({ index: block.index, firstFrame: block.firstFrame, order: block.order.join(''),
        startedAtUtcMs: block.startedAtUtcMs, endedAtUtcMs: block.endedAtUtcMs,
        aMs: block.builds.A.costMs[stage], bMs: block.builds.B.costMs[stage], bMinusAMs: block.pairedDeltaMs[stage] });
      return [stage, { pairs: relevant.length, negative: relevant.filter(block => block.pairedDeltaMs[stage] < 0).length,
        zero: relevant.filter(block => block.pairedDeltaMs[stage] === 0).length, positive: relevant.filter(block => block.pairedDeltaMs[stage] > 0).length,
        largestPositive: [...relevant].sort((a, b) => b.pairedDeltaMs[stage] - a.pairedDeltaMs[stage]).slice(0, 3).map(compact),
        largestNegative: [...relevant].sort((a, b) => a.pairedDeltaMs[stage] - b.pairedDeltaMs[stage]).slice(0, 3).map(compact),
        note: 'Extreme pairs remain in all summaries. Totals and a difference of marginal medians do not imply the paired median or a uniform per-block effect.' }];
    })),
    initialCaptureBytes: { pairs: checkpointCount,
      equalPairs: blocks.filter(block => block.kind === 'checkpoint' && block.builds.A.checkpointBytesAtCapture === block.builds.B.checkpointBytesAtCapture).length,
      mismatches: blocks.filter(block => block.kind === 'checkpoint' && block.builds.A.checkpointBytesAtCapture !== block.builds.B.checkpointBytesAtCapture).map(block => ({ index: block.index, a: block.builds.A.checkpointBytesAtCapture, b: block.builds.B.checkpointBytesAtCapture })) },
    observedMemoryBytes: Object.fromEntries(['A', 'B'].map(id => [id, [...new Set([raw.pageMetadata[id].initialMemoryBytes, ...blocks.map(block => finite(block.builds[id].memoryBytes, 'paired memory observation'))])]])),
    pageConditionObservations: { available: active, count: conditions.length, visible: conditions.filter(x => x.visibilityState === 'visible').length, focused: conditions.filter(x => x.hasFocus).length, allVisibleAndFocused: active ? conditions.every(x => x.visibilityState === 'visible' && x.hasFocus) : null, sha256: active ? jsonHash(conditions) : null, samplesExcludedByFocus: 0, note: 'Block-entry conditions are observations, not a focus-based performance filter or a guarantee of symmetric scheduling.' },
    host: hostEvidence(raw.hostLoad), hostByWorkloadPhase: pairedHostPhaseEvidence(raw),
    interpretation: active ? 'Explicit activation changes the page conditions. Interpret phase costs beside the matching same-build activation control and keep all pairs and order summaries; this diagnostic does not establish live release performance or uniquely identify the cause of earlier asymmetry.' : control ? 'Identical frozen M on both pages still shows a similar A/B base-time difference. This is direct evidence of a paired-method/page-environment confound, not VM code regression. It does not quantify how much of the earlier checkpoint phase difference comes from that confound.' : 'P was slower across phases in this run, including the no-checkpoint base and checkpoint-free resimulation. This does not isolate VM capture cost or establish a production regression. The same-build control, when explicitly included, is a separate method check; no observations are removed.',
    audioScope: 'The rollback-step probe temporarily sets recording_frame, suppressing tick_inputs audio update/pump; it also bypasses rollback_tick confirmed-audio publication. Base and resimulation timings do not include real AudioDevice::pump and cannot exclude live audio as a bottleneck.',
    criticalCostDefinition: control ? 'No checkpoint or VM first-write capture, restore or resimulation is exercised. This is a page/method control only.' : 'VM first-write work belongs to forwardEightTicks after Save. correction excludes that original forward; totalMeasured includes capture, forwardEightTicks, restore and resimEightTicks. One checkpoint per eight ticks and DEV overlap assertions differ from live per-tick final release.',
    pending: ['Final-release live response and network-recovery acceptance; all diagnostic page conditions remain separate']
  };
}

function responsivenessEvidence(raw) {
  assert.equal(raw.valid, true, 'response probe integrity');
  assert.equal(raw.inputDelay, 0, 'existing D=0 contract');
  assert.deepEqual(raw.errors, [], 'response errors');
  const counts = { planned: raw.intents.length, submitted: 0, captured: 0, presented: 0,
    notSubmitted: 0, submittedNotCaptured: 0, capturedNotPresented: 0, sameMaskResubmissions: 0 };
  const definitions = {
    scheduleToSubmit: ['submittedAtMs', 'scheduledAtMs'], submitToFirstWire: ['firstWireAtMs', 'submittedAtMs'],
    firstWireToPresentationObserved: ['presentationObservedAtMs', 'firstWireAtMs'],
    scheduleToPresentationObserved: ['presentationObservedAtMs', 'scheduledAtMs']
  };
  for (const [i, row] of raw.intents.entries()) {
    assert.equal(row.id, i, 'response intent identity');
    if (row.submittedAtMs !== null) counts.submitted++;
    if (row.captureFrame !== null) counts.captured++;
    if (row.presentationObservedAtMs !== null) counts.presented++;
    if (row.changedSubmittedMask === false) counts.sameMaskResubmissions++;
    const status = row.presentationObservedAtMs !== null ? 'presented' : row.captureFrame !== null ?
      'captured-not-presented' : row.submittedAtMs !== null ? 'submitted-not-captured' : 'not-submitted';
    assert.equal(row.status, status, 'response outcome');
    if (status === 'not-submitted') counts.notSubmitted++;
    if (status === 'submitted-not-captured') counts.submittedNotCaptured++;
    if (status === 'captured-not-presented') counts.capturedNotPresented++;
    for (const [name, [to, from]] of Object.entries(definitions)) {
      if (row[to] === null) assert.equal(row[name + 'Ms'], null, 'unobserved response time');
      else close(row[name + 'Ms'], row[to] - row[from], 'response time difference');
    }
  }
  assert.deepEqual(counts, raw.counts, 'all response coverage counts');
  const extremeIntents = {};
  for (const name of Object.keys(definitions)) {
    const rows = raw.intents.filter(row => row.changedSubmittedMask !== false && row[name + 'Ms'] !== null);
    const actual = raw.conditionalLatencyMs[name], computed = distribution(rows.map(row => row[name + 'Ms']));
    assert.equal(actual.count, computed.count, 'response conditional count');
    for (const field of ['p50', 'p95', 'p99', 'max']) close(actual[field], computed[field], 'response ' + field);
    extremeIntents[name] = [...rows].sort((a, b) => b[name + 'Ms'] - a[name + 'Ms']).slice(0, 3);
  }
  const { intents, captures, ...summary } = raw;
  assert(captures.length > 0, 'response capture coverage');
  for (const [i, capture] of captures.entries()) {
    assert.equal(capture.frame, captures[0].frame + i, 'continuous capture trace');
    assert.equal(capture.contiguous, true, 'capture trace continuity flag');
  }
  return { ...summary, coverageAndLatencyRecomputed: true, initialIntent: intents[0], extremeIntents,
    unresolvedIntents: intents.filter(row => row.status !== 'presented'),
    intentTrace: { count: intents.length, sha256: jsonHash(intents) },
    captureTrace: { count: captures.length, first: captures[0].frame, last: captures.at(-1).frame, sha256: jsonHash(captures) },
    interpretation: 'Counts accompany conditional latency. Same-mask resubmissions are excluded only from those distributions. Initial and uncompleted intents are retained; first wire is after sampling/serialization, and presentation is a callback observation, not physical pixels or acknowledgement.' };
}
function responsivePresentationEvidence(peer, summary) {
  const raw = peer.measure.samples, diagnostics = peer.rafDiagnostics;
  assert.equal(diagnostics.valid, true, 'burst RAF diagnostics');
  assert.equal(diagnostics.overflow, false, 'burst RAF overflow');
  const frames = new Map();
  for (const frame of diagnostics.frames) if (frame.sampleAfter === frame.sampleBefore + 1) {
    assert(!frames.has(frame.sampleBefore), 'one frame hook per sample');
    frames.set(frame.sampleBefore, frame);
  }
  function point(index) {
    const frame = frames.get(index);
    assert(frame, 'sample has an actual frame hook');
    return { ...endpoint(peer, index), onGameFrame: frame };
  }
  function window(filter) {
    const indices = raw.map((sample, i) => ({ sample, i })).filter(({ sample }) => filter(sample));
    let prior = null;
    const intervals = [];
    for (const current of indices) {
      if ((!prior || current.sample[11] !== prior.sample[11]) && (!prior || current.sample[3] > prior.sample[3])) {
        if (prior) {
          const a = frames.get(prior.i), b = frames.get(current.i);
          assert(a && b, 'new-frontier endpoints have frame hooks');
          intervals.push({ from: prior.i, to: current.i,
            rafGapMs: current.sample[13] - prior.sample[13],
            legacyObservationGapMs: current.sample[0] - prior.sample[0],
            onFrameEntryGapMs: b.enteredAt - a.enteredAt, onFrameExitGapMs: b.exitedAt - a.exitedAt });
        }
        prior = current;
      }
    }
    const names = ['rafGapMs', 'legacyObservationGapMs', 'onFrameEntryGapMs', 'onFrameExitGapMs'];
    const extremes = Object.fromEntries(names.map(name => [name, [...intervals].sort((a, b) => b[name] - a[name]).slice(0, 3)
      .map(value => ({ ...value, from: point(value.from), to: point(value.to) }))]));
    return { sampleCount: indices.length,
      callbackWorkMs: distribution(indices.map(({ sample }) => sample[2])),
      intervals: Object.fromEntries(names.map(name => [name, distribution(intervals.map(value => value[name]))])),
      extremes, intervalCount: intervals.length };
  }
  const formal = window(sample => sample[3] >= 180 && sample[3] < 1800);
  const expected = summary.effectivePresentationGapMs;
  assert.equal(formal.intervals.rafGapMs.count, expected.count, 'formal effective-gap count');
  for (const field of ['total', 'p50', 'p95', 'p99', 'max']) close(formal.intervals.rafGapMs[field], expected[field], 'formal effective-gap ' + field);
  for (const field of ['over100ms', 'over250ms']) assert.equal(formal.intervals.rafGapMs[field], expected[field], 'formal gap threshold');
  return { definitions: 'Each interval requires a new renderer submission and a strictly newer logical frontier. Original RAF timestamps, legacy observation times and actual onGameFrame entry/exit are kept separately. No startup samples are deleted.',
    diagnostics: { valid: diagnostics.valid, counts: diagnostics.counts, limits: diagnostics.limits, overflow: diagnostics.overflow, hookErrors: diagnostics.hookErrors },
    formal180To1799: formal, startupBelow180: window(sample => sample[3] < 180), entireRecordedActiveWindow: window(() => true),
    firstRecordedSamples: raw.slice(0, 4).map((_, i) => point(i)) };
}
function transportTraceEvidence(peers, candidate) {
  const origins = peers.map(peer => finite(peer.networkTransport.clock.started.timeOrigin, 'network page origin'));
  const starts = peers.map((peer, i) => origins[i] + peer.measure.started);
  const commonStart = starts[0];
  const columnMap = names => Object.fromEntries(names.map((name, i) => [name, i]));
  const traces = peers.map((peer, side) => {
    const trace = peer.networkTransport;
    assert.equal(trace.valid, true, 'network probe valid');
    assert.equal(trace.coverage.exercised, true, 'network trace exercised');
    assert.equal(trace.coverage.pacingAvailable, candidate === 'P', 'pacing availability belongs to loaded build');
    assert.equal(trace.coverage.exactNativeAcceptance, false, 'dequeue is not ApplyInputPacket acceptance');
    assert.equal(trace.coverage.relayTraceIncluded, false, 'no relay packet trace');
    for (const [name, value] of Object.entries(trace.overflow)) assert.equal(value, name === 'firstAt' ? null : 0, 'network overflow');
    assert.deepEqual(trace.errors, [], 'network trace errors');
    assert.deepEqual(trace.columns.phases, ['intent', 'ws-send', 'message', 'poll-copy'], 'network phases');
    const pc = columnMap(trace.columns.packet), ec = columnMap(trace.columns.event), nc = columnMap(trace.columns.pacing);
    const packets = new Map();
    for (const row of trace.packets) {
      assert.equal(row.length, trace.columns.packet.length, 'packet row schema');
      assert(!packets.has(row[pc.id]), 'packet IDs unique');
      packets.set(row[pc.id], row);
    }
    const phases = [[], [], [], []];
    for (const row of trace.events) {
      assert.equal(row.length, trace.columns.event.length, 'event row schema');
      const packet = packets.get(row[ec.packetId]);
      assert(packet, 'event packet identity');
      const phase = row[ec.phase];
      assert(Number.isInteger(phase) && phase >= 0 && phase < 4, 'event phase');
      const at = finite(row[ec.enteredAt], 'event time'), exit = finite(row[ec.exitedAt], 'event exit');
      assert(exit >= at, 'event exits after entry');
      phases[phase].push({ side, key: JSON.stringify(packet.filter((_, i) => i !== pc.id)), packet,
        eventId: row[ec.id], packetId: row[ec.packetId], phase, enteredAt: at, exitedAt: exit,
        absoluteAt: origins[side] + at, senderAgeMs: at - peer.measure.started,
        p1AgeMs: origins[side] + at - commonStart });
    }
    for (const [i, count] of ['inputIntent', 'inputWsSend', 'inputMessage', 'inputPollCopy'].entries())
      assert.equal(phases[i].length, trace.counts[count], 'trace phase count ' + count);
    function paceWindow(lo, hi) {
      const selected = trace.pacing.filter(row => row[nc.at] - peer.measure.started >= lo && row[nc.at] - peer.measure.started < hi);
      const read = name => selected.map(row => row[nc[name]]);
      const range = values => values.length ? { min: Math.min(...values), max: Math.max(...values) } : null;
      return { ageFromMs: lo, ageToMs: hi, rows: selected.length,
        outstanding: range(read('outstandingFrames')), networkScaleMilli: range(read('networkIntervalScaleMilli')),
        effectiveScaleMilli: range(read('effectiveIntervalScaleMilli')),
        first: selected[0] ?? null, last: selected.at(-1) ?? null };
    }
    let actualRegressions = 0, previousActual = null;
    for (const row of trace.pacing) {
      assert.equal(row.length, trace.columns.pacing.length, 'pacing row schema');
      const version = row[nc.version];
      assert.equal(version, 1, 'pacing getter schema');
      const f = row[nc.predictionFrontier], a = row[nc.actualContiguousExclusive];
      assert.equal(row[nc.outstandingFrames], Math.max(0, f - a), 'actual-frontier deficit');
      if (previousActual !== null && a < previousActual) actualRegressions++;
      previousActual = a;
    }
    if (candidate === 'M') assert.equal(trace.pacing.length, 0, 'M has no raw actual/pacing export');
    const metadata = Object.fromEntries(['schema', 'scope', 'clock', 'limits', 'overflow', 'counts', 'valid', 'coverage', 'identity', 'columns', 'states', 'sockets', 'errors'].map(key => [key, trace[key]]));
    return { phases, evidence: { peer: side + 1, ...metadata, packetRows: trace.packets.length, eventRows: trace.events.length,
      packetSha256: jsonHash(trace.packets), eventSha256: jsonHash(trace.events),
      rawPacing: { available: candidate === 'P', rows: trace.pacing.length, sha256: jsonHash(trace.pacing),
        actualRegressionCount: candidate === 'P' ? actualRegressions : null,
        windows: [paceWindow(19000, 20000), paceWindow(20000, 20500), paceWindow(20500, 21000), paceWindow(21000, 22000)],
        interpretation: 'Only existing safe-boundary reads. Missing M exports are unavailable, not zero shortage; normalized published/confirmed stats are not substituted. Input shortage may reflect a slow peer sampling/sending later as well as transport delay.' } } };
  });
  function group(rows) {
    const result = new Map();
    for (const row of rows) { if (!result.has(row.key)) result.set(row.key, []); result.get(row.key).push(row); }
    return result;
  }
  function publicEvent(value) { const { key, absoluteAt, ...rest } = value; return rest; }
  function join(left, right) {
    const a = group(left), b = group(right), matched = [], unmatchedLeft = [], unmatchedRight = [];
    for (const key of new Set([...a.keys(), ...b.keys()])) {
      const x = a.get(key) ?? [], y = b.get(key) ?? [], count = Math.min(x.length, y.length);
      for (let i = 0; i < count; i++) matched.push({ key, left: x[i], right: y[i], elapsedMs:
        x[i].side === y[i].side ? y[i].enteredAt - x[i].enteredAt : y[i].absoluteAt - x[i].absoluteAt });
      unmatchedLeft.push(...x.slice(count)); unmatchedRight.push(...y.slice(count));
    }
    return { matched, unmatchedLeft, unmatchedRight,
      duplicateKeysLeft: [...a.values()].filter(values => values.length > 1).length,
      duplicateKeysRight: [...b.values()].filter(values => values.length > 1).length };
  }
  function summarizeJoin(joined, subset = null) {
    const pairs = subset ? joined.matched.filter(value => subset.has(value.key)) : joined.matched;
    const left = subset ? joined.unmatchedLeft.filter(value => subset.has(value.key)) : joined.unmatchedLeft;
    const right = subset ? joined.unmatchedRight.filter(value => subset.has(value.key)) : joined.unmatchedRight;
    return { matched: pairs.length, latencyMs: distribution(pairs.map(value => value.elapsedMs)),
      negativeClockDifferences: pairs.filter(value => value.elapsedMs < 0).length,
      duplicateKeysLeft: joined.duplicateKeysLeft, duplicateKeysRight: joined.duplicateKeysRight,
      unmatchedLeftCount: left.length, unmatchedRightCount: right.length,
      unmatchedLeft: left.map(publicEvent), unmatchedRight: right.map(publicEvent),
      longest: [...pairs].sort((a, b) => b.elapsedMs - a.elapsedMs).slice(0, 3)
        .map(value => ({ elapsedMs: value.elapsedMs, from: publicEvent(value.left), to: publicEvent(value.right) })) };
  }
  const directions = traces.map((trace, side) => {
    const receiver = traces[1 - side], burst = new Set(trace.phases[0].filter(value => value.senderAgeMs >= 20000 && value.senderAgeMs < 20500).map(value => value.key));
    const joins = { intentToWsSend: join(trace.phases[0], trace.phases[1]), wsSendToPeerMessage: join(trace.phases[1], receiver.phases[2]), peerMessageToPollCopy: join(receiver.phases[2], receiver.phases[3]) };
    return { fromPeer: side + 1, toPeer: 2 - side, burstIntentKeys: burst.size,
      entireTrace: Object.fromEntries(Object.entries(joins).map(([name, value]) => [name, summarizeJoin(value)])),
      senderBurstWindow: Object.fromEntries(Object.entries(joins).map(([name, value]) => [name, summarizeJoin(value, burst)])) };
  });
  return { clock: { method: 'Page timeOrigin + enteredAt; examples use one P1 measure-start axis. These browser clocks do not separate relay/OS stages.', p1StartEpochMs: commonStart, peerStartOffsetFromP1Ms: starts.map(value => value - commonStart) },
    matching: 'Every packet header field except local packet ID, including session/sender/destination/sequence/ACK/first/count/payload length/FNV32; FIFO occurrences kept without deduplication. Unmatched observations and negative clock differences are retained.',
    peers: traces.map(trace => trace.evidence), directions,
    interpretation: 'Message/poll-copy and websocket boundaries are observed. Poll-copy is not native packet acceptance; no relay/OS trace exists. End-window unmatched observations remain unresolved, and these new runs cannot localize the old M smoke missing-frame packet.' };
}
function networkBurstEvidence() {
  const config = [
    { candidate: 'P', file: 'rollback4x-responsive-vm-network-burst-01-report.json', wasm: frozen.P, loader: '4e53b06c7e2a99624bc4234ffde1682a9a18ce7c89be3d2a414b4f0cb3620ee8' },
    { candidate: 'M', file: 'rollback4x-responsive-text-network-burst-01-report.json', wasm: build.M, loader: 'ba7571dbb7da2e2503930b50389761fa85f7c118c5a86e7cd9947023b21eebf9' }
  ];
  const expectedParameters = { ...parameters, inputMode: 'wall-clock', responsiveness: true,
    burstAt: 20000, burstFor: 500, burstDelay: 120, rafDiagnostics: true, hostLoad: true, networkTrace: true };
  const observations = config.map(item => {
    const raw = report(item.file), identity = {
      wasm: item.wasm, harness: '6137632e9b273512a529f2f027a00fa02e1a880f7d274db6bad68c7996df6398', loader: item.loader,
      relay: '56c44ab7520381859b05d21130da40ad605722b42e49ed37c089d0ecf8bbe90c',
      shell: '719347be3c3bdbf9f793145ae0ea8806dcb25b7bb775cf853c4a792c451c9ec8',
      audioProbe: 'c0ab0092b4f49c20e3d07125dcf1df39496ebfafacfbc34f9eff00942c46e5bf',
      responsivenessProbe: 'b0abac8dd20e117c0579f65a0fb9a49eb8a0178ae15b9fd88605a8ae15969cc8',
      hostLoadProbe: frozen.hostProbe, networkTransportProbe: '090fe224bf5642b3daf4aaf354b67287ba5ad0b39a9d4f336cc1abe6603578c3'
    };
    assert.deepEqual(raw.identity, identity, 'network diagnostic frozen identities');
    assert.deepEqual(raw.parameters, expectedParameters, 'network diagnostic parameters');
    assert.equal(raw.confirmedChecks, 15, 'network canonical checkpoint count');
    assert.equal(raw.finalComparable, false, 'network final frame scope');
    array(raw.results, 'network peers', 2); array(raw.summary, 'network summaries', 2);
    const confirmed = same(raw.results.map(peer => peer.confirmedHashes.slice(0, 15)), 'within-run network canonical hashes');
    assert.deepEqual(confirmed.map(row => row[0]), Array.from({ length: 15 }, (_, i) => 120 * (i + 1)), 'network canonical coverage');
    for (const peer of raw.results) {
      assert(peer.impairment.burstPackets > 0, 'burst actually exercised');
      assert.equal(peer.impairment.bursts.length, peer.impairment.burstPackets, 'burst count');
    }
    return { candidate: item.candidate, report: reportDirectory + item.file, passed: true, identity: raw.identity,
      parameters: raw.parameters, scope: raw.scope, originalPresentationMeasurement: raw.presentationMeasurement,
      browsers: raw.browsers, originalDualPeerSummary: raw.summary,
      withinRunChecks: { confirmedCheckpoints: confirmed, twoPeerConfirmedHashesEqual: true, liveFinalHashesCompared: false },
      peers: raw.results.map((peer, i) => ({ peer: i + 1, ...peerEvidence(peer, raw.summary[i]).evidence,
        responsiveness: responsivenessEvidence(peer.responsiveness), presentation: responsivePresentationEvidence(peer, raw.summary[i]) })),
      transport: transportTraceEvidence(raw.results, item.candidate), host: hostEvidence(raw.hostLoad) };
  });
  return { requested: true, mandatoryFiles: observations.map(value => value.report), observations,
    performanceBenefit: 'unconfirmed', causalABComparison: false, sameLogicalFrameInputTapeClaimed: false,
    interpretation: 'Two separate wall-clock synthetic-input observations with the same configured impairment, not a matched input tape or causal A/B comparison. Every startup, formal interval, unsubmitted intent and end-window transport observation remains represented.',
    pacingConclusion: 'P1 was already near network scale 2 before the burst and returned near 2 afterward; P2 stayed at 1. This run does not demonstrate restoration to normal speed after a network-only disturbance. CPU pressure can delay peer input production even though the controller does not read CPU duration.',
    clientImpairmentBoundary: 'The configured 39+/-5ms and burst delays are injected by client-main-thread setTimeout. The measured startup intent-to-WS delays include impairment timers delayed by the same long C++ callbacks. They cannot be directly extrapolated to the production path with no client timer injection. C++ callback durations and startup presentation/response gaps remain actual observations. Setting application DELAY=0 still uses setTimeout(0); it is not a synchronous-send control.',
    nextNetworkCondition: 'Planned only: synchronous client send plus delay/jitter/burst injection and per-packet receive/send timestamps in the independent relay process. Existing relay delay/jitter environment settings can support the baseline condition. No such new run is included here.',
    acceptance: 'D=0 and continuing genuinely new game frames remain the target. Observed final P 4x experience is not accepted; absence of a >250ms formal gap does not remove the startup stalls or prove the old stall fixed.' };
}

const evidence = {
  schemaVersion: 1,
  generator: 'web/tests/multiplayer/summarize-responsive.mjs',
  scope: 'Responsive TH09 follow-up evidence only. Historical J/M ABBA, explicitly listed correctness scopes, and opt-in P progress, N/P HOST ABBA and paired diagnostics remain separate. No earlier rollback4x evidence is imported.',
  verdict: {
    performanceBenefit: 'unconfirmed',
    stable60HzAt4xAchievedInObservedRuns: false,
    measuredP2LogicHzInABBAOrder: runs.map(run => ({ run: run.id, candidate: run.candidate, logicHz: run.raw.summary[1].logicHz })),
    correctness: 'The explicitly listed scopes passed. Their scope is not extended to unmeasured production pacing or acoustic continuity.',
    interpretation: 'Historical J/M ABBA: both M runs were slower than J A1; J A2 also slowed. Ordinary single-frame callbacks differ even after exact logical-frame matching. Those four runs do not isolate code effects from unrecorded host competition. Separate HOST, paired and network extensions retain their own conditions and interpretation.',
    historicalJmHostCompetitionRecorded: false,
    status: 'experimental-correctness-passed-in-listed-scopes-performance-not-accepted',
    limitations: [
      'Historical J/M ABBA reports contain no host CPU or build-process record; other tasks may have compiled or competed for resources. Separate HOST, paired and network extensions do record host observations under their own conditions. Neither a code regression nor a noise-only explanation is established by the historical group.',
      'No samples or outliers were discarded. Raw reports, including both earlier coverage failures, remain unchanged.',
      'Counter reductions are not CPU-time improvements. Release frameDetails expose a capture count but no separate Save, audio-decoder or GPU timing.',
      'Only P2 used the 4x browser CPU throttle in the live ABBA and network observations. Paired diagnostics requested 4x for both pages and executed game work sequentially. These desktop observations are not phone or physical-display acceptance.',
      'Frozen J and M runs did not contain the subsequent N network-pacing change.'
    ]
  },
  builds: build,
  measurementDefinitions: {
    selectedWindow: 'Every raw callback with reported simulation frame >= 180 and < 1800; deltas use its immediately preceding raw callback. No outlier exclusions.',
    categories: 'Correction/replay if corrections or resimulated delta is nonzero, or frame moves backwards; otherwise positive frame delta with or without a WorldState save; all remaining callbacks are no-forward.',
    matchedOrdinary: 'Same exact frame in every run; one forward tick, one new submission, phase 1, no correction, no replay and no WorldState save. Whole callback cost still includes other work such as audio.',
    checkpointCounter: 'frameDetails[i][1] is cumulative WorldState saves. sample[7] counts local input captures; they are different operations.',
    confirmedBoundary: 'sample[4] exports max(published, ConfirmedThrough + 1), with published fallback. It is not a raw remote-actual frontier measurement.',
    presentation: 'sample[13] is the browser-provided RAF timestamp. Submission intervals indicate new renderer submissions, not physical scanout.',
    legacyObservation: 'sample[0] + measure.started is recorded after play.frame(), before updateInput/readStats/samples.push. It is not the true onGameFrame exit. The original summary field callbackCompletionGapMs is retained unchanged, with this naming limitation.',
    extremeAttribution: 'Each extreme retains both endpoint indices, frames, RAF times and legacy observation times. A long interval cannot automatically be attributed to the frame at its later endpoint or to reported C++ work.',
    sdl: 'sdlMeasured is the recorded counter delta from the first >=180 crossing to the report end, which can exceed frame 1800. Close totals are workload evidence, not per-call GPU timing.',
    audio: 'Audio continues in wall-clock time; a slower completion can require more PCM blocks for the same logic tape. No per-decoder CPU time or acoustic continuity test is present.',
    quantiles: 'Sorted sample at floor(count * q), matching the generating harness.'
  },
  abba: {
    order: ['a1', 'b1', 'b2', 'a2'], parameters,
    crossRunIdentity: {
      sameHarnessRelayShellAudioProbe: true, sameBrowserVersions: true,
      frozenAPairIdentical: true, frozenBPairIdentical: true
    },
    crossRunCorrectness: {
      peerTapes: tapes, confirmedCheckpointCountPerPeer: 15,
      eightConfirmedTracesEqual: true, confirmedCheckpoints: commonConfirmed,
      liveFinalHashesCompared: false,
      reason: 'The live reports end on unequal simulation frames; finalComparable is false in every run.'
    },
    exactOrdinaryFrameComparisons: [matchedOrdinary(0), matchedOrdinary(1)],
    runs: runs.map(run => ({
      run: run.id, candidate: run.candidate,
      rawReport: reportDirectory + 'rollback4x-responsive-release-' + run.id + '-report.json',
      passed: run.raw.passed, identity: run.raw.identity, parameters: run.raw.parameters,
      browsers: run.raw.browsers, scope: run.raw.scope,
      originalPresentationMeasurement: run.raw.presentationMeasurement,
      sameFrame: run.raw.sameFrame, finalComparable: run.raw.finalComparable,
      confirmedChecks: run.raw.confirmedChecks,
      originalDualPeerSummary: run.raw.summary,
      peers: run.peers.map((peer, i) => ({ peer: i + 1, ...peer.evidence }))
    }))
  },
  correctness: {
    L_nativeTouch900: {
      passed: true, report: reportDirectory + 'rollback4x-responsive-input-touch-01-report.json',
      scope: touch.scope, wasm: touch.wasm, harnessSha256: touch.harnessSha256,
      parameters: touch.parameters, peerSummary: touch.result.summary,
      hashes: touch.result.hashes, commonAudit: touch.result.audits[0],
      framebuffersEqual: true, tapeSha256: touch.tapeSha256,
      replaySha256: touch.replaySha256, replayBytes: touch.replayBytes,
      motionSamples: touch.result.motionSamples, gestureEvents: touch.result.events,
      waits: touch.result.waits, iterations: touch.result.iterations, measured: touch.result.measured,
      note: 'Native gesture, exact tape and once-only capture/reference coverage at the recorded build. Not wall-clock relay, 4x performance or phone responsiveness acceptance.'
    },
    L_actualFrontierDeep8: {
      passed: true, report: reportDirectory + 'rollback4x-responsive-input-deep8-frontier-01-report.json',
      scope: deep.scope, wasm: deep.wasm, harnessSha256: deep.harnessSha256, parameters: deep.parameters,
      frames: deep.result.frames, iterations: deep.result.iterations, peerSummary: deep.result.summary,
      hashes: deep.result.hashes, commonAudit: deep.result.audited[0], framebuffersEqual: true,
      replayBytes: deep.result.replayBytes,
      costs: deep.result.costs, peakBullets: deep.result.peakBullets, phases: deep.result.phases,
      pauseCalls: deep.result.pauseCalls, preRecoveryCaptures: deep.result.preRecoveryCaptures,
      partialRecoveryCalls: deep.result.partialRecoveryCalls,
      captureTraces: deep.result.captures.map(trace => ({ count: trace.length, sha256: jsonHash(trace), oncePerFrontierFrame: true })),
      note: 'Sustained requested depth, not mandatory completion within one callback. Replay equality is a generating-harness assertion; this report retains the byte length rather than raw replay bytes. No real-relay 4x pacing conclusion.'
    },
    M_imageResampleComponent: {
      report: reportDirectory + 'image-resample-report.json', ...componentEvidence(image),
      fixture: imageFixture, authoredFixtureCases: 390,
      caseCountProvenance: 'Source-audited pinned fixture: 17 cache cases + 225 format/size cases + 3 workspace boundaries + 6 reuse cases + 128 random cases + 5 in-place cases + 6 invalid cases. The component report does not separately emit this count.'
    },
    M_productionRender7200: {
      passed: true, report: reportDirectory + 'rollback4x-responsive-text-render-01-report.json',
      scope: textRender.scope, identity: textRender.identity, frames: textRender.frames,
      browser: textRender.browser, renderers: textRender.renderers,
      checkpointCount: renderChecks.length, checks: renderChecks,
      finalSdl: textRender.checks.at(-1).map(check => sdlCounters(check.sdl)),
      note: 'Two frozen production releases, public spectator feed/tick exports, pause and results coverage. Exact PNG/hash/session equality at 73 checkpoints; no dev exports or rollback transport, no timed performance claim.'
    },
    N_sessionComponent: {
      report: reportDirectory + 'session-report.json', ...componentEvidence(session),
      fixture: sessionFixture,
      scopeDetail: 'Session contract fixtures include continuous actual-input frontier, out-of-order holes, replay frontier, reset/clock handling and CPU-slow-but-input-complete pacing. Component scope only; no production 4x pacing or presentation acceptance.'
    }
  },
  retainedCoverageFailures: retainedFailures,
  progress: requestedExtensions.has('--include-progress') ? progressEvidence() : { requested: false, flag: '--include-progress' },
  hostAbba: requestedExtensions.has('--include-host-abba') ? hostAbbaEvidence() : { requested: false, flag: '--include-host-abba' },
  paired: requestedExtensions.has('--include-paired') ? pairedEvidence() : { requested: false, flag: '--include-paired' },
  pairedControls: requestedExtensions.has('--include-paired-controls') ? { requested: true, runs: [pairedEvidence('mm')] } : { requested: false, flag: '--include-paired-controls' },
  pairedActive: requestedExtensions.has('--include-paired-active') ? { requested: true, runs: [pairedEvidence('mm-active'), pairedEvidence('mp-active')] } : { requested: false, flag: '--include-paired-active' },
  networkBurst: requestedExtensions.has('--include-network-burst') ? networkBurstEvidence() : { requested: false, flag: '--include-network-burst' },
  pendingEvidence: [
    'Paired phase-cost results are included only with --include-paired; same-build control only with --include-paired-controls. Activated-page controls and live network-recovery comparisons are separate conditions.',
    'The old M smoke has no missing-frame packet correlation; its counterpart-wire-observation to local-native-application interval cannot be narrowed using new runs.',
    'A recorded stable host interval remains necessary to assess throughput gains; no existing game run is filtered by host load.'
  ],
  rawReports: manifest
};

const serialized = JSON.stringify(evidence, null, 2) + '\n';
mkdirSync(dirname(resolve(workspace, outputPath)), { recursive: true });
writeFileSync(resolve(workspace, outputPath), serialized);
console.log(JSON.stringify({
  passed: true, output: outputPath, bytes: Buffer.byteLength(serialized),
  sha256: sha256(serialized), rawReports: manifest.length,
  confirmedTracesEqual: true, perPeerTapeFrames: tapes.map(tape => tape.frames),
  commonOrdinaryFramesP2: evidence.abba.exactOrdinaryFrameComparisons[1].count,
  performanceBenefit: evidence.verdict.performanceBenefit
}, null, 2));

