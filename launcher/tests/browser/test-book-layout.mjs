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
    assert.equal(await page.$eval('.book-sidebar .book-category-fold', el => getComputedStyle(el).display), 'none', 'Mobile keeps the category list collapsed');
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
      const start = section.closest('aside').querySelector('#launch').getBoundingClientRect();
      const importButton = section.closest('aside').querySelector('#gamePackageImport').getBoundingClientRect();
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
        actionWidthDifference: Math.abs(start.width - importButton.width),
        actionOrder: start.bottom <= importButton.top,
      };
    });
    assert.notEqual(hierarchy.selectedBackground, closedBackground);
    assert.ok(hierarchy.inset >= 16 && hierarchy.separateSurface, 'Nested settings need a visibly separate inset surface');
    assert.ok(hierarchy.fits, 'Nested settings must fit without horizontal scrolling');
    assert.ok(hierarchy.actionsVisible, 'Start and Import must remain visible outside the fold');
    assert.ok(hierarchy.actionWidthDifference < 1, 'Mobile Start and Import buttons must have equal width');
    assert.ok(hierarchy.actionOrder, 'Mobile Start must be above Import');
    await page.tap(toggle);
    assert.equal(await page.$eval(fold, el => el.inert), true);
    await page.waitForFunction(selector => document.querySelector(selector)?.clientHeight === 0, {}, fold);
    await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
    await page.tap(toggle);
    assert.deepEqual(await page.$eval(fold, el => [el.inert, el.getAnimations().length]), [false, 0]);
    await page.tap(`${sheet} #libraryBack`);
    await page.waitForSelector(sheet, {hidden: true});
    await page.close();
  }

  const resize = await browser.newPage();
  await resize.setViewport({width: 1440, height: 540});
  await resize.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await resize.goto(origin, {waitUntil: 'domcontentloaded'});
  await resize.waitForFunction(() => window.__eaglerBoot?.done);
  const card = '#bookGameGrid [data-book-product="th01"]';
  const detail = '[data-game-detail="th01"]';
  const sheet = '.tools[data-book-sheet][data-state="open"]';
  await resize.waitForSelector(card, {visible: true});
  const categoryToggle = '[data-book-category-disclosure="pc98"]';
  const categoryPanel = '#bookCategory-pc98';
  await resize.focus(categoryToggle);
  await resize.keyboard.press('Enter');
  await resize.waitForFunction(selector => {
    const node = document.querySelector(selector);
    return node?.dataset.open === 'true' && node.clientHeight > 0 && !node.getAnimations().length;
  }, {}, categoryPanel);
  assert.equal(await resize.$eval(categoryToggle, el => el.getAttribute('aria-controls')), categoryPanel.slice(1));
  assert.equal(await resize.$eval(categoryPanel, el => el.inert), false);
  await resize.keyboard.press('Space');
  await resize.waitForFunction(selector => {
    const node = document.querySelector(selector);
    return node?.inert && node.clientHeight === 0 && getComputedStyle(node).visibility === 'hidden';
  }, {}, categoryPanel);
  await resize.click(card);
  await resize.waitForSelector(`${detail} #launch`, {visible: true});
  const desktopLayout = await resize.$eval(detail, node => {
    const bounds = selector => node.querySelector(selector).getBoundingClientRect();
    return {
      statistics: bounds('.book-game-detail-statistics').width,
      settings: bounds('.book-game-detail-settings').width,
      launch: bounds('#launch').width,
      import: bounds('#gamePackageImport').width,
      actionOrder: bounds('#launch').bottom <= bounds('#gamePackageImport').top,
    };
  });
  assert.ok(desktopLayout.statistics > desktopLayout.settings, 'Desktop gives the statistics column more space');
  assert.ok(Math.abs(desktopLayout.launch - desktopLayout.import) < 1, 'Desktop Start and Import buttons must have equal width');
  assert.ok(desktopLayout.actionOrder, 'Desktop Start must be above Import');
  await resize.waitForSelector(`${detail} .profile-clear-chart .profile-clear-shot [data-status="unread"]`, {visible: true});
  const selectedUrl = resize.url(), historyLength = await resize.evaluate(() => history.length);
  await resize.$eval(`${detail} .book-game-detail-settings`, node => node.scrollIntoView({block:'end'}));
  for (const width of [390, 1440, 390, 1440, 390]) {
    await resize.setViewport({width, height: 540});
    const compact = width === 390;
    const surface = compact ? sheet : detail;
    await resize.waitForSelector(`${surface} #launch`, {visible: true});
    const state = await resize.$eval(surface, node => {
      const rect = node.getBoundingClientRect();
      return {
        bottomOffset: Math.abs(rect.bottom - innerHeight),
        animations: [...node.getAnimations(), ...node.parentElement.getAnimations()].map(animation => ({
          id: animation.id, frames: animation.effect.getKeyframes(),
        })),
        launchCount: document.querySelectorAll('#launch').length,
        grid: !!document.querySelector('#bookGameGrid'),
        libraryInert: document.querySelector('.game-library').inert,
        focus: document.activeElement.id,
        historyLength: history.length,
      };
    });
    assert.deepEqual(state.animations, [], `${width}px resize must not start a navigation animation`);
    if (compact) assert.ok(state.bottomOffset <= 1, 'The first mobile sheet frame is already pinned to the viewport');
    assert.equal(state.launchCount, 1, 'Only the active surface owns the game controls');
    assert.equal(state.grid, compact);
    assert.equal(state.libraryInert, compact);
    assert.equal(state.focus, compact ? 'libraryBack' : 'gameTitle');
    assert.equal(state.historyLength, historyLength);
    assert.equal(resize.url(), selectedUrl);
  }
  await resize.goBack();
  await resize.waitForFunction(() => !new URLSearchParams(location.search).has('game'));
  await resize.setViewport({width: 1440, height: 540});
  await resize.waitForSelector(card, {visible: true});
  assert.equal(await resize.evaluate(() =>
    document.querySelector('.game-library').inert ||
    document.body.classList.contains('library-tools-closing') ||
    !!document.querySelector('[data-game-detail]')), false, 'Resize must retire an exiting mobile sheet');
  await resize.close();

  const handbook = await browser.newPage();
  await handbook.setViewport({width: 1280, height: 800});
  await handbook.goto(new URL('ui-review.html#page-room-settings', origin), {waitUntil: 'domcontentloaded'});
  const sample = '#page-room-settings #mpRoomSettings';
  await handbook.waitForSelector(`${sample} #mpRoomVisibility-trigger`, {visible: true});
  assert.equal(await handbook.$eval(sample, el => !!el.querySelector('#mpRoomName')), false);
  await handbook.close();
  console.log('PASS: room action alignment, shared disclosure, desktop/mobile detail handoff, and room settings specimen');
} finally {
  await browser.close();
}
