import assert from 'node:assert/strict';
import {existsSync, mkdirSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

if (process.argv.includes('--help')) {
  console.log('TH19_BROWSER=/path/to/browser TH19_PUPPETEER_MODULE=/path/to/puppeteer-core/lib/puppeteer/puppeteer-core.js node tests/browser-smoke.mjs --url=http://127.0.0.1:8130 --relay=ws://127.0.0.1:18142 --output=artifacts/browser');
  process.exit(0);
}
const options = new Map(process.argv.slice(2).map(argument => {
  const match = /^--(url|relay|output)=(.+)$/.exec(argument);
  if (!match) throw new Error(`Unknown argument: ${argument}`);
  return [match[1], match[2]];
}));
const base = new URL(options.get('url') || 'http://127.0.0.1:8130/');
const relay = options.get('relay') || 'ws://127.0.0.1:18142/';
const output = resolve(options.get('output') || 'artifacts/browser');
mkdirSync(output, {recursive: true});
writeFileSync(resolve(output, 'report.json'), JSON.stringify({passed: false, status: 'running', startedAt: new Date().toISOString()}, null, 2) + '\n');
const modulePath = process.env.TH19_PUPPETEER_MODULE || resolve(import.meta.dirname, '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js');
if (!existsSync(modulePath)) throw new Error('Set TH19_PUPPETEER_MODULE to the installed puppeteer-core ES module.');
const {default: puppeteer} = await import(pathToFileURL(modulePath).href);
const executablePath = process.env.TH19_BROWSER;
if (!executablePath) throw new Error('Set TH19_BROWSER to Chrome or Edge.');
const browser = await puppeteer.launch({
  executablePath, headless: true, timeout: 60000,
  defaultViewport: {width: 960, height: 720},
  args: ['--no-sandbox', '--use-angle=swiftshader', '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required', '--disable-background-timer-throttling', '--disable-renderer-backgrounding'],
});
const errors = [], results = [];let latestStates=[];
async function pageFor(query) {
  const page = await browser.newPage();
  page.on('pageerror', error => errors.push(error.message));
  page.on('response', response => {if (response.status() >= 400) errors.push(`${response.status()} ${response.url()}`);});
  await page.goto(new URL(`tests/harness.html?${query}`, base).href, {waitUntil: 'domcontentloaded', timeout: 120000});
  await until(async () => page.evaluate(() => !!window.__TH19_HARNESS?.configured), 'Runtime configuration');
  return page;
}
async function until(check, label, timeout = 120000) {
  const deadline = Date.now() + timeout;
  while (Date.now() < deadline) {
    const value = await check();
    if (value) return value;
    await new Promise(resolve => setTimeout(resolve, 100));
  }
  throw new Error(`Timed out waiting for ${label}`);
}
async function snapshot(page) {
  return page.evaluate(() => {
    const harness = window.__TH19_HARNESS, runtime = harness.frame.contentWindow.__th19Runtime;
    return {preview:harness.preview,launched:harness.launched,events:harness.events.slice(-4),title:harness.frame.contentWindow.Module?Array.from(harness.frame.contentWindow.Module.HEAP32.subarray(harness.frame.contentWindow.Module._th19_title_status()/4,harness.frame.contentWindow.Module._th19_title_status()/4+8)):null,errors: harness.errors, active: runtime?.netplay?.active, frame: runtime?.netplay?.frame, path: runtime?.netplay?.socket?.route, hashes: runtime ? [...runtime.netplay.hashes] : []};
  });
}
try {
  const storage = await pageFor('autolaunch=0');
  const original = await storage.evaluate(async () => {
    const command = window.__TH19_HARNESS.command;
    try {return (await command('read', {path: 'th19.cfg'})).bytes;} catch {return null;}
  });
  try {
    await storage.evaluate(() => window.__TH19_HARNESS.command('write', {path: 'th19.cfg', bytes: [84,72,49,57,1,2,3]}));
    await storage.reload({waitUntil: 'domcontentloaded'});
    await until(() => storage.evaluate(() => !!window.__TH19_HARNESS?.configured), 'IndexedDB restore');
    assert.deepEqual(await storage.evaluate(async () => (await window.__TH19_HARNESS.command('read', {path: 'th19.cfg'})).bytes), [84,72,49,57,1,2,3]);
    const escaped = await storage.evaluate(async () => {
      try {await window.__TH19_HARNESS.command('write', {path: '../th19.dat', bytes: [1]}); return false;} catch {return true;}
    });
    assert.equal(escaped, true);
    results.push({check: 'IndexedDB restore and path boundary', passed: true});
  } finally {
    await storage.evaluate(async bytes => {
      const command = window.__TH19_HARNESS.command;
      if (bytes) await command('write', {path: 'th19.cfg', bytes});
      else await command('remove', {path: 'th19.cfg'});
    }, original);
    await storage.close();
  }
  for (const mode of [1, 0]) {
    const room = `th19mp-${String(2000 + Date.now() % 7000)}`;
    const pages = await Promise.all([0, 1].map(side => pageFor(new URLSearchParams({side: String(side), room, mode: String(mode), relay}).toString())));
    try {
      await until(async () => (latestStates=await Promise.all(pages.map(snapshot))).every(state => state.active), `RTC mode ${mode}`);
      // The first native frame has already been presented. Capture subsequent
      // simulation without GPU submission so software rendering cannot limit
      // the deterministic-input verification on a headless machine.
      for (const page of pages) await page.evaluate(() => {
        const core = window.__TH19_HARNESS.frame.contentWindow.__th19Runtime.core;
        const step = core._th19_network_step;
        core._th19_network_step = (...args) => {args[9] = 0; return step(...args);};
      });
      await until(async () => (await Promise.all(pages.map(snapshot))).every(state => state.active && state.frame >= 360), `360 confirmed frames in mode ${mode}`);
      const states = await Promise.all(pages.map(snapshot));
      assert.ok(states.every(state => !state.errors.length && state.path === 'rtc'));
      const other = new Map(states[1].hashes);
      const common = states[0].hashes.filter(([frame]) => other.has(frame));
      assert.ok(common.length >= 2, 'both players publish confirmed hashes');
      for (const [frame, hash] of common) assert.equal(hash, other.get(frame), `mode ${mode}, frame ${frame}`);
      results.push({check: mode ? 'RTC exact-input mode' : 'RTC rollback mode', passed: true, frames: states.map(state => state.frame), hashes: common});
      for(const page of pages){const result=await page.evaluate(async()=>{const h=window.__TH19_HARNESS,c=h.frame.contentWindow.__th19Runtime.core;const reply=await h.command('return-room');return {presented:reply.presented,scene:c.HEAP32[c._th19_title_status()/4+1],sameCore:c===h.frame.contentWindow.__th19Runtime.core};});assert.deepEqual(result,{presented:true,scene:0,sameCore:true});}
      results.push({check:'Native room return retains the loaded core',mode,passed:true});
    } finally {await Promise.all(pages.map(page => page.close()));}
  }
  assert.deepEqual(errors, []);
  writeFileSync(resolve(output, 'report.json'), JSON.stringify({passed: true, results, errors}, null, 2) + '\n');
  console.log(JSON.stringify({passed: true, results}));
} catch (error) {
  writeFileSync(resolve(output, 'report.json'), JSON.stringify({passed: false, results, errors, latestStates, failure: error.stack}, null, 2) + '\n');
  throw error;
} finally {await browser.close();}
