'use strict';
// Actual MenuAudioGate RED scenarios (deferred AudioContext methods).
// Read-only: imports the pinned real gate from the shared checkout and never
// writes outside this folder. No fix is applied here.
const path = require('node:path');
const {pathToFileURL} = require('node:url');

const SOUND = path.resolve(__dirname, '../../site/runtime/sound.mjs');
const SOUND_URL = pathToFileURL(SOUND).href;

class Deferred {
  constructor(label) {
    this.label = label;
    this.settled = false;
    this.promise = new Promise((resolve, reject) => { this._resolve = resolve; this._reject = reject; });
    this.promise.catch(() => {});
  }
  resolve() { if (!this.settled) { this.settled = true; this._resolve(); } }
  reject(error) { if (!this.settled) { this.settled = true; this._reject(error); } }
}

function invalidState(message) {
  const error = new Error(message || 'context is closed');
  error.name = 'InvalidStateError';
  return error;
}

class Env {
  constructor(holds) {
    this.holds = new Set(holds || []);
    this.deferreds = new Map();
    this.reachedWaiters = new Map();
    this.trace = [];
    this.writes = [];
    this.closes = [];
    this.stops = [];
    this.resumeAttempts = [];
    this.phase = 'pre';
    this.seq = 0;
    this.created = 0;
    this.retiredAt = 0;
  }
  deferred(label) {
    if (!this.deferreds.has(label)) this.deferreds.set(label, new Deferred(label));
    return this.deferreds.get(label);
  }
  hold(label) { this.holds.add(label); this.deferred(label); return this; }
  release(label) { this.deferred(label).resolve(); return this; }
  fail(label, error) { this.deferred(label).reject(error); return this; }
  await(label) { return this.holds.has(label) ? this.deferred(label).promise : Promise.resolve(); }
  note(name, method) {
    const row = {seq: ++this.seq, name, method, phase: this.phase};
    this.trace.push(row);
    const waiters = this.reachedWaiters.get(name + '.' + method);
    if (waiters) for (const resolve of waiters.splice(0)) resolve(row);
    return row;
  }
  whenReached(label) {
    const already = this.trace.find(row => row.name + '.' + row.method === label);
    if (already) return Promise.resolve(already);
    if (!this.reachedWaiters.has(label)) this.reachedWaiters.set(label, []);
    return new Promise(resolve => this.reachedWaiters.get(label).push(resolve));
  }
  markRetired() { this.retiredAt = this.writes.length; this.phase = 'retired'; }
}

function makeContext(env, name) {
  const context = {
    name,
    state: 'running',
    closed: false,
    closeCount: 0,
    gains: [],
    createGain() {
      const gain = {
        context,
        gain: {value: 0.5},
        connects: 0,
        connect() { this.connects += 1; },
        disconnect() { this.disconnected = true; },
      };
      context.gains.push(gain);
      return gain;
    },
    suspend() {
      env.note(name, 'suspend');
      if (context.closed) return Promise.reject(invalidState('suspend on closed context'));
      context.state = 'suspended';
      return env.await(name + '.suspend').then(() => {});
    },
    resume() {
      env.note(name, 'resume');
      if (context.state === 'closed' || context.closed) {
        env.resumeAttempts.push({name, phase: env.phase});
        return Promise.reject(invalidState('resume on closed context'));
      }
      context.state = 'running';
      return env.await(name + '.resume').then(() => {});
    },
    close() {
      env.note(name, 'close');
      context.closeCount += 1;
      env.closes.push(name);
      context.closed = true;
      context.state = 'closed';
      return env.await(name + '.close').then(() => {});
    },
  };
  return context;
}

function makeQueue(env, gameContext) {
  const gameGain = {context: gameContext, gain: {value: 0.5}, connect() {}, disconnect() {}};
  let currentContext = gameContext;
  let currentGain = gameGain;
  const queue = {
    channels: new Map(),
    retiredChannels: new Set(),
    stopChannel(channel) { env.stops.push(channel.id); },
  };
  Object.defineProperty(queue, 'context', {
    get() { return currentContext; },
    set(value) {
      env.writes.push({seq: ++env.seq, from: currentContext?.name ?? null, to: value?.name ?? null, phase: env.phase});
      currentContext = value;
    },
  });
  Object.defineProperty(queue, 'gain', {
    get() { return currentGain; },
    set(value) { currentGain = value; },
  });
  return queue;
}

async function loadGate(soundUrl = SOUND_URL) {
  const module = await import(soundUrl);
  if (typeof module.MenuAudioGate !== 'function') throw new Error('pinned MenuAudioGate export missing');
  return module.MenuAudioGate;
}

function settle(promise) {
  return promise.then(value => ({status: 'fulfilled', value}), error => ({status: 'rejected', name: error?.name ?? 'Error', message: String(error?.message ?? error)}));
}

function classify(env) {
  const post = env.writes.slice(env.retiredAt ?? 0);
  // Corruption = a retired menu context re-entering the live queue, or a stale
  // menu-owned cleanup overwriting whoever holds the queue now. A no-op restore
  // (game1->game1) is not corruption. Compare-and-swap cleanup is the only
  // acceptable post-retirement write.
  return {
    post,
    corruption: post.filter(row => row.to === 'menu1'),
    menuCloses: env.closes.filter(name => name.startsWith('menu')).length,
  };
}

// S0 clean path: enter completes, leave restores the queue exactly once.
async function scenarioClean(MenuAudioGate) {
  const env = new Env([]);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const gate = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  await gate.enter();
  const enteredName = queue.context?.name;
  await gate.leave();
  return {
    name: 'clean enter/leave',
    ok: enteredName === 'menu1' && queue.context === game && env.closes.join(',') === 'menu1' && env.resumeAttempts.length === 0,
    detail: {enteredName, after: queue.context?.name, closes: env.closes.slice(), writes: env.writes.length},
  };
}

