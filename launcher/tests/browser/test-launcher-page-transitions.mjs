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
  assert.equal(await page.$('.site-footer'), null, 'the library has no site footer');
  assert.equal(await page.$('.library-announcements-trigger'), null, 'the retired announcement button cannot reappear');
  await page.waitForSelector('.masthead-inbox-action .lucide-megaphone', {visible:true});
  assert.equal(await page.$$eval('.masthead-inbox-action .lucide-megaphone', nodes => nodes.length), 1);
  await page.evaluate(() => {
    window.routeSnapshots = 0;
    window.routeFades = [];
    const animate = Element.prototype.animate;
    Element.prototype.animate = function (frames, timing) {
      const animation = animate.call(this, frames, timing);
      const opacity = animation.effect.getKeyframes().map(frame => frame.opacity);
      if (opacity.join(',') === '0.85,1') {
        const page = this.closest('.launcher-page-surface')?.dataset.page;
        window.routeFades.push({page, finished:animation.finished.catch(() => {})});
      }
      return animation;
    };
    if (!document.startViewTransition) return;
    const start = document.startViewTransition.bind(document);
    document.startViewTransition = (...args) => {
      window.routeSnapshots++;
      return start(...args);
    };
  });
  const settlePage = () => page.evaluate(async () => {
    await Promise.all(window.routeFades.map(({finished}) => finished));
  });
  async function openProfile() {
    const compact = page.viewport().width <= 780;
    if (compact && !await page.$('.launcher-page-surface[data-page="library"]')) {
      await page.click('.launcher-bottom-navigation a:first-child');
      await page.waitForSelector('.launcher-page-surface[data-page="library"]');
      await settlePage();
    }
    const trigger = compact ? '#mastheadMenuToggle' : '.masthead-account-entry';
    await page.waitForSelector(trigger, {visible:true});
    await page.click(trigger);
    const profile = compact ? '#mastheadMenuPanel .launcher-account-link' : '.account-menu-panel a[href*="/u/"]';
    const link = await page.waitForSelector(profile, {visible:true});
    await page.waitForFunction(node => {
      const {x,y,width,height} = node.getBoundingClientRect();
      return node.contains(document.elementFromPoint(x+width/2,y+height/2));
    }, {}, link);
    const before = await page.evaluate(() => window.routeFades.length);
    await link.click();
    return before;
  }
  async function navigate(destination) {
    await settlePage();
    const compact = page.viewport().width <= 780;
    const currentPage = () => page.$eval('.launcher-page-surface', node => node.dataset.page);
    let backCount;
    // The compact player profile uses its own header; use browser history to
    // return to the launcher before using the persistent primary controls.
    if (compact && await page.$('.launcher-page-surface[data-page="profile"]')) {
      backCount = await page.evaluate(() => window.routeFades.length);
      await page.goBack();
      await page.waitForSelector('.launcher-bottom-navigation a', {visible:true});
      await settlePage();
    }
    let before;
    if (backCount !== undefined && await currentPage() === destination) before = backCount;
    else if (destination === 'profile') before = await openProfile();
    else {
      before = await page.evaluate(() => window.routeFades.length);
      const selector = `${compact ? '.launcher-bottom-navigation' : '.book-main-nav'} a:nth-child(${destination === 'library' ? 1 : 2})`;
      await page.waitForSelector(selector, {visible:true});
      await page.click(selector);
    }
    await page.waitForSelector(`.launcher-page-surface[data-page="${destination}"]`);
    await settlePage();
    assert.deepEqual(await page.evaluate(count => window.routeFades.slice(count).map(fade => fade.page), before),
      [destination], `${destination} reveals only the incoming page at ${page.viewport().width}px`);
    if (destination === 'profile') await page.waitForSelector('.profile-wrap');
    if (destination === 'lobby') {
      if (compact) {
        await page.waitForSelector('.lobby-library .book-lobby-picker-trigger', {visible:true});
        await page.waitForSelector('#lobbyGameRail', {visible:true});
        assert.equal(await page.$eval('.book-directory', lobby => {
          const bounds = selector => lobby.querySelector(selector).getBoundingClientRect();
          const more = bounds('.lobby-tools-more'), bar = bounds('.book-lobby-games'), tools = bounds('.lobby-room-tools');
          const filter = bounds('.book-room-filter-trigger');
          const list = bounds('.book-lobby-picker-trigger'), all = bounds('.book-lobby-all');
          return bar.bottom <= tools.top && filter.right <= more.left && filter.top === more.top && more.width === 44 &&
            list.width === list.height && all.width === all.height && list.width === all.width && list.top === all.top;
        }), true, 'mobile lobby keeps its overflow trigger and filter aligned below the game bar');
      } else {
        await page.waitForSelector('.lobby-library .book-lobby-picker-trigger', {hidden:true});
        const sample = await page.waitForSelector('.lobby-game-sidebar [data-lobby-rail-product]', {visible:true});
        assert.equal(await sample.evaluate(row => {
          const box = selector => row.querySelector(selector).getBoundingClientRect();
          const game = box('.book-rail-game'), check = box('.book-lobby-sidebar-check'), favorite = box('.book-favorite');
          return check.right <= game.left && game.right <= favorite.left && check.height === game.height && favorite.height === game.height;
        }), true, 'multi-select, work and favorite have three independent, full-height targets');
      }
    }
    const geometry = await page.evaluate(() => {
      const header = document.querySelector('.book-masthead').getBoundingClientRect();
      const container = document.querySelector('.launcher-page-surface[data-page=library] .book-library,.launcher-page-surface[data-page=lobby] .book-directory,.launcher-page-surface[data-page=profile] .profile-wrap');
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
  const sidebarGeometry = () => page.$eval('.book-sidebar:not([style*="display: none"])', rail => {
    const all = rail.querySelector('[data-book-category-group="all"]');
    const category = all.querySelector('.book-category-row');
    const choice = category.querySelector('.book-category-choice');
    const row = rail.querySelector(':is([data-book-sidebar-product="th07"],[data-lobby-rail-product="th07mp"] .book-rail-game)');
    const favorite = row.parentElement.querySelector('.book-favorite');
    const subtitle = row.querySelector('.book-category-game-subtitle');
    const box = element => element.getBoundingClientRect();
    choice.focus({preventScroll:true});
    return {
      width:box(rail).width, searchWidth:box(rail.querySelector('input')).width,
      categoryWidth:box(category).width, actionWidth:[...category.querySelectorAll('button')].reduce((width,button) => width + box(button).width,0),
      countRight:box(category.querySelector('.book-category-count')).right,
      rowHeight:box(row).height, layout:getComputedStyle(row).display,
      subtitleClearsFavorite:box(subtitle).top >= box(favorite).bottom - 1 || box(subtitle).right <= box(favorite).left,
    };
  });
  for (const width of [1440, 1024, 390]) {
    await page.setViewport({width, height:900});
    const bounds = [];
    // Every directed pair, including profile → library and profile → lobby.
    for (const destination of ['lobby','profile','library','profile','lobby','library']) bounds.push(await navigate(destination));
    if (width > 780) {
      const library = await sidebarGeometry();
      await navigate('lobby');
      const lobby = await sidebarGeometry();
      for (const key of ['width','searchWidth','categoryWidth','actionWidth','countRight','rowHeight','layout'])
        assert.equal(lobby[key],library[key],`desktop rails share ${key}`);
      assert.ok(library.subtitleClearsFavorite && lobby.subtitleClearsFavorite,'favorite leaves the subtitle unobstructed');
      assert.equal(library.actionWidth,library.categoryWidth,'the category controls fill the row');
      await navigate('library');
    }
    const firstByRoute = new Map();
    for (const value of bounds) {
      const routeBaseline = firstByRoute.get(value.route);
      if (!routeBaseline) firstByRoute.set(value.route,value);
      if (width <= 780 && value.route !== 'library') {
        assert.deepEqual(value.header,[0,0],`mobile ${value.route} exposes the masthead`);
        continue;
      }
      assert.deepEqual(value.header,value.body,`masthead does not cover the body content width at ${width}px`);
      if (width > 780) assert.deepEqual(value.header,bounds[0].header,`masthead width changes during navigation at ${width}px`);
      assert.deepEqual(value.body,bounds[0].body,`body width changes during navigation at ${width}px`);
      if (routeBaseline) assert.deepEqual(value.content,routeBaseline.content,`${value.route} content width changes when revisited at ${width}px`);
      assert.ok(Math.abs((value.content[0]+value.content[1])/2 - (value.body[0]+value.body[1])/2) < 1,`${value.route} content is not centered at ${width}px`);
      assert.equal(value.overflow, false);
    }
    console.log(`PASS ${width}px: six transitions, stable masthead and content bounds`);
  }
  for (const width of [320, 390]) {
    await page.setViewport({width, height:720});
    const bottom = '.launcher-bottom-navigation';
    await page.waitForSelector(bottom, {visible:true});
    await page.evaluate(selector => {
      window.initialBottomNavigation = document.querySelector(selector);
      window.initialBottomBounds = window.initialBottomNavigation.getBoundingClientRect().toJSON();
    }, bottom);
    for (const [index, route] of [[3,'home'], [2,'lobby'], [1,'library'], [2,'lobby'], [3,'home'], [1,'library']]) {
      await page.click(`${bottom} a:nth-child(${index})`);
      await page.waitForSelector(`.launcher-page-surface[data-page="${route}"]`);
      const state = await page.evaluate(selector => {
        const node = document.querySelector(selector);
        return {
          sameNode: node === window.initialBottomNavigation,
          bounds: node.getBoundingClientRect().toJSON(),
          expected: window.initialBottomBounds,
          visible: getComputedStyle(node).visibility === 'visible',
          mastheadHeight: document.querySelector('.book-masthead').getBoundingClientRect().height,
          pageTop: document.querySelector('.launcher-page-surface').getBoundingClientRect().top,
        };
      }, bottom);
      assert.equal(state.sameNode, true, `${route} remounted the bottom navigation`);
      assert.equal(state.visible, true, `${route} hid the bottom navigation`);
      assert.deepEqual(state.bounds, state.expected, `${route} shifted the bottom navigation`);
      if (route === 'library') {
        assert.equal(state.mastheadHeight,56);
        await page.waitForSelector('.book-masthead .book-compact-search input', {visible:true});
      } else {
        assert.equal(state.mastheadHeight,0,`${route} shows a mobile masthead`);
        assert.equal(state.pageTop,0,`${route} leaves room for a hidden masthead`);
      }
      if (route !== 'lobby') await page.waitForFunction(() => document.querySelectorAll('#main').length === 1);
    }
    await page.evaluate(() => window.scrollTo({top:180,behavior:'instant'}));
    await page.waitForFunction(() => window.scrollY >= 140);
    assert.equal(await page.$eval('.book-masthead', element => element.getBoundingClientRect().bottom <= 1), true,
      `the mobile library masthead scrolls out of view at ${width}px`);
    assert.deepEqual(await page.$eval(bottom, element => element.getBoundingClientRect().toJSON()),
      await page.evaluate(() => window.initialBottomBounds), 'scrolling does not move the bottom navigation');
    await page.evaluate(() => window.scrollTo({top:0,behavior:'instant'}));
    console.log(`PASS ${width}px: library, lobby and home share fixed bottom navigation`);
  }
  async function switchPrimaryPages() {
    return page.evaluate(async () => {
      const surface = document.querySelector('.launcher-page-surface');
      const tabs = document.querySelectorAll('.launcher-bottom-navigation a');
      const before = window.routeSnapshots, fadeCount = window.routeFades.length;
      for (const [tab, destination] of [[1,'lobby'], [2,'home'], [0,'library']]) {
        const entered = new Promise(resolve => {
          const observer = new MutationObserver(() => {
            if (surface.dataset.page !== destination) return;
            observer.disconnect();
            resolve();
          });
          observer.observe(surface, {attributes:true, attributeFilter:['data-page']});
        });
        tabs[tab].click();
        await entered;
      }
      const fades = window.routeFades.slice(fadeCount);
      await Promise.all(fades.map(({finished}) => finished));
      const library = surface.querySelector('.book-content');
      return {
        snapshots: window.routeSnapshots - before,
        pages: fades.map(fade => fade.page),
        remaining: library.getAnimations().length,
        opacity: getComputedStyle(library).opacity,
      };
    });
  }
  assert.deepEqual(await switchPrimaryPages(),{snapshots:0,pages:['lobby','home','library'],remaining:0,opacity:'1'},
    'each fast tab switch cleans up the previous transition');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  assert.deepEqual(await switchPrimaryPages(),{snapshots:0,pages:[],remaining:0,opacity:'1'},
    'fast tab switches respect reduced motion');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  console.log('PASS rapid mobile switching with and without motion');
  await page.setViewport({width:1440, height:900});
  await page.goBack();
  await page.waitForSelector('.launcher-page-surface');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  const reducedProfileBefore = await openProfile();
  await page.waitForFunction(() => document.querySelector('.launcher-page-surface').dataset.page === 'profile');
  await settlePage();
  assert.equal(await page.evaluate(() => window.routeFades.length), reducedProfileBefore);
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.click('.masthead-inbox-action[data-inbox-section="announcements"]');
  await page.click('.masthead-inbox-panel a[href*="section=announcements"]');
  await page.waitForSelector('.launcher-page-surface[data-page="announcements"]');
  await settlePage();
  assert.equal(await page.evaluate(() => window.routeFades.at(-1).page), 'announcements');
  const messageBefore = await page.evaluate(() => window.routeFades.length);
  await page.evaluate(() => document.body.classList.add('less-motion'));
  await page.click('.message-center-navigation a[href*="section=messages"]');
  await page.waitForSelector('.launcher-page-surface[data-page="messages"]');
  await settlePage();
  assert.equal(await page.evaluate(() => window.routeFades.length), messageBefore);
  assert.equal(await page.evaluate(() => window.routeSnapshots), 0);
  assert.deepEqual(errors, []);
  console.log('PASS Back, message sections and both reduced-motion settings');
} finally {await browser.close();}
