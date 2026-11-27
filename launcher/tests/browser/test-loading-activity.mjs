import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:5185/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', error => errors.push(error.message));
  const faces = '.loading-activity-faces img';
  const selector = '.loading-activity';
  const waitForFaces = () => page.waitForFunction(selector => {
    const images = [...document.querySelectorAll(selector)];
    return images.length === 4 && images.every(image => image.complete && image.naturalWidth > 0);
  }, {}, faces);
  const snapshot = () => page.$eval(selector, (loader, faces) => {
    const images = [...loader.querySelectorAll(faces)];
    const dots = [...loader.querySelectorAll('.loading-activity-dots > span')];
    const bounds = loader.getBoundingClientRect();
    return {
      images:images.map(image => image.src),
      motion:images.map(image => getComputedStyle(image).animationName),
      faceHop:getComputedStyle(images[0]).getPropertyValue('--loading-hop').trim(),
      dotHop:getComputedStyle(dots[0]).getPropertyValue('--loading-hop').trim(),
      crops:images.map(image => [getComputedStyle(image).objectFit, getComputedStyle(image).objectPosition]),
      delays:images.map(image => getComputedStyle(image).animationDelay),
      dots:dots.map(dot => [dot.textContent, getComputedStyle(dot).animationName, getComputedStyle(dot).animationDelay]),
      tip:loader.querySelector('.loading-activity-tip span').textContent,
      fits:bounds.left >= 0 && bounds.right <= innerWidth,
    };
  }, faces);
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.setViewport({width:1440, height:900});
  await page.goto(new URL('ui-review.html?preview=loading',origin).href,{waitUntil:'domcontentloaded'});
  await waitForFaces();
  const first = await snapshot();
  assert.equal(new Set(first.images).size,4);
  assert.deepEqual(first.motion,Array(4).fill('ui-loading-hop'));
  assert.deepEqual([first.faceHop,first.dotHop],['-2px','-1px'],'loading faces and dots keep their stagger without a large jump');
  assert.deepEqual(first.crops,Array(4).fill(['cover','0% 50%']),'the avatar is cropped out of its wordmark SVG');
  assert.deepEqual(first.delays,['0s','0.3s','0.6s','0.9s']);
  assert.deepEqual(first.dots,first.delays.slice(0,3).map(delay => ['.','ui-loading-hop',delay]));
  assert.ok(first.tip);
  assert.ok(first.fits);
  await page.setViewport({width:320,height:640});
  assert.ok((await snapshot()).fits);
  assert.deepEqual((await snapshot()).images,first.images,'resizing does not reroll the current load');
  const draws = [first.images.join(',')];
  for (let i = 0; i < 3; i++) {
    await page.click('.loading-activity-preview-tools button:last-child');
    await waitForFaces();
    const current = await snapshot();
    assert.equal(new Set(current.images).size,4);
    draws.push(current.images.join(','));
  }
  assert.ok(new Set(draws).size > 1,'new loads draw new random lineups');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  assert.deepEqual((await snapshot()).motion,Array(4).fill('none'));
  assert.deepEqual((await snapshot()).dots.map(([dot,motion]) => [dot,motion]),Array(3).fill(['.','none']));
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.evaluate(() => document.body.classList.add('less-motion'));
  assert.deepEqual((await snapshot()).motion,Array(4).fill('none'));
  assert.deepEqual((await snapshot()).dots.map(([dot,motion]) => [dot,motion]),Array(3).fill(['.','none']));
  assert.deepEqual(errors,[]);
  console.log('PASS randomized load lineups, sequential motion, tips, narrow layout, and both reduced-motion settings');
} finally {await browser.close();}
