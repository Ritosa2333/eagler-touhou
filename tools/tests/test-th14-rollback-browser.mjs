import assert from 'node:assert/strict';
import {mkdir,readFile,stat,writeFile} from 'node:fs/promises';
import {createReadStream} from 'node:fs';
import {createServer} from 'node:http';
import {randomBytes} from 'node:crypto';
import {extname,resolve,sep} from 'node:path';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

const option=(name,fallback)=>process.argv.find(value=>value.startsWith('--'+name+'='))?.slice(name.length+3)??fallback;
assert.ok(option('runtime')&&option('data')&&option('output'),'Use --runtime=DIR --data=TH14.DAT --output=DIR [--players=2|3] [--frames=N] [--delay=MS] [--relay=URL] [--observe] [--manual-delay]');
const runtime=resolve(option('runtime'));
const data=resolve(option('data'));
const output=resolve(option('output'));
const count=Number(option('players',2));
const maxFrame=Number(option('frames',count===2?1800:2350));
const delay=Number(option('delay',count===2?90:110));
const mode=+process.argv.includes('--manual-delay');
const watchAndReplay=process.argv.includes('--observe');
assert.ok([2,3].includes(count)&&Number.isInteger(maxFrame)&&maxFrame>=500&&Number.isInteger(delay)&&delay>=0);
await mkdir(output,{recursive:true});
const manifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
assert.equal(manifest.product,'th14mp');
const relay=option('relay','ws://127.0.0.1:18350/');
const cors={'Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Embedder-Policy':'require-corp','Cross-Origin-Resource-Policy':'same-origin'};
const mime={'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'};
const hostPage=`<!doctype html><meta charset=utf-8><style>body{margin:0}iframe{width:640px;height:480px;border:0}</style>
<iframe></iframe><script>
const frame=document.querySelector('iframe');let serial=0;window.events=[];
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await (await fetch('/retail.dat')).arrayBuffer()});
addEventListener('message',event=>{if(event.source===frame.contentWindow)events.push(event.data);});
window.command=(command,fields={})=>new Promise((resolve,reject)=>{
 const request=String(++serial);
 const timeout=setTimeout(()=>{removeEventListener('message',receive);reject(Error('command timeout: '+command));},45000);
 const receive=event=>{if(event.source!==frame.contentWindow||event.data?.request!==request)return;
  clearTimeout(timeout);removeEventListener('message',receive);event.data.ok?resolve(event.data):reject(Error(event.data.error));
 };
 addEventListener('message',receive);
 frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th14',epoch:1,command,request,...fields},location.origin);
});
frame.src='/runtime/th14.html?hosted=1&managedData=1&runtimeEpoch=1';
</script>`;
const server=createServer(async(req,res)=>{
 try{
  const pathname=new URL(req.url,'http://localhost').pathname;
  if(pathname==='/'){res.writeHead(200,{...cors,'Content-Type':'text/html'});res.end(hostPage);return;}
  const file=pathname==='/retail.dat'?data:resolve(runtime,decodeURIComponent(pathname.slice('/runtime/'.length)));
  if(pathname!=='/retail.dat'&&(!pathname.startsWith('/runtime/')||!file.startsWith(runtime+sep)))throw Error('outside runtime directory');
  const size=(await stat(file)).size;
  res.writeHead(200,{...cors,'Content-Type':mime[extname(file)]||'application/octet-stream','Content-Length':size});
  createReadStream(file).on('error',e=>res.destroy(e)).pipe(res);
 }catch{res.writeHead(404,cors);res.end();}
});
await new Promise((done,fail)=>server.listen(0,'127.0.0.1',done).once('error',fail));
const base='http://127.0.0.1:'+server.address().port+'/';
const browser=await launchBrowser();
const sockets=[];
function next(socket,match,send){
 return new Promise((resolve,reject)=>{
  const done=(fn,val)=>{clearTimeout(timer);socket.removeEventListener('message',receive);socket.removeEventListener('close',closed);fn(val);};
  const timer=setTimeout(()=>done(reject,Error('lobby timeout '+JSON.stringify(send))),8000);
  const receive=event=>{const m=JSON.parse(String(event.data));if(m.type==='error')done(reject,Error(JSON.stringify(m)));else if(match(m))done(resolve,m);};
  const closed=e=>done(reject,Error('lobby closed '+e.reason));
  socket.addEventListener('message',receive);socket.addEventListener('close',closed,{once:true});if(send)socket.send(JSON.stringify(send));
 });
}
async function makeClient(seat){
 const context=await browser.newContext({viewport:{width:800,height:650}});
 await context.addInitScript(({seat,delay})=>{
  const original=RTCDataChannel.prototype.send;
  globalThis.delayedPackets=0;
  globalThis.delayedAfter=60;
  RTCDataChannel.prototype.send=function(payload){
   if(!delay||seat===0||typeof payload==='string')return original.call(this,payload);
   const bytes=payload instanceof ArrayBuffer?new Uint8Array(payload):new Uint8Array(payload.buffer,payload.byteOffset,payload.byteLength);
   if(bytes.length<6||!globalThis.core||globalThis.core.th14_room_frame()<globalThis.delayedAfter)return original.call(this,payload);
   const copy=bytes.slice(),lag=delay+(++globalThis.delayedPackets%5)*6;
   setTimeout(()=>{if(this.readyState==='open')original.call(this,copy);},lag);
  };
  // Records each number once when the input method advances. Chosen by the
  // simulation head rather than wall time to exercise short edges repeatedly.
  const read=(ptr,n)=>ptr?Array.from(new Uint32Array(Module.HEAPU8.buffer,ptr,n)):null;
  const stores=new Map();
  globalThis.traceRoom=()=>({frames:[...stores.values()],rollback:read(core.th14_room_rollback(),6),route:globalThis.__eaglerPeerTransport?.route,
   packets:delayedPackets,error:document.querySelector('#error')?.textContent||''});
  globalThis.watchAt=(target)=>{
   const ptr=core.th14_room_live_status(),row=read(ptr,8);
   if(row&&row[0]===target){globalThis.watchRow=row;return true;}
   return false;
  };
  const raf=requestAnimationFrame;
  requestAnimationFrame=function(callback){
   return raf.call(this,t=>{
    if(globalThis.core&&globalThis.Module&&core.th14_room_rollback){
     if(seat<0&&globalThis.watchTarget&&!globalThis.watchRow)watchAt(globalThis.watchTarget);
     const status=read(core.th14_room_rollback(),6);
     if(status&&seat>=0){
      for(let n=Math.max(1,status[1]-200);n<=status[1];n++)if(!stores.has(n)){
       const row=read(core.th14_room_confirmed(n),8);if(row)stores.set(n,row);
      }
      const f=status[0],lane=(Math.floor((f+seat*23)/29)%4);
      core.th11_key(0x2c,1); // Z
      core.th11_key(0xcb,+(lane===0)); // left
      core.th11_key(0xcd,+(lane===2)); // right
      core.th11_key(0xc8,+((Math.floor(f/83)%3)===1));
      core.th11_key(0xd0,+((Math.floor(f/83)%3)===2));
      core.th11_key(0x2a,+((f+seat*3)%23<10));
      core.th11_key(0x2d,+(f>=430&&(f+seat*15)%299===0)); // X
      core.th11_key(0x1d,+(f>1040&&(f+seat*6)%89<18)); // Ctrl
     }
    }
    callback(t);
   });
  };
 },{seat,delay});
 const page=await context.newPage();
 page.on('pageerror',err=>console.log('page',seat,err.message));
 page.on('console',m=>{if(m.type()==='error'&&!m.text().includes('status of 404'))console.log('console',seat,m.text());});
 await page.goto(base);
 await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:45000});
 const frame=page.frames()[1];
 return {page,frame,cmd:(c,f={})=>page.evaluate(([c,f])=>command(c,f),[c,f])};
}
const runId='th14mp-local-'+randomBytes(5).toString('hex');
try{
 const members=[];
 const loadouts=[{character:0,shot:0},{character:2,shot:0},{character:1,shot:0}].slice(0,count);
 for(let seat=0;seat<count;seat++){
  const member=runId+'-'+seat;
  const socket=new WebSocket(relay+'?'+new URLSearchParams({room:runId,lobby:runId+'-seat-'+seat,member,intent:seat?'join':'create',players:String(count),difficulty:'1'}));
  sockets.push(socket);
  await next(socket,m=>m.type==='state');
  console.log('joined',seat);
  await next(socket,m=>m.type==='state'&&m.room.seats[seat],{type:'take-seat',seat,loadout:2*loadouts[seat].character});
  members.push({socket,member});
 }
 for(let seat=0;seat<count;seat++)await next(members[seat].socket,m=>m.type==='state'&&m.room.seats[seat]?.ready,{type:'set-ready',ready:true});
 console.log('ready');
 const start=await next(members[0].socket,m=>m.type==='start',{type:'start',adonisMode:mode,inputDelay:mode?3:0,predictionLimit:12});
 console.log('room started',start.serial,start.room.seed,start.room.adonisMode,start.room.predictionLimit);
 const clients=await Promise.all(members.map((_,seat)=>makeClient(seat)));
 const options=(member,seat)=>({
  netplayMode:'lan',netplayUrl:relay+'?'+new URLSearchParams({room:runId,run:String(start.serial),member}),
  netplayLoadouts:loadouts,netplayPlayerCount:count,netplayPlayer:seat,netplaySeed:start.room.seed??1234,
  netplayDifficulty:1,netplayAdonisMode:mode,netplayInputDelay:mode?3:0,netplayAllowWebsocketFallback:false,netplayPredictionLimit:mode?0:12,
 });
 await Promise.all(clients.map((c,i)=>c.cmd('configure',{options:options(members[i].member,i),language:'ja',music:'none'})));
 await Promise.all(clients.map(c=>c.cmd('launch')));
 console.log('launched',count,maxFrame,delay);
 const traces=async()=>Promise.all(clients.map(c=>c.frame.evaluate(()=>traceRoom())));
 let compared=0,lastTraces;
 const deadline=Date.now()+180000;
 let nextProgress=100;
 for(;;){
  assert.ok(Date.now()<deadline,'confirmed frame deadline');
  await new Promise(r=>setTimeout(r,450));
  lastTraces=await traces();
  if(lastTraces.some(t=>t.error))throw Error(JSON.stringify(lastTraces.map(({frames,...t})=>t)));
  const maps=lastTraces.map(t=>new Map(t.frames.map(row=>[row[0],row])));
  while(maps.every(m=>m.has(compared+1))){
   ++compared;
   for(let seat=1;seat<count;seat++)assert.deepEqual(maps[seat].get(compared),maps[0].get(compared),'seat '+seat+' confirmed frame '+compared);
  }
  if(compared>=nextProgress){console.log('passed',compared, JSON.stringify(lastTraces.map(({frames,...t})=>t)));nextProgress=compared+200;}
  if(compared>=maxFrame)break;
 }
 const peakBullets=Math.max(...lastTraces[0].frames.filter(f=>f[0]<=compared).map(f=>f[5]));
 assert.ok(peakBullets>0,'nonempty enemy bullet pool');
 if(delay>0&&!mode){
  assert.ok(lastTraces[0].rollback[2]>0,'host received input late enough to correct');
  assert.ok(lastTraces[0].rollback[3]>0,'host replayed corrected frames');
  assert.ok(lastTraces[0].rollback[4]>0,'correction happened with a live enemy projectile');
  assert.ok(lastTraces.slice(1).every(t=>t.packets>0),'every guest delayed packets');
 }
 if(mode)assert.ok(lastTraces.every(t=>t.rollback[2]===0),'manual delay never speculates');
 assert.ok(lastTraces.every(t=>t.route==='rtc'),'every seat uses WebRTC');
 let viewers;
 if(watchAndReplay){
  const target=500,expected=lastTraces[0].frames.find(f=>f[0]===target);
  const member=runId+'-watch',spectatorId='spectator-local-01';
  const lobby=new WebSocket(relay+'?'+new URLSearchParams({room:runId,lobby:spectatorId,member,intent:'join'}));sockets.push(lobby);
  await next(lobby,m=>m.type==='state');
  await next(lobby,m=>m.type==='spectator-start',{type:'spectate',name:'Watcher'});
  const watcher=await makeClient(-1);
  await watcher.frame.evaluate(target=>{watchTarget=target;},target);
  await watcher.cmd('configure',{options:{...options(member,0),netplaySpectator:true,netplaySpectatorId:spectatorId},language:'ja',music:'none'});
  await watcher.cmd('launch');
  await watcher.frame.waitForFunction(()=>!!globalThis.watchRow,null,{timeout:60000});
  assert.deepEqual(await watcher.frame.evaluate(()=>watchRow),expected,'spectator replays the same confirmed battle');
  console.log('spectator matched frame',target);
  await clients[0].cmd('sync');
  const files=(await clients[0].cmd('list')).files;
  const path=files.find(f=>f.path.startsWith('replay/th14mp_'))?.path;assert.ok(path);
  const recording=await clients[0].cmd('read',{path});
  const replay=await makeClient(-2);
  await replay.frame.evaluate(target=>{watchTarget=target;},target);
  await replay.cmd('configure',{options:{replayViewer:true},language:'ja',music:'none'});
  await replay.cmd('write',{path,bytes:recording.bytes});
  await replay.cmd('launch');
  await replay.cmd('playback',{action:'open',path});
  await replay.frame.waitForFunction(()=>!!globalThis.watchRow,null,{timeout:60000});
  assert.deepEqual(await replay.frame.evaluate(()=>watchRow),expected,'disk replay reproduces the confirmed battle');
  console.log('disk replay matched frame',target);
  viewers={spectatorFrame:target,replayFrame:target};
 }
 const report={version:manifest.version,wasm:manifest.execution.sha256,mode:mode?'manual-delay':'rollback',count,
  delay,jitter:24,compared,peakBullets,viewers,peers:lastTraces.map(({frames,rollback,...peer})=>{
   const [head,confirmed,corrections,resimulated,combatCorrections,prediction]=rollback;
   return {head,confirmed,corrections,resimulated,combatCorrections,prediction,...peer};
  })};
 await writeFile(resolve(output,'th14-'+report.mode+'-'+count+'p.json'),JSON.stringify(report,null,2)+'\n');
 console.log('verified',JSON.stringify(report));
}finally{sockets.forEach(s=>s.close());await browser.close();server.close();}
