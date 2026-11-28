import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.argv[2] || process.env.LAUNCHER_URL || 'http://127.0.0.1:5173/';
const pc98Cards = '#bookGameGrid :is([data-book-product="th01"],[data-book-product="th05"])';

async function assertSettingsSections(page, selector) {
  await page.waitForSelector(selector, {visible: true});
  const sections = await page.$$eval(`${selector} .options-settings > .options-group`, groups => groups.map(group => [
    group.querySelector(':scope > h3')?.tagName,
    Boolean(group.querySelector(':scope > .options-group-body')?.getClientRects().length),
    Boolean(group.querySelector(':scope > button[aria-expanded]')),
  ]));
  assert.equal(sections.length, 6);
  assert.ok(sections.every(([heading, visible, collapsible]) => heading === 'H3' && visible && !collapsible),
    'the dedicated settings page renders every group as a visible section');
}

async function assertMasthead(page, width) {
  await page.waitForSelector('#mastheadMenuToggle:not(:disabled)', {visible:true});
  const layout = await page.$eval('.book-masthead', header => {
    const box = node => node.getBoundingClientRect();
    const visible = selector => [...header.querySelectorAll(selector)].filter(node => box(node).width > 0);
    const actions = visible('.masthead-action');
    const icons = actions.filter(node => ![...node.querySelectorAll(':scope > .masthead-action-label, :scope.masthead-account-entry > span:not(.account-avatar)')]
      .some(child => box(child).width > 0));
    const navigation = visible('.book-main-nav a');
    const groups = visible('.masthead-leading, .book-main-nav, .masthead-trailing, .masthead-library-search')
      .map(box).sort((left, right) => left.left - right.left);
    const active = header.querySelector('.book-main-nav a[aria-current="page"]');
    const headerBounds = box(header);
    const navBounds = box(header.querySelector('.book-main-nav'));
    const menu = header.querySelector('#mastheadMenuToggle');
    const feedback = header.querySelector('.masthead-leading .book-top-feedback');
    const trailing = header.querySelector('.masthead-trailing');
    const account = trailing.querySelector('.masthead-account-entry, .account-login-link, .account-loading, .book-profile-trigger');
    const login = trailing.querySelector('.account-login-link');
    const wordmark = box(header.querySelector('.launcher-wordmark'));
    const center = node => box(node).top + box(node).height / 2;
    const glyphs = visible('.masthead-action:not(.masthead-account-entry) > svg')
      .map(node => [parseFloat(getComputedStyle(node).width), parseFloat(getComputedStyle(node).height), center(node)]);
    const navGlyphs = visible('.book-main-nav a > svg')
      .map(node => [parseFloat(getComputedStyle(node).width), parseFloat(getComputedStyle(node).height), center(node)]);
    return {
      height: headerBounds.height,
      controls: actions.map(node => [box(node).height, getComputedStyle(node).borderTopWidth]),
      icons: icons.map(node => box(node).width),
      labels: visible('.masthead-action-label').map(label => label.textContent),
      labelOffsets: visible('.masthead-action-label, .book-main-nav a > span').flatMap(label => {
        const glyph = label.parentElement.querySelector(':scope > svg');
        return glyph && box(glyph).width ? [center(label) - center(glyph)] : [];
      }),
      centers: [...actions, ...navigation].map(center),
      nav: navigation.map(node => box(node).width),
      navHeight: navBounds.height,
      navBottom: navBounds.bottom - headerBounds.top,
      navGap: navigation.length && box(navigation[1]).left - box(navigation[0]).right,
      underline: active && getComputedStyle(active, '::after').backgroundColor,
      foreground: active && getComputedStyle(active).color,
      underlineWidths: navigation.map(node=>parseFloat(getComputedStyle(node, '::after').width)),
      underlineBottoms: navigation.map(node=>getComputedStyle(node, '::after').bottom),
      menuGroup: menu.closest('.masthead-leading, .masthead-trailing').className,
      menuLast: menu === menu.parentElement.lastElementChild,
      menuIconOnly: [box(menu).width, menu.textContent.trim(), menu.getAttribute('aria-label')],
      feedback: [box(feedback).width > 0, feedback.href, feedback.target, feedback.previousElementSibling.matches('.book-top-faq')],
      donation: visible('.book-top-donation').length > 0,
      accountLast: !account || account === trailing.lastElementChild,
      login: login && [login.dataset.size, login.dataset.variant,
        login.querySelector('svg').classList.contains('lucide-user-round')],
      glyphs, navGlyphs,
      strokeWidths: visible('.masthead-action > svg').map(svg => getComputedStyle(svg).strokeWidth),
      navFonts:navigation.map(node => parseFloat(getComputedStyle(node).fontSize)),
      wordmarkWidth: wordmark.width,
      // Center of the l in the 480×140 source image.
      wordmarkGlyphCenter: wordmark.top + wordmark.height * 80.5 / 140,
      gutters: [groups[0].left - headerBounds.left, headerBounds.right - groups.at(-1).right],
      navOffset: (navBounds.left + navBounds.right - headerBounds.left - headerBounds.right) / 2,
      navCenter: center(header.querySelector('.book-main-nav')),
      activeFill: navigation.length ? getComputedStyle(active).backgroundColor : null,
      fits: groups.every((bounds,index) => bounds.left >= headerBounds.left - 1 &&
        bounds.right <= headerBounds.right + 1 && (index === 0 || groups[index-1].right <= bounds.left + 1)),
    };
  });
  assert.equal(layout.height, width <= 780 ? 56 : 44, `${width}px masthead retains the expected height`);
  assert.ok(layout.controls.length >= 3);
  assert.ok(layout.controls.every(([height,border]) => height === (width <= 780 ? 44 : 43) && border === '0px'),
    `${width}px masthead actions fit in the header row without borders`);
  assert.ok(layout.icons.every(size => size === 44), `${width}px icon actions have a common width`);
  assert.ok(width < 1440 ? layout.labels.length === 0 : layout.labels.length >= 5 && layout.labels.every(Boolean),
    `${width}px masthead shows action labels only on wide screens`);
  assert.ok(layout.labelOffsets.every(offset => Math.abs(offset) < .5),
    `${width}px masthead labels and icons share their optical alignment`);
  assert.ok(Math.max(...layout.centers) - Math.min(...layout.centers) <= 1, `${width}px masthead controls share a centerline`);
  assert.ok(layout.fits, `${width}px masthead groups fit without overlap`);
  assert.equal(layout.menuGroup, 'masthead-leading');
  assert.deepEqual(layout.menuIconOnly.slice(0,2), [44,'']);
  assert.ok(layout.menuIconOnly[2]);
  assert.deepEqual(layout.feedback, [width > 780, 'https://feedback.touhouonline.com/', '_blank', true]);
  assert.equal(layout.donation, width > 1024, 'the donation action uses the menu on narrower screens');
  if (layout.nav.length) {
    assert.deepEqual([layout.navHeight, layout.navBottom], [43,43], 'primary navigation fills the desktop row');
    assert.ok(layout.menuLast, 'More sits after the left-hand desktop actions');
    assert.ok(layout.accountLast, 'The account sits after announcements and messages');
    if (layout.login) assert.deepEqual(layout.login, ['icon', 'ghost', true], 'The sign-in link matches the other masthead actions');
    assert.equal(new Set(layout.strokeWidths).size, 1, 'Masthead actions use the same line weight');
    const expectedSize = layout.wordmarkWidth * .165;
    assert.ok(layout.glyphs.length >= 6);
    assert.ok(layout.glyphs.every(([w, h, center]) => Math.abs(w - expectedSize) < .25 &&
      Math.abs(h - expectedSize) < .25 && Math.abs(center - layout.wordmarkGlyphCenter) < .5),
    'Desktop glyphs share the scale and optical center of the wordmark ascender');
    if (width > 960) {
      const navSize = layout.wordmarkWidth * .205;
      assert.deepEqual(layout.navFonts,[21,21]);
      assert.equal(layout.navGlyphs.length,2);
      assert.ok(layout.navGlyphs.every(([w,h,center]) => Math.abs(w-navSize)<.25 && Math.abs(h-navSize)<.25 &&
        Math.abs(center-layout.navCenter)<.5), 'The primary modes share larger icons centered vertically in the row');
    }
    assert.equal(layout.navGap, 0, 'the navigation indicators can meet edge to edge');
    assert.equal(layout.underline, layout.foreground, 'the underline uses the theme foreground instead of the accent');
    assert.ok(layout.underlineWidths.every((width, index) => Math.abs(width - layout.nav[index]) < 1));
    assert.deepEqual(layout.underlineBottoms, ['0px','0px'], 'indicators end inside the desktop row');
    assert.ok(layout.gutters.every(gap => gap >= 12 && gap <= 32), `${width}px outer groups sit by the viewport edges`);
    assert.equal(new Set(layout.nav).size, 1, 'Desktop mode links have the same width');
    assert.equal(layout.activeFill, 'rgba(0, 0, 0, 0)', 'The current mode uses an underline without a filled button');
    if (width >= 1440) {
      assert.ok(Math.abs(layout.navOffset) < 1, 'Desktop mode links are centered independently of side groups');
      assert.ok(layout.nav.every(width => width >= 108), 'Desktop mode links have room around icon and label');
    }
  }
}

