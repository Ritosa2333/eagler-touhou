// Local-only visual geometry check for TH09 on simulated phone viewports.
import puppeteer from 'puppeteer-core';
import assert from 'node:assert/strict';
import { existsSync, readdirSync } from 'node:fs';
import { resolve } from 'node:path';

const browsers = resolve(process.env.LOCALAPPDATA || '', 'ms-playwright');
const installations = existsSync(browsers) ? readdirSync(browsers).filter(name => /^chromium-\d+$/.test(name)).sort().reverse() : [];
const executablePath = process.env.EAGLER_CHROMIUM || resolve(browsers, installations[0] || '', 'chrome-win64/chrome.exe');
if (!existsSync(executablePath)) throw Error(`Chromium not found: ${executablePath}`);
const browser = await puppeteer.launch({ executablePath, headless: true, args: ['--no-sandbox'] });
try {
  const page = await browser.newPage();
  await page.setUserAgent('Mozilla/5.0 (Linux; Android 15; Pixel 8) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/153.0.0.0 Mobile Safari/537.36');
  await page.evaluateOnNewDocument(() => {
    localStorage.setItem('eagler-touhou-game-options-v1-th09', JSON.stringify({ options: { touchEnabled: true } }));
    localStorage.setItem('eagler-touch-help-seen-v8', '1');
  });
  if (process.env.EAGLER_BLOCK_TH09_CSS === '1') {
    await page.setRequestInterception(true);
    page.on('request', request => request.url().endsWith('/managed.css') ? request.abort() : request.continue());
  }
  const viewport = (width, height) => ({ width, height, isMobile: true, hasTouch: true, deviceScaleFactor: 2 });
  await page.setViewport(viewport(844, 390));
  await page.goto(new URL('?game=th09', process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8131/').href, { waitUntil: 'networkidle2' });
  if (await page.$eval('#firstUseNoticeDialog', dialog => dialog.open)) await page.evaluate(() => document.querySelector('#firstUseNoticeClose').click());
  await page.evaluate(() => document.querySelector('#launch').click());
  await page.waitForFunction(() => document.querySelector('#decisionDialog').open || document.querySelector('#player').classList.contains('open'), { timeout: 10000 });
  if (await page.$eval('#decisionDialog', dialog => dialog.open)) {
    await page.evaluate(() => document.querySelector('#decisionConfirm').click());
  }
  try {
    await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(frame => frame.contentWindow?.__th09Runtime?.status().title[0] > 120), { timeout: 20000 });
  } catch {
    const state = await page.evaluate(() => ({
      status: document.querySelector('#status')?.textContent,
      startupError: document.querySelector('#startupErrorText')?.textContent,
      decision: [...document.querySelectorAll('dialog')].filter(dialog => dialog.open).map(dialog => dialog.textContent.slice(0, 500)),
      player: document.querySelector('#player')?.className,
      frames: [...document.querySelectorAll('iframe')].map(frame => frame.src),
    }));
    throw Error(`TH09 mobile launch did not reach title: ${JSON.stringify(state)}`);
  }
  await page.evaluate(() => { if (!document.querySelector('#touchHelp').hidden) document.querySelector('#touchHelpClose').click(); });
  const measure = async name => {
    const result = await page.evaluate(() => {
      const rect = node => { const r = node.getBoundingClientRect(); return { x: r.x, y: r.y, width: r.width, height: r.height }; };
      const frame = document.querySelector('#gameFrame');
      const canvas = frame.contentDocument.querySelector('canvas');
      const style = frame.contentWindow.getComputedStyle(canvas);
      return { viewport: { innerWidth, innerHeight, visualWidth: visualViewport?.width, visualHeight: visualViewport?.height },
        fullscreen: !!document.fullscreenElement, player: rect(document.querySelector('#player')),
        frame: rect(frame), canvas: rect(canvas), canvasCss: { width: style.width, height: style.height, display: style.display,
          position: style.position, transform: style.transform },
        runtimeViewport: { width: frame.contentWindow.innerWidth, height: frame.contentWindow.innerHeight } };
    });
    await page.screenshot({ path: resolve(import.meta.dirname, `../.cache/th09-mobile-${name}.png`) });
    console.log(JSON.stringify({ name, ...result }));
    const { canvas, player } = result;
    assert.ok(canvas.x >= -1 && canvas.y >= -1 && canvas.x + canvas.width <= player.width + 1 && canvas.y + canvas.height <= player.height + 1,
      `${name}: TH09 canvas must fit inside the phone viewport`);
    assert.ok(Math.abs(canvas.width / canvas.height - 4 / 3) < 0.001, `${name}: TH09 canvas must remain 4:3`);
    assert.ok(Math.abs(canvas.x - (player.width - canvas.width) / 2) < 1 &&
      Math.abs(canvas.y - (player.height - canvas.height) / 2) < 1, `${name}: TH09 canvas must be centered`);
  };
  await measure('landscape');
  const surface = await page.evaluate(() => {
    const node = document.querySelector('#touchDirectSurface');
    const frame = document.querySelector('#gameFrame');
    const canvas = frame.contentDocument.querySelector('canvas');
    const frameRect = frame.getBoundingClientRect();
    const canvasRect = canvas.getBoundingClientRect();
    frame.contentWindow.__directTouchProbe = [];
    const core = frame.contentWindow.__th09Runtime.core;
    const original = core._th09_touch;
    core._th09_touch = (...args) => { frame.contentWindow.__directTouchProbe.push(args); return original(...args); };
    return { hidden: node.hidden, frameLeft: frameRect.left, canvasLeft: frameRect.left + canvasRect.left,
      canvasRight: frameRect.left + canvasRect.right, frameRight: frameRect.right,
      y: frameRect.top + canvasRect.top + canvasRect.height / 2 };
  });
  assert.equal(surface.hidden, false, 'Android direct-touch surface must be visible');
  assert.ok(surface.canvasRight + 25 < surface.frameRight, 'test needs a right letterbox');
  console.log(JSON.stringify({ name: 'touch-target', surface, hit: await page.evaluate((x, y) => {
    const node = document.elementFromPoint(x, y);
    return { id: node?.id, className: node?.className, pointerEvents: node ? getComputedStyle(node).pointerEvents : null };
  }, surface.canvasRight - 25, surface.y) }));
  const cdp = await page.createCDPSession();
  const touch = (type, x, y) => cdp.send('Input.dispatchTouchEvent', {
    type, touchPoints: type === 'touchEnd' ? [] : [{ x, y, id: 1, radiusX: 1, radiusY: 1 }],
  });
  const fire = await page.evaluate(() => {
    const button = document.querySelector('#touchFire');
    const frame = document.querySelector('#gameFrame');
    const core = frame.contentWindow.__th09Runtime.core;
    frame.contentWindow.__fireKeyProbe = [];
    const original = core._th09_key;
    core._th09_key = (...args) => { frame.contentWindow.__fireKeyProbe.push(args); return original(...args); };
    const rect = button.getBoundingClientRect();
    return { x: rect.left + rect.width / 2, y: rect.top + rect.height / 2, label: button.querySelector('small').textContent };
  });
  assert.equal(fire.label, '按住开火／蓄力', 'TH09 Fire must explain its hold-to-charge behavior');
  await touch('touchStart', fire.x, fire.y);
  await page.waitForFunction(() => document.querySelector('#gameFrame').contentWindow.__fireKeyProbe.length >= 1);
  assert.equal(await page.$eval('#touchFire', button => button.getAttribute('aria-pressed')), 'true');
  await touch('touchEnd', fire.x, fire.y);
  await page.waitForFunction(() => document.querySelector('#gameFrame').contentWindow.__fireKeyProbe.length >= 2);
  assert.equal(await page.$eval('#touchFire', button => button.getAttribute('aria-pressed')), 'false');
  const fireKeys = await page.evaluate(() => document.querySelector('#gameFrame').contentWindow.__fireKeyProbe);
  assert.deepEqual(fireKeys, [[44, 1], [44, 0]], 'TH09 Fire must hold and release native Z exactly once');
  await touch('touchStart', surface.canvasRight - 25, surface.y);
  await touch('touchMove', surface.canvasRight + 25, surface.y);
  await touch('touchEnd', surface.canvasRight + 25, surface.y);
  try {
    await page.waitForFunction(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe.length >= 3, { timeout: 4000 });
  } catch {
    console.log(JSON.stringify({ name: 'touch-diagnostics', state: await page.evaluate(() => ({
      events: document.querySelector('#gameFrame').contentWindow.__directTouchProbe,
      captured: document.querySelector('#touchDirectSurface').hasPointerCapture(1),
      hit: document.elementFromPoint(700, 195)?.id,
    })) }));
    throw Error('TH09 touch did not forward down/move/up');
  }
  const events = await page.evaluate(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe);
  assert.deepEqual(events.map(event => event[0]), [0, 1, 2], 'one finger must survive crossing the letterbox boundary');
  assert.equal(events[0][1], events[1][1], 'cross-border movement must keep the same finger ID');
  assert.ok(events[1][2] > 1, 'black-bar coordinates must be forwarded outside the 4:3 canvas');
  await page.evaluate(() => { document.querySelector('#gameFrame').contentWindow.__directTouchProbe = []; });
  const outsideX = surface.canvasRight + 35;
  const outsideHit = await page.evaluate((x, y) => document.elementFromPoint(x, y)?.id, outsideX, surface.y);
  await touch('touchStart', outsideX, surface.y);
  await touch('touchMove', surface.canvasRight - 35, surface.y);
  await touch('touchEnd', surface.canvasRight - 35, surface.y);
  await page.waitForFunction(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe.length >= 3, { timeout: 4000 });
  const outsideEvents = await page.evaluate(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe);
  console.log(JSON.stringify({ name: 'outside-start', outsideHit, outsideEvents }));
  assert.deepEqual(outsideEvents.map(event => event[0]), [0, 1, 2], 'a gesture starting in the letterbox must reach the game');
  assert.equal(outsideEvents[0][1], outsideEvents[1][1], 'outside-start must retain its finger ID');
  assert.ok(outsideEvents[0][2] > 1 && outsideEvents[1][2] < 1, 'outside-start coordinates must cross into the canvas');
  await page.setViewport(viewport(390, 844));
  await page.waitForFunction(() => innerWidth < innerHeight, { timeout: 10000 });
  await measure('portrait');
  const portraitTouch = await page.evaluate(() => {
    const frame = document.querySelector('#gameFrame');
    const canvas = frame.contentDocument.querySelector('canvas').getBoundingClientRect();
    frame.contentWindow.__directTouchProbe = [];
    return { x: innerWidth / 2, outsideY: canvas.bottom + 35, insideY: canvas.bottom - 35,
      hit: document.elementFromPoint(innerWidth / 2, canvas.bottom + 35)?.id };
  });
  await touch('touchStart', portraitTouch.x, portraitTouch.outsideY);
  await touch('touchMove', portraitTouch.x, portraitTouch.insideY);
  await touch('touchEnd', portraitTouch.x, portraitTouch.insideY);
  await page.waitForFunction(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe.length >= 3, { timeout: 4000 });
  const portraitEvents = await page.evaluate(() => document.querySelector('#gameFrame').contentWindow.__directTouchProbe);
  console.log(JSON.stringify({ name: 'portrait-outside-start', portraitTouch, portraitEvents }));
  assert.deepEqual(portraitEvents.map(event => event[0]), [0, 1, 2], 'portrait black-bar start must reach the game');
  assert.ok(portraitEvents[0][3] > 1 && portraitEvents[1][3] < 1, 'portrait coordinates must cross into the canvas');
  const desktop = await browser.newPage();
  if (process.env.EAGLER_BLOCK_TH09_CSS === '1') {
    await desktop.setRequestInterception(true);
    desktop.on('request', request => request.url().endsWith('/managed.css') ? request.abort() : request.continue());
  }
  await desktop.setViewport({ width: 1440, height: 900, deviceScaleFactor: 1 });
  await desktop.goto(new URL('?game=th09', process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8131/').href, { waitUntil: 'networkidle2' });
  if (await desktop.$eval('#firstUseNoticeDialog', dialog => dialog.open)) await desktop.evaluate(() => document.querySelector('#firstUseNoticeClose').click());
  await desktop.evaluate(() => document.querySelector('#launch').click());
  await desktop.waitForFunction(() => document.querySelector('#decisionDialog').open || document.querySelector('#player').classList.contains('open'));
  if (await desktop.$eval('#decisionDialog', dialog => dialog.open)) await desktop.evaluate(() => document.querySelector('#decisionConfirm').click());
  await desktop.waitForFunction(() => [...document.querySelectorAll('iframe')].some(frame => frame.contentWindow?.__th09Runtime?.status().title[0] > 120), { timeout: 20000 });
  const desktopGeometry = await desktop.evaluate(() => {
    const frame = document.querySelector('#gameFrame');
    const player = document.querySelector('#player').getBoundingClientRect();
    const canvas = frame.contentDocument.querySelector('canvas').getBoundingClientRect();
    return { player: { width: player.width, height: player.height }, canvas: { x: canvas.x, y: canvas.y, width: canvas.width, height: canvas.height },
      directTouchHidden: document.querySelector('#touchDirectSurface').hidden };
  });
  assert.equal(desktopGeometry.directTouchHidden, true, 'desktop must not receive a touch overlay');
  assert.ok(Math.abs(desktopGeometry.canvas.x - (desktopGeometry.player.width - desktopGeometry.canvas.width) / 2) < 1 &&
    Math.abs(desktopGeometry.canvas.y - (desktopGeometry.player.height - desktopGeometry.canvas.height) / 2) < 1,
  `desktop canvas must be centered: ${JSON.stringify(desktopGeometry)}`);
  assert.ok(desktopGeometry.canvas.width > 640 && desktopGeometry.canvas.height > 480,
    `desktop canvas must scale up: ${JSON.stringify(desktopGeometry)}`);
  console.log(JSON.stringify({ name: 'desktop', ...desktopGeometry, directTouchEvents: events }));
} finally { await browser.close(); }
