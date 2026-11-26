import assert from 'node:assert/strict';
import { createRequire } from 'node:module';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import { join } from 'node:path';

const [url, data, output, executable, playwrightModule = 'playwright', route, nativeSamplesPath] = process.argv.slice(2);
const clearStageOne = route === '--clear-stage-one';
const quitAfterSave = route === '--save-replay-and-quit';
const saveReplay = route === '--save-replay' || quitAfterSave;
const checkPixelScaling = route === '--check-pixel-scaling';
const checkCampaign = route === '--check-campaign';
const checkRestartMemory = route === '--check-restart-memory';
const checkPauseInput = route === '--check-pause-input';
const checkTouchMenu = route === '--check-touch-menu';
const checkRankBack = route === '--check-rank-back';
const checkTouchMotion = route === '--check-touch-motion';
const touchTest = checkTouchMenu || checkRankBack || checkTouchMotion;
const checkCoopEngine = route === '--check-coop-engine';
const checkCoopRewind = checkCoopEngine && process.env.THIBP_TEST_COOP_REWIND === '1';
const checkCoopSkill = checkCoopEngine && process.env.THIBP_TEST_COOP_SKILL === '1';
const checkCoopGhost = checkCoopEngine && process.env.THIBP_TEST_COOP_GHOST === '1';
const checkCoopStockHud = checkCoopEngine && process.env.THIBP_TEST_COOP_STOCK_HUD === '1';
const coopSeats = Number(process.env.THIBP_TEST_COOP_SEATS || 3);
const replayPath = clearStageOne || saveReplay || checkPixelScaling || checkCampaign || checkRestartMemory ||
  checkPauseInput || touchTest || checkCoopEngine ? undefined : route;
if (!executable || (nativeSamplesPath && !replayPath)) {
  throw new Error('Usage: check-browser.mjs URL ZIP OUTPUT BROWSER [PLAYWRIGHT] [STAGE6_REPLAY [NATIVE_SAMPLES]|--clear-stage-one|--save-replay|--save-replay-and-quit|--check-pixel-scaling|--check-campaign|--check-restart-memory|--check-pause-input|--check-touch-menu|--check-rank-back|--check-touch-motion|--check-coop-engine]');
}
if (checkCoopEngine && ![2, 3].includes(coopSeats)) throw new Error('Use two or three cooperative seats');
const nativeSamples = nativeSamplesPath && JSON.parse(await readFile(nativeSamplesPath, 'utf8'));

const { chromium } = createRequire(import.meta.url)(playwrightModule);
const browser = await chromium.launch({ executablePath: executable, headless: true,
  ...(touchTest ? { args: ['--use-angle=swiftshader', '--enable-unsafe-swiftshader'] } : {}),
});
const report = { browser: browser.version(), messages: [], failedRequests: [], runtimeRequests: [] };
const outputPath = join(output, 'browser-launcher.json');
const refreshHz = Number(process.env.THIBP_TEST_REFRESH_HZ || 0);
const lockDisplayTo60 = process.env.THIBP_TEST_LOCK_DISPLAY === '1';
const candidateRuntime = process.env.THIBP_TEST_RUNTIME_DIR;
if (refreshHz && ![60, 120, 144].includes(refreshHz)) throw new Error('Use 60, 120 or 144 for THIBP_TEST_REFRESH_HZ');
const replayFile = '/savethibp/replay/Th_IBP_replay01.dat';
const extraReplayFile = '/savethibp/replay/Th_IBP_replay02.dat';
const scoreFile = '/savethibp/Th_IBP_common_Save.dat';
const hudFpsProbeFile = '/savethibp/Th_IBP_common_WebHudFpsProbe.dat';
const savedPaths = [replayFile, scoreFile];
await mkdir(output, { recursive: true });

const screenshot = (frame, filename) => frame.locator('#canvas').screenshot({ path: join(output, filename) });
const readSavedFiles = (frame, paths) => frame.evaluate(paths => Object.fromEntries(paths.map(path => [path,
  Array.from(ph3Runtime.FS.readFile(path)),
])), paths);

function recordField(bytes, header, key) {
  const buffer = Buffer.from(bytes);
  assert.equal(buffer.toString('ascii', 0, header.length), header);
  const entries = buffer.readUInt32LE(header.length);
  let offset = header.length + 4;
  for (let entry = 0; entry < entries; entry++) {
    const keySize = buffer.readUInt32LE(offset + 1);
    const keyStart = offset + 5;
    const keyEnd = keyStart + keySize;
    const size = buffer.readUInt32LE(keyEnd);
    const start = keyEnd + 4;
    offset = start + size;
    assert(offset <= buffer.length, 'Truncated record');
    if (buffer.toString('ascii', keyStart, keyEnd) === key) return buffer.subarray(start, offset);
  }
  throw new Error(`Missing ${key} in ${header} record`);
}

function replayStages(bytes) {
  const list = recordField(bytes, 'REPLAY', 'stageIndexList');
  assert.equal(list.length % 4, 0);
  return Array.from({ length: list.length / 4 }, (_, index) => list.readInt32LE(index * 4));
}

async function checkPresentation(page, frame, width, height, label) {
  const viewport = page.viewportSize();
  const available = await frame.evaluate(() => ({ width: innerWidth, height: innerHeight }));
  await page.setViewportSize({
    width: width + Math.max(0, viewport.width - available.width),
    height: height + Math.max(0, viewport.height - available.height),
  });
  await frame.waitForFunction(({ width, height }) => {
    const canvas = document.querySelector('#canvas');
    return Math.abs(canvas.width / devicePixelRatio - width) <= 1 &&
      Math.abs(canvas.height / devicePixelRatio - height) <= 1;
  }, { width, height }, { timeout: 6000 });
  const result = await frame.evaluate(() => {
    const canvas = document.querySelector('#canvas');
    const gl = canvas.getContext('webgl2');
    const blit = gl.blitFramebuffer;
    return new Promise((resolve, reject) => {
      const timeout = setTimeout(() => {
        delete gl.blitFramebuffer;
        reject(new Error('The runtime did not present a frame'));
      }, 5000);
      gl.blitFramebuffer = function (...args) {
        blit.apply(gl, args);
        const [sx, sy, sw, sh, dx, dy, dw, dh, , filter] = args;
        if (sx || sy || dx || dy || dw !== gl.drawingBufferWidth || dh !== gl.drawingBufferHeight ||
            gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING) !== null) return;
        const source = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
        if (source === null) return;
        clearTimeout(timeout);
        delete gl.blitFramebuffer;
        const original = new Uint8Array(sw * sh * 4);
        const presented = new Uint8Array(dw * dh * 4);
        gl.readPixels(0, 0, sw, sh, gl.RGBA, gl.UNSIGNED_BYTE, original);
        gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);
        try {
          gl.readPixels(0, 0, dw, dh, gl.RGBA, gl.UNSIGNED_BYTE, presented);
        } finally {
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER, source);
        }
        let samples = 0;
        let different = 0;
        for (let y = 0; y < dh; y++) {
          const sourceY = (y + 0.5) * sh / dh;
          if (Number.isInteger(sourceY)) continue; // Nearest samples can tie at a texel boundary.
          for (let x = 0; x < dw; x++) {
            const sourceX = (x + 0.5) * sw / dw;
            if (Number.isInteger(sourceX)) continue;
            const from = (Math.floor(sourceY) * sw + Math.floor(sourceX)) * 4;
            const to = (y * dw + x) * 4;
            samples++;
            if (original[from] !== presented[to] || original[from + 1] !== presented[to + 1] ||
                original[from + 2] !== presented[to + 2]) different++;
          }
        }
        const { width, height } = canvas.getBoundingClientRect();
        resolve({ source: [sw, sh], presented: [dw, dh], cssSize: [width, height],
          viewport: [innerWidth, innerHeight], devicePixelRatio, samples, different,
          filter: filter === gl.NEAREST ? 'nearest' : 'linear',
          imageRendering: getComputedStyle(canvas).imageRendering });
      };
    });
  });
  (report.presentation ??= []).push({ label, ...result });
  await frame.locator('#canvas').screenshot({ path: join(output, `${label}.png`) });
  assert.deepEqual(result.source, [640, 480]);
  if (width < 640) {
    assert.equal(result.filter, 'linear', 'Downscaling should keep the existing filter');
  } else {
    assert.equal(result.filter, 'nearest', 'Enlargement must use nearest-neighbor presentation');
    assert(result.samples >= 100000 && result.different === 0, 'Presentation must preserve original RGB pixels');
    assert.match(result.imageRendering, /^(pixelated|crisp-edges)$/);
  }
}

