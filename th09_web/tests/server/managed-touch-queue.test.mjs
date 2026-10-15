import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import vm from 'node:vm';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {setImmediate as nodeTurn} from 'node:timers/promises';

// Real-source candidate goals by default; explicit baseline characterization is
// TH09_TOUCH_QUEUE_REQUIRE_GOALS=0. This is not a browser/native/latency test.
// Reuse managed-storage.test.mjs's VM/source-loading approach without its build
// or persistent-storage fixture. Keep the real message and lifecycle listeners.
// No test calls command(), dispatchCommand(), or writes the launched variable.
const runtimeURL = process.env.TH09_TOUCH_QUEUE_RUNTIME_SOURCE
  ? pathToFileURL(resolve(process.env.TH09_TOUCH_QUEUE_RUNTIME_SOURCE))
  : new URL('../../sdl-runtime/managed.mjs', import.meta.url);
const keyboardURL = new URL('./directory-keyboard.mjs', runtimeURL);
const replayURL = new URL('./motion-replay.mjs', runtimeURL);
const {createAutoReplay} = await import(new URL('./auto-replay.mjs', runtimeURL));
const [{createBrowserKeyboard}, {exportReplayName, importReplayName}] =
  await Promise.all([import(keyboardURL.href), import(replayURL.href)]);
const sourceFiles = [
  ['managed', runtimeURL], ['keyboard', keyboardURL], ['replay', replayURL],
  ['test', new URL(import.meta.url)],
];
const sourceTexts = await Promise.all(sourceFiles.map(async ([name, url]) => [name, await readFile(url, 'utf8')]));
const sourceIdentity = Object.fromEntries(sourceTexts.map(([name, text]) =>
  [name, createHash('sha256').update(text).digest('hex')]));
const runtimeSource = sourceTexts.find(([name]) => name === 'managed')[1];
const runtimeBody = runtimeSource.replace(/^import .*;\r?\n/gm, '');
assert.ok(!/^import /m.test(runtimeBody), 'fixture must account for every module dependency');

const ORIGIN = 'https://managed-touch-queue.invalid';
const PROTOCOL = 'eagler-touhou/1';
const TRACE_LIMIT = 256, MESSAGE_LIMIT = 48, PUMP_LIMIT = 160;
const goalSetting = process.env.TH09_TOUCH_QUEUE_REQUIRE_GOALS;
assert.ok(goalSetting === undefined || goalSetting === '0' || goalSetting === '1', 'goal option must be 0 or 1');
const requireGoals = goalSetting !== '0', mode = requireGoals ? 'candidate-goals' : 'characterization';
const sourcePaths = Object.fromEntries(sourceFiles.map(([name, url]) => [name, url.href]));
const touchOps = new Set(['touch', 'touch-controls', 'touch-stick', 'touch-cancel']);
const copy = value => JSON.parse(JSON.stringify(value));

function deferred() {
  let resolve, settled = false;
  const promise = new Promise(done => {resolve = done;});
  return {promise, release(value) {if (!settled) {settled = true;resolve(value);}}};
}

function eventTarget() {
  const listeners = new Map();
  return {
    addEventListener(name, callback) {
      const list = listeners.get(name) || [];list.push(callback);listeners.set(name, list);
    },
    removeEventListener(name, callback) {
      listeners.set(name, (listeners.get(name) || []).filter(item => item !== callback));
    },
    dispatch(name, event = {}) {
      for (const callback of [...(listeners.get(name) || [])]) callback(event);
    },
    count(name) {return (listeners.get(name) || []).length;},
  };
}

