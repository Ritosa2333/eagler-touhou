import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {mkdir} from 'node:fs/promises';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
const base = process.argv[2] || 'http://127.0.0.1:5173/';
const viewportWidths = process.env.ACCOUNT_TEST_FAST ? [320] : [1920, 1440, 1024, 900, 768, 390, 320];
const selectMode = async (page, mode) => {
  await page.click(`.account-tabs button:nth-child(${mode === '注册' ? 2 : 1})`);
  await page.waitForSelector(mode === '注册' ? '[name=username]' : '[name=identity]');
};
const settle = page => page.evaluate(() => Promise.all(document.getAnimations().filter(animation => animation.effect?.getTiming().iterations !== Infinity).map(animation => animation.finished.catch(() => {}))));
try {
  await mkdir('.cache/account', {recursive: true});
  const page = await browser.newPage(), errors = [], submissions = [];
  page.on('pageerror', error => errors.push(error.message));
  let loginAttempts = 0, challengeStarts = 0, verificationAttempts = 0;
  let sessionUser = null;
  const avatar = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAAEklEQVR4nGO8pOTCwMDAxAAGAA9YATw80XvmAAAAAElFTkSuQmCC';
  await page.setRequestInterception(true);
  page.on('request', request => {
    const url = new URL(request.url());
    if (url.pathname === '/api/ready') {
      void request.respond({status: 200, contentType: 'application/json', body: JSON.stringify({ready: true})});
      return;
    }
    if (url.pathname.startsWith('/api/')) {
      const entry = {path: url.pathname, method: request.method(), body: request.postData()};
      if (request.method() === 'POST') submissions.push(entry);
      let status = 200, body = {};
      if (url.pathname === '/api/auth/get-session') body = {user: sessionUser};
      if (url.pathname === '/api/v1/profiles/reimu') body = {
        username: 'reimu', nickname: '博丽灵梦', joinedAt: '2025-01-01T00:00:00.000Z',
        details: {name: '博丽灵梦', bio: '', region: 'Japan', avatar, cover: '', links: [],
          display: {visible: {friends: true, career: true, records: true, content: true}, order: ['career', 'records', 'content', 'friends', 'favorites']}},
      };
      if (url.pathname === '/api/account/register/start') {
        challengeStarts++;
        body = {challengeId: `challenge-${challengeStarts}`, expiresAt: 1_893_456_000_000};
      }
      if (url.pathname === '/api/account/register/resend')
        body = {expiresAt: 1_893_456_000_000};
      if (url.pathname === '/api/account/register/verify') {
        if (verificationAttempts++ === 0) {status = 410; body = undefined;}
      }
      if (url.pathname === '/api/auth/sign-in/email' && loginAttempts++ === 0) {
        status = 401; body = {code: 'INVALID_EMAIL_OR_PASSWORD', message: 'Raw server error must stay hidden'};
      }
      if (url.pathname === '/api/auth/sign-out') sessionUser = null;
      void request.respond({status, contentType: 'application/json', body: body === undefined ? '' : JSON.stringify(body)});
      return;
    }
    void request.continue();
  });
  await page.setViewport({width: 1440, height: 1000});
  await page.goto(new URL('login.html', base).href, {waitUntil: 'networkidle2'});
  await page.evaluate(() => document.fonts.ready);
  // Hide the site's timed announcement in isolated screenshots.
  await page.addStyleTag({content: '.site-notice{display:none!important}'});
  for (const width of viewportWidths) {
    await page.setViewport({width, height: 900});
    for (const theme of ['dark', 'light']) {
      await page.evaluate(theme => document.documentElement.setAttribute('data-book-theme', theme), theme); await settle(page);
      await selectMode(page, '登录');
      const dimensions = () => page.evaluate(() => {
        const box = selector => document.querySelector(selector).getBoundingClientRect().toJSON();
        return {header: box('.book-masthead'), shell: box('.account-shell'), art: box('.account-art'), overflow: document.documentElement.scrollWidth > innerWidth};
      });
      const login = await dimensions();
      const pageBounds = await page.evaluate(() => document.querySelector('.account-page').getBoundingClientRect().toJSON());
      assert.ok(Math.abs(pageBounds.left - login.shell.left) < 1, `${width}: content left edge`);
      assert.ok(Math.abs(pageBounds.right - login.shell.right) < 1, `${width}: content right edge`);
      assert.ok(Math.abs(pageBounds.top - login.shell.top) < 1, `${width}: content top edge`);
      if (width >= 780) assert.ok(Math.abs(pageBounds.top - login.header.bottom - 28) < 1, `${width}: desktop page gap`);
      else assert.ok(Math.abs(pageBounds.top - login.header.bottom) < 1, `${width}: compact page starts below header`);
      if (width >= 960) assert.ok(Math.abs(login.shell.height - 700) < 1, `${width}: desktop panel height`);
      else assert.ok(login.shell.bottom >= 899, `${width}: at least viewport height`);
      assert.equal(login.overflow, false);
      if (width >= 960) {
        assert.ok(Math.abs(login.art.top - (login.shell.top + 1)) < 1, `${width}: artwork begins inside the panel border`);
        assert.ok(Math.abs(login.art.bottom - (login.shell.bottom - 1)) < 1, `${width}: artwork reaches the panel bottom`);
      }
      if (width < 960) {
        const print = await page.evaluate(() => {
          const art = document.querySelector('.account-art'), input = document.querySelector('.account-content input');
          const a = art.getBoundingClientRect(), f = document.querySelector('form').getBoundingClientRect();
          return {artDisplay: getComputedStyle(art).display, scroll: getComputedStyle(document.querySelector('.account-content')).overflowY, size: getComputedStyle(input).fontSize, overlaps: a.left < f.right && a.right > f.left && a.top < f.bottom && a.bottom > f.top};
        });
        assert.deepEqual(print, {artDisplay: 'none', scroll: 'visible', size: '16px', overlaps: false});
      }
      const artSignature = width >= 960 ? await page.$eval('.account-art-image', image => ({
        src: image.currentSrc, fit: getComputedStyle(image).objectFit,
        position: getComputedStyle(image).objectPosition, filter: getComputedStyle(image).filter,
      })) : null;
      if ([1440, 390].includes(width)) await page.screenshot({path: `.cache/account/login-${width}-${theme}.png`});
      await selectMode(page, '注册'); await settle(page);
      const registration = await dimensions();
      if (width >= 960) assert.deepEqual(registration.art, login.art, `${width}: stable art geometry`);
      if (artSignature) assert.deepEqual(await page.$eval('.account-art-image', image => ({
        src: image.currentSrc, fit: getComputedStyle(image).objectFit,
        position: getComputedStyle(image).objectPosition, filter: getComputedStyle(image).filter,
      })), artSignature, `${width}: stable artwork source and presentation`);
      assert.equal(await page.$$eval('[data-captcha-slot]', elements => elements.length), 0);
      if ([1440, 390].includes(width)) await page.screenshot({path: `.cache/account/register-${width}-${theme}.png`});
    }
  }
  const registrationPassword = 'Tengu-password-42';
  await page.type('[name=username]', 'reimu');
  await page.type('[name=nickname]', '博丽灵梦');
  await page.type('[name=email]', 'player@example.com');
  await page.type('[name=password]', registrationPassword);
  await page.type('[name=confirmation]', 'wrong-password');
  await page.click('.account-code button');
  assert.match(await page.$eval('.account-page [role=alert]', el => el.textContent), /不一致/);
  assert.equal(submissions.length, 0, 'invalid registration draft is not sent');
  await page.$eval('[name=confirmation]', el => {el.select();});
  await page.type('[name=confirmation]', registrationPassword);
  await page.click('.account-code button');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /验证码已发送/);
  assert.equal(await page.$eval('.account-code button', el => el.disabled), true);
  for (const name of ['username', 'nickname', 'email', 'password', 'confirmation'])
    assert.equal(await page.$eval(`[name=${name}]`, el => el.disabled), true, `${name} is locked after the OTP request`);
  await selectMode(page, '登录');
  await selectMode(page, '注册');
  assert.equal(await page.$eval('[name=username]', el => el.value), 'reimu', 'registration draft survives mode changes');
  assert.equal(await page.$eval('[name=email]', el => el.value), 'player@example.com');
  assert.equal(await page.$eval('[name=username]', el => el.disabled), true, 'registration draft remains locked');
  await page.type('[name=code]', '123456');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /已过期/);
  assert.equal(await page.$eval('[name=email]', el => el.disabled), true, 'an expired challenge stays bound until the visitor restarts it');
  await page.click('.account-edit-details');
  assert.equal(await page.$eval('[name=email]', el => el.disabled), false, 'editing is available after explicitly abandoning the expired challenge');
  await page.$eval('[name=email]', el => {el.select();});
  await page.type('[name=email]', 'player2@example.com');
  await page.click('.account-code button');
  await page.waitForSelector('.account-page [role=status]');
  assert.match(await page.$eval('.account-page [role=status]', el => el.textContent), /验证码已发送/);
  await page.type('[name=code]', '123456');
  await page.click('.account-submit');
  await page.waitForFunction(() => location.pathname === '/profile.html' || (location.pathname === '/login.html' && new URLSearchParams(location.search).has('next')));
  const signUps = submissions.filter(request => request.path === '/api/account/register/start');
  assert.equal(signUps.length, 2);
  const signUp = signUps[0];
  assert.deepEqual(JSON.parse(signUp.body), {username: 'reimu', name: '博丽灵梦', email: 'player@example.com', password: registrationPassword});
  assert.deepEqual(JSON.parse(signUps[1].body), {username: 'reimu', name: '博丽灵梦', email: 'player2@example.com', password: registrationPassword});
  const verifications = submissions.filter(request => request.path === '/api/account/register/verify');
  assert.deepEqual(JSON.parse(verifications[0].body), {challengeId: 'challenge-1', email: 'player@example.com', otp: '123456', password: registrationPassword});
  assert.deepEqual(JSON.parse(verifications[1].body), {challengeId: 'challenge-2', email: 'player2@example.com', otp: '123456', password: registrationPassword});
  assert.equal(page.url().includes(registrationPassword), false, 'credentials never enter the URL');
  await page.goto(new URL('/profile.html', base).href, {waitUntil: 'networkidle2'});
  await page.waitForFunction(() => location.pathname === '/login.html' && new URLSearchParams(location.search).get('next') === '/profile.html');
  await page.waitForSelector('.account-page');
  await page.goto(new URL('login.html', base).href, {waitUntil: 'networkidle2'});
  await page.type('[name=identity]', 'player2@example.com'); await page.type('[name=password]', 'incorrect-password');
  await page.click('.account-password-toggle');
  assert.equal(await page.$eval('[name=password]', el => el.type), 'text');
  await page.click('.account-submit');
  await page.waitForSelector('.account-page [role=alert]');
  const loginError = await page.$eval('.account-page [role=alert]', el => el.textContent);
  assert.match(loginError, /用户名、邮箱或密码不正确/);
  assert.equal(loginError.includes('Raw server error'), false, 'backend text is not shown to users');
  await page.$eval('[name=password]', el => {el.select();});
  await page.type('[name=password]', registrationPassword);
  await page.click('.account-submit');
  await page.waitForFunction(() => location.pathname === '/profile.html' || (location.pathname === '/login.html' && new URLSearchParams(location.search).has('next')));
  assert.equal(await page.evaluate(password => JSON.stringify({...localStorage, ...sessionStorage}).includes(password), registrationPassword), false);
  const loginRequest = submissions.find(request => request.path === '/api/auth/sign-in/email');
  assert.deepEqual(JSON.parse(loginRequest.body), {email: 'player2@example.com', password: 'incorrect-password'});
  assert.equal(submissions.some(request => new URL(request.path, base).search.includes('password')), false);
  sessionUser = {username: 'reimu', name: '博丽灵梦'};
  await page.goto(new URL('/u/reimu', base).href, {waitUntil: 'networkidle2'});
  await page.waitForSelector('.profile-page');
  await page.waitForFunction(() => document.querySelector('.masthead-account-entry .account-avatar img')?.naturalWidth > 0);
  assert.equal(await page.$eval('.profile-avatar img', image => image.naturalWidth > 0), true, 'uploaded avatar appears on the profile');
  assert.equal(await page.$eval('.masthead-account-entry .account-handle', el => el.textContent), '博丽灵梦', 'header shows the member name');
  assert.equal(await page.$('.profile-section-nav a[href*="profile-content"]'), null, 'removed profile content tab stays hidden');
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  assert.equal(await page.$eval('.account-menu-identity img', image => image.naturalWidth > 0), true, 'uploaded avatar appears in the account menu');
  const profileLink = await page.$eval('.account-menu-panel a', link => link.getAttribute('href'));
  assert.match(profileLink, /\/u\/reimu/);
  await page.click('.account-menu-panel a[href$="#edit"]');
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#edit');
  await page.waitForSelector('.profile-settings');
  await page.goto(new URL('/u/reimu', base).href, {waitUntil: 'networkidle2'});
  await page.waitForSelector('.profile-page');
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  await page.click('.account-menu-panel a[href$="#settings"]');
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#settings');
  await page.waitForSelector('.profile-settings');
  await page.goto(new URL('/login.html?next=%2Fu%2Freimu%23settings', base).href, {waitUntil: 'networkidle2'});
  await page.waitForFunction(() => location.pathname === '/u/reimu' && location.hash === '#settings');
  await page.waitForSelector('.profile-settings');
  await page.click('.masthead-account-entry');
  await page.waitForSelector('.account-menu-panel');
  const menuItems = await page.$$('.account-menu-panel [role="menuitem"]');
  await menuItems.at(-1).click();
  await page.waitForFunction(() => location.pathname === '/');
  sessionUser = null;
  await page.waitForSelector('.account-guest');
  assert.ok(await page.$('.account-guest .account-login-icon svg'), 'guest header uses the login glyph');
  assert.ok(await page.$('.account-guest a[href*="mode=register"]'), 'guest header offers registration');
  await page.setViewport({width: 320, height: 720});
  const compactHeader = await page.evaluate(() => {
    const root = document.documentElement, brand = document.querySelector('.book-brand'), account = document.querySelector('.account-guest'), actions = document.querySelector('.masthead-links');
    const b = brand.getBoundingClientRect(), a = actions.getBoundingClientRect(), c = account.getBoundingClientRect();
    return {overflow: root.scrollWidth > innerWidth, brandRight: b.right, actionsLeft: a.left, accountRight: c.right, accountLeft: c.left};
  });
  assert.equal(compactHeader.overflow, false, '320px header has no horizontal overflow');
  assert.ok(compactHeader.brandRight <= compactHeader.actionsLeft || compactHeader.accountLeft > compactHeader.brandRight, '320px brand and account controls do not overlap');
  assert.ok(await page.$('.account-guest .account-login-icon svg'));
  assert.deepEqual(errors, []);
  console.log('PASS: aligned viewport layout, stable artwork, immutable OTP challenge, expiry recovery, localized login errors, profile navigation and no credential persistence');
} finally {await browser.close();}
