import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const replay=process.argv[2];if(!replay)throw Error('Provide an authorized local .rpy fixture');
const bytes=Array.from(readFileSync(replay)),output=fileURLToPath(new URL('../build/replay-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:960,height:720}});
const report={browser:await browser.version(),errors:[]};
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data',{waitUntil:'load'});await page.waitForFunction(()=>globalThis.__thkkdRuntime?.status().first||document.querySelector('#error').textContent,{timeout:30000});
 const startupError=await page.$eval('#error',e=>e.textContent);if(startupError)throw Error(startupError);
 report.open=await page.evaluate(async bytes=>{const r=__thkkdRuntime;await r.command({command:'write',path:'replay/verification.rpy',bytes});const {description}=await r.command({command:'playback',action:'inspect',path:'replay/verification.rpy'});await r.command({command:'playback',action:'open',path:'replay/verification.rpy'});return {character:description.character,shot:description.shot,difficulty:description.difficulty,frames:description.stages[0].frames};},bytes);
 await new Promise(r=>setTimeout(r,2000));await page.screenshot({path:output+'replay.png'});
 report.before=await page.evaluate(()=>({cursor:__thkkdRuntime.core._thkkd_word(0xb43110),scene:__thkkdRuntime.core._thkkd_word(0xa8ce58),difficulty:__thkkdRuntime.core._thkkd_word(0xa7e610),stage:__thkkdRuntime.core._thkkd_word(0xb3e2ec),nativeCount:__thkkdRuntime.core._thkkd_word(0xb43a08),mode:__thkkdRuntime.core._thkkd_word(0xb4310c),error:document.querySelector('#error').textContent}));
 await page.evaluate(()=>__thkkdRuntime.command({command:'playback',action:'pause'}));await new Promise(r=>setTimeout(r,200));report.paused=await page.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xb43110));
 await page.evaluate(()=>__thkkdRuntime.command({command:'playback',action:'seek',value:240}));await new Promise(r=>setTimeout(r,1500));report.seek=await page.evaluate(()=>({cursor:__thkkdRuntime.core._thkkd_word(0xb43110),error:document.querySelector('#error').textContent}));
 await page.evaluate(()=>__thkkdRuntime.command({command:'playback',action:'seek',value:60}));await new Promise(r=>setTimeout(r,1500));report.backward=await page.evaluate(()=>({cursor:__thkkdRuntime.core._thkkd_word(0xb43110),error:document.querySelector('#error').textContent}));
 writeFileSync(output+'result.json',JSON.stringify(report,null,2));console.log(JSON.stringify(report));if(report.errors.length||report.before.error||report.seek.error||report.backward.error||report.seek.cursor!==240||report.backward.cursor!==60)process.exitCode=1;
}finally{await browser.close();}