async function playStageOne(page, frame, tap) {
  const waitForStage = predicate => frame.waitForFunction(predicate, null, { timeout: 6000 });
  const stage = () => frame.evaluate(() => ph3Diagnostics.stage);
  await tap('Escape');
  await waitForStage(() => ph3Diagnostics.stage?.paused);
  report.pausedStage = await stage();
  await frame.locator('#canvas').screenshot({ path: join(output, 'pause.png') });
  await tap('z');
  await waitForStage(() => ph3Diagnostics.stage && !ph3Diagnostics.stage.paused);
  report.resumedStage = await stage();

  const deadline = Date.now() + 360000;
  let direction = 'ArrowRight';
  let nextConfirm = report.resumedStage.frame + 180;
  let nextBomb = 1820;
  let nextSample = 600;
  report.stageOne = [];
  await page.keyboard.down('Shift');
  await page.keyboard.down('z');
  await page.keyboard.down(direction);
  try {
    while (Date.now() < deadline) {
      const current = await stage();
      if (!current) {
        await page.waitForTimeout(85);
        continue;
      }
      if (current?.index === 2) return current;
      assert(current.index === 1 && current.playerState !== 3, 'Story run ended before Stage 2');
      if (current.frame >= nextSample) {
        report.stageOne.push(current);
        nextSample += 600;
      }
      const boundary = current.frame >= 7000 ? 65 : 104;
      if ((direction === 'ArrowRight' && current.x >= 192 + boundary) ||
          (direction === 'ArrowLeft' && current.x <= 192 - boundary)) {
        await page.keyboard.up(direction);
        direction = direction === 'ArrowRight' ? 'ArrowLeft' : 'ArrowRight';
        await page.keyboard.down(direction);
      }
      if (current.frame >= nextBomb && current.spells > 0 && current.enemyShots >= 65) {
        await page.keyboard.press('x', { delay: 65 });
        nextBomb = current.frame + 850;
      }
      if (current.frame >= nextConfirm) {
        await page.keyboard.up('z');
        await page.waitForTimeout(36);
        await page.keyboard.down('z');
        nextConfirm = current.frame + 118;
      }
      await page.waitForTimeout(85);
    }
    throw new Error('The live Easy route did not reach Stage 2 within six minutes');
  } finally {
    await page.keyboard.up(direction);
    await page.keyboard.up('z');
    await page.keyboard.up('Shift');
    report.lastStage = await stage();
    await frame.locator('#canvas').screenshot({ path: join(output, 'last-stage.png') });
  }
}

async function measureStageRestarts(frame, tap) {
  report.restartMemory = [];
  for (let attempt = 0; attempt <= 4; attempt++) {
    await frame.waitForFunction(() => ph3Diagnostics.stage?.frame >= 350 ||
      ph3Diagnostics.phase === 'error', null, { timeout: 15000 });
    const sample = await frame.evaluate(() => ({
      stage: ph3Diagnostics.stage,
      errors: ph3Diagnostics.errors,
      wasmBytes: ph3Runtime.HEAPU8.byteLength,
      jsHeapBytes: performance.memory?.usedJSHeapSize,
      textureCache: ph3Diagnostics.textureCache,
      scriptCache: ph3Diagnostics.stageScriptCache,
    }));
    report.restartMemory.push(sample);
    assert.deepEqual(sample.errors, []);
    assert.equal(sample.stage.index, 1);
    if (attempt === 4) break;
    await tap('Escape');
    await frame.waitForFunction(() => ph3Diagnostics.stage?.paused);
    await tap('ArrowUp'); // Retry is the last pause option.
    await tap('z');
    await frame.waitForFunction(() => ph3Diagnostics.stage?.playerPresent && !ph3Diagnostics.stage.paused &&
      ph3Diagnostics.stage.frame < 350, null, { timeout: 15000 });
  }
  assert.equal(report.restartMemory.at(-1).wasmBytes, report.restartMemory.at(-2).wasmBytes,
    'Wasm memory capacity should reach a plateau across identical stage restarts');
  console.log(`Stage 1 entered five times; final Wasm capacity ${(report.restartMemory.at(-1).wasmBytes / 2 ** 20).toFixed(1)} MiB`);
}

async function verifyPauseInput(page, frame, tap) {
  const key = ({ code, key, keyCode, type }) => {
    const target = document.querySelector('#canvas') ?? document.body;
    const init = { code, key, keyCode, bubbles: true, cancelable: true };
    for (const eventType of type ? [type] : ['keydown', 'keyup']) {
      target.dispatchEvent(new KeyboardEvent(eventType, init));
    }
  };
  const esc = { code: 'Escape', key: 'Escape', keyCode: 27 };
  const waitPaused = () => frame.waitForFunction(() => ph3Diagnostics.stage?.paused, null, { timeout: 2500 });
  const getStage = () => frame.evaluate(() => ph3Diagnostics.stage);
  const advance = (previous, frames) => frame.waitForFunction(target => ph3Diagnostics.stage?.frame >= target &&
    !ph3Diagnostics.stage.paused, previous.frame + frames, { timeout: 9000 });
  const waitForKeyPoll = async () => {
    const frames = await frame.evaluate(() => ph3Diagnostics.frames);
    await frame.waitForFunction(previous => ph3Diagnostics.frames >= previous + 3,
      frames, { timeout: 2500 });
  };
  const canvas = frame.locator('#canvas');
  report.pauseInput = {};

  // The launcher's key down/up messages can reach the game before one simulation tick.
  await page.evaluate(key, esc);
  await waitPaused();
  report.pauseInput.hostTap = await getStage();
  await screenshot(frame, 'pause-from-launcher.png');
  await canvas.focus();
  await tap('z');
  await frame.waitForFunction(previous => ph3Diagnostics.stage?.frame > previous && !ph3Diagnostics.stage.paused,
    report.pauseInput.hostTap.frame, { timeout: 4000 });

  await frame.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent('keydown', { code: 'Escape', key: 'Escape', bubbles: true, repeat: true }));
  });
  await frame.waitForFunction(previous => ph3Diagnostics.stage?.frame >= previous + 15 && !ph3Diagnostics.stage.paused,
    (await getStage()).frame, { timeout: 4000 });

  await frame.evaluate(key, esc);
  await waitPaused();
  report.pauseInput.canvasTap = await getStage();
  await tap('ArrowDown'); // Select Return to title.
  await page.evaluate(key, { code: 'KeyX', key: 'x', keyCode: 88 }); // Cancel back to Resume.
  await waitForKeyPoll();
  await tap('z');
  await frame.waitForFunction(previous => ph3Diagnostics.stage?.frame > previous && !ph3Diagnostics.stage.paused,
    report.pauseInput.canvasTap.frame, { timeout: 4000 });
  await canvas.click();
  await tap('Escape');
  await waitPaused();
  report.pauseInput.physicalTap = await getStage();
  const titleTrack = '/sound/bgm/bgm01.ogg';
  const titleStarts = await frame.evaluate(path =>
    ph3Diagnostics.messages.filter(message => message.includes(path)).length, titleTrack);
  await tap('ArrowDown'); // Return to title.
  await tap('z');
  await frame.waitForFunction(({ titleTrack, titleStarts }) =>
    ph3Diagnostics.messages.filter(message => message.includes(titleTrack)).length > titleStarts ||
    ph3Diagnostics.phase === 'error', { titleTrack, titleStarts }, { timeout: 6000 });
  assert.deepEqual(await frame.evaluate(() => ph3Diagnostics.errors), []);
  await screenshot(frame, 'pause-return-to-title.png');
  console.log('Short Esc taps paused from both the launcher and canvas, then resumed and returned to title');

  await tap('z'); // Story
  await tap('z'); // Confirm difficulty
  await tap('z'); // Reimu A
  await frame.waitForFunction(previous => {
    const { stage } = ph3Diagnostics;
    return stage?.playerPresent && !stage.paused && stage.frame >= 90 && stage.frame < previous;
  }, report.pauseInput.physicalTap.frame, { timeout: 12000 });
  const initial = await getStage();
  await page.keyboard.press('ArrowRight', { delay: 150 });
  await advance(initial, 16);
  const fast = await getStage();
  assert(fast.x > initial.x, 'Right should move Reimu');
  await page.keyboard.down('ShiftLeft');
  try {
    await page.keyboard.press('ArrowLeft', { delay: 160 });
  } finally {
    await page.keyboard.up('ShiftLeft');
  }
  await advance(fast, 16);
  const slow = await getStage();
  assert(slow.x < fast.x && fast.x - slow.x < fast.x - initial.x, 'Left Shift should reduce the movement speed');
  await page.keyboard.down('z');
  try {
    await frame.waitForFunction(() => ph3Diagnostics.stage?.playerShots > 0, null, { timeout: 6000 });
  } finally {
    await page.keyboard.up('z');
  }
  const specialFile = '/savethibp/Th_IBP_common_KzControlProbe.dat';
  await page.evaluate(key, { code: 'KeyC', key: 'c', keyCode: 67 });
  await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists, specialFile, { timeout: 3500 });
  const special = await getStage();
  await advance(special, 245);
  const beforeBomb = await getStage();
  await frame.evaluate(key, { code: 'KeyX', key: 'x', keyCode: 88 });
  await frame.waitForFunction(spells => ph3Diagnostics.stage?.spells === spells - 1,
    beforeBomb.spells, { timeout: 3500 });
  report.gameplayInput = { fastDistance: fast.x - initial.x, slowDistance: fast.x - slow.x,
    specialFrame: special.frame, spellsAfterBomb: (await getStage()).spells };
  const clock = () => frame.evaluate(() => ({ frame: ph3Diagnostics.stage.frame, ms: performance.now() }));
  async function retryWithCtrl(setCtrl) {
    await tap('Escape');
    await waitPaused();
    const paused = await getStage();
    await tap('ArrowUp'); // Retry.
    await setCtrl(true);
    try {
      await tap('z');
      await frame.waitForFunction(previous => {
        const { stage } = ph3Diagnostics;
        return stage?.playerPresent && !stage.paused && stage.frame < previous;
      }, paused.frame, { timeout: 10000 });
      const restarted = await clock();
      await frame.waitForFunction(target => ph3Diagnostics.stage?.frame >= target,
        restarted.frame + 240, { timeout: 10000 });
      const progressed = await clock();
      return 1000 * (progressed.frame - restarted.frame) / (progressed.ms - restarted.ms);
    } finally {
      await setCtrl(false);
    }
  }
  report.gameplayInput.heldCtrlHz = await retryWithCtrl(down =>
    down ? page.keyboard.down('ControlLeft') : page.keyboard.up('ControlLeft'));
  assert(report.gameplayInput.heldCtrlHz > 75, 'Left Ctrl held on the canvas across Retry should still fast-forward');
  await frame.waitForFunction(() => ph3Diagnostics.stage?.frame >= 900);
  report.gameplayInput.hostHeldCtrlHz = await retryWithCtrl(down => page.evaluate(key, {
    code: 'ControlLeft', key: 'Control', keyCode: 17, type: down ? 'keydown' : 'keyup',
  }));
  assert(report.gameplayInput.hostHeldCtrlHz > 75, 'Left Ctrl held through the launcher across Retry should still fast-forward');
  const released = await clock();
  await frame.waitForFunction(target => ph3Diagnostics.stage?.frame >= target,
    released.frame + 180, { timeout: 9000 });
  const normal = await clock();
  report.gameplayInput.releasedCtrlHz = 1000 * (normal.frame - released.frame) / (normal.ms - released.ms);
  assert(report.gameplayInput.releasedCtrlHz < 75, 'Releasing the forwarded Left Ctrl should restore normal speed');
  console.log('Original movement, focus, shooting, special attack and bomb controls passed');
}

