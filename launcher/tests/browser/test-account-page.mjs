import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir} from 'node:fs/promises';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
const base = process.argv[2] || 'http://127.0.0.1:5173/';
const selectMode = async (page, mode) => {
  await page.evaluate(mode => [...document.querySelectorAll('.account-tabs button')].find(el => el.textContent === mode).click(), mode);
  await page.waitForSelector(mode === '注册' ? '[name=username]' : '[name=identity]');
};
const settle = page => page.evaluate(() => Promise.all(document.getAnimations().filter(animation => animation.effect?.getTiming().iterations !== Infinity).map(animation => animation.finished.catch(() => {}))));
try {
  await mkdir('.cache/account', {recursive: true});
  const page = await browser.newPage(), errors = [], submissions = [];
  page.on('pageerror', error => errors.push(error.message));
  page.on('request', request => {if (request.method() === 'POST') submissions.push(request.url());});
  await page.setViewport({width: 1440, height: 1000});
  await page.goto(new URL('login.html', base).href, {waitUntil: 'networkidle2'});
  await page.evaluate(() => document.fonts.ready);
  // Hide the site's timed announcement in isolated screenshots.
  await page.addStyleTag({content: '.site-notice{display:none!important}'});
  for (const width of [1920, 1440, 1024, 900, 768, 390, 320]) {
    await page.setViewport({width, height: 900});
    for (const theme of ['dark', 'light']) {
      await page.evaluate(theme => document.documentElement.setAttribute('data-book-theme', theme), theme); await settle(page);
      await selectMode(page, '登录');
      const dimensions = () => page.evaluate(() => {
        const box = selector => document.querySelector(selector).getBoundingClientRect().toJSON();
        return {header: box('.book-masthead'), radius: parseFloat(getComputedStyle(document.querySelector('.book-masthead')).borderBottomLeftRadius), shell: box('.account-shell'), art: box('.account-art'), overflow: document.documentElement.scrollWidth > innerWidth};
      });
      const login = await dimensions();
      assert.ok(Math.abs(login.header.left - login.shell.left) < 1, `${width}: left edges`);
      assert.ok(Math.abs(login.header.right - login.shell.right) < 1, `${width}: right edges`);
      assert.ok(Math.abs(login.header.bottom - login.shell.top) < 1, `${width}: top edge`);
      if (width >= 960) assert.ok(Math.abs(login.shell.bottom - 900) < 1, `${width}: viewport height`);
      else assert.ok(login.shell.bottom >= 899, `${width}: at least viewport height`);
      assert.equal(login.overflow, false);
      if (width >= 960) {
        assert.ok(Math.abs(login.art.top - (login.header.bottom - login.radius)) < 1, `${width}: artwork extends behind rounded corners`);
        assert.ok(Math.abs(login.art.bottom - login.shell.bottom) < 1, `${width}: artwork reaches the bottom`);
      }
      assert.equal(await page.$eval('.account-shell', el => getComputedStyle(el, '::before').height), `${login.radius}px`);
      if (width < 960) {
        const print = await page.evaluate(() => {
          const art = document.querySelector('.account-art'), input = document.querySelector('.account-content input');
          const a = art.getBoundingClientRect(), f = document.querySelector('form').getBoundingClientRect();
          return {fit: getComputedStyle(art.querySelector('img')).objectFit, events: getComputedStyle(art).pointerEvents, scroll: getComputedStyle(document.querySelector('.account-content')).overflowY, size: getComputedStyle(input).fontSize, overlaps: a.left < f.right && a.right > f.left && a.top < f.bottom && a.bottom > f.top};
        });
        assert.deepEqual(print, {fit: 'cover', events: 'none', scroll: 'visible', size: '16px', overlaps: true});
      }
      const image = width >= 960 ? await (await page.$('.account-art')).screenshot() : null;
      if ([1440, 390].includes(width)) await page.screenshot({path: `.cache/account/login-${width}-${theme}.png`});
      await selectMode(page, '注册'); await settle(page);
      const registration = await dimensions();
      if (width >= 960) assert.deepEqual(registration.art, login.art, `${width}: stable art geometry`);
      if (image) assert.deepEqual(await (await page.$('.account-art')).screenshot(), image, `${width}: stable art pixels`);
      assert.equal(await page.$eval('[data-captcha-slot]', el => el.childElementCount), 0);
      if ([1440, 390].includes(width)) await page.screenshot({path: `.cache/account/register-${width}-${theme}.png`});
    }
  }
  await page.type('[name=username]', '博丽灵梦');
  await page.type('[name=email]', 'player@example.com');
  await page.click('.account-code button');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /123456/);
  assert.equal(await page.$eval('.account-code button', el => el.disabled), true);
  await page.type('[name=code]', '123456');
  await page.type('[name=password]', 'Demo-password-42');
  await page.type('[name=confirmation]', 'wrong-password');
  await page.click('.account-submit');
  assert.match(await page.$eval('.account-page [role=alert]', el => el.textContent), /不一致/);
  await page.$eval('[name=confirmation]', el => {el.select();});
  await page.type('[name=confirmation]', 'Demo-password-42');
  await page.click('.account-submit');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /注册演示完成/);
  await selectMode(page, '登录');
  await page.type('[name=identity]', 'player@example.com'); await page.type('[name=password]', 'Demo-password-42');
  await page.click('.account-password-toggle');
  assert.equal(await page.$eval('[name=password]', el => el.type), 'text');
  await page.click('.account-submit');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /登录演示完成/);
  await page.click('.account-discord');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /Discord/);
  assert.equal(await page.evaluate(() => JSON.stringify({...localStorage, ...sessionStorage}).includes('Demo-password-42')), false);
  assert.deepEqual(submissions, []);
  await page.goto(new URL('profile.html', base).href, {waitUntil: 'networkidle2'});
  await page.click('.profile-account-login'); await page.waitForSelector('.account-page');
  assert.match(page.url(), /login\.html/);
  await page.goto(new URL('login.html?mode=register', base).href, {waitUntil: 'networkidle2'});
  assert.ok(await page.$('[name=username]'));
  assert.deepEqual(errors, []);
  console.log('PASS: aligned viewport layout, stable artwork, login, registration, demo codes, empty captcha slot, Discord, navigation and no credential persistence');
} finally {await browser.close();}
