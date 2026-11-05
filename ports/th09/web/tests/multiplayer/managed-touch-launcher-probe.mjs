// Explicit test-only observation of an already launched React Launcher.
// Importing this module does not install hooks. The exported function is wholly
// self-contained so its toString() can be evaluated in the parent page.
export function installManagedTouchLauncherProbe(options = {}) {
  const hostWindow = window, hostDocument = document;
  const capacity = Number(options.capacity ?? 16384);
  if (!Number.isInteger(capacity) || capacity < 256 || capacity > 131072)
    throw Error('Launcher probe capacity must be an integer from 256 to 131072');
  const label = String(options.label ?? 'managed-touch-launcher').slice(0, 96);
  const frames = hostDocument.querySelectorAll('[data-runtime-host] iframe');
  if (frames.length !== 1) throw Error('Launcher probe requires exactly one runtime iframe');
  const frame = frames[0], host = frame.closest('[data-runtime-host]');
  const overlays = hostDocument.querySelectorAll('.touch-runtime.touch-controls-surface');
  if (overlays.length !== 1) throw Error('Launcher probe requires exactly one real touch overlay');
  const overlay = overlays[0], childWindow = frame.contentWindow, childDocument = frame.contentDocument;
  if (!host || !frame.isConnected || !childWindow || !childDocument ||
      childWindow.parent !== hostWindow || childWindow.document !== childDocument)
    throw Error('Launcher probe requires the current same-origin child document');
  const childURL = new URL(childWindow.location.href), hostOrigin = hostWindow.location.origin;
  if (childURL.origin !== hostOrigin) throw Error('Launcher probe requires same-origin parent and child');
  const epoch = Number(childURL.searchParams.get('runtimeEpoch'));
  if (!Number.isSafeInteger(epoch) || epoch <= 0 ||
      (options.expectedEpoch !== undefined && options.expectedEpoch !== epoch))
    throw Error('Launcher probe runtime epoch is invalid or differs from expectedEpoch');
  const runtime = childWindow.__th09Runtime, core = runtime?.core;
  if (!runtime || !core || childWindow.Module !== core || !core.HEAPU8?.buffer)
    throw Error('Launcher probe must be installed after the real TH09 launch settles');

  const nativeNames = ['_th09_touch', '_th09_touch_cancel', '_th09_touch_controls',
    '_th09_touch_stick', '_th09_touch_options', '_th09_key', '_th09_keys_clear'];
  const getterNames = ['_th09_title_status', '_th09_session_status', '_th09_touch_state',
    '_th09_touch_probe', '_th09_rollback_info', '_th09_game_metrics'];
  const getters = Object.fromEntries(getterNames.map(name => [name, core[name]]));
  for (const name of [...nativeNames, ...getterNames, 'onGameFrame'])
    if (typeof core[name] !== 'function') throw Error('Missing real TH09 hook/getter: ' + name);
  if (typeof childWindow.postMessage !== 'function') throw Error('Missing child postMessage');
  const marker = Symbol.for('th09.managed-touch-launcher-probe.v1');
  const hookSpecs = [[childWindow, 'postMessage'], ...nativeNames.map(name => [core, name]), [core, 'onGameFrame']];
  for (const [object, name] of hookSpecs) {
    const descriptor = Object.getOwnPropertyDescriptor(object, name);
    if (object[name][marker] || descriptor && !descriptor.configurable &&
        (!Object.hasOwn(descriptor, 'value') || !descriptor.writable))
      throw Error('Hook is already probed or cannot be restored: ' + name);
  }

  const installedHostURL = hostWindow.location.href;
  if (typeof hostWindow.structuredClone !== 'function') throw Error('Launcher probe requires structuredClone for detached snapshots');
  const cloneSnapshot = hostWindow.structuredClone.bind(hostWindow);
  const childObjectPrototype = childWindow.Object.prototype;
  const hostPerformance = hostWindow.performance, childPerformance = childWindow.performance;
  const hostNow = hostPerformance.now.bind(hostPerformance), childNow = childPerformance.now.bind(childPerformance);
  const clocks = {
    host: {source: 'captured parent performance.now()', timeOrigin: hostPerformance.timeOrigin},
    child: {source: 'captured child performance.now()', timeOrigin: childPerformance.timeOrigin},
    comparison: 'Distinct clocks. No host-minus-child durations are computed. Global seq orders observations only.',
  };
  const records = new Array(capacity), hooks = [], listeners = [], errors = [];
  const counters = {records: 0, overwritten: 0, observationErrors: 0, payloadTruncations: 0,
    pointerEvents: {}, postSubmissions: {}, childDeliveries: {}, nativeCalls: {}, callbacks: 0, frameSamples: 0};
  let head = 0, retained = 0, sequence = 0, callSerial = 0, observing = true, disposed = false;
  let invalidOwner = null, phaseId = 0, currentPhase = 'installed', hooksAtDisposal = null;
  function phaseCounter(id, name) {
    return {id, name, records: 0, pointerEvents: {}, postSubmissions: {}, childDeliveries: {}, nativeCalls: {}, callbacks: 0, frameSamples: 0};
  }
  const phases = [{id: 0, name: currentPhase}], phaseCounts = [phaseCounter(0, currentPhase)];
  const knownCommands = new Set(['configure', 'resources', 'keyboard', 'keyboard-clear', 'touch-cancel',
    'direct-touch', 'touch-controls', 'launch', 'sync', 'list', 'read', 'write', 'remove',
    'network-open', 'network-cancel']);
  const schemas = {
    title: ['frames', 'inTitle', 'screen', 'state', 'selection', 'loadFrame', 'leaving', 'requestedTransition'],
    session: ['phase', 'frames', 'replayFrame', 'replayLength', 'continues', 'stage', 'isReplay', 'recordable'],
    touchState: ['context', 'ready', 'active', 'inTitle'],
    touchProbe: ['positionX', 'positionY', 'velocityX', 'velocityY', 'motionEnabled', 'motionTarget', 'motionX', 'motionY'],
    rollback: ['frame', 'confirmedOrPublishedMax', 'corrections', 'resimulated', 'captures', 'repairsSent', 'ready', 'rollbackPublished'],
    rollbackField1: 'INVALID_FRAME confirmation uses rollback_published; otherwise max(rollback_published, confirmed+1)',
    gameMetrics: ['batches', 'uploadBytes', 'readBytes', 'vertexUploadBytes', 'programCompiles',
      'bufferReplacements', 'bufferSubUpdates', 'rendererFrames', 'resamples', 'presentations'],
  };
  const scope = {
    boundary: 'Real host DOM, parent-installed child postMessage property, child message observation, original native exports, post-onGameFrame state getters',
    noInputAdded: true, noCommandCalled: true, noGameTickDrawOrPump: true, noTimersOrRafScheduled: true,
    callerOfPostMessageNotIndependentlyIdentified: true,
    submission: 'Method invocation/return only; neither a child dispatch ACK nor a delivery timestamp',
    delivery: 'Capture listener registered after real launch; registration/event-phase ordering may put this observation after native calls',
    nativeTiming: 'Synchronous original exported-function wall duration; includes its existing wrappers/bridge, not pure native CPU',
    callbackTiming: 'ms argument is the whole C++ rAF callback wall work, with zero or multiple game ticks; source onGameFrame JS follows it',
    sampleBoundary: 'Read-only getters after original onGameFrame returns; not atomic across getters, capture time, input tape or physical presentation',
    inputTiming: 'DOM isTrusted is retained but does not prove a physical finger. This probe measures no physical input-to-display latency.',
    observerOverheadCalibrated: false,
    hookCoverage: 'Only calls through installed properties are observed; previously cached original references can bypass them. Binding checks are point-in-time.',
    snapshots: 'Snapshot copying/serialization is extra diagnostic work. Prefer disposing before the final snapshot.',
  };

  function now(clock) {
    try {
      const value = clock === 'host' ? hostNow() : childNow();
      if (!Number.isFinite(value)) throw Error('Non-finite performance clock');
      return value;
    } catch (error) {noteError('clock:' + clock, error);return null;}
  }
  function errorText(error) {
    try {return String(error).slice(0, 240);} catch {return '<unprintable error>';}
  }
  function noteError(stage, error) {
    counters.observationErrors++;
    if (errors.length < 16) errors.push({stage, error: errorText(error)});
  }
  function observe(stage, action) {
    try {return action();} catch (error) {noteError(stage, error);return null;}
  }
  function increment(object, key) {object[key] = (object[key] || 0) + 1;}
  function bump(group, key) {increment(counters[group], key);increment(phaseCounts[phaseId][group], key);}
  function append(clock, role, kind, fields = {}, atMs = now(clock)) {
    const row = {seq: ++sequence, clock, role, kind, atMs, phaseId, ...fields};
    counters.records++;phaseCounts[phaseId].records++;
    const index = (head + retained) % capacity;
    if (retained === capacity) {records[head] = row;head = (head + 1) % capacity;counters.overwritten++;}
    else {records[index] = row;retained++;}
    return row;
  }
  function ownerReason() {
    try {
      if (hostWindow.document !== hostDocument || frame.ownerDocument !== hostDocument) return 'parent-document-changed';
      if (!host.isConnected || !frame.isConnected || !host.contains(frame)) return 'runtime-frame-detached-or-moved';
      if (!overlay.isConnected || overlay.ownerDocument !== hostDocument) return 'overlay-detached';
      if (frame.contentWindow !== childWindow || frame.contentDocument !== childDocument ||
          childWindow.document !== childDocument) return 'child-document-changed';
      if (childWindow.location.search !== childURL.search) return 'runtime-query-changed';
      if (childWindow.__th09Runtime !== runtime || runtime.core !== core || childWindow.Module !== core)
        return 'runtime-or-core-owner-changed';
      return null;
    } catch {return 'owner-inaccessible';}
  }
  function invalidate(reason, clock, boundary) {
    if (!observing) return false;
    invalidOwner = {reason, clock, atMs: now(clock), boundary, lastObservedSeq: sequence};
    append(clock, 'probe', 'owner-invalid', {...invalidOwner});
    observing = false;return false;
  }
  function eligible(clock, boundary) {
    if (!observing || disposed) return false;
    const reason = ownerReason();
    return reason ? invalidate(reason, clock, boundary) : true;
  }
  function ownValue(object, key) {
    if (!object || typeof object !== 'object') return undefined;
    const descriptor = Object.getOwnPropertyDescriptor(object, key);
    return descriptor && Object.hasOwn(descriptor, 'value') ? descriptor.value : undefined;
  }
  function envelope(data) {
    const command = ownValue(data, 'command');
    return ownValue(data, 'protocol') === 'eagler-touhou/1' && ownValue(data, 'game') === 'th09' &&
      ownValue(data, 'epoch') === epoch && typeof command === 'string' && knownCommands.has(command) ? command : null;
  }
  function payload(value) {
    const state = {nodes: 0, truncated: 0};
    function clone(item, depth) {
      if (++state.nodes > 128 || depth > 4) {state.truncated++;return {omitted: 'copy-limit'};}
      if (item === null || typeof item === 'boolean') return item;
      if (typeof item === 'number') return Number.isFinite(item)
        ? Object.is(item, -0) ? {number: '-0'} : item : {number: String(item)};
      if (typeof item === 'string') {
        if (item.length > 1024) {state.truncated++;return item.slice(0, 1024);}
        return item;
      }
      if (typeof item !== 'object') return {type: typeof item};
      const prototype = Object.getPrototypeOf(item);
      if (!Array.isArray(item) && prototype !== null && prototype !== Object.prototype && prototype !== childObjectPrototype) {
        state.truncated++;return {omitted: 'non-plain-object'};
      }
      const keys = Object.keys(item), result = Array.isArray(item) ? [] : {};
      if (keys.length > 32) state.truncated++;
      for (const key of keys.slice(0, 32)) {
        const descriptor = Object.getOwnPropertyDescriptor(item, key);
        const copied = descriptor && Object.hasOwn(descriptor, 'value')
          ? clone(descriptor.value, depth + 1) : {omitted: 'accessor'};
        if (!descriptor || !Object.hasOwn(descriptor, 'value')) state.truncated++;
        Object.defineProperty(result, key, {value: copied, enumerable: true, writable: true, configurable: true});
      }
      return result;
    }
    const data = clone(value, 0);counters.payloadTruncations += state.truncated;
    return {data, truncated: state.truncated};
  }
  function register(target, event, callback) {
    target.addEventListener(event, callback, {capture: true, passive: true});
    listeners.push({target, event, callback});
  }
  function install(object, name, wrapper) {
    const original = object[name], descriptor = Object.getOwnPropertyDescriptor(object, name);
    Object.defineProperty(wrapper, marker, {value: true});
    const replacement = {value: wrapper, enumerable: descriptor?.enumerable ?? true,
      configurable: descriptor?.configurable ?? true, writable: descriptor && Object.hasOwn(descriptor, 'writable') ? descriptor.writable : true};
    Object.defineProperty(object, name, replacement);
    hooks.push({object, name, original, descriptor, wrapper, restored: false, restoreSkipped: false, restoreError: null});
    if (object[name] !== wrapper) throw Error('Hook replacement did not stick: ' + name);
  }
  function readWords(name, count, type) {
    if (core[name] !== getters[name]) throw Error('Diagnostic getter binding changed: ' + name);
    const pointer = Reflect.apply(getters[name], core, []);
    // A getter may run after memory growth. Always read the current HEAPU8 buffer.
    const buffer = core.HEAPU8.buffer;
    if (!Number.isSafeInteger(pointer) || pointer < 0 || pointer + count * 4 > buffer.byteLength)
      throw Error('Invalid diagnostic pointer: ' + name);
    const view = new DataView(buffer, pointer, count * 4), values = [], bits = [];
    for (let i = 0; i < count; i++) {
      values.push(type === 'f32' ? view.getFloat32(i * 4, true)
        : type === 'i32' ? view.getInt32(i * 4, true) : view.getUint32(i * 4, true));
      if (type === 'f32') bits.push(view.getUint32(i * 4, true));
    }
    return type === 'f32' ? {values, bits} : values;
  }
  function nativeWrapper(name, original) {
    return function(...args) {
      const ticket = observe(name + ':entry', () => {
        if (!eligible('child', name)) return null;
        bump('nativeCalls', name);
        const callId = ++callSerial, copied = payload(args);
        const row = append('child', 'child-native', 'native-entry', {name, callId, args: copied.data, argsTruncated: copied.truncated});
        return {callId, row};
      });
      const enteredAtMs = ticket ? now('child') : null;
      if (ticket) ticket.row.originalEnteredAtMs = enteredAtMs;
      let thrown = false, failure;
      try {return Reflect.apply(original, this, args);}
      catch (error) {thrown = true;failure = error;throw error;}
      finally {
        const exitedAtMs = ticket ? now('child') : null;
        if (ticket) observe(name + ':exit', () => {
          if (!eligible('child', name + ':exit')) return;
          append('child', 'child-native', 'native-exit', {name, callId: ticket.callId, originalEnteredAtMs: enteredAtMs,
            originalExitedAtMs: exitedAtMs, originalWallMs: enteredAtMs === null || exitedAtMs === null ? null : exitedAtMs - enteredAtMs,
            thrown, error: thrown ? errorText(failure) : null}, exitedAtMs);
        });
      }
    };
  }
  const originalPostMessage = childWindow.postMessage;
  function postMessageWrapper(...args) {
    const ticket = observe('postMessage:entry', () => {
      const command = envelope(args[0]);
      if (!command || !eligible('host', 'postMessage')) return null;
      bump('postSubmissions', command);
      const callId = ++callSerial, copied = payload(args[0]);
      const target = typeof args[1] === 'string' ? args[1] : ownValue(args[1], 'targetOrigin');
      const row = append('host', 'host-submit', 'postMessage-entry',
        {callId, command, message: copied.data, payloadTruncated: copied.truncated, targetOrigin: target ?? null});
      return {callId, command, row};
    });
    const enteredAtMs = ticket ? now('host') : null;
    if (ticket) ticket.row.originalEnteredAtMs = enteredAtMs;
    let thrown = false, failure;
    try {return Reflect.apply(originalPostMessage, this, args);}
    catch (error) {thrown = true;failure = error;throw error;}
    finally {
      const exitedAtMs = ticket ? now('host') : null;
      if (ticket) observe('postMessage:exit', () => {
        if (!eligible('host', 'postMessage:exit')) return;
        append('host', 'host-submit', 'postMessage-exit', {callId: ticket.callId, command: ticket.command,
          originalEnteredAtMs: enteredAtMs, originalExitedAtMs: exitedAtMs,
          originalWallMs: enteredAtMs === null || exitedAtMs === null ? null : exitedAtMs - enteredAtMs,
          thrown, error: thrown ? errorText(failure) : null}, exitedAtMs);
      });
    }
  }
  const originalFrame = core.onGameFrame;
  function frameWrapper(...args) {
    const ticket = observe('onGameFrame:entry', () => {
      if (!eligible('child', 'onGameFrame')) return null;
      counters.callbacks++;phaseCounts[phaseId].callbacks++;
      const callId = ++callSerial, copied = payload(args);
      const row = append('child', 'child-frame', 'frame-entry', {callId, args: copied.data, argsTruncated: copied.truncated,
        cppRafCallbackWallMs: typeof args[1] === 'number' ? args[1] : null});
      return {callId, row};
    });
    const enteredAtMs = ticket ? now('child') : null;
    if (ticket) {ticket.enteredAtMs = enteredAtMs;ticket.row.originalEnteredAtMs = enteredAtMs;}
    let thrown = false, failure;
    try {return Reflect.apply(originalFrame, this, args);}
    catch (error) {thrown = true;failure = error;throw error;}
    finally {
      const originalExitedAtMs = ticket ? now('child') : null;
      if (ticket) observe('onGameFrame:sample', () => {
        if (!eligible('child', 'onGameFrame:after-original')) return;
        const sampledAtMs = now('child');
        const state = thrown ? null : {
          title: readWords('_th09_title_status', 8, 'i32'), session: readWords('_th09_session_status', 8, 'i32'),
          touchState: readWords('_th09_touch_state', 4, 'i32'), touchProbe: readWords('_th09_touch_probe', 8, 'f32'),
          rollback: readWords('_th09_rollback_info', 8, 'u32'), gameMetrics: readWords('_th09_game_metrics', 10, 'u32'),
        };
        const afterSampleAtMs = now('child');
        if (state) {counters.frameSamples++;phaseCounts[phaseId].frameSamples++;}
        append('child', 'child-frame', 'frame-after-original', {callId: ticket.callId, args: payload(args).data,
          cppRafCallbackWallMs: typeof args[1] === 'number' ? args[1] : null,
          originalEnteredAtMs: ticket.enteredAtMs, originalExitedAtMs, sampledAtMs, afterSampleAtMs,
          originalJsWallMs: ticket.enteredAtMs === null || originalExitedAtMs === null ? null : originalExitedAtMs - ticket.enteredAtMs,
          thrown, error: thrown ? errorText(failure) : null, state,
          childHidden: childDocument.hidden, hostHidden: hostDocument.hidden}, afterSampleAtMs);
      });
    }
  }
  function onPointer(event) {
    observe('host-pointer', () => {
      if (!eligible('host', 'host-pointer')) return;
      bump('pointerEvents', event.type);
      append('host', 'host-overlay', 'pointer', {type: event.type, isTrusted: event.isTrusted,
        eventTimeStamp: event.timeStamp, pointerId: event.pointerId, pointerType: event.pointerType,
        isPrimary: event.isPrimary, clientX: event.clientX, clientY: event.clientY,
        pageX: event.pageX, pageY: event.pageY, screenX: event.screenX, screenY: event.screenY,
        pressure: event.pressure, width: event.width, height: event.height, button: event.button, buttons: event.buttons,
        defaultPreventedAtObservation: event.defaultPrevented, eventPhase: event.eventPhase,
        targetTag: event.target?.nodeName ?? null,
        targetClass: typeof event.target?.className === 'string' ? event.target.className.slice(0, 160) : null});
    });
  }
  function onMessage(event) {
    observe('child-message', () => {
      const command = envelope(event.data);
      if (!command || event.source !== hostWindow || event.origin !== hostOrigin || !eligible('child', 'child-message')) return;
      bump('childDeliveries', command);
      const copied = payload(event.data);
      append('child', 'child-delivery', 'message-observed', {command, message: copied.data, payloadTruncated: copied.truncated,
        origin: event.origin, sourceIsCapturedParent: true, isTrusted: event.isTrusted,
        eventTimeStamp: event.timeStamp, eventPhase: event.eventPhase, registeredAfterLaunch: true});
    });
  }
  function lifecycle(clock, boundary) {
    return event => observe(boundary, () => {
      if (!eligible(clock, boundary)) return;
      const targetWindow = clock === 'host' ? hostWindow : childWindow;
      const targetDocument = clock === 'host' ? hostDocument : childDocument;
      const windowTarget = event.target === targetWindow;
      // Capture sees element blur as well. Retain it without claiming that
      // the Window/document lost focus; real Window blur stays a boundary.
      const elementBlur = boundary.endsWith('blur') && !windowTarget;
      append(clock, 'probe', elementBlur ? 'focus-observed' : 'lifecycle', {
        boundary, windowTarget, documentTarget: event.target === targetDocument,
        targetTag: typeof event.target?.tagName === 'string' ? event.target.tagName : null,
        eventPhase: event.eventPhase, hostHasFocus: hostDocument.hasFocus(), childHasFocus: childDocument.hasFocus(),
        hostHidden: hostDocument.hidden, childHidden: childDocument.hidden});
      if (boundary.endsWith('pagehide')) invalidate(boundary, clock, boundary);
    });
  }
  function restore() {
    for (const {target, event, callback} of listeners.splice(0)) {
      try {target.removeEventListener(event, callback, true);} catch (error) {noteError('remove-listener:' + event, error);}
    }
    for (const hook of [...hooks].reverse()) {
      try {
        if (hook.object[hook.name] !== hook.wrapper) {hook.restoreSkipped = true;continue;}
        if (hook.descriptor) Object.defineProperty(hook.object, hook.name, hook.descriptor);
        else delete hook.object[hook.name];
        hook.restored = hook.object[hook.name] === hook.original;
        if (!hook.restored) hook.restoreError = 'restored value differs from saved original';
      } catch (error) {hook.restoreError = errorText(error);noteError('restore:' + hook.name, error);}
    }
  }
  try {
    install(childWindow, 'postMessage', postMessageWrapper);
    for (const name of nativeNames) install(core, name, nativeWrapper(name, core[name]));
    install(core, 'onGameFrame', frameWrapper);
    for (const event of ['pointerdown', 'pointermove', 'pointerup', 'pointercancel', 'lostpointercapture'])
      register(overlay, event, onPointer);
    register(childWindow, 'message', onMessage);
    register(hostWindow, 'blur', lifecycle('host', 'host-blur'));
    register(childWindow, 'blur', lifecycle('child', 'child-blur'));
    register(hostDocument, 'visibilitychange', lifecycle('host', 'host-visibilitychange'));
    register(childDocument, 'visibilitychange', lifecycle('child', 'child-visibilitychange'));
    register(hostWindow, 'pagehide', lifecycle('host', 'host-pagehide'));
    register(childWindow, 'pagehide', lifecycle('child', 'child-pagehide'));
    append('host', 'probe', 'installed', {label, epoch, capacity, overlaySelector: '.touch-runtime.touch-controls-surface',
      childMessageRegisteredAfterLaunch: true});
  } catch (error) {observing = false;disposed = true;restore();throw error;}

  function hookStatus() {
    return hooks.map(hook => {
      let stillOurs = null;
      try {stillOurs = hook.object[hook.name] === hook.wrapper;} catch {}
      return {name: hook.name, stillOurs, restored: hook.restored, restoreSkipped: hook.restoreSkipped, restoreError: hook.restoreError};
    });
  }
  function snapshot({includeRecords = true} = {}) {
    const snapshotStartedHostMs = now('host');
    if (observing && !disposed) observe('snapshot-owner', () => eligible('host', 'snapshot'));
    const currentHooks = hookStatus();
    const chronological = includeRecords ? Array.from({length: retained}, (_, i) => records[(head + i) % capacity]) : [];
    const result = {version: 1, label, epoch, capacity, clocks, schemas, scope, observedCommands: [...knownCommands],
      identity: {installedHostURL, installedChildURL: childURL.href,
        runtimeIframeSelector: '[data-runtime-host] iframe', installedOverlaySelector: '.touch-runtime.touch-controls-surface'},
      observedAt: {hostMs: now('host'), childMs: now('child')}, observing, disposed,
      owner: {valid: ownerReason() === null, invalidation: invalidOwner},
      hooks: currentHooks, hooksAtDisposal,
      gettersCurrent: observe('snapshot-getters', () => getterNames.every(name => core[name] === getters[name])),
      counters, errors, phases, byPhase: phaseCounts,
      coverage: {retained, includedRecords: includeRecords, firstSeq: retained ? records[head].seq : null,
        lastSeq: retained ? records[(head + retained - 1) % capacity].seq : null,
        overwritten: counters.overwritten,
        rawComplete: includeRecords && counters.overwritten === 0 && counters.observationErrors === 0 && counters.payloadTruncations === 0,
        allHooksCurrent: currentHooks.every(hook => hook.stillOurs === true)},
      records: chronological};
    // Snapshot users may keep the result on window while observation continues.
    // Detach nested counters/records so later events cannot rewrite that evidence.
    const detached = cloneSnapshot(result), snapshotFinishedHostMs = now('host');
    detached.snapshotWork = {clock: 'host', startedAtMs: snapshotStartedHostMs, finishedAtMs: snapshotFinishedHostMs,
      copyWallMs: snapshotStartedHostMs === null || snapshotFinishedHostMs === null ? null : snapshotFinishedHostMs - snapshotStartedHostMs};
    return detached;
  }
  return {
    phase(name, metadata = null) {
      if (typeof name !== 'string' || !name.length || name.length > 96) throw Error('Phase requires a 1..96 character name');
      if (phases.length >= 64) throw Error('Launcher probe phase limit reached');
      if (!eligible('host', 'phase')) return {accepted: false, disposed, invalidOwner};
      phaseId++;currentPhase = name;phases.push({id: phaseId, name});phaseCounts.push(phaseCounter(phaseId, name));
      const copied = payload(metadata);
      const row = append('host', 'probe', 'phase', {name, metadata: copied.data, payloadTruncated: copied.truncated});
      return {accepted: true, phaseId, seq: row.seq, hostAtMs: row.atMs};
    },
    snapshot,
    dispose() {
      if (!disposed) {
        hooksAtDisposal = hookStatus();
        if (observing) append('host', 'probe', 'disposed');
        observing = false;disposed = true;restore();
      }
      return snapshot();
    },
  };
}
