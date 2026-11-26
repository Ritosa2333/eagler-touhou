import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir, writeFile} from 'node:fs/promises';
import {join} from 'node:path';

const [base, zip, output, executablePath, playwrightPath = 'playwright', seats = '2', difficulty = '1'] = process.argv.slice(2);
assert(base && zip && output && executablePath, 'Usage: check-room.mjs URL ZIP REPORT_DIR BROWSER [PLAYWRIGHT] [2|3] [DIFFICULTY]');
const count = Number(seats);
const mixedLoadouts = process.env.THIBP_TEST_MIXED_LOADOUTS === '1';
const challenge = process.env.THIBP_TEST_CHALLENGE === '1';
const retryKey = process.env.THIBP_TEST_RETRY_KEY === '1';
const testReplay = process.env.THIBP_TEST_ROOM_REPLAY === '1';
const testViewer = process.env.THIBP_TEST_VIEWER === '1';
const testRollback = process.env.THIBP_TEST_ROLLBACK === '1';
const testTitle = process.env.THIBP_TEST_ROOM_TITLE === '1';
const testGameOverRetry = process.env.THIBP_TEST_GAMEOVER_RETRY === '1';
const testPublicRoom = process.env.THIBP_TEST_PUBLIC_ROOM === '1';
const testTouch = process.env.THIBP_TEST_TOUCH === '1';
const testQuickTouch = testTouch && process.env.THIBP_TEST_QUICK_TOUCH === '1';
const testDisconnect = process.env.THIBP_TEST_DISCONNECT === '1';
assert(!testReplay || (!challenge && difficulty === '1'), 'The scripted Replay fixture requires Normal');
assert(!testGameOverRetry || (!challenge && !testReplay), 'The Game Over fixture needs a cooperative run with normal stocks');
const types = (mixedLoadouts ? [1, 3, 4] : [0, 0, 0]).slice(0, count);
assert([2, 3].includes(count));
assert(/^[0-4]$/.test(difficulty));
const {chromium} = createRequire(import.meta.url)(playwrightPath);
const browser = await chromium.launch({executablePath, headless: true, args: [
  '--disable-background-timer-throttling', '--disable-renderer-backgrounding',
  '--disable-backgrounding-occluded-windows',
  ...(testTouch ? ['--use-angle=swiftshader', '--enable-unsafe-swiftshader'] : []),
  ...(testTitle ? [] : ['--autoplay-policy=no-user-gesture-required']),
]});
const report = {passed: false, rooms: [], errors: []};
await mkdir(output, {recursive: true});

