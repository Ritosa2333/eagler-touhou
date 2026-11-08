import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {dirname, join, resolve, extname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const runtime = join(port, 'web/sdl-runtime');
const build = process.env.TH13_SDL3_BUILD_DIR ?? join(port, 'web/artifacts');
const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

const MIME = {'.mjs': 'text/javascript', '.wasm': 'application/wasm',
  '.html': 'text/html', '.png': 'image/png'};

// Real page + real production wasm. New coverage below only dispatches
// PointerEvents and reads production state through the exported probes.
function startRuntimeServer() {
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      const file = path.startsWith('/artifacts/') ? join(build, path.slice(11))
        : join(runtime, path.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      response.writeHead(200, {'Content-Type': MIME[extname(file)] ?? 'application/octet-stream'}).end(data);
    } catch { response.writeHead(404).end(); }
  });
  return new Promise(done => server.listen(0, '127.0.0.1', () => done(server)));
}

async function openRuntime(server) {
  const browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
    args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader', '--enable-unsafe-swiftshader']});
  const page = await browser.newPage({viewport: {width: 640, height: 480}, hasTouch: true, isMobile: true});
  const consoleMessages = [], pageErrors = [];
  page.on('console', message => consoleMessages.push({type: message.type(), text: message.text(),
    url: message.location().url}));
  page.on('pageerror', error => pageErrors.push(String(error)));
  await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`);
  await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 120000});
  return {browser, page, consoleMessages, pageErrors};
}

function assertRuntimeLogs(consoleMessages, pageErrors) {
  assert.deepEqual(pageErrors, [], 'pageerror raised by the runtime page');
  // The test server has no favicon.ico; that 404 is host-page noise, not a
  // product failure. Every other console error fails the test.
  const unexpected = consoleMessages.filter(message =>
    message.type === 'error' && !message.url.endsWith('/favicon.ico'));
  assert.deepEqual(unexpected, [], 'unexpected console errors');
  console.log('browser console: ' + JSON.stringify(consoleMessages));
}

test('standalone mobile controls preserve fire across cancellation and input edges', {timeout: 180000}, async () => {
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      const file = path.startsWith('/artifacts/') ? join(build, path.slice(11))
        : join(runtime, path.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      response.writeHead(200, {'Content-Type': ({'.mjs': 'text/javascript', '.wasm': 'application/wasm',
        '.html': 'text/html', '.png': 'image/png'})[extname(file)] ?? 'application/octet-stream'}).end(data);
    } catch { response.writeHead(404).end(); }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader', '--enable-unsafe-swiftshader']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}, hasTouch: true, isMobile: true});
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`);
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 120000});
    await page.waitForSelector('[data-touch="fire"]');
    const result = await page.evaluate(() => {
      const mod = window.mod;
      const calls = [];
      for (const name of ['_th13_touch_controls', '_th13_touch_cancel', '_th13_touch', '_th13_key']) {
        const original = mod[name];
        mod[name] = (...args) => { calls.push([name, ...args]); return original(...args); };
      }
      const pointer = (node, type, id = 1) => node.dispatchEvent(new PointerEvent(type, {
        bubbles: true, cancelable: true, pointerType: 'touch', pointerId: id,
        clientX: 200, clientY: 200,
      }));
      const button = name => document.querySelector(`[data-touch="${name}"]`);
      const press = name => { pointer(button(name), 'pointerdown'); pointer(button(name), 'pointerup'); };
      const fire = button('fire');
      const initial = fire.getAttribute('aria-pressed');
      window.dispatchEvent(new Event('blur'));
      const afterBlur = fire.getAttribute('aria-pressed');
      const lastControls = calls.filter(c => c[0] === '_th13_touch_controls').at(-1);
      press('fire');
      const off = fire.getAttribute('aria-pressed');
      window.dispatchEvent(new Event('blur'));
      const offAfterBlur = fire.getAttribute('aria-pressed');
      press('fire'); press('pause'); press('bomb'); press('trance'); press('retry');
      const canvas = document.querySelector('canvas');
      pointer(canvas, 'pointerdown', 7); pointer(canvas, 'pointermove', 7); pointer(canvas, 'pointerup', 7);
      return {initial, afterBlur, lastControls, off, offAfterBlur, calls};
    });
    assert.equal(result.initial, 'true');
    assert.equal(result.afterBlur, 'true');
    assert.equal(result.lastControls[2], 1);
    assert.equal(result.off, 'false');
    assert.equal(result.offAfterBlur, 'false');
    for (const [scan, down] of [[46, 1], [46, 0], [19, 1], [19, 0]])
      assert.ok(result.calls.some(c => c[0] === '_th13_key' && c[1] === scan && c[2] === down));
    assert.ok(result.calls.some(c => c[0] === '_th13_touch_controls' && c[4] === 1));
    assert.ok(result.calls.some(c => c[0] === '_th13_touch_controls' && c[5] === 1));
    assert.deepEqual(result.calls.filter(c => c[0] === '_th13_touch').map(c => c[1]), [0, 1, 2]);
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});

