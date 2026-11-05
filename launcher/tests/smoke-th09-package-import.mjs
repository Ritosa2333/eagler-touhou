// Local-only end-to-end check: import the generated TH09 package in a fresh browser profile.
import puppeteer from 'puppeteer-core';
import { existsSync, readdirSync } from 'node:fs';
import { resolve } from 'node:path';

const browsers = resolve(process.env.LOCALAPPDATA || '', 'ms-playwright');
const installations = existsSync(browsers) ? readdirSync(browsers).filter(name => /^chromium-\d+$/.test(name)).sort().reverse() : [];
const executablePath = process.env.EAGLER_CHROMIUM || resolve(browsers, installations[0] || '', 'chrome-win64/chrome.exe');
const packagePath = process.env.EAGLER_TEST_PACKAGE || resolve(import.meta.dirname, '../../games/th09-import-package.zip');
const requestedLanguage = process.env.EAGLER_TEST_LANGUAGE || 'ja';
if (!existsSync(executablePath)) throw Error(`Chromium not found: ${executablePath}`);
if (!existsSync(packagePath)) throw Error(`Package not found: ${packagePath}`);

const browser = await puppeteer.launch({ executablePath, headless: true, args: ['--no-sandbox'] });
try {
  const page = await browser.newPage();
  const errors = [];
  const languageRequests = [];
  page.on('pageerror', error => errors.push(error.message));
  page.on('console', message => { if (message.type() === 'error') errors.push(message.text()); });
  page.on('response', response => { if (response.status() >= 400) errors.push(`${response.status()} ${response.url()}`); });
  page.on('request', request => { if (request.url().includes('/games/th09/language/')) languageRequests.push(request.url()); });
  const site = process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8131/';
  await page.goto(new URL('?game=th09', site).href, { waitUntil: 'networkidle2', timeout: 30000 });
  if (await page.$eval('#firstUseNoticeDialog', dialog => dialog.open)) {
    await page.evaluate(() => document.querySelector('#firstUseNoticeClose').click());
  }
  await page.evaluate(() => document.querySelector('#gamePackageImport').click());
  await page.waitForFunction(() => !document.querySelector('#gameDataImportWindow').hidden, { timeout: 10000 });
  await (await page.$('#gameDataImportInput')).uploadFile(packagePath);
  try {
    await page.waitForFunction(() => document.querySelector('#gameDataImportWindow').hidden, { timeout: 180000 });
  } catch {
    const error = await page.evaluate(() => ({
      reason: document.querySelector('#gameDataImportReason')?.textContent,
      status: document.querySelector('#status')?.textContent,
      playerStatus: document.querySelector('#playerStatus')?.textContent,
    }));
    throw Error(`Package import did not finish: ${JSON.stringify(error)}`);
  }
  const result = await page.evaluate(() => ({
    game: new URLSearchParams(location.search).get('game'),
    status: document.querySelector('#status')?.textContent,
    music: document.querySelector('#musicSelect')?.value,
    toast: document.querySelector('#toast')?.textContent,
    importWindowHidden: document.querySelector('#gameDataImportWindow')?.hidden,
  }));
  console.log(JSON.stringify(result));
  if (!result.importWindowHidden || result.game !== 'th09' || result.music !== 'ogg-stream') {
    throw Error('TH09 package import did not restore OGG music');
  }
  if (requestedLanguage !== 'ja') await page.select('#languageSelect', requestedLanguage);
  await page.evaluate(() => document.querySelector('#launch').click());
  try {
    await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(frame => frame.contentWindow?.__th09Runtime?.status().title[0] > 120) || !!document.querySelector('#startupErrorText').textContent, { timeout: 20000 });
  } catch {
    const state = await page.evaluate(() => ({
      status: document.querySelector('#status')?.textContent,
      playerStatus: document.querySelector('#playerStatus')?.textContent,
      startupError: document.querySelector('#startupError')?.textContent,
      startupErrorHtml: document.querySelector('#startupError')?.innerHTML.slice(0, 2000),
      player: document.querySelector('#player')?.className,
      dialogs: [...document.querySelectorAll('dialog')].filter(dialog => dialog.open).map(dialog => ({ id: dialog.id, text: dialog.textContent.slice(0, 500) })),
      decision: { hidden: document.querySelector('#decisionDialog')?.hidden,
        text: document.querySelector('#decisionDialog')?.textContent?.slice(0, 500) },
      launchDisabled: document.querySelector('#launch')?.disabled,
      frames: [...document.querySelectorAll('iframe')].map(frame => ({ url: frame.src,
        error: frame.contentDocument?.querySelector('#error')?.textContent,
        title: frame.contentWindow?.__th09Runtime?.status().title })),
    }));
    throw Error(`TH09 did not reach title: ${JSON.stringify({ state, errors })}`);
  }
  if (await page.$eval('#startupErrorText', element => !!element.textContent)) {
    const detail = await page.evaluate(() => ({ html: document.querySelector('#startupError').innerHTML,
      status: document.querySelector('#playerStatus')?.textContent }));
    throw Error(`TH09 startup error: ${JSON.stringify({ detail, errors })}`);
  }
  const audio = await page.evaluate(() => [...document.querySelectorAll('iframe')].flatMap(frame => {
    const core = frame.contentWindow?.__th09Runtime?.core;
    if (!core) return [];
    return [{ context: core.SDL3.audioContext?.state,
      titleMusicFile: core.FS.analyzePath('/music/th09_00.ogg').exists,
      languageMounted: core.FS.analyzePath('/thcrap/th09/localization/strings.etl').exists &&
        core.FS.analyzePath('/thcrap/th09/pl00.msg').exists,
      configMusicMode: core.FS.readFile('/save/th09.cfg')[0xae],
      title: frame.contentWindow.__th09Runtime.status().title }];
  })[0]);
  console.log(JSON.stringify({ requestedLanguage, audio, languageRequests }));
  if (!audio || !audio.titleMusicFile || !audio.configMusicMode) {
    throw Error(`TH09 title BGM is not active: ${JSON.stringify(audio)}`);
  }
  if (requestedLanguage !== 'ja' && (!audio.languageMounted || languageRequests.length)) {
    throw Error(`TH09 imported language pack was not used locally: ${JSON.stringify({ audio, languageRequests })}`);
  }
} finally {
  await browser.close();
}
