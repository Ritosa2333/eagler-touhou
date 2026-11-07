import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {resolve,extname,sep} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),repo=resolve(root,'../..'),launcher=resolve(repo,'launcher');
const argument=(name,fallback)=>process.argv.find(v=>v.startsWith('--'+name+'='))?.slice(name.length+3)??fallback;
const build=resolve(argument('build',resolve(root,'web/artifacts/rollback-performance')));
const singleBuild=resolve(argument('single-build',resolve(repo,'ports/th15/web/artifacts/input-latency')));
const runtime=resolve(root,'build-eagler-multiplayer');
const bridge=resolve(argument('bridge',resolve(root,'web/sdl-runtime/multiplayer.mjs')));
const bridgeSource=await readFile(bridge);
const out=resolve(argument('output',resolve(repo,'artifacts/th15mp-input-latency')));
const samples=Number(argument('samples','60')),label=argument('label','candidate'),recoveryOnly=process.argv.includes('--recovery-only');
assert.ok(Number.isInteger(samples)&&samples>=20);
for(const name of ['TH15_TEST_ARCHIVE','TH15_TEST_FONT','TH15_TEST_UNICODE_FONT'])assert.ok(process.env[name],'Set '+name);
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
const fonts=new Map([['msgothic.ttc',process.env.TH15_TEST_FONT],['unifont.otf',process.env.TH15_TEST_UNICODE_FONT]]);
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const sha=b=>createHash('sha256').update(b).digest('hex');
const delay=ms=>new Promise(r=>setTimeout(r,ms));
const stats=a=>{a.sort((a,b)=>a-b);return a.length?{count:a.length,p50:a[Math.floor(a.length*.5)],p95:a[Math.floor(a.length*.95)],p99:a[Math.floor(a.length*.99)],max:a.at(-1)}:{count:0};};
await mkdir(out,{recursive:true});
const report={label,samples,archive:sha(archive),driver:sha(await readFile(import.meta.filename)),bridge:sha(bridgeSource),build:JSON.parse(await readFile(resolve(build,'build.json'),'utf8')),singleBuild:JSON.parse(await readFile(resolve(singleBuild,'build.json'),'utf8')),cases:[]};
const html=`<!doctype html><style>body{margin:0;display:flex}iframe{width:640px;height:480px;flex-shrink:0;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.mount=(seat,single=false)=>{const f=document.createElement('iframe');f.id='p'+seat;f.src=(single?'/single/':'/runtime/')+'th15.html?managedData=1&hosted=1&runtimeEpoch='+(seat+1);document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const request=crypto.randomUUID(),f=document.querySelector('#p'+seat);const timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timed out'));},60000);function receive(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:seat+1,request,command,...fields},location.origin);});
</script>`;
const server=createServer(async(req,res)=>{try{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(path==='/data'){res.end(archive);return;}
 if(path==='/favicon.ico'){res.writeHead(204).end();return;}
 if(path.startsWith('/shared/')){const font=fonts.get(path.slice(8));assert.ok(font);res.end(await readFile(font));return;}
 const single=path.startsWith('/single/');assert.ok(single||path.startsWith('/runtime/'));
 const name=decodeURIComponent(path.slice(single?8:9));let file=resolve(runtime,name);assert.ok(file.startsWith(runtime+sep));
 if(name==='th15.mjs')file=resolve(single?singleBuild:build,'th15-application.mjs');
 if(name==='th15-application.wasm')file=resolve(single?singleBuild:build,name);
 if(name==='multiplayer.mjs')file=bridge;
 let bytes;
 if(single&&name==='shell.mjs'){
  const shell=await readFile(resolve(repo,'ports/th15/web/sdl-runtime/shell.mjs'),'utf8');
  bytes=shell.replace("/*TH15_BUILD_INFO*/{version:'development-incomplete',completeGame:false}",JSON.stringify({version:'input-latency-control',completeGame:false,multiplayer:false}));
 }else bytes=name==='multiplayer.mjs'?bridgeSource:await readFile(file);
 res.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html','.css':'text/css'})[extname(name)]||'application/octet-stream');res.end(bytes);
}catch(e){res.writeHead(404).end(String(e));}});
let browser,relay,relayLog='';const sockets=[];
const listen=await new Promise(r=>server.listen(0,'127.0.0.1',()=>r(server.address().port)));
const base='http://127.0.0.1:'+listen;
function message(socket,predicate){return new Promise((yes,no)=>{
 const timer=setTimeout(()=>{cleanup();no(Error('Lobby timed out'));},10000);
 const receive=e=>{const m=JSON.parse(String(e.data));if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();yes(m);}};
 function cleanup(){clearTimeout(timer);socket.removeEventListener('message',receive);}
 socket.addEventListener('message',receive);
});}
async function send(socket,body,predicate){const reply=message(socket,predicate);socket.send(JSON.stringify(body));return reply;}
async function pageFor(count){
 const context=await browser.createBrowserContext(),page=await context.newPage();await page.setViewport({width:640*count,height:480});
 await page.evaluateOnNewDocument(()=>{addEventListener('keydown',e=>{if(['ArrowLeft','ArrowRight'].includes(e.code))globalThis.lastPhysicalKey={code:e.code,at:e.timeStamp,dispatchedAt:performance.now()};},true);});
 await page.goto(base);return {context,page,frame:seat=>page.frames().find(f=>f.url().includes('runtimeEpoch='+(seat+1)))};
}
async function launch(page,seat,options,single=false){
 await page.evaluate(({seat,single})=>mount(seat,single),{seat,single});
 await page.waitForFunction(seat=>ready.has(seat+1),{timeout:60000},seat);
 await page.evaluate(async({seat,options})=>{await rpc(seat,'configure',{music:'none',sharedResources:['msgothic.ttc','unifont.otf'].map(name=>({path:'/'+name,url:'/shared/'+name})),options});await rpc(seat,'launch');},{seat,options});
}
async function measure(page,frame,name){
 const start=await frame.evaluate(()=>Module._th15_mp_status?Module.HEAPU32[(Module._th15_mp_status()>>>2)+3]:Module._th15_frame());
 await frame.waitForFunction(start=>Module._th15_probe_loading_pending()===0&&Module._th15_probe_loading_remaining()===0&&
  (Module._th15_mp_status?Module.HEAPU32[(Module._th15_mp_status()>>>2)+3]:Module._th15_frame())>=start+8,{timeout:30000},start);
 await frame.evaluate(()=>{
  const c=Module;globalThis.latencyEvents=[];globalThis.latencyRows=[];globalThis.activeLatencyKey=null;
  globalThis.motion=()=>Array.from(new Float32Array(c.HEAPU8.buffer,c._th15_probe_player_state(),10));
  globalThis.logicFrame=()=>c._th15_mp_status?c.HEAPU32[(c._th15_mp_status()>>>2)+3]:c._th15_frame();
  if(!globalThis.latencyInstalled){
   const key=c._th15_key.bind(c);c._th15_key=(scan,down)=>{if((scan===203||scan===205)&&down){const e={nativeAt:performance.now(),domAt:lastPhysicalKey?.at,frame:logicFrame(),scan,player:motion()};latencyEvents.push(e);activeLatencyKey=e;}return key(scan,down);};
   const old=c.onGameFrame;c.onGameFrame=(...args)=>{old?.(...args);const p=motion(),f=logicFrame(),at=performance.now();latencyRows.push({at,frame:f,workMs:args[1],ticks:args[2]});const e=activeLatencyKey;if(e&&!e.response&&f>e.frame&&(e.scan===205?p[0]-e.player[0]:e.player[0]-p[0])>.01)e.response={at,frame:f,x:p[0]};};
   globalThis.latencyInstalled=true;
  }
  c._th15_probe_protection(100000);
 });
 await (await frame.$('canvas')).click();
 for(let n=0;n<samples;n++){const code=n%2?'ArrowLeft':'ArrowRight';await page.keyboard.down(code);await delay(120);await page.keyboard.up(code);await delay(70);}
 await delay(200);
 const data=await frame.evaluate(()=>({events:latencyEvents,rows:latencyRows,diagnostics:globalThis.__eaglerNetplayDiagnostics,rollback:globalThis.__eaglerNetplayRollback,player:motion(),loading:[Module._th15_probe_loading_pending(),Module._th15_probe_loading_remaining()],error:document.querySelector('#error').textContent}));
 const observed=data.events.filter(e=>e.response),result={name,missing:data.events.length-observed.length,domToMotionMs:stats(observed.map(e=>e.response.at-e.domAt)),nativeToMotionMs:stats(observed.map(e=>e.response.at-e.nativeAt)),forwardFrames:stats(observed.map(e=>e.response.frame-e.frame)),...data};
 report.cases.push(result);await writeFile(resolve(out,label+'.json'),JSON.stringify(report,null,2));
 assert.equal(data.error,'');assert.equal(data.events.length,samples);assert.equal(result.missing,0,name+': movement input did not respond');
 console.log(JSON.stringify({name,domToMotionMs:result.domToMotionMs,forwardFrames:result.forwardFrames,rollback:data.rollback}));
}
async function checkRetry(frame){
 const blocked=await frame.evaluate(()=>{
  const c=Module,t=__eaglerPeerTransport;c._th15_loop_stop();globalThis.heldPackets=[];globalThis.receiveOriginal=t.receiveBinary.bind(t);t.receiveBinary=data=>heldPackets.push(data);
  globalThis.rafOriginal=requestAnimationFrame;globalThis.rafCallbacks=[];globalThis.rafOrigin=performance.now();globalThis.rafTime=0;
  requestAnimationFrame=callback=>(rafCallbacks.push(callback),rafCallbacks.length);
  globalThis.drive=ms=>{rafTime=ms;for(const cb of rafCallbacks.splice(0))cb(rafOrigin+ms);};
  globalThis.snapshot=()=>{const p=c._th15_mp_status()>>>2;return {frame:c.HEAPU32[p+3],confirmed:c.HEAPU32[p+4]};};
  c._th15_loop_start();drive(0);let before=snapshot();
  for(let n=1;n<=48;n++){drive(n*1000/60);const after=snapshot();if(after.frame===before.frame){globalThis.blockedTime=rafTime;return after;}before=after;}
  throw Error('Missing input failed to block');
 });
 await delay(250);
 const result=await frame.evaluate(async()=>{
  const c=Module,t=__eaglerPeerTransport;t.receiveBinary=receiveOriginal;const packets=heldPackets.splice(0);for(const data of packets)receiveOriginal(data);
  if(!c._th15_mp_pump())throw Error('Pump failed');const before=snapshot();await new Promise(r=>setTimeout(r,20));const withoutRaf=snapshot();
  drive(blockedTime+1000/120);const half=snapshot();drive(blockedTime+1000/60);const full=snapshot();
  const pending=[];const timerOriginal=setTimeout;globalThis.setTimeout=(fn,ms,...args)=>ms===0?(pending.push(()=>fn(...args)),999999):timerOriginal(fn,ms,...args);
  drive(blockedTime+1000/60+1000/15);c._th15_loop_stop();const stopped=snapshot();globalThis.setTimeout=timerOriginal;for(const fn of pending)fn();
  const afterCancelled=snapshot();requestAnimationFrame=rafOriginal;
  return {hasWake:typeof c._th15_mp_wake==='function',packets:packets.length,before,withoutRaf,half,full,cancelledTasks:pending.length,stopped,afterCancelled};
 });
 assert.ok(result.packets);assert.deepEqual(result.stopped,result.afterCancelled,'Old continuation advanced a stopped loop');
 if(result.hasWake){assert.ok(result.cancelledTasks,'Cancellation fixture did not queue work');assert.equal(result.withoutRaf.frame,blocked.frame+1,'Arrival did not retry the owed frame');assert.equal(result.half.frame,result.withoutRaf.frame,'Arrival invented new clock credit');}
 else assert.equal(result.half.frame,blocked.frame,'Baseline unexpectedly retried before its next tick');
 report.retry={blocked,...result};console.log('retry '+JSON.stringify(report.retry));
}
async function checkRecovery(players){
 const observations=[];
 for(const delayed of [false,true]){
  const fixture=await pageFor(1);
  try{
   await launch(fixture.page,0,{replayViewer:true,thpracEnabled:false,renderScale:2});
   const data=await fixture.frame(0).evaluate(async({players,delayed})=>{
    const c=Module;if(!c._th15_probe_rollback_receive)return null;
    c._th15_loop_stop();if(!c._th15_probe_rollback_prepare(players,1,0))throw Error('Recovery setup failed');
    for(let n=0;n<240;n++){const p=c._th15_probe_rollback_work(0,1);if(!p||!new Float64Array(c.HEAPU8.buffer,p,15)[11])throw Error('Recovery warmup failed');}
    const clock=()=>Array.from(new Uint32Array(c.HEAPU8.buffer,c._th15_probe_rollback_clock(),4));
    const original=requestAnimationFrame,callbacks=[],origin=performance.now();requestAnimationFrame=cb=>(callbacks.push(cb),callbacks.length);
    const drive=ms=>{for(const cb of callbacks.splice(0))cb(origin+ms);};
    c._th15_loop_start();c._th15_key(44,1);drive(0);
    for(let n=1;n<=7;n++){if(!delayed&&!c._th15_probe_rollback_receive(clock()[0],1))throw Error('Oracle input rejected');drive(n*1000/60);}
    const before=clock();
    if(delayed){if(!c._th15_probe_rollback_receive(before[0]-1,1))throw Error('Late input rejected');c._th15_mp_wake();await new Promise(r=>setTimeout(r,20));}
    else drive(7*1000/60+1);
    const after=clock(),p=c._th15_probe_rollback_fingerprint();
    const fingerprint=Array.from(new Uint32Array(c.HEAPU8.buffer,p,2));
    if(!c._th15_mp_replay_save())throw Error('Confirmed input save failed');
    const bytes=Array.from(c.FS.readFile('/save/replay/th15mp_01.rpy'));
    c._th15_loop_stop();requestAnimationFrame=original;
    return {before,after,fingerprint,bytes};
   },{players,delayed});
   if(data===null)return;
   const bytes=Buffer.from(data.bytes),offset=40+bytes.readUInt32LE(24)+bytes.readUInt32LE(28)*8;
   delete data.bytes;data.inputs=sha(bytes.subarray(offset));
   if(delayed){assert.equal(data.after[0],data.before[0],'History-only recovery advanced a new frame');assert.equal(data.after[1],data.before[1],'History-only recovery sampled new physical input');}
   observations.push(data);
  }finally{await fixture.context.close();}
 }
 assert.deepEqual(observations[1].after.slice(0,3),observations[0].after.slice(0,3));
 assert.deepEqual(observations[1].fingerprint,observations[0].fingerprint,'Recovered world differs from actual-input execution');
 assert.equal(observations[1].inputs,observations[0].inputs,'Recovery changed confirmed input history');
 (report.recovery??=[]).push({players,observations});console.log('recovery '+JSON.stringify({players,observations}));
}
async function checkPartialRecovery(){
 const fixture=await pageFor(1);
 try{
  await launch(fixture.page,0,{replayViewer:true,thpracEnabled:false,renderScale:2});
  const result=await fixture.frame(0).evaluate(async()=>{
   const c=Module;if(!c._th15_probe_rollback_input)return null;
   c._th15_loop_stop();if(!c._th15_probe_rollback_prepare(3,1,0))throw Error('Partial recovery setup failed');
   for(let n=0;n<240;n++){const p=c._th15_probe_rollback_work(0,1);if(!p||!new Float64Array(c.HEAPU8.buffer,p,15)[11])throw Error('Partial recovery warmup failed');}
   const clock=()=>Array.from(new Uint32Array(c.HEAPU8.buffer,c._th15_probe_rollback_clock(),4));
   const original=requestAnimationFrame,callbacks=[],origin=performance.now();requestAnimationFrame=cb=>(callbacks.push(cb),callbacks.length);
   const drive=ms=>{for(const cb of callbacks.splice(0))cb(origin+ms);};
   c._th15_loop_start();c._th15_key(44,1);drive(0);for(let n=1;n<=7;n++)drive(n*1000/60);
   const before=clock(),first=before[0]-7;
   if(!c._th15_probe_rollback_input(1,first,0x8001))throw Error('Late pause input rejected');
   c._th15_mp_wake();await new Promise(r=>setTimeout(r,20));const partial=clock();
   drive(8*1000/60);const pending=clock();
   for(let frame=first;frame<=before[0];frame++)for(let seat=1;seat<3;seat++){
    const buttons=seat===1&&frame===first?0x8001:1;
    if(!c._th15_probe_rollback_input(seat,frame,buttons))throw Error('Remaining input rejected');
   }
   c._th15_mp_wake();await new Promise(r=>setTimeout(r,50));const after=clock();
   c._th15_loop_stop();requestAnimationFrame=original;
   return {before,partial,pending,after,first};
  });
  if(result===null)return;
  assert.equal(result.partial[0],result.first,'Late pause did not interrupt history replay');
  assert.equal(result.partial[1],result.before[1],'Partial recovery sampled new physical input');
  assert.deepEqual(result.pending,result.partial,'A missing peer input failed to preserve the pending frame');
  assert.equal(result.after[0],result.before[0]+1,'History recovery consumed the owed fresh tick');
  assert.equal(result.after[1],result.before[1]+1,'Fresh input was not captured exactly once');
  report.partialRecovery=result;console.log('partialRecovery '+JSON.stringify(result));
 }finally{await fixture.context.close();}
}
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--disable-features=WebRtcHideLocalIpsWithMdns','--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 report.browser=await browser.version();
 if(!recoveryOnly){
 const solo=await pageFor(1);
 try{
  await launch(solo.page,0,{thpracEnabled:false,renderScale:2},true);
  await solo.frame(0).evaluate(()=>{
   const c=Module;c._th15_loop_stop();const state=()=>Array.from(c.HEAP32.subarray(c._th15_probe_state()/4,c._th15_probe_state()/4+16));
   const tick=()=>{if(!c._th15_probe_tick())throw Error('Offline tick failed');};const key=()=>{c._th15_key(28,1);tick();c._th15_key(28,0);tick();};
   const menu=screen=>{for(let n=0;n<350;n++){tick();if(state()[8]===screen&&state()[9]===2)return;}throw Error('Offline menu failed');};
   menu(1);key();menu(5);key();menu(6);key();menu(7);key();for(let n=0;n<250&&c._th15_phase()!==1;n++)tick();for(let n=0;n<245;n++)tick();c._th15_loop_start();
  });
  await delay(300);await measure(solo.page,solo.frame(0),'single');
 }finally{await solo.context.close();}
 relay=spawn(process.execPath,[resolve(repo,'services/relay/src/netplay-relay.mjs')],{cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},windowsHide:true,stdio:['ignore','pipe','pipe']});
 relay.stderr.on('data',b=>relayLog+=b);
 const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay did not start')),10000);relay.stdout.on('data',b=>{relayLog+=b;const m=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m){clearTimeout(timer);yes(Number(m[1]));}});});
 for(const count of [2,3]){
  const room='th15mp-latency-'+count,members=[],clients=[];
  for(let seat=0;seat<count;seat++){
   const member='latency_member_'+count+'_'+seat,client='latency_client_'+count+'_'+seat;members.push(member);
   const socket=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,lobby:client,member,intent:seat?'join':'create',players:String(count),difficulty:'1'}));sockets.push(socket);clients.push(socket);await message(socket,()=>true);
   await send(socket,{type:'take-seat',seat,loadout:seat},m=>!!m.room?.seats[seat]);
  }
  for(let seat=0;seat<count;seat++)await send(clients[seat],{type:'set-ready',ready:true},m=>m.room?.seats[seat]?.ready);
  const start=await send(clients[0],{type:'start',adonisMode:0,inputDelay:0,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
  const live=await pageFor(count);
  try{
   for(let seat=0;seat<count;seat++){
    await launch(live.page,seat,{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(start.serial),member:members[seat]}),netplayPlayer:seat,netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:417,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:0,netplayPredictionLimit:12,netplayPredictionReserve:2,thpracEnabled:false,renderScale:seat?1:2});
    await live.frame(seat).evaluate(()=>Module._th15_probe_protection(100000));
   }
   await live.frame(0).waitForFunction(()=>globalThis.__eaglerNetplayLanFrame>=240||document.querySelector('#error').textContent,{timeout:60000});
   if(count===2)await measure(live.page,live.frame(0),'2p-quiet');
   for(let seat=1;seat<count;seat++)await live.frame(seat).evaluate(()=>{globalThis.remoteLeft=false;globalThis.remoteTimer=setInterval(()=>{remoteLeft=!remoteLeft;Module._th15_key(203,+remoteLeft);Module._th15_key(205,+!remoteLeft);},170);});
   await live.frame(0).evaluate(()=>{const t=__eaglerPeerTransport;const receive=t.receiveBinary.bind(t);globalThis.receiveBeforeLatency=receive;t.receiveBinary=data=>setTimeout(()=>receive(data),80);});
   await delay(500);await measure(live.page,live.frame(0),count+'p-rollback');
   for(let seat=1;seat<count;seat++)await live.frame(seat).evaluate(()=>{clearInterval(remoteTimer);Module._th15_key(203,0);Module._th15_key(205,0);});
   await live.frame(0).evaluate(()=>{__eaglerPeerTransport.receiveBinary=receiveBeforeLatency;});await delay(500);
   if(count===2)await checkRetry(live.frame(0));
   for(let seat=0;seat<count;seat++)await live.frame(seat).evaluate(()=>{Module._th15_loop_stop();Module._th15_mp_close();});
  }finally{await live.context.close();for(const socket of clients)socket.close();}
 }
 }
 for(const players of [2,3])await checkRecovery(players);
 await checkPartialRecovery();
 if(recoveryOnly){assert.equal(report.recovery?.length,2,'Build lacks recovery test exports');assert.ok(report.partialRecovery,'Build lacks partial recovery test export');}
 report.passed=true;
}catch(e){report.error=String(e.stack||e);throw e;}
finally{await browser?.close();for(const socket of sockets)socket.close();relay?.kill();server.close();await writeFile(resolve(out,label+'.json'),JSON.stringify(report,null,2));await writeFile(resolve(out,label+'-relay.log'),relayLog);}