test('mobile auto fire survives pause, canvas resume and retry in a real run', {timeout: 180000}, async () => {
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      const file = path.startsWith('/artifacts/') ? join(build, path.slice(11))
        : join(runtime, path.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      response.writeHead(200, {'Content-Type': ({'.mjs': 'text/javascript', '.wasm': 'application/wasm',
        '.html': 'text/html', '.png': 'image/png'})[extname(file)] ?? 'application/octet-stream'}).end(data);
    } catch { response.writeHead(404).end(); }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader', '--enable-unsafe-swiftshader']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}, hasTouch: true, isMobile: true});
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`);
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 120000});
    const result = await page.evaluate(() => {
      const mod = window.mod;
      // Stop RAF without calling loop_stop(), which deliberately clears input.
      mod._th13_loop_pause(1);
      const state = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4, mod._th13_probe_state() / 4 + 16)];
      const step = (n = 1) => { for (let i = 0; i < n; ++i)
        if (!mod._th13_debug_frame()) throw Error(mod.UTF8ToString(mod._th13_error())); };
      const until = (predicate, n, name) => { for (let i = 0; i < n; ++i) { if (predicate()) return; step(); }
        throw Error(`${name}: ${JSON.stringify(state())}`); };
      const key = scan => { mod._th13_key(scan, 1); step(); mod._th13_key(scan, 0); step(); };
      const touch = (node, type, id = 1) => node.dispatchEvent(new PointerEvent(type, {
        bubbles: true, cancelable: true, pointerType: 'touch', pointerId: id, clientX: 200, clientY: 200,
      }));
      const button = name => { const node = document.querySelector(`[data-touch="${name}"]`);
        touch(node, 'pointerdown'); touch(node, 'pointerup'); };
      const tapCanvas = () => { const canvas = document.querySelector('canvas');
        touch(canvas, 'pointerdown', 7); touch(canvas, 'pointerup', 7); };
      const report = () => mod.UTF8ToString(mod._th13_debug_stage_report());
      const generations = () => [...report().matchAll(/player_shot gen=(\d+)/g)].map(m => Number(m[1]));
      const newest = () => Math.max(0, ...generations());
      const fire = document.querySelector('[data-touch="fire"]');
      if (fire.getAttribute('aria-pressed') !== 'true') throw Error('auto fire initially disabled');
      // Count calls from here onward to detect any control re-send after resume/retry.
      const original = mod._th13_touch_controls; let sends = 0;
      mod._th13_touch_controls = (...args) => { ++sends; return original(...args); };
      until(() => state()[0] === 0 && state()[8] === 1 && state()[9] === 2, 500, 'main');
      key(44); step(35); key(44); step(25); key(44);
      until(() => state()[0] === 1, 180, 'run');
      until(() => newest() > 0, 160, 'initial shots');
      const beforeResume = newest();
      button('pause'); step(20);
      if (state()[14] === 0) throw Error('pause did not open');
      const sendsAtPause = sends;
      tapCanvas();
      until(() => state()[14] === 0, 70, 'resume');
      until(() => newest() > beforeResume, 120, 'resumed shots');
      const sendsAtResume = sends;
      step(15); // Allow the menu-confirm and pause pulses to release.
      button('pause'); step(20);
      if (state()[14] === 0) throw Error('second pause did not open');
      const retry = document.querySelector('[data-touch="retry"]');
      touch(retry, 'pointerdown'); step(); touch(retry, 'pointerup'); step(25);
      key(200); key(44);
      until(() => state()[14] === 0 && state()[5] < 20, 150, 'retry');
      until(() => newest() > 0, 160, 'retry shots');
      return {sendsAtPause, sendsAtResume, sendsAtEnd: sends, state: state(), report: report()};
    });
    assert.equal(result.sendsAtResume, result.sendsAtPause, 'canvas resume did not resend fire state');
    assert.equal(result.sendsAtEnd, result.sendsAtResume + 1, 'only the second pause button sent controls');
    assert.match(result.report, /player_shot gen=/);
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});

test('four-finger canvas contacts open pause and auto fire resumes in a real run', {timeout: 180000}, async () => {
  const server = await startRuntimeServer();
  let opened;
  try {
    opened = await openRuntime(server);
    const {page} = opened;
    const result = await page.evaluate(() => {
      const mod = window.mod;
      // Stop RAF without calling loop_stop(), which deliberately clears input.
      mod._th13_loop_pause(1);
      const state = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4, mod._th13_probe_state() / 4 + 16)];
      const step = (n = 1) => { for (let i = 0; i < n; ++i)
        if (!mod._th13_debug_frame()) throw Error(mod.UTF8ToString(mod._th13_error())); };
      const until = (predicate, n, name) => { for (let i = 0; i < n; ++i) { if (predicate()) return; step(); }
        throw Error(`${name}: ${JSON.stringify(state())}`); };
      const key = scan => { mod._th13_key(scan, 1); step(); mod._th13_key(scan, 0); step(); };
      const pointer = (node, type, id, x, y) => node.dispatchEvent(new PointerEvent(type, {
        bubbles: true, cancelable: true, pointerType: 'touch', pointerId: id, clientX: x, clientY: y,
      }));
      const canvas = document.querySelector('canvas');
      const stageReport = () => mod.UTF8ToString(mod._th13_debug_stage_report());
      const generations = () => [...stageReport().matchAll(/player_shot gen=(\d+)/g)].map(m => Number(m[1]));
      const newest = () => Math.max(0, ...generations());

      until(() => state()[0] === 0 && state()[8] === 1 && state()[9] === 2, 500, 'main');
      key(44); step(35); key(44); step(25); key(44);
      until(() => state()[0] === 1, 180, 'run');
      until(() => newest() > 0, 160, 'initial shots');

      const fire = document.querySelector('[data-touch="fire"]');
      const before = {state: state(), fire: fire.getAttribute('aria-pressed'), newest: newest()};
      // Record only the canvas gesture path. The control-button path calls
      // _th13_touch_controls; the trance/retry buttons call _th13_key.
      const touchCalls = [], controlCalls = [], keyCalls = [];
      const originalTouch = mod._th13_touch;
      mod._th13_touch = (...args) => { touchCalls.push(args.slice(0, 4)); return originalTouch(...args); };
      const originalControls = mod._th13_touch_controls;
      mod._th13_touch_controls = (...args) => { controlCalls.push(args.slice()); return originalControls(...args); };
      const originalKey = mod._th13_key;
      mod._th13_key = (...args) => { keyCalls.push(args.slice()); return originalKey(...args); };

      const box = canvas.getBoundingClientRect();
      const fractions = [0.2, 0.35, 0.5, 0.65];
      const points = fractions.map(f => [box.left + box.width * f, box.top + box.height * 0.5]);
      for (let i = 0; i < 4; ++i) pointer(canvas, 'pointerdown', i + 1, points[i][0], points[i][1]);
      // DOM events alone must not fake the pause: the sampled controller
      // turns the fourth contact into its Escape pulse on the next frame.
      const immediate = {state: state(), phase: mod._th13_phase()};
      step(20); // Pause opening frames; same settle as the existing button-path test.
      const paused = {state: state(), phase: mod._th13_phase(), newest: newest()};
      const frozenFrame = paused.state[5];
      step(10);
      const stillPaused = {state: state(), phase: mod._th13_phase()};
      for (let i = 0; i < 4; ++i) pointer(canvas, 'pointerup', i + 1, points[i][0], points[i][1]);
      step(2);
      // A canvas tap confirms the pause menu and resumes the run.
      const center = [box.left + box.width * 0.5, box.top + box.height * 0.5];
      pointer(canvas, 'pointerdown', 11, center[0], center[1]);
      pointer(canvas, 'pointerup', 11, center[0], center[1]);
      until(() => state()[14] === 0, 70, 'resume');
      const resumedAt = newest();
      until(() => newest() > resumedAt, 120, 'resumed shots');
      return {before, immediate, paused, stillPaused, frozenFrame, resumedAt,
        after: {state: state(), phase: mod._th13_phase(),
          fire: fire.getAttribute('aria-pressed'), newest: newest()},
        touchCalls, controlCalls, keyCalls, fractions};
    });
    assert.equal(result.before.fire, 'true', 'auto fire toggle was off before the gesture');
    assert.equal(result.immediate.state[14], 0, 'pointerdown alone opened pause without a sampled frame');
    assert.notEqual(result.paused.state[14], 0, 'four canvas contacts did not open pause');
    assert.equal(result.paused.phase, 2, 'phase probe did not report pause');
    assert.ok(result.paused.state[6] & 0x10, 'pause scene flag was not set');
    assert.equal(result.stillPaused.state[5], result.frozenFrame, 'stage frame advanced while paused');
    assert.equal(result.stillPaused.state[14], result.paused.state[14], 'pause state changed without input');
    assert.equal(result.after.state[14], 0, 'canvas tap did not resume the run');
    assert.equal(result.after.phase, 1, 'phase probe did not report run after resume');
    assert.equal(result.after.fire, 'true', 'auto fire toggle was lost across pause');
    assert.ok(result.after.newest > result.resumedAt, 'auto fire did not produce new shots after resume');
    // Four distinct canvas contacts, ids 1..4, mapped through the canvas box.
    const downs = result.touchCalls.filter(call => call[0] === 0 && call[1] >= 1 && call[1] <= 4);
    assert.deepEqual(downs.map(call => call[1]), [1, 2, 3, 4], 'canvas pointerdown ids');
    for (let i = 0; i < downs.length; ++i) {
      assert.ok(Math.abs(downs[i][2] - result.fractions[i]) < 1e-6,
        `canvas x ${downs[i][2]} != ${result.fractions[i]}`);
      assert.ok(Math.abs(downs[i][3] - 0.5) < 1e-6, `canvas y ${downs[i][3]} != 0.5`);
    }
    const ups = result.touchCalls.filter(call => call[0] === 2 && call[1] >= 1 && call[1] <= 4);
    assert.deepEqual(ups.map(call => call[1]), [1, 2, 3, 4], 'canvas pointerup ids');
    assert.ok(result.touchCalls.some(call => call[0] === 0 && call[1] === 11), 'resume canvas tap down');
    assert.ok(result.touchCalls.some(call => call[0] === 2 && call[1] === 11), 'resume canvas tap up');
    // Canvas coordinates must not travel through the touch-control button path.
    assert.deepEqual(result.controlCalls, [], 'canvas gesture sent touch-control button state');
    assert.deepEqual(result.keyCalls, [], 'canvas gesture used the key-button path');
    console.log('four-finger evidence: ' + JSON.stringify({
      immediatePause: result.immediate.state[14], pausedScreen: result.paused.state[14],
      pausedPhase: result.paused.phase, pausedSceneFlags: result.paused.state[6],
      stageFrame: [result.frozenFrame, result.stillPaused.state[5]],
      resumedScreen: result.after.state[14], resumedPhase: result.after.phase,
      fire: result.after.fire, newest: [result.before.newest, result.resumedAt, result.after.newest],
      touchCalls: result.touchCalls.map(call => [call[0], call[1],
        Number(call[2].toFixed(4)), Number(call[3].toFixed(4))]),
    }));
    assertRuntimeLogs(opened.consoleMessages, opened.pageErrors);
  } finally {
    await opened?.browser.close();
    await new Promise(done => server.close(done));
  }
});

test('single-finger canvas drag moves the player and stops on release in a real run', {timeout: 180000}, async () => {
  const server = await startRuntimeServer();
  let opened;
  try {
    opened = await openRuntime(server);
    const {page} = opened;
    const result = await page.evaluate(() => {
      const mod = window.mod;
      mod._th13_loop_pause(1);
      const state = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4, mod._th13_probe_state() / 4 + 16)];
      const step = (n = 1) => { for (let i = 0; i < n; ++i)
        if (!mod._th13_debug_frame()) throw Error(mod.UTF8ToString(mod._th13_error())); };
      const until = (predicate, n, name) => { for (let i = 0; i < n; ++i) { if (predicate()) return; step(); }
        throw Error(`${name}: ${JSON.stringify(state())}`); };
      const key = scan => { mod._th13_key(scan, 1); step(); mod._th13_key(scan, 0); step(); };
      const pointer = (node, type, id, x, y) => node.dispatchEvent(new PointerEvent(type, {
        bubbles: true, cancelable: true, pointerType: 'touch', pointerId: id, clientX: x, clientY: y,
      }));
      const canvas = document.querySelector('canvas');
      const stageReport = () => mod.UTF8ToString(mod._th13_debug_stage_report());
      const generations = () => [...stageReport().matchAll(/player_shot gen=(\d+)/g)].map(m => Number(m[1]));
      const newest = () => Math.max(0, ...generations());
      const stateReport = () => JSON.parse(mod.UTF8ToString(mod._th13_debug_state_report()));
      const player = () => stateReport().player_position;

      until(() => state()[0] === 0 && state()[8] === 1 && state()[9] === 2, 500, 'main');
      key(44); step(35); key(44); step(25); key(44);
      until(() => state()[0] === 1, 180, 'run');
      until(() => newest() > 0, 160, 'initial shots');

      const before = {player: player(), keys: stateReport().keys, state: state()};
      const touchCalls = [], controlCalls = [], keyCalls = [];
      const originalTouch = mod._th13_touch;
      mod._th13_touch = (...args) => { touchCalls.push(args.slice(0, 4)); return originalTouch(...args); };
      const originalControls = mod._th13_touch_controls;
      mod._th13_touch_controls = (...args) => { controlCalls.push(args.slice()); return originalControls(...args); };
      const originalKey = mod._th13_key;
      mod._th13_key = (...args) => { keyCalls.push(args.slice()); return originalKey(...args); };

      const box = canvas.getBoundingClientRect();
      const start = [box.left + box.width * 0.3, box.top + box.height * 0.5];
      const end = [box.left + box.width * 0.5, box.top + box.height * 0.5];
      pointer(canvas, 'pointerdown', 5, start[0], start[1]);
      step(2);
      const down = {player: player(), state: state()};
      pointer(canvas, 'pointermove', 5, end[0], end[1]);
      step(30);
      const dragged = {player: player(), state: state()};
      pointer(canvas, 'pointerup', 5, end[0], end[1]);
      step(10);
      const released = {player: player(), state: state()};
      return {before, down, dragged, released, touchCalls, controlCalls, keyCalls};
    });
    // Auto fire holds Z (90); direction keys 37..40 must be absent so the
    // observed movement can only come from the canvas drag path.
    assert.deepEqual(result.before.keys.filter(key => key >= 37 && key <= 40), [],
      'direction keys were held before the drag');
    assert.equal(result.before.state[0], 1, 'run scene was not active before the drag');
    assert.equal(result.down.state[14], 0, 'canvas pointerdown paused the run');
    assert.ok(Math.abs(result.down.player[0] - result.before.player[0]) < 1e-6,
      'player moved on pointerdown alone');
    // 0.2 canvas-width drag: the production controller converts the delta to
    // playfield coordinates and the 60Hz battle moves the player each step.
    assert.ok(result.dragged.player[0] - result.before.player[0] > 20,
      `drag did not move player right: ${JSON.stringify([result.before.player, result.dragged.player])}`);
    assert.ok(Math.abs(result.dragged.player[1] - result.before.player[1]) < 1e-6,
      'horizontal drag changed player y');
    assert.ok(Math.abs(result.released.player[0] - result.dragged.player[0]) < 1e-6,
      'player kept moving after pointerup');
    assert.ok(Math.abs(result.released.player[1] - result.dragged.player[1]) < 1e-6,
      'player y changed after pointerup');
    assert.deepEqual(result.touchCalls.map(call => [call[0], call[1]]), [[0, 5], [1, 5], [2, 5]],
      'canvas pointer sequence');
    assert.ok(Math.abs(result.touchCalls[0][2] - 0.3) < 1e-6, 'pointerdown canvas x');
    assert.ok(Math.abs(result.touchCalls[0][3] - 0.5) < 1e-6, 'pointerdown canvas y');
    assert.ok(Math.abs(result.touchCalls[1][2] - 0.5) < 1e-6, 'pointermove canvas x');
    assert.ok(Math.abs(result.touchCalls[1][3] - 0.5) < 1e-6, 'pointermove canvas y');
    assert.deepEqual(result.controlCalls, [], 'drag sent touch-control button state');
    assert.deepEqual(result.keyCalls, [], 'drag used the key-button path');
    console.log('drag evidence: ' + JSON.stringify({
      playerBefore: result.before.player, playerDown: result.down.player,
      playerDragged: result.dragged.player, playerReleased: result.released.player,
      keys: result.before.keys,
      touchCalls: result.touchCalls.map(call => [call[0], call[1],
        Number(call[2].toFixed(4)), Number(call[3].toFixed(4))]),
    }));
    assertRuntimeLogs(opened.consoleMessages, opened.pageErrors);
  } finally {
    await opened?.browser.close();
    await new Promise(done => server.close(done));
  }
});
