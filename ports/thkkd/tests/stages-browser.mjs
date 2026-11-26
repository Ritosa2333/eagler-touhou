import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {replayFixture} from './replay-fixture.mjs';
const output=fileURLToPath(new URL('../build/stages-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:960,height:720}});
const report={browser:await browser.version(),cases:[],errors:[]};
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data',{waitUntil:'load'});await page.waitForFunction(()=>globalThis.__thkkdRuntime?.status().first||document.querySelector('#error').textContent,{timeout:30000});
 const cases=[...Array.from({length:6},(_,stage)=>({stage,difficulty:stage%4,practice:true})),{stage:6,difficulty:4},{stage:7,difficulty:5}];
 for(const item of cases){
   const bytes=Array.from(replayFixture({...item,bombFrame:-1}));
   await page.evaluate(async bytes=>{const r=__thkkdRuntime;await r.command({command:'write',path:'replay/stage-fixture.rpy',bytes});await r.command({command:'playback',action:'open',path:'replay/stage-fixture.rpy'});await r.command({command:'playback',action:'pause'});await r.command({command:'playback',action:'seek',value:150});},bytes);
   await page.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===150||document.querySelector('#error').textContent,{timeout:15000});
   await new Promise(r=>setTimeout(r,150));
   const state=await page.evaluate(()=>({stage:__thkkdRuntime.core._thkkd_word(0xb3e2ec),difficulty:__thkkdRuntime.core._thkkd_word(0xa7e610),cursor:__thkkdRuntime.core._thkkd_word(0xb43110),commands:__thkkdRuntime.platform.renderer.commands.length,mesh:!!__thkkdRuntime.platform.debug3d,error:document.querySelector('#error').textContent}));
   await page.screenshot({path:output+`stage-${item.stage}.png`});assert.equal(state.error,'');assert.equal(state.stage,item.stage);assert.equal(state.difficulty,item.difficulty);assert.ok(state.commands>0);report.cases.push({...item,state,result:'passed'});console.log(JSON.stringify({stage:item.stage,result:'passed'}));
 }
 assert.equal(report.errors.length,0);report.result='passed';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
