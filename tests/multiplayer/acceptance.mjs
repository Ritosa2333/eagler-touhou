// th13-mp MP 工作流浏览器层入口（M1/T8 测试作者；工单 M1-plan §2/T8）。
//
// 被 scripts/mp_workflow_gate.py browser()（gate:165-186）以 cwd=ports/th13-mp 调用：
//   node tests/multiplayer/acceptance.mjs --through M<n> --source-digest <hex>
//        --report <path> [--case <id> ...]
// 也可直接 node 运行（同一 CLI）。gate 在调用前已先行 build_identity()；
// 本入口再次核验构建身份（build.json.sha256 == 磁盘 wasm == 各客户端送达 wasm），
// 不一致即失败——不做像素对比替身（工单风险项）。
//
// 契约（docs/plans/th13-mp-m1-m9-workflow.md「验证入口与输出契约」）：
//   - --case 只认 tests/multiplayer/workflow-cases.json 中 layer:"browser" 的映射
//     条目（files 以 PORT=ports/th13-mp/ 为基，gate:119-123 同则）；无 --case 时
//     current/cumulative 全部固定 browser case 必须真实执行，缺映射即失败。
//   - 报告 schema th13/mp-workflow-test/v1，layer:"browser"；case 只在真实打开
//     生产 th13-debug.html 会话并断言通过后标 passed；任一失败整体非零退出。
//   - 报告 runtime 段：wasmSha256/servedWasmSha256（== build.json.sha256）、
//     clients（真实打开的浏览器客户端会话数）。M1–M6 无联网/回滚：
//     transportPaths 为空数组、rollbackCount/maxRollbackDistance 为 0，
//     不提前捏造联网数据；M7 起由对应 case 真实填充。
//   - wasm/脚本送达核验复用仓根共享工具 eagler-touhou/tools/browser/
//     verified-runtime.mjs（观察页面实际收到的响应字节，不另发预检请求）；
//     音频面复用 tools/browser/audio-output-probe.mjs（case 经
//     page.addInitScript 安装，与 th12 scripts/check-sdl-browser.mjs:43-44 同式）。
//   - 驱动面全部为现有生产导出（BrowserRuntime.cpp）：手动泵客户端 =
//     th13-debug.html（页内 rAF 不启动）逐帧 th13_debug_frame()；rAF 客户端 =
//     th13_loop_start() 自然泵；每帧观测 th13_probe_state()+th13_frame()。
//     不用 th13_frame_tick 做手动泵（loop_pause 后 no-op、不暂停则与 rAF
//     双泵，r4 F3 实证）；不等待 __e2e_loop_paused 之类页内不存在的信号
//     （debug 页只暴露 __th13_debug / __th13_debug_failed，r4 R4）。
import {createHash} from 'node:crypto';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';
import {verifyRuntimeDelivery} from '../../../../tools/browser/verified-runtime.mjs';
import {installPcmProbe, installSfxStartProbe, samplePcm} from '../../../../tools/browser/audio-output-probe.mjs';
import {startServe, PORT_ROOT, REPO_ROOT} from './browser/serve.mjs';
import {spawn} from 'node:child_process';

const HERE = dirname(fileURLToPath(import.meta.url));          // tests/multiplayer
const MAPPING = join(HERE, 'workflow-cases.json');
const BUILD_DIR = join(PORT_ROOT, 'web/artifacts/sdl3');

