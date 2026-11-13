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
import { evaluateFrameWork } from "./frame-work.mjs";
import { assertHardwareIdentity } from "./hardware-identity.mjs";
import { installFormalAudit, persistAuditCapture } from "./formal-audit.mjs";
import { installFramebufferCapture } from "./formal-framebuffer.mjs";
import { persistIndependent, terminalStream } from "./persist-independent.mjs";
import { boundedOperation } from "./bounded-operation.mjs";
import { freezeFrameWork } from "./freeze-frame-work.mjs";
import { createInspectionGuard, settleInspections } from "./inspection-serialization.mjs";
import { persistFramebufferCapture, finishFramebufferAck } from "./formal-framebuffer-persist.mjs";
import { assertBuildDelivery } from "./formal-delivery.mjs";
import { assertRuntimeBuildBinding, evaluateRequiredAcceptance } from "./formal-required-acceptance.mjs";
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
const expectedFiles = [];
let runtimeManifestBytes;
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
const helperFiles = ["formal-flow.mjs", "formal-delivery.mjs", "formal-required-acceptance.mjs", "formal-pilot.mjs", "formal-network.mjs", "formal-lifecycle.mjs", "formal-cooperation.mjs", "formal-input.mjs", "formal-profile.mjs", "formal-audit.mjs", "formal-framebuffer.mjs", "formal-framebuffer-persist.mjs", "frame-work.mjs", "hardware-identity.mjs", "persist-independent.mjs", "bounded-operation.mjs", "freeze-frame-work.mjs", "inspection-serialization.mjs"];
const launchOptions = {
  headless: process.env.TH13_HEADFUL !== "1",
  channel: "chromium",
  args: [
    "--no-sandbox",
    "--disable-dev-shm-usage",
    "--ignore-gpu-blocklist",
    "--disable-gpu-sandbox",
    "--use-angle=" + (process.env.TH13_ANGLE || "gl-egl"),
    "--disable-software-rasterizer",
    "--enable-gpu",
    "--autoplay-policy=no-user-gesture-required",
    "--enable-logging=stderr",
    "--disable-background-timer-throttling",
    "--disable-backgrounding-occluded-windows",
    "--disable-renderer-backgrounding",
  ],
};
const PLAYER_COUNT = Number(process.env.TH13_PLAYERS || 3);
const sharedBrowser = process.env.TH13_SHARED_BROWSER === "1";
const browsers = [];
report.browserTopology = { sharedBrowser, launchOptions, seats: [], snapshots: [] };
const pages = [];
const cpuSessions = [];
const workCaptures = [];
const workErrors = [];
const deliveryTasks = new Set();
const deliveryRequests = new Set();
const deliveryRequestIdentity = new WeakMap();
const deliveryFrames = new WeakMap();
const deliveryEpochs = new WeakMap();
const gameFrames = [];
const gameFrameGuids = [];
const gameDocumentEpochs = [];
report.playerCount = PLAYER_COUNT;
report.frameGuids = gameFrameGuids;
report.documentEpochs = gameDocumentEpochs;
let deliveryFrameSequence = 0;
const deliveryFrameGuid = frame => {
  if (!deliveryFrames.has(frame)) deliveryFrames.set(frame, `frame-${++deliveryFrameSequence}`);
  return deliveryFrames.get(frame);
};
const runtimeResponse = url => {
  try { return /\/th13-sdl\.(mjs|wasm)$/.test(new URL(url).pathname); }
  catch { return false; }
};
async function settleRuntimeDelivery(label) {
  await boundedOperation(label, async () => {
    // New responses may arrive while an earlier body is being read. Drain
    // the live request and task sets, rather than a single promise snapshot.
    while (deliveryTasks.size || deliveryRequests.size) {
      await Promise.all([...deliveryTasks]);
      if (deliveryRequests.size) await new Promise(resolve => setTimeout(resolve, 10));
    }
  }, 15000);
}
const inspectTimedOut = new Set();
const guardInspection = createInspectionGuard();
let closingBrowsers = false;
const defaultFlow = !process.env.TH13_ROOM_REENTRIES && process.env.TH13_COOP_GIFT !== "1" && process.env.TH13_INPUT_EDGES !== "1";
const auditCursors = Array.from({length: PLAYER_COUNT}, () => ({next:1,rawPending:0,errorsWritten:false}));
const workCursors = Array.from({length: PLAYER_COUNT}, () => ({failure:null}));
const framebufferEvidence = Array.from({length: PLAYER_COUNT}, () => []);
const framebufferCursors = Array.from({length: PLAYER_COUNT}, () => ({next:1,rawPending:0,failure:null}));
const framebufferObservations = Array(PLAYER_COUNT).fill(null);
const requireRollback = Number(process.env.TH13_REQUIRE_ROLLBACK || 0) > 0;
report.cpuThrottlingRate = 4;
report.requestedCharacters = [0, 1, 3].slice(0, PLAYER_COUNT);
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
    const contexts = await Promise.all(report.browserTopology.seats
      .filter(row => row.browserIndex === browserIndex)
      .map(async ({seat}) => ({seat, ...await pages[seat].evaluate(() => {
        const w = document.querySelector("#gameFrame")?.contentWindow;
        const canvas = w?.document.querySelector("canvas");
        const gl = canvas?.getContext("webgl2");
        if (!gl) return {error: "Game has no WebGL2 context"};
        const info = gl.getExtension("WEBGL_debug_renderer_info");
        return {version: gl.getParameter(gl.VERSION),
          renderer: info && gl.getParameter(info.UNMASKED_RENDERER_WEBGL),
          vendor: info && gl.getParameter(info.UNMASKED_VENDOR_WEBGL)};
      })})));
    const identity = {
      phase, at: new Date().toISOString(), browserIndex,
      seats: report.browserTopology.seats.filter(row => row.browserIndex === browserIndex)
        .map(row => row.seat),
      version, gpu: system.gpu, processes, contexts,
    };
    report.browserTopology.snapshots.push(identity);
    await appendFile(dir + "/browser-identity.ndjson", JSON.stringify(identity) + "\n");
    return identity;
  } finally {
    await session.detach();
  }
}
const inspect = async (page, {expired = () => false} = {}) => {
  const state = await page.evaluate(({playerCount, freeze}) => {
    const frame = document.querySelector("#gameFrame"),
      w = frame?.contentWindow,
      m = w?.Module;
    const read = (name) => {
      if (!m?.[name]) return null;
      const text = m.UTF8ToString(m[name]());
      return text ? JSON.parse(text) : { empty: true };
    };
    const tail = w?.__formalTail;
    const drain = w?.__formalAudit;
    if (drain) drain.drain();
    const captured = drain?.snapshot();
    const audit = drain ? {frames: [], overflow: 0, terminal: null} : read("_th13_mp_audit_take");
    const timing = drain ? {clock: "steady-microseconds", events: [], overflow: 0} : read("_th13_mp_timing_take");
    if (captured) for (const batch of captured.batches) {
      audit.frames.push(...batch.audit.frames);
      audit.overflow = Math.max(audit.overflow, batch.audit.overflow);
      audit.terminal ||= batch.audit.terminal;
      timing.events.push(...batch.timing.events);
      timing.overflow = Math.max(timing.overflow, batch.timing.overflow);
    }
    if (tail) {
      audit.frames = [...tail.audit.frames, ...audit.frames];
      audit.overflow += tail.audit.overflow;
      audit.terminal ||= tail.audit.terminal;
      timing.events = [...tail.timing.events, ...timing.events];
      timing.overflow += tail.timing.overflow;
      w.__formalTail = null;
    }
    return {
      auditDrain: captured || null,
      framebuffer: w?.__framebufferCapture?.snapshot() || null,
      work: w?.__frameWork ? {capture: {...w.__frameWork.capture},
        samples: w.__frameWork.samples.slice(), errors: [...w.__frameWork.errors],
        attached: !m || m.onGameWorkFrame === w.__frameWork.observer} : null,
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
      raf: w?.__rafProbe,
      health: w?.__health,
      canvas: w?.document.querySelector("canvas")
        ? {
            width: w.document.querySelector("canvas").width,
            height: w.document.querySelector("canvas").height,
          }
        : null,
      pilot: w?.__pilot,
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
  }, {playerCount:PLAYER_COUNT});
  if (expired()) throw Error('Evidence inspection expired before persistence');
  const seat = pages.indexOf(page);
  await persistIndependent([
  ["framebuffer",async()=>{
  if (state.framebuffer) {
    const capture=state.framebuffer;
    framebufferObservations[seat]={observeCalls:capture.observeCalls,observeMs:capture.observeMs,
      pendingReasons:capture.pendingReasons, failures:capture.failures};
    const cursor=framebufferCursors[seat];
    const accepted=await persistFramebufferCapture(dir,seat,capture,cursor,framebufferEvidence[seat]);
    try { if (capture.captures.length) await page.evaluate(({first,last,count}) => {
      const capture=document.querySelector('#gameFrame')?.contentWindow?.__framebufferCapture;
      if (!capture) throw Error('Framebuffer collector disappeared before acknowledgement');
      capture.ack(first,last,count);
    },{first:capture.captures[0].sequence,last:capture.captures.at(-1).sequence,count:capture.captures.length}); }
    catch(error) { cursor.failure=`Framebuffer acknowledgement failed: ${error}`; throw error; }
    finishFramebufferAck(cursor,framebufferEvidence[seat],accepted);
    state.framebuffer={observeCalls:capture.observeCalls,observeMs:capture.observeMs,
      pendingReasons:capture.pendingReasons, count:framebufferEvidence[seat].length};
  }
  }],
  ["audit",async()=>{
  if (state.auditDrain) {
    const {batches} = state.auditDrain;
    const cursor = auditCursors[seat];
    await terminalStream(cursor,'Audit',async()=>{
    await persistAuditCapture(dir + `/audit-raw-${seat}.ndjson`, state.auditDrain, cursor);
    if (batches.length) {
      await appendFile(dir + `/audit-batches-${seat}.ndjson`, batches.map(row => JSON.stringify(row) + "\n").join(""));
      await page.evaluate(({first,last,count}) => {
        const audit = document.querySelector("#gameFrame")?.contentWindow?.__formalAudit;
        if (!audit) throw Error("Audit drain disappeared before acknowledgement");
        audit.ack(first,last,count);
      }, {first:batches[0].sequence,last:batches.at(-1).sequence,count:batches.length});
      cursor.rawPending = 0;
    }
    state.auditDrain = {calls:state.auditDrain.calls, elapsedMs:state.auditDrain.elapsedMs};
    });
  }
  }],
  ["work",async()=>{
  if (state.work) {
    const cursor=workCursors[seat];
    await terminalStream(cursor,'Work',async()=>{
    await appendFile(dir + `/work-${seat}.ndjson`,
      state.work.samples.map(row => JSON.stringify(row) + "\n").join(""));
    const rows = state.work.samples;
    if (rows.length) await page.evaluate(({count, first, last}) => {
      const work = document.querySelector("#gameFrame")?.contentWindow?.__frameWork;
      if (!work || work.samples[0]?.sequence !== first || work.samples[count - 1]?.sequence !== last)
        throw Error("Work evidence changed before persistence acknowledgement");
      work.samples.splice(0, count);
    }, {count: rows.length, first: rows[0].sequence, last: rows.at(-1).sequence});
    workCaptures[seat] = state.work.capture;
    if (!state.work.attached) workErrors.push({seat, error: "Work observer was replaced"});
    workErrors.push(...state.work.errors.map(error => ({seat, error})));
    state.work = {capture: state.work.capture, count: rows.length,
      errors: state.work.errors, attached: state.work.attached};
    });
  }
  }],
  ]);
  return state;
};
const inspectBounded = async page => {
  const seat=pages.indexOf(page);
  if (inspectTimedOut.has(seat)) throw Error(`Seat ${seat} prior inspection timed out; tail incomplete`);
  try {
    return await boundedOperation(`seat ${seat} inspect`,
      expired => guardInspection(seat,()=>inspect(page,{expired})), 15000);
  } catch (error) {
    if (String(error).includes('timed out')) inspectTimedOut.add(seat);
    throw error;
  }
};
const inspectAll = () => settleInspections(pages.map(page=>()=>inspectBounded(page)));
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
  if (![2, 3].includes(PLAYER_COUNT)) throw Error("TH13_PLAYERS must be 2 or 3");
  await Promise.all(helperFiles.map(async name => {
    await writeFile(resolve(dir, name), await readFile(new URL(name, import.meta.url)));
  }));
  expectedFiles.push(...await Promise.all(["th13-sdl.mjs", "th13-sdl.wasm"].map(async name => {
    const bytes = await readFile(process.env.TH13_MP_BUILD
      ? resolve(process.env.TH13_MP_BUILD, name)
      : new URL("../../../build-eagler-multiplayer/" + name, import.meta.url));
    return {name, bytes:bytes.length, sha256:createHash("sha256").update(bytes).digest("hex")};
  })));
  // Retain the actual build artifact before launching any client. Delivered
  // response hashes below bind each document to these exact output bytes.
  runtimeManifestBytes = await readFile(process.env.TH13_MP_BUILD
    ? resolve(process.env.TH13_MP_BUILD, "build.json")
    : new URL("../../../build-eagler-multiplayer/build.json", import.meta.url));
  const binding = assertRuntimeBuildBinding({expectedFiles, runtimeManifestBytes});
  await writeFile(resolve(dir, "runtime-build.json"), runtimeManifestBytes);
  report.runtimeBuild = {path:"runtime-build.json", sha256:binding.sha256};
  for (let seat = 0; seat < PLAYER_COUNT; seat++) {
    const browser = sharedBrowser && browsers.length
      ? browsers[0] : await chromium.launch(launchOptions);
    if (!browsers.includes(browser)) {
      browsers.push(browser);
      browser.on('disconnected',()=>{if(!closingBrowsers)report.errors.push({message:'Browser disconnected',browserIndex:browsers.indexOf(browser)});});
    }
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
    if (defaultFlow) await context.addInitScript(installFormalAudit);
    if (defaultFlow) await context.addInitScript(installFramebufferCapture);
    await context.addInitScript(() => {
      // Install before the managed runtime publishes Module, so loading and
      // the first callback cannot disappear from the performance evidence.
      const work = globalThis.__frameWork = {samples: [], errors: [],
        capture: {first: null, last: null, count: 0}};
      let moduleValue;
      Object.defineProperty(globalThis, "Module", {configurable: true,
        get: () => moduleValue,
        set: value => {
          moduleValue = value;
          if (!value || typeof value !== "object") return;
          const previous = value.onGameWorkFrame;
          work.observer = value.onGameWorkFrame = row => {
            work.samples.push({...row});
            if (!work.capture.count) work.capture.first = row.sequence;
            work.capture.last = row.sequence;
            ++work.capture.count;
            globalThis.__framebufferCapture?.onWork(row);
            previous?.(row);
          };
        }});
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
    page.on('crash',()=>report.errors.push({seat,message:'Browser page crashed'}));
    page.on('close',()=>{if(!closingBrowsers)report.errors.push({seat,message:'Browser page closed'});});
    page.on('framenavigated', frame => {
      deliveryEpochs.set(frame, (deliveryEpochs.get(frame) || 0) + 1);
      if (gameFrames[seat] === frame)
        report.errors.push({seat,message:'Runtime iframe navigated after room identity binding'});
    });
    const cpu = await context.newCDPSession(page);
    await cpu.send("Emulation.setCPUThrottlingRate", {rate: 4});
    cpuSessions.push(cpu);
    page.setDefaultTimeout(10000);
    await page.addLocatorHandler(page.locator("#firstUseNoticeDialog[open]"),
      async () => { await page.locator("#firstUseNoticeClose").click(); });
    await page.addLocatorHandler(page.locator("#decisionDialog[open]:not(.closing)"),
      async () => { await page.locator("#decisionConfirm").click(); });
    page.on("requestfailed", (r) =>
      {
      deliveryRequests.delete(r);
      report.errors.push({
        seat,
        url: r.url(),
        message: r.failure()?.errorText,
      });
      },
    );
    page.on('request', request => {
      if (runtimeResponse(request.url())) {
        deliveryRequests.add(request);
        try {
          const frame = request.frame();
          deliveryRequestIdentity.set(request, {frameGuid:deliveryFrameGuid(frame),
            documentEpoch:deliveryEpochs.get(frame) || 0});
        } catch (error) {
          report.errors.push({seat,url:request.url(),message:`Runtime request frame: ${error}`});
        }
      }
    });
    page.on('requestfinished', request => deliveryRequests.delete(request));
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
    page.on("response", (response) => {
      if (runtimeResponse(response.url())) {
        const task = (async () => {
        const identity = deliveryRequestIdentity.get(response.request());
        assert(identity, 'Runtime response has no observed request identity');
        const bytes = await response.body();
        const row = {
          seat,
          url: response.url(),
          ...identity,
          bytes: bytes.length,
          sha256: createHash("sha256").update(bytes).digest("hex"),
        };
        report.delivery.push(row);
        const expected = expectedFiles.find(file => new URL(row.url).pathname.endsWith('/' + file.name));
        assert(expected, 'Unexpected runtime response filename');
        assert.equal(row.bytes, expected.bytes, `Seat ${seat}: runtime response size`);
        assert.equal(row.sha256, expected.sha256, `Seat ${seat}: runtime response hash`);
        if (gameFrameGuids[seat])
          assert.equal(row.frameGuid, gameFrameGuids[seat], `Seat ${seat}: runtime iframe changed`);
        if (gameDocumentEpochs[seat] !== undefined)
          assert.equal(row.documentEpoch, gameDocumentEpochs[seat], `Seat ${seat}: runtime document changed`);
        })().catch(error => {
          report.errors.push({seat,url:response.url(),message:`Runtime delivery body: ${error}`});
        });
        deliveryTasks.add(task);
        task.finally(() => deliveryTasks.delete(task));
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
    await page.waitForFunction(() => {
      if (document.querySelector("#decisionDialog[open]:not(.closing)"))
        document.querySelector("#decisionConfirm").click();
      return !!document.querySelector("#mpReady:not(:disabled)") &&
        !!document.querySelector("#gameFrame")?.contentWindow?.Module?._th13_probe_state;
    }, {}, {timeout: 120000});
    const desired = report.requestedCharacters[seat];
    const characterField = page.locator("#mpLoadoutField0");
    if (await characterField.count()) {
      await characterField.selectOption(String(desired));
      await page.waitForFunction(value =>
        document.querySelector("#mpLoadoutField0")?.value === String(value) &&
        !document.querySelector("#mpLoadoutField0")?.disabled, desired);
    } else {
      // TH13 has one shot per character, with the room's normal next control.
      await page.locator(`[data-mp-seat="${seat}"] .mp-seat-edit`).click();
      for (let step = 0; step < desired; ++step) {
        await page.locator("#mpLoadoutNextSeat").click();
        await page.waitForTimeout(100);
      }
      await page.locator("#mpRoomPanelClose").click();
    }
    await page.evaluate(() => {
      const m = document.querySelector("#gameFrame")?.contentWindow?.Module;
      m?._th13_mp_audit?.(1);
      const u = new URL(location.href);
      u.searchParams.set("renderScale", "1");
      history.replaceState(null, "", u);
    });
    const state = await inspectBounded(page);
    const frameElement = await page.locator('#gameFrame').elementHandle();
    const gameFrame = await frameElement.contentFrame();
    assert(gameFrame && gameFrame !== page.mainFrame(), `Seat ${seat}: missing game iframe`);
    gameFrames[seat] = gameFrame;
    gameFrameGuids[seat] = deliveryFrameGuid(gameFrame);
    gameDocumentEpochs[seat] = deliveryEpochs.get(gameFrame) || 0;
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
    states: await inspectAll(),
  });
  for (const page of pages)
    await page.evaluate(({enableDrain,seatCount}) => {
      const w = document.querySelector("#gameFrame").contentWindow;
      const m = w.Module;
      if (!m?._th13_mp_audit) throw Error("Missing audit");
      if (enableDrain) {
        if (!w.__installFormalAudit) throw Error("Missing audit drain installer");
        w.__installFormalAudit(m);
        if (!w.__installFramebufferCapture) throw Error("Missing framebuffer installer");
        w.__installFramebufferCapture(m, seatCount);
      }
      m._th13_mp_audit(1);
    }, {enableDrain:defaultFlow,seatCount:PLAYER_COUNT});
  await writeFile(
    dir + "/formal-smoke-progress.json",
    JSON.stringify(report, null, 2),
  );
  await settleRuntimeDelivery('Initial runtime response bodies');
  report.initialDelivery = assertBuildDelivery({expectedFiles,
    delivery:report.delivery,playerCount:PLAYER_COUNT,frameGuids:gameFrameGuids,
    documentEpochs:gameDocumentEpochs});
  assert.equal(report.errors.length, 0, 'Browser errors before starting');
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
          if (!w.__formalAudit && !w.__formalTail && m._th13_room_return_ready()) {
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
  for (const identity of report.browserTopology.snapshots.filter(row => row.phase === "gameplay")) {
    identity.hardwareVerification = assertHardwareIdentity(identity);
  }
  assert.equal(report.browserTopology.snapshots.filter(row => row.phase === "gameplay").length,
    browsers.length, "Every browser has a gameplay identity");
  const activeStates = await inspectAll();
  for (const state of activeStates) assert.deepEqual(
    state.runtime.seats.map(player => player.character), report.requestedCharacters,
    "Actual runtime character seats");

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
  profileCapture = await startCpuProfiles({pages, sessions:cpuSessions, directory: dir});
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
  let initialStates = activeStates;
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
    if (!initialStates) await new Promise((r) => setTimeout(r, 2000));
    // inspect drains the confirmed audit. The identity/character check above
    // already took the first batch; consume it before requesting another.
    const states = initialStates || await inspectAll();
    initialStates = null;
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
  await settleRuntimeDelivery('Final runtime response bodies');
  for (const [seat, page] of pages.entries()) {
    const element = await page.locator('#gameFrame').elementHandle();
    assert.equal(await element.contentFrame(), gameFrames[seat], `Seat ${seat}: game iframe replaced`);
  }
  report.finalDelivery = assertBuildDelivery({expectedFiles,
    delivery:report.delivery,playerCount:PLAYER_COUNT,frameGuids:gameFrameGuids,
    documentEpochs:gameDocumentEpochs});
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
} finally {
  // Freeze every reachable client before any profile finalization, file IO or
  // analysis; a failed client is still an incomplete observation, and cannot
  // prevent the others from saving their independently bounded tails.
  const freezes=await Promise.allSettled(pages.map((page,seat)=>
    inspectTimedOut.has(seat) ? Promise.reject(Error('Prior inspection timed out; tail incomplete')) :
      boundedOperation(`seat ${seat} freeze`,()=>page.evaluate(freezeFrameWork),15000)));
  const boundaries=freezes.map((result,seat)=>{
    if(result.status==='fulfilled')return result.value;
    workErrors.push({seat,error:`Final evidence freeze: ${result.reason}`});return null;
  });
  if (profileCapture) {
    try {
      report.cpuProfiles = await boundedOperation('finish CPU profiles',()=>profileCapture.finish(),35000);
      if (!report.cpuProfiles.passed) workErrors.push({error:'CPU profiling incomplete'});
    }
    catch (error) { report.profileError = String(error); workErrors.push({error: String(error)}); }
  }
  report.performance = [];
  const finalFlushes=await Promise.allSettled(pages.map(async (page,seat)=>{
      const boundary=boundaries[seat];
      // Even if freeze failed, attempt to preserve reachable raw observations;
      // it cannot qualify as a complete boundary.
      const final = await inspectBounded(page);
      if (!boundary || boundary.unmatchedCandidate || !final.work || boundary.count!==final.work.capture.count || boundary.first!==final.work.capture.first ||
          boundary.last!==final.work.capture.last)
        throw Error('Frozen work boundary differs from persisted capture');
      (report.finalBoundaries ||= [])[seat]=boundary;
  }));
  for (let seat = 0; seat < pages.length; ++seat) {
    if(finalFlushes[seat].status==='rejected')
      workErrors.push({seat,error:`Final evidence flush: ${finalFlushes[seat].reason}`});
    try {
      const samples = (await readFile(dir + `/work-${seat}.ndjson`, "utf8"))
        .split("\n").filter(Boolean).map(JSON.parse);
      report.performance.push({seat, ...evaluateFrameWork(samples, {
        capture: workCaptures[seat], expectedFirstSequence: 1,
        expectedStages: process.env.TH13_DIFFICULTY === "4" ? [7] : [1, 2, 3, 4, 5, 6],
        expectedPhases: ["menu", "loading", "active"], requireZero: true,
        requireCatchup: true, requireRollback,
      })});
    } catch (error) { report.performance.push({seat, passed: false, error: String(error)}); }
  }
  report.gameplayPassed = report.passed === true;
  report.workErrors = workErrors;
  report.passed = report.gameplayPassed && report.performance.length === PLAYER_COUNT &&
    report.performance.every(row => row.passed) && !sharedBrowser && !workErrors.length && !report.errors.length;
  if (defaultFlow) {
    const expectedStages=process.env.TH13_DIFFICULTY === "4" ? [7] : [1,2,3,4,5,6];
    report.framebuffer = framebufferEvidence.map((captures,seat) => {
      const reasons=[...new Set(captures.flatMap(row => row.reasons))];
      const missing=[...expectedStages.filter(stage=>!reasons.includes(`stage:${stage}`)).map(stage=>`stage:${stage}`),
        ...['ghost:', 'rescue:', 'laser-start'].filter(reason=>!reasons.some(value=>value.startsWith(reason)))];
      return {seat, captures, ...framebufferObservations[seat], failure:framebufferCursors[seat].failure,
        missing, observationTiming:'inside onGameFrame before work end; capture follows event observation at the next submission',
        stateComparison:'read-only host/core/entities/network/seat projections before and after readPixels'};
    });
    report.framebufferLimitations=[
      ...report.framebuffer.flatMap(row=>row.missing.map(reason=>`seat ${row.seat}: missing ${reason}`)),
      ...report.framebuffer.filter(row=>row.failure).map(row=>`seat ${row.seat}: ${row.failure}`)];
  }
  report.finished = new Date().toISOString();
  try {
    const identityResults = await Promise.allSettled(browsers.map((browser, index) =>
      boundedOperation(`browser ${index} final identity`,()=>captureBrowserIdentity(browser, index, "final"),15000)));
    report.browserTopology.finalIdentityErrors = identityResults.flatMap((result, browserIndex) =>
      result.status === "rejected" ? [{ browserIndex, error: String(result.reason) }] : []);
    if (report.browserTopology.finalIdentityErrors.length) report.passed=false;
  } finally {
    closingBrowsers=true;
    const closed = await Promise.allSettled(browsers.map((browser,index) =>
      boundedOperation(`browser ${index} close`,()=>browser.close(),15000)));
    for (const [index,result] of closed.entries())
      if (result.status === 'rejected') {
        report.errors.push({browserIndex:index,message:`Browser close failed: ${result.reason}`});
        report.passed=false;
      }
    // A response may finish during final identity collection or browser close.
    // Persist after the observation sources close and all body reads settle.
    try {
      await settleRuntimeDelivery('Closed runtime response bodies');
      report.closedDelivery = assertBuildDelivery({expectedFiles,delivery:report.delivery,
        playerCount:PLAYER_COUNT,frameGuids:gameFrameGuids,documentEpochs:gameDocumentEpochs});
      report.deliveryBoundary = {
        closed:closed.length===(sharedBrowser ? 1 : PLAYER_COUNT) &&
          closed.every(result=>result.status==='fulfilled'),
        settled:true, pendingRequests:deliveryRequests.size, pendingBodies:deliveryTasks.size,
      };
    } catch (error) {
      report.errors.push({message:`Final runtime delivery: ${error}`});
      report.passed=false;
    }
    if (report.errors.length) report.passed=false;
    if (defaultFlow) {
      try {
        report.requiredAcceptance = await evaluateRequiredAcceptance({
          formalReport:report, runtimeManifestBytes,
        });
      } catch (error) {
        report.errors.push({message:`Required acceptance evaluation: ${error}`});
        report.requiredAcceptance = {passed:false,status:'BLOCKED',error:String(error)};
      }
      report.requiredAcceptance.limitations = report.framebufferLimitations || [];
      report.passed = report.passed && report.requiredAcceptance.passed;
    }
    report.finished = new Date().toISOString();
    await writeFile(dir + '/formal-smoke.json', JSON.stringify(report,null,2) + '\n');
  }
}
if (!report.passed) process.exitCode = 1;