async function setRefreshPreference(page) {
  await page.locator('#gameSettingsOpen').click();
  const display = page.locator('#displayOptions');
  await display.waitFor({ state: 'visible' });
  if (await display.getAttribute('data-state') === 'closed') {
    await display.locator('button.options-group-head').click();
  }
  const toggle = page.locator('#frameLimitToggle');
  const available = await toggle.isVisible();
  if (available && (await toggle.getAttribute('aria-checked')) !== String(!lockDisplayTo60)) {
    await toggle.click();
  }
  const followsDisplay = available && (await toggle.getAttribute('aria-checked')) === 'true';
  await page.locator('#libraryBack').click();
  return { available, followsDisplay };
}

async function checkRefreshCadence(frame, scene) {
  const read = () => ({
    callbacks: globalThis.thibpTestRefreshCallbacks,
    logic: ph3Diagnostics.frames,
    presented: ph3Diagnostics.presentedFrames,
  });
  const start = await frame.evaluate(read);
  await frame.waitForFunction(callbacks => globalThis.thibpTestRefreshCallbacks >= callbacks,
    start.callbacks + refreshHz * 2, { timeout: 15000, polling: 25 });
  const end = await frame.evaluate(read);
  const seconds = (end.callbacks - start.callbacks) / refreshHz;
  const logic = end.logic - start.logic, presented = end.presented - start.presented;
  const displayHz = lockDisplayTo60 ? 60 : refreshHz;
  report.refresh[scene] = {
    callbacks: end.callbacks - start.callbacks,
    logicHz: logic / seconds,
    displayHz: presented / seconds,
  };
  assert(Math.abs(logic - 60 * seconds) <= 2, scene + ' must advance logic at 60 Hz');
  assert(Math.abs(presented - displayHz * seconds) <= 2,
    scene + ' must present at ' + displayHz + ' Hz, measured ' + (presented / seconds).toFixed(1));
}

async function checkInterpolatedMotion(page, frame) {
  await page.keyboard.down('ArrowRight');
  try {
    const samples = await frame.evaluate(() => new Promise(resolve => {
      const canvas = document.querySelector('#canvas');
      const gl = canvas.getContext('webgl2');
      const left = Math.floor(canvas.width * 180 / 640);
      const bottom = Math.floor(canvas.height * 25 / 480);
      const width = Math.floor(canvas.width * 150 / 640);
      const height = Math.floor(canvas.height * 56 / 480);
      const pixels = new Uint8Array(width * height * 4);
      const samples = [];
      let callbacks = 0;
      function sample() {
        const tick = ph3Diagnostics.frames, presented = ph3Diagnostics.presentedFrames;
        if (samples.at(-1)?.presented !== presented) {
          const framebuffer = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);
          gl.readPixels(left, bottom, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER, framebuffer);
          let hash = 2166136261;
          for (const byte of pixels) hash = Math.imul(hash ^ byte, 16777619);
          samples.push({ tick, presented, hash });
        }
        if (++callbacks < 24) requestAnimationFrame(sample);
        else resolve(samples);
      }
      requestAnimationFrame(sample);
    }));
    const different = samples.filter((sample, index) =>
      index > 0 && samples[index - 1].tick === sample.tick && samples[index - 1].hash !== sample.hash);
    report.refresh.sameTickMotionSamples = different.length;
    assert(different.length >= 2, 'Moving gameplay must produce distinct images within a logic tick');
  } finally {
    await page.keyboard.up('ArrowRight');
  }
}

async function openRuntime(page) {
  const player = page.locator('#player.open');
  const dialog = page.locator('#decisionDialog[open]');
  await page.locator('#launch').click();
  for (let attempt = 0; attempt < 6 && !(await player.count()); attempt++) {
    await Promise.race([
      dialog.waitFor({ state: 'visible', timeout: 8000 }).catch(() => {}),
      player.waitFor({ state: 'visible', timeout: 8000 }).catch(() => {}),
    ]);
    if (await dialog.isVisible()) await dialog.locator('#decisionConfirm').click();
  }
  await player.waitFor({ state: 'visible', timeout: 15000 });
  const isGameFrame = frame => new URL(frame.url()).pathname.endsWith('/runtime/thibp/thibp.html');
  const frame = page.frames().find(isGameFrame) ?? await page.waitForEvent('framenavigated', {
    predicate: isGameFrame,
    timeout: 15000,
  });
  report.runtime = frame.url();
  assert.equal(new URL(report.runtime).searchParams.get('managedData'), '1');
  await frame.waitForFunction(() => globalThis.ph3Diagnostics?.frames >= 185 ||
    globalThis.ph3Diagnostics?.phase === 'error', null, { timeout: 35000 });
  assert.equal(await frame.title(), '东方鬼葬剑 ～ Infinite Blade Pavilion',
    'SDL must preserve the game title of the launcher Runtime after startup');
  return frame;
}

async function waitForMenuFrames(frame, count) {
  const before = await frame.evaluate(() => ph3Diagnostics.frames);
  await frame.waitForFunction(target => ph3Diagnostics.frames >= target,
    before + count, { timeout: 8000 });
}

async function verifyRankBack(page, frame, tap) {
  async function enterCharacterAfterBack(back, description) {
    await tap('z'); // Title to difficulty.
    await tap('ArrowDown'); // Move the difficulty cursor.
    await back();
    await waitForMenuFrames(frame, 8);
    await frame.locator('#canvas').focus();
    await tap('z'); // Reopen difficulty.
    await tap('z'); // Select difficulty and reach character selection.
    await waitForMenuFrames(frame, 120);
    assert(!await frame.evaluate(() => ph3Diagnostics.stage?.playerPresent),
      `${description} must leave difficulty before we can reopen it with Z`);
  }
  await enterCharacterAfterBack(() => tap('Escape'), 'The keyboard ESC key');
  await tap('x'); // Character to difficulty.
  await tap('x'); // Difficulty to title.
  await enterCharacterAfterBack(() => page.locator('#touchEscape').tap(), 'The touch ESC button');
  await tap('z'); // Confirm character.
  report.rankBack = { keyboardEsc: true, touchEsc: true, x: true };
}

async function verifyTouchPauseMenu(page, frame) {
  const surface = page.locator('#touchDirectSurface');
  await surface.waitFor({ state: 'visible', timeout: 5000 });
  const client = await page.context().newCDPSession(page);
  async function pauseAndPick(direction) {
    await page.locator('#touchEscape').tap();
    await frame.waitForFunction(() => ph3Diagnostics.stage?.paused, null, { timeout: 5000 });
    await waitForMenuFrames(frame, 10);
    const { x, y, width, height } = await frame.locator('#canvas').boundingBox();
    const start = { x: Math.round(x + width * 0.43), y: Math.round(y + height * 0.56), id: 1 };
    const target = { ...start, y: start.y + direction * Math.round(height * 0.14) };
    const dispatch = (type, touchPoints) => client.send('Input.dispatchTouchEvent', { type, touchPoints });
    await dispatch('touchStart', [start]);
    await dispatch('touchMove', [target]);
    await dispatch('touchEnd', []);
    await waitForMenuFrames(frame, 10);
    await page.touchscreen.tap(start.x, start.y);
  }

  try {
    const starting = await frame.evaluate(() => ph3Diagnostics.stage.frame);
    await pauseAndPick(-1);
    await frame.waitForFunction(previous => {
      const { stage } = ph3Diagnostics;
      return stage?.index === 1 && !stage.paused && stage.playerPresent && stage.frame < previous;
    }, starting, { timeout: 7000 }).catch(error => {
      throw new Error('Swiping up from Continue must select Retry', { cause: error });
    });
    await frame.waitForFunction(() => ph3Diagnostics.stage?.frame > 140, null, { timeout: 7000 });
    const countTitleTracks = () => frame.evaluate(() =>
      ph3Diagnostics.messages.filter(line => line.includes('/sound/bgm/bgm01.ogg')).length);
    const previous = await countTitleTracks();
    await pauseAndPick(1);
    await frame.waitForFunction(previous =>
      ph3Diagnostics.messages.filter(line => line.includes('/sound/bgm/bgm01.ogg')).length > previous,
    previous, { timeout: 7000 }).catch(error => {
      throw new Error('Swiping down from Continue must select Return to Title', { cause: error });
    });
    report.touchPauseMenu = { swipeUp: 'retry', swipeDown: 'title' };
  } finally {
    await client.detach();
  }
}