async function openPlayer(seat) {
  const mobile = testTouch && seat > 0 && seat < count;
  const context = await browser.newContext({viewport: mobile ? {width: 960, height: 640} :
    {width: seat === 1 ? 485 : 1280, height: 900},
    ...(mobile ? {isMobile: true, hasTouch: true, deviceScaleFactor: 1} : {}),
    ...(testReplay || testGameOverRetry ? {serviceWorkers: 'block'} : {}),
  });
  if (testReplay || testGameOverRetry) {
    await context.route('**/runtime/thibp/*/room.mjs', async route => {
      const response = await route.fetch();
      const source = await response.text();
      const anchor = '  const skillEnds = ';
      assert(source.includes(anchor), 'The Game Over fixture needs the cooperative scripts');
      const wipe = testGameOverRetry ? `
        if (p_count >= 600 && p_count % 55 == 0) {
          SetPlayerInvincibilityFrame(0);
          CreateShotA1(GetPlayerX, GetPlayerY, 0, 0, 1, 0);
        }
      ` : `
        if (p_count == 350 || p_count == 390 || p_count == 430) {
          SetPlayerInvincibilityFrame(0);
          CreateShotA1(GetPlayerX, GetPlayerY, 0, 0, 1, 0);
        }
      `;
      const fixture = testReplay
        ? source.replace('if (IsThibpCooperative) { pl = 2; }', 'if (IsThibpCooperative) { pl = 0; }')
        : source;
      const body = fixture
        .replace(anchor, `  edit('stage/Stage_Main.dnh', '@MainLoop{', ${JSON.stringify('@MainLoop{' + wipe)});\n${anchor}`);
      await route.fulfill({response, body});
    });
  }
  const page = await context.newPage();
  if (testRollback && seat === 1) {
    await page.addInitScript(() => {
      const original = RTCDataChannel.prototype.send;
      const delayed = globalThis.thibpDelayedInput = {packets: 0, delivered: 0, burstPackets: 0, burstUntil: 0};
      RTCDataChannel.prototype.send = function(data) {
        const inputChannel = this.label === 'thibp-input' || this.label === 'thibp-route-input';
        if (!inputChannel || !ArrayBuffer.isView(data)) return original.call(this, data);
        const bytes = new Uint8Array(data.buffer, data.byteOffset, data.byteLength);
        // Leave RTC transport probes alone and delay only THIBP input packets (EINP, type 1).
        if (bytes[0] !== 69 || bytes[1] !== 73 || bytes[2] !== 78 || bytes[3] !== 80 || bytes[5] !== 1) {
          return original.call(this, data);
        }
        const packet = bytes.slice();
        const burst = performance.now() < delayed.burstUntil;
        const delay = (burst ? 120 : 27) + (++delayed.packets % 7) * 3;
        if (burst) delayed.burstPackets++;
        setTimeout(() => {
          if (this.readyState !== 'open') return;
          original.call(this, packet);
          delayed.delivered++;
        }, delay);
      };
    });
  }
  page.on('pageerror', error => report.errors.push({seat, message: error.message}));
  await page.addInitScript(({mobile, seat, testQuickTouch}) => {
    if (window !== window.top) {
      if (!testQuickTouch || !mobile || !location.pathname.endsWith('/thibp.html')) return;
      // Freeze only the local input pump until the gesture is delivered.
      // WebRTC and the native frame loop continue using their ordinary timers.
      const startInterval = window.setInterval.bind(window);
      window.setInterval = (callback, delay, ...args) => startInterval(
        delay === 4 ? (...values) => { if (!window.thibpHoldInput) callback(...values); } : callback,
        delay, ...args);
      return;
    }
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
    if (mobile) {
      localStorage.setItem('eagler-touch-help-seen-v8', '1');
      localStorage.setItem('eagler-touhou-game-options-v1-thibp', JSON.stringify({options: {touchEnabled: true}}));
      localStorage.setItem('eagler-touhou-touch-options-v1', JSON.stringify({
        touchMovementMode: seat === 2 ? 'joystick-free' : 'touch-unlimited', restartButtonEnabled: true,
      }));
    }
  }, {mobile, seat, testQuickTouch});
  await page.goto(new URL('/?game=thibp', base).href);
  await page.waitForFunction(() => window.__eaglerBoot?.done === true);
  await page.locator('#gamePackageImport').click({timeout: 15000});
  await page.locator('#gameDataImportWindow').waitFor();
  if (await page.locator('#transferImport').isVisible()) await page.locator('#transferImport').click();
  await page.locator('#gameDataImportInput').setInputFiles(zip);
  await page.waitForFunction(() => {
    const launch = document.querySelector('#launch')?.innerText;
    return launch && !launch.includes('导入') && !launch.includes('安装中');
  }, null, {timeout: 55000});
  await page.goto(new URL('/lobby.html?game=thibpmp', base).href);
  await page.locator('#createButton').waitFor();
  return page;
}

async function enterRoom(page, code) {
  if (testPublicRoom && code) {
    await page.locator(`[data-key="thibpmp-${code}"] [data-entry]`).click({timeout: 20000});
    return (await page.locator('#mpRoomCode').innerText({timeout: 20000})).trim();
  }
  await page.locator(code ? '#codeButton' : '#createButton').click();
  await page.locator('#roomDialog[open]').waitFor();
  const hasProduct = await page.locator('#gameSelect option[value="thibpmp"]').count();
  assert.equal(hasProduct, 1, 'The author launcher must expose the THIBP multiplayer room');
  await page.locator('#gameSelect').selectOption('thibpmp', {force: true});
  if (code) {
    await page.locator('#roomCodeInput').fill(code);
  } else {
    await page.locator('#capacitySelect').selectOption(seats, {force: true});
    await page.locator('#difficultySelect').selectOption(difficulty, {force: true});
    await page.locator('#visibilitySelect').selectOption(testPublicRoom ? 'public' : 'private', {force: true});
  }
  await page.locator('#submitRoom').click();
  await page.locator('#mpRoomCode').waitFor({timeout: 20000});
  return (await page.locator('#mpRoomCode').innerText()).trim();
}

async function gameState(page) {
  return page.evaluate(() => {
    const iframe = document.querySelector('#gameFrame');
    const runtime = iframe?.contentWindow;
    if (!runtime?.ph3Diagnostics) return null;
    runtime.ph3Runtime?._thibp_coop_probe();
    const {coop, stage, errors, online, performance, spectator, phase, presentedFrames, messages,
      audioPeak, audioSamples} = runtime.ph3Diagnostics;
    const preparation = document.querySelector('.room-workspace-preparation');
    const canvas = runtime.document.querySelector('#canvas');
    const presentation = {
      viewportWidth: window.innerWidth,
      frameWidth: iframe.getBoundingClientRect().width,
      covered: preparation ? !preparation.hidden : null,
      mode: runtime.Module?.eaglerOptions?.netplayMode ?? null,
      canvasPixels: canvas ? [canvas.width, canvas.height] : null,
    };
    const title = {phase, presentedFrames, tracks: messages.filter(line => /\/sound\/bgm\/bgm\d+\.ogg/.test(line)),
      audioPeak, audioSamples, audioState: runtime.Module?.SDL3?.audioContext?.state};
    return {coop, stage, errors, online, performance, spectator, presentation, title};
  });
}

