// Three production multiplayer Wasm instances, real local Relay and private DATA.
// A spectator returns to the native menu and rejoins the same ongoing match.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th06','th07','th08','th09','th10','th11','th15'].includes(game),'ROOM_RUNTIME_GAME must be a multiplayer title');
const timing=multiplayerInputTimingPolicy(PRODUCT_GAMES[game].multiplayer);
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA','EAGLER_SHARED_GOTHIC','EAGLER_SHARED_UNIFONT'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),out=resolve('.cache/native-room-retention',game);
await mkdir(out,{recursive:true});
const data=await readFile(process.env.ROOM_RUNTIME_DATA),fonts=new Map([
  ['/msgothic.ttc',await readFile(process.env.EAGLER_SHARED_GOTHIC)],['/unifont.otf',await readFile(process.env.EAGLER_SHARED_UNIFONT)]]);
let relayLog='',browser,page;
const clients=[],errors=[],requests=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
  cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},stdio:['ignore','pipe','pipe']});
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
const playerFrame=()=>{
  if(typeof globalThis.__eaglerNetplayLanFrame==='number')return globalThis.__eaglerNetplayLanFrame;
  const c=globalThis.Module;return c?._th09_rollback_info?c.HEAPU32[(c._th09_rollback_info()>>>2)+1]:0;
};
const versusPlayback=()=>{const c=Module;return Array.from(c.HEAPU32.subarray(c._th09_spectator_playback_info()>>>2,(c._th09_spectator_playback_info()>>>2)+11));};
try{
  await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'Relay startup');
  const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-1234';
  for(let actor=0;actor<3;actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:'2',difficulty:'1'}));clients.push(client);
    client.addEventListener('message',e=>{const message=JSON.parse(e.data);if(message.room)client.retentionRoom=message.room;});
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
  const start=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  let run=start.serial,roles=[0,1,null];
  const options=actor=>({touchEnabled:false,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(run),member:'retention_member_'+actor,...roles[actor]===null?{spectator:'retention_client_'+actor}:{player:String(roles[actor])}}),netplayPlayer:roles[actor]??0,netplayPlayerCount:2,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:timing.rollback?0:1,netplayPredictionLimit:timing.predictionLimit,netplaySpectator:roles[actor]===null,netplaySpectatorId:roles[actor]===null?'retention_client_'+actor:'',netplaySpectatorCount:1,netplaySpectatorTimeline:true,netplayInGameSelection:game==='th09'});
  const configure=async(actor,serial)=>{
    await page.evaluate(async({actor,options})=>rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]}),{actor,options:options(actor)});
    // Give the running native title several ticks between protocol commands.
    // Configuration must not initialize or connect the upcoming run early.
    await new Promise(r=>setTimeout(r,500));
    assert(await frames()[actor].evaluate(()=>!globalThis.__eaglerPeerTransport || globalThis.__eaglerPeerTransport.closed),
      'room configuration leaves the native title offline until launch');
    await page.evaluate(({actor,serial})=>rpc(actor,'launch',{launchSerial:serial}),{actor,serial});
  };
  await Promise.all([configure(0,1),configure(1,1)]);
  const selectVersus=async actors=>{
  if(game!=='th09')return;
  await wait(async()=>{const reports=await Promise.all(actors.map(n=>frames()[n].evaluate(()=>Array.from(Module.HEAPU32.subarray(Module._th09_title_status()/4,Module._th09_title_status()/4+8)))));return reports.every(s=>s[2]===7&&s[3]===1&&s[5]>=10);},'native versus selection menu');
  for(let press=0;press<3;press++){
    for(const down of [true,false]){
      await page.evaluate(async({down,actors})=>Promise.all(actors.map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down}))),{down,actors});
      await new Promise(r=>setTimeout(r,400));
    }
  }
  };
  await selectVersus([0,1]);
  await wait(async()=>{const values=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));return values.every(v=>v>120);},'native players advancing');
  const initialRequests=requests.length,viewer=frames()[2],cycles=[];
  for(let attempt=0;attempt<3;attempt++){
    await configure(2,attempt+1);
    await page.waitForFunction(serial=>events.some(e=>e.epoch===3&&e.event==='first-frame'&&e.launchSerial===serial),{timeout:90000},attempt+1);
    if(game==='th09')await wait(async()=>{const p=await viewer.evaluate(versusPlayback);return p[2]>30&&p[1]>0;},'native versus spectator playback');
    else await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.spectatorPlayback?.position>30),{timeout:90000});
    if(attempt===0&&game==='th09'){
      await viewer.evaluate(()=>document.getElementById('spectatorPause').click());
      await wait(async()=>(await viewer.evaluate(versusPlayback))[5]===1,'versus spectator pause');
      await wait(()=>viewer.evaluate(()=>{const c=Module,p=c.HEAPU32[c._th09_spectator_playback_info()/4+1],view=__th09Runtime.netplay.spectatorTape.timeline(p);return view&&view.edge-view.min>=240;}),'versus spectator retained seek range');
      const target=await viewer.evaluate(()=>{const range=document.getElementById('spectatorSeek'),position=Math.floor((Number(range.min)+Number(range.max))/2),target=__th09Runtime.netplay.spectatorTape.locate(position);if(!target)throw Error('Spectator seek requires retained match history');range.value=String(position);range.dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowLeft'}));range.dispatchEvent(new Event('input',{bubbles:true}));range.dispatchEvent(new Event('change',{bubbles:true}));return target.frame;});
      await wait(async()=>{const p=await viewer.evaluate(versusPlayback);return !p[6]&&p[4]===target&&p[1]>=target;},'versus spectator exact seek');
      await wait(()=>viewer.evaluate(()=>!document.getElementById('spectatorLive').disabled),'versus spectator Live available');
      await viewer.evaluate(()=>document.getElementById('spectatorLive').click());
      await wait(async()=>{const p=await viewer.evaluate(versusPlayback);return !!p[7]&&!p[6];},'versus spectator Live');
    }else if(attempt===0){
      const control=async(action,frame)=>page.evaluate(async({action,frame})=>{
        const run=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback.run;
        await rpc(2,'spectator-playback',{run,action,...frame===undefined?{}:{frame}});
      },{action,frame});
      await control('pause');
      await page.waitForFunction(()=>{const p=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return p?.paused&&!p.seeking;});
      const target=await page.evaluate(()=>{const p=events.findLast(e=>e.epoch===3&&e.spectatorPlayback).spectatorPlayback;return Math.floor((p.min+p.edge)/2);});
      await control('seek',target);
      await page.waitForFunction(target=>{const p=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return p?.paused&&!p.seeking&&p.position===target;},{timeout:30000},target);
      await control('live');
      await page.waitForFunction(()=>{const p=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return p?.following&&!p.paused&&!p.seeking;});
    }
    if(game==='th09'&&attempt===1){
      await viewer.evaluate(()=>__th09Runtime.netplay.close('Spectator connection interrupted'));
      await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.event==='room-menu'&&e.launchSerial===2));
    }
    await page.evaluate(async()=>{await rpc(2,'sync');await rpc(2,'return-room');});
    assert(await viewer.evaluate(()=>Module===retainedModule&&document===retainedDocument&&(!Module._eagler_room_preview_ready||!!Module._eagler_room_preview_ready())),'spectator returns to the same native title');
    const beforeIdle=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));
    await new Promise(r=>setTimeout(r,1500));
    const players=await Promise.all(frames().slice(0,2).map(async f=>({frame:await f.evaluate(playerFrame),failed:await f.evaluate(()=>globalThis.__eaglerNetplayFailed)})));
    assert(players.every((s,i)=>!s.failed&&s.frame>beforeIdle[i]),'players keep advancing while the spectator stays at the menu');cycles.push({attempt:attempt+1,players});
    await page.evaluate(()=>{events=events.filter(e=>e.epoch!==3);});
  }
  await page.evaluate(async()=>Promise.all([0,1].map(async n=>{await rpc(n,'sync');await rpc(n,'return-room');})));
  await wait(()=>clients[0].retentionRoom?.phase==='lobby','both players return to the same room');
  await send(clients[2],{type:'leave-spectator'},m=>m.room&&!m.room.spectators.some(s=>s.clientId==='retention_client_2'));
  await send(clients[1],{type:'spectate'},m=>m.room?.spectators.some(s=>s.clientId==='retention_client_1'));
  await send(clients[2],{type:'take-seat',seat:1,loadout:1},m=>m.room?.seats[1]?.clientId==='retention_client_2');
  for(const [actor,seat] of [[0,0],[2,1]])await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[seat]?.ready);
  const next=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  run=next.serial;roles=[0,null,1];
  await Promise.all([configure(0,2),configure(2,4)]);
  await selectVersus([0,2]);
  await configure(1,2);
  await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.event==='first-frame'&&e.launchSerial===4),{timeout:90000});
  await wait(async()=>{const values=await Promise.all([0,2].map(n=>frames()[n].evaluate(playerFrame)));return values.every(v=>v>120);},'former spectator advances as a player');
  for(const frame of frames())assert(await frame.evaluate(()=>Module===retainedModule&&document===retainedDocument),'role changes retain every native game instance');
  const roleSwap={spectatorToPlayer:true,playerToSpectator:true,run};
  assert(!requests.slice(initialRequests).some(u=>/\.(?:data|wasm|js|mjs)(?:\?|$)/.test(u)||u.endsWith('/data')),'spectator return/rejoin never reloads program or DATA');
  assert.deepEqual(errors,[]);await page.screenshot({path:resolve(out,'retained-role-swap.png')});
  await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,game,cycles,roleSwap,sameDocument:true,sameModule:true,noProgramOrDataReload:true,scope:'Production Wasm and original DATA, two local players and one spectator, real local Relay. No physical device or public-network acceptance.'},null,2));
  console.log(JSON.stringify({passed:true,game,cycles,roleSwap}));
}catch(error){if(page){await writeFile(resolve(out,'failure.json'),JSON.stringify({error:String(error),errors,relayLog,events:await page.evaluate(()=>window.events).catch(()=>[]),native:await Promise.all(page.frames().filter(f=>f.url().includes('runtimeEpoch=')).map(f=>f.evaluate(()=>{const r=globalThis.__th08Runtime;return {globals:Object.fromEntries(Object.keys(globalThis).filter(k=>k.startsWith('__eaglerNetplay')&&['number','boolean','string'].includes(typeof globalThis[k])).map(k=>[k,globalThis[k]])),peer:(()=>{const p=globalThis.__eaglerPeerTransport;return p&&{closed:p.closed,retained:p.retainSpectatorHistory,route:p.route,relay:p.relay?.readyState};})(),th08:r&&{status:r.status(),netplay:Array.from(new Uint32Array(r.core.memory.buffer,r.core.multiplayer_netplay_status(r.app),15)),spectator:Array.from(new Uint32Array(r.core.memory.buffer,r.core.multiplayer_spectator_status(r.app),8))}};}).catch(()=>null)))},null,2));await page.screenshot({path:resolve(out,'failure.png')}).catch(()=>{});}throw error;}
finally{for(const client of clients)client.close();await browser?.close();server.closeAllConnections();server.close();relay.kill();}