// 固定 browser case 表——外层 scripts/mp_workflow_gate.py:33-43 BROWSER 的只读
// 镜像（主代理所有，勿在此改语义；两端须逐条一致）。cumulative=1..stage 并集。
const BROWSER_REQUIRED = {
  1: ['draw_purity_rng', 'side_effect_boundary'],
  2: ['independent_inputs_2_3', 'four_characters_duplicates'],
  3: ['shared_pickup_once', 'target_all_consumers_ties'],
  4: ['team_trance_shooting_four_roles', 'protection_owner_only_death'],
  5: ['hud_world_owner_visibility', 'menu_edges_pause_dialogue', 'no_sp_save_writes'],
  6: ['full_state_restore_replay', 'anm_borrowed_parent_chains'],
  7: ['rtc_two_three_clients', 'relay_fallback_path', 'confirmed_hash_lockstep', 'frame_zero_load_barrier', 'device_seat_isolation'],
  8: ['rollback_one_three_twelve', 'audio_bgm_once', 'trance_death_resimulation', 'production_draw_pixels', 'jitter_loss_reorder_duplicate', 'resim_budget_new_progress', 'rollback_full_window_raf'],
  9: ['replay_roundtrip_seek', 'spectator_confirmed_latejoin', 'slow_spectator_isolation', 'stage_entry_carry', 'three_reentries_identity_swap', 'run_generation_isolation', 'terminal_local_save', 'disconnect_confirmed_save_return', 'scene_coverage_matrix', 'runtime_page_soak'],
};

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

class CaseFailure extends Error {}

// ---- CLI（与 gate browser() 传参逐字一致）----
function parseArgs(argv) {
  const out = {through: null, sourceDigest: null, report: null, cases: []};
  for (let i = 0; i < argv.length; ++i) {
    const arg = argv[i];
    const value = () => {
      if (i + 1 >= argv.length) throw new CaseFailure(`${arg} needs a value`);
      return argv[++i];
    };
    switch (arg) {
      case '--through': out.through = value(); break;
      case '--source-digest': out.sourceDigest = value(); break;
      case '--report': out.report = value(); break;
      case '--case': out.cases.push(value()); break;
      default: throw new CaseFailure(`unknown argument: ${arg}`);
    }
  }
  if (!/^M[1-9]$/.test(out.through || '')) throw new CaseFailure('--through must be M1..M9');
  if (!/^[0-9a-f]{64}$/.test(out.sourceDigest || '')) throw new CaseFailure('--source-digest must be 64 hex chars');
  if (!out.report) throw new CaseFailure('--report is required');
  return out;
}

function browserExpected(stage, cumulative) {
  const stages = cumulative ? [...Array(stage).keys()].map(i => i + 1) : [stage];
  return new Set(stages.flatMap(s => (BROWSER_REQUIRED[s] ?? []).map(name => `m${s}.${name}`)));
}

// ---- 映射（唯一真值；files 以 PORT 为基，gate:119-123 同则校验）----
// gate 固定表允许同一 id 跨层并存（m1.draw_purity_rng/side_effect_boundary
// 均有 native+browser 两条；gate:117 先按 layer 过滤再键 id）。本入口同样只
// 在 browser 层内键控：平铺 Map<id> 会让 native 条目与 browser 条目互相覆盖，
// 同层重复 id 直接失败（防映射机械错误静默吞掉 case）。
async function selectCases(stage, selectedIds) {
  const mapping = JSON.parse(await readFile(MAPPING, 'utf8'));
  const byId = new Map();
  for (const entry of mapping.cases ?? []) {
    if (!entry || typeof entry.id !== 'string') throw new CaseFailure('mapping entry without id');
    if (entry.layer !== 'browser') continue;   // native 条目由 run-native.sh 消费
    if (byId.has(entry.id))
      throw new CaseFailure(`duplicate browser mapping entry: ${entry.id}`);
    byId.set(entry.id, entry);
  }
  if (selectedIds.length) {
    const current = browserExpected(stage, false);
    for (const id of selectedIds) {
      if (!current.has(id) && !new RegExp(`^m${stage}\\.unit_[a-z0-9_]+$`).test(id))
        throw new CaseFailure(`browser-unit requests cases outside its milestone: ${id}`);
    }
  }
  const wanted = selectedIds.length ? new Set(selectedIds) : browserExpected(stage, true);
  const missing = [...wanted].filter(id => !byId.has(id));
  if (missing.length)
    throw new CaseFailure('browser cases not registered in workflow-cases.json (layer=browser): ' + missing.join(', '));
  const cases = [];
  for (const id of wanted) {
    const entry = byId.get(id);
    if (!Array.isArray(entry.files) || !entry.files.length)
      throw new CaseFailure(`case ${id} has no files`);
    const files = [];
    for (const name of entry.files) {
      const file = resolve(PORT_ROOT, name);
      if (!file.startsWith(resolve(PORT_ROOT) + '/') || !(file.endsWith('.mjs') || file.endsWith('.js')))
        throw new CaseFailure(`invalid case test file: ${name}`);
      files.push({name, file});
    }
    cases.push({id, files});
  }
  return cases;
}

