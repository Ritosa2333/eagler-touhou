import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.argv[2] || process.env.LAUNCHER_URL || 'http://127.0.0.1:5173/';
const pc98Cards = '#bookGameGrid :is([data-book-product="th01"],[data-book-product="th05"])';
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
    const card = await page.waitForSelector(pc98Cards, {visible: true});
    await card.tap();
    const detail = '.tools[data-full-page][data-state="open"]';
    const settings = '.mobile-game-settings';
    const settingsControl = '#gameSettingsOpen';
    const back = detail + ' #libraryBack';
    await page.waitForSelector(settingsControl, {visible: true});
    assert.equal(await page.$('.book-sidebar'), null, 'Mobile uses category tabs instead of the desktop sidebar');
    await page.waitForFunction(selector => {
      const node = document.querySelector(selector);
      if (!node) return false;
      const rect = node.getBoundingClientRect();
      return Math.abs(rect.top) < 1 && Math.abs(rect.bottom - innerHeight) < 1;
    }, {}, detail);
    const actions = await page.$eval(detail, panel => ['#launch', '#gamePackageImport', '#gameSettingsOpen'].map(selector => {
      const button = panel.querySelector(selector);
      const {left, right, top, bottom, height} = button.getBoundingClientRect();
      return left >= 0 && right <= innerWidth + 1 && top >= 0 && bottom <= innerHeight + 1 && height >= 44 && !button.closest('[inert]');
    }));
    assert.deepEqual(actions, [true, true, true], 'Detail actions fit with comfortable touch targets');
    await page.tap(settingsControl);
    await page.waitForSelector(settings, {visible: true});
    assert.equal(await page.$eval(settings, node => node.scrollWidth <= node.clientWidth + 1), true);
    assert.equal(await page.$eval(detail + ' #launch', button => Boolean(button.closest('[hidden][inert]'))), true);
    await page.tap(back);
    await page.waitForSelector(settingsControl, {visible: true});
    await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
    await page.tap(settingsControl);
    await page.waitForSelector(settings, {visible: true});
    assert.equal(await page.$eval('.mobile-game-pages', node => node.getAnimations().length), 0);
    await page.goBack();
    await page.waitForSelector(settings, {hidden: true});
    await page.waitForSelector(settingsControl, {visible: true});
    await page.tap(back);
    await page.waitForSelector(detail, {hidden: true});
    await page.close();
  }

  const resize = await browser.newPage();
  await resize.setViewport({width: 1440, height: 540});
  await resize.evaluateOnNewDocument(() => {
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
    localStorage.setItem('eagler.book.recent', JSON.stringify(['th01','th05']));
  });
  await resize.goto(origin, {waitUntil: 'domcontentloaded'});
  await resize.waitForFunction(() => window.__eaglerBoot?.done);
  await resize.waitForSelector('#bookRecentTitle', {visible:true});
  assert.equal(await resize.evaluate(() => {
    const recent = document.querySelector('#bookRecentTitle'), all = document.querySelector('.book-current h2');
    return recent.getBoundingClientRect().left === all.getBoundingClientRect().left &&
      getComputedStyle(recent).font === getComputedStyle(all).font && !recent.querySelector('svg');
  }),true,'shelf headings use the same typography and left edge');
  const publishedCard = await resize.waitForSelector(pc98Cards, {visible: true});
  const product = await publishedCard.evaluate(link => link.dataset.bookProduct);
  const card = `#bookGameGrid [data-book-product="${product}"]`;
  const detail = `[data-game-detail="${product}"]`;
  const mobileDetail = '.tools[data-full-page][data-state="open"]';
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
  await resize.waitForFunction(selector => !document.querySelector(selector).getAnimations().length, {}, detail);
  const desktopLayout = await resize.$eval(detail, node => {
    const bounds = selector => node.querySelector(selector).getBoundingClientRect();
    const hero = bounds('.book-game-detail-hero');
    const content = node.closest('.book-content').getBoundingClientRect();
    const launch = bounds('#launch'), importButton = bounds('#gamePackageImport'), settings = bounds('#gameSettingsOpen');
    return {
      flush: hero.left === content.left && hero.right === content.right,
      actionHeight: [launch.height, importButton.height, settings.height],
      actionRow: [launch.top, importButton.top, settings.top],
      actionOrder: launch.right <= importButton.left && importButton.right <= settings.left,
    };
  });
  assert.equal(desktopLayout.flush, true, 'Desktop hero reaches the sidebar and the viewport edge');
  assert.equal(new Set(desktopLayout.actionHeight).size, 1, 'Desktop buttons have the same height');
  assert.equal(new Set(desktopLayout.actionRow).size, 1, 'Desktop actions share one row');
  assert.ok(desktopLayout.actionOrder, 'Desktop Start is left of Import and Settings');
  await resize.waitForSelector(`${detail} .clear-matrix[data-loadouts="true"] tbody [data-status="unread"]`, {visible: true});
  const selectedUrl = resize.url(), historyLength = await resize.evaluate(() => history.length);
  await resize.click('#gameSettingsOpen');
  await resize.waitForSelector('.book-game-detail-settings', {visible: true});
  await resize.setViewport({width: 390, height: 540});
  await resize.waitForSelector('.mobile-game-settings', {visible: true});
  await resize.setViewport({width: 1440, height: 540});
  await resize.waitForSelector('.book-game-detail-settings', {visible: true});
  await resize.goBack();
  await resize.waitForSelector(`${detail} #gameSettingsOpen`, {visible: true});
  for (const width of [390, 1440, 390, 1440, 390]) {
    await resize.setViewport({width, height: 540});
    const compact = width === 390;
    const surface = compact ? mobileDetail : detail;
    await resize.waitForSelector(`${surface} #launch`, {visible: true});
    const state = await resize.$eval(surface, node => {
      const rect = node.getBoundingClientRect();
      return {
        bottomOffset: Math.abs(rect.bottom - innerHeight),
        animations: [...node.getAnimations(), ...node.parentElement.getAnimations()].map(animation => ({
          id: animation.id, frames: animation.effect.getKeyframes(),
        })),
        launchCount: document.querySelectorAll('#launch').length,
        grid: !!document.querySelector('#bookGameGrid')?.getClientRects().length,
        libraryInert: document.querySelector('.game-library').inert,
        focus: document.activeElement.id,
        historyLength: history.length,
      };
    });
    assert.deepEqual(state.animations, [], `${width}px resize must not start a navigation animation`);
    if (compact) assert.ok(state.bottomOffset <= 1, 'The first mobile detail frame is already pinned to the viewport');
    assert.equal(state.launchCount, 1, 'Only the active surface owns the game controls');
    assert.equal(state.grid, compact);
    assert.equal(state.libraryInert, compact);
    assert.equal(state.focus, compact ? 'libraryBack' : 'gameTitle');
    assert.equal(state.historyLength, historyLength + 1, 'Only opening Settings adds a history entry');
    assert.equal(resize.url(), selectedUrl);
  }
  await resize.goBack();
  await resize.waitForFunction(() => !new URLSearchParams(location.search).has('game'));
  await resize.setViewport({width: 1440, height: 540});
  await resize.waitForSelector('.book-sidebar', {visible: true});
  await resize.waitForSelector(card, {visible: true});
  assert.equal(await resize.evaluate(() =>
    document.querySelector('.game-library').inert ||
    document.body.classList.contains('library-tools-closing') ||
    !!document.querySelector('[data-game-detail]')), false, 'Resize must retire an exiting mobile detail');
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