function openRuntime({epoch = 73, holdModule = false, holdData = false, holdConnect = false,
  hasFocus = true, observeInitialization = true} = {}) {
  const trace = [], replies = new Map(), emitted = [], jobs = [], gates = new Map();
  const window = eventTarget(), document = eventTarget(), body = eventTarget();
  const moduleGate = deferred(), dataGate = deferred(), connectGate = deferred(), syncGates = [];
  const frame = {isConnected: true, contentWindow: window, contentDocument: document};
  window.document = document;window.frameElement = frame;document.defaultView = window;
  let clock = 0, messageSerial = 0, jobSerial = 0, bridge, moduleCallbacks, transport;
  let initializedDone = false, initializationError, nextSyncGate = null, initializationObserved = false;
  document.hidden = false;document.hasFocus = () => hasFocus;
  dataGate.started = false;connectGate.started = false;
  const life = () => ({hidden: document.hidden, hasFocus: document.hasFocus(),
    frameConnected: frame.isConnected, documentOwned: frame.contentDocument === document,
    ...(bridge ? copy(bridge.state()) : {launched: false, stopping: false})});
  const note = (kind, fields = {}) => {
    assert.ok(trace.length < TRACE_LIMIT, 'bounded fixture trace overflow');
    const row = {seq: trace.length, fixtureAtMs: clock, kind, ...fields};trace.push(row);return row;
  };
  const native = (op, args) => note('native', {op, args: [...args], life: life()});
  const canvas = {...eventTarget(), getBoundingClientRect: () => ({left: 80, top: 60, width: 640, height: 480}),
    focus() {}, setPointerCapture() {}};
  body.setPointerCapture = () => {};
  document.body = body;
  const elements = new Map();
  document.querySelector = selector => {
    if (selector === 'canvas') return canvas;
    if (selector === 'meta[name="eagler-runtime-variant"]') return {content: 'multiplayer'};
    if (!elements.has(selector)) elements.set(selector, {textContent: ''});
    return elements.get(selector);
  };
  const files = new Set();
  const FS = {
    mkdirTree() {}, mount() {}, symlink(_target, path) {files.add(path);},
    syncfs(populate, done) {
      const gate = nextSyncGate;nextSyncGate = null;
      if (!gate) {done();return;}
      gate.started = true;note('sync-wait', {populate});
      void gate.promise.then(() => {note('sync-released', {populate});done();});
    },
    writeFile(path) {files.add(path);note('fs-write', {path});},
    analyzePath(path) {return {exists: files.has(path)};},
    unlink(path) {files.delete(path);},
  };
  const memory = new ArrayBuffer(4096);
  const core = {FS, IDBFS: {}, HEAPU8: new Uint8Array(memory), HEAP32: new Int32Array(memory), SDL3: {},
    _th09_title_status: () => 0, _th09_session_status: () => 32, _th09_touch_state: () => 64,
    _th09_error: () => 512, _th09_storage_revision: () => 0,
    _th09_prepare_loading: () => 1, _th09_save_snapshot: () => 1,
    _th09_game_open: () => {native('game-open', []);return 1;},
    _th09_game_close: () => native('game-close', []),
  };
  core.HEAP32[7] = 1; // Keep the unrelated title-exit branch inactive.
  core.HEAP32.set([1, 1, 0, 0], 16);
  for (const [name, op] of [
    ['touch', 'touch'], ['touch_cancel', 'touch-cancel'], ['touch_controls', 'touch-controls'],
    ['touch_stick', 'touch-stick'], ['touch_options', 'touch-options'], ['key', 'key'],
    ['keys_clear', 'keys-clear'], ['music_enabled', 'music'], ['loop_start', 'loop-start'],
    ['loop_pause', 'loop-pause'], ['loop_stop', 'loop-stop'],
  ]) core['_th09_' + name] = (...args) => native(op, args);

  const parent = {
    postMessage(message, origin) {
      assert.equal(origin, ORIGIN);
      const saved = copy(message);emitted.push(saved);
      note('response', {request: saved.request ?? null, event: saved.event ?? null, ok: saved.ok ?? null});
      if (typeof saved.request === 'string') {
        assert.ok(!replies.has(saved.request), 'duplicate command response: ' + saved.request);
        replies.set(saved.request, saved);
      }
    },
    async __eaglerPrepareManagedRuntimeDataV1(request) {
      assert.equal(request.game, 'th09');assert.equal(request.epoch, epoch);
      assert.equal(request.generation, 'fixture-generation');
      dataGate.started = true;await dataGate.promise;
      return {buffer: new ArrayBuffer(1000000)};
    },
  };
  function schedule(callback, delay = 0, kind = 'timer') {
    assert.ok(jobs.length < 16, 'fixture scheduling queue overflow');
    const id = ++jobSerial;
    jobs.push({id, callback, at: clock + Math.max(0, Number(delay) || 0), kind});return id;
  }
  const response = () => ({ok: true, arrayBuffer: async () => new ArrayBuffer(1),
    json: async () => ({builtAt: 'fixture-only'})});
  const fetch = async input => {
    const url = new URL(String(input), ORIGIN + '/runtime/th09/multiplayer/th09.html').href;
    const gate = gates.get(url);
    if (gate) {
      gate.started = true;note('resource-wait', {url});
      await gate.promise;note('resource-released', {url});
      if (gate.fail) return {...response(), ok: false};
      if (gate.body) return {...response(), arrayBuffer: async () => {
        gate.body.started = true;await gate.body.promise;return new ArrayBuffer(1);
      }};
    }
    return response();
  };
  const context = {
    console, URL, URLSearchParams, Uint8Array, Int32Array, ArrayBuffer, TextDecoder, Promise, queueMicrotask,
    window, document, parent, innerWidth: 800, innerHeight: 600,
    location: {origin: ORIGIN, href: ORIGIN + '/runtime/th09/multiplayer/th09.html',
      search: '?managedData=1&runtimeEpoch=' + epoch + '&gameGeneration=fixture-generation'},
    performance: {now: () => clock, mark: name => note('mark', {name})},
    Date: {now: () => 1700000000000 + clock},
    requestAnimationFrame: callback => schedule(callback, 1000 / 60, 'raf'),
    setTimeout: (callback, delay) => schedule(callback, delay),
    clearTimeout: id => {const index = jobs.findIndex(job => job.id === id);if (index >= 0) jobs.splice(index, 1);},
    createBrowserKeyboard, exportReplayName, importReplayName, createAutoReplay, scanCodes: {KeyZ: 44},
    createModule: async callbacks => {moduleCallbacks = callbacks;await moduleGate.promise;return core;},
    installStartupBranding: async () => {},
    SharedNetplay: class {
      constructor(receivedCore, callbacks) {
        assert.equal(receivedCore, core);this.callbacks = callbacks;this.active = false;
        this.finished = false;this.disconnected = false;this.recovering = false;this.spectator = false;transport = this;
      }
      async connect(options) {
        this.spectator = !!options.netplaySpectator;connectGate.started = true;
        note('connect-wait');await connectGate.promise;note('connect-released');
        this.active = !this.disconnected;
      }
      close() {this.active = false;this.disconnected = true;note('transport-close');}
      frame() {}
    },
    fetch,
  };
  bridge = vm.runInNewContext('(function(){\n' + runtimeBody +
    '\nreturn {initialized, idle:()=>queue, state:()=>({launched,stopping,validEpoch})};\n})()', context,
    {filename: 'managed-touch-queue.vm.js', timeout: 1000});
  assert.equal(window.count('message'), 1, 'the real managed message listener must be present');
  assert.equal(window.count('blur'), 1, 'the real managed blur listener must be present');
  assert.equal(document.count('visibilitychange'), 1, 'the real managed visibility listener must be present');
  function observeInitializationOutcome() {
    if (initializationObserved) return;
    initializationObserved = true;
    bridge.initialized.then(() => {initializedDone = true;}, error => {initializationError = error;initializedDone = true;});
  }
  if (observeInitialization) observeInitializationOutcome();
  if (!holdModule) moduleGate.release();
  if (!holdData) dataGate.release();
  if (!holdConnect) connectGate.release();

  async function pumpUntil(predicate, {runTasks = false, label = 'condition'} = {}) {
    for (let turn = 0; turn < PUMP_LIMIT; turn++) {
      await Promise.resolve();
      if (predicate()) return;
      if (runTasks && jobs.length) {
        jobs.sort((a, b) => a.at - b.at || a.id - b.id);
        const job = jobs.shift();clock = Math.max(clock, job.at);job.callback(clock);
      }
    }
    throw Error('bounded fixture pump exhausted: ' + label);
  }
  async function idle({runTasks = false} = {}) {
    let done = false, failure;
    bridge.idle().then(() => {done = true;}, error => {failure = error;done = true;});
    await pumpUntil(() => done, {runTasks, label: 'managed queue settlement'});
    if (failure) throw failure;
  }
  function send(command, payload = {}, override = {}) {
    assert.ok(messageSerial < MESSAGE_LIMIT, 'bounded fixture message budget exceeded');
    const request = 'fixture-' + (++messageSerial);
    const data = {protocol: PROTOCOL, game: 'th09', epoch, command, ...payload, request, ...override.data};
    note('message', {request: data.request ?? null, command: data.command, claimedEpoch: data.epoch});
    window.dispatch('message', {source: Object.hasOwn(override, 'source') ? override.source : parent,
      origin: override.origin ?? ORIGIN, data});
    return request;
  }
  async function ready() {
    observeInitializationOutcome();
    await pumpUntil(() => initializedDone, {label: 'managed initialization'});
    if (initializationError) throw initializationError;
    assert.ok(emitted.some(message => message.event === 'ready'));
  }
  function ok(request) {assert.equal(replies.get(request)?.ok, true, 'successful response: ' + request);}
  function noFatal() {assert.deepEqual(emitted.filter(message => message.event === 'error'), []);}
  async function beginLaunch(movement = 'touch', options = {}) {
    await ready();
    const configured = send('configure', {options: {touchEnabled: true, touchMovementMode: movement, touchSensitivity: 150, ...options}, music: 'none'});
    await idle();ok(configured);return send('launch');
  }
  async function launch(movement = 'touch', options = {}) {
    const request = await beginLaunch(movement, options);
    await pumpUntil(() => replies.has(request), {runTasks: true, label: 'real launch command'});
    ok(request);assert.equal(bridge.state().launched, true);noFatal();return request;
  }
  function resourceGate({holdBody = false} = {}) {
    const serial = gates.size + 1, url = ORIGIN + '/controlled-resource-' + serial + '.bin', gate = deferred();
    if (holdBody) {gate.body = deferred();gate.body.started = false;}
    gate.started = false;gates.set(url, gate);
    return {gate, resource: {path: '/fonts/queue-fixture-' + serial + '.bin', url}};
  }
  function blockResource() {
    const {gate, resource} = resourceGate();
    return {gate, request: send('resources', {resources: [resource]})};
  }
  return {trace, replies, emitted, send, ready, beginLaunch, launch, idle, pumpUntil, ok, noFatal, blockResource, resourceGate,
    state: life, releaseModule: () => moduleGate.release(), dataGate, connectGate,
    isInitializationPromise: promise => promise === bridge.initialized,
    async initializationOutcome() {
      observeInitializationOutcome();
      await pumpUntil(() => initializedDone, {label: 'initialization completion or cancellation'});
      return {completed: initializedDone, error: initializationError ? String(initializationError) : null,
        readyEmitted: emitted.some(row => row.event === 'ready')};
    },
    async checkpoint() {for (let turn = 0; turn < 16; turn++) await Promise.resolve();},
    blockNextSync() {
      assert.equal(nextSyncGate, null);const gate = deferred();gate.started = false;
      nextSyncGate = gate;syncGates.push(gate);return gate;
    },
    remoteClose(reason = '') {
      assert.ok(transport);note('transport-onClose', {reason});
      transport.active = false;transport.disconnected = true;transport.callbacks.onClose(reason);
    },
    nativeExitFrame() {
      assert.equal(typeof moduleCallbacks?.onGameFrame, 'function');
      core.HEAP32[7] = 0;moduleCallbacks.onGameFrame(1, 0);
    },
    replaceDocument() {frame.contentDocument = {...eventTarget(), defaultView: window};note('document-replaced');},
    detachFrame() {frame.isConnected = false;note('frame-detached');},
    pointer(name, {id = 42, x = 400, y = 300, pointerType = 'touch', ui = false} = {}) {
      body.dispatch(name, {pointerId: id, clientX: x, clientY: y, pointerType,
        target: {closest: () => ui}, preventDefault() {}});
    },
    pending: requests => requests.filter(request => !replies.has(request)),
    blur() {note('lifecycle', {event: 'blur'});window.dispatch('blur');},
    hide() {document.hidden = true;note('lifecycle', {event: 'visibilitychange', hidden: true});document.dispatch('visibilitychange');},
    show() {document.hidden = false;note('lifecycle', {event: 'visibilitychange', hidden: false});document.dispatch('visibilitychange');},
    pagehide() {note('lifecycle', {event: 'pagehide'});window.dispatch('pagehide');},
    async close() {
      moduleGate.release();dataGate.release();connectGate.release();
      for (const gate of [...gates.values(), ...syncGates]) {gate.release();gate.body?.release();}
      // No real timers, device, network or browser exist. Settle pending VM work
      // under the same finite budget; never turn cleanup into an unbounded wait.
      for (let turn = 0; turn < PUMP_LIMIT; turn++) {
        await Promise.resolve();
        if (jobs.length) {const job = jobs.shift();clock = Math.max(clock, job.at);job.callback(clock);}
      }
      jobs.length = 0;
    },
  };
}

