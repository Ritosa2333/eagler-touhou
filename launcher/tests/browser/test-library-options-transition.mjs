import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = process.env.LAUNCHER_URL || 'http://127.0.0.1:8131/';
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  for (const width of [1440, 390]) {
    const page = await browser.newPage();
    await page.setViewport({width, height:900});
    await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
    await page.goto(origin, {waitUntil:'domcontentloaded'});
    await page.waitForFunction(() => window.__eaglerBoot?.done && document.querySelector('.launcher-page-surface[data-page="library"]'));
    await page.evaluate(() => {
      window.pageSnapshots = [];
      const start = document.startViewTransition.bind(document);
      document.startViewTransition = (...args) => {
        const transition = start(...args);
        void transition.ready.then(() => {
          const root = document.documentElement;
          const participants = [root, ...document.querySelectorAll('.book-masthead,.launcher-page-surface')];
          window.pageSnapshots.push({
            names: participants.map(node => getComputedStyle(node).viewTransitionName).filter(name => name !== 'none'),
            duration: getComputedStyle(root, '::view-transition-new(launcher-page)').animationDuration,
          });
        }, () => {});
        return transition;
      };
    });
    const settle = () => page.waitForFunction(() => !document.documentElement.matches(':active-view-transition'));
    async function openOverview() {
      await page.waitForFunction(() => getComputedStyle(document.body).pointerEvents !== 'none');
      await page.click('#mastheadMenuToggle');
      await page.waitForFunction(() => {
        const panel = document.querySelector('#mastheadMenuPanel[data-state="open"]');
        const transform = panel && getComputedStyle(panel).transform;
        return transform && (transform === 'none' || Math.abs(new DOMMatrixReadOnly(transform).m41) < .5);
      });
    }
    async function navigate(destination) {
      const index = destination === 'library' ? 1 : 2;
      let selector = `.book-main-nav a:nth-child(${index})`;
      if (width < 780) {
        await openOverview();
        selector = `#mastheadMenuPanel[data-state="open"] nav a:nth-child(${index})`;
      }
      await page.click(selector);
      await page.waitForSelector(`.launcher-page-surface[data-page="${destination}"]`);
      await settle();
    }
    if (width < 780) {
      const url = page.url();
      await openOverview();
      assert.equal(page.url(), url);
      await page.goBack();
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
      assert.equal(page.url(), url);
      await navigate('lobby');
      await page.goBack();
      await page.waitForSelector('.launcher-page-surface[data-page="library"]');
      assert.equal(await page.$('#mastheadMenuPanel[data-state="open"]'), null, 'Back after a drawer link returns to the page without reopening the drawer');
      await page.waitForSelector('#mastheadMenuPanel', {hidden:true});
      await openOverview();
      await page.click('#mastheadMenuPanel [aria-labelledby="masthead-overview-site"] > button');
      await page.waitForSelector('#donationDialog[open]');
      assert.equal(await page.$('#mastheadMenuPanel'), null, 'The donation entry must not revive the drawer');
      await page.goBack();
      await page.waitForSelector('#donationDialog:not([open])');
      assert.equal(page.url(), url);
    }
    await navigate('lobby');
    await navigate('library');
    const pageSnapshots = () => page.evaluate(() => window.pageSnapshots);
    const snapshots = await pageSnapshots();
    assert.deepEqual(snapshots, width < 780 ? [] : Array.from({length:2}, () => ({names:['launcher-page'], duration:'0.16s'})));
    for (const exit of ['button', 'browser Back']) {
      await page.click('#bookGameGrid .book-game-link');
      await page.waitForSelector('.tools.is-open #libraryBack', {visible:true});
      if (exit === 'button') await page.click('#libraryBack');
      else await page.goBack();
      await page.waitForFunction(() => !new URLSearchParams(location.search).has('game') &&
        !document.body.classList.contains('library-tools-open'));
      await settle();
      assert.ok((await pageSnapshots()).slice(snapshots.length).every(({names}) => names.length === 0), `${exit} must not capture a page snapshot`);
      assert.equal(await page.evaluate(() => document.activeElement.matches('#bookGameGrid .book-game-link')), true);
    }
    if (width === 1440) {
      // Give automation time to send trusted clicks during an unfinished fade.
      await page.addStyleTag({content:':root {--launcher-route-duration: 2s !important}'});
      await page.evaluate(() => {
        window.clickedDuringFade = [];
        document.addEventListener('click', event => {
          const link = event.target.closest('.book-main-nav a');
          if (link && document.documentElement.matches(':active-view-transition')) {
            window.clickedDuringFade.push(link.matches('.book-lobby-link') ? 'lobby' : 'library');
          }
        }, true);
      });
      for (const [destination, selector] of [
        ['lobby', '.book-main-nav .book-lobby-link'],
        ['library', '.book-main-nav a:first-child'],
        ['lobby', '.book-main-nav .book-lobby-link'],
      ]) {
        const count = (await pageSnapshots()).length;
        await page.click(selector);
        await page.waitForFunction((destination, count) => window.pageSnapshots.length > count &&
          document.querySelector('.launcher-page-surface')?.dataset.page === destination, {timeout:4000}, destination, count);
      }
      assert.deepEqual(await page.evaluate(() => window.clickedDuringFade), ['library', 'lobby']);
    }
    await page.close();
    console.log(`PASS ${width}px: options close locally after lobby navigation`);
  }
} finally {await browser.close();}
