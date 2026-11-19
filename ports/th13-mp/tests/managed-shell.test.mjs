import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';

const require = createRequire(new URL('../../../launcher/package.json', import.meta.url));
const {parse} = require('acorn');
const source = readFileSync(new URL('../web/sdl-runtime/shell.mjs', import.meta.url), 'utf8');
const ast = parse(source, {ecmaVersion: 'latest', sourceType: 'module'});
const functions = ['apply', 'command'].map(name => {
  const node = ast.body.find(node => node.type === 'FunctionDeclaration' && node.id.name === name);
  return source.slice(node.start, node.end);
}).join('\n');

test('managed DATA accepts a parent-realm ArrayBuffer and rejects invalid buffers', async () => {
  let managed;
  function visit(node) {
    if (!node || typeof node !== 'object') return;
    if (node.type === 'IfStatement' && source.slice(node.test.start, node.test.end) === "query.get('managedData') === '1'") managed = node;
    for (const value of Object.values(node)) {
      if (Array.isArray(value)) value.forEach(visit);
      else if (value && typeof value === 'object') visit(value);
    }
  }
  visit(ast);
  assert.ok(managed);
  const view = managed.consequent.body.find(node => node.type === 'VariableDeclaration' &&
    node.declarations.some(declaration => declaration.id.name === 'view'));
  assert.ok(view);
  const preflight = source.slice(managed.consequent.start + 1, view.start);
  const validate = buffer => runInNewContext('(async () => {' + preflight + ';return buffer.byteLength;})()', {
    window: {}, validEpoch: true, epoch: 7, game: 'th13',
    query: new URLSearchParams({gameGeneration: 'installed-generation'}),
    parent: {async __eaglerPrepareManagedRuntimeDataV1(request) {
      assert.equal(request.game, 'th13'); assert.equal(request.epoch, 7);
      assert.equal(request.generation, 'installed-generation'); return {buffer};
    }},
  });
  assert.equal(await validate(new ArrayBuffer(32)), 32);
  const detached = new ArrayBuffer(32); structuredClone(detached, {transfer: [detached]});
  for (const invalid of [null, {}, new Uint8Array(32), new ArrayBuffer(0), detached,
    new SharedArrayBuffer(32), {byteLength: 32, [Symbol.toStringTag]: 'ArrayBuffer'}]) {
    await assert.rejects(validate(invalid), /游戏资源包无效/);
  }
});

function adapter(options = {}) {
  const calls = [];
  const context = {options, music: true, launched: true, stopping: false,
    document: {hidden: false}, innerWidth: 1000, innerHeight: 800,
    canvas: {getBoundingClientRect: () => ({left: 100, top: 40, width: 640, height: 480})},
    core: new Proxy({}, {set: () => true, get: (_, name) => (...args) => calls.push([name, ...args])})};
  runInNewContext(functions + ';globalThis.adapter = {apply, command};', context);
  return {calls, context, ...context.adapter};
}

test('managed touch settings reach native endpoints with clamped sensitivity', async () => {
  const h = adapter({touchEnabled: true, touchMovementMode: 'touch-unlimited',
    touchSensitivity: 900, touchFocusMode: 'two-finger', doubleTapBombEnabled: true});
  h.apply();
  assert.deepEqual(h.calls.at(-1), ['_th13_touch_options', 1, 1, 3, 1, 1]);
  h.calls.length = 0;
  await h.command({command: 'touch-controls', touchSensitivity: 200,
    fireEnabled: true, focusEnabled: true, bombSerial: 12, escapeSerial: 8, joystickX: 32767, joystickY: -100});
  assert.deepEqual(h.calls.at(-1), ['_th13_touch_controls', 1, 1, 1, 12, 8, 32767, -100]);
  assert.equal(h.context.options.touchSensitivity, 200);
  assert.deepEqual(h.calls.find(row => row[0] === '_th13_touch_options'),
    ['_th13_touch_options', 1, 1, 2, 1, 1]);
  h.calls.length = 0;
  await h.command({command: 'touch-controls', touchSensitivity: 99});
  assert.equal(h.context.options.touchSensitivity, 200);
  assert.equal(h.calls.some(row => row[0] === '_th13_touch_options'), false);
});

