// Offline reader only. Usage: node journal-live-summary.mjs output.json report1.json [report2.json ...]
// No browser, build, raw-report mutation, or inferred A/B labels.
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { basename, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';

const SCHEMA = 'th09/journal-live-summary/3';
// Declare the comparison condition before reading results; never infer it from a run.
const EXPECTED_DELAY_MS = Number(process.env.JOURNAL_EXPECTED_DELAY_MS ?? 39);
if (!Number.isInteger(EXPECTED_DELAY_MS) || EXPECTED_DELAY_MS < 0 || EXPECTED_DELAY_MS > 2000)
  throw Error('JOURNAL_EXPECTED_DELAY_MS must be an integer in [0, 2000]');
const WINDOW_SCHEMA = 'continuous-prediction-frontier-v1';
const finite = n => typeof n === 'number' && Number.isFinite(n);
const uint = n => Number.isInteger(n) && n >= 0;
const array = value => Array.isArray(value) ? value : [];
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);
const issue = (reasons, code, details = {}) => reasons.push({ code, ...details });
const equal = (reasons, field, actual, expected) => {
  if (!same(actual, expected)) issue(reasons, 'unexpected-field', { field, expected, actual: actual ?? null });
};
const countKeys = ['plannedEvents', 'dueEvents', 'futureCensoredEvents', 'plannedNativeEvents', 'dueNativeEvents',
  'submittedEvents', 'rawNativeCalls', 'coalescedMoves', 'notSubmitted', 'attributedCaptures',
  'renderProgressObservedEvents', 'submittedWithoutAttributableChangedWire', 'packets', 'duplicates',
  'changedHistory', 'captureGaps', 'actualInputFrames', 'analogActiveFrames', 'targetChanges',
  'newFrontierCallbacks', 'newPresentedCallbacks', 'forwardFrames', 'positionChangedObservations'];
const distribution = (values, unit = 'ms') => {
  const sorted = values.filter(finite).sort((a, b) => a - b);
  return { unit, count: sorted.length, nonFiniteValues: values.length - sorted.length,
    total: sorted.reduce((sum, n) => sum + n, 0),
    p50: sorted[Math.floor(sorted.length * .5)] ?? null,
    p95: sorted[Math.floor(sorted.length * .95)] ?? null,
    p99: sorted[Math.floor(sorted.length * .99)] ?? null, max: sorted.at(-1) ?? null,
    ...(unit === 'ms' ? {
      over25ms: sorted.filter(n => n > 25).length, over50ms: sorted.filter(n => n > 50).length,
      over100ms: sorted.filter(n => n > 100).length, over250ms: sorted.filter(n => n > 250).length } : {}) };
};
const frequency = d => uint(d?.count) && finite(d?.total) && d.total > 0 ? d.count * 1000 / d.total : null;

function inputRows(rows) {
  const statuses = {};
  for (const event of rows) statuses[event?.status ?? 'missing-status'] = (statuses[event?.status ?? 'missing-status'] ?? 0) + 1;
  const presented = rows.filter(e => e?.status === 'render-progress-after-conditional-wire');
  const difference = (a, b) => presented.map(e => finite(e?.[a]) && finite(e?.[b]) ? e[a] - e[b] : null);
  return { events: rows.length, nativeEvents: rows.filter(e => e?.kind !== 'marker').length,
    submitted: rows.filter(e => finite(e?.submittedAtMs)).length,
    submittedNative: rows.filter(e => e?.kind !== 'marker' && finite(e?.submittedAtMs)).length,
    coalesced: statuses['coalesced-before-submit'] ?? 0, notSubmitted: statuses['not-submitted'] ?? 0,
    statuses, conditionalRendered: presented.length,
    sourceLatenessMs: distribution(rows.filter(e => finite(e?.submittedAtMs)).map(e => finite(e?.atMs) ? e.submittedAtMs - e.atMs : null)),
    conditionalResponseMs: { plannedToRender: distribution(difference('presentationObservedAtMs', 'atMs')),
      submitToFirstWire: distribution(difference('firstWireAtMs', 'submittedAtMs')),
      firstWireToRender: distribution(difference('presentationObservedAtMs', 'firstWireAtMs')),
      submitToRender: distribution(difference('presentationObservedAtMs', 'submittedAtMs')) } };
}

