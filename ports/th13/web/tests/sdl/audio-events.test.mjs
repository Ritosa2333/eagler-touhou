// Production browser audio-event matrix for the real TH13 stage-1 replay.
//
// Expected rows below are transcribed from the original v1.00c SFX request
// trace, not from this Web implementation. The first 600 rows were verified
// against the checked-in reference trace
//   artifacts/th13-longrun-20261029/sound-native-600/battle-sound-trace.jsonl
// and the full-stage rows come from the same Unicorn battle oracle run with the
// same real replay (games/replays/th13_01.rpy, stage 0, 11675 frames):
//   TH13_SOUND_TRACE=1 python3 -m oracle.battle_oracle \
//     games/th13/th13.exe artifacts/reference/raw /tmp/battle-frame-counts.jsonl \
//     --replay games/replays/th13_01.rpy --replay-stage 0 --frames 11675 \
//     --draw-log off --out-dir /tmp/native-full
// The Web side only supplies the observed production output; it is never used
// to derive expected values. Timing is compared by exact frame id, request id,
// positional flag and (where transcribed) the original x coordinate bits.
//
// Covered matrix:
//   pause activation voice 14 (physical Esc on a frozen real replay frame)
//   spirit birth 60, spirit collect 63, spirit gauge 68 (0x44)
//   big-power pair 13 (0x0d power level) + 37 (0x25 item collect)
//   spell start 0x21 (33) and spell capture 0x2e (46)
//
// Not covered here: the 0x15/0x26/0x2a gaps visible in the same native trace
// (IDs 21/38/42) are outside this matrix and are reported separately; see the
// final report for their native counts and callers.
import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {dirname, join, resolve, extname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const portRoot = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const runtimeDir = join(portRoot, 'web/sdl-runtime');
const buildRoot = process.env.TH13_SDL3_BUILD_DIR ?? join(portRoot, 'web/artifacts');
const buildDir = join(buildRoot, 'sdl3');
const replayPath = join(runtimeDir, 'replays', 'th13_01.rpy');
const replayName = 'th13_01.rpy';
const REPLAY_SHA256 = '2869c854167537e066579c1435d794bbefb06ed49b771a74904b0c394a5f9d09';
const STAGE_FRAMES = 11675;
const PAUSE_AFTER_FRAMES = 30;

// Original trace rows, frame <= 600: [frame, sound_id, positional, x_bits].
const FIRST_600_TARGET_STREAM = [
  [98,60,1,3271369687],[128,60,1,3265897431],[179,60,1,1116813394],[216,60,1,1116034826],[237,60,1,3263274680],[267,63,1,3270950257],
  [272,37,1,3271191337],[277,63,1,3266305065],[300,60,1,3198072914],[301,60,1,1034147594],[331,60,1,3250611814],[333,63,1,1119142543],
  [365,60,1,3255322542],[378,60,1,3245678264],[380,37,1,3265629282],[402,60,1,3242815652],[409,37,1,1116107719],[437,60,1,1083661353],
  [456,37,1,1084182871],[457,60,1,3225545605],[460,63,1,3229362422],[472,63,1,3251041731],[472,63,1,3250632786],[476,37,1,3241800564],
  [487,37,1,3250360127],[492,37,1,3250984618],[501,37,1,3240598723],[503,37,1,3216650852],[505,37,1,1084808394],[505,63,1,3229656023],
  [506,37,1,1088121913],[507,37,1,1100453702],[512,63,1,1105555620],[514,60,1,1115304755],[514,63,1,1108064338],[515,37,1,1106521515],
  [516,63,1,1107159941],[518,37,1,1111645686],[522,37,1,1110363274],[544,63,1,1104176742],[545,63,1,1101146358],[564,60,1,3264688947],
  [576,60,1,3265803059],[598,63,1,1095751434],[598,63,1,1094608486],
];

// Original full-stage stream for the spirit/spell matrix: [frame, id, positional].
// 60 = spirit birth, 63 = spirit collect, 68 = 0x44 spirit gauge crossing,
// 33 = 0x21 spell start, 46 = 0x2e spell capture, 13 = 0x0d power level-up.
const SPIRIT_SPELL_STREAM = [
  [98,60,1],[128,60,1],[179,60,1],[216,60,1],[237,60,1],[267,63,1],[277,63,1],[300,60,1],
  [301,60,1],[331,60,1],[333,63,1],[365,60,1],[378,60,1],[402,60,1],[437,60,1],[457,60,1],
  [460,63,1],[472,63,1],[472,63,1],[505,63,1],[512,63,1],[514,60,1],[514,63,1],[516,63,1],
  [544,63,1],[545,63,1],[564,60,1],[576,60,1],[598,63,1],[598,63,1],[615,60,1],[680,60,1],
  [796,63,1],[807,63,1],[833,63,1],[845,63,1],[845,63,1],[850,63,1],[907,60,1],[927,60,1],
  [947,60,1],[965,60,1],[986,60,1],[1004,60,1],[1025,60,1],[1046,60,1],[1064,60,1],[1085,60,1],
  [1106,60,1],[1125,60,1],[1141,63,1],[1141,63,1],[1142,63,1],[1144,63,1],[1145,63,1],[1149,63,1],
  [1161,63,1],[1164,63,1],[1165,63,1],[1165,63,1],[1165,63,1],[1165,63,1],[1165,63,1],[1168,63,1],
  [1173,63,1],[1279,60,1],[1288,60,1],[1296,60,1],[1321,60,1],[1461,60,1],[1461,60,1],[1461,60,1],
  [1688,60,1],[1707,63,1],[1720,63,1],[1758,63,1],[1772,60,1],[1772,60,1],[1828,60,1],[1875,63,1],
  [1952,60,1],[2052,60,1],[2052,60,1],[2052,60,1],[2090,60,1],[2126,60,1],[2144,60,1],[2457,63,1],
  [2471,63,1],[2477,63,1],[2478,63,1],[2489,63,1],[2522,60,1],[2567,60,1],[2608,60,1],[2679,60,1],
  [2724,60,1],[2778,60,1],[2842,60,1],[2933,60,1],[2955,60,1],[2989,60,1],[3078,60,1],[3096,60,1],
  [3110,60,1],[3157,60,1],[3178,60,1],[3232,60,1],[3232,60,1],[3232,60,1],[3271,63,1],[3273,63,1],
  [3277,63,1],[3280,63,1],[3302,63,1],[3303,63,1],[3311,63,1],[3317,63,1],[3323,63,1],[3325,63,1],
  [3331,63,1],[3342,63,1],[3347,63,1],[3348,63,1],[3361,63,1],[3491,60,1],[3569,60,1],[3604,60,1],
  [3851,60,1],[3851,60,1],[3851,60,1],[4007,60,1],[4007,60,1],[4102,60,1],[4177,60,1],[4717,60,1],
  [4717,60,1],[4742,60,1],[4752,60,1],[4878,60,1],[5243,13,1],[5825,60,1],[5891,60,1],[5957,60,1],
  [6026,60,1],[6091,60,1],[6157,60,1],[6198,60,1],[6237,60,1],[6242,63,1],[6296,60,1],[6315,63,1],
  [6356,60,1],[6387,63,1],[6396,60,1],[6452,60,1],[6465,63,1],[6482,33,0],[6484,63,1],[6502,60,1],
  [6532,63,1],[6564,63,1],[6583,63,1],[6657,60,1],[6693,60,1],[6735,60,1],[6803,60,1],[6887,60,1],
  [7126,60,1],[7288,60,1],[7366,60,1],[7455,63,1],[7483,60,1],[7549,60,1],[7822,60,1],[7843,46,0],
  [7905,63,1],[7908,63,1],[7909,63,1],[7910,63,1],[7911,63,1],[7912,63,1],[7914,63,1],[7921,63,1],
  [7924,63,1],[7924,63,1],[7928,63,1],[7945,63,1],[7948,63,1],[7963,63,1],[7964,63,1],[7972,63,1],
  [7973,60,1],[7980,63,1],[7986,63,1],[7989,63,1],[8005,63,1],[8007,63,1],[8007,68,0],[8007,63,1],
  [8027,63,1],[8028,63,1],[8035,63,1],[8063,60,1],[8103,60,1],[8158,60,1],[8231,60,1],[8305,60,1],
  [8399,60,1],[8484,60,1],[8558,60,1],[8630,60,1],[8751,60,1],[8795,33,0],[8826,60,1],[8929,63,1],
  [8930,63,1],[8946,63,1],[8947,63,1],[9014,60,1],[9054,60,1],[9123,60,1],[9207,60,1],[9273,60,1],
  [9339,60,1],[9405,60,1],[9473,60,1],[9545,60,1],[9669,63,1],[9702,63,1],[9714,60,1],[9723,63,1],
  [9754,60,1],[9794,46,0],[9800,60,1],[9869,63,1],[9870,63,1],[9879,63,1],[9884,63,1],[9885,63,1],
  [9892,63,1],[9901,63,1],[9914,33,0],[9937,63,1],[9939,60,1],[9940,63,1],[9948,63,1],[9949,63,1],
  [9951,63,1],[9952,63,1],[9953,63,1],[9954,63,1],[9954,63,1],[9958,63,1],[9960,63,1],[9963,63,1],
  [9964,63,1],[9987,63,1],[9989,63,1],[9989,63,1],[9991,63,1],[10001,60,1],[10011,63,1],[10014,63,1],
  [10036,63,1],[10058,60,1],[10136,60,1],[10202,60,1],[10263,60,1],[10303,60,1],[10343,60,1],[10410,60,1],
  [10479,60,1],[10545,60,1],[10631,60,1],[10700,60,1],[10775,60,1],[10845,60,1],[10917,60,1],[10989,60,1],
  [11064,60,1],[11135,60,1],[11206,60,1],[11275,60,1],[11309,63,1],[11311,46,0],[11338,63,1],[11339,63,1],
  [11357,63,1],[11380,63,1],[11386,63,1],[11396,63,1],
];

const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

function buildIdentity() {
  return (async () => {
    const manifestBytes = await readFile(join(buildDir, 'build.json'));
    const manifest = JSON.parse(manifestBytes);
    const wasmBytes = await readFile(join(buildDir, 'th13-sdl.wasm'));
    const loaderBytes = await readFile(join(buildDir, 'th13-sdl.mjs'));
    const identity = {
      builtAt: manifest.builtAt,
      manifestSha256: sha256(manifestBytes),
      wasmSha256: sha256(wasmBytes),
      loaderSha256: sha256(loaderBytes),
      manifestWasmSha256: manifest.sha256,
      manifestLoaderSha256: manifest.loaderSha256,
      sourceDrift: 0,
      sourceMissing: 0,
    };
    assert.equal(identity.wasmSha256, identity.manifestWasmSha256,
      'served th13-sdl.wasm must match build.json; a new production build is owned by the main session');
    assert.equal(identity.loaderSha256, identity.manifestLoaderSha256,
      'served th13-sdl.mjs must match build.json');
    for (const [path, expected] of Object.entries(manifest.sourceFiles ?? {})) {
      try {
        if (sha256(await readFile(path)) !== expected) ++identity.sourceDrift;
      } catch {
        ++identity.sourceMissing;
      }
    }
    return identity;
  })();
}

async function withProductionPage(run) {
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      const file = path.startsWith('/artifacts/')
        ? join(buildRoot, path.slice('/artifacts/'.length))
        : join(runtimeDir, path.replace(/^\/sdl-runtime\//, ''));
      const data = await readFile(file);
      const type = {'.mjs': 'text/javascript', '.wasm': 'application/wasm',
        '.html': 'text/html', '.png': 'image/png'}[extname(file)] ?? 'application/octet-stream';
      response.writeHead(200, {'Content-Type': type}).end(data);
    } catch {
      response.writeHead(404).end();
    }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  const pageErrors = [];
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}});
    page.on('pageerror', error => pageErrors.push(String(error)));
    // The original reference trace was produced with synthetic boot clock 0.
    await page.addInitScript(() => { window.__th13_boot_seed = 0; });
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
      {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 180000});
    const result = await run(page);
    return {...result, pageErrors};
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
}

