// 低延迟画布验收工具（th13-shot 镜像内运行）：
//   node canvas-latency-check.mjs <th13|th13-mp>
// 三项断言（对应 e1 验证清单 b 项）：
//   (a) 授予断言：真实 Chromium 里画布 WebGL2 getContextAttributes 报告
//       desynchronized===true 且 powerPreference==='high-performance'，
//       且 window.__th13_canvas_presentation() 同源一致；
//   (b) 连续帧互异：连续 rAF tick 采样画布 PNG sha256，不全相同
//       （防冻结/防黑屏），且游戏帧号单调前进；
//   (c) 降级模拟：伪造 getContextAttributes 返回 desynchronized:false、
//       或干脆缺失该 API 时，页面启动路径与行为无差异（mod 装载、帧前进、
//       画面持续变化、无错误横幅）——证明游戏路径不随授予状态分支。
// 运行方式与 harness/latency-probe.mjs 同型：本地 http 服务 sdl-runtime 页面
// + 构建产物 + 资产，chromium(playwright-core) 打开生产 th13.html。
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve, dirname, join, extname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const here = dirname(fileURLToPath(import.meta.url));
const port = resolve(here, '..');                      // 本工具属 ports/th13（SP 目录）
const workspace = resolve(port, '../../..');           // → 工作区根 /work（games/、artifacts/ 所在）
const portName = process.argv[2] ?? 'th13';
if (!['th13', 'th13-mp'].includes(portName)) {
  console.error('用法：node canvas-latency-check.mjs <th13|th13-mp>');
  process.exit(2);
}
// 目标 port 目录：SP 是本工具所在目录，MP 是同仓姐妹目录（均在本 worktree 内）。
const target = portName === 'th13' ? port : resolve(port, '..', portName);
const runtimeDir = join(target, 'web/sdl-runtime');
const buildDir = join(target, 'web/artifacts');
const replaysDir = join(workspace, 'games/replays');
const rawDir = join(workspace, 'artifacts/reference/raw');

