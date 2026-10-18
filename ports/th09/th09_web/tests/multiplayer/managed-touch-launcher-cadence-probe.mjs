// Test-only, self-contained installer for one already launched real Launcher.
// Import is inert. No input/message/native-input hooks, timer, rAF, tick or pump.
export function installManagedTouchLauncherCadenceProbe(options = {}) {
  const hostWindow = window, hostDocument = document;
  const capacity = Number(options.capacity ?? 8192), boundaryCapacity = 256, phaseLimit = 64;
  if (!Number.isInteger(capacity) || capacity < 256 || capacity > 32768)
    throw Error('Cadence capacity must be an integer from 256 to 32768');
  const label = String(options.label ?? 'managed-touch-launcher-cadence').slice(0, 96);
  const frames = hostDocument.querySelectorAll('[data-runtime-host] iframe');
  const overlays = hostDocument.querySelectorAll('.touch-runtime.touch-controls-surface');
  if (frames.length !== 1 || overlays.length !== 1) throw Error('Cadence requires one real Runtime and touch overlay');
  const frame = frames[0], host = frame.closest('[data-runtime-host]'), overlay = overlays[0];
  const childWindow = frame.contentWindow, childDocument = frame.contentDocument;
  if (!host || !frame.isConnected || !childWindow || !childDocument ||
      childWindow.parent !== hostWindow || childWindow.document !== childDocument)
    throw Error('Cadence requires the current same-origin child document');
  const childURL = new URL(childWindow.location.href), installedHostURL = hostWindow.location.href;
  if (childURL.origin !== hostWindow.location.origin) throw Error('Cadence requires same-origin parent and child');
  const epoch = Number(childURL.searchParams.get('runtimeEpoch'));
  if (!Number.isSafeInteger(epoch) || epoch <= 0 ||
      options.expectedEpoch !== undefined && options.expectedEpoch !== epoch) throw Error('Cadence runtime epoch mismatch');
  const runtime = childWindow.__th09Runtime, core = runtime?.core;
  if (!runtime || !core || childWindow.Module !== core || !core.HEAPU8?.buffer)
    throw Error('Install cadence after the real TH09 launch settles');
  const getterNames = ['_th09_session_status', '_th09_game_metrics', '_th09_rollback_info', '_th09_network_pacing_info'];
  const getters = getterNames.map(name => core[name]), originalFrame = core.onGameFrame;
  if (getters.some(fn => typeof fn !== 'function') || typeof originalFrame !== 'function')
    throw Error('Missing real cadence getter or onGameFrame');
  const marker = Symbol.for('th09.managed-touch-launcher-probe.v1');
  const descriptor = Object.getOwnPropertyDescriptor(core, 'onGameFrame');
  if (originalFrame[marker] || descriptor && !descriptor.configurable &&
      (!Object.hasOwn(descriptor, 'value') || !descriptor.writable))
    throw Error('onGameFrame is already probed or cannot be restored');

  const hostNow = hostWindow.performance.now.bind(hostWindow.performance);
  const childNow = childWindow.performance.now.bind(childWindow.performance);
  const clocks = {
    host: {source: 'captured parent performance.now()', timeOrigin: hostWindow.performance.timeOrigin},
    child: {source: 'captured child performance.now()', timeOrigin: childWindow.performance.timeOrigin},
    comparison: 'Separate clocks; sequence orders observations, not clock synchronization.',
  };
  if (!Number.isFinite(clocks.host.timeOrigin) || !Number.isFinite(clocks.child.timeOrigin))
    throw Error('Non-finite cadence clock origin');
  const frameColumns = ['seq', 'phaseId', 'barrierAtEntry', 'flags',
    'wrapperEnteredAtMs', 'originalEnteredAtMs', 'originalExitedAtMs',
    'sampleStartedAtMs', 'sampleEndedAtMs', 'wrapperBeforeReturnAtMs', 'ok', 'cppRafCallbackWallMs',
    'sessionPhase', 'sessionFrames', 'sessionStage', 'sessionIsReplay', 'rendererFrames', 'presentations',
    'rollbackFrame', 'confirmedOrPublishedMax', 'corrections', 'resimulated', 'captures', 'repairsSent',
    'rollbackReady', 'rollbackPublished', 'pacingVersion', 'missingRemoteInputs',
    'networkIntervalScaleMilli', 'effectiveIntervalScaleMilli', 'predictionFrontier', 'remoteInputFrontier'];
  const boundaryColumns = ['seq', 'kind', 'phaseId', 'barrier', 'hostAtMs', 'childAtMs', 'flags', 'framesStarted', 'reason'];
  const flags = {completed: 1, timesValid: 2, stateValid: 4, originalThrew: 8, hostHidden: 16,
    childHidden: 32, ownerInvalid: 64, stateReadError: 128, frameHookChanged: 256,
    lifecycleDuringCallback: 512, phaseDuringCallback: 1024, stoppedDuringCallback: 2048, invalidArguments: 4096};
  const boundaryKinds = {installed: 1, phase: 2, hostVisibility: 3, childVisibility: 4,
    hostWindowBlur: 5, childWindowBlur: 6, hostElementBlur: 7, childElementBlur: 8,
    hostPagehide: 9, childPagehide: 10, ownerInvalid: 11, stopped: 12};
  const boundaryFlags = {hostHidden: 1, childHidden: 2, hostHasFocus: 4, childHasFocus: 8};
  const ownerReasons = [null, 'parent-document-changed', 'runtime-frame-detached-or-moved',
    'overlay-detached', 'child-document-changed', 'runtime-query-changed',
    'runtime-or-core-owner-changed', 'owner-inaccessible', 'host-pagehide', 'child-pagehide'];
  // Fixed storage, filled before the observation starts. No circular overwrite.
  const frameData = new Float64Array(capacity * frameColumns.length); frameData.fill(NaN);
  const boundaryData = new Float64Array(boundaryCapacity * boundaryColumns.length); boundaryData.fill(NaN);
  const errors = [], listeners = [], phases = [{id: 0, name: 'installed', metadata: null}];
  const counters = {callbacks: 0, completedCallbacks: 0, frameSamples: 0, originalThrows: 0,
    observationErrors: 0, clockErrors: 0, records: 0};
  const overflow = {frames: 0, boundaries: 0, phases: 0, errors: 0};
  let observing = false, disposed = false, phaseId = 0, barrier = 0, sequence = 0;
  let frameCount = 0, boundaryCount = 0, inFlight = 0, invalidOwner = null, stopInfo = null;
  let started = null, fullExport = null, hookAtStop = null, restored = false, restoreSkipped = false, restoreError = null;
  let heapBuffer = null, heapI32 = null, heapU32 = null;

  function note(code, error) {
    counters.observationErrors++;
    if (errors.length >= 16) {overflow.errors++;return;}
    let message = '';
    try {message = error === undefined ? '' : String(error).slice(0, 240);} catch {message = '<unprintable>';}
    errors.push({seq: sequence, code, message});
  }
  function now(which) {
    try {
      const value = which === 0 ? hostNow() : childNow();
      if (!Number.isFinite(value)) throw Error('Non-finite clock');
      return value;
    } catch (error) {counters.clockErrors++;note(which === 0 ? 'host-clock' : 'child-clock', error);return NaN;}
  }
  function ownerReason() {
    try {
      if (hostWindow.document !== hostDocument || frame.ownerDocument !== hostDocument) return 1;
      if (!host.isConnected || !frame.isConnected || !host.contains(frame)) return 2;
      if (!overlay.isConnected || overlay.ownerDocument !== hostDocument) return 3;
      if (frame.contentWindow !== childWindow || frame.contentDocument !== childDocument || childWindow.document !== childDocument) return 4;
      if (childWindow.location.search !== childURL.search) return 5;
      if (childWindow.__th09Runtime !== runtime || runtime.core !== core || childWindow.Module !== core) return 6;
      return 0;
    } catch {return 7;}
  }
  function hiddenFlags() {
    let value = 0;
    try {if (hostDocument.hidden) value |= flags.hostHidden;if (childDocument.hidden) value |= flags.childHidden;}
    catch (error) {note('hidden-state', error);}
    return value;
  }
  function hookCurrent() {try {return core.onGameFrame === frameWrapper;} catch {return false;}}
  function gettersCurrent() {try {return getterNames.every((name, i) => core[name] === getters[i]);} catch {return false;}}
  function boundary(kind, reason = 0) {
    if (boundaryCount >= boundaryCapacity) {overflow.boundaries++;return false;}
    const base = boundaryCount++ * boundaryColumns.length;
    let bits = 0;
    try {
      if (hostDocument.hidden) bits |= boundaryFlags.hostHidden;if (childDocument.hidden) bits |= boundaryFlags.childHidden;
      if (hostDocument.hasFocus()) bits |= boundaryFlags.hostHasFocus;if (childDocument.hasFocus()) bits |= boundaryFlags.childHasFocus;
    } catch (error) {note('boundary-state', error);}
    boundaryData[base] = ++sequence;boundaryData[base + 1] = kind;boundaryData[base + 2] = phaseId;
    boundaryData[base + 3] = barrier;boundaryData[base + 4] = now(0);boundaryData[base + 5] = now(1);
    boundaryData[base + 6] = bits;boundaryData[base + 7] = frameCount;boundaryData[base + 8] = reason;
    counters.records++;return true;
  }
  function invalidate(reason, source) {
    if (!invalidOwner) {
      invalidOwner = {reason: ownerReasons[reason], source, lastSeq: sequence, hostAtMs: now(0), childAtMs: now(1)};
      barrier++;boundary(boundaryKinds.ownerInvalid, reason);
    }
    stop('owner-invalid');
  }
  function restore() {
    for (const entry of listeners) {
      try {entry.target.removeEventListener(entry.event, entry.callback, true);}
      catch (error) {note('remove-listener', error);}
    }
    listeners.length = 0;
    try {
      if (!hookCurrent()) restoreSkipped = true;
      else {
        if (descriptor) Object.defineProperty(core, 'onGameFrame', descriptor);else delete core.onGameFrame;
        restored = core.onGameFrame === originalFrame;
        if (!restored) {restoreError = 'Restored callback differs from saved original';note('restore', restoreError);}
      }
    } catch (error) {restoreError = 'Callback restore failed';note('restore', error);}
  }
  // Stop locks the observation endpoint before restoration or export work.
  // It is safe during a callback; that reserved row remains raw and is flagged.
  function stop(reason = 'explicit-stop') {
    if (!stopInfo) {
      const endHostMs = now(0), endChildMs = now(1);
      observing = false;
      const currentOwner = ownerReason();
      if (currentOwner && !invalidOwner)
        invalidOwner = {reason: ownerReasons[currentOwner], source: 'stop', lastSeq: sequence, hostAtMs: endHostMs, childAtMs: endChildMs};
      hookAtStop = {name: 'onGameFrame', stillOurs: hookCurrent()};
      stopInfo = {reason: String(reason).slice(0, 96), hostAtMs: endHostMs, childAtMs: endChildMs,
        inFlight, ownerValid: currentOwner === 0, gettersCurrent: gettersCurrent(), hookCurrent: hookAtStop.stillOurs};
      boundary(boundaryKinds.stopped, currentOwner);
      disposed = true;restore();
    }
    return {stopped: true, disposed, phaseId, frames: frameCount, lastSeq: sequence,
      endHostMs: stopInfo.hostAtMs, endChildMs: stopInfo.childAtMs, reason: stopInfo.reason};
  }
  function wordPointer(slot, count) {
    if (core[getterNames[slot]] !== getters[slot]) throw Error('Cadence getter binding changed: ' + getterNames[slot]);
    const pointer = getters[slot].call(core), buffer = core.HEAPU8.buffer;
    if (!Number.isSafeInteger(pointer) || pointer < 0 || pointer % 4 !== 0 || pointer + count * 4 > buffer.byteLength)
      throw Error('Invalid cadence getter pointer: ' + getterNames[slot]);
    if (buffer !== heapBuffer) {
      heapBuffer = buffer;heapI32 = new Int32Array(buffer);heapU32 = new Uint32Array(buffer);
    }
    return pointer / 4;
  }
  function readState(base) {
    let at = wordPointer(0, 8);
    frameData[base + 12] = heapI32[at];frameData[base + 13] = heapI32[at + 1];
    frameData[base + 14] = heapI32[at + 5];frameData[base + 15] = heapI32[at + 6];
    at = wordPointer(1, 10);frameData[base + 16] = heapU32[at + 7];frameData[base + 17] = heapU32[at + 9];
    at = wordPointer(2, 8);for (let i = 0; i < 8; i++) frameData[base + 18 + i] = heapU32[at + i];
    at = wordPointer(3, 6);for (let i = 0; i < 6; i++) frameData[base + 26 + i] = heapU32[at + i];
    if (frameData[base + 26] !== 1) throw Error('Unsupported pacing schema');
  }
  function frameWrapper(ok, ms) {
    if (!observing || disposed) return Reflect.apply(originalFrame, this, arguments);
    const entered = now(1);counters.callbacks++;
    if (frameCount >= capacity) {
      overflow.frames++;stop('frame-capacity');return Reflect.apply(originalFrame, this, arguments);
    }
    const base = frameCount++ * frameColumns.length;
    frameData[base] = ++sequence;frameData[base + 1] = phaseId;frameData[base + 2] = barrier;
    frameData[base + 3] = hiddenFlags();frameData[base + 4] = entered;
    frameData[base + 10] = typeof ok === 'boolean' ? +ok : typeof ok === 'number' ? ok : NaN;
    frameData[base + 11] = typeof ms === 'number' ? ms : NaN;counters.records++;inFlight++;
    try {
      const reason = ownerReason();
      if (reason) {frameData[base + 3] |= flags.ownerInvalid;invalidate(reason, 'frame-entry');}
      if (!hookCurrent() && !disposed) {
        frameData[base + 3] |= flags.frameHookChanged;note('frame-hook-changed');stop('hook-changed');
      }
      if (!Number.isFinite(frameData[base + 10]) || !Number.isFinite(ms) || ms < 0) {
        frameData[base + 3] |= flags.invalidArguments;note('callback-arguments');
      }
    } catch (error) {note('frame-entry', error);stop('observer-error');}
    frameData[base + 5] = now(1);
    let threw = true;
    try {const result = Reflect.apply(originalFrame, this, arguments);threw = false;return result;}
    finally {
      frameData[base + 6] = now(1);
      try {
        let bits = frameData[base + 3] | hiddenFlags();
        if (threw) {bits |= flags.originalThrew;counters.originalThrows++;}
        frameData[base + 7] = now(1);
        const reason = ownerReason();
        if (reason) {bits |= flags.ownerInvalid;invalidate(reason, 'frame-after-original');}
        if (!hookCurrent() && !disposed) {bits |= flags.frameHookChanged;note('frame-hook-changed');stop('hook-changed');}
        if (!threw && !disposed && !reason) {
          try {readState(base);bits |= flags.stateValid;counters.frameSamples++;}
          catch (error) {bits |= flags.stateReadError;note('frame-state', error);stop('observer-error');}
        }
        frameData[base + 8] = now(1);
        if (barrier !== frameData[base + 2]) bits |= flags.lifecycleDuringCallback;
        if (phaseId !== frameData[base + 1]) bits |= flags.phaseDuringCallback;
        if (disposed) bits |= flags.stoppedDuringCallback;
        frameData[base + 3] = bits | flags.completed;
      } catch (error) {frameData[base + 3] |= flags.stateReadError;note('frame-finally', error);stop('observer-error');}
      finally {
        frameData[base + 9] = now(1);
        let validTimes = true;
        for (let i = 4; i <= 9; i++)
          if (!Number.isFinite(frameData[base + i]) || i > 4 && frameData[base + i] < frameData[base + i - 1]) validTimes = false;
        if (validTimes) frameData[base + 3] |= flags.timesValid;
        else {note('frame-clock-order');stop('clock-error');}
        counters.completedCallbacks++;inFlight--;
      }
    }
  }
  function register(target, event, callback) {
    target.addEventListener(event, callback, {capture: true, passive: true});
    listeners.push({target, event, callback});
  }
  function lifecycle(kind, sourceWindow, pagehide = false, blur = false) {
    return event => {
      if (!observing || disposed) return;
      try {
        const elementBlur = blur && event.target !== sourceWindow;
        if (!elementBlur) barrier++;
        if (!boundary(elementBlur ? kind + 2 : kind)) {stop('boundary-capacity');return;}
        const reason = ownerReason();
        if (reason) invalidate(reason, 'lifecycle');
        else if (pagehide) invalidate(kind === boundaryKinds.hostPagehide ? 8 : 9, 'pagehide');
      } catch (error) {note('lifecycle', error);stop('observer-error');}
    };
  }
  function phase(name, metadata = null) {
    if (typeof name !== 'string' || !name.length || name.length > 96) throw Error('Cadence phase requires a 1..96 character name');
    if (!observing || disposed) return {accepted: false, disposed, invalidOwner};
    const reason = ownerReason();
    if (reason) {invalidate(reason, 'phase');return {accepted: false, disposed, invalidOwner};}
    if (!hookCurrent() || !gettersCurrent()) {
      note('phase-binding-changed');stop('binding-changed');return {accepted: false, disposed};
    }
    if (phases.length >= phaseLimit) {overflow.phases++;stop('phase-capacity');return {accepted: false, disposed};}
    let copied = null;
    if (metadata !== null) {
      if (typeof metadata !== 'object' || Array.isArray(metadata) || Object.keys(metadata).length > 12)
        throw Error('Cadence phase metadata must contain at most 12 scalar fields');
      copied = {};
      for (const key of Object.keys(metadata)) {
        const value = metadata[key];
        if (key.length > 96 || value !== null && !['string', 'number', 'boolean'].includes(typeof value) ||
            typeof value === 'number' && !Number.isFinite(value) || typeof value === 'string' && value.length > 240)
          throw Error('Cadence phase metadata requires bounded finite scalars');
        copied[key] = value;
      }
    }
    phaseId++;phases.push({id: phaseId, name, metadata: copied});
    if (!boundary(boundaryKinds.phase)) {stop('boundary-capacity');return {accepted: false, disposed};}
    return {accepted: true, phaseId, seq: sequence,
      hostAtMs: boundaryData[(boundaryCount - 1) * boundaryColumns.length + 4],
      childAtMs: boundaryData[(boundaryCount - 1) * boundaryColumns.length + 5]};
  }
  function metadata(includeRecords) {
    const owner = ownerReason(), currentHook = hookCurrent();
    const complete = overflow.frames + overflow.boundaries + overflow.phases + overflow.errors === 0 &&
      counters.observationErrors === 0 && counters.clockErrors === 0 && inFlight === 0 &&
      counters.completedCallbacks === frameCount && counters.callbacks === frameCount;
    return {schema: 'th09-managed-touch-launcher-cadence/v1', version: 1, kind: 'cadence-only', label, epoch, capacity,
      limits: {frames: capacity, boundaries: boundaryCapacity, phases: phaseLimit, errors: 16, bytesPerFrame: 256},
      observing, disposed, phaseId, clocks: {host: {...clocks.host}, child: {...clocks.child}, comparison: clocks.comparison},
      started: started ? {...started} : null, stopped: stopInfo ? {...stopInfo} : null,
      counters: {...counters}, overflow: {...overflow}, owner: {valid: owner === 0, reason: ownerReasons[owner],
        invalidation: invalidOwner ? {...invalidOwner} : null}, gettersCurrent: gettersCurrent(),
      hooksAtDisposal: hookAtStop ? [{...hookAtStop}] : null,
      hooks: [{name: 'onGameFrame', stillOurs: currentHook, restored, restoreSkipped, restoreError}],
      coverage: {includedRecords: includeRecords, retained: frameCount, boundaryCount,
        firstSeq: frameCount ? frameData[0] : null, lastSeq: sequence,
        rawComplete: includeRecords && disposed && complete,
        allHooksCurrent: currentHook, ownerAndHooksQualified: owner === 0 && !invalidOwner &&
          (disposed ? stopInfo?.hookCurrent === true && restored && !restoreSkipped && !restoreError : currentHook) &&
          gettersCurrent() && counters.originalThrows === 0},
      identity: {installedHostURL, installedChildURL: childURL.href, runtimeIframeSelector: '[data-runtime-host] iframe',
        installedOverlaySelector: '.touch-runtime.touch-controls-surface'},
      scope: {
        boundary: 'Only the original onGameFrame property and four read-only native getters after it returns.',
        qualification: 'Cadence-only recorder completeness; no DOM/message/native-input or network route qualification.',
        newStateCriterion: 'Within an eligible same phase/session segment, presentation count must advance and sessionFrames must exceed the previous presentation-advance observation. Keep raw counters and regressions.',
        rawGaps: 'Retain every adjacent frame entry, including hidden, lifecycle, throwing and invalid rows. Derived continuous segments must break at those boundaries; excluded raw intervals remain evidence.',
        openTail: 'Use the captured child stop time as a censored bound after the last valid child-clock observation; do not invent a following callback or state.',
        callbackTiming: 'C++ ms is elapsed whole rAF callback work before onGameFrame; original JS wall time is separately bracketed. Neither is pure CPU or replay time.',
        samples: 'Four sequential getters, not an atomic state, input capture, physical presentation or native packet-acceptance trace.',
        scheduling: 'No pending-rAF identity is observed. A callback gap does not prove an already queued rAF was not dispatched.',
        inputs: 'No input generation or per-MOVE/native-input/postMessage wrapping. The driver retains planned/actual CDP sends separately; this probe cannot prove their delivery.',
        lifecycle: 'Focus is recorded at boundaries, never an input eligibility gate. Owner and hook checks are point-in-time; replacement and restoration between checks may escape observation.',
        noGameTickDrawOrPump: true, noTimersOrRafScheduled: true, observerOverheadCalibrated: false,
        exports: 'Full numeric-row materialization only after stop has locked both endpoints and restored the captured callback; never during observation.',
      }};
  }
  function snapshot({includeRecords = false} = {}) {
    if (observing && !disposed) {
      const reason = ownerReason();
      if (reason) invalidate(reason, 'snapshot');
      else if (!hookCurrent() || !gettersCurrent()) {note('snapshot-binding-changed');stop('binding-changed');}
    }
    if (!includeRecords) return metadata(false);
    if (!disposed || observing || inFlight !== 0) throw Error('Stop cadence and finish the original callback before full export');
    if (fullExport) return fullExport;
    const beginHostMs = now(0), beginChildMs = now(1);
    const result = metadata(true);
    result.schemas = {frame: frameColumns.slice(), boundary: boundaryColumns.slice(), flags: {...flags},
      boundaryKinds: {...boundaryKinds}, boundaryFlags: {...boundaryFlags}, ownerReasons: ownerReasons.slice()};
    result.phases = phases.map(value => ({...value, metadata: value.metadata ? {...value.metadata} : null}));
    result.records = Array.from({length: frameCount}, (_, i) =>
      Array.from(frameData.subarray(i * frameColumns.length, (i + 1) * frameColumns.length)));
    result.boundaries = Array.from({length: boundaryCount}, (_, i) =>
      Array.from(boundaryData.subarray(i * boundaryColumns.length, (i + 1) * boundaryColumns.length)));
    result.exportWork = {hostStartedAtMs: beginHostMs, hostFinishedAtMs: now(0),
      childStartedAtMs: beginChildMs, childFinishedAtMs: now(1), outsideObservationWindow: true};
    // Include any export-clock failure without copying numeric rows again.
    Object.assign(result, metadata(true));result.errors = errors.map(value => ({...value}));
    fullExport = result;return result;
  }
  try {
    Object.defineProperty(frameWrapper, marker, {value: true});
    Object.defineProperty(core, 'onGameFrame', {value: frameWrapper, enumerable: descriptor?.enumerable ?? true,
      configurable: descriptor?.configurable ?? true,
      writable: descriptor && Object.hasOwn(descriptor, 'writable') ? descriptor.writable : true});
    if (!hookCurrent()) throw Error('Cadence callback replacement did not stick');
    register(hostWindow, 'blur', lifecycle(boundaryKinds.hostWindowBlur, hostWindow, false, true));
    register(childWindow, 'blur', lifecycle(boundaryKinds.childWindowBlur, childWindow, false, true));
    register(hostDocument, 'visibilitychange', lifecycle(boundaryKinds.hostVisibility, hostWindow));
    register(childDocument, 'visibilitychange', lifecycle(boundaryKinds.childVisibility, childWindow));
    register(hostWindow, 'pagehide', lifecycle(boundaryKinds.hostPagehide, hostWindow, true));
    register(childWindow, 'pagehide', lifecycle(boundaryKinds.childPagehide, childWindow, true));
    started = {hostAtMs: now(0), childAtMs: now(1)};
    observing = true;boundary(boundaryKinds.installed);
    if (counters.observationErrors) throw Error('Cadence initialization observation failed');
  } catch (error) {stop('installation-failed');throw error;}
  return {phase, snapshot, stop, dispose() {stop('dispose');return snapshot({includeRecords: true});}};
}
