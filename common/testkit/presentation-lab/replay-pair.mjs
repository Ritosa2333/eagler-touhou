const SCHEMA = 'eagler-presentation/replay-pair-run/1';
const PAIR_SCHEMA = 'eagler-presentation/replay-pair/1';
const CADENCE_SCHEMA = 'eagler-presentation/replay-cadence-comparison/1';
const CADENCE_DIFFERENCE_GROUPS = Object.freeze(['host-frame-cadence', 'synchronous-audio-operations']);
const DRAW_ALPHAS = Object.freeze([0, 0.5, 1]);

const isRecord = value => value !== null && typeof value === 'object' && !Array.isArray(value);
const clone = value => JSON.parse(JSON.stringify(value));

function canonical(value) {
  if (Array.isArray(value)) return `[${value.map(canonical).join(',')}]`;
  if (isRecord(value)) return `{${Object.keys(value).sort().map(key => `${JSON.stringify(key)}:${canonical(value[key])}`).join(',')}}`;
  return JSON.stringify(value);
}

const same = (left, right) => canonical(left) === canonical(right);

function checkCheckpoint(value) {
  const issues = [];
  if (!isRecord(value)) return ['checkpoint is missing'];
  if (value.schema !== 'eagler-presentation/checkpoint/1') issues.push('checkpoint schema is missing');
  if (!isRecord(value.state) || value.state.complete !== true || !Array.isArray(value.state.groups) || !value.state.groups.length)
    issues.push('complete authority groups are missing');
  else {
    const ids = new Set();
    for (const group of value.state.groups) {
      if (!isRecord(group) || typeof group.id !== 'string' || !group.id || !Number.isSafeInteger(group.digest))
        issues.push('an authority group has no id or integer digest');
      else if (ids.has(group.id)) issues.push(`duplicate authority group ${group.id}`);
      else ids.add(group.id);
    }
  }
  if (!isRecord(value.runtime) || !Array.isArray(value.runtime.status) || !Array.isArray(value.runtime.trace))
    issues.push('Runtime status or trace is missing');
  else if (value.runtime.trace.length < 1) issues.push('Runtime trace is empty');
  if (!isRecord(value.replay) || value.replay.available !== true || !Array.isArray(value.replay.status) ||
      !isRecord(value.replay.trace) || !Array.isArray(value.replay.trace.latest))
    issues.push('Replay status or consumer trace is missing');
  if (!isRecord(value.audio) || !Array.isArray(value.audio.auditWords) || value.audio.auditWords.length < 2 ||
      !isRecord(value.audio.sfx) || value.audio.sfx.available !== true || value.audio.sfx.enabled !== true ||
      !Number.isSafeInteger(value.audio.sfx.sequence) || !isRecord(value.audio.sfx.dropped))
    issues.push('enabled SFX event trace or synchronous audio audit is missing');
  else if (Object.values(value.audio.sfx.dropped).some(count => !Number.isSafeInteger(count) || count !== 0))
    issues.push('SFX event trace overflowed or reported invalid drops');
  return issues;
}

function comparableCheckpoint(value) {
  return {
    state: value.state.groups.map(({id, digest}) => ({id, digest})),
    runtime: value.runtime,
    replay: value.replay,
    audioAuditWords: value.audio.auditWords,
  };
}

function normalizeAudioRows(rows) {
  if (!isRecord(rows) || rows.available !== true || !isRecord(rows.dropped) ||
      Object.values(rows.dropped).some(count => !Number.isSafeInteger(count) || count !== 0) ||
      !Array.isArray(rows.events) || !Array.isArray(rows.routes) || !Array.isArray(rows.uploads) || !Array.isArray(rows.errors))
    return {status: 'incomplete', issues: ['SFX interval rows are missing or incomplete'], value: null};
  if (rows.errors.length) return {status: 'failed', issues: ['SFX observer reported an error'], value: null};
  const events = [...rows.events].sort((a, b) => a.serial - b.serial);
  const routes = [...rows.routes].sort((a, b) => a.serial - b.serial);
  const issues = [];
  for (const event of events) if (!Number.isSafeInteger(event.logicalIndex)) issues.push('SFX event has no native logical index');
  for (const event of events) {
    const matches = routes.filter(route => route.eventSerial === event.serial);
    if (matches.length !== 1) issues.push(`SFX event ${event.serial} has ${matches.length} native routes`);
    else if (matches[0].logicalIndex !== event.logicalIndex) issues.push(`SFX event ${event.serial} and route have different logical indexes`);
  }
  for (const route of routes) {
    if (!Number.isSafeInteger(route.logicalIndex) || !Number.isSafeInteger(route.eventSerial))
      issues.push('native SFX route is not linked to a logical event');
    if (['direct','fallback','fallbackStarted','pending'].some(key => typeof route[key] !== 'boolean'))
      issues.push('native SFX route is missing a strict boolean disposition');
  }
  const normalizedRoutes = routes.map(route => ({
    logicalIndex: route.logicalIndex,
    direct: route.direct === true,
    fallback: route.fallback === true,
    fallbackStarted: route.fallbackStarted === true,
    pending: route.pending === true,
  }));
  return {status: issues.length ? 'failed' : 'passed', issues, value: issues.length ? null : {
    events: events.map(event => event.logicalIndex),
    routes: normalizedRoutes,
    uploads: rows.uploads.length,
    errors: rows.errors.length,
  }};
}

function replaySequence(checkpoint) {
  return checkpoint?.replay?.trace?.nextSequence;
}

