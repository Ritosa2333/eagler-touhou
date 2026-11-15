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
    return {right: document.documentElement.clientWidth - panel.right, left: panel.left, centerY: (panel.top + panel.bottom) / 2, overlay: overlay.backgroundColor};
  });
  assert.equal(desktop.overlay, 'rgba(0, 0, 0, 0)', 'preview is not dimmed by the dialog');
  assert.ok(Math.abs(desktop.left - desktop.right) <= 16, 'desktop settings panel starts centered within the page scrollbar gutter');
  assert.ok(Math.abs(desktop.centerY - 320) < 2, 'desktop settings panel starts slightly below center');
  const initial = await page.$eval('.custom-background-dialog', panel => ({rect: panel.getBoundingClientRect().toJSON(), background: getComputedStyle(panel).backgroundColor}));
  assert.match(initial.background, /0\.75\)$/, 'settings background is 25% transparent');
  const handle = await page.$eval('.custom-background-drag-handle', node => node.getBoundingClientRect().toJSON());
  await page.mouse.move(handle.left + 40, handle.top + 15);
  await page.mouse.down();
  await page.mouse.move(handle.left - 40, handle.top + 35, {steps: 5});
  await page.mouse.up();
  const moved = await page.$eval('.custom-background-dialog', panel => panel.getBoundingClientRect().toJSON());
  assert.ok(moved.left < initial.rect.left - 70, 'dragging the title row moves the settings panel');
  assert.ok(moved.left >= 0 && moved.top >= 0 && moved.right <= 800 && moved.bottom <= 600, 'dragged panel remains inside the viewport');
  const touchHandle = await page.$eval('.custom-background-drag-handle', node => node.getBoundingClientRect().toJSON());
  const client = await page.createCDPSession();
  await client.send('Input.dispatchTouchEvent', {type:'touchStart', touchPoints:[{x:touchHandle.left + 40, y:touchHandle.top + 15}]});
  await client.send('Input.dispatchTouchEvent', {type:'touchMove', touchPoints:[{x:touchHandle.left + 80, y:touchHandle.top + 15}]});
  await client.send('Input.dispatchTouchEvent', {type:'touchEnd', touchPoints:[]});
  const touched = await page.$eval('.custom-background-dialog', panel => panel.getBoundingClientRect().toJSON());
  assert.ok(touched.left > moved.left + 30, 'touching and dragging the title row moves the settings panel');
  await client.detach();
  await page.click('.custom-theme-switch [data-theme-value="light"]');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'light');
  await page.click('.custom-theme-switch [data-theme-value="dark"]');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'dark');
  await (await page.$('.custom-background-dialog input[type=file]')).uploadFile(image);
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  const imageUrl = await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-background-image'));
  for (const mode of ['dots', 'grid', 'seigaiha']) {
    await page.click(`.background-preset[data-preset="${mode}"]`);
    assert.equal(await page.evaluate(() => document.documentElement.dataset.backgroundMode), mode);
    assert.equal(await page.$eval(`.background-preset[data-preset="${mode}"]`, node => node.getAttribute('aria-pressed')), 'true');
    assert.match(await page.evaluate(() => getComputedStyle(document.body).backgroundImage), /gradient/);
    assert.doesNotMatch(await page.evaluate(() => getComputedStyle(document.body).backgroundImage), /blob:/, `${mode} must not display the custom image`);
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
  await page.click('#mastheadMenuToggle');
  await page.waitForSelector('.masthead-overview-theme', {visible:true});
  await page.click('.masthead-overview-theme');
  await page.waitForSelector('.custom-background-dialog');
  const mobile = await page.$eval('.custom-background-dialog', panel => {
    const box = panel.getBoundingClientRect();
    return {left:box.left, right:box.right, width:innerWidth};
  });
  assert.ok(mobile.left >= 0 && mobile.right <= mobile.width, 'mobile settings fit within the viewport');
  await page.click('.background-preset[data-preset="seigaiha"]');
  await page.keyboard.press('Escape');
  await page.waitForSelector('.custom-background-dialog', {hidden:true});
  await page.reload({waitUntil:'networkidle2'});
  await page.waitForFunction(() => document.documentElement.dataset.backgroundMode === 'seigaiha');
  assert.equal(await page.evaluate(() => document.documentElement.hasAttribute('data-custom-background')), true, 'built-in background persists');
  const rankings = await page.$eval('body', body => body.querySelector('.lobby-support-links a[href*="ratings.html"]')?.href);
  if (rankings) {
    await page.setViewport({width:1280, height:820});
    await page.goto(rankings, {waitUntil:'networkidle2'});
    await page.click('.book-theme-toggle');
    await page.waitForSelector('.custom-background-dialog');
    const sliderNames = () => page.$$eval('.custom-background-dialog input[type=range]', sliders => sliders.map(slider => slider.name));
    const appearance = () => page.evaluate(() => ({
      background: getComputedStyle(document.body).backgroundImage,
      shell: getComputedStyle(document.querySelector('.standalone-page-shell')).backgroundColor,
      navigation: getComputedStyle(document.querySelector('.book-masthead')).backgroundColor,
      panel: getComputedStyle(document.querySelector('.rating-table-wrap')).backgroundColor,
    }));
    assert.deepEqual(await sliderNames(), ['shade', 'navigation', 'panel']);
    assert.equal(await page.$('.custom-background-dialog select[name="background-fit"]'), null);
    const before = await appearance();
    assert.equal(before.shell, 'rgba(0, 0, 0, 0)');
    assert.match(before.background, /linear-gradient/);
    await page.$$eval('.custom-background-dialog input[type=range]', sliders => {
      const set = Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, 'value').set;
      for (const slider of sliders) {
        set.call(slider, '20');
        slider.dispatchEvent(new Event('input', {bubbles:true}));
      }
    });
    const after = await appearance();
    for (const surface of ['background', 'navigation', 'panel']) assert.notEqual(after[surface], before[surface], `${surface} responds on the rankings page`);
    await page.click('.background-preset[data-preset="image"]');
    await page.select('.custom-background-dialog select[name="background-fit"]', 'contain');
    assert.match(await page.evaluate(() => getComputedStyle(document.body).backgroundSize), /contain/);
    await page.click('.background-preset[data-preset="plain"]');
    assert.deepEqual(await sliderNames(), []);
    assert.equal(await page.evaluate(() => document.documentElement.hasAttribute('data-custom-background')), false);
    await page.click('.background-preset[data-preset="grid"]');
    await page.keyboard.press('Escape');
    await page.waitForSelector('.custom-background-dialog', {hidden:true});
    await page.setViewport({width:390, height:844});
    await page.click('#mastheadMenuToggle');
    await page.waitForSelector('.masthead-overview-theme', {visible:true});
    await page.click('.masthead-overview-theme');
    await page.waitForSelector('.custom-background-dialog');
    assert.deepEqual(await sliderNames(), ['shade', 'navigation', 'panel']);
  }
  console.log('PASS: continuous background adjustment and persistence');
} finally {
  await browser.close();
}
