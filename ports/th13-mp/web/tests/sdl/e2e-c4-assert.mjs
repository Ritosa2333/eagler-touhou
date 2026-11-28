import {createServer} from 'node:http';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const args = process.argv.slice(2);
const opt = (name, fallback) => {
  const i = args.indexOf('--' + name);
  return i >= 0 && args[i + 1] !== undefined ? args[i + 1] : fallback;
};
const root = resolve(dirname(fileURLToPath(import.meta.url)), '../../../../../..');
const portRoot = join(root, 'eagler-touhou/ports/th13');
const runtimeDir = join(portRoot, 'web/sdl-runtime');
const buildDir = process.env.TH13_SDL3_BUILD_DIR ?? join(portRoot, 'web/artifacts');
const dir = resolve(root, opt('dir', 'artifacts/tmp-scratch/e2e'));
const exportBytes = await readFile(join(dir, 'e2e_export.rpy'));
const scoreBytes = await readFile(join(dir, 'e2e_score.dat'));
const b64 = buf => buf.toString('base64');
const indexEntry = [{name: 'th13_01.rpy', size: exportBytes.length, ts: 1793927339327}];
await mkdir(dir, {recursive: true});
const log = (...xs) => console.log('[c4]', ...xs);
const timeline = {};
const failures = [];