async function readStableCheckpoint(readCheckpoint, repetitions = 2) {
  const samples = [];
  for (let i = 0; i < repetitions; i++) samples.push(clone(await readCheckpoint()));
  const issues = [];
  for (let i = 0; i < samples.length; i++) issues.push(...checkCheckpoint(samples[i]).map(issue => `read ${i + 1}: ${issue}`));
  if (samples.some(sample => !same(sample, samples[0]))) issues.push('Repeated observation reads changed Runtime, Replay, or audio evidence');
  return {checkpoint: samples[0], samples, issues, status: issues.length ? 'failed' : 'passed'};
}

/** Repeat only read-only adapters. Seventeen reads mirror the TH08 authority-group count. */
export async function verifyReadOnlyCheckpoint({readCheckpoint, repetitions = 2, expectedAuthorityGroups = null} = {}) {
  if (typeof readCheckpoint !== 'function') throw Error('readCheckpoint must be a function');
  if (!Number.isSafeInteger(repetitions) || repetitions < 2 || repetitions > 64) throw Error('repetitions must be 2..64');
  if (expectedAuthorityGroups !== null && (!Number.isSafeInteger(expectedAuthorityGroups) || expectedAuthorityGroups < 1))
    throw Error('expectedAuthorityGroups must be a positive integer');
  const result = await readStableCheckpoint(readCheckpoint, repetitions);
  const groupCount = result.checkpoint?.state?.groups?.length ?? 0;
  if (result.status === 'passed' && expectedAuthorityGroups !== null && groupCount !== expectedAuthorityGroups)
    result.issues.push(`expected ${expectedAuthorityGroups} authority groups, found ${groupCount}`);
  if (result.issues.length) result.status = 'failed';
  return {schema: 'eagler-presentation/read-only-observation/1', status: result.status,
    repetitions, authorityGroups: groupCount, checkpoint: result.checkpoint, issues: result.issues};
}

/** Capture one freshly booted real Replay epoch. Draw stress is explicit and bounded. */
export async function captureReplayPairRun({
  controller, role, ticks, identity, replayIdentity, epoch,
  readCheckpoint, readAudioRowsSince, drawAlphas = DRAW_ALPHAS, expectedAuthorityGroups = 17,
  scope, yieldEvery = 60, onBatch = async () => {},
} = {}) {
  if (!['baseline', 'controlled-extra-draw'].includes(role)) throw Error('role must be baseline or controlled-extra-draw');
  if (!Number.isSafeInteger(ticks) || ticks < 1 || ticks > 3600) throw Error('ticks must be 1..3600');
  if (!Number.isSafeInteger(epoch) || epoch < 1) throw Error('a positive Runtime epoch is required');
  if (!isRecord(identity) || !isRecord(identity.build) || !isRecord(identity.data) || !isRecord(replayIdentity))
    throw Error('build, DATA, and Replay identities are required');
  if (!isRecord(scope) || typeof scope.game !== 'string' || !scope.game || typeof scope.variant !== 'string' || !scope.variant)
    throw Error('an explicit game and variant scope is required');
  if (typeof controller?.step !== 'function' || typeof controller?.preview !== 'function' ||
      typeof readCheckpoint !== 'function' || typeof readAudioRowsSince !== 'function')
    throw Error('controller and read-only Runtime observers are required');
  if (role === 'controlled-extra-draw' && (!Array.isArray(drawAlphas) || drawAlphas.length < 2 ||
      drawAlphas.some(alpha => !Number.isFinite(alpha) || alpha < 0 || alpha > 1)))
    throw Error('controlled extra draw requires at least two valid alphas');
  if (!Number.isSafeInteger(yieldEvery) || yieldEvery < 1 || yieldEvery > 600) throw Error('yieldEvery must be 1..600');
  if (!Number.isSafeInteger(expectedAuthorityGroups) || expectedAuthorityGroups < 1) throw Error('expectedAuthorityGroups is required');

  controller.freeze();
  const readAudit = () => readStableCheckpoint(readCheckpoint, 2);
  const initialRead = await verifyReadOnlyCheckpoint({readCheckpoint, repetitions: 2, expectedAuthorityGroups});
  const start = await readAudit();
  const issues = [...initialRead.issues, ...start.issues];
  const samples = [];
  let status = issues.length ? 'failed' : 'passed';
  let audioEventCount = 0, audioRouteCount = 0;
  if (start.checkpoint?.replay?.status?.[0] !== 1) {
    issues.push('No selected Replay stream is active at capture start');
    if (status === 'passed') status = 'incomplete';
  }

  for (let index = 0; index < ticks && status === 'passed'; index++) {
    const beforeRead = await readAudit();
    const before = beforeRead.checkpoint;
    issues.push(...beforeRead.issues.map(issue => `tick ${index + 1}: ${issue}`));
    if (issues.length) { status = 'failed'; break; }
    const beforeSequence = before.audio.sfx.sequence;
    const beforeReplaySequence = replaySequence(before);
    const stepReceipt = controller.step(1);
    const stepRead = await readAudit();
    const afterStep = stepRead.checkpoint;
    issues.push(...stepRead.issues.map(issue => `tick ${index + 1}: ${issue}`));
    if (stepRead.issues.length) { status = 'failed'; break; }
    const stepAudioRaw = await readAudioRowsSince(beforeSequence);
    const stepAudio = normalizeAudioRows(stepAudioRaw);
    issues.push(...stepAudio.issues.map(issue => `tick ${index + 1}: ${issue}`));
    if (replaySequence(afterStep) !== beforeReplaySequence + 1)
      issues.push(`tick ${index + 1}: Replay consumer trace did not append exactly one completed update`);
    if (stepAudio.status === 'failed') status = 'failed';
    else if (stepAudio.status === 'incomplete') status = 'incomplete';
    else { audioEventCount += stepAudio.value.events.length; audioRouteCount += stepAudio.value.routes.length; }
    const row = {logicalTick: index + 1, before: comparableCheckpoint(before), afterStep: comparableCheckpoint(afterStep),
      replayTraceSequence: replaySequence(afterStep), audio: stepAudio.status === 'passed' ? stepAudio.value : null, drawReceipts: []};
    if (stepReceipt !== undefined) row.scheduler = clone(stepReceipt);

    if (role === 'controlled-extra-draw' && status === 'passed') {
      for (const alpha of drawAlphas) {
        const drawBeforeRead = await readAudit();
        const drawBefore = drawBeforeRead.checkpoint;
        issues.push(...drawBeforeRead.issues.map(issue => `tick ${index + 1}, alpha ${alpha}, before: ${issue}`));
        if (drawBeforeRead.issues.length) { status = 'failed'; break; }
        const audioCursor = drawBefore?.audio?.sfx?.sequence;
        let receipt;
        try { receipt = controller.preview(alpha, {world: true}); }
        catch (error) { issues.push(`tick ${index + 1}, alpha ${alpha}: draw failed: ${error.message}`); status = 'failed'; break; }
        const drawAfterRead = await readAudit();
        const drawAfter = drawAfterRead.checkpoint;
        issues.push(...drawAfterRead.issues.map(issue => `tick ${index + 1}, alpha ${alpha}, after: ${issue}`));
        if (drawAfterRead.issues.length) { status = 'failed'; break; }
        const drawAudio = normalizeAudioRows(await readAudioRowsSince(audioCursor));
        issues.push(...drawAudio.issues.map(issue => `tick ${index + 1}, alpha ${alpha}: ${issue}`));
        if (drawAudio.status === 'failed') status = 'failed';
        else if (drawAudio.status === 'incomplete') status = 'incomplete';
        const changed = !same(comparableCheckpoint(drawBefore), comparableCheckpoint(drawAfter));
        const audioChanged = drawAudio.status === 'passed' && (drawAudio.value.events.length || drawAudio.value.routes.length ||
          drawAudio.value.uploads || drawAudio.value.errors);
        row.drawReceipts.push({alpha, status: receipt?.status ?? 'missing',
          unchanged: !changed && !audioChanged && drawAudio.status === 'passed',
          before: comparableCheckpoint(drawBefore), after: comparableCheckpoint(drawAfter),
          audio: drawAudio.status === 'passed' ? drawAudio.value : null});
        if (changed) { issues.push(`tick ${index + 1}, alpha ${alpha}: draw changed authoritative Runtime, Replay, input, or audio state`); status = 'failed'; break; }
        if (audioChanged) { issues.push(`tick ${index + 1}, alpha ${alpha}: draw emitted audio uploads, events, or routes`); status = 'failed'; break; }
        if (status !== 'passed') break;
      }
    }
    samples.push(row);
    if (issues.length && status === 'passed') status = 'failed';
    if ((index + 1) % yieldEvery === 0) await onBatch({completedTicks: index + 1, totalTicks: ticks});
  }
  if (!audioEventCount) {
    issues.push('No native SFX events occurred in the captured logical window');
    if (status === 'passed') status = 'incomplete';
  }
  const readVerification = {schema: initialRead.schema, status: initialRead.status,
    repetitions: initialRead.repetitions, expectedAuthorityGroups, authorityGroups: initialRead.authorityGroups, issues: initialRead.issues};
  return {schema: SCHEMA, role, status, scope:clone(scope),identity: clone(identity), replayIdentity: clone(replayIdentity), epoch,
    capture: {requestedTicks: ticks, completedTicks: samples.length,
      drawAlphas: role === 'controlled-extra-draw' ? [...drawAlphas] : [],
      method: role === 'controlled-extra-draw' ? 'logical tick with fixed-alpha draw-only calls' : 'logical tick only'},
    readVerification, start: start.checkpoint, samples, audioEvents: audioEventCount, audioRoutes: audioRouteCount,
    claims: {controlledExtraDrawPurity: status === 'passed', physicalRefreshVerified: false,
      replayNaturalTerminalVerified: false, rollbackAudioVerified: false}, issues};
}

