import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18330/';
const out=resolve(import.meta.dirname,'../.cache/launcher-room');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[],errors=[],events=[[],[]];
async function navigate(page,url){
  await page.goto(url);await page.waitForFunction(()=>/lobby\.html/.test(location.pathname)?!document.documentElement.hasAttribute('data-lobby-boot'):window.__eaglerBoot?.done===true,{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
}
async function open(seat,url){
  const context=await browser.createBrowserContext(),page=await context.newPage();pages.push(page);await page.setViewport({width:1400,height:900});
  page.on('pageerror',e=>errors.push(e.message));
  await page.exposeFunction('recordRuntimeEvent',m=>events[seat].push(m));
  await page.evaluateOnNewDocument(()=>{addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')window.recordRuntimeEvent(e.data);});});
  await navigate(page,url);return page;
}
async function wait(check,label,ms=120000){const until=Date.now()+ms;let lastLog=0;while(Date.now()<until){
  for(const page of pages)await page.evaluate(()=>{document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
  if(await check())return;
  if(Date.now()-lastLog>10000){console.log(label,await Promise.all(pages.map(page=>page.evaluate(()=>({status:document.querySelector('#mpRoomResourceLabel')?.textContent,ready:document.querySelector('#mpReady')?.disabled})))));lastLog=Date.now();}
  await new Promise(r=>setTimeout(r,200));
}throw Error(label+' timeout');}
try{
  for(const round of ['check','match-1','match-2']){
  for(const list of events)list.length=0;
  const host=pages[0]??await open(0,base+'lobby.html?test=th03');
  if(round!=='match-2'){
  if(round!=='check')await navigate(host,base+'lobby.html?test=th03');
  await host.waitForFunction(()=>document.querySelector('#createButton')?.disabled===false);await host.click('#createButton');
  await host.waitForSelector('#roomDialog[open]');await host.select('#gameSelect','th03mp');await host.click('#submitRoom');
  await host.waitForSelector('#mpRoomCode',{timeout:45000});const code=await host.$eval('#mpRoomCode',e=>e.textContent);
  console.log('Created through main lobby',code);
  const joinUrl=base+'?'+new URLSearchParams({game:'th03mp',mpRoom:code,lobbyAction:'join',test:'th03'});
  const guest=pages[1]??await open(1,joinUrl);if(round!=='check')await navigate(guest,joinUrl);
  await guest.waitForSelector('#mpSeatStage',{timeout:45000});
  for(const [seat,page] of pages.entries()){
    const join=await page.$('[data-mp-seat-drop="'+seat+'"] button');
    if(join&&await join.evaluate(e=>!e.closest('[hidden]')&&!e.disabled))await join.click();
  }
  }
  await wait(async()=>{const e=events.flat().filter(e=>e.event==='error');if(e.length)throw Error(JSON.stringify(e));return(await Promise.all(pages.map(page=>page.evaluate(()=>document.querySelector('#mpReady')?.disabled===false)))).every(Boolean);},'main room preparation');
  assert(await host.$eval('#mpRoomChallengeToggle',e=>!!e.closest('[hidden]')),'unsupported challenge rules must not be offered');
  if(round==='check'){
    await host.click('[data-mp-seat="0"] .mp-seat-edit');await host.waitForSelector('#mpCheckGame',{visible:true});await host.click('#mpCheckGame');
    await wait(async()=>{const failure=events[0].find(e=>e.event==='error');if(failure)throw Error(JSON.stringify(failure));return events[0].some(e=>e.event==='first-frame');},'main room real native preflight');
    const frame=host.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
    assert(await frame.evaluate(()=>!globalThis.__th03PeerTransport),'game check must not open gameplay transport');
    await host.screenshot({path:resolve(out,'preflight.png')});console.log('PASS main room check presents native title without gameplay transport');
  }else{
  for(const page of pages)await page.click('#mpReady');
  await host.waitForFunction(()=>document.querySelector('#mpStartGame')?.disabled===false,{timeout:30000});
  await host.click('#mpStartGame');
  await wait(async()=>{const e=events.flat().filter(e=>e.event==='error');if(e.length)throw Error(JSON.stringify(e));return events.every(list=>list.some(e=>e.event==='first-frame'));},'main room native launch');
  const frames=pages.map(page=>page.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url())));
  await wait(async()=>{const states=await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const m=await import('./managed-room.mjs');return m.managedAPI.health();})));return states.every(s=>s.confirmed>=120&&s.frames>0);},'main room confirmed gameplay');
  await wait(async()=>(await Promise.all(pages.map(page=>page.evaluate(()=>{const e=document.querySelector('.room-workspace-preparation');return !e||e.hidden})))).every(Boolean),'main room loading cover must close',10000);
  for(const [seat,page] of pages.entries())await page.screenshot({path:resolve(out,'player-'+seat+'.png')});
  assert.deepEqual(errors,[]);console.log('PASS main Launcher create/join/seat/preload/ready/start and native multiplayer');
  if(round==='match-1'){
    await host.bringToFront();
    await host.click('.room-workspace-fullscreen');
    await frames[0].waitForFunction(()=>__eaglerKeyboardInputBridge.active(),{timeout:5000});
    await host.$eval('#gameFrame',frame=>frame.focus());await host.keyboard.down('ArrowRight');
    await wait(()=>frames[0].evaluate(()=>__eaglerKeyboardInputBridge.keyDown('ArrowRight')),'mapped keyboard down',5000);
    await host.keyboard.up('ArrowRight');
    await wait(()=>frames[0].evaluate(()=>!__eaglerKeyboardInputBridge.keyDown('ArrowRight')),'mapped keyboard release',5000);
    await Promise.all(pages.map(page=>page.$eval('#gameFrame',frame=>frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th03',epoch:Number(new URL(frame.src).searchParams.get('runtimeEpoch')),command:'network-cancel'},location.origin))));
    await wait(async()=>events.every(list=>list.some(e=>e.event==='exit'&&e.status==='success')),'runtime cancellation',15000);
    await wait(async()=>(await Promise.all(pages.map(page=>page.evaluate(()=>document.querySelector('#mpReady')?.disabled===false)))).every(Boolean),'same room prepares the next match',120000);
    console.log('PASS mapped keyboard, runtime cancellation and next-match preparation');continue;
  }
  }
  // Exercise the actual main room leave handler, including after a preflight.
  // Both contexts keep their Package Store for the next room and match.
  await Promise.all(pages.map(page=>page.$eval('#mpLeaveRoom',button=>button.click())));
  await wait(async()=>(await Promise.all(pages.map(page=>page.evaluate(()=>!document.querySelector('#mpRoomView')&&!document.querySelector('#gameFrame')?.src.includes('runtimeEpoch='))))).every(Boolean),'main leave retires Runtime and room',15000);
  console.log('PASS leave and release',round);
  }
}catch(error){for(const [seat,page] of pages.entries())await page.screenshot({path:resolve(out,'failure-'+seat+'.png')});throw error;}
finally{await writeFile(resolve(out,'result.json'),JSON.stringify({errors,events},null,2));await browser.close();}
