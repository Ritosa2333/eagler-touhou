// Replay localStorage 持久化冒烟（th13.html standalone 壳；Playwright 无头）。
// 断言链（真实用户路径：隐藏 file input 导入，不走任何测试后门）：
//   1. setInputFiles 导入合成 th13_21.rpy → 成功后 localStorage 落
//      索引 th13.replays（{name,size,ts}）+ blob th13.replay.th13_21.rpy，
//      blob base64 解码与输入字节一致。
//   2. page.reload() 整页刷新（wasm catalog 归零）→ 启动序列跑完
//      __th13_replay_restore_done → 索引/blob 仍在，__th13_log 无
//      'replay-restore' 失败记录。
//   3. catalog 回灌直证：th13_replay_import_hint() === 20（th13_21.rpy →
//      编号槽 21-1；标题层未进 Replay 子菜单，hint 未被消费）。
// 合成 .rpy 与 replay-fixture.mjs 同源（t13r 容器，无原版字节入库）。
// 运行（th13-shot 镜像；需 web 构建产物 sdl3/th13-sdl.mjs）：
//   make shot CMD='node --test eagler-touhou/ports/th13/web/tests/sdl/replay-persistence.smoke.test.mjs'
import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, writeFile, mkdtemp} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const here = dirname(fileURLToPath(import.meta.url));
const portRoot = resolve(here, '../../..');
const runtimeDir = join(portRoot, 'web/sdl-runtime');
const buildDir = process.env.TH13_SDL3_BUILD_DIR ?? join(portRoot, 'web/artifacts');
const require = createRequire(new URL('../../../../../launcher/package.json', import.meta.url));
let chromium;
try {
  ({chromium} = require('playwright-core'));
} catch {
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}
const {replayFixture} = await import(join(here, 'replay-fixture.mjs'));

test('replay import persists across page reload and is restored into the catalog',
     {timeout: 600000}, async () => {
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
  const fixture = replayFixture({frames: 3});
  const importFile = join(await mkdtemp(join(tmpdir(), 'th13-replay-')), 'th13_21.rpy');
  await writeFile(importFile, fixture);
  try {
    browser = await chromium.launch({
      executablePath: process.env.CHROMIUM_PATH,
      headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
             '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
    });
    const page = await browser.newPage({viewport: {width: 800, height: 560}});
    page.on('pageerror', e => pageErrors.push(String(e)));
    const boot = async () => {
      await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
                      {waitUntil: 'domcontentloaded'});
      // 页面壳启动序到标题 + 内置/恢复导入序列全部完成。
      await page.waitForFunction(
        'window.mod && window.mod._th13_phase() === 0 && window.__th13_builtin_replays_done === true' +
        ' && window.__th13_replay_restore_done === true', null, {timeout: 300000});
    };
    await boot();

    // 1) 真实导入路径（隐藏 file input）：合成 th13_21.rpy。
    await page.setInputFiles('#replay-file', {name: 'th13_21.rpy',
      mimeType: 'application/octet-stream', buffer: fixture});
    await page.waitForFunction(
      "JSON.parse(localStorage.getItem('th13.replays') || '[]').some(e => e.name === 'th13_21.rpy')",
      null, {timeout: 30000});
    const stored = await page.evaluate(() => ({
      index: JSON.parse(localStorage.getItem('th13.replays')),
      blob: localStorage.getItem('th13.replay.th13_21.rpy'),
    }));
    const entry = stored.index.find(item => item.name === 'th13_21.rpy');
    assert.ok(entry, 'index must record th13_21.rpy');
    assert.equal(entry.size, fixture.length);
    assert.equal(typeof entry.ts, 'number');
    assert.ok(stored.blob, 'blob key must exist');
    assert.ok(Buffer.from(stored.blob, 'base64').equals(fixture), 'blob bytes must match input');

    // 2) 整页刷新：catalog（wasm 内存）归零后由启动序列恢复。
    await page.reload({waitUntil: 'domcontentloaded'});
    await boot();
    const after = await page.evaluate(() => ({
      index: JSON.parse(localStorage.getItem('th13.replays')),
      blob: localStorage.getItem('th13.replay.th13_21.rpy'),
      restoreFailures: (window.__th13_log ?? []).filter(line => line.startsWith('replay-restore')),
      hint: window.mod._th13_replay_import_hint(),
    }));
    assert.deepEqual(after.index, stored.index, 'index must survive reload');
    assert.equal(after.blob, stored.blob, 'blob must survive reload');
    assert.deepEqual(after.restoreFailures, [], 'restore must not log failures');

    // 3) catalog 回灌直证：编号名 → 槽 21-1，hint 未被标题层消费。
    assert.equal(after.hint, 20, 'th13_21.rpy must be restored into catalog slot 20');
    assert.deepEqual(pageErrors, [], 'no page-level exceptions');
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