function runIssues(run, role, requiredTicks) {
  const issues = [];
  if (!isRecord(run) || run.schema !== SCHEMA) return ['Run evidence schema is missing or unsupported'];
  if (run.role !== role) issues.push(`Expected ${role} run`);
  if (run.status !== 'passed') issues.push(`${role} run status is ${run.status || 'missing'}`);
  if (!Number.isSafeInteger(run.epoch) || run.epoch < 1) issues.push(`${role} Runtime epoch is missing`);
  if (!isRecord(run.scope) || typeof run.scope.game !== 'string' || !run.scope.game ||
      typeof run.scope.variant !== 'string' || !run.scope.variant)
    issues.push(`${role} game and variant scope is missing`);
  if (run.readVerification?.status !== 'passed' || run.readVerification?.repetitions !== 2 ||
      run.readVerification?.authorityGroups !== run.readVerification?.expectedAuthorityGroups ||
      !Number.isSafeInteger(run.readVerification?.expectedAuthorityGroups))
    issues.push(`${role} read-only verification is incomplete`);
  if (!Array.isArray(run.samples) || run.samples.length !== requiredTicks || run.capture?.completedTicks !== requiredTicks)
    issues.push(`${role} does not contain ${requiredTicks} complete logical ticks`);
  return issues;
}

