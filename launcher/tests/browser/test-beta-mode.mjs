import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';
import {PRODUCT_IDS, productEnabledForBuild} from '../../lib/contracts/product-catalog.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const bundle = await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
  import React from 'react';
  import {createRoot} from 'react-dom/client';
  import {MemoryRouter} from 'react-router';
  import {LocaleProvider} from './app/i18n.tsx';
  import {LauncherMasthead} from './app/components/launcher/LauncherMasthead.tsx';
  import {BookLibrary} from './app/components/launcher/BookLibrary.tsx';
  import {createLibraryProducts} from './app/components/launcher/products.ts';
  import {PRODUCT_IDS,productEnabledForBuild} from './src/contracts/product-catalog.mts';
  import {testFeaturesEnabled} from './src/launcher/test-features.mts';
  const base = new URL('./', location.href).href;
  function App() {
    const [open,setOpen] = React.useState(false);
    const testBuild = testFeaturesEnabled(window);
    const products = createLibraryProducts(PRODUCT_IDS.filter(id=>productEnabledForBuild(id,testBuild)),
      path=>new URL(path,base).href,id=>new URL('index.html?game='+id,base).href);
    return <><LauncherMasthead variant="library" assetUrl={path=>new URL(path,base).href}
      launcherHref={base} faqHref="faq.html" aboutHref="about.html" menuOpen={open} onMenuOpenChange={setOpen}
      lessMotion={false} onToggleMotion={()=>{}}
      diagnosticsEnabled={false} onToggleDiagnostics={()=>{}} onDonation={()=>{}} onFirstUse={()=>{}}
      languageControl={<select value="zh-CN" readOnly><option value="zh-CN">中文</option></select>}/>
      <main><BookLibrary products={products} onSelect={()=>{}} onActivate={()=>{}}/></main></>;
  }
  createRoot(document.getElementById('root')).render(<MemoryRouter><LocaleProvider locale="zh-CN"><App/></LocaleProvider></MemoryRouter>);
`}, bundle: true, write: false, outfile: '.cache/beta-fixture.js', format: 'iife', platform: 'browser', jsx: 'automatic',
  loader: {'.webp': 'dataurl'}, plugins: [authoredSourcesPlugin(project)], logLevel: 'silent'});
const javascript = bundle.outputFiles.find(file => !file.path.endsWith('.css'))?.text;
const stylesheet = bundle.outputFiles.find(file => file.path.endsWith('.css'))?.text || '';
const requests = [];
const server = createServer((request, response) => {
  const path = new URL(request.url, 'http://fixture.invalid').pathname;
  requests.push(path);
  if (path.startsWith('/beta/')) {
    response.writeHead(404); response.end();
  } else if (path === '/fixture.js') {
    response.writeHead(200, {'Content-Type': 'text/javascript'}); response.end(javascript);
  } else if (path === '/fixture.css') {
    response.writeHead(200, {'Content-Type': 'text/css'}); response.end(stylesheet);
  } else {
    response.writeHead(200, {'Content-Type': 'text/html'});
    response.end('<html><head><link rel="stylesheet" href="/fixture.css"></head><body><div id="root"></div><script src="/fixture.js"></script></body></html>');
  }
});
await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
const base = `http://127.0.0.1:${server.address().port}/`;
let browser;
try {
  browser = await puppeteer.launch({executablePath: process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe', headless: true});
  const page = await browser.newPage(), errors = [];
  const roomUrl = base + 'lobby.html?lang=zh-CN&room=th09-1234&game=th06mp&run=run-1&member=m0123456789abcdef#room';
  const expectedProducts = enabled => PRODUCT_IDS.filter(product => !product.endsWith('mp') && productEnabledForBuild(product, enabled));
  const visibleProducts = () => page.$$eval('.book-grid [data-book-product]', cards => cards.map(card => card.dataset.bookProduct));
  assert.ok(expectedProducts(true).includes('th075') && expectedProducts(true).includes('th20'));
  assert.ok(!expectedProducts(false).includes('th075') && !expectedProducts(false).includes('th20'));
  await page.goto(base + 'seed.html');
  await page.evaluate(() => {
    localStorage.setItem('eagler-touhou-mp-member-v1', 'm0123456789abcdef');
    localStorage.setItem('eagler-touhou-mp-display-name-v1', 'Alice');
    sessionStorage.setItem('eagler-touhou-th06mp-lobby-client-v1', 'client_12345678');
  });
  page.on('pageerror', error => errors.push(error.message));
  await page.goto(roomUrl);
  async function developer() {
    const menuOpen = await page.$eval('#mastheadMenuPanel', node => node.dataset.state === 'open').catch(() => false);
    if (!menuOpen) {
      await page.click('#mastheadMenuToggle');
      await page.waitForSelector('.masthead-developer-trigger');
    }
    const testFeaturesOpen = await page.$eval('#betaFeaturesToggle', node => node.getAttribute('aria-checked') !== null).catch(() => false);
    if (!testFeaturesOpen) {
      await page.click('.masthead-developer-trigger');
      await page.waitForSelector('#betaFeaturesToggle');
    }
  }
  const checked = () => page.$eval('#betaFeaturesToggle', node => node.getAttribute('aria-checked'));
  async function toggle(expected) {
    await developer();
    assert.equal(await checked(), expected === 'true' ? 'false' : 'true');
    await Promise.all([page.waitForNavigation(), page.click('#betaFeaturesToggle')]);
    assert.equal(page.url(), roomUrl);
    await developer();
    assert.equal(await checked(), expected);
    const values = await page.evaluate(() => ({
      enabled: localStorage.getItem('eagler-touhou-beta-opt-in-v1'),
      member: localStorage.getItem('eagler-touhou-mp-member-v1'),
      name: localStorage.getItem('eagler-touhou-mp-display-name-v1'),
      client: sessionStorage.getItem('eagler-touhou-th06mp-lobby-client-v1'),
    }));
    assert.deepEqual(values, {
      enabled: expected === 'true' ? '1' : null,
      member: 'm0123456789abcdef', name: 'Alice', client: 'client_12345678',
    });
    assert.deepEqual(await visibleProducts(), expectedProducts(expected === 'true'));
    assert.equal(await page.$('[role=dialog]'), null);
  }
  await developer();
  assert.equal(await checked(), 'false');
  assert.deepEqual(await visibleProducts(), expectedProducts(false));
  await toggle('true');
  await toggle('false');
  assert.deepEqual(requests.filter(path => path.includes('/beta/')), []);
  assert.deepEqual(errors, []);
  console.log('Beta visibility: same room URL and multiplayer identity across toggle, with no /beta/ request PASS');
} finally {
  await browser?.close();
  await new Promise(resolve => server.close(resolve));
}
