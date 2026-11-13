// Production multiplayer performance evaluation and untimed pilot bridge.
// Wraps unmodified evaluateFrameWork with anti-dilution gates:
//   1. Callbacks with simulated > 0 must independently satisfy P95 <= 16.67ms
//      overall and in every required stage (prevents zero-sim wakeups from
//      diluting the P95 percentile).
//   2. Active gameplay forward progression rate ((simulated - rollback) / wall)
//      must stay >= minForwardFps overall and per stage (prevents passing P95
//      by starving simulation throughput).
//   3. Production wire digests (every 60 confirmed frames) must remain active
//      and mismatch-free while per-frame audit stays disabled.
import {evaluateFrameWork} from './frame-work.mjs';

const LIMIT_MS = 16.67;

export function distribution(values) {
  if (!values.length) return {count: 0};
  const sorted = [...values].sort((a, b) => a - b);
  const percentile = p => sorted[Math.ceil(sorted.length * p) - 1];
  return {
    count: sorted.length,
    min: sorted[0],
    p50: percentile(0.5),
    p95: percentile(0.95),
    p99: percentile(0.99),
    max: sorted.at(-1),
  };
}

function computeActiveProgression(samples) {
  // Accumulate contiguous phase === 'active' segments per stage > 0 so pauses,
  // loading screens and menus between stages are excluded, while every stall or
  // frame drop during active gameplay is included in wall time.
  const byStage = new Map();
  let current = null;
  const flush = () => {
    if (!current) return;
    const entry = byStage.get(current.stage);
    entry.wallMs += Math.max(0, current.end - current.begin);
    current = null;
  };
  for (const row of samples) {
    if (!row || typeof row !== 'object' || !Number.isSafeInteger(row.stage) ||
        !Number.isFinite(row.begin) || !Number.isFinite(row.end)) {
      flush();
      continue;
    }
    if (row.phase === 'active' && row.stage > 0) {
      if (!byStage.has(row.stage)) {
        byStage.set(row.stage, {
          activeCallbacks: 0,
          simulatedCallbacks: 0,
          simulatedFrames: 0,
          rollbackFrames: 0,
          forwardFrames: 0,
          restores: 0,
          wallMs: 0,
        });
      }
      const entry = byStage.get(row.stage);
      ++entry.activeCallbacks;
      if (row.simulated > 0) ++entry.simulatedCallbacks;
      entry.simulatedFrames += row.simulated;
      entry.rollbackFrames += row.rollback;
      entry.forwardFrames += row.simulated - row.rollback;
      entry.restores += row.restores;
      if (current && current.stage === row.stage) {
        current.end = row.end;
      } else {
        flush();
        current = {stage: row.stage, begin: row.begin, end: row.end};
      }
    } else {
      flush();
    }
  }
  flush();

  const stages = {};
  let totalActiveCallbacks = 0;
  let totalSimulatedCallbacks = 0;
  let totalSimulatedFrames = 0;
  let totalRollbackFrames = 0;
  let totalForwardFrames = 0;
  let totalRestores = 0;
  let totalWallMs = 0;

  for (const [stage, entry] of [...byStage.entries()].sort((a, b) => a[0] - b[0])) {
    const wallSeconds = entry.wallMs / 1000;
    const forwardFps = wallSeconds > 0 ? entry.forwardFrames / wallSeconds : 0;
    stages[stage] = {
      ...entry,
      wallSeconds,
      forwardFps,
    };
    totalActiveCallbacks += entry.activeCallbacks;
    totalSimulatedCallbacks += entry.simulatedCallbacks;
    totalSimulatedFrames += entry.simulatedFrames;
    totalRollbackFrames += entry.rollbackFrames;
    totalForwardFrames += entry.forwardFrames;
    totalRestores += entry.restores;
    totalWallMs += entry.wallMs;
  }

  const totalWallSeconds = totalWallMs / 1000;
  return {
    total: {
      activeCallbacks: totalActiveCallbacks,
      simulatedCallbacks: totalSimulatedCallbacks,
      simulatedFrames: totalSimulatedFrames,
      rollbackFrames: totalRollbackFrames,
      forwardFrames: totalForwardFrames,
      restores: totalRestores,
      wallMs: totalWallMs,
      wallSeconds: totalWallSeconds,
      forwardFps: totalWallSeconds > 0 ? totalForwardFrames / totalWallSeconds : 0,
    },
    stages,
  };
}

