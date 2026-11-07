// Exercise TH02 native pause exits and the demo-to-title transition.
// PC98_EXIT_CHOICE=1|2 or PC98_TEST_DEMO=1 node launcher/tests/browser/test-th02-native-exit.mjs
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const repo=resolve(import.meta.dirname,'../../..');
const games=['th02'];
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
    if(path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(`\nexport function testTrack(){globalThis.opens=[];const f=emulator.module.FS,o=f.open;f.open=function(...a){const r=o.apply(this,a);if((new Error()).stack.includes('___syscall_openat'))opens.push(r.path);return r;};}export function testTrace(){return {files:globalThis.opens,host:emulator.externalHost.status,practice:emulator.practice.status,demo:emulator.module.HEAPU8[emulator.practice.debug.resident+39],read:readState(),logs:emulator.logs};}\n`)]);
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
    for(let i=0;i<35;i++){
      if((await state()).native?.practice?.ready)break;
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('KeyZ',{delay:100});await new Promise(r=>setTimeout(r,600));
    }
    await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.mode==='full'&&p.ready&&p.frames>20;},{timeout:90000});
    await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testTrack());
    const key=async(code,key,keyCode)=>{await page.evaluate(x=>rpc('keyboard',{...x,location:0,down:true}),{code,key,keyCode});await new Promise(r=>setTimeout(r,180));await page.evaluate(x=>rpc('keyboard',{...x,location:0,down:false}),{code,key,keyCode});await new Promise(r=>setTimeout(r,180));};
    await key('Escape','Escape',27);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
    await page.screenshot({path:resolve(output,'th02-pause-edge.png')});
    const choice=Number(process.env.PC98_EXIT_CHOICE||2);for(let i=0;i<choice;i++)await key('ArrowDown','ArrowDown',40);
    await frame.waitForFunction(async choice=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.choice===choice,{timeout:15000},choice);
    await key('KeyZ','z',90);
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).testTrace().files.some(x=>/op\.exe$/i.test(x)),{timeout:60000}).catch(async e=>{console.log('TRACE',JSON.stringify(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testTrace())));throw e;});
    if(process.env.PC98_TEST_DEMO==='1'){
      await frame.waitForFunction(async()=>{const s=(await import('./managed-solo.mjs')).testTrace();return s.demo===1&&s.files.some(x=>/main\.exe$/i.test(x));},{timeout:60000});
      await frame.waitForFunction(async()=>{const s=(await import('./managed-solo.mjs')).testTrace();return s.files.filter(x=>/op\.exe$/i.test(x)).length>=4;},{timeout:180000});
      await new Promise(r=>setTimeout(r,5000));await page.screenshot({path:resolve(output,'th02-demo-return.png')});
      await key('KeyZ','z',90);await new Promise(r=>setTimeout(r,5000));await page.screenshot({path:resolve(output,'th02-demo-title.png')});
    }else{
      await new Promise(r=>setTimeout(r,5000));assert.equal((await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testTrace())).read,null,'released MAIN must not receive input in OP');await page.screenshot({path:resolve(output,`th02-exit-${choice}.png`)});
      await key('KeyZ','z',90);await new Promise(r=>setTimeout(r,2500));await page.screenshot({path:resolve(output,`th02-exit-${choice}-selection.png`)});
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);assert.deepEqual(errors,[]);console.log('TH02 native exit completed',process.env.PC98_TEST_DEMO==='1'?'demo':choice);
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
