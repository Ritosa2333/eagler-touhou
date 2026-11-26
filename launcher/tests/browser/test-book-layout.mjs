import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.argv[2] || process.env.LAUNCHER_URL || 'http://127.0.0.1:5173/';
const pc98Cards = '#bookGameGrid :is([data-book-product="th01"],[data-book-product="th05"])';

async function assertMasthead(page, width) {
  await page.waitForSelector('#mastheadMenuToggle:not(:disabled)', {visible:true});
  const layout = await page.$eval('.book-masthead', header => {
    const box = node => node.getBoundingClientRect();
    const visible = selector => [...header.querySelectorAll(selector)].filter(node => box(node).width > 0);
    const actions = visible('.masthead-action');
    const icons = actions.filter(node => ![...node.children].some(child =>
      child.tagName === 'SPAN' && child.classList.length === 0 && box(child).width > 0));
    const navigation = visible('.book-main-nav a');
    const groups = visible('.masthead-leading, .book-main-nav, .masthead-trailing, .masthead-library-search')
      .map(box).sort((left, right) => left.left - right.left);
    const active = header.querySelector('.book-main-nav a[aria-current="page"]');
    const headerBounds = box(header);
    const navBounds = box(header.querySelector('.book-main-nav'));
    const menu = header.querySelector('#mastheadMenuToggle');
    const center = node => box(node).top + box(node).height / 2;
    return {
      height: headerBounds.height,
      controls: actions.map(node => [box(node).height, getComputedStyle(node).borderTopWidth]),
      icons: icons.map(node => box(node).width),
      centers: [...actions, ...navigation].map(center),
      nav: navigation.map(node => box(node).width),
      navGap: navigation.length && box(navigation[1]).left - box(navigation[0]).right,
      underline: active && getComputedStyle(active, '::after').backgroundColor,
      foreground: active && getComputedStyle(active).color,
      underlineWidths: navigation.map(node=>parseFloat(getComputedStyle(node, '::after').width)),
      menuGroup: menu.closest('.masthead-leading, .masthead-trailing').className,
      menuLast: menu === menu.parentElement.lastElementChild,
      gutters: [groups[0].left - headerBounds.left, headerBounds.right - groups.at(-1).right],
      navOffset: (navBounds.left + navBounds.right - headerBounds.left - headerBounds.right) / 2,
      activeFill: navigation.length ? getComputedStyle(active).backgroundColor : null,
      fits: groups.every((bounds,index) => bounds.left >= headerBounds.left - 1 &&
        bounds.right <= headerBounds.right + 1 && (index === 0 || groups[index-1].right <= bounds.left + 1)),
    };
  });
  assert.ok(layout.height >= (width <= 780 ? 56 : 60), `${width}px masthead has room around controls`);
  assert.ok(layout.controls.length >= 3);
  assert.ok(layout.controls.every(([height,border]) => height === 44 && border === '0px'), `${width}px masthead actions share an unframed 44px height`);
  assert.ok(layout.icons.every(size => size === 44), `${width}px icon actions are square`);
  assert.ok(Math.max(...layout.centers) - Math.min(...layout.centers) <= 1, `${width}px masthead controls share a centerline`);
  assert.ok(layout.fits, `${width}px masthead groups fit without overlap`);
  assert.equal(layout.menuGroup, width <= 780 ? 'masthead-leading' : 'masthead-trailing');
  if (layout.nav.length) {
    assert.ok(layout.menuLast, 'More is the rightmost desktop action');
    assert.equal(layout.navGap, 0, 'the navigation indicators can meet edge to edge');
    assert.equal(layout.underline, layout.foreground, 'the underline uses the theme foreground instead of the accent');
    assert.ok(layout.underlineWidths.every((width, index) => Math.abs(width - layout.nav[index]) < 1));
    assert.ok(layout.gutters.every(gap => gap >= 12 && gap <= 32), `${width}px outer groups sit by the viewport edges`);
    assert.equal(new Set(layout.nav).size, 1, 'Desktop mode links have the same width');
    assert.equal(layout.activeFill, 'rgba(0, 0, 0, 0)', 'The current mode uses an underline without a filled button');
    if (width >= 1440) {
      assert.ok(Math.abs(layout.navOffset) < 1, 'Desktop mode links are centered independently of side groups');
      assert.ok(layout.nav.every(width => width >= 108), 'Desktop mode links have room around icon and label');
    }
  }
}