async function checkRoomTitle(page) {
  await page.waitForFunction(() => {
    const runtime = document.querySelector('#gameFrame')?.contentWindow;
    const status = runtime?.ph3Diagnostics;
    return document.querySelector('.room-workspace-preparation')?.hidden === true &&
      status?.phase === 'running' && status.presentedFrames > 0 &&
      status.messages.some(line => line.includes('/sound/bgm/bgm01.ogg')) &&
      status.audioPeak > 0.001 && runtime.Module?.SDL3?.audioContext?.state === 'running';
  }, null, {timeout: 15000});
  const {title, coop, stage, online, errors} = await gameState(page);
  assert.deepEqual(errors, []);
  assert(!stage?.playerPresent && !coop && !online, 'The room title must stay idle before the match');
  assert(title.tracks.every(line => line.includes('/sound/bgm/bgm01.ogg')), 'Waiting players must hear the title track');
  return title;
}

async function checkIdleTitleInput(page) {
  const canvas = page.frameLocator('#gameFrame').locator('#canvas');
  await canvas.focus();
  await page.keyboard.press('x', {delay: 60});
  await page.waitForTimeout(85);
  await page.keyboard.press('x', {delay: 60});
  const {title} = await gameState(page);
  await page.waitForFunction(presented =>
    document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.presentedFrames > presented + 5,
    title.presentedFrames, {timeout: 4000});
  return checkRoomTitle(page);
}

function delayedInput(page) {
  return page.frameLocator('#gameFrame').locator('#canvas').evaluate(() => globalThis.thibpDelayedInput);
}

async function chooseLoadout(page, seat) {
  const ownSeat = page.locator('[data-mp-seat-me="' + seat + '"]');
  await page.waitForFunction(seat => {
    const own = document.querySelector('[data-mp-seat-me="' + seat + '"]');
    const open = document.querySelector('[data-mp-seat-drop="' + seat + '"] button');
    return (own && !own.hidden) || (open && !open.disabled && open.getClientRects().length);
  }, seat, {timeout: 25000});
  if (await ownSeat.evaluate(node => node.hidden)) {
    await page.locator('[data-mp-seat-drop="' + seat + '"] button').click();
  }
  if (types[seat]) {
    await page.locator('.mp-seat-edit:visible').click();
    const next = page.locator('#mpLoadoutNextSeat');
    const role = page.locator('#mpLocalRoleLabel');
    for (let step = 0; step < types[seat]; step++) {
      const previous = (await role.textContent()).trim();
      await next.click();
      await page.waitForFunction(previous => document.querySelector('#mpLocalRoleLabel')?.textContent.trim() !== previous,
        previous, {timeout: 5000});
    }
    await page.keyboard.press('Escape');
  }
}

async function ready(page) {
  await page.locator('#mpReady').click({timeout: 55000});
  await page.waitForFunction(() => document.querySelector('#mpReady')?.getAttribute('aria-pressed') === 'true',
    null, {timeout: 60000});
}

function waitForGameOver(page) {
  return page.waitForFunction(() => {
    const status = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return status?.messages.some(line => line.includes('/sound/bgm/bgm16.ogg')) &&
      status.coop?.players.every(player => player.life < 0);
  }, null, {timeout: 35000});
}

async function verifyReplay(host, guest) {
  const slot = '/savethibpmp/replay/Th_IBP_replay01.dat';
  if (testTouch) await waitForTouchBattle(guest);
  const base = await gameState(host);
  const observe = () => host.waitForFunction(x => {
    const diagnostics = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return diagnostics?.coop?.frame >= 235 && diagnostics.coop.players[1].x > x + 30;
  }, base.coop.players[1].x, {timeout: 12000});
  if (testTouch) {
    await dragTouch(guest, '#touchDirectSurface', 0.14, observe);
  } else {
    await guest.frameLocator('#gameFrame').locator('#canvas').focus();
    await guest.keyboard.down('ArrowRight');
    await guest.keyboard.down('z');
    await guest.waitForTimeout(500);
    await guest.keyboard.up('ArrowRight');
    await guest.keyboard.up('z');
    await observe();
  }
  const live = await gameState(host);
  const iframe = host.frameLocator('#gameFrame');
  const handle = await host.locator('#gameFrame').elementHandle();
  const frame = await handle.contentFrame();
  await Promise.all([waitForGameOver(host), waitForGameOver(guest)]);
  assert.deepEqual((await gameState(host)).errors, []);
  await iframe.locator('#canvas').focus();
  await host.waitForTimeout(1600);
  const tap = async key => {
    await host.keyboard.press(key, {delay: 60});
    await host.waitForTimeout(190);
  };
  await tap('z'); // Save Replay
  await frame.waitForFunction(() => ph3Runtime._thibp_coop_finished() === 1, null, {timeout: 7000});
  await tap('z'); // Confirm the first slot
  await tap('z'); // Name A
  await tap('ArrowLeft');
  await tap('ArrowUp');
  await tap('z'); // Save
  await frame.waitForFunction(path => ph3Runtime.FS.analyzePath(path).exists, slot, {timeout: 10000});
  const bytes = await frame.evaluate(path => ph3Runtime.FS.readFile(path).byteLength, slot);
  assert(bytes > 1000, 'The original named-save screen must serialize every player lane');
  await frame.waitForFunction(path => new Promise(resolve => {
    const request = indexedDB.open('/savethibpmp');
    request.onerror = () => resolve(false);
    request.onsuccess = () => {
      const db = request.result;
      const entry = db.transaction('FILE_DATA').objectStore('FILE_DATA').get(path);
      entry.onsuccess = () => {
        db.close();
        resolve(entry.result?.contents?.length > 1000);
      };
      entry.onerror = () => { db.close(); resolve(false); };
    };
  }), slot, {timeout: 10000});
  for (let row = 0; row < 6; row++) await tap('ArrowDown');
  await tap('z'); // Replay selection on the original title
  await tap('z'); // The new multiplayer save slot
  await frame.waitForFunction(() => (ph3Diagnostics.stage?.replay && ph3Diagnostics.coop?.frame >= 235) ||
    ph3Diagnostics.errors.length > 0, null, {timeout: 20000});
  const playback = await gameState(host);
  assert.deepEqual(playback.errors, []);
  assert.equal(playback.stage.replay, true);
  assert.equal(playback.coop.players.length, count);
  assert.deepEqual(playback.coop.players.map(player => [player.x, player.y, player.life]),
    live.coop.players.map(player => [player.x, player.y, player.life]));
  await host.screenshot({path: join(output, 'room-replay-playback.png')});
  return {live, playback, bytes};
}

