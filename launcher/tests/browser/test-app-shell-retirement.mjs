// Upgrade from the committed old cache-first worker using its actual Launcher
// lifecycle. One idle tab upgrades while another active tab keeps its document.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {APP_SHELL_RETIREMENT_WORKER} from '../../lib/app-shell-retirement.mjs';
import {readUiHistoryText} from '../support/ui-history.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const prior = path => readUiHistoryText('8bdf6dbf4a81032f0725cc841b86d03e1465fe07', path);
const client = await build({stdin: {contents: prior('src/launcher/app-shell-client.mts') + `
window.busy = true;
window.shell = createAppShellClient({shouldDeferReload: () => window.busy});
`, loader: 'ts'}, bundle: true, format: 'iife', write: false});
const oldHtml = '<!doctype html><body>OLD LAUNCHER<script src="client.js"></script></body>';
const digest = value => createHash('sha256').update(value).digest('hex');
const oldWorker = prior('src/app-shell-sw.js').replaceAll('__APP_SHELL_BUILD_ID__', '12345678901234567890')
  .replace('__APP_SHELL_DEFERRED_PATHS__', '[]').replace('self.__WB_MANIFEST', JSON.stringify([
    {url: './', revision: digest(oldHtml)}, {url: 'client.js', revision: digest(client.outputFiles[0].contents)},
  ]));
let upgraded = false;
const server = createServer((request, response) => {
  const path = new URL(request.url, 'http://localhost').pathname;
  response.setHeader('Cache-Control', 'no-store');
  if (path.endsWith('/app-shell-sw.js')) {
    response.setHeader('Content-Type', 'text/javascript'); response.end(upgraded ? APP_SHELL_RETIREMENT_WORKER : oldWorker);
  } else if (path.endsWith('/client.js')) {
    response.setHeader('Content-Type', 'text/javascript'); response.end(client.outputFiles[0].contents);
  } else if (path.endsWith('/game.html')) {
    response.setHeader('Content-Type', 'text/html'); response.end('<body>RUNNING GAME</body>');
  } else {
    response.setHeader('Content-Type', 'text/html'); response.end(upgraded ? '<!doctype html><body>NEW REACT</body>' : oldHtml);
  }
});
await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
let browser;
try {
  browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
  for (const mount of ['/', '/nested/']) {
    upgraded = false;
    const base = `http://127.0.0.1:${server.address().port}${mount}`;
    const idle = await browser.newPage(), busy = await browser.newPage();
    await idle.goto(base);
    await idle.evaluate(async () => {await window.shell.ready; await navigator.serviceWorker.ready;});
    await idle.reload(); await busy.goto(base);
    await Promise.all([idle, busy].map(page => page.waitForFunction(() => !!navigator.serviceWorker.controller && !!window.shell)));
    await busy.evaluate(async () => {
      localStorage.setItem('save-sentinel', 'keep');
      const cache = await caches.open('eagler-touhou-language-packs-v1'); await cache.put('/sentinel', new Response('keep'));
      const request = indexedDB.open('retirement-save-sentinel', 1);
      await new Promise((yes, no) => {request.onupgradeneeded = () => request.result.createObjectStore('save'); request.onsuccess = yes; request.onerror = no;});
      const db = request.result, transaction = db.transaction('save', 'readwrite'); transaction.objectStore('save').put('keep', 'score');
      await new Promise((yes, no) => {transaction.oncomplete = yes; transaction.onerror = no;}); db.close();
      const frame = document.createElement('iframe'); frame.src = 'game.html'; document.body.append(frame);
      await new Promise(yes => frame.onload = yes); window.retainedGame = frame.contentWindow.document;
    });
    upgraded = true;
    await idle.evaluate(() => window.shell.checkForUpdate());
    await idle.waitForFunction(() => window.shell.snapshot().updateWaiting);
    assert.match(await idle.content(), /OLD LAUNCHER/, 'busy Launcher must not activate retirement');
    await idle.evaluate(() => {window.busy = false; void window.shell.maybeActivateWaiting();});
    await idle.waitForFunction(() => document.body.textContent === 'NEW REACT');
    assert.equal(await idle.evaluate(() => navigator.serviceWorker.getRegistration().then(value => !!value)), false);
    assert.equal(await busy.evaluate(() => window.retainedGame === document.querySelector('iframe').contentWindow.document), true, 'active game keeps its document');
    assert.match(await busy.content(), /OLD LAUNCHER/, 'active old tab does not reload');
    const fresh = await browser.newPage(); await fresh.goto(base);
    assert.equal(await fresh.evaluate(() => document.body.textContent), 'NEW REACT');
    assert.equal(await fresh.evaluate(() => !!navigator.serviceWorker.controller), false);
    assert.deepEqual(await fresh.evaluate(async () => {
      const cache = await caches.open('eagler-touhou-language-packs-v1');
      const request = indexedDB.open('retirement-save-sentinel', 1);
      await new Promise(yes => {request.onsuccess = yes;}); const db = request.result;
      const read = db.transaction('save').objectStore('save').get('score');
      const stored = await new Promise(yes => {read.onsuccess = () => yes(read.result);}); db.close();
      return [localStorage.getItem('save-sentinel'), await (await cache.match('/sentinel')).text(), stored];
    }), ['keep', 'keep', 'keep']);
    await busy.evaluate(() => {window.busy = false; window.shell.maybeReload();});
    await busy.waitForFunction(() => document.body.textContent === 'NEW REACT');
    await Promise.all([idle.close(), busy.close(), fresh.close()]);
    console.log(`PASS old App Shell retirement ${mount}: deferred activation, old/fresh tabs, saved data`);
  }
} finally {await browser?.close(); await new Promise(resolve => server.close(resolve));}
