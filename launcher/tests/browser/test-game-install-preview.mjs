import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5185/');
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  const resources = [];
  page.on('request', request => {
    if (/\.(?:package\.json|data|wasm|bin)(?:$|\?)/.test(request.url())) resources.push(request.url());
  });
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await page.goto(new URL('?game=th07&lang=zh-CN&preview=install',origin).href, {waitUntil:'domcontentloaded'});
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '安装游戏');
  await page.locator('#launch').click();
  await page.waitForSelector('#launch [aria-valuenow]');
  await page.locator('.game-install-shortcut').click();
  await page.waitForFunction(() => document.querySelector('[data-install-speed]')?.textContent === '5 MiB/s');
  await page.waitForSelector('[data-install-eta]');
  await page.waitForSelector('.game-install-empty', {timeout:30000});
  await page.locator('.game-install-page .launcher-back').click();
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '开始游戏');
  assert.equal(await page.$eval('#launch', button => button.disabled), true, 'simulated installs do not enable actual play');
  await page.locator('#installPreviewAgain').click();
  await page.waitForSelector('#launch [aria-valuenow]');
  await page.locator('.game-install-shortcut').click();
  await page.locator('[data-download-active="th07"] button').click();
  await page.waitForSelector('.game-install-empty');
  await page.locator('.game-install-page .launcher-back').click();
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '安装游戏');
  assert.deepEqual(resources, []);
  console.log('PASS: local-only simulated install, same progress views, completion, repeat, cancel, and no game-resource requests');
} finally {
  await browser.close();
}
