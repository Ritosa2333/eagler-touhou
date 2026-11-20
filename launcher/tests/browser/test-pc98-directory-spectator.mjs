// Join a real ongoing PC-98 match from the public Lobby, using a fresh viewer.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {browserTestUrl} from '../support/browser-target.mjs';

const game = process.env.ROOM_RUNTIME_GAME;
assert.match(game ?? '', /^th0[1-4]$/);
const entry = process.env.PC98_SPECTATOR_ENTRY ?? 'directory';
assert.ok(['directory','room'].includes(entry));
const product = game + 'mp', base = new URL(browserTestUrl());
const url = path => new URL(path, base).href;
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable(),
  args:['--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages = [], errors = [];
const timeout = 45000;
const enabled = (page, selector) => page.waitForFunction(selector => {
  const node = document.querySelector(selector);
  return node && !node.disabled && !!node.getClientRects().length;
}, {timeout}, selector);

async function open(probeEffects = false) {
  const page = await (await browser.createBrowserContext()).newPage();
  pages.push(page); page.on('pageerror', error => errors.push(error.message));
  await page.setViewport({width:1440,height:900});
  await page.evaluateOnNewDocument(probeEffects => {
    window.pc98TestEvents = [];
    addEventListener('message', e => {
      if (e.data?.protocol === 'eagler-touhou/1') window.pc98TestEvents.push(e.data);
    });
    if (!probeEffects || !location.search.includes('runtimeEpoch=')) return;
    window.pc98EffectPeak = 0;
    const start = AudioBufferSourceNode.prototype.start;
    AudioBufferSourceNode.prototype.start = function (...args) {
      if (this.buffer && this.context.state === 'running') {
        for (const sample of this.buffer.getChannelData(0))
          window.pc98EffectPeak = Math.max(window.pc98EffectPeak, Math.abs(sample));
      }
      return start.apply(this,args);
    };
  },probeEffects);
  return page;
}
async function navigate(page, path) {
  await page.goto(url(path));
  await page.waitForFunction(() => /lobby\.html/.test(location.pathname)
    ? !document.documentElement.hasAttribute('data-lobby-boot') : window.__eaglerBoot?.done === true,{timeout});
  await page.evaluate(() => document.querySelector('#firstUseNoticeDialog')?.close());
}
async function click(page, selector) {
  await page.bringToFront();
  await enabled(page, selector);
  await page.click(selector);
}
const firstFrame = (page, after = 0) => page.waitForFunction(after =>
  pc98TestEvents.slice(after).some(e => e.event === 'first-frame'), {timeout}, after);
const frame = page => page.frames().find(frame => frame.url().includes('runtimeEpoch='));
const playable = page => page.waitForFunction(() => {
  const plays = pc98TestEvents.filter(e => e.spectatorPlayback?.position > 30);
  return plays.length > 2 && plays.at(-1).spectatorPlayback.position > plays[0].spectatorPlayback.position
    && !plays.at(-1).spectatorPlayback.seeking;
}, {timeout});

try {
  const host = await open();
  await navigate(host, 'lobby.html?test=' + game);
  await click(host, '#createButton');
  await host.waitForSelector('#roomDialog[open] #gameSelect',{timeout});
  await host.select('#gameSelect', product);
  await click(host, '#submitRoom');
  await host.waitForSelector('#mpRoomCode', {timeout});
  const code = await host.$eval('#mpRoomCode', n => n.textContent.trim());
  const guest = await open();
  await navigate(guest, '?' + new URLSearchParams({game:product,mpRoom:code,lobbyAction:'join',test:game}));
  for (const [seat, player] of [host,guest].entries()) {
    await player.waitForSelector('#mpSeatStage',{timeout});
    const join = `[data-mp-seat-drop="${seat}"] button`;
    const seated = `[data-mp-seat-me="${seat}"]:not([hidden])`;
    await player.bringToFront();
    await player.waitForFunction((seated,join) => {
      const button = document.querySelector(join);
      return document.querySelector(seated) || button && !button.disabled && !!button.getClientRects().length;
    },{timeout},seated,join);
    if (!await player.$(seated)) await player.click(join);
    await player.waitForSelector(seated,{timeout});
  }
  for (const player of [host,guest]) {
    await click(player, '#mpReady');
    await player.waitForSelector('#mpReady[aria-pressed="true"]',{timeout});
  }
  const before = await Promise.all([host,guest].map(page => page.evaluate(() => pc98TestEvents.length)));
  await click(host, '#mpStartGame');
  await Promise.all([host,guest].map((page,index) => firstFrame(page,before[index])));
  if (game === 'th03') {
    for (let turn=0;turn<4;turn++) for (const page of [host,guest]) {
      await page.bringToFront();
      await frame(page).click('#canvas');
      await page.keyboard.press('KeyZ',{delay:160});
    }
    await frame(host).waitForFunction(() => th03SyncState?.phase === 1, {timeout});
  }
  const viewer = await open(game === 'th03');
  if (entry === 'room') {
    await navigate(viewer, '?' + new URLSearchParams({game:product,mpRoom:code,lobbyAction:'join',test:game}));
    await click(viewer, '#mpSpectatorPreviewJoin');
  } else {
    await navigate(viewer, 'lobby.html?test=' + game);
    const row = `[data-key="${product}-${code}"]`;
    await viewer.waitForSelector(`${row} .lobby-room-state[data-state="playing"]`,{timeout});
    const join = `${row} .lobby-join`;
    await enabled(viewer,join);
    assert.match(await viewer.$eval(join, n => n.textContent), /Spectate|旁观|觀戰|観戦/);
    await viewer.click(join);
  }
  await playable(viewer);
  await viewer.waitForFunction(() => !document.querySelector('.room-workspace-preparation')?.getClientRects().length,{timeout});
  if (game === 'th03') {
    const view = frame(viewer);
    // Do not interact with the iframe: the real join click must unlock audio.
    await view.waitForFunction(async () => {
      const {managedAPI} = await import('./managed-native-room.mjs'), {scene,audio} = managedAPI.health();
      return scene === 9 && audio.state === 'running' && audio.musicPlaying && !!audio.music && audio.music !== 'select';
    },{timeout});
    await view.evaluate(() => { window.pc98EffectPeak = 0; });
    await host.bringToFront();
    await frame(host).click('#canvas');
    await host.keyboard.down('KeyZ');
    await view.waitForFunction(() => pc98EffectPeak > 0.001,{timeout});
    await host.keyboard.up('KeyZ');
    await click(viewer, '#cooperativeSpectatorPause');
    await view.waitForFunction(async () => !(await import('./managed-native-room.mjs')).managedAPI.health().audio.musicPlaying,{timeout});
    await click(viewer, '#cooperativeSpectatorLive');
    await view.waitForFunction(async () => (await import('./managed-native-room.mjs')).managedAPI.health().audio.musicPlaying,{timeout});
    console.log('PASS TH03 late spectator plays current BGM and nonzero SFX after joining');
  }
  assert.deepEqual(errors,[]);
  console.log('PASS spectator entry',entry,product,code);
} catch (error) {
  for (const page of pages) console.error(await page.evaluate(() => ({
    url:location.href, body:document.body?.innerText.slice(-1100), events:window.pc98TestEvents?.slice(-10),
  })).catch(() => null));
  console.error(errors);
  throw error;
} finally {await browser.close();}
