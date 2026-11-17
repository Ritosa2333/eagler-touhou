#!/usr/bin/env node
// th13-mp MP 工作流原生 runner（M1/T1 测试基建）。
//
// 被 tests/multiplayer/run-native.sh exec 调用（CLI 与外层
// scripts/mp_workflow_gate.py native() 完全一致），也可直接 node 运行：
//   native-runner.mjs --through M<n> --scope current|cumulative
//                     --source-digest <hex> --report <path> [--case <id> ...]
//
// 契约（外层 docs/plans/th13-mp-m1-m9-workflow.md「验证入口与输出契约」、
// ports/th13-mp/artifacts/workflow/M1-plan.md T1）：
//   - --case 只认 tests/multiplayer/workflow-cases.json 映射（无命名约定回退）；
//     映射条目 files 以 PORT=ports/th13-mp/ 为基解析（与 gate:121-123 同则）。
//   - 无 --case 时 current/cumulative 必需固定 case 未登记即非零退出并打印清单。
//   - 报告 schema th13/mp-workflow-test/v1；case 只在真实编译运行且断言通过后
//     标 passed；任一失败整体非零退出。
//   - 编译源集 = game 全量 + e2e 同款 sdl 8 文件 + tests/e2e/native/
//     native_stub.cpp（复用不复制）+ case 套件；flags/宏与 tests/e2e/run.sh
//     完全同款（-DTH13_AUDIO_MIXER_ONLY，freetype 真栅格）。
//   - 缓存键 = 全部源/头/TU 宏/flags 的 sha256（含编译器标识与 freetype
//     配置）；缓存与 SP 目录隔离（<outer>/artifacts/mp-native-cache）。
//   - 运行注入 TH13_NATIVE_ASSET_ROOT / TH13_NATIVE_REPO_ROOT /
//     TH13_NATIVE_FONT（与 tests/e2e/run.sh 同义）；映射条目 "tests" 数组
//     经 TH13_CASE_FILTER 做共享套件 per-case 归属。
import { createHash } from "node:crypto";
import { spawn, spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

// 固定 native case 表——外层 scripts/mp_workflow_gate.py:21-31 NATIVE 的只读
// 镜像（主代理所有，勿在此改语义；两端须逐条一致）。current=本里程碑，
// cumulative=1..stage 并集。
const NATIVE_REQUIRED = {
  1: ["update_present_equivalence", "draw_purity_rng", "scheduler_order", "side_effect_boundary", "focus_sync"],
  2: ["independent_inputs_2_3", "world_tick_once", "four_characters_duplicates", "damage_owner_ids", "single_seat_sp_equivalence"],
  3: ["target_all_consumers_ties", "shared_pickup_once", "resource_all_types_caps", "spirit_role_and_trance_grant", "drop_rng_unchanged", "graze_owner_generation", "boss_damage_remainders", "gift_not_broadcast"],
  4: ["team_trance_shooting_four_roles", "protection_owner_only_death", "gauge_once_thresholds", "all_collision_invulnerability", "simultaneous_requests", "deathbomb_manual_c", "generation_respawn_pause_stage", "peer_bomb_policy"],
  5: ["ghost_rescue_resources", "team_wipe_180", "menu_edges_pause_dialogue", "hud_world_owner_visibility", "no_sp_save_writes", "stage_revive"],
  6: ["full_state_restore_replay", "snapshot_mutation_sensitivity", "pools_ids_free_order", "anm_borrowed_parent_chains", "scheduler_callback_rebind", "dual_rng_events_transition"],
  7: ["contract_asset_mismatch", "frame_zero_load_barrier", "device_seat_isolation", "confirmed_hash_lockstep"],
  8: ["rollback_one_three_twelve", "delay_zero_two_nine", "jitter_loss_reorder_duplicate", "window_stall_conflict", "audio_bgm_once", "trance_death_resimulation", "resim_budget_new_progress"],
  9: ["replay_roundtrip_seek", "spectator_confirmed_latejoin", "slow_spectator_isolation", "stage_entry_carry", "run_generation_isolation", "terminal_local_save", "disconnect_confirmed_save_return"],
};

const HERE = path.dirname(fileURLToPath(import.meta.url));           // tests/multiplayer/lib
const MULTIPLAYER = path.resolve(HERE, "..");                        // tests/multiplayer
const PORT = path.resolve(MULTIPLAYER, "..", "..");                  // ports/th13-mp
const REPO = path.resolve(PORT, "..", "..");                         // eagler-touhou
const OUTER = path.resolve(REPO, "..");                              // 外层仓（games/ artifacts/ 所在）
const MAPPING = path.join(MULTIPLAYER, "workflow-cases.json");

const SDL_SOURCES = ["Application", "ApplicationTitle", "ApplicationGame", "AudioDevice",
  "FileStore", "FontDevice", "GraphicsDevice", "SceneDisplay"]
  .map((name) => path.join(PORT, "web/cpp/sdl", `${name}.cpp`));
const STUB_SOURCE = path.join(PORT, "tests/e2e/native/native_stub.cpp");

const die = (message) => {
  console.error(`native-runner: ${message}`);
  process.exit(1);
};

const sha256 = (buf) => createHash("sha256").update(buf).digest("hex");

// ---- CLI（与 gate native() 传参逐字一致）----
function parseArgs(argv) {
  const out = { through: null, scope: null, sourceDigest: null, report: null, cases: [] };
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    const value = () => {
      if (i + 1 >= argv.length) die(`${arg} needs a value`);
      return argv[++i];
    };
    switch (arg) {
      case "--through": out.through = value(); break;
      case "--scope": out.scope = value(); break;
      case "--source-digest": out.sourceDigest = value(); break;
      case "--report": out.report = value(); break;
      case "--case": out.cases.push(value()); break;
      default: die(`unknown argument: ${arg}`);
    }
  }
  if (!/^M[1-9]$/.test(out.through || "")) die("--through must be M1..M9");
  if (out.scope !== "current" && out.scope !== "cumulative") die("--scope must be current|cumulative");
  if (!/^[0-9a-f]{64}$/.test(out.sourceDigest || "")) die("--source-digest must be 64 hex chars");
  if (!out.report) die("--report is required");
  return out;
}

