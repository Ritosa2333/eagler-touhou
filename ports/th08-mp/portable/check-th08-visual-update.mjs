import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const base='http://127.0.0.1:8095',out='th08_web/artifacts/architecture-candidate/validation';
const previous=JSON.parse(readFileSync('th08_web/artifacts/sdl-release/site/packages/th08/package.json'));
const next=JSON.parse(readFileSync('th08_web/artifacts/architecture-candidate/site/packages/th08/package.json'));
const replay=readFileSync('th08_web/artifacts/cpp/campaign/character0-final/original.rpy');
assert.notEqual(previous.revision,next.revision);assert.deepEqual(previous.files,next.files,'Visual patch reuses all DATA and music');
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:390,height:844},isMobile:true,hasTouch:true}),report={passed:false,errors:[],requests:[]};
page.on('pageerror',e=>report.errors.push(e.message));page.on('request',r=>report.requests.push(new URL(r.url()).pathname));
const select=async()=>{await page.locator('.game[data-game="th08"]').click();await page.locator('#launch').click();await page.locator('#decisionConfirm').waitFor({state:'visible'});};
const game=async()=>{await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app,null,{timeout:120000});return page.frames().find(f=>f.url().includes('/runtime/th08/'));};
try{
 await page.goto(base+'/?debug=1');
 await page.evaluate(async descriptor=>{const{installPackageFromRemote}=await import('/package/package-installer.mjs');await installPackageFromRemote(descriptor,{descriptorUrl:location.origin+'/packages/th08/package.json',desiredFileIds:descriptor.base.files});},previous);
 await page.evaluate(()=>navigator.serviceWorker.ready.then(()=>true));await page.reload();await select();await page.locator('#decisionCancel').click();
 let frame=await game();await frame.evaluate(async bytes=>{await __th08Runtime.command({command:'write',path:'replay/th8_24.rpy',bytes});await __th08Runtime.command({command:'sync'});await __th08Runtime.stop();},Array.from(replay));
 await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('open'));await page.reload();await select();const start=report.requests.length;
 await page.locator('#decisionConfirm').click();await page.waitForFunction(()=>/游戏资源(?:更新失败|已更新)/.test(document.querySelector('#toastText').textContent),null,{timeout:120000});
 report.toast=await page.locator('#toastText').textContent();assert.equal(report.toast,'游戏资源已更新。');
 report.updateRequests=report.requests.slice(start).filter(p=>p.startsWith('/packages/'));assert.deepEqual(report.updateRequests,['/packages/th08/package.json']);
 report.revision=await page.evaluate(async()=>{const{readCurrentPackageGeneration}=await import('/package/package-store.mjs');return(await readCurrentPackageGeneration('th08')).generation.descriptor.revision;});assert.equal(report.revision,next.revision);
 frame=await game();const saved=await frame.evaluate(()=>__th08Runtime.command({command:'read',path:'replay/th8_24.rpy'}));assert(Buffer.from(saved.bytes).equals(replay));
 assert(await frame.evaluate(()=>typeof __th08Runtime.core.visual_diagnostics==='function'),'Corrected C++ runtime loaded');
 await frame.evaluate(()=>__th08Runtime.stop());assert.deepEqual(report.errors,[]);report.passed=true;report.checks=['3.2 to 3.2.1 package update through mobile launcher','unchanged game assets/music reused','imported replay preserved byte-for-byte','corrected runtime launches'];console.log(JSON.stringify(report));
}finally{await browser.close();writeFileSync(out+'/visual-update.json',JSON.stringify(report,null,2));}