const mime = {'.html': 'text/html', '.mjs': 'text/javascript', '.wasm': 'application/wasm'};
const server = createServer(async (req, res) => {
  try {
    const url = req.url.split('?', 2)[0];
    const data = url.startsWith('/sdl-runtime/')
        ? await readFile(join(runtimeDir, url.replace('/sdl-runtime/', '')))
        : url.startsWith('/artifacts/')
          ? await readFile(join(buildDir, url.replace('/artifacts/', '')))
          : null;
    if (!data) { res.writeHead(404).end(); return; }
    res.writeHead(200, {'Content-Type': mime[url.slice(url.lastIndexOf('.'))] ?? 'application/octet-stream',
                        'Cross-Origin-Opener-Policy': 'same-origin',
                        'Cross-Origin-Embedder-Policy': 'require-corp'});
    res.end(data);
  } catch { res.writeHead(404).end(); }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

let browser;
let page;
try {
  browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
    args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
           '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
  const context = await browser.newContext({viewport: {width: 800, height: 560}});
  page = await context.newPage();
  await page.addInitScript(({idx, blob, score}) => {
    localStorage.setItem('th13.replays', idx);
    localStorage.setItem('th13.replay.th13_01.rpy', blob);
    localStorage.setItem('th13.scoredat', score);
  }, {idx: JSON.stringify(indexEntry), blob: b64(exportBytes), score: b64(scoreBytes)});
  await page.addInitScript(() => {
    let module;
    Object.defineProperty(window, 'mod', {configurable: true,
      get() { return module; }, set(value) {
        module = value;
        const start = value._th13_loop_start;
        value._th13_loop_start = () => { start(); value._th13_loop_pause(1); window.__paused = true; };
      }});
  });
  const boot = async () => {
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
                    {waitUntil: 'domcontentloaded'});
    await page.waitForFunction('window.mod && window.__paused && window.mod._th13_phase() === 0' +
                               ' && window.__th13_builtin_replays_done === true' +
                               ' && window.__th13_replay_restore_done === true',
                               null, {timeout: 300000});
    await page.evaluate(() => {
      window.__e2e = {
        probe() { const p = window.mod._th13_probe_state() / 4;
                  return Array.from(window.mod.HEAP32.subarray(p, p + 16)); },
        progress() { return JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_replay_progress())); },
        err() { return window.mod.UTF8ToString(window.mod._th13_error()); },
        key(s, d) { window.mod._th13_key(s, d); },
        pump(n) { for (let i = 0; i < n; ++i)
          if (!window.mod._th13_debug_frame()) throw new Error(window.__e2e.err());
          return window.__e2e.probe(); },
        pumpWhile(source, cap) {
          const pred = new Function('p', 'return (' + source + ');');
          for (let i = 0; i < cap; ++i) {
            let p;
            try { p = window.__e2e.pump(1); }
            catch (e) { return {p: window.__e2e.probe(), ok: false, pumpError: String(e)}; }
            if (!p || typeof p.length !== 'number')
              return {p, ok: false, badProbe: String(p)};
            if (pred(p)) return {p, ok: true};
          }
          return {p: window.__e2e.probe(), ok: false};
        },
      };
    });
  };
  const tap = async scan => {
    await page.evaluate(s => { window.__e2e.key(s, 1); window.__e2e.pump(1);
                               window.__e2e.key(s, 0); window.__e2e.pump(2); }, scan);
  };
  const pumpUntil = async (pred, cap, what) => {
    let done = 0, last;
    while (done < cap) {
      const r = await page.evaluate(({src, n}) => window.__e2e.pumpWhile(src, n),
                                    {src: pred, n: Math.min(2000, cap - done)});
      last = r.p;
      done += Math.min(2000, cap - done);
      if (r.ok) return last;
    }
    throw Error(`pumpUntil('${what}') exhausted; probe=${JSON.stringify(last)}`);
  };
  await boot();

  const restored = await page.evaluate(() => ({
    hint: window.mod._th13_replay_import_hint(),
    failures: (window.__th13_log ?? []).filter(l => l.startsWith('replay-restore')),
    index: JSON.parse(localStorage.getItem('th13.replays') || '[]'),
    blobLen: (localStorage.getItem('th13.replay.th13_01.rpy') || '').length,
    scoreLen: (localStorage.getItem('th13.scoredat') || '').length,
  }));
  timeline.restored = {...restored, blobMatches: restored.blobLen === b64(exportBytes).length,
                       scoreMatches: restored.scoreLen === b64(scoreBytes).length};
  log('restored', JSON.stringify(timeline.restored));
  if (restored.hint !== 0) failures.push(`restore hint ${restored.hint} != 0`);
  if (JSON.stringify(restored.failures) !== '[]') failures.push('restore failures: ' + JSON.stringify(restored.failures));
  if (!timeline.restored.blobMatches) failures.push('blob changed at restore');

  log('env', await page.evaluate(() => JSON.stringify({
    e2e: typeof window.__e2e, probe: typeof window.__e2e?.probe,
    heap32: typeof window.mod.HEAP32,
    p0: window.__e2e?.probe ? window.__e2e.probe()[0] : null})));
  await pumpUntil('p[0]===0 && p[8]===1 && p[9]===2', 1500, 'title interactive');
  {
    let p = await page.evaluate(() => window.__e2e.probe());
    let guard = 0;
    while (p[10] !== 4 && guard++ < 10) { await tap(208); p = await page.evaluate(() => window.__e2e.probe()); }
    if (p[10] !== 4) failures.push(`main cursor ${p[10]} != 4`);
    timeline.mainCursor = p[10];
  }
  await tap(44);
  await pumpUntil('p[8]===11', 1200, 'replay menu');
  let rp = null;
  for (let attempt = 0; attempt < 8 && !(rp && rp.active); ++attempt) {
    await page.evaluate(n => window.__e2e.pump(n), 60);
    await tap(44);
    rp = await page.evaluate(cap => {
      let j = null;
      for (let i = 0; i < cap; ++i) {
        if (!window.mod._th13_debug_frame()) break;
        j = window.__e2e.progress();
        if (j.active && j.slot_index === 0 && j.slot_number === 1 && j.consumed >= 30) return j;
      }
      return j;
    }, 4000);
    log('playback-attempt', JSON.stringify({attempt, rp}));
  }
  rp = rp && rp.active ? rp : await page.evaluate(() => window.__e2e.progress());
  timeline.playback = rp;
  log('playback', JSON.stringify(rp));
  if (!rp || !rp.active || rp.slot_index !== 0 || rp.slot_number !== 1 ||
      rp.slot_frames !== 11675)
    failures.push('replay menu playback did not open our export: ' + JSON.stringify(rp));

  await page.reload({waitUntil: 'domcontentloaded'});
  await boot();
  const after = await page.evaluate(() => ({
    index: JSON.parse(localStorage.getItem('th13.replays') || '[]'),
    blob: localStorage.getItem('th13.replay.th13_01.rpy'),
    score: localStorage.getItem('th13.scoredat'),
    hint: window.mod._th13_replay_import_hint(),
    failures: (window.__th13_log ?? []).filter(l => l.startsWith('replay-restore')),
  }));
  timeline.reload = {
    indexEqual: JSON.stringify(after.index) === JSON.stringify(indexEntry),
    blobEqual: after.blob === b64(exportBytes),
    scoreEqual: after.score === b64(scoreBytes),
    hint: after.hint, failures: after.failures,
  };
  log('reload', JSON.stringify(timeline.reload));
  if (!timeline.reload.indexEqual) failures.push('index changed across reload');
  if (!timeline.reload.blobEqual) failures.push('blob changed across reload');
  if (!timeline.reload.scoreEqual) failures.push('scoredat changed across reload');
  if (after.hint !== 0) failures.push(`post-reload hint ${after.hint} != 0`);
  if (JSON.stringify(after.failures) !== '[]') failures.push('post-reload restore failures');

  await writeFile(join(dir, 'e2e_c4_report.json'),
                  JSON.stringify({timeline, failures, verdict:
                                  failures.length ? 'FAIL' : 'PASS'}, null, 2) + '\n');
  log('verdict', failures.length ? 'FAIL ' + JSON.stringify(failures) : 'PASS');
  if (failures.length) process.exitCode = 1;
} catch (e) {
  log('failure', String((e && e.stack) ?? e));
  await writeFile(join(dir, 'e2e_c4_report.json'),
                  JSON.stringify({timeline, failure: String((e && e.stack) ?? e)},
                                 null, 2) + '\n').catch(() => {});
  process.exitCode = 1;
} finally {
  await browser?.close().catch(() => {});
  await new Promise(done => server.close(done));
}
