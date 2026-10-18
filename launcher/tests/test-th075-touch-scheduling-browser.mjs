// Browser-managed touches plus a zero-delay timer gate model mobile tap scheduling.
// rAF remains available: drawing must not await SDL swap timers.
// Real Launcher + rebuilt ordinary TH075 Runtime. Private local DATA is required.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/';
const output=resolve('artifacts/th075-touch-scheduling');await mkdir(output,{recursive:true});
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

 await new Promise(resolve=>setTimeout(resolve,500));await page.evaluate(()=>document.querySelector('#firstUseNoticeCloseHint')?.click());
 await runtime.evaluate(()=>{const send=Module._th075_touch_controls;globalThis.actionEdges=0;let previous=0;Module._th075_touch_controls=(...args)=>{const bits=args.slice(1,5).reduce((bits,n,i)=>bits|(n?1<<i:0),0);if(bits!==previous){actionEdges++;previous=bits;}return send(...args);};});
 await runtime.evaluate(()=>{globalThis.frameGaps=[];let last=performance.now();function f(t){frameGaps.push(t-last);last=t;requestAnimationFrame(f);}requestAnimationFrame(f);});
 const points=await page.evaluate(()=>Object.fromEntries(['touchFire','touchFocus','touchBomb','touchFunction','touchJoystick'].map(id=>{let r=document.getElementById(id).getBoundingClientRect();return[id,{x:r.x+r.width/2,y:r.y+r.height/2}]})));
 const sleep=ms=>new Promise(r=>setTimeout(r,ms));
 const results=[];
 await page.evaluate(()=>{
  const frame=document.querySelector('#gameFrame').contentWindow;let blockedUntil=0;const timer=frame.setTimeout.bind(frame);window.timerGate=false;frame.delayedZeroTimers=0;
  document.addEventListener('pointerdown',()=>{if(window.timerGate)blockedUntil=performance.now()+100;},true);
  frame.setTimeout=(callback,delay,...args)=>{
   if(delay!==0||!window.timerGate)return timer(callback,delay,...args);
   frame.delayedZeroTimers++;
   const resume=()=>{const remaining=blockedUntil-performance.now();if(remaining>0)timer(resume,remaining+1);else callback(...args);};return timer(resume,0);
  };
 });
 for(const [button,moving] of [['touchFire',false],['touchFocus',false],['touchBomb',false],['touchFunction',false],['touchFire',true]]){
  await page.evaluate(moving=>window.timerGate=!moving,moving);
  await runtime.evaluate(()=>{frameGaps=[];actionEdges=0;delayedZeroTimers=0;globalThis.testStartFrame=Module._th075_frame();globalThis.testStartTime=performance.now();});
  const joystick=moving?await page.touchscreen.touchStart(points.touchJoystick.x,points.touchJoystick.y):null;
  for(let i=0;i<30;i++){
   if(joystick)await joystick.move(points.touchJoystick.x+25+10*Math.sin(i),points.touchJoystick.y);
   const finger=await page.touchscreen.touchStart(points[button].x,points[button].y);
   await sleep(24);await finger.end();await sleep(24);
  }
  if(joystick)await joystick.end();
  const v=await runtime.evaluate(()=>({frames:Module._th075_frame()-testStartFrame,ms:performance.now()-testStartTime,gaps:frameGaps,edges:actionEdges,swapTimers:delayedZeroTimers}));
  assert.equal(v.edges,60,button+' must deliver every down/up edge');assert.equal(v.swapTimers,0,'Rendering must not yield to zero-delay timers');
  const r={button,moving,edges:v.edges,swapTimers:v.swapTimers,frames:v.frames,ms:v.ms,fps:v.frames*1000/v.ms,maxGap:Math.max(...v.gaps),over50:v.gaps.filter(n=>n>50).length};results.push(r);console.log(r);
 }
 await writeFile(resolve(output,'report.json'),JSON.stringify(results,null,2));for(const result of results)assert.ok(result.fps>45,JSON.stringify(result));await context.close();
}finally{await browser.close();}