async function replayBytes() {
  try { return await readFile(replayPath); }
  catch { return null; }
}

function startReplay(page, bytes) {
  return page.evaluate(({bytes, replayName}) => {
    const mod = window.mod;
    mod._th13_loop_stop();
    mod.FS.writeFile('/assets/' + replayName, new Uint8Array(bytes));
    const name = mod.allocateUTF8(replayName);
    let stageFrames = 0;
    try {
      stageFrames = mod._th13_debug_replay(name, 0);
    } finally {
      mod._free(name);
    }
    if (!stageFrames) throw new Error(mod.UTF8ToString(mod._th13_error()));
    mod._th13_debug_audio_observe(1);
    return stageFrames;
  }, {bytes: [...bytes], replayName});
}

test('pause activation emits original voice 14 at the frozen replay frame',
    {timeout: 240000}, async t => {
  const identity = await buildIdentity();
  const bytes = await replayBytes();
  if (!bytes) {
    t.skip(`missing real replay fixture ${replayPath}; pause/audio matrix not covered`);
    return;
  }
  assert.equal(sha256(bytes), REPLAY_SHA256,
    'replay fixture must be the original bytes used by the native reference');
  const result = await withProductionPage(async page => {
    const stageFrames = await startReplay(page, bytes);
    assert.equal(stageFrames, STAGE_FRAMES);
    return page.evaluate(frames => {
      const mod = window.mod;
      const events = [];
      const take = () => {
        const text = mod.UTF8ToString(mod._th13_debug_audio_take());
        for (const line of text.split('\n')) if (line) events.push(JSON.parse(line));
      };
      const progress = () => JSON.parse(mod.UTF8ToString(mod._th13_debug_replay_progress()));
      for (let i = 0; i < frames; ++i) {
        if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
        take();
      }
      const beforePause = progress();
      const beforePauseEvents = events.length;
      mod._th13_key(1, 1);  // physical Esc
      if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
      take();
      mod._th13_key(1, 0);
      const afterPause = progress();
      for (let i = 0; i < 2; ++i) {
        if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
        take();
      }
      const after = progress();
      return {beforePause, beforePauseEvents, afterPause, after, events,
        bootSeed: mod._th13_boot_seed(), error: mod.UTF8ToString(mod._th13_error())};
    }, PAUSE_AFTER_FRAMES);
  });
  assert.equal(result.error, '');
  assert.deepEqual(result.pageErrors, []);
  assert.equal(result.bootSeed, 0, 'the native reference trace uses synthetic boot clock 0');
  assert.equal(result.beforePause.consumed, PAUSE_AFTER_FRAMES);
  assert.equal(result.beforePause.environment_paused, false);
  const prePause = result.events.slice(0, result.beforePauseEvents);
  assert.equal(prePause.filter(event => event.kind === 'voice' && event.id === 14).length, 0,
    'voice 14 must not appear before the physical pause input');
  const pauseEvents = result.events.slice(result.beforePauseEvents);
  const pauseVoices = pauseEvents.filter(event => event.kind === 'voice');
  assert.deepEqual(pauseVoices.map(event => event.id), [14],
    'pause activation must emit exactly voice 14');
  const pause = pauseVoices[0];
  assert.equal(pause.frame, PAUSE_AFTER_FRAMES - 1,
    'voice 14 must be stamped at the frozen replay frame');
  assert.equal(pause.positional, false);
  assert.equal(pause.value, 0);
  assert.equal(pause.x_bits, 0);
  assert.equal(result.afterPause.consumed, PAUSE_AFTER_FRAMES,
    'pause activation must not consume a replay input frame');
  assert.equal(result.afterPause.overlay, 1);
  assert.equal(result.afterPause.scene_flags & 0x10, 0x10);
  assert.equal(result.afterPause.environment_paused, true);
  assert.equal(result.after.consumed, PAUSE_AFTER_FRAMES);
  assert.equal(result.after.environment_paused, true);
  t.diagnostic(JSON.stringify({buildIdentity: identity,
    pause: {frame: pause.frame, id: pause.id, positional: pause.positional}}));
});