const rows = (runtime, start = 0) => runtime.trace.slice(start).filter(row => row.kind === 'native');
const inputs = (runtime, start = 0) => rows(runtime, start).filter(row => touchOps.has(row.op));
const shape = list => list.map(row => ({op: row.op, args: row.args}));
function controlPairs(runtime, start = 0, end = runtime.trace.length) {
  const list = inputs(runtime, start).filter(row => row.seq < end), result = [];
  for (let i = 0; i < list.length - 1; i++) if (list[i].op === 'touch-controls' && list[i + 1].op === 'touch-stick')
    result.push({controls: list[i], stick: list[++i]});
  return result;
}
function standaloneControls(runtime, start = 0, end = runtime.trace.length) {
  const paired = new Set(controlPairs(runtime, start, end).map(pair => pair.controls.seq));
  return inputs(runtime, start).filter(row => row.seq < end && row.op === 'touch-controls' && !paired.has(row.seq));
}
function commandInputs(runtime, start = 0, end = runtime.trace.length) {
  const neutral = new Set(standaloneControls(runtime, start, end).map(row => row.seq));
  return inputs(runtime, start).filter(row => row.seq < end && !neutral.has(row.seq));
}
const directCalls = (runtime, start = 0, end = runtime.trace.length) =>
  inputs(runtime, start).filter(row => row.seq < end && row.op === 'touch');
const down = {type: 'down', id: -1000000, x: .5, y: .5};
const move = {type: 'move', id: -1000000, x: .7, y: .6};
const controls = (extra = {}) => ({fireEnabled: true, focusEnabled: false, bombSerial: 0, escapeSerial: 0,
  joystickX: 0, joystickY: 0, touchSensitivity: 150, ...extra});

function report(t, name, runtime, start, observed, goals) {
  const nativeCalls = rows(runtime, start);
  assert.ok(nativeCalls.length <= 64, 'bounded evidence native trace');
  t.diagnostic(JSON.stringify({case: name, mode, requireGoals,
    scope: {boundary: 'real managed listener to stub native calls and parent ACK',
      gameProgressMeasured: false, realLatencyMeasured: false, realLauncherOrBrowser: false},
    clock: {name: 'fixture scheduling clock', timeOrigin: null, synthetic: true},
    sourcePaths, sourceIdentity, observed, goals, nativeCalls}));
  if (requireGoals) for (const [name, passed] of Object.entries(goals)) assert.equal(passed, true, 'candidate goal: ' + name);
}

test('real message listener rejects foreign envelopes and invalid Runtime epoch', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
  const start = runtime.trace.length;
  const rejected = [
    {source: {}}, {origin: 'https://foreign.invalid'}, {data: {protocol: 'wrong/1'}},
    {data: {game: 'th07'}}, {data: {epoch: 72}}, {data: {epoch: '73'}}, {data: {command: null}},
  ].map(override => runtime.send('direct-touch', down, override));
  await runtime.idle();
  assert.deepEqual(inputs(runtime, start), []);
  assert.deepEqual(runtime.pending(rejected), rejected);
  const accepted = runtime.send('direct-touch', down);await runtime.idle();runtime.ok(accepted);
  assert.deepEqual(shape(inputs(runtime, start)), [{op: 'touch', args: [0, -1000000, .5, .5]}]);
  const invalid = openRuntime({epoch: 0});t.after(() => invalid.close());await invalid.ready();
  const invalidStart = invalid.trace.length, invalidRequest = invalid.send('direct-touch', down);
  await invalid.idle();
  assert.equal(invalid.state().validEpoch, false);
  assert.deepEqual(inputs(invalid, invalidStart), []);
  assert.deepEqual(invalid.pending([invalidRequest]), [invalidRequest]);
  runtime.noFatal();invalid.noFatal();
  report(t, 'envelope-gates', runtime, start, {rejectedEnvelopeCount: rejected.length,
    invalidRuntimeEpochIgnored: true, acceptedNativeCalls: 1}, {envelopeGatesPreserved: true});
});

