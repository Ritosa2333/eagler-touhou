import assert from 'node:assert/strict';
import {
  assertTouchMotionSamples,
  assertTouchReleaseSamples,
  isTouchMotionTickEligible,
  mapFloat32TouchDragTarget,
  isTouchMotionTargetReached,
  predictTouchMotionStep,
  TOUCH_TRACE_POSITION_EPSILON,
  TOUCH_TRACE_SCHEMA,
} from './touch-motion-contract.mjs';

const TRACE_CAPACITY = 512;
const POLL_MS = 8;
const SCENE_TIMEOUT_MS = 2000;
const SETTLE_TICKS = 3;
const REQUIRED_MOTION_TICKS = 3;
const EPSILON = TOUCH_TRACE_POSITION_EPSILON;

class SceneIncomplete extends Error {}

function isBitMask(value) {
  return Number.isSafeInteger(value) && value > 0 && value <= 0xffff && (value & (value - 1)) === 0;
}

function validQueuedMovementPolicy(policy) {
  const ownership = policy?.keyboardTouchOwnership;
  return policy?.semantics === 'queued-for-next-movement' &&
    typeof policy.observationSource === 'string' && policy.observationSource.length > 0 &&
    typeof policy.observationPhase === 'string' && policy.observationPhase.length > 0 &&
    isBitMask(policy.directionMasks?.ArrowRight) && isBitMask(policy.directionMasks?.ArrowLeft) &&
    policy.directionMasks.ArrowRight !== policy.directionMasks.ArrowLeft &&
    isBitMask(policy.focusMask) &&
    !(policy.focusMask & (policy.directionMasks.ArrowRight | policy.directionMasks.ArrowLeft)) &&
    policy.maxMovementTicks === 1 &&
    ['independent-input-owners', 'direction-key-clears-touch-motion'].includes(ownership?.semantics) &&
    Array.isArray(ownership.source) && ownership.source.length > 0 &&
    ownership.source.every(source => typeof source === 'string' && source.length > 0) &&
    policy.touchFocusTransitions?.semantics === 'action-queue-then-next-movement' &&
    policy.touchFocusTransitions.maxMovementTicks === 1;
}

function close(actual, expected, label, epsilon = EPSILON) {
  assert.ok(Number.isFinite(actual), `${label} must be finite`);
  assert.ok(Math.abs(actual - expected) <= epsilon,
    `${label}: expected ${expected}, received ${actual} (tolerance ${epsilon})`);
}

function delta(sample) {
  return { x: sample.afterX - sample.beforeX, y: sample.afterY - sample.beforeY };
}

function position(sample) {
  return { x: sample.afterX, y: sample.afterY };
}

function atPosition(sample, point) {
  return Math.abs(sample.targetX - point.x) <= EPSILON &&
    Math.abs(sample.targetY - point.y) <= EPSILON;
}

function assertNativeFocus(sample, focused, label) {
  if (sample.abiVersion === 2) {
    assert.equal(Boolean(sample.flags & 4), focused, `${label}: native focus state differs`);
  }
}

function queueDirectionMask(policy) {
  return policy.directionMasks.ArrowRight | policy.directionMasks.ArrowLeft;
}

function queueFocusValue(boundary, policy) {
  return Boolean(boundary.value & policy.focusMask);
}

function assertFocusQueueAfterAction(before, after, focused, policy, label = 'touch focus') {
  assert.equal(after.value & queueDirectionMask(policy), before.value & queueDirectionMask(policy),
    `${label}: queued direction masks changed across a touch-focus transition`);
  assert.equal(after.value & policy.focusMask, focused ? policy.focusMask : 0,
    `${label}: next native movement queue has the wrong focus mask`);
}

function assertTouchCancelTickStationary(sample, point, label = 'touch cancel') {
  assert.equal(sample.mode, 0, `${label}: touch movement remained active after cancel`);
  for (const [field, value] of [['beforeX', point.x], ['targetX', point.x], ['afterX', point.x],
    ['beforeY', point.y], ['targetY', point.y], ['afterY', point.y]]) {
    close(sample[field], value, `${label}: position drift (${field})`);
  }
}

function assertSample(sample, { abiVersion, localSeat } = {}) {
  for (const field of ['beforeX', 'beforeY', 'targetX', 'targetY', 'afterX', 'afterY',
    'speed', 'scaleX', 'scaleY', 'sensitivity']) {
    assert.ok(Number.isFinite(sample[field]), `Native ${field} must be finite`);
  }
  assert.ok(Number.isSafeInteger(sample.sequence) && sample.sequence > 0,
    'Native trace sequence must be a positive integer');
  assert.ok(Number.isSafeInteger(sample.frame) && sample.frame >= 0,
    'Native movement frame must be a non-negative integer');
  assert.ok(sample.mode === 0 || sample.mode === 1 || sample.mode === 2,
    `Native touch mode is invalid: ${sample.mode}`);
  assert.ok(Number.isInteger(sample.flags) && sample.flags >= 0,
    'Native gameplay flags must be a non-negative integer');
  if (sample.abiVersion !== undefined) {
    assert.equal(sample.abiVersion, abiVersion, 'Native touch row ABI differs from its trace stream');
  }
  if (sample.executionKind !== undefined) {
    assert.equal(sample.executionKind, 0,
      'Touch trace row executionKind must be forward (0); replay/resimulation rows are excluded');
  }
  if (localSeat !== null && localSeat !== undefined && sample.localSeat !== undefined) {
    assert.equal(sample.localSeat, localSeat, 'Native touch row changed its local-seat owner');
  }
  if (abiVersion >= 2) {
    assert.equal(sample.abiVersion, abiVersion, 'ABI v2 touch rows must identify their ABI version');
    assert.equal(sample.executionKind, 0,
      'ABI v2 touch rows must be forward execution; replay/resimulation rows are excluded');
    assert.equal(sample.localSeat, localSeat, 'ABI v2 touch row seat differs from its trace stream');
  }
}

function validateSnapshot(trace, expected = {}) {
  assert.equal(trace.schema, TOUCH_TRACE_SCHEMA, 'Native touch trace schema differs');
  const abiVersion = trace.abiVersion ?? 1;
  assert.ok(abiVersion === 1 || abiVersion === 2, `Unsupported touch trace ABI version: ${abiVersion}`);
  if (trace.executionKind !== undefined) {
    assert.equal(trace.executionKind, 0,
      'Touch trace stream executionKind must be forward (0); replay/resimulation streams are excluded');
  }
  if (abiVersion >= 2) {
    assert.equal(trace.executionKind, 0,
      'ABI v2 touch scenes require forward execution; replay/resimulation streams are excluded');
    assert.ok(Number.isSafeInteger(trace.localSeat) && trace.localSeat >= 0,
      'ABI v2 touch trace must identify its local seat');
  }
  if (expected.abiVersion !== undefined) {
    assert.equal(abiVersion, expected.abiVersion, 'Touch trace ABI changed during the scene');
  }
  if (expected.localSeat !== undefined && expected.localSeat !== null) {
    assert.equal(trace.localSeat, expected.localSeat, 'Touch trace changed local seat during the scene');
  }
  assert.ok(Number.isSafeInteger(trace.total) && trace.total >= 0,
    'Native touch trace total must be a non-negative integer');
  assert.ok(Array.isArray(trace.samples), 'Native touch trace samples are missing');
  assert.ok(trace.samples.length <= TRACE_CAPACITY, 'Native touch trace exceeds its 512-row ring');
  assert.equal(trace.samples.length, Math.min(trace.total, TRACE_CAPACITY),
    'Native touch trace ring count differs from its total');
  if (trace.samples.length) {
    assert.equal(trace.samples.at(-1).sequence, trace.total,
      'Native touch trace does not end at its reported total');
    assert.equal(trace.samples[0].sequence, trace.total - trace.samples.length + 1,
      'Native touch trace starts after a missing sequence');
  }
  for (let i = 0; i < trace.samples.length; i++) {
    const sample = trace.samples[i];
    assertSample(sample, { abiVersion, localSeat: trace.localSeat });
    if (i) {
      assert.equal(sample.sequence, trace.samples[i - 1].sequence + 1,
        'Native touch trace snapshot has a sequence gap');
      assert.equal(sample.frame, (trace.samples[i - 1].frame + 1) >>> 0,
        'Native touch trace snapshot has a movement-frame gap');
    }
  }
}

function requiredAdapter(adapter) {
  for (const name of ['controls', 'pointer', 'key', 'wait']) {
    assert.equal(typeof adapter?.[name], 'function', `Touch scene adapter requires ${name}()`);
  }
}

function neutralControls(sensitivityPercent = 100) {
  return {
    fireEnabled: false,
    focusEnabled: false,
    bombSerial: 0,
    escapeSerial: 0,
    joystickX: 0,
    joystickY: 0,
    touchSensitivity: sensitivityPercent,
  };
}

/**
 * Run a compact direct-touch acceptance group in the already-running Runtime.
 * The adapter owns only production-boundary operations; this module never
 * launches, restarts, reads a touch limiter, or derives expectations from the
 * trace target.
 */
