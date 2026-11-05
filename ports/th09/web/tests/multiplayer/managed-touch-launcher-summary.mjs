// Offline summary of retained Launcher observations. No browser or game is run.
// Usage: node managed-touch-launcher-summary.mjs NEW_OUTPUT.json REPORT.json [...]
import {createHash} from 'node:crypto';
import {existsSync, readFileSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const EXPECTED_PHASES = ['baseline-gesture', 'hud-focus', 'hud-fire',
  'queue-request', 'queue-held', 'fresh-after-resource'];
const MOVEMENT_PHASES = new Set(['baseline-gesture', 'queue-held', 'fresh-after-resource']);
const COUNT_MAPS = ['pointerEvents', 'postSubmissions', 'childDeliveries', 'nativeCalls'];
const STATE_SCHEMAS = {
  title: ['frames', 'inTitle', 'screen', 'state', 'selection', 'loadFrame', 'leaving', 'requestedTransition'],
  session: ['phase', 'frames', 'replayFrame', 'replayLength', 'continues', 'stage', 'isReplay', 'recordable'],
  touchState: ['context', 'ready', 'active', 'inTitle'],
  touchProbe: ['positionX', 'positionY', 'velocityX', 'velocityY', 'motionEnabled', 'motionTarget', 'motionX', 'motionY'],
  rollback: ['frame', 'confirmedOrPublishedMax', 'corrections', 'resimulated', 'captures', 'repairsSent', 'ready', 'rollbackPublished'],
  gameMetrics: ['batches', 'uploadBytes', 'readBytes', 'vertexUploadBytes', 'programCompiles',
    'bufferReplacements', 'bufferSubUpdates', 'rendererFrames', 'resamples', 'presentations'],
};
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const list = value => Array.isArray(value) ? value : [];
const finite = value => typeof value === 'number' && Number.isFinite(value);
const stable = value => JSON.stringify(value ?? null, (_, item) =>
  item && typeof item === 'object' && !Array.isArray(item)
    ? Object.fromEntries(Object.keys(item).sort().map(key => [key, item[key]])) : item);
const digest = value => sha(Buffer.from(stable(value), 'utf8'));
const same = (a, b) => stable(a) === stable(b);
const increment = (object, key) => { object[key] = (object[key] || 0) + 1; };
const number = value => typeof value === 'number' ? value
  : value && typeof value === 'object' && Object.keys(value).length === 1 && Object.hasOwn(value, 'number')
    ? Number(value.number) : NaN;
const sourceNumber = value => value && typeof value === 'object' ? number(value) : Number(value);
const ref = row => {
  if (!row) return null;
  const {_rawIndex, ...fields} = row;
  return {rawRecordIndex: _rawIndex, ...fields};
};
function series(rows, project = value => value) {
  const values = rows.map(project);
  return {count: values.length, orderedJsonSha256: digest(values),
    first: values.slice(0, 3), last: values.slice(-3)};
}
function distribution(values) {
  const ordered = values.filter(value => finite(value) && value >= 0).sort((a, b) => a - b);
  const quantile = q => {
    if (!ordered.length) return null;
    const at = (ordered.length - 1) * q, lo = Math.floor(at), hi = Math.ceil(at);
    return ordered[lo] + (ordered[hi] - ordered[lo]) * (at - lo);
  };
  const sum = ordered.reduce((total, value) => total + value, 0);
  return {observations: values.length, valid: ordered.length, invalid: values.length - ordered.length,
    sum: ordered.length ? sum : null, mean: ordered.length ? sum / ordered.length : null,
    min: ordered[0] ?? null, p50: quantile(.5), p95: quantile(.95), p99: quantile(.99),
    max: ordered.at(-1) ?? null, greaterThan50: ordered.filter(value => value > 50).length,
    greaterThan100: ordered.filter(value => value > 100).length};
}
function counts(rows) {
  const result = {records: rows.length, callbacks: 0, frameSamples: 0};
  for (const key of COUNT_MAPS) result[key] = Object.create(null);
  for (const row of rows) {
    if (row.kind === 'pointer') increment(result.pointerEvents, row.type);
    if (row.kind === 'postMessage-entry') increment(result.postSubmissions, row.command);
    if (row.kind === 'message-observed') increment(result.childDeliveries, row.command);
    if (row.kind === 'native-entry') increment(result.nativeCalls, row.name);
    if (row.kind === 'frame-entry') result.callbacks++;
    if (row.kind === 'frame-after-original' && row.state) result.frameSamples++;
  }
  return result;
}
const countsMatch = (observed, declared) => !!declared &&
  ['records', 'callbacks', 'frameSamples', ...COUNT_MAPS].every(key => same(observed[key], declared[key]));
function snapshotMeta(probe) {
  if (!probe || typeof probe !== 'object') return {available: false};
  return {available: true, version: probe.version, epoch: probe.epoch, label: probe.label,
    observing: probe.observing, disposed: probe.disposed, observedAt: probe.observedAt,
    clocks: probe.clocks, owner: probe.owner, coverage: probe.coverage,
    counters: probe.counters, errors: probe.errors, hooks: probe.hooks,
    hooksAtDisposal: probe.hooksAtDisposal, gettersCurrent: probe.gettersCurrent,
    snapshotWork: probe.snapshotWork};
}
function inspectProbe(probe) {
  const rows = list(probe.records).map((row, index) => ({...row, _rawIndex: index}));
  const phases = list(probe.phases), phaseIds = new Set(phases.map(phase => phase.id));
  const observed = counts(rows), previousTime = {host: null, child: null};
  let clocksValid = true, phaseOrder = true, previousPhase = -1;
  for (const row of rows) {
    if (!['host', 'child'].includes(row.clock) || !finite(row.atMs) ||
        previousTime[row.clock] !== null && row.atMs < previousTime[row.clock]) clocksValid = false;
    previousTime[row.clock] = row.atMs;
    if (!Number.isSafeInteger(row.phaseId) || !phaseIds.has(row.phaseId) || row.phaseId < previousPhase)
      phaseOrder = false;
    previousPhase = row.phaseId;
  }
  const atEnd = probe.observedAt || {}, boundHooks = probe.disposed ? probe.hooksAtDisposal : probe.hooks;
  const checks = {
    versionKnown: probe.version === 1,
    schemaColumnsKnown: Object.entries(STATE_SCHEMAS).every(([name, columns]) => same(probe.schemas?.[name], columns)),
    recordsIncluded: Array.isArray(probe.records) && probe.coverage?.includedRecords === true,
    retainedLengthMatches: probe.coverage?.retained === rows.length,
    noOverwrite: probe.counters?.overwritten === 0 && probe.coverage?.overwritten === 0,
    allRecordedRowsRetained: probe.counters?.records === rows.length,
    contiguousSequence: rows.every((row, i) => row.seq === i + 1),
    coverageEndpointsMatch: (probe.coverage?.firstSeq ?? null) === (rows[0]?.seq ?? null) &&
      (probe.coverage?.lastSeq ?? null) === (rows.at(-1)?.seq ?? null),
    declaredRawComplete: probe.coverage?.rawComplete === true,
    noObservationErrors: probe.counters?.observationErrors === 0 && list(probe.errors).length === 0,
    noPayloadTruncation: probe.counters?.payloadTruncations === 0 &&
      rows.every(row => !row.payloadTruncated && !row.argsTruncated),
    countersMatch: countsMatch(observed, probe.counters),
    uniquePhases: phases.length > 0 && phaseIds.size === phases.length &&
      phases.every(phase => Number.isSafeInteger(phase.id) && typeof phase.name === 'string'),
    phaseOrder,
    phaseCountersMatch: phases.every(phase => countsMatch(counts(rows.filter(row => row.phaseId === phase.id)),
      list(probe.byPhase).find(row => row.id === phase.id))),
    clockOriginsPresent: ['host', 'child'].every(clock => finite(probe.clocks?.[clock]?.timeOrigin)),
    recordClockMonotonic: clocksValid,
    snapshotClockEndpointsValid: ['host', 'child'].every(clock =>
      finite(atEnd[clock + 'Ms']) && (previousTime[clock] === null || atEnd[clock + 'Ms'] >= previousTime[clock])),
    ownerValid: probe.owner?.valid === true && probe.owner?.invalidation === null &&
      !rows.some(row => row.kind === 'owner-invalid'),
    hooksCurrentAtObservationEnd: list(boundHooks).length > 0 && boundHooks.every(row => row.stillOurs === true),
    gettersCurrentAtObservationEnd: probe.gettersCurrent === true,
  };
  if (!probe.disposed) checks.declaredHooksCurrent = probe.coverage?.allHooksCurrent === true;
  return {rows, phases, checks, complete: Object.values(checks).every(Boolean),
    failedChecks: Object.keys(checks).filter(key => !checks[key]), observedCounters: observed};
}
function matchedExits(rows, entryKind, exitKind) {
  const entries = rows.filter(row => row.kind === entryKind), exits = rows.filter(row => row.kind === exitKind);
  const matches = entries.map(entry => ({entry, exits: exits.filter(exit => exit.callId === entry.callId)}));
  return {entries: entries.length, exits: exits.length,
    uniqueEntryIds: new Set(entries.map(row => row.callId)).size === entries.length,
    missingOrDuplicateExits: matches.filter(pair => pair.exits.length !== 1).length,
    orphanExits: exits.filter(exit => !entries.some(entry => entry.callId === exit.callId)).length,
    thrown: exits.filter(row => row.thrown).length,
    wallMs: distribution(exits.map(row => row.originalWallMs)),
    incompleteExamples: matches.filter(pair => pair.exits.length !== 1).slice(0, 3)
      .map(pair => ({entry: ref(pair.entry), exits: pair.exits.map(ref)}))};
}
function inputSummary(rows, movement, complete) {
  const pointers = rows.filter(row => row.kind === 'pointer');
  const posts = rows.filter(row => row.kind === 'postMessage-entry');
  const delivered = rows.filter(row => row.kind === 'message-observed');
  const natives = rows.filter(row => row.kind === 'native-entry');
  const byCommand = {};
  for (const command of new Set([...posts, ...delivered].map(row => row.command))) {
    const sent = posts.filter(row => row.command === command), got = delivered.filter(row => row.command === command);
    byCommand[command] = {submitted: sent.length, delivered: got.length,
      completeMessagesEqualInObservedOrder: same(sent.map(row => row.message), got.map(row => row.message)),
      submittedParameters: series(sent, row => row.message), deliveredParameters: series(got, row => row.message)};
  }
  const directPosts = posts.filter(row => row.command === 'direct-touch');
  const directCalls = natives.filter(row => row.name === '_th09_touch');
  const directExpected = directPosts.map(row => [({down: 0, move: 1, up: 2, cancel: 2})[row.message?.type] ?? 2,
    sourceNumber(row.message?.id) || 0]);
  const directActual = directCalls.map(row => list(row.args).slice(0, 2).map(number));
  const controlPosts = posts.filter(row => row.command === 'touch-controls');
  const controls = natives.filter(row => ['_th09_touch_controls', '_th09_touch_stick'].includes(row.name));
  const controlsShape = same(controls.map(row => row.name),
    controlPosts.flatMap(() => ['_th09_touch_controls', '_th09_touch_stick']));
  const controlPayloads = controlPosts.map(row => row.message?.controls || row.message);
  const pairs = controlPayloads.map((message, i) => ({message, control: controls[i * 2], stick: controls[i * 2 + 1]}));
  const controlsParameters = controlsShape && pairs.every(({message, control, stick}) =>
    list(control.args).length === 5 && list(stick.args).length === 2 &&
    [...control.args, ...stick.args].every(value => finite(number(value))) &&
    number(control.args[1]) === +!!message.fireEnabled && number(control.args[2]) === +!!message.focusEnabled &&
    number(stick.args[0]) === (sourceNumber(message.joystickX) || 0) &&
    number(stick.args[1]) === (sourceNumber(message.joystickY) || 0));
  const standalone = controls.filter((row, index) =>
    row.name === '_th09_touch_controls' && controls[index + 1]?.name !== '_th09_touch_stick');
  const firePosts = posts.filter(row => row.command === 'keyboard' && row.message?.code === 'KeyZ');
  const fireCalls = natives.filter(row => row.name === '_th09_key' && number(row.args?.[0]) === 44);
  const moves = pointers.filter(row => row.type === 'pointermove');
  const moveTimes = moves.map(row => row.atMs), nativeCosts = {};
  for (const name of new Set(natives.map(row => row.name))) {
    nativeCosts[name] = matchedExits(rows.filter(row => row.name === name), 'native-entry', 'native-exit');
  }
  const movementCommand = movement === 'touch' ? 'direct-touch' : movement === 'joystick-free' ? 'touch-controls' : null;
  const movementOrder = movement === 'touch'
    ? same(directExpected, directActual) && directCalls.every(row =>
      list(row.args).length === 4 && row.args.every(value => finite(number(value))))
    : movement === 'joystick-free' ? controlsParameters : false;
  return {counts: counts(rows), completeRetainedEvidence: complete,
    pointerEvents: series(pointers, ref), pointerTrusted: pointers.filter(row => row.isTrusted === true).length,
    pointerUntrusted: pointers.filter(row => row.isTrusted === false).length,
    pointerMoveHostClock: {count: moves.length, firstAtMs: moveTimes[0] ?? null, lastAtMs: moveTimes.at(-1) ?? null,
      adjacentGapMs: distribution(moveTimes.slice(1).map((time, i) => time - moveTimes[i])),
      endpointObservedHz: moves.length > 1 && moveTimes.at(-1) > moveTimes[0]
        ? (moves.length - 1) * 1000 / (moveTimes.at(-1) - moveTimes[0]) : null},
    byCommand,
    allCommandsEqualInObservedOrder: same(posts.map(row => row.message), delivered.map(row => row.message)),
    nativeArguments: series(natives, row => ({seq: row.seq, name: row.name, args: row.args})),
    nativeCallsByExport: nativeCosts, postMethodCalls: matchedExits(rows, 'postMessage-entry', 'postMessage-exit'),
    direct: {submissions: directPosts.length, calls: directCalls.length,
      typeAndIdEqualInOrder: same(directExpected, directActual),
      allFourArgumentsFinite: directCalls.every(row => list(row.args).length === 4 && row.args.every(value => finite(number(value)))),
      sourceTypeId: series(directExpected), nativeTypeId: series(directActual),
      xyTransformOracle: 'Not available: per-event canvas transform was not recorded; full native XY remains in argument evidence.'},
    controls: {submissions: controlPosts.length, callsIncludingStandalone: controls.length,
      exactControlsThenStickOrder: controlsShape, fireFocusAndStickArgumentsEqual: controlsParameters,
      nonzeroStickCalls: controls.filter(row => row.name === '_th09_touch_stick' &&
        list(row.args).some(value => number(value) !== 0)).length,
      standaloneControls: series(standalone, ref),
      serialAndEnabledScope: 'Native enabled and mapped serial values are retained; no first-sample serial mapping oracle is inferred.'},
    hudKeyZ: {submissions: firePosts.length, nativeScanCode44Calls: fireCalls.length,
      downUpOrderEqual: same(firePosts.map(row => +!!row.message.down), fireCalls.map(row => number(row.args?.[1]))),
      meaning: 'TH09 Fire is keyboard KeyZ (scan code 44), not controls.fireEnabled.'},
    activeMovementOrderQualified: complete && moves.length > 0 &&
      (byCommand[movementCommand]?.submitted ?? 0) >= 10 &&
      byCommand[movementCommand]?.completeMessagesEqualInObservedOrder === true && movementOrder,
    orderingScope: 'Observation-order comparison, not event-to-native latency or a causal transform oracle. A phase boundary may split a submission/delivery pair; the run-wide comparison is separate.'};
}
function validState(row) {
  const state = row?.state;
  return !!state && [['title', 8], ['session', 8], ['touchState', 4], ['rollback', 8], ['gameMetrics', 10]]
    .every(([key, length]) => Array.isArray(state[key]) && state[key].length === length &&
      state[key].every(Number.isSafeInteger)) &&
    Array.isArray(state.touchProbe?.values) && state.touchProbe.values.length === 8 && state.touchProbe.values.every(finite) &&
    Array.isArray(state.touchProbe?.bits) && state.touchProbe.bits.length === 8 && state.touchProbe.bits.every(Number.isSafeInteger);
}
function frameSummary(rows, probe, complete, lastPhaseId) {
  const entries = rows.filter(row => row.kind === 'frame-entry');
  const samples = rows.filter(row => row.kind === 'frame-after-original');
  const diagnostics = [], frames = [], sampleIds = new Set();
  for (const entry of entries) {
    const matches = samples.filter(row => row.callId === entry.callId), sample = matches.length === 1 ? matches[0] : null;
    const okay = !!sample && validState(sample) && !sample.thrown &&
      sample.clock === 'child' && entry.clock === 'child' && finite(entry.originalEnteredAtMs) &&
      finite(sample.sampledAtMs) && finite(sample.afterSampleAtMs) &&
      sample.seq > entry.seq && sample.sampledAtMs >= entry.originalEnteredAtMs &&
      sample.afterSampleAtMs >= sample.sampledAtMs;
    if (sample) sampleIds.add(sample.callId);
    frames.push({entry, sample, okay});
    if (!okay) diagnostics.push({kind: 'incomplete-callback-sample', entry: ref(entry), matches: matches.map(ref)});
  }
  for (const sample of samples) if (!sampleIds.has(sample.callId))
    diagnostics.push({kind: 'orphan-or-ambiguous-sample', sample: ref(sample)});
  if (new Set(entries.map(row => row.callId)).size !== entries.length)
    diagnostics.push({kind: 'duplicate-callback-id'});
  const observable = frames.filter(frame => frame.okay).map(frame => frame.sample);
  const frameById = new Map(frames.map(frame => [frame.entry.callId, frame]));
  const isHidden = row => row?.childHidden === true || row?.hostHidden === true;
  const barrierRows = rows.filter(row => row.kind === 'lifecycle' || row.kind === 'owner-invalid' || isHidden(row));
  const barriersBetween = (from, to) => barrierRows.filter(row => row.seq >= from.seq && row.seq <= to.seq);
  const sampleFor = row => row.kind === 'frame-entry' ? frameById.get(row.callId)?.sample : row;
  let previous = null, previousPresentation = null, previousNew = null, positionChanges = 0, bitChanges = 0;
  let newCount = 0, staticPresentations = 0, multiPresentationObservations = 0;
  const newGaps = [], segments = [], sessionPhases = {}, touchContexts = {}, positive = [];
  for (const frame of frames) {
    const sample = frame.sample;
    if (!frame.okay) {previous = previousPresentation = previousNew = null;continue;}
    const state = sample.state, currentFrame = state.session[1], currentPresent = state.gameMetrics[9];
    increment(sessionPhases, state.session[0]);increment(touchContexts, state.touchState.join(','));
    const discontinuity = previous && (sample.sampledAtMs < previous.sampledAtMs ||
      state.session[0] !== previous.state.session[0] || currentFrame < previous.state.session[1] ||
      currentPresent < previous.state.gameMetrics[9]);
    const barriers = barriersBetween(previous || sample, sample);
    if (discontinuity || barriers.length) {
      diagnostics.push({kind: discontinuity ? 'clock-session-or-presentation-discontinuity' : 'lifecycle-owner-or-hidden-boundary',
        from: ref(previous), to: ref(sample), barriers: series(barriers, ref),
        previousPositive: ref(previousNew),
        observedSincePreviousPositiveMs: previousNew ? sample.sampledAtMs - previousNew.sampledAtMs : null,
        scope: 'Censored observation boundary; this elapsed span is not a closed new-state gap.'});
      previous = previousPresentation = previousNew = null;
    }
    // A hidden sample remains in raw sample/range evidence, but cannot anchor a visible segment.
    if (isHidden(sample)) continue;
    if (!previous) segments.push({first: ref(sample), last: ref(sample), observations: 0, positiveNewStates: 0});
    const segment = segments.at(-1);segment.last = ref(sample);segment.observations++;
    if (previous) {
      if (state.touchProbe.values[0] !== previous.state.touchProbe.values[0] ||
          state.touchProbe.values[1] !== previous.state.touchProbe.values[1]) positionChanges++;
      if (state.touchProbe.bits[0] !== previous.state.touchProbe.bits[0] ||
          state.touchProbe.bits[1] !== previous.state.touchProbe.bits[1]) bitChanges++;
    }
    if (previousPresentation && currentPresent > previousPresentation.state.gameMetrics[9]) {
      const presentationDelta = currentPresent - previousPresentation.state.gameMetrics[9];
      if (presentationDelta > 1) multiPresentationObservations++;
      if (currentFrame > previousPresentation.state.session[1]) {
        if (previousNew) newGaps.push({from: previousNew, to: sample,
          ms: sample.sampledAtMs - previousNew.sampledAtMs,
          sessionFrameDelta: currentFrame - previousNew.state.session[1],
          presentationDelta: currentPresent - previousNew.state.gameMetrics[9]});
        positive.push(sample);newCount++;segment.positiveNewStates++;
        previousNew = sample;
      } else staticPresentations += presentationDelta;
      previousPresentation = sample;
    } else if (!previousPresentation) previousPresentation = sample;
    previous = sample;
  }
  const callbackGaps = entries.slice(1).map((to, i) => ({from: entries[i], to,
    ms: finite(to.originalEnteredAtMs) && finite(entries[i].originalEnteredAtMs)
      ? to.originalEnteredAtMs - entries[i].originalEnteredAtMs : null}));
  const hasBoundary = gap => barriersBetween(gap.from, gap.to).length > 0 ||
    isHidden(sampleFor(gap.from)) || isHidden(sampleFor(gap.to));
  const incompleteEndpoint = gap => [gap.from, gap.to].some(row =>
    row.kind === 'frame-entry' && !frameById.get(row.callId)?.okay);
  const intervalDetail = gap => {
    const inside = rows.filter(row => row.seq > gap.from.seq && row.seq <= gap.to.seq);
    const lifecycle = inside.filter(row => row.kind === 'lifecycle' || row.kind === 'owner-invalid');
    const invalid = diagnostics.filter(item => {
      const seq = item.entry?.seq ?? item.sample?.seq ?? item.to?.seq;
      return seq === undefined || seq >= gap.from.seq && seq <= gap.to.seq;
    });
    const hidden = inside.some(isHidden) || isHidden(sampleFor(gap.from)) || isHidden(sampleFor(gap.to));
    const costs = inside.filter(row => row.kind === 'frame-entry' && finite(row.cppRafCallbackWallMs))
      .sort((a, b) => b.cppRafCallbackWallMs - a.cppRafCallbackWallMs);
    return {elapsedMs: gap.ms, sessionFrameDelta: gap.sessionFrameDelta ?? null,
      presentationDelta: gap.presentationDelta ?? null, from: ref(gap.from), to: ref(gap.to),
      fromCallbackSample: gap.from.kind === 'frame-entry' ? ref(sampleFor(gap.from)) : null,
      toCallbackSample: gap.to.kind === 'frame-entry' ? ref(sampleFor(gap.to)) : null,
      completeRetainedInterval: gapComplete(gap),
      uninterruptedVisibleObservation: !hidden && lifecycle.length === 0,
      interveningObservationCounts: counts(inside), lifecycle: lifecycle.map(ref),
      incompleteSampleEvidence: series(invalid), worstCppCallbackObservedInInterval: ref(costs[0]),
      scope: 'Seq selects intervening observations only; callback cost is context and is not subtracted from the gap.'};
  };
  const gapComplete = gap => complete && finite(gap.ms) && gap.ms >= 0 &&
    !hasBoundary(gap) && !incompleteEndpoint(gap) && !diagnostics.some(item => {
      const seq = item.entry?.seq ?? item.sample?.seq ?? item.to?.seq;
      return seq === undefined || seq >= gap.from.seq && seq <= gap.to.seq;
    });
  const gaps = values => ({allRetainedAdjacentGapsMs: distribution(values.map(gap => gap.ms)),
    completeRetainedIntervals: values.filter(gapComplete).length,
    top3: values.filter(gap => finite(gap.ms) && gap.ms >= 0).sort((a, b) => b.ms - a.ms)
      .slice(0, 3).map(intervalDetail),
    invalidExamples: values.filter(gap => !finite(gap.ms) || gap.ms < 0).slice(0, 3).map(intervalDetail)});
  const lastSample = observable.at(-1), lastPositive = positive.at(-1), phaseId = rows[0]?.phaseId;
  const lastChild = rows.filter(row => row.clock === 'child' && finite(row.atMs)).at(-1);
  const useSnapshotEnd = phaseId === lastPhaseId && !probe.disposed && probe.observing === true &&
    probe.owner?.valid === true && finite(probe.observedAt?.childMs);
  const tailAt = useSnapshotEnd ? probe.observedAt.childMs : lastChild?.atMs ?? null;
  const lastPositiveStillContinuous = lastPositive && previousNew === lastPositive && lastSample &&
    lastPositive.state.session[0] === lastSample.state.session[0] &&
    !rows.some(row => row.seq > lastPositive.seq && (row.kind === 'owner-invalid' || row.kind === 'lifecycle' ||
      row.childHidden === true || row.hostHidden === true)) &&
    !diagnostics.some(item => (item.entry?.seq ?? item.sample?.seq ?? item.to?.seq ?? Infinity) > lastPositive.seq);
  const range = axis => {
    const values = observable.map(row => row.state.touchProbe.values[axis]);
    return {count: values.length, min: values.length ? Math.min(...values) : null,
      max: values.length ? Math.max(...values) : null};
  };
  return {callbackEntries: entries.length, sampledRows: samples.length, validSamplePairs: observable.length,
    incomplete: !complete || diagnostics.length > 0, discontinuities: series(diagnostics),
    sessionPhaseCounts: sessionPhases, touchContextReadyActiveTitleCounts: touchContexts,
    cppRafCallbackWallMs: distribution(entries.map(row => row.cppRafCallbackWallMs)),
    top3CppCallbacks: entries.filter(row => finite(row.cppRafCallbackWallMs))
      .sort((a, b) => b.cppRafCallbackWallMs - a.cppRafCallbackWallMs).slice(0, 3).map(ref),
    originalOnGameFrameJsWallMs: distribution(samples.map(row => row.originalJsWallMs)),
    diagnosticReadWallMs: distribution(samples.map(row => finite(row.afterSampleAtMs) && finite(row.sampledAtMs)
      ? row.afterSampleAtMs - row.sampledAtMs : null)),
    callbackEntryChildClockGaps: {...gaps(callbackGaps.filter(gapComplete)),
      allObservedIncludingCensored: gaps(callbackGaps),
      excludedFromContinuous: gaps(callbackGaps.filter(gap => !gapComplete(gap))),
      definition: 'Continuous qualified adjacency only. Every raw adjacent callback interval, including lifecycle/hidden/incomplete crossings, remains in allObservedIncludingCensored and the excluded subset.'},
    observablePresentedNewStateChildSampleGaps: {...gaps(newGaps), positiveObservations: newCount,
      staticPresentationDelta: staticPresentations, multiPresentationObservations,
      definition: 'A presentation counter increase with session frame increase since the previous observed presentation, within one continuous visible internal session phase. Lifecycle/owner/hidden/missing samples reset the anchors; only successive positive observations within a segment close a gap. Presentation jumps may hide intermediate presentations.'},
    firstSample: ref(observable[0]), lastSample: ref(lastSample),
    firstPositiveNewState: ref(positive[0]), lastPositiveNewState: ref(lastPositive),
    observedSegments: series(segments),
    leftBoundary: {censored: true, reason: 'Host phase markers are not child-clock start timestamps; state before the first observation is unknown.'},
    rightBoundary: {censored: true, endpointChildMs: tailAt,
      endpointKind: useSnapshotEnd ? 'selected-snapshot-child-clock' : 'last-retained-child-observation-lower-bound',
      observedChildEndpointMinusLastPositiveMs: lastPositive && finite(tailAt) && tailAt >= lastPositive.sampledAtMs
        ? tailAt - lastPositive.sampledAtMs : null,
      noFurtherObservedCallbackLowerBoundMs: complete && entries.length &&
        frameById.get(entries.at(-1).callId)?.okay && !isHidden(sampleFor(entries.at(-1))) &&
        finite(entries.at(-1).originalEnteredAtMs) && finite(tailAt) && tailAt >= entries.at(-1).originalEnteredAtMs &&
        !rows.some(row => row.seq > entries.at(-1).seq && (row.kind === 'owner-invalid' || row.kind === 'lifecycle'))
        ? tailAt - entries.at(-1).originalEnteredAtMs : null,
      noFurtherObservedNewStateLowerBoundMs: complete && lastPositiveStillContinuous && finite(tailAt) &&
        tailAt >= lastPositive.sampledAtMs ? tailAt - lastPositive.sampledAtMs : null,
      scope: 'Open observation tail, not an extra closed gap or proof that no unobserved presentation occurred.'},
    position: {x: range(0), y: range(1), numericChangesBetweenContiguousSamples: positionChanges,
      bitChangesBetweenContiguousSamples: bitChanges,
      scope: 'Observed local player state changes; not causal attribution to the most recent MOVE, nor physical response.'},
    costScope: 'C++ rAF callback wall-work argument, potentially zero or multiple ticks, synchronous bridges/audio/draw included. Not pure game-tick CPU.'};
}
function identitySummary(identity) {
  if (!identity) return null;
  const runtimes = {};
  for (const [name, runtime] of Object.entries(identity.runtimes || {})) {
    runtimes[name] = {root: runtime.root, variant: runtime.variant, inventory: runtime.inventory,
      version: runtime.version, fileCount: Object.keys(runtime.files || {}).length,
      filesOrderedJsonSha256: digest(runtime.files || null),
      wasm: runtime.files?.['th09.wasm'] ?? null, managed: runtime.files?.['shell.mjs'] ?? null};
  }
  return {schema: identity.schema, recordedIdentityJsonSha256: digest(identity),
    ui: identity.ui ? {sourceRoot: identity.ui.sourceRoot ?? null, root: identity.ui.root, artifactId: identity.ui.artifactId,
      fileCount: Array.isArray(identity.ui.files) ? identity.ui.files.length : Object.keys(identity.ui.files || {}).length,
      filesOrderedJsonSha256: digest(identity.ui.files)} : null,
    runtimes, content: identity.content, hostManifestSha256: identity.hostManifestSha256,
    inputCheck: identity.inputCheck, trackedInputCount: list(identity.inputs).length,
    trackedInputsJsonSha256: digest(identity.inputs), probe: identity.probe};
}
function analyzeReport(raw, input) {
  const choices = [['probe', raw.probe], ['failureProbe', raw.failureProbe],
    ['resourceProbe.beforeReleaseProbe', raw.resourceProbe?.beforeReleaseProbe], ['disposed', raw.disposed]];
  const selected = choices.find(([, probe]) => probe && typeof probe === 'object');
  const result = {input, schema: raw.schema, runLabel: raw.runLabel ?? null,
    platform: raw.platform ?? null, movement: raw.movement ?? null, cpuRate: raw.cpuRate ?? null,
    platformScope: raw.platform === 'desktop' ? 'Desktop Chromium with mobile/touch emulation and declared CDP CPU throttle; not a physical phone.'
      : raw.platform === 'android' ? 'Recorded Android browser via CDP; injected source, no CPU4 equivalence asserted.' : 'Unknown platform.',
    startedAt: raw.startedAt ?? null, endedAt: raw.endedAt ?? null,
    driverPassed: raw.passed === true, driverFailure: raw.failure ?? null, driverGoals: raw.goals ?? null,
    sourceBefore: raw.sourceBefore ?? null, sourceAfter: raw.sourceAfter ?? null,
    sourceStableClaim: raw.sourceStable ?? null,
    sourceIdentityRowsValid: Array.isArray(raw.sourceBefore) && raw.sourceBefore.length === 3 &&
      new Set(raw.sourceBefore.map(row => row.path)).size === 3 && raw.sourceBefore.every(row =>
        typeof row.path === 'string' && Number.isSafeInteger(row.bytes) && row.bytes > 0 && /^[a-f0-9]{64}$/i.test(row.sha256 || '')),
    sourceIdentityRowsEqual: Array.isArray(raw.sourceBefore) && raw.sourceBefore.length > 0 &&
      same(raw.sourceBefore, raw.sourceAfter),
    sourceIdentityScope: 'Recorded before/after byte identities compared; this analyzer does not reread or replace frozen source/artifact files.',
    environment: {browserVersion: raw.browserVersion, deviceBrowser: raw.deviceBrowser,
      desktopHeadless: raw.desktopHeadless, desktopLaunch: raw.desktopLaunch, gameGpu: raw.gameGpu,
      dispatchWindow: raw.dispatchWindow, dispatchScope: raw.dispatchScope,
      android: raw.android, audio: raw.audio, launched: raw.launched, gameReady: raw.gameReady,
      finalState: raw.finalState, failureState: raw.failureState, surface: raw.surface,
      launchFlow: raw.launchFlow, acceptedWarnings: raw.acceptedWarnings},
    serverIdentity: identitySummary(raw.server?.identity),
    failures: {pageErrors: series(list(raw.pageErrors)), consoleErrors: series(list(raw.consoleErrors)),
      requestsFailed: series(list(raw.requestsFailed)), failureUi: raw.failureUi,
      failureProbeError: raw.failureProbeError, failureStateError: raw.failureStateError},
    cleanup: {probeRestored: raw.probeRestored ?? null, errors: raw.cleanupErrors ?? null,
      disposeError: raw.disposeError, pageCloseError: raw.pageCloseError,
      browserCloseError: raw.browserCloseError, serverStopError: raw.serverStopError,
      androidMappingCleanup: raw.androidMappingCleanup, forwardCleanupSkipped: raw.forwardCleanupSkipped,
      reverseCleanupSkipped: raw.reverseCleanupSkipped,
      serverClean: raw.serverFinal?.clean ?? null, sourceAudit: raw.serverFinal?.inputsAfter,
      serverEvidence: raw.serverFinal?.evidence, serverErrors: raw.serverFinal?.errors},
    sourceDispatch: {available: Array.isArray(raw.injections), nodeClockOnly: true,
      rows: series(list(raw.injections)), errors: list(raw.injections).filter(row => row.error),
      byType: Object.create(null),
      phaseAssignment: 'Injection rows have no probe phase id; Node times are not converted to host/child clocks or assigned to phases.'},
    phasePlans: {baselineGesture: raw.baselineGesture ?? null, queueGesture: raw.queueGesture ?? null,
      afterResourceGesture: raw.afterResourceGesture ?? null},
    resource: {request: raw.resourceProbe?.request ?? null, arm: raw.resourceProbe?.arm,
      pendingState: raw.resourceProbe?.pendingState, release: raw.resourceProbe?.release,
      finalStatus: raw.resourceProbe?.finalStatus,
      beforeReleaseState: raw.resourceProbe?.beforeReleaseState,
      beforeReleaseEvents: series(list(raw.resourceProbe?.beforeReleaseEvents)),
      beforeReleaseMatchingEvents: list(raw.resourceProbe?.beforeReleaseEvents)
        .filter(row => row.request === raw.resourceProbe?.request),
      finalMatchingEvents: list(raw.runtimeEvents).filter(row => row.request === raw.resourceProbe?.request),
      clockScope: 'Server, Node driver and host event times remain their original fields; no cross-clock duration is derived.'},
    stages: raw.stages, runtimeEvents: series(list(raw.runtimeEvents)),
    snapshots: Object.fromEntries(choices.map(([name, probe]) => [name, snapshotMeta(probe)])),
    selectedSnapshot: selected?.[0] ?? null,
    snapshotScope: selected?.[0] === 'probe' ? 'final observation snapshot; driver and cleanup qualifications remain separate'
      : 'partial/failure fallback; never promoted to final PASS; snapshots are not concatenated',
    analysisIssues: [], phases: []};
  for (const row of list(raw.injections)) increment(result.sourceDispatch.byType, row.type);
  if (raw.schema !== 'th09-managed-touch-hosted-integration/v1') result.analysisIssues.push('unknown driver schema');
  if (!selected) result.analysisIssues.push('no retained probe snapshot');
  else {
    const probe = selected[1], inspected = inspectProbe(probe);
    result.probeSchema = probe.schemas;result.probeScope = probe.scope;result.probeClocks = probe.clocks;
    result.probeIntegrity = {completeRetainedEvidence: inspected.complete,
      checks: inspected.checks, failedChecks: inspected.failedChecks, observedCounters: inspected.observedCounters};
    const allInputs = inputSummary(inspected.rows, raw.movement, inspected.complete);
    result.runWideInputOrder = {allCommandsEqualInObservedOrder: allInputs.allCommandsEqualInObservedOrder,
      byCommand: allInputs.byCommand, nativeArguments: allInputs.nativeArguments,
      activeMovementOrderQualified: allInputs.activeMovementOrderQualified};
    const lastPhaseId = inspected.rows.at(-1)?.phaseId;
    for (const phase of inspected.phases) {
      const rows = inspected.rows.filter(row => row.phaseId === phase.id);
      const start = rows.find(row => row.kind === 'phase' || row.kind === 'installed');
      const next = inspected.rows.find(row => row.phaseId > phase.id);
      const row = {id: phase.id, name: phase.name, available: true,
        movementPhase: MOVEMENT_PHASES.has(phase.name), observationRecords: rows.length,
        rawRecordRange: [rows[0]?._rawIndex ?? null, rows.at(-1)?._rawIndex ?? null],
        observedPhaseBoundary: {startHostMs: start?.clock === 'host' ? start.atMs : null,
          nextPhaseHostMs: next?.clock === 'host' ? next.atMs : null,
          scope: 'Observed phase labels; asynchronous messages can originate in a preceding phase.'},
        lifecycle: series(rows.filter(record => record.kind === 'lifecycle' || record.kind === 'owner-invalid'), ref)};
      try {
        row.input = inputSummary(rows, raw.movement, inspected.complete);
        row.frames = frameSummary(rows, probe, inspected.complete, lastPhaseId);
        if (row.frames.incomplete) result.analysisIssues.push('Incomplete frame evidence in phase ' + phase.id +
          ' (' + phase.name + '): local diagnostics=' + row.frames.discontinuities.count +
          ', probeComplete=' + inspected.complete);
        row.nativeInputAndObservedProgressQualified = row.input.activeMovementOrderQualified && !row.frames.incomplete &&
          row.frames.validSamplePairs >= 2 && row.frames.observablePresentedNewStateChildSampleGaps.positiveObservations > 0 &&
          [row.frames.position.x, row.frames.position.y].some(axis => finite(axis.min) && axis.max > axis.min);
      } catch (error) {row.analysisFailure = String(error);result.analysisIssues.push('phase ' + phase.id + ': ' + error);}
      result.phases.push(row);
    }
    for (const name of EXPECTED_PHASES) if (!result.phases.some(phase => phase.name === name))
      result.phases.push({id: null, name, available: false, reason: 'phase not present in selected snapshot; no zero-valued performance substituted'});
  }
  result.cleanupQualified = raw.probeRestored === true && Array.isArray(raw.cleanupErrors) &&
    raw.cleanupErrors.length === 0 && raw.serverFinal?.clean === true &&
    !['disposeError', 'pageCloseError', 'browserCloseError', 'serverStopError'].some(key => raw[key]) &&
    (!raw.androidMappingCleanup || Object.values(raw.androidMappingCleanup).every(value => value === true));
  const expectedGoals = ['singleCurrentNativeRuntime', 'originalOwnerAndHooksPreserved',
    'originalInputReachesNativeDuringResourceWait', 'realGameStillRunning', 'resourcesAwaitedUntilExplicitRelease',
    'resourceEventuallyAcknowledged', 'noUnhandledPageError'];
  result.driverGoalFieldsTrue = expectedGoals.every(key => raw.goals?.[key] === true);
  result.movementPhasesQualified = [...MOVEMENT_PHASES].every(name =>
    result.phases.some(phase => phase.name === name && phase.nativeInputAndObservedProgressQualified === true));
  result.finalIntegrationQualified = result.driverPassed && result.driverGoalFieldsTrue && raw.sourceStable === true &&
    result.sourceIdentityRowsValid && result.sourceIdentityRowsEqual && result.movementPhasesQualified &&
    result.cleanupQualified && selected?.[0] === 'probe' &&
    result.probeIntegrity?.completeRetainedEvidence === true && result.analysisIssues.length === 0;
  return result;
}
const argv = process.argv.slice(2);
if (argv.length < 2) throw Error('Usage: node managed-touch-launcher-summary.mjs NEW_OUTPUT.json REPORT.json [...]');
const output = resolve(argv[0]), inputs = argv.slice(1).map(path => resolve(path));
if (existsSync(output) || inputs.includes(output)) throw Error('Output must be new and different from all inputs: ' + output);
const selfPath = fileURLToPath(import.meta.url), selfBytes = readFileSync(selfPath);
const evidence = {schema: 'th09-managed-touch-launcher-summary/v1', generatedAt: new Date().toISOString(),
  analyzer: {path: selfPath, bytes: selfBytes.length, sha256: sha(selfBytes), argv: process.argv.slice(1)},
  methods: {
    digest: 'Input/source SHA hashes raw file bytes. OrderedJsonSha256 uses UTF-8 JSON with recursively sorted object keys; array order and encoded special numbers are retained.',
    quantiles: 'Linear interpolation at (n - 1) * q; >50 and >100 ms are strict. Empty/invalid measurements remain explicit.',
    qualification: 'All failed/incomplete runs and phases remain. All raw callback adjacency, including censored boundaries, is retained separately from the named continuous subset. State anchors reset at recorded boundaries; censored spans and diagnostics remain, and any incomplete frame evidence prevents final integration qualification. No good-segment subset qualifies an incomplete phase.',
    clocks: 'Callback entries and sampled new states use distinct child performance.now boundaries. Host pointer times and Node dispatch times remain separate; no host-minus-child or Node-minus-page latency.',
    progress: 'Positively observed session-frame and presentation progress, not static repeated presentation. Gaps are between observable states and may span multiple underlying presentations.',
    coverage: 'Owner/hook/getter checks are observed boundaries only. Cached original references or transient changes between checks are not ruled out. Ring loss/errors invalidate completeness globally; recorded internal discontinuities split state continuity and remain reported.',
    scope: 'CDP-generated source through real Launcher/native properties; not a physical-finger delay, rAF request dispatch trace, single-tick CPU measurement, multiplayer rollback benchmark, no-hitch acceptance, or causal comparison across differently timed/frequency phases.',
    instrumentation: 'Diagnostic wrappers, getters and snapshots add uncalibrated work; no performance improvement is inferred from these observations.'},
  runs: []};
for (const path of inputs) {
  let input = {path, bytes: null, sha256: null}, raw = null;
  try {
    const bytes = readFileSync(path);input = {path, bytes: bytes.length, sha256: sha(bytes)};
    raw = JSON.parse(bytes.toString('utf8'));evidence.runs.push(analyzeReport(raw, input));
  } catch (error) {
    evidence.runs.push({input, runLabel: raw?.runLabel ?? null, schema: raw?.schema ?? null,
      driverPassed: typeof raw?.passed === 'boolean' ? raw.passed : null, driverFailure: raw?.failure ?? null,
      finalIntegrationQualified: false, analysisIssues: [String(error)], failureRetained: true});
  }
}
evidence.summary = {runs: evidence.runs.length,
  driverPassed: evidence.runs.filter(run => run.driverPassed === true).length,
  finalIntegrationQualified: evidence.runs.filter(run => run.finalIntegrationQualified).length,
  analysisFailures: evidence.runs.filter(run => run.analysisIssues.length > 0).length,
  comparison: 'No performance ranking, phase pooling, FPS claim or causal A/B ratio is computed.'};
const encoded = JSON.stringify(evidence, null, 2) + '\n';
writeFileSync(output, encoded, {flag: 'wx'});
console.log(JSON.stringify({output, bytes: Buffer.byteLength(encoded), sha256: sha(Buffer.from(encoded)),
  analyzer: evidence.analyzer, summary: evidence.summary}));
if (evidence.runs.some(run => !run.finalIntegrationQualified)) process.exitCode = 1;
