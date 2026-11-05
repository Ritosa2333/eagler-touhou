// Private browser smoke for the changed release Runtime paths. Visual curve
// parity belongs to test-hitbox-native-visuals; screenshots remain reviewable.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,readdir,stat} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {workspacePath,workspaceRoot} from '../lib/workspace-layout.mjs';
const output=process.env.EAGLER_HITBOX_TEST_OUTPUT;
assert.ok(output,'Set EAGLER_HITBOX_TEST_OUTPUT to a private validation directory');await mkdir(output,{recursive:true});
const games=(process.env.EAGLER_HITBOX_GAMES||'th09,th10,th15,th20').split(','),files=new Map();
const inventories={};
for(const game of games){
 const runtime=process.env['EAGLER_'+game.toUpperCase()+'_HITBOX_RUNTIME'];assert.ok(runtime,game+' runtime required');
 const inventory=JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8'));inventories[game]=inventory;
 for(const [name,expected] of Object.entries(inventory.files)){const file=resolve(runtime,name),bytes=await readFile(file);assert.equal(bytes.length,expected.bytes);assert.equal(createHash('sha256').update(bytes).digest('hex'),expected.sha256);files.set('/'+game+'/'+name,file);}
 const data=process.env['EAGLER_'+game.toUpperCase()+'_TEST_ARCHIVE'];assert.ok(data&&(await stat(data)).isFile(),game+' private archive required');files.set('/private/'+game+'.dat',data);
}
files.set('/private/msgothic.ttc',process.env.EAGLER_TEST_FONT||resolve(process.env.WINDIR||'C:/Windows','Fonts/msgothic.ttc'));
files.set('/private/unifont.otf',process.env.EAGLER_TEST_UNICODE_FONT||resolve(workspaceRoot(),'dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const parentHtml=game=>`<!doctype html><style>html,body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await fetch('/private/${game}.dat').then(r=>r.arrayBuffer())});
window.addEventListener('message',event=>{if(event.origin===location.origin&&event.source===document.querySelector('iframe')?.contentWindow)events.push(event.data)});
window.rpc=(command,fields={})=>new Promise((ok,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error('Timeout: '+command))},120000);function receive(event){if(event.origin!==location.origin||event.source!==document.querySelector('iframe').contentWindow||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?ok(event.data):no(Error(event.data.error))}addEventListener('message',receive);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
</script><iframe src="/${game}/${game}.html?hosted=1&managedData=1&gameGeneration=hitbox-validation&runtimeEpoch=1"></iframe>`;
const server=createServer(async(req,res)=>{const path=new URL(req.url,'http://localhost').pathname;try{
 if(games.some(game=>path==='/'+game)){res.setHeader('Content-Type','text/html');res.end(parentHtml(path.slice(1)));return;}
 const file=files.get(path);if(!file){res.writeHead(404).end('Missing fixture');return;}
 res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.html')?'text/html':path.endsWith('.css')?'text/css':path.endsWith('.wasm')?'application/wasm':path.endsWith('.json')?'application/json':'application/octet-stream');res.end(await readFile(file));
}catch(error){res.writeHead(500).end(String(error));}});
await new Promise(ok=>server.listen(0,'127.0.0.1',ok));
const browserRoot=resolve(process.env.LOCALAPPDATA,'ms-playwright'),installed=(await readdir(browserRoot)).filter(name=>/^chromium-\d+$/.test(name)).sort((a,b)=>Number(b.split('-')[1])-Number(a.split('-')[1]));
const executablePath=process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE||resolve(browserRoot,installed[0],'chrome-win64/chrome.exe');
let browser;const report=[];
const delay=ms=>new Promise(ok=>setTimeout(ok,ms));
try{
 browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--enable-unsafe-swiftshader','--use-gl=angle','--use-angle=swiftshader','--autoplay-policy=no-user-gesture-required']});
 for(const game of games){const page=await browser.newPage();await page.setViewport({width:640,height:480});const errors=[];page.on('pageerror',error=>{errors.push(error.message);console.error(game+': '+error.message);});page.on('requestfailed',request=>console.error(game+': request failed '+request.url()+' '+request.failure()?.errorText));
  try{
   console.log(game+': loading release Runtime');await page.goto('http://127.0.0.1:'+server.address().port+'/'+game);await page.waitForFunction(()=>events.some(event=>event.event==='ready'),{timeout:120000});
   await page.evaluate(()=>rpc('configure',{language:'ja',music:'none',options:{alwaysHitbox:true,touchEnabled:false,thpracEnabled:false},runtimeResources:[],resources:[],sharedResources:[{path:'/msgothic.ttc',url:'/private/msgothic.ttc'},{path:'/unifont.otf',url:'/private/unifont.otf'}]}));
   console.log(game+': launch configured Runtime');await page.evaluate(()=>rpc('launch'));await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),{timeout:120000});
   const frame=page.frames().find(frame=>new URL(frame.url()).pathname.endsWith('/'+game+'.html'));assert.ok(frame);
   await page.bringToFront();await frame.$eval('canvas',canvas=>canvas.focus());
   const inspect=()=>frame.evaluate(game=>{const runtime=window['__'+game+'Runtime'];if(runtime?.status)return runtime.status();const c=window.Module;return{phase:c['_'+game+'_phase']()};},game);
   await delay(2500);const title=await inspect();await page.screenshot({path:resolve(output,game+'-title.png')});
   const key=async(code,time=110)=>{await page.evaluate(code=>rpc('keyboard',{code,down:true,inputSource:'physical'}),code);await delay(time);await page.evaluate(code=>rpc('keyboard',{code,down:false,inputSource:'physical'}),code);await delay(450);};
   // Follow the actual title menu, then dismiss any in-stage dialogue.
   for(let i=0;i<(game==='th20'?30:10);i++){await key('KeyZ');if(game==='th20'){const current=await inspect();console.log(game+': menu '+JSON.stringify(current));if(current[0]!==4)break;}}
   if(game==='th09'){await page.evaluate(()=>rpc('keyboard',{code:'ControlLeft',down:true,inputSource:'physical'}));await delay(1400);await page.evaluate(()=>rpc('keyboard',{code:'ControlLeft',down:false,inputSource:'physical'}));}
   await delay(1000);
   const stage=await inspect();await page.screenshot({path:resolve(output,game+'-unfocused.png')});
   await page.evaluate(()=>rpc('keyboard',{code:'ShiftLeft',down:true,inputSource:'physical'}));await delay(650);await page.screenshot({path:resolve(output,game+'-native-focused.png')});await page.evaluate(()=>rpc('keyboard',{code:'ShiftLeft',down:false,inputSource:'physical'}));await delay(1000);await page.screenshot({path:resolve(output,game+'-focus-released.png')});
   if(game==='th15'){await page.setViewport({width:800,height:600});await page.$eval('iframe',frame=>{frame.style.width='800px';frame.style.height='600px';});await delay(600);await page.screenshot({path:resolve(output,game+'-resized.png')});}
   const events=await page.evaluate(()=>window.events);assert.equal(events.some(event=>event.event==='error'||event.event==='abort'),false,JSON.stringify(events.filter(event=>event.event==='error'||event.event==='abort')));assert.deepEqual(errors,[]);
   if(game==='th09'){assert.equal(stage.title[1],0);assert.equal(stage.session[0],1);}
   else if(game==='th10')assert.equal(stage[0],7);
   else if(game==='th15')assert.equal(stage.phase,1);
   else {assert.notEqual(stage[0],title[0]);assert.equal(stage[3],-1);}
   report.push({game,passed:true,title,stage,runtime:inventories[game]});console.log(game+': release Runtime configured before launch and keyboard scene entry passed');
  }catch(error){await page.screenshot({path:resolve(output,game+'-failure.png')}).catch(()=>{});report.push({game,passed:false,error:error.stack,events:await page.evaluate(()=>window.events).catch(()=>[])});throw error;}
  finally{await page.close();}
 }
}finally{await writeFile(resolve(output,'report.json'),JSON.stringify({scope:'Private Chromium release Runtime smoke and review screenshots; native visual parity has a separate component gate',report},null,2));await browser?.close();await new Promise(ok=>server.close(ok));}
