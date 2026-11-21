'use strict';
// Corrected ownership scenarios for the ACTUAL OriginalPauseController.
//
// Every scenario uses explicit deferred signals so the test PROVES it reached the exact yield point,
// then disposes the owner, snapshots the call counters as the post-dispose baseline, releases the
// pending continuation, and asserts that no NEW call happened afterwards. Native API attempts are
// counted at function ENTRY, so a stale continuation that merely calls into the native menu is
// visible even if the native side rejects it.
//
// A pre-dispose commit is allowed when the scenario reaches it (post-commit/audio.leave case): the
// assertion is always "no NEW calls after dispose", never "no commit may exist at all".
//
// No production file is modified. `soundUrl` lets the caller run these same scenarios against the
// pinned shared controller (RED today) or against an in-memory minimal-fix preview.
const deferred = () => {
  let resolve;
  const promise = new Promise((done) => { resolve = done; });
  return {promise, resolve};
};
const tick = () => new Promise((resolve) => setImmediate(resolve));

function createGates() {
  const enterGate = {index: 0, at: 0, entered: deferred(), release: deferred()};
  const consumeGate = {index: 0, at: 0, entered: deferred(), release: deferred()};
  const leaveGate = {index: 0, at: 0, entered: deferred(), release: deferred()};
  const presentationGate = {index: 0, at: 0, entered: deferred(), release: deferred()};
  return {enterGate, consumeGate, leaveGate, presentationGate};
}

function gated(holder, index, label) {
  return async () => {
    holder.index += 1;
    if (holder.at === holder.index) {
      holder.entered.resolve(label);
      await holder.release.promise;
    }
  };
}

function fakeModule(model) {
  const attempts = {open: 0, tick: 0, commit: 0, state: 0};
  const calls = [];
  return {
    attempts, calls, model,
    UTF8ToString: (pointer) => (pointer === 100 ? JSON.stringify(model) : 'fake native error'),
    _th128_world_pause_open(value) { attempts.open += 1; calls.push(['open', value]); return 1; },
    _th128_world_pause_tick(held, pressed) { attempts.tick += 1; calls.push(['tick', held, pressed]); return 1; },
    _th128_world_pause_commit(action) { attempts.commit += 1; calls.push(['commit', action]); model.action = 0; return 1; },
    _th128_world_pause_state() { attempts.state += 1; return 100; },
  };
}

async function boot(soundUrl, options = {}) {
  const {OriginalPauseController} = await import(soundUrl);
  const gates = createGates();
  if (options.enterAt) gates.enterGate.at = options.enterAt;
  if (options.consumeAt) gates.consumeGate.at = options.consumeAt;
  if (options.leaveAt) gates.leaveGate.at = options.leaveAt;
  if (options.presentationAt) gates.presentationGate.at = options.presentationAt;
  const model = {
    active: true, ready: true, closing: false, confirm: false, manual: false,
    replayPhase: 0, replayNameContextReady: true, action: options.action || 0, selection: 0, serial: 1, helpPage: -1,
  };
  const module = fakeModule(model);
  const counters = {enter: 0, leave: 0, suspend: 0, release: 0, consume: 0, presentation: 0, resume: 0, title: 0, restart: 0};
  const log = [];
  const audio = {
    async enter() { counters.enter += 1; log.push('enter'); await gated(gates.enterGate, counters.enter, 'audio.enter')(); },
    async suspend() { counters.suspend += 1; log.push('suspend'); },
    async leave() { counters.leave += 1; log.push('leave'); await gated(gates.leaveGate, counters.leave, 'audio.leave')(); },
  };
  const controller = new OriginalPauseController({
    module,
    releaseInput: () => { counters.release += 1; log.push('releaseInput'); },
    consumeSound: async () => {
      counters.consume += 1; log.push('consume');
      await gated(gates.consumeGate, counters.consume, 'consumeSound')();
    },
    audio,
    onResume: async () => { counters.resume += 1; log.push('onResume'); },
    onTitle: async () => { counters.title += 1; log.push('onTitle'); },
    onRestart: async () => { counters.restart += 1; log.push('onRestart'); },
    onPresentation: async () => {
      counters.presentation += 1; log.push('presentation');
      await gated(gates.presentationGate, counters.presentation, 'onPresentation')();
    },
  });
  return {controller, module, audio, counters, log, gates};
}

function snapshot(controller, module, counters) {
  return {
    active: controller.active,
    closed: controller.closed,
    open: module.attempts.open,
    tick: module.attempts.tick,
    commit: module.attempts.commit,
    state: module.attempts.state,
    consume: counters.consume,
    enter: counters.enter,
    leave: counters.leave,
    suspend: counters.suspend,
    release: counters.release,
    presentation: counters.presentation,
    resume: counters.resume,
    title: counters.title,
    restart: counters.restart,
  };
}

function differences(before, after) {
  const keys = new Set([...Object.keys(before), ...Object.keys(after)]);
  const changed = [];
  for (const key of keys) if (before[key] !== after[key]) changed.push(`${key}:${before[key]}->${after[key]}`);
  return changed;
}

