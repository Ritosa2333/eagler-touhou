// Ending 尾段探针：th13_debug_ending(0,1) 直入 Reimu/Normal 结局（与全通同一
// 生产入口链 0x43c1c0 case0xf），验证 Z 连打推进（op5/6 等待的按下沿放行）→
// staff roll → AfterEnding → 标题 reason 3 结算屏（ResultName）→ 回主菜单。
// 说明：debug 入口 recorder 无数据 → result_replay_save_available()=false，
// 名录入 END 后直接回主菜单（不进回放保存屏）——保存链本身由全量运行验证。
// 用法（外层仓库根，shot 镜像）：
//   make shot CMD='node eagler-touhou/ports/th13/web/tests/sdl/e2e-ending-tail.probe.mjs \
//     --out artifacts/tmp-scratch/ending-tail'
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
const outDir = resolve(root, opt('out', 'artifacts/tmp-scratch/ending-tail'));
await mkdir(outDir, {recursive: true});
const log = (...xs) => console.log('[tail]', ...xs);

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

const timeline = [];
const mark = (e, d = {}) => { timeline.push({e, ...d}); log(e, JSON.stringify(d)); };
let browser;
try {
  browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
    args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
           '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
  const page = await (await browser.newContext({viewport: {width: 800, height: 560}})).newPage();
  await page.addInitScript(() => {
    let module;
    Object.defineProperty(window, 'mod', {configurable: true,
      get() { return module; }, set(value) {
        module = value;
        const start = value._th13_loop_start;
        value._th13_loop_start = () => { start(); value._th13_loop_pause(1); window.__paused = true; };
      }});
  });
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
      err() { return window.mod.UTF8ToString(window.mod._th13_error()); },
      key(s, d) { window.mod._th13_key(s, d); },
      pump(n) { for (let i = 0; i < n; ++i)
        if (!window.mod._th13_debug_frame()) throw new Error(window.__e2e.err()); },
      audio() {
        const m = window.mod, a = {...m.th13Audio};
        const fmt = m.FS.readFile('/assets/thbgm.fmt');
        const offset = a.track * 52;
        a.filename = '';
        if (a.track >= 0 && offset + 16 <= fmt.length)
          for (let i = offset; i < offset + 16 && fmt[i]; ++i)
            a.filename += String.fromCharCode(fmt[i]);
        return a;
      },
    };
  });
  // 结算覆盖层直入（生产 debug 入口）：Reimu/Normal 无续关通关态。
  // 入口要求 running==false：先 loop_stop（rAF 已被暂停包装冻结）。
  const ok = await page.evaluate(() => { window.mod._th13_loop_stop();
                                         return window.mod._th13_debug_ending(0, 1); });
  if (!ok) throw Error('th13_debug_ending failed: ' +
    await page.evaluate(() => window.mod.UTF8ToString(window.mod._th13_error())));
  mark('ending-entered');

  // Z 连打推进直到标题结算屏（ResultName=14）。
  let pumps = 0;
  for (;;) {
    const r = await page.evaluate(({cap}) => {
      for (let i = 0; i < cap; ++i) {
        const p = window.__e2e.probe();
        if (p[0] === 0 && p[8] === 14) return {done: true, p, pumps: i};
        window.__e2e.key(44, 1);
        if (!window.mod._th13_debug_frame()) return {failed: window.__e2e.err(), p};
        window.__e2e.key(44, 0);
        window.mod._th13_debug_frame();
        window.mod._th13_debug_frame();
      }
      return {done: false, p: window.__e2e.probe(), pumps: cap};
    }, {cap: 2000});
    if (r.failed) throw Error('ending failure: ' + r.failed);
    pumps += r.pumps * 3;
    if (r.done) { mark('result-name-screen', {pumps, probe: r.p}); break; }
    log('progress', JSON.stringify({pumps, probe: r.p}));
    if (pumps > 200000) throw Error('ending did not finish');
  }
  const scoreAudio = await page.evaluate(() => {
    window.__e2e.pump(10); return window.__e2e.audio();
  });
  mark('result-audio', scoreAudio);
  if (scoreAudio.filename !== 'th128_08.wav' || scoreAudio.paused || scoreAudio.failed)
    throw Error('Result screen must play Player\'s Score: ' + JSON.stringify(scoreAudio));
  await page.screenshot({path: join(outDir, 'result-name.png')});

  // 结算名屏（debug 态 rank<0 = 锁定）：Z → case3 → 无回放保存 → 回主菜单。
  for (;;) {
    const r = await page.evaluate(({cap}) => {
      for (let i = 0; i < cap; ++i) {
        const p = window.__e2e.probe();
        if (p[0] === 0 && p[8] === 1) return {done: true, p, pumps: i};
        window.__e2e.key(44, 1);
        if (!window.mod._th13_debug_frame()) return {failed: window.__e2e.err(), p};
        window.__e2e.key(44, 0);
        window.mod._th13_debug_frame();
        window.mod._th13_debug_frame();
      }
      return {done: false, p: window.__e2e.probe(), pumps: cap};
    }, {cap: 1000});
    if (r.failed) throw Error('result screen failure: ' + r.failed);
    pumps += r.pumps * 3;
    if (r.done) { mark('back-to-main', {pumps, probe: r.p}); break; }
    if (pumps > 220000) throw Error('result screen did not return to main');
  }
  const titleAudio = await page.evaluate(() => {
    window.__e2e.pump(10); return window.__e2e.audio();
  });
  mark('title-audio', titleAudio);
  if (titleAudio.filename !== 'th13_00.wav' || titleAudio.paused || titleAudio.failed)
    throw Error('Leaving results must restore title music: ' + JSON.stringify(titleAudio));
  await page.screenshot({path: join(outDir, 'back-to-main.png')});
  await writeFile(join(outDir, 'report.json'),
                  JSON.stringify({timeline}, null, 2) + '\n');
  mark('tail-ok');
} catch (e) {
  mark('failure', {error: String((e && e.stack) ?? e)});
  throw e;
} finally {
  await browser?.close().catch(() => {});
  await new Promise(done => server.close(done));
}