async function assertModeHover(page) {
  const inactive = '.book-main-nav a:not([aria-current="page"])';
  const active = '.book-main-nav a[aria-current="page"]';
  const indicator = node => {
    const line = getComputedStyle(node, '::after');
    return {
      scale: new DOMMatrixReadOnly(line.transform).a,
      origin: parseFloat(line.transformOrigin),
      transition: line.transitionProperty,
      width: node.offsetWidth,
    };
  };
  const initial = await page.$eval(inactive, indicator);
  assert.equal(initial.scale, 0, 'inactive underline starts collapsed');
  assert.equal(initial.origin, initial.width / 2, 'underline grows from the center');
  assert.equal(initial.transition, 'transform');
  await page.hover(inactive);
  await page.waitForFunction(selector => {
    const width = new DOMMatrixReadOnly(getComputedStyle(document.querySelector(selector), '::after').transform).a;
    return width > 0 && width < 1;
  }, {}, inactive);
  assert.equal((await page.$eval(active, indicator)).scale, 1, 'the active tab stays marked while the other is hovered');
  await page.waitForFunction(selector => new DOMMatrixReadOnly(getComputedStyle(document.querySelector(selector), '::after').transform).a === 1, {}, inactive);
  assert.equal((await page.$eval(inactive, indicator)).width, initial.width, 'hover does not resize the tab');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  await page.mouse.move(1, 100);
  assert.equal((await page.$eval(inactive, indicator)).scale, 0);
  assert.equal((await page.$eval(inactive, indicator)).transition, 'none');
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
}

