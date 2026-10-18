import assert from 'node:assert/strict';

export const REQUIRED_RUNTIME_CHECKS=Object.freeze(['runtime-input-identity','runtime-ready','configure-launch-first-frame',
  'reject-unknown-command','save-list-and-flush','hosted-keyboard-native-effect','native-gamepad-sampling','native-gamepad-effect',
  'native-pause-and-restart','resize-and-continued-rendering','runtime-errors','reload-storage-and-epoch']);

export function runtimeScenarioResult({checks=[],failure}={}) {
  const missing=REQUIRED_RUNTIME_CHECKS.filter(id=>!checks.some(check=>check.id===id&&check.status==='passed'));
  const failed=Boolean(failure)||checks.some(check=>check.status==='failed');
  return {status:failed?'failed':missing.length?'incomplete':'passed',passed:!failed&&!missing.length,unverifiedChecks:missing};
}

/** Read only established Runtime exports. This function is self-contained so it can be passed to frame.evaluate. */
export function readRuntimeObservation(target) {
  const th11Phases = ['title', 'stage', 'paused', 'game_over', 'finished', 'ending'];
  const th09Phases = ['inactive', 'match', 'game_over', 'match_complete', 'ending', 'finished'];
  const th15Phases = ['title', 'stage', 'paused', 'ending', 'exit'];
  const game = typeof target === 'string' ? target : target?.game;
  const variant = typeof target === 'object' ? target.variant : undefined;
  const root = typeof window === 'object' ? window : globalThis;
  const runtime = root[`__${game}Runtime`];
  const module = runtime?.Module ?? root.Module;
  // Shells expose a few different Emscripten/WebAssembly shapes: a named
  // runtime wrapper, `window.core` as raw instance.exports, or only Module.
  // Keep the reader tolerant of those real mounting shapes without probing
  // or invoking game actions.
  const core = runtime?.core ?? root.core ?? module;
  const nativeExport = name => {
    for (const owner of [core, module]) {
      if (!owner) continue;
      const fn = owner[name] ?? owner[`_${name}`];
      if (typeof fn === 'function') return fn.bind(owner);
    }
    return undefined;
  };
  const buffer = core?.memory?.buffer ?? module?.HEAPU8?.buffer ?? null;
  const finite = value => typeof value === 'number' && Number.isFinite(value) ? value : null;
  const wasmArray = (fn, count, Type) => {
    if (typeof fn !== 'function' || !buffer) return null;
    const pointer = fn();
    if (!Number.isInteger(pointer) || pointer <= 0 || pointer + count * 4 > buffer.byteLength) return null;
    return Array.from(new Type(buffer, pointer, count));
  };
  const wasmFloat2 = (pointer, offset = 0) => {
    if (!buffer || !Number.isInteger(pointer) || pointer <= 0 || pointer + (offset + 2) * 4 > buffer.byteLength) return null;
    const values = Array.from(new Float32Array(buffer, pointer + offset * 4, 2));
    return values.every(Number.isFinite) ? {x: values[0], y: values[1]} : null;
  };
  const unsupported = [];
  const result = {
    schema: 'eagler-touhou/runtime-observation/1', game, variant: variant ?? null,
    frames: {}, phase: null, input: null, position: null, pause: null,
    render: {presentationFrame: null, renderCallbacks: null}, replay: null,
    status: null, motionSample: null,
    capabilities: {phase: false, input: false, position: false, pause: false,
      restartFrame: null, presentationFrame: false},
    unsupported,
  };
  const setPresentation = value => {
    const n = finite(value);
    if (n !== null) {
      result.render.presentationFrame = n;
      result.capabilities.presentationFrame = true;
    }
  };
  const readPresentation = () => {
    const values = wasmArray(nativeExport('sdl_stats'), 6, Uint32Array);
    if (values) setPresentation(values[5]);
  };

  if (game === 'th06' || game === 'th07') {
    const ready = nativeExport('eagler_replay_ready');
    const replayState = nativeExport('eagler_replay_state');
    if (typeof ready === 'function') {
      const pointer = ready();
      result.replay = {ready: pointer === 1};
      if (typeof replayState === 'function') {
        const values = wasmArray(replayState, 5, Uint32Array);
        if (values) result.replay.state = {frame: values[0], total: values[1], ended: values[2] !== 0,
          stage: values[3], startStage: values[4]};
      }
    }
    if (game === 'th07') result.render.renderCallbacks = finite(root.__eaglerDebugRenderCallbacks);
    unsupported.push('gameplay-phase', 'gameplay-frame', 'keyboard-gameplay-effect', 'pause-state', 'restart-state', 'player-position');
  } else if (game === 'th08') {
    const app = runtime?.app;
    const statusExport = nativeExport('status'), traceExport = nativeExport('trace'), diagnosticsExport = nativeExport('diagnostics');
    if (statusExport && traceExport && diagnosticsExport && app != null) {
      const status = Array.from({length: 11}, (_, index) => finite(statusExport(app, index)));
      const tracePtr = traceExport(app);
      const trace = Number.isInteger(tracePtr) && tracePtr > 0 && buffer && tracePtr + 68 * 4 <= buffer.byteLength
        ? Array.from(new Uint32Array(buffer, tracePtr, 68)) : null;
      const diagnostics = wasmArray(() => diagnosticsExport(app), 16, Int32Array);
      if (trace) {
        result.frames.enemy = finite(status[6]);
        result.frames.control = trace[3];
        result.position = wasmFloat2(tracePtr, 7);
        result.capabilities.position = result.position !== null;
        result.input = diagnostics ? {kind: 'logical-buttons', value: diagnostics[11]} : null;
        result.capabilities.input = result.input !== null;
        result.pause = diagnostics ? {active: diagnostics[6] !== 0, raw: diagnostics[6]} : null;
        result.capabilities.pause = result.pause !== null;
        result.status = {supervisorActive: status[0], active: status[1], invalid: status[2], stage: status[3],
          loading: status[7], titleScreen: status[8], cursor: status[9], supervisorState: diagnostics?.[9] ?? null};
        result.capabilities.restartFrame = 'enemy';
      }
      const stats = wasmArray(nativeExport('sdl_stats'), 6, Uint32Array);
      if (stats) setPresentation(stats[5]);
    }
    unsupported.push('standardized-phase');
    if (!result.capabilities.input) unsupported.push('keyboard-gameplay-effect');
    if (!result.capabilities.position) unsupported.push('player-position');
  } else if (game === 'th09') {
    let status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (!status && core) {
      const title = wasmArray(nativeExport('th09_title_status'), 8, Int32Array);
      const session = wasmArray(nativeExport('th09_session_status'), 8, Int32Array);
      const touch = wasmArray(nativeExport('th09_touch_state'), 4, Int32Array);
      if (title && session && touch) status = {title, session, touch};
    }
    if (status && Array.isArray(status.session)) {
      const session = status.session;
      const phaseValue = finite(session[0]);
      result.phase = {value: phaseValue, name: phaseValue === null ? null : th09Phases[phaseValue] ?? null};
      result.frames.session = finite(session[1]);
      result.capabilities.phase = result.phase.name !== null;
      result.capabilities.restartFrame = 'session';
      const appProbe = wasmArray(nativeExport('th09_probe_status'), 16, Int32Array);
      if (appProbe) {
        result.frames.runtime = finite(appProbe[0]);
        result.pause = {active: appProbe[9] === 1, raw: appProbe[9]};
        result.capabilities.pause = true;
      }
      const touchProbe = wasmArray(nativeExport('th09_touch_probe'), 8, Float32Array);
      if (touchProbe && phaseValue === 1) {
        result.position = {x: touchProbe[0], y: touchProbe[1]};
        result.motionSample = {enabled: touchProbe[4] !== 0, target: touchProbe[5] !== 0,
          x: touchProbe[6], y: touchProbe[7]};
        result.capabilities.position = true;
      }
      const metrics = wasmArray(nativeExport('th09_game_metrics'), 10, Uint32Array);
      if (metrics) setPresentation(metrics[9]);
    }
    if (!result.capabilities.phase) unsupported.push('session-phase');
    if (!result.capabilities.position) unsupported.push('player-position-outside-match');
    unsupported.push('keyboard-button-mask');
  } else if (game === 'th10') {
    const status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (Array.isArray(status)) {
      result.status = {screen: finite(status[0]), stage: finite(status[1]), error: finite(status[2]),
        lives: finite(status[3]), power: finite(status[4]), touchContext: finite(status[5]),
        touchActive: finite(status[6]), fire: finite(status[7]), focus: finite(status[8])};
      result.input = {kind: 'native-raw-input', value: finite(status[9])};
      result.capabilities.input = result.input.value !== null;
      const applicationWorld = nativeExport('application_world'), worldActor = nativeExport('world_actor');
      if (applicationWorld && worldActor && buffer) {
        const world = applicationWorld(runtime.app);
        const actor = world ? worldActor(world, 1) : 0;
        if (Number.isInteger(actor) && actor > 0 && actor + 0x3c8 <= buffer.byteLength) {
          const view = new DataView(buffer);
          const x = view.getFloat32(actor + 0x3c0, true), y = view.getFloat32(actor + 0x3c4, true);
          if (Number.isFinite(x) && Number.isFinite(y)) {
            result.position = {x, y};
            result.capabilities.position = true;
          }
        }
      }
    }
    readPresentation();
    unsupported.push('semantic-phase', 'pause-state', 'restart-frame');
    if (!result.capabilities.position) unsupported.push('player-position');
  } else if (game === 'th11') {
    const frame = nativeExport('th11_frame'), phase = nativeExport('th11_phase');
    if (typeof frame === 'function' && typeof phase === 'function') {
      const value = finite(phase());
      result.phase = {value, name: value === null ? null : th11Phases[value] ?? null};
      result.frames.session = finite(frame());
      result.capabilities.phase = result.phase.name !== null;
      result.pause = {active: result.phase.value === 2, raw: result.phase.value};
      result.capabilities.pause = true;
      if (variant === 'normal') result.capabilities.restartFrame = 'session';
    }
    unsupported.push('keyboard-gameplay-effect', 'player-position', 'presentation-frame');
    if (variant === 'multiplayer') unsupported.push('restart');
  } else if (game === 'th15') {
    const frame = nativeExport('th15_frame'), phase = nativeExport('th15_phase');
    if (typeof frame === 'function' && typeof phase === 'function') {
      const value = finite(phase());
      result.phase = {value, name: value === null ? null : th15Phases[value] ?? null};
      result.frames.application = finite(frame());
      result.capabilities.phase = result.phase.name !== null;
      result.pause = {active: result.phase.value === 2, raw: result.phase.value};
      result.capabilities.pause = true;
    }
    unsupported.push('keyboard-gameplay-effect', 'player-position', 'restart-frame', 'presentation-frame');
  } else if (game === 'th20') {
    const status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (Array.isArray(status)) {
      result.status = {scene: finite(status[0]), stage: finite(status[1]), error: finite(status[2]),
        titleState: finite(status[3]), titlePhase: finite(status[4]), titleCursor: finite(status[7]),
        pageCursor: finite(status[8])};
      result.input = {kind: 'native-raw-input', value: finite(status[9])};
      result.capabilities.input = result.input.value !== null;
    }
    readPresentation();
    unsupported.push('semantic-phase', 'player-position', 'pause-state', 'restart-frame');
  } else {
    unsupported.push('unknown-runtime');
  }

  if (!result.capabilities.phase && !unsupported.includes('semantic-phase') && !unsupported.includes('standardized-phase')) unsupported.push('phase');
  if (!result.capabilities.input && !unsupported.some(value => value.includes('input') || value.includes('keyboard'))) unsupported.push('input');
  if (!result.capabilities.position && !unsupported.some(value => value.includes('position'))) unsupported.push('position');
  if (!result.capabilities.pause && !unsupported.includes('pause-state')) unsupported.push('pause-state');
  if (!result.capabilities.presentationFrame && !unsupported.includes('presentation-frame')) unsupported.push('presentation-frame');
  return result;
}