function summarizeTouch(result, bounds, limit, reasons) {
  const p = result?.touchInput, events = array(p?.events), future = array(p?.futureEvents);
  equal(reasons, 'touchInput.valid', p?.valid, true);
  for (const [field, expected] of [['profile', 'joystick'], ['pattern', 'continuous'], ['sourceHz', 120]])
    equal(reasons, 'touchInput.configuration.' + field, p?.configuration?.[field], expected);
  if (!Array.isArray(p?.events) || !Array.isArray(p?.futureEvents)) issue(reasons, 'touch-event-denominator-unavailable');
  for (const [key, value] of Object.entries(p?.overflow ?? {}))
    if (value !== 0) issue(reasons, 'touch-overflow', { key, value });
  if (array(p?.errors).length) issue(reasons, 'touch-errors', { errors: p.errors });
  const counts = Object.fromEntries(countKeys.map(key => [key, p?.counts?.[key] ?? null]));
  for (const key of countKeys) if (!uint(counts[key])) issue(reasons, 'touch-count-unavailable', { key, value: counts[key] });
  equal(reasons, 'touchInput.counts.dueEvents', counts.dueEvents, events.length);
  equal(reasons, 'touchInput.counts.futureCensoredEvents', counts.futureCensoredEvents, future.length);
  if ([counts.plannedEvents, counts.dueEvents, counts.futureCensoredEvents].every(uint))
    equal(reasons, 'touchInput.planned-denominator', counts.plannedEvents, counts.dueEvents + counts.futureCensoredEvents);
  if ([counts.dueEvents, counts.submittedEvents, counts.coalescedMoves, counts.notSubmitted].every(uint))
    equal(reasons, 'touchInput.due-denominator', counts.dueEvents, counts.submittedEvents + counts.coalescedMoves + counts.notSubmitted);
  for (const [key, minimum] of [['analogActiveFrames', 100], ['targetChanges', 50], ['positionChangedObservations', 20]])
    if (!(counts[key] >= minimum)) issue(reasons, 'touch-load-not-exercised', { key, minimum, actual: counts[key] });
  for (const key of ['changedHistory', 'captureGaps']) equal(reasons, 'touchInput.counts.' + key, counts[key], 0);
  const captures = array(p?.captures), tapeErrors = [];
  if (!uint(limit) || captures.length < limit) tapeErrors.push({ code: 'missing-frame-prefix', frames: captures.length, expected: limit ?? null });
  if (uint(limit)) for (let i = 0; i < Math.min(limit, captures.length); ++i) {
    const row = captures[i];
    if (row?.frame !== i || typeof row?.rawHex !== 'string' || !/^[0-9a-f]{24}$/i.test(row.rawHex))
      tapeErrors.push({ code: 'invalid-tape-frame', index: i, frame: row?.frame ?? null });
  }
  const computedTapeSha256 = !tapeErrors.length && uint(limit)
    ? sha(JSON.stringify(captures.slice(0, limit).map(c => [c.frame, c.rawHex]))) : null;
  if (tapeErrors.length) issue(reasons, 'touch-tape-invalid', { errors: tapeErrors });
  if (computedTapeSha256) equal(reasons, 'touchInputTapeSha256', result?.touchInputTapeSha256, computedTapeSha256);
  const allObserved = inputRows(events);
  equal(reasons, 'touchInput.submitted-event-count', counts.submittedEvents, allObserved.submitted);
  equal(reasons, 'touchInput.rendered-event-count', counts.renderProgressObservedEvents, allObserved.conditionalRendered);
  equal(reasons, 'touchInput.coalesced-event-count', counts.coalescedMoves, allObserved.coalesced);
  equal(reasons, 'touchInput.not-submitted-event-count', counts.notSubmitted, allObserved.notSubmitted);
  equal(reasons, 'touchInput.due-native-event-count', counts.dueNativeEvents, allObserved.nativeEvents);
  equal(reasons, 'touchInput.actual-input-frame-count', counts.actualInputFrames, captures.length);
  let body = null;
  if (bounds && finite(result?.measure?.started) && finite(p?.clock?.startedAt)) {
    const from = result.measure.started + bounds.start[0] - p.clock.startedAt;
    const to = result.measure.started + bounds.end[0] - p.clock.startedAt;
    const due = events.filter(e => finite(e?.atMs) && e.atMs >= from && e.atMs < to);
    const censored = future.filter(e => finite(e?.atMs) && e.atMs >= from && e.atMs < to);
    body = { deadlineRangeMs: [from, to], ...inputRows(due),
      sourceCoversBodyEnd: finite(p?.clock?.endedAt) && p.clock.endedAt >= result.measure.started + bounds.end[0],
      futureCensoredInsideBody: censored.length,
      submittedInsideBody: due.filter(e => finite(e?.submittedAtMs) && e.submittedAtMs >= from && e.submittedAtMs < to).length,
      submittedBeforeBody: due.filter(e => finite(e?.submittedAtMs) && e.submittedAtMs < from).length,
      submittedAtOrAfterBodyEnd: due.filter(e => finite(e?.submittedAtMs) && e.submittedAtMs >= to).length,
      scope: 'All retained due-event deadlines in the Page body interval. Eventual conditional response may occur after the body. This is not a capture-frame success subset.' };
    if (!body.sourceCoversBodyEnd || censored.length) issue(reasons, 'touch-source-does-not-cover-body-end',
      { sourceCoversBodyEnd: body.sourceCoversBodyEnd, futureCensoredInsideBody: censored.length });
  }
  if (!body) issue(reasons, 'touch-body-denominator-unavailable', { detail: 'Page body bounds or source clock not provided' });
  const invalidTimes = events.filter(e => !finite(e?.atMs) || (e?.submittedAtMs !== undefined && !finite(e.submittedAtMs))).map(e => e?.sequence ?? null);
  if (invalidTimes.length) issue(reasons, 'touch-event-time-invalid', { sequences: invalidTimes });
  return { valid: p?.valid ?? null, counts, overflow: p?.overflow ?? null, errors: p?.errors ?? null,
    clock: p?.clock ?? null, source: p ? { wakes: p.source?.wakes ?? null, maxWakeCommands: p.source?.maxWakeCommands ?? null,
      sourceExhausted: p.source?.sourceExhausted ?? null, latenessMs: p.source?.latenessMs ?? null } : null,
    declaredTapeSha256: result?.touchInputTapeSha256 ?? null, computedTapeSha256, tapeErrors,
    allObserved, body, bodyUnavailableReason: body ? null : 'Page body bounds or source clock not provided',
    rawResponse: p?.response ?? null };
}

