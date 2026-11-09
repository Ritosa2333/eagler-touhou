import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath:process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe',headless:true});
try {
  const page = await browser.newPage();
  await page.goto(new URL('lobby.html',process.argv[2] || 'http://127.0.0.1:5173/'),{waitUntil:'networkidle2'});
  await page.waitForSelector('.book-theme-toggle:not(:disabled)');
  await page.click('.book-theme-toggle');
  await page.waitForSelector('.custom-background-dialog');
  assert.equal(await page.$eval('.custom-background-dialog',element=>getComputedStyle(element).animationName),'kit-dialog-fade-in');
  await new Promise(resolve=>setTimeout(resolve,300));
  await page.keyboard.press('Escape');
  const closing=await page.$eval('.custom-background-dialog',element=>({animation:getComputedStyle(element).animationName,state:element.dataset.state}));
  assert.deepEqual(closing,{animation:'kit-dialog-fade-out',state:'closed'});
  await page.waitForSelector('.custom-background-dialog',{hidden:true});
  await page.$eval('#roomDialog',element=>element.showModal());
  assert.match(await page.$eval('#roomDialog',element=>getComputedStyle(element).transitionProperty),/opacity/);
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  assert.equal(await page.$eval('#roomDialog',element=>getComputedStyle(element).transitionDuration),'0s');
  await page.$eval('#roomDialog',element=>element.close());
  console.log('PASS: dialog fades and reduced motion');
} finally {await browser.close();}
