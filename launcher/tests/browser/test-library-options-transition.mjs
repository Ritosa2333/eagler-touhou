import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:8131/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  for (const width of [1440, 390]) {
    const page = await browser.newPage();
    const compact = width < 780;
    const surface = '.launcher-page-surface';
    const primaryNav = compact ? '.launcher-bottom-navigation' : '.book-main-nav';
    const gameLink = '#bookGameGrid .book-game-link';
    const detailBack = compact ? '.mobile-game-view #libraryBack' : '.book-game-detail-back';
    const errors = [];
    page.on('pageerror', error => errors.push(error.message));
    await page.setViewport({width, height:900});
    await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
    await page.goto(origin, {waitUntil:'domcontentloaded'});
    await page.waitForFunction(() => window.__eaglerBoot?.done && document.querySelector('.launcher-page-surface[data-page="library"]'));
    await page.$eval(surface, node => {
      const state = window.navigationProbe = {surface:node, fades:[], railFades:[], paneFades:[], snapshots:0, clickedDuringFade:[]};
      const animate = Element.prototype.animate;
      Element.prototype.animate = function (keyframes, options) {
        const animation = animate.call(this, keyframes, options);
        if (this.matches('.book-content, .lobby-rooms-column')) state.fades.push({
          target:this, page:this.closest('.launcher-page-surface').dataset.page, duration:animation.effect.getTiming().duration,
        });
        if (this.matches('.book-sidebar nav')) state.railFades.push({
          page:this.closest('.launcher-page-surface').dataset.page, categories:this.children.length,
        });
        if (this.matches('.book-game-detail, .book-content-main')) state.paneFades.push({
          detail:this.matches('.book-game-detail'), frames:animation.effect.getKeyframes(), duration:animation.effect.getTiming().duration,
        });
        return animation;
      };
      if (document.startViewTransition) {
        const start = document.startViewTransition.bind(document);
        document.startViewTransition = (...args) => {
          state.snapshots++;
          return start(...args);
        };
      }
      document.addEventListener('click', event => {
        const link = event.target.closest('.book-main-nav a');
        if (link && state.fades.at(-1)?.target.getAnimations().some(animation => animation.playState === 'running')) {
          state.clickedDuringFade.push(link.matches('.book-lobby-link') ? 'lobby' : 'library');
        }
      }, true);
    });
    const click = async selector => {
      await page.waitForSelector(selector, {visible:true});
      await page.click(selector);
    };
    const settle = () => page.$eval(surface, async node => {
      await Promise.allSettled(node.getAnimations({subtree:true}).filter(animation => animation.effect?.target.matches('.book-content, .lobby-rooms-column, .book-sidebar nav')).map(animation => animation.finished));
    });
    const counts = () => page.evaluate(() => ({
      fades:window.navigationProbe.fades.length, snapshots:window.navigationProbe.snapshots,
    }));
    async function navigate(destination) {
      const index = destination === 'library' ? 1 : 2;
      await click(primaryNav + ' a:nth-child(' + index + ')');
      await page.waitForSelector(surface + '[data-page="' + destination + '"]');
    }
    async function openOverview() {
      await page.waitForFunction(() => getComputedStyle(document.body).pointerEvents !== 'none');
      await click('#mastheadMenuToggle');
      await page.waitForSelector('#mastheadMenuPanel[data-state="open"]', {visible:true});
    }
    if (compact) {
      const url = page.url();
      await openOverview();
      assert.equal(page.url(), url);
      await page.goBack();
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
      assert.equal(page.url(), url);
      await openOverview();
      await click('#mastheadMenuPanel [aria-labelledby="masthead-overview-site"] button');
      await page.waitForSelector('#donationDialog[open]', {visible:true});
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
      await page.goBack();
      await page.waitForSelector('#donationDialog:not([open])');
      assert.equal(page.url(), url);
    }
    await navigate('lobby');
    await navigate('library');
    await settle();
    assert.deepEqual(await page.evaluate(() => {
      const probe = window.navigationProbe;
      return {
        fades:probe.fades.map(({target, page, duration}) => ({local:target !== probe.surface, page, duration})),
        railFades:probe.railFades.map(({page, categories}) => ({page, populated:categories > 1})),
        snapshots:probe.snapshots,
      };
    }), {fades:['lobby','library'].map(page => ({local:true, page, duration:180})),
      railFades:compact ? [] : ['lobby','library'].map(page => ({page, populated:true})), snapshots:0});

    const beforeOptions = await counts();
    const shelf = await page.$('#bookGameGrid');
    for (const exit of ['button', 'browser Back']) {
      const beforeEntry = await page.evaluate(() => window.navigationProbe.paneFades.length);
      await click(gameLink);
      await click('#gameSettingsOpen');
      if (!compact) assert.equal(await page.evaluate(count => window.navigationProbe.paneFades.slice(count)
        .some(({detail, frames}) => detail && Number.parseFloat(frames[0].translate) === 8),beforeEntry),true);
      await page.waitForSelector('.game-settings-page:not([hidden])', {visible:true});
      if (exit === 'button') await click('#libraryBack');
      else await page.goBack();
      await page.waitForSelector('#gameSettingsOpen', {visible:true});
      await page.waitForSelector('.game-settings-page:not([hidden])', {hidden:true});
      assert.equal(await page.evaluate(() => document.activeElement.id), 'gameSettingsOpen');

      const oldFades = await page.evaluate(() => window.navigationProbe.paneFades.length);
      if (exit === 'button') await click(detailBack);
      else await page.goBack();
      await page.waitForFunction(() => !new URLSearchParams(location.search).has('game'));
      await page.waitForSelector(gameLink, {visible:true});
      await page.waitForFunction(selector => document.activeElement.matches(selector), {timeout:3000}, gameLink);
      if (!compact) {
        assert.equal(await page.evaluate(count => window.navigationProbe.paneFades.slice(count)
          .some(({detail, frames}) => !detail && Number.parseFloat(frames[0].translate) === -8),oldFades),true);
        assert.equal(await shelf.evaluate(grid => grid === document.querySelector('#bookGameGrid')),true);
      }
    }
    assert.deepEqual(await counts(), beforeOptions, 'Game detail and settings should transition within the library surface.');

    if (!compact) {
      await page.evaluate(() => {window.navigationProbe.clickedDuringFade.length = 0;});
      await page.addStyleTag({content:'.launcher-page-surface {--ui-surface-duration: 2s}'});
      for (const destination of ['lobby', 'library', 'lobby']) {
        const {fades} = await counts();
        await navigate(destination);
        await page.waitForFunction((count, page) =>
          window.navigationProbe.fades[count]?.page === page, {}, fades, destination);
      }
      await settle();
      assert.deepEqual(await page.evaluate(() => {
        const {surface, clickedDuringFade, snapshots} = window.navigationProbe;
        return {clickedDuringFade, snapshots, opacity:getComputedStyle(surface).opacity, remaining:surface.getAnimations().length};
      }), {clickedDuringFade:['library', 'lobby'], snapshots:0, opacity:'1', remaining:0});
    }
    assert.deepEqual(errors, []);
    await page.close();
    console.log('PASS ' + width + 'px: primary navigation and local settings returns');
  }
} finally {await browser.close();}
