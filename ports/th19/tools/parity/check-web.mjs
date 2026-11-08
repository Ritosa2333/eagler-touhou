import assert from "node:assert/strict";
import {createHash} from "node:crypto";
import {mkdir, mkdtemp, readFile, writeFile} from "node:fs/promises";
import {tmpdir} from "node:os";
import {resolve} from "node:path";
import {fileURLToPath} from "node:url";
import {once} from "node:events";
import {startServer} from "../../scripts/static-server.mjs";
import puppeteer from "puppeteer-core";

const capture = process.argv[2];
if (!capture) throw Error("Usage: node check-web.mjs <capture directory> [--reverse]");
const nativePath = resolve(capture, "native-present.jsonl");
const port = Number(process.env.TH19_CAPTURE_PORT ?? 18439);
const nativeBytes = await readFile(nativePath);
const nativeRows = nativeBytes.toString("utf8").trim().split("\n").map(JSON.parse);
const nativeFrames = new Map();
for (const row of nativeRows) {
  if (row.segment === 0 && Number.isInteger(row.frame) && !nativeFrames.has(row.frame)) nativeFrames.set(row.frame, row);
}
const nativeRng = nativeFrames.get(0).rng;
const frameNumbers = [...Array.from({length: 201}, (_, i) => i), 300, 600].filter(n => nativeFrames.has(n));
const originalHud = {120:598, 300:595, 600:590, 1200:580};
const reverse = process.argv.includes("--reverse");
const nativeExe = JSON.parse(await readFile(resolve(capture, "capture.json"), "utf8")).originalExecutableSha256;
const wasmPath = fileURLToPath(new URL("../../runtime/th19.wasm", import.meta.url));
const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");
const wasmSha256 = sha256(await readFile(wasmPath));
const output = resolve(process.env.TH19_CAPTURE_OUTPUT || tmpdir());
await mkdir(output, {recursive: true});
const directory = await mkdtemp(resolve(output, "th19-web-"));
const errors = [];
let server;
const browser = await puppeteer.launch({
  executablePath: process.env.TH19_CHROME || "C:/Program Files/Google/Chrome/Application/chrome.exe",
  headless: true, protocolTimeout: 180000,
  args: ["--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--autoplay-policy=no-user-gesture-required"]
});

async function inGame(action, payload) {
  const context = await browser.createBrowserContext();
  try {
    const page = await context.newPage();
    page.on("pageerror", error => errors.push(error.message));
    await page.setViewport({width: 1280, height: 960});
    await page.goto(`http://127.0.0.1:${port}/tests/harness.html?autolaunch=0`);
    await page.waitForFunction(() => window.__TH19_HARNESS.configured, {timeout: 60000});
    await page.evaluate(captureSeed => window.__TH19_HARNESS.command("launch", {captureSeed}), nativeRng[0]);
    return await page.evaluate(action, payload);
  } finally {
    await context.close();
  }
}

function runCapture({seed, otherSeed, frames, visualFrames, reverse}) {
  const win = window.__TH19_HARNESS.frame.contentWindow;
  const c = win.__th19Runtime.core;
  c._th19_loop_stop();
  c._th19_loop_pause(1);
  c._th19_test_ticks(180); // Complete the normal loading animation before reseeding.
  if (c._th19_capture_cpu_begin(seed, otherSeed, 3, reverse?1:0, reverse?0:1, 2, 4) !== 1) throw Error("CPU match failed to start");
  const canvas = win.document.querySelector("canvas");
  const results = [];
  let previous = 0;
  for (const frame of frames) {
    if ((frame === 1 || frame === 86 || (frame >= 122 && frame <= 130))) c._th19_capture_rng_trace(1);
    if (frame > previous) c._th19_test_ticks(frame - previous);
    let openingDraws;
    if ((frame === 1 || frame === 86 || (frame >= 122 && frame <= 130))) {
      const count = c._th19_capture_rng_trace(2), start = c._th19_capture_rng_trace(0)/4;
      openingDraws = Array.from({length:count}, (_,i) => {
        const [kind, seed, time, ip, sprite, primary] = c.HEAP32.slice(start+i*7, start+i*7+7);
        return {kind, seed, time, ip, sprite, primary:!!primary};
      });
    }
    const offset = c._th19_capture_cpu_state() / 4;
    const row = Array.from(c.HEAP32.subarray(offset, offset + 30), value => value >>> 0);
    results.push({
      frame, openingDraws, playedFrames: row[0], round: row[1], lives: row.slice(2, 4),
      controls: row.slice(4, 6), enemies: row.slice(6, 8), bullets: row.slice(8, 10),
      animationRng: row.slice(10, 12), world: row[18], strips: row[19], stripFrame: row[20], side: row[21], waves: row.slice(22,24), waveFinalState:row[24], shotClock:row.slice(25,27), shots:row.slice(27,29), elapsed:row[29], remaining: row[15], announcement: !!row[16],
      png: visualFrames.includes(frame) ? canvas.toDataURL("image/png") : undefined
    });
    previous = frame;
  }
  return results;
}

