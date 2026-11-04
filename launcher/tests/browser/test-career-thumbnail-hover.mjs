import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage();
  await page.setViewport({width: 1440, height: 1000});
  await page.goto(new URL('profile.html', process.argv[2] || 'http://127.0.0.1:5173/'), {waitUntil: 'networkidle2'});
  await page.addStyleTag({content: '.site-notice{display:none!important}'});
  const settle = () => page.evaluate(() => new Promise(resolve => setTimeout(resolve, 450)));
  const style = () => page.$eval('.career-thumbnail', el => {
    const s = getComputedStyle(el);
    return {border: s.borderColor, shadow: s.boxShadow, outline: s.outlineStyle};
  });
  for (const tab of ['概览', '生涯']) {
    await page.evaluate(tab => [...document.querySelectorAll('.profile-tabs button')].find(el => el.textContent === tab).click(), tab);
    await page.waitForSelector('.career-thumbnail');
    for (const theme of ['dark', 'light']) {
      await page.evaluate(theme => document.documentElement.dataset.bookTheme = theme, theme);
      await page.mouse.move(0, 0); await settle();
      const before = await style();
      await page.hover('.career-thumbnail'); await settle();
      assert.deepEqual(await style(), before, `${tab}/${theme}: hover must not add an accent frame`);
      // Force the keyboard-focus modality after pointer interaction.
      await page.$eval('.career-thumbnail', el => el.focus({focusVisible: true}));
      assert.ok(await page.$eval('.career-thumbnail', el => {
        const s = getComputedStyle(el);
        const probe = document.createElement('span');
        probe.style.color = 'color-mix(in srgb,var(--book-muted) 65%,transparent)';
        el.append(probe);
        const neutral = getComputedStyle(probe).color;
        probe.remove();
        return el === document.activeElement && el.matches(':focus-visible') &&
          s.outlineStyle === 'solid' && parseFloat(s.outlineWidth) > 0 &&
          s.outlineColor === neutral;
      }), `${tab}/${theme}: retain a neutral keyboard focus indicator`);
      await page.$eval('.career-thumbnail', el => el.blur());
    }
  }
  console.log('PASS: overview and career thumbnails retain neutral borders on hover and visible keyboard focus in both themes');
} finally {await browser.close();}
