// 回放导入失败文案生存期回归（th13_replay_import_error 悬垂指针修复，
// 独立回归名：replay-import-error.lifetime）。
// 修复面：web/cpp/sdl/BrowserRuntime.cpp th13_replay_import_error() 原先返回
// ApplicationState::replay_import_error() 按值 std::string 临时量的 c_str()
// ——页面壳 UTF8ToString 读取前临时量已析构（UB）。修复后函数级 static
// std::string 快照承接按值返回；导入成功应用侧 clear() 后快照回空串。
//
// 断言序（wasm 导出面直驱；坏 .rpy 不 fail() 应用——phase 保持标题 0）：
//   1. 损坏导入 A（magic 对、version=0）→ 精确文案 "corrupt-a.rpy:
//      unsupported replay version"（ReplayFile.cpp:157）。
//   2. heap churn（反复 malloc/free 冲刷）后再读 → 文案逐字节不变。
//   3. 损坏导入 B（magic 错）→ 精确文案 "corrupt-b.rpy: bad magic"
//      （ReplayFile.cpp:156），同前保持稳定。
//   4. 合法真实录像（replays/th13_01.rpy）导入 → 返回 1、文案清空（空串）、
//      应用未死（th13_phase()==0、页面无 pageerror）。
// 运行（th13-shot 镜像；需 web 构建产物 sdl3/th13-sdl.mjs）：
//   CHROMIUM_PATH=… node ports/th13/web/tests/sdl/replay-import-error.lifetime.test.mjs
// 独立验证可用 TH13_SDL3_BUILD_DIR 指向隔离构建（默认共享 web/artifacts）。
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
    // 页面壳启动序到标题（app 活着且 initialized；phase 0 = title）。
    await page.waitForFunction('window.mod && window.mod._th13_phase() === 0',
                               null, {timeout: 180000});

    // 导入辅助：bytes + name → {ok, text}（text = 导出面 UTF8ToString 读回）。
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

    // churn 辅助：反复大块 malloc/free 冲刷 libc 堆后再读文案。
    const churnAndRead = () => page.evaluate(() => {
      const blocks = [];
      for (let i = 0; i < 48; i++) blocks.push(window.mod._malloc(8192));
      for (const p of blocks) window.mod._free(p);
      return window.mod.UTF8ToString(window.mod._th13_replay_import_error());
    });

    // A：magic "t13r" 对、version=0 → "unsupported replay version"。
    const corruptA = new Uint8Array(64);
    corruptA.set([0x74, 0x31, 0x33, 0x72], 0);
    const a = await importBytes(corruptA, 'corrupt-a.rpy');
    assert.equal(a.ok, 0, 'corrupt A must fail');
    assert.equal(a.text, 'corrupt-a.rpy: unsupported replay version');
    assert.equal(await churnAndRead(), a.text, 'text A stable after heap churn');

    // B：magic 错 → "bad magic"。
    const corruptB = new Uint8Array(64);
    corruptB.set([0x62, 0x61, 0x64, 0x21], 0);
    const b = await importBytes(corruptB, 'corrupt-b.rpy');
    assert.equal(b.ok, 0, 'corrupt B must fail');
    assert.equal(b.text, 'corrupt-b.rpy: bad magic');
    assert.equal(await churnAndRead(), b.text, 'text B stable after heap churn');

    // 合法真实录像导入：返回 1、错误清空、应用仍活（无 fail()、无 pageerror）。
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
