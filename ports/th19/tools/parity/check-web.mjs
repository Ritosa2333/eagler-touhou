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
const continuous = process.argv.includes("--continuous");
if (!capture) throw Error("Usage: node check-web.mjs <capture directory> [--reverse] [--opening-only] [--through FRAME] [--trace-window FIRST LAST] [--rollback-window FIRST LAST]");
const throughIndex = process.argv.indexOf("--through");
const through = throughIndex < 0 ? null : Number(process.argv[throughIndex + 1]);
if (through !== null && (!Number.isSafeInteger(through) || through < 1 || through > 216000)) {
  throw Error("--through requires a positive frame number up to 216000");
}
function frameWindow(option, minimumSpan = 0) {
  const index = process.argv.indexOf(option);
  if (index < 0) return null;
  const first = Number(process.argv[index + 1]), last = Number(process.argv[index + 2]);
  if (!Number.isSafeInteger(first) || !Number.isSafeInteger(last) || first < 1 ||
      last - first < minimumSpan || last > (through ?? 216000)) {
    throw Error(`${option} requires ordered frame numbers within the capture limit`);
  }
  return [first, last];
}
const traceWindow = frameWindow("--trace-window");
const requestedRollback = frameWindow("--rollback-window", 1);
const nativePath = resolve(capture, "native-present.jsonl");
const port = Number(process.env.TH19_CAPTURE_PORT ?? 18439);
const nativeBytes = await readFile(nativePath);
const nativeRows = nativeBytes.toString("utf8").trim().split("\n").map(JSON.parse);
const nativeFrames = new Map();
const nativeFramesWithMultipleRngStates = new Set();
let previousNativeFrame;
let continuousNativeFrame = 0;
for (const row of nativeRows) {
  if ((!continuous && row.segment !== 0) || !Number.isInteger(row.frame)) continue;
  if (continuous && previousNativeFrame) {
    continuousNativeFrame += row.segment === previousNativeFrame.segment
      ? Math.max(0, row.frame - previousNativeFrame.frame) : 1;
  }
  previousNativeFrame = row;
  const frame = continuous ? continuousNativeFrame : row.frame;
  const first = nativeFrames.get(frame);
  if (!first) nativeFrames.set(frame, row);
  else if (row.rng[0] !== first.rng[0] || row.rng[2] !== first.rng[2]) {
    nativeFramesWithMultipleRngStates.add(frame);
  }
}
const nativeOpening = nativeFrames.get(0);
assert.ok(nativeOpening, "Missing native opening frame 0; verify that the original CPU capture entered battle");
const nativeRng = nativeOpening.rng;
const frameNumbers = new Set((through === null
  ? [...Array.from({length: 201}, (_, i) => i), 300, 600]
  : Array.from({length: through + 1}, (_, i) => i)).filter(n => nativeFrames.has(n)));
const includeFrame = frame => {
  if (!nativeFrames.has(frame)) throw Error(`Missing native frame ${frame} in the requested window`);
  frameNumbers.add(frame);
};
if (traceWindow) for (let frame = traceWindow[0]; frame <= traceWindow[1]; ++frame) includeFrame(frame);
if (requestedRollback) requestedRollback.forEach(includeFrame);
const frames = [...frameNumbers].sort((a, b) => a - b);
const bulletFrames = frames.filter(frame => nativeFrames.get(frame).bulletPositions);
const originalHud = {120:598, 300:595, 600:590, 1200:580};
const openingOnly = process.argv.includes("--opening-only");
const identity = JSON.parse(await readFile(resolve(capture, "capture.json"), "utf8"));
assert.ok(!identity.bulletPositions || bulletFrames.length, "Missing requested native bullet positions");
const selection = identity.cpuSelection;
assert.equal(selection?.matchup, 2, "Expected a native CPU-vs-CPU capture");
const settings = nativeRows.find(row => row.segment === 0 && row.settingsWords)?.settingsWords;
assert.ok(settings, "Missing native match settings");
const [common, leftRules, rightRules] = settings;
assert.equal(leftRules, rightRules, "Different per-side match rules are unsupported");
const cardMode = (common >> 6) & 3;
assert.ok(cardMode === 0 || (cardMode === 3 && selection.cards?.length === 2), "Missing verified card selection");
const fixture = {difficulty: selection.difficulty, characters: selection.characters,
  cardMode, cards: cardMode ? selection.cards : [0, 0], lives: (leftRules & 7) + 1, timeRule: common & 7};
