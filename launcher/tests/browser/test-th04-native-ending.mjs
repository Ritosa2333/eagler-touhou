// Exercise native TH04 ending and title return after entering the original ending branch.
// PC98_ENDING_SCENARIO=good|bad|extra node launcher/tests/browser/test-th04-native-ending.mjs
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const repo=resolve(import.meta.dirname,'../../..');
const games=['th04'];
const output=resolve(repo,'.cache/practice-load');await mkdir(output,{recursive:true});
for(const game of games){
  assert(['th02','th04'].includes(game));
  const root=process.env.PC98_PRACTICE_RUNTIME?resolve(process.env.PC98_PRACTICE_RUNTIME):resolve(repo,'ports',game,'build-eagler');
  const data=await readFile(resolve(repo,'ports',game,'content',game+'.data'));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe id="game" src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
  window.events=[];
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
  addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
  window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timed out'))},120000);
    function receive(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?ok(e.data):no(Error(e.data.error))}
    addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{
    const path=new URL(req.url,'http://localhost').pathname;
    if(path==='/'){res.setHeader('content-type','text/html');res.end(html);return;}
    if(path==='/data'){res.end(data);return;}
    const file=resolve(root,path.slice('/runtime/'.length));
    if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');
    res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let contents=await readFile(file);
    if(game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", `if(method==='testEnding'){const h=emulator.module.HEAPU8,v=new DataView(h.buffer),d=emulator.practice.debug.d,base=emulator.practice.debug.base,r=emulator.practice.debug.resident,load=d-patch.data_segment*16,scenario=args[0];globalThis.endFiles=[];const original=emulator.module.FS.open;emulator.module.FS.open=function(...a){const f=original.apply(this,a);endFiles.push({path:f.path,stack:(new Error()).stack?.split('\\n').slice(1,5)});return f;};emulator.practice.exitPractice();h[r+17]=scenario==='extra'?6:scenario==='bad'?4:5;h[r+19]=48+h[r+17];h[r+25]=0;const at=load+0xacf7;h[at]=0xea;v.setUint16(at+1,({good:0xb7b9,bad:0xb7e4,extra:0xb80f}[scenario]-patch.code_segment*16)&65535,true);v.setUint16(at+3,(load-base)/16+patch.code_segment,true);emulator.practiceClock.setSpeed(2);return;}if(method==='testEndingState')return {files:globalThis.endFiles?.filter(x=>!x.stack?.some(s=>s.includes('Object.export'))).map(x=>x.path),count:globalThis.endFiles?.length,fault:readSoloFault(emulator.module.HEAPU8,emulator.module),practice:emulator.practice.status,scan:scan04(performance.now()),host:emulator.externalHost.status,frames:emulator.module.localFrameCount,program:emulator.module._emGetRunningProgram?emulator.module.UTF8ToString(emulator.module._emGetRunningProgram()):null,logs:emulator.logs?.slice(-3)};if(method==='soloFault')`));
    if(path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th04'?`\nexport function testEnding(s){return emulator.call('testEnding',[s]);}export function testEndingState(){return emulator.call('testEndingState');}\n`:`\nexport function testEnding(s){const h=emulator.module.HEAPU8,v=new DataView(h.buffer),d=emulator.practice.debug.d,base=emulator.practice.debug.base,r=emulator.practice.debug.resident,cs=emulator.practice.debug.csBase;globalThis.endFiles=[];const original=emulator.module.FS.open;emulator.module.FS.open=function(...a){const f=original.apply(this,a);endFiles.push(f.path);return f;};if(s==='bad')v.setUint16(r+18,1,true);h[d+0x8f5]=4;h[d+0x19f6]=2;v.setUint16(d+0x1a18,0xbf90-0xb160,true);v.setUint16(d+0x1a1a,(cs-base)/16,true);h[cs+patch.entries.update]=0xea;h.set(h.slice(d+0x1a18,d+0x1a1c),cs+patch.entries.update+1);emulator.practiceClock.setSpeed(8);}export function testEndingState(){return {files:globalThis.endFiles,host:emulator.externalHost.status,frames:emulator.module.localFrameCount,logs:emulator.logs?.slice(-3)};}\n`)]);
    res.end(contents);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));
  let browser,page;
  const errors=[];
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    page=await browser.newPage();await page.setViewport({width:1100,height:800});
    page.on('pageerror',e=>{errors.push(e.message);console.error(game,e.message);});
    page.on('console',m=>{if(m.type()==='error')console.error(game,m.text());});
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
    await page.evaluate(language=>rpc('configure',{music:'none',language,options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}),process.env.PC98_LANGUAGE||'ja');
    await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const state=()=>frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState());
    await frame.waitForSelector('.managed-practice-open:not([hidden])',{timeout:120000});
    {
      await frame.click('.managed-practice-open');await frame.select('[data-jump=stage]',process.env.PC98_ENDING_SCENARIO==='extra'?'6':process.env.PC98_ENDING_SCENARIO==='bad'?'4':'5');await frame.select('[data-jump=entry]','stage');await frame.click('[data-practice-jump]');
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.ready&&p.waiting;},{timeout:180000});
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:150});
    }
    for(let i=0;i<35;i++){
      if((await state()).native?.practice?.ready)break;
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('KeyZ',{delay:100});await new Promise(r=>setTimeout(r,600));
    }
    await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.ready&&p.frames>10;},{timeout:90000});
    const scenario=process.env.PC98_ENDING_SCENARIO||'good';
    await frame.evaluate(async scenario=>(await import('./managed-solo.mjs')).testEnding(scenario),scenario);
    const initial=Date.now();let found=false;
    while(Date.now()-initial<180000){
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('KeyZ',{delay:120});await new Promise(r=>setTimeout(r,250));
      const s=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testEndingState());
      if(s.files?.some(x=>/maine\.exe$/i.test(x))&&s.files?.some(x=>/op\.exe$/i.test(x))){found=true;break;}
      if((Date.now()-initial)%15000<400)console.log(game,scenario,'ending running',s.frames);
    }
    const result=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testEndingState());
    await page.screenshot({path:resolve(output,`${game}-${scenario}-ending.png`)});
    console.log(game,scenario,'ending returned to title');assert(found,'ending must run MAINE and return to OP');assert.deepEqual(errors,[]);
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
