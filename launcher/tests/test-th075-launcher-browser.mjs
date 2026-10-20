// Real Launcher + rebuilt ordinary TH075 Runtime. Private local DATA is required.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/';
const output=resolve('artifacts/th075-launcher');await mkdir(output,{recursive:true});
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
  console.log('select music',selected);await page.select('#musicSelect',selected);await page.waitForFunction(()=>{const e=document.querySelector('#launch');return e&&!e.disabled&&e.getBoundingClientRect().height>0;});await page.click('#launch');console.log('launch requested');
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
  for(const mobile of process.env.EAGLER_TH075_TEST_MODE==='touch'?[true]:[false,true]){
    const context=await browser.createBrowserContext(),page=await context.newPage();await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
    await page.setViewport(mobile?{width:1000,height:650,isMobile:true,hasTouch:true}:{width:1280,height:900});
    if(mobile)await page.setUserAgent('Mozilla/5.0 (Linux; Android 13) AppleWebKit/537.36 Chrome/149.0.0.0 Mobile Safari/537.36');
    page.on('pageerror',e=>console.log('browser error:',e.message));
    const runtime=await open(page,!mobile,mobile);console.log('TH075 presented',mobile?'touch':'desktop');
    assert.equal(await runtime.evaluate(()=>typeof Module._th075_probe_state),'undefined');
    await rpc(page,'sync');const files=(await rpc(page,'list')).files;
    assert.ok(files.some(f=>f.path==='score.dat'));assert.ok(files.some(f=>f.path==='config.dat'));
    await assert.rejects(rpc(page,'read',{path:'../th075.data'}),/path/);
    await assert.rejects(rpc(page,'configure',{music:'none'}),/running/);
    const before=await runtime.evaluate(()=>Module._th075_frame());
    await page.evaluate(()=>{const frame=document.querySelector('#gameFrame');frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th075',epoch:999999,command:'configure',request:'stale',music:'none'},location.origin);});
    await runtime.waitForFunction(frame=>Module._th075_frame()>frame+10,{},before);
    assert.equal(await page.evaluate(()=>runtimeEvents.some(e=>e.request==='stale')),false);
    await rpc(page,'keyboard',{code:'KeyZ',down:true});await rpc(page,'touch-controls',{controls:{actionButtons:15,joystickX:20000,joystickY:0,escapeSerial:0}});await rpc(page,'touch-cancel');await rpc(page,'keyboard',{code:'KeyZ',down:false});
    await page.screenshot({path:resolve(output,mobile?'touch.png':'desktop.png')});
    const events=await page.evaluate(()=>runtimeEvents);
    for(const event of ['ready','first-frame','runtime-info','frame-health','audio-health'])assert.ok(events.some(e=>e.event===event),event);
    const audio=events.filter(e=>e.event==='audio-health').at(-1);assert.equal(audio.ready,true);assert.equal(audio.musicEnabled,!mobile);
    assert.ok(!events.some(e=>e.event==='error'),JSON.stringify(events.filter(e=>e.event==='error')));
    if(mobile){
      const geometry=await page.evaluate(()=>{const h=document.getElementById('touchHud').getBoundingClientRect();return Object.fromEntries(['touchFire','touchFocus','touchBomb','touchFunction'].map(id=>{const r=document.getElementById(id).getBoundingClientRect();return [id,{x:Math.round(r.x-h.x),y:Math.round(r.y-h.y),width:Math.round(r.width),height:Math.round(r.height)}];}));});
      assert.deepEqual(geometry,{touchFire:{x:0,y:70,width:60,height:60},touchFocus:{x:65,y:5,width:60,height:60},touchBomb:{x:130,y:70,width:60,height:60},touchFunction:{x:65,y:110,width:60,height:60}},'TH075 development-package button geometry');
      assert.equal(await page.$eval('#touchJoystick',e=>e.hidden),false);
      await page.click('#touchHelpOpen');await page.click('#guideTabGameControls');await page.waitForFunction(()=>!document.querySelector('#guideGameControls')?.hidden);
      const guide=await page.$eval('#guideGameControls',e=>e.textContent);for(const text of ['A 轻攻击 / 确认','B 重攻击 / 取消','弹幕攻击 / 冲刺','左侧轮盘'])assert.ok(guide.includes(text),text);
      assert.equal(await page.$('.focus-stage'),null);await page.screenshot({path:resolve(output,'touch-help.png')});await page.click('#touchHelpClose');
      await page.screenshot({path:resolve(output,'touch.png')});
      for(const [id,label] of [['touchFire','轻攻击'],['touchFocus','重攻击'],['touchFunction','冲刺']])assert.equal(await page.$eval('#'+id+' small',e=>getComputedStyle(e).display==='none'?'':e.textContent),label);
      const labels=await page.$$eval('#touchFire,#touchBomb,#touchFocus,#touchFunction',nodes=>nodes.map(n=>({hidden:n.hidden,label:n.querySelector('strong')?.textContent})));
      assert.ok(labels.every(n=>!n.hidden),JSON.stringify(labels));
      assert.ok(labels.some(n=>n.label==='D'),JSON.stringify(labels));
      await runtime.evaluate(()=>{const original=Module._th075_touch_controls;globalThis.actionSamples=[];Module._th075_touch_controls=(...args)=>{actionSamples.push(args);return original(...args);};});
      await page.evaluate(()=>{for(const [i,id] of ['touchFire','touchBomb','touchFocus','touchFunction'].entries())document.getElementById(id).dispatchEvent(new PointerEvent('pointerdown',{bubbles:true,cancelable:true,pointerId:i+10,pointerType:'touch'}));});
      await runtime.waitForFunction(()=>globalThis.actionSamples?.some(v=>v[1]&&v[2]&&v[3]&&v[4]));
      await page.evaluate(()=>{for(const [i,id] of ['touchFire','touchBomb','touchFocus','touchFunction'].entries())document.getElementById(id).dispatchEvent(new PointerEvent('pointercancel',{bubbles:true,cancelable:true,pointerId:i+10,pointerType:'touch'}));});
    }
    const config=(await rpc(page,'read',{path:'config.dat'})).bytes;
    await page.reload();await page.waitForFunction(()=>window.__eaglerBoot?.done===true);
    await page.$eval('.game[data-game=th075]',e=>{e.click();e.click();});
    await page.waitForFunction(()=>document.querySelector('[data-action="manage-replay"]'));
    await page.$eval('[data-action="manage-replay"]',e=>e.click());
    await page.waitForFunction(()=>document.querySelector('#replayDialog[open]')&&document.querySelector('#replayList'));
    await page.waitForFunction(()=>{const f=document.querySelector('#gameFrame');return !!f?.contentWindow?.Module?.FS;},{timeout:45000});
    assert.deepEqual((await rpc(page,'read',{path:'config.dat'})).bytes,config,'IDBFS survives a new Runtime document');
    if(process.env.EAGLER_TH075_TEST_REPLAY){
      const bytes=Array.from(await readFile(process.env.EAGLER_TH075_TEST_REPLAY));
      await rpc(page,'write',{path:'replay/imported.rep',bytes});
      assert.deepEqual((await rpc(page,'read',{path:'replay/imported.rep'})).bytes,bytes);
      assert.ok((await rpc(page,'list')).files.some(f=>f.path==='replay/imported.rep'));
      await rpc(page,'remove',{path:'replay/imported.rep'});await rpc(page,'sync');
      assert.ok(!(await rpc(page,'list')).files.some(f=>f.path==='replay/imported.rep'));
    }
    report.push({mode:mobile?'touch':'desktop',firstFrame:true,storageRestart:true,replayRoundTrip:!!process.env.EAGLER_TH075_TEST_REPLAY,epoch:true,audio:audio.musicEnabled?'ogg':'none'});
    await context.close();
  }
  await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
}finally{await browser.close();}
