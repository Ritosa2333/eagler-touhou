// Two or three real PC-98 player runtimes and a local Relay.
// Reports native gameplay, actual page displays and frame intervals; does not assert machine-specific FPS.
// Run from launcher with ROOM_RUNTIME_GAME, ROOM_RUNTIME_ROOT and
// ROOM_RUNTIME_DATA pointing to a built TH02 or TH04 runtime and data file.
// Set EAGLER_NETPLAY_RELAY_DELAY_MS / EAGLER_NETPLAY_RELAY_JITTER_MS in the
// test environment to simulate a slower link. Set PC98_FPS_ROLLBACK=0 for
// a confirmed-lockstep comparison. All three players
// share this machine, so CPU contention differs from three separate devices.
// The benchmark explicitly samples the worker's native game counter each second.
// PC98_FPS_MIN can optionally make any lower one-second sample fail the run.
// PC98_FPS_SECONDS sets the sampling duration (default 20 seconds).
// PC98_FPS_PLAYERS selects 2 or 3 players (default 3).
// PC98_FPS_TRANSPORT=rtc requires actual WebRTC, bypassing the relay delay.
// PC98_FPS_RTC_DROP_EVERY drops every Nth fast-lane send, not reliable control.
// PC98_FPS_PROFILE=1 adds TH04 worker phase counters to match TH02's profile.
// PC98_FPS_SINGLE_TICK_CHECKPOINT=1 tests exact per-tick DOSBox checkpoints.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th02','th04'].includes(game),'ROOM_RUNTIME_GAME must be th02 or th04');
const playerCount=Number(process.env.PC98_FPS_PLAYERS??3);
assert([2,3].includes(playerCount),'PC98_FPS_PLAYERS must be 2 or 3');
const sampleSeconds=Number(process.env.PC98_FPS_SECONDS??20);
assert(Number.isInteger(sampleSeconds)&&sampleSeconds>=1&&sampleSeconds<=300,'invalid PC98_FPS_SECONDS');
const route=process.env.PC98_FPS_TRANSPORT??'relay';
assert(['rtc','relay'].includes(route),'invalid PC98_FPS_TRANSPORT');
const rtcDropEvery=Number(process.env.PC98_FPS_RTC_DROP_EVERY??0);
assert(Number.isInteger(rtcDropEvery)&&(rtcDropEvery===0||route==='rtc'&&rtcDropEvery>=2),'invalid PC98_FPS_RTC_DROP_EVERY');
const timing={...multiplayerInputTimingPolicy(PRODUCT_GAMES[game].multiplayer),rollback:process.env.PC98_FPS_ROLLBACK!=='0'};
const singleTickCheckpoint=process.env.PC98_FPS_SINGLE_TICK_CHECKPOINT==='1';
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),data=await readFile(process.env.ROOM_RUNTIME_DATA);
const roomSource=await readFile(resolve(runtime,'netplay/room.js'),'utf8');
assert(roomSource.includes('export const managedAPI={'),'managed room instrumentation boundary');
const instrumentedRoom=roomSource.replace('export const managedAPI={','globalThis.__pc98TestRuntime=()=>runtime;\nglobalThis.__pc98TestTiming=()=>({inputAge,delay:queue.active?queue.inputDelay:queue.lockstepDelay,rollbackCount,resimulated,maxRewind});\nexport const managedAPI={');
const overrides=new Map([['netplay/room.js',instrumentedRoom]]);
const profileEnabled=game==='th04'&&process.env.PC98_FPS_PROFILE==='1';
function replace(source,marker,code){assert.equal(source.split(marker).length,2,'PC-98 benchmark marker: '+marker);return source.replace(marker,code);}
if(profileEnabled){
  const workerPath='netplay/runtime-worker.js',nativePath='netplay/runtime.js';
  let worker=await readFile(resolve(runtime,workerPath),'utf8'),native=await readFile(resolve(runtime,nativePath),'utf8');
  worker=replace(worker,'let chain=Promise.resolve();',`const profile={};
const phase=(name,fn)=>{const at=performance.now();try{return fn();}finally{const item=profile[name]??={count:0,ms:0,max:0};const elapsed=performance.now()-at;item.count++;item.ms+=elapsed;item.max=Math.max(item.max,elapsed);}};
globalThis.__pc98NativeStep=fn=>phase('native',fn);
let instrumented=false,chain=Promise.resolve();`);
  worker=replace(worker,'const r=th04WorkerHost.runtime,started=performance.now();let result;',`const r=th04WorkerHost.runtime,started=performance.now();let result;
  if(!instrumented){instrumented=true;for(const name of ['step','capture','restore','confirm','checksum','present']){const original=r[name];r[name]=(...args)=>phase(name,()=>original(...args));}}`);
  worker=replace(worker,'th04WorkerHost.canvas.transferToImageBitmap()',"phase('bitmap',()=>th04WorkerHost.canvas.transferToImageBitmap())");
  worker=replace(worker,'render:r.renderInfo()', 'render:{...r.renderInfo(),profile}');
  native=replace(native,'try{emulator.step();}catch(error){challenge?.abort();throw error;}', 'try{globalThis.__pc98NativeStep(()=>emulator.step());}catch(error){challenge?.abort();throw error;}');
  overrides.set(workerPath,worker);overrides.set(nativePath,native);
}
if(singleTickCheckpoint){
  const path='netplay/runtime.js',source=overrides.get(path)??await readFile(resolve(runtime,path),'utf8');
  const capture=game==='th02'?'const previous=states.get(frame-1),nativeFrame':'const previous=rollbackStates.get(frame-1);\n  const nativeFrame';
  const retain=game==='th02'?'const previous=states.get(prefix-1);':'const previous=rollbackStates.get(prefix-1);\n  ';
  const exact=replace(source.replaceAll('\r\n','\n'),capture+'=previous?.nativeFrame===frame-1?frame-1:frame','const nativeFrame=frame');
  overrides.set(path,replace(replace(exact,retain+'return previous?.nativeFrame===prefix-1?prefix-1:prefix;','return prefix;'),'checkpointFrames:2','checkpointFrames:1'));
}
let relayLog='',browser,page;
const clients=[],errors=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
  cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:route==='relay'?'1':'0',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},stdio:['ignore','pipe','pipe']});
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
  res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream');
  if(path==='/runtime/netplay/runtime.json'&&game==='th02'){
    const manifest=JSON.parse(await readFile(file,'utf8'));
    for(const [asset,source] of overrides)if(Object.hasOwn(manifest.assets,asset))manifest.assets[asset]=createHash('sha256').update(source).digest('hex');
    res.end(JSON.stringify(manifest));return;
  }
  const source=overrides.get(path.slice(9));
  if(source!==undefined){res.end(source);return;}
  res.end(await readFile(file));
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
  for(let actor=0;actor<playerCount;actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:String(playerCount),difficulty:'1'}));clients.push(client);
    client.addEventListener('message',e=>{const message=JSON.parse(e.data);if(message.room)client.retentionRoom=message.room;});
    await send(client,null,m=>m.type==='state');
    await send(client,{type:'take-seat',seat:actor,loadout:0},m=>!!m.room?.seats[actor]);
  }
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();if(route==='relay')await page.evaluateOnNewDocument(()=>Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true}));
  if(rtcDropEvery)await page.evaluateOnNewDocument(every=>{
    const send=RTCDataChannel.prototype.send,counts=new WeakMap();globalThis.__pc98DroppedFastPackets=0;
    RTCDataChannel.prototype.send=function(data){
      if(this.label.endsWith('-fast')&&this.readyState==='open'){
        const count=(counts.get(this)??0)+1;counts.set(this,count);
        if(count%every===0){globalThis.__pc98DroppedFastPackets++;return;}
      }
      return send.call(this,data);
    };
  },rtcDropEvery);
  page.on('pageerror',e=>errors.push(e.message));
  page.on('dialog',dialog=>{errors.push(dialog.message());void dialog.dismiss();});
  page.on('console',m=>console.log('Runtime:',m.text()));
  await page.setViewport({width:1920,height:540});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(count=>{for(let n=0;n<count;n++)addRuntime(n);},playerCount);
  await page.waitForFunction(count=>events.filter(e=>e.event==='ready').length===count,{timeout:120000},playerCount);
  const frames=()=>Array.from({length:playerCount},(_,i)=>i+1).map(epoch=>page.frames().find(f=>new URL(f.url()).searchParams.get('runtimeEpoch')===String(epoch)));
  for(let actor=0;actor<playerCount;actor++)await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[actor]?.ready);
  const start=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  const run=start.serial;
  const options=actor=>({touchEnabled:process.env.PC98_TOUCH_INPUT==='1',touchMovementMode:'touch-unlimited',touchSensitivity:150,netplayMode:'lan',netplayChallengeMode:process.env.PC98_TEST_CHALLENGE==='1',netplayDisableFocus:process.env.PC98_TEST_DISABLE_FOCUS==='1',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(run),member:'retention_member_'+actor,player:String(actor)}),netplayPlayer:actor,netplayPlayerCount:playerCount,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:0,shot:1},{character:game==='th02'?0:1,shot:game==='th02'?2:0}].slice(0,playerCount),netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:timing.rollback?0:1,netplayPredictionLimit:timing.predictionLimit,netplaySpectator:false,netplaySpectatorCount:0,netplaySpectatorTimeline:true});
  const configure=async(actor,serial)=>{
    await page.evaluate(async({actor,options})=>rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]}),{actor,options:options(actor)});
    // Give the running native title several ticks between protocol commands.
    // Configuration must not initialize or connect the upcoming run early.
    await new Promise(r=>setTimeout(r,500));
    await page.evaluate(({actor,serial})=>rpc(actor,'launch',{launchSerial:serial}),{actor,serial});
  };
  await Promise.all(Array.from({length:playerCount},(_,i)=>configure(i,1)));
  await page.evaluate(count=>Promise.all(Array.from({length:count},(_,n)=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}))),playerCount);
  await wait(async()=>{const values=await Promise.all(frames().slice(0,3).map(f=>f.evaluate(playerFrame)));return values.every(v=>v>120);},'native players advancing');
  await wait(async()=>{const values=await Promise.all(frames().map(f=>f.evaluate(playerFrame)));return values.every(v=>v>=300);},'warmup',120000);
  assert.deepEqual(await Promise.all(frames().map(f=>f.evaluate(()=>globalThis.__eaglerNetplayTransport))),Array(playerCount).fill(route),'unexpected gameplay transport');
  const sample=(includeTape=false)=>Promise.all(frames().map(f=>f.evaluate(async({game,includeTape})=>{const {managedAPI}=await import('./netplay/room.js');const recorder=globalThis[game+'Replay'],health=managedAPI.health();const native=await managedAPI.sampleGame();if(!native||!Number.isInteger(native.gameFrame))throw Error('native game counter unavailable');health.gameFrame=native.gameFrame;return {health,performance:globalThis.__pc98TestRuntime?.()?.performanceInfo?.()??null,render:globalThis.__pc98TestRuntime?.()?.renderInfo?.()??null,pointError:recorder?.pointError||'',snapshot:includeTape?recorder?.snapshot():null,diagnostics:includeTape?document.getElementById('diagnostics')?.textContent||'':''};},{game,includeTape})));
  const begin=await sample(),at=performance.now();
  const directions=setInterval(()=>void page.evaluate(count=>Promise.all(Array.from({length:count},(_,n)=>rpc(n,'keyboard',{code:'ArrowLeft',key:'ArrowLeft',keyCode:37,location:0,down:Math.floor(Date.now()/300)%2===0}))),playerCount).catch(()=>{}),300);
  const windows=[];let prior=begin,priorAt=at;
  try{for(let n=0;n<sampleSeconds;n++){await new Promise(r=>setTimeout(r,1000));const next=await sample(),now=performance.now(),seconds=(now-priorAt)/1000;windows.push(next.map((x,i)=>(x.health.gameFrame-prior[i].health.gameFrame)/seconds));prior=next;priorAt=now;}}finally{clearInterval(directions);}
  const elapsed=(priorAt-at)/1000,finalTape=await sample(true),end=prior;for(let i=0;i<end.length;i++){end[i].snapshot=finalTape[i].snapshot;end[i].pointError=finalTape[i].pointError;end[i].diagnostics=finalTape[i].diagnostics;}
  assert.deepEqual(errors,[]);const boundary=Math.floor(Math.min(...end.map(x=>x.snapshot.rows.length))/120)*120;
  if(profileEnabled)assert(end.every((x,i)=>x.render?.profile?.native.count>begin[i].render?.profile?.native.count&&x.render.profile.bitmap.count>begin[i].render.profile.bitmap.count),'TH04 phase counters did not advance');
  if(singleTickCheckpoint)assert(end.every(x=>x.diagnostics.includes('检查点每 1 步')&&x.diagnostics.includes('边界重建 0 步')),'per-tick checkpoint was not active');
  assert(end.every((x,i)=>x.health.running&&!x.health.stopped&&x.health.frame>begin[i].health.frame),'a player stopped advancing');
  assert(boundary>=120,'no common confirmed hash');
  const firstSampleBoundary=(Math.floor(Math.max(...begin.map(x=>x.health.frame))/120)+1)*120;
  if(sampleSeconds>=5)assert(boundary>=firstSampleBoundary,'no common confirmed hash in the sampling window');
  const firstBoundary=Math.min(boundary,firstSampleBoundary);
  assert(end.every(x=>!x.pointError),'checkpoint builder error');
  let hashes,hashBoundaries=0;
  for(let tick=firstBoundary;tick<=boundary;tick+=120){
    hashes=end.map(x=>x.snapshot.rows[tick-1]?.hash);
    assert(hashes.every(h=>h&&h.tick===tick&&typeof h.hash==='string'),`confirmed hash missing at tick ${tick}`);
    for(const hash of hashes)assert.deepEqual(hash,hashes[0],`confirmed hashes differ at tick ${tick}`);
    hashBoundaries++;
  }
  const intervals=values=>{if(!values?.length)return null;const sorted=values.slice().sort((a,b)=>a-b),percentile=p=>sorted[Math.floor((sorted.length-1)*p)];return {count:sorted.length,p50Ms:percentile(.5),p95Ms:percentile(.95),p99Ms:percentile(.99),maxMs:sorted.at(-1)};};
  const timings=await Promise.all(frames().map(f=>f.evaluate(()=>globalThis.__pc98TestTiming())));
  const droppedFastPackets=rtcDropEvery?await Promise.all(frames().map(f=>f.evaluate(()=>globalThis.__pc98DroppedFastPackets))):[];
  assert(droppedFastPackets.every(count=>count>0),'no fast-lane packet loss was exercised');
  console.log('RESULT',JSON.stringify({playerCount,route,singleTickCheckpoint,elapsed,boundary,hashes,hashBoundaries,droppedFastPackets,timings,points:(finalTape[0].snapshot.points||[]).map(p=>({frame:p.frame,base64Bytes:p.data.length})),windows,diagnostics:end.map(x=>x.diagnostics),clients:begin.map((b,i)=>{
    const finish=end[i],displayedStart=b.render?.pageDrawn??b.performance?.page?.displayed,displayedEnd=finish.render?.pageDrawn??finish.performance?.page?.displayed;
    return {begin:b.health,end:finish.health,performanceBegin:b.performance,performanceEnd:finish.performance,workerProfileBegin:b.render?.profile,workerProfileEnd:finish.render?.profile,
      pageIntervals:intervals(finish.render?.pageIntervals),replacedStart:b.render?.replaced??b.performance?.page?.replaced,replacedEnd:finish.render?.replaced??finish.performance?.page?.replaced,
      gameFps:(finish.health.gameFrame-b.health.gameFrame)/elapsed,syncFps:(finish.health.frame-b.health.frame)/elapsed,
      submittedFps:(finish.health.frames-b.health.frames)/elapsed,displayFps:Number.isFinite(displayedStart)&&Number.isFinite(displayedEnd)?(displayedEnd-displayedStart)/elapsed:null};
  })}));
  if(process.env.PC98_FPS_MIN!==undefined){const min=Number(process.env.PC98_FPS_MIN);assert(Number.isFinite(min)&&min>=0,'invalid PC98_FPS_MIN');assert(windows.every(window=>window.every(fps=>fps>=min)),`one-second game frame rate fell below ${min}`);}
}finally{for(const c of clients)c.close();await browser?.close();await new Promise(r=>server.close(r));relay.kill();}
