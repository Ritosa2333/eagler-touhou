// 渲染统计契约浏览器测试（th15 graphics-browser.test.mjs 的 th13 对应物）：
// 真实浏览器（th13-shot 镜像 chromium+SwiftShader）开 th13-debug.html 回放会话，
// 泵若干帧后断言共享 Renderer 的统计契约——AGENTS.md 性能检查单的固化门禁：
//   stats[7]  bufferSubUpdates == 0        禁止 glBufferSubData 分段流式
//                                          （Mali/Adreno 真机卡顿实锤）
//   stats[6]  bufferReplacements > 0        每批 glBufferData 整体替换在用
//   stats[9]  directBytes > 0               游戏合批直通顶点在用
//   stats[10] copiedBytes == 0              直通批无重复拷贝
//   stats[11] layoutSetups <= 2             顶点布局缓存有界
// 统计数组布局 = touhou::sdl::Renderer::Statistics 成员声明序（18 项 u32）。
// 用法（th13-shot 镜像，工作区根）：
//   make shot CMD='node --test eagler-touhou/ports/th13/web/tests/sdl/graphics-stats.test.mjs'
// （需先完成 web 构建 + demo0.rpy 备入 sdl-runtime/replays/）
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

// 最小会话资源（th13-debug.html 装载清单的子集不可裁剪——页面按固定表装）
// → 直接以静态文件服务整个 sdl-runtime + artifacts + demo 录像。
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
      // 路径须剥查询串（req.url 含 ?replay=…，直接拼路径会 ENOENT）。
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
    // demo0.rpy：随行演示录像（ZUN 作，chara=1 → pl01a；录像头由
    // begin_replay_run 推导，sht 参数仅供页面资产装载）。
    await page.goto(`${origin}/sdl-runtime/th13-debug.html?replay=demo0.rpy&sht=pl01a.sht&stage=0`,
                    {waitUntil: 'domcontentloaded'});
    await page.waitForFunction('!!window.__th13_debug || !!window.__th13_debug_failed',
                               null, {timeout: 120000});
    const failed = await page.evaluate('window.__th13_debug_failed');
    assert.ok(!failed, 'session boot: ' + String(failed) + ' | console: ' + consoleLines.join(' || '));
    // 泵 180 帧（装载挡板→对局内，覆盖合批/直通/批 flush 全路径）。
    // 统计读取：th13_debug_stats 返回 Statistics 聚合的 u32 视图地址；
    // 内存视图经 HEAPU8（默认运行时导出）+ DataView 取 u32，避免依赖
    // 未导出的 HEAPU32。
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
    // copiedBytes（stats[10]）：draw() 累积批路径的合法拷贝（shape/trail/mesh
    // 族经 primitives() 入批——顶点内存由 Renderer 持有，拷贝是合批的组成部
    // 分，非 th15 fixture 语义的「已合批三角形二次拷贝」）。th13 契约改为
    // 主路径占比：quad 顶点批（triangles→draw_batch 直通）必须是字节量大头。
    const direct = stats[9], copied = stats[10];
    assert.ok(direct > copied, 'quad batches must dominate (direct bytes ' + direct +
              ' vs accumulated-batch bytes ' + copied + ')');
    // layoutSetups（stats[11]）：VAO 缓存按 layoutKey 首建递增。th13 场景
    // 用满布局族（ScreenColorUv quad / ScreenColor shape / WorldUv 实例 /
    // WorldColorUv mesh）——th15 fixture 的 <=2 是其双布局场景值；th13 契约
    // = 布局缓存有界（键总数不超过 VertexLayout 枚举族 + 实例开关组合）。
    assert.ok(stats[11] <= 8, 'layoutSetups bounded (VAO per layout key, got ' +
              stats[11] + ')');
    assert.ok(stats[0] > 0 && stats[4] > 0, 'renderer active (calls/frames recorded)');
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
