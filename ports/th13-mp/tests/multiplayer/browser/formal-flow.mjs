// Production Launcher/Relay full-run acceptance. Start the repository Launcher
// and relay first; TH13_ORIGIN, TH13_CASE, TH13_ROUTE, TH13_RTT, TH13_DIFFICULTY
// select the matrix cell. Outputs remain in ignored artifacts. No state injection.
import { createRequire } from "node:module";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
const require = createRequire(import.meta.url);
const { chromium } = require(
  process.env.PLAYWRIGHT_MODULE || "/opt/shot/node_modules/playwright-core",
);
import { writeFile, readFile, readlink } from "node:fs/promises";
import { createHash } from "node:crypto";
import assert from "node:assert/strict";
import { installPilot } from "./formal-pilot.mjs";
import { installWireConditions } from "./formal-network.mjs";
import { runRoomLifecycle } from "./formal-lifecycle.mjs";
import { runDirectedGift } from "./formal-cooperation.mjs";
import { runInputEdges } from "./formal-input.mjs";
import { startCpuProfiles } from "./formal-profile.mjs";
import { appendFile, mkdir } from "node:fs/promises";
const workspace = fileURLToPath(new URL("../../../../../../", import.meta.url));
const dir = resolve(
  process.env.TH13_FLOW_ARTIFACTS ||
    resolve(workspace, "artifacts/th13-mp-formal"),
  process.env.TH13_CASE || "normal-rtc-0",
);
const origin = process.env.TH13_ORIGIN || "http://127.0.0.1:18760";
await mkdir(resolve(dir, ".."), { recursive: true });
await mkdir(dir); // A rerun must not append to evidence from a previous run.
const expectedFiles = await Promise.all(
  ["th13-sdl.mjs", "th13-sdl.wasm"].map(async (name) => {
    const bytes = await readFile(
      new URL("../../../build-eagler-multiplayer/" + name, import.meta.url),
    );
    return {
      name,
      bytes: bytes.length,
      sha256: createHash("sha256").update(bytes).digest("hex"),
    };
  }),
);
const report = {
  started: new Date().toISOString(),
  errors: [],
  events: [],
  delivery: [],
  expectedFiles,
  snapshots: [],
  conditions: Object.fromEntries(["TH13_ROUTE", "TH13_RTT", "TH13_JITTER", "TH13_LOSS",
    "TH13_DUPLICATE_EVERY", "TH13_BURST_EVERY", "TH13_BURST_MS", "TH13_DIFFICULTY"]
    .map(key => [key, process.env[key] || "0"])),
};
await Promise.all(["formal-flow.mjs", "formal-pilot.mjs", "formal-network.mjs", "formal-lifecycle.mjs", "formal-cooperation.mjs", "formal-input.mjs", "formal-profile.mjs"].map(async name => {
  const bytes = await readFile(new URL(name, import.meta.url));
  await writeFile(resolve(dir, name), bytes);
}));
const launchOptions = {
  headless: process.env.TH13_HEADFUL !== "1",
  channel: "chromium",
  args: [
    "--no-sandbox",
    "--disable-dev-shm-usage",
    "--ignore-gpu-blocklist",
    "--disable-gpu-sandbox",
    process.env.TH13_ANGLE === "gl"
      ? "--use-angle=gl"
      : "--use-angle=swiftshader",
    "--enable-unsafe-swiftshader",
    "--autoplay-policy=no-user-gesture-required",
    "--enable-logging=stderr",
    "--disable-background-timer-throttling",
    "--disable-backgrounding-occluded-windows",
    "--disable-renderer-backgrounding",
  ],
};
const PLAYER_COUNT = Number(process.env.TH13_PLAYERS || 2);
if (![2, 3].includes(PLAYER_COUNT)) throw Error("TH13_PLAYERS must be 2 or 3");
const sharedBrowser = process.env.TH13_SHARED_BROWSER === "1";
const browsers = [];
report.browserTopology = { sharedBrowser, launchOptions, seats: [], snapshots: [] };
const pages = [];
let profileCapture;
async function captureBrowserIdentity(browser, browserIndex, phase) {
  const session = await browser.newBrowserCDPSession();
  try {
    const [version, system, { processInfo }] = await Promise.all([
      session.send("Browser.getVersion"),
      session.send("SystemInfo.getInfo"),
      session.send("SystemInfo.getProcessInfo"),
    ]);
    const processes = await Promise.all(processInfo
      .filter(info => ["browser", "GPU", "renderer"].includes(info.type))
      .map(async info => ({
        ...info,
        executable: await readlink(`/proc/${info.id}/exe`),
        arguments: (await readFile(`/proc/${info.id}/cmdline`, "utf8"))
          .split("\0").filter(Boolean),
      })));
    const identity = {
      phase, at: new Date().toISOString(), browserIndex,
      seats: report.browserTopology.seats.filter(row => row.browserIndex === browserIndex)
        .map(row => row.seat),
      version, gpu: system.gpu, processes,
    };
    report.browserTopology.snapshots.push(identity);
    await appendFile(dir + "/browser-identity.ndjson", JSON.stringify(identity) + "\n");
    return identity;
  } finally {
    await session.detach();
  }
}
const inspect = (page) =>
  page.evaluate((playerCount) => {
    const frame = document.querySelector("#gameFrame"),
      w = frame?.contentWindow,
      m = w?.Module;
    const read = (name) => {
      if (!m?.[name]) return null;
      const text = m.UTF8ToString(m[name]());
      return text ? JSON.parse(text) : { empty: true };
    };
    const tail = w?.__formalTail;
    const audit = read("_th13_mp_audit_take"), timing = read("_th13_mp_timing_take");
    if (tail) {
      audit.frames = [...tail.audit.frames, ...audit.frames];
      audit.overflow += tail.audit.overflow;
      audit.terminal ||= tail.audit.terminal;
      timing.events = [...tail.timing.events, ...timing.events];
      timing.overflow += tail.timing.overflow;
      w.__formalTail = null;
    }
    return {
      url: location.href,
      frame: frame?.src,
      visibility: document.visibilityState,
      wire: w?.__wireConditions ? {...w.__wireConditions, records: undefined} : null,
      roomVisible: !!document.querySelector("#mpRoomView .room-workspace-header")?.getClientRects().length,
      readyEnabled: !!document.querySelector("#mpReady:not(:disabled)")?.getClientRects().length,
      contextLoss: w?.__contextLoss,
      nativeError: m?.UTF8ToString(m?._th13_error?.() || 0),
      rect: frame
        ? {
            ...Object.fromEntries(
              ["x", "y", "width", "height"].map((k) => [
                k,
                frame.getBoundingClientRect()[k],
              ]),
            ),
          }
        : null,
      raf: w.__rafProbe,
      health: w.__health,
      canvas: w.document.querySelector("canvas")
        ? {
            width: w.document.querySelector("canvas").width,
            height: w.document.querySelector("canvas").height,
          }
        : null,
      pilot: w.__pilot,
      probe: m
        ? Array.from(
            m.HEAP32.subarray(
              m._th13_probe_state() >>> 2,
              (m._th13_probe_state() >>> 2) + 16,
            ),
          )
        : null,
      room: JSON.parse(
        sessionStorage.getItem("eagler-touhou-th13mp-room-v1") || "null",
      ),
      text: document.body.innerText.slice(-6000),
      runtime: m
        ? {
            net: read("_th13_debug_mp_net_state"),
            pending: read("_th13_debug_pending"),
            pause: read("_th13_debug_pause_state"),
            result: read("_th13_debug_result_state"),
            audit,
            timing,
            diagnostic: read("_th13_mp_diagnostic_report"),
            rollbackMetrics: read("_th13_debug_mp_net_rollback"),
            hostDetail: m._th13_debug_mp_host_state
              ? m.UTF8ToString(m._th13_debug_mp_host_state()) : null,
            rollback: Array.from(
              m.HEAPU32.subarray(
                m._th13_mp_rollback_status() >>> 2,
                (m._th13_mp_rollback_status() >>> 2) + 9,
              ),
            ),
            stageFrame: m._th13_debug_stage_frame(),
            hostFrame: m._th13_frame(),
            seats: Array.from({length: playerCount}, (_, i) =>
              JSON.parse(m.UTF8ToString(m._th13_debug_mp_seat_state(i))),
            ),
            error: m.UTF8ToString(m._th13_mp_error()),
            route: w.__eaglerPeerTransport?.route,
            options: m.eaglerOptions,
          }
        : null,
    };
  }, PLAYER_COUNT);
