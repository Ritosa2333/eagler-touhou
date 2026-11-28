import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import puppeteer from 'puppeteer-core';
import {HOST_PROTOCOL} from '../../../common/contracts/product-catalog.mjs';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5185/');
const trigger = '.game-install-shortcut';
const installPage = '.game-install-page';
const catalog = {schema: 'eagler-touhou/release-catalog/1', games: {}};
const fixtures = new Map(), streams = new Map();
for (const game of ['th06', 'th07', 'th08']) {
  const path = `__queue_test/${game}.package.json`;
  const files = Object.fromEntries((game === 'th06' ? ['data','extra'] : ['data']).map((id, index) => {
    const source = `${game}-${id}.data`, bytes = Buffer.alloc((id === 'extra' ? 600 : 400)*1024, Number(game.slice(2)) + index);
    const sha256 = createHash('sha256').update(bytes).digest('hex');
    streams.set('/__queue_test/'+source, {game, id, bytes});
    return [id, {source, target: '/'+source, revision: 'queue-test', bytes: bytes.length, sha256}];
  }));
  const descriptor = {schema: 'eagler-touhou/package/1', game, revision: 'queue-test',
    runtimeRequirement: {protocol: HOST_PROTOCOL, target: game, dataFile: 'data', dataLayout: `sha256-${files.data.sha256}`},
    files, base: {files: Object.keys(files)}, components: {}};
  catalog.games[game] = {descriptor: path, revision: descriptor.revision};
  fixtures.set('/'+path, {contentType: 'application/json', body: JSON.stringify(descriptor)});
}
fixtures.set('/release-catalog.json', {contentType: 'application/json', body: JSON.stringify(catalog)});

const browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
const streamServer = createServer();
await new Promise(resolve => streamServer.listen(0,'127.0.0.1',resolve));
const streamOrigin = `http://127.0.0.1:${streamServer.address().port}`;
try {
  for (const [width, lang, labels] of [
    [1440, 'zh-CN', {install: '安装游戏', active: '安装中', queued: '排队中', start: '开始游戏'}],
    [1440, 'en', {install: 'Install game', active: 'Installing', queued: 'Queued', start: 'Start game'}],
    [320, 'zh-CN', {install: '安装游戏', active: '安装中', queued: '排队中', start: '开始游戏'}],
  ]) {
    const context = await browser.createBrowserContext();
    const gates = new Map(Array.from(streams.values(), ({game, id}) => [`${game}-${id}`, Promise.withResolvers()]));
    const firstChunk = Promise.withResolvers(), started = [], errors = [];
    const serve = async (request, response) => {
      const {game, id, bytes} = streams.get(request.url);
      started.push(`${game}-${id}`);
      if (game === 'th06' && id === 'data') await firstChunk.promise;
      response.writeHead(200, {'Access-Control-Allow-Origin':'*', 'Content-Type':'application/octet-stream', 'Content-Length':bytes.length});
      const partial = 300*1024;
      response.write(bytes.subarray(0,partial));
      await gates.get(`${game}-${id}`).promise;
      response.end(bytes.subarray(partial));
    };
    streamServer.on('request', serve);
    try {
      const page = await context.newPage();
      page.setDefaultTimeout(15000);
      page.on('pageerror', error => errors.push(error.message));
      await page.setViewport({width, height: 820, isMobile: width === 320, hasTouch: width === 320});
      await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
      await page.setRequestInterception(true);
      page.on('request', async request => {
        const url = new URL(request.url());
        if (url.origin === origin.origin && streams.has(url.pathname)) {
          await request.respond({status:302, headers:{location:streamOrigin+url.pathname}});
        } else if (url.origin === origin.origin && fixtures.has(url.pathname)) {
          await request.respond({status:200, ...fixtures.get(url.pathname)});
        } else await request.continue();
      });
      const waitText = (text, disabled) => page.waitForFunction((text, disabled) => {
        const action = document.querySelector('#launch');
        return action?.textContent.trim() === text && action.disabled === disabled && action.getBoundingClientRect().width > 0;
      }, {}, text, disabled);
      const launchWidth = async () => page.$eval('#launch', async action => {
        await document.fonts.ready;
        return action.offsetWidth;
      });
      const sameWidth = async (expected, state) => {
        assert.equal(await launchWidth(), expected, `${width}px/${lang}: primary width stays fixed when ${state}`);
      };
      const clickGame = game => page.locator(`[data-book-sidebar-product="${game}"]`).click();
      const noShortcut = () => page.waitForSelector(trigger, {hidden: true});
      const startedFile = async file => {
        const deadline = Date.now() + 10000;
        while (!started.includes(file) && Date.now() < deadline) await new Promise(resolve => setTimeout(resolve, 25));
        assert.ok(started.includes(file), `fixture request for ${file}`);
      };
      await page.goto(new URL(`?game=th06&lang=${lang}`, origin).href, {waitUntil: 'domcontentloaded'});
      await waitText(labels.install, false);
      const th06Width = await launchWidth();
      await noShortcut();
      await page.locator('#launch').click();
      await waitText(labels.active, true);
      await sameWidth(th06Width, 'acquiring');
      await page.waitForSelector(trigger, {visible: true});
      assert.equal(await page.$(installPage), null, 'adding a task stays on its game detail');
      await startedFile('th06-data');
      await page.waitForSelector('#launch [data-state="indeterminate"]');
      await page.locator(trigger).click();
      await page.waitForSelector(`${installPage} [data-download-active="th06"] [role="progressbar"]`, {visible:true});
      await page.waitForSelector(`${installPage} [data-loading-context="install"][data-compact] [data-loading-scene]`, {visible:true});
      assert.match(new URL(page.url()).search,/section=installs/);
      await noShortcut();
      firstChunk.resolve();
      await page.waitForSelector(`${installPage} [aria-valuenow="30"]`);
      await page.waitForSelector('[data-install-speed]');
      await page.waitForSelector('[data-install-eta]');
      await page.locator(`${installPage} .launcher-back`).click();
      await waitText(`${labels.active} 30%`, true);
      await sameWidth(th06Width, 'transferring');
      await page.waitForSelector('#launch [aria-valuenow="30"]');
      await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'reduce'}]);
      assert.equal(await page.$eval('#launch .ui-progress-indicator', node => getComputedStyle(node).transitionProperty), 'none');
      await page.emulateMediaFeatures([{name:'prefers-reduced-motion', value:'no-preference'}]);

      let th07Width;
      if (width === 1440) {
        await clickGame('th07');
        await waitText(labels.install, false);
        th07Width = await launchWidth();
        await page.locator('#launch').click();
        await waitText(labels.queued, true);
        await sameWidth(th07Width, 'queued');
        await clickGame('th08');
        await waitText(labels.install, false);
        await page.locator('#launch').click();
        await waitText(labels.queued, true);
        assert.deepEqual(started, ['th06-data']);
      }
      assert.equal(await page.$eval(trigger, node => {
        const button = node.getBoundingClientRect();
        const nav = document.querySelector('.launcher-bottom-navigation')?.getBoundingClientRect();
        return button.right <= innerWidth && button.bottom <= innerHeight &&
          (!nav?.height || button.bottom <= nav.top) &&
          node.contains(document.elementFromPoint(button.x + button.width / 2, button.y + button.height / 2));
      }), true, 'the floating button is reachable, including over a mobile game sheet');
      await page.locator(trigger).click();
      await page.waitForSelector(`${installPage} [data-download-active="th06"] [aria-valuenow="30"]`, {visible:true});
      assert.equal(await page.$eval(installPage, node => node.scrollWidth <= node.clientWidth && node.getBoundingClientRect().right <= innerWidth), true);
      if (width === 1440) {
        assert.deepEqual(await page.$$eval('[data-download-queued]', rows => rows.map(row => row.dataset.downloadQueued)), ['th07','th08']);
        await page.locator('[data-download-queued="th08"] button').click();
        gates.get('th06-data').resolve();
        await startedFile('th06-extra');
        await page.waitForSelector(`${installPage} [data-download-active="th06"] [aria-valuenow="70"]`);
        gates.get('th06-extra').resolve();
        await page.waitForSelector(`${installPage} [data-download-active="th07"]`);
        await startedFile('th07-data');
        assert.deepEqual(started, ['th06-data','th06-extra','th07-data']);
        gates.get('th07-data').resolve();
        await page.waitForSelector('.game-install-empty');
        await noShortcut();
        await page.locator(`${installPage} .launcher-back`).click();
        await waitText(labels.install, false);
        await clickGame('th07');
        await waitText(labels.start, false);
        await sameWidth(th07Width, 'installed');
        assert.equal(await page.$eval('#player', node => node.getAttribute('aria-hidden')), 'true', 'installing does not launch a game');
        await page.reload({waitUntil: 'domcontentloaded'});
        await waitText(labels.start, false);
        await noShortcut();
      } else {
        await page.locator('[data-download-active="th06"] button').click();
        await page.waitForSelector('.game-install-empty');
        await page.locator(`${installPage} .launcher-back`).click();
        await noShortcut();
        await waitText(labels.install, false);
        await sameWidth(th06Width, 'cancelled');
        await page.locator('#launch').click();
        await waitText(`${labels.active} 30%`, true);
        gates.get('th06-data').resolve();
        gates.get('th06-extra').resolve();
        await noShortcut();
        await waitText(labels.start, false);
        await sameWidth(th06Width, 'installed');
      }
      await page.goto(new URL(`?section=installs&lang=${lang}`, origin).href, {waitUntil:'domcontentloaded'});
      await page.waitForSelector('.game-install-empty', {visible:true});
      await page.locator(`${installPage} .launcher-back`).click();
      await page.waitForSelector(installPage, {hidden:true});
      assert.deepEqual(errors, []);
    } finally {
      streamServer.off('request', serve);
      firstChunk.resolve();
      for (const gate of gates.values()) gate.resolve();
      await context.close();
    }
  }
  console.log('PASS: in-place install progress, standalone install page, cumulative bytes, FIFO, cancel and launch action on desktop/mobile');
} finally {await browser.close(); streamServer.close();}