// ---- 构建身份（gate build_identity gate:151-163 的三方一致核验）----
async function loadBuildIdentity() {
  const buildJsonPath = join(BUILD_DIR, 'build.json');
  let buildJson;
  try {
    buildJson = JSON.parse(await readFile(buildJsonPath, 'utf8'));
  } catch (error) {
    throw new CaseFailure(`missing fresh production build: ${buildJsonPath} (${error.message})`);
  }
  if (typeof buildJson.sha256 !== 'string' || !/^[0-9a-f]{64}$/.test(buildJson.sha256))
    throw new CaseFailure('build.json has no sha256');
  if (!buildJson.sourceFiles || typeof buildJson.sourceFiles !== 'object' ||
      !Object.keys(buildJson.sourceFiles).length)
    throw new CaseFailure('build has no source identity inventory');
  const wasmBytes = await readFile(join(BUILD_DIR, 'th13-sdl.wasm'));
  const wasmSha = sha256(wasmBytes);
  if (buildJson.sha256 !== wasmSha)
    throw new CaseFailure(`build input changed: build.json.sha256 ${buildJson.sha256} != on-disk wasm ${wasmSha}`);
  const scriptBytes = await readFile(join(BUILD_DIR, 'th13-sdl.mjs'));
  return {buildJson, wasmBytes, wasmSha, scriptBytes};
}

// ---- 浏览器客户端（th13-debug.html 手动泵 / rAF 自然泵）----
const CHROMIUM_ARGS = ['--no-sandbox', '--disable-dev-shm-usage',
  process.env.TH13_ANGLE === 'gl' ? '--use-angle=gl' : '--use-angle=swiftshader',
  ...(process.env.TH13_ANGLE === 'gl' ? ['--ignore-gpu-blocklist', '--disable-gpu-sandbox'] : []),
  '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required',
  '--force-color-profile=srgb'];

async function loadChromium() {
  const require = createRequire(import.meta.url);
  try {
    return require('playwright-core').chromium;
  } catch {
    return require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')).chromium;
  }
}