function testMenu() {
  const win = window.__TH19_HARNESS.frame.contentWindow;
  const c = win.__th19Runtime.core;
  c._th19_loop_stop();
  const read = (name, count) => {
    const offset = c[name]() / 4;
    return Array.from(c.HEAP32.subarray(offset, offset + count));
  };
  const press = (key, wait=120) => {
    c._th19_key(key, 1);
    c._th19_test_ticks(1);
    c._th19_key(key, 0);
    c._th19_test_ticks(wait);
  };
  const state = () => read("_th19_achievement_status", 2);
  c._th19_test_ticks(600);
  press(81); press(29);
  const vs = state();
  for (let i = 0; i < 3; ++i) press(81, 3);
  press(29);
  const rules = state();
  for (let i = 0; i < 4; ++i) press(79, 3);
  const rulesPng = win.document.querySelector("canvas").toDataURL("image/png");
  press(27); press(82); press(29);
  const difficulty = state();
  for (let i = 0; i < 4; ++i) press(81, 3);
  const lunatic = state();
  press(29); press(29); press(29);
  const local = read("_th19_network_info", 31);
  const rulesRejected = c._th19_network_set_rules(4,1,1,4,4,1,1,0,0);
  const rulesAccepted = c._th19_network_set_rules(0,1,1,4,4,1,1,0,0);
  const networkBegan = c._th19_network_begin(3058262,3,0,1);
  const network = read("_th19_network_info", 31);
  return {vs, rules, difficulty, lunatic, local, rulesRejected, rulesAccepted, networkBegan, network, rulesPng};
}

function testAnnouncementRollback() {
  const win = window.__TH19_HARNESS.frame.contentWindow;
  const c = win.__th19Runtime.core;
  if (!c._th19_network_begin(3058262, 3, 0, 1)) throw Error("network fixture failed");
  const step = (from, to) => {
    for (let tick = from; tick < to; ++tick) {
      if (!c._th19_network_step(tick, 0, 0, 0, 0, 0, 0, 0, 0, +(tick + 1 === to)))
        throw Error(`network step ${tick} rejected`);
    }
  };
  const active = () => Boolean(c.HEAP32[c._th19_capture_cpu_state() / 4 + 16]);
  const region = () => {
    const canvas = win.document.createElement("canvas");
    canvas.width = 320; canvas.height = 160;
    canvas.getContext("2d").drawImage(win.document.querySelector("canvas"), 170, 400, 320, 160, 0, 0, 320, 160);
    return canvas.toDataURL();
  };
  step(0, 100);
  const checkpoint = c._th19_checkpoint_save();
  if (!checkpoint) throw Error("announcement checkpoint unavailable");
  step(100, 120);
  const first = {active: active(), region: region()};
  step(120, 150);
  const expired = !active();
  if (!c._th19_checkpoint_load(checkpoint)) throw Error("announcement checkpoint restore failed");
  step(100, 120);
  const second = {active: active(), region: region()};
  c._th19_checkpoint_release(checkpoint);
  return {visibleAt120:first.active, expiredAt150:expired, visibleAfterRestore:second.active,
    identicalRegion:first.region === second.region};
}

function testShotRollback() {
  const win = window.__TH19_HARNESS.frame.contentWindow;
  const c = win.__th19Runtime.core;
  if (!c._th19_network_begin(3058262, 3, 0, 1)) throw Error("network fixture failed");
  const step = (from, to, renderEach) => {
    for (let tick = from; tick < to; ++tick) {
      if (!c._th19_network_step(tick, 1, 1, 0, 0, 0, 0, 0, 0, +(renderEach || tick + 1 === to)))
        throw Error("network step rejected");
    }
  };
  const capture = () => {
    const offset = c._th19_capture_cpu_state()/4;
    return {state:Array.from(c.HEAP32.subarray(offset,offset+30)), image:win.document.querySelector("canvas").toDataURL()};
  };
  step(0,80,false);
  const checkpoint=c._th19_checkpoint_save();
  if (!checkpoint) throw Error("checkpoint unavailable");
  step(80,90,false);
  const first=capture();
  if (!c._th19_checkpoint_load(checkpoint)) throw Error("checkpoint restore failed");
  step(80,90,true);
  const second=capture();
  c._th19_checkpoint_release(checkpoint);
  return {same:JSON.stringify(first)===JSON.stringify(second),
          bothShooting:first.state[27]>0&&first.state[28]>0};
}

