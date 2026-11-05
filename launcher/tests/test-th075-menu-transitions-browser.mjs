// Browser-managed touches plus a zero-delay timer gate model mobile tap scheduling.
// rAF remains available: drawing must not await SDL swap timers.
// Real Launcher + rebuilt ordinary TH075 Runtime. Private local DATA is required.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/';
const output=resolve('artifacts/th075-menu-transitions');await mkdir(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
const report=[];
async function rpc(page,command,fields={}){return page.evaluate(({command,fields})=>new Promise((yes,no)=>{
  const frame=document.querySelector('#gameFrame'),epoch=Number(new URL(frame.src).searchParams.get('runtimeEpoch')),request=crypto.randomUUID();
  const timer=setTimeout(()=>{window.removeEventListener('message',receive);no(Error('RPC timed out: '+command));},30000);
  function receive(e){if(e.source!==frame.contentWindow||e.data?.request!==request||e.data.epoch!==epoch)return;clearTimeout(timer);window.removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}
  window.addEventListener('message',receive);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th075',epoch,command,request,...fields},location.origin);
}),{command,fields});}
async function open(page,music,touch=false){
  console.log('open page');await page.goto(base+'?test=th075');await page.waitForFunction(()=>window.__eaglerBoot?.done===true);
  await page.evaluate(()=>{document.querySelector('#firstUseNoticeDialog')?.close();globalThis.runtimeEvents=[];window.addEventListener('message',e=>{if(e.data?.game==='th075')runtimeEvents.push(e.data);});});
  console.log('boot complete');await page.$eval('.game[data-game=th075]',e=>{e.click();e.click();});
  await page.waitForFunction(()=>document.querySelector('#musicSelect'));
  const modes=await page.$$eval('#musicSelect option',nodes=>nodes.filter(n=>!n.disabled).map(n=>n.value));
  assert.ok(!modes.includes('midi'),modes);const selected=music?modes.find(v=>v.includes('ogg')):'none';assert.ok(selected,modes);
  if(touch){await page.$eval('#touchToggle',e=>{if(e.getAttribute('aria-checked')!=='true')e.click();});await page.evaluate(()=>document.querySelector('#decisionConfirm')?.click());}
  console.log('select music',selected);if(process.env.TH075_TEST_LANGUAGE)await page.select('#languageSelect',process.env.TH075_TEST_LANGUAGE);await page.select('#musicSelect',selected);await page.waitForFunction(()=>{const e=document.querySelector('#launch');return e&&!e.disabled&&e.getBoundingClientRect().height>0;});await page.$eval('#launch',e=>e.click());console.log('launch requested');
  const deadline=Date.now()+120000;let runtime,lastLog=Date.now();
  while(Date.now()<deadline){
    await page.evaluate(()=>{if(document.querySelector('#decisionDialog[open]'))document.querySelector('#decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
    if(Date.now()-lastLog>10000){console.log('launch status',await page.evaluate(()=>({status:document.querySelector('#playerStatus')?.textContent,dialogs:[...document.querySelectorAll('dialog[open]')].map(d=>d.textContent),frame:document.querySelector('#gameFrame')?.src,body:document.body.innerText.slice(-600)})));lastLog=Date.now();}
    runtime=page.frames().find(f=>f.url().includes('/th075.html'));
    const error=runtime&&await runtime.evaluate(()=>document.querySelector('#error')?.textContent).catch(()=>'');if(error)throw Error(error);
    if(runtime&&await runtime.evaluate(()=>globalThis.Module?._th075_frame()>90).catch(()=>false))return runtime;
    await new Promise(r=>setTimeout(r,200));
  }
  await page.screenshot({path:resolve(output,'failed.png')});throw Error('TH075 launch timed out: '+await page.$eval('#playerStatus',e=>e.textContent).catch(()=>''));
}


try{
 const context=await browser.createBrowserContext(),page=await context.newPage();await page.setViewport({width:1000,height:650,isMobile:true,hasTouch:true});
 const runtime=await open(page,process.env.TH075_MENU_MUSIC==='ogg',true);
 await runtime.evaluate(()=>{globalThis.ticks=[];globalThis.assetReads=[];const call=Module.ccall;Module.ccall=function(name,...args){const start=performance.now();const result=call.call(this,name,...args);if(name!=='th075_tick')return result;return Promise.resolve(result).then(value=>{ticks.push(performance.now()-start);return value;});};const read=Module.readAsset;Module.readAsset=async function(path){assetReads.push(path);return read.call(this,path);};});
 async function frames(n){const start=await runtime.evaluate(()=>Module._th075_frame());await runtime.waitForFunction(v=>Module._th075_frame()>=v,{},start+n);}
 async function tap(code='KeyZ'){await rpc(page,'keyboard',{code,down:true});await frames(2);await rpc(page,'keyboard',{code,down:false});await frames(3);}
 const results=[];
 for(let round=0;round<2;round++)for(const [index,scene] of [[0,3],[5,5],[6,12],[7,14],[8,9]]){
  await runtime.evaluate(()=>Module.ccall('th075_return_title','number',[],[],{async:true}));await runtime.waitForFunction(()=>Module._th075_scene()===2);await frames(55);
  // Return-title preserves selection; navigate from the previously entered item.
  const previous=results.at(-1)?.index??0;for(let n=0;n<(index-previous+10)%10;n++)await tap('ArrowDown');
  await runtime.evaluate(()=>{ticks=[];assetReads=[];});await tap();await runtime.waitForFunction(s=>Module._th075_scene()===s,{timeout:30000},scene);await frames(45);
  const result=await runtime.evaluate(()=>({maxTickMs:Math.max(...ticks),slowTicks:ticks.filter(n=>n>50).length,assetReads}));result.round=round;result.index=index;result.scene=scene;result.heapBytes=await runtime.evaluate(()=>Module.HEAPU8.byteLength);if(round===0)await page.screenshot({path:resolve(output,'menu-'+index+'.png')});results.push(result);console.log(JSON.stringify(result));
 }
 await writeFile(resolve(output,'report.json'),JSON.stringify(results,null,2));
 if(process.env.TH075_MENU_BASELINE!=='1'){for(const r of results){assert.equal(r.assetReads.length,0,JSON.stringify(r));assert.ok(r.maxTickMs<100,JSON.stringify(r));}const first=Math.max(...results.filter(r=>r.round===0).map(r=>r.heapBytes));assert.ok(results.filter(r=>r.round===1).every(r=>r.heapBytes<=first+16*1024*1024),'Repeated menu visits must retain a bounded heap');}
 await context.close();
}finally{await browser.close();}