const reverse = fixture.characters[0] === 1 && fixture.characters[1] === 0;
if (process.argv.includes("--reverse")) assert.ok(reverse, "Expected Marisa on the left and Reimu on the right");
const runtime = resolve(process.env.TH19_CPU_RUNTIME ?? fileURLToPath(new URL("../../runtime/", import.meta.url)));
const wasmPath = resolve(runtime, "th19.wasm");
const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");
const wasmSha256 = sha256(await readFile(wasmPath));
const output = resolve(process.env.TH19_CAPTURE_OUTPUT || tmpdir());
await mkdir(output, {recursive: true});
const directory = await mkdtemp(resolve(output, "th19-web-"));
const errors = [];
const cpuDebug = [];
const grazeDebug = [];
const bulletDebug = [];
const enemyDebug = [];
let server;
const browser = await puppeteer.launch({
  executablePath: process.env.TH19_CHROME || "C:/Program Files/Google/Chrome/Application/chrome.exe",
  headless: true, protocolTimeout: continuous || through > 18000 ? 900000 : 180000,
  args: ["--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--autoplay-policy=no-user-gesture-required"]
});

async function inGame(action, payload) {
  const context = await browser.createBrowserContext();
  try {
    const page = await context.newPage();
    page.on("console", message => {const line=message.text();if(line.startsWith("CPUDBG "))cpuDebug.push(JSON.parse(line.slice(7)));});
    page.on("console", message => {const line=message.text();if(line.startsWith("GRAZEDBG "))grazeDebug.push(JSON.parse(line.slice(9)));});
    page.on("console", message => {const line=message.text();if(line.startsWith("BULLETDBG "))bulletDebug.push(JSON.parse(line.slice(10)));});
    page.on("console", message => {const line=message.text();if(line.startsWith("ENEMYDBG "))enemyDebug.push(JSON.parse(line.slice(9)));});
    page.on("pageerror", error => errors.push(error.message));
    await page.setViewport({width: 1280, height: 960});
    await page.goto(`http://127.0.0.1:${port}/tests/harness.html?autolaunch=0`);
    await page.waitForFunction(() => window.__TH19_HARNESS.configured, {timeout: 60000});
    await page.evaluate(captureSeed => window.__TH19_HARNESS.command("launch", {captureSeed}), nativeRng[0]);
    await page.evaluate(() => {
      const win = window.__TH19_HARNESS.frame.contentWindow;
      win.__th19ParityImage = () => {
        const source = win.document.querySelector("canvas");
        const copy = win.document.createElement("canvas");
        copy.width = source.width;
        copy.height = source.height;
        const context = copy.getContext("2d");
        context.drawImage(source, 0, 0);
        // The bottom-right FPS counter uses wall time, even on a fixed tick.
        const x = Math.floor(copy.width * 1176 / 1280);
        const y = Math.floor(copy.height * 940 / 960);
        context.clearRect(x, y, copy.width - x, copy.height - y);
        return copy.toDataURL();
      };
    });
    return await page.evaluate(action, payload);
  } finally {
    await context.close();
  }
}

