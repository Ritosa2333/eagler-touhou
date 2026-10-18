// Offline only: node mobile-gap-dispatch-summary.mjs OUTPUT.json RELEASE_EVIDENCE.json
import {readFileSync, writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
const array = x => Array.isArray(x) ? x : [], finite = Number.isFinite, integer = Number.isInteger;
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const sum = xs => xs.reduce((a, b) => a + b, 0), sameTime = (a, b) => finite(a) && finite(b) && Math.abs(a - b) <= 1e-6;
function read(path) {
  try {const bytes = readFileSync(path); return {path, bytes: bytes.length, sha256: sha(bytes), buffer: bytes};}
  catch (error) {return {path, error: String(error)};}
}
const identity = ({buffer, ...metadata}) => metadata;
function indexed(rows) {
  const map = new Map(), duplicates = [];
  for (const row of rows) {if (map.has(row.index)) duplicates.push(row.index); else map.set(row.index, row);}
  return {map, duplicates};
}
function union(intervals) {
  const result = [];
  for (const row of intervals.slice().sort((a, b) => a.from - b.from || a.to - b.to)) {
    const last = result.at(-1);
    if (last && row.from <= last.to) {last.to = Math.max(last.to, row.to); last.requestIndices.push(row.requestIndex);}
    else result.push({from: row.from, to: row.to, requestIndices: [row.requestIndex]});
  }
  return result.map(row => ({...row, milliseconds: row.to - row.from}));
}
function peerSummary(peer, side) {
  const choices = array(peer?.worstGaps).flatMap((gap, index) => gap?.kind === 'sampledAt' ? [{gap, index}] : []);
  const g = choices[0]?.gap, raf = g?.raf, frames = array(raf?.frames), requests = array(raf?.requests);
  const sourceSamples = array(g?.samples), diagnostic = peer?.rafDiagnostics, issues = [];
  const out = {side, sourcePeerSide: peer?.side ?? null, sourceGapIndex: choices[0]?.index ?? null,
    sourceGapKinds: array(peer?.worstGaps).map(gap => gap?.kind ?? null), gap: g?.gap ?? null,
    localPerformanceMs: g?.localPerformanceMs ?? null, sourceSamples, sourceRafFrames: frames,
    sourceRafRequests: requests, rafClock: raf?.clock, rafValidity: raf?.valid,
    rafDiagnostics: diagnostic ?? null, controls: raf?.controls ?? null,
    lastControlBeforeContext: raf?.lastControlBeforeContext ?? null, lifecycle: peer?.lifecycle ?? null};
  if (choices.length !== 1) issues.push('Expected exactly one sampledAt maximum');
  if (peer?.available !== true || peer?.side !== side) issues.push('Missing or mismatched peer');
  const [from, to] = array(g?.localPerformanceMs), a = g?.gap?.fromSample, b = g?.gap?.toSample;
  const samples = indexed(sourceSamples), req = indexed(requests), fs = indexed(frames);
  const validBounds = finite(from) && finite(to) && to > from && integer(a) && integer(b) && b > a;
  if (!validBounds || !sameTime(to - from, g?.gap?.sampledAtMs)) issues.push('Invalid same-page gap bounds');
  if (samples.duplicates.length || req.duplicates.length || fs.duplicates.length) issues.push('Duplicate retained indices');
  const first = samples.map.get(a)?.sample, last = samples.map.get(b)?.sample;
  if (!array(first).length || !array(last).length || !sameTime(last?.[0] - first?.[0], to - from))
    issues.push('Endpoint samples do not establish the same sampledAt interval');
  const native = sourceSamples.filter(p => integer(p.index) && p.index > a && p.index <= b)
    .map(p => ({sampleIndex: p.index, cppCallbackElapsedMs: p.sample?.[2]})).sort((x, y) => x.sampleIndex - y.sampleIndex);
  const nativeComplete = validBounds && native.length === b - a && native.every((r, i) =>
    r.sampleIndex === a + i + 1 && finite(r.cppCallbackElapsedMs) && r.cppCallbackElapsedMs >= 0);
  if (!nativeComplete) issues.push('Incomplete destination callback costs');
  const origin = finite(from) && finite(first?.[0]) ? from - first[0] : null;
  const association = native.map(n => {
    const matches = frames.filter(f => f.sampleBefore === n.sampleIndex && f.sampleAfter === n.sampleIndex + 1);
    const sample = samples.map.get(n.sampleIndex)?.sample, f = matches[0];
    return {sampleIndex: n.sampleIndex, frameIndices: matches.map(f => f.index), rafIndex: f?.rafIndex ?? null,
      qualified: matches.length === 1 && integer(f?.rafIndex) && finite(origin) &&
        sameTime(f.sampleAt, origin + sample?.[0]) && sameTime(f.cppWorkMs, n.cppCallbackElapsedMs)};
  });
  if (association.some(a => !a.qualified)) issues.push('Incomplete sample-to-frame association');
  if (raf?.valid !== true || diagnostic?.valid !== true || diagnostic?.overflow !== false ||
      ['rafHookCurrent', 'cancelHookCurrent', 'frameHookCurrent'].some(k => diagnostic?.[k] !== true) ||
      !Array.isArray(diagnostic?.hookErrors) || diagnostic.hookErrors.length)
    issues.push('Missing or invalid RAF hook coverage');
  const groups = new Map();
  for (const frame of frames) {const id = frame.rafIndex; if (!groups.has(id)) groups.set(id, []); groups.get(id).push(frame);}
  const derived = [...groups].map(([id, owned]) => {
    const r = req.map.get(id), parent = req.map.get(r?.parentIndex), reasons = [];
    if (issues.length) reasons.push(...issues);
    if (!integer(id) || id < 0 || !r || !integer(r.parentIndex) || r.parentIndex < 0 || !parent) reasons.push('Missing indexed request or parent');
    for (const [name, row] of [['request', r], ['parent', parent]]) {
      if (!row || row.cancelledAt !== null || row.requestThrew !== false || row.callbackThrew !== false)
        reasons.push(name + ' cancellation/exception state is not clean');
      if (!row || ![row.requestedAt, row.enteredAt, row.exitedAt].every(finite) ||
          row.requestedAt > row.enteredAt || row.enteredAt > row.exitedAt)
        reasons.push(name + ' timestamps are unavailable or out of order');
      if (row?.frameHookAtEntry !== true || row?.frameHookAtExit !== true)
        reasons.push(name + ' frame hook identity is not verified');
    }
    if (r && parent && !(parent.enteredAt <= r.requestedAt && r.requestedAt <= parent.exitedAt &&
        parent.exitedAt <= r.enteredAt)) reasons.push('Child request is not bracketed by a completed parent');
    if (owned.some(f => f.threw !== false || ![f.enteredAt, f.exitedAt].every(finite) ||
        !r || f.enteredAt < r.enteredAt || f.exitedAt < f.enteredAt || f.exitedAt > r.exitedAt))
      reasons.push('Game-frame callback identity, timestamps or exception state is not clean');
    const eligibleAt = r && parent && finite(r.requestedAt) && finite(parent.exitedAt)
      ? Math.max(r.requestedAt, parent.exitedAt) : null;
    const qualified = reasons.length === 0, low = qualified ? Math.max(from, eligibleAt) : null;
    const high = qualified ? Math.min(to, r.enteredAt) : null;
    return {requestIndex: id ?? null, callbackId: r?.callbackId ?? null, parentIndex: r?.parentIndex ?? null,
      frameIndices: owned.map(f => f.index), sampleBeforeIndices: owned.map(f => f.sampleBefore),
      requestedAt: r?.requestedAt ?? null, parentExitedAt: parent?.exitedAt ?? null,
      enteredAt: r?.enteredAt ?? null, eligibleAt, qualified, unknownReasons: reasons,
      eligibleWaitMs: qualified ? r.enteredAt - eligibleAt : null,
      clippedEligibleInterval: qualified && high > low ? [low, high] : null,
      clippedEligibleWaitMs: qualified ? Math.max(0, high - low) : null,
      couldIntersectGapByRequestBounds: validBounds && (!finite(r?.enteredAt) || r.enteredAt > from) &&
        (!finite(r?.requestedAt) || r.requestedAt < to)};
  });
  const known = derived.filter(r => r.qualified), unknown = derived.filter(r => !r.qualified);
  const merged = union(known.flatMap(r => r.clippedEligibleInterval ? [{requestIndex: r.requestIndex,
    from: r.clippedEligibleInterval[0], to: r.clippedEligibleInterval[1]}] : []));
  const complete = !issues.length && derived.length > 0 && !unknown.length;
  return {...out, qualified: complete, issues, sampleFrameAssociations: association, requests: derived,
    eligibleWaitUnion: {coverage: complete ? 'qualified-retained-game-requests' : known.length ? 'partial-known-intervals' : 'unknown',
      milliseconds: known.length ? sum(merged.map(r => r.milliseconds)) : null, intervals: merged,
      qualifiedRequests: known.length, unknownRequests: unknown.length,
      unknownPossiblyIntersectingGap: unknown.filter(r => r.couldIntersectGapByRequestBounds).map(r => r.requestIndex)},
    wholeCppCallbackElapsed: {sampleRangeExclusiveInclusive: [a ?? null, b ?? null], complete: nativeComplete,
      count: native.length, milliseconds: nativeComplete ? sum(native.map(r => r.cppCallbackElapsedMs)) : null, callbacks: native},
    counterDeltas: first && last ? {next: last[3] - first[3], corrections: last[5] - first[5],
      resimulated: last[6] - first[6], captures: last[7] - first[7], rendererSubmissions: last[11] - first[11]} : null};
}
const [output, inputPath, ...extra] = process.argv.slice(2);
if (!output || !inputPath || extra.length) throw Error('Usage: node mobile-gap-dispatch-summary.mjs OUTPUT.json RELEASE_EVIDENCE.json');
const selfPath = fileURLToPath(import.meta.url), selfBefore = read(selfPath), inputBefore = read(inputPath);
let input = null, inputError = inputBefore.error ?? null;
try {if (inputBefore.buffer) input = JSON.parse(inputBefore.buffer);} catch (error) {inputError = String(error);}
const sourceRuns = array(input?.runs), issues = [];
if (inputError) issues.push(inputError);
if (input?.schema !== 'th09-mobile-release-summary/v1' || sourceRuns.length !== 6) issues.push('Expected all six release evidence runs');
const runs = sourceRuns.map((run, index) => {
  const peers = array(run.summaries);
  return {sourceRunIndex: index, requestedPath: run.requestedPath, raw: run.raw, identity: run.identity,
    runLabel: run.invocation?.record?.runLabel ?? null, parameters: run.parameters, declaredPassed: run.declaredPassed, verificationPassed: run.verificationPassed,
    sourceChecks: run.checks, error: run.error ?? run.extractionError ?? null, errors: run.errors,
    retainedPeerCount: peers.length, peers: Array.from({length: Math.max(2, peers.length)}, (_, side) => {
      try {return peerSummary(peers[side], side);} catch (error) {return {side, qualified: false, extractionError: String(error)};}
    })};
});
const inputAfter = read(inputPath), selfAfter = read(selfPath);
const sourceStable = !!inputBefore.sha256 && inputBefore.sha256 === inputAfter.sha256 &&
  !!selfBefore.sha256 && selfBefore.sha256 === selfAfter.sha256;
if (!sourceStable) issues.push('Input or analyzer identity changed or could not be checked');
const evidence = {schema: 'th09-mobile-gap-dispatch-summary/v1',
  analyzer: {before: identity(selfBefore), after: identity(selfAfter)},
  input: {before: identity(inputBefore), after: identity(inputAfter), sourceSchema: input?.schema, sourceAnalyzer: input?.analyzer},
  sourceStable, issues, runs, priorAttempts: input?.priorAttempts ?? null,
  methods: {
    selection: 'Every supplied run and peer retained. Select the unique sampledAt maximum of formalWithEndBoundary new states; do not substitute maximum RAF timestamp.',
    request: 'Game identity comes from retained onGameFrame records linked by rafIndex. Parent record, clean request/callback flags, verified hooks and ordered finite Page performance.now timestamps are required.',
    waiting: 'Eligible start is max(requestedAt,parent.exitedAt). Clip [eligibleAt,enteredAt] to the same-page absolute sampledAt gap; merge overlaps once. This observes registered, parent-returned RAF waiting, not a guaranteed browser deadline or OS, thread, GPU or network causality.',
    coverage: 'Only retained game-frame-linked requests are enumerated. Unknown records remain and are never zero-filled. Pause fields are requested states, not native state reads; controls and hook/error records are retained.',
    native: 'Sum whole sample[2] callback costs for (fromSample,toSample], separately from wait union. Includes synchronous bridges and may contain zero/multiple ticks; no wall-minus-native residual or scheduler attribution is computed.',
    clock: 'sampledAt is a post-native observation, not observer return or physical scanout. No cross-page, relay or host clock subtraction.'
  }};
evidence.qualified = !issues.length && runs.every(r => r.retainedPeerCount === 2 &&
  r.declaredPassed === true && r.verificationPassed === true && r.peers.every(p => p.qualified));
writeFileSync(output, JSON.stringify(evidence, null, 2) + '\n', {flag: 'wx'});
let stdout = JSON.stringify({output, sourceStable, qualified: evidence.qualified, issueCount: issues.length,
  runs: runs.map(r => ({sourceRunIndex: r.sourceRunIndex, runLabel: r.runLabel, peers: r.peers.map(p => ({
    side: p.side, qualified: p.qualified, gapMs: p.gap?.sampledAtMs,
    eligibleWaitUnionMs: p.eligibleWaitUnion?.milliseconds, wholeCppCallbackMs: p.wholeCppCallbackElapsed?.milliseconds,
    unknownRequests: p.eligibleWaitUnion?.unknownRequests, issueCount: p.issues?.length ?? null, error: !!p.extractionError}))}))});
if (Buffer.byteLength(stdout) > 8000) stdout = JSON.stringify({output, qualified: evidence.qualified,
  detail: 'Console summary exceeded 8000 bytes; complete records and errors are preserved in the output file.'});
console.log(stdout);
if (!evidence.qualified) process.exitCode = 1;
