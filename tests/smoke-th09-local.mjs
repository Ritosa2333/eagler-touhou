// Optional local-only browser smoke test for a running development server.
import puppeteer from 'puppeteer-core';
import { existsSync, readdirSync } from 'node:fs';
import { resolve } from 'node:path';

const browsers = resolve(process.env.LOCALAPPDATA || '', 'ms-playwright');
const installations = existsSync(browsers) ? readdirSync(browsers).filter(name => /^chromium-\d+$/.test(name)).sort().reverse() : [];
const executablePath = process.env.EAGLER_CHROMIUM || resolve(browsers, installations[0] || '', 'chrome-win64/chrome.exe');
if (!existsSync(executablePath)) throw Error(`Chromium not found: ${executablePath}`);
const site = process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8130/';
const browser = await puppeteer.launch({ executablePath, headless: true, args: ['--no-sandbox'] });
try {
  const page = await browser.newPage();
  const failures = [];
  page.on('pageerror', error => failures.push(error.message));
  page.on('console', message => { if (message.type() === 'error' && !message.text().startsWith('Failed to load resource')) failures.push(message.text()); });
  page.on('response', response => { if (response.status() >= 400) failures.push(`${response.status()} ${response.url()}`); });
  await page.goto(new URL('?game=th09', site).href, { waitUntil: 'networkidle2', timeout: 30000 });
  if (await page.$eval('#firstUseNoticeDialog', dialog => dialog.open)) await page.evaluate(() => document.querySelector('#firstUseNoticeClose').click());
  await page.select('#musicSelect', 'ogg-stream');
  await page.evaluate(() => document.querySelector('#launch').click());
  await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(frame => frame.contentWindow?.__th09Runtime?.status().title[0] > 120), { timeout: 60000 });
  const buttons = await page.evaluate(() => [...document.querySelectorAll('button')].filter(button => button.innerText.includes('启动游戏')).map(button => ({ text: button.innerText, disabled: button.disabled, html: button.outerHTML.slice(0, 300) })));
  const state = await page.evaluate(() => ({ title: document.title, text: document.body.innerText.slice(0, 600), frames: [...document.querySelectorAll('iframe')].map(frame => frame.src), dialogs: [...document.querySelectorAll('dialog')].filter(dialog => dialog.open).map(dialog => ({ id: dialog.id, text: dialog.innerText.slice(0, 400) })), player: document.querySelector('#player')?.className, status: document.querySelector('#status')?.textContent, importWindow: document.querySelector('#gameDataImportWindow')?.className, startupError: document.querySelector('#startupError')?.textContent }));
  const runtime = await Promise.all(page.frames().map(async frame => frame.url().includes('th09.html') ? frame.evaluate(() => {
    const core = window.__th09Runtime?.core;
    return { loaded: !!window.__th09Runtime, status: window.__th09Runtime?.status(),
      audio: core ? { context: core.SDL3.audioContext?.state,
        titleMusicFile: core.FS.analyzePath('/music/th09_00.ogg').exists,
        configMusicMode: core.FS.readFile('/save/th09.cfg')[0xae] } : null,
      text: document.body.innerText.slice(0, 500) };
  }).catch(error => ({ error: error.message })) : null));
  await page.screenshot({ path: resolve(import.meta.dirname, '../.cache/th09-local-smoke.png') });
  console.log(JSON.stringify({ buttons, state, runtime: runtime.filter(Boolean), failures }, null, 2));
} finally { await browser.close(); }
