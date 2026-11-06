import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const repo=resolve(import.meta.dirname,'../../..');
for(const game of ['th03']){
  const root=resolve(repo,'ports',game,'build-eagler'),data=await readFile(resolve(repo,'ports',game,'content',`${game}.data`));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'))},90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('content-type','text/html');return res.end(html);}if(path==='/data')return res.end(data);
    const file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let bytes=await readFile(file);if(game==='th01'&&path==='/runtime/app.js')bytes=Buffer.concat([bytes,Buffer.from('\nwindow.testMenuInput=menuInput;\n')]);if(game==='th03'&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nexport function testPoint(){return emulator.call("testPoint");}let testPrevious;export function testState(){return emulator?.snapshot?.state;} export function testMarkers(){return emulator?.snapshot?.markers;} export function testMark(){testPrevious=emulator;} export function testChanged(){return !!emulator&&emulator!==testPrevious;} export function testPlaybackEnded(){return playback&&emulator?.state==="paused";}\n')]);
    if(path==='/runtime/solo-worker.js')bytes=Buffer.from(bytes.toString().replace("if(method==='performance')","if(method==='testPoint')return emulator.module.HEAPU8[bridge.live+patch.fields.alwaysPoint];if(method==='performance')"));
    if(game==='th04'&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nexport function testTurbo(){return emulator.call("testTurbo");}\n')]);
    if(game==='th04'&&path==='/runtime/solo-worker.js')bytes=Buffer.from(bytes.toString().replace("if(method==='soloFault')","if(method==='testTurbo'){const d=emulator.practice.debug.d;return d<0?null:emulator.module.HEAPU8[d+0x53a0];}if(method==='soloFault')"));res.end(bytes);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));await page.setViewport({width:1000,height:700});await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    await page.evaluate(({language,focus,point})=>rpc('configure',{music:'none',language,options:{touchEnabled:false,th03FocusEnabled:focus,alwaysHitbox:point},resources:[],runtimeResources:[],sharedResources:[]}),{language:'ja',focus:process.env.PC98_TEST_SOLO_FOCUS!=='0',point:process.env.PC98_TEST_SOLO_POINT==='1'});await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),{timeout:120000});
    await new Promise(r=>setTimeout(r,4000));for(let i=0;i<30;i++){await frame.click('#canvas');await page.keyboard.press('KeyZ',{delay:140});await new Promise(r=>setTimeout(r,600));if(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1))break;}
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1,{timeout:30000});
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testMarkers()?.some(p=>p.seat===0),{timeout:15000});
    await frame.click('#canvas');await page.keyboard.down('Space');await new Promise(r=>setTimeout(r,900));
    const marker=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testMarkers().find(p=>p.seat===0));assert.equal(marker.focused,process.env.PC98_TEST_SOLO_FOCUS!=='0');
    await page.keyboard.up('Space');await new Promise(r=>setTimeout(r,500));assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPoint()),process.env.PC98_TEST_SOLO_POINT==='1'?1:0,'native always-point flag');await (await frame.$('#canvas')).screenshot({path:resolve(repo,'.cache','th03-solo-focus-'+(process.env.PC98_TEST_SOLO_FOCUS||'1')+'-'+(process.env.PC98_TEST_SOLO_POINT||'0')+'.png')});
    console.log('PASS solo focus',marker.focused,'always point',process.env.PC98_TEST_SOLO_POINT||'0');
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='exit'||e.event==='error')),[]);assert.deepEqual(errors,[]);

  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
