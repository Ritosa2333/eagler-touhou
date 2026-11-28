import {createHash} from 'node:crypto';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';
import {verifyRuntimeDelivery} from '../../../../tools/browser/verified-runtime.mjs';
import {installPcmProbe, installSfxStartProbe, samplePcm} from '../../../../tools/browser/audio-output-probe.mjs';
import {startServe, PORT_ROOT, REPO_ROOT} from './browser/serve.mjs';
import {spawn} from 'node:child_process';

const HERE = dirname(fileURLToPath(import.meta.url));
const MAPPING = join(HERE, 'workflow-cases.json');
const BUILD_DIR = join(PORT_ROOT, 'web/artifacts/sdl3');

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

async function selectCases(stage, selectedIds) {
  const mapping = JSON.parse(await readFile(MAPPING, 'utf8'));
  const byId = new Map();
  for (const entry of mapping.cases ?? []) {
    if (!entry || typeof entry.id !== 'string') throw new CaseFailure('mapping entry without id');
    if (entry.layer !== 'browser') continue;
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

async function openClient(ctx, {kind, seed, replay = 'th13_01.rpy', sht = 'pl00a.sht', stage = 0, query = '', init = null} = {}) {
  const {browser, server, build, counters} = ctx;
  const context = await browser.newContext({viewport: {width: 800, height: 560}});
  await context.addInitScript(value => { globalThis.__th13_boot_seed = value; }, seed);
  if (init) await context.addInitScript({content: init});
  await context.addInitScript(installPcmProbe);
  await context.addInitScript(installSfxStartProbe);
  const page = await context.newPage();
  const consoleLines = [];
  page.on('console', message => consoleLines.push(message.type() + ' ' + message.text()));
  page.on('pageerror', error => consoleLines.push('pageerror ' + error.message));
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
            if (ticks === 0) return;
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
            try { mod._th13_loop_pause(1); state.paused = true; } catch (_) {}
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
    resumeAudio: () => page.evaluate(() => window.mod._th13_debug_audio_resume()),
    loopPause: enabled => page.evaluate(v => window.mod._th13_loop_pause(v ? 1 : 0), enabled),
    audioStats: () => page.evaluate(() => ({...(window.mod.th13Audio ?? {})})),
    sfxMark: () => page.evaluate(() => globalThis.__sfxStarts.length),
    sfxSlice: from => page.evaluate(from => globalThis.__sfxStarts.slice(from).map(
      e => ({when: e.when, peak: e.peak, channels: e.channels})), from),
    audioObserveEnable: () => page.evaluate(() => window.mod._th13_debug_audio_observe(1)),
    audioTake: () => page.evaluate(() => window.mod.UTF8ToString(window.mod._th13_debug_audio_take())),
    close: async () => {
      await context.close();
    },
  };
  return client;
}

async function startRelay({inject = {}} = {}) {
  const relayEntry = join(REPO_ROOT, 'services/relay/src/netplay-relay.mjs');
  const relay = spawn(process.execPath, [relayEntry], {
    cwd: REPO_ROOT,
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
          EAGLER_NETPLAY_RELAY_PORT: '0',
          EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1',
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

  const counters = {clients: 0};
  const servedWasm = new Set();
  const runtimeFacts = {transportPaths: new Set(), rollbackCount: 0, maxRollbackDistance: 0};
  const log = (...items) => console.log('[acceptance]', ...items);

  const reported = [];
  let failedCase = null;
  try {
    for (const entry of cases) {
      const startedAt = Date.now();
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
        startRelay: options => startRelay(options),
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