// 单个 debug 页客户端。kind:"manual" 不启动页内 rAF（逐帧 th13_debug_frame 泵）；
// kind:"raf" 由调用方随后经 client.startRaf(targets) 走 th13_loop_start 自然泵。
async function openClient(ctx, {kind, seed, replay = 'th13_01.rpy', sht = 'pl00a.sht', stage = 0, query = '', init = null} = {}) {
  const {browser, server, build, counters} = ctx;
  const context = await browser.newContext({viewport: {width: 800, height: 560}});
  // 种子必须先于模块初始化（Application.cpp:80-88 的 EM_ASM 读取窗口）。
  await context.addInitScript(value => { globalThis.__th13_boot_seed = value; }, seed);
  // case 级 init 脚本（M8 发送侧延迟注入等；导航前安装）。
  if (init) await context.addInitScript({content: init});
  // 音频面探针（仓根共享工具；导航前安装才能拦到运行时创建的节点）。
  await context.addInitScript(installPcmProbe);
  await context.addInitScript(installSfxStartProbe);
  const page = await context.newPage();
  const consoleLines = [];
  page.on('console', message => consoleLines.push(message.type() + ' ' + message.text()));
  page.on('pageerror', error => consoleLines.push('pageerror ' + error.message));
  // 送达核验必须在导航前挂上（verified-runtime.mjs 语义：观察真实消费的响应）。
  const verified = await verifyRuntimeDelivery(page, {
    scriptUrl: `${server.origin}/artifacts/sdl3/th13-sdl.mjs`,
    scriptBytes: build.scriptBytes,
    wasmName: 'th13-sdl.wasm',
    wasmBytes: build.wasmBytes,
  });
  const url = `${server.origin}/th13-debug.html?replay=${encodeURIComponent(replay)}` +
              `&sht=${encodeURIComponent(sht)}&stage=${stage}` + (query ? `&${query}` : '');
  await page.goto(url, {waitUntil: 'domcontentloaded'});
  await page.waitForFunction('!!window.__th13_debug || !!window.__th13_debug_failed',
                             null, {timeout: 240000});
  const failed = await page.evaluate('window.__th13_debug_failed ?? null');
  if (failed) {
    throw new CaseFailure(`debug page failed to boot (${kind}): ${failed}\n` +
                          consoleLines.slice(-20).join('\n'));
  }
  const bootSeed = await page.evaluate('window.mod._th13_boot_seed()');
  if (bootSeed !== seed) {
    throw new CaseFailure(`boot seed override did not take effect (${kind}): ` +
                          `seen ${bootSeed}, configured ${seed}`);
  }
  counters.clients += 1;
  const total = await page.evaluate('window.__th13_debug.total');

  const client = {
    kind, page, verified, consoleLines, bootSeed, total,
    // 送达核验收尾（脚本+wasm 各恰一次、字节与构建产物一致，否则抛错）。
    assertDelivery: async () => {
      await verified.assertComplete();
      for (const receipt of verified.receipts) {
        const isScript = receipt.kind === 'script';
        const expectedSha = isScript ? sha256(build.scriptBytes) : build.wasmSha;
        const expectedBytes = isScript ? build.scriptBytes.length : build.wasmBytes.length;
        ctx.assert(receipt.sha256 === expectedSha,
          `${kind} 客户端送达 ${receipt.kind} sha256 与构建产物一致（${receipt.bytes}B）`);
        ctx.assert(receipt.bytes === expectedBytes,
          `${kind} 客户端送达 ${receipt.kind} 字节数与构建产物一致（${receipt.bytes}B）`);
      }
      const wasmReceipts = verified.receipts.filter(r => r.kind === 'wasm');
      ctx.assert(wasmReceipts.length === 1, `${kind} 客户端恰收到一次 wasm 响应`);
      ctx.servedWasm.add(wasmReceipts[0].sha256);
    },
    probe: () => page.evaluate(() => {
      const ptr = window.mod._th13_probe_state() / 4;
      return Array.from(window.mod.HEAP32.subarray(ptr, ptr + 16));
    }),
    frame: () => page.evaluate(() => window.mod._th13_frame()),
    // 手动泵（仅 manual）：单任务内逐帧 th13_debug_frame（BrowserRuntime.cpp:650-667，
    // 固定 1/60 步长、defer+commit+pump），每帧读 probe_state[16] + th13_frame；
    // snap 帧号处同任务内 canvas.toDataURL（WebGL 后备缓冲未合成前取像，同
    // shot-frames.mjs 配方）。帧体校验与生产工具 pumpTo 同式：泵失败抛错 +
    // 对局帧冻结抛错（输入流耗尽/场景停滞）。
    pump: async ({count, snap = []} = {}) => {
      if (kind !== 'manual') throw new CaseFailure('pump() only supports manual clients');
      return page.evaluate(({count, snap}) => {
        const mod = window.mod;
        const err = () => mod.UTF8ToString(mod._th13_error());
        const read = () => {
          const ptr = mod._th13_probe_state() / 4;
          return Array.from(mod.HEAP32.subarray(ptr, ptr + 16));
        };
        const records = [];
        for (let i = 0; i < count; ++i) {
          const stageBefore = mod._th13_debug_stage_frame();
          if (!mod._th13_debug_frame())
            throw new Error('帧泵失败@' + mod._th13_debug_replay_frame() + '：' + err());
          if (mod._th13_debug_stage_frame() === stageBefore)
            throw new Error('场景冻结@' + stageBefore + '（输入流耗尽或场景停滞）');
          const frame = mod._th13_frame();
          const record = {frame, probe: read()};
          if (snap.includes(frame))
            record.url = document.getElementById('canvas').toDataURL('image/png');
          records.push(record);
        }
        return records;
      }, {count, snap});
    },
    // rAF 客户端（仅 raf）：th13_loop_start（BrowserRuntime.cpp:436-459）自然泵，
    // 经生产回调 Module.onGameFrame（BrowserRuntime.cpp:32-34，drive_one_frame
    // 尾部每回调一次，携带 ok/milliseconds/ticks）观测。ticks===1 = 恰一逻辑帧；
    // ticks>1 = 帧钟追赶批次（快速软件渲染下 rAF 迟到），批次内中间帧不可见，
    // 记录批次尾帧供比对。抵达 target（批次尾帧 ≥ 目标）后 th13_loop_pause(1)
    // 冻结（下一回调起 no-op；本回调已在帧边界后）。
    startRaf: async (targets) => {
      if (kind !== 'raf') throw new CaseFailure('startRaf() only supports raf clients');
      return page.evaluate(targets => {
        const mod = window.mod;
        window.__raf = {records: [], pending: [...targets], done: [], failure: null, paused: false};
        mod.onGameFrame = (ok, milliseconds, ticks) => {
          const state = window.__raf;
          if (state.failure || state.paused) return;
          try {
            if (!ok) throw new Error('rAF 帧体失败 ticks=' + ticks);
            if (ticks === 0) return;  // 首回调 delta=0 不泵帧，无观测点
            const frame = mod._th13_frame();
            const ptr = mod._th13_probe_state() / 4;
            state.records.push({
              frame, ticks, milliseconds,
              probe: Array.from(new Int32Array(mod.HEAP32.buffer, ptr * 4, 16)),
            });
            const next = state.pending[0];
            if (next !== undefined && frame >= next) {
              state.done.push({target: next, frame, ticks});
              state.pending.shift();
              if (!state.pending.length) {
                state.paused = true;
                mod._th13_loop_pause(1);
              }
            }
          } catch (error) {
            state.failure = String(error && error.message || error);
            try { mod._th13_loop_pause(1); state.paused = true; } catch (_) { /* 已停 */ }
          }
        };
        mod._th13_loop_start();
        return mod._th13_frame();
      }, targets);
    },
    rafState: () => page.evaluate(() => ({
      records: window.__raf.records,
      pending: window.__raf.pending,
      done: window.__raf.done,
      failure: window.__raf.failure,
      paused: window.__raf.paused,
      frame: window.mod._th13_frame(),
    })),
    waitRafTargets: async (targets, timeoutMs = 300000) => {
      const began = Date.now();
      for (;;) {
        const state = await client.rafState();
        if (state.failure) throw new CaseFailure(`rAF 客户端帧体失败：${state.failure}`);
        if (state.paused && state.pending.length === 0) return state;
        if (Date.now() - began > timeoutMs)
          throw new CaseFailure(`rAF 客户端未在时限内抵达目标帧 ${JSON.stringify(targets)}：` +
                                `done=${JSON.stringify(state.done)} frame=${state.frame}`);
        await new Promise(done => setTimeout(done, 250));
      }
    },
    // 音频面（两 kind 通用）：仅解除 AudioDevice 挂起（输出面），不推进任何帧、
    // 不触碰 running/cadence/RNG（BrowserRuntime.cpp:423-426）。
    resumeAudio: () => page.evaluate(() => window.mod._th13_debug_audio_resume()),
    // 挂起音频设备 + 冻结 rAF 帧钟（BrowserRuntime.cpp:428-434：suspend(true)→
    // SDL_PauseAudioStreamDevice）。手动泵 th13_debug_frame 不读 loop 级
    // suspended 旗标，逐帧推进不受影响——副作用边界 case 的负控入口。
    loopPause: enabled => page.evaluate(v => window.mod._th13_loop_pause(v ? 1 : 0), enabled),
    // 生产音频统计面：AudioDevice::pump 每帧经 th13_publish_audio 发布
    //（AudioDevice.cpp:22-31；BrowserRuntime.cpp:664 每泵一次）。
    audioStats: () => page.evaluate(() => ({...(window.mod.th13Audio ?? {})})),
    sfxMark: () => page.evaluate(() => globalThis.__sfxStarts.length),
    sfxSlice: from => page.evaluate(from => globalThis.__sfxStarts.slice(from).map(
      e => ({when: e.when, peak: e.peak, channels: e.channels})), from),
    // 生产音频请求 tap（观察先于排队合并；BrowserRuntime.cpp:1419-1453）。
    audioObserveEnable: () => page.evaluate(() => window.mod._th13_debug_audio_observe(1)),
    audioTake: () => page.evaluate(() => window.mod.UTF8ToString(window.mod._th13_debug_audio_take())),
    close: async () => {
      await context.close();
    },
  };
  return client;
}

