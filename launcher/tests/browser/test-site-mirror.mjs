// HTTPS fixtures exercise the built worker, cross-origin CORS and browser restart.
// Supply a local test certificate/key through EAGLER_TEST_TLS_CERT/KEY.
import assert from 'node:assert/strict';
import {createServer} from 'node:https';
import {readFile, writeFile, mkdtemp, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import puppeteer from 'puppeteer-core';
import {buildAppShell} from '../../lib/app-shell-build.mjs';
import {resolveRuntimeGenerationWorkerSource} from '../../lib/contracts-build.mjs';
import {runtimeGenerationId} from '../../lib/runtime-generations.mjs';

const tls = {cert:await readFile(process.env.EAGLER_TEST_TLS_CERT),key:await readFile(process.env.EAGLER_TEST_TLS_KEY)};
const work = await mkdtemp(join(tmpdir(), 'site-mirror-browser-'));
const requests = [], servers = [], modes = ['ok', 'ok', 'ok'];
const files = new Map([
  ['index.html', ['text/html', '<h1>launcher</h1><script>navigator.serviceWorker.register("app-shell-sw.js").catch(()=>{});</script>']],
  ['a.mjs', ['text/javascript', 'export const entry = import.meta.url; export const release = "verified";']],
]);
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const root = 'runtime/th09/';
const runtimeFiles = {'game.html':'<h1>runtime</h1>', 'game.wasm':'verified wasm bytes'};
const identities = Object.entries(runtimeFiles).map(([path,body]) => ({path,bytes:Buffer.byteLength(body),sha256:sha(body)}));
const descriptor = {entry:'game.html',files:identities,generation:runtimeGenerationId('game.html',identities)};
const catalog = {schema:'eagler-touhou/runtime-manifest/1',protocol:'eagler-touhou/1',groups:[{root,current:descriptor,previous:[]}]};
files.set('runtime-manifest.json', ['application/json',JSON.stringify(catalog)]);
for (const [path,body] of Object.entries(runtimeFiles)) files.set(root+descriptor.generation+'/'+path, ['application/octet-stream',body]);
let browser;
try {
  const bases = [];
  for (let i=0;i<3;i++) {
    const server = createServer(tls,(request,response) => {
      const path = new URL(request.url,'https://fixture.test').pathname.slice(1) || 'index.html';
      requests.push({node:i,path});
      if (modes[i] === 'reset') {request.socket.destroy();return;}
      response.setHeader('Access-Control-Allow-Origin','*');
      const file = files.get(path);
      response.writeHead(file ? 200 : 404, {'Content-Type':file?.[0] || 'text/plain','Cache-Control':'no-store'});
      response.end(modes[i] === 'corrupt' ? 'corrupt' : file?.[1] || 'missing');
    });
    await new Promise(resolve => server.listen(0,'127.0.0.1',resolve));
    servers.push(server); bases.push(`https://127.0.0.1:${server.address().port}/`);
  }
  files.set('site-network.json',['application/json',JSON.stringify({schema:'eagler-touhou/site-network/1',mirrors:bases.slice(1)})]);
  for (const [path,[,body]] of files) if (!path.includes('/')) await writeFile(join(work,path),body);
  const built = await buildAppShell({globDirectory:work,shellFiles:['index.html','a.mjs'],quiet:true,
    workerContractSource:await readFile(await resolveRuntimeGenerationWorkerSource(),'utf8')});
  files.set('app-shell-sw.js',['text/javascript',built.worker]);
  const launch = () => puppeteer.launch({executablePath:process.env.CHROME_PATH,
    userDataDir:join(work,'profile'),headless:true,args:['--ignore-certificate-errors','--no-proxy-server']});
  browser = await launch();
  let page = await browser.newPage();
  await page.goto(bases[0],{waitUntil:'load'});
  await page.waitForFunction(async () => (await navigator.serviceWorker.getRegistration())?.active?.state === 'activated');
  await page.reload({waitUntil:'load'});
  await page.evaluate(() => localStorage.setItem('save-sentinel','preserved'));
  async function evict() {
    await page.evaluate(async () => {
      for (const name of await caches.keys()) if (name.startsWith('eagler-touhou-app-shell-')) {
        await (await caches.open(name)).delete(new URL('a.mjs',location.href).href);
      }
    });
  }
  await evict(); modes[0] = 'reset'; requests.length = 0;
  let module = await page.evaluate(async () => {const m=await import('./a.mjs?first'); return {url:m.entry,release:m.release};});
  assert.equal(module.release,'verified'); assert.ok(module.url.startsWith(bases[0]));
  assert.ok(requests.some(r => r.node===1 && r.path==='a.mjs'));
  await evict(); await browser.close(); browser = null;
  modes[1] = 'reset'; requests.length = 0;
  browser = await launch(); page = await browser.newPage();
  await page.goto(bases[0],{waitUntil:'load'});
  assert.equal(await page.evaluate(() => localStorage.getItem('save-sentinel')),'preserved');
  module = await page.evaluate(async () => {const m=await import('./a.mjs?restart');return {url:m.entry,release:m.release};});
  assert.equal(module.release,'verified'); assert.ok(module.url.startsWith(bases[0]));
  assert.ok(requests.some(r => r.node===2 && r.path==='a.mjs'));
  const runtimePath = root+descriptor.generation+'/game.wasm';
  assert.equal(await page.evaluate(async path => (await fetch(path)).text(),runtimePath),'verified wasm bytes');
  assert.ok(requests.some(r => r.node===2 && r.path.endsWith('game.wasm')));
  assert.equal(await page.evaluate(async () => (await (await fetch('runtime-manifest.json')).json()).groups[0].root), root);
  await page.setOfflineMode(true); await page.reload({waitUntil:'load'});
  assert.equal(await page.$eval('h1',node => node.textContent),'launcher');
  console.log(JSON.stringify({passed:true,primaryDownUsesSecond:true,restartUsesThird:true,
    immutableRuntimeUsesThird:true,sameOrigin:true,storagePreserved:true,offlineReload:true,requests}));
} finally {
  if(browser) await browser.close();
  await Promise.all(servers.map(server => new Promise(resolve => {server.closeAllConnections();server.close(resolve);} )));
  await rm(work,{recursive:true,force:true});
}
