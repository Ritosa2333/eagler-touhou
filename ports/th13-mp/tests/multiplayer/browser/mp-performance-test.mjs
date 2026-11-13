import test from 'node:test';
import assert from 'node:assert/strict';
import {
  evaluateMpPerformance,
  verifyProductionWireDigest,
  installUntimedPilot,
} from './mp-performance-eval.mjs';

function makeStream({
  stages = [1, 2],
  activePerStage = 200,
  stepMs = 16.6,
  workMs = 6.0,
  zeroEvery = 10,
  zeroWorkMs = 0.2,
  stageWorkOverride = {},
} = {}) {
  const rows = [];
  let seq = 0;
  let now = 0;
  // Startup loading row (stage 0, source 4) + menu row (stage 1, source 1)
  rows.push({
    sequence: ++seq, begin: now, end: now + 3, milliseconds: 3,
    stage: 0, endStage: 1, phase: 'loading', source: 4,
    simulated: 0, rollback: 0, restores: 0, presentations: 0, presentationAt: null, ok: true,
  });
  now += 16.6;
  rows.push({
    sequence: ++seq, begin: now, end: now + 1, milliseconds: 1,
    stage: stages[0], endStage: stages[0], phase: 'menu', source: 1,
    simulated: 1, rollback: 0, restores: 0, presentations: 1, presentationAt: now + 0.8, ok: true,
  });
  now += 16.6;
  for (const stage of stages) {
    for (let i = 0; i < activePerStage; ++i) {
      const isZero = zeroEvery > 0 && i % zeroEvery === 0;
      const isCatchup = !isZero && i === 5;
      const isRollback = !isZero && i === 15;
      const simulated = isZero ? 0 : isCatchup ? 2 : isRollback ? 3 : 1;
      const rollback = isRollback ? 2 : 0;
      const restores = isRollback ? 1 : 0;
      const ms = isZero ? zeroWorkMs : (stageWorkOverride[stage]?.(i) ?? workMs);
      const end = now + ms;
      rows.push({
        sequence: ++seq,
        begin: now,
        end,
        milliseconds: ms,
        stage,
        endStage: stage,
        phase: 'active',
        source: isZero ? 2 : 1,
        simulated,
        rollback,
        restores,
        presentations: isZero ? 0 : 1,
        presentationAt: isZero ? null : now + ms * 0.9,
        ok: true,
      });
      now = Math.max(end + 0.1, now + (isZero ? 1.0 : (isCatchup ? stepMs * 2 : stepMs)));
    }
  }
  return {
    rows,
    options: {
      expectedStages: stages,
      expectedPhases: ['loading', 'menu', 'active'],
      expectedFirstSequence: 1,
      requireZero: true,
      requireCatchup: true,
      requireRollback: true,
      capture: {first: 1, last: rows.length, count: rows.length},
    },
  };
}

test('healthy 60fps production stream passes all strict gates', () => {
  const {rows, options} = makeStream();
  const res = evaluateMpPerformance(rows, options);
  assert.equal(res.passed, true, JSON.stringify(res.errors));
  assert.equal(res.frameWork.passed, true);
  assert.ok(res.strictP95.overall <= 16.67);
  assert.ok(res.progression.total.forwardFps >= 58);
});

test('negative control: zero-sim wakeups diluting P95 below 16.67ms are rejected by simulatedOnly gate', () => {
  // Make half the callbacks simulated=0 (0.2ms) and 20% of simulated>0 callbacks 22ms:
  // All-callback P95 stays <= 16.67ms (~90% of all rows are <= 6ms),
  // so raw evaluateFrameWork passes, while simulatedOnly P95 is 22ms -> MUST FAIL.
  const {rows, options} = makeStream({
    stages: [1, 2],
    activePerStage: 300,
    zeroEvery: 2,
    stageWorkOverride: {
      1: i => (((i - 1) / 2) % 12 === 0 ? 22 : 6),
      2: i => (((i - 1) / 2) % 12 === 0 ? 22 : 6),
    },
  });
  const res = evaluateMpPerformance(rows, options);
  assert.equal(res.frameWork.passed, true, 'Raw diluted P95 would have passed: ' + JSON.stringify(res.frameWork.errors));
  assert.equal(res.passed, false, 'Anti-dilution gate must reject');
  assert.ok(res.simulatedOnly.total.p95 > 16.67);
  assert.ok(res.errors.some(e => e.includes('Simulated-only overall work p95 exceeds limit')));
});

test('negative control: single-stage simulated-only breach is rejected even when overall passes', () => {
  const {rows, options} = makeStream({
    stages: [1, 2, 3, 4, 5, 6],
    activePerStage: 240,
    zeroEvery: 2,
    stageWorkOverride: {
      4: i => (((i - 1) / 2) % 12 === 0 ? 21 : 5),
    },
  });
  const res = evaluateMpPerformance(rows, options);
  assert.equal(res.frameWork.passed, true);
  assert.equal(res.simulatedOnly.total.p95 <= 16.67, true);
  assert.equal(res.passed, false);
  assert.ok(res.errors.includes('Stage 4 simulated-only work p95 exceeds limit'));
});