// ---- M7：本机 netplay relay（services/relay 子进程；测试专用生命周期）----
// 与 ports/th15-mp/portable/multiplayer/browser-test.mjs:12-33 同配方：
// spawn netplay-relay.mjs → stdout 'listening ws://127.0.0.1:<port>' → url。
async function startRelay({inject = {}} = {}) {
  const relayEntry = join(REPO_ROOT, 'services/relay/src/netplay-relay.mjs');
  const relay = spawn(process.execPath, [relayEntry], {
    cwd: REPO_ROOT,
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
          EAGLER_NETPLAY_RELAY_PORT: '0',
          // relay_fallback case：服务端显式开启 gameplay relay fallback
          //（launcher 契约的可选能力；默认关闭是产品语义）。
          EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1',
          // M8 注入面：延迟/抖动/丢包（真实触发 rollback>0）。
          ...inject},
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let log = '';
  relay.stdout.on('data', chunk => { log += chunk; });
  relay.stderr.on('data', chunk => { log += chunk; });
  const port = await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new CaseFailure(
      `relay startup timeout: ${log.slice(-500)}`)), 15000);
    relay.stdout.on('data', chunk => {
      const match = String(chunk).match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);
      if (match) { clearTimeout(timer); resolve(Number(match[1])); }
    });
    relay.once('exit', code => {
      clearTimeout(timer);
      reject(new CaseFailure(`relay exited ${code}: ${log.slice(-500)}`));
    });
  });
  return {
    url: `ws://127.0.0.1:${port}`,
    port,
    log: () => log,
    close: () => { relay.kill(); },
  };
}