// S1: dispose while enter() is suspended at game.suspend().
async function scenarioSuspend(MenuAudioGate) {
  const env = new Env(['game1.suspend']);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const gate = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  const enter = settle(gate.enter());
  await env.whenReached('game1.suspend');
  env.markRetired();
  const leave = settle(gate.leave());
  env.release('game1.suspend');
  const [entered, left] = await Promise.all([enter, leave]);
  const {post, corruption, menuCloses} = classify(env);
  const postResumes = env.resumeAttempts.filter(row => row.phase === 'retired').length;
  return {
    name: 'dispose while enter waits game.suspend',
    ok: corruption.length === 0 && menuCloses <= 1 && entered.status === 'rejected' && postResumes <= 1,
    detail: {
      entered: entered.status,
      left: left.status,
      finalQueue: queue.context?.name,
      postWrites: post.map(row => row.from + '->' + row.to),
      corruption: corruption.length,
      menuCloses,
      postResumeAttempts: postResumes,
    },
  };
}

// S2: dispose after the swap, while enter() waits at menu.resume().
async function scenarioMenuResume(MenuAudioGate) {
  const env = new Env(['menu1.resume']);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const gate = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  const enter = settle(gate.enter());
  await env.whenReached('menu1.resume');
  env.markRetired();
  const leave = settle(gate.leave());
  env.release('menu1.resume');
  const [entered, left] = await Promise.all([enter, leave]);
  const {post, corruption, menuCloses} = classify(env);
  return {
    name: 'dispose after swap, enter waits menu.resume',
    ok: entered.status === 'rejected' && corruption.length === 0 && menuCloses <= 1 && queue.context === game,
    detail: {
      entered: entered.status,
      left: left.status,
      finalQueue: queue.context?.name,
      postWrites: post.map(row => row.from + '->' + row.to),
      corruption: corruption.length,
      menuCloses,
    },
  };
}

// S3: successor gate re-enters on the same queue while the first enter is pending.
async function scenarioSuccessor(MenuAudioGate) {
  const env = new Env(['game1.suspend']);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const first = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  const second = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  const firstEnter = settle(first.enter());
  await env.whenReached('game1.suspend');
  env.markRetired();
  const firstLeave = settle(first.leave());
  const secondEnter = settle(second.enter());
  env.release('game1.suspend');
  const [firstResult, secondResult] = await Promise.all([firstEnter, secondEnter, firstLeave]);
  const stable = queue.context?.name === 'menu2';
  return {
    name: 'successor gate re-entry on the same queue',
    ok: firstResult.status === 'rejected' && secondResult.status === 'fulfilled' && stable,
    detail: {
      first: firstResult.status,
      second: secondResult.status,
      finalQueue: queue.context?.name,
      expected: 'menu2',
      corruption: env.writes.filter(row => row.phase === 'retired' && row.to === 'menu1').length,
    },
  };
}

// S4: the game context is already closed when dispose happens.
async function scenarioGameClosed(MenuAudioGate) {
  const env = new Env(['game1.suspend']);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const gate = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  const enter = settle(gate.enter());
  await env.whenReached('game1.suspend');
  game.state = 'closed';
  env.markRetired();
  const leave = settle(gate.leave());
  env.release('game1.suspend');
  const [entered, left] = await Promise.all([enter, leave]);
  const closedGameResumes = env.resumeAttempts.filter(row => row.name === 'game1' && row.phase === 'retired').length;
  return {
    name: 'game context closed at dispose',
    ok: entered.status === 'rejected' && closedGameResumes === 0 && queue.context === game,
    detail: {
      entered: entered.status,
      left: left.status,
      finalQueue: queue.context?.name,
      closedGameResumes,
    },
  };
}

// S5: menu-owned channels are stopped exactly once by leave(); game channels are untouched.
async function scenarioChannelStop(MenuAudioGate) {
  const env = new Env([]);
  const game = makeContext(env, 'game1');
  const queue = makeQueue(env, game);
  const gate = new MenuAudioGate(queue, {createContext: () => makeContext(env, 'menu' + (++env.created))});
  await gate.enter();
  const menu = queue.context;
  queue.channels.set('menu-channel', {id: 'menu-channel', source: {context: menu}, gain: {context: menu}});
  queue.channels.set('game-channel', {id: 'game-channel', source: {context: game}, gain: {context: game}});
  await gate.leave();
  return {
    name: 'leave stops queued menu channels once',
    ok: env.stops.filter(id => id === 'menu-channel').length === 1
      && env.stops.filter(id => id === 'game-channel').length === 0
      && env.closes.join(',') === 'menu1',
    detail: {stops: env.stops.slice()},
  };
}

function scenarioList() {
  return [
    ['clean enter/leave', scenarioClean],
    ['dispose while enter waits game.suspend', scenarioSuspend],
    ['dispose after swap, enter waits menu.resume', scenarioMenuResume],
    ['successor gate re-entry on the same queue', scenarioSuccessor],
    ['game context closed at dispose', scenarioGameClosed],
    ['leave stops queued menu channels once', scenarioChannelStop],
  ];
}

async function runAll(soundUrl) {
  const MenuAudioGate = await loadGate(soundUrl);
  const rows = [];
  for (const [expected, fn] of scenarioList()) {
    try {
      rows.push(await fn(MenuAudioGate));
    } catch (error) {
      rows.push({name: expected, ok: false, detail: {error: String(error?.stack ?? error)}});
    }
  }
  return rows;
}

module.exports = {runAll, scenarioList, SOUND, SOUND_URL, Deferred, Env, makeContext, makeQueue};