async function assertActionMotion(page) {
  const action = section => `.masthead-inbox-action[data-inbox-section="${section}"]`;
  const announcements = action('announcements');
  const messages = action('messages');
  const theme = '.book-theme-toggle';
  const assertNoHoverFill = async trigger => {
    const paint = await page.$eval(trigger, node => {
      getComputedStyle(node).backgroundColor;
      node.getAnimations().find(animation => animation.transitionProperty === 'background-color')?.finish();
      return [node.matches(':hover'), getComputedStyle(node).backgroundColor, getComputedStyle(node).boxShadow];
    });
    assert.deepEqual(paint, [true,'rgba(0, 0, 0, 0)','none'], `${trigger} stays frameless on hover`);
  };
  const redesigned = [
    [announcements, `${announcements} > svg`, 'e', 0, 1.4],
    [theme, `${theme} > svg`, 'a', 1, 1.04],
  ];
  const progress = (selector, component, samples) => page.$eval(selector, (node, component, samples) => {
    getComputedStyle(node).transform;
    const motion = node.getAnimations().find(item => item instanceof CSSTransition && item.transitionProperty === 'transform');
    const {duration} = motion.effect.getTiming();
    const values = samples.map(fraction => {
      motion.currentTime = duration * fraction;
      return new DOMMatrixReadOnly(getComputedStyle(node).transform)[component];
    });
    return {duration, values, opacity: getComputedStyle(node).opacity};
  }, component, samples);
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  for (const [trigger, glyph, component, rest, pose] of [
    ...redesigned,
    [messages, `${messages} > svg`, 'f', 0, -1.3],
    ['.book-top-faq', '.book-faq-question', 'f', 0, -1.25],
    ['.book-top-donation', '.book-top-donation > svg', 'a', 1, 1.07],
  ]) {
    await page.hover(trigger);
    const enter = await progress(glyph, component, [0, .12, .5, .88, 1]);
    await assertNoHoverFill(trigger);
    assert.equal(enter.duration, 520);
    assert.equal(enter.opacity, '1');
    const [start, early, mid, late, end] = enter.values.map(value => (value-rest)/(pose-rest));
    assert.ok(Math.abs(start) < .001 && Math.abs(end-1) < .001);
    assert.ok(early > 0 && early < .08 && mid > .4 && mid < .6 && late > .92 && late < 1,
      'the icon eases in and out without a jump or overshoot');
    assert.equal(await page.$eval(glyph, node => node.getAnimations().length), 0, 'the icon holds its pose without repeating');
    await page.mouse.move(1, 100);
    const exit = await progress(glyph, component, [0, .5, 1]);
    assert.equal(exit.duration, enter.duration, 'the return uses the same pacing');
    assert.ok(Math.abs(exit.values[0] - pose) < .001);
    assert.ok(Math.abs(exit.values[1] - rest) < Math.abs(pose - rest));
    assert.ok(Math.abs(exit.values[2] - rest) < .001);
  }
  for (const trigger of ['.book-top-feedback', '#mastheadMenuToggle']) {
    await page.hover(trigger);
    await assertNoHoverFill(trigger);
  }
  await page.mouse.move(1, 100);
  assert.equal(await page.$eval(announcements, node => getComputedStyle(node, '::before').content), 'none');
  await page.keyboard.press('Tab');
  for (const [trigger, glyph, component, rest, pose] of redesigned) {
    await page.focus(trigger);
    assert.ok(await page.$eval(trigger, node => node.matches(':focus-visible')));
    const focus = await progress(glyph, component, [.5, 1]);
    assert.ok(Math.abs(focus.values[0] - (rest + pose) / 2) < .001);
    assert.ok(Math.abs(focus.values[1] - pose) < .001);
    await page.$eval(trigger, node => node.blur());
  }
  const motionStyles = selector => page.$eval(`${selector} > svg`, node => {
    const {transform, transitionProperty, animationName} = getComputedStyle(node);
    return [transform, transitionProperty, animationName];
  });
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  await page.hover(announcements);
  assert.deepEqual(await motionStyles(announcements), ['none', 'none', 'none']);
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  const hadLessMotion = await page.evaluate(() => document.body.classList.contains('less-motion'));
  await page.evaluate(() => document.body.classList.add('less-motion'));
  await page.hover(theme);
  assert.deepEqual(await motionStyles(theme), ['none', 'none', 'none']);
  await page.evaluate(hadLessMotion => document.body.classList.toggle('less-motion', hadLessMotion), hadLessMotion);
  await page.mouse.move(0, 0);
}

const browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
try {
  const lobby = await browser.newPage();
  await lobby.setViewport({width: 1440, height: 900});
  await lobby.goto(new URL('lobby.html', origin), {waitUntil: 'domcontentloaded'});
  await lobby.waitForSelector('.book-directory');
  assert.equal(await lobby.$eval('.book-masthead', node => node.getBoundingClientRect().height), 44, 'the lobby shares the desktop masthead height');
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

  const ratings = await browser.newPage();
  await ratings.setViewport({width:1707, height:819});
  await ratings.goto(new URL('ratings.html?lang=zh-CN', origin), {waitUntil:'domcontentloaded'});
  assert.equal(await ratings.$eval('.book-masthead', node => node.getBoundingClientRect().height), 44, 'ratings match the production masthead height');
  await ratings.close();

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
    const customBackground = width === 390;
    await page.evaluateOnNewDocument(customBackground => {
      localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
      if (customBackground) localStorage.setItem('eagler.background.settings', JSON.stringify({mode:'dots'}));
    }, customBackground);
    await page.goto(origin, {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.__eaglerBoot?.done);
    await assertMasthead(page, width);
    await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
    assert.match(await page.$eval('meta[name="viewport"]', node => node.content), /viewport-fit=contain/);
    if (width === 390) {
      await page.locator('#mastheadMenuToggle').click();
      const feedback = await page.waitForSelector('#mastheadMenuPanel a[href="https://feedback.touhouonline.com/"]', {visible:true});
      assert.equal(await feedback.evaluate(link => link.target), '_blank');
      await page.keyboard.press('Escape');
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
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
    if (customBackground) {
      await page.waitForSelector('html[data-background-mode="dots"][data-custom-background]');
    }
    const corners = await card.evaluate(link => [link.closest('.book-card'), link.querySelector('.book-art')].map(node => {
      const {borderRadius, overflow} = getComputedStyle(node);
      return [borderRadius, overflow];
    }));
    assert.deepEqual(corners, [['6px', 'hidden'], ['6px', 'hidden']],
      `${width}px cover and card surface share rounded clipping with ${customBackground ? 'a custom' : 'the default'} background`);
    const titleSpacing = await card.evaluate(link => {
      const card = link.closest('.book-card');
      const title = link.querySelector('.book-title-line');
      const favorite = card.querySelector('.book-favorite');
      return {
        inset: title.getBoundingClientRect().left - card.getBoundingClientRect().left,
        fits: title.getBoundingClientRect().right <= favorite.getBoundingClientRect().left,
        favoriteWidth: favorite.getBoundingClientRect().width,
      };
    });
    assert.deepEqual(titleSpacing, {inset:10, fits:true, favoriteWidth:44}, 'mobile cover titles have breathing room without shrinking the favorite target');
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
    await assertSettingsSections(page, settings);
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
  for (const width of [2560, 1440, 1707]) {
    await resize.setViewport({width, height: 819});
    await assertMasthead(resize, width);
    const coverGrid = await resize.$$eval('#bookGameGrid .book-card .book-art', covers => {
      const boxes = covers.map(cover => cover.getBoundingClientRect());
      return {columns:boxes.filter(box => box.top === boxes[0].top).length, coverWidth:boxes[0].width,
        square:boxes.every(({width,height}) => Math.abs(height - width) < 1)};
    });
    assert.ok(coverGrid.square, `covers stay square at ${width}px`);
    assert.equal(coverGrid.columns, 6, `${width}px cover grid column count`);
    console.log(`Cover grid at ${width}px: ${coverGrid.columns} covers per row, ${Math.round(coverGrid.coverWidth)}px per cover`);
  }
  await assertModeHover(resize);
  await assertActionMotion(resize);
  for (const width of [1439, 1280, 1024]) {
    await resize.setViewport({width, height: 819});
    await assertMasthead(resize, width);
  }
  await resize.setViewport({width: 1440, height: 540});
  await resize.waitForSelector('#bookRecentTitle', {visible:true});
  assert.equal(await resize.evaluate(() => {
    const recent = document.querySelector('#bookRecentTitle'), all = document.querySelector('.book-current h2');
    return recent.getBoundingClientRect().left === all.getBoundingClientRect().left &&
      getComputedStyle(recent).font === getComputedStyle(all).font && !recent.querySelector('svg');
  }),true,'shelf headings use the same typography and left edge');
  assert.deepEqual(await resize.evaluate(() => {
    const recent = document.querySelector('.book-recent-link');
    const cover = document.querySelector('.book-card');
    const root = document.documentElement;
    const context = document.createElement('canvas').getContext('2d');
    const backgroundOf = node => {
      context.clearRect(0, 0, 1, 1);
      context.fillStyle = getComputedStyle(node).backgroundColor;
      context.fillRect(0, 0, 1, 1);
      return Array.from(context.getImageData(0, 0, 1, 1).data);
    };
    const defaultBackground = backgroundOf(recent);
    const update = settings => {
      localStorage.setItem('eagler.background.settings', JSON.stringify(settings));
      window.dispatchEvent(new Event('eagler-background-settings-change'));
    };
    recent.style.transition = 'none';
    update({mode:'dots', card:68});
    const adjusted = backgroundOf(recent), matches = adjusted.join() === backgroundOf(cover).join();
    const hadBackground = root.hasAttribute('data-custom-background');
    update({mode:'plain', card:31});
    const plain = backgroundOf(recent);
    localStorage.removeItem('eagler.background.settings');
    window.dispatchEvent(new Event('eagler-background-settings-change'));
    recent.style.removeProperty('transition');
    return [defaultBackground[3], adjusted[3], plain[3], matches, hadBackground];
  }),[224,173,79,true,true],'recent cards use the theme card surface with or without a custom background');
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
  await assertSettingsSections(resize, '.book-game-detail-settings');
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
