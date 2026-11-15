import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
try {
  const page = await browser.newPage(), errors = [];
  page.on('pageerror', error => errors.push(error.message));
  for (const width of [1440, 390]) {
    await page.setViewport({width, height: 900});
    await page.goto(new URL('profile.html', process.argv[2] || 'http://127.0.0.1:5173/').href, {waitUntil: 'networkidle2'});
    await page.evaluate(() => {
      window.profileMotionTrace = []; window.profileSnapshotCalls = 0;
      const animate = Element.prototype.animate;
      Element.prototype.animate = function (...args) {
        const animation = animate.apply(this, args);
        if (this.matches('.profile-wrap,.profile-content,.profile-editor-shell')) window.profileMotionTrace.push({node: this, animation});
        return animation;
      };
      const start = document.startViewTransition;
      if (start) document.startViewTransition = function (...args) {window.profileSnapshotCalls++; return start.apply(this, args);};
    });
    const change = async (scope, label) => {
      await page.evaluate(({scope, label}) => {
        window.profileMotionTrace = [];
        const control = [...document.querySelectorAll(`${scope} button`)].find(node => node.textContent.includes(label));
        if (!control) throw new Error('Missing profile control: ' + label);
        control.click();
      }, {scope, label});
      await page.waitForFunction(() => window.profileMotionTrace.length > 0);
      const trace = await page.evaluate(() => window.profileMotionTrace.map(({node, animation}) => ({
        target: node.className, duration: animation.effect.getTiming().duration, easing: animation.effect.getTiming().easing,
        properties: [...new Set(animation.effect.getKeyframes().flatMap(frame => Object.keys(frame)))],
      })));
      assert.equal(trace.length, 1, `${width}/${label}: one presentation owner`);
      const target = ['账户设置', '返回主页'].includes(label) ? 'profile-wrap' : scope === '.profile-settings-heading' ? 'profile-editor-shell' : 'profile-content';
      assert.ok(trace[0].target.split(/\s+/).includes(target), `${width}/${label}: correct local surface`);
      assert.equal(trace[0].duration, 180);
      assert.equal(trace[0].easing, 'cubic-bezier(0.2, 0, 0, 1)');
      assert.equal(trace[0].properties.some(name => ['height','width','top','left'].includes(name)), false, 'presentation does not animate layout');
      await page.evaluate(() => Promise.allSettled(window.profileMotionTrace.map(({animation}) => animation.finished)));
      assert.equal(await page.evaluate(() => window.profileSnapshotCalls), 0, 'profile changes do not capture a page snapshot');
      assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
      assert.equal(await page.evaluate(() => window.profileMotionTrace.every(({node}) => !node.style.opacity)), true, 'temporary opacity is restored');
    };
    await change('.profile-actions', '视角：'); await change('.profile-actions', '视角：');
    await change('.profile-tabs', '好友'); await change('.profile-social-tools', '关注');
    assert.equal(await page.$eval('.profile-aside', node => getComputedStyle(node).display === 'none'), width <= 780);
    await change('.profile-tabs', '概览');
    assert.equal(await page.$eval('.profile-aside', node => getComputedStyle(node).display === 'none'), false);
    for (const label of ['生涯', '游玩记录', '内容']) {
      await change('.profile-tabs', label);
      assert.equal(await page.$eval('.profile-aside', node => getComputedStyle(node).display === 'none'), width <= 780);
    }
    await change('.profile-actions', '账户设置'); await change('.profile-settings-heading', '隐私');
    await page.evaluate(() => scrollTo(0, 80)); const scroll = await page.evaluate(() => scrollY);
    await change('.profile-settings-heading', '账户与安全');
    assert.equal(await page.evaluate(() => scrollY), scroll, 'settings tabs preserve scroll');
    await change('.profile-topbar', '返回主页');
  }
  await page.evaluate(() => {
    const tabs = [...document.querySelectorAll('.profile-tabs button')];
    tabs.find(node => node.textContent === '好友').click(); tabs.find(node => node.textContent === '概览').click();
  });
  await page.evaluate(() => Promise.allSettled(window.profileMotionTrace.map(({animation}) => animation.finished)));
  assert.equal(await page.$eval('.profile-tabs button[aria-current="page"]', node => node.textContent), '概览');
  for (const preference of ['site', 'system']) {
    if (preference === 'site') await page.evaluate(() => document.body.classList.add('less-motion'));
    else await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
    await page.evaluate(() => {window.profileMotionTrace = []; document.querySelector('.profile-perspective').click();});
    assert.equal(await page.evaluate(() => window.profileMotionTrace.length), 0, preference + ': no new animation');
    if (preference === 'site') await page.evaluate(() => {document.querySelector('.profile-perspective').click(); document.body.classList.remove('less-motion');});
    else await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'no-preference'}]);
  }
  await page.evaluate(() => {document.startViewTransition = undefined; document.querySelector('.profile-perspective').click();});
  await page.evaluate(() => Promise.allSettled(window.profileMotionTrace.map(({animation}) => animation.finished)));
  assert.equal(await page.$eval('.profile-perspective', node => node.getAttribute('aria-pressed')), 'false');
  assert.equal(await page.evaluate(() => window.profileSnapshotCalls), 0);
  assert.deepEqual(errors, []);
  console.log('PASS: shared profile motion, rapid replacement, local scopes, narrow layouts, reduced motion and optional-API independence');
} finally {await browser.close();}
