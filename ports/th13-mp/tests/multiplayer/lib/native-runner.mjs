#!/usr/bin/env node
import { createHash } from "node:crypto";
import { spawn, spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

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

const HERE = path.dirname(fileURLToPath(import.meta.url));
const MULTIPLAYER = path.resolve(HERE, "..");
const PORT = path.resolve(MULTIPLAYER, "..", "..");
const REPO = path.resolve(PORT, "..", "..");
const OUTER = path.resolve(REPO, "..");
const MAPPING = path.join(MULTIPLAYER, "workflow-cases.json");

const SDL_SOURCES = ["Application", "ApplicationTitle", "ApplicationGame", "AudioDevice",
  "FileStore", "FontDevice", "GraphicsDevice", "LanguagePack", "SceneDisplay"]
  .map((name) => path.join(PORT, "web/cpp/sdl", `${name}.cpp`));
const STUB_SOURCE = path.join(PORT, "tests/e2e/native/native_stub.cpp");

const die = (message) => {
  console.error(`native-runner: ${message}`);
  process.exit(1);
};

const sha256 = (buf) => createHash("sha256").update(buf).digest("hex");

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
  const requiredIds = stages.flatMap((s) => NATIVE_REQUIRED[s].map((name) => `m${s}.${name}`));
  const missing = requiredIds.filter((id) => !mapping.native.get(id));
  if (missing.length > 0) {
    console.error(`native-runner: missing required native cases (scope=${args.scope}):`);
    for (const id of missing) console.error(`  - ${id}`);
    process.exit(1);
  }
  return requiredIds.map((id) => ({ id, entry: mapping.native.get(id) }));
}

function freetypeConfig() {
  const tryPkgConfig = (flag) => {
    const result = spawnSync("pkg-config", [flag, "freetype2"], { encoding: "utf8" });
    return result.status === 0 ? (result.stdout ?? "").trim() : "";
  };
  let cflags = tryPkgConfig("--cflags");
  if (!cflags) {
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

  const fontFile = path.join(PORT, "web/sdl-runtime/assets/msgothic.ttc");
  const assetRoot = path.join(OUTER, "artifacts/reference/raw");
  if (!fs.existsSync(fontFile)) die(`missing real asset: ${fontFile}`);
  if (!fs.existsSync(assetRoot)) die(`missing real asset root: ${assetRoot}`);

  const compiler = "g++";
  const version = spawnSync(compiler, ["--version"], { encoding: "utf8" });
  if (version.status !== 0) die("g++ not available");
  const compilerId = (version.stdout ?? "").split("\n")[0].trim();

  const flags = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-fno-exceptions", "-fno-rtti", "-ffp-contract=off"];
  const defines = ["-DTH13_AUDIO_MIXER_ONLY", "-DTH13_MULTIPLAYER_DEBUG_ROLLBACK"];
  const includeDirs = [
    `-I${path.join(PORT, "web/cpp")}`,
    `-I${path.join(REPO, "common/include")}`,
    `-I${path.join(PORT, "tests/e2e/native/include")}`,
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
    ...fs.readdirSync(path.join(REPO, "common/src/netplay"))
      .filter((name) => name.endsWith(".cpp") && !/^(Native|DesktopSwarmTurn)/.test(name)).sort()
      .map((name) => path.join(REPO, "common/src/netplay", name)),
    ...SDL_SOURCES,
    STUB_SOURCE,
  ];
  for (const file of sharedSources) {
    if (!fs.existsSync(file)) die(`missing compile source: ${file}`);
  }
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
  const caseFiles = new Map();
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
        try { fs.rmSync(job.tmp, { force: true }); } catch {}
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

    const runDir = path.join(runRoot, id.replaceAll(/[^a-z0-9_]/gi, "_"));
    fs.mkdirSync(runDir);
    const logPath = path.join(logsDir, `${id.replaceAll(/[^a-z0-9_]/gi, "_")}.log`);
    const env = {
      ...process.env,
      TH13_NATIVE_REPO_ROOT: OUTER,
      TH13_NATIVE_ASSET_ROOT: assetRoot,
      TH13_NATIVE_FONT: fontFile,
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
