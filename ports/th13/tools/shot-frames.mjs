import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import {readFile, readdir, writeFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve, dirname, join, extname} from 'node:path';
import {fileURLToPath} from 'node:url';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../../..');
const runtimeDir = join(port, 'web/sdl-runtime');
const buildDir = join(port, 'web/artifacts');
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
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}

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

const reportFrame = option('report-frame', null);
if (reportFrame !== null) {
  const consumed = await page.evaluate('window.__th13_debug.consumed()');
  if (Number(reportFrame) + 1 > consumed) await capture(page, Number(reportFrame));
  console.log(await page.evaluate('window.__th13_debug.report()'));
  console.log(await page.evaluate('window.__th13_debug.stageReport()'));
}

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

const sideFrames = (option('side-by-side', null) ?? '0,61,121,901,1242,1671,2381,2450,5494,5806,5856,5867,5890,5919')
    .split(',').map(Number)
    .filter(frame => existsSync(join(goldenDir, `frame_${String(frame).padStart(6, '0')}.png`)));
const composePage = await browser.newPage();
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