async function verifyTouchMotion(page, frame) {
  const canvas = await frame.locator('#canvas').boundingBox();
  const start = { x: Math.round(canvas.x + canvas.width * 0.5),
    y: Math.round(canvas.y + canvas.height * 0.5), id: 1 };
  const client = await page.context().newCDPSession(page);
  const dispatch = (type, touchPoints = []) => client.send('Input.dispatchTouchEvent', {type, touchPoints});
  const stage = () => frame.evaluate(() => ph3Diagnostics.stage);
  await frame.evaluate(() => {
    const game = ph3Runtime;
    const call = game.ccall;
    window.thibpTouchEdges = [];
    game.ccall = function(name, type, argumentTypes, values) {
      const result = call.call(this, name, type, argumentTypes, values);
      if (name === 'hsob_touch_pointer') {
        thibpTouchEdges.push({type: values[0], frame: ph3Diagnostics.frames});
      }
      return result;
    };
  });
  const drags = [
    {name: 'horizontal', dx: 48, dy: 0},
    {name: 'vertical', dx: 0, dy: -48},
    {name: 'diagonal', dx: -36, dy: -36},
    {name: 'vertical-letterbox', dx: 0, dy: -24, origin: {...start, y: Math.round(canvas.y - 30)}},
    {name: 'quick-horizontal', dx: -16, dy: 0, quick: true},
    {name: 'quick-vertical', dx: 0, dy: -16, quick: true},
  ];
  report.touchMotion = { canvas, sensitivity: 150, drags: [] };
  try {
    for (const {name, dx, dy, quick, origin = start} of drags) {
      assert.equal(await page.evaluate(({x, y}) => document.elementFromPoint(x, y)?.id, origin),
        'touchDirectSurface', `Touch gesture must start on the game surface: ${name}`);
      const before = await stage();
      const expectedX = dx * 640 / canvas.width * 1.5;
      const expectedY = dy * 480 / canvas.height * 1.5;
      await frame.evaluate(quick => {
        thibpTouchEdges.length = 0;
        window.thibpHoldFrames = !!quick;
      }, quick ?? false);
      try {
        await dispatch('touchStart', [origin]);
        await dispatch('touchMove', [{ ...origin, x: origin.x + dx, y: origin.y + dy }]);
        if (!quick) await waitForMenuFrames(frame, 8);
        await dispatch('touchEnd');
        await frame.waitForFunction(() => thibpTouchEdges.at(-1)?.type === 2, null, {polling: 15, timeout: 3000});
      } finally {
        await frame.evaluate(() => { window.thibpHoldFrames = false; });
      }
      const edges = await frame.evaluate(() => thibpTouchEdges);
      assert.deepEqual(edges.map(edge => edge.type), [0, 1, 2]);
      if (quick) assert.equal(new Set(edges.map(edge => edge.frame)).size, 1,
        'A quick gesture must finish between logic ticks');
      await waitForMenuFrames(frame, 8);
      const after = await stage();
      const measuredX = after.x - before.x, measuredY = after.y - before.y;
      report.touchMotion.drags.push({name, dx, dy, expectedX, expectedY, measuredX, measuredY, edges});
    }
    const missed = report.touchMotion.drags.filter(({expectedX, expectedY, measuredX, measuredY}) =>
      Math.abs(measuredX - expectedX) >= 0.25 || Math.abs(measuredY - expectedY) >= 0.25);
    assert.deepEqual(missed.map(({name, measuredX, measuredY}) => ({name, measuredX, measuredY})), []);
  } finally {
    await client.detach();
  }
}

async function addScriptProbe(frame, script, anchor, probe) {
  await frame.evaluate(({ script, pattern, probe }) => {
    const path = `/game/script/Th_IBP/stage/${script}`;
    const fs = ph3Runtime.FS;
    const bytes = fs.readFile(path);
    if (bytes[0] !== 0xff || bytes[1] !== 0xfe) throw new Error(`Expected UTF-16LE in ${path}`);
    const source = new TextDecoder('utf-16le', { ignoreBOM: true }).decode(bytes);
    const anchor = new RegExp(pattern);
    if (!anchor.test(source)) throw new Error(`Missing probe position in ${path}`);
    const updated = source.replace(anchor, match => `${match}\n${probe}\n`);
    const encoded = new Uint8Array(updated.length * 2);
    const view = new DataView(encoded.buffer);
    for (let index = 0; index < updated.length; index++) view.setUint16(index * 2, updated.charCodeAt(index), true);
    // This browser's temporary filesystem owns the test edit.
    fs.writeFile(path, encoded);
  }, { script, pattern: anchor.source, probe });
}

const addStageFrameProbe = (frame, probe) =>
  addScriptProbe(frame, 'Stage_Main.dnh', /@MainLoop\s*\{\s*p_count\+\+;/, probe);

async function prepareHudFpsProbe(frame) {
  await addScriptProbe(frame, 'Stage_UI_Main.dnh', /rtos\(\s*"00\.0",\s*GetCurrentFps\s*\)\s*~\s*"FPS";\s*ObjText_SetText\(\s*obj,\s*str\s*\);/, `
    if(p_count == 120){
      CreateCommonDataArea("WebHudFpsProbe");
      SetAreaCommonData("WebHudFpsProbe", "label", str);
      SaveCommonDataAreaA1("WebHudFpsProbe");
    }
  `);
}

async function checkHudFps(frame) {
  await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists, hudFpsProbeFile, { timeout: 4000 });
  const bytes = (await readSavedFiles(frame, [hudFpsProbeFile]))[hudFpsProbeFile];
  const buffer = recordField(bytes, 'RecordBufferFile', 'label');
  assert.equal(buffer.readUInt32LE(0), 3); // ScriptCommonData array type.
  const length = buffer.readUInt32LE(4);
  assert.equal(buffer.length, 8 + 6 * length);
  const label = Array.from({ length }, (_, index) => {
    const offset = 8 + 6 * index;
    assert.equal(buffer.readUInt32LE(offset), 1); // UTF-16 code unit.
    return String.fromCharCode(buffer.readUInt16LE(offset + 4));
  }).join('');
  assert.match(label, /^\d+\.\dFPS$/);
  report.refresh.hudFps = label;
  const actual = report.refresh.stage.displayHz;
  assert(Math.abs(Number.parseFloat(label) - actual) <= 2,
    `The game should show the presentation rate. HUD ${label}, ${actual.toFixed(1)} presented FPS`);
  await screenshot(frame, 'gameplay-fps.png');
}

const enableCampaignInvincibility = frame => addStageFrameProbe(frame,
  'if(p_count == 1){ SetPlayerInvincibilityFrame(60000); }');

const stageProbeFields = {
  frame: 'p_count', stage: 'sc_stage_index', difficulty: 'sc_difficulty', player: 'sc_player_type',
  player_x: 'GetPlayerX', player_y: 'GetPlayerY', life: 'GetPlayerLife', spell: 'GetPlayerSpell',
  enemy_count: 'length(GetAllEnemyID)', enemy_shots: 'GetShotCount(TARGET_ENEMY)',
  player_shots: 'GetShotCount(TARGET_PLAYER)', music_track: 'GetNowPlayBGM',
  mode: 'sc_gamemode', player_state: 'GetPlayerState', graze: 'GetGraze', score: 'GetScore',
};

async function prepareNativeComparison(frame) {
  assert.deepEqual(nativeSamples.map(row => row.frame), Array.from({ length: 70 }, (_, index) => 30 * (index + 1)));
  await addStageFrameProbe(frame, `
    if(p_count == 1){ CreateCommonDataArea("WebStageProbe"); }
    if(p_count % 30 == 0 && p_count <= 2100){
      let row = [${Object.values(stageProbeFields).join(', ')}];
      let rows = GetAreaCommonData("WebStageProbe", "samples", []);
      SetAreaCommonData("WebStageProbe", "samples", rows ~ [row]);
      if(p_count == 2100){ SaveCommonDataAreaA1("WebStageProbe"); }
    }
  `);
}

async function compareNativeSamples(frame) {
  const path = '/savethibp/Th_IBP_common_WebStageProbe.dat';
  await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists, path, { timeout: 6000 });
  const bytes = await frame.evaluate(path => Array.from(ph3Runtime.FS.readFile(path)), path);
  const buffer = recordField(bytes, 'RecordBufferFile', 'samples');
  assert.equal(buffer.readUInt32LE(0), 3); // ScriptCommonData array type.
  assert.equal(buffer.readUInt32LE(4), nativeSamples.length);
  const fields = Object.keys(stageProbeFields);
  const samples = [];
  const mismatches = [];
  let offset = 8;
  for (const reference of nativeSamples) {
    assert.equal(buffer.readUInt32LE(offset), 3);
    assert.equal(buffer.readUInt32LE(offset + 4), fields.length);
    offset += 8;
    const row = {};
    for (const key of fields) {
      assert.equal(buffer.readUInt32LE(offset), 0); // ScriptCommonData real type.
      row[key] = buffer.readDoubleLE(offset + 4);
      offset += 12;
      if (!Number.isFinite(row[key]) || !Number.isFinite(reference[key]) || Math.abs(row[key] - reference[key]) > 1e-7) {
        mismatches.push({ frame: reference.frame, field: key, native: reference[key], web: row[key] });
      }
    }
    samples.push(row);
  }
  assert.equal(offset, buffer.length);
  report.nativeComparison = { samples, mismatches };
  assert.equal(mismatches.length, 0, `First Stage 6 checkpoint difference: ${JSON.stringify(mismatches[0])}`);
  console.log(`The browser matched ${samples.length} original Stage 6 checkpoints`);
}