/** Compare two separately saved records; a single window cannot satisfy a pair. */
export function compareReplayPairRuns({baseline, controlledExtraDraw, requiredTicks, scope} = {}) {
  if (!Number.isSafeInteger(requiredTicks) || requiredTicks < 1 || requiredTicks > 3600)
    throw Error('requiredTicks must be 1..3600');
  const issues = [
    ...runIssues(baseline, 'baseline', requiredTicks),
    ...runIssues(controlledExtraDraw, 'controlled-extra-draw', requiredTicks),
  ];
  if (baseline?.epoch === controlledExtraDraw?.epoch) issues.push('Baseline and stress runs must use distinct Runtime epochs');
  const selectedScope=scope||baseline?.scope;
  const safeScope=isRecord(selectedScope)&&typeof selectedScope.game==='string'&&selectedScope.game.trim()&&
    typeof selectedScope.variant==='string'&&selectedScope.variant.trim()
    ?{game:selectedScope.game,variant:selectedScope.variant}:{game:null,variant:null};
  if (!isRecord(selectedScope)||typeof selectedScope.game!=='string'||!selectedScope.game||typeof selectedScope.variant!=='string'||!selectedScope.variant)
    issues.push('An explicit game and variant comparison scope is required');
  if (!same(baseline?.scope,controlledExtraDraw?.scope)||!same(baseline?.scope,selectedScope))
    issues.push('Paired Replay scopes differ or do not match the selected product');
  if (!same(baseline?.identity, controlledExtraDraw?.identity)) issues.push('Build, DATA, or tool identity differs between runs');
  if (!same(baseline?.replayIdentity, controlledExtraDraw?.replayIdentity)) issues.push('Replay bytes differ between runs');
  const pairStatus=[baseline?.status,controlledExtraDraw?.status].includes('failed')?'failed':
    [baseline?.status,controlledExtraDraw?.status].includes('incomplete')?'incomplete':'passed';
  const boundCheck=(id,status,complete,evidence={},machineComplete=complete)=>({id,status,complete,machineComplete,
    evidence:{game:safeScope.game,variant:safeScope.variant,adaptationIds:[id],...evidence}});
  const makeIncompleteChecks=checkStatus=>[
    boundCheck('DISPLAY-01',checkStatus,false,{pairedTicks:0}),
    boundCheck('DISPLAY-02','incomplete',false,{reason:'No physical display refresh or compositor cadence was measured'}),
    boundCheck('DISPLAY-03',checkStatus,false,{controlledExtraDrawPurity:false,authorityGroups:17}),
    boundCheck('DISPLAY-07','incomplete',false,{reason:'A same-epoch first-present receipt is unavailable'}),
    boundCheck('REPLAY-07',checkStatus,false,{controlledDrawReplayEquivalent:false,naturalTerminalVerified:false}),
    boundCheck('AUDIO-06',checkStatus==='failed'?'failed':'incomplete',false,{reason:'A complete paired native event window was not captured'}),
    boundCheck('VERIFY-07','incomplete',false,{reason:'Repeated read-only evidence is incomplete'}),
  ];
  if (baseline?.status !== 'passed' || controlledExtraDraw?.status !== 'passed' || issues.length) {
    const checkStatus=pairStatus==='failed'||(baseline?.status==='passed'&&controlledExtraDraw?.status==='passed'&&issues.length)
      ?'failed':'incomplete';
    return {schema: PAIR_SCHEMA,scope:safeScope,status:checkStatus,issues,
      checks:makeIncompleteChecks(checkStatus),
      claims:{controlledExtraDrawPurity:false,physicalRefreshVerified:false,
        replayNaturalTerminalVerified:false,rollbackAudioVerified:false}};
  }
  if (!same(comparableCheckpoint(baseline.start), comparableCheckpoint(controlledExtraDraw.start)))
    issues.push('Fresh Replay epochs did not begin with the same authority, consumer, and audio state');

  let comparedTicks = 0;
  for (let i = 0; i < requiredTicks; i++) {
    const left = baseline.samples[i], right = controlledExtraDraw.samples[i];
    if (left.logicalTick !== i + 1 || right.logicalTick !== i + 1) {
      issues.push(`Logical tick ${i + 1} is missing, duplicated, or out of order`); break;
    }
    if (!same(left.afterStep, right.afterStep)) issues.push(`Logical tick ${i + 1} diverged between baseline and extra-draw run`);
    if (!same(left.audio, right.audio)) issues.push(`Logical tick ${i + 1} SFX event or native route sequence diverged`);
    if (right.drawReceipts.length !== controlledExtraDraw.capture.drawAlphas.length)
      issues.push(`Logical tick ${i + 1} has missing extra-draw receipts`);
    if (right.drawReceipts.some((receipt, index) => receipt.alpha !== controlledExtraDraw.capture.drawAlphas[index] ||
        receipt.status !== 'drawn' || receipt.unchanged !== true || !same(receipt.before, receipt.after) ||
        receipt.audio?.events?.length || receipt.audio?.routes?.length || receipt.audio?.uploads || receipt.audio?.errors))
      issues.push(`Logical tick ${i + 1} extra draw changed an observed domain or emitted an audio event`);
    if (left.audio?.events?.length) {
      const routeCount = left.audio.routes.length, eventCount = left.audio.events.length;
      if (routeCount !== eventCount) issues.push(`Logical tick ${i + 1} baseline SFX events and routes are not one-to-one`);
    }
    comparedTicks++;
  }
  const audioEvents = baseline.audioEvents + controlledExtraDraw.audioEvents;
  if (!baseline.audioEvents || !controlledExtraDraw.audioEvents)
    issues.push('AUDIO-06 event-route evidence needs at least one real SFX event in each run');
  const status = issues.length ? 'failed' : 'passed';
  const evidence={baselineEpoch: baseline.epoch, controlledExtraDrawEpoch: controlledExtraDraw.epoch,
    build: baseline.identity.build, data: baseline.identity.data, replay: baseline.replayIdentity,
    baselineSfxEvents: baseline.audioEvents, baselineSfxRoutes: baseline.audioRoutes,
    controlledSfxEvents: controlledExtraDraw.audioEvents, controlledSfxRoutes: controlledExtraDraw.audioRoutes,
    drawAlphas: controlledExtraDraw.capture.drawAlphas,
    baselineFirstFrame:baseline.display07??null,controlledFirstFrame:controlledExtraDraw.display07??null};
  const pairOk=status==='passed';
  const sfxWindowStatus=pairOk&&baseline.audioEvents>0&&controlledExtraDraw.audioEvents>0?'passed':
    status==='failed'?'failed':'incomplete';
  const build=baseline.identity.build,productionBuild=build.productionBuild;
  const verifyInputsAvailable=build.diagnostic===true&&build.instrumented===true&&
    productionBuild?.available===true&&productionBuild.diagnostic===false&&productionBuild.sourceMatchesCurrent===true&&
    typeof build.wasm==='string'&&typeof productionBuild.wasmSha256==='string'&&build.wasm!==productionBuild.wasmSha256;
  return {schema: PAIR_SCHEMA, scope:safeScope,status,issues,comparedTicks,
    evidence,
    checks:[
      boundCheck('DISPLAY-01',pairOk?'passed':'failed',false,{controlledTicks:comparedTicks,displayRateComparison:false}),
      boundCheck('DISPLAY-02','incomplete',false,{reason:'No physical display refresh or compositor cadence was measured'}),
      boundCheck('DISPLAY-03',pairOk?'passed':'failed',pairOk,{controlledExtraDrawPurity:pairOk,authorityGroups:17}),
      boundCheck('DISPLAY-07',pairOk&&baseline.display07?.status==='passed'&&controlledExtraDraw.display07?.status==='passed'?'passed':'incomplete',false,
        {baseline:baseline.display07??null,controlledExtraDraw:controlledExtraDraw.display07??null,
          scope:'native successful SDL presentation count before Runtime first-frame event; no browser compositor or scanout claim'}),
      boundCheck('REPLAY-07',pairOk?'passed':'failed',pairOk,{controlledDrawReplayEquivalent:pairOk,physicalRefreshVerified:false,naturalTerminalVerified:false}),
      boundCheck('AUDIO-06',sfxWindowStatus,false,{realSfxEventRouteWindow:sfxWindowStatus,rollbackWindowObserved:false,
        eventCount:baseline.audioEvents+controlledExtraDraw.audioEvents},sfxWindowStatus==='passed'),
      boundCheck('VERIFY-07',verifyInputsAvailable?'passed':'incomplete',false,
        {diagnosticRepeatedReads:baseline.readVerification.status==='passed'&&controlledExtraDraw.readVerification.status==='passed',
          productionReadParity:'not-observed',diagnosticBuild:{profile:build.profile,wasm:build.wasm,
            instrumented:build.instrumented},productionBuild:productionBuild??null}),
    ],
    claims: {controlledExtraDrawPurity: status === 'passed', displayLogicalReplayEquivalent: status === 'passed',
      physicalRefreshVerified: false, replayNaturalTerminalVerified: false, rollbackAudioVerified: false}};
}

