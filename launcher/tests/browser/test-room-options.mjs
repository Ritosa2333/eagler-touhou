// Production room preview | real menu navigation and native option changes.
// ROOM_OPTIONS_INPUTS supplies locally owned DATA and the selected Runtime roots.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createHash} from 'node:crypto';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

assert(process.env.ROOM_OPTIONS_INPUTS,'Set ROOM_OPTIONS_INPUTS to the private Runtime/DATA input JSON');
const inputs=JSON.parse(await readFile(process.env.ROOM_OPTIONS_INPUTS));
const output=resolve(process.env.ROOM_OPTIONS_OUTPUT||'.cache/room-options');await mkdir(output,{recursive:true});
const selected=process.env.ROOM_OPTIONS_GAMES?.split(',')||Object.keys(inputs.games);
const fonts=new Map(await Promise.all([
 ['/msgothic.ttc',process.env.EAGLER_SHARED_GOTHIC],['/unifont.otf',process.env.EAGLER_SHARED_UNIFONT],
].map(async([name,path])=>[name,await readFile(path)])));
const browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable(),args:[
 '--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding',
]});
const results=[];
const pause=ms=>new Promise(r=>setTimeout(r,ms));
try{for(const game of selected){
 const entry=inputs.games[game],runtime=resolve(entry.runtime),data=await readFile(entry.data),errors=[];
 const report={game,release:inputs.release||null,runtime,dataSha256:createHash('sha256').update(data).digest('hex'),errors};
 const wasmName=['th08','th10','th11'].includes(game)?game+'-sdl.wasm':game+'.wasm';
 report.wasmSha256=createHash('sha256').update(await readFile(resolve(runtime,wasmName))).digest('hex');
 const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0}iframe{width:640px;height:480px;border:0}</style><script>
 window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data')).arrayBuffer();return{buffer,bytes:buffer.byteLength};};
 addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data);});
 window.rpc=(command,fields={})=>new Promise((ok,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timeout'));},90000);function receive(e){if(e.source===document.querySelector('iframe').contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?ok(e.data):no(Error(e.data.error));}}addEventListener('message',receive);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin);});
 </script><iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeVariant=multiplayer&runtimeEpoch=1"></iframe>`;
 const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  if(path==='/data'){res.end(data);return;}
  if(fonts.has(path)){res.end(fonts.get(path));return;}
  assert(path.startsWith('/runtime/'));
  const file=resolve(runtime,path.slice(9));assert(file.startsWith(runtime+sep));
  res.setHeader('Content-Type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(error){res.writeHead(404).end(String(error));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 const page=await browser.newPage();page.on('pageerror',e=>errors.push(e.message));
 try{
  await page.setViewport({width:650,height:490});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.waitForFunction(()=>events.some(e=>e.event==='ready'),{timeout:120000});
  await page.evaluate(async game=>{await rpc('configure',{music:'none',resources:[],runtimeResources:[],sharedResources:game==='th075'?[]:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{thpracEnabled:false,renderScale:1,touchEnabled:true}});await rpc('prepare-room');},game);
  if(process.env.ROOM_OPTIONS_RETURN==='1'&&game!=='th075'){
   // Reuse the actual native return operation, then the acknowledged local
   // configuration sent by RuntimeService before it re-enables menu input.
   report.moduleRetained=await page.evaluate(async game=>{
    const frame=window.frames[0],r=frame['__'+game+'Runtime'],owner=frame.Module||r?.Module||r?.core,document=frame.document;
    await rpc('configure',{music:'none',options:{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:1/?room='+game+'mp-1234&run=1&member=room-options&spectator=room-options-viewer',netplayPlayer:0,netplayPlayerCount:2,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:game==='th15'?1:0,netplayPredictionLimit:game==='th11'?12:game==='th15'?0:8,netplayPredictionReserve:2,netplaySpectator:true,netplaySpectatorId:'room-options-viewer',touchEnabled:true},resources:[],runtimeResources:[],sharedResources:[]});
    await rpc('return-room');
    await rpc('configure',{music:'none',options:{thpracEnabled:false,renderScale:1,touchEnabled:true},resources:[],runtimeResources:[],sharedResources:[]});
    const returned=frame['__'+game+'Runtime'];return frame.document===document&&!!owner&&owner===(frame.Module||returned?.Module||returned?.core);
   },game);
   assert.equal(report.moduleRetained,true,'return restores local input in the same game instance');
   report.roomReturn=true;
  }
  await pause(report.roomReturn&&['th06','th07'].includes(game)?5000:1000);
  const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));assert(frame);
  const snapshot=()=>frame.evaluate(game=>{
   const r=window['__'+game+'Runtime'],c=window.Module||r?.Module||r?.core,fs=c?.FS||window.FS,configs={};
   if(fs)for(const root of fs.readdir('/').filter(n=>n==='save'||n.startsWith('saves'))){
    try{for(const name of fs.readdir('/'+root))if(/\.cfg$|^config\.dat$/i.test(name))configs[root+'/'+name]=Array.from(fs.readFile('/'+root+'/'+name));}catch{}
   }
   let title;
   if(game==='th09')title=r.status().title;
   else if(game==='th075')title=[c._th075_scene(),c._th075_frame()];
   else if(game==='th06'||game==='th07')title=[c._eagler_room_preview_ready()];
   else if(game==='th11')title=[r.core.th11_phase(),r.core.th11_frame()];
   else if(game==='th15')title=[c._th15_phase(),c._th15_frame()];
   else title=r.status();
   return{title,configs};
  },game);
  const key=async(code,physical=false)=>{
   if(physical){await frame.focus('canvas');await page.keyboard.down(code);await pause(100);await page.keyboard.up(code);}
   else for(const down of [true,false]){await page.evaluate(({game,code,down})=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:1,command:'keyboard',code,key:code==='KeyZ'?'z':code,keyCode:({ArrowUp:38,ArrowLeft:37,KeyZ:90,Escape:27})[code],location:0,down},location.origin),{game,code,down});await pause(100);}
   await pause(150);
  };
  report.before=await snapshot();await page.screenshot({path:resolve(output,game+'-title.png')});
  // Confirm is blocked on game-start entries, including the initial 06 title.
  await key('KeyZ');await pause(1000);
  for(let n=0;n<(game==='th15'?3:2);n++)await key('ArrowUp');
  await key('KeyZ');await pause(1000);
  report.options=await snapshot();await page.screenshot({path:resolve(output,game+'-options.png')});
  if(game==='th09')assert.equal(report.options.title[2],9,'enter native Options before launch');
  if(game==='th075')assert.equal(report.options.title[0],9,'enter native Options before launch');
  if(game==='th06'||game==='th07')assert.equal(report.options.title[0],0,'leave the main title for native Options');
  // Change an actual native option using a focused physical key, then return
  // through the menu. No internal setter or diagnostic mutation drives this.
  await key('ArrowLeft',true);await page.screenshot({path:resolve(output,game+'-changed.png')});
  await key('Escape');await key('Escape');if(game==='th06')await key('KeyZ');await pause(1000);
  await page.evaluate(()=>rpc('sync'));report.after=await snapshot();
  await page.screenshot({path:resolve(output,game+'-returned.png')});
  report.configChanged=JSON.stringify(report.after.configs)!==JSON.stringify(report.before.configs);
  assert.deepEqual(errors,[]);report.passed=true;
 }catch(error){report.passed=false;report.failure=error.stack;report.events=await page.evaluate(()=>events.filter(e=>e.event==='error')).catch(()=>[]);}
 finally{await page.close();await new Promise(r=>server.close(r));}
 results.push(report);await writeFile(resolve(output,game+'.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({game,passed:report.passed,before:report.before?.title,options:report.options?.title,after:report.after?.title,failure:report.failure?.split('\n')[0]}));
}}finally{await browser.close();await writeFile(resolve(output,'results.json'),JSON.stringify(results,null,2)+'\n');}
process.exitCode=results.every(r=>r.passed)?0:1;