async function assertInboxMotion(page) {
  const action = section => `.masthead-inbox-action[data-inbox-section="${section}"]`;
  const announcements = action('announcements');
  const messages = action('messages');
  const animation = (selector, pseudo = null) => page.$eval(selector,
    (node, pseudo) => getComputedStyle(node, pseudo).animationName, pseudo);
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  await page.hover(announcements);
  assert.equal(await animation(`${announcements} > svg`), 'masthead-megaphone-call');
  assert.equal(await animation(announcements, '::before'), 'masthead-megaphone-wave');
  await page.hover(messages);
  assert.equal(await animation(`${messages} > svg`), 'masthead-mail-lift');
  assert.equal(await animation(`${messages} > svg > path`), 'masthead-mail-flap');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  assert.equal(await animation(`${messages} > svg`), 'none');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  const hadLessMotion = await page.evaluate(() => document.body.classList.contains('less-motion'));
  await page.evaluate(() => document.body.classList.add('less-motion'));
  await page.hover(announcements);
  assert.equal(await animation(`${announcements} > svg`), 'none');
  assert.equal(await animation(announcements, '::before'), 'none');
  await page.evaluate(hadLessMotion => document.body.classList.toggle('less-motion', hadLessMotion), hadLessMotion);
  await page.mouse.move(0, 0);
}

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

  const narrowDesktop = await browser.newPage();
  await narrowDesktop.setViewport({width:781, height:760});
  await narrowDesktop.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await narrowDesktop.goto(new URL('en.html', origin), {waitUntil:'domcontentloaded'});
  await narrowDesktop.waitForFunction(() => window.__eaglerBoot?.done);
  await assertMasthead(narrowDesktop, 781);
  await narrowDesktop.close();

  for (const [width, theme] of [[320, 'light'], [390, 'dark']]) {
    const page = await browser.newPage();
    await page.setViewport({width, height: 780, isMobile: true, hasTouch: true});
    await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
    await page.goto(origin, {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.__eaglerBoot?.done);
    await assertMasthead(page, width);
    await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
    assert.match(await page.$eval('meta[name="viewport"]', node => node.content), /viewport-fit=contain/);
    if (width === 390) {
      const chromeBounds = () => page.evaluate(() => {
        const header = document.querySelector('.book-masthead');
        const nav = document.querySelector('.launcher-bottom-navigation');
        const surface = document.querySelector('.launcher-page-surface');
        return [header.offsetHeight, nav.offsetHeight, getComputedStyle(surface).paddingBottom];
      });
      const baseline = await chromeBounds();
      const cdp = await page.createCDPSession();
      await cdp.send('Emulation.setSafeAreaInsetsOverride', {insets: {top: 40, bottom: 16}});
      assert.deepEqual(await chromeBounds(), baseline, 'Forwarded Android system bars must not enlarge the browser chrome');
      await cdp.send('Emulation.setSafeAreaInsetsOverride', {insets: {top: 0, bottom: 0}});
      await cdp.detach();
    }
    const card = await page.waitForSelector(pc98Cards, {visible: true});
    await card.tap();
    const detail = '.tools[data-book-sheet][data-state="open"]';
    const settings = '.mobile-game-settings';
    const settingsControl = '#gameSettingsOpen';
    const back = detail + ' #libraryBack';
    await page.waitForSelector(settingsControl, {visible: true});
    assert.equal(await page.$('.book-sidebar'), null, 'Mobile uses category tabs instead of the desktop sidebar');
    await page.waitForFunction(selector => {
      const node = document.querySelector(selector);
      if (!node) return false;
      const rect = node.getBoundingClientRect();
      return rect.top >= Math.min(innerHeight * .18, 120) - 1 && Math.abs(rect.bottom - innerHeight) < 1 && !node.getAnimations().length;
    }, {}, detail);
    assert.equal(await page.$eval('#libraryBackdrop[data-book-sheet]', node => getComputedStyle(node).backgroundColor), 'rgba(0, 0, 0, 0.52)');
    assert.equal(await page.$eval(detail, node => getComputedStyle(node).borderTopLeftRadius), '20px');
    const artwork = await page.$eval(detail + ' .game-detail-backdrop', node => {
      const style = getComputedStyle(node);
      return {mask: style.maskImage, filter: style.filter};
    });
    assert.match(artwork.mask, /linear-gradient/, 'Mobile detail uses the faded artwork');
    assert.doesNotMatch(artwork.filter, /blur/);
    const naturalSheet = await page.$eval(detail, panel => {
      const scroll = panel.querySelector('.mobile-game-scroll');
      const footer = panel.querySelector('.mobile-game-footer');
      return {height: panel.offsetHeight, limit: parseFloat(getComputedStyle(panel).maxHeight),
        content: scroll.scrollHeight + footer.offsetHeight + panel.offsetHeight - panel.clientHeight};
    });
    assert.ok(Math.abs(naturalSheet.height - Math.min(naturalSheet.content, naturalSheet.limit)) <= 2,
      'The sheet hugs short details and caps long ones');
    const actions = await page.$eval(detail, panel => ['#launch', '#gamePackageImport', '#gameSettingsOpen'].map(selector => {
      const button = panel.querySelector(selector);
      const {left, right, top, bottom, height} = button.getBoundingClientRect();
      return left >= 0 && right <= innerWidth + 1 && top >= 0 && bottom <= innerHeight + 1 && height >= 44 && !button.closest('[inert]');
    }));
    assert.deepEqual(actions, [true, true, true], 'Detail actions fit with comfortable touch targets');
    assert.equal(await page.$eval(detail, panel => {
      const scroll = panel.querySelector('.mobile-game-scroll');
      const footer = panel.querySelector('.mobile-game-footer');
      const actionsTop = footer.getBoundingClientRect().top;
      scroll.scrollTop = scroll.scrollHeight;
      const fixed = footer.getBoundingClientRect().top === actionsTop && !scroll.contains(footer);
      scroll.scrollTop = 0;
      return fixed;
    }), true, 'Detail actions stay docked below the scroll region');
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
    await card.tap();
    await page.waitForSelector(detail, {visible: true});
    await page.touchscreen.tap(width / 2, 12);
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
  await assertMasthead(resize, 1440);
  await resize.setViewport({width: 1707, height: 819});
  await assertMasthead(resize, 1707);
  await assertInboxMotion(resize);
  await resize.setViewport({width: 1440, height: 540});
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
  const mobileDetail = '.tools[data-book-sheet][data-state="open"]';
  const categoryToggle = '[data-book-category-disclosure="pc98"]';
  const categoryPanel = '#bookCategory-pc98';
  assert.equal(await resize.$('[data-book-category-group="all"] .book-category-disclosure, #bookCategory-all'), null);
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
      backdrop: getComputedStyle(node.querySelector('.game-detail-backdrop')).maskImage,
      actionHeight: [launch.height, importButton.height, settings.height],
      actionRow: [launch.top, importButton.top, settings.top],
      actionOrder: launch.right <= importButton.left && importButton.right <= settings.left,
    };
  });
  assert.equal(desktopLayout.flush, true, 'Desktop hero reaches the sidebar and the viewport edge');
  assert.match(desktopLayout.backdrop, /linear-gradient/, 'Desktop artwork fades in from the right');
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
  const name = `${sample} #mpRoomName`;
  const save = `${sample} div:has(> #mpRoomName) + button`;
  const nameStatus = `${save} + [role="status"]`;
  await handbook.waitForSelector(name, {visible: true});
  await handbook.$eval(name, input => input.focus());
  await handbook.keyboard.sendCharacter('初见练习');
  await handbook.click(save);
  await handbook.waitForFunction(selector => document.querySelector(selector)?.textContent === '房间名已通过审核', {}, nameStatus);
  assert.equal(await handbook.$eval(name, input => input.value), '初见练习');
  await handbook.$eval(name, input => {input.focus(); input.select();});
  await handbook.keyboard.press('Backspace');
  await handbook.click(save);
  await handbook.waitForFunction(selector => document.querySelector(selector)?.textContent === '房间名已保存', {}, nameStatus);
  await handbook.close();
  console.log('PASS: masthead control sizes, room action alignment, shared disclosure, desktop/mobile detail handoff, and room settings specimen');
} finally {
  await browser.close();
}