test('ready and launched input delivery is characterized while an unrelated resource awaits', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
  const start = runtime.trace.length, blocker = runtime.blockResource();
  await runtime.pumpUntil(() => blocker.gate.started, {label: 'controlled resource fetch entry'});
  const requests = [runtime.send('direct-touch', down), runtime.send('direct-touch', move),
    runtime.send('touch-controls', controls({focusEnabled: true})), runtime.send('touch-cancel'),
    runtime.send('touch-controls', controls())]; // The real host cancel also sends a neutral controls snapshot.
  const key = runtime.send('keyboard', {code: 'KeyZ', key: 'z', keyCode: 90, down: true});
  const cleared = runtime.send('keyboard-clear');
  await runtime.pumpUntil(() => runtime.replies.has(key) && runtime.replies.has(cleared), {label: 'keyboard bypass witness'});
  runtime.ok(key);runtime.ok(cleared);
  const before = {nativeInputCalls: inputs(runtime, start).length, commandInputCalls: commandInputs(runtime, start).length,
    unacknowledgedInputRequests: runtime.pending(requests).length,
    resourceAcknowledged: runtime.replies.has(blocker.request),
    keyboardNativeCalls: rows(runtime, start).filter(row => row.op === 'key' || row.op === 'keys-clear').length};
  assert.equal(before.resourceAcknowledged, false);assert.ok(before.keyboardNativeCalls >= 2);
  blocker.gate.release();await runtime.idle();runtime.ok(blocker.request);
  for (const request of requests) runtime.ok(request);
  assert.deepEqual(shape(commandInputs(runtime, start)), [
    {op: 'touch', args: [0, -1000000, .5, .5]}, {op: 'touch', args: [1, -1000000, .75, .625]},
    {op: 'touch-controls', args: [1, 1, 1, 0, 0]}, {op: 'touch-stick', args: [0, 0]},
    {op: 'touch-cancel', args: []}, {op: 'touch-controls', args: [1, 1, 0, 0, 0]}, {op: 'touch-stick', args: [0, 0]},
  ]);
  const neutral = standaloneControls(runtime, start);
  assert.ok(neutral.length <= 1);
  for (const row of neutral) assert.deepEqual(row.args, [1, 1, 0, 0, 0], 'active cancel preserves fire and current mapped serials');
  runtime.noFatal();
  report(t, 'active-resource-backlog', runtime, start, {workflow: 'ready-and-launched', inputMessages: requests.length, beforeRelease: before,
    classification: before.commandInputCalls === 0 ? 'native-input-wait-reproduced'
      : before.commandInputCalls === 7 ? 'all-command-input-calls-before-release' : 'partial-input-wait',
    standaloneNeutralControls: standaloneControls(runtime, start),
    afterRelease: {nativeInputCalls: inputs(runtime, start).length, unacknowledgedInputRequests: runtime.pending(requests).length}},
  {activeInputDoesNotAwaitUnrelatedResource: before.commandInputCalls === 7,
    activeInputAcknowledgedBeforeResourceRelease: before.unacknowledgedInputRequests === 0});
});

test('queued direct input is classified against the real blur cancellation boundary', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
  const start = runtime.trace.length, blocker = runtime.blockResource();
  await runtime.pumpUntil(() => blocker.gate.started, {label: 'controlled resource fetch entry'});
  const requests = [runtime.send('direct-touch', down), runtime.send('direct-touch', move)];
  // This invokes the source's real lifecycle listener, not a synthetic native cancel.
  runtime.blur();
  const barrier = runtime.trace.findLast(row => row.kind === 'native' && row.op === 'touch-cancel').seq;
  assert.ok(barrier >= start);
  const before = {unacknowledgedInputRequests: runtime.pending(requests).length, cancelNativeSeq: barrier};
  blocker.gate.release();await runtime.idle();runtime.ok(blocker.request);
  const delivered = inputs(runtime, start).filter(row => row.op === 'touch');
  for (const request of requests) runtime.ok(request);
  assert.ok(delivered.length <= 2);
  for (const row of delivered) assert.deepEqual(row.args, row.args[0] === 0
    ? [0, -1000000, .5, .5] : [1, -1000000, .75, .625]);
  assert.equal(new Set(delivered.map(row => row.args[0])).size, delivered.length, 'no duplicate native delivery');
  assert.ok(delivered.every((row, index) => index === 0 || delivered[index - 1].args[0] < row.args[0]),
    'any retained direct inputs preserve DOWN before MOVE');
  const afterBarrier = delivered.filter(row => row.seq > barrier);
  runtime.noFatal();
  report(t, 'blur-barrier', runtime, start, {workflow: 'ready-and-launched-before-blur', beforeRelease: before,
    preBlurInputCallsAfterCancel: afterBarrier.length, unacknowledgedAfterRelease: runtime.pending(requests).length,
    classification: afterBarrier.length ? 'old-native-input-calls-after-cancel' : 'no-old-native-input-after-cancel',
    actualGestureRearmMeasured: false}, {noPreBarrierDirectInputCallsAfterBlur: afterBarrier.length === 0});
});

test('queued joystick controls are classified against the real hidden-document boundary', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch('joystick-free');
  const start = runtime.trace.length, blocker = runtime.blockResource();
  await runtime.pumpUntil(() => blocker.gate.started, {label: 'controlled resource fetch entry'});
  const request = runtime.send('touch-controls', controls({joystickX: 12000, joystickY: -9000}));
  runtime.hide();
  const barrier = runtime.trace.findLast(row => row.kind === 'native' && row.op === 'touch-cancel').seq;
  assert.ok(barrier >= start);
  const hiddenKey = runtime.send('keyboard', {code: 'KeyZ', key: 'z', keyCode: 90, down: true});
  await runtime.pumpUntil(() => runtime.replies.has(hiddenKey), {label: 'hidden keyboard response'});
  runtime.ok(hiddenKey);
  assert.equal(rows(runtime, barrier + 1).filter(row => row.op === 'key').length, 0, 'existing keyboard lifecycle gate');
  const before = {unacknowledgedInputRequests: runtime.pending([request]).length, hidden: runtime.state().hidden};
  blocker.gate.release();await runtime.idle();runtime.ok(blocker.request);
  const delivered = controlPairs(runtime, start).flatMap(pair => [pair.controls, pair.stick]);
  runtime.ok(request);
  assert.ok(delivered.length === 0 || delivered.length === 2);
  if (delivered.length) assert.deepEqual(shape(delivered), [
    {op: 'touch-controls', args: [1, 1, 0, 0, 0]}, {op: 'touch-stick', args: [12000, -9000]},
  ]);
  const hiddenCalls = delivered.filter(row => row.seq > barrier && row.life.hidden);
  runtime.noFatal();
  report(t, 'hidden-joystick-barrier', runtime, start, {workflow: 'ready-and-launched-before-hidden', beforeRelease: before,
    oldControlNativeCallsWhileHidden: hiddenCalls.length, unacknowledgedAfterRelease: runtime.pending([request]).length,
    standaloneNeutralControls: standaloneControls(runtime, start),
    actualMovementOrCaptureMeasured: false}, {noPreBarrierJoystickCommandCallsWhileHidden: hiddenCalls.length === 0});
});

