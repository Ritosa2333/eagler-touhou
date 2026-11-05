// Real browser navigation with network disabled. This checks the Launcher,
// not whether any game package has been installed for offline gameplay.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir, writeFile} from 'node:fs/promises';
if (!process.argv[2]) throw new Error('usage: node tests/browser/test-react-shell-offline.mjs URL');
const base = new URL(process.argv[2]);
const browser = await puppeteer.launch({
  executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe',
  headless: true,
  args: process.env.EAGLER_BROWSER_PROXY ? ['--proxy-server=' + process.env.EAGLER_BROWSER_PROXY] : [],
});
const report = [];
try {
  const page = await browser.newPage();
  await page.goto(base.href, {waitUntil: 'networkidle2'});
  await page.waitForFunction(async () => (await navigator.serviceWorker.getRegistration())?.active?.state === 'activated', {timeout: 120000});
  const worker = await page.evaluate(async () => (await navigator.serviceWorker.getRegistration()).active.scriptURL);
  assert.equal(worker, new URL('app-shell-sw.js', base).href);
  await page.setCacheEnabled(false); // HTTP cache cannot mask missing SW entries.
  await page.setOfflineMode(true);
  for (const path of ['', 'en.html', 'about.html', 'faq.html', 'lobby.html']) {
    console.log('Checking offline: ' + (path || '/'));
    const errors = [];
    const error = value => errors.push(value.message);
    page.on('pageerror', error);
    await page.goto(new URL(path, base).href, {waitUntil: 'load'});
    assert.equal(await page.evaluate(() => navigator.onLine), false);
    if (path === 'lobby.html') {
      await page.waitForSelector('.lobby-main');
      await page.waitForFunction(() => !document.documentElement.hasAttribute('data-lobby-boot') && !document.querySelector('#lobbyPreload:not(.is-done)'), {timeout: 15000});
      // Offline directory displays its unavailable state; it cannot fetch rooms.
    } else if (['', 'en.html'].includes(path)) {
      await page.waitForFunction(() => document.querySelectorAll('.game').length > 0 &&
        !document.documentElement.hasAttribute('data-lobby-boot') &&
        !document.querySelector(document.documentElement.dataset.originalEntry === 'lobby' ? '#lobbyPreload:not(.is-done)' : '#eaglerPreload:not(.is-done)'), {timeout: 15000}).catch(async error => {
          console.log(await page.evaluate(() => ({url: location.href, cards: document.querySelectorAll('.game').length, boot: document.documentElement.dataset, text: document.body.innerText.slice(-2500)})));
          throw error;
        });
    } else await page.waitForSelector('.about-page');
    assert.deepEqual(errors, [], path + ' offline errors');
    report.push({path: path || '/', offline: true, rendered: true});
    page.off('pageerror', error);
  }
  await page.goto(base.href, {waitUntil: 'load'});
  await page.reload({waitUntil: 'load'});
  await page.waitForFunction(() => document.querySelectorAll('.game').length > 0);
  report.push({path: '/', offline: true, reload: true});
  await mkdir('.cache/react-shell-offline', {recursive: true});
  await writeFile('.cache/react-shell-offline/report.json', JSON.stringify({passed: true, worker, report}, null, 2));
  console.log(JSON.stringify({passed: true, report}));
} finally {await browser.close();}
