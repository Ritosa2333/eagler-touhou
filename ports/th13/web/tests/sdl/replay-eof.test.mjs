import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const workspace = resolve(port, '../../..');
const runtime = join(port, 'web/sdl-runtime');
const build = process.env.TH13_SDL3_BUILD_DIR ?? join(port, 'web/artifacts');
const fixture = process.env.TH13_EOF_REPLAY ?? join(workspace,
  'artifacts/verification/product-20261026/replay-end/eof-stage1.synthetic.rpy');
const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

test('Replay EOF freezes gameplay and accepts physical title-menu input', {timeout: 180000}, async () => {
  const errors = [];
  const server = createServer(async (request, response) => {
    try {
      const pathname = new URL(request.url, 'http://localhost').pathname;
      const file = pathname.startsWith('/artifacts/')
        ? join(build, pathname.slice('/artifacts/'.length))
        : join(runtime, pathname.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      const type = file.endsWith('.mjs') ? 'text/javascript' : file.endsWith('.wasm')
        ? 'application/wasm' : file.endsWith('.html') ? 'text/html' : 'application/octet-stream';
      response.writeHead(200, {'Content-Type': type}).end(data);
    } catch { response.writeHead(404).end(); }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader', '--force-color-profile=srgb']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}});
    page.on('pageerror', error => errors.push(String(error)));
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
      {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 120000});
    const bytes = [...new Uint8Array(await readFile(fixture))];
    const result = await page.evaluate(bytes => {
      const mod = window.mod;
      mod._th13_loop_stop();
      mod.FS.writeFile('/assets/eof.synthetic.rpy', new Uint8Array(bytes));
      const name = mod.allocateUTF8('eof.synthetic.rpy');
      try {
        const frames = mod._th13_debug_replay(name, 0);
        const first = mod._th13_debug_frame();
        const eof = mod._th13_debug_frame();
        const read = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4,
          mod._th13_probe_state() / 4 + 16)];
        const frozen = read();
        for (let i = 0; i < 14; ++i) assertFrame();
        mod._th13_key(1, 1);
        assertFrame();
        mod._th13_key(1, 0);
        for (let i = 0; i < 20; ++i) assertFrame();
        const afterEscape = read();
        const tap = scan => {
          mod._th13_key(scan, 1); assertFrame();
          mod._th13_key(scan, 0); assertFrame();
        };
        tap(44);
        for (let i = 0; i < 22; ++i) assertFrame();
        tap(200);
        tap(44);
        for (let i = 0; i < 45; ++i) assertFrame();
        return {frames, first, eof, frozen, afterEscape, final: read(),
          error: mod.UTF8ToString(mod._th13_error())};
        function assertFrame() {
          if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
        }
      } finally { mod._free(name); }
    }, bytes);
    assert.equal(result.frames, 2);
    assert.equal(result.first, 1);
    assert.equal(result.eof, 1);
    assert.equal(result.frozen[14], 3, 'EOF opens the original PracticeEnd overlay');
    assert.equal(result.frozen[6] & 0x10, 0x10);
    assert.equal(result.afterEscape[14], 3, 'Escape cannot resume an ended replay');
    assert.equal(result.afterEscape[5], result.frozen[5], 'gameplay stays frozen');
    assert.equal(result.final[0], 0, 'physical Yes input returns to the title');
    assert.equal(result.error, '');
    assert.deepEqual(errors, []);
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