async function playCampaign(page, frame) {
  const stages = new Map();
  const start = Date.now();
  report.campaign = { invincibility: true, stages: [], ending: false, credits: false, result: false };
  await page.keyboard.down('ControlLeft');
  try {
    while (Date.now() - start < 480000) {
      const { stage, errors, ending, credits } = await frame.evaluate(() => ({
        stage: ph3Diagnostics.stage, errors: ph3Diagnostics.errors,
        ending: ph3Diagnostics.messages.some(message => message.includes('/sound/bgm/bgm14.ogg')),
        credits: ph3Diagnostics.messages.some(message => message.includes('/sound/bgm/bgm15.ogg')),
      }));
      assert.deepEqual(errors, []);
      if (stage?.playerPresent) {
        assert.notEqual(stage.playerState, 3, 'The campaign ended at Game Over');
        if (!stages.has(stage.index)) console.log(`Campaign entered Stage ${stage.index}`);
        stages.set(stage.index, stage);
      }
      if (ending && !report.campaign.ending) {
        await screenshot(frame, 'ending.png');
        report.campaign.ending = true;
      }
      if (credits) {
        report.campaign.credits = true;
        assert(report.campaign.ending, 'The credits should follow the story ending');
        assert.deepEqual([...stages.keys()], [1, 2, 3, 4, 5, 6]);
        await page.keyboard.up('z');
        const creditsStart = await frame.evaluate(() => ph3Diagnostics.frames);
        await frame.waitForFunction(target => ph3Diagnostics.frames >= target, creditsStart + 540, { timeout: 8000 });
        await screenshot(frame, 'credits.png');
        await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists ||
          ph3Diagnostics.phase === 'error', scoreFile, { timeout: 25000 });
        assert.deepEqual(await frame.evaluate(() => ph3Diagnostics.errors), []);
        report.campaign.result = true;
        console.log('Story credits ended and the clear score was saved');
        return;
      }
      await page.keyboard.down('z');
      await page.waitForTimeout(145);
      await page.keyboard.up('z');
      await page.waitForTimeout(28);
    }
    throw new Error('The accelerated campaign did not reach the credits');
  } finally {
    await page.keyboard.up('z');
    await page.keyboard.up('ControlLeft');
    report.campaign.stages = [...stages.values()];
    report.campaign.seconds = (Date.now() - start) / 1000;
  }
}

async function prepareCoopEngine(frame) {
  const config = await frame.evaluate(seats =>
    ph3Runtime._thibp_coop_configure?.(seats, 0x5a03110b) ?? 0, coopSeats);
  assert.equal(config, 1, 'Runtime must prepare a cooperative session before opening Stage 1');
}

async function verifyCoopStockHud(frame, play) {
  await play(135);
  const before = await screenshot(frame, 'stock-hud-before.png');
  await play(28);
  const pickedUp = await frame.evaluate(() =>
    ph3Runtime.FS.analyzePath('/savethibp/Th_IBP_common_WebStockHudProbe.dat').exists);
  assert(pickedUp, 'The original item collision and pickup must award each fragment to the right player');
  const after = await screenshot(frame, 'stock-hud-after.png');
  const lanes = coopSeats === 2 ? [[461, 106], [552, 146]] : [[461, 98], [552, 131], [461, 164], [552, 164]];
  // The fixture fills the third stock icon and changes the numerator on each of these lanes.
  const positions = lanes.flatMap(([x, y]) => [[x + 22, y], [x + 45, y + 13]]);
  const changed = await frame.evaluate(async ({before, after, positions}) => {
    const read = async base64 => {
      const bytes = Uint8Array.from(atob(base64), char => char.charCodeAt(0));
      const image = await createImageBitmap(new Blob([bytes], {type: 'image/png'}));
      const canvas = new OffscreenCanvas(640, 480);
      const context = canvas.getContext('2d', {willReadFrequently: true});
      context.drawImage(image, 0, 0, 640, 480);
      image.close();
      return context.getImageData(0, 0, 640, 480).data;
    };
    const [empty, filled] = await Promise.all([read(before), read(after)]);
    return positions.map(([x, y]) => {
      let count = 0;
      for (let row = y - 5; row <= y + 5; row++) {
        for (let column = x - 5; column <= x + 5; column++) {
          const i = (row * 640 + column) * 4;
          if ([0, 1, 2].some(c => Math.abs(filled[i + c] - empty[i + c]) > 40)) count++;
        }
      }
      return count;
    });
  }, {before: before.toString('base64'), after: after.toString('base64'), positions});
  report.cooperativeStockHud = {pickedUp, changed};
  assert(changed.every(pixels => pixels >= 4),
    `Each collected fragment must fill its seat's stock icon and update its counter: ${changed}`);
}

