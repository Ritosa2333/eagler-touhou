import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {createHash} from 'node:crypto';
export const wait=ms=>new Promise(resolve=>setTimeout(resolve,ms));
export async function productionBrowser({url,report,unlimited=false,touch=false,headed=false,protocolTimeout=180000,launchArgs=[],nativeViewport=false,profile=process.env.THKKD_TEST_PROFILE}){
  const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:!headed,protocolTimeout,userDataDir:profile,args:[...(unlimited?['--disable-frame-rate-limit','--disable-gpu-vsync']:[]),...launchArgs],defaultViewport:nativeViewport?null:{width:1280,height:900,hasTouch:touch}});
  report.browser=await browser.version();report.errors??=[];report.loads=[];report.consoleErrors=[];report.httpErrors=[];report.navigations=[];report.telemetry=[];
  const page=await browser.newPage(),pending=[];
  await page.exposeFunction('__qualityObserve',event=>report.telemetry.push(event));
  page.on('pageerror',error=>report.errors.push(error.message));
  page.on('console',message=>{if(message.type()==='error')report.consoleErrors.push(message.text());});page.on('framenavigated',frame=>{if(frame===page.mainFrame())report.navigations.push(frame.url());});
  page.on('response',response=>{
    if(response.status()>=400)report.httpErrors.push({url:response.url(),status:response.status()});
    if(!/\/runtime\/thkkd\/|thkkd\.data(?:$|\?)/.test(response.url()))return;
    const row={url:response.url(),status:response.status(),cache:response.fromCache(),serviceWorker:response.fromServiceWorker()};report.loads.push(row);
    if(response.url().endsWith('.wasm'))pending.push(response.buffer().then(bytes=>{row.bytes=bytes.length;row.sha256=createHash('sha256').update(bytes).digest('hex');}));
  });
  await page.evaluateOnNewDocument(()=>{
    window.__qualityEvents=[];window.__qualityProtocol=[];window.__qualityRuntimeEvents=[];
    for(const type of ['keydown','keyup','blur','focus','click'])addEventListener(type,event=>{__qualityEvents.push({type,code:event.code,active:document.activeElement?.tagName,target:event.target?.tagName,product:event.target?.closest?.('[data-book-product]')?.getAttribute('data-book-product'),time:performance.now()});if(__qualityEvents.length>100)__qualityEvents.shift();},true);
    addEventListener('message',event=>{const m=event.data;if(m?.protocol!=='eagler-touhou/1')return;if(['configure','launch'].includes(m.command))__qualityProtocol.push(m);if(['error','audio-health','frame-health','exit'].includes(m.event)){__qualityRuntimeEvents.push(m);if(__qualityRuntimeEvents.length>200)__qualityRuntimeEvents.shift();if(parent===window&&typeof __qualityObserve==='function')void __qualityObserve(m);}});
  });
  async function library({select=true}={}){
    await page.goto(url,{waitUntil:'networkidle0'});
    if(await page.$('#firstUseNoticeDialog[open]')){
      await page.$eval('#firstUseNoticeDialog',dialog=>[...dialog.querySelectorAll('button')].find(button=>/^(Got it|我知道了|知道了|我已了解)$/.test(button.textContent.trim()))?.click());
      await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog')?.open);await wait(350);
    }
    if(!select)return;
    if(!new URL(page.url()).searchParams.has('game')){
      await page.waitForFunction(()=>{const element=document.querySelector('a.book-game-link[data-book-product="thkkd"]');return element&&!element.closest('[inert]')&&element.getBoundingClientRect().width>0&&typeof element[Object.keys(element).find(key=>key.startsWith('__reactProps$'))]?.onClick==='function';});
      await page.locator('a.book-game-link[data-book-product="thkkd"]').click();
    }
    await page.waitForFunction(()=>new URL(location.href).searchParams.get('game')==='thkkd');
  }
  async function start(){
    const buttons=await page.$$('button');let launched=false;
    for(const button of buttons)if(await button.evaluate(node=>['Start game','开始游戏','启动游戏'].includes(node.textContent.trim()))){await button.click();launched=true;break;}
    assert.ok(launched,'正式启动按钮存在');
    await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__thkkdRuntime?.status().first,{timeout:60000});
    const frame=page.frames().find(item=>item.url().includes('thkkd.html'));assert.ok(frame);
    await Promise.all(pending);report.runtime=await frame.evaluate(async()=>({url:location.href,manifest:await(await fetch('./manifest.json')).json(),configure:__qualityProtocol.find(m=>m.command==='configure'),serviceWorker:navigator.serviceWorker.controller?.scriptURL||null}));
    return frame;
  }
  async function key(code,hold=80,settle=450){await page.keyboard.down(code);await wait(hold);await page.keyboard.up(code);await wait(settle);}
  async function collect(){try{report.finalDocumentEvents=await page.evaluate(()=>__qualityRuntimeEvents);report.events=await page.evaluate(()=>__qualityEvents);}catch(error){report.observationError=String(error);}return report.telemetry;}
  return {browser,page,library,start,key,collect};
}
export const nativeState=frame=>frame.evaluate(()=>{
  const r=__thkkdRuntime,w=address=>r.core._thkkd_word(address);
  return {outer:w(0xa8ce58),phase:w(0xa87f48),choice:w(0xa87f20),gameScene:w(0xa78e2c),gameFrame:w(0xb3e2e8),cursor:w(0xb43110),dialogue:w(0xa73060),dialogueTimer:w(0xa7305c),zPhysical:w(0xa71e08+44*4),zLogical:w(0xa72208+w(0xa733ec)*4),nativePresents:r.platform.presented,held:[...r.input.owners].map(([code,owners])=>[code,[...owners]]),error:document.querySelector('#error').textContent,focused:document.hasFocus(),active:document.activeElement?.tagName};
});
export async function runtimeCommand(page,command,fields={}){
  return page.evaluate(({command,fields})=>new Promise((resolve,reject)=>{
    const target=document.querySelector('#gameFrame')?.contentWindow;if(!target)return reject(Error('Runtime iframe 不存在'));
    const epoch=Number(new URL(target.location.href).searchParams.get('runtimeEpoch')),request='quality-'+crypto.randomUUID();
    const timeout=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Runtime 响应超时：'+command));},30000);
    function receive(event){const m=event.data;if(event.source!==target||event.origin!==location.origin||m?.epoch!==epoch||m.request!==request)return;clearTimeout(timeout);removeEventListener('message',receive);m.ok?resolve(m):reject(Error(m.error));}
    addEventListener('message',receive);target.postMessage({protocol:'eagler-touhou/1',game:'thkkd',epoch,request,command,...fields},location.origin);
  }),{command,fields});
}
export async function observeCadence(frame,duration=1600,trigger){
  await frame.evaluate(()=>{
    const r=__thkkdRuntime;
    if(!window.__qualityMeasure){
      window.__qualityMeasure={rafTimes:new Set(),presents:0,steps:0};
      const present=r.platform.renderer.present.bind(r.platform.renderer);r.platform.renderer.present=(...args)=>{__qualityMeasure.presents++;return present(...args);};
      const step=r.core._thkkd_step;r.core._thkkd_step=(...args)=>{__qualityMeasure.steps++;return step(...args);};
      const raf=requestAnimationFrame;window.requestAnimationFrame=callback=>raf.call(window,time=>{__qualityMeasure.rafTimes.add(time);callback(time);});
    }
    Object.assign(__qualityMeasure,{start:performance.now(),presents:0,steps:0});__qualityMeasure.rafTimes.clear();
  });
  const before=await nativeState(frame);if(trigger)await trigger();await wait(duration);const after=await nativeState(frame);
  return frame.evaluate(({before,after})=>{
    const m=__qualityMeasure,seconds=(performance.now()-m.start)/1000;
    return {before,after,seconds,callbacks:m.rafTimes.size,presents:m.presents,steps:m.steps,callbackHz:m.rafTimes.size/seconds,presentHz:m.presents/seconds,logicHz:m.steps/seconds,renderedFPS:document.querySelector('#fps')?.textContent||[...__thkkdRuntime.platform.renderer.commands].map(command=>command.texture?.text).filter(text=>typeof text==='string'&&text.endsWith('FPS')).at(-1)};
  },{before,after});
}
