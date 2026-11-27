import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage();
  await page.setViewport({width: 1440, height: 1000});
  await page.goto(new URL('lobby.html', process.argv[2] || 'http://127.0.0.1:5173/'), {waitUntil: 'networkidle2'});
  // Local style fixture: no live room or relay action is invoked.
  await page.$eval('.book-directory', el => {
    const fixture = document.createElement('section');
    fixture.innerHTML = '<div class="lobby-room-row lobby-room-initial"><button id="hoverJoin" class="ui-button ui-control lobby-button lobby-join" data-ui-variant="primary" data-entry="join">加入</button></div>';
    el.append(fixture);
  });
  const settle = () => page.evaluate(async () => {
    await new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve)));
    await Promise.allSettled(document.getAnimations()
      .filter(animation => animation.effect.getTiming().iterations !== Infinity)
      .map(animation => animation.finished));
  });
  const style = selector => page.$eval(selector, el => {
    const s = getComputedStyle(el.matches('.book-category-choice, [data-lobby-rail-product] > .book-rail-game') ? el.parentElement : el), r = el.getBoundingClientRect();
    const cover = el.querySelector('.book-lobby-cover');
    const railCover = el.querySelector('.book-category-game-cover');
    const offset = node => node ? node.getBoundingClientRect().x - r.x : null;
    const canvas = document.createElement('canvas');
    canvas.width = canvas.height = 1;
    const context = canvas.getContext('2d');
    const rgba = value => {
      context.clearRect(0,0,1,1);
      context.fillStyle = value;
      context.fillRect(0,0,1,1);
      return context.getImageData(0,0,1,1).data.join(',');
    };
    return {background: rgba(s.backgroundColor), color: rgba(s.color), transform: s.transform, width: r.width, height: r.height,
      pressed: el.getAttribute('aria-pressed'), coverScale: cover && getComputedStyle(cover).scale,
      railCover: offset(railCover),
      railMarker: railCover && offset(el.parentElement.querySelector('.book-lobby-sidebar-check')),
      railFavorite: railCover && offset(el.parentElement.querySelector('.book-favorite'))};
  });
  const toolbarFills = {};
  for (const theme of ['dark', 'light']) {
    await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
    await page.mouse.move(0, 0); await settle(); await settle();
    const toolbar = await page.$$eval('.lobby-filter-tools [data-slot="button"]', buttons => buttons.map(button => {
      const css = getComputedStyle(button), rect = button.getBoundingClientRect();
      const token = document.createElement('span');
      button.append(token);
      token.style.backgroundColor = 'var(--kit-secondary)';
      const fill = getComputedStyle(token).backgroundColor;
      token.remove();
      return {variant: button.dataset.variant, filled: css.backgroundColor === fill, background: css.backgroundColor, width: rect.width, height: rect.height};
    }));
    assert.ok(toolbar.length >= 3);
    assert.ok(toolbar.every(control => control.variant === 'toolbar' && control.filled && control.height === 44));
    assert.ok(toolbar.slice(1).every(control => control.width === 44));
    toolbarFills[theme] = toolbar[0].background;
    for (const selector of ['.book-room-filter-trigger', '#lobbyGuideOpen', '#lobbyNetworkCheck', '.lobby-game-sidebar [data-book-category="original"]', '#refreshRooms', '.lobby-game-sidebar .book-rail-game', '#hoverJoin']) {
      await page.mouse.move(0, 0); await settle();
      const before = await style(selector);
      await page.hover(selector); await settle();
      const after = await style(selector);
      assert.ok(after.background !== before.background || after.coverScale !== before.coverScale, `${theme}/${selector}: visible hover`);
      assert.equal(after.width, before.width); assert.equal(after.height, before.height);
      assert.equal(after.pressed, before.pressed); assert.equal(after.transform, 'none');
      if (before.railCover !== null) {
        assert.equal(after.railCover - before.railCover, 3, 'the artwork follows the shared list hover');
        assert.equal(after.railMarker, before.railMarker, 'the separate selection target stays fixed');
        assert.equal(after.railFavorite, before.railFavorite, 'the favorite remains at the row edge');
      }
    }
    const selected = '.lobby-game-sidebar [data-selected="true"]';
    await page.mouse.move(0, 0); await settle(); const before = await style(selected);
    await page.hover(selected); await settle(); const after = await style(selected);
    assert.equal(after.background, before.background); assert.equal(after.color, before.color);
    if (await page.$eval('#createButton', button => button.disabled)) {
      await page.mouse.move(0, 0); await settle(); const disabled = await style('#createButton');
      await page.hover('#createButton'); await settle(); assert.deepEqual(await style('#createButton'), disabled);
    }
    await page.hover('#hoverJoin'); await page.mouse.down(); await settle();
    assert.equal((await style('#hoverJoin')).transform, 'none', 'No legacy scale on press');
    await page.mouse.up();
  }
  assert.notEqual(toolbarFills.dark, toolbarFills.light);
  const openFilters = async (columns) => {
    await page.click('.book-room-filter-trigger');
    const popup = await page.waitForSelector('.book-room-filter-popup[data-state="open"]', {visible:true});
    await popup.evaluate(element => Promise.allSettled(element.getAnimations().map(animation => animation.finished)));
    assert.equal(await popup.$eval('.book-room-filter-groups', el => getComputedStyle(el).gridTemplateColumns.split(' ').length), columns);
    assert.ok(await popup.evaluate(el => el.getBoundingClientRect().left >= 0 && el.getBoundingClientRect().right <= innerWidth));
  };
  await openFilters(3);
  await page.keyboard.press('Escape');
  await page.waitForSelector('.book-room-filter-popup', {hidden:true});
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  const row = '.lobby-game-sidebar .book-rail-game';
  await page.mouse.move(0, 0); await settle();
  const reduced = await style(row);
  await page.hover(row); await settle();
  assert.equal((await style(row)).railCover, reduced.railCover, 'reduced motion disables horizontal hover movement');
  await page.setViewport({width: 390, height: 844, isMobile: true, hasTouch: true});
  await page.waitForSelector('.book-lobby-picker-trigger', {visible:true});
  assert.ok(await page.$eval('[data-lobby-icon] .book-lobby-cover', el => parseFloat(getComputedStyle(el).transitionDuration) < .00001));
  const filterTrigger = '.book-room-filter-trigger';
  await page.mouse.move(0, 0); await settle();
  const idleFilter = await style(filterTrigger);
  await openFilters(2);
  await settle();
  assert.notEqual((await style(filterTrigger)).background, idleFilter.background, 'an open filter is highlighted');
  await page.waitForSelector('[data-room-filter="open"]', {visible:true});
  await page.click('[data-room-filter="open"]');
  assert.equal(await page.$eval(filterTrigger, el => el.dataset.active), 'true');
  await page.mouse.move(0, 0); await settle();
  assert.notEqual((await style(filterTrigger)).background, idleFilter.background, 'an applied filter stays highlighted after closing');
  assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
  for (const width of [320, 390, 600, 850, 1440]) {
    await page.setViewport({width, height: 844});
    await page.waitForSelector('.book-room-filter-trigger', {visible: true});
    const toolbar = await page.$eval('.lobby-room-tools', node => {
      const bounds = selector => node.querySelector(selector).getBoundingClientRect();
      const tools = bounds('.lobby-actions'), filter = bounds('.book-room-filter-trigger');
      const more = bounds('.lobby-tools-more'), support = bounds('.lobby-support-links');
      const buttons = ['#createButton', '#codeButton', '#refreshRooms', '.lobby-tools-more'].map(bounds);
      return {
        compact: more.width > 0, support: support.width > 0,
        fits: tools.right <= node.getBoundingClientRect().right + 1,
        touchTargets: buttons.every(rect => rect.width === 44 && rect.height === 44),
        aligned: filter.top === tools.top || filter.left === tools.left,
        label: bounds('.book-room-filter-trigger > span').width > 0,
      };
    });
    if ([320, 390, 850, 1440].includes(width)) {
      assert.equal(toolbar.compact, width !== 1440, `${width}px uses the room column width`);
    }
    assert.equal(toolbar.support, !toolbar.compact);
    assert.ok(toolbar.fits && toolbar.label && toolbar.aligned);
    if (toolbar.compact) assert.ok(toolbar.touchTargets);
  }
  await page.setViewport({width: 390, height: 844});
  await page.waitForSelector('.lobby-tools-more', {visible: true});
  await page.focus('.lobby-tools-more');
  await page.keyboard.press('Enter');
  const more = await page.waitForSelector('.lobby-tools-menu[data-state="open"]', {visible: true});
  assert.ok(await more.$('a[href*="/docs/multiplayer"]'), 'the overflow retains the guide link');
  assert.ok(await more.$('.lucide-activity'), 'the overflow includes network diagnostics');
  assert.ok(await more.$('.lucide-clapperboard'), 'the overflow includes replays');
  await page.keyboard.press('Escape');
  assert.equal(await page.evaluate(() => document.activeElement.matches('.lobby-tools-more')), true);
  console.log('PASS: lobby hover in both themes, selected/disabled states, fixed geometry, no scale, reduced motion and narrow filtering');
} finally {await browser.close();}
