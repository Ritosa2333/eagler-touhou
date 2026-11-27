import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const base = new URL(process.argv[2] || 'http://127.0.0.1:5185/');
const rating = {rating:2100, interval_halfwidth:160, status:'active', games:50, wins:30,
  opponents:8, connected_group:1, last_at:Date.UTC(2026, 10, 25)};
const top = [{character:0,games:34},{character:1,games:12},{character:2,games:4}];
const scores = [{character:0,rating:2000,interval:180,games:34},{character:1,rating:2400,interval:190,games:12}];
const board = {as_of:rating.last_at, board:[
  {...rating, player:'Frequent', profile:{username:'frequent',nickname:'常用玩家'}, top_characters:top, character_ratings:scores},
  {...rating, player:'Private', rating:2050, top_characters:top},
  {...rating, player:'No data', rating:2000},
  {...rating, player:'One character', rating:1950, top_characters:top.slice(0,1), character_ratings:scores.slice(0,1)},
]};
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
  await page.evaluateOnNewDocument(() => {
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
    localStorage.setItem('eagler.background.settings', JSON.stringify({mode:'grid', panel:88}));
  });
  const accountPaths = [];
  await page.setRequestInterception(true);
  page.on('request', request => {
    const path = new URL(request.url()).pathname;
    if (/^\/assets\/th09-portraits\/\d+\.webp$/.test(path)) return request.respond({status:200, contentType:'image/svg+xml',
      body:'<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24"><circle cx="12" cy="12" r="10"/></svg>'});
    if (!path.startsWith('/api/')) return request.continue();
    accountPaths.push(path);
    const body = path === '/api/v1/th09/ratings/board' ? board : path === '/api/ready' ? {ready:true} : {user:null};
    return request.respond({status:200, contentType:'application/json', body:JSON.stringify(body)});
  });
  const header = '.rating-table th[data-column="main-characters"]';
  const rows = '.rating-table tbody > tr';
  const main = '[data-column="main-characters"] .rating-main-characters [data-character]';
  const compact = '.rating-main-characters-compact';
  const avatar = '.th09-character-avatar';
  const scope = '#rating-board-scope-trigger';
  const shown = selector => page.$eval(selector, node => node.getBoundingClientRect().width > 0);
  const checkAvatar = async (selector, size) => {
    const box = await page.$eval(selector, node => {
      const style = getComputedStyle(node);
      return [node.offsetWidth, node.offsetHeight, style.borderRadius, style.overflow, node.firstElementChild.offsetWidth > node.offsetWidth];
    });
    assert.deepEqual(box, [size, size, '50%', 'hidden', true]);
  };
  await page.setViewport({width:1707, height:819});
  await page.goto(new URL('ratings.html?lang=zh-CN', base), {waitUntil:'domcontentloaded'});
  await page.waitForSelector(main, {visible:true});
  assert.equal(await page.$eval('.rating-heading h1', h => h.textContent.trim()), '排行榜');
  await page.click('.rating-game-picker');
  const gameOption = '[role="menuitemradio"][data-game]';
  await page.waitForSelector(gameOption, {visible:true});
  assert.deepEqual(await page.$$eval('[data-rating-category]', (groups, selector) => groups.map(group =>
    [group.dataset.ratingCategory, group.getAttribute('aria-label'), [...group.querySelectorAll(selector)].map(option =>
      [option.dataset.game, option.hasAttribute('data-disabled'), option.getAttribute('aria-checked')])]), gameOption),
  [['versus','对战',[['th03',true,'false'], ['th09',false,'true'], ['th19',true,'false']]],
    ['fighting','格斗',[['th075',true,'false']]]]);
  const checkHorizontalGames = async () => {
    const boxes = await page.$$eval(gameOption, options => options.map(option => {
      const {x, y} = option.getBoundingClientRect();
      return {x, y};
    }));
    assert.ok(boxes.every(({x,y}, index) => y === boxes[0].y && (index === 0 || x > boxes[index - 1].x)));
    assert.equal(await page.$eval('.rating-game-menu', node => {
      const {left,right} = node.getBoundingClientRect();
      return left >= 0 && right <= innerWidth;
    }), true);
  };
  await checkHorizontalGames();
  await page.keyboard.press('Escape');
  assert.equal(await page.$eval(header, h => h.textContent.trim()), '惯用机体');
  assert.deepEqual(await page.$$eval(rows, found => found.map(row =>
    [...row.querySelectorAll('td[data-column="main-characters"] [data-character]')].map(icon => icon.dataset.character))), [['0','1','2'],[],[],['0']],
    'portraits follow games played, not WHR; hidden or absent values are omitted and short lists are not padded');
  assert.equal(await page.$$eval(main + ' img', images => images.length), 4);
  await checkAvatar(main + ' ' + avatar, 24);
  assert.match(await page.$eval(main, node => node.title), /34 局/);
  assert.equal(await shown(compact), false);
  assert.equal(await page.$(scope + ' ' + avatar), null);
  await page.click(scope);
  const option = '[role="option"]';
  await page.waitForSelector(option + ' ' + avatar, {visible:true});
  assert.deepEqual(await page.$$eval(option + ' ' + avatar + ' img', images => images.map(img => new URL(img.src).pathname)),
    Array.from({length:16}, (_, id) => '/assets/th09-portraits/' + id + '.webp'));
  await checkAvatar(option + '[data-value="0"] ' + avatar, 26);
  await page.keyboard.press('Escape');
  for (const width of [860, 390, 320]) {
    await page.setViewport({width, height:819});
    assert.equal(await shown(header), width > 800);
    assert.equal(await shown(compact), width <= 800);
    if (width <= 800) {
      assert.deepEqual(await page.$$eval(compact + ' [data-character]', icons => icons.map(icon => icon.dataset.character)), ['0','1','2','0']);
      await checkAvatar(compact + ' ' + avatar, 20);
    }
    assert.equal(await page.$eval('.rating-table-wrap', node => node.scrollWidth > node.clientWidth), false);
    assert.equal(await page.evaluate(() => document.documentElement.scrollWidth > innerWidth), false);
  }
  await page.click('.rating-game-picker');
  await page.waitForSelector(gameOption, {visible:true});
  await checkHorizontalGames();
  await page.keyboard.press('Escape');
  await page.goto(new URL('ratings.html?lang=zh-CN&character=0', base), {waitUntil:'domcontentloaded'});
  await page.waitForSelector('.rating-layout[data-character-board] .rating-character-score');
  await checkAvatar(scope + ' ' + avatar, 22);
  assert.equal(await page.$(header), null, 'character boards do not repeat the overall-board main character');
  assert.equal(await page.$(compact), null);
  assert.equal(accountPaths.some(path => path.startsWith('/api/v1/profiles/')), false, 'no per-player requests are needed');
  console.log('PASS: TH09 rankings show public most-played characters on desktop and mobile');
} finally {await browser.close();}
