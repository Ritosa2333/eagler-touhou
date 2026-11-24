import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {dirname, extname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

export const HERE = dirname(fileURLToPath(import.meta.url));
export const PORT_ROOT = resolve(HERE, '..', '..', '..');
export const REPO_ROOT = resolve(PORT_ROOT, '..', '..');
export const WORKSPACE = resolve(PORT_ROOT, '..', '..', '..');

const RUNTIME_DIR = join(PORT_ROOT, 'web/sdl-runtime');
const BENCH_DIR = join(HERE, 'bench');
const BUILD_DIR = join(PORT_ROOT, 'web/artifacts');
const SP_BUILD_DIR = join(REPO_ROOT, 'ports/th13/web/artifacts');
const RAW_DIR = join(WORKSPACE, 'artifacts/reference/raw');
const REPLAYS_DIR = join(WORKSPACE, 'games/replays');

const MIME = {
  '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.mjs': 'text/javascript',
  '.wasm': 'application/wasm', '.png': 'image/png', '.json': 'application/json',
  '.anm': 'application/octet-stream', '.ecl': 'application/octet-stream',
  '.rpy': 'application/octet-stream', '.sht': 'application/octet-stream',
};

export async function startServe({host = '127.0.0.1'} = {}) {
  const routes = [
    ['/bench/', BENCH_DIR, 'bench'],
    ['/assets/', join(RUNTIME_DIR, 'assets'), 'assets'],
    ['/sp/artifacts/', SP_BUILD_DIR, 'sp-artifacts'],
    ['/artifacts/', BUILD_DIR, 'artifacts'],
    ['/replays/', REPLAYS_DIR, 'replays'],
    ['/', RUNTIME_DIR, 'runtime'],
  ];
  const server = createServer(async (request, response) => {
    try {
      const path = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
      let dir = null, rest = path, label = null;
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