const MIME = {
  '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.mjs': 'text/javascript',
  '.wasm': 'application/wasm', '.png': 'image/png', '.json': 'application/json',
  '.anm': 'application/octet-stream', '.ecl': 'application/octet-stream',
  '.rpy': 'application/octet-stream', '.sht': 'application/octet-stream',
  '.ttc': 'application/octet-stream', '.wav': 'audio/wav', '.fmt': 'application/octet-stream',
  '.css': 'text/css', '.dat': 'application/octet-stream', '.txt': 'text/plain',
  '.part': 'application/octet-stream', '.otf': 'application/octet-stream',
};
const routes = [
  ['/assets/', join(runtimeDir, 'assets')],
  ['/artifacts/', buildDir],
  ['/replays/', replaysDir],
  ['/', runtimeDir],
];
const server = createServer(async (request, response) => {
  try {
    const path = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
    if (!path.startsWith('/assets/')) console.error(`[server] ${path}`);
    let dir = null, rest = path;
    if (path.startsWith('/assets/') && path.endsWith('.sht')) {
      dir = rawDir; rest = path.slice('/assets/'.length);
    } else {
      for (const [prefix, root] of routes) {
        if (!path.startsWith(prefix)) continue;
        dir = root; rest = path.slice(prefix.length); break;
      }
    }
    if (dir) {
      const file = resolve(dir, rest);
      if (file.startsWith(resolve(dir))) {
        const bytes = await readFile(file);
        response.writeHead(200, {'Content-Type': MIME[extname(file)] ?? 'application/octet-stream'});
        response.end(bytes); return;
      }
    }
    console.error(`[server] 404 ${path}`);
    response.writeHead(404).end('not found');
  } catch { response.writeHead(404).end('not found'); }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
const origin = `http://127.0.0.1:${server.address().port}`;

const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

const browser = await chromium.launch({
  executablePath: process.env.CHROMIUM_PATH,
  headless: true,
  args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
         '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
});

let failures = 0;
const check = (name, ok, detail = '') => {
  console.log(`${ok ? 'PASS' : 'FAIL'}  ${name}${detail ? '  ' + detail : ''}`);
  if (!ok) failures++;
};

// 每个会话：打开生产 th13.html → 等 mod 装载与帧泵就绪 → 采样。
// degrade: 'none' | 'false'（伪造 desynchronized:false）| 'missing'（摘除 API）。
async function session(degrade) {
  const page = await browser.newPage({viewport: {width: 800, height: 560}});
  const diagnostics = [];
  page.on('console', m => { if (m.type() === 'error' || m.type() === 'warning') diagnostics.push(`console.${m.type()}: ${m.text()}`); });
  page.on('pageerror', e => diagnostics.push(`pageerror: ${e?.stack || e}`));
  page.on('requestfailed', r => diagnostics.push(`requestfailed: ${r.url()} ${r.failure()?.errorText}`));
  page.on('response', r => { if (r.status() >= 400) diagnostics.push(`http${r.status()}: ${r.url()}`); });
  if (degrade === 'false') {
    await page.addInitScript(() => {
      // 伪造：浏览器/驱动不支持低延迟时 getContextAttributes 的回报样态。
      WebGL2RenderingContext.prototype.getContextAttributes =
        () => ({alpha: true, antialias: true, desynchronized: false, powerPreference: 'default'});
    });
  } else if (degrade === 'missing') {
    await page.addInitScript(() => {
      // 伪造：极端旧实现缺 getContextAttributes（presentation() 得 null）。
      delete WebGL2RenderingContext.prototype.getContextAttributes;
    });
  }
  const bootAt = Date.now();
  await page.goto(`${origin}/th13.html`, {waitUntil: 'domcontentloaded'});
  try {
    await page.waitForFunction('!!window.mod', null, {timeout: 180000});
  } catch (error) {
    const info = await page.evaluate(() => ({url: location.href, log: window.__th13_log ?? null,
      loading: document.getElementById('loading')?.textContent ?? null,
      errorText: document.getElementById('error')?.textContent ?? null,
      hasModule: !!window.Module, title: document.title})).catch(e => ({evalError: String(e)}));
    console.error('BOOT TIMEOUT diagnostics:', JSON.stringify({diagnostics, info}, null, 2));
    throw error;
  }
  await page.waitForFunction('window.mod._th13_frame() > 30', null, {timeout: 120000});
  const bootMs = Date.now() - bootAt;

  // (b) 连续帧采样：rAF tick 内读画布 PNG 哈希 + 游戏帧号（防冻结/防黑屏）。
  const samples = await page.evaluate(() => new Promise(resolve => {
    const hashes = [], frames = [];
    const canvas = document.querySelector('canvas');
    const tick = () => {
      frames.push(window.mod._th13_frame());
      hashes.push(canvas.toDataURL('image/png').slice(22));
      if (hashes.length < 30) requestAnimationFrame(tick);
      else resolve({frames, hashes});
    };
    requestAnimationFrame(tick);
  }));
  const digests = samples.hashes.map(b64 =>
    createHash('sha256').update(Buffer.from(b64, 'base64')).digest('hex'));
  const distinct = new Set(digests).size;
  const monotonic = samples.frames.every((f, i) => i === 0 || f >= samples.frames[i - 1]);
  const advanced = samples.frames[samples.frames.length - 1] > samples.frames[0];
  const grew = await page.evaluate(async () => {
    const before = window.mod._th13_frame();
    await new Promise(r => setTimeout(r, 1000));
    return window.mod._th13_frame() > before;
  });
  const errorText = await page.evaluate("document.getElementById('error').textContent");
  const presentation = await page.evaluate('window.__th13_canvas_presentation?.() ?? null');
  const attributes = degrade === 'none' ? await page.evaluate(() => {
    const gl = document.querySelector('canvas').getContext('webgl2');
    return gl?.getContextAttributes?.() ?? null;
  }) : null;
  await page.close();
  return {bootMs, distinct, monotonic, advanced, grew, errorText, presentation, attributes,
          frameRange: [samples.frames[0], samples.frames[samples.frames.length - 1]]};
}

console.log(`canvas-latency-check port=${portName} origin=${origin}`);
const control = await session('none');
// (a) 授予断言（仅真实会话）
check('granted: desynchronized===true', control.attributes?.desynchronized === true,
  JSON.stringify(control.attributes));
check('granted: powerPreference===high-performance', control.attributes?.powerPreference === 'high-performance');
check('presentation() 与 getContextAttributes 同源一致',
  control.presentation?.desynchronized === (control.attributes?.desynchronized === true) &&
  control.presentation?.powerPreference === (control.attributes?.powerPreference ?? null),
  JSON.stringify(control.presentation));
// (b) 连续帧互异
check('连续 30 帧画布哈希不全相同', control.distinct >= 2, `distinct=${control.distinct}/30`);
check('游戏帧号单调前进且持续增长', control.monotonic && control.advanced && control.grew,
  `frames ${control.frameRange[0]}→${control.frameRange[1]}`);
check('无启动错误横幅', control.errorText === '', JSON.stringify(control.errorText));

// (c) 降级模拟：伪造/摘除 getContextAttributes 后启动路径与行为不变。
for (const degrade of ['false', 'missing']) {
  const run = await session(degrade);
  const expected = degrade === 'false'
    ? {desynchronized: false, powerPreference: 'default'}
    : {desynchronized: false, powerPreference: null};
  check(`降级(${degrade}): 页面正常启动且帧泵就绪`, run.bootMs > 0 && run.grew,
    `boot=${run.bootMs}ms frames ${run.frameRange[0]}→${run.frameRange[1]}`);
  check(`降级(${degrade}): presentation 如实回报未授予`,
    run.presentation?.desynchronized === false &&
    (degrade === 'missing' ? run.presentation.powerPreference === null : true),
    JSON.stringify(run.presentation));
  check(`降级(${degrade}): 画面持续变化（非冻结/非黑屏）`, run.distinct >= 2,
    `distinct=${run.distinct}/30`);
  check(`降级(${degrade}): 无错误横幅`, run.errorText === '', JSON.stringify(run.errorText));
  check(`降级(${degrade}): 期望回报形态`, JSON.stringify(run.presentation) === JSON.stringify(expected));
}

await browser.close();
server.close();
console.log(failures ? `RESULT: FAIL (${failures} 项断言失败)` : 'RESULT: PASS (全部断言通过)');
process.exit(failures ? 1 : 0);