async function verifyCoopEngine(frame) {
  const installed = await frame.evaluate(() => typeof ph3Runtime._thibp_coop_confirm_frame === 'function');
  assert(installed, 'Runtime must accept complete cooperative input frames');
  await frame.waitForFunction(() => ph3Runtime._thibp_coop_frame() === 0 ||
    ph3Diagnostics.phase === 'error', null, { timeout: 15000 });

  const readStage = () => frame.evaluate(() => {
    ph3Runtime._thibp_coop_probe();
    const { coop, stage, errors } = ph3Diagnostics;
    return { ...coop, stage, errors };
  });
  const opened = await readStage();
  assert.deepEqual(opened.errors, []);
  assert.equal(opened.frame, 0, 'Stage must stop at its first unconfirmed frame');
  assert.equal(opened.players.length, coopSeats, 'Stage 1 must contain one real player object per seat');
  assert.equal(new Set(opened.players.map(player => player.id)).size, coopSeats);
  assert.equal(new Set(opened.players.map(player => player.x)).size, coopSeats);

  const rejected = await frame.evaluate(() => [
    ph3Runtime._thibp_coop_confirm_frame(1, 0, 0, 0),
    ph3Runtime._thibp_coop_confirm_frame(0, 0x2000, 0, 0),
  ]);
  assert.deepEqual(rejected, [0, 0], 'Reject a gap or an unknown input bit without advancing');
  if (coopSeats === 2) {
    assert.equal(await frame.evaluate(() => ph3Runtime._thibp_coop_confirm_frame(0, 0, 0, 1)), 0,
      'Reject input from a third seat in a two-player run');
  }

  let next = 0;
  const play = async (count, held = [0, 0, 0]) => {
    for (let remaining = count; remaining > 0;) {
      const size = Math.min(remaining, 24);
      const sent = await frame.evaluate(({ first, size, held }) => {
        for (let tick = first; tick < first + size; tick++) {
          if (ph3Runtime._thibp_coop_confirm_frame(tick, ...held) !== 1) return tick - first;
        }
        return size;
      }, { first: next, size, held });
      assert.equal(sent, size, 'Accept consecutive frames in a bounded batch');
      next += size;
      remaining -= size;
      await frame.waitForFunction(target => ph3Runtime._thibp_coop_frame() >= target ||
        ph3Diagnostics.phase === 'error', next, { timeout: 10000 });
    }
    const snapshot = await readStage();
    assert.deepEqual(snapshot.errors, []);
    assert.equal(snapshot.tick, next);
    return snapshot;
  };
  if (checkCoopRewind) {
    const base = await play(178);
    const checkpoint = await frame.evaluate(() => {
      const heap = ph3Runtime.HEAPU8;
      const copy = new Uint8Array(heap.length);
      const measures = [];
      for (let sample = 0; sample < 28; sample++) {
        const start = performance.now();
        copy.set(heap);
        if (sample >= 4) measures.push(performance.now() - start);
      }
      globalThis.thibpTestCheckpoint = copy;
      measures.sort((a, b) => a - b);
      return {bytes: copy.byteLength, p95Ms: measures[Math.ceil(measures.length * 0.95) - 1]};
    });
    const restore = async () => {
      await frame.evaluate(() => {
        const heap = ph3Runtime.HEAPU8;
        const copy = globalThis.thibpTestCheckpoint;
        if (heap.length !== copy.length) throw new Error('Native memory grew inside the rewind window');
        heap.set(copy);
      });
      next = base.tick;
      const restored = await readStage();
      assert.equal(restored.frame, base.frame);
      assert.equal(restored.tick, base.tick);
    };
    const exact = [0, (1 << 1) | (1 << 6), 0];
    const predicted = [0, (1 << 0) | (1 << 6), 0];
    const preview = await frame.evaluate(({predicted, exact}) => {
      const helper = ph3Runtime.thibpPrediction;
      const tick = ph3Runtime._thibp_coop_frame();
      if (!helper) throw new Error('The candidate runtime did not install the prediction guard');
      const gl = document.querySelector('#canvas').getContext('webgl2');
      const confirmedTarget = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
      const confirmedPixels = new Uint8Array(640 * 480 * 4);
      gl.readPixels(0, 0, 640, 480, gl.RGBA, gl.UNSIGNED_BYTE, confirmedPixels);
      if (!confirmedTarget || gl.getError() !== gl.NO_ERROR) throw new Error('Missing confirmed render target');
      const latest = new Uint32Array(ph3Runtime.HEAPU8.buffer);
      const before = latest.slice();
      const samples = [];
      let tested = true;
      for (let trial = 0; trial < 28; trial++) {
        const begin = performance.now();
        if (!helper.render(ph3Runtime, tick, [predicted, predicted], begin)) {
          tested = false;
          break;
        }
        if (trial >= 8) samples.push(performance.now() - begin);
      }
      helper.resolve(tick, exact);
      let firstDifference = -1;
      for (let offset = 0; offset < latest.length; offset++) {
        if (latest[offset] !== before[offset]) {
          firstDifference = offset * 4;
          break;
        }
      }
      const previewTarget = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
      gl.bindFramebuffer(gl.READ_FRAMEBUFFER, confirmedTarget);
      const afterPixels = new Uint8Array(confirmedPixels.length);
      gl.readPixels(0, 0, 640, 480, gl.RGBA, gl.UNSIGNED_BYTE, afterPixels);
      gl.bindFramebuffer(gl.READ_FRAMEBUFFER, previewTarget);
      if (gl.getError() !== gl.NO_ERROR) throw new Error('Cannot compare confirmed render target');
      const gpuRestored = afterPixels.every((byte, index) => byte === confirmedPixels[index]);
      samples.sort((a, b) => a - b);
      return {tested, firstDifference, gpuRestored, separateTarget: previewTarget !== confirmedTarget,
        observation: helper.observation, before: tick,
        after: ph3Runtime._thibp_coop_frame(), p95Ms: samples[Math.ceil(samples.length * .95) - 1]};
    }, {predicted, exact});
    report.preview = preview;
    assert.equal(preview.after, preview.before, 'Presentation must leave the confirmed timeline untouched');
    assert.equal(preview.tested, true, JSON.stringify(preview.observation));
    assert.equal(preview.firstDifference, -1, 'The page journal must restore every changed Wasm word');
    assert(preview.separateTarget && preview.gpuRestored, 'Prediction must preserve the confirmed framebuffer');
    assert.equal(preview.observation.corrections, 1);
    const reference = await play(8, exact);
    await restore();
    const speculative = await play(8, predicted);
    assert(speculative.players[1].x < reference.players[1].x, 'The forecast must actually change the native world');
    await restore();
    const corrected = await play(8, exact);
    assert.deepEqual(corrected, reference, 'Re-simulating the corrected lane must match the exact-input result');
    await frame.evaluate(() => { delete globalThis.thibpTestCheckpoint; });
    report.cooperativeRewind = {checkpoint, base, reference, speculative, corrected};
    return;
  }
  if (checkCoopSkill) {
    const before = await play(160);
    const special = 1 << 9;
    const heldByGuest = [0, special, 0];
    await play(1, [special, special, coopSeats === 3 ? special : 0]);
    const started = await play(5);
    assert.equal(started.specialSeat, 0, 'Simultaneous C presses must start one native skill');
    assert.equal(started.specialStarts, 1);
    assert(started.specialProtected);
    await play(1, [0, special, coopSeats === 3 ? special : 0]);
    const during = await play(90, heldByGuest);
    assert.equal(during.specialStarts, 1, 'A new C press during the active skill must be discarded');
    assert(during.specialProtected);
    for (let seat = 1; seat < coopSeats; seat++) {
      assert.equal(during.players[seat].state, 0, 'Each teammate must be immune to a bullet during the native skill');
      assert.equal(during.players[seat].life, before.players[seat].life, 'Each teammate must keep their life');
    }
    const released = await play(210, heldByGuest);
    assert.equal(released.specialSeat, -1, 'The original skill must release the lock on completion');
    assert.equal(released.specialStarts, 1, 'A blocked and held C press must not be queued');
    await play(2);
    await play(1, heldByGuest);
    const accepted = await play(4);
    assert.equal(accepted.specialSeat, 1, 'A fresh C press must work when the skill has finished');
    assert.equal(accepted.specialStarts, 2);
    report.cooperativeSkill = {before, started, during, released, accepted};
    await screenshot(frame, 'cooperative-special.png');
    return;
  }
  if (checkCoopGhost) {
    const ghost = await play(190);
    assert.equal(ghost.players[0].state, 3, 'P1 must become a ghost after its last hit');
    assert.equal(ghost.players[1].state, 0, 'P2 must remain active after P1 runs out of lives');
    assert.equal(ghost.players[1].life, 2);
    const stillPlaying = await play(150);
    assert(stillPlaying.frame > ghost.frame + 140, 'P1 reaching zero lives must not put the team into results');
    const after = await play(30, [1 << 0, 1 << 1, 0]);
    assert(after.frame > ghost.frame + 20, 'The stage must continue after P1 is out');
    assert(after.players[1].x > ghost.players[1].x + 50);
    assert(Math.abs(after.players[0].x - ghost.players[0].x) < 20, 'Ghost input must be ignored');
    await play(36, [0, 1 << 0, 0]);
    for (let tap = 0; tap < 5; tap++) {
      await play(2, [0, 1 << 8, 0]);
      await play(2);
    }
    const revived = await play(10);
    assert.equal(revived.players[0].state, 0, 'Five focus taps must revive a nearby ghost');
    assert.equal(revived.players[0].life, 0);
    assert.equal(revived.players[0].bombs, 2);
    assert.equal(revived.players[1].life, 1);
    report.cooperativeGhost = {ghost, after, revived};
    await screenshot(frame, 'cooperative-revival.png');
    return;
  }
  if (checkCoopStockHud) {
    await verifyCoopStockHud(frame, play);
    return;
  }
  const before = await play(135);
  assert.equal(before.frame, 135);
  await frame.waitForTimeout(120);
  assert.deepEqual(await readStage(), before, 'A missing input frame must leave the same battle state');

  const left = 1 << 0, right = 1 << 1, up = 1 << 2, confirm = 1 << 4;
  const shot = 1 << 6, bomb = 1 << 7, pause = 1 << 11;
  const after = await play(30, [left, right | shot, coopSeats === 3 ? up : 0]);
  assert(after.players[0].x < before.players[0].x - 24, 'P1 should move left');
  assert(after.players[1].x > before.players[1].x + 24, 'P2 should move right');
  assert.equal(after.players[0].y, before.players[0].y, 'P1 must not see P3 vertical input');
  assert.equal(after.players[1].y, before.players[1].y, 'P2 must not see P3 vertical input');
  if (coopSeats === 3) {
    assert(after.players[2].y < before.players[2].y - 24, 'P3 should move up');
    assert.equal(after.players[2].x, before.players[2].x, 'P3 must not see P1 and P2 horizontal input');
  }
  assert(after.stage.playerShots > 0, 'P2 must fire real shots from its own script');

  await play(1, [0, bomb, 0]);
  const afterBomb = await play(15);
  assert.equal(afterBomb.players[1].bombs, after.players[1].bombs - 1);
  for (const seat of [0, 2].filter(seat => seat < coopSeats)) {
    assert.equal(afterBomb.players[seat].bombs, after.players[seat].bombs);
  }

  await play(1, [0, pause, 0]); // P2 opens the original pause menu.
  const paused = await play(16);
  assert.equal(paused.stage.paused, true);
  await play(1, coopSeats === 3 ? [0, 0, confirm] : [confirm, 0, 0]);
  const resumed = await play(24);
  assert.equal(resumed.stage.paused, false);
  assert(resumed.frame > paused.frame);

  const final = await play(240);
  assert(final.frame > 400);
  await play(1, [0, pause, 0]);
  await play(16);
  await play(2, [0, up, 0]); // From Continue, Up wraps to the original Retry option.
  await play(2);
  await play(1, [0, confirm, 0]);
  const restarted = await play(90);
  assert.equal(restarted.stage.index, 1);
  assert(restarted.frame < 90, 'The native Retry menu must open a fresh stage for the whole room');
  assert.equal(restarted.seed, opened.seed);
  assert.deepEqual(restarted.players.map(({x, y, life, bombs}) => ({x, y, life, bombs})),
    opened.players.map(({x, y, life, bombs}) => ({x, y, life, bombs})));
  report.cooperativeEngine = { opened, before, after, afterBomb, paused, resumed, final, restarted };
  await screenshot(frame, 'cooperative-stage.png');
  console.log('Native Stage 1 with independent players, P2 shots and shared pause:', coopSeats, final.tick);
}

async function saveNamedReplay(frame, tap, extra = false) {
  const slotFile = extra ? extraReplayFile : replayFile;
  const files = [slotFile, scoreFile];
  const prefix = extra ? 'extra-' : '';
  await tap('z'); // Save Replay
  if (extra) await tap('ArrowDown'); // Keep the story replay in slot 01.
  await screenshot(frame, prefix + 'replay-slots.png');
  await tap('z'); // Confirm slot
  await tap('z'); // Name A
  await tap('ArrowLeft'); // Last column
  await tap('ArrowUp'); // Last row
  await screenshot(frame, prefix + 'replay-name.png');
  await tap('z'); // Save and return to title
  await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists ||
    ph3Diagnostics.phase === 'error', slotFile, { timeout: 6000 });
  assert.deepEqual(await frame.evaluate(() => ph3Diagnostics.errors), []);
  const saved = await readSavedFiles(frame, files);
  for (const path of files) assert(saved[path].length > 0, `Empty ${path}`);
  await frame.waitForFunction(paths => new Promise(resolve => {
    const request = indexedDB.open('/savethibp');
    request.onerror = () => resolve(false);
    request.onsuccess = () => {
      const database = request.result;
      const store = database.transaction('FILE_DATA', 'readonly').objectStore('FILE_DATA');
      let matched = 0;
      for (const path of paths) {
        const read = store.get(path);
        read.onsuccess = () => { if (read.result?.contents?.length > 0) matched++; };
      }
      store.transaction.oncomplete = () => { database.close(); resolve(matched === paths.length); };
      store.transaction.onabort = () => { database.close(); resolve(false); };
    };
  }), files, { timeout: 10000, polling: 200 });
  return saved;
}

async function reopenSavedGame(page, saved, quitFromTitle) {
  const player = page.locator('#player.open');
  if (quitFromTitle) {
    await quitFromTitle();
  } else {
    await player.locator('.game-settings-toggle').click();
    await page.locator('.game-settings-exit').click();
  }
  await player.waitFor({ state: 'hidden', timeout: 10000 });
  await page.goto(url);
  await page.waitForFunction(() => window.__eaglerBoot?.done === true);
  report.relaunchHost = await page.evaluate(() => ({ isolated: crossOriginIsolated,
    worker: navigator.serviceWorker.controller?.scriptURL ?? null }));
  const reopened = await openRuntime(page);
  assert.deepEqual(await readSavedFiles(reopened, Object.keys(saved)), saved,
    'Game files changed after restarting the launcher');
  return reopened;
}