// ---- 主流程 ----
async function main() {
  const args = parseArgs(process.argv.slice(2));
  const stage = Number(args.through.slice(1));
  const cases = await selectCases(stage, args.cases);
  const build = await loadBuildIdentity();

  const server = await startServe();
  const chromium = await loadChromium();
  const browser = await chromium.launch({
    executablePath: process.env.CHROMIUM_PATH,
    headless: true,
    args: CHROMIUM_ARGS,
  });

  // 共享态：客户端会话数、各客户端送达 wasm 摘要（终局三方一致核验用）。
  const counters = {clients: 0};
  const servedWasm = new Set();
  // M7+ runtime 元数据贡献（真实观测聚合）：transportPaths 来自页面
  // __eaglerPeerTransport.route（BrowserPeerTransport 的实际路径），回滚
  // 计数/距离来自 th13_debug_mp_net_rollback 的会话统计。gate browser()
  // 对 M7+ 强制 rtc/relay、M8+ 强制 rollbackCount>0 与 maxRollbackDistance
  // >=12——不聚合即门必挂，不得预先捏造。
  const runtimeFacts = {transportPaths: new Set(), rollbackCount: 0, maxRollbackDistance: 0};
  const log = (...items) => console.log('[acceptance]', ...items);

  const reported = [];
  let failedCase = null;
  try {
    for (const entry of cases) {
      const startedAt = Date.now();
      // case 级断言计数与证据；浏览器/服务/构建身份共享，送达摘要汇聚共享。
      const caseCtx = {
        browser, server, build, servedWasm, log,
        caseId: entry.id,
        clients: 0,
        assertions: 0,
        evidence: [],
        openClient: options => openClient(
          {browser, server, build, counters, servedWasm, assert: makeAssert(caseCtx)},
          options),
        samplePcm: page => samplePcm(page),
        // M7 联机 case 设施：本机 relay（services/relay 独立服务子进程；
        // RTC/relay 竞速在 transport 内收敛——relay URL 即真实 ws 信令）。
        startRelay: options => startRelay(options),
        // runtime 元数据贡献：case 从真实会话观测登记（见 runtimeFacts）。
        noteRuntime(facts) {
          if (!facts) return;
          for (const path of facts.transportPaths ?? [])
            if (typeof path === 'string' && path) runtimeFacts.transportPaths.add(path);
          if (Number.isInteger(facts.rollbackCount))
            runtimeFacts.rollbackCount =
              Math.max(runtimeFacts.rollbackCount, facts.rollbackCount);
          if (Number.isInteger(facts.maxRollbackDistance))
            runtimeFacts.maxRollbackDistance =
              Math.max(runtimeFacts.maxRollbackDistance, facts.maxRollbackDistance);
        },
        assert: null,
        equal(actual, expected, message) {
          const same = typeof expected === 'object' && expected !== null
            ? JSON.stringify(actual) === JSON.stringify(expected)
            : actual === expected;
          caseCtx.assert(same, `${message}（actual=${JSON.stringify(actual)?.slice(0, 200)} ` +
                               `expected=${JSON.stringify(expected)?.slice(0, 200)}）`);
        },
      };
      const makeAssert = context => (condition, message) => {
        if (!condition) throw new CaseFailure(message);
        context.assertions += 1;
      };
      caseCtx.assert = makeAssert(caseCtx);

      let passed = true, errorText = null;
      try {
        for (const {name, file} of entry.files) {
          const module = await import(file);
          if (typeof module.run !== 'function')
            throw new CaseFailure(`case file ${name} does not export run(ctx)`);
          log(`case ${entry.id}: file ${name}`);
          await module.run(caseCtx);
        }
        if (caseCtx.assertions === 0)
          throw new CaseFailure('case executed no assertions');
      } catch (error) {
        passed = false;
        errorText = String(error && error.message || error);
        failedCase = failedCase ?? {id: entry.id, error: errorText};
      }
      reported.push({
        id: entry.id,
        passed,
        assertions: caseCtx.assertions,
        evidence: [
          `files=${entry.files.map(f => f.name).join(',')} elapsedMs=${Date.now() - startedAt}`,
          ...caseCtx.evidence,
          ...(passed
            ? [`真实执行：${caseCtx.assertions} 条原子断言全部通过`]
            : [`失败：${errorText}`]),
        ],
      });
      log(`case ${entry.id}: ${passed ? 'PASS' : 'FAIL'} (${caseCtx.assertions} assertions)`);
      if (!passed) break;
    }
  } finally {
    await browser.close().catch(() => {});
    await server.close().catch(() => {});
  }

  const runtime = {
    wasmSha256: build.wasmSha,
    servedWasmSha256: servedWasm.size === 1 ? [...servedWasm][0] : null,
    clients: counters.clients,
    transportPaths: [...runtimeFacts.transportPaths].sort(),
    rollbackCount: runtimeFacts.rollbackCount,
    maxRollbackDistance: runtimeFacts.maxRollbackDistance,
  };
  let exitCode = 0;
  if (failedCase) {
    exitCode = 1;
    log(`FAILED case ${failedCase.id}: ${failedCase.error}`);
  } else if (servedWasm.size !== 1 || [...servedWasm][0] !== build.wasmSha) {
    exitCode = 1;
    log(`FAILED served wasm identity: ${[...servedWasm].join(',')} vs build ${build.wasmSha}`);
  } else {
    log(`PASS through=${args.through} cases=${reported.length} clients=${counters.clients} ` +
        `wasm=${build.wasmSha.slice(0, 12)}…`);
  }
  const report = {
    schema: 'th13/mp-workflow-test/v1',
    layer: 'browser',
    through: args.through,
    sourceDigest: args.sourceDigest,
    skipped: 0,
    cases: reported,
    runtime,
    ...(failedCase ? {failure: failedCase.error} : {}),
  };
  await mkdir(dirname(resolve(args.report)), {recursive: true});
  await writeFile(args.report, JSON.stringify(report, null, 2) + '\n');
  return exitCode;
}

main().then(code => process.exit(code), error => {
  console.error('acceptance: ' + (error && error.message || error));
  process.exit(1);
});
