import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {dirname, join, resolve, extname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const runtime = join(port, 'web/sdl-runtime');
const build = process.env.TH13_SDL3_BUILD_DIR ?? join(port, 'web/artifacts');
const output = process.env.TH13_MENU_EVIDENCE;
const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

test('production Manual, Music Room and menu return survive repeated entry with audio output',
    {timeout: 300000}, async () => {
  const errors = [];
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      const file = path.startsWith('/artifacts/') ? join(build, path.slice(11))
        : join(runtime, path.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      response.writeHead(200, {'Content-Type': ({'.mjs': 'text/javascript', '.wasm': 'application/wasm',
        '.html': 'text/html', '.png': 'image/png'})[extname(file)] ?? 'application/octet-stream'}).end(data);
    } catch (error) { response.writeHead(404).end(String(error)); }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}});
    page.on('pageerror', error => errors.push(String(error)));
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
      {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 180000});
    const result = await page.evaluate(async () => {
      const mod = window.mod;
      mod._th13_loop_stop();
      const state = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4,
        mod._th13_probe_state() / 4 + 16)];
      const frame = (count = 1) => {
        for (let i = 0; i < count; ++i)
          if (!mod._th13_debug_frame()) throw Error(mod.UTF8ToString(mod._th13_error()));
      };
      const tap = scan => {
        mod._th13_key(scan, 1); frame();
        mod._th13_key(scan, 0); frame();
      };
      const report = () => mod.UTF8ToString(mod._th13_debug_layer_report());
      const enter = row => {
        if (state()[8] !== 1 || state()[9] !== 2) throw Error('Main menu is not interactive');
        for (let i = 0; state()[10] !== row && i < 12; ++i) tap(208);
        if (state()[10] !== row) throw Error('Cannot select main-menu row ' + row);
        tap(44); frame(28);
      };
      const portrait = () => {
        const lines = report().split('\n');
        for (let i = 0; i < lines.length; ++i)
          if (/script=(90|91)\b/.test(lines[i]) && /color=ff[0-9a-f]{6}\b/i.test(lines[i + 1] ?? ''))
            return true;
        return false;
      };
      frame(440);
      const snapshots = [];
      for (let visit = 0; visit < 2; ++visit) {
        enter(8);
        if (state()[8] !== 16) throw Error('Manual entry failed');
        frame(30); tap(44); frame(55);
        const first = report();
        if (!/script=9\b/.test(first)) throw Error('Manual first-page VM is missing');
        tap(208); frame(55);
        const second = report();
        if (!/script=9\b/.test(second)) throw Error('Manual next-page VM is missing');
        tap(1); frame(35);
        // Manual end needs age>29 (0x42da10) and the rebuilt 90/91 fade is
        // ins_409(60,0,255); allow both before asserting alpha 255.
        tap(1); frame(100);
        if (state()[8] !== 1 || state()[9] !== 2 || !portrait())
          throw Error('Manual return did not restore the title portrait: ' + JSON.stringify(state()) + '\n' + report());
        snapshots.push({kind: 'manual', visit, state: state(), firstPage: first, secondPage: second});
      }
      enter(6); frame(45);
      if (state()[8] !== 13) throw Error('Music Room entry failed');
      const commentsBefore = report();
      tap(208); frame(40);
      const commentsAfter = report();
      const visibleComments = text => {
        const lines = text.split('\n');
        return lines.filter((line, i) => /script=(4[1-8])\b/.test(line) &&
          /color=(?!00)[0-9a-f]{8}\b/i.test(lines[i + 1] ?? '')).length;
      };
      if (visibleComments(commentsBefore) !== 8 || visibleComments(commentsAfter) !== 8)
        throw Error('Music Room did not reveal all eight comment lines');
      tap(44); frame(4); tap(44); frame(25);
      tap(1); frame(80);
      if (state()[8] !== 1 || !portrait()) throw Error('Music Room return lost the title portrait');
      snapshots.push({kind: 'music', before: commentsBefore, after: commentsAfter, state: state()});
      const node = mod.SDL3?.audio_playback?.scriptProcessorNode;
      const context = mod.SDL3?.audioContext;
      if (!node || !context || typeof node.onaudioprocess !== 'function')
        throw Error('Production SDL audio output is unavailable');
      const pcm = {callbacks: 0, samples: 0, nonzero: 0, peak: 0};
      const original = node.onaudioprocess;
      node.onaudioprocess = function(event) {
        original.call(this, event);
        ++pcm.callbacks;
        for (let channel = 0; channel < event.outputBuffer.numberOfChannels; ++channel) {
          const values = event.outputBuffer.getChannelData(channel);
          pcm.samples += values.length;
          for (const value of values) {
            if (value !== 0) ++pcm.nonzero;
            pcm.peak = Math.max(pcm.peak, Math.abs(value));
          }
        }
      };
      await context.resume();
      mod._th13_loop_start();
      await new Promise(resolve => setTimeout(resolve, 700));
      mod._th13_loop_pause(1);
      const paused = {...pcm};
      mod._th13_loop_pause(0);
      await new Promise(resolve => setTimeout(resolve, 700));
      const resumed = {...pcm};
      node.onaudioprocess = original;
      mod._th13_loop_stop();
      return {snapshots, paused, resumed, audio: mod.th13Audio,
        error: mod.UTF8ToString(mod._th13_error())};
    });
    assert.equal(result.error, '');
    assert.equal(result.snapshots.length, 3);
    assert.ok(result.paused.nonzero > 0, 'actual Web Audio output contains PCM');
    assert.ok(result.resumed.nonzero > result.paused.nonzero, 'actual PCM output resumes');
    assert.ok(result.resumed.peak > 0 && Number.isFinite(result.resumed.peak));
    assert.deepEqual(errors, []);
    if (output) {
      await mkdir(output, {recursive: true});
      await writeFile(join(output, 'menu-closure.json'), JSON.stringify(result, null, 2) + '\n');
    }
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
