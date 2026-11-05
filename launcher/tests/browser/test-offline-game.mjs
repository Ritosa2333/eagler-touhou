// Installs through the actual Launcher, then reloads and launches with the
// network and HTTP cache disabled. A native first-frame message is required.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir, writeFile} from 'node:fs/promises';
const base = process.argv[2], game = process.argv[3] || 'th06';
if (!base || !['th06', 'th07'].includes(game)) throw new Error('usage: node tests/browser/test-offline-game.mjs URL [th06|th07]');
const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true,
  args: ['--autoplay-policy=no-user-gesture-required', ...(process.env.EAGLER_BROWSER_PROXY ? ['--proxy-server=' + process.env.EAGLER_BROWSER_PROXY] : [])]});
try {
  const page = await browser.newPage();
  await page.goto(base, {waitUntil: 'networkidle2'});
  await page.waitForFunction(async () => (await navigator.serviceWorker.getRegistration())?.active?.state === 'activated', {timeout: 120000});
  await page.reload({waitUntil: 'networkidle2'});
  async function launch() {
    await page.waitForFunction(() => document.querySelectorAll('.game').length > 0);
    await new Promise(resolve => setTimeout(resolve, 1000));
    await page.evaluate(game => {
      document.querySelector('#firstUseNoticeClose')?.click();
      window.__offlineFirstFrame = false;
      addEventListener('message', event => {
        if (event.data?.protocol === 'eagler-touhou/1' && event.data.game === game && event.data.event === 'first-frame') window.__offlineFirstFrame = true;
      });
    }, game);
    await new Promise(resolve => setTimeout(resolve, 250));
    const card = `.game[data-game="${game}"]:not([data-product])`;
    await page.click(card);
    await new Promise(resolve => setTimeout(resolve, 600));
    if (!await page.evaluate(() => document.body.classList.contains('library-tools-open'))) await page.click(card);
    await page.waitForFunction(() => document.body.classList.contains('library-tools-open') && !document.body.classList.contains('library-tools-closing'));
    await new Promise(resolve => setTimeout(resolve, 600));
    await page.select('#musicSelect', 'none');
    await page.select('#languageSelect', 'ja');
    await page.click('#launch');
    await page.waitForFunction(() => [...document.querySelectorAll('dialog[open] button')].some(button => /仍要启动|Start anyway/.test(button.textContent)));
    await page.evaluate(() => [...document.querySelectorAll('dialog[open] button')].find(button => /仍要启动|Start anyway/.test(button.textContent)).click());
    await page.waitForFunction(() => window.__offlineFirstFrame === true, {timeout: 150000});
    const frame = await page.$eval('#gameFrame', node => node.src);
    assert.match(frame, /\/runtime\/[^/]+\/[a-f0-9]{64}\//);
    assert.equal(new URL(frame).searchParams.get('managedData'), '1');
    return frame;
  }
  await launch();
  await page.reload({waitUntil: 'networkidle2'});
  await page.setCacheEnabled(false);
  await page.setOfflineMode(true);
  await page.reload({waitUntil: 'load'});
  const frame = await launch();
  assert.equal(await page.evaluate(() => navigator.onLine), false);
  const report = {passed: true, game, offline: true, firstFrame: true, runtime: frame};
  await mkdir('.cache/offline-game-evidence', {recursive: true});
  await writeFile(`.cache/offline-game-evidence/${game}.json`, JSON.stringify(report, null, 2));
  console.log(JSON.stringify(report));
} finally {await browser.close();}
