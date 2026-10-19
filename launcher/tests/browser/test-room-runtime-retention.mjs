// Three production TH06/TH07 Wasm instances, real local Relay and private DATA.
// A spectator returns to the native menu and rejoins the same ongoing match.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th06','th07'].includes(game),'ROOM_RUNTIME_GAME must be th06 or th07');
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA','EAGLER_SHARED_GOTHIC','EAGLER_SHARED_UNIFONT'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),out=resolve('.cache/native-room-retention',game);
await mkdir(out,{recursive:true});
const data=await readFile(process.env.ROOM_RUNTIME_DATA),fonts=new Map([
  ['/msgothic.ttc',await readFile(process.env.EAGLER_SHARED_GOTHIC)],['/unifont.otf',await readFile(process.env.EAGLER_SHARED_UNIFONT)]]);
let relayLog='',browser,page;
const clients=[],errors=[],requests=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
  cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
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
  if(fonts.has(path)){res.end(fonts.get(path));return;}
  if(!path.startsWith('/runtime/'))throw Error('route');
  const file=resolve(runtime,path.slice(9));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
}catch{res.writeHead(404).end();}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
function send(client,message,predicate){return new Promise((ok,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('Relay timeout '+JSON.stringify(message)));},15000);
  function receive(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();ok(m);}}
  function cleanup(){clearTimeout(timer);client.removeEventListener('message',receive);}
  client.addEventListener('message',receive);if(message)client.send(JSON.stringify(message));});}
try{
  await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'Relay startup');
  const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-retained-menu';
  for(let actor=0;actor<3;actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:'2',difficulty:'1'}));clients.push(client);
    await send(client,null,m=>m.type==='state');
    await send(client,actor<2?{type:'take-seat',seat:actor,loadout:actor}:{type:'spectate'},m=>actor<2?!!m.room?.seats[actor]:m.room?.spectators?.some(s=>s.clientId==='retention_client_2'));
  }
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();page.on('pageerror',e=>errors.push(e.message));page.on('request',r=>requests.push(r.url()));
  page.on('dialog',dialog=>{errors.push(dialog.message());void dialog.dismiss();});
  page.on('console',m=>console.log('Runtime:',m.text()));
  await page.setViewport({width:1920,height:540});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(()=>{for(let n=0;n<3;n++)addRuntime(n);});
  await page.waitForFunction(()=>events.filter(e=>e.event==='ready'&&e.roomReturn).length===3,{timeout:120000});
  await page.evaluate(async()=>{for(let n=0;n<3;n++){await rpc(n,'configure',{music:'none',resources:[],runtimeResources:[],sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{touchEnabled:false}});await rpc(n,'prepare-room');}});
  const frames=()=>page.frames().filter(f=>f.url().includes('runtimeEpoch='));
  for(const frame of frames())await frame.evaluate(()=>{window.retainedModule=Module;window.retainedDocument=document;});
  for(let actor=0;actor<2;actor++)await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[actor]?.ready);
  const start=await send(clients[0],{type:'start',inputDelay:0},m=>m.type==='start');
  const options=actor=>({touchEnabled:false,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(start.serial),member:'retention_member_'+actor}),netplayPlayer:actor<2?actor:0,netplayPlayerCount:2,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:0,netplayPredictionLimit:8,netplaySpectator:actor===2,netplaySpectatorId:actor===2?'retention_client_2':'',netplaySpectatorCount:1,netplaySpectatorTimeline:true});
  const configure=async(actor,serial)=>page.evaluate(async({actor,options,serial})=>{await rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]});await rpc(actor,'launch',{launchSerial:serial});},{actor,options:options(actor),serial});
  await Promise.all([configure(0,1),configure(1,1)]);
  await wait(async()=>{const values=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>Number(globalThis.__eaglerNetplayLanFrame)||0)));return values.every(v=>v>120);},'native players advancing');
  const initialRequests=requests.length,viewer=frames()[2],cycles=[];
  for(let attempt=0;attempt<3;attempt++){
    await configure(2,attempt+1);
    await page.waitForFunction(serial=>events.some(e=>e.epoch===3&&e.event==='first-frame'&&e.launchSerial===serial),{timeout:90000},attempt+1);
    await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.spectatorPlayback?.position>30),{timeout:90000});
    await page.evaluate(async()=>{await rpc(2,'sync');await rpc(2,'return-room');});
    assert(await viewer.evaluate(()=>Module===retainedModule&&document===retainedDocument&&!!Module._eagler_room_preview_ready()),'spectator returns to the same native title');
    const beforeIdle=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>globalThis.__eaglerNetplayLanFrame)));
    await new Promise(r=>setTimeout(r,1500));
    const players=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>({frame:globalThis.__eaglerNetplayLanFrame,failed:globalThis.__eaglerNetplayFailed}))));
    assert(players.every((s,i)=>!s.failed&&s.frame>beforeIdle[i]),'players keep advancing while the spectator stays at the menu');cycles.push({attempt:attempt+1,players});
    await page.evaluate(()=>{events=events.filter(e=>e.epoch!==3);});
  }
  assert(!requests.slice(initialRequests).some(u=>/\.(?:data|wasm|js|mjs)(?:\?|$)/.test(u)||u.endsWith('/data')),'spectator return/rejoin never reloads program or DATA');
  assert.deepEqual(errors,[]);await page.screenshot({path:resolve(out,'spectator-menu-retained.png')});
  await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,game,cycles,sameDocument:true,sameModule:true,noProgramOrDataReload:true,scope:'Production Wasm and original DATA, two local players and one spectator, real local Relay. No physical device or public-network acceptance.'},null,2));
  console.log(JSON.stringify({passed:true,game,cycles}));
}catch(error){if(page){await writeFile(resolve(out,'failure.json'),JSON.stringify({error:String(error),errors,relayLog,events:await page.evaluate(()=>window.events).catch(()=>[])},null,2));await page.screenshot({path:resolve(out,'failure.png')}).catch(()=>{});}throw error;}
finally{for(const client of clients)client.close();await browser?.close();server.closeAllConnections();server.close();relay.kill();}