function runCapture({seed, otherSeed, frames, visualFrames, fixture, rollbackWindow, traceWindow, fastCapture, bulletFrames, continuous}) {
  const win = window.__TH19_HARNESS.frame.contentWindow;
  const c = win.__th19Runtime.core;
  c._th19_loop_stop();
  c._th19_loop_pause(1);
  c._th19_test_ticks(180); // Complete the normal loading animation before reseeding.
  if (c._th19_capture_cpu_begin(seed, otherSeed, fixture.difficulty, ...fixture.characters,
      fixture.lives, fixture.timeRule, fixture.cardMode, ...fixture.cards) !== 1) throw Error("CPU match failed to start");
  if (traceWindow && c._th19_capture_cpu_trace) c._th19_capture_cpu_trace(...traceWindow);
  const canvas = win.document.querySelector("canvas");
  const screenshots = new Set(visualFrames);
  const sampleBullets = new Set(bulletFrames);
  const readBullets = side => {
    const count = c._th19_capture_bullet_positions(side, true);
    const offset = c._th19_capture_bullet_positions(side, false) / 4;
    return Array.from({length: count}, (_, i) =>
      Array.from(c.HEAP32.subarray(offset + 4 * i, offset + 4 * i + 4), value => value >>> 0));
  };
  const readState = () => {
    const offset = c._th19_capture_cpu_state() / 4;
    return Array.from(c.HEAP32.subarray(offset, offset + 36), value => value >>> 0);
  };
  let continuousFrame = 0;
  let previousWebState = readState();
  const advanceToFrame = (frame, present, fast) => {
    let elapsed = continuous ? continuousFrame : readState()[29];
    let stalledTicks = 0;
    while (elapsed < frame) {
      if (fast) c._th19_capture_step(present && elapsed === frame - 1);
      else c._th19_test_ticks(1);
      const state = readState();
      if (!continuous && (state[1] !== 1 || state[29] < elapsed)) throw Error(`Round ended before frame ${frame}`);
      if (continuous) {
        continuousFrame += state[1] === previousWebState[1]
          ? Math.max(0, state[29] - previousWebState[29]) : 1;
        previousWebState = state;
      }
      const current = continuous ? continuousFrame : state[29];
      stalledTicks = current === elapsed ? stalledTicks + 1 : 0;
      if (stalledTicks > 600) throw Error(`Web clock stopped at frame ${elapsed}`);
      elapsed = current;
    }
  };
  const results = [];
  let checkpoint = 0;
  let impactRollback;
  for (const frame of frames) {
    const traceRng = frame === 1 || frame === 86 || (frame >= 122 && frame <= 150) ||
      (traceWindow && frame >= traceWindow[0] && frame <= traceWindow[1]);
    if (traceRng) c._th19_capture_rng_trace(1);
    const contacts = c._th19_capture_homing_contacts(1) / 4;
    advanceToFrame(frame, screenshots.has(frame) || frame === rollbackWindow?.[1], fastCapture);
    let openingDraws;
    if (traceRng) {
      const count = c._th19_capture_rng_trace(2), start = c._th19_capture_rng_trace(0)/4;
      openingDraws = Array.from({length:count}, (_,i) => {
        const [kind, seed, time, ip, sprite, primary] = c.HEAP32.slice(start+i*7, start+i*7+7);
        return {kind, seed, time, ip, sprite, primary:!!primary};
      });
    }
    const contactPositions = new Float32Array(c.HEAP32.buffer);
    const homingContacts = Array.from({length: c.HEAP32[contacts]}, (_, i) => {
      const offset = contacts + 1 + i * 3;
      return {side: c.HEAP32[offset], x: contactPositions[offset + 1], y: contactPositions[offset + 2]};
    });
    const row = readState();
    const damageOffset = traceRng ? c._th19_test_player_damage(0) / 4 : 0;
    const playerDamage = traceRng ? Array.from(c.HEAP32.subarray(damageOffset, damageOffset + 12)) : undefined;
    const bulletPositions = sampleBullets.has(frame) ? [readBullets(0), readBullets(1)] : undefined;
    results.push({
      frame, openingDraws, homingContacts, playerDamage, playedFrames: row[0], round: row[1], lives: row.slice(2, 4),
      controls: row.slice(4, 6), enemies: row.slice(6, 8), bullets: row.slice(8, 10),
      cards: row.slice(30, 32), positionBits: row.slice(32, 36),
      bulletPositions,
      animationRng: row.slice(10, 12), world: row[18], strips: row[19], stripFrame: row[20], side: row[21], waves: row.slice(22,24), waveFinalState:row[24], shotClock:row.slice(25,27), shots:row.slice(27,29), elapsed:row[29], remaining: row[15], announcement: !!row[16],
      png: screenshots.has(frame) ? canvas.toDataURL("image/png") : undefined
    });
    if (frame === rollbackWindow?.[0]) {
      checkpoint = c._th19_capture_checkpoint(1);
      if (!checkpoint) throw Error("Impact checkpoint unavailable");
    }
    if (checkpoint && frame === rollbackWindow[1]) {
      const image = win.__th19ParityImage();
      if (!c._th19_capture_checkpoint(2)) throw Error("Impact checkpoint restore failed");
      if (continuous) {
        continuousFrame = rollbackWindow[0];
        previousWebState = readState();
      }
      advanceToFrame(frame, true, false);
      const replayed = readState();
      const differences = row.flatMap((value, index) => value === replayed[index] ? [] :
        [{index, original: value, replayed: replayed[index]}]);
      if (bulletPositions) {
        for (const [side, original] of bulletPositions.entries()) {
          const replayedPositions = readBullets(side);
          if (JSON.stringify(original) !== JSON.stringify(replayedPositions)) {
            differences.push({side, field: "bulletPositions", original, replayed: replayedPositions});
          }
        }
      }
      impactRollback = {
        differences,
        image: image === win.__th19ParityImage(),
      };
      checkpoint = 0;
    }
  }
  if (checkpoint) c._th19_capture_checkpoint(0);
  return {results, impactRollback};
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
    return {state:Array.from(c.HEAP32.subarray(offset,offset+32)), image:win.__th19ParityImage()};
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
  const differences=first.state.flatMap((value,index)=>value===second.state[index]?[]:
    [{index,original:value,replayed:second.state[index]}]);
  return {differences, image:first.image===second.image,
          bothShooting:first.state[27]>0&&first.state[28]>0};
}

