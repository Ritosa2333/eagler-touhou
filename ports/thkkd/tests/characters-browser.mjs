import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {replayFixture} from './replay-fixture.mjs';
const output=fileURLToPath(new URL('../build/characters-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:960,height:720}});
const report={browser:await browser.version(),cases:[],errors:[]};
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data',{waitUntil:'load'});await page.waitForFunction(()=>globalThis.__thkkdRuntime?.status().first||document.querySelector('#error').textContent,{timeout:30000});
 for(let character=0;character<3;character++)for(let shot=0;shot<3;shot++){
  const bytes=Array.from(replayFixture({character,shot}));
  await page.evaluate(async bytes=>{const r=__thkkdRuntime;await r.command({command:'write',path:'replay/web-fixture.rpy',bytes});await r.command({command:'playback',action:'open',path:'replay/web-fixture.rpy'});await r.command({command:'playback',action:'pause'});await r.command({command:'playback',action:'seek',value:70});},bytes);
  await page.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===70||document.querySelector('#error').textContent,{timeout:10000});
  const before=await page.evaluate(()=>{const c=__thkkdRuntime.core,w=a=>c._thkkd_word(a),sum=(base,count,stride)=>Array.from({length:count},(_,i)=>w(base+i*stride)===1).filter(Boolean).length;return {character:w(0xa74c0c),shot:w(0xa7e614),difficulty:w(0xa7e610),spells:w(0xa8cd80),shots:sum(0xa87be0,20,24)+sum(0xa79090,80,56),error:document.querySelector('#error').textContent};});
  await page.screenshot({path:output+`${character}-${shot}-shot.png`});
  await page.evaluate(()=>__thkkdRuntime.command({command:'playback',action:'seek',value:110}));await page.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===110||document.querySelector('#error').textContent,{timeout:10000});
  const after=await page.evaluate(()=>({spells:__thkkdRuntime.core._thkkd_word(0xa8cd80),bombTimer:__thkkdRuntime.core._thkkd_word(0xa8cd94),error:document.querySelector('#error').textContent}));
  await page.screenshot({path:output+`${character}-${shot}-bomb.png`});
  assert.equal(before.character,character);assert.equal(before.shot,shot);assert.ok(before.shots>0);assert.equal(after.spells,before.spells-1);assert.ok(after.bombTimer>0);assert.equal(after.error,'');
  report.cases.push({character,shot,before,after,result:'passed'});console.log(JSON.stringify({character,shot,result:'passed'}));
 }
 assert.equal(report.errors.length,0);report.result='passed';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
