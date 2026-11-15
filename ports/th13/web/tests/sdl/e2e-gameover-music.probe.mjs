// GameOver 音乐链探针（PR#2 疮痍计分曲 + 续关恢复验证）：
//   标题 → 开局 → 刷分过进榜门 → th13_debug_game_over（生产 0x43f570 同链）
//   → 断言覆盖层即选 Player's Score（th128_08）→ 名前录入 END → 续关菜单
//   → Continue → 断言对局曲按疮痍快照恢复（0043f66d/0043f68a 存，
//   00440772/00440786/0044078f 重排→排空→seek）。
// 用法（外层仓库根，shot 镜像）：
//   make shot CMD='node eagler-touhou/ports/th13/web/tests/sdl/e2e-gameover-music.probe.mjs \
//     --out artifacts/tmp-scratch/gameover-music'
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
const outDir = resolve(root, opt('out', 'artifacts/tmp-scratch/gameover-music'));
await mkdir(outDir, {recursive: true});
const log = (...xs) => console.log('[gom]', ...xs);

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
  } catch (e) { res.writeHead(500).end(String(e)); }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));

const require = createRequire(import.meta.url);
const {chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'));
const browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
  args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
         '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
const timeline = [];
const mark = (name, extra) => timeline.push({name, ...extra});
try {
  const page = await (await browser.newContext({viewport: {width: 800, height: 560}})).newPage();
  const errors = [];
  page.on('pageerror', e => errors.push(String(e)));
  // 冻结 rAF：外部 1/60 单帧泵（与 web_feedback_capture 同法）。
  await page.addInitScript(() => {
    let module = null;
    const raf = window.requestAnimationFrame.bind(window);
    window.requestAnimationFrame = cb => raf(() => { cb(); window.__th13_raf_frozen = true; });
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
                             {timeout: 300000});
  await page.evaluate(() => {
    window.__e2e = {
      probe() { const p = window.mod._th13_probe_state() / 4;
                return Array.from(window.mod.HEAP32.subarray(p, p + 16)); },
      err() { return window.mod.UTF8ToString(window.mod._th13_error()); },
      pause() { return JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_pause_state())); },
      key(s, d) { window.mod._th13_key(s, d); },
      frame() { if (!window.mod._th13_debug_frame())
                  throw new Error(window.__e2e.err()); },
      pump(n) { for (let i = 0; i < n; ++i) window.__e2e.frame(); },
      tap(k) { window.__e2e.key(k, 1); window.__e2e.frame();
               window.__e2e.key(k, 0); window.__e2e.frame(); },
      waitState(index, value, limit) {
        let n = 0;
        while (window.__e2e.probe()[index] !== value && n++ < limit) window.__e2e.frame();
        return window.__e2e.probe()[index] === value;
      },
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
  const drive = await page.evaluate(() => {
    const e = window.__e2e, report = {};
    // 标题 → 開始 → Easy → 灵梦 → 实机对局（web_gameover_name_entry 同驱动）。
    if (!e.waitState(8, 1, 2400)) throw Error('title-main 未到达');
    while (e.probe()[9] !== 2) e.frame();
    e.tap(44);
    if (!e.waitState(8, 6, 240)) throw Error('difficulty 未到达');
    while (e.probe()[9] !== 2) e.frame();
    e.tap(44);
    if (!e.waitState(8, 7, 240)) throw Error('character 未到达');
    while (e.probe()[9] !== 2) e.frame();
    e.pump(60);
    e.tap(44);
    if (!e.waitState(0, 1, 1200)) throw Error('run-start 未到达');
    // 刷分过进榜门（默认榜最低 10000）。
    e.key(44, 1);
    let n = 0;
    while (e.probe()[8] < 10000 && n++ < 7200) e.frame();
    e.key(44, 0);
    e.pump(10);
    if (e.probe()[8] < 10000) throw Error('分数未达进榜门：' + e.probe()[8]);
    report.score = e.probe()[8];
    report.stageMusic = e.audio().filename;
    if (!report.stageMusic || report.stageMusic === 'th128_08.wav')
      throw Error('开局对局曲异常：' + report.stageMusic);
    // 生产 0x43f570 同链 GameOver。
    if (!window.mod._th13_debug_game_over()) throw Error('debug_game_over 被拒：' + e.err());
    return report;
  });
  mark('game-over-open', drive);
  const scoreAudio = await page.evaluate(() => {
    window.__e2e.pump(30); return window.__e2e.audio();
  });
  mark('score-audio', scoreAudio);
  if (scoreAudio.filename !== 'th128_08.wav' || scoreAudio.paused)
    throw Error('Game over must select Player\'s Score: ' + JSON.stringify(scoreAudio));
  // 名前录入 END（光标初值 90）→ 续关菜单（phase 6）。
  const toContinue = await page.evaluate(() => {
    const e = window.__e2e;
    e.pump(120);
    if (e.pause().phase !== 18) throw Error('phase18 未到达：' + JSON.stringify(e.pause()));
    e.tap(44);  // END → LAB_00440520 → phase 6 续关菜单
    e.pump(30);
    const menu = e.pause();
    if (menu.phase !== 6 || menu.can_continue !== 1)
      throw Error('continue menu 未到达：' + JSON.stringify(menu));
    return menu;
  });
  mark('continue-menu', toContinue);
  // Continue（默认光标 0）→ continue_run：恢复对局曲与播放位置。
  const afterContinue = await page.evaluate(stageMusic => {
    const e = window.__e2e;
    e.tap(44);  // Z：Continue → finish_selection(0) → continue_run()
    e.pump(30);
    return {pause: e.pause(), audio: e.audio(), err: e.err()};
  }, drive.stageMusic);
  mark('after-continue', afterContinue);
  if (afterContinue.err !== '') throw Error('续关运行期错误：' + afterContinue.err);
  if (afterContinue.audio.filename !== drive.stageMusic || afterContinue.audio.paused)
    throw Error('Continue must restore the stage track ' + drive.stageMusic + ': ' +
                JSON.stringify(afterContinue.audio));
  if (afterContinue.pause.phase !== 0)
    throw Error('续关后覆盖层未收起：' + JSON.stringify(afterContinue.pause));
  if (errors.length) throw Error('页面错误：' + errors.join('; '));
  await page.screenshot({path: join(outDir, 'after-continue.png')});
  await writeFile(join(outDir, 'report.json'), JSON.stringify({timeline}, null, 2) + '\n');
  mark('gom-ok');
} catch (e) {
  mark('failure', {error: String((e && e.stack) ?? e)});
  await writeFile(join(outDir, 'report.json'), JSON.stringify({timeline}, null, 2) + '\n')
    .catch(() => {});
  throw e;
} finally {
  await browser.close();
  server.close();
}