function cadenceScheduleIssues(run, rateHz, requiredTicks) {
  const issues = [];
  const scheduler = run?.scheduler;
  if (!isRecord(scheduler) || scheduler.schema !== 'eagler-presentation/native-frame-callback-cadence/1')
    return ['production frame-callback cadence evidence is missing'];
  if (scheduler.source !== 'GameHost::frame') issues.push('cadence was not driven through the production GameHost frame callback');
  if (scheduler.mode !== 'normal-fixed-60hz') issues.push('cadence did not declare the normal fixed-60-Hz simulation mode');
  if (scheduler.rateHz !== rateHz || scheduler.requestedTicks !== requiredTicks ||
      !Array.isArray(scheduler.logicalTicks) || scheduler.logicalTicks.length !== requiredTicks)
    return [...issues, `the ${rateHz} Hz schedule does not contain ${requiredTicks} complete logical-tick receipts`];
  const callbackRows = scheduler.logicalTicks.flatMap(row => Array.isArray(row?.callbacks) ? row.callbacks : []);
  if (!isRecord(scheduler.summary) || scheduler.summary.timingSampleCount !== scheduler.summary.callbackCount ||
      scheduler.summary.callbackCount !== callbackRows.length)
    issues.push('native timing-ring rows do not match the recorded frame callbacks');
  else {
    const count = predicate => callbackRows.filter(row => predicate(row?.timing?.flags)).length;
    const expectedSummary = {
      tickDueCount: count(flags => flags?.tickDue === true),
      presentedCount: count(flags => flags?.presented === true),
      highRefreshPresented: count(flags => flags?.highRefresh === true && flags?.presented === true),
      betweenTickPresentations: count(flags => flags?.highRefresh === true && flags?.presented === true && flags?.tickDue === false),
    };
    for (const [key, expected] of Object.entries(expectedSummary)) {
      if (scheduler.summary[key] !== expected)
        issues.push(`native callback summary ${key} does not match the retained callback receipts`);
    }
  }
  const interval = 1000 / rateHz;
  let expectedSequence = null;
  let previousSimulationFrame = null;
  let callbackIndex = 0;
  for (let index = 0; index < requiredTicks; index++) {
    const row = scheduler.logicalTicks[index];
    if (!isRecord(row) || row.logicalTick !== index + 1 || !Number.isSafeInteger(row.sequenceBefore) ||
        !Number.isSafeInteger(row.sequenceAfter) || !Array.isArray(row.callbacks) || !row.callbacks.length) {
      issues.push(`logical tick ${index + 1} has no complete native callback receipt`);
      continue;
    }
    const sample = run.samples?.[index];
    if (!isRecord(sample) || !isRecord(sample.afterStep) || !isRecord(sample.afterStep.replay?.trace) ||
        !Number.isSafeInteger(sample.replayTraceSequence) || !isRecord(sample.scheduler)) {
      issues.push(`logical tick ${index + 1} has no paired Replay sample receipt`);
    } else {
      if (!same(sample.scheduler, row)) issues.push(`logical tick ${index + 1} scheduler receipt differs from its paired Replay sample`);
      if (sample.replayTraceSequence !== row.sequenceAfter || sample.afterStep.replay.trace.nextSequence !== row.sequenceAfter)
        issues.push(`logical tick ${index + 1} scheduler receipt is not bound to its Replay consumer sample`);
    }
    if (expectedSequence !== null && row.sequenceBefore !== expectedSequence)
      issues.push(`logical tick ${index + 1} does not continue the preceding Replay consumer cursor`);
    if (row.sequenceAfter !== row.sequenceBefore + 1)
      issues.push(`logical tick ${index + 1} did not consume exactly one Replay update`);
    expectedSequence = row.sequenceAfter;
    let consumed = false;
    let previousReplaySequence = row.sequenceBefore;
    for (const receipt of row.callbacks) {
      if (!isRecord(receipt) || receipt.source !== 'GameHost::frame' || receipt.success !== true ||
          !Number.isFinite(receipt.timestampMs) || !Number.isFinite(receipt.deltaMs) ||
          !Number.isSafeInteger(receipt.replaySequence) || !isRecord(receipt.timing) ||
          !isRecord(receipt.timing.flags) || !isRecord(receipt.timingRing?.before) ||
          !isRecord(receipt.timingRing?.after)) {
        issues.push(`logical tick ${index + 1} contains an incomplete native callback receipt`);
        continue;
      }
      const expectedTimestamp = callbackIndex * interval;
      const expectedDelta = callbackIndex === 0 ? 0 : interval;
      if (Math.abs(receipt.timestampMs - expectedTimestamp) > 0.02 ||
          Math.abs(receipt.deltaMs - expectedDelta) > 0.02)
        issues.push(`${rateHz} Hz callback ${callbackIndex + 1} did not use the declared timestamp cadence`);
      const beforeRing = receipt.timingRing.before, afterRing = receipt.timingRing.after;
      if (!Number.isSafeInteger(beforeRing.capacity) || beforeRing.capacity < 1 ||
          !Number.isSafeInteger(beforeRing.count) || beforeRing.count < 0 || beforeRing.count > beforeRing.capacity ||
          !Number.isSafeInteger(beforeRing.next) || beforeRing.next < 0 || beforeRing.next >= beforeRing.capacity ||
          beforeRing.capacity !== afterRing.capacity || afterRing.count !== Math.min(afterRing.capacity, beforeRing.count + 1) ||
          afterRing.next !== (beforeRing.next + 1) % beforeRing.capacity)
        issues.push(`${rateHz} Hz callback ${callbackIndex + 1} is not bound to one new timing-ring slot`);
      if (!Number.isSafeInteger(receipt.timing.simulationFrame) ||
          Math.abs(receipt.timing.timestampMs - receipt.timestampMs) > 0.02 ||
          Math.abs(receipt.timing.deltaMs - receipt.deltaMs) > 0.02)
        issues.push(`${rateHz} Hz callback ${callbackIndex + 1} timing-ring row does not match its callback receipt`);
      if (previousSimulationFrame !== null) {
        const expectedFrame = previousSimulationFrame + (receipt.timing.flags.tickDue ? 1 : 0);
        if (receipt.timing.simulationFrame !== expectedFrame)
          issues.push(`${rateHz} Hz callback ${callbackIndex + 1} simulation-frame stamp does not match its Replay tick boundary`);
      }
      previousSimulationFrame = receipt.timing.simulationFrame;
      const flags = receipt.timing.flags;
      for (const key of ['ready', 'customSpeed', 'tickDue', 'highRefresh', 'presented'])
        if (typeof flags[key] !== 'boolean') issues.push(`${rateHz} Hz callback ${callbackIndex + 1} is missing strict ${key} timing evidence`);
      if (flags.customSpeed === true) issues.push(`${rateHz} Hz callback ${callbackIndex + 1} used custom simulation speed`);
      if (flags.ready !== true) issues.push(`${rateHz} Hz callback ${callbackIndex + 1} did not run in the normal ready presentation path`);
      const delta = receipt.replaySequence - previousReplaySequence;
      if (delta < 0 || delta > 1) issues.push(`${rateHz} Hz callback advanced the Replay consumer by more than one update`);
      if (receipt.timing.flags.tickDue !== (delta === 1))
        issues.push(`${rateHz} Hz timing-ring tickDue flag disagrees with the Replay consumer cursor`);
      if (receipt.timing.flags.highRefresh && !receipt.timing.flags.presented)
        issues.push(`${rateHz} Hz callback marked high-refresh without a successful presentation`);
      if (receipt.timing.flags.tickDue && !receipt.timing.flags.presented)
        issues.push(`${rateHz} Hz callback ${callbackIndex + 1} did not present its fixed logical tick`);
      if (rateHz === 60 && receipt.timing.flags.highRefresh)
        issues.push(`60 Hz callback ${callbackIndex + 1} was incorrectly classified as high-refresh`);
      if (receipt.replaySequence !== row.sequenceBefore && receipt.replaySequence !== row.sequenceAfter)
        issues.push(`${rateHz} Hz callback receipt belongs to another Replay sequence boundary`);
      if (delta === 1) {
        if (consumed) issues.push(`logical tick ${index + 1} consumed more than once`);
        consumed = true;
      }
      previousReplaySequence = receipt.replaySequence;
      callbackIndex++;
    }
    if (!consumed || row.callbacks.at(-1)?.replaySequence !== row.sequenceAfter)
      issues.push(`logical tick ${index + 1} was not completed by its recorded native callback sequence`);
  }
  const timing = isRecord(scheduler.summary) ? scheduler.summary : {};
  if (rateHz === 60 && (timing.highRefreshPresented !== 0 || timing.tickDueCount !== requiredTicks ||
      timing.presentedCount !== requiredTicks))
    issues.push('60 Hz native callback schedule did not present exactly once per fixed logical tick');
  if (rateHz > 60 && timing.tickDueCount !== requiredTicks)
    issues.push(`${rateHz} Hz native callback schedule did not consume exactly one fixed logical tick per Replay sample`);
  if (rateHz > 60 && (!Number.isSafeInteger(timing.highRefreshPresented) || timing.highRefreshPresented < 1 ||
      !Number.isSafeInteger(timing.betweenTickPresentations) || timing.betweenTickPresentations < 1))
    issues.push(`${rateHz} Hz production callback schedule produced no measured between-tick presentation`);
  return issues;
}