test('adversarial same-epoch pre-ready input records initialization and launch eligibility separately', async t => {
  const runtime = openRuntime({holdModule: true});t.after(() => runtime.close());
  const start = runtime.trace.length;
  const requests = [runtime.send('direct-touch', down), runtime.send('touch-controls', controls({joystickX: 1000}))];
  for (let turn = 0; turn < 8; turn++) await Promise.resolve();
  assert.deepEqual(inputs(runtime, start), [], 'no native access while createModule is unresolved');
  const before = {nativeInputCalls: 0, unacknowledgedInputRequests: runtime.pending(requests).length};
  runtime.releaseModule();await runtime.ready();await runtime.idle();
  for (const request of requests) runtime.ok(request);
  assert.equal(runtime.state().launched, false);
  const preLaunchCalls = inputs(runtime, start).filter(row => !row.life.launched);
  runtime.noFatal();
  report(t, 'adversarial-pre-ready', runtime, start, {workflow: 'adversarial-valid-envelope',
    normalLauncherWouldSendBeforeReady: false, beforeModuleRelease: before,
    nativeInputCallsBeforeLaunch: preLaunchCalls.length, unacknowledgedAfterInitialization: runtime.pending(requests).length,
    actualGameplayMeasured: false}, {noPreReadyInputDeliveredAfterInitializationWithoutLaunch: preLaunchCalls.length === 0});
});


test('core creation, full initialization and launch eligibility are separate boundaries', async t => {
  const runtime = openRuntime({holdData: true});t.after(() => runtime.close());
  await runtime.pumpUntil(() => runtime.dataGate.started, {label: 'real DATA acquisition after core creation'});
  assert.equal(runtime.emitted.some(row => row.event === 'ready'), false);
  const start = runtime.trace.length;
  const early = [runtime.send('direct-touch', down), runtime.send('touch-controls', controls({joystickX: 1000, bombSerial: 3}))];
  await runtime.checkpoint();
  const duringData = {nativeCalls: inputs(runtime, start).length, pending: runtime.pending(early).length};
  runtime.dataGate.release();await runtime.ready();await runtime.idle();
  for (const request of early) runtime.ok(request);
  const initializedEnd = runtime.trace.length, beforeLaunchStart = runtime.trace.length;
  const beforeLaunch = [runtime.send('direct-touch', down), runtime.send('touch-controls', controls({joystickY: 2000, bombSerial: 4}))];
  await runtime.idle();for (const request of beforeLaunch) runtime.ok(request);
  assert.equal(runtime.state().launched, false);runtime.noFatal();
  report(t, 'initialization-versus-launch', runtime, start, {
    duringData, afterInitialization: inputs(runtime, start).filter(row => row.seq < initializedEnd).length,
    afterReadyBeforeLaunch: inputs(runtime, beforeLaunchStart).length, actualNativeReadinessMeasured: false,
  }, {
    noTouchNativeBeforeFullInitialization: duringData.nativeCalls === 0,
    earlyInputAcknowledgedWithoutWaitingForData: duringData.pending === 0,
    noEarlyInputReplayAfterInitialization: inputs(runtime, start).filter(row => row.seq < initializedEnd).length === 0,
    initializedButUnlaunchedRejectsActivity: inputs(runtime, beforeLaunchStart).length === 0,
  });
});

test('launched before connect settles does not qualify active input', async t => {
  const runtime = openRuntime({holdConnect: true});t.after(() => runtime.close());
  const launch = await runtime.beginLaunch('touch', {netplayMode: 'lan'});
  await runtime.pumpUntil(() => runtime.connectGate.started, {runTasks: true, label: 'real launch awaits connect'});
  assert.equal(runtime.state().launched, true);assert.equal(runtime.replies.has(launch), false);
  const start = runtime.trace.length;
  const early = [runtime.send('direct-touch', down), runtime.send('touch-controls', controls({joystickX: 12000}))];
  await runtime.checkpoint();
  const before = {nativeCalls: inputs(runtime, start).length, pending: runtime.pending(early).length};
  runtime.connectGate.release();await runtime.idle();runtime.ok(launch);
  for (const request of early) runtime.ok(request);
  const completed = runtime.trace.length, fresh = runtime.send('direct-touch', {...down, id: -1000001});
  await runtime.idle();runtime.ok(fresh);
  assert.deepEqual(shape(directCalls(runtime, completed)), [{op: 'touch', args: [0, -1000001, .5, .5]}]);
  runtime.noFatal();
  report(t, 'launch-connect-boundary', runtime, start, {
    launchedBeforeConnect: true, launchAcknowledgedWhileConnectPending: false, beforeConnect: before,
    oldInputNativeCallsByConnectCompletion: inputs(runtime, start).filter(row => row.seq < completed).length,
    freshInputAfterLaunchAck: 1, realTransportMeasured: false,
  }, {
    noActivityWhileConnectPending: before.nativeCalls === 0,
    inputAcknowledgedWhileConnectPending: before.pending === 0,
    noOldInputReplayWhenConnectSettles: inputs(runtime, start).filter(row => row.seq < completed).length === 0,
  });
});

test('real onClose stop revokes receipt-time input and queued launch/configure while sync drains', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch('touch', {netplayMode: 'lan'});
  const resource = runtime.blockResource();
  await runtime.pumpUntil(() => resource.gate.started, {label: 'resource command before real stop'});
  const start = runtime.trace.length, sync = runtime.blockNextSync();
  runtime.remoteClose();
  await runtime.pumpUntil(() => sync.started, {label: 'real stop waiting for FS.syncfs'});
  assert.equal(runtime.state().stopping, true);
  assert.ok(rows(runtime, start).some(row => row.op === 'loop-stop'));
  const inputStart = runtime.trace.length;
  const input = [runtime.send('direct-touch', down), runtime.send('touch-controls', controls({joystickY: 12000}))];
  const staleLaunch = runtime.send('launch');
  const staleConfigure = runtime.send('configure', {options: {touchEnabled: true, touchMovementMode: 'touch', touchSensitivity: 150}});
  await runtime.checkpoint();
  const pendingWhileStopping = runtime.pending(input).length;
  sync.release();
  await runtime.pumpUntil(() => !runtime.state().stopping && !runtime.state().launched, {label: 'real stop completion'});
  const stopped = runtime.trace.length;
  resource.gate.release();await runtime.idle({runTasks: true});
  assert.equal(runtime.replies.get(resource.request)?.ok, false, 'stop cancels the older resource request');
  assert.deepEqual(runtime.trace.slice(stopped).filter(row => row.kind === 'fs-write'), []);
  for (const request of input) runtime.ok(request);
  assert.ok(runtime.replies.has(staleLaunch));assert.ok(runtime.replies.has(staleConfigure));
  runtime.noFatal();
  report(t, 'onClose-stop-and-old-receipts', runtime, start, {
    stopEntry: 'SharedNetplay onClose -> source queueMicrotask -> source stop -> FS.syncfs',
    inputPendingWhileStopping: pendingWhileStopping, stoppedBeforeResourceRelease: true,
    inputNativeCalls: commandInputs(runtime, inputStart).filter(row => row.op !== 'touch-cancel').length,
    staleLaunchAck: runtime.replies.get(staleLaunch), staleConfigureAck: runtime.replies.get(staleConfigure),
    gameOpenAfterStop: rows(runtime, stopped).filter(row => row.op === 'game-open').length,
    finalState: runtime.state(), fabricatedStopCommand: false,
  }, {
    inputAcknowledgedWhileStopSyncPending: pendingWhileStopping === 0,
    stoppedReceiptsNeverReachNative: directCalls(runtime, inputStart).length === 0 && controlPairs(runtime, inputStart).length === 0,
    staleLaunchRejected: runtime.replies.get(staleLaunch).ok === false,
    staleConfigureRejected: runtime.replies.get(staleConfigure).ok === false,
    queuedReceiptsCannotReopenStoppedRuntime: !runtime.state().launched && !rows(runtime, stopped).some(row => row.op === 'game-open'),
  });
});