async function verifyGameOverRetry(players, requester, level) {
  await Promise.all(players.map(waitForGameOver));
  const ended = await Promise.all(players.map(gameState));
  const stageTrack = level === 7 ? '/sound/bgm/bgm17.ogg' : '/sound/bgm/bgm02.ogg';
  const numberOfStarts = state => state.title.tracks.filter(line => line.includes(stageTrack)).length;
  await requester.frameLocator('#gameFrame').locator('#canvas').focus();
  await requester.locator('#gameViewport').screenshot({path: join(output, 'game-over-before-retry.png')});
  await requester.keyboard.press('ArrowUp', {delay: 125}); // Wrap from Save Replay to Retry
  await requester.waitForTimeout(150);
  await requester.keyboard.press('z', {delay: 65});
  const nextRound = (page, previous) => page.waitForFunction(({stageTrack, previous, level}) => {
    const status = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return status?.stage?.index === level && status.stage.frame > 15 && status.stage.frame < 420 &&
      status.messages.filter(line => line.includes(stageTrack)).length > previous;
  }, {stageTrack, previous: numberOfStarts(previous), level}, {timeout: 12000});
  await nextRound(requester, ended[players.indexOf(requester)]);
  try {
    await Promise.all(players.map((page, seat) => nextRound(page, ended[seat])));
  } catch (error) {
    throw new Error('Retry from Game Over must start a new round on every player browser', {cause: error});
  }
  const retried = await Promise.all(players.map(gameState));
  for (const [seat, state] of retried.entries()) {
    assert.deepEqual(state.errors, []);
    assert.equal(state.coop?.players.length, players.length, 'Retry must keep every cooperative seat');
    assert.equal(state.coop.seed, ended[seat].coop.seed);
    assert(state.coop.tick > ended[seat].coop.tick && state.online.confirmed > ended[seat].online.confirmed,
      'Retry must keep exchanging and consuming confirmed room inputs');
    assert(state.coop.players.every(player => player.life === 2), 'Retry must restore every seat’s stock');
    assert.deepEqual(state.coop.players.map(player => player.type), types);
  }
  const seat = players.indexOf(requester);
  await requester.keyboard.press('ArrowLeft', {delay: 400});
  await Promise.all(players.map(page => page.waitForFunction(({seat, x}) => {
    const status = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return status?.coop?.players[seat]?.x < x - 32;
  }, {seat, x: retried[seat].coop.players[seat].x}, {timeout: 6000})));
  return {ended, retried};
}

async function verifyViewerRestart(watcher, previous, level) {
  await watcher.waitForFunction(({run, level}) => {
    const status = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return status?.spectator?.run && status.spectator.run !== run &&
      status.stage?.index === level && status.coop?.frame > 60;
  }, {run: previous.spectator.run, level}, {timeout: 25000});
  const state = await gameState(watcher);
  assert.equal(state.stage.index, level);
  assert.deepEqual(state.errors, []);
  return state;
}

function waitForTouchBattle(page) {
  return page.waitForFunction(() =>
    document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.frame > 180,
    null, {timeout: 8000});
}

