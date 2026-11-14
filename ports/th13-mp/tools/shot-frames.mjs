// 移植侧同帧取像驱动（th13-shot 镜像内运行）：
//   1. 本地 http 服务：sdl-runtime 页面/资产 + sdl3 构建产物 + 录像 + golden 参考帧；
//   2. chromium（playwright-core，executablePath=$CHROMIUM_PATH）打开
//      th13-debug.html——回放会话直入对局，th13_debug_frame 确定性 60fps 帧泵；
//   3. 按 golden 目录 frame_%06d.png 的游戏帧号逐帧泵到目标并导出 PNG
//      （ours-frame-%06d.png，与参考帧同目录并排存放）；
//   4. 确定性自检：第二个全新会话重泵校验帧，dataURL 逐字节一致方为绿；
//   5. side-by-side.png：页面内 canvas 合成（左参考帧/右移植，同游戏帧并排）。
// 用法（工作区根）：
//   make shot CMD='node eagler-touhou/ports/th13/tools/shot-frames.mjs'
//   可选：--frames 0,61,121 --check-frame 1242 --replay th13_01.rpy --sht pl00a.sht
//         --stage 0 --side-by-side 0,61,121,901,1242,1671,2381,2450,5494,5806,5856,5867,5890,5919
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import {readFile, readdir, writeFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve, dirname, join, extname} from 'node:path';
import {fileURLToPath} from 'node:url';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../../..');
const runtimeDir = join(port, 'web/sdl-runtime');
const buildDir = join(port, 'web/artifacts');  // 页面自 /artifacts/sdl3/ 取构建产物
const goldenDir = join(workspace, 'artifacts/reference/golden');
const replaysDir = join(workspace, 'games/replays');

const args = process.argv.slice(2);
const option = (name, fallback) => {
  const at = args.indexOf('--' + name);
  return at >= 0 && args[at + 1] ? args[at + 1] : fallback;
};
const replay = option('replay', 'th13_01.rpy');
const sht = option('sht', 'pl00a.sht');
const stage = Number(option('stage', '0'));
const checkFrame = Number(option('check-frame', '1242'));

