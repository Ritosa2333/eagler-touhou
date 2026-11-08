// Three real PC-98 player runtimes and a delayed local Relay.
// Reports native gameplay and displayed frames; does not assert machine-specific FPS.
// Run from launcher with ROOM_RUNTIME_GAME, ROOM_RUNTIME_ROOT and
// ROOM_RUNTIME_DATA pointing to a built TH02 or TH04 runtime and data file.
// Relay delay defaults to 45 ms each way plus 5 ms jitter. Set
// PC98_FPS_ROLLBACK=0 for a confirmed-lockstep comparison. All three players
// share this machine, so CPU contention differs from three separate devices.
// The benchmark explicitly samples the worker's native game counter each second.
// PC98_FPS_MIN can optionally make any lower one-second sample fail the run.
// PC98_FPS_SECONDS sets the sampling duration (default 20 seconds).
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th02','th04'].includes(game),'ROOM_RUNTIME_GAME must be th02 or th04');
const sampleSeconds=Number(process.env.PC98_FPS_SECONDS??20);
assert(Number.isInteger(sampleSeconds)&&sampleSeconds>=1&&sampleSeconds<=300,'invalid PC98_FPS_SECONDS');
const timing={...multiplayerInputTimingPolicy(PRODUCT_GAMES[game].multiplayer),rollback:process.env.PC98_FPS_ROLLBACK!=='0'};
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),data=await readFile(process.env.ROOM_RUNTIME_DATA);
let relayLog='',browser,page;
const clients=[],errors=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
  cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_DELAY_MS:process.env.EAGLER_NETPLAY_RELAY_DELAY_MS??'45',EAGLER_NETPLAY_RELAY_JITTER_MS:process.env.EAGLER_NETPLAY_RELAY_JITTER_MS??'5',EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
async function wait(check,label,timeout=90000){const until=Date.now()+timeout;while(Date.now()<until){if(await check())return;await new Promise(r=>setTimeout(r,30));}throw Error(label+' timed out\n'+relayLog);}
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111;display:flex}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data')).arrayBuffer();return{buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
window.addRuntime=n=>{const f=document.createElement('iframe');f.id='p'+n;f.src='/runtime/${game}.html?hosted=1&managedData=1&runtimeVariant=multiplayer&runtimeEpoch='+(n+1);document.body.append(f);};
window.rpc=(n,command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('#p'+n),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'));},90000);function recv(e){if(e.source===f.contentWindow&&e.data?.event==='exit'){clearTimeout(timer);removeEventListener('message',recv);no(Error('Native exit: '+e.data.status));return;}if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:n+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('content-type','text/html');res.end(html);return;}
  if(path==='/data'){res.end(data);return;}
  if(!path.startsWith('/runtime/'))throw Error('route');
  const file=resolve(runtime,path.slice(9));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream');  res.end(await readFile(file));
}catch{res.writeHead(404).end();}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
function send(client,message,predicate){return new Promise((ok,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('Relay timeout '+JSON.stringify(message)));},15000);
  function receive(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();ok(m);}}
  function cleanup(){clearTimeout(timer);client.removeEventListener('message',receive);}
  client.addEventListener('message',receive);if(message)client.send(JSON.stringify(message));});}