async function dismiss(page) {
  await page.waitForFunction(
    () =>
      localStorage.getItem("eagler-touhou-first-use-notice-seen-v1") === "1" ||
      document.querySelector("#firstUseNoticeDialog")?.open === true,
  );
  if (await page.locator("#decisionDialog[open]:not(.closing)").count())
    await page.locator("#decisionConfirm").click();
}
try {
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const browser = sharedBrowser && browsers.length
      ? browsers[0] : await chromium.launch(launchOptions);
    if (!browsers.includes(browser)) browsers.push(browser);
    const browserIndex = browsers.indexOf(browser);
    report.browserTopology.seats.push({ seat, browserIndex });
    const context = await browser.newContext({
      viewport: { width: 1280, height: 900 },
      screen: { width: 640, height: 480 },
      hasTouch: true,
      isMobile: true,
    });
    if (process.env.TH13_RELAY_PORT)
      await context.addInitScript((port) => {
        const Original = WebSocket;
        globalThis.WebSocket = class extends Original {
          constructor(url, ...args) {
            super(String(url).replace(":18761", ":" + port), ...args);
          }
        };
      }, process.env.TH13_RELAY_PORT);
    await context.addInitScript(installWireConditions, {
      route: process.env.TH13_ROUTE || "rtc",
      rtt: +(process.env.TH13_RTT || 0),
      jitter: +(process.env.TH13_JITTER || 0),
      loss: +(process.env.TH13_LOSS || 0),
      duplicateEvery: +(process.env.TH13_DUPLICATE_EVERY || 0),
      burstEvery: +(process.env.TH13_BURST_EVERY || 0),
      burstMs: +(process.env.TH13_BURST_MS || 0),
    });
    await context.addInitScript(() => {
      localStorage.setItem(
        "eagler-touhou-game-options-v1-th13mp",
        JSON.stringify({ options: { renderScale: 1, thpracEnabled: false } }),
      );
      localStorage.setItem(
        "eagler-touhou-game-options-v1-th13",
        JSON.stringify({ options: { renderScale: 1, thpracEnabled: false } }),
      );
      window.addEventListener(
        "webglcontextlost",
        (e) => {
          window.__contextLoss = {
            message: e.statusMessage,
            t: performance.now(),
          };
        },
        true,
      );
      const original = window.requestAnimationFrame;
      window.__rafProbe = { calls: 0, scheduled: 0, first: 0, last: 0 };
      window.requestAnimationFrame = function (cb) {
        ++window.__rafProbe.scheduled;
        return original.call(this, (t) => {
          const p = window.__rafProbe;
          ++p.calls;
          p.first ||= t;
          p.last = t;
          cb(t);
        });
      };
    });
    const page = await context.newPage();
    pages.push(page);
    page.setDefaultTimeout(10000);
    await page.addLocatorHandler(page.locator("#firstUseNoticeDialog[open]"),
      async () => { await page.locator("#firstUseNoticeClose").click(); });
    page.on("requestfailed", (r) =>
      report.errors.push({
        seat,
        url: r.url(),
        message: r.failure()?.errorText,
      }),
    );
    page.on("response", (r) => {
      if (r.status() >= 400)
        report.errors.push({ seat, url: r.url(), status: r.status() });
    });
    page.on("pageerror", (e) =>
      report.errors.push({ seat, message: e.message, stack: e.stack }),
    );
    page.on("console", (m) => {
      if (m.type() === "error") report.errors.push({ seat, message: m.text() });
    });
    page.on("response", async (response) => {
      if (/th13-sdl\.(mjs|wasm)(\?|$)/.test(response.url())) {
        const bytes = await response.body();
        report.delivery.push({
          seat,
          url: response.url(),
          bytes: bytes.length,
          sha256: createHash("sha256").update(bytes).digest("hex"),
        });
      }
    });
    await page.addInitScript(() => {
      window.addEventListener("message", (e) => {
        if (e.data?.type || e.data?.event) {
          window.__health ||= [];
          if (window.__health.length < 100) window.__health.push(e.data);
        }
      });
    });
    await page.goto(origin + "/?renderScale=1");
    await page.waitForFunction(
      () => window.__eaglerBoot?.done === true,
      {},
      { timeout: 60000 },
    );
    await dismiss(page);
    await page.getByRole("link", { name: "联机", exact: true }).click();
    await page.goto(origin + "/lobby.html?game=th13mp");
    await page.waitForFunction(
      () => !document.querySelector("#createButton")?.disabled,
    );
    await dismiss(page);
    await page.locator(seat ? "#codeButton" : "#createButton").click();
    if (!seat) {
      await page.locator("#capacitySelect").selectOption(String(PLAYER_COUNT));
      await page
        .locator("#difficultySelect")
        .selectOption(process.env.TH13_DIFFICULTY || "1");
    } else await page.locator("#roomCodeInput").fill(String(report.room));
    await page.locator("#submitRoom").click();
    await page.waitForSelector("#mpRoomView:not([hidden])", { timeout: 60000 });
    await dismiss(page);
    await page.evaluate(() => {
      const m = document.querySelector("#gameFrame")?.contentWindow?.Module;
      m?._th13_mp_audit?.(1);
      const u = new URL(location.href);
      u.searchParams.set("renderScale", "1");
      history.replaceState(null, "", u);
    });
    const state = await inspect(page);
    report.events.push({ seat, at: "room", state });
    if (!seat)
      report.room = (await page.locator("#mpRoomCode").innerText()).trim();
    await captureBrowserIdentity(browser, browserIndex, "room");
    console.log("room entered", seat, report.room);
  }
  for (const page of pages) {
    await page.waitForFunction(() => {
      if (document.querySelector("#decisionDialog[open]:not(.closing)"))
        document.querySelector("#decisionConfirm").click();
      return !!document.querySelector("#mpReady:not(:disabled)");
    }, {}, {timeout: 60000});
    await page.locator("#mpReady").click({ timeout: 60000 });
    await page.waitForFunction(
      () =>
        document.querySelector("#decisionDialog")?.open ||
        JSON.parse(
          sessionStorage.getItem("eagler-touhou-th13mp-room-v1") || "{}",
        ).ready === true,
      {},
      { timeout: 60000 },
    );
    if (await page.locator("#decisionDialog[open]:not(.closing)").count())
      await page.locator("#decisionConfirm").click();
    await page.waitForFunction(
      () =>
        JSON.parse(
          sessionStorage.getItem("eagler-touhou-th13mp-room-v1") || "{}",
        ).ready === true,
      {},
      { timeout: 60000 },
    );
  }
  report.events.push({
    at: "ready",
    states: await Promise.all(pages.map(inspect)),
  });
  for (const page of pages)
    await page.evaluate(() => {
      const m = document.querySelector("#gameFrame").contentWindow.Module;
      if (!m?._th13_mp_audit) throw Error("Missing audit");
      m._th13_mp_audit(1);
    });
  await writeFile(
    dir + "/formal-smoke-progress.json",
    JSON.stringify(report, null, 2),
  );
  await pages[0].locator("#mpStartGame").click({ timeout: 30000 });
  console.log("started");
  await writeFile(
    dir + "/formal-smoke-progress.json",
    JSON.stringify(report, null, 2),
  );
  await Promise.all(
    pages.map(async (page) => {
      await page.waitForFunction(
        () => {
          const d = document.querySelector(
            "#decisionDialog[open]:not(.closing)",
          );
          if (d) document.querySelector("#decisionConfirm").click();
          const m = document.querySelector("#gameFrame")?.contentWindow?.Module;
          return (
            m &&
            JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())).sim_frame >
              0
          );
        },
        {},
        { timeout: 60000 },
      );
      await page.evaluate(() => {
        const w = document.querySelector("#gameFrame").contentWindow,
          m = w.Module,
          old = m.onGameFrame;
        w.__health = [];
        m.onGameFrame = (ok, ms, ticks) => {
          w.__health.push({ t: performance.now(), ok, ms, ticks });
          if (w.__health.length > 100) w.__health.shift();
          // Capture the final confirmed tail before the normal shell closes
          // its session on room return. Read-only, once per finished world.
          if (!w.__formalTail && m._th13_room_return_ready()) {
            w.__formalTail = {
              audit: JSON.parse(m.UTF8ToString(m._th13_mp_audit_take())),
              timing: JSON.parse(m.UTF8ToString(m._th13_mp_timing_take())),
            };
          }
          old?.(ok, ms, ticks);
        };
      });
    }),
  );
  await Promise.all(browsers.map((browser, index) =>
    captureBrowserIdentity(browser, index, "gameplay")));

  if (process.env.TH13_ROOM_REENTRIES) {
    report.scope = "Production room pause/resume/save/return/reentry";
    report.lifecycle = [];
    await runRoomLifecycle({pages, inspect, rounds: +process.env.TH13_ROOM_REENTRIES, evidence: report.lifecycle});
    report.passed = true;
  } else if (process.env.TH13_COOP_GIFT === "1") {
    report.scope = "Production three-player directed life gift";
    report.cooperation = [];
    await runDirectedGift({pages, evidence: report.cooperation});
    report.passed = true;
  } else if (process.env.TH13_INPUT_EDGES === "1") {
    report.scope = "Production room short input edges through correction";
    report.inputEdges = [];
    const input = await runInputEdges({pages, inspect, evidence: report.inputEdges});
    for (let seat = 0; seat < PLAYER_COUNT; ++seat) {
      await writeFile(dir + `/audit-${seat}.ndjson`, input.histories[seat].map(row => JSON.stringify(row) + "\n").join(""));
      await writeFile(dir + `/timing-${seat}.ndjson`, input.timings[seat].map(row => JSON.stringify(row) + "\n").join(""));
    }
    report.current = input.states;
    report.comparison = {comparedThrough: input.common - 1, counts: input.histories.map(rows => rows.length)};
    assert.equal(report.errors.length, 0, "Browser errors");
    report.passed = true;
  } else {
  for (let seat = 0; seat < pages.length; seat++)
    await pages[seat].evaluate(installPilot, { seat });
  profileCapture = await startCpuProfiles({pages, directory: dir});
  const next = Array(PLAYER_COUNT).fill(0),
    pending = Array.from({length: PLAYER_COUNT}, () => new Map());
  let compared = -1,
    lastProgress = Date.now(),
    lastFrame = 0;
  const stageSet = new Set(),
    start = Date.now();
  let seenEnding = false,
    seenExtraEnd = false;
  const routes = Array.from({length: PLAYER_COUNT}, () => new Set()), maxCorrections = Array(PLAYER_COUNT).fill(0);
  const terminals = Array(PLAYER_COUNT).fill(null);
  for (const page of pages)
    assert.equal(
      await page.evaluate(
        () =>
          document.querySelector("#gameFrame").contentWindow.Module
            .eaglerOptions.netplayInputDelay,
      ),
      0,
      "Default input delay",
    );
  while (
    Date.now() - start <
    Number(process.env.TH13_TIMEOUT_MINUTES || 90) * 60 * 1000
  ) {
    await new Promise((r) => setTimeout(r, 2000));
    const states = await Promise.all(pages.map(inspect));
    for (let seat = 0; seat < PLAYER_COUNT; seat++) {
      const state = states[seat],
        runtime = state.runtime;
      assert(runtime, "Missing runtime");
      if (runtime.net.sim_frame > 0 && runtime.route) routes[seat].add(runtime.route);
      maxCorrections[seat] = Math.max(maxCorrections[seat], runtime.rollback[2]);
      assert.equal(runtime.audit.overflow, 0, "Audit overflow");
      if (runtime.audit.terminal) {
        assert(!runtime.audit.terminal.error, "Terminal capture failed");
        terminals[seat] = runtime.audit.terminal;
      }
      for (const row of runtime.audit.frames) {
        assert.equal(row.frame, next[seat]++, "Missing/duplicate audit frame");
        pending[seat].set(row.frame, row);
      }
      await appendFile(
        dir + `/audit-${seat}.ndjson`,
        runtime.audit.frames.map((row) => JSON.stringify(row) + "\n").join(""),
      );
      assert.equal(state.nativeError, "", "Native game error");
      assert.equal(runtime.timing.overflow, 0, "Timing overflow");
      await appendFile(
        dir + `/timing-${seat}.ndjson`,
        JSON.stringify(runtime.timing) + "\n",
      );
      await appendFile(
        dir + `/health-${seat}.ndjson`,
        JSON.stringify({
          ...state,
          runtime: {
            ...runtime,
            audit: {
              frames: runtime.audit.frames.length,
              overflow: runtime.audit.overflow,
            },
          },
        }) + "\n",
      );
      assert.equal(runtime.error, "", "Native network error");
      assert(!state.contextLoss, "WebGL context lost");
    }
    while (pending.every((queue) => queue.has(compared + 1))) {
      const f = ++compared,
        a = pending[0].get(f);
      for (let seat = 1; seat < PLAYER_COUNT; ++seat)
        assert.deepEqual(a, pending[seat].get(f), "Confirmed full state mismatch at " + f + " seat " + seat);
      for (const queue of pending) queue.delete(f);
    }
    report.comparison = { comparedThrough: compared, counts: next };
    report.current = states.map((s) => ({
      ...s,
      runtime: {
        ...s.runtime,
        audit: {
          frames: s.runtime.audit.frames.length,
          overflow: s.runtime.audit.overflow,
        },
      },
    }));
    for (const s of states) {
      if (s.probe?.[0] === 1) stageSet.add(s.probe[2]);
      if (s.probe?.[0] === 2) seenEnding = true;
      if (s.runtime.pause.screen === 4) seenExtraEnd = true;
    }
    if (states[0].runtime.hostFrame !== lastFrame) {
      lastFrame = states[0].runtime.hostFrame;
      lastProgress = Date.now();
    }
    console.log(
      JSON.stringify({
        compared,
        clients: states.map((s) => ({
          stage: s.probe?.[2],
          frame: s.runtime.stageFrame,
          pause: s.runtime.pause,
          net: s.runtime.net,
          seats: s.runtime.seats,
        })),
      }),
    );
    await writeFile(dir + "/progress.json", JSON.stringify(report, null, 2));
    assert(
      Date.now() - lastProgress < 30000,
      "No simulation progress for 30 seconds",
    );
    assert.equal(report.errors.length, 0, "Browser errors");
    if (
      states.every((s) => s.probe?.[0] === 0 && s.probe?.[8] === 1 && s.roomVisible && s.readyEnabled) &&
      compared > 5000
    ) {
      report.completedStages = [...stageSet];
      report.passed = true;
      break;
    }
  }
  assert(report.passed, "Full flow did not complete and return to the ready room");
  report.transport = [];
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const wire = await pages[seat].evaluate(() => document.querySelector("#gameFrame").contentWindow.__wireConditions);
    const expectedRoute = process.env.TH13_ROUTE || "rtc";
    assert.deepEqual([...routes[seat]], [expectedRoute], "Actual transport route");
    assert(wire.sent > 0 && wire.delivered > 0, "Transport must carry real packets");
    if (+process.env.TH13_LOSS > 0 && expectedRoute === "rtc") assert(wire.dropped > 0, "Input loss must occur");
    if (+process.env.TH13_DUPLICATE_EVERY > 0) assert(wire.duplicated > 0, "Duplicates must occur");
    if (+process.env.TH13_BURST_EVERY > 0) assert(wire.bursts > 0, "Burst delay must occur");
    if (+process.env.TH13_JITTER > 0) assert(wire.reordered > 0, "Jitter must reorder deliveries");
    if (+process.env.TH13_REQUIRE_ROLLBACK > 0) assert(maxCorrections[seat] > 0, "Rollback must actually occur");
    report.transport.push({seat, routes: [...routes[seat]], maxCorrections: maxCorrections[seat], wire,
      datagramLossApplied: expectedRoute === "rtc" && +process.env.TH13_LOSS > 0});
  }
  assert.deepEqual(
    report.completedStages,
    process.env.TH13_DIFFICULTY === "4" ? [7] : [1, 2, 3, 4, 5, 6],
    "Required stages",
  );
  assert(
    process.env.TH13_DIFFICULTY === "4" ? seenExtraEnd : seenEnding,
    "Clear result/Ending must be observed",
  );
  assert.equal(pending.reduce((sum, queue) => sum + queue.size, 0), 0, "Unpaired confirmed tail");
  assert(terminals.every(Boolean), "Missing final resulting world audit");
  for (const terminal of terminals.slice(1))
    assert.deepEqual(terminals[0], terminal, "Final resulting world differs");
  assert.equal(terminals[0].completed, true, "Final world must be a clear");
  assert(next.every((frame) => frame === terminals[0].frames), "Missing confirmed audit tail");
  report.terminal = terminals[0];
  for (let seat = 0; seat < PLAYER_COUNT; seat++)
    for (const file of expectedFiles) {
      const rows = report.delivery.filter(
        (r) =>
          r.seat === seat && new URL(r.url).pathname.endsWith("/" + file.name),
      );
      assert(rows.length, "Missing delivered " + file.name);
      for (const row of rows) {
        assert.equal(row.bytes, file.bytes);
        assert.equal(row.sha256, file.sha256, "Delivered build identity");
      }
    }
  report.saved = [];
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const saved = await pages[seat].evaluate(async () => {
      const w = document.querySelector("#gameFrame").contentWindow,
        m = w.Module;
      const names = m.FS.readdir("/save/replay").filter((n) =>
        /^th13mp_\d{2}\.rpy$/.test(n),
      );
      if (!names.length) throw Error("No replay saved through result menu");
      const request = "full-flow-sync",
        epoch = Number(
          new URL(w.location.href).searchParams.get("runtimeEpoch"),
        );
      const ack = await new Promise((resolve, reject) => {
        const timer = setTimeout(
          () => reject(Error("Replay sync timed out")),
          10000,
        );
        const on = (e) => {
          if (e.source === w && e.data.request === request) {
            clearTimeout(timer);
            removeEventListener("message", on);
            resolve(e.data);
          }
        };
        addEventListener("message", on);
        w.postMessage(
          {
            protocol: "eagler-touhou/1",
            game: "th13",
            epoch,
            request,
            command: "sync",
          },
          location.origin,
        );
      });
      if (!ack.ok) throw Error("Replay sync failed: " + ack.error);
      const db = await new Promise((resolve, reject) => {
        const r = w.indexedDB.open("th13mp.replays.v1", 1);
        r.onsuccess = () => resolve(r.result);
        r.onerror = () => reject(r.error);
      });
      const records = [];
      for (const name of names) {
        const record = await new Promise((resolve, reject) => {
          const r = db.transaction("files").objectStore("files").get(name);
          r.onsuccess = () => resolve(r.result);
          r.onerror = () => reject(r.error);
        });
        const bytes = m.FS.readFile("/save/replay/" + name);
        if (
          !record ||
          record.bytes.length !== bytes.length ||
          record.bytes.some((b, i) => b !== bytes[i])
        )
          throw Error("Durable replay differs: " + name);
        records.push({ name, bytes: Array.from(bytes) });
      }
      db.close();
      return records;
    });
    const files = [];
    for (const file of saved) {
      const bytes = Buffer.from(file.bytes),
        words = 22;
      const replayBody = 12 + bytes.readUInt32LE(8);
      assert(replayBody + 36 <= bytes.length, "Replay body is complete");
      const inputFrames = bytes.readUInt32LE(replayBody + 32);
      assert.equal(inputFrames, terminals[seat].frames, "Saved replay input history is complete");
      assert.equal(
        bytes.readUInt32LE(12 + words * 4 + 8 + 4),
        1,
        "Replay must contain completed clear",
      );
      const nameLength = bytes.readUInt32LE(12 + words * 4 + 8 + 4 + 4);
      let offset = 12 + words * 4 + 8 + 4 + 4 + 4 + nameLength;
      const count = bytes.readUInt32LE(offset),
        chapters = [];
      offset += 4;
      for (let i = 0; i < count; i++) {
        chapters.push(bytes.readUInt32LE(offset));
        offset += 8 + bytes.readUInt32LE(offset + 4);
      }
      assert.deepEqual(
        chapters,
        report.completedStages,
        "Saved replay chapters",
      );
      await writeFile(dir + `/seat-${seat}-` + file.name, bytes);
      files.push({
        name: file.name,
        bytes: bytes.length,
        sha256: createHash("sha256").update(bytes).digest("hex"),
        chapters,
        inputFrames,
      });
    }
    report.saved.push({ seat, files });
  }
  }

} catch (error) {
  report.passed = false;
  report.failure = error.stack;
  console.error(error.stack);
  for (let i = 0; i < pages.length; i++) {
    report.events.push({
      seat: i,
      at: "failure",
      state: await inspect(pages[i]).catch((e) => ({ error: String(e) })),
    });
    await pages[i]
      .screenshot({ path: dir + `/formal-failure-${i}.png` })
      .catch(() => {});
  }
} finally {
  if (profileCapture) report.cpuProfiles = await profileCapture.finish();
  report.finished = new Date().toISOString();
  try {
    const identityResults = await Promise.allSettled(browsers.map((browser, index) =>
      captureBrowserIdentity(browser, index, "final")));
    report.browserTopology.finalIdentityErrors = identityResults.flatMap((result, browserIndex) =>
      result.status === "rejected" ? [{ browserIndex, error: String(result.reason) }] : []);
    await writeFile(
      dir + "/formal-smoke.json",
      JSON.stringify(report, null, 2) + "\n",
    );
  } finally {
    await Promise.allSettled(browsers.map(browser => browser.close()));
  }
}
if (!report.passed) process.exitCode = 1;
