// Real browser coverage for decorative icon hit-testing and Motion ownership.
// Run against an assembled candidate UI; no game is launched or room joined.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || '/usr/bin/chromium', headless: true});
try {
  const page = await browser.newPage(), errors = [];
  page.on('pageerror', error => errors.push(error.message));
  await page.setViewport({width: 1440, height: 960});
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  const ready = async () => {
    await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.body.classList.contains('app-shell-activation-pending'));
    if (await page.$('#firstUseNoticeDialog[open]')) {
      await page.click('#firstUseNoticeClose'); await page.waitForSelector('#firstUseNoticeDialog:not([open])');
    }
  };
  const url = process.argv[2] || 'http://127.0.0.1:8130/';
  await page.goto(url, {waitUntil: 'networkidle2'}); await ready();
  await page.evaluate(() => document.documentElement.style.setProperty('--motion-duration', '1s'));
  await page.click('[data-site-trigger="settings"]');
  await page.waitForSelector('.site-navigation-inner[data-motion-enter="menu"]');
  const animations = await page.$eval('.site-navigation-inner', node => node.getAnimations().map(animation => animation.effect.getTiming().duration));
  assert.ok(animations.length >= 2); assert.ok(animations.every(duration => duration === 1000));
  await page.waitForFunction(() => document.querySelector('.site-navigation-tray').getBoundingClientRect().height >= document.querySelector('.site-navigation-inner').getBoundingClientRect().height - 1);
  await page.click('#lessMotionToggle');
  await page.waitForFunction(() => !document.querySelector('.site-navigation-inner').hasAttribute('data-motion-enter'));
  assert.equal(await page.$eval('.site-navigation-inner', node => node.getAnimations().length), 0);
  assert.equal(await page.$eval('.site-navigation-inner', node => node.style.transform), '');
  await page.click('#lessMotionToggle'); await page.keyboard.press('Escape');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  await page.click('[data-site-trigger="settings"]');
  assert.equal(await page.$eval('.site-navigation-inner', node => node.getAnimations().length), 0);
  await page.keyboard.press('Escape');
  await page.setViewport({width: 390, height: 844, isMobile: true, hasTouch: true});
  await page.goto(url, {waitUntil: 'networkidle2'}); await ready();
  await page.waitForFunction(() => document.querySelector('#mastheadMenu').dataset.compact === 'true');
  for (let cycle = 0; cycle < 3; cycle++) {
    // Hit the actual glyph, not the button's padding. Replacing Settings with X
    // must not detach the click target before the document outside handler.
    const target = await page.$eval('#mastheadMenuToggle', node => {
      const rect = node.getBoundingClientRect();
      return document.elementFromPoint(rect.x + rect.width / 2, rect.y + rect.height / 2)?.closest('button')?.id;
    });
    assert.equal(target, 'mastheadMenuToggle');
    await page.tap('#mastheadMenuToggle');
    await page.waitForFunction(() => document.querySelector('#mastheadMenu').dataset.open === 'true');
    assert.ok(await page.$('#mastheadMenuToggle svg.lucide-x'));
    await page.tap('#mastheadMenuToggle');
    await page.waitForFunction(() => document.querySelector('#mastheadMenu').dataset.open === 'false');
    assert.ok(await page.$('#mastheadMenuToggle svg.lucide-settings-2'));
  }
  assert.deepEqual(errors, []);
  console.log(JSON.stringify({lucideMotion: 'PASS', mobileToggleCycles: 3, reducedMotion: 'PASS', browser: await browser.version()}));
} finally {await browser.close();}