function summarizePeer(report, side, limit) {
  const reasons = [], result = report.results?.[side], s = report.summary?.[side], w = s?.window;
  if (!result) issue(reasons, 'peer-result-unavailable', { side });
  equal(reasons, 'measurementEndpointQualification.qualified', report.measurementEndpointQualification?.[side]?.qualified, true);
  equal(reasons, 'route', result?.route, 'relay');
  equal(reasons, 'window.schema', w?.schema, WINDOW_SCHEMA);
  for (const field of ['complete', 'nativeFrontierComplete', 'epochValid'])
    equal(reasons, 'window.' + field, w?.[field], true);
  equal(reasons, 'window.progressSource', w?.progressSource, 'native-networkPacing-PredictionFrontier');
  equal(reasons, 'window.requestedFrameRange', w?.requestedFrameRange, [180, limit]);
  const samples = array(result?.measure?.samples), pacing = array(result?.measure?.networkPacing);
  const details = array(result?.measure?.frameDetails), start = w?.startSampleIndex, end = w?.endSampleIndexExclusive;
  const indicesOkay = uint(start) && uint(end) && start < end && end < samples.length;
  const bounds = indicesOkay && Array.isArray(samples[start]) && Array.isArray(samples[end])
    ? { start: samples[start], end: samples[end] } : null;
  if (!bounds) issue(reasons, 'body-sample-bounds-unavailable', { start: start ?? null, end: end ?? null, samples: samples.length });
  else equal(reasons, 'window.bodyRange', w?.bodyRange, [start, end]);
  if (!finite(s?.logicHz) || !finite(s?.elapsedMs) || !(s.elapsedMs > 0))
    issue(reasons, 'summary-progress-unavailable', { logicHz: s?.logicHz ?? null, elapsedMs: s?.elapsedMs ?? null });
  for (const key of ['callbacks', 'waitCallbacks', 'corrections', 'resimulated', 'captures'])
    if (!uint(s?.[key])) issue(reasons, 'summary-counter-unavailable', { key, value: s?.[key] ?? null });
  if (bounds) equal(reasons, 'summary.callbacks', s?.callbacks, end - start);
  if (!uint(s?.callbackWorkMs?.count) || !['p50', 'p95', 'p99', 'max'].every(key => finite(s?.callbackWorkMs?.[key])))
    issue(reasons, 'native-callback-distribution-unavailable');
  else equal(reasons, 'native-callback-count', s.callbackWorkMs.count, s?.callbacks);
  const continuityErrors = [];
  if (pacing.length !== samples.length) continuityErrors.push({ code: 'native-frontier-row-count', samples: samples.length, pacing: pacing.length });
  for (let i = 0; i < samples.length; ++i) {
    const row = samples[i], native = pacing[i], prior = samples[i - 1];
    if (!Array.isArray(row) || row.length < 14 || ![0, 1, 2, 3, 5, 6, 7, 11, 12, 13].every(k => finite(row[k]))) {
      continuityErrors.push({ code: 'sample-invalid', index: i }); continue;
    }
    if (!Array.isArray(native) || native.length < 7 || native[0] !== i || native[1] !== 1 ||
        !native.slice(2, 7).every(finite) || !uint(native[5]) || native[5] < row[3])
      continuityErrors.push({ code: 'native-frontier-row-invalid', index: i });
    if (i && Array.isArray(prior)) {
      for (const col of [0, 5, 6, 7, 11, 13]) if (finite(prior[col]) && row[col] < prior[col])
        continuityErrors.push({ code: 'counter-or-clock-regression', index: i, column: col });
      if (finite(native?.[5]) && finite(pacing[i - 1]?.[5]) && native[5] < pacing[i - 1][5])
        continuityErrors.push({ code: 'native-frontier-regression', index: i });
    }
  }
  if (continuityErrors.length) issue(reasons, 'raw-continuity-errors', { errors: continuityErrors });
  const saveErrors = [];
  if (details.length !== samples.length) saveErrors.push({ code: 'save-row-count', details: details.length, samples: samples.length });
  for (let i = 0; i < details.length; ++i) {
    if (!Array.isArray(details[i]) || details[i].length < 2 || !uint(details[i][1]) || details[i][0] !== samples[i]?.[3])
      saveErrors.push({ code: 'save-row-invalid', index: i });
    else if (i && uint(details[i - 1]?.[1]) && details[i][1] < details[i - 1][1])
      saveErrors.push({ code: 'save-counter-regression', index: i });
  }
  if (saveErrors.length) issue(reasons, 'world-save-count-unavailable', { errors: saveErrors });
  const savesOkay = bounds && !saveErrors.length;
  const bodyPacing = bounds ? pacing.slice(start, end) : [];
  const newStates = w?.segments?.body?.newStates ?? null;
  for (const clock of ['rafTimestampMs', 'sampledAtMs']) {
    const d = newStates?.[clock];
    if (!uint(d?.count) || d.count === 0 || !finite(d?.total) || !['p50', 'p95', 'p99', 'max'].every(k => finite(d[k])) ||
        !['over50ms', 'over100ms', 'over250ms'].every(k => uint(d[k])))
      issue(reasons, 'new-state-distribution-unavailable', { clock });
  }
  if (typeof result?.renderer !== 'string' || /swiftshader|llvmpipe|microsoft basic render/i.test(result.renderer))
    issue(reasons, 'native-renderer-unavailable', { renderer: result?.renderer ?? null });
  if (report.parameters?.rafDiagnostics) equal(reasons, 'rafDiagnostics.valid', result?.rafDiagnostics?.valid, true);
  const touch = summarizeTouch(result, bounds, limit, reasons);
  return { side, qualified: reasons.length === 0, reasons,
    measurementEndpointQualification: report.measurementEndpointQualification?.[side] ?? null,
    route: result?.route ?? null, routeEvidence: result?.routeEvidence ?? null, renderer: result?.renderer ?? null,
    confirmedHashes: result?.confirmedHashes ?? null, endpointHash: result?.hash ?? null, endpointStats: result?.stats ?? null,
    postDrain: result?.postDrain ?? null, window: w ? { schema: w.schema, status: w.status,
      complete: w.complete, nativeFrontierComplete: w.nativeFrontierComplete, epochValid: w.epochValid,
      requestedFrameRange: w.requestedFrameRange, bodyRange: w.bodyRange, progressSource: w.progressSource,
      nativeFrontierMissingSampleIndices: w.nativeFrontierMissingSampleIndices, nativeFrontierErrors: w.nativeFrontierErrors,
      epochErrors: w.epochErrors, startMarker: w.startMarker, endMarker: w.endMarker,
      formalWithEndBoundary: w.formalWithEndBoundary, crossingNewStateIntervals: w.crossingNewStateIntervals } : null,
    logicHz: s?.logicHz ?? null, newStateHzRaf: frequency(newStates?.rafTimestampMs),
    newStateHzPage: frequency(newStates?.sampledAtMs), elapsedMs: s?.elapsedMs ?? null,
    newStates, nativeCallbackWorkMs: s?.callbackWorkMs ?? null, callbacks: s?.callbacks ?? null,
    waitCallbacks: s?.waitCallbacks ?? null, corrections: s?.corrections ?? null,
    resimulated: s?.resimulated ?? null, inputCaptures: s?.captures ?? null, sessionPhases: s?.sessionPhases ?? null,
    worldSaves: { endpointDelta: savesOkay ? details[end][1] - details[start][1] : null,
      bodyCallbackDelta: savesOkay && start > 0 ? details[end - 1][1] - details[start - 1][1] : null,
      scope: 'endpointDelta shares the summary counter boundaries. bodyCallbackDelta aligns with [start,end) callback costs. These count actual WorldState saves, not input captures.' },
    networkPacing: { outstanding: distribution(bodyPacing.map(row => row?.[2]), 'frames'),
      networkIntervalScale: distribution(bodyPacing.map(row => finite(row?.[3]) ? row[3] / 1000 : null), 'ratio'),
      effectiveIntervalScale: distribution(bodyPacing.map(row => finite(row?.[4]) ? row[4] / 1000 : null), 'ratio'),
      replayDebt: distribution(bodyPacing.map(row => finite(row?.[5]) && finite(samples[row?.[0]]?.[3]) ? row[5] - samples[row[0]][3] : null), 'frames') },
    touch, rafDiagnostics: result?.rafDiagnostics ? { valid: result.rafDiagnostics.valid, overflow: result.rafDiagnostics.overflow,
      counts: result.rafDiagnostics.counts, hookErrors: result.rafDiagnostics.hookErrors } : null };
}

