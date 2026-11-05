// Three production multiplayer Wasm instances, real local Relay and private DATA.
// Tests live playback, pause, seeking, and original sound effects.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th01','th02','th03','th04'].includes(game),'ROOM_RUNTIME_GAME must be a multiplayer title');
const timing=multiplayerInputTimingPolicy(PRODUCT_GAMES[game].multiplayer);
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),out=resolve('.cache/native-room-retention',game);
await mkdir(out,{recursive:true});
const data=await readFile(process.env.ROOM_RUNTIME_DATA),fonts=new Map();
let relayLog='',browser,page;
const clients=[],errors=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
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
try{
  await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'Relay startup');
  const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-1234';
  for(let actor=0;actor<3;actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:'2',difficulty:'1'}));clients.push(client);
    client.addEventListener('message',e=>{const message=JSON.parse(e.data);if(message.room)client.retentionRoom=message.room;});
    await send(client,null,m=>m.type==='state');
    await send(client,actor<2?{type:'take-seat',seat:actor,loadout:0}:{type:'spectate'},m=>actor<2?!!m.room?.seats[actor]:m.room?.spectators?.some(s=>s.clientId==='retention_client_2'));
  }
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();page.on('pageerror',e=>errors.push(e.message));
  page.on('dialog',dialog=>{errors.push(dialog.message());void dialog.dismiss();});
  page.on('console',m=>console.log('Runtime:',m.text()));
  await page.setViewport({width:1920,height:540});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(()=>{for(let n=0;n<3;n++)addRuntime(n);});
  await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===3,{timeout:120000});
  const frames=()=>[1,2,3].map(epoch=>page.frames().find(f=>new URL(f.url()).searchParams.get('runtimeEpoch')===String(epoch)));
  for(let actor=0;actor<2;actor++)await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[actor]?.ready);
  const start=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  let run=start.serial,roles=[0,1,null];
  const options=actor=>({touchEnabled:process.env.PC98_TOUCH_INPUT==='1',touchMovementMode:'touch-unlimited',touchSensitivity:150,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(run),member:'retention_member_'+actor,...roles[actor]===null?{spectator:'retention_client_'+actor}:{player:String(roles[actor])}}),netplayPlayer:roles[actor]??0,netplayPlayerCount:2,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:0,shot:1}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:timing.rollback?0:1,netplayPredictionLimit:timing.predictionLimit,netplaySpectator:roles[actor]===null,netplaySpectatorId:roles[actor]===null?'retention_client_'+actor:'',netplaySpectatorCount:1,netplaySpectatorTimeline:true});
  const configure=async(actor,serial)=>{
    await page.evaluate(async({actor,options})=>rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]}),{actor,options:options(actor)});
    // Give the running native title several ticks between protocol commands.
    // Configuration must not initialize or connect the upcoming run early.
    await new Promise(r=>setTimeout(r,500));
    await page.evaluate(({actor,serial})=>rpc(actor,'launch',{launchSerial:serial}),{actor,serial});
  };
  await Promise.all([configure(0,1),configure(1,1)]);
  if(game==='th03')for(let i=0;i<4;i++)for(const actor of [0,1]){
    await frames()[actor].click('#canvas');
    if(process.env.PC98_TOUCH_INPUT==='1'){
      await page.evaluate(()=>{let button=document.getElementById('focus-out');if(!button){button=document.createElement('button');button.id='focus-out';button.textContent='focus';document.body.append(button);}button.focus();});
      for(const type of ['down','up']){await page.evaluate(({actor,type})=>rpc(actor,'direct-touch',{type,id:1,x:.5,y:.5}),{actor,type});await new Promise(r=>setTimeout(r,250));}
    }else for(const down of [true,false]){await page.evaluate(({actor,down})=>rpc(actor,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down}),{actor,down});await new Promise(r=>setTimeout(r,250));}
  }
  if(game==='th02'||game==='th04')await page.evaluate(()=>Promise.all([0,1].map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}))));
  await wait(async()=>{const values=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));return values.every(v=>v>120);},'native players advancing');
  if(game==='th03'&&process.env.PC98_TOUCH_INPUT==='1'){
    await wait(async()=>(await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>globalThis.th03SyncState?.phase===1)))).every(Boolean),'touch selection enters gameplay',30000);
    const position=()=>frames()[1].evaluate(async()=>(await import('./managed-room.mjs')).managedAPI.health().players.find(p=>p.seat===1)?.x);
    const before=await position();assert.equal(typeof before,'number');
    await page.evaluate(()=>rpc(1,'direct-touch',{type:'down',id:42,x:.4,y:.5}));
    for(let i=1;i<=8;i++){await page.evaluate(i=>rpc(1,'direct-touch',{type:'move',id:42,x:.4+i*.015,y:.5}),i);await new Promise(r=>setTimeout(r,100));}
    await page.evaluate(()=>rpc(1,'direct-touch',{type:'up',id:42,x:.52,y:.5}));await new Promise(r=>setTimeout(r,500));
    assert.notEqual(await position(),before,'P2 drag moves the actual player');
    await page.evaluate(()=>rpc(1,'keyboard',{code:'Space',key:' ',keyCode:32,location:0,down:true}));
    await wait(async()=>frames()[1].evaluate(async()=>(await import('./managed-room.mjs')).managedAPI.health().players.some(p=>p.seat===1&&p.focused)),'native focus activated',20000);
    const beforeFocus=await frames()[1].evaluate(playerFrame);
    await wait(async()=>(await frames()[1].evaluate(playerFrame))>=beforeFocus+300,'native marker keeps match synchronized',20000);
    await page.evaluate(()=>rpc(1,'keyboard',{code:'Space',key:' ',keyCode:32,location:0,down:false}));
  }
  if(game==='th01'){
    await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));await new Promise(r=>setTimeout(r,180));await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
    await new Promise(r=>setTimeout(r,1500));
  }
  if(game==='th02'||game==='th04')await page.evaluate(()=>Promise.all([0,1].map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}))));
  if((game==='th02'||game==='th04')&&process.env.PC98_NATIVE_MARKER==='1'){
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:true}));
    const start=await frames()[1].evaluate(playerFrame);
    await wait(async()=>await frames()[1].evaluate(playerFrame)>=start+300,'native marker keeps cooperative match synchronized',30000);
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:false}));
  }
  async function measure(label){const start=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame))),at=performance.now();await new Promise(r=>setTimeout(r,5000));const end=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));const seconds=(performance.now()-at)/1000;console.log('QA player rate',label,end.map((value,i)=>((value-start[i])/seconds).toFixed(1)));}
  if(process.env.PC98_SPECTATOR_PERF==='1')await measure('before spectator');
  await configure(2,1);
  await page.waitForFunction(()=>events.some(e=>e.epoch===3&&['first-frame','error'].includes(e.event)),{timeout:120000});const viewerError=await page.evaluate(()=>events.find(e=>e.epoch===3&&e.event==='error'));if(viewerError)throw Error(JSON.stringify(viewerError));
  await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s&&!s.seeking;},{timeout:90000});
  assert(await frames()[2].evaluate(game=>{const peer=globalThis['__'+game+'PeerTransport'];return peer?.relay?.readyState===WebSocket.OPEN&&peer.peers instanceof Map;},game),'launcher can observe the live spectator relay');
  await frames()[2].waitForFunction(()=>{const c=[...document.querySelectorAll('canvas')].find(n=>n.style.position==='fixed');if(!c)return false;const context=c.getContext('2d'),pixels=context.getImageData(0,0,c.width,c.height).data;for(let i=0;i<pixels.length;i+=4)if(pixels[i]||pixels[i+1]||pixels[i+2])return true;return false;},{timeout:90000});
  if(process.env.PC98_SPECTATOR_PERF==='1'){await measure('spectator active');console.log('QA spectator',JSON.stringify(await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback)));}
  assert(await frames()[2].evaluate(()=>{const c=[...document.querySelectorAll('canvas')].find(n=>n.style.position==='fixed'),r=c.getBoundingClientRect();return document.elementFromPoint(r.left+r.width/2,r.top+r.height/2)===c;}),'spectator canvas is visible above the room');
  if(game==='th01'){
    const read=()=>frames()[2].evaluate(()=>{const c=[...document.querySelectorAll('canvas')].find(n=>n.style.position==='fixed'),p=c.getContext('2d').getImageData(0,0,c.width,c.height).data;let hash=2166136261;for(let i=0;i<p.length;i+=4)hash=Math.imul(hash^p[i]^p[i+1]<<8^p[i+2]<<16,16777619);return hash>>>0;});
    const before=await read(),state=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback);await new Promise(r=>setTimeout(r,3500));
    const after=await read(),next=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback);console.log('TH01 spectator picture',before,after,'position',state?.position,next?.position);
    assert(next.position>state.position,'TH01 spectator receives new gameplay frames');assert.notEqual(after,before,'TH01 spectator picture keeps changing');
  }
  await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.spectatorPlayback?.position>30),{timeout:120000});
  const control=async(action,frame)=>page.evaluate(async({action,frame})=>{const run=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback.run;await rpc(2,'spectator-playback',{run,action,...frame===undefined?{}:{frame}});},{action,frame});
  await control('pause');await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.paused&&!s.seeking;},{timeout:30000});
  const target=await page.evaluate(()=>Math.floor(events.findLast(e=>e.epoch===3&&e.spectatorPlayback).spectatorPlayback.position/2));
  await control('seek',target);await page.waitForFunction(target=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.paused&&!s.seeking&&s.position===target;},{timeout:90000},target);
  await control('live');await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.following&&!s.paused&&!s.seeking;},{timeout:90000});
  if(game==='th02'||game==='th03'||game==='th04'){
    await page.click('#p0');await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
    await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.event==='audio-health'&&e.effects>0&&e.effectsPeak>0.001),{timeout:20000});
    await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
  }
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  console.log('PASS PC98 real managed players and spectator',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.epoch===3&&e.spectatorPlayback).at(-1))));
}finally{if(page){console.log('QA errors',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.event==='error'))));console.log('QA tail',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.epoch===3).slice(-6))));console.log('QA relay',relayLog);console.log('QA game',JSON.stringify(await Promise.all(page.frames().filter(f=>f.url().includes('runtimeEpoch=')).map(f=>f.evaluate(()=>({state:globalThis.th03SyncState||null,replayError:globalThis.th03Replay?.error||'',replayEnd:globalThis.th03Replay?.snapshot()?.end||null}))))));}for(const c of clients)c.close();await browser?.close();await new Promise(r=>server.close(r));relay.kill();}