const saveImage = (name, png) => writeFile(`${directory}/${name}.png`, Buffer.from(png.split(",")[1], "base64"));

const report = {
  nativePath,
  nativeExe,
  nativeTraceSha256: sha256(nativeBytes), wasmPath, wasmSha256,
  fixture: {difficulty: "Lunatic", characters: reverse ? ["Marisa", "Reimu"] : ["Reimu", "Marisa"],
            cards: false, lives: 2, seconds: 600},
  initialAnimationRng: [nativeRng[0], nativeRng[2]]
};
try {
  const root = fileURLToPath(new URL("../../", import.meta.url));
  server = startServer({routes:[{prefix:"/",directory:root}],host:"127.0.0.1",port});
  await once(server, "listening");
  const seeded = await inGame(runCapture, {
    reverse, seed: nativeRng[0], otherSeed: nativeRng[2], frames: frameNumbers, visualFrames: [86,105,112,113,120,124,125,126,127,130,140,180]
  });
  for (const {frame, png} of seeded) if (png) await saveImage(`web-step-${frame}`, png);
  report.samples = seeded.map(({png, ...state}) => ({...state,
    originalRng: [nativeFrames.get(state.frame).rng[0], nativeFrames.get(state.frame).rng[2]],
    seconds: Math.ceil(state.remaining / 60)
  }));
  report.firstDifferentRngSample = report.samples.find(row => row.animationRng.some((value, i) => value !== row.originalRng[i])) ?? null;
  let consecutiveMatches = 0;
  for (const row of report.samples) {
    if (row.frame !== consecutiveMatches ||
        row.animationRng.some((value, i) => value !== row.originalRng[i])) break;
    ++consecutiveMatches;
  }
  report.firstConsecutiveMatchingFrames = consecutiveMatches;
  assert.deepEqual(report.samples.slice(0, 125).map(row=>row.frame),
                   Array.from({length:125}, (_,i)=>i), "Missing baseline frames");
  assert.deepEqual(report.samples[0].animationRng, report.samples[0].originalRng);
  assert.equal(report.samples[0].world, reverse?0:1);
  assert.equal(report.samples[0].waveFinalState, nativeRows.find(row=>row.waveRng).waveRng[0]);
  assert.deepEqual(report.samples.find(row=>row.frame===85).waves,[11,11]);
  for(const row of report.samples.filter(row=>row.frame<=124)) assert.deepEqual(row.animationRng,row.originalRng);
  const calls=report.samples.find(row=>row.frame===86).openingDraws.filter(row=>row.kind===10011);
  assert.deepEqual(calls.map(row=>row.seed),reverse?[424182232,1591430374]:[2062611093,515744342]);
  assert.equal(report.samples[0].strips, reverse?0:11);
  assert.equal(report.samples[0].stripFrame, 0);
  for (const row of report.samples) {
    assert.equal(row.remaining, 36000 - row.frame);
    assert.equal(row.stripFrame, reverse?0:row.frame);
  }
  const {rulesPng, ...menu} = await inGame(testMenu);
  await saveImage("vs-rules", rulesPng);
  report.menu = menu;
  assert.deepEqual([menu.rules[1], menu.lunatic[1], menu.local[12], menu.local[19]], [0, 3, 4, 36000]);
  assert.deepEqual([menu.rulesRejected, menu.rulesAccepted, menu.networkBegan, menu.network[19]], [0, 1, 1, 0]);
  report.announcementRollback = await inGame(testAnnouncementRollback);
  assert.deepEqual(report.announcementRollback, {
    visibleAt120:true, expiredAt150:true, visibleAfterRestore:true, identicalRegion:true
  });
  report.shotRollback = await inGame(testShotRollback);
  assert.deepEqual(report.shotRollback, {same: true, bothShooting: true});
  assert.deepEqual(errors, []);
} catch (error) {
  report.failure = error.stack;
  throw error;
} finally {
  report.browserErrors = errors;
  await writeFile(`${directory}/report.json`, JSON.stringify(report, null, 2) + "\n");
  console.log(directory, report.firstDifferentRngSample);
  await browser.close();
  if (server?.listening) await new Promise(resolve => server.close(resolve));
}
