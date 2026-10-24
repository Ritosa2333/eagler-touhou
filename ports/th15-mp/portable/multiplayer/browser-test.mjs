// Production shell/WASM and local shared relay with legally supplied retail DATA.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {resolve,extname} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..');
const launcher=resolve(process.env.TH15_LAUNCHER_ROOT||resolve(root,'../../launcher'));
const relayEntry=resolve(process.env.EAGLER_RELAY_ROOT||resolve(root,'../../services/relay'),'src/netplay-relay.mjs');
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const runtime=resolve(root,'build-eagler-multiplayer');
const modes=process.argv.includes('--rollback')?[0,2]:[1];
const out=resolve(process.argv.find(value=>value.startsWith('--output='))?.slice(9)||resolve(root,'web/artifacts/sdl-multiplayer/browser-test'));await mkdir(out,{recursive:true});
if(!process.env.TH15_TEST_ARCHIVE)throw Error('Set TH15_TEST_ARCHIVE to the Japanese TH15 1.00b archive');
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
const fonts=[['msgothic.ttc',process.env.TH15_TEST_FONT||process.env.EAGLER_DEVELOPMENT_VANILLA_FONT],
 ['unifont.otf',process.env.TH15_TEST_UNICODE_FONT||process.env.EAGLER_DEVELOPMENT_UNICODE_FONT]];
