// Real Launcher + rebuilt ordinary TH075 Runtime. Private local DATA is required.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/';
const output=resolve('artifacts/th075-transitions');await mkdir(output,{recursive:true});
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
  console.log('select music',selected);await page.select('#musicSelect',selected);await page.waitForFunction(()=>{const e=document.querySelector('#launch');return e&&!e.disabled&&e.getBoundingClientRect().height>0;});await page.$eval('#launch',e=>e.click());console.log('launch requested');
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

try {
 const context=await browser.createBrowserContext(),page=await context.newPage();await page.setViewport({width:1000,height:650,isMobile:true,hasTouch:true});
 const runtime=await open(page,false,true);await runtime.evaluate(()=>Module.ccall('th075_return_title','number',[],[],{async:true}));await runtime.waitForFunction(()=>Module._th075_scene()===2,{timeout:30000});
 async function frames(n){const start=await runtime.evaluate(()=>Module._th075_frame());await runtime.waitForFunction(v=>Module._th075_frame()>=v,{},start+n);}
 async function tap(code='KeyZ'){await rpc(page,'keyboard',{code,down:true});await frames(3);await rpc(page,'keyboard',{code,down:false});await frames(5);}
 await frames(50);await tap();
 for(let i=0;i<14;i++){await frames(15);console.log('story selection',i,await runtime.evaluate(()=>[Module._th075_scene(),Module._th075_phase()]));if(await runtime.evaluate(()=>Module._th075_scene()===0))break;await tap();}
 await frames(45);await page.screenshot({path:resolve(output,'story.png')});
 await runtime.evaluate(()=>Module.ccall('th075_return_title','number',[],[],{async:true}));await runtime.waitForFunction(()=>Module._th075_scene()===2);await frames(50);
 for(let i=0;i<4;i++)await tap('ArrowDown');await tap();
 for(let i=0;i<20;i++){await frames(15);console.log('practice selection',i,await runtime.evaluate(()=>[Module._th075_scene(),Module._th075_phase()]));if(await runtime.evaluate(()=>Module._th075_scene()===0))break;await tap();}
 await frames(420);await page.screenshot({path:resolve(output,'practice-after-story.png')});
 assert.equal(await runtime.evaluate(()=>Module._th075_scene()),0);
 await runtime.evaluate(()=>{const original=Module._th075_touch_controls;globalThis.nativeActionSamples=[];Module._th075_touch_controls=(...v)=>{nativeActionSamples.push(v.slice(1,5).reduce((bits,n,i)=>bits|(n?1<<i:0),0));return original(...v);};const bridge=window.__eaglerDirectInputBridge;const epoch=bridge.epoch;if(bridge.submit({protocol:'eagler-touhou/1',game:'th075',epoch:epoch+1,command:'touch-controls',actionButtons:15})!==false||nativeActionSamples.length)throw Error('Stale bridge input accepted');});
 await page.evaluate(()=>{const target=document.querySelector('#gameFrame').contentWindow,original=target.postMessage;window.inputMessages=0;target.postMessage=function(message,...args){if(['touch-controls','keyboard','touch-cancel'].includes(message?.command))inputMessages++;return original.call(this,message,...args);};});
 const cdp=await page.createCDPSession();await cdp.send('Profiler.enable');await cdp.send('Profiler.start');
 console.log('rapid input profile');await page.evaluate(async()=>{const keys=['touchFire','touchFocus','touchBomb','touchFunction'];for(let i=0;i<120;i++){const b=document.getElementById(keys[i%4]);b.dispatchEvent(new PointerEvent('pointerdown',{bubbles:true,cancelable:true,pointerId:i+10,pointerType:'touch'}));await new Promise(r=>setTimeout(r,12));b.dispatchEvent(new PointerEvent('pointerup',{bubbles:true,cancelable:true,pointerId:i+10,pointerType:'touch'}));b.dispatchEvent(new PointerEvent('lostpointercapture',{bubbles:true,cancelable:true,pointerId:i+10,pointerType:'touch'}));await new Promise(r=>setTimeout(r,12));}});
 const {profile}=await cdp.send('Profiler.stop');await writeFile(resolve(output,'rapid.cpuprofile'),JSON.stringify(profile));const samples=await runtime.evaluate(()=>nativeActionSamples);assert.equal(samples.length,240);assert.deepEqual(samples,Array.from({length:240},(_,i)=>i%2===0?1<<((i/2)%4):0));assert.equal(await page.evaluate(()=>inputMessages),0,'Continuous input must use the immediate same-origin bridge');const result={storyThenPractice:true,nativeEdges:samples.length,duplicateReleases:0,queuedInputMessages:0,staleEpochRejected:true};await writeFile(resolve(output,'report.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));await context.close();
}finally{await browser.close();}

