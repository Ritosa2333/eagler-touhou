import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {withPlayerCueScript} from '../../lib/runtime-player-cue.mjs';

const source = await readFile(new URL('../../../common/browser/local-player-cue.js', import.meta.url), 'utf8');
const game = withPlayerCueScript(`<!doctype html><html><head></head><body>
  <canvas width="640" height="400" tabindex="0"></canvas>
  <script>let x=180;window.movePilot=()=>x+=80;
  __eaglerLocalPlayerCue.track(()=>({canvas:document.querySelector('canvas'),seat:2,x,y:350,width:640,height:400}));</script>
</body></html>`, source);
const server = createServer((req, res) => {
  res.setHeader('Content-Type', 'text/html');
  res.end(req.url === '/game' ? game : '<button id="launcher">Settings</button><input id="chat"><iframe src="/game" width="650" height="420"></iframe>');
});
await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
let browser;
try {
  browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
  const page = await browser.newPage();
  await page.goto(`http://127.0.0.1:${server.address().port}/`);
  const frame = await page.waitForFrame(item => item.url().endsWith('/game'));
  const visible = 'span[aria-hidden="true"]:not([hidden])';
  await page.focus('#launcher');
  await page.keyboard.down('ControlLeft');
  await frame.waitForSelector(visible);
  assert.equal(await frame.$eval(visible, element => element.textContent), 'Player 3');
  const original = await frame.$eval(visible, element => element.getBoundingClientRect().x);
  await frame.evaluate(() => window.movePilot());
  await frame.waitForFunction(x => document.querySelector('span').getBoundingClientRect().x > x + 70, {}, original);
  await page.keyboard.up('ControlLeft');
  await frame.waitForSelector(visible, {hidden: true});
  await frame.focus('canvas');
  await page.keyboard.press('ControlRight');
  await frame.waitForSelector(visible);
  await frame.waitForSelector(visible, {hidden: true});
  await page.focus('#chat');
  await page.keyboard.press('ControlLeft');
  await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
  assert.equal(await frame.$(visible), null, 'typing in the Launcher must not show a cue');
  await page.focus('#launcher');
  await page.keyboard.down('ControlLeft');
  await frame.waitForSelector(visible);
  await page.evaluate(() => document.querySelector('iframe').src = 'about:blank');
  await page.keyboard.up('ControlLeft');
  console.log('Ctrl follows the local seat from Launcher and game focus, expires, and ignores text input');
} finally {
  await browser?.close();
  await new Promise(resolve => server.close(resolve));
}