function cadenceComparable(sample, cadenceDifferenceGroups) {
  if (!isRecord(sample) || !isRecord(sample.afterStep) || !Array.isArray(sample.afterStep.state) ||
      !isRecord(sample.afterStep.runtime) || !isRecord(sample.afterStep.replay) || !isRecord(sample.audio)) return null;
  return {
    state: sample.afterStep.state.filter(group => !cadenceDifferenceGroups.includes(group.id)),
    runtime: sample.afterStep.runtime,
    replay: sample.afterStep.replay,
    audio: sample.audio,
  };
}

function cadenceStartComparable(checkpoint, cadenceDifferenceGroups) {
  if (!isRecord(checkpoint) || !Array.isArray(checkpoint.state?.groups) || !isRecord(checkpoint.runtime) || !isRecord(checkpoint.replay))
    return null;
  return {state:checkpoint.state.groups.filter(group=>!cadenceDifferenceGroups.includes(group.id)),
    runtime:checkpoint.runtime,replay:checkpoint.replay};
}

/** Compare one Replay window under explicitly driven production callback rates. */
export function compareReplayCadenceRuns({runs, requiredTicks, ratesHz = [60, 120, 144], scope,
  expectedAuthorityGroups = 17, cadenceDifferenceGroups} = {}) {
  if (!Number.isSafeInteger(requiredTicks) || requiredTicks < 1 || requiredTicks > 3600)
    throw Error('requiredTicks must be 1..3600');
  if (!Array.isArray(ratesHz) || ratesHz.length !== 3 || ratesHz.some(rate => ![60, 120, 144].includes(rate)) ||
      new Set(ratesHz).size !== ratesHz.length || ratesHz.join(',') !== '60,120,144')
    throw Error('ratesHz must select 60, 120, and 144 exactly once');
  if (!Array.isArray(cadenceDifferenceGroups) || !same([...cadenceDifferenceGroups].sort(), [...CADENCE_DIFFERENCE_GROUPS].sort()))
    throw Error(`cadenceDifferenceGroups must contain exactly ${CADENCE_DIFFERENCE_GROUPS.join(', ')}`);
  const safeScope = isRecord(scope) && typeof scope.game === 'string' && scope.game.trim() &&
    typeof scope.variant === 'string' && scope.variant.trim() ? {game: scope.game, variant: scope.variant} : null;
  const issues = [];
  if (!safeScope) issues.push('an explicit game and variant cadence-comparison scope is required');
  if (!Array.isArray(runs) || runs.length !== ratesHz.length) {
    issues.push('three fresh Replay runs are required for the declared cadence rates');
    runs = Array.isArray(runs) ? runs : [];
  }
  const byRate = new Map();
  for (const run of runs) {
    const rate = run?.scheduler?.rateHz;
    if (byRate.has(rate)) issues.push(`duplicate ${rate} Hz Replay run`);
    else byRate.set(rate, run);
  }
  const ordered = ratesHz.map(rate => byRate.get(rate));
  if (ordered.some(run => !run)) issues.push('one or more declared cadence Replay runs are missing');
  const presentRuns = ordered.filter(Boolean);
  for (const [index, rate] of ratesHz.entries()) {
    const run = ordered[index];
    if (!run) continue;
    issues.push(...runIssues(run, rate === 60 ? 'baseline' : 'controlled-extra-draw', requiredTicks)
      .map(issue => `${rate} Hz: ${issue}`));
    if (run.readVerification?.authorityGroups !== expectedAuthorityGroups ||
        run.readVerification?.expectedAuthorityGroups !== expectedAuthorityGroups)
      issues.push(`${rate} Hz run did not observe the declared ${expectedAuthorityGroups} authority groups`);
    if (safeScope && !same(run.scope, safeScope)) issues.push(`${rate} Hz run belongs to another game or variant`);
    issues.push(...cadenceScheduleIssues(run, rate, requiredTicks).map(issue => `${rate} Hz: ${issue}`));
  }
  if (new Set(presentRuns.map(run => run.epoch)).size !== presentRuns.length)
    issues.push('cadence runs must use distinct Runtime epochs');
  for (const run of presentRuns.slice(1)) {
    if (!same(run.identity, presentRuns[0].identity)) issues.push('build or DATA identity differs across cadence runs');
    if (!same(run.replayIdentity, presentRuns[0].replayIdentity)) issues.push('Replay input identity differs across cadence runs');
    if (!same(cadenceStartComparable(run.start, cadenceDifferenceGroups),
        cadenceStartComparable(presentRuns[0].start, cadenceDifferenceGroups)))
      issues.push('fresh cadence runs did not begin from the same authority and Replay state');
  }
  for (const group of cadenceDifferenceGroups) {
    if (presentRuns.some(run => !run.samples?.[0]?.afterStep?.state?.some(item => item.id === group)))
      issues.push(`declared cadence-dependent authority group is not present: ${group}`);
  }
  if (presentRuns.length === ratesHz.length && issues.length === 0) {
    const base = presentRuns[0];
    for (let index = 0; index < requiredTicks; index++) {
      const baseline = cadenceComparable(base.samples[index], cadenceDifferenceGroups);
      for (const run of presentRuns.slice(1)) {
        if (!same(baseline, cadenceComparable(run.samples[index], cadenceDifferenceGroups)))
          issues.push(`logical tick ${index + 1} diverged under ${run.scheduler.rateHz} Hz callback cadence`);
      }
    }
    for (const run of presentRuns.slice(1)) for (let index = 0; index < requiredTicks; index++) {
      const sample = run.samples[index];
      if (!Array.isArray(sample.drawReceipts) || sample.drawReceipts.length !== DRAW_ALPHAS.length ||
          sample.drawReceipts.some((receipt, drawIndex) => receipt?.alpha !== DRAW_ALPHAS[drawIndex] ||
            receipt?.status !== 'drawn' || receipt?.unchanged !== true ||
            !same(receipt.before, receipt.after) || !isRecord(receipt.audio) ||
            !Array.isArray(receipt.audio.events) || receipt.audio.events.length !== 0 ||
            !Array.isArray(receipt.audio.routes) || receipt.audio.routes.length !== 0 ||
            receipt.audio.uploads !== 0 || receipt.audio.errors !== 0))
        issues.push(`${run.scheduler.rateHz} Hz logical tick ${index + 1} did not retain three unchanged draw-only receipts`);
    }
    if (base.samples.some(sample => !Array.isArray(sample.drawReceipts) || sample.drawReceipts.length !== 0))
      issues.push('60 Hz baseline unexpectedly contains controlled extra draws');
    if (presentRuns.some(run => !run.audioEvents || !run.audioRoutes))
      issues.push('each cadence run must contain a real logical SFX event and route window');
  }
  const audioContradiction=presentRuns.some(run => run.audioEvents>0&&run.audioRoutes!==run.audioEvents);
  const audioDivergence=presentRuns.length===ratesHz.length&&presentRuns.every(run=>Array.isArray(run.samples))&&
    presentRuns.slice(1).some(run=>run.samples.some((sample,index)=>!same(sample?.audio,presentRuns[0].samples[index]?.audio)));
  const audioAvailable=presentRuns.length===ratesHz.length&&presentRuns.every(run=>run.audioEvents>0&&run.audioRoutes>0&&
    run.samples?.length===requiredTicks&&run.samples.every(sample=>isRecord(sample?.audio)));
  const failed = audioContradiction || audioDivergence || presentRuns.some(run=>run.status==='failed') || issues.some(issue =>
    /diverged|differs|duplicate|Expected .* run|another game|another variant|another Replay|another Runtime|more than one|consumed more|timestamp cadence|tickDue flag|did not consume exactly|did not present exactly|did not present its|produced no measured|belongs to another|does not continue|was not bound to one new|timing-ring row does not match|simulation-frame stamp|disagrees with|used custom simulation speed|incorrectly classified|without a successful presentation|was not driven/u.test(issue)) ||
    presentRuns.some(run=>run.scheduler?.mode&&run.scheduler.mode!=='normal-fixed-60hz');
  const check = (id, checkStatus, evidence) => ({id, status: checkStatus, complete: checkStatus === 'passed',
    machineComplete: checkStatus === 'passed', evidence:{game:safeScope?.game??null,variant:safeScope?.variant??null,
      adaptationIds:[id],ratesHz:[...ratesHz],requiredTicks, ...evidence}});
  const logicStatus = failed ? 'failed' : issues.length ? 'incomplete' : 'passed';
  const audioStatus = audioContradiction||audioDivergence ? 'failed' :
    audioAvailable && presentRuns.every(run => run.audioEvents > 0 && run.audioRoutes === run.audioEvents)
      ? 'passed' : 'incomplete';
  const status = failed ? 'failed' : issues.length || audioStatus !== 'passed' ? 'incomplete' : 'passed';
  return {schema:CADENCE_SCHEMA,scope:safeScope,status,issues,ratesHz:[...ratesHz],requiredTicks,
    cadenceDifferenceGroups:[...cadenceDifferenceGroups],runs:presentRuns.map(run=>({rateHz:run.scheduler?.rateHz,
      epoch:run.epoch,callbackCount:run.scheduler?.summary?.callbackCount??null,
      highRefreshPresented:run.scheduler?.summary?.highRefreshPresented??null,
      betweenTickPresentations:run.scheduler?.summary?.betweenTickPresentations??null,
      audioEvents:run.audioEvents??0,audioRoutes:run.audioRoutes??0})),
    checks:[check('DISPLAY-01',logicStatus,{sameAuthorityAtEachTick:logicStatus,productionScheduler:'GameHost::frame',
        extent:'same declared logical-tick window at 60, 120 and 144 Hz'}),
      check('REPLAY-07',logicStatus,{sameReplayStateAtEachLogicalTick:logicStatus,extent:'same declared logical-tick endpoint',
        physicalRefreshVerified:false,naturalTerminalVerified:false}),
      check('AUDIO-06',audioStatus,{sameLogicalEventRouteSequence:audioStatus,extent:'normal-runtime event window',
        rollbackWindowObserved:false,rollbackAudioVerified:false})],
    claims:{productionSchedulerCadenceVerified:logicStatus==='passed',physicalRefreshVerified:false,
      browserCompositorSubmissionMeasured:false,replayNaturalTerminalVerified:false,rollbackAudioVerified:false}};
}

export const PAIRED_REPLAY_ALPHAS = DRAW_ALPHAS;
