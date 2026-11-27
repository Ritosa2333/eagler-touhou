import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import puppeteer from 'puppeteer-core';
import {HOST_PROTOCOL} from '../../../common/contracts/product-catalog.mjs';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const origin = new URL(process.argv[2] || 'http://127.0.0.1:5185/');
const trigger = '.game-download-trigger';
const dialog = '.game-download-dialog';
const catalog = {schema: 'eagler-touhou/release-catalog/1', games: {}};
const fixtures = new Map();
for (const game of ['th06', 'th07', 'th08']) {
  const source = `${game}.data`, path = `__queue_test/${game}.package.json`;
  const bytes = Buffer.from(`queue-fixture:${game}`);
  const sha256 = createHash('sha256').update(bytes).digest('hex');
  const descriptor = {schema: 'eagler-touhou/package/1', game, revision: 'queue-test',
    runtimeRequirement: {protocol: HOST_PROTOCOL, target: game, dataFile: 'data', dataLayout: `sha256-${sha256}`},
    files: {data: {source, target: '/'+source, revision: 'queue-test', bytes: bytes.length, sha256}},
    base: {files: ['data']}, components: {}};
  catalog.games[game] = {descriptor: path, revision: descriptor.revision};
  fixtures.set('/'+path, {contentType: 'application/json', body: JSON.stringify(descriptor)});
  fixtures.set('/__queue_test/'+source, {contentType: 'application/octet-stream', body: bytes});
}
fixtures.set('/release-catalog.json', {contentType: 'application/json', body: JSON.stringify(catalog)});

const browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
try {
  for (const [width, lang, labels] of [
    [1440, 'zh-CN', {install: '安装游戏', active: '获取中', queued: '排队中', start: '开始游戏'}],
    [1440, 'en', {install: 'Install game', active: 'Getting game', queued: 'Queued', start: 'Start game'}],
    [320, 'zh-CN', {install: '安装游戏', active: '获取中', queued: '排队中', start: '开始游戏'}],
  ]) {
    const context = await browser.createBrowserContext();
    const gates = new Map();
    try {
      const page = await context.newPage();
      const started = [], errors = [];
      for (const game of ['th06','th07','th08']) gates.set(game, Promise.withResolvers());
      page.setDefaultTimeout(15000);
      page.on('pageerror', error => errors.push(error.message));
      await page.setViewport({width, height: 820, isMobile: width === 320, hasTouch: width === 320});
      await page.evaluateOnNewDocument(() => localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1'));
      await page.setRequestInterception(true);
      page.on('request', async request => {
        const url = new URL(request.url());
        const fixture = url.origin === origin.origin && fixtures.get(url.pathname);
        if (!fixture) {await request.continue(); return;}
        const id = url.pathname.match(/\/__queue_test\/(th\d+)\.data$/)?.[1];
        if (id) {started.push(id); await gates.get(id).promise;}
        if (!request.isInterceptResolutionHandled()) await request.respond({status: 200, ...fixture});
      });
      const waitText = (text, disabled) => page.waitForFunction((text, disabled) => {
        const action = document.querySelector('#launch');
        return action?.textContent.trim() === text && action.disabled === disabled && action.getBoundingClientRect().width > 0;
      }, {}, text, disabled);
      const launchWidth = async () => page.$eval('#launch', async action => {
        await document.fonts.ready;
        return action.getBoundingClientRect().width;
      });
      const sameWidth = async (expected, state) => {
        assert.equal(await launchWidth(), expected, `${width}px/${lang}: primary width stays fixed when ${state}`);
      };
      const clickGame = game => page.locator(`[data-book-sidebar-product="${game}"]`).click();
      const noTask = () => page.waitForFunction((trigger, dialog) => !document.querySelector(trigger) && !document.querySelector(dialog), {}, trigger, dialog);
      const startedFile = async game => {
        const deadline = Date.now() + 10000;
        while (!started.includes(game) && Date.now() < deadline) await new Promise(resolve => setTimeout(resolve, 25));
        assert.ok(started.includes(game), `fixture request for ${game}`);
      };
      await page.goto(new URL(`?game=th06&lang=${lang}`, origin).href, {waitUntil: 'domcontentloaded'});
      await waitText(labels.install, false);
      const th06Width = await launchWidth();
      await noTask();
      await page.locator('#launch').click();
      await waitText(labels.active, true);
      await sameWidth(th06Width, 'acquiring');
      await page.waitForSelector(trigger, {visible: true});
      assert.equal(await page.$(dialog), null, 'adding a task never opens its popup');
      await startedFile('th06');

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
        assert.deepEqual(started, ['th06']);
      }
      assert.equal(await page.$eval(trigger, node => {
        const button = node.getBoundingClientRect();
        const nav = document.querySelector('.launcher-bottom-navigation')?.getBoundingClientRect();
        return button.right <= innerWidth && button.bottom <= innerHeight &&
          (!nav?.height || button.bottom <= nav.top) &&
          node.contains(document.elementFromPoint(button.x + button.width / 2, button.y + button.height / 2));
      }), true, 'the floating button is reachable, including over a mobile game sheet');
      await page.locator(trigger).click();
      await page.waitForSelector(`${dialog} [data-download-active="th06"] [role="progressbar"]`, {visible: true});
      await page.waitForSelector(`${dialog} [data-loading-context="install"][data-compact] img`, {visible:true});
      assert.equal(await page.$$eval(`${dialog} [data-loading-context="install"] img`, images => new Set(images.map(image=>image.src)).size),4);
      if (width === 1440) {
        assert.deepEqual(await page.$$eval('[data-download-queued]', rows => rows.map(row => row.dataset.downloadQueued)), ['th07','th08']);
        await page.locator('[data-download-queued="th08"] button').click();
        await page.locator(`${dialog} .ui-kit-dialog-close`).click();
        await page.waitForSelector(dialog, {hidden: true});
        await page.locator(trigger).click();
        await page.waitForSelector('[data-download-queued="th07"]');
        gates.get('th06').resolve();
        await page.waitForSelector(`${dialog} [data-download-active="th07"]`);
        await startedFile('th07');
        assert.deepEqual(started, ['th06','th07']);
        gates.get('th07').resolve();
        await noTask();
        await clickGame('th07');
        await waitText(labels.start, false);
        await sameWidth(th07Width, 'installed');
        assert.equal(await page.$eval('#player', node => node.getAttribute('aria-hidden')), 'true', 'installing does not launch a game');
        await page.reload({waitUntil: 'domcontentloaded'});
        await waitText(labels.start, false);
        await noTask();
      } else {
        await page.locator('[data-download-active="th06"] button').click();
        await noTask();
        await waitText(labels.install, false);
        await sameWidth(th06Width, 'cancelled');
        await page.locator('#launch').click();
        await waitText(labels.active, true);
        gates.get('th06').resolve();
        await noTask();
        await waitText(labels.start, false);
        await sameWidth(th06Width, 'installed');
      }
      assert.deepEqual(errors, []);
    } finally {
      for (const gate of gates.values()) gate.resolve();
      await context.close();
    }
  }
  console.log('PASS: per-game acquisition, FIFO, on-demand progress dialog, cancellation, and installed action on desktop/mobile');
} finally {await browser.close();}
