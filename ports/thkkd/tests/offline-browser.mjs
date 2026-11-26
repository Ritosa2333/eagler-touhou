import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {spawn} from 'node:child_process';
import {writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../../',import.meta.url)),output=fileURLToPath(new URL('../build/offline-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const port=8175,url=`http://127.0.0.1:${port}/`;
const server=spawn(process.execPath,[root+'launcher/scripts/serve-static.mjs',root+'ports/thkkd/build/site',String(port)],{cwd:root,windowsHide:true,stdio:'ignore'});
let browser;
const report={checks:[],errors:[]};
try{
 for(let i=0;i<40;i++){try{if((await fetch(url)).ok)break;}catch{}await new Promise(r=>setTimeout(r,100));}
 browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:1280,height:800}});report.browser=await browser.version();
 const page=await browser.newPage();page.on('pageerror',error=>report.errors.push(error.message));
 const ready=()=>page.waitForFunction(()=>[...document.querySelectorAll('iframe')].some(frame=>frame.contentWindow?.__thkkdRuntime?.status().first),{timeout:60000});
 const runtime=async function(fn,...args){const frames=page.frames();for(const frame of frames)if(await frame.evaluate(()=>!!globalThis.__thkkdRuntime).catch(()=>false))return frame.evaluate(fn,...args);throw Error('Runtime iframe missing');};
 async function launch(){
   const first=await page.$('#firstUseNoticeDialog[open]');if(first){await page.evaluate(()=>[...document.querySelectorAll('#firstUseNoticeDialog button')].find(b=>b.textContent.trim()==='Got it')?.click());await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog')?.open,{timeout:10000});}
   if(!new URL(page.url()).searchParams.has('game')){await page.click('a[data-book-product="thkkd"]');await page.waitForFunction(()=>new URL(location.href).searchParams.get('game')==='thkkd');}
   await page.waitForFunction(()=>[...document.querySelectorAll('button')].some(b=>b.textContent.trim()==='Start game'&&!b.disabled),{timeout:15000});
   await page.evaluate(()=>[...document.querySelectorAll('button')].find(b=>b.textContent.trim()==='Start game')?.click());await ready();
 }
 await page.goto(url,{waitUntil:'networkidle0'});await launch();report.checks.push('hosted-package-install-and-first-frame');
 await runtime(async()=>{await __thkkdRuntime.command({command:'write',path:'config.txt',bytes:[48,49,48,48,48,10]});await __thkkdRuntime.command({command:'sync'});});
 await page.evaluate(()=>navigator.serviceWorker.ready);
 report.serviceWorker=await page.evaluate(async()=>({active:(await navigator.serviceWorker.ready).active.scriptURL}));
 // 关闭实际 HTTP 服务，再以 DevTools 离线模式重新载入页面。
 server.kill();await new Promise(resolve=>server.once('exit',resolve));await page.setOfflineMode(true);
 await page.reload({waitUntil:'networkidle0',timeout:30000});await launch();report.checks.push('offline-reload-and-launch-with-server-stopped');
 const restored=await runtime(async()=>(await __thkkdRuntime.command({command:'read',path:'config.txt'})).bytes);assert.deepEqual(restored,[48,49,48,48,48,10]);report.checks.push('offline-user-data-preserved');
 report.audio=await runtime(()=>({state:__thkkdRuntime.platform.context.state,bgm:[...__thkkdRuntime.platform.sounds.values()].filter(s=>s.bgm).length}));
 await page.screenshot({path:output+'offline.png'});assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify(report));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{server.kill();if(browser)await browser.close();writeFileSync(output+'result.json',JSON.stringify(report,null,2));}