function nativeHomingContacts(rows) {
  const contacts = new Map();
  const bits = new DataView(new ArrayBuffer(4));
  const float = value => {
    bits.setUint32(0, value, true);
    return bits.getFloat32(0, true);
  };
  for (const row of rows) {
    if (row.segment !== 0) continue;
    for (const [frame, side, , x, y, type] of row.impacts ?? []) {
      if (type !== 1 || frame < 110 || frame > 130) continue;
      if (!contacts.has(frame)) contacts.set(frame, []);
      contacts.get(frame).push({side, x: float(x), y: float(y)});
    }
  }
  return contacts;
}

const saveImage = (name, png) => writeFile(`${directory}/${name}.png`, Buffer.from(png.split(",")[1], "base64"));

const positions = bits => Array.from(new Float32Array(Uint32Array.from(bits).buffer));

// Native slots are reused. Compare the coordinate multiset within each field,
// kind and color, including duplicate coordinates.
function compareBulletPositions(original, web) {
  const groups = new Map();
  for (const [source, rows] of [["original", original], ["web", web]]) {
    for (const [kind, color, x, y] of rows) {
      const key = `${kind}:${color}`;
      if (!groups.has(key)) groups.set(key, {kind, color, original: [], web: []});
      groups.get(key)[source].push(positions([x, y]));
    }
  }
  const byPosition = ([ax, ay], [bx, by]) => ax - bx || ay - by;
  for (const group of groups.values()) {
    group.original.sort(byPosition);
    group.web.sort(byPosition);
    if (group.original.length !== group.web.length ||
        group.original.some((point, i) => byPosition(point, group.web[i]) !== 0)) return group;
  }
  return null;
}