test('negative control: low forward FPS (starved simulation) is rejected even when callback P95 is tiny', () => {
  // 30 fps progression (stepMs = 33.3ms) with 4ms callbacks: P95 is 4ms, but game runs at half speed.
  const {rows, options} = makeStream({stepMs: 33.3, workMs: 4.0, zeroEvery: 20});
  const res = evaluateMpPerformance(rows, options);
  assert.equal(res.frameWork.passed, true);
  assert.equal(res.passed, false);
  assert.ok(res.errors.some(e => e.includes('forward progression FPS')));
});

test('negative control: missing required stage or corrupted sequence is rejected', () => {
  const {rows, options} = makeStream({stages: [1, 2]});
  assert.equal(evaluateMpPerformance(rows, {...options, expectedStages: [1, 2, 3]}).passed, false);
  assert.equal(evaluateMpPerformance(rows.slice(1), options).passed, false);
});

test('untimed pilot keeps production onGameFrame inside timed window and pilot outside timed window', () => {
  const order = [];
  const work = {samples: [], errors: [], capture: {first: null, last: null, count: 0}};
  const module = {
    onGameFrame(ok, ms, ticks) {
      order.push(['prodOnGameFrame', ok, ms, ticks]);
    },
  };
  const win = {Module: module, __frameWork: work, order};
  work.observer = module.onGameWorkFrame = row => {
    order.push(['recordWorkRow', row.sequence]);
    work.samples.push({...row});
    if (!work.capture.count) work.capture.first = row.sequence;
    work.capture.last = row.sequence;
    ++work.capture.count;
    const deferred = win.__pilotDeferred;
    if (deferred && deferred.pending !== null) {
      const args = deferred.pending;
      deferred.pending = null;
      deferred.run(...args);
      ++deferred.executed;
    }
  };
  const fakePilot = function fakePilot({seat}) {
    const w = document.querySelector('#gameFrame').contentWindow;
    const m = w.Module;
    const old = m.onGameFrame;
    m.onGameFrame = (ok, ms, ticks) => {
      old?.(ok, ms, ticks);
      w.order.push(['pilotDecision', seat, ok, ms, ticks]);
    };
  };
  const prevDoc = globalThis.document;
  globalThis.document = {querySelector: () => ({contentWindow: win})};
  try {
    installUntimedPilot({seat: 2, pilotSource: String(fakePilot)});
    // Simulate C++ drive_one_frame: th13_browser_frame -> finish -> th13_browser_work_frame
    module.onGameFrame(1, 5.5, 1);
    assert.deepEqual(order, [['prodOnGameFrame', 1, 5.5, 1]], 'Pilot must NOT run before work row is recorded');
    module.onGameWorkFrame({sequence: 1, begin: 10, end: 15.5, milliseconds: 5.5});
    assert.deepEqual(order, [
      ['prodOnGameFrame', 1, 5.5, 1],
      ['recordWorkRow', 1],
      ['pilotDecision', 2, 1, 5.5, 1],
    ]);
    assert.equal(win.__pilotDeferred.executed, 1);
    assert.deepEqual(work.errors, []);

    // Unpaired onGameFrame must flag an error
    module.onGameFrame(1, 4.0, 1);
    module.onGameFrame(1, 4.2, 1);
    assert.equal(work.errors.length, 1);
    assert.match(work.errors[0], /Unconsumed deferred pilot/);
  } finally {
    globalThis.document = prevDoc;
  }
});

test('verifyProductionWireDigest enforces audit off and active mismatch-free wire digests', () => {
  const okRuntime = {
    error: '',
    audit: {frames: [], overflow: 0},
    diagnostic: {confirmedThrough: 240, wireDigest: {sent: 4, received: 8}},
  };
  assert.equal(verifyProductionWireDigest({runtime: okRuntime, seat: 0}).passed, true);

  assert.equal(verifyProductionWireDigest({
    runtime: {...okRuntime, audit: {frames: [{frame: 0}], overflow: 0}},
    seat: 0,
  }).passed, false);

  assert.equal(verifyProductionWireDigest({
    runtime: {
      ...okRuntime,
      diagnostic: {confirmedThrough: 240, wireDigest: {sent: 4, received: 8, mismatch: {frame: 120}}},
    },
    seat: 1,
  }).passed, false);

  assert.equal(verifyProductionWireDigest({
    runtime: {...okRuntime, diagnostic: {confirmedThrough: 240, wireDigest: {sent: 0, received: 0}}},
    seat: 2,
  }).passed, false);
});
