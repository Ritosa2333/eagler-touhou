// Real-browser scenario for the Stage 1 boss death on the production WASM build.
//
// Covered behaviour (all expected values come from the original v1.00c data,
// never from this Web build's output):
//   1. Boss-death explosion ANM identity and timing.
//      artifacts/reference/dumps/ecl/st01bs.txt BossDead(): on the death frame
//      ins_307(1,25) then ins_307(1,96); after the +60 wait ins_307(1,25),
//      ins_307(1,97), ins_516(5). artifacts/reference/dumps/ecl/st01.txt
//      MBossDead() uses ins_307(1,98) and ins_516(5) for the midboss.
//      artifacts/reference/dumps/anm-list.txt effect.anm entry2 is
//      effect/eff_base.png; scripts 25/96/97/98/99 run on layer 16
//      (ins_304(16)); script25 selects sprite25, script99 selects sprite31;
//      scripts 96/97/98 are blank roots that spawn script99 children
//      (script96 loop count 30, script97 60+60, script98 30+30).
//   2. Blue HP-ring (front.anm scripts 240/241/242/243) appearance on layer 17
//      with texture front/lifebar.png (sprites 150/151/152) and the
//      proximity-driven 10-frame alpha fade.
//      artifacts/decomp/jp/functions/004265a0.c compares the player/boss
//      distance against 6400 (enter, label 3) and 9216 (leave, label 2); the
//      front.anm scripts answer with ins_409(10,0,64) (dim to alpha 64 over
//      10 frames) and ins_409(10,0,255) (restore over 10 frames). The same
//      function's gate-failure branch marks the seven ring VMs with
//      render_flags 0x20, which 0x46f170 collects on its next pass.
//   3. Audio death markers from the checked-in native trace
//      artifacts/audio-probe-tmp/native-full/battle-sound-trace.jsonl:
//      voice 5 at frames 3232 (x_bits 3249647124), 11251 (x_bits 0) and
//      11311 (x_bits 1101497631); voice 46 at 11311; voice 33 at 9914.
//   4. No pageerror and no unexpected console error on the production page.
//
// The scenario uses the real Reimu Stage 1 replay (games/replays/th13_01.rpy,
// byte-identical fixture in web/sdl-runtime/replays) and only reads the
// existing debug exports; no product source or probe is changed.
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
const REPLAY_SHA256 = '2869c854167537e066579c1435d794bbefb06ed49b771a74904b0c394a5f9d09';
const STAGE_FRAMES = 11675;
const STOP_FRAME = 11325;  // covers the +60 BossDead batch at 11311

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
      'served th13-sdl.wasm must match build.json');
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
  const consoleMessages = [];
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}});
    page.on('pageerror', error => pageErrors.push(String(error)));
    page.on('console', message => consoleMessages.push({type: message.type(), text: message.text(),
      url: message.location().url}));
    // The original reference trace was produced with synthetic boot clock 0.
    await page.addInitScript(() => { window.__th13_boot_seed = 0; });
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
      {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 180000});
    const result = await run(page);
    return {...result, pageErrors, consoleMessages};
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
  }, {bytes: [...bytes], replayName: 'th13_01.rpy'});
}

// Frames are reported as game frames (consumed replay inputs - 1), the same
// numbering used by the native sound trace and the replay state probes.
function collectScenario(page) {
  return page.evaluate(({windows, stopFrame}) => {
    const mod = window.mod;
    const events = [];
    const takeAudio = () => {
      const text = mod.UTF8ToString(mod._th13_debug_audio_take());
      for (const line of text.split('\n')) if (line) events.push(JSON.parse(line));
    };
    const inWindow = frame => windows.some(([first, last]) => frame >= first && frame <= last);
    const samples = {};
    const takeSample = frame => {
      const state = JSON.parse(mod.UTF8ToString(mod._th13_debug_anm_state()));
      const core = JSON.parse(mod.UTF8ToString(mod._th13_debug_replay_core()));
      const ring = [];
      const effects = [];
      for (const vm of state.vms) {
        if (vm.bank === 3 && [240, 241, 242, 243].includes(vm.script)) {
          ring.push({script: vm.script, layer: vm.layer, sprite: vm.sprite,
            tex: vm.texture_name, alpha: (vm.color >>> 24) & 255, handle: vm.handle,
            parent: vm.parent, translation: vm.translation_bits, pending: vm.pending});
        }
        if (vm.bank === 8 && [25, 96, 97, 98, 99].includes(vm.script)) {
          effects.push({script: vm.script, layer: vm.layer, sprite: vm.sprite,
            tex: vm.texture_name, alpha: (vm.color >>> 24) & 255,
            handle: vm.handle, parent: vm.parent});
        }
      }
      samples[frame] = {ring, effects,
        player: [core.player.x_bits, core.player.y_bits]};
    };
    let frame = -1;
    while (frame < stopFrame) {
      if (!mod._th13_debug_frame()) throw new Error(mod.UTF8ToString(mod._th13_error()));
      takeAudio();
      frame = mod._th13_debug_replay_frame() - 1;
      if (inWindow(frame)) takeSample(frame);
    }
    return {events, samples,
      progress: JSON.parse(mod.UTF8ToString(mod._th13_debug_replay_progress())),
      bootSeed: mod._th13_boot_seed(), error: mod.UTF8ToString(mod._th13_error())};
  }, {windows: [[3228, 3242], [6638, 6670], [9980, 10022], [11245, STOP_FRAME]],
    stopFrame: STOP_FRAME});
}

