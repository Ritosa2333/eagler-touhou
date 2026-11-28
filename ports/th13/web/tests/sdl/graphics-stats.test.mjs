import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve, dirname, join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const here = dirname(fileURLToPath(import.meta.url));
const port = resolve(here, '../../..');
const runtimeDir = join(port, 'web/sdl-runtime');
const buildDir = join(port, 'web/artifacts');
const require = createRequire(import.meta.url);
let chromium;
try {
  ({chromium} = require('playwright-core'));
} catch {
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}

const serve = (root, prefix) => (name) => readFile(join(root, name));
test('TH13 shared renderer streaming contract (glBufferData batches, no subData streaming)',
     {timeout: 240000}, async () => {
  const files = new Map();
  const put = (prefix, provider) => files.set(prefix, provider);
  put('/sdl-runtime/', name => readFile(join(runtimeDir, name.replace('/sdl-runtime/', ''))));
  put('/artifacts/', name => readFile(join(buildDir, name.replace('/artifacts/', ''))));
  put('/assets/', name => readFile(join(runtimeDir, 'assets', name.replace('/assets/', ''))));
  put('/replays/', name => readFile(join(runtimeDir, 'replays', name.replace('/replays/', ''))));
  const server = createServer(async (req, res) => {
    try {
      const url = req.url.split('?', 2)[0];
      for (const [prefix, provider] of files) {
        if (url.startsWith(prefix)) {
          const data = await provider(url);
          const type = url.endsWith('.mjs') ? 'text/javascript'
              : url.endsWith('.wasm') ? 'application/wasm'
              : url.endsWith('.html') ? 'text/html' : 'application/octet-stream';
          res.writeHead(200, {'Content-Type': type,
                              'Cross-Origin-Opener-Policy': 'same-origin',
                              'Cross-Origin-Embedder-Policy': 'require-corp'});
          res.end(data);
          return;
        }
      }
      res.writeHead(404).end();
    } catch (error) {
      res.writeHead(500).end(String(error));
    }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  const origin = `http://127.0.0.1:${server.address().port}`;
  let browser;
  const errors = [];
  try {
    browser = await chromium.launch({
      executablePath: process.env.CHROMIUM_PATH,
      headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
             '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
    });
    const page = await browser.newPage({viewport: {width: 800, height: 560}});
    page.on('pageerror', error => errors.push(String(error)));
    const consoleLines = [];
    page.on('console', message => consoleLines.push(message.type() + ' ' + message.text()));
    page.on('requestfailed', request =>
        consoleLines.push('reqfail ' + request.url() + ' ' + (request.failure()?.errorText ?? '')));
    await page.goto(`${origin}/sdl-runtime/th13-debug.html?replay=demo0.rpy&sht=pl01a.sht&stage=0`,
                    {waitUntil: 'domcontentloaded'});
    await page.waitForFunction('!!window.__th13_debug || !!window.__th13_debug_failed',
                               null, {timeout: 120000});
    const failed = await page.evaluate('window.__th13_debug_failed');
    assert.ok(!failed, 'session boot: ' + String(failed) + ' | console: ' + consoleLines.join(' || '));
    const stats = await page.evaluate(() => {
      const debug = window.__th13_debug;
      debug.pumpTo(179);
      const mod = window.mod;
      const pointer = mod._th13_debug_stats();
      if (!pointer) throw new Error('stats unavailable (no active app)');
      const out = [];
      for (let i = 0; i < 18; i++) {
        out.push(new DataView(mod.HEAPU8.buffer,
                              pointer + i * 4, 4).getUint32(0, true));
      }
      return out;
    });
    assert.deepEqual(errors, []);
    assert.equal(stats[7], 0, 'bufferSubUpdates must stay zero (no glBufferSubData streaming)');
    assert.ok(stats[6] > 0, 'bufferReplacements must occur (glBufferData full-batch upload)');
    assert.ok(stats[9] > 0, 'directBytes must occur (game-side premerged batches bypass copy)');
    const direct = stats[9], copied = stats[10];
    assert.ok(direct > copied, 'quad batches must dominate (direct bytes ' + direct +
              ' vs accumulated-batch bytes ' + copied + ')');
    assert.ok(stats[11] <= 8, 'layoutSetups bounded (VAO per layout key, got ' +
              stats[11] + ')');
    assert.ok(stats[0] > 0 && stats[4] > 0, 'renderer active (calls/frames recorded)');
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