export function evaluateMpPerformance(samples, {
  expectedStages,
  expectedPhases,
  expectedFirstSequence,
  requireZero,
  requireCatchup,
  requireRollback,
  capture,
  minForwardFps = 55,
  minActiveCallbacksPerStage = 120,
} = {}) {
  const frameWork = evaluateFrameWork(samples, {
    expectedStages,
    expectedPhases,
    expectedFirstSequence,
    requireZero,
    requireCatchup,
    requireRollback,
    capture,
  });
  const errors = [...frameWork.errors];
  const validRows = Array.isArray(samples)
    ? samples.filter(row => row && typeof row === 'object' &&
        Number.isSafeInteger(row.simulated) && Number.isSafeInteger(row.stage) &&
        Number.isFinite(row.milliseconds))
    : [];

  const simDurations = [];
  const simStageMap = new Map();
  for (const row of validRows) {
    if (row.simulated > 0) {
      simDurations.push(row.milliseconds);
      if (!simStageMap.has(row.stage)) simStageMap.set(row.stage, []);
      simStageMap.get(row.stage).push(row.milliseconds);
    }
  }
  const simulatedTotal = distribution(simDurations);
  const simulatedStages = Object.fromEntries(
    [...simStageMap.entries()].map(([stage, values]) => [stage, distribution(values)]),
  );

  if (!simulatedTotal.count) {
    errors.push('No simulated-only work samples');
  } else if (simulatedTotal.p95 > LIMIT_MS) {
    errors.push('Simulated-only overall work p95 exceeds limit');
  }

  for (const stage of expectedStages ?? []) {
    const stats = simulatedStages[stage];
    if (!stats?.count) {
      errors.push(`Missing simulated-only work in stage ${stage}`);
    } else if (+stage > 0 && stats.p95 > LIMIT_MS) {
      errors.push(`Stage ${stage} simulated-only work p95 exceeds limit`);
    }
  }

  const progression = computeActiveProgression(validRows);
  if (!Number.isFinite(minForwardFps) || minForwardFps <= 0) {
    errors.push('Invalid minForwardFps requirement');
  } else {
    for (const stage of expectedStages ?? []) {
      if (+stage <= 0) continue;
      const stageProg = progression.stages[stage];
      if (!stageProg || stageProg.activeCallbacks < minActiveCallbacksPerStage) {
        errors.push(`Stage ${stage} active callback coverage below ${minActiveCallbacksPerStage}`);
      } else if (stageProg.forwardFrames <= 0 || stageProg.wallSeconds <= 0) {
        errors.push(`Stage ${stage} has no forward progression`);
      } else if (stageProg.forwardFps < minForwardFps) {
        errors.push(`Stage ${stage} forward progression FPS (${stageProg.forwardFps.toFixed(2)}) below ${minForwardFps}`);
      }
    }
    if (progression.total.activeCallbacks > 0 && progression.total.forwardFps < minForwardFps) {
      errors.push(`Overall active forward progression FPS (${progression.total.forwardFps.toFixed(2)}) below ${minForwardFps}`);
    }
  }

  const strictStages = {};
  for (const stage of new Set([...Object.keys(frameWork.stages), ...Object.keys(simulatedStages)])) {
    const a = frameWork.stages[stage]?.p95;
    const b = simulatedStages[stage]?.p95;
    strictStages[stage] = Number.isFinite(a) && Number.isFinite(b) ? Math.max(a, b) : (a ?? b ?? null);
  }
  const strictOverall = Number.isFinite(frameWork.total.p95) && Number.isFinite(simulatedTotal.p95)
    ? Math.max(frameWork.total.p95, simulatedTotal.p95)
    : (frameWork.total.p95 ?? simulatedTotal.p95 ?? null);

  return {
    passed: errors.length === 0,
    errors,
    limitMs: LIMIT_MS,
    minForwardFps,
    strictP95: {overall: strictOverall, stages: strictStages},
    frameWork,
    simulatedOnly: {total: simulatedTotal, stages: simulatedStages},
    progression,
  };
}

export function verifyProductionWireDigest({runtime, seat, minConfirmedForWireDigest = 180} = {}) {
  const errors = [];
  if (!runtime || typeof runtime !== 'object') {
    return {passed: false, errors: [`Seat ${seat}: missing runtime state`]};
  }
  if (runtime.error) {
    errors.push(`Seat ${seat}: native network error: ${runtime.error}`);
  }
  if ((runtime.audit?.frames?.length ?? 0) > 0 || (runtime.audit?.overflow ?? 0) > 0) {
    errors.push(`Seat ${seat}: per-frame audit must remain disabled in production performance mode`);
  }
  const wireDigest = runtime.diagnostic?.wireDigest;
  if (!wireDigest || typeof wireDigest !== 'object') {
    errors.push(`Seat ${seat}: missing diagnostic wireDigest telemetry`);
  } else {
    if (wireDigest.mismatch) {
      errors.push(`Seat ${seat}: wire digest mismatch at frame ${wireDigest.mismatch.frame}`);
    }
    const confirmed = runtime.diagnostic?.confirmedThrough;
    if (Number.isInteger(confirmed) && confirmed >= minConfirmedForWireDigest) {
      if (!(wireDigest.sent > 0)) {
        errors.push(`Seat ${seat}: no wire digests sent by confirmed frame ${confirmed}`);
      }
      if (!(wireDigest.received > 0)) {
        errors.push(`Seat ${seat}: no wire digests received by confirmed frame ${confirmed}`);
      }
    }
  }
  return {passed: errors.length === 0, errors};
}

// Serialized into the page. Keeps shell.mjs's production onGameFrame inside the
// timed C++ callback window while deferring the test pilot's debug JSON reads
// until onGameWorkFrame has already recorded the callback's end timestamp.
export function installUntimedPilot({seat, pilotSource}) {
  const w = document.querySelector('#gameFrame')?.contentWindow;
  const m = w?.Module;
  if (!w || !m || !w.__frameWork || m.onGameWorkFrame !== w.__frameWork.observer) {
    throw Error('Work observer must be active before installing untimed pilot');
  }
  if (w.__pilotDeferred) {
    throw Error('Untimed pilot already installed');
  }
  const prodOnGameFrame = m.onGameFrame;
  m.onGameFrame = null;
  const install = (0, eval)(`(${pilotSource})`);
  install({seat});
  const pilotOnGameFrame = m.onGameFrame;
  if (typeof pilotOnGameFrame !== 'function') {
    m.onGameFrame = prodOnGameFrame;
    throw Error('Pilot did not install an onGameFrame handler');
  }
  const deferred = (w.__pilotDeferred = {
    pending: null,
    executed: 0,
    run: pilotOnGameFrame,
  });
  m.onGameFrame = (ok, ms, ticks) => {
    if (deferred.pending !== null) {
      w.__frameWork.errors.push('Unconsumed deferred pilot tick before next onGameFrame');
    }
    deferred.pending = [ok, ms, ticks];
    return prodOnGameFrame?.(ok, ms, ticks);
  };
}
