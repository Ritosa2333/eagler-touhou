#!/usr/bin/env node
/**
 * 启动耗时探针 —— 用 headless Edge + CDP 量 `bro.html` 从导航到首帧的时间与网络量。
 *
 * 先起 `node tools/serve-dev.mjs --gz 1`，再：
 *   node tools/load-probe.mjs                         # 本机不限速
 *   node tools/load-probe.mjs --kbps 2000 --latency 80  # 2 MB/s + 80ms，逼近真实家庭宽带
 *   node tools/load-probe.mjs --url "bro.html?standalone=1&stage=TH11@BossRush" --settle 4000 --shot out.png
 *
 * 输出一行 JSON：toRunningMs / toFirstFrameMs / requests / megabytes / tex / console 错误。
 * 注意：headless 下 WebGL 走 swiftshader，帧率不代表真机；只看加载时序与网络量。
 */
import { spawn } from 'node:child_process';
import { existsSync, mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const argv = process.argv.slice(2);
const arg = (name, dflt) => {
  const i = argv.indexOf('--' + name);
  return i >= 0 && argv[i + 1] && !argv[i + 1].startsWith('--') ? argv[i + 1] : dflt;
};

const PORT = Number(arg('port', 8801));
const PAGE = arg('url', 'bro.html?standalone=1&audio=force');
const KBPS = Number(arg('kbps', 0));
const LATENCY = Number(arg('latency', 0));
const SETTLE = Number(arg('settle', 1500));
const SHOT = arg('shot', '');
const LIMIT_MS = Number(arg('limit', 180000));

const EDGE_CANDIDATES = [
  'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
  'C:/Program Files/Microsoft/Edge/Application/msedge.exe',
  'C:/Program Files/Google/Chrome/Application/chrome.exe',
];
const BROWSER = EDGE_CANDIDATES.find((p) => existsSync(p));
if (!BROWSER) {
  console.error('找不到 Edge/Chrome 可执行文件');
  process.exit(1);
}

const profile = mkdtempSync(join(tmpdir(), 'bro-load-'));
const CDP_PORT = 9000 + Math.floor(Math.random() * 900);
const child = spawn(
  BROWSER,
  [
    '--headless=new',
    '--disable-gpu-sandbox',
    '--use-gl=angle',
    '--use-angle=swiftshader',
    '--no-first-run',
    '--no-default-browser-check',
    '--disable-features=Translate',
    '--autoplay-policy=no-user-gesture-required',
    `--remote-debugging-port=${CDP_PORT}`,
    `--user-data-dir=${profile}`,
    'about:blank',
  ],
  { stdio: 'ignore', env: { ...process.env, TEMP: profile, TMP: profile } },
);

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

async function wsUrl(retries = 60) {
  for (let i = 0; i < retries; i++) {
    try {
      const res = await fetch(`http://127.0.0.1:${CDP_PORT}/json/list`);
      const list = await res.json();
      const page = list.find((t) => t.type === 'page');
      if (page?.webSocketDebuggerUrl) return page.webSocketDebuggerUrl;
    } catch {
      /* 还没起来 */
    }
    await sleep(200);
  }
  throw new Error('CDP 未就绪');
}

function cleanup(code) {
  try {
    child.kill('SIGKILL');
  } catch {
    /* 忽略 */
  }
  try {
    rmSync(profile, { recursive: true, force: true });
  } catch {
    /* 忽略 */
  }
  process.exit(code);
}

const ws = new WebSocket(await wsUrl());
await new Promise((res, rej) => {
  ws.addEventListener('open', res, { once: true });
  ws.addEventListener('error', rej, { once: true });
});

let nextId = 1;
const pending = new Map();
const events = [];
ws.addEventListener('message', (ev) => {
  const msg = JSON.parse(ev.data);
  if (msg.id && pending.has(msg.id)) {
    const { res, rej } = pending.get(msg.id);
    pending.delete(msg.id);
    msg.error ? rej(new Error(msg.error.message)) : res(msg.result);
  } else if (msg.method) {
    events.push(msg);
  }
});
const send = (method, params = {}) =>
  new Promise((res, rej) => {
    const id = nextId++;
    pending.set(id, { res, rej });
    ws.send(JSON.stringify({ id, method, params }));
  });

await send('Page.enable');
await send('Runtime.enable');
await send('Network.enable');
if (KBPS > 0) {
  await send('Network.emulateNetworkConditions', {
    offline: false,
    latency: LATENCY,
    // --kbps 是「千字节每秒」（2000 = 2 MB/s），与 CDP 的 bytes/s 直接换算
    downloadThroughput: Math.round(KBPS * 1024),
    uploadThroughput: Math.round(KBPS * 1024),
    connectionType: 'cellular3g',
  });
}

const t0 = Date.now();
await send('Page.navigate', { url: `http://127.0.0.1:${PORT}/${PAGE}` });

// 等 shell 就绪后自己发 launch（standalone 模式必须，否则停在 waiting for launcher）
const launchJs = `(() => {
  const shell = window.__broShell;
  if (!shell) return 'no-shell';
  if (window.__broLoadLaunched) return 'already';
  window.__broLoadLaunched = true;
  const msg = { protocol: 'eagler-touhou/1', game: 'bro', epoch: 1, command: 'launch' };
  const t = shell.messageTarget || window;
  t.postMessage(msg, '*');
  return 'sent';
})()`;

let launchAt = 0;
let state = '';
let frames = 0;
let tex = null;
let firstFrameAt = 0;
const errors = [];

while (Date.now() - t0 < LIMIT_MS) {
  await sleep(150);
  if (!launchAt) {
    try {
      const r = await send('Runtime.evaluate', { expression: launchJs, returnByValue: true });
      if (r.result?.value === 'sent' || r.result?.value === 'already') launchAt = Date.now() - t0;
    } catch {
      /* 页面还没加载完 */
    }
  }
  try {
    const r = await send('Runtime.evaluate', {
      expression: `(() => {
        const rt = window.__broRuntime || null;
        const rd = window.__broRenderer || null;
        return JSON.stringify({
          state: (rt && rt.state) || '',
          frames: (rt && rt.frames) || 0,
          tex: rd && rd.stats ? rd.stats.textures : null,
        });
      })()`,
      returnByValue: true,
    });
    const v = JSON.parse(r.result?.value || '{}');
    state = v.state;
    frames = Number(v.frames) || 0;
    tex = v.tex;
    if (frames > 0) {
      firstFrameAt = Date.now() - t0;
      break;
    }
  } catch {
    /* 上下文重建中 */
  }
}

for (const e of events) {
  if (e.method === 'Runtime.exceptionThrown') {
    errors.push(String(e.params?.exceptionDetails?.exception?.description || '').split('\n')[0]);
  }
}

// 采样稳定后状态
await sleep(SETTLE);
let settled = null;
let requests = 0;
let megabytes = 0;
try {
  const r = await send('Runtime.evaluate', {
    expression: `(() => {
      const rd = window.__broRenderer || null;
      return JSON.stringify({
        state: (window.__broRuntime && window.__broRuntime.state) || '',
        frames: (window.__broRuntime && window.__broRuntime.frames) || 0,
        fps: (window.__broRuntime && window.__broRuntime.fps) || 0,
        tex: rd && rd.stats ? rd.stats.textures : null,
      });
    })()`,
    returnByValue: true,
  });
  settled = JSON.parse(r.result?.value || 'null');
} catch {
  /* 忽略 */
}
for (const e of events) {
  if (e.method === 'Network.responseReceived') requests++;
  if (e.method === 'Network.loadingFinished') {
    megabytes += (e.params?.encodedDataLength || 0) / 1048576;
  }
}

if (SHOT) {
  try {
    const shot = await send('Page.captureScreenshot', { format: 'png' });
    writeFileSync(SHOT, Buffer.from(shot.data, 'base64'));
  } catch {
    /* 忽略 */
  }
}

console.log(
  JSON.stringify(
    {
      url: PAGE,
      kbps: KBPS || 'unlimited',
      launchAtMs: launchAt,
      toRunningMs: state === 'running' ? firstFrameAt : null,
      toFirstFrameMs: firstFrameAt || null,
      requests,
      megabytes: Math.round(megabytes * 10) / 10,
      settled,
      errors,
      shot: SHOT || null,
    },
    null,
    1,
  ),
);

ws.close();
cleanup(0);