function summarizeRun(path, order) {
  const source = { path: resolve(path), label: basename(path), order, bytes: null, sha256: null };
  let report;
  try { const bytes = readFileSync(source.path); source.bytes = bytes.length; source.sha256 = sha(bytes); report = JSON.parse(bytes.toString('utf8')); }
  catch (error) { return { source, qualified: false, reasons: [{ code: 'raw-read-or-parse-failed', error: String(error) }], peers: [] }; }
  if (!report || typeof report !== 'object' || Array.isArray(report))
    return { source, qualified: false, reasons: [{ code: 'raw-report-object-unavailable' }], peers: [] };
  const reasons = [], p = report.parameters, limit = p?.limit;
  equal(reasons, 'passed', report.passed, true);
  if (array(report.errors).length || report.error) issue(reasons, 'reported-runtime-errors', { errors: report.errors ?? null, error: report.error ?? null });
  const fields = [['release', true], ['headedControl', true], ['fallback', true], ['impairmentSite', 'relay'],
    ['delay', EXPECTED_DELAY_MS], ['jitter', 5], ['dropEvery', 0], ['burstAt', 0], ['burstFor', 0], ['burstDelay', 0],
    ['inputMode', 'wall-clock'], ['profile', false], ['liveCost', false]];
  for (const [key, expected] of fields) equal(reasons, 'parameters.' + key, p?.[key], expected);
  equal(reasons, 'parameters.cpuThrottle.ratesBySide', p?.cpuThrottle?.ratesBySide, [4, 4]);
  equal(reasons, 'parameters.cpuThrottle.configuredAfter', p?.cpuThrottle?.configuredAfter, 'openProbe');
  for (const [key, expected] of [['enabled', true], ['profile', 'joystick'], ['pattern', 'continuous'], ['sourceHz', 120], ['mode', 0], ['inputDelay', 0]])
    equal(reasons, 'parameters.touchInput.' + key, p?.touchInput?.[key], expected);
  const frameLimitValid = uint(limit) && limit >= 300 && limit <= 18000;
  if (!frameLimitValid) issue(reasons, 'frame-limit-invalid', { limit: limit ?? null });
  equal(reasons, 'headedControl.identityVerified', report.headedControl?.identityVerified, true);
  const launches = array(report.headedControl?.launches);
  equal(reasons, 'headedControl.launches.length', launches.length, 2);
  for (let side = 0; side < 2; ++side) {
    equal(reasons, 'headedControl.launches[' + side + '].verified', launches[side]?.verified, true);
    if (!uint(launches[side]?.browserPid) || launches[side].browserPid === 0)
      issue(reasons, 'headed-browser-pid-unavailable', { side });
  }
  if (launches.length === 2 && launches[0]?.browserPid === launches[1]?.browserPid)
    issue(reasons, 'headed-browser-pids-not-distinct');
  for (const key of ['wasm', 'loader', 'harness', 'smoothnessWindow', 'touchInputProbe', 'relay'])
    if (typeof report.identity?.[key] !== 'string' || !/^[0-9a-f]{64}$/i.test(report.identity[key]))
      issue(reasons, 'artifact-identity-unavailable', { key, value: report.identity?.[key] ?? null });
  const relay = report.relayTransport;
  equal(reasons, 'relayTransport.valid', relay?.valid, true);
  equal(reasons, 'relayTransport.coverage.exercised', relay?.coverage?.exercised, true);
  equal(reasons, 'relayTransport.coverage.connectionHookExercised', relay?.coverage?.connectionHookExercised, true);
  for (const [field, key] of [['relaySha256', 'relay'], ['probeSha256', 'relayTransportProbe'], ['fixtureSha256', 'relayTransportFixture'], ['startupProbeSha256', 'relayStartupProbe']])
    if (!report.identity?.[key] || !relay?.sourceIdentity?.[field]) issue(reasons, 'relay-identity-unavailable', { field, key });
    else equal(reasons, 'relayTransport.sourceIdentity.' + field, relay.sourceIdentity[field], report.identity[key]);
  equal(reasons, 'results.length', array(report.results).length, 2);
  const peers = [0, 1].map(side => summarizePeer(report, side, limit));
  const requiredChecks = frameLimitValid ? Math.floor(limit / 120) : 0, confirmedErrors = [];
  for (let side = 0; side < 2; ++side) {
    const trace = array(report.results?.[side]?.confirmedHashes);
    if (!requiredChecks || trace.length < requiredChecks) confirmedErrors.push({ code: 'confirmed-prefix-incomplete', side, actual: trace.length, required: requiredChecks });
    for (let i = 0; i < Math.min(requiredChecks, trace.length); ++i)
      if (!Array.isArray(trace[i]) || trace[i][0] !== (i + 1) * 120 || !uint(trace[i][1]))
        confirmedErrors.push({ code: 'confirmed-point-invalid', side, index: i, point: trace[i] ?? null });
  }
  for (let i = 0; i < requiredChecks; ++i) if (!same(report.results?.[0]?.confirmedHashes?.[i], report.results?.[1]?.confirmedHashes?.[i]))
    confirmedErrors.push({ code: 'confirmed-peer-mismatch', index: i });
  if (confirmedErrors.length) issue(reasons, 'confirmed-hash-qualification-failed', { errors: confirmedErrors });
  if (report.finalComparable === true && !same(report.results?.[0]?.hash, report.results?.[1]?.hash))
    issue(reasons, 'comparable-endpoint-hash-mismatch');
  return { source, qualified: reasons.length === 0 && peers.every(peer => peer.qualified), reasons, peers,
    reportStatus: { passed: report.passed ?? null, passScope: report.passScope ?? null, error: report.error ?? null, errors: report.errors ?? null },
    identity: report.identity ?? null, parameters: p ?? null, graphicsIdentity: report.graphicsIdentity ?? null,
    headedControl: report.headedControl ?? null, browsers: report.browsers ?? null,
    teardownClean: report.teardownClean ?? null, sameFrame: report.sameFrame ?? null, finalComparable: report.finalComparable ?? null,
    confirmedHashQualification: { qualified: !confirmedErrors.length, requiredChecks, errors: confirmedErrors },
    relay: relay ? { valid: relay.valid, coverage: relay.coverage, sourceIdentity: relay.sourceIdentity,
      counts: relay.counts ?? null, overflow: relay.overflow ?? null, errors: relay.errors ?? null } : null,
    hostLoad: report.hostLoad ? { scope: 'Whole retained run, not per-gap causal attribution.', summary: report.hostLoad.summary ?? null,
      source: 'See unchanged raw hostLoad for retained samples and known-process context.' } : null };
}

