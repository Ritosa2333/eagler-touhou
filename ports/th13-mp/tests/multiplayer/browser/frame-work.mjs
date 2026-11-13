// Complete work observations. Sources: 1=RAF, 2=network wake, 3=manual
// frame, 4=prepare_loading, 5=initialize, 6=draw_startup, 7=room_preview,
// 8=mp_start, 9=room_return, 10=mp_start waiting at the READY barrier.
// Startup sources cover
// each synchronous exported call in full and never overlap a frame callback.
// The caller persists every sample and supplies the required coverage.
const phases = new Set(['active', 'menu', 'loading', 'suspended', 'paused']);
const distribution = values => {
  if (!values.length) return {count: 0};
  const sorted = [...values].sort((a, b) => a - b);
  const percentile = p => sorted[Math.ceil(sorted.length * p) - 1];
  return {count: sorted.length, min: sorted[0], p50: percentile(.5),
    p95: percentile(.95), p99: percentile(.99), max: sorted.at(-1)};
};

export function evaluateFrameWork(samples, {expectedStages, expectedPhases, expectedFirstSequence,
  requireZero, requireCatchup, requireRollback, capture} = {}) {
  const errors = [];
  const limitMs = 16.67;
  if (!Array.isArray(expectedStages) || !expectedStages.length ||
      !expectedStages.every(Number.isInteger)) errors.push('Required stage coverage missing');
  if (!Array.isArray(expectedPhases) || !expectedPhases.length ||
      !expectedPhases.every(phase => phases.has(phase))) errors.push('Required phase coverage missing');
  if (!Number.isSafeInteger(expectedFirstSequence) || expectedFirstSequence < 1)
    errors.push('Expected first sequence missing');
  if (![requireZero, requireCatchup, requireRollback].every(value => typeof value === 'boolean'))
    errors.push('Explicit zero, catchup and rollback requirements missing');
  if (!capture || !Number.isSafeInteger(capture.first) || !Number.isSafeInteger(capture.last) ||
      !Number.isSafeInteger(capture.count) || capture.count < 1 || capture.last < capture.first)
    errors.push('Missing capture boundaries');
  if (!Array.isArray(samples) || !samples.length) {
    return {passed: false, errors: [...errors, 'No work samples'], total: {count: 0}, stages: {},
      phases: {}, presentationIntervals: {count: 0}, ambiguousPresentationIntervals: 0};
  }
  const stageRows = new Map(), phaseRows = new Map(), sourceRows = new Map(), durations = [], intervals = [];
  if (capture && (samples.length !== capture.count || samples[0]?.sequence !== capture.first ||
      samples.at(-1)?.sequence !== capture.last || capture.last - capture.first + 1 !== capture.count ||
      capture.first !== expectedFirstSequence))
    errors.push('Capture boundaries do not match complete sample sequence');
  let previous = null, previousPresentation = null, ambiguous = 0;
  const coverage = {zero: 0, catchup: 0, rollback: 0, restores: 0};
  for (const [index, row] of samples.entries()) {
    if (!row || typeof row !== 'object') { errors.push(`Invalid sample ${index}`); continue; }
    const ints = ['sequence', 'stage', 'endStage', 'source', 'simulated', 'rollback', 'restores', 'presentations'];
    if (ints.some(key => !Number.isSafeInteger(row[key])) || row.sequence < 1 || row.stage < 0 ||
        row.endStage < 0 || ![1, 2, 3, 4, 5, 6, 7, 8, 9, 10].includes(row.source) || row.simulated < 0 ||
        row.rollback < 0 || row.rollback > row.simulated || row.restores < 0 || row.presentations < 0 ||
        (row.source >= 4 && (row.phase !== 'loading' || row.simulated !== 0 || row.rollback !== 0)) ||
        (row.stage === 0 && row.phase === 'active') ||
        !phases.has(row.phase) || typeof row.ok !== 'boolean' ||
        ![row.begin, row.end, row.milliseconds].every(Number.isFinite) ||
        row.begin < 0 || row.end < row.begin || Math.abs(row.milliseconds - (row.end - row.begin)) > 1e-7 ||
        (row.presentationAt !== null && (!Number.isFinite(row.presentationAt) || !row.presentations ||
          row.presentationAt < row.begin || row.presentationAt > row.end))) {
      errors.push(`Invalid sample ${index}`); continue;
    }
    if (previous && (row.sequence !== previous.sequence + 1 || row.begin < previous.end))
      errors.push(`Missing, duplicate or reordered sample ${index}`);
    previous = row;
    if (!row.ok) errors.push(`Failed work sample ${row.sequence}`);
    if (row.simulated === 0) ++coverage.zero;
    if (row.simulated > 1) ++coverage.catchup;
    coverage.rollback += row.rollback;
    coverage.restores += row.restores;
    durations.push(row.milliseconds);
    if (!stageRows.has(row.stage)) stageRows.set(row.stage, []);
    stageRows.get(row.stage).push(row.milliseconds);
    if (!phaseRows.has(row.phase)) phaseRows.set(row.phase, []);
    phaseRows.get(row.phase).push(row.milliseconds);
    if (!sourceRows.has(row.source)) sourceRows.set(row.source, []);
    sourceRows.get(row.source).push(row.milliseconds);
    if (row.presentations) {
      if (row.presentations === 1 && row.presentationAt !== null && previousPresentation !== null)
        intervals.push(row.presentationAt - previousPresentation);
      if (row.presentations !== 1 || row.presentationAt === null) ++ambiguous;
      previousPresentation = row.presentations === 1 ? row.presentationAt : null;
    }
  }
  for (const stage of expectedStages ?? []) if (!stageRows.has(stage)) errors.push(`Missing stage ${stage}`);
  for (const phase of expectedPhases ?? [])
    if (!phaseRows.has(phase)) errors.push(`Missing phase ${phase}`);
  for (const [kind, required] of Object.entries({zero: requireZero, catchup: requireCatchup,
      rollback: requireRollback, restores: requireRollback}))
    if (required && !coverage[kind]) errors.push(`Missing ${kind} work`);
  const total = distribution(durations);
  const stages = Object.fromEntries([...stageRows].map(([key, values]) => [key, distribution(values)]));
  const phaseStats = Object.fromEntries([...phaseRows].map(([key, values]) => [key, distribution(values)]));
  const sources = Object.fromEntries([...sourceRows].map(([key, values]) => [key, distribution(values)]));
  if (total.count && total.p95 > limitMs) errors.push('Overall work p95 exceeds limit');
  for (const [stage, stats] of Object.entries(stages))
    // Zero denotes startup before a world exists. Keep every such duration
    // in the overall distribution and the phase/source breakdown, without
    // inventing a separate "Stage 0" in the game's required stage list.
    if (+stage > 0 && stats.p95 > limitMs) errors.push(`Stage ${stage} work p95 exceeds limit`);
  return {passed: errors.length === 0, errors, total, stages, phases: phaseStats, sources, coverage,
    requirements: {expectedStages, expectedPhases, expectedFirstSequence,
      requireZero, requireCatchup, requireRollback},
    presentationIntervals: distribution(intervals), ambiguousPresentationIntervals: ambiguous};
}

// Installs exactly one observer, preserving an existing observer and every row.
// Stop is explicit so the caller controls evidence boundaries and persistence.
export function installFrameWorkCapture(module) {
  if (!module || typeof module !== 'object') throw new TypeError('Module required');
  const samples = [], previous = module.onGameWorkFrame;
  const capture = {first: null, last: null, count: 0};
  let stopped = false;
  const observer = row => {
    samples.push(row && typeof row === 'object' ? {...row} : row);
    if (capture.count === 0) capture.first = row?.sequence;
    capture.last = row?.sequence;
    ++capture.count;
    if (typeof previous === 'function') previous(row);
  };
  module.onGameWorkFrame = observer;
  return {
    samples, capture,
    stop() {
      if (stopped) throw new Error('Work capture already stopped');
      if (module.onGameWorkFrame !== observer) throw new Error('Work observer replaced during capture');
      stopped = true;
      module.onGameWorkFrame = previous;
      return samples;
    },
  };
}
