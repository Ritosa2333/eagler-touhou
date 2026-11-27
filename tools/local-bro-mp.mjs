// 完整 launcher 路线：本机模拟 BRO(bromp) 联机的一键助手。
// 用法：node tools/local-bro-mp.mjs [--skip-prepare] [--web=8130] [--relay=18142]
//
// 它做四件事：
//   1) 准备前置：补齐 sparse-checkout 缺的 motion-replay.mjs、装 launcher 依赖、
//      构建 React 前端、把 BRO web 产物摆位到 runtime/bro/{,multiplayer/}。
//   2) 起本地 netplay relay（ws://127.0.0.1:18142）。
//   3) 起 launcher dev server（http://127.0.0.1:8130/?test），manifest 已注入 relay。
//   4) 打印房间操作步骤。Ctrl+C 一起收尾。
//
// 前置只需要：Node>=22、bun 在 PATH（构建 UI 用）、同机有 BRO web 产物
// （ports/bossrush/web/，已在 eagler 仓内）。游戏资源首次进关需在 UI 里导入
// bro-import-package.zip（见打印的指引）。--skip-prepare 跳过第 1 步（已准备好时）。
import { spawn, execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, copyFileSync, cpSync, writeFileSync } from 'node:fs';
import { resolve, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const arg = (name, fallback) => {
  const hit = process.argv.find((v) => v.startsWith(`--${name}=`));
  return hit ? hit.slice(name.length + 3) : fallback;
};
const WEB_PORT = arg('web', '8130');
const RELAY_PORT = arg('relay', '18142');
const SKIP_PREPARE = process.argv.includes('--skip-prepare');
const kids = [];

function run(label, cmd, args, opts = {}) {
  const p = spawn(cmd, args, { cwd: ROOT, stdio: ['ignore', 'pipe', 'pipe'], ...opts });
  p.stdout.on('data', (d) => process.stdout.write(`[${label}] ${d}`));
  p.stderr.on('data', (d) => process.stderr.write(`[${label}] ${d}`));
  kids.push(p);
  return p;
}
const sh = (cmd, args, opts = {}) =>
  execFileSync(cmd, args, { cwd: ROOT, encoding: 'utf8', stdio: 'pipe', ...opts });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

async function waitHttp(url, test = (d) => d && d.ok) {
  for (let i = 0; i < 60; i++) {
    try {
      const res = await fetch(url);
      if (res.ok) {
        const data = await res.json().catch(() => null);
        if (test(data)) return data;
      }
    } catch { /* 还没起来 */ }
    await sleep(150);
  }
  return null;
}

function prepare() {
  console.log('\n[prepare] 前置准备开始（--skip-prepare 可跳过）');
  // 1) sparse-checkout 缺文件：UI 静态 import 它。
  const motion = 'ports/th09/web/sdl-runtime/motion-replay.mjs';
  if (!existsSync(resolve(ROOT, motion))) {
    mkdirSync(dirname(resolve(ROOT, motion)), { recursive: true });
    writeFileSync(resolve(ROOT, motion), sh('git', ['show', `HEAD:${motion}`]));
    console.log(`[prepare] 已恢复 ${motion}`);
  }
  // 2) 前端依赖 + UI 构建（缺字体包会导致构建失败）。
  try {
    sh('bun', ['install', '--frozen-lockfile'], { cwd: resolve(ROOT, 'launcher') });
    sh('bun', ['--bun', 'run', 'build:ui-rewrite'], { cwd: resolve(ROOT, 'launcher') });
    console.log('[prepare] React UI 构建完成');
  } catch (e) {
    console.error('[prepare] UI 构建失败，确认 bun 在 PATH：', String(e.message || e).slice(0, 300));
    process.exit(1);
  }
  // 3) BRO web 产物摆位到 runtime/bro/ 与 multiplayer 子目录。
  const web = resolve(ROOT, 'ports/bossrush/web');
  const rt = resolve(ROOT, 'runtime/bro');
  if (!existsSync(resolve(web, 'bro.wasm'))) {
    console.error('[prepare] 缺 ports/bossrush/web/bro.wasm，请先同步 BRO 产物');
    process.exit(1);
  }
  mkdirSync(resolve(rt, 'multiplayer'), { recursive: true });
  cpSync(web, rt, { recursive: true, force: true });
  for (const f of ['bro.html', 'shell.mjs', 'managed-shell.mjs', 'bro.mjs', 'bro.wasm']) {
    copyFileSync(resolve(web, f), resolve(rt, 'multiplayer', f));
  }
  cpSync(resolve(web, 'dist'), resolve(rt, 'multiplayer/dist'), { recursive: true, force: true });
  cpSync(resolve(web, 'src'), resolve(rt, 'multiplayer/src'), { recursive: true, force: true });
  console.log('[prepare] runtime/bro/{,multiplayer/} 摆位完成\n');
}

async function main() {
  if (!SKIP_PREPARE) prepare();

  // relay（WS 无 /health，用端口探测代替）
  run('relay', process.execPath, ['services/relay/src/netplay-relay.mjs'], {
    env: {
      ...process.env,
      EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
      EAGLER_NETPLAY_RELAY_PORT: RELAY_PORT,
      EAGLER_NETPLAY_STUN_URLS: '',
      EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '0',
    },
  });

  // dev server
  run('web', process.execPath, ['launcher/scripts/serve.mjs', WEB_PORT], {
    env: {
      ...process.env,
      EAGLER_DEVELOPMENT_SITE_MANIFEST: 'ports/bossrush/local-dev-site-manifest.json',
      EAGLER_TOUHOU_ARTWORK_DIR: 'games/host-artwork',
      EAGLER_TOUHOU_NETPLAY_RELAY: `ws://127.0.0.1:${RELAY_PORT}`,
    },
  });
  // manifest 对象没有 ok 字段，用 games 字典判定就绪。
  const manifest = await waitHttp(
    `http://127.0.0.1:${WEB_PORT}/site-manifest.json`,
    (d) => d && d.games && typeof d.games === 'object',
  );
  if (!manifest) {
    console.error('[web] dev server 启动失败，检查端口占用 / UI 是否构建');
    shutdown();
    return;
  }
  console.log('[web] dev server 就绪，relay =', manifest.shared?.netplayRelay);

  const base = `http://127.0.0.1:${WEB_PORT}`;
  console.log('\n=== 完整 launcher 本机联机（bromp）===');
  console.log(`入口：${base}/?test`);
  console.log('两个标签页（localhost 下用 sessionStorage 隔离，不必双 profile）：');
  console.log(`  P1: ${base}/?test`);
  console.log(`  P2: ${base}/?test\n`);
  console.log('操作步骤：');
  console.log('  1. 首次进游戏若提示导入资源，导入 bro-import-package.zip（183MB，含全量游戏素材）。');
  console.log('  2. 进「联机大厅」：P1 点创建房间，游戏选 bromp，容量 2，难度 Vanilla，可见性私有。');
  console.log('  3. 记下 4 位房间码；P2 用「输入房间码」加入。');
  console.log('  4. 两端都准备好后，房主点开始游戏；route 默认 rtc（host candidate 直连）。');
  console.log('  5. 游戏内：P1 用方向键移动，观察 P2 屏幕里 P1 是否同步移动。');
  console.log('\n提示：relay 与 BRO 网页 signaling.mjs 是两套协议，launcher 路线必须用本脚本起的 relay。');
  console.log('Ctrl+C 同时关闭 relay 与 dev server。\n');
}

const shutdown = () => {
  for (const p of kids) if (!p.killed) p.kill();
  process.exit(0);
};
process.on('SIGINT', shutdown);
process.on('SIGTERM', shutdown);

main().catch((e) => {
  console.error('启动失败：', e);
  shutdown();
});
