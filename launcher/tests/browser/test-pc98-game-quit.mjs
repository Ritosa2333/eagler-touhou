// Exercise the real DOS and host exit path after the game batch returns.
// Replace only the game command to reach that boundary without menu timing.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const repo=resolve(import.meta.dirname,'../../..');
for(const game of process.argv.slice(2).length?process.argv.slice(2):['th02','th03','th04']){
  const root=resolve(repo,'ports',game,'build-eagler'),data=await readFile(resolve(repo,'ports',game,'content',`${game}.data`));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'))},90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('content-type','text/html');return res.end(html);}if(path==='/data')return res.end(data);
    const file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let bytes=await readFile(file);if(path==='/runtime/dosbox-disk.js')bytes=Buffer.from(bytes.toString().replace("fastBoot?'FASTBOOT.BAT':'GAME.BAT'","fastBoot?'ver':'ver'").replace("fastBoot ? 'FASTBOOT.BAT' : 'GAME.BAT'","fastBoot ? 'ver' : 'ver'"));res.end(bytes);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));await page.setViewport({width:1000,height:700});await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
    const keys=await frame.evaluate(async()=>{
      const {DOSBoxPC98}=await import('./dosbox-adapter.js'),core=new DOSBoxPC98({}),queued=[];
      core.module={};core.externalHost={status:{mode:'record'},queue:value=>queued.push(value)};
      core.key('F12',true);core.key('F12',false);core.key('KeyZ',true);
      return queued.map(value=>value.code);
    });
    assert.deepEqual(keys,['KeyZ'],'F12 never reaches the emulator input queue');
    await page.evaluate(language=>rpc('configure',{music:'none',language,options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}),process.env.PC98_LANGUAGE||'ja');await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    await page.waitForFunction(()=>events.some(e=>e.event==='exit'||e.event==='error'),{timeout:60000});

    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);assert.equal(await page.evaluate(()=>events.filter(e=>e.event==='exit').length),1);assert.deepEqual(errors,[]);
    console.log('PASS',game,'batch completion returns to launcher');
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