const args = process.argv.slice(2);
if (args.length < 2) throw Error('Usage: node journal-live-summary.mjs output.json report1.json [report2.json ...]');
const output = resolve(args[0]), inputs = args.slice(1).map(path => resolve(path));
const pathKey = path => process.platform === 'win32' ? path.toLowerCase() : path;
if (existsSync(output)) throw Error('Refusing to overwrite existing output: ' + output);
if (inputs.some(path => pathKey(path) === pathKey(output))) throw Error('Output must not be an input report');
const runs = inputs.map(summarizeRun);
const controls = ['loader', 'harness', 'smoothnessWindow', 'gpuVsyncControl', 'touchInputProbe', 'touchInputSchedules',
  'relay', 'shell', 'presentationHarness', 'headedBrowserExecutable', 'hostLoadProbe', 'chromeSchedulerTrace'];
const matching = controls.map(key => {
  const values = runs.map(run => run.identity?.[key] ?? null);
  return { key, allProvided: values.every(value => value !== null), same: values.every(value => same(value, values[0])), values };
});
const ownPath = fileURLToPath(import.meta.url), ownBytes = readFileSync(ownPath);
const summary = { schema: SCHEMA, createdAt: new Date().toISOString(),
  summarizer: { path: ownPath, bytes: ownBytes.length, sha256: sha(ownBytes) },
  method: { window: 'Use the driver summary/window verbatim; validate required structure and raw continuity without redefining window selection.',
    qualifications: 'Observed report/endpoint/hash/input evidence and declared conditions only. No FPS acceptance threshold is invented.',
    requiredCondition: `Release; two headed native-GPU desktop processes; CPU rates [4,4]; D0; relay ${EXPECTED_DELAY_MS} +/- 5 ms without burst/drop; independent Page-clock continuous 120 Hz joystick.`,
    declaredDelayMs: EXPECTED_DELAY_MS,
    externalBuildChecks: ['H8 is a compile-time constant, not independently exported by this report.', 'Release compiler macros and A/B source differences require frozen build identities.', 'Raw identity is not a before/after filesystem-drift audit.'],
    newStateFrequency: '1000 * body new-state gap count / gap total, separately for rAF and Page. logicHz counts prediction-frontier progress.',
    nativeCost: 'Existing native callback wall time, not CPU self time, whole JavaScript callback duration, or GPU completion.',
    input: 'Conditional changed-wire-to-new-state proxy only; firstWire is send intent, not delivery or exact capture. No physical touch, Launcher bridge, visible-effect or photon claim.',
    comparison: 'Run order is preserved without guessing A/B labels. Same source plans do not imply same captured tapes, world trajectories, packet delivery, browser scheduling or host load.',
    teardown: 'teardownClean is retained separately. False does not automatically become a performance failure or erase qualified workload evidence.',
    failures: 'All inputs and all discovered disqualification reasons are retained; failed or incomplete runs are never filtered out. Exit 0 means the summary was written, not that runs qualified.' },
  runCount: runs.length, qualifiedRunCount: runs.filter(run => run.qualified).length,
  performanceAcceptance: 'Not automatically assessed; review all runs and long-gap tails.',
  crossRunControlIdentity: matching, runs };
