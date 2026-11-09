import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
import puppeteer from 'puppeteer-core';

const image = fileURLToPath(new URL('../../public/assets/favicon.png', import.meta.url));
const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage();
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
  await page.goto(new URL('lobby.html', process.argv[2] || 'http://127.0.0.1:5173/'), {waitUntil:'networkidle2'});
  await page.click('.book-theme-toggle');
  await page.waitForSelector('.custom-background-dialog');
  const desktop = await page.evaluate(() => {
    const panel = document.querySelector('.custom-background-dialog').getBoundingClientRect();
    const overlay = getComputedStyle(document.querySelector('.custom-background-overlay'));
    return {right: innerWidth - panel.right, left: panel.left, overlay: overlay.backgroundColor};
  });
  assert.equal(desktop.overlay, 'rgba(0, 0, 0, 0)', 'preview is not dimmed by the dialog');
  assert.ok(desktop.right <= 40 && desktop.left > desktop.right, 'desktop settings panel sits at the right edge');
  await page.click('.custom-theme-switch [data-theme-value="light"]');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'light');
  await page.click('.custom-theme-switch [data-theme-value="dark"]');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'dark');
  await (await page.$('.custom-background-dialog input[type=file]')).uploadFile(image);
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  const imageUrl = await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-background-image'));
  for (const mode of ['dots', 'grid']) {
    await page.click(`.background-preset[data-preset="${mode}"]`);
    assert.equal(await page.evaluate(() => document.documentElement.dataset.backgroundMode), mode);
    assert.equal(await page.$eval(`.background-preset[data-preset="${mode}"]`, node => node.getAttribute('aria-pressed')), 'true');
    assert.match(await page.evaluate(() => getComputedStyle(document.body).backgroundImage), /gradient/);
  }
  await page.click('.background-preset[data-preset="plain"]');
  assert.equal(await page.evaluate(() => document.documentElement.hasAttribute('data-custom-background')), false);
  await page.click('.background-preset[data-preset="image"]');
  assert.equal(await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-background-image')), imageUrl, 'presets preserve the selected image');
  await page.evaluate(() => {
    const setter = Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, 'value').set;
    for (const slider of document.querySelectorAll('.custom-background-dialog input[type=range]')) {
      for (let value = 10; value <= 90; value += 10) {
        setter.call(slider, String(value));
        slider.dispatchEvent(new Event('input', {bubbles:true}));
      }
    }
  });
  await new Promise(resolve => setTimeout(resolve, 300));
  assert.equal(await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-background-image')), imageUrl, 'slider changes keep the image URL stable');
  assert.deepEqual(await page.evaluate(() => ['background-shade','navigation-opacity','category-opacity','card-opacity','panel-opacity'].map(key => document.documentElement.style.getPropertyValue('--custom-' + key))), Array(5).fill('90%'));
  await page.reload({waitUntil:'networkidle2'});
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  assert.equal(await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-category-opacity')), '90%');
  assert.equal(await page.evaluate(() => document.documentElement.dataset.bookTheme), 'dark');
  assert.equal(await page.evaluate(() => document.documentElement.dataset.backgroundMode), 'image');
  await page.setViewport({width:390, height:844});
  await page.click('.book-theme-toggle');
  await page.waitForSelector('.custom-background-dialog');
  const mobile = await page.$eval('.custom-background-dialog', panel => {
    const box = panel.getBoundingClientRect();
    return {left:box.left, right:box.right, width:innerWidth};
  });
  assert.ok(mobile.left >= 0 && mobile.right <= mobile.width, 'mobile settings fit within the viewport');
  await page.click('.background-preset[data-preset="grid"]');
  await page.keyboard.press('Escape');
  await page.waitForSelector('.custom-background-dialog', {hidden:true});
  await page.reload({waitUntil:'networkidle2'});
  await page.waitForFunction(() => document.documentElement.dataset.backgroundMode === 'grid');
  assert.equal(await page.evaluate(() => document.documentElement.hasAttribute('data-custom-background')), true, 'built-in background persists');
  console.log('PASS: continuous background adjustment and persistence');
} finally {
  await browser.close();
}
