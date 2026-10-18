import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const game=process.argv.includes('--th08')?'th08':'th10',base=game==='th08'?'http://127.0.0.1:8095':'http://127.0.0.1:8093',out=game+'_web/artifacts/architecture-candidate/validation';
const previous=JSON.parse(readFileSync(game+'_web/artifacts/sdl-release/site/packages/'+game+'/package.json'));
const next=JSON.parse(readFileSync(game+'_web/artifacts/architecture-candidate/site/packages/'+game+'/package.json'));
const replay=readFileSync(game==='th08'?'th08_web/artifacts/cpp/campaign/character0-final/original.rpy':'th10_web/artifacts/persistence/th10_01.rpy');
assert.notEqual(previous.revision,next.revision);assert.deepEqual(previous.files,next.files,'Visual patch reuses all DATA and music');
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:390,height:844},isMobile:true,hasTouch:true}),report={passed:false,errors:[],requests:[]};
page.on('pageerror',e=>report.errors.push(e.message));page.on('request',r=>report.requests.push(new URL(r.url()).pathname));
const select=async()=>{await page.locator('.game[data-game="'+game+'"]').click();await page.locator('#launch').click();await page.locator('#decisionConfirm').waitFor({state:'visible'});};
const runningGame=async()=>{await page.waitForFunction(game=>document.querySelector('#gameFrame')?.contentWindow?.['__'+game+'Runtime']?.app,game,{timeout:120000});return page.frames().find(f=>f.url().includes('/runtime/'+game+'/'));};
try{
 await page.goto(base+'/?debug=1');
 await page.evaluate(async ({descriptor,game})=>{const{installPackageFromRemote}=await import('/package/package-installer.mjs');await installPackageFromRemote(descriptor,{descriptorUrl:location.origin+'/packages/'+game+'/package.json',desiredFileIds:descriptor.base.files});},{descriptor:previous,game});
 await page.evaluate(()=>navigator.serviceWorker.ready.then(()=>true));await page.reload();await select();await page.locator('#decisionCancel').click();
 let frame=await runningGame();await frame.evaluate(async ({bytes,game})=>{const runtime=window['__'+game+'Runtime'];await runtime.command({command:'write',path:'replay/'+(game==='th08'?'th8':'th10')+'_24.rpy',bytes});await runtime.command({command:'sync'});await runtime.stop();},{bytes:Array.from(replay),game});
 await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('open'));await page.reload();await select();const start=report.requests.length;
 await page.locator('#decisionConfirm').click();await page.waitForFunction(()=>/游戏资源(?:更新失败|已更新)/.test(document.querySelector('#toastText').textContent),null,{timeout:120000});
 report.toast=await page.locator('#toastText').textContent();assert.equal(report.toast,'游戏资源已更新。');
 report.updateRequests=report.requests.slice(start).filter(p=>p.startsWith('/packages/'));assert.deepEqual(report.updateRequests,['/packages/'+game+'/package.json']);
 report.revision=await page.evaluate(async game=>{const{readCurrentPackageGeneration}=await import('/package/package-store.mjs');return(await readCurrentPackageGeneration(game)).generation.descriptor.revision;},game);assert.equal(report.revision,next.revision);
 frame=await runningGame();const saved=await frame.evaluate(game=>window['__'+game+'Runtime'].command({command:'read',path:'replay/'+(game==='th08'?'th8':'th10')+'_24.rpy'}),game);assert(Buffer.from(saved.bytes).equals(replay));
 await frame.waitForFunction(game=>{const c=window['__'+game+'Runtime'].core;return new Uint32Array(c.memory.buffer,c.sdl_stats(),9)[6]>0;},game);
 const stats=await frame.evaluate(game=>Array.from(new Uint32Array(window['__'+game+'Runtime'].core.memory.buffer,window['__'+game+'Runtime'].core.sdl_stats(),9)),game);assert(stats[6]>0);assert.equal(stats[7],0,'New GLES uploader loaded');
 await frame.evaluate(game=>window['__'+game+'Runtime'].stop(),game);assert.deepEqual(report.errors,[]);report.passed=true;report.checks=[previous.revision+' to '+next.revision+' through mobile launcher','unchanged game assets/music reused','imported replay preserved byte-for-byte','corrected runtime launches'];console.log(JSON.stringify(report));
}finally{await browser.close();writeFileSync(out+'/gles-update.json',JSON.stringify(report,null,2));}