// ---- 映射（唯一真值；files 以 PORT 为基，gate:121-123 同则校验）----
// gate 的固定表 NATIVE/BROWSER 允许同一 id 跨层并存（如 m1.draw_purity_rng
// 同时有 native/browser 条目；gate:117 `if c.get("layer") == layer` 先分层再
// 键 id）。本 runner 同样必须分层键控：平铺 Map<id> 会让后读的 browser 条目
// 覆盖同名 native 条目，native 侧误报 unmapped/missing（本轮 fast 实证的
// 假失败面）。同层内重复 id 是映射错误，直接非零退出。
function loadMapping() {
  if (!fs.existsSync(MAPPING)) die(`missing rule-to-test case map: ${MAPPING}`);
  let data;
  try {
    data = JSON.parse(fs.readFileSync(MAPPING, "utf8"));
  } catch (err) {
    die(`mapping is not valid JSON: ${err.message}`);
  }
  const byLayer = {native: new Map(), browser: new Map()};
  for (const entry of data.cases ?? []) {
    if (!entry || typeof entry.id !== "string") die("mapping entry without id");
    if (entry.layer !== "native" && entry.layer !== "browser")
      die(`mapping entry ${entry.id} has invalid layer: ${entry.layer}`);
    const map = byLayer[entry.layer];
    if (map.has(entry.id)) die(`duplicate ${entry.layer} mapping entry: ${entry.id}`);
    map.set(entry.id, entry);
  }
  return byLayer;
}

function resolveCaseFile(portRelative) {
  const resolved = path.resolve(PORT, portRelative);
  const portRoot = path.resolve(PORT);
  if (!resolved.startsWith(portRoot + path.sep))
    die(`invalid case test file (outside PORT): ${portRelative}`);
  if (!fs.existsSync(resolved) || !fs.statSync(resolved).isFile())
    die(`invalid case test file (not a file): ${portRelative}`);
  return resolved;
}

