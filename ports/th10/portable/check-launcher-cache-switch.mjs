import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';

const base='http://127.0.0.1:8093';
let server,browser,page;
const report={passed:false,steps:[]};
async function until(predicate,arg){
  const deadline=Date.now()+60000;
  do{if(await page.evaluate(predicate,arg))return;await delay(100);}while(Date.now()<deadline);
  throw Error('Timed out waiting for the new cached release');
}
async function stopServer(){if(server&&!server.killed){const exited=once(server,'exit');server.kill();await exited;}server=null;}
async function startServer(site){
  await stopServer();
  server=spawn(process.execPath,['portable/serve.mjs','--site',site,'--port','8093'],{windowsHide:true,stdio:['ignore','pipe','pipe']});
  await new Promise((ok,fail)=>{server.on('error',fail);server.stdout.on('data',d=>{if(String(d).includes('http://'))ok();});server.stderr.on('data',d=>fail(Error(String(d))));});
}
try{
  browser=await launchBrowser();
  const context=await browser.newContext();page=await context.newPage();
  for(const root of ['th10_web/artifacts/sdl-release','th08_web/artifacts/sdl-release','th10_web/artifacts/architecture-candidate']){
    const site=resolve(root,'site'),release=JSON.parse(readFileSync(resolve(root,'release.json'),'utf8'));
    const inventory=JSON.parse(readFileSync(resolve(site,'files.json'),'utf8'));
    await startServer(site);
    if(page.url()==='about:blank'){
      // Keep a client at this origin without running the launcher's automatic
      // reloads, so each controller transition can be observed independently.
      await page.goto(base+'/manifest.json');
      await page.evaluate(async()=>{await navigator.serviceWorker.register('/app-shell-sw.js',{scope:'/',updateViaCache:'none'});await navigator.serviceWorker.ready;});
    }else await page.evaluate(async()=>{await(await navigator.serviceWorker.ready).update();});
    await until(async expected=>{
      if(!navigator.serviceWorker.controller)return false;
      const response=await fetch('/app-shell-sw.js');
      if(!(await response.text()).includes(expected))return false;
      const cacheName='eagler-touhou-app-shell-'+expected;
      return (await caches.keys()).includes(cacheName)&&!!(await(await caches.open(cacheName)).match('/__app-shell-meta__/'+expected));
    },release.buildId);
    // Install completion precedes activation; wait for reads through the
    // controlling worker to actually use the new cache, not just its existence.
    const expectedFiles=Object.fromEntries(['index.html','manifest.json','host-manifest.json','release-catalog.json','assets/launcher/app.mjs'].map(path=>[path,inventory[path]]));
    await until(async files=>{
      for(const [path,hash] of Object.entries(files)){
        const bytes=await(await fetch('/'+path)).arrayBuffer();
        const actual=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),b=>b.toString(16).padStart(2,'0')).join('');
        if(actual!==hash)return false;
      }
      return true;
    },expectedFiles);
    const observed=await page.evaluate(async()=>({
      manifest:await(await fetch('/manifest.json')).json(),
      catalog:await(await fetch('/release-catalog.json')).json(),
      caches:await caches.keys(),
    }));
    assert.equal(observed.manifest.game,release.manifest.game);
    assert.equal(observed.catalog.games[release.manifest.game]?.revision,release.revision);
    report.steps.push({root,buildId:release.buildId,game:observed.manifest.game,caches:observed.caches});
    console.log('Cache switch passed: '+root);
  }
  await page.goto(base+'/?lang=chs');
  await page.locator('#changelogConfirm').click();
  await page.locator('.game[data-game="th10"]').click();
  assert(await page.locator('#launch').isVisible());
  report.passed=true;
}catch(error){
  report.error=error.stack;
  if(page)report.snapshot=await page.evaluate(async()=>{
    const cached=[];
    for(const name of await caches.keys()){
      const cache=await caches.open(name),response=await cache.match('/release-catalog.json');
      cached.push({name,catalog:await response?.json()});
    }
    return {href:location.href,controller:navigator.serviceWorker.controller?.scriptURL,cached};
  }).catch(()=>null);
  console.log(JSON.stringify(report,null,2));throw error;
}finally{
  if(browser)await browser.close();await stopServer();
  mkdirSync('th10_web/artifacts/package-update-check',{recursive:true});
  writeFileSync('th10_web/artifacts/package-update-check/cache-switch.json',JSON.stringify(report,null,2));
}