const playerFrame=()=>globalThis.__eaglerNetplayLanFrame??0;
try{
  await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'Relay startup');
  const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-1234';
  for(let actor=0;actor<3;actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:'3',difficulty:'1'}));clients.push(client);
    client.addEventListener('message',e=>{const message=JSON.parse(e.data);if(message.room)client.retentionRoom=message.room;});
    await send(client,null,m=>m.type==='state');
    await send(client,{type:'take-seat',seat:actor,loadout:0},m=>!!m.room?.seats[actor]);
  }
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();await page.evaluateOnNewDocument(()=>Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true}));page.on('pageerror',e=>errors.push(e.message));
  page.on('dialog',dialog=>{errors.push(dialog.message());void dialog.dismiss();});
  page.on('console',m=>console.log('Runtime:',m.text()));
  await page.setViewport({width:1920,height:540});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(()=>{for(let n=0;n<3;n++)addRuntime(n);});
  await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===3,{timeout:120000});
  const frames=()=>[1,2,3].map(epoch=>page.frames().find(f=>new URL(f.url()).searchParams.get('runtimeEpoch')===String(epoch)));
  for(let actor=0;actor<3;actor++)await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[actor]?.ready);
  const start=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  const run=start.serial;
  const options=actor=>({touchEnabled:process.env.PC98_TOUCH_INPUT==='1',touchMovementMode:'touch-unlimited',touchSensitivity:150,netplayMode:'lan',netplayChallengeMode:process.env.PC98_TEST_CHALLENGE==='1',netplayDisableFocus:process.env.PC98_TEST_DISABLE_FOCUS==='1',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(run),member:'retention_member_'+actor,player:String(actor)}),netplayPlayer:actor,netplayPlayerCount:3,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:0,shot:1},{character:game==='th02'?0:1,shot:game==='th02'?2:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:timing.rollback?0:1,netplayPredictionLimit:timing.predictionLimit,netplaySpectator:false,netplaySpectatorCount:0,netplaySpectatorTimeline:true});
  const configure=async(actor,serial)=>{
    await page.evaluate(async({actor,options})=>rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]}),{actor,options:options(actor)});
    // Give the running native title several ticks between protocol commands.
    // Configuration must not initialize or connect the upcoming run early.
    await new Promise(r=>setTimeout(r,500));
    await page.evaluate(({actor,serial})=>rpc(actor,'launch',{launchSerial:serial}),{actor,serial});
  };
  await Promise.all([configure(0,1),configure(1,1),configure(2,1)]);
  await page.evaluate(()=>Promise.all([0,1,2].map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}))));
  await wait(async()=>{const values=await Promise.all(frames().slice(0,3).map(f=>f.evaluate(playerFrame)));return values.every(v=>v>120);},'native players advancing');
  await wait(async()=>{const values=await Promise.all(frames().map(f=>f.evaluate(playerFrame)));return values.every(v=>v>=300);},'warmup',120000);
  const sample=(includeTape=false)=>Promise.all(frames().map(f=>f.evaluate(async({game,includeTape})=>{const {managedAPI}=await import('./netplay/room.js');const recorder=globalThis[game+'Replay'],health=managedAPI.health();const native=await managedAPI.sampleGame();if(!native||!Number.isInteger(native.gameFrame))throw Error('native game counter unavailable');health.gameFrame=native.gameFrame;return {health,pointError:recorder?.pointError||'',snapshot:includeTape?recorder?.snapshot():null,diagnostics:includeTape?document.getElementById('diagnostics')?.textContent||'':''};},{game,includeTape})));
  const begin=await sample(),at=performance.now();
  const directions=setInterval(()=>void page.evaluate(()=>Promise.all([0,1,2].map(n=>rpc(n,'keyboard',{code:'ArrowLeft',key:'ArrowLeft',keyCode:37,location:0,down:Math.floor(Date.now()/300)%2===0})))).catch(()=>{}),300);
  const windows=[];let prior=begin,priorAt=at;for(let n=0;n<sampleSeconds;n++){await new Promise(r=>setTimeout(r,1000));const next=await sample(),now=performance.now(),seconds=(now-priorAt)/1000;windows.push(next.map((x,i)=>(x.health.gameFrame-prior[i].health.gameFrame)/seconds));prior=next;priorAt=now;}clearInterval(directions);const elapsed=(priorAt-at)/1000,finalTape=await sample(true),end=prior;for(let i=0;i<end.length;i++){end[i].snapshot=finalTape[i].snapshot;end[i].pointError=finalTape[i].pointError;end[i].diagnostics=finalTape[i].diagnostics;}
  assert.deepEqual(errors,[]);const boundary=Math.floor(Math.min(...end.map(x=>x.snapshot.rows.length))/120)*120;
  const hashes=end.map(x=>x.snapshot.rows[boundary-1]?.hash);assert(hashes.every(h=>h&&h.tick===boundary&&typeof h.hash==='string'&&JSON.stringify(h)===JSON.stringify(hashes[0])),'confirmed hashes missing or differ');assert(end.every(x=>!x.pointError),'checkpoint builder error');
  console.log('RESULT',JSON.stringify({elapsed,boundary,hashes,points:(finalTape[0].snapshot.points||[]).map(p=>({frame:p.frame,base64Bytes:p.data.length})),windows,diagnostics:end.map(x=>x.diagnostics),clients:begin.map((b,i)=>({begin:b.health,end:end[i].health,gameFps:(end[i].health.gameFrame-b.health.gameFrame)/elapsed,syncFps:(end[i].health.frame-b.health.frame)/elapsed,drawFps:(end[i].health.frames-b.health.frames)/elapsed}))}));
  if(process.env.PC98_FPS_MIN!==undefined){const min=Number(process.env.PC98_FPS_MIN);assert(Number.isFinite(min)&&min>=0,'invalid PC98_FPS_MIN');assert(windows.every(window=>window.every(fps=>fps>=min)),`one-second game frame rate fell below ${min}`);}
}finally{for(const c of clients)c.close();await browser?.close();await new Promise(r=>server.close(r));relay.kill();}
