import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';
import {betaRuntimeHtml} from '../../lib/beta-runtime-publication.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const bundle = await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
  import React from 'react';
  import {createRoot} from 'react-dom/client';
  import {MemoryRouter} from 'react-router';
  import {LocaleProvider} from './app/i18n.tsx';
  import {LauncherMasthead} from './app/components/launcher/LauncherMasthead.tsx';
  import {installPackageFromAcquisition} from './package/package-installer.mjs';
  globalThis.installPackageFromAcquisition = installPackageFromAcquisition;
  const base = new URL('./', location.href).href;
  function App() {
    const [open,setOpen] = React.useState(false);
    return <LauncherMasthead variant="library" assetUrl={path=>new URL(path,base).href}
      launcherHref={base} faqHref="faq.html" aboutHref="about.html" menuOpen={open} onMenuOpenChange={setOpen}
      lessMotion={false} onToggleMotion={()=>{}} noticeEnabled={true} onToggleNotice={()=>{}}
      diagnosticsEnabled={false} onToggleDiagnostics={()=>{}} onDonation={()=>{}} onFirstUse={()=>{}}
      languageControl={<select value="zh-CN" readOnly><option value="zh-CN">中文</option></select>}/>;
  }
  createRoot(document.getElementById('root')).render(<MemoryRouter><LocaleProvider locale="zh-CN"><App/></LocaleProvider></MemoryRouter>);
`}, bundle: true, write: false, outfile: '.cache/beta-fixture.js', format: 'iife', platform: 'browser', jsx: 'automatic',
  loader: {'.webp': 'dataurl'}, plugins: [authoredSourcesPlugin(project)], logLevel: 'silent'});
const javascript = bundle.outputFiles.find(file => !file.path.endsWith('.css'))?.text;
const stylesheet = bundle.outputFiles.find(file => file.path.endsWith('.css'))?.text || '';
const hash = 'a'.repeat(64);
const site = {schema: 'eagler-touhou/site-manifest/1', protocol: 'eagler-touhou/1', profile: 'web-validation-test',
  shared: {resourceMode: 'hosted', vanillaFont: 'font.ttc', unicodeFont: 'font.otf', testBuild: true, saveChannel: 'beta'},
  games: {th06: {runtime: 'runtime/th06/th06.html', music: {midi: {files: []}},
    gameData: {version: `sha256-${hash}`, layout: `sha256-${hash}`, path: 'th06.data', bytes: 1, sha256: hash}}}};
let published = false;
const server = createServer((request, response) => {
  const path = new URL(request.url, 'http://fixture.invalid').pathname;
  if (path === '/beta/site-manifest.json') {
    response.writeHead(published ? 200 : 404, {'Content-Type': 'application/json'}); response.end(JSON.stringify(site));
  } else if (path.endsWith('/runtime.html')) {
    response.writeHead(200, {'Content-Type': 'text/html'});
    response.end(betaRuntimeHtml('<html><head></head><body>Runtime fixture</body></html>'));
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
  page.on('pageerror', error => errors.push(error.message));
  await page.goto(base + 'index.html?lang=zh-CN&room=private');
  async function developer() {
    await page.click('#mastheadMenuToggle');
    await page.waitForSelector('.masthead-developer-trigger');
    await page.click('.masthead-developer-trigger');
    await page.waitForSelector('#betaFeaturesToggle');
  }
  async function enterDialog() {
    await developer(); await page.click('#betaFeaturesToggle');
    await page.waitForSelector('[role=dialog]');
    assert.equal(await page.$('#mastheadMenuPanel'), null);
  }
  const button = text => page.evaluate(text => [...document.querySelectorAll('[role=dialog] button')].find(node => node.textContent === text).click(), text);
  await enterDialog();
  assert.ok((await page.$eval('[role=dialog]', node => node.textContent)).includes('您正在进入测试版模式。打开此模式后，您可以体验目前不稳定的测试版功能。'));
  await button('不进入'); await page.waitForSelector('[role=dialog]', {hidden: true});
  await page.waitForFunction(() => document.activeElement?.id === 'mastheadMenuToggle');
  assert.ok(page.url().includes('/index.html?'));
  await enterDialog(); await button('确认进入测试版'); await page.waitForSelector('[role=alert]');
  assert.ok(page.url().includes('/index.html?'), 'An unpublished beta leaves the stable document in place');
  await button('不进入'); await page.waitForSelector('[role=dialog]', {hidden: true});
  await enterDialog(); assert.equal(await page.$('[role=alert]'), null);
  published = true;
  await Promise.all([page.waitForNavigation(), button('确认进入测试版')]);
  assert.equal(page.url(), base + 'beta/index.html?lang=zh-CN');
  await developer(); assert.equal(await page.$eval('#betaFeaturesToggle', node => node.getAttribute('aria-checked')), 'true');
  await Promise.all([page.waitForNavigation(), page.click('#betaFeaturesToggle')]);
  assert.equal(page.url(), base + 'index.html?lang=zh-CN');

  // Real same-origin iframe realms: beta interception must never modify the parent factory.
  const names = await page.evaluate(async () => {
    const iframe = document.createElement('iframe'); iframe.src = '/beta/runtime.html'; document.body.append(iframe);
    await new Promise(resolve => iframe.onload = resolve);
    const open = (factory, name) => new Promise((resolve, reject) => {
      const request = factory.open(name); request.onsuccess = () => {request.result.close(); resolve();}; request.onerror = () => reject(request.error);
    });
    await open(indexedDB, '/savesth06');
    await open(iframe.contentWindow.indexedDB, '/savesth06');
    await open(iframe.contentWindow.indexedDB, 'eagler-touhou-package-store-v1');
    return (await indexedDB.databases()).map(db => db.name).sort();
  });
  assert.deepEqual(names, ['/savesth06', 'beta:/savesth06', 'eagler-touhou-package-store-v1']);
  // Install from stable, then a different beta descriptor: identical DATA is never reacquired.
  const reuse = await page.evaluate(async () => {
    await new Promise((resolve, reject) => {
      const request = indexedDB.deleteDatabase('eagler-touhou-package-store-v1'); request.onsuccess = resolve; request.onerror = () => reject(request.error);
    });
    const bytes = new Uint8Array([1, 2, 3]);
    const hash = [...new Uint8Array(await crypto.subtle.digest('SHA-256', bytes))].map(n => n.toString(16).padStart(2, '0')).join('');
    const descriptor = {schema: 'eagler-touhou/package/1', game: 'th06', revision: 'stable-data',
      runtimeRequirement: {protocol: 'eagler-touhou/1', target: 'th06', dataFile: 'data'},
      files: {data: {source: 'th06.data', target: '/th06.data', revision: 'data-r1', sha256: hash, bytes: 3}}, base: {files: ['data']}, components: {}};
    let downloads = 0;
    const acquire = async () => {downloads++; return bytes.buffer;};
    await globalThis.installPackageFromAcquisition({descriptor, desiredFileIds: ['data'], source: 'remote', acquire});
    await globalThis.installPackageFromAcquisition({descriptor: {...descriptor, revision: 'beta-data'}, desiredFileIds: ['data'], source: 'remote', acquire});
    return downloads;
  });
  assert.equal(reuse, 1);
  assert.deepEqual(errors, []);
  console.log('Beta browser: warning/cancel/unpublished/enter/exit, isolated iframe saves, shared DATA without a second download PASS');
} finally {
  await browser?.close();
  await new Promise(resolve => server.close(resolve));
}
