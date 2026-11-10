// MP 工作流浏览器层静态服务（M1/T8 测试作者）。
//
// 配方复用 ports/th13-mp/tools/shot-frames.mjs:43-82 的路由表（不自造服务面）：
//   /assets/<file>        → web/sdl-runtime/assets（SP 228 件共享资产；T1 retarget 符号链接）
//   /assets/<name>.sht    → 外层 artifacts/reference/raw（移速源不落 runtime/assets）
//   /artifacts/<file>     → web/artifacts（sdl3 构建产物：th13-sdl.mjs/.wasm/build.json）
//   /replays/<file>       → 外层 games/replays（th13_01.rpy 等合法私有资产）
//   /<file>               → web/sdl-runtime（th13-debug.html 及页内模块）
// 仅 127.0.0.1 回环监听、只读固定目录、路径穿越拒绝（resolve 前缀校验同配方）。
//
// 用法（库）：
//   import {startServe} from './serve.mjs';
//   const server = await startServe();  // {origin, port, close()}
// 用法（独立诊断）：
//   node tests/multiplayer/browser/serve.mjs   # 打印 origin 后常驻到 SIGINT
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {dirname, extname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

export const HERE = dirname(fileURLToPath(import.meta.url));   // tests/multiplayer/browser
export const PORT_ROOT = resolve(HERE, '..', '..', '..');      // ports/th13-mp
export const REPO_ROOT = resolve(PORT_ROOT, '..', '..');       // eagler-touhou
// 容器内 /work/eagler-touhou/ports/th13-mp → 外层工作区根 /work（宿主直跑则为仓库根，
// 与 shot-frames.mjs:20-25 的 workspace 推导同式，port 根起三级上跳）。
export const WORKSPACE = resolve(PORT_ROOT, '..', '..', '..');

const RUNTIME_DIR = join(PORT_ROOT, 'web/sdl-runtime');
const BUILD_DIR = join(PORT_ROOT, 'web/artifacts');
const RAW_DIR = join(WORKSPACE, 'artifacts/reference/raw');
const REPLAYS_DIR = join(WORKSPACE, 'games/replays');

// MIME 与 shot-frames.mjs:37-42 一致。
const MIME = {
  '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.mjs': 'text/javascript',
  '.wasm': 'application/wasm', '.png': 'image/png', '.json': 'application/json',
  '.anm': 'application/octet-stream', '.ecl': 'application/octet-stream',
  '.rpy': 'application/octet-stream', '.sht': 'application/octet-stream',
};

export async function startServe({host = '127.0.0.1'} = {}) {
  const routes = [
    ['/assets/', join(RUNTIME_DIR, 'assets'), 'assets'],
    ['/artifacts/', BUILD_DIR, 'artifacts'],
    ['/replays/', REPLAYS_DIR, 'replays'],
    ['/', RUNTIME_DIR, 'runtime'],
  ];
  const server = createServer(async (request, response) => {
    try {
      const path = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
      let dir = null, rest = path, label = null;
      // shot-frames.mjs:57-60 的唯一例外：/assets/*.sht 出自 raw 解包目录。
      if (path.startsWith('/assets/') && path.endsWith('.sht')) {
        dir = RAW_DIR;
        rest = path.slice('/assets/'.length);
        label = 'raw-sht';
      } else {
        for (const [prefix, root, name] of routes) {
          if (!path.startsWith(prefix)) continue;
          dir = root;
          rest = path.slice(prefix.length);
          label = name;
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
      response.writeHead(404).end(`not found: ${path} (route=${label ?? 'none'})`);
    } catch (error) {
      response.writeHead(404).end('not found');
      void error;
    }
  });
  await new Promise(done => server.listen(0, host, done));
  return {
    origin: `http://${host}:${server.address().port}`,
    port: server.address().port,
    routes: routes.map(([prefix, root]) => `${prefix} -> ${root}`),
    close: () => new Promise(done => server.close(done)),
  };
}

// 独立诊断入口：node tests/multiplayer/browser/serve.mjs [--until <ms>]
const invokedDirectly = process.argv[1] &&
  import.meta.url === new URL(`file://${resolve(process.argv[1])}`).href;
if (invokedDirectly) {
  const until = (() => {
    const at = process.argv.indexOf('--until');
    return at >= 0 ? Number(process.argv[at + 1]) : 0;
  })();
  const server = await startServe();
  console.log(`serve: ${server.origin}`);
  for (const line of server.routes) console.log('  ' + line);
  if (until > 0) {
    setTimeout(() => process.exit(0), until);
  } else {
    process.on('SIGINT', () => process.exit(0));
  }
}
