import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const output=fileURLToPath(new URL('../build/storage-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required']});
const report={browser:await browser.version(),checks:[]};
try{
 const page=await browser.newPage();await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data',{waitUntil:'load'});
 const ready=()=>page.waitForFunction(()=>globalThis.__thkkdRuntime?.status().first||document.querySelector('#error').textContent,{timeout:30000});await ready();
 await page.evaluate(async()=>{await __thkkdRuntime.command({command:'write',path:'config.txt',bytes:[48,49,48,48,48,10]});await __thkkdRuntime.command({command:'sync'});});
 await page.reload({waitUntil:'load'});await ready();const restored=await page.evaluate(async()=>(await __thkkdRuntime.command({command:'read',path:'config.txt'})).bytes);assert.deepEqual(restored,[48,49,48,48,48,10]);report.checks.push('refresh-persistence');
 const rejected=await page.evaluate(async()=>{for(const path of ['../outside.txt','/assets/anything','replay/../outside.rpy'])try{await __thkkdRuntime.command({command:'write',path,bytes:[1]});return false;}catch{}return true;});assert.ok(rejected);report.checks.push('path-boundaries');
 await page.evaluate(()=>__thkkdRuntime.stop('test'));await page.evaluate(()=>__thkkdRuntime.command({command:'launch'}));await new Promise(r=>setTimeout(r,300));
 assert.deepEqual(await page.evaluate(async()=>(await __thkkdRuntime.command({command:'read',path:'config.txt'})).bytes),restored);report.checks.push('exit-restart-persistence');
 report.result='passed';writeFileSync(output+'result.json',JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}finally{await browser.close();}