test('managed direct touch normalizes coordinates and fails closed outside live input', async () => {
  const h = adapter({touchEnabled: true});
  const down = {command: 'direct-touch', type: 'down', id: 7, x: .42, y: .35};
  await h.command(down);
  assert.deepEqual(h.calls, [['_th13_touch', 0, 7, .5, .5]]);
  h.calls.length = 0;
  await h.command({...down, x: NaN});
  h.context.document.hidden = true;
  await h.command(down);
  h.context.document.hidden = false;
  h.context.options.touchEnabled = false;
  await h.command(down);
  assert.deepEqual(h.calls, []);
  await h.command({command: 'touch-cancel'});
  assert.deepEqual(h.calls, [['_th13_touch_cancel']]);
});

test('managed pointer cancellation releases only the cancelled finger', () => {
  const loop = ast.body.find(node => node.type === 'ForOfStatement' &&
    source.slice(node.start, node.end).includes('canvas.addEventListener(event'));
  assert.ok(loop);
  const listeners = new Map(), calls = [];
  const context = {launched: true, options: {touchEnabled: true},
    canvas: {addEventListener: (name, callback) => listeners.set(name, callback),
      getBoundingClientRect: () => ({left: 0, top: 0, width: 640, height: 480})},
    core: {_th13_touch: (...args) => calls.push(args),
      _th13_touch_cancel: () => { throw Error('single-finger cancellation must not reset all input'); }}};
  runInNewContext(source.slice(loop.start, loop.end), context);
  listeners.get('pointercancel')({pointerType: 'touch', pointerId: 22,
    clientX: 320, clientY: 240, preventDefault() {}});
  assert.deepEqual(calls, [[2, 22, .5, .5]]);
});

test('managed frame callback reports the actual SDL audio queue', () => {
  const initialized = ast.body.find(node => node.type === 'VariableDeclaration' &&
    node.declarations.some(declaration => declaration.id.name === 'initialized'));
  let callback;
  function visit(node) {
    if (!node || typeof node !== 'object') return;
    if (node.type === 'Property' && node.key.name === 'onGameFrame') callback = node.value;
    for (const value of Object.values(node)) {
      if (Array.isArray(value)) value.forEach(visit);
      else if (value && typeof value === 'object') visit(value);
    }
  }
  visit(initialized);
  assert.ok(callback);
  const emitted = [];
  const context = {core: {th13Audio: {device: true, failed: false, mixedFrames: 44100,
    queuedFrames: 4410, sampleRate: 44100, track: 2, musicCursor: 100, suspended: false},
    _th13_phase: () => 1}, music: true, ticks: 0, presents: 0, maxFrameMs: 0,
    first: true, last: 0, performance: {now: () => 1000},
    // P5 后 onGameFrame 泵多人桥帧（生产 shell 的模块级 let multiplayerBridge
    // = null——沙箱同型提供绑定，可选链对 null 无操作）。
    multiplayerBridge: {frame() { calls2.push('frame'); }},
    emit: (event, fields) => emitted.push([event, fields])};
  const calls2 = [];
  runInNewContext('const onGameFrame = function' + source.slice(callback.start, callback.end) +
    ';onGameFrame(true, 4, 1);', context);
  const audio = emitted.find(([event]) => event === 'audio-health')?.[1];
  assert.ok(audio);
  assert.equal(audio.ready, true);
  assert.equal(audio.renderedFrames, 44100);
  assert.equal(audio.queuedFrames, 4410);
  assert.equal(audio.queuedMs, 100);
  assert.equal(audio.track, 2);
  assert.equal(audio.errorCode, 0);
});
