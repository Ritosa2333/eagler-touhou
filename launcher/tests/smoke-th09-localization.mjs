// Local-only end-to-end check for TH09 hosted language selection and Runtime mounts.
import puppeteer from 'puppeteer-core';
import { existsSync, readdirSync } from 'node:fs';
import { resolve } from 'node:path';

const browsers = resolve(process.env.LOCALAPPDATA || '', 'ms-playwright');
const installations = existsSync(browsers)
  ? readdirSync(browsers).filter(name => /^chromium-\d+$/.test(name)).sort().reverse()
  : [];
const executablePath = process.env.EAGLER_CHROMIUM || resolve(browsers, installations[0] || '', 'chrome-win64/chrome.exe');
if (!existsSync(executablePath)) throw Error(`Chromium not found: ${executablePath}`);
const site = process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8132/';
const browser = await puppeteer.launch({ executablePath, headless: true, args: ['--no-sandbox'] });
try {
  for (const language of ['lang_zh-hans', 'lang_en']) {
    const page = await browser.newPage();
    const failures = [];
    page.on('pageerror', error => failures.push(error.message));
    await page.goto(new URL('?game=th09', site).href, { waitUntil: 'networkidle2', timeout: 30000 });
    if (await page.$eval('#firstUseNoticeDialog', dialog => dialog.open)) {
      await page.evaluate(() => document.querySelector('#firstUseNoticeClose').click());
    }
    await page.select('#languageSelect', language);
    await page.select('#musicSelect', 'ogg-stream');
    await page.evaluate(() => document.querySelector('#launch').click());
    await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(frame =>
      frame.contentWindow?.__th09Runtime?.status().title[0] > 120) ||
      !!document.querySelector('#startupErrorText')?.textContent, { timeout: 90000 });
    const result = await page.evaluate(() => {
      const frame = [...document.querySelectorAll('iframe')].find(item => item.contentWindow?.__th09Runtime);
      const runtime = frame?.contentWindow?.__th09Runtime;
      const fs = runtime?.core.FS;
      const exists = path => !!fs?.analyzePath(path).exists;
      return {
        selected: document.querySelector('#languageSelect')?.value,
        titleFrames: runtime?.status().title[0] || 0,
        error: document.querySelector('#startupErrorText')?.textContent || frame?.contentDocument?.querySelector('#error')?.textContent || '',
        pack: {
          options: exists('/thcrap/th09/localization/options.json'),
          strings: exists('/thcrap/th09/localization/strings.etl'),
          dialogue: exists('/thcrap/th09/pl00.msg'),
          image: exists('/thcrap/th09/data/title/sl_text.png'),
          font: exists('/thcrap/th09/fonts/unifont-15.1.05-subset.otf'),
        },
      };
    });
    console.log(JSON.stringify({ language, ...result, failures }));
    if (result.selected !== language || result.titleFrames <= 120 || result.error ||
        Object.values(result.pack).some(value => !value) || failures.length) {
      throw Error(`TH09 ${language} localization smoke failed`);
    }
    if (process.env.EAGLER_TEST_MUSIC_ROOM === '1') {
      const frame = page.frames().find(item => item.url().includes('/th09.html'));
      await frame.evaluate(async () => {
        const runtime = window.__th09Runtime;
        const wait = ms => new Promise(resolve => setTimeout(resolve, ms));
        for (let index = 0; index < 12 && runtime.status().title[4] !== 5; index++) {
          const code = runtime.status().title[4] < 5 ? 'ArrowDown' : 'ArrowUp';
          await runtime.command({ command: 'keyboard', code, down: true });
          await wait(25);
          await runtime.command({ command: 'keyboard', code, down: false });
          await wait(45);
        }
        if (runtime.status().title[4] !== 5) throw Error('Music Room menu selection was not reached');
        await runtime.command({ command: 'keyboard', code: 'KeyZ', down: true });
        await wait(55);
        await runtime.command({ command: 'keyboard', code: 'KeyZ', down: false });
      });
      console.log(JSON.stringify({ language, afterMusicRoomKeys: await frame.evaluate(() => window.__th09Runtime?.status().title) }));
      await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(item => {
        const title = item.contentWindow?.__th09Runtime?.status().title;
        return title?.[2] === 12 && title?.[3] === 1;
      }), { timeout: 30000 });
      await new Promise(resolve => setTimeout(resolve, 600));
      console.log(JSON.stringify({ language, musicRoom: true }));
    }
    if (language === 'lang_zh-hans' && process.env.EAGLER_TEST_STORY === '1') {
      const frame = page.frames().find(item => item.url().includes('/th09.html'));
      for (const screen of [2, 3]) {
        await frame.evaluate(async () => {
          const runtime = window.__th09Runtime;
          await runtime.command({ command: 'keyboard', code: 'KeyZ', down: true });
          await new Promise(resolve => setTimeout(resolve, 40));
          await runtime.command({ command: 'keyboard', code: 'KeyZ', down: false });
        });
        await page.waitForFunction(expected => [...document.querySelectorAll('iframe')].some(item => {
          const title = item.contentWindow?.__th09Runtime?.status().title;
          return title?.[2] === expected && title?.[3] === 1;
        }), { timeout: 30000 }, screen);
      }
      await frame.evaluate(async () => {
        const runtime = window.__th09Runtime;
        await runtime.command({ command: 'keyboard', code: 'KeyZ', down: true });
        await new Promise(resolve => setTimeout(resolve, 40));
        await runtime.command({ command: 'keyboard', code: 'KeyZ', down: false });
      });
      await page.waitForFunction(() => [...document.querySelectorAll('iframe')].some(item => {
        const status = item.contentWindow?.__th09Runtime?.status();
        return status?.title[1] === 0 && status?.session[1] > 120;
      }), { timeout: 60000 });
      console.log(JSON.stringify({ language, story: true,
        status: await frame.evaluate(() => window.__th09Runtime?.status()) }));
      await new Promise(resolve => setTimeout(resolve, 1200));
    }
    if (language === 'lang_zh-hans' && process.env.EAGLER_TEST_SCREENSHOT) {
      await page.screenshot({ path: resolve(process.env.EAGLER_TEST_SCREENSHOT) });
    }
    await page.close();
  }
} finally {
  await browser.close();
}