test('real onGameFrame native-exit status revokes touch during stop persistence', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
  const start = runtime.trace.length, sync = runtime.blockNextSync();
  runtime.nativeExitFrame();
  await runtime.pumpUntil(() => sync.started, {label: 'real onGameFrame stop awaits persistence'});
  assert.equal(runtime.state().stopping, true);
  const inputStart = runtime.trace.length, request = runtime.send('direct-touch', down);
  await runtime.checkpoint();const acknowledgedWhileStopping = runtime.replies.get(request)?.ok === true;
  sync.release();
  await runtime.pumpUntil(() => !runtime.state().stopping && !runtime.state().launched, {label: 'native-exit stop completes'});
  await runtime.idle();runtime.ok(request);runtime.noFatal();
  report(t, 'onGameFrame-stop', runtime, start, {
    stopEntry: 'controlled title status -> actual createModule onGameFrame callback -> source stop -> FS.syncfs',
    nativeGameExitMeasured: false, acknowledgedWhileStopping, inputCalls: directCalls(runtime, inputStart).length,
  }, {
    inputAcknowledgedDuringNativeExitStop: acknowledgedWhileStopping,
    nativeExitDoesNotPermitLateInput: directCalls(runtime, inputStart).length === 0,
  });
});

test('a connect continuation returning after real stop cannot rearm the old launch', async t => {
  const runtime = openRuntime({holdConnect: true});t.after(() => runtime.close());
  const launch = await runtime.beginLaunch('touch', {netplayMode: 'lan'});
  await runtime.pumpUntil(() => runtime.connectGate.started, {runTasks: true, label: 'connect pending before remote close'});
  const start = runtime.trace.length, sync = runtime.blockNextSync();
  runtime.remoteClose();
  await runtime.pumpUntil(() => sync.started, {label: 'stop sync during pending connect'});
  sync.release();
  await runtime.pumpUntil(() => !runtime.state().stopping && !runtime.state().launched, {label: 'stop before connect return'});
  runtime.connectGate.release();await runtime.idle();
  assert.ok(runtime.replies.has(launch));
  const inputStart = runtime.trace.length;
  const request = runtime.send('direct-touch', down);await runtime.idle();runtime.ok(request);
  runtime.noFatal();
  report(t, 'late-connect-after-stop', runtime, start, {
    launchAck: runtime.replies.get(launch), finalState: runtime.state(),
    activityAfterOldConnectReturn: directCalls(runtime, inputStart).length, realNetworkTimingMeasured: false,
  }, {
    supersededLaunchDoesNotAcknowledgeSuccess: runtime.replies.get(launch).ok === false,
    supersededLaunchDoesNotRearmInput: !runtime.state().launched && directCalls(runtime, inputStart).length === 0,
  });
});

for (const invalidation of ['replaceDocument', 'detachFrame', 'pagehide']) {
  test('old listener cannot deliver activity after ' + invalidation, async t => {
    const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
    const contact = runtime.send('direct-touch', down);await runtime.idle();runtime.ok(contact);
    const start = runtime.trace.length;runtime[invalidation]();
    const requests = [runtime.send('direct-touch', move), runtime.send('touch-controls', controls({joystickX: 9000, bombSerial: 1}))];
    await runtime.idle();for (const request of requests) runtime.ok(request);
    const second = runtime.send('direct-touch', move);await runtime.idle();runtime.ok(second);
    runtime.noFatal();
    const cleanup = inputs(runtime, start).filter(row => row.op === 'touch-cancel');
    report(t, 'owner-boundary-' + invalidation, runtime, start, {
      model: 'same fixture window object and epoch; retained real listener; document identity/lifecycle signal changes',
      actualWindowProxyNavigationMeasured: false, finalState: runtime.state(),
      cleanupCalls: cleanup.length, directCalls: directCalls(runtime, start).length,
      controlCommandPairs: controlPairs(runtime, start).length, neutralControls: standaloneControls(runtime, start),
    }, {
      invalidOwnerNeverDeliversActivity: directCalls(runtime, start).length === 0 && controlPairs(runtime, start).length === 0,
      invalidOwnerCleanupIsBounded: cleanup.length <= 1,
    });
  });
}


test('hidden serial baselines do not become new native action serials on resume', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch('joystick-free');
  const start = runtime.trace.length;
  async function snapshot(payload) {
    const begin = runtime.trace.length, request = runtime.send('touch-controls', controls(payload));
    await runtime.idle();runtime.ok(request);
    return {begin, end: runtime.trace.length, pairs: controlPairs(runtime, begin)};
  }
  const first = await snapshot({bombSerial: 7, escapeSerial: 11, focusEnabled: true, joystickX: 500, joystickY: -600});
  assert.equal(first.pairs.length, 1);assert.deepEqual(first.pairs[0].stick.args, [500, -600]);
  const hiddenStart = runtime.trace.length;runtime.hide();
  const hidden = await snapshot({bombSerial: 8, escapeSerial: 12, fireEnabled: false, focusEnabled: true, joystickX: 700, joystickY: 800});
  runtime.show();
  const repeated = await snapshot({bombSerial: 8, escapeSerial: 12});
  const fresh = await snapshot({bombSerial: 9, escapeSerial: 13, focusEnabled: true, joystickX: 900, joystickY: -1000});
  assert.equal(repeated.pairs.length, 1);assert.equal(fresh.pairs.length, 1);
  assert.deepEqual(repeated.pairs[0].stick.args, [0, 0]);assert.deepEqual(fresh.pairs[0].stick.args, [900, -1000]);
  const serials = phase => phase.pairs.map(pair => pair.controls.args.slice(3, 5));
  const neutral = standaloneControls(runtime, hiddenStart, fresh.begin);
  runtime.noFatal();
  report(t, 'hidden-serial-resume', runtime, start, {
    requestedRawSerials: [[7, 11], [8, 12], [8, 12], [9, 13]],
    nativeSerials: {first: serials(first), hidden: serials(hidden), repeated: serials(repeated), fresh: serials(fresh)},
    neutralControls: neutral, mappingBoundaryOnly: true, actualBombEscapeSamplingMeasured: false,
  }, {
    hiddenActiveControlsNotDelivered: hidden.pairs.length === 0,
    firstAcceptedChangeMapsOnce: JSON.stringify(serials(first)) === '[[1,1]]',
    hiddenSerialNotReplayedOnResume: JSON.stringify(serials(repeated)) === '[[1,1]]',
    laterAcceptedChangeMapsOnce: JSON.stringify(serials(fresh)) === '[[2,2]]',
    neutralRetainsAcceptedFireAndMappedSerials: neutral.length > 0 &&
      neutral.every(row => row.args[1] === 1 && row.args[2] === 0 && row.args[3] === 1 && row.args[4] === 1),
  });
});