const bitsToFloat = bits => {
  const view = new DataView(new ArrayBuffer(4));
  view.setUint32(0, bits >>> 0, true);
  return view.getFloat32(0, true);
};

test('stage-1 real replay: boss explosion ANM, blue HP ring fade and clean page',
    {timeout: 900000}, async t => {
  const identity = await buildIdentity();
  const bytes = await replayBytes();
  if (!bytes) {
    t.skip(`missing real replay fixture ${replayPath}; boss death scenario not covered`);
    return;
  }
  assert.equal(sha256(bytes), REPLAY_SHA256,
    'replay fixture must be the original bytes used by the native reference');

  const result = await withProductionPage(async page => {
    const stageFrames = await startReplay(page, bytes);
    assert.equal(stageFrames, STAGE_FRAMES);
    return collectScenario(page);
  });

  // ---- production page cleanliness ----
  assert.equal(result.error, '');
  assert.deepEqual(result.pageErrors, [], 'pageerror raised by the runtime page');
  // The test server has no favicon.ico; that 404 is host-page noise, not a
  // product failure. Every other console error fails the scenario.
  const unexpectedConsole = result.consoleMessages.filter(message =>
    message.type === 'error' && !message.url.endsWith('/favicon.ico'));
  assert.deepEqual(unexpectedConsole, [], 'unexpected console errors');
  assert.equal(result.bootSeed, 0, 'the native reference trace uses synthetic boot clock 0');
  assert.equal(result.progress.slot_frames, STAGE_FRAMES);
  assert.equal(result.progress.current_stage, 1);
  assert.equal(result.progress.active, true);

  const samples = new Map(Object.entries(result.samples)
    .map(([frame, value]) => [Number(frame), value]));
  const sample = frame => {
    const value = samples.get(frame);
    assert.ok(value, `missing state sample at game frame ${frame}`);
    return value;
  };
  const ringAt = frame => sample(frame).ring;
  const effectsAt = frame => sample(frame).effects;
  const ringAlpha = (frame, script) => {
    const vm = ringAt(frame).find(value => value.script === script);
    return vm ? vm.alpha : null;
  };

  // ---- native death audio markers ----
  const voice = result.events.filter(event => event.kind === 'voice');
  const voiceAt = (id, frame) =>
    voice.filter(event => event.id === id && event.frame === frame);
  assert.deepEqual(voice.filter(event => event.id === 5).map(event => event.frame),
    [3232, 11251, 11311],
    'voice 5 must appear exactly at the midboss and main-boss death frames');
  assert.deepEqual(voiceAt(5, 3232).map(event => [event.positional, event.x_bits]),
    [[true, 3249647124]]);
  assert.deepEqual(voiceAt(5, 11251).map(event => [event.positional, event.x_bits]),
    [[true, 0]]);
  assert.deepEqual(voiceAt(5, 11311).map(event => [event.positional, event.x_bits]),
    [[true, 1101497631]]);
  assert.equal(voiceAt(46, 11311).length, 1,
    'the final spell capture must land on the BossDead +60 frame');
  assert.equal(voiceAt(33, 9914).length, 1,
    'the final spell start frame must match the native trace');

  // ---- midboss explosion: MBossDead ins_307(1,98) / ins_516(5) ----
  const midboss = effectsAt(3232);
  const midbossRoot = midboss.filter(vm => vm.script === 98);
  assert.equal(midbossRoot.length, 1, 'MBossDead must create exactly one effect.anm script98');
  assert.equal(midbossRoot[0].layer, 16);
  assert.equal(midbossRoot[0].parent, 0);
  const midbossChildren = frame => effectsAt(frame)
    .filter(vm => vm.script === 99 && vm.parent === midbossRoot[0].handle);
  assert.ok(midbossChildren(3232).length >= 30,
    'script98 must spawn its script-literal 30 particles');
  for (const child of midbossChildren(3232)) {
    assert.equal(child.layer, 16);
    assert.equal(child.sprite, 31);
    assert.equal(child.tex, 'effect/eff_base.png');
  }
  assert.equal(midbossChildren(3235).length, 60,
    'script98 spawns 30+30 particles in total');

  // ---- blue HP ring appearance on the last spell (front.anm 240-243) ----
  const ring = ringAt(11250);
  assert.deepEqual(ring.map(vm => vm.script).sort((a, b) => a - b),
    [240, 241, 242, 243, 243, 243, 243],
    'three ring bands plus four phase stars must be live before the boss dies');
  for (const vm of ring) {
    assert.equal(vm.layer, 17, 'front.anm ring scripts select layer 17 (ins_304(17))');
    assert.equal(vm.tex, 'front/lifebar.png');
  }
  const arc = ring.find(vm => vm.script === 240);
  const overlays = ring.filter(vm => vm.script === 241 || vm.script === 242);
  assert.equal(arc.sprite, 150);
  assert.deepEqual(overlays.map(vm => vm.sprite), [151, 151]);
  assert.deepEqual(ring.filter(vm => vm.script === 243).map(vm => vm.sprite), [152, 152, 152, 152]);
  for (const vm of [arc, ...overlays])
    assert.deepEqual(vm.translation, arc.translation,
      'the three ring bands share the boss world position (0x4265a0 write)');

  // ---- proximity alpha fade timing: 004265a0 6400/9216 + ins_409(10,...) ----
  const fadeWindows = [[6638, 6670], [9980, 10022]];
  const fades = [];
  for (const [first, last] of fadeWindows) {
    const rows = [...samples.keys()]
      .filter(frame => frame >= first && frame <= last)
      .sort((a, b) => a - b)
      .map(frame => {
        const arcVm = ringAt(frame).find(vm => vm.script === 240);
        const player = sample(frame).player;
        const distanceSquared = (bitsToFloat(player[0]) - bitsToFloat(arcVm.translation[0])) ** 2 +
          (bitsToFloat(player[1]) - bitsToFloat(arcVm.translation[1])) ** 2;
        return {frame, distanceSquared, alphas: ringAt(frame).map(vm => vm.alpha)};
      });
    assert.ok(rows.every(row => row.alphas.length === 7),
      'all seven ring VMs must be present across the fade window');
    const trigger = rows.findIndex(row => row.distanceSquared < 6400);
    assert.ok(trigger > 0, 'the replay must bring the player inside the 80px ring radius');
    assert.ok(rows[trigger - 1].distanceSquared >= 6400);
    assert.equal(rows[trigger - 1].alphas[0], 255);
    assert.ok(rows[trigger].alphas[0] < 255, 'the ring must start dimming on the entry frame');
    const dim = rows.slice(trigger, trigger + 10);
    assert.equal(dim.length, 10, 'ins_409(10,0,64) must take ten frames');
    for (const row of dim)
      assert.ok(row.alphas.every(alpha => alpha === row.alphas[0]),
        'the same label must reach all seven ring VMs');
    for (let i = 1; i < dim.length; ++i)
      assert.ok(dim[i].alphas[0] < dim[i - 1].alphas[0], 'dimming must be monotonic');
    assert.ok(dim[8].alphas[0] > 64);
    assert.equal(dim[9].alphas[0], 64, 'the dimmed ring must settle on alpha 64');
    let restore = -1;
    for (let i = trigger + 10; i < rows.length; ++i)
      if (rows[i].distanceSquared >= 9216) { restore = i; break; }
    assert.ok(restore > 0, 'the replay must move the player outside the 96px restore radius');
    for (let i = trigger + 10; i < restore; ++i)
      assert.equal(rows[i].alphas[0], 64, 'the ring stays dimmed between the two thresholds');
    assert.equal(rows[restore - 1].alphas[0], 64);
    assert.ok(rows[restore].alphas[0] > 64, 'the ring must start restoring on the exit frame');
    const lift = rows.slice(restore, restore + 10);
    assert.equal(lift.length, 10, 'ins_409(10,0,255) must take ten frames');
    for (const row of lift)
      assert.ok(row.alphas.every(alpha => alpha === row.alphas[0]));
    for (let i = 1; i < lift.length; ++i)
      assert.ok(lift[i].alphas[0] > lift[i - 1].alphas[0], 'restore must be monotonic');
    assert.ok(lift[8].alphas[0] < 255);
    assert.equal(lift[9].alphas[0], 255, 'the restored ring must settle on alpha 255');
    fades.push({trigger: rows[trigger].frame, restore: rows[restore].frame,
      dim: dim.map(row => row.alphas[0]), lift: lift.map(row => row.alphas[0])});
  }

  // ---- main-boss death: ring retirement and effect.anm identity ----
  assert.deepEqual(effectsAt(11250), [],
    'no death explosion may exist before the BossDead frame');
  assert.equal(ringAt(11251).length, 0,
    'the death frame must retire the ring group (0x4265a0 gate-failure 0x20 branch)');
  const death = effectsAt(11251);
  const firstBurst = death.filter(vm => vm.script === 25);
  assert.equal(firstBurst.length, 1, 'BossDead creates effect.anm script25 on the death frame');
  assert.equal(firstBurst[0].layer, 16);
  assert.equal(firstBurst[0].sprite, 25);
  assert.equal(firstBurst[0].tex, 'effect/eff_base.png');
  assert.ok(firstBurst[0].alpha > 0 && firstBurst[0].alpha < 255,
    'script25 starts its 30-frame alpha fade');
  const root96 = death.filter(vm => vm.script === 96);
  assert.equal(root96.length, 1, 'BossDead creates effect.anm script96 on the death frame');
  assert.equal(root96[0].layer, 16);
  assert.equal(root96[0].parent, 0);
  const childrenOf = (frame, root) => effectsAt(frame)
    .filter(vm => vm.script === 99 && vm.parent === root.handle);
  const kids96 = childrenOf(11251, root96[0]);
  assert.ok(kids96.length >= 1, 'script96 must spawn its script99 particles');
  for (const child of kids96) {
    assert.equal(child.layer, 16);
    assert.equal(child.sprite, 31);
    assert.equal(child.tex, 'effect/eff_base.png');
  }
  assert.equal(death.filter(vm => vm.script === 97).length, 0,
    'script97 belongs to the +60 batch, not the death frame');
  assert.equal(childrenOf(11310, root96[0]).length, 30,
    'script96 spawns its script-literal 30 particles');

  const late = effectsAt(11311);
  const secondBurst = late.filter(vm => vm.script === 25);
  assert.equal(secondBurst.length, 1, 'the +60 batch re-creates effect.anm script25');
  assert.equal(secondBurst[0].tex, 'effect/eff_base.png');
  assert.ok(secondBurst[0].alpha < 255);
  const root97 = late.filter(vm => vm.script === 97);
  assert.equal(root97.length, 1, 'BossDead +60 creates effect.anm script97');
  assert.equal(root97[0].layer, 16);
  assert.equal(root97[0].parent, 0);
  const kids97 = childrenOf(11311, root97[0]);
  assert.equal(kids97.length, 60, 'script97 first loop spawns its script-literal 60 particles');
  for (const child of kids97) {
    assert.equal(child.layer, 16);
    assert.equal(child.sprite, 31);
    assert.equal(child.tex, 'effect/eff_base.png');
  }
  assert.equal(childrenOf(11312, root97[0]).length, 120,
    'script97 second loop brings the total to 60+60 particles');
  assert.equal(childrenOf(11320, root96[0]).length, 30,
    'the first burst particles must stay alive independently of the second burst');

  t.diagnostic(JSON.stringify({buildIdentity: identity,
    deathFrames: {midboss: 3232, bossDead: 11251, bossDeadPlus60: 11311},
    ringAlphaFades: fades,
    effects: {
      midboss: {script: 98, children: midbossChildren(3235).length},
      deathFrame: {scripts: [25, 96], children96: childrenOf(11310, root96[0]).length},
      plus60: {scripts: [25, 97], children97: childrenOf(11312, root97[0]).length},
    },
    finalProgress: result.progress,
    consoleMessages: result.consoleMessages}));
});
