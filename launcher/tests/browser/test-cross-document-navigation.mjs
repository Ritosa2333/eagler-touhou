import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:8131/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  const pageErrors = [], transitionErrors = [];
  page.on('pageerror', error => pageErrors.push(error.message));
  page.on('console', message => {
    if (message.type() === 'error' && message.text().includes('ViewTransition opt-in disabled')) transitionErrors.push(message.text());
  });
  await page.setViewport({width:390,height:844,deviceScaleFactor:1,isMobile:true,hasTouch:true});
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.evaluateOnNewDocument(() => {
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
    window.documentSnapshots = 0;
    window.addEventListener('pagereveal', event => {if (event.viewTransition) window.documentSnapshots++;});
    window.addEventListener('pageswap', event => {if (event.viewTransition) window.documentSnapshots++;});
    window.pageEntrances = [];
    if (/\/(faq|about)\.html$/.test(location.pathname)) {
      const firstFrame = () => {
        const article = document.querySelector('#nd-page');
        if (!article?.querySelector('h1')) {requestAnimationFrame(firstFrame); return;}
        window.firstArticleOpacity = Number(getComputedStyle(article).opacity);
      };
      requestAnimationFrame(firstFrame);
    }
    window.addEventListener('animationstart', event => {
      if (event.animationName !== 'ui-page-entrance') return;
      const animation = event.target.getAnimations().find(effect => effect.animationName === event.animationName);
      if (animation) window.pageEntrances.push({target:event.target.id, source:'css', duration:animation.effect.getTiming().duration,
        finished:animation.finished.catch(() => {})});
    });
    const animate = Element.prototype.animate;
    Element.prototype.animate = function (frames, timing) {
      const animation = animate.call(this, frames, timing);
      const [first, last] = animation.effect.getKeyframes();
      if (first?.opacity === '0.28' && last?.opacity === '1') {
        window.pageEntrances.push({target:this.id, source:'live', duration:animation.effect.getTiming().duration,
          finished:animation.finished.catch(() => {})});
      }
      return animation;
    };
  });
  await page.goto(origin, {waitUntil:'domcontentloaded'});
  await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'), {timeout:90000});
  assert.equal(await page.evaluate(() => window.pageEntrances.length), 0, 'direct entry has no route animation');

  async function tap(selector) {
    const element = await page.waitForSelector(selector, {visible:true});
    assert.ok(element, `missing ${selector}`);
    await element.evaluate(node => node.scrollIntoView({behavior:'instant',block:'center'}));
    const rect = await element.boundingBox();
    assert.ok(rect, `not visible ${selector}`);
    await page.touchscreen.tap(rect.x + rect.width / 2, rect.y + rect.height / 2);
  }
  async function openMenuAndTapLink(pathname) {
    await page.waitForSelector('#mastheadMenuToggle[aria-haspopup="dialog"]:not([disabled])', {visible:true});
    await tap('#mastheadMenuToggle');
    await page.waitForFunction(() => {
      const panel = document.querySelector('#mastheadMenuPanel[data-state="open"]');
      const transform = panel && getComputedStyle(panel).transform;
      return transform && (transform === 'none' || Math.abs(new DOMMatrixReadOnly(transform).m41) < .5);
    });
    await tap(`#mastheadMenuPanel a[href*="${pathname}"]`);
  }
  async function waitForDocument(pathname) {
    await page.waitForFunction(path => location.pathname.endsWith(path) && document.querySelector('h1')?.textContent.trim(), {}, pathname);
    assert.equal(await page.evaluate(() => window.documentSnapshots), 0, 'document navigation does not capture snapshots');
  }
  async function expectDocumentEntrance() {
    await page.waitForFunction(() => window.pageEntrances.length === 1);
    assert.ok(await page.evaluate(() => window.firstArticleOpacity < .65), 'the first content frame is already fading in');
    assert.deepEqual(await page.evaluate(async () => {
      const [{target, source, duration, finished}] = window.pageEntrances;
      await finished;
      return {target, source, duration, opacity:getComputedStyle(document.getElementById(target)).opacity};
    }), {target:'nd-page', source:'css', duration:240, opacity:'1'});
  }
  async function expectNoDocumentEntrance(message) {
    assert.deepEqual(await page.evaluate(() => ({
      count:window.pageEntrances.length, prepared:document.documentElement.hasAttribute('data-ui-document-entry'),
    })), {count:0, prepared:false}, message);
  }

  const closeNotice = await page.$('#firstUseNoticeClose');
  if (closeNotice && await closeNotice.isVisible()) {
    await tap('#firstUseNoticeClose');
    await page.waitForFunction(() => !document.querySelector('#firstUseNoticeDialog')?.open);
  }
  await openMenuAndTapLink('faq.html');
  await waitForDocument('/faq.html');
  await expectDocumentEntrance();

  // Help pages use their own mobile navigation, not the launcher drawer.
  await page.evaluate(() => location.assign(new URL('about.html?lang=zh-CN', location.href)));
  await waitForDocument('/about.html');
  await expectDocumentEntrance();
  assert.equal(await page.$eval('[data-help-page="about"] a[href="https://qm.qq.com/q/eeUrxIltug"]', node => node.textContent), '闲聊向QQ群');
  assert.ok(await page.$eval('[data-help-page="about"]', node => node.textContent.includes('反馈向QQ群：1075766678')));
  await page.reload({waitUntil:'domcontentloaded'});
  await waitForDocument('/about.html');
  await expectNoDocumentEntrance('reload has no route animation');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  await page.goBack();
  await waitForDocument('/faq.html');
  assert.equal(await page.evaluate(() => window.pageEntrances.length <= 1), true, 'Back does not add an entrance in reduced motion');
  await page.goBack();
  await page.waitForFunction(() => location.pathname === '/' || location.pathname.endsWith('/index.html'));
  assert.equal(await page.evaluate(() => window.documentSnapshots), 0);
  await page.evaluate(() => location.assign(new URL('faq.html', location.href)));
  await waitForDocument('/faq.html');
  await expectNoDocumentEntrance('system reduced motion skips cross-document entrance');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.evaluate(() => {
    localStorage.setItem('eagler-touhou-less-motion-v1','1');
    location.assign(new URL('about.html', location.href));
  });
  await waitForDocument('/about.html');
  await expectNoDocumentEntrance('saved site preference skips cross-document entrance');

  assert.deepEqual(transitionErrors, []);
  assert.deepEqual(pageErrors, []);
  console.log('PASS cross-document content fade, About contacts, reload, Back, reduced motion, and zero snapshots');
} finally {await browser.close();}
