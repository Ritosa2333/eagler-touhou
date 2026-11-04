import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage();
  const base = process.argv[2] || 'http://127.0.0.1:5173/';
  for (const width of [320, 390, 768, 1440]) {
    await page.setViewport({width, height: 900});
    for (const route of ['', 'lobby.html', 'profile.html', 'login.html']) {
      await page.goto(new URL(route, base).href, {waitUntil: 'networkidle2'});
      await page.evaluate(() => document.fonts.ready);
      await page.waitForSelector('.book-theme-toggle');
      const geometry = () => page.evaluate(() => [...document.querySelectorAll('.book-masthead,.book-library,.book-card,.book-directory,.profile-wrap,.account-shell')].map(el => {
        const rect = el.getBoundingClientRect();
        return [rect.x, rect.width];
      }));
      const before = await geometry();
      await page.click('.book-theme-toggle');
      assert.deepEqual(await geometry(), before, `${width}/${route}: geometry during theme change`);
      await page.evaluate(() => Promise.all(document.getAnimations().filter(a => a.effect?.getTiming().iterations !== Infinity).map(a => a.finished.catch(() => {}))));
      assert.deepEqual(await geometry(), before, `${width}/${route}: geometry after theme change`);
      if (!route && width <= 768) {
        const icon = await page.$eval('.book-favorite', el => {
          el.style.fontSize = '8px';
          const rect = el.querySelector('svg').getBoundingClientRect();
          return [rect.width, rect.height, el.getBoundingClientRect().width];
        });
        assert.deepEqual(icon, [22, 22, 44], 'Star size is independent of text size');
      }
    }
  }
  await page.click('.account-tabs button:last-child');
  await page.waitForSelector('[name=username]');
  assert.equal(await page.$eval('.account-mode-content', el => getComputedStyle(el).animationName), 'account-mode-enter');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  assert.equal(await page.$eval('.account-mode-content', el => getComputedStyle(el).animationName), 'none');
  console.log('PASS: theme preserves page geometry, narrow stars keep fixed size, account transition respects reduced motion');
} finally {await browser.close();}