const report = {
  nativePath,
  nativeExe: identity.originalExecutableSha256,
  nativeTraceSha256: sha256(nativeBytes), wasmPath, wasmSha256,
  fixture,
  continuous,
  initialAnimationRng: [nativeRng[0], nativeRng[2]]
};
try {
  const root = fileURLToPath(new URL("../../", import.meta.url));
  server = startServer({routes:[{prefix:"/runtime/",directory:runtime},{prefix:"/",directory:root}],host:"127.0.0.1",port});
  await once(server, "listening");
  const {results: seeded, impactRollback} = await inGame(runCapture, {
    fixture, seed: nativeRng[0], otherSeed: nativeRng[2], frames, traceWindow, continuous,
    bulletFrames,
    rollbackWindow: requestedRollback ?? (openingOnly ? null : [124, 128]), fastCapture: through !== null,
    visualFrames: through !== null
      ? [...nativeFrames].filter(([frame,row]) => row.image && frame <= through).map(([frame]) => frame)
      : openingOnly ? [86, 125, 140, 180] : [86,105,112,113,120,124,125,126,127,130,140,180]
  });
  for (const {frame, png} of seeded) if (png) await saveImage(`web-step-${frame}`, png);
  report.samples = seeded.map(({png, bulletPositions, ...state}) => ({...state,
    originalRng: [nativeFrames.get(state.frame).rng[0], nativeFrames.get(state.frame).rng[2]],
    originalPositionBits: nativeFrames.get(state.frame).playerPositionBits,
    originalControls: nativeFrames.get(state.frame).playerControls,
    originalRound: nativeFrames.get(state.frame).segment + 1,
    originalRoundFrame: nativeFrames.get(state.frame).frame,
    seconds: Math.ceil(state.remaining / 60)
  }));
  const bulletSamples = seeded.filter(row => row.bulletPositions);
  if (bulletSamples.length) {
    await writeFile(`${directory}/web-bullet-positions.jsonl`, bulletSamples.map(({frame, bulletPositions}) =>
      JSON.stringify({frame, bulletPositions})).join("\n") + "\n");
    report.bulletPositionCoverage = {
      frames: bulletSamples.length, first: bulletSamples[0].frame, last: bulletSamples.at(-1).frame,
      nonemptyFrames: 0, originalBulletSamples: 0, webBulletSamples: 0,
      missingFrames: seeded.filter(row => !row.bulletPositions).map(row => row.frame),
    };
    report.differentBulletPositionFrames = 0;
    report.firstDifferentBulletPosition = null;
    report.firstDifferentBulletCount = null;
    for (const {frame, bulletPositions: web} of bulletSamples) {
      const original = nativeFrames.get(frame).bulletPositions;
      if ([...original, ...web].some(field => field.length)) ++report.bulletPositionCoverage.nonemptyFrames;
      let different = false;
      for (let side = 0; side < 2; ++side) {
        report.bulletPositionCoverage.originalBulletSamples += original[side].length;
        report.bulletPositionCoverage.webBulletSamples += web[side].length;
        if (original[side].length !== web[side].length) {
          report.firstDifferentBulletCount ??= {frame, side, original: original[side].length, web: web[side].length};
        }
        const difference = compareBulletPositions(original[side], web[side]);
        if (difference) {
          different = true;
          report.firstDifferentBulletPosition ??= {frame, side, ...difference};
        }
      }
      if (different) ++report.differentBulletPositionFrames;
    }
  }
  const comparablePositions = report.samples.filter(row => row.originalPositionBits?.length === 4);
  const differentRound = report.samples.find(row => row.round !== row.originalRound || row.elapsed !== row.originalRoundFrame);
  report.firstDifferentRoundClock = differentRound ? {
    frame: differentRound.frame, web: [differentRound.round, differentRound.elapsed],
    original: [differentRound.originalRound, differentRound.originalRoundFrame],
  } : null;
  report.playerPositionCoverage = comparablePositions.length;
  report.firstDifferentPlayerPosition = null;
  for (const row of comparablePositions) {
    const web = positions(row.positionBits), original = positions(row.originalPositionBits);
    if (web.some((value, i) => value !== original[i])) {
      report.firstDifferentPlayerPosition = {frame: row.frame, web, original};
      break;
    }
  }
  const comparableControls = report.samples.filter(row => row.originalControls?.length === 2);
  report.playerControlCoverage = comparableControls.length;
  const differentControls = comparableControls.find(row => row.controls.some((value, i) => value !== row.originalControls[i]));
  report.firstDifferentPlayerControls = differentControls ? {
    frame: differentControls.frame, web: differentControls.controls, original: differentControls.originalControls,
  } : null;
  report.firstDifferentRngSample = report.samples.find(row => row.animationRng.some((value, i) => value !== row.originalRng[i])) ?? null;
  report.firstDifferentRngByStream = Object.fromEntries(['primary', 'animation'].map((stream, index) => {
    const row = report.samples.find(sample => sample.animationRng[index] !== sample.originalRng[index]);
    return [stream, row ? {frame: row.frame, web: row.animationRng[index], original: row.originalRng[index]} : null];
  }));
  const openingEnd = through ?? Math.min(200, frames.at(-1));
  report.nativeFramesWithMultipleRngStates = [...nativeFramesWithMultipleRngStates].filter(frame => frame <= openingEnd);
  report.missingNativeOpeningFrames = Array.from({length: openingEnd + 1}, (_, index) => index)
    .filter(frame => !nativeFrames.has(frame));
  let consecutiveMatches = 0;
  for (const row of report.samples) {
    if (row.frame !== consecutiveMatches ||
        row.animationRng.some((value, i) => value !== row.originalRng[i])) break;
    ++consecutiveMatches;
  }
  report.firstConsecutiveMatchingFrames = consecutiveMatches;
  const nativeContacts = nativeHomingContacts(nativeRows);
  if (nativeContacts.size) {
    report.homingContactSamples = report.samples.filter(row => row.frame >= 110 && row.frame <= 130 &&
      (row.homingContacts.length || nativeContacts.has(row.frame))).map(row => ({
      frame: row.frame, native: nativeContacts.get(row.frame) ?? [], web: row.homingContacts,
    }));
  }
  assert.deepEqual(report.samples[0].cards, fixture.cards);
  assert.deepEqual(report.samples[0].lives, [fixture.lives, fixture.lives]);
  if (requestedRollback) {
    report.frameRollback = {frames: requestedRollback, ...impactRollback};
    assert.deepEqual(impactRollback, {differences: [], image: true});
  }
  if (!openingOnly) {
    const lastBaseline = Math.min(139, openingEnd);
    assert.ok(lastBaseline >= 128, "Missing baseline frames");
    assert.ok(consecutiveMatches > lastBaseline, "RNG differs in the verified opening interval");
    for (const contact of report.homingContactSamples ?? []) {
      assert.deepEqual(contact.web.map(hit => hit.side), contact.native.map(hit => hit.side),
                       `Homing contact differs at frame ${contact.frame}`);
    }
    assert.deepEqual(report.samples[0].animationRng, report.samples[0].originalRng);
    assert.equal(report.samples[0].world, reverse?0:1);
    assert.equal(report.samples[0].waveFinalState, nativeRows.find(row=>row.waveRng).waveRng[0]);
    assert.deepEqual(report.samples.find(row=>row.frame===85).waves,[11,11]);
    const calls=report.samples.find(row=>row.frame===86).openingDraws.filter(row=>row.kind===10011);
    assert.deepEqual(calls.map(row=>row.seed),reverse?[424182232,1591430374]:[2062611093,515744342]);
    if (!requestedRollback) {
      report.impactRollback = impactRollback;
      assert.deepEqual(impactRollback, {differences: [], image: true});
    }
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
    assert.deepEqual(report.shotRollback, {differences: [], image: true, bothShooting: true});
  }
  assert.deepEqual(errors, []);
} catch (error) {
  report.failure = error.stack;
  throw error;
} finally {
  report.browserErrors = errors;
  if(cpuDebug.length)await writeFile(`${directory}/web-cpu-debug.jsonl`,cpuDebug.map(JSON.stringify).join("\n")+"\n");
  if(grazeDebug.length)await writeFile(`${directory}/web-graze-debug.jsonl`,grazeDebug.map(JSON.stringify).join("\n")+"\n");
  if(bulletDebug.length)await writeFile(`${directory}/web-bullet-debug.jsonl`,bulletDebug.map(JSON.stringify).join("\n")+"\n");
  if(enemyDebug.length)await writeFile(`${directory}/web-enemy-debug.jsonl`,enemyDebug.map(JSON.stringify).join("\n")+"\n");
  await writeFile(`${directory}/report.json`, JSON.stringify(report, null, 2) + "\n");
  console.log(JSON.stringify({directory, matchingRngThrough: report.firstConsecutiveMatchingFrames - 1,
    firstDifferentRngFrame: report.firstDifferentRngSample?.frame ?? null,
    firstDifferentRngByStream: report.firstDifferentRngByStream,
    playerControlCoverage: report.playerControlCoverage,
    firstDifferentPlayerControls: report.firstDifferentPlayerControls,
    playerPositionCoverage: report.playerPositionCoverage ?? 0,
    firstDifferentPlayerPosition: report.firstDifferentPlayerPosition ?? null,
    bulletPositionCoverage: report.bulletPositionCoverage,
    differentBulletPositionFrames: report.differentBulletPositionFrames,
    firstDifferentBulletCount: report.firstDifferentBulletCount,
    firstDifferentBulletPosition: report.firstDifferentBulletPosition}));
  await browser.close();
  if (server?.listening) await new Promise(resolve => server.close(resolve));
}
