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
  const menuEntry = '#mastheadMenuPanel a[href*="section=installs"]';
  const railEntry = '.book-sidebar-footer a[href*="section=installs"]';
  const railProgress = `${railEntry} .book-sidebar-footer-progress`;
  const menuToggle = '#mastheadMenuToggle:not(:disabled)';
  const openDownloads = async width => {
    if (width === 390) {
      assert.equal(await page.$(railEntry), null, 'phones have no desktop sidebar entry');
      await page.locator(menuToggle).click();
      await page.locator(menuEntry).click();
    } else {
      await page.locator(railEntry).click();
    }
    await page.waitForSelector('.game-install-page .beauty-packs', {visible:true});
    await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
  };
  for (const width of [1440, 390]) {
    await page.setViewport({width, height:width === 390 ? 819 : 430, isMobile:width === 390, hasTouch:width === 390});
    await page.goto(new URL('?preview=install', origin).href, {waitUntil:'domcontentloaded'});
    await page.waitForSelector('#eaglerPreload', {hidden:true});
    const entryUrl = page.url();
    if (width === 1440) {
      assert.equal(await page.$(railProgress), null, 'the sidebar has no percent while idle');
      assert.equal(await page.$eval(railEntry, link => {
        const sidebar = link.closest('.book-sidebar');
        const list = sidebar.querySelector('nav');
        const rail = sidebar.getBoundingClientRect();
        const footer = link.parentElement.getBoundingClientRect();
        const before = link.getBoundingClientRect();
        list.scrollTop = list.scrollHeight;
        const after = link.getBoundingClientRect();
        const icon = link.querySelector('svg').getBoundingClientRect();
        return list.scrollTop > 0 && before.top === after.top && footer.bottom === rail.bottom &&
          footer.left === rail.left && Math.abs(footer.right - rail.right) <= 1 &&
          list.getBoundingClientRect().bottom <= footer.top && after.height >= 50 && icon.width === 26 &&
          getComputedStyle(link).fontSize === '14px';
      }), true, 'the downloads band fills only the bottom of the sidebar and stays put when categories scroll');
      await page.locator(menuToggle).click();
      await page.waitForSelector('#mastheadMenuPanel', {visible:true});
      assert.equal(await page.$(menuEntry), null, 'the library desktop menu does not duplicate the sidebar link');
      await page.keyboard.press('Escape');
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
    }
    await openDownloads(width);
    assert.equal(await page.$('.game-install-shortcut'), null, 'the floating shortcut stays hidden while idle');
    assert.ok(await page.$eval('.game-install-empty', node => node.offsetHeight < 60), 'idle install status occupies one row');
    await page.locator('.game-install-page .launcher-back').click();
    await page.waitForSelector('.game-install-page', {hidden:true});
    assert.equal(page.url(), entryUrl);
    await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
  }
  await page.setViewport({width:1440, height:819, isMobile:false, hasTouch:false});
  await page.goto(new URL('?game=th07&lang=zh-CN&preview=install',origin).href, {waitUntil:'domcontentloaded'});
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '安装游戏');
  const size = () => page.$eval('#launch', async node => {
    await document.fonts.ready;
    return [node.offsetWidth, node.offsetHeight];
  });
  const idleSize = await size();
  await page.locator('#launch').click();
  await page.waitForFunction(() => {
    const progress = document.querySelector('#launch .launch-action-progress[data-state="indeterminate"]');
    const fill = progress?.firstElementChild;
    return fill && getComputedStyle(fill).animationName === 'ui-progress-sweep' &&
      getComputedStyle(fill).backgroundImage.startsWith('linear-gradient');
  });
  await page.waitForSelector('#launch [aria-valuenow]');
  await page.waitForFunction(selector => {
    const percent = document.querySelector('#launch [aria-valuenow]')?.getAttribute('aria-valuenow');
    return Number(percent) > 0 && document.querySelector(selector)?.textContent === `${percent}%`;
  }, {}, railProgress);
  assert.deepEqual(await size(), idleSize);
  await page.waitForFunction(() => {
    const button = document.querySelector('#launch');
    const progress = button?.querySelector('.launch-action-progress[aria-valuenow]');
    const fill = progress?.firstElementChild;
    if (!progress || !fill) return false;
    const track = progress.getBoundingClientRect();
    const box = fill.getBoundingClientRect();
    return track.height >= button.clientHeight - 1 && track.width >= button.clientWidth - 1 &&
      Math.abs(box.height - track.height) < 1 && box.width > 0 && box.width < track.width &&
      getComputedStyle(fill).backgroundColor !== getComputedStyle(progress).backgroundColor;
  });
  await page.locator('.game-install-shortcut').click();
  await page.waitForFunction(() => document.querySelector('[data-install-speed]')?.textContent === '5 MiB/s');
  await page.waitForSelector('[data-install-eta]');
  await page.waitForSelector('.game-install-empty', {timeout:30000});
  await page.locator('.game-install-page .launcher-back').click();
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '开始游戏');
  assert.equal(await page.$(railProgress), null, 'the sidebar clears completed progress');
  assert.equal(await page.$eval('#launch', button => button.disabled), true, 'simulated installs do not enable actual play');
  await page.locator('#installPreviewAgain').click();
  await page.waitForSelector('#launch [aria-valuenow]');
  await openDownloads(1440);
  await page.locator('[data-download-active="th07"] button').click();
  await page.waitForSelector('.game-install-empty');
  await page.locator('.game-install-page .launcher-back').click();
  await page.waitForFunction(() => document.querySelector('#launchText')?.textContent === '安装游戏');
  assert.equal(await page.$(railProgress), null, 'the sidebar clears cancelled progress');
  assert.deepEqual(resources, []);
  console.log('PASS: sidebar and mobile entries, synchronized sidebar/button progress, repeat, cancel, and no game-resource requests');
} finally {
  await browser.close();
}
