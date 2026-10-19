// Actual CacheStorage + runtime-only Service Worker. Authored Launcher helper,
// immutable HTML/module/WASM fixtures, no App Shell or user browser profile.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {mkdir, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {build} from 'esbuild';
import {loadPlaywright} from '../../tools/browser/playwright.mjs';
import {buildRuntimeProgramWorkerSource} from '../lib/frontend-assets.mjs';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';
import {canonicalRuntimePayload, RUNTIME_MANIFEST_SCHEMA, RUNTIME_PROTOCOL} from '../lib/contracts/runtime-generations.mjs';

const variant=process.argv.includes('--multiplayer')?'multiplayer':'normal';
const project=resolve(import.meta.dirname,'..'),mount='/nested/',root=variant==='multiplayer'?'runtime/th08mp/':'runtime/th08/',requests=[];
const engine=process.argv.find(value=>value.startsWith('--engine='))?.slice('--engine='.length)||'chromium';
if(!['chromium','firefox','webkit'].includes(engine))throw Error('Unknown browser engine: '+engine);
const out=resolve(project,'artifacts/validation/runtime-offline-first',...(variant==='multiplayer'?['multiplayer',engine]:[engine]));await mkdir(out,{recursive:true});
const hash=body=>createHash('sha256').update(body).digest('hex');
function generation(version) {
  const bodies={
    'game.html':'<!doctype html><script type="module" src="./game.mjs"></script>',
    'game.mjs':`const bytes=await(await fetch('./game.wasm')).text();parent.postMessage({version:${JSON.stringify(version)},bytes,controlled:!!navigator.serviceWorker.controller},'*');`,
    'game.wasm':`wasm fixture ${version}`,
  };
  const files=Object.entries(bodies).map(([path,body])=>({path,bytes:Buffer.byteLength(body),sha256:hash(body)}));
  return {bodies,descriptor:{entry:'game.html',files,generation:hash(canonicalRuntimePayload('game.html',files))}};
}
const A=generation('a'),B=generation('b'),C=generation('c'),versions=[A,B,C];
let current=A,corrupt=false,holdBytes=false,releaseBytes,dropConnections=false;
const worker=await buildRuntimeProgramWorkerSource({project});
const api=await build({entryPoints:[resolve(project,'src/launcher/runtime-launch.mts')],bundle:true,write:false,format:'esm',platform:'browser',plugins:[authoredSourcesPlugin(project)]});
const manifest=()=>({schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[{root,current:current.descriptor,previous:versions.filter(v=>v!==current).map(v=>v.descriptor)}]});
const server=createServer(async(req,res)=>{
  const pathname=new URL(req.url,'http://localhost').pathname;requests.push(pathname);
  if(dropConnections){req.socket.destroy();return;}
  if(pathname==='/probe/sw.js'){
    res.setHeader('Content-Type','text/javascript');res.end(`self.addEventListener('install',e=>e.waitUntil(self.skipWaiting()));self.addEventListener('fetch',e=>e.respondWith(new Response('<h1>Literal worker response</h1>',{headers:{'Content-Type':'text/html'}})));`);return;
  }
  if(pathname==='/probe/'){res.end('<!doctype html><title>Independent offline probe</title>');return;}
  const path=pathname.slice(mount.length);
  res.setHeader('Cache-Control','no-store');
  res.setHeader('Content-Type',path.endsWith('.mjs')||path.endsWith('-sw.js')?'text/javascript':path.endsWith('.json')?'application/json':'text/html');
  if(path==='runtime-program-sw.js'){res.end(worker);return;}
  if(path==='api.mjs'){res.end(api.outputFiles[0].contents);return;}
  if(path==='runtime-manifest.json'){res.end(JSON.stringify(manifest()));return;}
  const version=versions.find(v=>path.startsWith(root+v.descriptor.generation+'/'));
  if(version){
    const file=path.slice((root+version.descriptor.generation+'/').length),body=version.bodies[file];
    if(file==='game.wasm'&&version===current&&holdBytes)await new Promise(r=>{releaseBytes=r;});
    if(body!==undefined){res.end(corrupt&&version===current&&file==='game.wasm'?'corrupt bytes':body);return;}
  }
  if(path===''){res.end('<!doctype html><title>Offline program acceptance</title><iframe id="game"></iframe>');return;}
  res.writeHead(404).end('Unexpected fixture request');
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const base='http://127.0.0.1:'+server.address().port+mount;
const playwright=await loadPlaywright();
const browser=await playwright[engine].launch({headless:true});
// Independently reproduce the known WebKit offline-emulation failure. Permit
// origin connection drops only if even a literal-response worker fails; never
// turn an application-cache failure into a skipped test.
const probe=await browser.newContext(),probePage=await probe.newPage();
let emulationSupported=true,probeError=null;
try {
  await probePage.goto(new URL('/probe/',base).href);
  await probePage.evaluate(async()=>{await navigator.serviceWorker.register('./sw.js');await navigator.serviceWorker.ready;});
  await probePage.goto(new URL('/probe/literal',base).href);
  assert.equal(await probePage.locator('h1').textContent(),'Literal worker response');
  await probe.setOffline(true);
  try{await probePage.goto(new URL('/probe/literal',base).href,{timeout:5000});}
  catch(error){
    if(engine!=='webkit'||!String(error).includes('WebKit encountered an internal error'))throw error;
    emulationSupported=false;probeError=String(error);
  }
}finally{await probe.close();}
const context=await browser.newContext(),offline=async value=>{
  dropConnections=value;
  if(emulationSupported)await context.setOffline(value);
};
const report=[];
try {
  const page=await context.newPage(),errors=[],failures=[];page.on('pageerror',error=>errors.push(error.message));
  page.on('requestfailed',request=>failures.push({url:request.url(),error:request.failure()?.errorText}));
  await page.goto(base);
  await page.evaluate(async()=>{
    window.api=await import('./api.mjs');window.events=[];
    addEventListener('message',e=>{if(e.source===document.querySelector('iframe').contentWindow)events.push(e.data);});
  });
  const prepare=options=>page.evaluate(async({entry,options})=>{window.prepared=await api.prepareRuntimeLaunch(entry,options);return {url:prepared.url,generation:prepared.generation,cached:prepared.cached};},{entry:root+'game.html',options});
  const blocked=async(options,pattern)=>{
    // Catch deliberate failures inside the page. Firefox reports a rejected
    // evaluate promise as pageerror before the driver handles assert.rejects.
    const reason=await page.evaluate(async({entry,options})=>{try{await api.prepareRuntimeLaunch(entry,options);return null;}catch(error){return error.message;}},{entry:root+'game.html',options});
    assert.equal(typeof reason,'string','multiplayer preparation must reject');assert.match(reason,pattern);
  };
  let visit=0;
  const navigate=async version=>{
    // Real launches have a fresh epoch parameter. Force distinct navigations in
    // this fixture too, rather than relying on same-src iframe reload behavior.
    await page.evaluate(value=>{window.events=[];const url=new URL(prepared.url);url.searchParams.set('probe',String(value));document.querySelector('iframe').src=url.href;},++visit);
    try {await page.waitForFunction(()=>events.length>0);}
    catch(error){await writeFile(resolve(out,'failure.json'),JSON.stringify({engine,errors,failures,requests},null,2));throw error;}
    const event=await page.evaluate(()=>events[0]);
    assert.deepEqual(event,{version,bytes:`wasm fixture ${version}`,controlled:true});
  };
  assert.equal((await prepare()).generation,A.descriptor.generation);await navigate('a');
  assert.equal(await page.evaluate(()=>!!navigator.serviceWorker.controller),false,'Runtime worker must not control Launcher');
  report.push('cold install registers runtime-only worker and caches complete program');

  await offline(true);requests.length=0;
  assert.equal(await page.evaluate(()=>fetch('./uncached-outage-control',{cache:'no-store'}).then(()=>true,()=>false)),false,'negative control must fail to reach the origin');
  assert.equal((await prepare()).generation,A.descriptor.generation);await navigate('a');
  assert.equal(requests.some(path=>path.endsWith('runtime-manifest.json')),false);
  assert.equal(await page.evaluate(()=>prepared.checkForUpdate()),null);
  report.push('offline launch runs HTML, module and WASM without a manifest response');
  await blocked({requireCurrent:true},/版本检查失败/);
  await offline(false);requests.length=0;
  await prepare();assert.equal(requests.length,0,'warm launch consults local authority only');
  assert.equal(await page.evaluate(()=>prepared.checkForUpdate()),null);
  assert.equal(requests.filter(path=>path.endsWith('runtime-manifest.json')).length,1);
  assert.equal(requests.some(path=>path.includes(A.descriptor.generation)),false);
  report.push('identical manifest downloads no program and announces no update');

  current=B;holdBytes=true;await prepare();
  await page.evaluate(()=>{window.updateState='pending';window.updating=prepared.checkForUpdate().then(value=>{window.updateState=value;return value;});});
  await page.waitForFunction(()=>performance.getEntriesByType('resource').some(e=>e.name.endsWith('runtime-manifest.json')));
  for(let attempts=0;!releaseBytes&&attempts<300;attempts++)await new Promise(r=>setTimeout(r,10));
  assert.ok(releaseBytes,'background download reached held WASM');
  // The multiplayer preparation shares staging work but cannot resolve to A.
  await page.evaluate(async entry=>{window.strictState='pending';window.strictPreparing=api.prepareRuntimeLaunch(entry,{requireCurrent:true}).then(value=>{window.strictState=value;return value;});},root+'game.html');
  assert.equal(await page.evaluate(()=>strictState),'pending');
  assert.equal(await page.evaluate(()=>updateState),'pending');
  assert.equal((await prepare()).generation,A.descriptor.generation);await navigate('a');
  holdBytes=false;releaseBytes();
  const ready=await page.evaluate(()=>updating);assert.equal(ready.generation,B.descriptor.generation);
  assert.equal((await page.evaluate(()=>strictPreparing)).generation,B.descriptor.generation);
  await blocked({requireCurrent:true,expectedGeneration:A.descriptor.generation},/双方重新准备/);
  assert.equal(await page.evaluate(()=>events[0].version),'a','running document stays on old program');
  assert.equal((await prepare()).generation,B.descriptor.generation);await navigate('b');
  report.push('incomplete successor stays invisible; verified successor is ready for next launch');

  current=C;corrupt=true;await prepare();
  assert.equal(await page.evaluate(()=>prepared.checkForUpdate()),null,'failed integrity check announces no update');
  assert.equal((await prepare()).generation,B.descriptor.generation);
  await blocked({requireCurrent:true},/integrity|mismatch/i);
  report.push('corrupt successor leaves last complete program launchable');
  corrupt=false;
  assert.equal((await page.evaluate(()=>prepared.checkForUpdate())).generation,C.descriptor.generation);
  await prepare();await navigate('c');
  await offline(true);await page.reload({waitUntil:'domcontentloaded'}).catch(()=>{});
  // Launcher itself is network-owned. Reopening its offline document is outside
  // this worker's scope; use a fresh online Launcher to verify persisted cache.
  await offline(false);await page.goto(base);
  await page.evaluate(async()=>{window.api=await import('./api.mjs');});
  await offline(true);
  assert.equal((await prepare()).generation,C.descriptor.generation);
  assert.deepEqual(errors,[]);
  report.push('persisted complete cache survives Launcher reload');
  const outageMode=emulationSupported?'browser-offline-plus-origin-drop':'origin-connection-drop';
  report.push('multiplayer blocks offline, wrong agreement and corrupt current generations; waits for complete successor');
  await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,engine,variant,version:browser.version(),outageMode,probeError,deviceAirplaneModeVerified:false,report},null,2));
  console.log(JSON.stringify({passed:true,engine,outageMode,report}));
} finally {releaseBytes?.();await browser.close();server.closeAllConnections();await new Promise(r=>server.close(r));}
