import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const base='http://127.0.0.1:8095',out='th08_web/artifacts/architecture-candidate/validation';
const oldSite='th08_web/artifacts/sdl-release/site',newSite='th08_web/artifacts/architecture-candidate/site';
const previous=JSON.parse(readFileSync(oldSite+'/packages/th08/package.json')),next=JSON.parse(readFileSync(newSite+'/packages/th08/package.json'));
assert.deepEqual(previous.files['game-data'],next.files['game-data']);
assert(Object.values(next.files).filter(f=>f.target.startsWith('/music/')).every(f=>f.target.endsWith('.ogg')));
const replay=readFileSync(out+'/recorded-first-slot.rpy');
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:390,height:844},isMobile:true,hasTouch:true}),page=await context.newPage(),report={passed:false,errors:[],requests:[]};
page.on('pageerror',e=>report.errors.push(e.message));page.on('request',r=>report.requests.push(new URL(r.url()).pathname));
try{
 // Real old FLAC bytes are mounted only for the migration fixture.
 await page.route('**/packages/th08/music/*.flac',route=>route.fulfill({body:readFileSync(oldSite+new URL(route.request().url()).pathname),contentType:'audio/flac'}));
 await page.goto(base+'/runtime/th08/th08.html?manual=1');await page.waitForFunction(()=>window.__th08Runtime,null,{timeout:120000});
 await page.evaluate(bytes=>__th08Runtime.command({command:'write',path:'replay/th8_01.rpy',bytes}),Array.from(replay));
 await page.goto(base+'/?debug=1');await page.evaluate(async descriptor=>{const{installPackageFromRemote}=await import('/package/package-installer.mjs');await installPackageFromRemote(descriptor,{descriptorUrl:location.origin+'/packages/th08/package.json',desiredFileIds:descriptor.base.files});},previous);
 await page.evaluate(()=>navigator.serviceWorker.ready.then(()=>true));await page.reload();
 await page.locator('.game[data-game="th08"]').click();await page.locator('#launch').click();await page.locator('#decisionConfirm').waitFor({state:'visible'});
 assert.match(await page.locator('#decisionMessage').textContent(),/OGG/);assert(await page.locator('#decisionSecondary').isHidden());
 // Cancelling cannot accidentally start an OGG-only runtime with old FLAC.
 await page.locator('#decisionCancel').click();await page.waitForTimeout(300);
 assert.equal(await page.evaluate(()=>!!document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app),false);
 await page.reload();await page.locator('.game[data-game="th08"]').click();await page.locator('#launch').click();await page.locator('#decisionConfirm').waitFor({state:'visible'});
 const begin=report.requests.length;await page.locator('#decisionConfirm').click();
 await page.waitForFunction(()=>/游戏资源已更新|更新失败/.test(document.querySelector('#toastText').textContent),null,{timeout:120000});
 assert.equal(await page.locator('#toastText').textContent(),'游戏资源已更新。');
 report.updateRequests=report.requests.slice(begin).filter(p=>p.startsWith('/packages/'));
 assert(!report.updateRequests.some(p=>p.endsWith('.data')),'Reuse existing DATA');assert(!report.updateRequests.some(p=>p.endsWith('.flac')),'Never fetch FLAC for new generation');
 assert.equal(new Set(report.updateRequests.filter(p=>p.endsWith('.ogg'))).size,21);
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app,null,{timeout:120000});let frame=page.frames().find(f=>f.url().includes('/runtime/th08/'));
 await frame.waitForFunction(()=>{const c=__th08Runtime.core;return new Float32Array(c.memory.buffer,c.sdl_audio_stats()+40,1)[0]>.001;});
 const saved=await frame.evaluate(()=>__th08Runtime.command({command:'read',path:'replay/th8_01.rpy'}));assert(Buffer.from(saved.bytes).equals(replay));
 report.installed=await page.evaluate(async()=>{const{readCurrentPackageGeneration}=await import('/package/package-store.mjs');const g=(await readCurrentPackageGeneration('th08')).generation;return {revision:g.descriptor.revision,targets:Object.values(g.descriptor.files).map(f=>f.target)};});assert.equal(report.installed.revision,next.revision);
 await frame.evaluate(()=>__th08Runtime.stop());await context.setOffline(true);await page.reload();await page.locator('.game[data-game="th08"]').click();await page.locator('#launch').click();
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app,null,{timeout:120000});frame=page.frames().find(f=>f.url().includes('/runtime/th08/'));
 await frame.waitForFunction(()=>{const c=__th08Runtime.core;return new Float32Array(c.memory.buffer,c.sdl_audio_stats()+40,1)[0]>.001;});await frame.evaluate(()=>__th08Runtime.stop());
 assert.deepEqual(report.errors,[]);report.passed=true;report.checks=['old FLAC generation migrates through mobile launcher','cancel preserves saves and blocks incompatible launch','all 21 OGG replace FLAC','unchanged DATA reused','actual recorded replay preserved','OGG audio plays online and offline'];console.log(JSON.stringify(report));
}finally{writeFileSync(out+'/ogg-update.json',JSON.stringify(report,null,2));await browser.close();}