async function playSavedReplay(frame, tap) {
  await frame.locator('#canvas').focus();
  for (let command = 0; command < 6; command++) await tap('ArrowDown');
  await tap('z'); // Replay menu
  await tap('z'); // The saved slot
  await frame.waitForFunction(() => {
    const { stage, phase } = ph3Diagnostics;
    return (stage?.index === 1 && stage.replay && stage.frame >= 130) || phase === 'error';
  }, null, { timeout: 15000 });
  const restored = await frame.evaluate(() => ph3Diagnostics);
  assert.deepEqual(restored.errors, []);
  assert.equal(restored.stage?.index, 1);
  assert.equal(restored.stage?.replay, true);
  report.savedReplayPlayback = restored.stage;
  await screenshot(frame, 'replay-playback.png');
  console.log(`Saved replay and score survived a launcher restart. Replayed Stage 1 through frame ${restored.stage.frame}`);
}

async function verifySavedReplay(page, frame, tap) {
  const progress = () => frame.evaluate(() => ({ frame: ph3Diagnostics.stage.frame, ms: performance.now() }));
  const start = await progress();
  await page.keyboard.down('ControlLeft');
  try {
    await frame.waitForFunction(target => ph3Diagnostics.stage?.frame >= target ||
      ph3Diagnostics.phase === 'error', start.frame + 360, { timeout: 6000 });
    const accelerated = await progress();
    report.fastForward = {
      frames: accelerated.frame - start.frame,
      seconds: (accelerated.ms - start.ms) / 1000,
    };
    assert(report.fastForward.frames / report.fastForward.seconds > 75, 'Left Ctrl should advance the game faster than 60 Hz');
    await frame.waitForFunction(() => ph3Diagnostics.stage?.playerState === 3 ||
      ph3Diagnostics.messages.some(line => line.includes('/sound/bgm/bgm16.ogg')) ||
      ph3Diagnostics.phase === 'error', null, { timeout: 90000 });
  } finally {
    await page.keyboard.up('ControlLeft');
  }
  await frame.waitForFunction(() => ph3Diagnostics.messages.some(line => line.includes('/sound/bgm/bgm16.ogg')) ||
    ph3Diagnostics.phase === 'error', null, { timeout: 15000 });
  assert.deepEqual(await frame.evaluate(() => ph3Diagnostics.errors), []);
  await screenshot(frame, 'game-over.png');
  const saved = await saveNamedReplay(frame, tap);
  report.savedFiles = Object.fromEntries(savedPaths.map(path => [path, saved[path].length]));
  assert.deepEqual(replayStages(saved[replayFile]), [1]);
  const quit = quitAfterSave ? async () => {
    const isTitleMusic = message => message.includes('/sound/bgm/bgm01.ogg');
    await frame.waitForFunction(previous => ph3Diagnostics.messages.filter(message =>
      message.includes('/sound/bgm/bgm01.ogg')).length > previous, report.title.messages.filter(isTitleMusic).length,
      { timeout: 10000 });
    await frame.locator('#canvas').focus();
    await tap('x'); // Move the original title menu cursor to Quit.
    assert(await page.locator('#player.open').isVisible(), 'The first X should select Quit');
    await tap('x'); // ClosePackage.
  } : undefined;
  await playSavedReplay(await reopenSavedGame(page, saved, quit), tap);
}

async function playExtra(page, frame, tap) {
  await enableCampaignInvincibility(frame);
  await frame.locator('#canvas').focus();
  await tap('ArrowDown'); // ExtraStart is available after a story clear.
  await tap('z');
  await tap('z'); // Confirm Extra difficulty.
  await tap('z'); // Reimu A.
  await frame.waitForFunction(() => (ph3Diagnostics.stage?.index === 7 && ph3Diagnostics.stage.frame >= 130) ||
    ph3Diagnostics.phase === 'error', null, { timeout: 25000 });
  const entry = await frame.evaluate(() => ph3Diagnostics.stage);
  assert.equal(entry?.index, 7, 'The saved story clear should unlock Extra');
  assert.equal(entry.replay, false);
  assert(entry.playerPresent);
  await screenshot(frame, 'extra-start.png');
  const previousSave = await frame.evaluate(path => ph3Runtime.FS.stat(path).mtime.getTime(), scoreFile);
  const start = Date.now();
  report.extra = { entry, result: false, peakEnemyShots: 0 };
  await page.keyboard.down('ControlLeft');
  try {
    while (Date.now() - start < 180000) {
      const { stage, errors, boss, gameOver, scoreModified } = await frame.evaluate(path => ({
        stage: ph3Diagnostics.stage, errors: ph3Diagnostics.errors,
        boss: ph3Diagnostics.messages.some(message => message.includes('/sound/bgm/bgm18.ogg')),
        gameOver: ph3Diagnostics.messages.some(message => message.includes('/sound/bgm/bgm16.ogg')),
        scoreModified: ph3Runtime.FS.stat(path).mtime.getTime(),
      }), scoreFile);
      assert.deepEqual(errors, []);
      assert(!gameOver && stage?.playerState !== 3, 'Extra ended at Game Over');
      report.extra.peakEnemyShots = Math.max(report.extra.peakEnemyShots, stage?.enemyShots ?? 0);
      if (boss && !report.extra.boss) {
        report.extra.boss = stage;
        await screenshot(frame, 'extra-boss.png');
      }
      if (scoreModified > previousSave) {
        assert(report.extra.boss, 'Extra should reach its boss before the clear result');
        report.extra.result = true;
        return;
      }
      await page.keyboard.down('z');
      await page.waitForTimeout(145);
      await page.keyboard.up('z');
      await page.waitForTimeout(28);
    }
    throw new Error('The accelerated Extra route did not reach the result');
  } finally {
    await page.keyboard.up('z');
    await page.keyboard.up('ControlLeft');
    report.extra.seconds = (Date.now() - start) / 1000;
  }
}

async function verifyCampaign(page, frame, tap) {
  await playCampaign(page, frame);
  await screenshot(frame, 'story-result.png');
  const saved = await saveNamedReplay(frame, tap);
  report.savedFiles = Object.fromEntries(savedPaths.map(path => [path, saved[path].length]));
  report.campaign.replayStages = replayStages(saved[replayFile]);
  assert.deepEqual(report.campaign.replayStages, [1, 2, 3, 4, 5, 6]);
  await writeFile(join(output, 'assisted-story-replay.dat'), Buffer.from(saved[replayFile]), { flag: 'wx' });
  const reopened = await reopenSavedGame(page, saved);
  await playExtra(page, reopened, tap);
  await screenshot(reopened, 'extra-result.png');
  const extra = await saveNamedReplay(reopened, tap, true);
  report.extra.replayStages = replayStages(extra[extraReplayFile]);
  assert.deepEqual(report.extra.replayStages, [7]);
  await writeFile(join(output, 'assisted-extra-replay.dat'), Buffer.from(extra[extraReplayFile]), { flag: 'wx' });
  await playSavedReplay(await reopenSavedGame(page, { ...saved, ...extra }), tap);
}

