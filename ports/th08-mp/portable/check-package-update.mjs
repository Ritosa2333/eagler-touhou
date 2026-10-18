import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {readFileSync, mkdirSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';

const argument=(key,fallback)=>process.argv.includes(key)?process.argv[process.argv.indexOf(key)+1]:fallback;
const site=resolve(argument('--site','th10_web/artifacts/architecture-candidate/site'));
const output=resolve(argument('--out','th10_web/artifacts/package-update-check'));
const expectFailure=process.argv.includes('--expect-failure'),base='http://127.0.0.1:8093';
mkdirSync(output,{recursive:true});
const oldDescriptor=JSON.parse(readFileSync('th10_web/artifacts/sdl-release-3.1.0-previous/site/packages/th10/package.json','utf8'));
const expected=JSON.parse(readFileSync(resolve(site,'packages/th10/package.json'),'utf8'));
let server,browser;
async function stopServer(){if(server&&!server.killed){const stopped=once(server,'exit');server.kill();await stopped;}server=null;}
async function serve(root){await stopServer();server=spawn(process.execPath,['portable/serve.mjs','--site',root,'--port','8093'],{windowsHide:true,stdio:['ignore','pipe','pipe']});await new Promise((ok,fail)=>{server.stdout.on('data',data=>{if(String(data).includes('http://'))ok();});server.stderr.on('data',data=>fail(Error(String(data))));server.on('error',fail);server.on('exit',code=>fail(Error('server exit '+code)));});}
const report={passed:false,expectFailure,checks:[],errors:[],toasts:[],requests:[]};
try {
  await serve(site);
  browser=await launchBrowser();
  const context=await browser.newContext(),page=await context.newPage();
  page.on('pageerror',error=>report.errors.push(error.message));
  page.on('request',request=>report.requests.push(new URL(request.url()).pathname));
  await page.exposeFunction('recordUpdateToast',text=>report.toasts.push(text));
  await page.addInitScript(()=>addEventListener('DOMContentLoaded',()=>{
    const element=document.querySelector('#toastText');
    if(element)new MutationObserver(()=>window.recordUpdateToast(element.textContent)).observe(element,{subtree:true,childList:true,characterData:true});
  }));
  await page.goto(base+'/?lang=chs&debug=1');
  await page.evaluate(async descriptor=>{
    const {installPackageFromRemote}=await import('/package/package-installer.mjs');
    await installPackageFromRemote(descriptor,{descriptorUrl:location.origin+'/packages/th10/package.json',desiredFileIds:descriptor.base.files});
  },oldDescriptor);
  await page.evaluate(async()=>{await navigator.serviceWorker.ready;});
  await page.reload();
  await page.locator('.game[data-game="th10"]').click();
  await page.locator('#launch').click();
  await page.locator('#decisionConfirm').waitFor({state:'visible'});
  if(!expectFailure){
    // Write through the real runtime before updating; this exercises the same
    // save/replay database as a player, including shutdown persistence.
    await page.locator('#decisionCancel').click();
    await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th10Runtime?.app,null,{timeout:120000});
    const frame=page.frames().find(frame=>frame.url().includes('/runtime/th10/'));
    const replay=Array.from(readFileSync('th10_web/artifacts/sdl3/motion/recording.rpyx'));
    await frame.evaluate(async bytes=>{await __th10Runtime.command({command:'write',path:'replay/th10_24.rpyx',bytes});await __th10Runtime.stop();},replay);
    await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('open'));
    await page.reload();
    await page.locator('.game[data-game="th10"]').click();
    await page.locator('#launch').click();
    await page.locator('#decisionConfirm').waitFor({state:'visible'});
  }
  const before=report.requests.length;
  await page.locator('#decisionConfirm').click();
  await page.waitForFunction(()=>/游戏资源(?:更新失败|已更新)/.test(document.querySelector('#toastText').textContent),null,{timeout:120000});
  report.updateToast=await page.locator('#toastText').textContent();
  report.updateRequests=report.requests.slice(before).filter(path=>path.startsWith('/packages/'));
  report.installedRevision=await page.evaluate(async()=>{const {readCurrentPackageGeneration}=await import('/package/package-store.mjs');return (await readCurrentPackageGeneration('th10')).generation.descriptor.revision;});
  if(expectFailure){
    assert.match(report.updateToast,/unknown requested Package component: language/);
    assert.equal(report.installedRevision,oldDescriptor.revision);
    report.checks.push('Reproduced built-in Chinese update requesting a missing language component');
  }else{
    assert.equal(report.updateToast,'游戏资源已更新。');
    assert.equal(report.installedRevision,expected.revision);
    assert.deepEqual(report.updateRequests,['/packages/th10/package.json']);
    await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th10Runtime?.app,null,{timeout:120000});
    const frame=page.frames().find(frame=>frame.url().includes('/runtime/th10/'));
    const saved=await frame.evaluate(()=>__th10Runtime.command({command:'read',path:'replay/th10_24.rpyx'}));
    assert(Buffer.from(saved.bytes).equals(readFileSync('th10_web/artifacts/sdl3/motion/recording.rpyx')));
    report.checks.push('Chinese update succeeds','Unchanged DATA/music reused without downloads','Replay preserved byte-for-byte','Updated game launches');
    await frame.evaluate(()=>__th10Runtime.stop());
    assert.equal(report.errors.length,0,report.errors.join('\n'));
  }
  report.passed=true;
  console.log(JSON.stringify({passed:true,toast:report.updateToast,checks:report.checks,updateRequests:report.updateRequests},null,2));
}catch(error){report.failure=error.stack;throw error;}
finally{if(browser)await browser.close();await stopServer();writeFileSync(resolve(output,expectFailure?'reproduced.json':'regression.json'),JSON.stringify(report,null,2));}
