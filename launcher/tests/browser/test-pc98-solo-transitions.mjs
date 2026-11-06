import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const repo=resolve(import.meta.dirname,'../../..');
for(const game of process.argv.slice(2).length?process.argv.slice(2):['th02','th04']){
  const root=resolve(repo,'ports',game,'build-eagler'),data=await readFile(resolve(repo,'ports',game,'content',`${game}.data`));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'))},90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('content-type','text/html');return res.end(html);}if(path==='/data')return res.end(data);
    const file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let bytes=await readFile(file);    if((game==='th04'||game==='th02')&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nlet old;export function mark(){old=emulator;}export function same(){return old===emulator;}export function snapshot(){return readState();}\n')]);
    if(game==='th04'&&path==='/runtime/solo-worker.js')bytes=Buffer.from(bytes.toString().replace("if(method==='soloFault')","if(method==='testTurbo'){const d=emulator.practice.debug.d;return d<0?null:emulator.module.HEAPU8[d+0x53a0];}if(method==='soloFault')"));res.end(bytes);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));await page.setViewport({width:1000,height:700});await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    await page.evaluate(language=>rpc('configure',{music:'none',language,options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}),process.env.PC98_LANGUAGE||'ja');await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),{timeout:120000});
    {
      for(let i=0;i<20;i++){await frame.click('#canvas');await page.keyboard.press('KeyZ',{delay:120});await new Promise(r=>setTimeout(r,500));if((await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice))?.ready)break;}
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.ready,{timeout:90000});

      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).mark());
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).snapshot()?.mode===1,{timeout:60000});
      const before=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).snapshot());
      await frame.click('#canvas');await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:true}));await new Promise(r=>setTimeout(r,150));await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:false}));await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:15000});
      await new Promise(r=>setTimeout(r,1000));await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(b=>b.textContent==='重新开始').click());
      await frame.waitForFunction(async old=>{const s=(await import('./managed-solo.mjs')).snapshot();return s?.mode===1&&s.generation!==old;},{timeout:60000},before.generation).catch(async e=>{console.log('RESTART DEBUG',before,await frame.evaluate(async()=>({snapshot:(await import('./managed-solo.mjs')).snapshot(),practice:(await import('./managed-solo.mjs')).managedAPI.practiceState(),events:parent.events.filter(e=>e.event==='error')})));throw e;});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).same()),true);console.log('PASS',game,'native restart');
      await frame.waitForFunction(async()=>{const s=(await import('./managed-solo.mjs')).snapshot();return s?.mode===1&&(s.stageFrame??s.ticks)>120;},{timeout:60000});await frame.click('#canvas');await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:true}));await new Promise(r=>setTimeout(r,150));await page.evaluate(()=>rpc('keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down:false}));await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:15000});
      await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(b=>b.textContent==='退出游戏').click());await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(b=>b.textContent==='保存本次 REP').click());
      await frame.waitForFunction(game=>[...__eaglerPC98Files.memory.keys()].some(k=>k.endsWith('.'+game+'r')),{timeout:60000},game);
      await frame.evaluate(async game=>{const api=(await import('./managed-solo.mjs')).managedAPI,path=[...__eaglerPC98Files.memory.keys()].find(k=>k.endsWith('.'+game+'r'));api.stop();await api.playback({action:'open',path});},game);
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult!==null,{timeout:120000});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult),true);console.log('PASS replay after restart');


    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='exit'||e.event==='error')),[]);assert.deepEqual(errors,[]);
    console.log('PASS',game,'restart and replay');
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