test('stage-1 real replay emits the original spirit, big-power and spell sound matrix',
    {timeout: 900000}, async t => {
  const identity = await buildIdentity();
  const bytes = await replayBytes();
  if (!bytes) {
    t.skip(`missing real replay fixture ${replayPath}; stage-1 audio matrix not covered`);
    return;
  }
  assert.equal(sha256(bytes), REPLAY_SHA256,
    'replay fixture must be the original bytes used by the native reference');
  const result = await withProductionPage(async page => {
    const stageFrames = await startReplay(page, bytes);
    assert.equal(stageFrames, STAGE_FRAMES);
    return page.evaluate(frames => {
      const mod = window.mod;
      const events = [];
      const take = () => {
        const text = mod.UTF8ToString(mod._th13_debug_audio_take());
        for (const line of text.split('\n')) if (line) events.push(JSON.parse(line));
      };
      for (let i = 0; i < frames; ++i) {
        if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
        take();
      }
      const finalProgress = JSON.parse(mod.UTF8ToString(mod._th13_debug_replay_progress()));
      return {events, finalProgress, bootSeed: mod._th13_boot_seed(),
        error: mod.UTF8ToString(mod._th13_error())};
    }, STAGE_FRAMES);
  });
  assert.equal(result.error, '');
  assert.deepEqual(result.pageErrors, []);
  assert.equal(result.bootSeed, 0, 'the native reference trace uses synthetic boot clock 0');
  assert.equal(result.finalProgress.consumed, STAGE_FRAMES);
  assert.equal(result.finalProgress.slot_frame, STAGE_FRAMES);
  assert.equal(result.finalProgress.slot_frames, STAGE_FRAMES);

  const voice = result.events.filter(event => event.kind === 'voice');
  const targetIds = new Set([13, 33, 37, 46, 60, 63, 68]);
  const observedFirst600 = voice
    .filter(event => event.frame <= 600 && targetIds.has(event.id))
    .map(event => [event.frame, event.id, event.positional ? 1 : 0, event.x_bits]);
  assert.deepEqual(observedFirst600, FIRST_600_TARGET_STREAM,
    'first 600 target frames/order/positions must match the original 600-frame trace');

  const spiritSpellIds = new Set([13, 33, 46, 60, 63, 68]);
  const observedSpiritSpell = voice
    .filter(event => spiritSpellIds.has(event.id))
    .map(event => [event.frame, event.id, event.positional ? 1 : 0]);
  assert.deepEqual(observedSpiritSpell, SPIRIT_SPELL_STREAM,
    'spirit birth/collect/gauge, big-power level and spell start/capture must match the original full-stage stream');

  const count = id => voice.filter(event => event.id === id).length;
  assert.equal(count(13), 1, 'one big-power level-up event');
  assert.equal(count(37), 794, 'all ordinary item-collect requests remain present');
  assert.equal(count(33), 3, 'three spell starts');
  assert.equal(count(46), 3, 'three spell captures');
  assert.equal(count(60), 150, 'spirit birth requests');
  assert.equal(count(63), 134, 'spirit collect requests');
  assert.equal(count(68), 1, 'one spirit-gauge crossing');
  assert.equal(count(14), 0, 'a replay-only run must not emit pause voice 14');

  const bigPower = voice
    .filter(event => event.frame === 5243 && (event.id === 13 || event.id === 37))
    .map(event => [event.id, event.positional ? 1 : 0, event.x_bits]);
  assert.deepEqual(bigPower, [[13, 1, 1120351552], [37, 1, 1120351552]],
    'big-power collect must emit voice 13 then voice 37 at the same original frame and position');

  const spellOrder = voice
    .filter(event => event.id === 33 || event.id === 46)
    .map(event => [event.frame, event.id]);
  assert.deepEqual(spellOrder, [[6482, 33], [7843, 46], [8795, 33], [9794, 46], [9914, 33], [11311, 46]],
    'spell start and capture order must match the original trace');

  const gaugeFrame = voice
    .filter(event => event.frame === 8007 && (event.id === 63 || event.id === 68))
    .map(event => [event.id, event.positional ? 1 : 0, event.x_bits]);
  assert.deepEqual(gaugeFrame,
    [[63, 1, 1117905224], [68, 0, 0], [63, 1, 1117728276]],
    'spirit gauge voice 68 must sit between the two original spirit collects on frame 8007');

  t.diagnostic(JSON.stringify({buildIdentity: identity,
    events: result.events.length, voice: voice.length,
    finalProgress: result.finalProgress}));
});
