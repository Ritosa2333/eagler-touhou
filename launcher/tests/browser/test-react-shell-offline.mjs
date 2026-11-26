// Exercise the deployed Launcher shell with the browser's HTTP cache and network disabled.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir, writeFile} from 'node:fs/promises';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

if (!process.argv[2]) throw new Error('usage: node tests/browser/test-react-shell-offline.mjs URL');
const base = new URL(process.argv[2]);
const browser = await puppeteer.launch({
  executablePath: process.env.CHROME_PATH || await findChromiumExecutable(),
  headless: true,
  args: process.env.EAGLER_BROWSER_PROXY ? ['--proxy-server=' + process.env.EAGLER_BROWSER_PROXY] : [],
});
const book = '.launcher-page-surface[data-page="library"] #bookGameGrid .book-card';
const lobby = '.launcher-page-surface[data-page="lobby"] .lobby-main';
const routes = [
  ['', book],
  ['en.html', book],
  ['ja.html', book],
  ['about.html', '[data-help-page="about"]'],
  ['faq.html', '[data-help-page="faq"]'],
  ['lobby.html', lobby],
];
const worker = new URL('app-shell-sw.js', base).href;
const report = [];

try {
  for (const width of [1440, 390]) {
    const context = await browser.createBrowserContext();
    try {
      const page = await context.newPage();
      const errors = [], missingAssets = [];
      page.on('pageerror', error => errors.push(error.message));
      page.on('requestfailed', request => {
        if (new URL(request.url()).origin === base.origin &&
          ['script', 'stylesheet'].includes(request.resourceType())) missingAssets.push(request.url());
      });
      await page.setViewport({width, height:800, hasTouch:width < 780, isMobile:width < 780});
      await page.goto(base.href, {waitUntil:'domcontentloaded'});
      await page.waitForFunction(expected => navigator.serviceWorker.getRegistration()
        .then(registration => registration?.active?.scriptURL === expected), {timeout:120000}, worker);
      await page.reload({waitUntil:'domcontentloaded'});
      await page.waitForFunction(expected => navigator.serviceWorker.controller?.scriptURL === expected, {}, worker);
      await page.waitForSelector(book, {visible:true});
      await page.setCacheEnabled(false);
      await page.setOfflineMode(true);

      async function assertOffline(response, path, selector) {
        assert.ok(response?.ok() && response.fromServiceWorker(), path + ' did not load from the App Shell');
        await page.waitForSelector(selector, {visible:true});
        if (selector === book) {
          await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'));
        } else if (selector === lobby) {
          await page.waitForFunction(() => !document.documentElement.hasAttribute('data-lobby-boot') &&
            !document.querySelector('#lobbyPreload:not(.is-done)'));
        }
        const state = await page.evaluate(() => ({
          offline:!navigator.onLine,
          overflow:document.documentElement.scrollWidth > document.documentElement.clientWidth,
        }));
        assert.deepEqual(state, {offline:true, overflow:false}, path + ' offline layout');
        report.push({width, path});
      }

      for (const [path, selector] of routes) {
        const response = await page.goto(new URL(path, base).href, {waitUntil:'domcontentloaded'});
        await assertOffline(response, path || '/', selector).catch(error => {
          throw new Error(width + 'px offline request ' + (path || '/') + ' landed on ' + page.url(), {cause:error});
        });
      }
      await page.goto(base.href, {waitUntil:'domcontentloaded'});
      await assertOffline(await page.reload({waitUntil:'domcontentloaded'}), '/ (reload)', book);
      assert.deepEqual(errors, [], width + 'px offline errors');
      assert.deepEqual(missingAssets, [], width + 'px offline script and stylesheet requests');
      console.log('PASS ' + width + 'px: offline Launcher routes and reload');
    } finally {await context.close();}
  }
  await mkdir('.cache/react-shell-offline', {recursive:true});
  await writeFile('.cache/react-shell-offline/report.json', JSON.stringify({passed:true, worker, report}, null, 2));
} finally {await browser.close();}
