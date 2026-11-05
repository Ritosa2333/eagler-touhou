import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const repo=resolve(import.meta.dirname,'../../..');
for(const game of process.argv.slice(2).length?process.argv.slice(2):['th01','th03']){
  const root=resolve(repo,'ports',game,'build-eagler'),data=await readFile(resolve(repo,'ports',game,'content',`${game}.data`));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'))},90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('content-type','text/html');return res.end(html);}if(path==='/data')return res.end(data);
    const file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let bytes=await readFile(file);if(game==='th01'&&path==='/runtime/app.js')bytes=Buffer.concat([bytes,Buffer.from('\nwindow.testMenuInput=menuInput;\n')]);if(game==='th03'&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nlet testPrevious;export function testState(){return emulator?.snapshot?.state;} export function testMark(){testPrevious=emulator;} export function testChanged(){return !!emulator&&emulator!==testPrevious;}\n')]);res.end(bytes);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));await page.setViewport({width:1000,height:700});await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    await page.evaluate(()=>rpc('configure',{music:'none',options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}));await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),{timeout:120000});
    if(game==='th01'){
      await frame.waitForFunction(()=>th01Native.core.th_get(50,0)===1,{timeout:30000});
      await frame.evaluate(()=>{th01Native.core.th_menu_open(3,0);});
      // Navigate the real native pause and quit-confirmation menus.
      await frame.evaluate(()=>{let i=0;while(th01Native.core.th_get(51,0)!==1&&i++<8)testMenuInput(3);testMenuInput(4);});
      const menu=await frame.evaluate(()=>th01Native.core.th_get(50,0));
      if(menu===4)await frame.evaluate(()=>{let i=0;while(th01Native.core.th_get(51,0)!==1&&i++<8)testMenuInput(3);testMenuInput(4);});
      await frame.waitForFunction(()=>th01Native.core.th_get(50,0)===1,{timeout:10000}).catch(async error=>{console.log('TH01 menu after',await frame.evaluate(()=>[th01Native.core.th_get(50,0),th01Native.core.th_get(51,0)]));throw error;});
    }else{
      // Start a single-player game through its own title menu.
      for(let i=0;i<15;i++){await frame.click('#canvas');await page.keyboard.press('KeyZ',{delay:120});await new Promise(r=>setTimeout(r,500));if(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1))break;}
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1,{timeout:90000}).catch(async error=>{console.log('TH03 state',await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testState()));throw error;});
      await frame.click('#canvas');await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:true}));await new Promise(r=>setTimeout(r,150));await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:false}));await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:15000}).catch(async error=>{console.log('TH03 pause state',await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testState()));throw error;});
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testMark());
      await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(n=>n.textContent==='不存 REP 退出游戏').click());
      await frame.waitForFunction(async()=>{const m=await import('./managed-solo.mjs');return m.testChanged()&&m.managedAPI.health().frames>0&&!document.querySelector('.pc98-pause-menu:not([hidden])');},{timeout:90000});
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='exit'||e.event==='error')),[]);assert.deepEqual(errors,[]);
    console.log('PASS',game,'pause quit returns to its own title');
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