function selectCases(args, mapping) {
  const nativeEntry = (id) => {
    // 只查 native 层；同名 browser 条目与本 runner 无关（acceptance.mjs 消费）。
    const entry = mapping.native.get(id);
    if (!entry) {
      if (mapping.browser.has(id))
        die(`case ${id} is registered only as layer=browser, not native`);
      return null;
    }
    if (!Array.isArray(entry.files) || entry.files.length === 0)
      die(`case ${id} has no files in the mapping`);
    return entry;
  };
  if (args.cases.length > 0) {
    const selected = [];
    for (const id of args.cases) {
      const entry = nativeEntry(id);
      if (!entry) die(`unmapped case (workflow-cases.json is the only truth): ${id}`);
      selected.push({ id, entry });
    }
    return selected;
  }
  const stage = Number(args.through.slice(1));
  const stages = args.scope === "cumulative"
    ? Array.from({ length: stage }, (_, i) => i + 1)
    : [stage];
  // NATIVE_REQUIRED 存的是 gate 表内的固定语义名（不带 m<n>. 前缀），逐阶段拼前缀。
  const requiredIds = stages.flatMap((s) => NATIVE_REQUIRED[s].map((name) => `m${s}.${name}`));
  const missing = requiredIds.filter((id) => !mapping.native.get(id));
  if (missing.length > 0) {
    console.error(`native-runner: missing required native cases (scope=${args.scope}):`);
    for (const id of missing) console.error(`  - ${id}`);
    process.exit(1);
  }
  return requiredIds.map((id) => ({ id, entry: mapping.native.get(id) }));
}

// ---- freetype 配置（tests/e2e/run.sh:39-56 同款解析）----
function freetypeConfig() {
  const tryPkgConfig = (flag) => {
    const result = spawnSync("pkg-config", [flag, "freetype2"], { encoding: "utf8" });
    return result.status === 0 ? (result.stdout ?? "").trim() : "";
  };
  let cflags = tryPkgConfig("--cflags");
  if (!cflags) {
    // 与 tests/e2e/run.sh:41-48 同款回退（artifacts 内的 emscripten 缓存副本），
    // 外加当前工作区 .cache/emscripten 缓存直连（同为 ignored 目录）。
    const candidates = [
      path.join(OUTER, "artifacts/pr159-fix-20261104/emscripten-cache/sysroot/include/freetype2"),
      path.join(OUTER, ".cache/emscripten/sysroot/include/freetype2"),
    ];
    const dir = candidates.find((candidate) => fs.existsSync(path.join(candidate, "ft2build.h")));
    if (!dir) die(`freetype2 headers not found (pkg-config nor ${candidates.join(" / ")})`);
    cflags = `-I${dir}`;
  }
  let libs = tryPkgConfig("--libs");
  if (!libs) {
    const ldconfig = spawnSync("ldconfig", ["-p"], { encoding: "utf8" });
    const listed = ldconfig.status === 0 && (ldconfig.stdout ?? "").includes("libfreetype.so ");
    libs = listed ? "-lfreetype" : "-l:libfreetype.so.6";
  }
  return { cflags, libs };
}

// ---- 缓存键：全部源/头/TU 宏/flags 的 sha256 ----
function listFiles(root) {
  const out = [];
  const walk = (dir) => {
    for (const item of fs.readdirSync(dir, { withFileTypes: true })) {
      if (item.name === "artifacts" || item.name === "build" || item.name === "node_modules" ||
          item.name === ".git" || item.name.startsWith(".cache")) continue;
      const full = path.join(dir, item.name);
      if (item.isDirectory()) walk(full);
      else if (item.isFile()) out.push(full);
    }
  };
  if (fs.existsSync(root)) walk(root);
  return out;
}

function computeKey(compiler, flags, defines, includeDirs, ft, sharedSources, caseFiles) {
  const digest = createHash("sha256");
  const push = (text) => digest.update(`${text}\0`);
  push(`compiler=${compiler}`);
  push(`flags=${flags.join("\u001f")}`);
  push(`defines=${defines.join("\u001f")}`);
  push(`includes=${includeDirs.join("\u001f")}`);
  push(`ft=${ft.cflags}\u001f${ft.libs}`);
  const inputs = new Map();
  const addFile = (file) => {
    const rel = path.relative(PORT, file);
    if (!inputs.has(rel)) inputs.set(rel, sha256(fs.readFileSync(file)));
  };
  for (const file of sharedSources) addFile(file);
  for (const file of caseFiles) addFile(file);
  // 头与其它可影响 TU 的输入面：web/cpp 全树、common/include、e2e native 桩、
  // 测试框架与 multiplayer 测试树。
  for (const root of [path.join(PORT, "web/cpp"), path.join(REPO, "common/include"),
                      path.join(PORT, "tests/e2e/native"), path.join(PORT, "tests/harness"),
                      MULTIPLAYER]) {
    for (const file of listFiles(root)) addFile(file);
  }
  for (const rel of [...inputs.keys()].sort()) push(`${rel}\u001f${inputs.get(rel)}`);
  return digest.digest("hex");
}