for (const origin of ['hosted', 'native-pointer']) {
  test(origin + ' per-MOVE order survives ordinary HUD controls, sensitivity and sync', async t => {
    const runtime = openRuntime({hasFocus: false});t.after(() => runtime.close());await runtime.launch();
    const start = runtime.trace.length, requests = [], id = origin === 'hosted' ? -1000000 : 42;
    function contact(type, x, y) {
      if (origin === 'hosted') requests.push(runtime.send('direct-touch', {type, id, x, y}));
      else runtime.pointer('pointer' + type, {id, x: x * 800, y: y * 600});
    }
    contact('down', .5, .5);contact('move', .6, .5);
    requests.push(runtime.send('touch-controls', controls({focusEnabled: true})));
    await runtime.idle();
    const synced = runtime.send('sync');await runtime.idle();runtime.ok(synced);
    contact('move', .7, .6);
    requests.push(runtime.send('touch-controls', controls({fireEnabled: false, touchSensitivity: 175})));
    contact('move', .8, .65);contact('up', .8, .65);
    await runtime.idle();for (const request of requests) runtime.ok(request);
    const touches = directCalls(runtime, start), pairs = controlPairs(runtime, start);
    assert.deepEqual(shape(touches), [
      {op: 'touch', args: [0, id, .5, .5]}, {op: 'touch', args: [1, id, .625, .5]},
      {op: 'touch', args: [1, id, .75, .625]}, {op: 'touch', args: [1, id, .875, .6875]},
      {op: 'touch', args: [2, id, .875, .6875]},
    ], 'every original MOVE reaches the native boundary once, in order');
    assert.equal(pairs.length, 2);
    assert.deepEqual(pairs.map(pair => pair.stick.args), [[0, 0], [0, 0]]);
    const cancelled = inputs(runtime, start).filter(row => row.op === 'touch-cancel');
    runtime.noFatal();
    report(t, 'ordinary-controls-preserve-' + origin, runtime, start, {
      hasFocus: false, documentOwned: runtime.state().documentOwned, directNativeCalls: touches.length,
      neutralControls: standaloneControls(runtime, start), cancellationCalls: cancelled.length,
      actualDraggingOrClampSimulationMeasured: false,
    }, {
      ordinaryControlsDoNotCancelMovement: cancelled.length === 0,
      ordinaryControlsPreserveFocusAndFire: pairs[0].controls.args[1] === 1 && pairs[0].controls.args[2] === 1 &&
        pairs[1].controls.args[1] === 0 && pairs[1].controls.args[2] === 0,
      syncIsNotPermanentStop: runtime.state().launched && !runtime.state().stopping,
    });
  });
}

test('touch-policy configure fences awaiting resources and stays fenced after failure until success', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
  const held = runtime.send('direct-touch', down);await runtime.idle();runtime.ok(held);
  const start = runtime.trace.length, firstGate = runtime.resourceGate();
  const configure = runtime.send('configure', {options: {
    touchEnabled: true, touchMovementMode: 'joystick-free', touchSensitivity: 150,
  }, resources: [firstGate.resource]});
  await runtime.pumpUntil(() => firstGate.gate.started, {label: 'touch-policy configure resource wait'});
  const blockedStart = runtime.trace.length;
  const blocked = [runtime.send('direct-touch', move), runtime.send('touch-controls', controls({joystickX: 6000, bombSerial: 1}))];
  await runtime.checkpoint();const pendingBeforeApply = runtime.pending(blocked).length;
  firstGate.gate.release();await runtime.idle();runtime.ok(configure);
  for (const request of blocked) runtime.ok(request);
  const afterApply = runtime.trace.length;
  const fresh = runtime.send('touch-controls', controls({joystickX: 7000, bombSerial: 2}));
  await runtime.idle();runtime.ok(fresh);
  const freshPairs = controlPairs(runtime, afterApply);assert.equal(freshPairs.length, 1);
  assert.deepEqual(freshPairs[0].stick.args, [7000, 0]);

  const failureStart = runtime.trace.length, failGate = runtime.resourceGate();failGate.gate.fail = true;
  const failedConfigure = runtime.send('configure', {options: {
    touchEnabled: true, touchMovementMode: 'touch', touchSensitivity: 150,
  }, resources: [failGate.resource]});
  await runtime.pumpUntil(() => failGate.gate.started, {label: 'failing touch-policy configure resource wait'});
  failGate.gate.release();await runtime.idle();
  assert.equal(runtime.replies.get(failedConfigure)?.ok, false, 'controlled resource failure reaches the real response path');
  const afterFailure = runtime.trace.length, ignored = runtime.send('direct-touch', down);
  await runtime.idle();runtime.ok(ignored);const beforeRecovery = runtime.trace.length;
  const recovered = runtime.send('configure', {options: {touchEnabled: true, touchMovementMode: 'touch', touchSensitivity: 150}});
  await runtime.idle();runtime.ok(recovered);
  const recoveredStart = runtime.trace.length, accepted = runtime.send('direct-touch', {...down, id: -1000002});
  await runtime.idle();runtime.ok(accepted);
  assert.deepEqual(shape(directCalls(runtime, recoveredStart)), [{op: 'touch', args: [0, -1000002, .5, .5]}]);
  const disabledAtStart = standaloneControls(runtime, start, blockedStart).some(row => row.args[0] === 0);
  runtime.noFatal();
  report(t, 'configure-policy-barrier', runtime, start, {
    pendingDuringConfigure: pendingBeforeApply, disabledNativeBeforeResourceWait: disabledAtStart,
    oldDirectCallsByApply: directCalls(runtime, blockedStart, afterApply).length,
    oldControlPairsByApply: controlPairs(runtime, blockedStart, afterApply).length,
    failureAck: runtime.replies.get(failedConfigure), ignoredAfterFailure: directCalls(runtime, afterFailure, beforeRecovery).length,
    restoredWithSamePolicyAfterFailure: true, failureCleanup: standaloneControls(runtime, failureStart, afterFailure),
  }, {
    policyBarrierDisablesNativeBeforeAwait: disabledAtStart,
    inputAcknowledgedDuringConfigure: pendingBeforeApply === 0,
    oldConfigurePeriodInputsNeverReplayed: directCalls(runtime, blockedStart, afterApply).length === 0 &&
      controlPairs(runtime, blockedStart, afterApply).length === 0,
    failedConfigureKeepsInputFenced: directCalls(runtime, afterFailure, beforeRecovery).length === 0,
  });
});


