import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
  await page.setViewport({width:1440, height:1440});
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  const detailUrl = new URL('?game=th07&lang=zh-CN', process.argv[2] || 'http://127.0.0.1:5173/');
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
  const slide = (name, value) => page.$eval(`.custom-background-dialog input[name="${name}"]`, (slider, value) => {
    Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, 'value').set.call(slider, value);
    slider.dispatchEvent(new Event('input', {bubbles:true}));
  }, String(value));
  await slide('category', 40);
  assert.equal((await rgba('.book-sidebar', 'backgroundColor'))[3], 102, 'the category slider controls the sidebar without wallpaper');
  assert.equal(await style('body', 'backgroundImage'), 'none');
  for (const theme of ['dark', 'light']) {
    await page.click(`.custom-theme-switch [data-theme-value="${theme}"]`);
    await page.waitForFunction(theme => document.documentElement.dataset.bookTheme === theme, {}, theme);
    assert.deepEqual(await rgba(desktopHero, 'backgroundColor'), [37, 34, 45, 255]);
    const text = await rgba(`${desktopHero} h2`, 'color');
    assert.ok(text[0] > 200, 'the original dark hero retains its light foreground in both themes');
    assert.ok(await page.$(`${desktopHero} .game-detail-backdrop`), 'the main detail keeps its existing cover backdrop');
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
  await slide('shade', 0);
  assert.match(await style('body', 'backgroundImage'), /blob:/);
  assert.equal((await rgba('.book-sidebar', 'backgroundColor'))[3], 102, 'changing background keeps the sidebar opacity');
  assert.equal(await page.$eval('.book-sidebar', sidebar => {
    for (let node = sidebar.parentElement; node !== document.body; node = node.parentElement) {
      if (getComputedStyle(node).backgroundColor !== 'rgba(0, 0, 0, 0)') return false;
    }
    return true;
  }), true, 'the sidebar has no opaque ancestor covering the shared wallpaper');
  for (const percent of [35, 100, 88]) {
    await slide('panel', percent);
    assert.equal((await rgba('.book-sidebar', 'backgroundColor'))[3], 102, `panel opacity at ${percent}% does not replace the category opacity`);
    assert.equal((await rgba(desktopPanel, 'backgroundColor'))[3], 255, 'reading detail stays opaque while ordinary panel transparency changes');
    assert.equal(await style(desktopPanel, 'opacity'), '1', 'panel text stays opaque');
    assert.equal((await rgba(desktopHero, 'backgroundColor'))[3], 255, 'panel transparency does not affect the hero');
  }
  await page.keyboard.press('Escape');
  assert.equal(await page.$eval(desktopPanel, panel => panel.getBoundingClientRect().bottom >= innerHeight - 1), true, 'the detail fills the available window height');
  await page.setViewport({width:390, height:844, isMobile:true, hasTouch:true});
  await page.goto(detailUrl, {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.mobile-game-hero', {visible:true});
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  assert.deepEqual(await rgba('.mobile-game-hero', 'backgroundColor'), [37, 34, 45, 255]);
  assert.equal((await rgba('.tools[data-full-page]', 'backgroundColor'))[3], 255, 'the main full-page detail remains opaque');
  await page.setViewport({width:1440, height:900, isMobile:false, hasTouch:false});
  await page.goto(new URL('lobby.html', detailUrl), {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.lobby-list-surface', {visible:true});
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  assert.equal((await rgba('.lobby-list-surface', 'backgroundColor'))[3], 224, 'the same setting is used by the room list');
  console.log('PASS: main detail presentation, plain-background opacity controls and lobby panel opacity');
} finally {
  await browser.close();
}
