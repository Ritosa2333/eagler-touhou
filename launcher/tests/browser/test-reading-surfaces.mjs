import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const base = new URL(process.argv[2] || 'http://127.0.0.1:5173/');
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
  await page.evaluateOnNewDocument(() => {
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
    localStorage.setItem('eagler.background.settings', JSON.stringify({mode:'grid', shade:0, panel:88}));
  });
  const profilePath = '/api/v1/profiles/surface-fixture';
  const responses = {
    '/api/ready': {ready:true},
    '/api/auth/get-session': {user:null},
    '/api/v1/announcements': {canPublish:false, nextBefore:null,
      items:[{id:1, author:'Fixture', body:'Announcement preview line.\n'.repeat(14).trim(),
        publishedAt:Date.parse('2026-11-26T23:00:00Z')}]},
    [profilePath]: {username:'surface-fixture', nickname:'Surface fixture', joinedAt:'2026-01-01T00:00:00Z',
      details:{bio:'A local profile fixture.', display:{visible:{versus:true, records:true}}}},
    [profilePath + '/statistics']: {games:[], records:true, playtimeSeconds:0},
    [profilePath + '/th09-duels']: {summary:{games:0,wins:0},duels:[]},
    [profilePath + '/social']: {visible:true, followerCount:0, followingCount:0, followers:[], followingUsers:[], friends:[]},
  };
  await page.setRequestInterception(true);
  page.on('request', request => {
    const path = new URL(request.url()).pathname;
    if (!path.startsWith('/api/')) return request.continue();
    return request.respond({status:200, contentType:'application/json',
      headers:{'Access-Control-Allow-Origin':base.origin, 'Access-Control-Allow-Credentials':'true'},
      body:JSON.stringify(responses[path] ?? {items:[]})});
  });
  const settings = (mode, panel) => page.evaluate((mode, panel) => {
    localStorage.setItem('eagler.background.settings', JSON.stringify({mode, shade:0, panel}));
    window.dispatchEvent(new Event('eagler-background-settings-change'));
  }, mode, panel);
  const paint = selector => page.$eval(selector, node => {
    const context = document.createElement('canvas').getContext('2d');
    const css = getComputedStyle(node);
    context.fillStyle = css.backgroundColor;
    context.fillRect(0, 0, 1, 1);
    return {alpha:context.getImageData(0, 0, 1, 1).data[3], opacity:css.opacity};
  });
  const announcement = responses['/api/v1/announcements'].items[0];
  const checkMessagePageHeight = async (route, width) => {
    const sizes = await page.$eval('.message-center', panel => ({
      viewport: innerHeight,
      pageBottom: panel.parentElement.getBoundingClientRect().bottom,
      panelBottom: panel.getBoundingClientRect().bottom,
      bodyBottom: document.body.getBoundingClientRect().bottom,
      scrollHeight: document.documentElement.scrollHeight,
    }));
    const bottomMargin = width <= 780 ? 0 : 40;
    assert.ok(sizes.bodyBottom >= sizes.viewport - 1, `${route}: wallpaper covers the viewport at ${width}px`);
    assert.ok(Math.abs(sizes.panelBottom + bottomMargin - sizes.viewport) <= 1, `${route}: panel fills the available height at ${width}px`);
    assert.ok(Math.abs(sizes.pageBottom - sizes.viewport) <= 1 && sizes.scrollHeight <= sizes.viewport + 1,
      `${route}: no unnecessary page scrolling at ${width}px`);
  };
  const cases = [
    ['?section=announcements', ['.message-center'], '.message-center .announcement-copy-content'],
    ['?section=messages', ['.message-center'], '.message-center .message-empty'],
    ['?section=home', ['.launcher-home'], '.launcher-home h1'],
    ['u/surface-fixture', ['.profile-identity','.profile-aside','.profile-game-data-card'], '.profile-bio'],
    ['uptime', ['.uptime-page'], '.uptime-heading'],
  ];
  for (const width of [1440, 390]) {
    if (width === 390) await page.evaluate(() => localStorage.setItem('eagler.book.theme', JSON.stringify('light')));
    await page.setViewport({width,height:900,isMobile:width===390,hasTouch:width===390});
    for (const [route, surfaces, content] of cases) {
      await page.goto(new URL(route, base), {waitUntil:'domcontentloaded'});
      await page.waitForSelector(content, {visible:true});
      await page.waitForFunction(() => document.documentElement.hasAttribute('data-custom-background'));
      if (route === '?section=announcements') {
        const toggle = '.message-announcements .announcement-more[aria-expanded="false"]';
        await page.locator(toggle).click();
        await page.waitForSelector('.message-announcements .announcement-more[aria-expanded="true"]', {visible:true});
        await page.locator('.message-announcements .announcement-more[aria-expanded="true"]').click();
      }
      if (route === '?section=messages') await checkMessagePageHeight(route, width);
      for (const panel of [35, 88, 100]) {
        await settings('grid', panel);
        for (const selector of surfaces) {
          const {alpha, opacity} = await paint(selector);
          assert.ok(Math.abs(alpha - panel * 2.55) < 1, `${route} ${selector} at ${width}px uses ${panel}%`);
          assert.equal(opacity, '1', 'foreground does not fade with the wallpaper');
        }
      }
      await settings('plain', 88);
      for (const selector of surfaces) assert.equal((await paint(selector)).alpha, 0, `${route} keeps its plain-theme layout`);
      assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false, `${route} fits ${width}px`);
    }
    const route = '?section=announcements';
    responses['/api/v1/announcements'].items = [];
    await page.goto(new URL(route, base), {waitUntil:'domcontentloaded'});
    await page.waitForSelector('.message-announcements .announcement-entry', {visible:true});
    await checkMessagePageHeight(route, width);
    const length = 12;
    responses['/api/v1/announcements'].items = Array.from({length}, (_, index) =>
      ({...announcement, id:index + 1, body:'Announcement preview.', publishedAt:announcement.publishedAt + index}));
    await page.goto(new URL(route, base), {waitUntil:'domcontentloaded'});
    await page.waitForFunction(count => document.querySelectorAll('.message-announcements .announcement-entry').length >= count, {}, length);
    const last = await page.$('.message-announcements .announcement-entry:last-child');
    await last.evaluate(node => node.scrollIntoView({block:'end'}));
    assert.ok(await last.evaluate(node => node.getBoundingClientRect().bottom <= innerHeight + 1));
    assert.ok(await page.evaluate(() => scrollY > 0), `long announcements scroll naturally at ${width}px`);
    responses['/api/v1/announcements'].items = [announcement];
  }
  console.log('PASS: announcements, messages, home, public profile and uptime use the shared reading surface on desktop/mobile');
} finally {await browser.close();}