writeFileSync(output, JSON.stringify(summary, null, 2) + '\n', { flag: 'wx' });
console.log(JSON.stringify({ output, schema: SCHEMA, runCount: runs.length, qualifiedRunCount: summary.qualifiedRunCount,
  runs: runs.map(run => ({ label: run.source.label, rawSha256: run.source.sha256, wasm: run.identity?.wasm ?? null,
    qualified: run.qualified, reasonGroupCount: run.reasons.length + run.peers.reduce((n, p) => n + p.reasons.length, 0),
    teardownClean: run.teardownClean ?? null, peers: run.peers.map(p => ({ side: p.side, qualified: p.qualified,
      logicHz: p.logicHz, newStateHzRaf: p.newStateHzRaf, rafP99Ms: p.newStates?.rafTimestampMs?.p99 ?? null,
      rafMaxMs: p.newStates?.rafTimestampMs?.max ?? null, pageMaxMs: p.newStates?.sampledAtMs?.max ?? null,
      rafOver50: p.newStates?.rafTimestampMs?.over50ms ?? null, pageOver50: p.newStates?.sampledAtMs?.over50ms ?? null,
      nativeP99Ms: p.nativeCallbackWorkMs?.p99 ?? null, saves: p.worldSaves.endpointDelta,
      corrections: p.corrections, resimulated: p.resimulated, touchDue: p.touch.counts.dueEvents,
      touchSubmitted: p.touch.counts.submittedEvents, touchCoalesced: p.touch.counts.coalescedMoves,
      touchNotSubmitted: p.touch.counts.notSubmitted })) })) }));
