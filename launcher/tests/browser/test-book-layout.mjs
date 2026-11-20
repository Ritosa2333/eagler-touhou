import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.argv[2] || process.env.LAUNCHER_URL || 'http://127.0.0.1:5173/';
const browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
try {
  const lobby = await browser.newPage();
  await lobby.setViewport({width: 1440, height: 900});
  await lobby.goto(new URL('lobby.html', origin), {waitUntil: 'domcontentloaded'});
  await lobby.waitForSelector('.book-directory');
  await lobby.$eval('.book-directory', directory => {
    // Match the directory's layout classes without joining a live room.
    const list = document.createElement('ul');
    list.className = 'lobby-room-list';
    list.innerHTML = ['join', 'spectate'].map(entry => `
      <li class="lobby-room-row directory-room-grid lobby-room-initial">
        <div class="lobby-room-identity">
          <span class="lobby-cover"></span>
          <div class="lobby-room-title">
            <h2>TH03</h2>
            <div class="lobby-room-meta"><span class="lobby-room-code">#TEST</span></div>
            <div class="lobby-room-facts">2/2</div>
          </div>
        </div>
        <div class="lobby-room-rules"><span>Normal</span><span>Standard</span></div>
        <div class="lobby-party">1P vs 2P</div>
        <button id="layout-${entry}" class="ui-control lobby-button lobby-join" data-entry="${entry}">${entry}</button>
      </li>`).join('');
    directory.append(list);
  });
  for (const entry of ['join', 'spectate']) {
    const offset = await lobby.$eval(`#layout-${entry}`, button => {
      const action = button.getBoundingClientRect(), row = button.parentElement.getBoundingClientRect();
      return action.top + action.height / 2 - (row.top + row.height / 2);
    });
    assert.ok(Math.abs(offset) < 1, `Desktop ${entry} button must center within the room row (offset: ${offset}px)`);
  }
  await lobby.setViewport({width: 390, height: 844});
  for (const entry of ['join', 'spectate']) {
    assert.equal(await lobby.$eval(`#layout-${entry}`, el => getComputedStyle(el).gridRowStart), '5', `Mobile ${entry} button must stay in the bottom row`);
  }
  await lobby.close();

  for (const [width, theme] of [[320, 'light'], [390, 'dark']]) {
    const page = await browser.newPage();
    await page.setViewport({width, height: 780, isMobile: true, hasTouch: true});
    await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
    await page.goto(origin, {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.__eaglerBoot?.done);
    await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
    const card = '#bookGameGrid [data-book-product="th01"]';
    await page.waitForSelector(card, {visible: true});
    await page.tap(card);
    const sheet = '.tools[data-book-sheet][data-state="open"]';
    const settings = `${sheet} [data-book-sheet-section="settings"]`;
    const toggle = `${settings} .book-sheet-toggle`;
    const fold = `${settings} .book-sheet-fold`;
    await page.waitForSelector(toggle, {visible: true});
    await page.waitForFunction(selector => {
      const node = document.querySelector(selector);
      return node && Math.abs(node.getBoundingClientRect().bottom - innerHeight) < 1;
    }, {}, sheet);
    assert.equal(await page.$eval(fold, el => el.inert), true);
    const closedBackground = await page.$eval(toggle, el => getComputedStyle(el).backgroundColor);
    await page.tap(toggle);
    await page.waitForFunction(selector => {
      const node = document.querySelector(selector);
      return node?.dataset.open === 'true' && !node.getAnimations().length && node.clientHeight > 0;
    }, {}, fold);
    const hierarchy = await page.$eval(settings, section => {
      const button = section.querySelector('.book-sheet-toggle');
      const detail = section.querySelector('.book-sheet-section-content');
      const main = button.getBoundingClientRect(), nested = detail.getBoundingClientRect();
      return {
        selectedBackground: getComputedStyle(button).backgroundColor,
        inset: Math.min(nested.left - main.left, main.right - nested.right),
        separateSurface: getComputedStyle(detail).backgroundColor !== getComputedStyle(section.closest('aside')).backgroundColor,
        fits: detail.scrollWidth <= detail.clientWidth + 1,
        actionsVisible: ['#launch', '#gamePackageImport'].every(selector => {
          const action = section.closest('aside').querySelector(selector);
          const bounds = action?.getBoundingClientRect();
          return bounds && bounds.width > 0 && bounds.top >= 0 && bounds.bottom <= innerHeight + 1 && !action.closest('[inert]');
        }),
      };
    });
    assert.notEqual(hierarchy.selectedBackground, closedBackground);
    assert.ok(hierarchy.inset >= 16 && hierarchy.separateSurface, 'Nested settings need a visibly separate inset surface');
    assert.ok(hierarchy.fits, 'Nested settings must fit without horizontal scrolling');
    assert.ok(hierarchy.actionsVisible, 'Start and Import must remain visible outside the fold');
    await page.tap(toggle);
    assert.equal(await page.$eval(fold, el => el.inert), true);
    await page.tap(`${sheet} #libraryBack`);
    await page.waitForSelector(sheet, {hidden: true});
    await page.close();
  }
  console.log('PASS: desktop room entry alignment, mobile room actions, and light/dark mobile settings hierarchy');
} finally {
  await browser.close();
}
