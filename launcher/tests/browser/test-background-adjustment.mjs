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
  await page.click('input[name="base-theme"]:first-of-type');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'light');
  await page.click('.custom-theme-base label:last-child input');
  await page.waitForFunction(() => document.documentElement.dataset.bookTheme === 'dark');
  await (await page.$('.custom-background-dialog input[type=file]')).uploadFile(image);
  await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
  const imageUrl = await page.evaluate(() => document.documentElement.style.getPropertyValue('--custom-background-image'));
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
  console.log('PASS: continuous background adjustment and persistence');
} finally {
  await browser.close();
}