async function dragTouch(page, selector, distance, observe) {
  const control = page.locator(selector);
  await control.waitFor({state: 'visible', timeout: 4000});
  const box = await control.boundingBox();
  const start = {x: Math.round(box.x + box.width * 0.5),
    y: Math.round(box.y + box.height * 0.5), id: 1};
  const hit = await page.evaluate(({x, y, selector}) => {
    const node = document.elementFromPoint(x, y);
    return node?.closest(selector) ? null : node?.id || node?.className || node?.nodeName || 'outside viewport';
  }, {...start, selector});
  assert(!hit, `${selector} is covered by ${hit}`);
  const client = await page.context().newCDPSession(page);
  try {
    await client.send('Input.dispatchTouchEvent', {type: 'touchStart', touchPoints: [start]});
    await client.send('Input.dispatchTouchEvent', {type: 'touchMove',
      touchPoints: [{...start, x: start.x + Math.round(box.width * distance)}]});
    try { await observe(); }
    catch (error) {
      const state = await control.evaluate(node => ({
        active: node.classList.contains('active'),
        player: node.closest('#player')?.className,
        mode: document.querySelector('#gameFrame')?.contentWindow?.Module?.eaglerOptions?.touchMovementMode,
      }));
      throw new Error(`${selector} failed: ${JSON.stringify(state)}`, {cause: error});
    }
  } finally {
    await client.send('Input.dispatchTouchEvent', {type: 'touchEnd', touchPoints: []});
    await client.detach();
  }
}

async function checkQuickTouch(pages, seat) {
  const player = pages[seat];
  const canvas = player.frameLocator('#gameFrame').locator('#canvas');
  const rect = await canvas.boundingBox();
  const previous = (await gameState(player)).coop.players[seat];
  const client = await player.context().newCDPSession(player);
  const start = {x: Math.round(rect.x + rect.width / 2),
    y: Math.round(rect.y + rect.height / 2), id: 1};
  await canvas.evaluate(() => {
    window.thibpHoldInput = true;
    window.thibpLastTouch = null;
    const game = ph3Runtime;
    const ccall = game.ccall;
    game.ccall = function(name, ...args) {
      const result = ccall.call(this, name, ...args);
      if (name === 'hsob_touch_pointer') window.thibpLastTouch = args[2][0];
      return result;
    };
  });
  try {
    await client.send('Input.dispatchTouchEvent', {type: 'touchStart', touchPoints: [start]});
    await client.send('Input.dispatchTouchEvent', {type: 'touchMove', touchPoints: [{...start, y: start.y - 20}]});
    await client.send('Input.dispatchTouchEvent', {type: 'touchEnd', touchPoints: []});
    await player.waitForFunction(() =>
      document.querySelector('#gameFrame')?.contentWindow?.thibpLastTouch === 2,
      null, {timeout: 3000});
  } finally {
    await canvas.evaluate(() => { window.thibpHoldInput = false; });
    await client.detach();
  }
  await Promise.all(pages.map(page => page.waitForFunction(({seat, previous}) => {
    const p = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.players[seat];
    return p?.y < previous.y - 16 && p.x === previous.x;
  }, {seat, previous}, {timeout: 5000})));
}

async function checkTouch(pages, seat) {
  const player = pages[seat];
  await waitForTouchBattle(player);
  const initial = (await gameState(pages[0])).coop.players;
  await dragTouch(player, seat === 2 ? '#touchJoystick' : '#touchDirectSurface', seat === 2 ? -0.44 : -0.14,
    () => Promise.all(pages.map(page => page.waitForFunction(({seat, previous}) => {
      const players = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.players;
      return players?.[seat]?.x < previous - 54;
    }, {seat, previous: initial[seat].x}, {timeout: 4000}))));
  const moved = await Promise.all(pages.map(gameState));
  for (const {coop} of moved) {
    for (let other = 0; other < count; other++) {
      if (other !== seat) assert.equal(coop.players[other].x, initial[other].x, 'Touch may move only the local seat');
    }
  }
  if (testQuickTouch && seat === 1) await checkQuickTouch(pages, seat);
  await player.locator('#touchFunction').waitFor({state: 'visible', timeout: 4000});
  const fire = player.locator('#touchFire');
  const waitForShots = active => Promise.all(pages.map(page => page.waitForFunction(active => {
    const status = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return status?.stage?.playerPresent && (status.stage.playerShots > 0) === active;
  }, active, {timeout: 7500})));
  await waitForShots(false);
  await fire.tap();
  await waitForShots(true);
  await fire.tap();
  await player.locator('#touchBomb').tap();
  await Promise.all(pages.map(page => page.waitForFunction(({seat, bombs}) => {
    const players = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.players;
    return players?.[seat]?.bombs === bombs - 1;
  }, {seat, bombs: initial[seat].bombs}, {timeout: 6000})));
  await player.locator('#touchEscape').tap();
  await Promise.all(pages.map(page => page.waitForFunction(() =>
    document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.stage?.paused === true,
    null, {timeout: 6000})));
  if (seat === 2) await player.locator('#touchRestart').tap({timeout: 5000});
  else {
    const box = await player.locator('#touchDirectSurface').boundingBox();
    const tap = {x: Math.round(box.x + box.width * 0.49), y: Math.round(box.y + box.height * 0.5), id: 2};
    const waitTicks = async () => player.waitForFunction(tick =>
      document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.tick > tick + 8,
      (await gameState(player)).coop.tick, {timeout: 5000});
    await waitTicks();
    const client = await player.context().newCDPSession(player);
    await client.send('Input.dispatchTouchEvent', {type: 'touchStart', touchPoints: [tap]});
    await client.send('Input.dispatchTouchEvent', {type: 'touchMove',
      touchPoints: [{...tap, y: tap.y - Math.round(box.height * 0.12)}]});
    await client.send('Input.dispatchTouchEvent', {type: 'touchEnd', touchPoints: []});
    await client.detach();
    await waitTicks();
    await player.touchscreen.tap(tap.x, tap.y);
  }
  await Promise.all(pages.map(page => page.waitForFunction(() => {
    const diagnostics = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return diagnostics?.stage?.paused === false && diagnostics.coop?.players.every(p => p.bombs === 2);
  }, null, {timeout: 8000})));
}

