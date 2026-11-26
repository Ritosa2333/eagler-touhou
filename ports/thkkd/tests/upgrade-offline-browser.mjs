import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const root=fileURLToPath(new URL('../../../',import.meta.url)),output=resolve(root,'ports/thkkd/build/quality-mp/upgrade-offline');mkdirSync(output,{recursive:true});
const previous=resolve(process.env.THKKD_PREVIOUS_SITE||resolve(root,'../exports/thkkd-web-20261126/thkkd-web-local/site')),candidate=resolve(process.env.THKKD_CANDIDATE_SITE||resolve(root,'ports/thkkd/build/quality-mp/site'));
const url='http://127.0.0.1:8175/',expected=createHash('sha256').update(readFileSync(resolve(root,'ports/thkkd/runtime/thkkd.wasm'))).digest('hex');
const expectedShell=readFileSync(resolve(candidate,'app-shell-sw.js'),'utf8').match(/const CACHE_NAME[^\n]*-([a-f0-9]{20})/)[1];
const report={schema:'thkkd/upgrade-offline/1',checks:[]};let server,driver;
async function serving(site){
 server=spawn(process.execPath,[resolve(root,'launcher/scripts/serve-static.mjs'),site,'8175'],{cwd:root,windowsHide:true,stdio:'ignore'});
 for(let i=0;i<100;i++){if(server.exitCode!==null)throw Error('本机服务提前退出');try{if((await fetch(url)).ok)return;}catch{}await wait(50);}throw Error('本机服务没有就绪');
}
async function stopServer(){if(!server||server.exitCode!==null)return;const ended=new Promise(resolve=>server.once('exit',resolve));server.kill();await ended;server=null;}
try{
 await serving(previous);driver=await productionBrowser({url,report});const {page}=driver;await driver.library();await driver.start();report.previousRuntime=report.runtime;
 const data=new Uint8Array(329*4),view=new DataView(data.buffer);[1,2,3,0,5,4,11,13,6,0,77,80,1,0].forEach((value,index)=>view.setUint32(index*4,value,true));
 const replay=[...replayFixture()];await runtimeCommand(page,'write',{path:'th_kaikeidou01.dat',bytes:[...data]});await runtimeCommand(page,'write',{path:'replay/preserved.rpy',bytes:replay});await runtimeCommand(page,'sync');report.checks.push('old-delivery-installs-and-persists-native-save-and-replay');
 await stopServer();await serving(candidate);await driver.library({select:false});
 await page.evaluate(async()=>{const registration=await navigator.serviceWorker.ready;await registration.update();});
 await page.waitForFunction(async expected=>{const worker=navigator.serviceWorker.controller;if(!worker)return false;const status=await new Promise(resolve=>{const channel=new MessageChannel(),timer=setTimeout(()=>{channel.port1.close();resolve(null);},1500);channel.port1.onmessage=event=>{clearTimeout(timer);channel.port1.close();resolve(event.data);};worker.postMessage({type:'GET_APP_SHELL_STATUS'},[channel.port2]);});return status?.build===expected;},{timeout:60000},expectedShell);
 report.shellBuild=expectedShell;await wait(1000);await driver.library();let frame=await driver.start();report.updatedRuntime=report.runtime;
 let loaded=report.loads.filter(row=>row.url.endsWith('.wasm')).at(-1);
 if(loaded.sha256!==expected){
   await page.waitForFunction(()=>[...document.querySelectorAll('button')].some(button=>/^(Restart now|立即重启|立即重新开始)$/.test(button.textContent.trim())),{timeout:60000});
   report.programUpdatePrompt=await page.evaluate(()=>document.body.innerText.slice(-1600));
   const buttons=await page.$$('button');for(const button of buttons)if(await button.evaluate(element=>/^(Restart now|立即重启|立即重新开始)$/.test(element.textContent.trim()))){await button.click();break;}
   await page.waitForFunction(old=>{const frame=document.querySelector('#gameFrame')?.contentWindow;return frame&&frame.location.href!==old&&frame.__thkkdRuntime?.status().first;},{timeout:60000},report.updatedRuntime.url);
   frame=page.frames().find(frame=>frame.url().includes('thkkd.html'));await wait(200);loaded=report.loads.filter(row=>row.url.endsWith('.wasm')).at(-1);report.updatedRuntime=await frame.evaluate(async()=>({url:location.href,manifest:await(await fetch('./manifest.json')).json()}));
 }
 assert.equal(loaded.sha256,expected,'升级后实际加载本次 WASM');
 assert.equal(await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc)),77,'升级后原生核心读入原存档配置');assert.deepEqual((await runtimeCommand(page,'read',{path:'replay/preserved.rpy'})).bytes,replay);report.checks.push('old-to-current-runtime-upgrade-preserves-and-loads-user-data');
 await page.evaluate(()=>navigator.serviceWorker.ready);await runtimeCommand(page,'sync');await stopServer();await page.setOfflineMode(true);await driver.library();frame=await driver.start();report.offlineRuntime=report.runtime;
 assert.equal(await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc)),77);assert.deepEqual((await runtimeCommand(page,'read',{path:'replay/preserved.rpy'})).bytes,replay);assert.equal(report.loads.filter(row=>row.url.endsWith('.wasm')).at(-1).sha256,expected);report.checks.push('HTTP-server-stopped-and-browser-offline-reload-launch-native-data');
 await page.screenshot({path:resolve(output,'offline.png')});assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,wasm:expected}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);if(driver){report.failure=await driver.page.evaluate(()=>({url:location.href,text:document.body.innerText.slice(-2200),links:[...document.querySelectorAll('a[data-book-product="thkkd"]')].map(element=>({html:element.outerHTML.slice(0,650),rect:element.getBoundingClientRect().toJSON(),inert:!!element.closest('[inert]')}))}));await driver.page.screenshot({path:resolve(output,'failure.png')});}}
finally{if(driver){await driver.collect();await driver.browser.close();}await stopServer();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));}