export async function runTouchMotionSceneGroup(adapter, {
  mode,
  inputSpace = { width: 640, height: 480 },
  precision,
  inputCommand,
  inputBoundary = 'unknown',
  inputBoundarySource = null,
  queuedMovementPolicy = null,
  focusMode = 'hold-button',
  scenarioSet = 'all',
} = {}) {
  const misplacedOptions = ['mode', 'inputSpace', 'precision', 'inputCommand', 'inputBoundary', 'inputBoundarySource', 'queuedMovementPolicy', 'focusMode', 'scenarioSet']
    .filter(name => Object.hasOwn(adapter ?? {}, name));
  if (misplacedOptions.length) {
    throw new TypeError(`Touch motion scene options belong in the second argument, not the IO adapter: ${misplacedOptions.join(', ')}`);
  }

  const joystickMode = mode === 'joystick-free';
  const traceMode = joystickMode ? 1 : mode;
  const evidence = {
    schema: 'eagler-touhou/touch-motion-scenes/1',
    mode,
    inputSpace: { ...inputSpace },
    inputCommand: inputCommand ? structuredClone(inputCommand) : null,
    inputBoundary,
    inputBoundarySource,
    queuedMovementPolicy: queuedMovementPolicy ? structuredClone(queuedMovementPolicy) : null,
    precision: precision ? { ...precision } : {
      unitsPerPixel: 1, rounding: 'float', scalePlacement: 'before-limit',
    },
    focusMode,
    scenarioSet,
    timeoutMs: SCENE_TIMEOUT_MS,
    pollMs: POLL_MS,
    ringCapacity: TRACE_CAPACITY,
    setup: [],
    calibration: {},
    scenarios: [],
    keyboardTransitions: [],
    pointerCommands: [],
    keyboardDrainRows: [],
    nativeMode: traceMode,
    nativeRows: [],
    trace: { available: null, abiVersion: null, localSeat: null, initialTotal: null, initialRingFull: null, finalTotal: null },
  };

  if (typeof adapter?.readTrace !== 'function') {
    evidence.trace.available = false;
    return { checkStatus: 'incomplete', reason: 'Native touch trace reader is unavailable', evidence };
  }

  let cursor = null;
  let lastFrame = null;
  let initialTotal = null;
  let traceAbiVersion = null;
  let traceLocalSeat = null;
  let pointerId = 700;
  const pointerOwner=adapter,pointerIO=adapter.pointer;
  if(typeof pointerIO==='function')adapter={...adapter,pointer:async(type,id,x,y)=>{
    const beforeSequence=cursor,result=await pointerIO.call(pointerOwner,type,id,x,y);
    evidence.pointerCommands.push({type,id,x,y,beforeSequence,result:result??null});return result;
  }};
  let latestSnapshot = null;
  const requireQueuedMovementBoundary = inputBoundary === 'queued-for-next-movement';
  const keyboardTouchTakeoverEnabled = queuedMovementPolicy?.keyboardTouchOwnership?.semantics ===
    'direction-key-clears-touch-motion';

  const validNextMovementInputBoundary = (boundary, total) => requireQueuedMovementBoundary &&
    boundary?.available === true &&
    boundary.semantics === queuedMovementPolicy.semantics &&
    boundary.observationSource === queuedMovementPolicy.observationSource &&
    boundary.observationPhase === queuedMovementPolicy.observationPhase &&
    Number.isInteger(boundary.value) && boundary.value >= 0 && boundary.value <= 0xffff &&
    boundary.traceTotal === total;

  const sendKey = async (code, down, { requireTransition = false } = {}) => {
    const acknowledgement = await adapter.key(code, down);
    if (acknowledgement?.acknowledged === false) {
      throw new Error(`Runtime keyboard bridge did not acknowledge ${down ? 'down' : 'up'} for ${code}`);
    }
    if (requireTransition) {
      assert.equal(acknowledgement?.acknowledged, true,
        `Runtime keyboard bridge did not acknowledge ${down ? 'down' : 'up'} for ${code}`);
      assert.equal(acknowledgement.changed, true,
        `Runtime keyboard bridge did not change held state for ${code} ${down ? 'down' : 'up'}`);
      if (acknowledgement.stableTraceBoundary !== true) {
        if (down) {
          try { await adapter.key(code, false); } catch {}
        }
        throw new SceneIncomplete('Strict keyboard calibration requires a stopped native clock and an acknowledged trace boundary');
      }
      if (requireQueuedMovementBoundary &&
          (!validNextMovementInputBoundary(acknowledgement.nextMovementInputBoundary,
            acknowledgement.nativeTraceTotal) || acknowledgement.nativeTraceTotal !== cursor)) {
        if (down) {
          try { await adapter.key(code, false); } catch {}
        }
        throw new SceneIncomplete('Queued-movement keyboard release requires a stable trace total and declared input observation at each key boundary');
      }
    }
    evidence.keyboardTransitions.push({ code, down, acknowledgement: acknowledgement ?? null });
    return acknowledgement;
  };

  const snapshot = async () => {
    const value = await adapter.readTrace();
    if (value?.available === false) {
      evidence.trace.available = false;
      throw new SceneIncomplete(value.reason ?? 'Native touch trace is unavailable');
    }
    if (!value || value.available === false) {
      evidence.trace.available = false;
      throw new SceneIncomplete('Native touch trace is unavailable');
    }
    latestSnapshot = value;
    validateSnapshot(value, { abiVersion: traceAbiVersion ?? undefined, localSeat: traceLocalSeat });
    if (requireQueuedMovementBoundary && cursor !== null &&
        (value.stableTraceBoundary !== true ||
         !validNextMovementInputBoundary(value.nextMovementInputBoundary, value.total))) {
      throw new SceneIncomplete('Queued-movement keyboard release requires a stable native input observation');
    }
    evidence.trace.available = true;
    if (requireQueuedMovementBoundary &&
        !validNextMovementInputBoundary(value.nextMovementInputBoundary, value.total)) {
      throw new SceneIncomplete('Queued-movement keyboard release requires the declared input observation before keyboard calibration');
    }
    if (cursor === null) {
      traceAbiVersion = value.abiVersion ?? 1;
      traceLocalSeat = value.localSeat ?? null;
      evidence.trace.abiVersion = traceAbiVersion;
      evidence.trace.localSeat = traceLocalSeat;
      cursor = value.total;
      lastFrame = value.samples.at(-1)?.frame ?? null;
      initialTotal = value.total;
      evidence.trace.initialTotal = value.total;
      evidence.trace.initialRingFull = value.samples.length === TRACE_CAPACITY;
      return { value, added: [] };
    }

    assert.equal(value.abiVersion ?? 1, traceAbiVersion, 'Touch trace ABI changed during the scene');
    if (traceLocalSeat !== null) {
      assert.equal(value.localSeat, traceLocalSeat, 'Touch trace switched local-seat streams during the scene');
    }

    const missing = value.total - cursor;
    assert.ok(missing >= 0, 'Native touch trace total moved backwards');
    assert.ok(missing <= TRACE_CAPACITY, 'Native touch trace ring overflowed before sampling');
    const added = value.samples.filter(sample => sample.sequence > cursor);
    assert.equal(added.length, missing, 'Native touch trace lost rows since the previous poll');
    for (const sample of added) {
      assert.equal(sample.sequence, cursor + 1, 'Native touch trace sequence is missing or repeated');
      if (lastFrame !== null) {
        assert.equal(sample.frame, (lastFrame + 1) >>> 0, 'Native movement tick is missing or repeated');
      }
      if (initialTotal !== null) {
        assert.ok(value.total - initialTotal <= TRACE_CAPACITY,
          'Touch scene exceeded the native 512-row trace window');
      }
      evidence.nativeRows.push(sample);
      assert.ok(evidence.nativeRows.length <= TRACE_CAPACITY,
        'Touch scene exceeded the native 512-row trace window');
      cursor = sample.sequence;
      lastFrame = sample.frame;
      if (!isTouchMotionTickEligible(sample)) {
        throw new SceneIncomplete(`Native gameplay eligibility changed at sequence ${sample.sequence}`);
      }
    }
    evidence.trace.finalTotal = value.total;
    return { value, added };
  };

  const pollUntil = async (label, predicate, { timeoutMs = SCENE_TIMEOUT_MS } = {}) => {
    const maxPolls = Math.min(TRACE_CAPACITY, Math.ceil(timeoutMs / POLL_MS));
    for (let poll = 0; poll <= maxPolls; poll++) {
      const current = await snapshot();
      if (predicate(current)) return current;
      if (poll === maxPolls) break;
      await adapter.wait(POLL_MS);
    }
    assert.fail(`${label}: no qualifying native observation within ${timeoutMs} ms / ${maxPolls} polls`);
  };

  const currentSample = async () => {
    const current = await snapshot();
    const sample = current.value.samples.at(-1);
    assert.ok(sample, 'Native touch trace has no movement rows');
    return sample;
  };

  const queuedFocusTransitionsEnabled = requireQueuedMovementBoundary &&
    queuedMovementPolicy.touchFocusTransitions?.semantics === 'action-queue-then-next-movement';
  const queueBoundaryRecord = boundary => ({
    value: boundary.value,
    traceTotal: boundary.traceTotal,
    observationSource: boundary.observationSource,
    observationPhase: boundary.observationPhase,
  });
  const captureQueuedActionBoundary = async label => {
    const { value, added } = await snapshot();
    if (value.stableTraceBoundary !== true ||
        !validNextMovementInputBoundary(value.nextMovementInputBoundary, value.total)) {
      throw new SceneIncomplete(`${label}: queued native input observation is unavailable at the stopped movement boundary`);
    }
    if (added.length !== 0 || value.total !== cursor) {
      throw new SceneIncomplete(`${label}: native movement advanced before the action boundary was recorded`);
    }
    return { sequence: value.total, ...queueBoundaryRecord(value.nextMovementInputBoundary) };
  };
  const waitForFirstQueuedActionTick = async (label, afterSequence) => {
    const current = await pollUntil(label, ({ added }) => {
      const rows = added.filter(sample => sample.sequence > afterSequence);
      assert.ok(rows.length <= 1,
        `${label}: more than one movement tick passed before the first post-action queue was observed`);
      if (!rows.length) return false;
      assert.equal(rows[0].sequence, afterSequence + 1,
        `${label}: the first post-action movement tick is missing`);
      return true;
    });
    const sample = current.added.find(row => row.sequence === afterSequence + 1);
    assert.ok(sample, `${label}: the first post-action movement tick is unavailable`);
    const boundary = current.value.nextMovementInputBoundary;
    if (!validNextMovementInputBoundary(boundary, current.value.total)) {
      throw new SceneIncomplete(`${label}: queued native input observation is unavailable after the first movement tick`);
    }
    return { sample, boundary: queueBoundaryRecord(boundary), total: current.value.total };
  };
  const assertConsumedFocusAndSpeed = (sample, queuedValue, label) => {
    const focused = Boolean(queuedValue & queuedMovementPolicy.focusMask);
    assertNativeFocus(sample, focused, label);
    const speedInfo = focused ? evidence.calibration.slow : evidence.calibration.fast;
    assert.ok(speedInfo, `${label}: independent keyboard calibration for the queued focus state is missing`);
    close(sample.speed, speedInfo.speed, `${label}: queued focus speed`);
    close(sample.scaleX, speedInfo.scale.x, `${label}: queued focus X scale`);
    close(sample.scaleY, speedInfo.scale.y, `${label}: queued focus Y scale`);
    return { focused, speedInfo };
  };
  const waitForQueuedFocusStable = async ({ label, afterSequence, point, focused, initialBoundary,
    expectedMode = traceMode, count = 2, requireStationary = true }) => {
    const rows = [];
    let priorInput = initialBoundary.value;
    await pollUntil(label, ({ value, added }) => {
      const newRows = added.filter(sample => sample.sequence > afterSequence);
      assert.ok(newRows.length <= 1,
        `${label}: more than one movement tick passed before its input queue was checked`);
      for (const sample of newRows) {
        const consumed = assertConsumedFocusAndSpeed(sample, priorInput, label);
        assert.equal(consumed.focused, focused, `${label}: native focus did not enter the requested stable state`);
        assert.equal(sample.mode, expectedMode, `${label}: native movement mode differs`);
        close(sample.beforeX, point.x, `${label}: position before focus-owner transition X`);
        close(sample.beforeY, point.y, `${label}: position before focus-owner transition Y`);
        close(sample.targetX, point.x, `${label}: focus-owner transition changed target X`);
        close(sample.targetY, point.y, `${label}: focus-owner transition changed target Y`);
        if (requireStationary) {
          close(sample.afterX, point.x, `${label}: focus-owner transition moved X`);
          close(sample.afterY, point.y, `${label}: focus-owner transition moved Y`);
        }
        assertFocusQueueAfterAction({ value: priorInput }, value.nextMovementInputBoundary, focused,
          queuedMovementPolicy, label);
        priorInput = value.nextMovementInputBoundary.value;
        rows.push(sample);
      }
      return rows.length >= count;
    });
    return rows;
  };

  const assertEligibleRows = rows => {
    for (const sample of rows) {
      if (!isTouchMotionTickEligible(sample)) {
        throw new SceneIncomplete(`Native gameplay eligibility changed at sequence ${sample.sequence}`);
      }
    }
  };

  const waitForStableNeutral = async (label, { afterSequence = cursor, count = SETTLE_TICKS,
    drainKeyboard = null, expectedFocused } = {}) => {
    const collected = [];
    const drainStages = drainKeyboard?.stages ?? [];
    const drainStageCounts = new Array(drainStages.length).fill(0);
    const prior = latestSnapshot?.samples?.at(-1);
    let lastPosition = prior && prior.sequence <= afterSequence ? position(prior) : null;
    await pollUntil(label, ({ value, added }) => {
      assertEligibleRows(added);
      for (const sample of added) {
        if (sample.sequence <= afterSequence) continue;
        assert.equal(sample.mode, 0, `${label}: touch movement stayed active after release`);
        const move = delta(sample);
        if (lastPosition) {
          close(sample.beforeX, lastPosition.x, `${label}: X continuity`);
          close(sample.beforeY, lastPosition.y, `${label}: Y continuity`);
        }
        const stageIndex = drainStages.findIndex(stage => sample.sequence <= stage.throughSequence);
        if (stageIndex >= 0) {
          const stage = drainStages[stageIndex];
          drainStageCounts[stageIndex]++;
          assert.ok(drainStageCounts[stageIndex] <= stage.maxRows,
            `${label}: more than ${stage.maxRows} native movement tick(s) crossed the ${stage.classification} boundary`);
          assertNativeFocus(sample, stage.focused, `${label} ${stage.classification}`);
          if (stage.speed !== undefined) close(sample.speed, stage.speed, `${label}: ${stage.classification} speed`);
          if (stage.scale) {
            close(sample.scaleX, stage.scale.x, `${label}: ${stage.classification} X scale`);
            close(sample.scaleY, stage.scale.y, `${label}: ${stage.classification} Y scale`);
          }
          close(sample.targetX, sample.beforeX, `${label}: inactive touch target X during keyboard queue`);
          close(sample.targetY, sample.beforeY, `${label}: inactive touch target Y during keyboard queue`);
          if (stage.allowMovement) {
            const sign = stage.direction === 'ArrowRight' ? 1 : -1;
            close(move.x / sample.scaleX, sign * stage.speed, `${label}: queued keyboard movement`);
            assert.ok(move.x * sign > 0, `${label}: queued keyboard tick moved in the wrong direction`);
            close(move.y, 0, `${label}: horizontal calibration changed Y`);
          } else {
            close(move.x, 0, `${label}: movement continued after direction-key acknowledgement`);
            close(move.y, 0, `${label}: movement continued after direction-key acknowledgement`);
          }
          if (stage.nextInputMask !== undefined) {
            const nextInput = value.nextMovementInputBoundary;
            if (!validNextMovementInputBoundary(nextInput, value.total)) {
              throw new SceneIncomplete(`${label}: declared queued-input observation is unavailable after the queued tick`);
            }
            assert.equal(nextInput.value & stage.nextInputMask, stage.baselineInputValue & stage.nextInputMask,
              `${label}: released direction or focus remained in the next native movement queue`);
          }
          evidence.keyboardDrainRows.push({ label, classification: stage.classification,
            boundarySequence: stage.throughSequence, queuedInput: stage.queuedInput ?? null,
            nextMovementInput: value.nextMovementInputBoundary ?? null, sample, movement: move });
          lastPosition = position(sample);
          continue;
        }
        if (expectedFocused !== undefined) assertNativeFocus(sample, expectedFocused, label);
        close(move.x, 0, `${label}: position drift X`);
        close(move.y, 0, `${label}: position drift Y`);
        collected.push(sample);
        lastPosition = position(sample);
      }
      return collected.length >= count;
    });
    return collected;
  };

  const setTouchControls = async ({
    sensitivityPercent = 100,
    focus = false,
    expectedNativeSpeed,
    label = 'touch controls',
  } = {}) => {
    assert.ok([100, 200, 300].includes(sensitivityPercent), 'Scenario sensitivity must be 100, 200 or 300 percent');
    const startSequence = cursor;
    await adapter.controls({ ...neutralControls(sensitivityPercent), focusEnabled: focus });
    let consumed = null;
    await pollUntil(`${label} consumed by native Runtime`, ({ added }) => {
      assertEligibleRows(added);
      for (const row of added) {
        if (row.sequence <= startSequence ||
          Math.abs(row.sensitivity - sensitivityPercent / 100) > EPSILON) continue;
        assert.equal(row.mode, 0, `${label}: no movement owner may be active while changing controls`);
        const move = delta(row);
        close(move.x, 0, `${label}: control change moved X`);
        close(move.y, 0, `${label}: control change moved Y`);
        if (expectedNativeSpeed !== undefined) {
          const calibrated = [evidence.calibration.fast?.speed, evidence.calibration.slow?.speed]
            .filter(Number.isFinite);
          assert.ok(calibrated.some(speed => Math.abs(row.speed - speed) <= EPSILON),
            `${label}: native speed ${row.speed} is neither calibrated fast nor slow speed`);
          if (Math.abs(row.speed - expectedNativeSpeed) > EPSILON) continue;
        }
        assertNativeFocus(row, focus, label);
        consumed = row;
        return true;
      }
      return false;
    });
    evidence.setup.push({ label, sensitivityPercent, focus, expectedNativeSpeed: expectedNativeSpeed ?? null,
      consumed: consumed ?? null });
  };

  const calibrate = async ({ focused, label }) => {
    await setTouchControls({ sensitivityPercent: 100, focus: false, label: `${label} touch neutral` });
    const before = await currentSample();
    const baselineInput = requireQueuedMovementBoundary ? latestSnapshot?.nextMovementInputBoundary : null;
    assert.equal(before.mode, 0, `${label}: touch must be inactive during keyboard calibration`);
    const direction = before.afterX <= inputSpace.width / 2 ? 'ArrowRight' : 'ArrowLeft';
    const sign = direction === 'ArrowRight' ? 1 : -1;
    const directionMask = requireQueuedMovementBoundary ? queuedMovementPolicy.directionMasks[direction] : null;
    const oppositeDirectionMask = requireQueuedMovementBoundary ?
      queuedMovementPolicy.directionMasks[direction === 'ArrowRight' ? 'ArrowLeft' : 'ArrowRight'] : null;
    const focusMask = requireQueuedMovementBoundary ? queuedMovementPolicy.focusMask : null;
    const controlledInputMask = requireQueuedMovementBoundary ?
      queuedMovementPolicy.directionMasks.ArrowRight | queuedMovementPolicy.directionMasks.ArrowLeft | focusMask : null;
    if (requireQueuedMovementBoundary) {
      assert.equal(baselineInput.value & controlledInputMask, 0,
        `${label}: direction and focus inputs must be neutral before keyboard calibration`);
    }
    const startSequence = cursor;
    const moving = [];
    let releaseAcknowledgements = [];
    let focusKeyPressed = false;
    let directionKeyPressed = false;
    const calibrationRecord = { label, focused, direction, startSequence, rows: moving };
    evidence.calibration[label] = calibrationRecord;

    try {
      if (focused) {
        await sendKey('ShiftLeft', true, { requireTransition: true });
        focusKeyPressed = true;
      }
      await sendKey(direction, true, { requireTransition: true });
      directionKeyPressed = true;
      await pollUntil(`${label} keyboard speed calibration`, ({ added }) => {
        assertEligibleRows(added);
        for (const sample of added) {
          if (sample.sequence <= startSequence) continue;
          assert.equal(sample.mode, 0, `${label}: touch became active during keyboard calibration`);
          const move = delta(sample);
          if (Math.abs(move.x) <= EPSILON && Math.abs(move.y) <= EPSILON) continue;
          assertNativeFocus(sample, focused, `${label} keyboard calibration`);
          assert.ok(move.x * sign > 0, `${label}: keyboard movement went in the wrong direction`);
          close(move.y, 0, `${label}: horizontal calibration changed Y`);
          moving.push(sample);
          if (moving.length >= REQUIRED_MOTION_TICKS) return true;
        }
        return false;
      });
    } finally {
      if (directionKeyPressed) {
        const directionRelease = await sendKey(direction, false, { requireTransition: true });
        directionKeyPressed = false;
        releaseAcknowledgements = [directionRelease];
      }
      if (focusKeyPressed) {
        releaseAcknowledgements.push(await sendKey('ShiftLeft', false, { requireTransition: true }));
        focusKeyPressed = false;
      }
    }

    assert.equal(moving.length, REQUIRED_MOTION_TICKS, `${label}: calibration must use exactly three native movement ticks`);
    const scaleX = moving[0].scaleX;
    const scaleY = moving[0].scaleY;
    const measuredSpeeds = moving.map(sample => Math.abs((sample.afterX - sample.beforeX) / sample.scaleX));
    const speed = measuredSpeeds.reduce((sum, value) => sum + value, 0) / measuredSpeeds.length;
    assert.ok(speed > EPSILON, `${label}: keyboard calibration speed must be positive`);
    for (let i = 0; i < moving.length; i++) {
      close(moving[i].scaleX, scaleX, `${label}: X scale changed during calibration`);
      close(moving[i].scaleY, scaleY, `${label}: Y scale changed during calibration`);
      close(measuredSpeeds[i], speed, `${label}: keyboard speed was not stable`);
      close(moving[i].speed, speed, `${label}: trace speed disagrees with independent keyboard displacement`);
    }
    calibrationRecord.speed = speed;
    calibrationRecord.scale = { x: scaleX, y: scaleY };
    calibrationRecord.measuredWorldSteps = measuredSpeeds;
    calibrationRecord.endSequence = cursor;
    calibrationRecord.position = position(moving.at(-1));
    let releaseBoundary = cursor;
    const releaseStages = [];
    if (requireQueuedMovementBoundary) {
      const directionAck = releaseAcknowledgements[0];
      const lastReleaseAck = releaseAcknowledgements.at(-1) ?? directionAck;
      assert.equal(directionAck.nativeTraceTotal, cursor,
        `${label}: direction release must be ordered at the latest completed movement trace`);
      assert.equal(lastReleaseAck.nativeTraceTotal, directionAck.nativeTraceTotal,
        `${label}: key releases must share one stopped native movement boundary`);
      assert.deepEqual(lastReleaseAck.nextMovementInputBoundary, directionAck.nextMovementInputBoundary,
        `${label}: queued native input changed without a movement tick between key releases`);
      const queuedInput = lastReleaseAck.nextMovementInputBoundary;
      assert.ok(queuedInput.value & directionMask,
        `${label}: queued input at release boundary does not contain ${direction}`);
      assert.equal(queuedInput.value & oppositeDirectionMask, 0,
        `${label}: queued input at release boundary also contains the opposite direction`);
      assert.equal(Boolean(queuedInput.value & focusMask), focused,
        `${label}: queued focus state does not match the calibrated movement`);
      releaseBoundary = directionAck.nativeTraceTotal;
      releaseStages.push({
        throughSequence: releaseBoundary + queuedMovementPolicy.maxMovementTicks,
        classification: 'single-next-movement-tick-from-published-input',
        maxRows: queuedMovementPolicy.maxMovementTicks,
        focused: Boolean(queuedInput.value & focusMask),
        allowMovement: true,
        direction,
        speed,
        scale: { x: scaleX, y: scaleY },
        queuedInput,
        nextInputMask: controlledInputMask,
        baselineInputValue: baselineInput.value,
      });
    } else {
      const directionBoundary = Math.max(cursor, Number.isSafeInteger(releaseAcknowledgements[0]?.nativeTraceTotal) ?
        releaseAcknowledgements[0].nativeTraceTotal : cursor);
      releaseBoundary = directionBoundary;
      releaseStages.push({ throughSequence: directionBoundary, classification: 'before-direction-key-release-acknowledgement',
        maxRows: 1, focused, allowMovement: true, direction, speed, scale: { x: scaleX, y: scaleY } });
      if (focused) {
        releaseBoundary = Math.max(directionBoundary, Number.isSafeInteger(releaseAcknowledgements[1]?.nativeTraceTotal) ?
          releaseAcknowledgements[1].nativeTraceTotal : directionBoundary);
        releaseStages.push({ throughSequence: releaseBoundary, classification: 'direction-released-focus-key-held',
          maxRows: 1, focused: true, allowMovement: false, speed, scale: { x: scaleX, y: scaleY } });
      }
    }
    calibrationRecord.releaseAcknowledgements = releaseAcknowledgements;
    calibrationRecord.releaseBoundarySequence = releaseBoundary;
    calibrationRecord.neutralRows = await waitForStableNeutral(`${label} keyboard release`, {
      afterSequence: cursor,
      drainKeyboard: { stages: releaseStages },
      expectedFocused: false,
    });
    return calibrationRecord;
  };

  const runDrag = async ({ label, delta: requestedDelta, sensitivityPercent, focused = false, release = 'up',
    speedInfo, beforeRelease, afterRelease, requiredQueueDirection }) => {
    const factor = sensitivityPercent / 100;
    const focusFactor = traceMode === 1 && focused ? speedInfo.speed / evidence.calibration.fast.speed : 1;
    const startPositionSample = await currentSample();
    const startPosition = position(startPositionSample);
    const expectedTarget = inputCommand?.coordinate === 'float32-normalized-pointer' ?
      mapFloat32TouchDragTarget(startPosition,requestedDelta,{inputSpace,sensitivity:factor,focusFactor,
        pixelsPerUnit:inputCommand.pixelsPerUnit}) : {
          x: startPosition.x + requestedDelta.x * factor * focusFactor,
          y: startPosition.y + requestedDelta.y * factor * focusFactor,
        };
    assert.ok(expectedTarget.x > 0 && expectedTarget.x < inputSpace.width &&
      expectedTarget.y > 0 && expectedTarget.y < inputSpace.height,
    `${label}: requested target leaves the configured input space`);

    const id = pointerId++;
    const origin = { x: 0.5, y: 0.5 };
    const destination = {
      x: origin.x + requestedDelta.x / inputSpace.width,
      y: origin.y + requestedDelta.y / inputSpace.height,
    };
    assert.ok(destination.x > 0 && destination.x < 1 && destination.y > 0 && destination.y < 1,
      `${label}: pointer command leaves the normalized input space`);
    const scenario = {
      label, mode, traceMode, sensitivityPercent, factor, focused, focusFactor, requestedDelta: { ...requestedDelta },
      expectedTarget, startPosition, release, id, pointerOrigin: origin, pointerDestination: destination,
      commandStartSequence: cursor, downRows: [], movementRows: [], releaseRows: [], status: 'running',
    };
    evidence.scenarios.push(scenario);

    await adapter.pointer('down', id, origin.x, origin.y);
    const downStartSequence = cursor;
    const down = await pollUntil(`${label} pointer-down consumed`, ({ added }) => {
      assertEligibleRows(added);
      scenario.downRows.push(...added.filter(sample => sample.sequence > downStartSequence));
      return scenario.downRows.some(sample => sample.mode === mode);
    });
    const downRow = scenario.downRows.find(sample => sample.mode === mode);
    assert.ok(downRow, `${label}: native Runtime did not consume pointer-down`);
    close(downRow.beforeX, startPosition.x, `${label}: position before pointer-down X`);
    close(downRow.beforeY, startPosition.y, `${label}: position before pointer-down Y`);
    close(downRow.afterX, startPosition.x, `${label}: pointer-down moved X`);
    close(downRow.afterY, startPosition.y, `${label}: pointer-down moved Y`);
    close(downRow.targetX, startPosition.x, `${label}: pointer-down target X`);
    close(downRow.targetY, startPosition.y, `${label}: pointer-down target Y`);
    assert.equal(down.value.total, cursor, `${label}: trace cursor changed unexpectedly`);

    const movementStartSequence = cursor;
    await adapter.pointer('move', id, destination.x, destination.y);
    const movementRows = [];
    let targetConsumed = false;
    let reached = false;
    await pollUntil(`${label} native movement`, ({ added }) => {
      assertEligibleRows(added);
      for (const sample of added) {
        if (sample.sequence <= movementStartSequence) continue;
        const isExpectedTarget = atPosition(sample, expectedTarget);
        if (!targetConsumed && !isExpectedTarget) {
          assert.ok(atPosition(sample, startPosition),
            `${label}: native target changed to an unexpected point before consuming the command`);
          assert.ok(sample.mode === 0 || sample.mode === traceMode,
            `${label}: unexpected mode ${sample.mode} during the idle prefix`);
          close(sample.beforeX, startPosition.x, `${label}: idle-prefix position X`);
          close(sample.beforeY, startPosition.y, `${label}: idle-prefix position Y`);
          close(sample.afterX, startPosition.x, `${label}: idle-prefix movement X`);
          close(sample.afterY, startPosition.y, `${label}: idle-prefix movement Y`);
          if (sample.mode === traceMode) {
            close(sample.sensitivity, factor, `${label}: native sensitivity during idle prefix`);
            close(sample.speed, speedInfo.speed, `${label}: calibrated speed during idle prefix`);
            close(sample.scaleX, speedInfo.scale.x, `${label}: calibrated X scale during idle prefix`);
            close(sample.scaleY, speedInfo.scale.y, `${label}: calibrated Y scale during idle prefix`);
          }
          continue;
        }
        if (!targetConsumed) {
          assert.equal(sample.mode, traceMode,
            `${label}: the command target appeared without its native movement mode`);
          close(sample.beforeX, startPosition.x, `${label}: first consumed target starts at X`);
          close(sample.beforeY, startPosition.y, `${label}: first consumed target starts at Y`);
          targetConsumed = true;
        }
        assert.equal(sample.mode, traceMode, `${label}: movement mode stopped after target consumption`);
        close(sample.sensitivity, factor, `${label}: native sensitivity`);
        close(sample.speed, speedInfo.speed, `${label}: calibrated native speed`);
        close(sample.scaleX, speedInfo.scale.x, `${label}: calibrated X movement scale`);
        close(sample.scaleY, speedInfo.scale.y, `${label}: calibrated Y movement scale`);
        close(sample.targetX, expectedTarget.x, `${label}: independently mapped target X`);
        close(sample.targetY, expectedTarget.y, `${label}: independently mapped target Y`);
        movementRows.push(sample);
      }
      if (!targetConsumed || movementRows.length < REQUIRED_MOTION_TICKS) return false;
      const last = movementRows.at(-1);
      reached = isTouchMotionTargetReached(position(last), expectedTarget, { precision });
      return reached;
    });
    scenario.movementRows = movementRows;
    scenario.movementStartSequence = movementStartSequence;
    scenario.firstTargetConsumedSequence = movementRows[0]?.sequence ?? null;
    scenario.firstMovementSequence = movementRows.find(sample =>
      Math.hypot(sample.afterX - sample.beforeX, sample.afterY - sample.beforeY) > EPSILON)?.sequence ?? null;
    scenario.movementEndSequence = cursor;
    scenario.firstMovementTargetArrival = movementRows.findIndex(sample =>
      isTouchMotionTargetReached(position(sample), expectedTarget, { precision }));
    assert.ok(movementRows.length >= REQUIRED_MOTION_TICKS,
      `${label}: fewer than ${REQUIRED_MOTION_TICKS} consecutive moving-window native ticks`);
    assert.equal(reached, true, `${label}: expected target was not reached`);
    const checked = assertTouchMotionSamples(movementRows, {
      mode: traceMode,
      target: expectedTarget,
      speed: speedInfo.speed,
      sensitivity: factor,
      scale: speedInfo.scale,
      precision,
      focused,
      requireArrival: true,
    });
    scenario.oracle = checked;
    if (traceMode === 2) {
      assert.equal(scenario.firstMovementTargetArrival, 0,
        `${label}: unlimited movement must arrive on the first target-consumed native tick`);
    }

    scenario.movementEndSequence = cursor;
    const finalPosition = position(movementRows.at(-1));
    if (beforeRelease) await beforeRelease({ scenario, finalPosition });
    const releaseBoundary = cursor;
    scenario.releaseBoundarySequence = releaseBoundary;
    const cancelQueueBefore = release === 'cancel' && queuedFocusTransitionsEnabled && traceMode === 1
      ? await captureQueuedActionBoundary(`${label} cancel`)
      : null;
    if (cancelQueueBefore) {
      const queuedFocus = queueFocusValue(cancelQueueBefore, queuedMovementPolicy);
      assert.equal(queuedFocus, focused, `${label}: pre-cancel focus queue differs from the active touch-focus owner`);
      if (requiredQueueDirection) {
        const directionMask = queuedMovementPolicy.directionMasks[requiredQueueDirection];
        assert.equal(cancelQueueBefore.value & queueDirectionMask(queuedMovementPolicy), directionMask,
          `${label}: required keyboard direction is not queued before touch cancellation`);
      }
      scenario.cancelFocusQueueBefore = cancelQueueBefore;
    }
    await adapter.pointer(release, id, destination.x, destination.y);
    let cancelFocusReceipt = null;
    if (cancelQueueBefore) {
      const first = await waitForFirstQueuedActionTick(`${label} cancel first movement`, releaseBoundary);
      const consumed = assertConsumedFocusAndSpeed(first.sample, cancelQueueBefore.value,
        `${label} cancel first movement`);
      assert.equal(first.sample.mode, 0, `${label}: touch movement remained active after cancel`);
      if (!afterRelease) assertTouchCancelTickStationary(first.sample, finalPosition, label);
      cancelFocusReceipt = {
        action: 'cancel',
        preActionSequence: cancelQueueBefore.sequence,
        preActionInput: cancelQueueBefore,
        firstMovement: first.sample,
        firstMovementConsumedFocus: consumed.focused,
        afterFirstMovementInput: first.boundary,
      };
      scenario.cancelFocusQueueTransition = cancelFocusReceipt;
      assertFocusQueueAfterAction(cancelQueueBefore, first.boundary, false, queuedMovementPolicy,
        `${label} cancel`);
    }
    if (afterRelease) {
      await afterRelease({ scenario, finalPosition, releaseBoundary, cancelFocusReceipt });
    } else {
      const released = cancelFocusReceipt
        ? [cancelFocusReceipt.firstMovement, ...await waitForQueuedFocusStable({
            label: `${label} cancel stable focus`,
            afterSequence: cancelFocusReceipt.firstMovement.sequence,
            point: finalPosition,
            focused: false,
            initialBoundary: cancelFocusReceipt.afterFirstMovementInput,
            expectedMode: 0,
            count: SETTLE_TICKS - 1,
          })]
        : await waitForStableNeutral(`${label} ${release} release`, {
            afterSequence: releaseBoundary,
            count: SETTLE_TICKS,
            expectedFocused: release === 'cancel' ? false : focused,
          });
      scenario.releaseRows = released;
      assertTouchReleaseSamples(released, {
        position: finalPosition,
        afterSequence: releaseBoundary,
        minimumTicks: SETTLE_TICKS,
      });
    }
    scenario.status = 'passed';
    scenario.endSequence = cursor;
    return scenario;
  };

  const runJoystickVector = async ({ label, angle, amplitude, focused, axisX, axisY, speedInfo }) => {
    await setTouchControls({
      sensitivityPercent: 100,
      focus: focused,
      expectedNativeSpeed: speedInfo.speed,
      label: `${label} focus neutral calibrated`,
    });
    const startSample = await currentSample();
    const startPosition = position(startSample);
    const axes = { x: axisX / 32767, y: axisY / 32767 };
    const targets = [];
    let predictedPosition = { ...startPosition };
    for (let index = 0; index < REQUIRED_MOTION_TICKS; index++) {
      const target = {
        x: predictedPosition.x + speedInfo.speed * axes.x,
        y: predictedPosition.y + speedInfo.speed * axes.y,
      };
      targets.push(target);
      predictedPosition = predictTouchMotionStep(predictedPosition, target, {
        mode: 1,
        speed: speedInfo.speed,
        scale: speedInfo.scale,
        precision,
      });
    }
    for (const target of targets) {
      assert.ok(target.x > 0 && target.x < inputSpace.width && target.y > 0 && target.y < inputSpace.height,
        `${label}: joystick command leaves the configured input space`);
    }
    const scenario = {
      label, mode, traceMode, input: { joystickX: axisX, joystickY: axisY }, axes,
      angleDegrees: angle, amplitude, focused, startPosition, speed: speedInfo.speed,
      expectedTargets: targets, movementRows: [], neutralRows: [], status: 'running',
      focusReadySequence: cursor, focusReadySpeed: startSample.speed, commandStartSequence: cursor,
    };
    evidence.scenarios.push(scenario);

    await adapter.controls({ ...neutralControls(100), focusEnabled: focused, joystickX: axisX, joystickY: axisY });
    const movementRows = [];
    const commandStartSequence = cursor;
    await pollUntil(`${label} native joystick vector`, ({ added }) => {
      assertEligibleRows(added);
      for (const sample of added) {
        if (sample.sequence <= commandStartSequence) continue;
        if (movementRows.length === 0 && sample.mode === 0) {
          close(sample.beforeX, startPosition.x, `${label}: idle-prefix position X`);
          close(sample.beforeY, startPosition.y, `${label}: idle-prefix position Y`);
          close(sample.afterX, startPosition.x, `${label}: idle-prefix movement X`);
          close(sample.afterY, startPosition.y, `${label}: idle-prefix movement Y`);
          close(sample.targetX, startPosition.x, `${label}: idle-prefix target X`);
          close(sample.targetY, startPosition.y, `${label}: idle-prefix target Y`);
          continue;
        }
        assert.equal(sample.mode, 1, `${label}: free joystick must use limited native movement mode 1`);
        const expectedTarget = targets[movementRows.length];
        assert.ok(expectedTarget, `${label}: native joystick produced more than three commanded ticks`);
        close(sample.targetX, expectedTarget.x, `${label}: command-derived joystick target X`);
        close(sample.targetY, expectedTarget.y, `${label}: command-derived joystick target Y`);
        close(sample.speed, speedInfo.speed, `${label}: independently calibrated joystick speed`);
        close(sample.sensitivity, 1, `${label}: native touch sensitivity remains at 100 percent`);
        close(sample.scaleX, speedInfo.scale.x, `${label}: native X movement scale`);
        close(sample.scaleY, speedInfo.scale.y, `${label}: native Y movement scale`);
        movementRows.push(sample);
      }
      return movementRows.length === REQUIRED_MOTION_TICKS;
    });
    scenario.movementRows = movementRows;
    scenario.movementStartSequence = movementRows[0].sequence;
    scenario.movementEndSequence = movementRows.at(-1).sequence;
    scenario.firstTargetConsumedSequence = movementRows[0].sequence;
    scenario.firstMovementSequence = movementRows.find(sample => Math.hypot(...Object.values(delta(sample))) > EPSILON)?.sequence ?? null;
    scenario.oracle = assertTouchMotionSamples(movementRows, {
      mode: 1,
      targets,
      speed: speedInfo.speed,
      sensitivity: 1,
      scale: speedInfo.scale,
      precision,
      focused,
      requireArrival: true,
    });

    const releaseBoundary = cursor;
    scenario.releaseBoundarySequence = releaseBoundary;
    await adapter.controls({ ...neutralControls(100), focusEnabled: focused });
    scenario.neutralRows = await waitForStableNeutral(`${label} joystick recenter`, {
      afterSequence: releaseBoundary,
      count: SETTLE_TICKS,
      expectedFocused: focused,
    });
    scenario.endSequence = cursor;
    scenario.status = 'passed';
    return scenario;
  };

  const runNativePointerSegment = async ({ label, pointer, destination, expectedTarget, speedInfo,
    sensitivityPercent = 100, startPosition, focused = false }) => {
    const factor = sensitivityPercent / 100;
    assert.ok(expectedTarget.x > 0 && expectedTarget.x < inputSpace.width &&
      expectedTarget.y > 0 && expectedTarget.y < inputSpace.height,
    `${label}: expected target leaves the configured input space`);
    assert.ok(destination.x > 0 && destination.x < 1 && destination.y > 0 && destination.y < 1,
      `${label}: pointer command leaves the normalized input space`);
    const startSequence = cursor;
    const rows = [];
    let targetConsumed = false;
    await adapter.pointer('move', pointer.id, destination.x, destination.y);
    await pollUntil(`${label} native pointer movement`, ({ added }) => {
      assertEligibleRows(added);
      for (const sample of added) {
        if (sample.sequence <= startSequence) continue;
        if (!targetConsumed && !atPosition(sample, expectedTarget)) {
          assert.ok(atPosition(sample, startPosition),
            `${label}: native target changed before consuming the pointer command`);
          assert.ok(sample.mode === 0 || sample.mode === traceMode,
            `${label}: unexpected mode ${sample.mode} in the idle prefix`);
          close(sample.beforeX, startPosition.x, `${label}: idle-prefix position X`);
          close(sample.beforeY, startPosition.y, `${label}: idle-prefix position Y`);
          close(sample.afterX, startPosition.x, `${label}: idle-prefix movement X`);
          close(sample.afterY, startPosition.y, `${label}: idle-prefix movement Y`);
          continue;
        }
        if (!targetConsumed) {
          assert.equal(sample.mode, traceMode, `${label}: target appeared without touch movement mode`);
          close(sample.beforeX, startPosition.x, `${label}: first target-consumed position X`);
          close(sample.beforeY, startPosition.y, `${label}: first target-consumed position Y`);
          targetConsumed = true;
        }
        assert.equal(sample.mode, traceMode, `${label}: movement owner stopped before consuming its target`);
        close(sample.speed, speedInfo.speed, `${label}: calibrated native speed`);
        close(sample.scaleX, speedInfo.scale.x, `${label}: calibrated X scale`);
        close(sample.scaleY, speedInfo.scale.y, `${label}: calibrated Y scale`);
        close(sample.sensitivity, factor, `${label}: consumed sensitivity`);
        close(sample.targetX, expectedTarget.x, `${label}: independently calculated target X`);
        close(sample.targetY, expectedTarget.y, `${label}: independently calculated target Y`);
        rows.push(sample);
      }
      if (!targetConsumed || rows.length < REQUIRED_MOTION_TICKS) return false;
      const last = rows.at(-1);
      return isTouchMotionTargetReached(position(last), expectedTarget, { precision });
    });
    const oracle = assertTouchMotionSamples(rows, {
      mode: traceMode,
      target: expectedTarget,
      speed: speedInfo.speed,
      sensitivity: factor,
      scale: speedInfo.scale,
      precision,
      focused,
      requireArrival: true,
    });
    return { rows, oracle, startSequence, endSequence: cursor };
  };

  const waitForUnchangedPointerOwner = async ({ label, afterSequence, point, expectedSpeedInfo,
    expectedFocused = false, requireStableTicks = 2 }) => {
    const rows = [];
    let stable = 0;
    await pollUntil(label, ({ added }) => {
      assertEligibleRows(added);
      for (const sample of added) {
        if (sample.sequence <= afterSequence) continue;
        assert.equal(sample.mode, traceMode, `${label}: primary pointer owner was released`);
        assertNativeFocus(sample, expectedFocused, label);
        close(sample.beforeX, point.x, `${label}: position before secondary-pointer operation X`);
        close(sample.beforeY, point.y, `${label}: position before secondary-pointer operation Y`);
        close(sample.afterX, point.x, `${label}: secondary-pointer operation moved X`);
        close(sample.afterY, point.y, `${label}: secondary-pointer operation moved Y`);
        close(sample.targetX, point.x, `${label}: secondary-pointer operation changed target X`);
        close(sample.targetY, point.y, `${label}: secondary-pointer operation changed target Y`);
        const knownSpeed = [evidence.calibration.fast, evidence.calibration.slow]
          .some(calibrated => Math.abs(sample.speed - calibrated.speed) <= EPSILON);
        assert.ok(knownSpeed, `${label}: native speed is not independently calibrated`);
        close(sample.scaleX, expectedSpeedInfo.scale.x, `${label}: native X scale`);
        close(sample.scaleY, expectedSpeedInfo.scale.y, `${label}: native Y scale`);
        if (Math.abs(sample.speed - expectedSpeedInfo.speed) <= EPSILON) stable++;
        else stable = 0;
        rows.push(sample);
      }
      return stable >= requireStableTicks;
    });
    return rows;
  };

  const assertKeyboardMovementRow = (sample, { label, direction, queuedInput }) => {
    const sign = direction === 'ArrowRight' ? 1 : -1;
    assertEligibleRows([sample]);
    assert.equal(sample.mode, 0, `${label}: touch owner remained active after cancel`);
    const consumed = assertConsumedFocusAndSpeed(sample, queuedInput.value, label);
    assert.equal(consumed.focused, false, `${label}: keyboard movement consumed focused speed`);
    const speedInfo = consumed.speedInfo;
    close(sample.scaleX, speedInfo.scale.x, `${label}: keyboard X scale after cancel`);
    close(sample.scaleY, speedInfo.scale.y, `${label}: keyboard Y scale after cancel`);
    const move = delta(sample);
    close(move.x / sample.scaleX, sign * speedInfo.speed, `${label}: keyboard-owned X movement`);
    close(move.y / sample.scaleY, 0, `${label}: keyboard-owned Y movement`);
    close(sample.targetX, sample.beforeX, `${label}: inactive touch target X`);
    close(sample.targetY, sample.beforeY, `${label}: inactive touch target Y`);
    return move;
  };

  const waitForKeyboardOwnerAfterCancel = async ({ label, afterSequence, direction, speedInfo,
    firstMovement, queuedInput }) => {
    const rows = [];
    const append = sample => {
      if (queuedInput) {
        assertKeyboardMovementRow(sample, { label, direction, queuedInput });
      } else {
        const sign = direction === 'ArrowRight' ? 1 : -1;
        assertEligibleRows([sample]);
        assert.equal(sample.mode, 0, `${label}: touch owner remained active after cancel`);
        assertNativeFocus(sample, false, label);
        close(sample.speed, speedInfo.speed, `${label}: keyboard fast speed after cancel`);
        close(sample.scaleX, speedInfo.scale.x, `${label}: keyboard X scale after cancel`);
        close(sample.scaleY, speedInfo.scale.y, `${label}: keyboard Y scale after cancel`);
        const move = delta(sample);
        close(move.x / sample.scaleX, sign * speedInfo.speed, `${label}: keyboard-owned X movement`);
        close(move.y / sample.scaleY, 0, `${label}: keyboard-owned Y movement`);
      }
      if (rows.length) {
        close(sample.beforeX, rows.at(-1).afterX, `${label}: keyboard X continuity`);
        close(sample.beforeY, rows.at(-1).afterY, `${label}: keyboard Y continuity`);
      }
      rows.push(sample);
    };
    if (firstMovement) append(firstMovement);
    await pollUntil(`${label} keyboard remains active after touch cancel`, ({ added }) => {
      const newRows = added.filter(sample => sample.sequence > afterSequence);
      assert.ok(newRows.length <= 1,
        `${label}: more than one keyboard movement tick passed before its queued input was checked`);
      const boundary = latestSnapshot?.nextMovementInputBoundary;
      if (queuedInput && newRows.length) {
        if (!validNextMovementInputBoundary(boundary, latestSnapshot.total)) {
          throw new SceneIncomplete(`${label}: queued direction observation is unavailable after keyboard movement`);
        }
        assert.equal(boundary.value & queueDirectionMask(queuedMovementPolicy),
          queuedInput.value & queueDirectionMask(queuedMovementPolicy),
        `${label}: held keyboard direction disappeared or changed in the next movement queue`);
        assert.equal(boundary.value & queuedMovementPolicy.focusMask, 0,
          `${label}: focus returned while the keyboard owner remained held`);
      }
      for (const sample of newRows) append(sample);
      return rows.length >= REQUIRED_MOTION_TICKS;
    });
    return rows;
  };

  const runMultipointerOwnership = async () => {
    assert.equal(traceMode, 1, 'Multipointer ownership needs limited touch mode');
    assert.ok(['hold-button', 'toggle-button', 'two-finger'].includes(focusMode),
      `Unsupported touch focus mode: ${focusMode}`);
    const fast = evidence.calibration.fast;
    const slow = evidence.calibration.slow;
    const focusFactor = focusMode === 'two-finger' ? slow.speed / fast.speed : 1;
    const unpressedSpeed = { speed: fast.speed, scale: fast.scale };
    const pressedSpeed = focusMode === 'two-finger'
      ? { speed: slow.speed, scale: slow.scale }
      : unpressedSpeed;
    await setTouchControls({ sensitivityPercent: 100, focus: false, expectedNativeSpeed: fast.speed,
      label: 'multipointer neutral controls' });

    const primaryId = pointerId++;
    const secondaryId = pointerId++;
    const origin = { x: 0.5, y: 0.5 };
    const start = position(await currentSample());
    const scenario = {
      label: `multipointer-primary-owner-${focusMode}`, focusMode, focusFactor, primaryId, secondaryId,
      startPosition: start, observations: [], status: 'running',
    };
    evidence.scenarios.push(scenario);

    const downStart = cursor;
    let primaryDownRow = null;
    const primaryDownRows = [];
    await adapter.pointer('down', primaryId, origin.x, origin.y);
    const down = await pollUntil(`${scenario.label} primary down`, ({ added }) => {
      assertEligibleRows(added);
      for (const row of added) {
        if (row.sequence <= downStart) continue;
        assertNativeFocus(row, false, `${scenario.label} primary pointer-down`);
        if (!primaryDownRow && row.mode === 0) {
          close(row.beforeX, start.x, `${scenario.label}: primary down idle-prefix before X`);
          close(row.beforeY, start.y, `${scenario.label}: primary down idle-prefix before Y`);
          close(row.afterX, start.x, `${scenario.label}: primary down idle-prefix after X`);
          close(row.afterY, start.y, `${scenario.label}: primary down idle-prefix after Y`);
          close(row.targetX, start.x, `${scenario.label}: primary down idle-prefix target X`);
          close(row.targetY, start.y, `${scenario.label}: primary down idle-prefix target Y`);
          primaryDownRows.push(row);
          continue;
        }
        assert.equal(row.mode, 1, `${scenario.label}: primary pointer did not own limited touch movement`);
        close(row.beforeX, start.x, `${scenario.label}: primary down before X`);
        close(row.beforeY, start.y, `${scenario.label}: primary down before Y`);
        close(row.afterX, start.x, `${scenario.label}: primary down after X`);
        close(row.afterY, start.y, `${scenario.label}: primary down after Y`);
        close(row.targetX, start.x, `${scenario.label}: primary down target X`);
        close(row.targetY, start.y, `${scenario.label}: primary down target Y`);
        primaryDownRow ??= row;
        primaryDownRows.push(row);
      }
      return primaryDownRow !== null;
    });
    scenario.primaryDown = primaryDownRow;
    scenario.primaryDownRows = primaryDownRows;
    assert.ok(down.value.total >= primaryDownRow.sequence);

    let primaryPoint = { x: origin.x, y: origin.y + 4 / inputSpace.height };
    const firstTarget = { x: start.x, y: start.y + 4 };
    const firstMove = await runNativePointerSegment({
      label: `${scenario.label} initial movement`, pointer: { id: primaryId }, destination: primaryPoint,
      expectedTarget: firstTarget, speedInfo: unpressedSpeed, startPosition: start,
    });
    scenario.observations.push({ label: 'primary-initial-move', ...firstMove });
    let currentPoint = firstTarget;

    const secondDownBoundary = cursor;
    const secondOrigin = { x: 0.25, y: 0.25 };
    const secondDownInputBefore = queuedFocusTransitionsEnabled
      ? await captureQueuedActionBoundary(`${scenario.label} second finger down`)
      : null;
    if (secondDownInputBefore) {
      assert.equal(queueFocusValue(secondDownInputBefore, queuedMovementPolicy), false,
        `${scenario.label}: focus was already queued before the second finger joined`);
      scenario.secondaryDownInputBefore = secondDownInputBefore;
    }
    await adapter.pointer('down', secondaryId, secondOrigin.x, secondOrigin.y);
    if (secondDownInputBefore && focusMode === 'two-finger') {
      const first = await waitForFirstQueuedActionTick(`${scenario.label} second finger first movement`,
        secondDownBoundary);
      const consumed = assertConsumedFocusAndSpeed(first.sample, secondDownInputBefore.value,
        `${scenario.label} second finger first movement`);
      assert.equal(consumed.focused, false,
        `${scenario.label}: first secondary-down movement did not consume the pre-action focus queue`);
      assert.equal(first.sample.mode, traceMode, `${scenario.label}: secondary down released the primary owner`);
      close(first.sample.beforeX, currentPoint.x, `${scenario.label}: secondary-down first row before X`);
      close(first.sample.beforeY, currentPoint.y, `${scenario.label}: secondary-down first row before Y`);
      close(first.sample.afterX, currentPoint.x, `${scenario.label}: secondary-down first row moved X`);
      close(first.sample.afterY, currentPoint.y, `${scenario.label}: secondary-down first row moved Y`);
      close(first.sample.targetX, currentPoint.x, `${scenario.label}: secondary-down first row target X`);
      close(first.sample.targetY, currentPoint.y, `${scenario.label}: secondary-down first row target Y`);
      scenario.secondaryDownFirstTick = {
        sample: first.sample,
        consumedInput: secondDownInputBefore,
        nextMovementInput: first.boundary,
      };
      assertFocusQueueAfterAction(secondDownInputBefore, first.boundary, true, queuedMovementPolicy,
        `${scenario.label} second finger down`);
      scenario.secondaryDownRows = [first.sample, ...await waitForQueuedFocusStable({
        label: `${scenario.label} second finger focus stable`,
        afterSequence: first.sample.sequence,
        point: currentPoint,
        focused: true,
        initialBoundary: first.boundary,
        expectedMode: traceMode,
        count: 2,
      })];
    } else {
      scenario.secondaryDownRows = await waitForUnchangedPointerOwner({
        label: `${scenario.label} second finger down`, afterSequence: secondDownBoundary,
        point: currentPoint, expectedSpeedInfo: pressedSpeed, expectedFocused: focusMode === 'two-finger',
      });
    }

    const secondaryMoveBoundary = cursor;
    await adapter.pointer('move', secondaryId, secondOrigin.x + 0.04, secondOrigin.y + 0.04);
    scenario.secondaryMoveRows = await waitForUnchangedPointerOwner({
      label: `${scenario.label} secondary finger move`, afterSequence: secondaryMoveBoundary,
      point: currentPoint, expectedSpeedInfo: pressedSpeed, expectedFocused: focusMode === 'two-finger',
    });

    const secondDelta = { x: 0, y: 2 };
    const secondTarget = { x: currentPoint.x, y: currentPoint.y + secondDelta.y * focusFactor };
    primaryPoint = { x: primaryPoint.x, y: primaryPoint.y + secondDelta.y / inputSpace.height };
    scenario.primaryWithSecondExpectedTarget = secondTarget;
    scenario.primaryWithSecondPhysicalPoint = { ...primaryPoint };
    const primaryWithSecond = await runNativePointerSegment({
      label: `${scenario.label} primary remains owner while second finger is down`,
      pointer: { id: primaryId }, destination: primaryPoint, expectedTarget: secondTarget,
      speedInfo: pressedSpeed, startPosition: currentPoint, focused: focusMode === 'two-finger',
    });
    scenario.observations.push({ label: 'primary-move-with-second-finger', ...primaryWithSecond });
    currentPoint = secondTarget;

    const secondUpBoundary = cursor;
    const secondUpInputBefore = queuedFocusTransitionsEnabled && focusMode === 'two-finger'
      ? await captureQueuedActionBoundary(`${scenario.label} second finger up`)
      : null;
    if (secondUpInputBefore) {
      assert.equal(queueFocusValue(secondUpInputBefore, queuedMovementPolicy), true,
        `${scenario.label}: focus was not queued before the second finger lifted`);
      scenario.secondaryUpInputBefore = secondUpInputBefore;
    }
    await adapter.pointer('up', secondaryId, secondOrigin.x + 0.04, secondOrigin.y + 0.04);
    if (secondUpInputBefore) {
      const first = await waitForFirstQueuedActionTick(`${scenario.label} second finger up first movement`,
        secondUpBoundary);
      const consumed = assertConsumedFocusAndSpeed(first.sample, secondUpInputBefore.value,
        `${scenario.label} second finger up first movement`);
      assert.equal(consumed.focused, true,
        `${scenario.label}: first secondary-up movement did not consume the pre-action focus queue`);
      assert.equal(first.sample.mode, traceMode, `${scenario.label}: second finger up released the primary owner`);
      close(first.sample.beforeX, currentPoint.x, `${scenario.label}: secondary-up first row before X`);
      close(first.sample.beforeY, currentPoint.y, `${scenario.label}: secondary-up first row before Y`);
      close(first.sample.afterX, currentPoint.x, `${scenario.label}: secondary-up first row moved X`);
      close(first.sample.afterY, currentPoint.y, `${scenario.label}: secondary-up first row moved Y`);
      close(first.sample.targetX, currentPoint.x, `${scenario.label}: secondary-up first row target X`);
      close(first.sample.targetY, currentPoint.y, `${scenario.label}: secondary-up first row target Y`);
      scenario.secondaryUpFirstTick = {
        sample: first.sample,
        consumedInput: secondUpInputBefore,
        nextMovementInput: first.boundary,
      };
      assertFocusQueueAfterAction(secondUpInputBefore, first.boundary, false, queuedMovementPolicy,
        `${scenario.label} second finger up`);
      scenario.secondaryUpRows = [first.sample, ...await waitForQueuedFocusStable({
        label: `${scenario.label} second finger up focus stable`,
        afterSequence: first.sample.sequence,
        point: currentPoint,
        focused: false,
        initialBoundary: first.boundary,
        expectedMode: traceMode,
        count: 2,
      })];
    } else {
      scenario.secondaryUpRows = await waitForUnchangedPointerOwner({
        label: `${scenario.label} second finger up preserves primary`, afterSequence: secondUpBoundary,
        point: currentPoint, expectedSpeedInfo: unpressedSpeed, expectedFocused: false,
      });
    }

    const recenterDelta = { x: 0, y: -(4 + 2 * focusFactor) };
    primaryPoint = {
      x: primaryPoint.x + recenterDelta.x / inputSpace.width,
      y: primaryPoint.y + recenterDelta.y / inputSpace.height,
    };
    const recentered = await runNativePointerSegment({
      label: `${scenario.label} primary recenters`, pointer: { id: primaryId }, destination: primaryPoint,
      expectedTarget: start, speedInfo: unpressedSpeed, startPosition: currentPoint,
    });
    scenario.observations.push({ label: 'primary-recenter', ...recentered });
    scenario.releaseBoundarySequence = cursor;
    await adapter.pointer('up', primaryId, primaryPoint.x, primaryPoint.y);
    scenario.releaseRows = await waitForStableNeutral(`${scenario.label} primary release`, {
      afterSequence: scenario.releaseBoundarySequence,
      count: SETTLE_TICKS,
      expectedFocused: false,
    });
    assertTouchReleaseSamples(scenario.releaseRows, {
      position: start,
      afterSequence: scenario.releaseBoundarySequence,
      minimumTicks: SETTLE_TICKS,
    });
    scenario.endSequence = cursor;
    scenario.status = 'passed';
    return scenario;
  };

  const runKeyboardTouchOwnerIsolation = async () => {
    assert.equal(traceMode, 1, 'Keyboard/touch owner isolation needs limited touch mode');
    const fast = evidence.calibration.fast;
    await setTouchControls({ sensitivityPercent: 100, focus: false, expectedNativeSpeed: fast.speed,
      label: 'keyboard-touch owner neutral controls' });
    const before = position(await currentSample());
    const direction = before.x < inputSpace.width / 2 ? 'ArrowRight' : 'ArrowLeft';
    const sign = direction === 'ArrowRight' ? 1 : -1;
    const scenario = {
      label: `keyboard-${direction}-survives-touch-cancel`, direction, status: 'running',
      startPosition: before,
    };
    evidence.scenarios.push(scenario);

    await runDrag({
      label: scenario.label,
      delta: { x: -sign * 6, y: 8 },
      sensitivityPercent: 100,
      release: 'cancel',
      speedInfo: { speed: fast.speed, scale: fast.scale },
      requiredQueueDirection: keyboardTouchTakeoverEnabled ? direction : undefined,
      beforeRelease: async ({ scenario: dragScenario, finalPosition }) => {
        if (keyboardTouchTakeoverEnabled) {
          const beforeKey = await captureQueuedActionBoundary(`${scenario.label} before direction keydown`);
          const directionMask = queuedMovementPolicy.directionMasks[direction];
          const directionBits = queueDirectionMask(queuedMovementPolicy);
          assert.equal(beforeKey.value & directionBits, 0,
            `${scenario.label}: direction queue must be neutral before keydown`);
          assert.equal(queueFocusValue(beforeKey, queuedMovementPolicy), false,
            `${scenario.label}: keyboard takeover starts without focused movement`);
          const keySequence = cursor;
          const acknowledgement = await sendKey(direction, true, { requireTransition: true });
          assert.equal(acknowledgement.nativeTraceTotal, keySequence,
            `${scenario.label}: native movement advanced before the direction-key action boundary`);
          const first = await waitForFirstQueuedActionTick(`${scenario.label} direction takeover`, keySequence);
          assertConsumedFocusAndSpeed(first.sample, beforeKey.value, `${scenario.label} direction takeover`);
          assertTouchCancelTickStationary(first.sample, finalPosition, `${scenario.label} direction takeover`);
          assert.equal(first.boundary.value & directionBits, directionMask,
            `${scenario.label}: direction key was not published for the next native movement`);
          assert.equal(first.boundary.value & queuedMovementPolicy.focusMask,
            beforeKey.value & queuedMovementPolicy.focusMask,
            `${scenario.label}: direction takeover changed the independent focus queue`);
          scenario.directionTakeover = { beforeKey, firstMovement: first.sample, afterFirstMovementInput: first.boundary };
          scenario.mixedOwnerRows = [first.sample];
        } else {
          await sendKey(direction, true, { requireTransition: true });
          scenario.mixedOwnerRows = await waitForUnchangedPointerOwner({
            label: `${scenario.label} keyboard and touch both held`,
            afterSequence: cursor,
            point: finalPosition,
            expectedSpeedInfo: { speed: fast.speed, scale: fast.scale },
            requireStableTicks: 1,
          });
        }
        scenario.touchDrag = dragScenario;
      },
      afterRelease: async ({ scenario: dragScenario, releaseBoundary, cancelFocusReceipt }) => {
        let firstMovement = null;
        let queuedInput = null;
        if (keyboardTouchTakeoverEnabled) {
          assert.ok(cancelFocusReceipt, `${scenario.label}: touch cancellation needs a queued native boundary`);
          const directionMask = queuedMovementPolicy.directionMasks[direction];
          const directionBits = queueDirectionMask(queuedMovementPolicy);
          assert.equal(cancelFocusReceipt.preActionInput.value & directionBits, directionMask,
            `${scenario.label}: direction was not queued before touch cancellation`);
          assert.equal(cancelFocusReceipt.afterFirstMovementInput.value & directionBits, directionMask,
            `${scenario.label}: touch cancel cleared the independently held keyboard direction`);
          firstMovement = cancelFocusReceipt.firstMovement;
          queuedInput = cancelFocusReceipt.preActionInput;
          scenario.cancelFirstKeyboardMovement = firstMovement;
        }
        const rows = await waitForKeyboardOwnerAfterCancel({
          label: scenario.label,
          afterSequence: releaseBoundary,
          direction,
          speedInfo: fast,
          firstMovement,
          queuedInput,
        });
        scenario.touchDrag = dragScenario;
        scenario.keyboardRows = rows;
        const releaseAcknowledgement = await sendKey(direction, false, { requireTransition: true });
        let drainStages = [];
        if (keyboardTouchTakeoverEnabled) {
          const boundary = releaseAcknowledgement.nextMovementInputBoundary;
          const directionBits = queueDirectionMask(queuedMovementPolicy);
          const queuedDirections = boundary.value & directionBits;
          const directionMask = queuedMovementPolicy.directionMasks[direction];
          assert.equal(boundary.value & queuedMovementPolicy.focusMask, 0,
            `${scenario.label}: keyup acknowledgement retained focused movement`);
          assert.ok(queuedDirections === 0 || queuedDirections === directionMask,
            `${scenario.label}: keyup boundary published a conflicting direction`);
          if (queuedDirections === directionMask) {
            drainStages = [{
              throughSequence: releaseAcknowledgement.nativeTraceTotal + 1,
              classification: 'one movement queued at direction-key release acknowledgement', maxRows: 1,
              focused: queueFocusValue(boundary, queuedMovementPolicy), allowMovement: true, direction,
              speed: fast.speed, scale: fast.scale,
              nextInputMask: directionBits | queuedMovementPolicy.focusMask, baselineInputValue: 0,
              queuedInput: boundary,
            }];
          }
        }
        scenario.neutralRows = await waitForStableNeutral(`${scenario.label} final key release`, {
          afterSequence: cursor,
          count: SETTLE_TICKS,
          drainKeyboard: { stages: drainStages },
          expectedFocused: false,
        });
      },
    });
    scenario.endSequence = cursor;
    scenario.status = 'passed';
    return scenario;
  };

  const runFocusedTouchCancelProbe = async () => {
    assert.equal(traceMode, 1, 'Focused touch cancellation needs limited touch mode');
    const slow = evidence.calibration.slow;
    const fast = evidence.calibration.fast;
    await setTouchControls({ sensitivityPercent: 100, focus: true, expectedNativeSpeed: slow.speed,
      label: 'focused cancel controls' });
    const scenario = await runDrag({
      label: 'focused-touch-cancel-queue-transition',
      delta: { x: 6, y: 8 },
      sensitivityPercent: 100,
      focused: true,
      release: 'cancel',
      speedInfo: { speed: slow.speed, scale: slow.scale },
    });
    await setTouchControls({ sensitivityPercent: 100, focus: false, expectedNativeSpeed: fast.speed,
      label: 'focused cancel controls restored' });
    return scenario;
  };

  try {
    assert.ok(mode === 1 || mode === 2 || joystickMode,
      'Touch scene mode must be 1 (limited), 2 (unlimited), or joystick-free');
    if(!['queued-for-next-movement','synchronous-consumption'].includes(inputBoundary))
      throw new SceneIncomplete('Native movement input-consumption boundary is unknown or unsupported');
    if(typeof inputBoundarySource!=='string'||!inputBoundarySource.trim())
      throw new SceneIncomplete('Native movement input boundary has no production source anchor');
    if(inputBoundary==='synchronous-consumption'&&queuedMovementPolicy)
      throw new SceneIncomplete('Synchronous input consumption cannot use a queued-movement release policy');
    if(requireQueuedMovementBoundary&&!queuedMovementPolicy)
      throw new SceneIncomplete('Queued movement has no source-anchored input release policy');
    assert.ok(['hold-button', 'toggle-button', 'two-finger'].includes(focusMode),
      `Unsupported touch focus mode: ${focusMode}`);
    assert.ok(['all', 'focus-sensitivity', 'input-ownership'].includes(scenarioSet),
      `Unsupported touch scenario set: ${scenarioSet}`);
    assert.ok(scenarioSet === 'all' || (mode === 1 && !joystickMode),
      `${scenarioSet} scenarios require limited direct touch mode`);
    assert.ok(Number.isFinite(inputSpace.width) && inputSpace.width > 0 &&
      Number.isFinite(inputSpace.height) && inputSpace.height > 0,
    'Input space must have positive finite width and height');
    if (requireQueuedMovementBoundary && !validQueuedMovementPolicy(queuedMovementPolicy)) {
      throw new SceneIncomplete('Queued-movement keyboard release policy is missing or invalid');
    }
    requiredAdapter(adapter);

    // Establish that the package exposes a readable native trace before sending
    // any input into the live game. An unavailable trace is incomplete evidence,
    // not a reason to perturb the current Runtime.
    const initialTrace = await snapshot();
    if (initialTrace.value.abiVersion !== 2) {
      throw new SceneIncomplete(`Strict touch motion requires native ABI v2; package exposes ABI v${initialTrace.value.abiVersion ?? 1}`);
    }
    if (initialTrace.value.localSeat !== 0 || initialTrace.value.executionKind !== 0) {
      throw new SceneIncomplete('Strict touch motion requires the native local-seat 0 forward-execution stream');
    }
    assertEligibleRows(initialTrace.value.samples.slice(-1));

    // Clear this group's input owners without changing Runtime or game state.
    await adapter.controls(neutralControls(100));
    for (const code of ['ArrowRight', 'ArrowLeft', 'ArrowUp', 'ArrowDown', 'ShiftLeft']) {
      await sendKey(code, false);
    }
    await adapter.pointer('cancel', pointerId++, 0.5, 0.5);
    if (!initialTrace.value.samples.length) {
      await pollUntil('initial native touch movement row', current => current.value.samples.length > 0);
    }
    const neutral = await waitForStableNeutral('initial gameplay and input neutral', {
      afterSequence: cursor,
      count: SETTLE_TICKS,
      expectedFocused: false,
    });
    evidence.setup.push({ label: 'initial-neutral', rows: neutral });

    const fast = await calibrate({ focused: false, label: 'fast' });
    const slow = await calibrate({ focused: true, label: 'slow' });
    assert.ok(slow.speed < fast.speed, 'Shift calibration did not produce the native focused speed');
    evidence.calibration.focusRatio = slow.speed / fast.speed;

    if (joystickMode) {
      await setTouchControls({ sensitivityPercent: 100, focus: false, label: 'free-joystick-neutral' });
      const angles = [23, 37, 71, 203, 217, 251];
      const amplitudes = [0.25, 0.5, 1];
      const runVectorPair = async (angle, amplitude, focused, labelPrefix) => {
        const radians = angle * Math.PI / 180;
        const axisX = Math.round(Math.cos(radians) * amplitude * 32767);
        const axisY = Math.round(Math.sin(radians) * amplitude * 32767);
        const speedInfo = focused
          ? { speed: slow.speed, scale: slow.scale }
          : { speed: fast.speed, scale: fast.scale };
        const suffix = `${labelPrefix}-angle-${angle}-amplitude-${amplitude}-focus-${focused ? 'on' : 'off'}`;
        await runJoystickVector({
          label: `${suffix}-forward`, angle, amplitude, focused, axisX, axisY, speedInfo,
        });
        await runJoystickVector({
          label: `${suffix}-reverse`, angle, amplitude, focused,
          axisX: -axisX, axisY: -axisY, speedInfo,
        });
      };

      // Exercise the focused scale, then explicitly return to calibrated fast
      // movement before covering the remaining direction/magnitude matrix.
      await runVectorPair(23, 0.25, true, 'focus-calibration');
      await runVectorPair(37, 0.25, false, 'focus-restored-fast');
      for (const angle of angles) {
        for (const amplitude of amplitudes) {
          if ((angle === 23 || angle === 37) && amplitude === 0.25) continue;
          await runVectorPair(angle, amplitude, false, 'free-joystick');
        }
      }
      await runVectorPair(43, 1, false, 'radial-speed-boundary');
      await adapter.controls(neutralControls(100));
      const neutralAfterJoystick = await waitForStableNeutral('free joystick neutralized', {
        afterSequence: cursor,
      });
      evidence.setup.push({ label: 'free-joystick-neutralized', rows: neutralAfterJoystick });
    } else if (mode === 1 && scenarioSet === 'input-ownership') {
      await runMultipointerOwnership();
      await runKeyboardTouchOwnerIsolation();
      if (queuedFocusTransitionsEnabled && focusMode === 'two-finger') {
        await runFocusedTouchCancelProbe();
      }
    } else {
      if (scenarioSet !== 'focus-sensitivity') {
      for (const sensitivityPercent of [100, 200, 300]) {
        await setTouchControls({ sensitivityPercent, focus: false, expectedNativeSpeed: fast.speed,
          label: `sensitivity-${sensitivityPercent}` });
        const speedInfo = { speed: fast.speed, scale: fast.scale };
        await runDrag({
          label: `sensitivity-${sensitivityPercent}-micro-up`,
          delta: { x: 0.3, y: 0.4 }, sensitivityPercent, release: 'up', speedInfo,
        });
        await runDrag({
          label: `sensitivity-${sensitivityPercent}-diagonal-up`,
          delta: { x: 6, y: 8 }, sensitivityPercent, release: 'up', speedInfo,
        });
        await runDrag({
          label: `sensitivity-${sensitivityPercent}-diagonal-reverse-cancel`,
          delta: { x: -6, y: -8 }, sensitivityPercent, release: 'cancel', speedInfo,
        });
      }
      }

      if (mode === 1) {
        const slowSpeedInfo = { speed: slow.speed, scale: slow.scale };
        for (const sensitivityPercent of [100, 200, 300]) {
          await setTouchControls({ sensitivityPercent, focus: true, expectedNativeSpeed: slow.speed,
            label: `focused-sensitivity-${sensitivityPercent}` });
          await runDrag({
            label: `focused-sensitivity-${sensitivityPercent}-micro-up`,
            delta: { x: 0.3, y: 0.4 }, sensitivityPercent, focused: true,
            release: 'up', speedInfo: slowSpeedInfo,
          });
          if (scenarioSet === 'all') {
            await runDrag({
              label: `focused-sensitivity-${sensitivityPercent}-diagonal-up`,
              delta: { x: 6, y: 8 }, sensitivityPercent, focused: true,
              release: 'up', speedInfo: slowSpeedInfo,
            });
            await runDrag({
              label: `focused-sensitivity-${sensitivityPercent}-diagonal-reverse-cancel`,
              delta: { x: -6, y: -8 }, sensitivityPercent, focused: true,
              release: 'cancel', speedInfo: slowSpeedInfo,
            });
          }
        }
        await setTouchControls({ sensitivityPercent: 100, focus: false, expectedNativeSpeed: fast.speed,
          label: 'focus-released' });
        const neutralAfterFocus = await waitForStableNeutral('focus release settled', { afterSequence: cursor });
        evidence.setup.push({ label: 'focus-released-neutral', rows: neutralAfterFocus });
        const fastSpeedInfo = { speed: fast.speed, scale: fast.scale };
        await runDrag({
          label: 'focus-restored-fast-diagonal-up', delta: { x: 6, y: 8 }, sensitivityPercent: 100,
          focused: false, release: 'up', speedInfo: fastSpeedInfo,
        });
        await runDrag({
          label: 'focus-restored-fast-diagonal-reverse-cancel', delta: { x: -6, y: -8 }, sensitivityPercent: 100,
          focused: false, release: 'cancel', speedInfo: fastSpeedInfo,
        });
        if (scenarioSet === 'all') {
          await runMultipointerOwnership();
          await runKeyboardTouchOwnerIsolation();
        }
      }
    }

    evidence.trace.finalTotal = cursor;
    evidence.status = 'passed';
    return { checkStatus: 'passed', evidence };
  } catch (error) {
    evidence.trace.finalTotal = cursor;
    evidence.failure = {
      name: error?.name ?? 'Error',
      message: error?.message ?? String(error),
      lastSequence: cursor,
      lastFrame,
      latestSnapshotTotal: latestSnapshot?.total ?? null,
      latestTraceRows: latestSnapshot?.samples?.slice(-8) ?? [],
    };
    if (error instanceof SceneIncomplete) {
      return { checkStatus: 'incomplete', reason: error.message, evidence };
    }
    const failure = error instanceof Error ? error : new Error(String(error));
    failure.touchMotionEvidence = evidence;
    throw failure;
  }
}
