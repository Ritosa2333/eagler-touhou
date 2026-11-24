#!/usr/bin/env node
/**
 * BRO 端口本地开发服务器 —— 用来验证 `bro.html` 的启动/加载耗时。
 *
 * 用法：
 *   node tools/serve-dev.mjs                        # 8801，资源目录自动找 ../TouHou BossRush/game
 *   node tools/serve-dev.mjs --port 8802 --gz 1     # gzip 文本/脚本/wasm，更接近真实网络
 *   node tools/serve-dev.mjs --game "E:/Web/TouHou BossRush/game"
 *
 * 页面：http://127.0.0.1:8801/bro.html?standalone=1
 *   - standalone=1 让 managed-shell 脱离 eagler 启动器自启动（否则停在 waiting for launcher）
 *   - 想跳菜单直进关：&stage=TH18@BossRush
 *   - 无头/无音频设备时加 &audio=force
 *   - 想量真实网络：加 --kbps 2000 --latency 80，用 CDP 限速（见 RUNTIME_ARTIFACTS.md §15）
 */
import { createServer } from 'node:http';
import { createReadStream, existsSync, statSync } from 'node:fs';
import { gzipSync } from 'node:zlib';
import { extname, join, normalize, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = resolve(fileURLToPath(import.meta.url), '..', '..');
const argv = process.argv.slice(2);
const arg = (name, dflt) => {
  const i = argv.indexOf('--' + name);
  return i >= 0 && argv[i + 1] && !argv[i + 1].startsWith('--') ? argv[i + 1] : dflt;
};

const port = Number(arg('port', 8801));
const useGzip = String(arg('gz', '0')) === '1';
const gameDirArg = arg('game', '');
const gameDir = gameDirArg
  ? resolve(gameDirArg)
  : resolve(HERE, '..', '..', '..', '..', 'TouHou BossRush', 'game');

const TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.wasm': 'application/wasm',
  '.txt': 'text/plain; charset=utf-8',
  '.fnt': 'text/plain; charset=utf-8',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.webp': 'image/webp',
  '.ogg': 'audio/ogg',
  '.mp3': 'audio/mpeg',
  '.wav': 'audio/wav',
};
const GZIPPABLE = new Set(['.html', '.mjs', '.js', '.json', '.txt', '.fnt', '.wasm', '.svg']);

// gzip 结果按 文件+mtime 缓存：26MB 的 bro.wasm 每次现压会阻塞事件循环好几秒，
// 真实服务器也是预压好的，这里缓存一次即可。
const gzCache = new Map();

function send(res, code, body, headers = {}) {
  res.writeHead(code, { 'cache-control': 'no-store', ...headers });
  res.end(body);
}

const server = createServer((req, res) => {
  let pathname;
  try {
    pathname = decodeURIComponent(new URL(req.url, 'http://127.0.0.1').pathname);
  } catch {
    return send(res, 400, 'bad request');
  }
  if (pathname === '/') pathname = '/bro.html';

  // 资源包：/game/* → BRO 仓库的 game/ 目录
  let file;
  if (pathname.startsWith('/game/')) {
    file = join(gameDir, normalize(pathname.slice('/game/'.length)));
    if (!file.startsWith(gameDir)) return send(res, 403, 'forbidden');
  } else {
    file = join(HERE, normalize(pathname));
    if (!file.startsWith(HERE)) return send(res, 403, 'forbidden');
  }

  if (!existsSync(file) || !statSync(file).isFile()) {
    if (pathname === '/game/') {
      return send(res, 200, JSON.stringify({ gameDir }), { 'content-type': 'application/json' });
    }
    return send(res, 404, 'not found: ' + pathname);
  }

  const ext = extname(file).toLowerCase();
  const headers = { 'content-type': TYPES[ext] || 'application/octet-stream' };
  if (useGzip && GZIPPABLE.has(ext)) {
    const st = statSync(file);
    const hit = gzCache.get(file);
    if (hit && hit.mtimeMs === st.mtimeMs) {
      return send(res, 200, hit.gz, { ...headers, 'content-encoding': 'gzip', 'content-length': hit.gz.length });
    }
    const raw = [];
    const stream = createReadStream(file);
    stream.on('data', (c) => raw.push(c));
    stream.on('end', () => {
      // 大文件用 level 1：wasm 压缩率本来就低，省 CPU 更接近真实预压件
      const gz = gzipSync(Buffer.concat(raw), { level: st.size > 8 * 1048576 ? 1 : 6 });
      gzCache.set(file, { mtimeMs: st.mtimeMs, gz });
      send(res, 200, gz, { ...headers, 'content-encoding': 'gzip', 'content-length': gz.length });
    });
    stream.on('error', () => send(res, 500, 'read error'));
    return;
  }
  res.writeHead(200, { 'cache-control': 'no-store', ...headers, 'content-length': statSync(file).size });
  createReadStream(file).pipe(res);
});

server.listen(port, '127.0.0.1', () => {
  console.log(`BRO dev server: http://127.0.0.1:${port}/bro.html?standalone=1`);
  console.log(`  静态目录: ${HERE}`);
  console.log(`  /game/ →  ${gameDir}${existsSync(gameDir) ? '' : '  (不存在，请用 --game 指定)'}`);
  console.log(`  gzip: ${useGzip ? 'on' : 'off'}`);
});