/** Assert only a behavior for which the selected Runtime publishes a suitable observer. */
export function assertRuntimeObservation(observation, behavior, expected = {}) {
  const {game, capabilities} = observation ?? {};
  const missing = (capability, detail = capability) => {
    if (!capabilities?.[capability]) throw new Error(`${game ?? 'runtime'} has no read-only observer for ${detail}`);
  };
  switch (behavior) {
    case 'phase':
      missing('phase');
      assert.equal(observation.phase?.name, expected.name, `${game}: unexpected native phase`);
      break;
    case 'input-held':
      missing('input', 'the requested native input state');
      assert.ok(Number.isFinite(observation.input?.value), `${game}: native input state is unavailable`);
      if (expected.mask !== undefined) assert.equal(observation.input.value & expected.mask, expected.mask,
        `${game}: expected logical input mask was not present`);
      else assert.notEqual(observation.input.value, 0, `${game}: no native input is held`);
      break;
    case 'input-released':
      missing('input', 'the requested native input release');
      assert.ok(Number.isFinite(observation.input?.value), `${game}: native input state is unavailable`);
      if (expected.mask !== undefined) assert.equal(observation.input.value & expected.mask, 0,
        `${game}: native input mask remains held after release`);
      else assert.equal(observation.input.value, expected.value ?? 0, `${game}: native input was not released`);
      break;
    case 'pause-active':
      missing('pause', 'native pause state');
      assert.equal(observation.pause?.active, true, `${game}: native pause state is inactive`);
      break;
    case 'pause-inactive':
      missing('pause', 'native pause state');
      assert.equal(observation.pause?.active, false, `${game}: native pause state remains active`);
      break;
    case 'presentation-advanced':
      missing('presentationFrame', 'a native presentation counter');
      assert.ok(observation.render.presentationFrame > expected.after,
        `${game}: no new native presentation after resize`);
      break;
    case 'position-moved': {
      missing('position', 'native player position');
      const axis = expected.axis;
      assert.ok(axis === 'x' || axis === 'y', 'expected axis must be x or y');
      const before = expected.before?.[axis], after = observation.position?.[axis];
      assert.ok(Number.isFinite(before) && Number.isFinite(after), `${game}: player position is unavailable`);
      const delta = after - before;
      if (expected.direction === 'positive') assert.ok(delta > 0, `${game}: player did not move in positive ${axis}`);
      else if (expected.direction === 'negative') assert.ok(delta < 0, `${game}: player did not move in negative ${axis}`);
      else assert.notEqual(delta, 0, `${game}: player position did not change`);
      if (expected.minDelta !== undefined) assert.ok(Math.abs(delta) >= expected.minDelta,
        `${game}: player movement was below the requested threshold`);
      break;
    }
    case 'frame-advanced':
    case 'frame-frozen': {
      const key = expected.key;
      if (!key || !Object.hasOwn(observation?.frames ?? {}, key)) throw new Error(`${game}: frame ${key ?? '(unspecified)'} is not observable`);
      const value = observation.frames[key], before = expected.before;
      assert.ok(Number.isFinite(value) && Number.isFinite(before), `${game}: native frame is unavailable`);
      if (behavior === 'frame-advanced') assert.ok(value > before, `${game}: ${key} did not advance`);
      else assert.equal(value, before, `${game}: ${key} advanced while the scenario expected it frozen`);
      break;
    }
    case 'restart-frame-reset': {
      const key = capabilities?.restartFrame;
      if (!key || !Object.hasOwn(observation?.frames ?? {}, key)) throw new Error(`${game}: restart frame is not observable`);
      const value = observation.frames[key], before = expected.before;
      assert.ok(Number.isFinite(value) && Number.isFinite(before), `${game}: restart frame is unavailable`);
      assert.ok(value < before, `${game}: observed ${key} did not reset after restart`);
      break;
    }
    default: throw new Error(`Unknown Runtime observation assertion: ${behavior}`);
  }
  return {passed: true, game, behavior};
}
