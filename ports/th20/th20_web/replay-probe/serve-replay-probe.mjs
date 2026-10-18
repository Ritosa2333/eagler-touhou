// Static server for the TH20 deterministic replay probe.
//
// Serves the probe directory plus the two files the browser runtime needs but
// which are not part of the repository: the retail th20.dat archive and a
// system font. Read-only.
//
//   node th20_web/replay-probe/serve-replay-probe.mjs [--port 8791]
//        [--root <probe dir>] [--game "<TH20 install dir>"] [--font <ttc path>]
//
// Then open http://127.0.0.1:8791/replay-probe.html?build=new
import { createServer } from 'node:http';
import { createReadStream, statSync, existsSync } from 'node:fs';
import { extname, join, normalize, resolve, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const args = process.argv.slice(2);
const arg = (name, fallback) => {
  const i = args.indexOf(name);
  return i >= 0 && args[i + 1] ? args[i + 1] : fallback;
};
const ROOT = resolve(arg('--root', here));
const PORT = Number(arg('--port', '8791'));
const GAME = arg('--game', 'C:/Program Files (x86)/上海アリス幻樂団/東方錦上京');
const FONT = arg('--font', 'C:/Windows/Fonts/msgothic.ttc');
const REPLAY = arg('--replay', resolve(here, '..', '..', '..', 'th20_02.rpy'));
// Build outputs are not committed; serve the newest artifact directly when the
// probe directory has no local copy.
const ARTIFACTS = resolve(here, '..', 'artifacts', 'sdl3');

const extra = { '/th20.dat': join(GAME, 'th20.dat'), '/msgothic.ttc': FONT, '/th20_02.rpy': REPLAY };
for (const name of ['th20-sdl.mjs', 'th20-sdl.wasm'])
  if (!existsSync(join(ROOT, name))) extra['/' + name] = join(ARTIFACTS, name);
for (const [url, path] of Object.entries(extra))
  if (!existsSync(path)) throw new Error(`required file missing for ${url}: ${path}`);

const types = {
  '.html': 'text/html', '.mjs': 'text/javascript', '.js': 'text/javascript',
  '.wasm': 'application/wasm', '.json': 'application/json',
  '.rpy': 'application/octet-stream', '.dat': 'application/octet-stream', '.ttc': 'font/collection'
};

createServer((req, res) => {
  const url = decodeURIComponent((req.url ?? '/').split('?')[0]);
  const path = extra[url] ?? join(ROOT, normalize(url).replace(/^([/\\.])+/, ''));
  try {
    const st = statSync(path);
    if (!st.isFile()) throw new Error('not a file');
    res.writeHead(200, {
      'content-type': types[extname(path)] ?? 'application/octet-stream',
      'content-length': st.size, 'cache-control': 'no-store'
    });
    createReadStream(path).pipe(res);
  } catch {
    res.writeHead(404, { 'content-type': 'text/plain' });
    res.end('not found: ' + url);
  }
}).listen(PORT, '127.0.0.1', () => console.log(`replay probe on http://127.0.0.1:${PORT}/replay-probe.html`));
