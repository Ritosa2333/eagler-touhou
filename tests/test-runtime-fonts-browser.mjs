// Private local integration: real packaged shells, WASM and user-supplied DATA.
// No generated glyph tables are served, installed or accepted as test inputs.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir,readdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';

const workspace=process.env.EAGLER_WORKSPACE;
if(!workspace)throw Error('Set EAGLER_WORKSPACE to the private validation workspace');
const output=process.env.EAGLER_FONT_TEST_OUTPUT||resolve(import.meta.dirname,'../artifacts/validation/runtime-fonts');await mkdir(output,{recursive:true});
const games=['th11','th15'],files=new Map(),inventories={};
for(const game of games){
 const root=resolve(workspace,game+'-eagler'),runtime=process.env['EAGLER_'+game.toUpperCase()+'_FONT_RUNTIME']||resolve(root,'artifacts/validation/font-raster/runtime');
 const inventory=JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8'));inventories[game]=inventory;
 assert.deepEqual(Object.keys(inventory.files).filter(name=>name.startsWith('fonts/')),['fonts/cp932.bin']);
 for(const name of Object.keys(inventory.files))files.set('/'+game+'/'+name,resolve(runtime,name));
 const retail=game==='th11'?'[th11] 东方地灵殿 (汉化版+日文版)':'[th15] 东方绀珠传 (汉化版+日文版)';
 files.set('/private/'+game+'.dat',process.env['EAGLER_'+game.toUpperCase()+'_TEST_ARCHIVE']||resolve(root,retail,game+'.dat'));
}
files.set('/private/msgothic.ttc',process.env.EAGLER_TEST_FONT||resolve(process.env.WINDIR||'C:/Windows','Fonts/msgothic.ttc'));
files.set('/private/unifont.otf',process.env.EAGLER_TEST_UNICODE_FONT||resolve(workspace,'dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const parentHtml=game=>`<!doctype html><style>html,body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await fetch('/private/${game}.dat').then(r=>r.arrayBuffer())});
window.addEventListener('message',event=>{if(event.origin===location.origin&&event.source===document.querySelector('iframe')?.contentWindow)events.push(event.data)});
window.rpc=(command,fields={})=>new Promise((ok,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error('Timeout: '+command))},90000);function receive(event){if(event.origin!==location.origin||event.source!==document.querySelector('iframe').contentWindow||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?ok(event.data):no(Error(event.data.error))}addEventListener('message',receive);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
</script><iframe src="/${game}/${game}.html?hosted=1&managedData=1&gameGeneration=font-validation&runtimeEpoch=1"></iframe>`;
const requests=[];
const server=createServer(async(req,res)=>{
 const path=new URL(req.url,'http://localhost').pathname;requests.push(path);
 try{
  if(games.some(game=>path==='/'+game)){res.setHeader('Content-Type','text/html');res.end(parentHtml(path.slice(1)));return;}
  const file=files.get(path);if(!file){res.writeHead(404).end('Missing fixture');return;}
  res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.html')?'text/html':path.endsWith('.css')?'text/css':path.endsWith('.wasm')?'application/wasm':path.endsWith('.json')?'application/json':'application/octet-stream');
  res.end(await readFile(file));
 }catch(error){res.writeHead(500).end(String(error));}
});
await new Promise(ok=>server.listen(0,'127.0.0.1',ok));
const browserRoot=resolve(process.env.LOCALAPPDATA,'ms-playwright');
const installed=(await readdir(browserRoot)).filter(name=>/^chromium-\d+$/.test(name)).sort((a,b)=>Number(b.split('-')[1])-Number(a.split('-')[1]));
const executablePath=process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE||resolve(browserRoot,installed[0],'chrome-win64/chrome.exe');
let browser;const report=[];
try{
 browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--enable-unsafe-swiftshader','--use-gl=angle','--use-angle=swiftshader','--autoplay-policy=no-user-gesture-required']});
 for(const game of games){
  const page=await browser.newPage();await page.setViewport({width:640,height:480});
  const errors=[];page.on('pageerror',error=>errors.push(error.message));
  await page.goto('http://127.0.0.1:'+server.address().port+'/'+game);
  await page.waitForFunction(()=>events.some(event=>event.event==='ready'),{timeout:90000});
  await page.evaluate(()=>rpc('configure',{language:'ja',music:'none',options:{touchEnabled:false,thpracEnabled:false},runtimeResources:[],resources:[],sharedResources:[{path:'/msgothic.ttc',url:'/private/msgothic.ttc'},{path:'/unifont.otf',url:'/private/unifont.otf'}]}));
  await page.evaluate(()=>rpc('launch'));
  await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),{timeout:90000});
  const frame=page.frames().find(frame=>new URL(frame.url()).pathname.endsWith('/'+game+'.html'));
  assert(frame);
  const inspect=()=>frame.evaluate(game=>{const c=window.Module;const string=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));return{phase:c['_'+game+'_phase'](),error:string(c['_'+game+'_error']()),fontInstalled:c.FS.analyzePath('/msgothic.ttc').exists,obsoleteFontInstalled:c.FS.analyzePath('/fonts/font0.bin').exists,hidden:document.hidden,focused:document.hasFocus(),frame:c['_'+game+'_frame']?.()};},game);
  const title=await inspect();assert.equal(title.error,'');assert.equal(title.fontInstalled,true);assert.equal(title.obsoleteFontInstalled,false);
  await page.bringToFront();await frame.$eval('canvas',canvas=>canvas.focus());
  const key=async code=>{await page.evaluate(code=>rpc('keyboard',{code,down:true,inputSource:'physical'}),code);await new Promise(ok=>setTimeout(ok,120));await page.evaluate(code=>rpc('keyboard',{code,down:false,inputSource:'physical'}),code);await new Promise(ok=>setTimeout(ok,800));};
  // Let the authored title animation expose its menu before choosing Music Room.
  await new Promise(ok=>setTimeout(ok,2300));
  await page.screenshot({path:resolve(output,game+'-title.png')});
  for(let i=0;i<4;i++)await key('ArrowDown');await key('KeyZ');
  const musicRoom=await inspect();assert.equal(musicRoom.error,'');
  await page.screenshot({path:resolve(output,game+'-music-room.png')});
  await key('Escape');for(let i=0;i<4;i++)await key('ArrowUp');
  for(let i=0;i<12&&(await inspect()).phase===0;i++)await key('KeyZ');
  const entry=await inspect();assert.equal(entry.error,'');assert.notEqual(entry.phase,0,JSON.stringify({message:'Title controls must enter a running game',entry,events:await page.evaluate(()=>window.events.slice(-6))}));
  await new Promise(ok=>setTimeout(ok,1300));await page.screenshot({path:resolve(output,game+'-entry.png')});
  const events=await page.evaluate(()=>window.events);assert.equal(events.some(event=>event.event==='error'||event.event==='abort'),false,JSON.stringify(events.filter(event=>event.event==='error'||event.event==='abort')));
  assert.deepEqual(errors,[]);
  report.push({game,title,musicRoom,entry,firstFrame:true,runtimeBytes:Object.values(inventories[game].files).reduce((sum,file)=>sum+file.bytes,0)});
  await page.close();
 }
 assert.equal(requests.some(path=>/\/fonts\/font\d+\.bin$/.test(path)),false);
 await writeFile(resolve(output,'report.json'),JSON.stringify({passed:true,scope:'Packaged ordinary Runtimes, shared Gothic font, title and Stage entry; private local Chromium',report,requests},null,2)+'\n');
 console.log(JSON.stringify({passed:true,report,output}));
}finally{await browser?.close();await new Promise(ok=>server.close(ok));}
