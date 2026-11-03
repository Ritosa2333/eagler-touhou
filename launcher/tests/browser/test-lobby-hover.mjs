import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage();
  await page.setViewport({width: 1440, height: 1000});
  await page.goto(new URL('lobby.html', process.argv[2] || 'http://127.0.0.1:5173/'), {waitUntil: 'networkidle2'});
  await page.addStyleTag({content: '.site-notice{display:none!important}'});
  // Local style fixture: no live room or relay action is invoked.
  await page.$eval('.book-directory', el => {
    const fixture = document.createElement('section');
    fixture.innerHTML = '<div class="lobby-room-row lobby-room-initial"><button id="hoverJoin" class="ui-control lobby-button lobby-join" data-ui-variant="primary">加入</button><button id="hoverDisabled" class="ui-control lobby-button lobby-primary" data-ui-variant="primary" disabled>不可用</button></div>';
    el.append(fixture);
  });
  const settle = () => page.evaluate(() => new Promise(resolve => setTimeout(resolve, 180)));
  const style = selector => page.$eval(selector, el => {
    const s = getComputedStyle(el), r = el.getBoundingClientRect();
    return {background: s.backgroundColor, color: s.color, transform: s.transform, width: r.width, height: r.height, pressed: el.getAttribute('aria-pressed')};
  });
  for (const theme of ['dark', 'light']) {
    await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
    await page.mouse.move(0, 0); await settle(); await settle();
    for (const selector of ['[data-room-filter="open"]', '.book-room-view-toggle button[aria-pressed="false"]', '#watchReplayButton', '.book-lobby-game', '.book-lobby-favorite', '#hoverJoin']) {
      await page.mouse.move(0, 0); await settle();
      const before = await style(selector);
      await page.hover(selector); await settle();
      const after = await style(selector);
      assert.notEqual(after.background, before.background, `${theme}/${selector}: visible hover`);
      assert.equal(after.width, before.width); assert.equal(after.height, before.height);
      assert.equal(after.pressed, before.pressed); assert.equal(after.transform, 'none');
    }
    const selected = '[data-room-filter="all"]';
    await page.mouse.move(0, 0); await settle(); const before = await style(selected);
    await page.hover(selected); await settle(); const after = await style(selected);
    assert.equal(after.background, before.background); assert.equal(after.color, before.color);
    await page.mouse.move(0, 0); await settle(); const disabled = await style('#hoverDisabled');
    await page.hover('#hoverDisabled'); await settle(); assert.deepEqual(await style('#hoverDisabled'), disabled);
    await page.hover('#hoverJoin'); await page.mouse.down(); await settle();
    assert.equal((await style('#hoverJoin')).transform, 'none', 'No legacy scale on press');
    await page.mouse.up();
  }
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  assert.ok(await page.$eval('[data-room-filter="open"]', el => parseFloat(getComputedStyle(el).transitionDuration) < .00001));
  await page.setViewport({width: 390, height: 844, isMobile: true, hasTouch: true});
  await page.waitForSelector('[data-room-filter="open"]');
  await page.click('[data-room-filter="open"]');
  assert.equal(await page.$eval('[data-room-filter="open"]', el => el.getAttribute('aria-pressed')), 'true');
  assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
  console.log('PASS: lobby hover in both themes, selected/disabled states, fixed geometry, no scale, reduced motion and narrow filtering');
} finally {await browser.close();}
