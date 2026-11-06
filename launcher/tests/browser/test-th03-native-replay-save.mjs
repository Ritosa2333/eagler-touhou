import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdtemp,readdir} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const repo=resolve(import.meta.dirname,'../../..'),downloads=await mkdtemp(resolve(tmpdir(),'th03-replay-test-'));const discard=process.env.TH03_TEST_DISCARD==='1';
for(const game of ['th03']){
  const root=resolve(repo,'ports',game,'build-eagler'),data=await readFile(resolve(repo,'ports',game,'content',`${game}.data`));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'))},90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('content-type','text/html');return res.end(html);}if(path==='/data')return res.end(data);
    const file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let bytes=await readFile(file);if(game==='th01'&&path==='/runtime/app.js')bytes=Buffer.concat([bytes,Buffer.from('\nwindow.testMenuInput=menuInput;\n')]);if(game==='th03'&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nexport function testDebug(){return emulator.call("testDebug");} export function testSelect(row){let s=emulator.snapshot.state;return emulator.call("soloPauseSelect",[s.generation,s.at,row]);} export function testPoint(){return emulator.call("testPoint");}let testPrevious;export function testState(){return emulator?.snapshot?.state;} export function testMarkers(){return emulator?.snapshot?.markers;} export function testMark(){testPrevious=emulator;} export function testChanged(){return !!emulator&&emulator!==testPrevious;} export function testPlaybackEnded(){return playback&&emulator?.state==="paused";}\n')]);
    if(path==='/runtime/dosbox-adapter.js')bytes=Buffer.from(bytes.toString().replace("emulator.config.onScoreSaved?.();","(globalThis.testSaved=(globalThis.testSaved||0)+1,emulator.config.onScoreSaved?.());"));if(path==='/runtime/solo-worker.js')bytes=Buffer.from(bytes.toString().replace('h[resident+0x17]=1;h[resident+0x34]=0;', 'h.fill(9,resident+0x18,resident+0x20);h[resident+0x17]=1;h[resident+0x34]=0;').replace("if(method==='performance')","if(method==='testDebug')return {programs:emulator.module.programReads,state:bridge.update(),saved:globalThis.testSaved||0};if(method==='testPoint')return emulator.module.HEAPU8[bridge.live+patch.fields.alwaysPoint];if(method==='performance')"));
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
    let dialogs=0;page.on('dialog',async d=>{dialogs++;await d.accept();});const client=await page.createCDPSession();await client.send('Page.setDownloadBehavior',{behavior:'allow',downloadPath:downloads});
    await frame.click('#canvas');await page.keyboard.press('Escape',{delay:100});
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testState()?.phase===2,{timeout:10000});

    await frame.evaluate(async row=>(await import('./managed-solo.mjs')).testSelect(row),discard?2:1);
    if(discard){
      await new Promise(r=>setTimeout(r,6000));assert.equal(dialogs,0);assert.equal((await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testDebug())).saved,0);
    }else{
      for(let i=0;i<14&&dialogs===0;i++){
        await new Promise(r=>setTimeout(r,1800));
        if(i===2){assert.equal(dialogs,0,'must wait for native name entry');assert.equal((await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testDebug())).saved,0);}
        if(i>=3){await frame.click('#canvas');await page.keyboard.press('KeyZ',{delay:120});}
      }
      assert.equal(dialogs,1,'ask once after name entry');
      let file;for(let i=0;i<100&&!file;i++){file=(await readdir(downloads)).find(name=>name.endsWith('.th03r'));if(!file)await new Promise(r=>setTimeout(r,100));}
      assert(file,'Replay should be downloaded');const bytes=await readFile(resolve(downloads,file)),{decodeExternalReplay}=await import('../../../ports/th03/web/replay/external-codec.js');await decodeExternalReplay(new Blob([bytes]),'03');
      await page.evaluate(()=>rpc('network-cancel'));
      await page.evaluate(bytes=>rpc('write',{path:'replay/downloaded.th03r',bytes}),Array.from(bytes));
      await page.evaluate(()=>rpc('configure',{music:'none',language:'ja',options:{replayViewer:true},resources:[],runtimeResources:[],sharedResources:[]}));
      await page.evaluate(()=>rpc('launch',{launchSerial:2}));
      const inspected=await page.evaluate(()=>rpc('playback',{action:'inspect',path:'replay/downloaded.th03r'}));assert.equal(inspected.description.stages.length,1);
      await page.evaluate(()=>rpc('playback',{action:'open',path:'replay/downloaded.th03r',stage:0}));
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testPlaybackEnded(),{timeout:120000});
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);assert.deepEqual(errors,[]);console.log('PASS',discard?'discard without replay':'native name entry and Replay download');

  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