async function scenario(name, fn) {
  try {
    const detail = await fn();
    return {name, ok: true, detail: detail || ''};
  } catch (error) {
    return {name, ok: false, detail: error && error.message ? error.message : String(error)};
  }
}

const mustBeSilentAfterDispose = (before, after, extra = '') => {
  const changed = differences(before, after);
  if (changed.length) throw new Error(`post-dispose calls appeared: ${changed.join(', ')}${extra ? ' | ' + extra : ''}`);
};

async function runScenarios(soundUrl) {
  const results = [];

  results.push(await scenario('open/audio.enter: dispose while enter pending must not revive or call natives', async () => {
    const {controller, module, counters, gates} = await boot(soundUrl, {enterAt: 1});
    const opening = controller.open();
    await gates.enterGate.entered.promise;
    assertReached('audio.enter');
    await controller.dispose();
    const before = snapshot(controller, module, counters);
    gates.enterGate.release.resolve();
    await opening;
    const after = snapshot(controller, module, counters);
    if (after.active !== false) throw new Error(`owner revived: active=${after.active} closed=${after.closed}`);
    mustBeSilentAfterDispose(before, after);
    return 'no new calls';
  }));

  results.push(await scenario('open/consumeSound: dispose during consume must not open the native menu or activate', async () => {
    const {controller, module, counters, gates} = await boot(soundUrl, {consumeAt: 1});
    const opening = controller.open();
    await gates.consumeGate.entered.promise;
    assertReached('consumeSound');
    await controller.dispose();
    const before = snapshot(controller, module, counters);
    gates.consumeGate.release.resolve();
    await opening;
    const after = snapshot(controller, module, counters);
    if (after.active !== false) throw new Error(`owner revived: active=${after.active}`);
    mustBeSilentAfterDispose(before, after);
    return 'no new calls';
  }));

  results.push(await scenario('step/wake: dispose while wake audio.enter is pending must not tick/commit/resume', async () => {
    const {controller, module, counters, gates} = await boot(soundUrl, {enterAt: 2, action: 1});
    await controller.open();
    await controller.suspend();
    controller.keyDown('KeyZ');
    const stepping = controller.step();
    await gates.enterGate.entered.promise;
    assertReached('wake audio.enter');
    await controller.dispose();
    const before = snapshot(controller, module, counters);
    gates.enterGate.release.resolve();
    await stepping;
    const after = snapshot(controller, module, counters);
    if (after.active !== false) throw new Error(`owner active after dispose: ${after.active}`);
    mustBeSilentAfterDispose(before, after);
    return 'no new calls';
  }));

  results.push(await scenario('step/consumeSound: dispose during step consume must not commit the pending action', async () => {
    const {controller, module, counters, gates} = await boot(soundUrl, {consumeAt: 2, action: 1});
    await controller.open();
    const stepping = controller.step();
    await gates.consumeGate.entered.promise;
    assertReached('step consumeSound');
    await controller.dispose();
    const before = snapshot(controller, module, counters);
    gates.consumeGate.release.resolve();
    await stepping;
    const after = snapshot(controller, module, counters);
    mustBeSilentAfterDispose(before, after, 'the pending action was committed after dispose');
    return 'no new calls';
  }));

  results.push(await scenario('step/onPresentation: dispose during presentation must not commit or resume', async () => {
    const {controller, module, counters, gates} = await boot(soundUrl, {presentationAt: 1, action: 1});
    await controller.open();
    const stepping = controller.step();
    await gates.presentationGate.entered.promise;
    assertReached('onPresentation');
    await controller.dispose();
    const before = snapshot(controller, module, counters);
    gates.presentationGate.release.resolve();
    await stepping;
    const after = snapshot(controller, module, counters);
    mustBeSilentAfterDispose(before, after, 'action processed after dispose');
    return 'no new calls';
  }));

  for (const action of [1, 2, 3, 6]) {
    results.push(await scenario(`postcommit/audio.leave action=${action}: dispose after commit must not fire the successor callback`, async () => {
      const {controller, module, counters, gates} = await boot(soundUrl, {leaveAt: 1, action});
      await controller.open();
      const stepping = controller.step();
      await gates.leaveGate.entered.promise;
      assertReached('audio.leave');
      const preDispose = snapshot(controller, module, counters);
      if (preDispose.commit !== 1) throw new Error(`scenario must reach commit BEFORE dispose, commit=${preDispose.commit}`);
      const disposing = controller.dispose();
      const before = snapshot(controller, module, counters);
      gates.leaveGate.release.resolve();
      await Promise.all([stepping, disposing]);
      const after = snapshot(controller, module, counters);
      mustBeSilentAfterDispose(before, after, `callback for action ${action} fired after dispose`);
      if (after.resume || after.title || after.restart) throw new Error('successor callback exceeded baseline');
      return 'pre-dispose commit=1, no callback after dispose';
    }));
  }

  return results;
}

function assertReached(label) {
  if (!label) throw new Error('scenario did not record its yield point');
}

module.exports = {runScenarios};
