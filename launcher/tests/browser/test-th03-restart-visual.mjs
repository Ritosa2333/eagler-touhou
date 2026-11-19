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
    let bytes=await readFile(file);
    if(path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nlet restartTestPrevious,restartTestFrames=[];export function mark(){restartTestPrevious=emulator;emulator.worker.addEventListener("message",e=>{if(e.data?.type==="restart-test-frame")restartTestFrames.push(e.data);});}export function testFrames(){return restartTestFrames;}export function same(){return restartTestPrevious===emulator;}export function testState(){return emulator?.snapshot?.state;}export function testSelect(row){const s=emulator.snapshot.state;return emulator.call("soloPauseSelect",[s.generation,s.at,row]);}\n')]);
    if(path==='/runtime/solo-worker.js')bytes=Buffer.from(bytes.toString().replace("if(args[1]){emulator.module.restartVisual?.release();", "if(args[1]){emulator.module.restartTestCollect=true;emulator.module.restartVisual?.release();"));
    if(path==='/runtime/frame-presenter.js')bytes=Buffer.from(bytes.toString().replace('    gl.bindFramebuffer(gl.FRAMEBUFFER,null);', `    if(mod.restartTestCollect){const p=mod.HEAPU8,w=frameWidth,h=frameHeight;let atlas=0,blue=0;for(let y=Math.floor(h*.68);y<Math.floor(h*.89);y+=4)for(let x=0;x<w;x+=4){let i=frame+(y*w+x)*3,r=p[i],g=p[i+1],b=p[i+2];if(Math.max(r,g,b)>150&&Math.max(r,g,b)-Math.min(r,g,b)>50)atlas++;}for(let y=8;y<48;y+=4)for(let x=0;x<w;x+=4){let i=frame+(y*w+x)*3,r=p[i],g=p[i+1],b=p[i+2];if(b>40&&b*10>r*14&&b*10>g*12)blue++;}postMessage({type:'restart-test-frame',atlas,blue});}gl.bindFramebuffer(gl.FRAMEBUFFER,null);`));
    res.end(bytes);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));await page.setViewport({width:1000,height:700});await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    await page.evaluate(({language,focus,point})=>rpc('configure',{music:'none',language,options:{touchEnabled:false,th03FocusEnabled:focus,alwaysHitbox:point},resources:[],runtimeResources:[],sharedResources:[]}),{language:process.env.TH03_TEST_LANGUAGE||'ja',focus:process.env.PC98_TEST_SOLO_FOCUS!=='0',point:process.env.PC98_TEST_SOLO_POINT==='1'});await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),{timeout:120000});
    await new Promise(r=>setTimeout(r,4000));for(let i=0;i<30;i++){await frame.click('#canvas');await page.keyboard.press('KeyZ',{delay:140});await new Promise(r=>setTimeout(r,600));if(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1))break;}
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===1,{timeout:30000});
    await frame.click('#canvas');await page.keyboard.press('Escape',{delay:120});await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===2,{timeout:10000});
    const before=await frame.evaluate(async()=>{const m=await import('./managed-solo.mjs');m.mark();return m.testState();});await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testSelect(3));
    await frame.waitForFunction(async()=>{const m=await import('./managed-solo.mjs'),s=m.testState();return m.testFrames().some(f=>f.blue>800)&&s?.phase===1&&s.ticks>50;},{timeout:30000});
    const frames=await frame.evaluate(async()=> (await import('./managed-solo.mjs')).testFrames());const intro=frames.findIndex(f=>f.blue>800);assert(intro>=0,'native stage intro should remain visible');
    const beforeIntro=frames.slice(0,intro),maxAtlas=Math.max(0,...beforeIntro.map(f=>f.atlas));assert(maxAtlas<450,'sprite atlas must not appear before the native stage intro: '+maxAtlas);assert(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).same()),'restart should retain the emulator');
    await frame.click('#canvas');await page.keyboard.press('Escape',{delay:120});await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===2,{timeout:10000});
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);console.log('PASS restart transition',JSON.stringify({maxAtlas,before}));

  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