for (const phase of ['module', 'data']) {
  test('pre-init pagehide settles queued command ACKs at the ' + phase + ' boundary', async t => {
    const runtime = openRuntime({holdModule: phase === 'module', holdData: phase === 'data'});
    t.after(() => runtime.close());
    if (phase === 'data') await runtime.pumpUntil(() => runtime.dataGate.started, {label: 'DATA pending before pagehide'});
    const start = runtime.trace.length;
    const configured = runtime.send('configure', {options: {touchEnabled: true, touchMovementMode: 'touch', touchSensitivity: 150}});
    const launch = runtime.send('launch');
    runtime.send('configure', {options: {}}, {data: {request: null}}); // Superseded requestless work must stay quiet.
    runtime.pagehide();
    const touch = runtime.send('direct-touch', down);await runtime.checkpoint();
    const before = {nativeInputCalls: inputs(runtime, start).length, touchAck: runtime.replies.get(touch) ?? null};
    runtime.releaseModule();runtime.dataGate.release();
    const initialization = await runtime.initializationOutcome();
    await runtime.idle({runTasks: true});runtime.ok(touch);runtime.noFatal();
    report(t, 'pre-init-pagehide-' + phase, runtime, start, {
      beforeRelease: before, initialization, configureAck: runtime.replies.get(configured) ?? null,
      launchAck: runtime.replies.get(launch) ?? null, finalState: runtime.state(),
      nativeInputCalls: inputs(runtime, start).length, directStopProtocolAdded: false,
    }, {
      touchAcknowledgedBeforeInitializationSettles: before.touchAck?.ok === true,
      cancelledInitializationFailsConfigureAck: runtime.replies.get(configured)?.ok === false,
      cancelledInitializationFailsLaunchAck: runtime.replies.get(launch)?.ok === false,
      cancellationDoesNotReplayInputOrLaunch: !runtime.state().launched && inputs(runtime, start).length === 0,
    });
  });
}

for (const phase of ['module', 'data']) {
  test('pre-init pagehide without queued requests handles rejection at the ' + phase + ' boundary', async t => {
    const runtime = openRuntime({holdModule: phase === 'module', holdData: phase === 'data',
      observeInitialization: false});
    const unhandled = [];
    const onUnhandled = (error, promise) => {
      if (runtime.isInitializationPromise(promise)) unhandled.push(String(error));
    };
    process.on('unhandledRejection', onUnhandled);
    t.after(async () => {
      try {await runtime.close();} finally {process.off('unhandledRejection', onUnhandled);}
    });
    if (phase === 'data') await runtime.pumpUntil(() => runtime.dataGate.started, {label: 'silent DATA cancellation'});
    const start = runtime.trace.length;runtime.pagehide();
    runtime.releaseModule();runtime.dataGate.release();
    // Do not attach the fixture's rejection observer until Node has had a full
    // opportunity to report an unhandled rejection. node:test also fails such
    // events; this listener records identity without suppressing that failure.
    // These two event-loop turns are scheduling checks, not latency evidence.
    await nodeTurn();await nodeTurn();
    const beforeFixtureObserver = [...unhandled];
    const initialization = await runtime.initializationOutcome();
    await runtime.idle();runtime.noFatal();
    assert.equal(runtime.trace.filter(row => row.kind === 'message').length, 0, 'no queued command masks rejection');
    report(t, 'pre-init-pagehide-no-requests-' + phase, runtime, start, {
      queuedRequests: 0, initialization, nodeEventLoopTurnsBeforeFixtureObserver: 2,
      unhandledInitializationRejections: beforeFixtureObserver, fixtureObserverInitiallyAttached: false,
      browserUnhandledRejectionMeasured: false,
    }, {
      cancellationHasNoUnhandledInitializationRejection: beforeFixtureObserver.length === 0,
      cancelledInitializationNeverEmitsReady: initialization.error !== null && !initialization.readyEmitted,
      cancelledInitializationNeverLaunches: !runtime.state().launched && inputs(runtime, start).length === 0,
    });
  });
}

for (const command of ['configure', 'resources']) for (const boundary of ['fetch', 'body'])
  for (const invalidation of ['pagehide', 'stop', 'document-replaced']) {
    test(`${command} rejects late ${boundary} resources after ${invalidation} without writes or transfers`, async t => {
      const runtime = openRuntime();t.after(() => runtime.close());await runtime.launch();
      const {gate, resource} = runtime.resourceGate({holdBody: boundary === 'body'});
      const request = runtime.send(command, {options: {touchEnabled: false}, resources: [resource]});
      await runtime.pumpUntil(() => gate.started);
      if (boundary === 'body') {gate.release();await runtime.pumpUntil(() => gate.body.started);}
      if (invalidation === 'pagehide') runtime.pagehide();
      else if (invalidation === 'stop') runtime.nativeExitFrame();
      else runtime.replaceDocument();
      const start = runtime.trace.length;
      gate.release();gate.body?.release();await runtime.idle();
      assert.equal(runtime.replies.get(request)?.ok, false);
      assert.match(runtime.replies.get(request)?.error, /superseded|document was replaced/);
      assert.deepEqual(runtime.trace.slice(start).filter(row => row.kind === 'fs-write' && row.path === resource.path), []);
      assert.deepEqual(runtime.emitted.filter(row => row.event === 'transfer' && row.path === resource.path), []);
      runtime.noFatal();
    });
  }

test('retired resources commands are rejected even with no resources', async t => {
  const runtime = openRuntime();t.after(() => runtime.close());await runtime.ready();runtime.pagehide();
  const empty = runtime.send('resources', {resources: []}), {gate, resource} = runtime.resourceGate();
  const request = runtime.send('resources', {resources: [resource]});await runtime.idle();
  assert.equal(runtime.replies.get(empty)?.ok, false);assert.equal(runtime.replies.get(request)?.ok, false);
  assert.equal(gate.started, false, 'retired commands never start fetching');runtime.noFatal();
});

test('records the unchanged source bytes actually evaluated', async t => {
  const current = await Promise.all(sourceFiles.map(async ([name, url]) => [name,
    createHash('sha256').update(await readFile(url, 'utf8')).digest('hex')]));
  assert.deepEqual(Object.fromEntries(current), sourceIdentity);
  t.diagnostic(JSON.stringify({sourcePaths, sourceIdentity, sourceStable: true, mode, requireGoals,
    resultMeaning: requireGoals ? 'PASS requires every reported candidate goal at the stub-native boundary; not native-game/mobile/latency/FPS acceptance'
      : 'Characterization PASS validates fixture contracts and records unmet goals; not repair acceptance',
    baseline: 'TH09_TOUCH_QUEUE_REQUIRE_GOALS=0; optional TH09_TOUCH_QUEUE_RUNTIME_SOURCE selects the managed source and its real sibling dependencies'}));
});