async function checkDisconnect(pages, code) {
  const host = pages[0];
  const departed = pages.pop();
  await departed.context().close();
  await Promise.all(pages.map(async page => {
    await page.waitForFunction(() =>
      document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.phase === 'disconnected',
      null, {timeout: 30000});
    assert.deepEqual((await gameState(page)).errors, [], 'A lost peer must not crash the game canvas');
    await page.locator('#netplayConnectionReturn').click({timeout: 10000});
    assert.equal((await page.locator('#mpRoomCode').innerText({timeout: 10000})).trim(), code);
    await page.locator('#mpReady').waitFor({state: 'visible', timeout: 10000});
  }));
  pages.push(await openPlayer(count - 1));
  assert.equal(await enterRoom(pages.at(-1), code), code);
  for (const [seat, page] of pages.entries()) {
    await chooseLoadout(page, seat);
    await ready(page);
  }
  await host.locator('#mpStartGame').click({timeout: 25000});
  await Promise.all(pages.map(page => page.waitForFunction(() => {
    const diagnostics = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics;
    return diagnostics?.coop?.frame > 100 && !diagnostics.errors.length;
  }, null, {timeout: 20000})));
}

const pages = [];
let watcher;
try {
  if (testTouch && testViewer) watcher = await openPlayer(count);
  pages.push(await openPlayer(0));
  const code = await enterRoom(pages[0]);
  if (testTitle) {
    await checkRoomTitle(pages[0]);
    report.title = [await checkIdleTitleInput(pages[0])];
    await pages[0].locator('#gameViewport').screenshot({path: join(output, 'room-title.png')});
  }
  for (let seat = 1; seat < count; seat++) {
    pages.push(await openPlayer(seat));
    assert.equal(await enterRoom(pages[seat], code), code);
  }
  if (challenge) {
    await pages[0].locator('#roomWorkspaceTab-game').click();
    await pages[0].locator('#mpRoomChallengeToggle').click();
    await pages[0].waitForFunction(() => document.querySelector('#mpRoomChallengeToggle')?.getAttribute('aria-checked') === 'true');
    await pages[0].keyboard.press('Escape');
  }
  for (const [seat, page] of pages.entries()) await chooseLoadout(page, seat);
  if (testTitle) {
    report.title.push(...await Promise.all(pages.slice(1).map(checkRoomTitle)));
    await Promise.all(pages.map(page => page.frameLocator('#gameFrame').locator('#canvas')
      .evaluate(canvas => { canvas.dataset.titlePreviewCanvas = '1'; })));
  }
  await Promise.all(pages.map(ready));
  await pages[0].locator('#mpStartGame').click({timeout: 25000});
  const both = await Promise.all(pages.map(async page => {
    const deadline = Date.now() + 65000;
    while (Date.now() < deadline) {
      const confirm = page.locator('#decisionDialog[open]:not(.closing) #decisionConfirm');
      if (await confirm.isVisible()) await confirm.click();
      const state = await gameState(page);
      if (state?.errors?.length) throw new Error(state.errors.join('\n'));
      if (state?.coop?.tick > 150) return state;
      await page.waitForTimeout(100);
    }
    throw new Error('The room did not advance the shared battle');
  }));
  const level = difficulty === '4' ? 7 : 1;
  const identity = state => ({seed: state.coop.seed, index: state.stage.index, players: state.coop.players});
  both.forEach(state => {
    assert.equal(state.coop.players.length, count);
    assert.deepEqual(state.coop.players.map(player => player.type), types);
    assert.deepEqual(state.coop.players.map(player => player.bombs), types.map(() => challenge ? 0 : 2));
    assert.equal(state.stage.index, level);
    assert(['rtc', 'relay'].includes(state.online.path));
    assert.deepEqual(identity(state), identity(both[0]));
    if (testTitle) {
      assert(state.title.tracks.some(line => line.includes(level === 1 ? '/sound/bgm/bgm02.ogg' : '/sound/bgm/bgm17.ogg')),
        'The title music must give way to the stage track');
    }
  });
  if (testTitle) {
    await Promise.all(pages.map(page => page.frameLocator('#gameFrame').locator('canvas[data-title-preview-canvas="1"]').waitFor()));
  }
  // Return from the launcher's automatic fullscreen to the embedded room.
  // A game can already be simulating even while this view is covered.
  await Promise.all(pages.map(page => page.goBack({timeout: 5000})));
  await Promise.all(pages.map(page => page.waitForFunction(() =>
    document.querySelector('#player.room-workspace .room-workspace-preparation')?.hidden === true,
    null, {timeout: 10000})));
  const displayed = await Promise.all(pages.map(gameState));
  displayed.forEach(({presentation}) => {
    assert.equal(presentation.mode, 'lan', 'Launcher must be able to read the active room mode');
    assert(presentation.frameWidth > 0 && presentation.frameWidth <= presentation.viewportWidth);
  });
  const guest = pages[1];
  await guest.locator('#gameViewport').screenshot({path: join(output, 'small-window-battle.png')});
  if (testTouch) {
    for (const page of pages.slice(1)) {
      const fire = page.locator('#touchFire');
      await fire.waitFor({state: 'visible', timeout: 5000});
      if (await fire.getAttribute('aria-pressed') === 'true') await fire.tap();
    }
    for (let seat = 1; seat < count; seat++) await checkTouch(pages, seat);
  }
  if (testRollback) {
    both.forEach(state => assert.equal(state.online.path, 'rtc', 'The delay fixture requires real RTC'));
    const delayed = await delayedInput(guest);
    assert(delayed?.delivered > 30, `The guest must inject RTC input delay: ${JSON.stringify(delayed)}`);
  }
  await guest.frameLocator('#gameFrame').locator('#canvas').focus();
  if (testRollback) {
    // Put an input edge inside a short jitter burst even if this peer started ahead.
    await guest.frameLocator('#gameFrame').locator('#canvas').evaluate(() => {
      globalThis.thibpDelayedInput.burstUntil = performance.now() + 300;
    });
  }
  await guest.keyboard.down('ArrowRight');
  await guest.keyboard.down('z');
  await guest.waitForTimeout(650);
  await guest.keyboard.up('ArrowRight');
  await guest.keyboard.up('z');
  await pages[0].waitForFunction(previous => {
    const runtime = document.querySelector('#gameFrame')?.contentWindow;
    return runtime?.ph3Diagnostics?.coop?.players[1]?.x > previous + 32 &&
      runtime.ph3Diagnostics.stage?.playerShots > 0;
  }, both[0].coop.players[1].x, {timeout: 10000});
  if (testRollback) {
    await pages[0].waitForFunction(() => {
      const runtime = document.querySelector('#gameFrame')?.contentWindow;
      const prediction = runtime?.ph3Diagnostics?.online?.prediction;
      return prediction && (prediction.corrections > 0 || prediction.error);
    }, null, {timeout: 15000});
  }
  const after = await Promise.all(pages.map(gameState));
  if (testRollback) {
    const prediction = after[0].online?.prediction;
    assert(prediction?.presented > 0 && prediction.corrections > 0, JSON.stringify(prediction));
    assert.equal(prediction.error, undefined);
    report.delayedInput = await delayedInput(guest);
    assert(report.delayedInput.burstPackets > 0);
  }
  assert.equal(after[0].coop.players[0].x, both[0].coop.players[0].x);
  assert(after[0].coop.players[1].x > both[0].coop.players[1].x + 32);
  if (count === 3) {
    const third = pages[2];
    await third.frameLocator('#gameFrame').locator('#canvas').focus();
    await third.keyboard.down('ArrowUp');
    await third.waitForTimeout(450);
    await third.keyboard.up('ArrowUp');
    await pages[0].waitForFunction(previous => {
      const state = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop;
      return state?.players[2]?.y < previous - 32;
    }, after[0].coop.players[2].y, {timeout: 10000});
  }
  let firstViewer;
  let touchWatchX;
  if (testViewer) {
    if (testTouch) {
      const host = pages[0];
      await waitForTouchBattle(host);
      const previous = (await gameState(host)).coop.players[1].x;
      await dragTouch(guest, '#touchDirectSurface', -0.14, () => host.waitForFunction(x =>
        document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.players[1]?.x < x - 54,
        previous, {timeout: 10000}));
      await guest.locator('#touchEscape').tap();
      await host.waitForFunction(() =>
        document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.stage?.paused === true,
        null, {timeout: 6000});
      touchWatchX = (await gameState(host)).coop.players[1].x;
    }
    watcher ??= await openPlayer(count);
    assert.equal(await enterRoom(watcher, code), code);
    const deadline = Date.now() + 45000;
    while (Date.now() < deadline) {
      const confirm = watcher.locator('#decisionDialog[open]:not(.closing) #decisionConfirm');
      if (await confirm.isVisible()) await confirm.click();
      firstViewer = await gameState(watcher);
      if (firstViewer?.errors?.length) throw new Error(firstViewer.errors.join('\n'));
      if (firstViewer?.coop?.frame > 160 && firstViewer.spectator?.run) break;
      await watcher.waitForTimeout(120);
    }
    assert(firstViewer?.coop?.frame > 160 && firstViewer.spectator, 'Late spectator must reach the battle');
    assert.equal(firstViewer.coop.players.length, count);
    assert.equal(firstViewer.stage.index, level);
    if (touchWatchX !== undefined) {
      await watcher.waitForFunction(x =>
        document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.coop?.players[1]?.x === x,
        touchWatchX, {timeout: 25000});
    }
    await watcher.screenshot({path: join(output, 'room-spectator.png')});
    await watcher.locator('#cooperativeSpectatorPause').click({timeout: 6000});
    const observe = () => document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.spectator;
    await watcher.waitForFunction(() => document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.spectator?.paused);
    const stopped = await watcher.evaluate(observe);
    await watcher.waitForFunction(received => {
      const view = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.spectator;
      return view?.paused && view.received > received + 12;
    }, stopped.received, {timeout: 6000});
    assert.equal((await watcher.evaluate(observe)).position, stopped.position);
    await watcher.locator('#cooperativeSpectatorLive').click({timeout: 6000});
    await watcher.waitForFunction(() => {
      const view = document.querySelector('#gameFrame')?.contentWindow?.ph3Diagnostics?.spectator;
      return view?.following && !view.paused && !view.seeking;
    }, null, {timeout: testTouch ? 30000 : 12000});
  }
  await guest.frameLocator('#gameFrame').locator('#canvas').focus();
  const involved = name => Promise.all(pages.map(page => page.waitForFunction(name => {
    const runtime = document.querySelector('#gameFrame')?.contentWindow;
    const { stage, coop } = runtime?.ph3Diagnostics ?? {};
    if (name === 'paused') return stage?.paused === true;
    return stage?.paused === false && coop?.frame > 15 && coop.frame < 110 && coop.tick > 200;
  }, name, {timeout: 15000})));
  if (touchWatchX === undefined) await guest.keyboard.press('Escape', {delay: 60});
  await involved('paused');
  if (testRollback) {
    const paused = await Promise.all(pages.map(gameState));
    for (const state of paused.slice(1)) {
      assert.equal(state.coop.frame, paused[0].coop.frame);
      assert.deepEqual(identity(state), identity(paused[0]), 'Late input must converge before the team pauses');
    }
  }
  if (retryKey) {
    await guest.waitForTimeout(200);
    await guest.keyboard.press('r', {delay: 60});
  } else {
    await guest.keyboard.press('ArrowUp', {delay: 90});
    await guest.waitForTimeout(80);
    await guest.keyboard.press('z', {delay: 60});
  }
  await involved('restarted');
  const restarted = await Promise.all(pages.map(gameState));
  restarted.forEach(state => {
    assert.equal(state.stage.index, level);
    assert.equal(state.coop.seed, both[0].coop.seed);
    assert.equal(state.coop.players.length, count);
  });
  await pages[0].screenshot({path: join(output, 'shared-battle.png')});
  const restartedViewer = watcher ? await verifyViewerRestart(watcher, firstViewer, level) : null;
  const gameOverRetry = testGameOverRetry ? await verifyGameOverRetry(pages, count === 2 ? guest : pages[0], level) : null;
  if (watcher && gameOverRetry) gameOverRetry.viewer = await verifyViewerRestart(watcher, restartedViewer, level);
  const replay = testReplay ? await verifyReplay(pages[0], guest) : null;
  if (testDisconnect) await checkDisconnect(pages, code);
  report.rooms.push({code, count, difficulty: Number(difficulty), challenge, types, retryKey, both, after,
    restarted, gameOverRetry, replay, firstViewer, restartedViewer});
  assert.deepEqual(report.errors, []);
  report.passed = true;
  console.log('Author Launcher room reached Stage', level, 'with', count, 'players');
} catch (error) {
  report.failure = error.stack;
  report.runtime = await Promise.all(pages.map(gameState));
  if (testRollback && pages[1]) report.delayedInput = await delayedInput(pages[1]).catch(() => null);
  if (watcher) {
    report.watcher = await gameState(watcher);
    report.watcherLaunch = {
      url: watcher.url(),
      browserText: (await watcher.locator('body').innerText()).slice(-2800),
    };
    await watcher.screenshot({path: join(output, 'watcher-failure.png')});
  }
  report.pages = await Promise.all(pages.map(async page => ({
    url: page.url(), recentText: (await page.locator('body').innerText()).slice(-2400),
  })));
  throw error;
} finally {
  await writeFile(join(output, 'room-report.json'), JSON.stringify(report, null, 2) + '\n');
  await browser.close();
}
