import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
  await page.setViewport({width:1440, height:900});
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  const detailUrl = new URL('?game=th05&lang=zh-CN', process.argv[2] || 'http://127.0.0.1:5173/');
  await page.goto(detailUrl, {waitUntil:'domcontentloaded'});
  const desktopHero = '.book-game-detail-hero';
  const desktopPanel = '.book-game-detail';
  await page.waitForSelector(desktopHero, {visible:true});
  await page.click('.book-theme-toggle');
  await page.waitForSelector('.custom-background-dialog', {visible:true});
  await page.click('.background-preset[data-preset="plain"]');
  const style = (selector, property) => page.$eval(selector, (node, key) => getComputedStyle(node)[key], property);
  const rgba = (selector, property) => page.$eval(selector, (node, key) => {
    const canvas = document.createElement('canvas');
    canvas.width = canvas.height = 1;
    const context = canvas.getContext('2d');
    context.fillStyle = getComputedStyle(node)[key];
    context.fillRect(0, 0, 1, 1);
    return Array.from(context.getImageData(0, 0, 1, 1).data);
  }, property);
  for (const [theme, neutral] of [['dark', 21], ['light', 246]]) {
    await page.click(`.custom-theme-switch [data-theme-value="${theme}"]`);
    await page.waitForFunction(theme => document.documentElement.dataset.bookTheme === theme, {}, theme);
    assert.deepEqual(await rgba(desktopHero, 'backgroundColor'), [neutral, neutral, neutral, 255]);
    const text = await rgba(`${desktopHero} h2`, 'color');
    assert.ok(theme === 'dark' ? text[0] > 200 : text[0] < 70);
    assert.match(await style(`${desktopHero} .game-detail-backdrop`, 'maskImage'), /linear-gradient/);
    if (theme === 'light') assert.match(await style(`${desktopHero} .game-detail-backdrop`, 'maskImage'), /rgba\(0, 0, 0, 0\./);
    assert.equal((await rgba(desktopPanel, 'backgroundColor'))[3], 255);
  }
  // Exercise the real upload and sliders with a high-contrast wallpaper.
  await page.$eval('.custom-background-dialog input[type=file]', async input => {
    const canvas = document.createElement('canvas');
    canvas.width = canvas.height = 32;
    const context = canvas.getContext('2d');
    context.fillStyle = '#ff18df';
    context.fillRect(0, 0, 32, 32);
    context.fillStyle = '#003933';
    context.fillRect(0, 0, 16, 16);
    context.fillRect(16, 16, 16, 16);
    const result = await new Promise(resolve => canvas.toBlob(resolve));
    const transfer = new DataTransfer();
    transfer.items.add(new File([result], 'checkerboard.png', {type:'image/png'}));
    input.files = transfer.files;
    input.dispatchEvent(new Event('change', {bubbles:true}));
  });
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background') &&
    document.documentElement.dataset.backgroundMode === 'image');
  const slide = (name, value) => page.$eval(`.custom-background-dialog input[name="${name}"]`, (slider, value) => {
    Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, 'value').set.call(slider, value);
    slider.dispatchEvent(new Event('input', {bubbles:true}));
  }, String(value));
  await slide('shade', 0);
  assert.match(await style('body', 'backgroundImage'), /blob:/);
  for (const percent of [35, 100, 88]) {
    await slide('panel', percent);
    const alpha = (await rgba(desktopPanel, 'backgroundColor'))[3];
    assert.ok(Math.abs(alpha - percent * 2.55) < 1, `detail uses the ${percent}% panel setting`);
    assert.equal(await style(desktopPanel, 'opacity'), '1', 'panel text stays opaque');
    assert.equal((await rgba(desktopHero, 'backgroundColor'))[3], 255, 'panel transparency does not affect the hero');
  }
  await page.keyboard.press('Escape');
  await page.setViewport({width:390, height:844, isMobile:true, hasTouch:true});
  await page.goto(detailUrl, {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.mobile-game-hero', {visible:true});
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  assert.deepEqual(await rgba('.mobile-game-hero', 'backgroundColor'), [246, 246, 246, 255]);
  assert.equal((await rgba('.tools[data-book-sheet]', 'backgroundColor'))[3], 224, 'the mobile sheet uses the saved panel opacity');
  await page.setViewport({width:1440, height:900, isMobile:false, hasTouch:false});
  await page.goto(new URL('lobby.html', detailUrl), {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.lobby-list-surface', {visible:true});
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  assert.equal((await rgba('.lobby-list-surface', 'backgroundColor'))[3], 224, 'the same setting is used by the room list');
  console.log('PASS: background adjustment, themed detail artwork and panel opacity on desktop/mobile');
} finally {
  await browser.close();
}