for(const [name,file] of fonts)if(!file)throw Error('Missing explicit shared font: '+name);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
await writeFile(resolve(out,'inputs.json'),JSON.stringify({scope:'TH15 MP production Runtime, 2P/3P, delayed input, confirmed spectator and offline Replay',modes,
 driver:{file:import.meta.filename,sha256:sha(await readFile(new URL(import.meta.url)))},
 archive:{file:process.env.TH15_TEST_ARCHIVE,sha256:sha(archive)},
 fonts:await Promise.all(fonts.map(async([name,file])=>({name,file,sha256:sha(await readFile(file))}))),
 runtime:JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8'))},null,2)+'\n');
let options=[],relayLog='',browser;
const relay=spawn(process.execPath,[relayEntry],{cwd:launcher,
 env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},windowsHide:true,stdio:['ignore','pipe','pipe']});
relay.stderr.on('data',b=>relayLog+=b);
const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+relayLog)),10000);relay.stdout.on('data',b=>{relayLog+=b;const match=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(match){clearTimeout(timer);yes(Number(match[1]));}});relay.once('exit',code=>no(Error('Relay exited '+code)));});
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=seat=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th15.html?managedData=1&hosted=1&runtimeEpoch='+(seat+1);document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+seat),request=crypto.randomUUID();const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},60000);function listen(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',listen);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:seat+1,request,command,...fields},location.origin);});
window.launchRuntime=async seat=>{const child=document.querySelector('#p'+seat).contentWindow;child.Module.onAbort=reason=>console.error('Native abort seat '+seat,reason,new Error().stack);const o=await(await fetch('/options')).json();await rpc(seat,'configure',{music:'none',sharedResources:[{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'},{path:'/unifont.otf',url:'/shared/unifont.otf'}],options:o[seat]});await rpc(seat,'launch');};
</script>`;
const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 const font=fonts.find(([name])=>path==='/shared/'+name);if(font){res.end(await readFile(font[1]));return;}
 if(path==='/data'){res.end(archive);return;}if(path==='/options'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(options));return;}
 if(!path.startsWith('/runtime/')||path.includes('..')){res.writeHead(404).end();return;}
 const file=resolve(runtime,path.slice(9));if(!file.startsWith(runtime+'\\')&&!file.startsWith(runtime+'/'))throw Error('Unsafe path');
 res.setHeader('Content-Type',({'.mjs':'application/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(e){res.writeHead(404).end(String(e));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));const base='http://127.0.0.1:'+server.address().port;
const report=[];
function nextJson(socket,predicate=()=>true){return new Promise((yes,no)=>{
 const timeout=setTimeout(()=>{cleanup();no(Error('Lobby response timeout '+relayLog));},10000);
 const receive=e=>{const m=JSON.parse(String(e.data));if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();yes(m);}};
 const closed=()=>{cleanup();no(Error('Lobby closed '+relayLog));};
 function cleanup(){clearTimeout(timeout);socket.removeEventListener('message',receive);socket.removeEventListener('close',closed);}
 socket.addEventListener('message',receive);socket.addEventListener('close',closed);
});}
async function lobbySend(socket,message,predicate){const reply=nextJson(socket,predicate);socket.send(JSON.stringify(message));return reply;}
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:[...(process.platform==='linux'&&process.getuid?.()===0?['--no-sandbox']:[]),'--disable-extensions','--no-first-run','--autoplay-policy=no-user-gesture-required','--use-angle=swiftshader','--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 for(const mode of modes)for(const count of [2,3]){
  const delay=mode===0?0:2,label='mode'+mode+'-'+count+'p';
  const watcher=count===2,room='th15mp-browser'+mode+'-'+count,lobbies=[],members=[];
  for(let seat=0;seat<count+(watcher?1:0);++seat){
   const client='th15_browser_'+mode+'_'+count+'_'+seat,member='th15_private_'+mode+'_'+count+'_'+seat;members.push({client,member});
   const q=new URLSearchParams({room,lobby:client,member,intent:seat===0?'create':'join',players:String(count),difficulty:'1'});
   const socket=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+q);lobbies.push(socket);await nextJson(socket);
   if(seat<count)await lobbySend(socket,{type:'take-seat',seat,loadout:count===2?seat+2:seat},m=>!!m.room?.seats[seat]);
   else await lobbySend(socket,{type:'spectate',name:'Browser watcher'},m=>m.room?.spectatorCount===1);
  }
  await lobbySend(lobbies[0],{type:'settings',playerCount:count,difficulty:1,chapterThreshold:count===2?500000:5000000},m=>m.room?.chapterThreshold===(count===2?500000:5000000));
  for(let seat=0;seat<count;++seat)await lobbySend(lobbies[seat],{type:'set-ready',ready:true},m=>m.room?.seats[seat]?.ready);
  const started=await lobbySend(lobbies[0],{type:'start',adonisMode:mode,inputDelay:delay,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
  options=Array.from({length:count+(watcher?1:0)},(_,seat)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member:members[seat].member}),netplayPlayer:seat<count?seat:null,netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:417,netplayLoadouts:Array.from({length:count},(_,s)=>({character:count===2?s+2:s,shot:0})),netplayInputDelay:delay,netplayInputDelayAuto:false,netplayAdonisMode:mode,netplayPredictionLimit:mode===1?0:12,netplayPredictionReserve:2,netplaySpectator:seat===count,netplaySpectatorId:seat===count?members[seat].client:'',netplaySpectatorCount:watcher?1:0,netplayChapterThreshold:count===2?500000:5000000,netplayChallengeMode:false,thpracEnabled:false,renderScale:1}));
  const context=await browser.createBrowserContext(),page=await context.newPage(),errors=[];
  await page.setViewport({width:1280,height:1000});page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser:',m.text());});
  await page.goto(base);await page.evaluate(()=>addRuntime(0));await page.waitForFunction(()=>ready.has(1),{timeout:60000});await page.evaluate(()=>launchRuntime(0));
  const frame=seat=>page.frames().find(f=>f.url().includes('runtimeEpoch='+(seat+1)));
  const status=f=>f.evaluate(()=>{const p=Module._th15_mp_status()>>>2;return Array.from(Module.HEAPU32.subarray(p,p+24));});
  const rollback=f=>f.evaluate(()=>{const p=Module._th15_mp_rollback_status()>>>2;return Array.from(Module.HEAPU32.subarray(p,p+20));});
  await new Promise(r=>setTimeout(r,1500));assert.equal((await status(frame(0)))[3],0,'Frame zero advanced without the other players');
  for(let seat=1;seat<count;++seat){await page.evaluate(s=>addRuntime(s),seat);await page.waitForFunction(s=>ready.has(s+1),{timeout:60000},seat);await page.evaluate(s=>launchRuntime(s),seat);}
  if(watcher){await page.evaluate(s=>addRuntime(s),count);await page.waitForFunction(s=>ready.has(s+1),{timeout:60000},count);await page.evaluate(s=>launchRuntime(s),count);}
  console.log('Waiting for '+label+' calibration and frames');
  const observe=setInterval(async()=>{try{console.log(JSON.stringify(await Promise.all(Array.from({length:count},(_,s)=>frame(s).evaluate(()=>{const c=Module._th15_mp_calibration_status()>>>2,p=Module._th15_mp_status()>>>2,t=globalThis.__eaglerPeerTransport;return {status:Array.from(Module.HEAPU32.subarray(p,p+24)),calibration:Array.from(Module.HEAPU32.subarray(c,c+32)),phase:Module._th15_phase(),frames:Module._th15_frame(),route:t?.route,queued:t? t.received.length-t.receivedHead:0,error:document.querySelector('#error').textContent};})))));}catch{}},10000);
  try{await frame(0).waitForFunction(()=>globalThis.__eaglerNetplayLanFrame>=180||document.querySelector('#error').textContent,{timeout:60000});}finally{clearInterval(observe);}
  for(let seat=0;seat<count;++seat){assert.equal(await frame(seat).$eval('#error',e=>e.textContent),'');assert.ok((await status(frame(seat)))[3]>=170);assert.equal(await frame(seat).evaluate(()=>Module._th15_phase()),1);}
  if(watcher){await frame(count).waitForFunction(()=>__eaglerNetplayLanFrame>=170||document.querySelector('#error').textContent,{timeout:60000});assert.equal(await frame(count).$eval('#error',e=>e.textContent),'');assert.equal((await status(frame(count)))[10],1);}
  let rollbackEvidence;
  if(mode!==1){
   // Delay the real receive lane after admission. Native packet ingestion,
   // input capture and correction remain the production implementation.
   for(let seat=0;seat<count;++seat)await frame(seat).evaluate(()=>{
    const transport=globalThis.__eaglerPeerTransport,receive=transport.receiveBinary.bind(transport);
    globalThis.rollbackHarness={delay:180,delayed:0,errors:[],samples:[],lastCapture:0,lastCommit:0};
    transport.receiveBinary=function(data){
     const deliver=bytes=>{const h=rollbackHarness;if(bytes.length>=44&&bytes[4]===4&&bytes[5]===1&&h.delay){++h.delayed;setTimeout(()=>receive(bytes),h.delay);}else receive(bytes);};
     if(data instanceof Blob){data.arrayBuffer().then(bytes=>deliver(new Uint8Array(bytes)));return;}
     if(data instanceof ArrayBuffer){deliver(new Uint8Array(data.slice(0)));return;}
     if(ArrayBuffer.isView(data)){deliver(new Uint8Array(data.buffer.slice(data.byteOffset,data.byteOffset+data.byteLength)));return;}
     receive(data);
    };
    rollbackHarness.monitor=setInterval(()=>{
     const h=rollbackHarness,p=Module._th15_mp_rollback_status()>>>2,v=Array.from(Module.HEAPU32.subarray(p,p+20));
     if(v[3]<h.lastCapture)h.errors.push('Physical capture rewound');
     if(v[4]!==0xffffffff&&(v[4]<h.lastCommit||v[4]>=v[2]))h.errors.push('Confirmed output frontier escaped reconciliation');
     if(v[5]>12)h.errors.push('Undo history exceeded 12 frames');
     h.lastCapture=v[3];if(v[4]!==0xffffffff)h.lastCommit=v[4];
     if(h.samples.length<400)h.samples.push(v);
    },30);
   });
   await frame(count-1).evaluate(()=>{
    let right=false;Module._th15_key(44,1);
    rollbackHarness.keys=setInterval(()=>{right=!right;Module._th15_key(203,right?0:1);Module._th15_key(205,right?1:0);},240);
   });
   await frame(0).waitForFunction(()=>{const p=Module._th15_mp_rollback_status()>>>2;return Module.HEAPU32[p+8]>=3||document.querySelector('#error').textContent;},{timeout:30000});
   for(let seat=0;seat<count;++seat)await frame(seat).evaluate(()=>{rollbackHarness.delay=0;clearInterval(rollbackHarness.keys);Module._th15_keys_clear();});
   await new Promise(r=>setTimeout(r,500));
   rollbackEvidence=await Promise.all(Array.from({length:count},(_,seat)=>frame(seat).evaluate(()=>{
    clearInterval(rollbackHarness.monitor);const {delayed,errors,samples}=rollbackHarness;const p=Module._th15_mp_rollback_status()>>>2;return {delayed,errors,samples,status:Array.from(Module.HEAPU32.subarray(p,p+20))};
   })));
   assert.ok(rollbackEvidence[0].status[8]>=3,'Late physical input did not cause actual correction');
   assert.ok(rollbackEvidence[0].status[11]>=3,'Correction counter had no matching state restores');
   assert.ok(rollbackEvidence[0].status[9]>0,'No replayed simulation frames');
   for(const evidence of rollbackEvidence){assert.deepEqual(evidence.errors,[]);assert.ok(evidence.delayed>0);}
   await writeFile(resolve(out,label+'-rollback.json'),JSON.stringify(rollbackEvidence,null,2)+'\n');
  }
  await page.screenshot({path:resolve(out,label+'.png'),fullPage:true});
  // Stop one producer, then drain its already captured/transmitted input.
  // The frontier follows native frame counts, independent of GPU throughput.
  const producer=await frame(count-1).evaluate(()=>{Module._th15_loop_stop();const p=Module._th15_mp_rollback_status()>>>2,s=Module._th15_mp_status()>>>2;return {capture:Module.HEAPU32[p+3],delay:Module.HEAPU32[s+7]};});
  const boundedFrontier=producer.capture+producer.delay+(mode===1?0:12);
  await frame(0).waitForFunction(limit=>{const p=Module._th15_mp_rollback_status()>>>2;return (Module.HEAPU32[p+2]>=limit&&!Module.HEAPU32[p+1])||document.querySelector('#error').textContent;},{timeout:30000},boundedFrontier);
  assert.equal(await frame(0).$eval('#error',e=>e.textContent),'');const waiting=await status(frame(0));
  assert.equal(waiting[3],boundedFrontier,'Missing peer input exceeded the captured-input frontier');
  await new Promise(r=>setTimeout(r,350));assert.equal((await status(frame(0)))[3],waiting[3],'Missing peer input advanced beyond the bounded frontier');
  assert.equal((await rollback(frame(count-1)))[3],producer.capture,'Stopped peer captured another physical sample');
  assert.equal((await rollback(frame(0)))[4],producer.capture+producer.delay-1,'Committed output exceeded the stopped producer input');
  if(mode!==1){
   const stopped=await rollback(frame(0));assert.ok(stopped[5]<=12);assert.ok(stopped[2]-stopped[4]-1<=12);
   const saved=await frame(0).evaluate(()=>{if(!Module._th15_mp_replay_save())throw Error('Confirmed prefix save failed');const bytes=Module.FS.readFile('/save/replay/th15mp_01.rpy'),p=Module._th15_mp_rollback_status()>>>2;return {frames:new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).getUint32(32,true),committed:Module.HEAPU32[p+4],next:Module.HEAPU32[p+2]};});
   assert.equal(saved.frames,saved.committed+1,'Replay published uncommitted predicted input');assert.equal(saved.frames,producer.capture+producer.delay,'Replay exceeded the independently observed producer input');assert.ok(saved.frames<=saved.next);
  }
  await frame(count-1).evaluate(()=>Module._th15_loop_start());await frame(0).waitForFunction(f=>__eaglerNetplayLanFrame>=f+30,{timeout:30000},waiting[3]);
  const recorder=count===3?2:0;
  await frame(recorder).evaluate(()=>Module._th15_mp_replay_save());
  const replay=await frame(recorder).evaluate(()=>Array.from(Module.FS.readFile('/save/replay/th15mp_01.rpy')));assert.ok(replay.length>200);
  assert.equal(replay[21],recorder,'Replay metadata belongs to another seat');
  assert.equal(await frame(0).evaluate(bytes=>{const p=Module._malloc(bytes.length);try{Module.HEAPU8.set(bytes,p);return Module._th15_mp_replay_validate(p,bytes.length);}finally{Module._free(p);}},replay),1);
  await writeFile(resolve(out,label+'.rpy'),new Uint8Array(replay));
  const observedRollback=await rollback(frame(0));
  if(mode===1){assert.equal(observedRollback[11],0,'Exact-input delay restored a predicted state');assert.equal(observedRollback[9],0,'Exact-input delay replayed a simulation frame');}
  assert.deepEqual(errors,[]);report.push({mode,players:count,actualRestores:rollbackEvidence?.[0].status[11]??observedRollback[11],replayedFrames:rollbackEvidence?.[0].status[9]??observedRollback[9],frame:(await status(frame(0)))[3],recordedSeat:recorder,frameZeroBarrier:true,missingInputWait:true,captureFence:{...producer,next:boundedFrontier,committed:producer.capture+producer.delay-1},replayValidated:true,spectator:watcher});
  for(let seat=0;seat<count+(watcher?1:0);++seat)await frame(seat).evaluate(()=>{Module._th15_loop_stop();Module._th15_mp_close();});await context.close();for(const socket of lobbies)socket.close();
  // Fresh offline viewer: import and open through the current hosted Replay
  // workspace contract. Native playback owns its cursor and EOF report.
  options=[{replayViewer:true,thpracEnabled:false,renderScale:1}];
  const replayContext=await browser.createBrowserContext(),replayPage=await replayContext.newPage();
  await replayPage.goto(base);await replayPage.evaluate(()=>addRuntime(0));await replayPage.waitForFunction(()=>ready.has(1),{timeout:60000});
  await replayPage.evaluate(bytes=>rpc(0,'write',{path:'replay/th15mp_01.rpy',bytes}),replay);
  await replayPage.evaluate(()=>launchRuntime(0));const viewer=replayPage.frames().find(f=>f.url().includes('runtimeEpoch=1'));
  const description=await replayPage.evaluate(async()=> (await rpc(0,'playback',{action:'inspect',path:'replay/th15mp_01.rpy'})).description);
  assert.ok(description.stages?.length>0,'Native Replay metadata has no playable stage');
  assert.equal(await viewer.evaluate(async()=>{
   const manifest=await(await fetch('./manifest.json')).json(),bytes=Module.FS.readFile('/save/replay/th15mp_01.rpy');
   const build=new Uint32Array(Array.from({length:4},(_,i)=>parseInt(manifest.execution.sha256.slice(i*8,i*8+8),16)>>>0));build[0]^=1;
   const p=Module._malloc(bytes.length),b=Module._malloc(16);try{Module.HEAPU8.set(bytes,p);Module.HEAPU32.set(build,b>>>2);return Module._th15_mp_replay_load(p,bytes.length,b);}finally{Module._free(p);Module._free(b);}
  }),0,'Viewer accepted a mismatched executable');
  await replayPage.evaluate(stage=>rpc(0,'playback',{action:'open',path:'replay/th15mp_01.rpy',stage}),description.stages[0].id);
  await replayPage.evaluate(()=>rpc(0,'playback',{action:'rate',value:0}));
  await replayPage.waitForFunction(()=>events.some(e=>e.event==='runtime-info'&&e.playback?.ended)||document.querySelector('#p0').contentDocument.querySelector('#error').textContent,{timeout:60000});
  assert.equal(await viewer.$eval('#error',e=>e.textContent),'');
  const playback=await replayPage.evaluate(()=>events.filter(e=>e.event==='runtime-info'&&e.playback).at(-1).playback);
  assert.equal(playback.frame,playback.total);assert.equal(playback.total,new DataView(Uint8Array.from(replay).buffer).getUint32(32,true));
  assert.equal(playback.ended,true);report.at(-1).offlinePlayback={frames:playback.frame,ended:true,stage:playback.startStage};await replayContext.close();
  console.log('PASS '+label+' | '+(mode===1?'exact-input delay':'real correction')+', capture/commit fence, confirmed spectator and offline Replay');
 }
 await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}finally{await browser?.close();relay.kill();server.close();await writeFile(resolve(out,'relay.log'),relayLog);}