async function compilePool(jobs, limit) {
  let index = 0;
  const failures = [];
  const worker = async () => {
    while (index < jobs.length && failures.length === 0) {
      const job = jobs[index++];
      await new Promise((resolve) => {
        const child = spawn(job.argv[0], job.argv.slice(1), { stdio: ["ignore", "pipe", "pipe"] });
        let stderr = "";
        child.stderr.on("data", (chunk) => { stderr += chunk; });
        child.on("close", (code) => {
          if (code !== 0) failures.push({ label: job.label, code, stderr });
          resolve();
        });
        child.on("error", (err) => {
          failures.push({ label: job.label, code: -1, stderr: String(err) });
          resolve();
        });
      });
    }
  };
  await Promise.all(Array.from({ length: Math.max(1, limit) }, worker));
  return failures;
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const reportPath = path.resolve(args.report);
  if (fs.existsSync(reportPath)) die(`refusing to overwrite existing report: ${reportPath}`);
  const mapping = loadMapping();
  const selected = selectCases(args, mapping);

  // 资产链哨兵（tests/e2e/run.sh:34-37 同款硬失败面；T1 retarget 后应全可解）。
  const fontFile = path.join(PORT, "web/sdl-runtime/assets/msgothic.ttc");
  const assetRoot = path.join(OUTER, "artifacts/reference/raw");
  if (!fs.existsSync(fontFile)) die(`missing real asset: ${fontFile}`);
  if (!fs.existsSync(assetRoot)) die(`missing real asset root: ${assetRoot}`);

  const compiler = "g++";
  const version = spawnSync(compiler, ["--version"], { encoding: "utf8" });
  if (version.status !== 0) die("g++ not available");
  const compilerId = (version.stdout ?? "").split("\n")[0].trim();

  const flags = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-fno-exceptions", "-fno-rtti", "-ffp-contract=off"];
  // TH13_MULTIPLAYER_DEBUG_ROLLBACK：ExecuteRollback 失败路径的 stderr
  // 定位打印（native 诊断面；生产构建不定义）。
  const defines = ["-DTH13_AUDIO_MIXER_ONLY", "-DTH13_MULTIPLAYER_DEBUG_ROLLBACK"];
  const includeDirs = [
    `-I${path.join(PORT, "web/cpp")}`,
    `-I${path.join(REPO, "common/include")}`,
    `-I${path.join(PORT, "tests/e2e/native/include")}`,
    // 共用网络栈的作品 config 头（<netplay/Netplay{Transport,Protocol}Config.hpp>）。
    `-I${path.join(PORT, "web/cpp/multiplayer")}`,
  ];
  const ft = freetypeConfig();
  const ftCflags = ft.cflags.split(/\s+/).filter(Boolean);
  const linkLibs = [...ft.libs.split(/\s+/).filter(Boolean), "-lm"];

  const sharedSources = [
    ...fs.readdirSync(path.join(PORT, "web/cpp/game"))
      .filter((name) => name.endsWith(".cpp")).sort()
      .map((name) => path.join(PORT, "web/cpp/game", name)),
    ...fs.readdirSync(path.join(PORT, "web/cpp/multiplayer"))
      .filter((name) => name.endsWith(".cpp")).sort()
      .map((name) => path.join(PORT, "web/cpp/multiplayer", name)),
    // 共用网络栈（M7）：common/src/netplay 直接编译（共用不 fork）。
    // BrowserPeerTransport 的 JS 桥在非 Emscripten 下为空实现——native
    // 测试用 stub transport（PeerTransport 接口）驱动纯逻辑。
    ...fs.readdirSync(path.join(REPO, "common/src/netplay"))
      .filter((name) => name.endsWith(".cpp")).sort()
      .map((name) => path.join(REPO, "common/src/netplay", name)),
    ...SDL_SOURCES,
    STUB_SOURCE,
  ];
  for (const file of sharedSources) {
    if (!fs.existsSync(file)) die(`missing compile source: ${file}`);
  }
  // case 文件并入缓存键与编译集（全部源）。键面取映射内全部 native 条目的
  // 现存文件（键与所选 case 无关，同一源状态共享同一缓存键）；所选 case 的
  // 文件仍严格逐个解析（缺失即非零）。
  const universeCaseSources = new Set();
  for (const entry of mapping.native.values()) {
    if (!Array.isArray(entry.files)) continue;
    for (const rel of entry.files) {
      const abs = path.resolve(PORT, rel);
      const portRoot = path.resolve(PORT);
      if (abs.startsWith(portRoot + path.sep) && fs.existsSync(abs) &&
          fs.statSync(abs).isFile()) {
        universeCaseSources.add(abs);
      }
    }
  }
  const caseFiles = new Map(); // caseId -> [abs]
  for (const { id, entry } of selected) {
    caseFiles.set(id, entry.files.map(resolveCaseFile));
  }
  const allCaseSources = [...universeCaseSources];

  const key = computeKey(compilerId, [...flags, ...defines], defines, [...includeDirs, ...ftCflags], ft,
                         sharedSources, allCaseSources);
  const cacheRoot = process.env.TH13_NATIVE_RUNNER_CACHE || path.join(OUTER, "artifacts/mp-native-cache");
  const keyDir = path.join(cacheRoot, key);
  const objDir = path.join(keyDir, "obj");
  const binDir = path.join(keyDir, "bin");
  fs.mkdirSync(objDir, { recursive: true });
  fs.mkdirSync(binDir, { recursive: true });

  // Existing caches and evidence belong to earlier work. Never prune them as
  // a side effect of verification; each invocation reserves fresh output paths.

  const objFor = (file) => {
    const rel = path.relative(PORT, file).replaceAll(path.sep, "__");
    return path.join(objDir, `${rel}.o`);
  };
  const compileJobs = [];
  for (const file of [...sharedSources, ...allCaseSources]) {
    const obj = objFor(file);
    if (fs.existsSync(obj)) continue;
    const tmp = `${obj}.tmp-${process.pid}`;
    compileJobs.push({
      label: path.relative(PORT, file),
      argv: [compiler, ...flags, ...defines, ...includeDirs, ...ftCflags, "-c", file, "-o", tmp],
      tmp, obj,
    });
  }
  if (compileJobs.length > 0) {
    console.error(`native-runner: compiling ${compileJobs.length} TU (key=${key.slice(0, 12)}…, pool=${Math.min(4, os.cpus().length)})`);
    const failures = await compilePool(compileJobs, Math.min(4, os.cpus().length));
    for (const failure of failures) {
      console.error(`native-runner: compile failed (${failure.label}, exit ${failure.code})`);
      console.error(failure.stderr);
    }
    if (failures.length > 0) {
      for (const job of compileJobs) {
        try { fs.rmSync(job.tmp, { force: true }); } catch { /* 尽力清理 */ }
      }
      process.exit(1);
    }
    for (const job of compileJobs) fs.renameSync(job.tmp, job.obj);
  }

  const outputBase = process.env.TH13_NATIVE_OUTPUT_ROOT || path.join(OUTER, "artifacts/mp-native-run");
  fs.mkdirSync(outputBase, { recursive: true });
  const runRoot = fs.mkdtempSync(path.join(outputBase, "run-"));
  const logsDir = path.join(runRoot, "logs");
  fs.mkdirSync(logsDir, { recursive: true });
  const timeoutSeconds = Number.parseInt(process.env.TH13_NATIVE_TIMEOUT_SECONDS ?? "", 10) || 7200;

  const results = [];
  for (const { id, entry } of selected) {
    const bin = path.join(binDir, `${id.replaceAll(/[^a-z0-9_]/gi, "_")}.out`);
    if (!fs.existsSync(bin)) {
      const tmp = `${bin}.tmp-${process.pid}`;
      const objects = [...sharedSources, ...caseFiles.get(id)].map(objFor);
      const link = spawnSync(compiler, [...objects, "-o", tmp, ...linkLibs], { stdio: ["ignore", "pipe", "pipe"], encoding: "utf8" });
      if (link.status !== 0) {
        console.error(`native-runner: link failed for ${id}`);
        console.error(link.stderr ?? "");
        process.exit(1);
      }
      fs.renameSync(tmp, bin);
    }

    // 运行：独立 cwd（生产存档/导出写 CWD，不污染仓树），环境与 e2e 同义。
    const runDir = path.join(runRoot, id.replaceAll(/[^a-z0-9_]/gi, "_"));
    fs.mkdirSync(runDir);
    const logPath = path.join(logsDir, `${id.replaceAll(/[^a-z0-9_]/gi, "_")}.log`);
    const env = {
      ...process.env,
      TH13_NATIVE_REPO_ROOT: OUTER,
      TH13_NATIVE_ASSET_ROOT: assetRoot,
      TH13_NATIVE_FONT: fontFile,
      // 测试产物（存档/trace）显式落点：<outer>/artifacts/mp-native-run/<case>。
      // case 文件用 TH13_NATIVE_RUN_DIR 拼输出路径；缺省仍以 cwd=runDir 兜底。
      TH13_NATIVE_RUN_DIR: runDir,
    };
    const requestedBlocks = Array.isArray(entry.tests) ? entry.tests.map(String) : [];
    if (requestedBlocks.length > 0) env.TH13_CASE_FILTER = requestedBlocks.join(",");

    const started = Date.now();
    const outcome = await new Promise((resolve) => {
      const child = spawn(bin, [], { cwd: runDir, env, stdio: ["ignore", "pipe", "pipe"] });
      let stdout = "", stderr = "";
      child.stdout.on("data", (chunk) => { stdout += chunk; });
      child.stderr.on("data", (chunk) => { stderr += chunk; });
      const timer = setTimeout(() => {
        child.kill("SIGKILL");
        resolve({ code: -9, stdout, stderr: `${stderr}\nnative-runner: killed after ${timeoutSeconds}s timeout` });
      }, timeoutSeconds * 1000);
      child.on("close", (code) => { clearTimeout(timer); resolve({ code, stdout, stderr }); });
      child.on("error", (err) => { clearTimeout(timer); resolve({ code: -1, stdout, stderr: String(err) }); });
    });
    const elapsedMs = Date.now() - started;
    fs.writeFileSync(logPath, `# binary: ${bin}\n# exit: ${outcome.code}\n# elapsed_ms: ${elapsedMs}\n`
      + `# cwd: ${runDir}\n# case_filter: ${env.TH13_CASE_FILTER ?? ""}\n--- stdout ---\n${outcome.stdout}`
      + (outcome.stderr ? `--- stderr ---\n${outcome.stderr}` : ""));

    // 解析逐块断言（test_framework.hpp 输出：ok/not ok 行 + 其后 "# assertions: N"）。
    const blocks = [];
    let lastBlock = null;
    for (const line of outcome.stdout.split("\n")) {
      const okMatch = line.match(/^(not )?ok \d+ - (.+)$/);
      if (okMatch) {
        lastBlock = { name: okMatch[2], failed: Boolean(okMatch[1]), assertions: 0 };
        blocks.push(lastBlock);
        continue;
      }
      const assertMatch = line.match(/^# assertions: (\d+)$/);
      if (assertMatch && lastBlock) lastBlock.assertions = Number(assertMatch[1]);
    }
    const totalAssertions = blocks.reduce((sum, block) => sum + block.assertions, 0);
    const ranAllRequested = requestedBlocks.every((name) => blocks.some((block) => block.name === name));
    const passed = outcome.code === 0 && totalAssertions > 0 && ranAllRequested
      && blocks.every((block) => !block.failed) && !outcome.stdout.includes("  失败 ");
    const evidence = [
      `binary=${path.relative(OUTER, bin)}`,
      `exit=${outcome.code} assertions=${totalAssertions} blocks=${blocks.length} elapsed_ms=${elapsedMs}`,
      `run_dir=${path.relative(OUTER, runDir)} log=${path.relative(OUTER, logPath)}`,
      ...blocks.map((block) => `${block.failed ? "not ok" : "ok"}: ${block.name} assertions=${block.assertions}`),
    ];
    if (!passed && outcome.code !== 0) evidence.push(`stderr_tail: ${outcome.stderr.trim().slice(-400)}`);
    results.push({ id, passed, assertions: totalAssertions, evidence });
    console.error(`native-runner: ${id} -> ${passed ? "PASS" : "FAIL"} (exit=${outcome.code}, assertions=${totalAssertions}, ${elapsedMs}ms)`);
  }

  const allPassed = results.every((result) => result.passed);
  const report = {
    schema: "th13/mp-workflow-test/v1",
    layer: "native",
    through: args.through,
    sourceDigest: args.sourceDigest,
    skipped: 0,
    cases: results,
  };
  fs.mkdirSync(path.dirname(reportPath), { recursive: true });
  fs.writeFileSync(reportPath, `${JSON.stringify(report, null, 2)}\n`, { flag: "wx" });
  console.error(`native-runner: report=${reportPath} passed=${results.filter((r) => r.passed).length}/${results.length}`);
  process.exit(allPassed ? 0 : 1);
}

main().catch((err) => {
  console.error(`native-runner: ${err && err.stack ? err.stack : err}`);
  process.exit(1);
});
