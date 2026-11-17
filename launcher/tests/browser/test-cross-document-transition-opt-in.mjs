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
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1'));
  await page.goto(origin, {waitUntil:'domcontentloaded'});
  await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'), {timeout:90000});

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
    if (pathname === 'about.html') {
      assert.equal(await page.$eval('#mastheadAboutToggle', node => node.getAttribute('aria-expanded')), 'false');
      assert.ok(await page.$eval('#mastheadMenuPanel', panel => {
        const footer = panel.lastElementChild;
        return footer.matches('footer') && footer.querySelector('#mastheadAboutToggle') &&
          Math.abs(footer.getBoundingClientRect().bottom - panel.getBoundingClientRect().bottom) < .5;
      }), 'About remains anchored to the bottom of the drawer');
      await tap('#mastheadAboutToggle');
    }
    await tap(`#mastheadMenuPanel a[href*="${pathname}"]`);
  }
  async function waitForDocument(pathname) {
    await page.waitForFunction(path => location.pathname.endsWith(path) && document.querySelector('h1')?.textContent.trim(), {}, pathname);
    assert.equal(await page.$eval('#launcherNavigationTransition', node => node.textContent), '@view-transition{navigation:auto}');
    await page.waitForFunction(() => ['160ms', '.16s', '0.16s'].includes(
      getComputedStyle(document.documentElement).getPropertyValue('--launcher-route-duration').trim(),
    ));
  }

  const closeNotice = await page.$('#firstUseNoticeClose');
  if (closeNotice && await closeNotice.isVisible()) {
    await tap('#firstUseNoticeClose');
    await page.waitForFunction(() => !document.querySelector('#firstUseNoticeDialog')?.open);
  }
  await openMenuAndTapLink('faq.html');
  await waitForDocument('/faq.html');

  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  await openMenuAndTapLink('about.html');
  await waitForDocument('/about.html');
  await page.goBack();
  await waitForDocument('/faq.html');
  await page.goBack();
  await page.waitForFunction(() => location.pathname === '/' || location.pathname.endsWith('/index.html'));

  assert.deepEqual(transitionErrors, []);
  assert.deepEqual(pageErrors, []);
  console.log('PASS home → FAQ → about → Back with reduced motion and no ViewTransition errors');
} finally {await browser.close();}