try {
  const viewport = checkPixelScaling ? { width: 1836, height: 1377 } :
    touchTest ? { width: 600, height: 820 } : { width: 1280, height: 900 };
  const page = await browser.newPage({ viewport,
    ...(touchTest ? { isMobile: true, hasTouch: true, deviceScaleFactor: 1 } : {}),
    ...(candidateRuntime ? { serviceWorkers: 'block' } : {}),
  });
  if (candidateRuntime) {
    await page.route(/\/runtime\/thibp\/(thibp\.html|shell\.mjs|eagler-host\.mjs|ph3-open\.(?:mjs|wasm)|ph3-rollback\.wasm|room\.mjs|room-wire\.(?:mjs|wasm)|(?:build|manifest)\.json|rollback-preview\.mjs)(?:\?|$)/, async route => {
      const name = new URL(route.request().url()).pathname.split('/').pop();
      if (checkCoopRewind && name === 'shell.mjs') {
        const source = await readFile(join(candidateRuntime, name), 'utf8');
        const body = source.replace("query.get('thibpPrediction')==='1'", 'true');
        return route.fulfill({body, contentType: 'text/javascript'});
      }
      return route.fulfill({
        path: join(candidateRuntime, name),
        headers: {
          'Cross-Origin-Embedder-Policy': 'require-corp',
          'Cross-Origin-Resource-Policy': 'same-origin',
        },
      });
    });
  }
  page.on('requestfailed', request => {
    report.failedRequests.push({ url: request.url(), error: request.failure()?.errorText });
  });
  page.on('response', response => {
    if (response.status() >= 400) report.failedRequests.push({ url: response.url(), status: response.status() });
    if (response.request().isNavigationRequest() && response.url().includes('/runtime/')) {
      report.runtimeRequests.push({ url: response.url(), status: response.status(), worker: response.fromServiceWorker() });
    }
  });
  page.on('pageerror', error => report.messages.push(error.message));
  page.on('console', message => {
    const text = message.text();
    if (message.type() === 'error' || text.includes('[Woo audio]')) report.messages.push(text);
  });
  await page.addInitScript(({ refreshHz, touchTest, checkTouchMotion }) => {
    if (window === window.top) {
      localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
      if (touchTest) {
        localStorage.setItem('eagler-touch-help-seen-v8', '1');
        localStorage.setItem('eagler-touhou-game-options-v1-thibp', JSON.stringify({ options: { touchEnabled: true } }));
        localStorage.setItem('eagler-touhou-touch-options-v1', JSON.stringify({
          touchMovementMode: 'touch-unlimited', touchSensitivity: 150,
        }));
      }
      return;
    }
    if (checkTouchMotion && location.pathname === '/runtime/thibp/thibp.html') {
      const raf = window.requestAnimationFrame.bind(window);
      window.requestAnimationFrame = callback => raf(function run(now) {
        if (window.thibpHoldFrames) raf(run); else callback(now);
      });
    }
    if (!refreshHz || location.pathname !== '/runtime/thibp/thibp.html') return;
    // Replay the monitor cadence even when headless Chromium itself runs at 60 Hz.
    // Share one timestamp per browser callback batch and keep wall time realistic.
    const period = 1000 / refreshHz;
    let timer, timestamp, sequence = 0;
    const callbacks = new Map();
    globalThis.thibpTestRefreshCallbacks = 0;
    function flush() {
      timer = undefined;
      timestamp += period;
      globalThis.thibpTestRefreshCallbacks++;
      const batch = [...callbacks.keys()];
      for (const id of batch) {
        const callback = callbacks.get(id);
        callbacks.delete(id);
        callback?.(timestamp);
      }
    }
    window.requestAnimationFrame = callback => {
      const id = ++sequence;
      callbacks.set(id, callback);
      if (timer === undefined) {
        if (timestamp === undefined) timestamp = performance.now();
        timer = setTimeout(flush, Math.max(0, timestamp + period - performance.now()));
      }
      return id;
    };
    window.cancelAnimationFrame = id => callbacks.delete(id);
  }, { refreshHz, touchTest, checkTouchMotion });
  await page.goto(url);
  await page.waitForFunction(() => window.__eaglerBoot?.done === true);
  await page.locator('#gamePackageImport').click({ timeout: 12000 });
  await page.locator('#gameDataImportWindow').waitFor({ state: 'visible' });
  if (await page.locator('#transferImport').isVisible()) await page.locator('#transferImport').click();
  const importStarted = Date.now();
  await page.locator('#gameDataImportInput').setInputFiles(data);
  try {
    await page.waitForFunction(() => {
      const ready = document.querySelector('#launch')?.innerText;
      return ready && !ready.includes('导入');
    }, null, { timeout: 50000 });
    report.importMs = Date.now() - importStarted;
    if (refreshHz) report.refresh = {
      requestedHz: refreshHz, locked: lockDisplayTo60, preference: await setRefreshPreference(page),
    };
    const runtimeFrame = await openRuntime(page);
    report.title = await runtimeFrame.evaluate(() => globalThis.ph3Diagnostics);
    assert.deepEqual(report.title.errors, []);
    assert(report.title.presentedFrames > 100);
    await page.screenshot({ path: join(output, 'launcher-title.png') });
    if (refreshHz) {
      await checkRefreshCadence(runtimeFrame, 'title');
      assert(report.refresh.preference.available, 'Launcher exposes the high-refresh option');
      assert.equal(report.refresh.preference.followsDisplay, !lockDisplayTo60);
      await prepareHudFpsProbe(runtimeFrame);
    }
    if (checkPixelScaling) {
      await checkPresentation(page, runtimeFrame, 1836, 1377, 'title-reference-size');
      await checkPresentation(page, runtimeFrame, 1280, 960, 'title-2x');
      await checkPresentation(page, runtimeFrame, 600, 450, 'title-small');
    }
    if (checkCampaign || checkTouchMotion) await enableCampaignInvincibility(runtimeFrame);
    if (checkCoopEngine) await prepareCoopEngine(runtimeFrame);
    if (checkCoopSkill || checkCoopStockHud) {
      await runtimeFrame.evaluate(async () => {
        const {prepareRoomScripts} = await import(new URL('room.mjs', location.href));
        prepareRoomScripts(ph3Runtime.FS);
      });
    }
    if (checkCoopStockHud) await addStageFrameProbe(runtimeFrame, `
      if (p_count == 140) {
        let hx = GetPlayerX;
        let hy = GetPlayerY;
        CreateItemU2(2, hx, hy, hx, hy, 0);
        CreateItemU2(3, hx + 32, hy, hx + 32, hy, 0);
        if (GetThibpSeatCount == 3) {
          CreateItemU2(2, hx + 64, hy, hx + 64, hy, 0);
          CreateItemU2(3, hx + 64, hy, hx + 64, hy, 0);
        }
      }
      if (p_count == 155) {
        let one = GetThibpSeatStock(0);
        let two = GetThibpSeatStock(1);
        if (one[2] != 1 || one[3] != 0 || two[2] != 0 || two[3] != 1) {
          RaiseError("Stock HUD test: fragments did not reach P1 and P2");
        }
        if (GetThibpSeatCount == 3) {
          let three = GetThibpSeatStock(2);
          if (three[2] != 1 || three[3] != 1) { RaiseError("Stock HUD test: fragments did not reach P3"); }
        }
        CreateCommonDataArea("WebStockHudProbe");
        SetAreaCommonData("WebStockHudProbe", "pickedUp", true);
        SaveCommonDataAreaA1("WebStockHudProbe");
      }
    `);
    if (checkCoopSkill) {
      await addStageFrameProbe(runtimeFrame, `
        if (p_count == 30 || p_count == 155 || p_count == 440) {
          SetAreaCommonData(COMMONAREA_SCENE, SC_SKILL, 4);
        }
        if (p_count == 210) {
          CreateShotA1(GetPlayerX + 32, GetPlayerY, 0, 0, 1, 0);
          if (GetThibpSeatCount == 3) {
            CreateShotA1(GetPlayerX + 64, GetPlayerY, 0, 0, 1, 0);
          }
        }
      `);
    }
    if (checkCoopGhost) await addStageFrameProbe(runtimeFrame, `
      if (p_count == 153) {
        SetPlayerLife(0);
        SetPlayerInvincibilityFrame(0);
        CreateShotA1(GetPlayerX, GetPlayerY, 0, 0, 1, 0);
      }
    `);
    if (checkPauseInput) await addStageFrameProbe(runtimeFrame, `
      if(p_count == 1){
        SetAreaCommonData(COMMONAREA_SCENE, SC_SKILL, 2);
        CreateCommonDataArea("KzControlProbe");
      }
      if(GetCommonData(DC_IS_SPECIAL, 0) && !GetAreaCommonData("KzControlProbe", "activated", 0)){
        SetAreaCommonData("KzControlProbe", "activated", 1);
        SaveCommonDataAreaA1("KzControlProbe");
      }
    `);
    const canvas = runtimeFrame.locator('#canvas');
    await canvas.focus();
    const tap = async code => {
      await page.keyboard.press(code, { delay: 90 });
      await page.waitForTimeout(180);
    };
    if (checkRankBack) {
      await verifyRankBack(page, runtimeFrame, tap);
    } else if (replayPath) {
      if (nativeSamples) await prepareNativeComparison(runtimeFrame);
      const replay = Array.from(await readFile(replayPath));
      await runtimeFrame.evaluate(bytes => {
        ph3Runtime.FS.writeFile('/savethibp/replay/Th_IBP_replay01.dat', new Uint8Array(bytes));
      }, replay);
      for (let command = 0; command < 6; command++) await tap('ArrowDown');
      await tap('z'); // Replay menu
      await tap('z'); // Native benchmark slot
    } else {
      await tap('z'); // Story
      if (saveReplay) await tap('ArrowUp'); // Lunatic
      else if (!clearStageOne && !checkCampaign) await tap('ArrowDown'); // Normal
      await tap('z'); // Confirm difficulty
      await tap('z'); // Confirm Reimu A
    }
    if (checkCoopEngine) await verifyCoopEngine(runtimeFrame);
    const targetFrame = checkCoopEngine ? 0 : replayPath ? 2100 : 130;
    const expectedStage = replayPath ? 6 : 1;
    await runtimeFrame.waitForFunction(frame => globalThis.ph3Diagnostics?.stage?.frame >= frame ||
      globalThis.ph3Diagnostics?.phase === 'error', targetFrame, { timeout: replayPath ? 60000 : 35000 });
    report.stage = await runtimeFrame.evaluate(() => globalThis.ph3Diagnostics);
    assert.deepEqual(report.stage.errors, []);
    assert.equal(report.stage.stage.index, expectedStage);
    assert.equal(report.stage.stage.replay, Boolean(replayPath));
    assert(report.stage.stage.playerPresent);
    assert(report.stage.audioSamples > 0, 'The launcher should start the SDL audio stream');
    if (refreshHz) {
      await checkRefreshCadence(runtimeFrame, 'stage');
      await checkHudFps(runtimeFrame);
    }
    if (refreshHz > 60 && !lockDisplayTo60 && !replayPath) await checkInterpolatedMotion(page, runtimeFrame);
    if (nativeSamples) await compareNativeSamples(runtimeFrame);
    if (checkPixelScaling) await checkPresentation(page, runtimeFrame, 1836, 1377, 'battle-reference-size');
    if (clearStageOne) {
      report.nextStage = await playStageOne(page, runtimeFrame, tap);
      assert.equal(report.nextStage.replay, false);
      console.log(`Live Easy story reached Stage 2 at score ${report.nextStage.score}`);
    }
    if (saveReplay) await verifySavedReplay(page, runtimeFrame, tap);
    if (checkCampaign) await verifyCampaign(page, runtimeFrame, tap);
    if (checkRestartMemory) await measureStageRestarts(runtimeFrame, tap);
    if (checkPauseInput) await verifyPauseInput(page, runtimeFrame, tap);
    if (checkTouchMenu) await verifyTouchPauseMenu(page, runtimeFrame);
    if (checkTouchMotion) await verifyTouchMotion(page, runtimeFrame);
    console.log(`Author launcher imported the local Package in ${report.importMs} ms and reached Stage ${expectedStage}, frame ${report.stage.stage.frame}`);
  } finally {
    report.launcher = await page.evaluate(() => ({
      url: location.href,
      launch: document.querySelector('#launch')?.innerText,
      messages: document.body.innerText.split('\n').filter(line => /导入|安装|资源|确认|启动|失败|不支持/.test(line)).slice(-25),
      dialogs: [...document.querySelectorAll('dialog[open]')].map(dialog => dialog.innerText),
    }));
    report.launcher.frames = page.frames().map(frame => frame.url());
  }
} finally {
  await writeFile(outputPath, JSON.stringify(report, null, 2) + '\n', { flag: 'wx' });
  await browser.close();
}
