import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:8131/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', error => errors.push(error.message));
  const fixtures = {
    '/api/ready': {ready:true},
    '/api/auth/get-session': {user:{username:'reimu', name:'Reimu'}},
    '/api/v1/profiles/reimu': {username:'reimu', nickname:'Reimu', joinedAt:'2025-01-01T00:00:00Z', details:null},
  };
  await page.setRequestInterception(true);
  page.on('request', request => {
    const fixture = fixtures[new URL(request.url()).pathname];
    if (fixture) void request.respond({contentType:'application/json', body:JSON.stringify(fixture)});
    else void request.continue();
  });
  await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
  await page.goto(origin, {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.launcher-page-surface');
  await page.waitForFunction(() => window.__eaglerBoot?.done === true && !document.querySelector('#eaglerPreload'));
  await page.evaluate(() => document.querySelector('#siteNotice:not([hidden]) #siteNoticeClose')?.click());
  await page.evaluate(() => {
    window.routeTransitions = [];
    const start = document.startViewTransition.bind(document);
    document.startViewTransition = (...args) => {
      const transition = start(...args);
      transition.ready.then(() => {
        const style = pseudo => {
          const value = getComputedStyle(document.documentElement, pseudo);
          return {name:value.animationName, duration:value.animationDuration};
        };
        window.routeTransitions.push({
          content:style('::view-transition-new(launcher-page)'),
          old:style('::view-transition-old(launcher-page)'),
          root:style('::view-transition-new(root)'),
          header:style('::view-transition-new(launcher-masthead)'),
          snapshots:[document.documentElement, document.querySelector('.book-masthead')].map(el => getComputedStyle(el).viewTransitionName),
          participants:[...document.querySelectorAll('body *')].filter(el => getComputedStyle(el).viewTransitionName === 'launcher-page').map(el => el.className),
          nestedAnimations:[...document.querySelectorAll('.sheet,.lobby-main,.player-profile')].map(el => getComputedStyle(el).animationName),
        });
      }).catch(error => window.routeTransitions.push({error:String(error)}));
      transition.finished.finally(() => {window.routeTransitionFinished = true;});
      return transition;
    };
  });
  async function openProfile() {
    await page.waitForSelector('.masthead-account-entry', {visible:true});
    await page.click('.masthead-account-entry');
    const profile = '.account-menu-panel a[href*="/u/"]';
    await page.waitForSelector(profile, {visible:true});
    await page.click(profile);
  }
  async function navigate(destination) {
    const compact = page.viewport().width <= 780;
    const count = await page.evaluate(() => {window.routeTransitionFinished = false; return window.routeTransitions.length;});
    if (destination === 'profile') await openProfile();
    else {
      if (compact) {
        await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
        await page.waitForSelector('#mastheadMenuToggle[aria-haspopup="dialog"]', {visible:true});
        await page.click('#mastheadMenuToggle');
        await page.waitForFunction(() => {
          const panel = document.querySelector('#mastheadMenuPanel[data-state="open"]');
          const transform = panel && getComputedStyle(panel).transform;
          return transform && (transform === 'none' || Math.abs(new DOMMatrixReadOnly(transform).m41) < .5);
        });
      }
      const selector = `${compact ? '#mastheadMenuPanel[data-state="open"] nav' : '.book-main-nav'} a:nth-child(${destination === 'library' ? 1 : 2})`;
      await page.waitForSelector(selector, {visible:true});
      await page.click(selector);
    }
    if (compact) {
      await page.waitForSelector(`.launcher-page-surface[data-page="${destination}"]`);
      assert.equal(await page.evaluate(() => window.routeTransitions.length), count, 'mobile changes live content without a snapshot');
    } else {
      await page.waitForFunction(count => window.routeTransitions.length > count && window.routeTransitionFinished, {}, count);
      const result = await page.evaluate(() => window.routeTransitions.at(-1));
      assert.deepEqual(result.content, {name:'launcher-route-in', duration:'0.16s'});
      assert.deepEqual(result.old, {name:'launcher-route-out', duration:'0.16s'});
      assert.equal(result.root.name, 'none');
      assert.equal(result.header.name, 'none');
      assert.deepEqual(result.snapshots, ['none', 'none']);
      assert.deepEqual(result.participants, ['launcher-page-surface']);
      assert.ok(result.nestedAnimations.every(name => name === 'none'));
    }
    if (destination === 'profile') await page.waitForSelector('.profile-wrap');
    const geometry = await page.evaluate(() => {
      const header = document.querySelector('.book-masthead').getBoundingClientRect();
      const container = document.querySelector('.book-library,.book-directory,.profile-wrap');
      const bounds = container.getBoundingClientRect(), style = getComputedStyle(container);
      return {
        header:[header.left,header.right],
        body:[document.body.getBoundingClientRect().left,document.body.getBoundingClientRect().right],
        content:[bounds.left + parseFloat(style.paddingLeft), bounds.right - parseFloat(style.paddingRight)],
        overflow:document.documentElement.scrollWidth > document.documentElement.clientWidth,
      };
    });
    return {...geometry,route:destination};
  }
  for (const width of [1440, 1024, 390]) {
    await page.setViewport({width, height:900});
    const bounds = [];
    // Every directed pair, including profile → library and profile → lobby.
    for (const destination of ['lobby','profile','library','profile','lobby','library']) bounds.push(await navigate(destination));
    const firstByRoute = new Map();
    for (const value of bounds) {
      const routeBaseline = firstByRoute.get(value.route);
      if (!routeBaseline) firstByRoute.set(value.route,value);
      assert.deepEqual(value.header,value.body,`masthead does not cover the body content width at ${width}px`);
      assert.deepEqual(value.header,bounds[0].header,`masthead width changes during navigation at ${width}px`);
      assert.deepEqual(value.body,bounds[0].body,`body width changes during navigation at ${width}px`);
      if (routeBaseline) assert.deepEqual(value.content,routeBaseline.content,`${value.route} content width changes when revisited at ${width}px`);
      assert.ok(Math.abs((value.content[0]+value.content[1])/2 - (value.body[0]+value.body[1])/2) < 1,`${value.route} content is not centered at ${width}px`);
      assert.equal(value.overflow, false);
    }
    console.log(`PASS ${width}px: six transitions, stable masthead and content bounds`);
  }
  await page.setViewport({width:1440, height:900});
  const before = await page.evaluate(() => {window.routeTransitionFinished=false;return window.routeTransitions.length;});
  await page.goBack();
  await page.waitForFunction(n => window.routeTransitions.length > n && window.routeTransitionFinished, {}, before);
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  await openProfile();
  await page.waitForFunction(() => document.querySelector('.launcher-page-surface').dataset.page === 'profile');
  assert.equal(await page.evaluate(() => getComputedStyle(document.documentElement,'::view-transition-new(launcher-page)').animationName), 'none');
  assert.deepEqual(errors, []);
  console.log('PASS Back and reduced motion');
} finally {await browser.close();}
