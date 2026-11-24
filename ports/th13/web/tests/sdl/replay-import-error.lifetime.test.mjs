import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const here = dirname(fileURLToPath(import.meta.url));
const portRoot = resolve(here, '../../..');
const runtimeDir = join(portRoot, 'web/sdl-runtime');
const buildDir = process.env.TH13_SDL3_BUILD_DIR ?? join(portRoot, 'web/artifacts');
const require = createRequire(import.meta.url);
let chromium;
try {
  ({chromium} = require('playwright-core'));
} catch {
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}

test('th13_replay_import_error: exact text stable across heap churn, cleared on success',
     {timeout: 300000}, async () => {
  const server = createServer(async (req, res) => {
    try {
      const url = req.url.split('?', 2)[0];
      const data = url.startsWith('/sdl-runtime/')
          ? await readFile(join(runtimeDir, url.replace('/sdl-runtime/', '')))
          : url.startsWith('/artifacts/')
            ? await readFile(join(buildDir, url.replace('/artifacts/', '')))
            : null;
      if (!data) { res.writeHead(404).end(); return; }
      const type = url.endsWith('.mjs') ? 'text/javascript'
          : url.endsWith('.wasm') ? 'application/wasm'
          : url.endsWith('.html') ? 'text/html' : 'application/octet-stream';
      res.writeHead(200, {'Content-Type': type,
                          'Cross-Origin-Opener-Policy': 'same-origin',
                          'Cross-Origin-Embedder-Policy': 'require-corp'});
      res.end(data);
    } catch (error) { res.writeHead(500).end(String(error)); }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  const pageErrors = [];
  try {
    browser = await chromium.launch({
      executablePath: process.env.CHROMIUM_PATH,
      headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
             '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
    });
    const page = await browser.newPage({viewport: {width: 800, height: 560}});
    page.on('pageerror', e => pageErrors.push(String(e)));
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
                    {waitUntil: 'domcontentloaded'});
    await page.waitForFunction('window.mod && window.mod._th13_phase() === 0',
                               null, {timeout: 180000});
    await page.waitForFunction('window.__th13_builtin_replays_done === true',
                               null, {timeout: 30000});

    const importBytes = (bytes, name) => page.evaluate(({bytes, name}) => {
      const dataPtr = window.mod._malloc(bytes.length);
      const namePtr = window.mod.allocateUTF8(name);
      window.mod.HEAPU8.set(bytes, dataPtr);
      let ok = 0, text = '';
      try {
        ok = window.mod._th13_replay_import(dataPtr, bytes.length, namePtr);
        text = window.mod.UTF8ToString(window.mod._th13_replay_import_error());
      } finally {
        window.mod._free(dataPtr);
        window.mod._free(namePtr);
      }
      return {ok, text};
    }, {bytes, name});

    const churnAndRead = () => page.evaluate(() => {
      const blocks = [];
      for (let i = 0; i < 48; i++) blocks.push(window.mod._malloc(8192));
      for (const p of blocks) window.mod._free(p);
      return window.mod.UTF8ToString(window.mod._th13_replay_import_error());
    });

    const corruptA = new Uint8Array(64);
    corruptA.set([0x74, 0x31, 0x33, 0x72], 0);
    const a = await importBytes(corruptA, 'corrupt-a.rpy');
    assert.equal(a.ok, 0, 'corrupt A must fail');
    assert.equal(a.text, 'corrupt-a.rpy: unsupported replay version');
    assert.equal(await churnAndRead(), a.text, 'text A stable after heap churn');

    const corruptB = new Uint8Array(64);
    corruptB.set([0x62, 0x61, 0x64, 0x21], 0);
    const b = await importBytes(corruptB, 'corrupt-b.rpy');
    assert.equal(b.ok, 0, 'corrupt B must fail');
    assert.equal(b.text, 'corrupt-b.rpy: bad magic');
    assert.equal(await churnAndRead(), b.text, 'text B stable after heap churn');

    const valid = new Uint8Array(await readFile(join(runtimeDir, 'replays', 'th13_01.rpy')));
    const v = await importBytes(valid, 'th13_01.rpy');
    assert.equal(v.ok, 1, 'valid replay import must succeed');
    assert.equal(v.text, '', 'success must clear the failure snapshot');
    assert.equal(await churnAndRead(), '', 'cleared state stable');
    assert.equal(await page.evaluate(() => window.mod._th13_phase()), 0,
                 'application must stay alive on title (no fatal failure)');
    assert.deepEqual(pageErrors, [], 'no page-level exceptions');
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
