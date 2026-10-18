// Offline: node mobile-release-evidence-summary.mjs OUTPUT.json SIX_REPORT_PATHS...
import {readFileSync, writeFileSync, existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {summarizeSmoothnessWindow} from './smoothness-window.mjs';
const root = fileURLToPath(new URL('../../../', import.meta.url));
const json = JSON.stringify, finite = Number.isFinite, array = x => Array.isArray(x) ? x : [];
const equal = (a, b) => json(a) === json(b);
const sha = x => createHash('sha256').update(x).digest('hex');
const sum = xs => xs.reduce((a, b) => a + b, 0);
const helperSha = sha(readFileSync(new URL('./smoothness-window.mjs', import.meta.url)));
const dist = xs => {
  if (!xs.every(finite)) return {count: xs.length, invalidValues: xs.filter(x => !finite(x)).length};
  const a = xs.slice().sort((x, y) => x - y);
  return {count: a.length, total: sum(a), p50: a[Math.floor(a.length * .5)] ?? null,
    p99: a[Math.floor(a.length * .99)] ?? null, max: a.at(-1) ?? null};
};
function read(path, parse = true) {
  try {
    const bytes = readFileSync(path), value = parse ? JSON.parse(bytes) : bytes.toString('utf8');
    return {path, sha256: sha(bytes), bytes: bytes.length, value};
  } catch (error) {return {path, error: String(error)};}
}
function artifact(file) {
  const {value, ...metadata} = file;
  return metadata;
}
function invocation(path) {
  const file = read(path), value = file.value, log = value?.logPath ? read(value.logPath, false) : null;
  return {...artifact(file), record: value ?? null, log: log ? {...artifact(log),
    matchesRecordedSha: log.sha256 === value.logSha256,
    firstFailureText: value.status !== 0 ? log.value?.slice(0, 4096) ?? null : null} : null};
}
function priorAttempts() {
  const file = read(resolve(root, 'docs/multiplayer/rollback-mobile-run-corrections.json'));
  return {...artifact(file), record: file.value ?? null, attempts: array(file.value?.attempts).map(a => ({
    ...a, retained: array(a.retained).map(p => p.endsWith('-invocation.json')
      ? invocation(resolve(root, p)) : (() => {const f = read(resolve(root, p), false);
        return {...artifact(f), firstFailureText: f.value?.slice(0, 4096) ?? null};})())
  }))};
}
function host(h) {
  if (!h) return {available: false};
  const cpu = array(h.cpuSamples), ps = array(h.processSamples), processes = new Map();
  const good = cpu.filter(x => !x.counterDiscontinuity &&
    [x.busyFraction, x.deltaTotalMs, x.deltaIdleMs, x.intervalMs].every(finite) &&
    x.busyFraction >= 0 && x.busyFraction <= 1 && x.intervalMs > 0 &&
    x.deltaTotalMs > 0 && x.deltaIdleMs >= 0 && x.deltaIdleMs <= x.deltaTotalMs);
  let previous = new Map();
  ps.forEach((s, index) => {
    const current = new Map(array(s.rows).map(r => [json(r.slice(0, 2)), r]));
    for (const [key, row] of current) {
      const p = processes.get(key) ?? {pid: row[0], name: row[1], observations: 0,
        deltaCpuSeconds: 0, pairedSourceMs: 0, resets: 0, unknownPairs: 0, appearances: [], absences: []};
      p.observations++; p.first ??= [index, s.sourceUtcMs, row[2]]; p.last = [index, s.sourceUtcMs, row[2]];
      if (!previous.has(key)) p.appearances.push(index);
      else {
        const before = previous.get(key), dt = s.sourceUtcMs - ps[index - 1].sourceUtcMs;
        if (!s.failed && !ps[index - 1].failed && finite(row[2]) && finite(before[2]) && dt > 0) {
          if (row[2] >= before[2]) {p.deltaCpuSeconds += row[2] - before[2]; p.pairedSourceMs += dt;}
          else p.resets++;
        } else p.unknownPairs++;
      }
      processes.set(key, p);
    }
    for (const key of previous.keys()) if (!current.has(key)) processes.get(key).absences.push(index);
    previous = current;
  });
  const total = sum(good.map(x => x.deltaTotalMs));
  return {available: true, valid: h.valid, complete: h.complete,
    startedAtUtcMs: h.startedAtUtcMs, endedAtUtcMs: h.endedAtUtcMs, snapshotAtMs: h.snapshotAtMs,
    counts: h.counts, overflow: h.overflow,
    errors: h.errors, marks: h.marks, knownProcesses: h.knownProcesses, processSampler: h.processSampler,
    cpu: {samples: cpu.length, legalIntervals: good.length, first: cpu[0], last: cpu.at(-1),
      weightedBusyFraction: total ? sum(good.map(x => x.deltaTotalMs - x.deltaIdleMs)) / total : null},
    processes: [...processes.values()], processSnapshots: ps.map(({rows, ...s}, index) =>
      ({index, ...s, retainedRows: array(rows).length})),
    scope: 'Host-wide counters, not per-call CPU attribution. Process deltas require adjacent same pid/name.',
    limitations: h.limitations};
}
function touch(t, recordedSha, limit) {
  if (!t) return {available: false};
  const events = array(t.events), future = array(t.futureEvents), captures = array(t.captures);
  const statuses = {};
  for (const e of [...events, ...future]) statuses[e.status] = (statuses[e.status] ?? 0) + 1;
  const presented = events.filter(e => e.status === 'render-progress-after-conditional-wire');
  const {coalescedRanges, ...source} = t.source ?? {};
  const response = {
    plannedToRenderProgressMs: dist(presented.map(e => e.presentationObservedAtMs - e.atMs)),
    submitToFirstWireMs: dist(presented.map(e => e.firstWireAtMs - e.submittedAtMs)),
    firstWireToRenderProgressMs: dist(presented.map(e => e.presentationObservedAtMs - e.firstWireAtMs))};
  const valid = captures.every((c, i) => {
    if (c.frame !== i || !/^[0-9a-f]{24}$/i.test(c.rawHex ?? '')) return false;
    const b = Buffer.from(c.rawHex, 'hex');
    return c.word0 === b.readUInt32LE(0) && c.buttons === b.readUInt16LE(0) &&
      c.analogMode === b[2] && c.flags === b[3] && c.xBits === b.readUInt32LE(4) &&
      c.yBits === b.readUInt32LE(8) && finite(c.x) && finite(c.y) &&
      c.x === b.readFloatLE(4) && c.y === b.readFloatLE(8);
  });
  const tapeSha = sha(json(captures.slice(0, limit).map(c => [c.frame, c.rawHex])));
  const recounted = {plannedEvents: events.length + future.length, dueEvents: events.length,
    futureCensoredEvents: future.length, submittedEvents: events.filter(e => e.submittedAtMs !== undefined).length,
    coalescedMoves: statuses['coalesced-before-submit'] ?? 0, notSubmitted: statuses['not-submitted'] ?? 0,
    renderProgressObservedEvents: presented.length, actualInputFrames: captures.length};
  return {available: true, valid: t.valid, clock: t.clock, configuration: t.configuration,
    counts: t.counts, recounted, countsMatch: Object.keys(recounted).every(k => recounted[k] === t.counts?.[k]),
    statuses, overflow: t.overflow, errors: t.errors, wireObservation: t.wireObservation,
    source: {...source, coalescedRangeCount: array(coalescedRanges).length,
      firstCoalescedRange: coalescedRanges?.[0], lastCoalescedRange: coalescedRanges?.at(-1)},
    tape: {full12BFieldsValid: valid, completePrefix: valid && captures.length >= limit,
      prefixFrames: Math.min(captures.length, limit), sha256: tapeSha, recordedSha256: recordedSha,
      shaMatches: tapeSha === recordedSha, allRowsSha256: sha(json(captures.map(c => [c.frame, c.rawHex]))),
      duplicatesReported: t.counts?.duplicates, changedHistoryReported: t.counts?.changedHistory,
      captureGapsReported: t.counts?.captureGaps,
      scope: 'Rechecks retained unique 12B rows; duplicate packet comparisons remain probe-reported.'},
    response: t.response, conditionalRecomputed: response,
    conditionalResponseMatches: Object.keys(response).every(k =>
      ['count', 'p50', 'p99', 'max'].every(field => response[k][field] === t.response?.[k]?.[field])),
    largestConditionalResponses: presented.slice().sort((a, b) =>
      (b.presentationObservedAtMs - b.atMs) - (a.presentationObservedAtMs - a.atMs)).slice(0, 3),
    futureBounds: [future[0] ?? null, future.at(-1) ?? null]};
}
function calibration(clock) {
  const pairs = [clock?.started, clock?.stopped, clock?.snapshot].filter(Boolean);
  const valid = pairs.length >= 2 && pairs.every((p, i) =>
    [p.before, p.after, p.epochMs, p.timeOrigin].every(finite) && p.after >= p.before &&
    p.timeOrigin === pairs[0].timeOrigin && (!i || p.before >= pairs[i - 1].before));
  if (!valid) return {available: false, clock, reason: 'Missing finite before/Date.now/after brackets'};
  const low = Math.max(...pairs.map(p => p.epochMs - 1 - p.after));
  const high = Math.min(...pairs.map(p => p.epochMs + 1 - p.before));
  return {available: low <= high, clock, fixedOffsetCompatibleAtObservedAnchors: low <= high,
    offsetMs: [low, high], coveredPerformanceMs: [Math.min(...pairs.map(p => p.after)),
      Math.max(...pairs.map(p => p.before))],
    scope: 'Observed-anchor consistency only; assumes Date.now within +/-1 ms and fixed offset between anchors.'};
}
function convert(epoch, proof) {
  if (!proof.available || !epoch) return null;
  const r = [epoch[0] - proof.offsetMs[1], epoch[1] - proof.offsetMs[0]];
  return r[0] >= proof.coveredPerformanceMs[0] && r[1] <= proof.coveredPerformanceMs[1] ? r : null;
}
function traceSlice(trace, span, frames) {
  if (!trace) return {available: false};
  const columns = trace.columns, ec = columns?.event ?? [], pc = columns?.packet ?? [];
  const entered = ec.indexOf('enteredAt'), exited = ec.indexOf('exitedAt');
  const first = pc.indexOf('firstInputFrame'), count = pc.indexOf('inputCount');
  const candidates = new Set(array(trace.packets).filter(p => first >= 0 && count >= 0 &&
    p[count] > 0 && p[first] < frames[1] && p[first] + p[count] > frames[0]).map(p => p[0]));
  const rows = array(trace.events).filter(e => span && entered >= 0 && exited >= 0
    ? e[entered] <= span[1] && e[exited] >= span[0] : candidates.has(e[2]));
  const kept = rows.slice(0, 2000), ids = new Set(kept.map(e => e[2]));
  return {available: true, selection: span ? 'same-clock interval including +/-100 ms context'
      : 'input-frame overlap only; contemporaneity unknown', span, frames, matchedRows: rows.length,
    retainedRows: kept.length, truncated: rows.length > kept.length, clock: trace.clock, columns,
    valid: trace.valid, coverage: trace.coverage, counts: trace.counts, overflow: trace.overflow,
    errors: trace.errors, events: kept, packets: array(trace.packets).filter(p => ids.has(p[0]))};
}
const point = (v, index) => ({index, sample: v.rows[index], nativePacing: v.pacing.get(index) ?? null,
  frameDetails: v.peer.measure?.frameDetails?.[index] ?? null});
function gapContext(views, side, gap, kind, relay) {
  if (!gap) return null;
  const v = views[side], other = views[1 - side], a = gap.fromSample, b = gap.toSample;
  const local = kind === 'sampledAt' ? [v.peer.measure.started + v.rows[a][0],
    v.peer.measure.started + v.rows[b][0]] : [v.rows[a][13], v.rows[b][13]];
  const span = [local[0] - 100, local[1] + 100], proof = calibration(v.peer.networkTransport?.clock);
  const covered = proof.available && span[0] >= proof.coveredPerformanceMs[0] &&
    span[1] <= proof.coveredPerformanceMs[1];
  const epoch = covered ? [span[0] + proof.offsetMs[0], span[1] + proof.offsetMs[1]] : null;
  const otherProof = calibration(other?.peer.networkTransport?.clock), relayProof = calibration(relay?.clock);
  const otherSpan = convert(epoch, otherProof), relaySpan = convert(epoch, relayProof);
  const frames = [v.pacing.get(a)?.[5] ?? v.rows[a][3], v.pacing.get(b)?.[5] ?? v.rows[b][3]];
  const ownRows = v.rows.slice(Math.max(0, a - 2), b + 3).map((_, i) => point(v, Math.max(0, a - 2) + i));
  const otherRows = other ? other.rows.flatMap((s, i) => {
    const t = other.peer.measure.started + s[0], f = other.pacing.get(i)?.[5];
    return (otherSpan ? t >= otherSpan[0] && t <= otherSpan[1] :
      finite(f) && f >= frames[0] && f <= frames[1]) ? [point(other, i)] : [];
  }) : [];
  const d = v.peer.rafDiagnostics, rafFrames = array(d?.frames).filter(f =>
    f.sampleBefore >= Math.max(0, a - 2) && f.sampleBefore <= b + 2);
  const ids = new Set(rafFrames.map(f => f.rafIndex));
  for (const r of array(d?.requests)) if (ids.has(r.index)) ids.add(r.parentIndex);
  return {kind, gap, localPerformanceMs: local, epochEnvelopeWithContextMs: epoch,
    clockQualification: {page: proof, otherPage: otherProof, relay: relayProof},
    samples: ownRows, otherSamples: otherRows,
    frameAssociationSource: finite(v.pacing.get(a)?.[5]) && finite(v.pacing.get(b)?.[5])
      ? 'native frontier interval [from,to)' : 'observed next only; native frontier unavailable',
    otherSampleSelection: otherSpan ? 'clock-bracket-compatible neighborhood' : 'frontier-frame context, not simultaneous',
    raf: {clock: d?.clock, valid: d?.valid, frames: rafFrames,
      requests: array(d?.requests).filter(r => ids.has(r.index)),
      controls: array(d?.controls).filter(r => r.enteredAt >= span[0] && r.enteredAt <= span[1]),
      lastControlBeforeContext: array(d?.controls).filter(r => r.enteredAt < span[0]).at(-1)},
    localTransport: traceSlice(v.peer.networkTransport, span, frames),
    otherTransport: traceSlice(other?.peer.networkTransport, otherSpan, frames),
    relayTransport: traceSlice(relay, relaySpan, frames)};
}
function anomaly(peer, enabled) {
  const a = peer.rafAnomaly;
  if (!a) return {status: enabled === false ? 'disabled' : enabled ? 'enabled-evidence-missing' : 'not-recorded'};
  const {history, ...rest} = a;
  return {...rest, history: {retainedJsonBytes: history?.retainedJsonBytes,
    pre: {count: array(history?.pre).length, firstClock: history?.pre?.[0]?.clock,
      lastClock: history?.pre?.at(-1)?.clock},
    post: {count: array(history?.post).length, firstClock: history?.post?.[0]?.clock,
      lastClock: history?.post?.at(-1)?.clock}}};
}
function peerSummary(raw, views, side) {
  const v = views[side];
  if (!v?.peer?.measure) return {side, available: false};
  try {
    const s = summarizeSmoothnessWindow(v.peer, {startFrame: 180, endFrame: raw.parameters.limit});
    const [a, b] = s.window.bodyRange, classes = {}, indices = {};
    for (let i = a; i < b; i++) {
      const r = v.rows[i], p = v.rows[i - 1], f = v.pacing.get(i)?.[5];
      let k = 'unknown-predecessor-or-native-frontier';
      if (p && finite(f)) {
        const next = r[3] - p[3], captures = r[7] - p[7], render = r[11] - p[11];
        const corrections = r[5] - p[5], resim = r[6] - p[6];
        k = corrections < 0 || resim < 0 ? 'invalid-counter-delta' : corrections || resim ? 'recovery-mixed' :
          next === 1 && captures === 1 && render === 1 && r[3] === f ? 'fresh-single-step' :
          next === 0 && captures === 0 && render === 0 ? 'no-step-no-capture-no-render' : 'other';
      }
      (classes[k] ??= []).push(r[2]); (indices[k] ??= []).push(i);
    }
    const partition = Object.fromEntries(Object.entries(classes).map(([k, v]) => [k, dist(v)]));
    const closed = s.window.formalWithEndBoundary.newStates;
    const diagnostic = v.peer.rafDiagnostics;
    return {side, available: true, helperShaMatches: raw.identity?.smoothnessWindow === helperSha,
      summaryMatchesRecorded: equal(s, raw.summary?.[side]), summary: s,
      cppRafCallbackCostPartition: partition,
      partitionConserved: sum(Object.values(partition).map(p => p.count)) === s.callbacks &&
        Math.abs(sum(Object.values(partition).map(p => p.total)) - s.callbackWorkMs.total) < 1e-6,
      freshSingleStepSampleIndices: indices['fresh-single-step'] ?? [],
      stats: v.peer.stats, routeEvidence: v.peer.routeEvidence, renderer: v.peer.renderer, audio: v.peer.audio,
      repairsSentAtStop: v.peer.stats?.[5] ?? null,
      repairsDelta: s.window.closedBoundaryAvailable ? v.rows[b][8] - v.rows[a][8] : null,
      returnCode2WaitOrRecoveryCallbacks: s.waitCallbacks,
      touch: touch(v.peer.touchInput, v.peer.touchInputTapeSha256, raw.parameters.limit),
      worstGaps: [gapContext(views, side, closed.maximumSampledAtGap, 'sampledAt', raw.relayTransport),
        gapContext(views, side, closed.maximumRafGap, 'RAF timestamp', raw.relayTransport)],
      rafAnomaly: anomaly(v.peer, raw.parameters.rafAnomaly), lifecycle: v.peer.lifecycle,
      rafDiagnostics: {valid: diagnostic?.valid, overflow: diagnostic?.overflow, counts: diagnostic?.counts,
        lastPauseRequested: diagnostic?.lastPauseRequested, lastControl: array(diagnostic?.controls).at(-1),
        rafHookCurrent: diagnostic?.rafHookCurrent, cancelHookCurrent: diagnostic?.cancelHookCurrent,
        frameHookCurrent: diagnostic?.frameHookCurrent, hookErrors: diagnostic?.hookErrors}};
  } catch (error) {return {side, available: false, error: String(error), stats: v.peer.stats};}
}
function run(requestedPath) {
  const fallback = requestedPath.replace(/-report\.json$/, '-failure.json');
  const file = read(existsSync(requestedPath) ? requestedPath : existsSync(fallback) ? fallback : requestedPath);
  const inv = invocation(requestedPath.replace(/-(report|failure)\.json$/, '-invocation.json'));
  const raw = file.value, base = {requestedPath, raw: artifact(file), invocation: inv};
  if (!raw) return {...base, declaredPassed: false, verificationPassed: false};
  try {
    const peers = raw.results ?? raw.partial ?? [], limit = raw.parameters?.limit;
    const views = [0, 1].map(side => ({peer: peers[side] ?? {}, rows: array(peers[side]?.measure?.samples),
      pacing: new Map(array(peers[side]?.measure?.networkPacing).map(p => [p[0], p]))}));
    const summaries = [0, 1].map(side => peerSummary(raw, views, side)), required = Math.floor(limit / 120);
    const prefixes = peers.slice(0, 2).map(p => array(p?.confirmedHashes).slice(0, required));
    const validPrefix = p => p.length === required && p.every((h, i) => Array.isArray(h) && h.length === 2 &&
      h[0] === (i + 1) * 120 && Number.isInteger(h[1]) && h[1] >= 0 && h[1] <= 0xffffffff);
    const comparable = peers.length === 2 && peers[0]?.stats?.[0] === peers[1]?.stats?.[0] &&
      peers.every(p => p?.stats?.[1] >= p?.stats?.[0] && p?.stats?.[7] >= p?.stats?.[0]);
    const checks = {declaredPass: raw.passed === true, invocationExit0: inv.record?.status === 0,
      invocationSourcesStable: !!inv.record?.identityBefore &&
        equal(inv.record.identityBefore, inv.record.identityAfter) && !inv.record.identityError,
      harnessIdentity: inv.record?.scriptSha256 === raw.identity?.harness &&
        inv.record?.scriptAfterSha256 === raw.identity?.harness,
      logSha: inv.log?.matchesRecordedSha === true,
      confirmedPrefix: required > 0 && prefixes.length === 2 && prefixes.every(validPrefix) && equal(...prefixes),
      finalComparable: comparable === raw.finalComparable && (!comparable || peers[0].hash === peers[1].hash),
      windows: summaries.every(p => p.available && p.helperShaMatches && p.summaryMatchesRecorded &&
        p.summary.window.complete && p.partitionConserved),
      touch: summaries.every(p => p.touch?.valid && p.touch.countsMatch &&
        p.touch.tape.completePrefix && p.touch.tape.shaMatches && p.touch.conditionalResponseMatches)};
    const node = raw.rafAnomalyNodePolls, {rows: nodeRows, ...nodeMeta} = node ?? {};
    return {...base, declaredPassed: raw.passed === true, error: raw.error ?? null, errors: raw.errors,
      verificationPassed: Object.values(checks).every(Boolean), checks, identity: raw.identity,
      parameters: raw.parameters, browsers: raw.browsers, scope: raw.scope, summaries,
      confirmed: {required, traces: peers.map(p => p?.confirmedHashes ?? null),
        tails: peers.map(p => Math.max(0, array(p?.confirmedHashes).length - required)),
        finalComparable: comparable, endpointHashes: peers.map(p => p?.hash ?? null)},
      relay: raw.relayTransport ? {clock: raw.relayTransport.clock, injection: raw.relayTransport.injection,
        valid: raw.relayTransport.valid, coverage: raw.relayTransport.coverage, counts: raw.relayTransport.counts,
        overflow: raw.relayTransport.overflow, errors: raw.relayTransport.errors} : null,
      rafAnomalyNodePolls: node ? {...nodeMeta, rowCount: array(nodeRows).length,
        first: nodeRows?.[0], lastRow: nodeRows?.at(-1)} : null, host: host(raw.hostLoad)};
  } catch (error) {return {...base, declaredPassed: raw.passed === true, verificationPassed: false,
    error: raw.error ?? null, extractionError: String(error)};}
}
const [output, ...paths] = process.argv.slice(2);
if (!output || paths.length !== 6) throw Error('Provide OUTPUT.json and all six report paths in declared order');
const runs = paths.map(run), firstFour = runs.slice(0, 4);
const tapes = firstFour.map(r => r.summaries?.map(p => p.touch?.tape.sha256 ?? null) ?? null);
const evidence = {schema: 'th09-mobile-release-summary/v1',
  analyzer: {path: fileURLToPath(import.meta.url), sha256: sha(readFileSync(fileURLToPath(import.meta.url))), helperSha},
  runs, priorAttempts: priorAttempts(), abba: {
    runLabels: firstFour.map(r => r.invocation.record?.runLabel ?? null),
    rawParametersEqual: firstFour.every(r => r.parameters && equal(r.parameters, firstFour[0].parameters)),
    observationSourceIdentity: Object.fromEntries(['harness', 'smoothnessWindow', 'touchInputProbe',
      'shell', 'presentationHarness', 'relay', 'rafAnomalyProbe'].map(key => {
      const values = firstFour.map(r => r.identity?.[key] ?? null);
      return [key, {values, equal: values.every(v => v !== null) ? values.every(v => v === values[0]) : null}];
    })), inputTapeSha256: tapes,
    allActualInputTapesEqual: tapes.every(t => t?.length === 2 && t.every(Boolean)) &&
      tapes.every(t => equal(t, tapes[0])),
    interpretation: 'Independent wall-clock sources produce actual tapes; unequal tapes prohibit a matched-input CPU A/B claim.'},
  methods: {
    body: 'Frozen first-cross native-frontier [s,e). Every body callback cost retained; counters/Hz close e-s. Endpoint cost is tail.',
    work: 'sample[2] is whole C++ rAF callback elapsed, potentially zero/multiple ticks, synchronous send/audio/draw included. Not pure Tick/Restore CPU.',
    fresh: 'Adjacent next/capture/renderer deltas exactly 1, correction/resim deltas 0, next equals native frontier. Still whole callback elapsed.',
    repairs: 'stats[5] and sample[8] count RepairsSent. repairsDelta closes e-s. Return code 2 includes waiting/recovery; it does not prove network waiting.',
    clocks: 'sampledAt is post-native observation; RAF timestamps remain distinct, neither is physical scanout. Cross-process joins require compatible observed clock brackets covering the context; otherwise only input-frame association is retained.',
    touch: 'All planned/due/future/coalesced/unsubmitted denominators retained. Conditional wire-to-render progress does not prove movement or physical input response.',
    failure: 'Missing reports use their failure artifact if present. Earlier preflight failure remains separately with source record, invocation/log SHA and first failure text.'
  }};
writeFileSync(output, json(evidence, null, 2) + '\n', {flag: 'wx'});
console.log(json({output, analyzer: evidence.analyzer, runs: runs.map(r => ({
  requestedPath: r.requestedPath, rawSha: r.raw.sha256, passed: r.declaredPassed,
  verified: r.verificationPassed, failedChecks: Object.keys(r.checks ?? {}).filter(k => !r.checks[k]),
  error: r.error ?? r.extractionError ?? r.raw.error ?? null
})), abba: evidence.abba}));
if (runs.some(r => !r.verificationPassed)) process.exitCode = 1;