const MIME = {
  '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.mjs': 'text/javascript',
  '.wasm': 'application/wasm', '.png': 'image/png', '.json': 'application/json',
  '.anm': 'application/octet-stream', '.ecl': 'application/octet-stream',
  '.rpy': 'application/octet-stream', '.sht': 'application/octet-stream',
};
// 路由（URL 前缀 → 本地目录）。仅本机回环监听，只读固定目录；
// /assets/*.sht 例外：移速源出自 raw 解包目录（不落 sdl-runtime/assets）。
const rawDir = join(workspace, 'artifacts/reference/raw');
const routes = [
  ['/assets/', join(runtimeDir, 'assets')],
  ['/artifacts/', buildDir],
  ['/replays/', replaysDir],
  ['/golden/', goldenDir],
  ['/', runtimeDir],
];
const server = createServer(async (request, response) => {
  try {
    const path = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
    let dir = null, rest = path;
    if (path.startsWith('/assets/') && path.endsWith('.sht')) {
      dir = rawDir;
      rest = path.slice('/assets/'.length);
    } else {
      for (const [prefix, root] of routes) {
        if (!path.startsWith(prefix)) continue;
        dir = root;
        rest = path.slice(prefix.length);
        break;
      }
    }
    if (dir) {
      const file = resolve(dir, rest);
      if (file.startsWith(resolve(dir))) {
        const bytes = await readFile(file);
        response.writeHead(200, {'Content-Type': MIME[extname(file)] ?? 'application/octet-stream'});
        response.end(bytes);
        return;
      }
    }
    response.writeHead(404).end('not found');
  } catch {
    response.writeHead(404).end('not found');
  }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
const origin = `http://127.0.0.1:${server.address().port}`;

const require = createRequire(import.meta.url);
let chromium;
try {
  ({chromium} = require('playwright-core'));
} catch {
  // NODE_PATH 未生效时显式落点（镜像内 /opt/shot/node_modules）
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}

// golden 帧号清单：目录内 frame_%06d.png 的数字（= 游戏帧号）。
const framesOf = async () => {
  const names = await readdir(goldenDir);
  const frames = names.filter(n => /^frame_\d{6}\.png$/.test(n))
      .map(n => Number(n.slice(6, 12))).sort((a, b) => a - b);
  if (!frames.length) throw new Error('golden 目录无 frame_*.png：' + goldenDir);
  return frames;
};
const wanted = option('frames', null)
    ? option('frames', '').split(',').map(Number).sort((a, b) => a - b)
    : await framesOf();

const browser = await chromium.launch({
  executablePath: process.env.CHROMIUM_PATH,
  headless: true,
  args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
         '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
});
const consoleLines = [];
const openSession = async () => {
  const page = await browser.newPage({viewport: {width: 800, height: 560}});
  page.on('console', message => consoleLines.push(message.type() + ' ' + message.text()));
  page.on('pageerror', error => consoleLines.push('pageerror ' + error.message));
  await page.goto(`${origin}/th13-debug.html?replay=${encodeURIComponent(replay)}` +
                  `&sht=${encodeURIComponent(sht)}&stage=${stage}`, {waitUntil: 'domcontentloaded'});
  await page.waitForFunction('!!window.__th13_debug || !!window.__th13_debug_failed',
                             null, {timeout: 120000});
  const failed = await page.evaluate('window.__th13_debug_failed ?? null');
  if (failed) throw new Error('调试页启动失败：' + failed + '\n' + consoleLines.slice(-20).join('\n'));
  const total = await page.evaluate('window.__th13_debug.total');
  console.log(`session ready ${replay} stage=${stage} total=${total}`);
  return page;
};
const capture = (page, frame) =>
    page.evaluate(f => window.__th13_debug.pumpTo(f), frame);

const page = await openSession();
const firstCapture = new Map();
const began = Date.now();
for (const frame of wanted) {
  const dataUrl = await capture(page, frame);
  if (!dataUrl.startsWith('data:image/png;base64,')) throw new Error('帧 ' + frame + ' 取像异常');
  firstCapture.set(frame, dataUrl);
  await writeFile(join(goldenDir, `ours-frame-${String(frame).padStart(6, '0')}.png`),
                  Buffer.from(dataUrl.slice(22), 'base64'));
  console.log(`ours frame ${frame} exported (${Math.round((Date.now() - began) / 1000)}s)`);
}

// 画面诊断：泵到指定帧后倾倒 33 层与表现层 VM 清单。置于导出循环之后——
// 中途的 evaluate 会让 WebGL 后备缓冲被合成清空，再取像即黑帧。
const reportFrame = option('report-frame', null);
if (reportFrame !== null) {
  // 只向前泵（导出循环可能已越过报告帧，此时直接读当前画面状态）。
  const consumed = await page.evaluate('window.__th13_debug.consumed()');
  if (Number(reportFrame) + 1 > consumed) await capture(page, Number(reportFrame));
  console.log(await page.evaluate('window.__th13_debug.report()'));
  console.log(await page.evaluate('window.__th13_debug.stageReport()'));
}

// 确定性自检：全新会话重泵校验帧，画面逐字节一致。
if (!firstCapture.has(checkFrame)) firstCapture.set(checkFrame, await capture(page, checkFrame));
const verifyPage = await openSession();
const verifyUrl = await capture(verifyPage, checkFrame);
const deterministic = verifyUrl === firstCapture.get(checkFrame);
console.log(`DETERMINISM frame=${checkFrame} match=${deterministic ? 'yes' : 'no'}`);
if (!deterministic) {
  await writeFile(join(goldenDir, `ours-frame-${String(checkFrame).padStart(6, '0')}-verify.png`),
                  Buffer.from(verifyUrl.slice(22), 'base64'));
}
await verifyPage.close();

// side-by-side：页面内合成（左 golden 参考帧 / 右移植帧，同游戏帧并排）。
const sideFrames = (option('side-by-side', null) ?? '0,61,121,901,1242,1671,2381,2450,5494,5806,5856,5867,5890,5919')
    .split(',').map(Number)
    // 参考帧缺失的帧号不入并排（golden 只有关键帧与 1非 连续段）。
    .filter(frame => existsSync(join(goldenDir, `frame_${String(frame).padStart(6, '0')}.png`)));
const composePage = await browser.newPage();
// 合成页只需要同源上下文：404 页即可，不装载游戏运行时。
await composePage.goto(origin + '/__compose', {waitUntil: 'domcontentloaded'}).catch(() => {});
const sideUrl = await composePage.evaluate(async frames => {
  const W = 640, H = 480, LABEL = 26, GAP = 10;
  const load = src => new Promise((done, fail) => {
    const image = new Image();
    image.onload = () => done(image);
    image.onerror = () => fail(new Error('load ' + src));
    image.src = src;
  });
  const pairs = [];
  for (const frame of frames) {
    const name = String(frame).padStart(6, '0');
    pairs.push([frame, await load(`/golden/frame_${name}.png`),
                await load(`/golden/ours-frame-${name}.png`)]);
  }
  const columns = 2, rows = Math.ceil(pairs.length / columns);
  const canvas = document.createElement('canvas');
  canvas.width = columns * (2 * W + GAP) + GAP;
  canvas.height = 40 + rows * (H + LABEL + GAP) + GAP;
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = '#181820';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  ctx.font = '18px system-ui, sans-serif';
  ctx.fillStyle = '#ddd';
  ctx.fillText('左：参考（速通视频） 右：移植（回放会话） — 同游戏帧并排', GAP, 27);
  pairs.forEach(([frame, golden, ours], index) => {
    const column = index % columns, row = Math.floor(index / columns);
    const x = GAP + column * (2 * W + GAP), y = 40 + row * (H + LABEL + GAP);
    ctx.fillStyle = '#666';
    ctx.font = '16px system-ui, sans-serif';
    ctx.fillText('游戏帧 ' + frame, x, y + 17);
    ctx.drawImage(golden, x, y + LABEL);
    // 几何适配（golden 校准勘误）：golden=游戏 x∈[97,640]（544 宽），ours 裁
    // x∈[98,640] 后绘制，两侧画格逐像素对齐（±1px 残差）。
    ctx.drawImage(ours, 98, 0, 542, 480, x + W, y + LABEL, 542, 480);
  });
  return canvas.toDataURL('image/png');
}, sideFrames);
await writeFile(join(goldenDir, 'side-by-side.png'), Buffer.from(sideUrl.slice(22), 'base64'));
console.log(`side-by-side.png composed: ${sideFrames.length} 帧 × 左右对照`);
await composePage.close();

await page.close();
await browser.close();
server.close();
if (!deterministic) {
  console.error('确定性自检未通过（见 ours-frame-*-verify.png）');
  process.exit(1);
}
console.log(`SHOT ok frames=${wanted.length} deterministic=yes out=${goldenDir}`);
