// Production shell/WASM and local shared relay with legally supplied retail DATA.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {resolve,extname} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
const root=resolve(import.meta.dirname,'../..');
const launcher=resolve(process.env.TH15_LAUNCHER_ROOT||resolve(root,'../th15-multiplayer-launcher'));
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const runtime=resolve(root,'build-eagler-multiplayer');
const out=resolve(root,'th15_web/artifacts/sdl-multiplayer/browser-test');await mkdir(out,{recursive:true});
if(!process.env.TH15_TEST_ARCHIVE)throw Error('Set TH15_TEST_ARCHIVE to the Japanese TH15 1.00b archive');
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
let options=[],relayLog='',browser;
const relay=spawn(process.execPath,[resolve(launcher,'server/netplay-relay.mjs')],{cwd:launcher,
 env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},windowsHide:true,stdio:['ignore','pipe','pipe']});
relay.stderr.on('data',b=>relayLog+=b);
const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+relayLog)),10000);relay.stdout.on('data',b=>{relayLog+=b;const match=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(match){clearTimeout(timer);yes(Number(match[1]));}});relay.once('exit',code=>no(Error('Relay exited '+code)));});
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=seat=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th15.html?managedData=1&hosted=1&runtimeEpoch='+(seat+1);document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+seat),request=crypto.randomUUID();const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},60000);function listen(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',listen);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:seat+1,request,command,...fields},location.origin);});
window.launchRuntime=async seat=>{const o=await(await fetch('/options')).json();await rpc(seat,'configure',{music:'none',options:o[seat]});await rpc(seat,'launch');};
</script>`;
const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
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
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--disable-extensions','--no-first-run','--autoplay-policy=no-user-gesture-required','--use-angle=swiftshader','--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 for(const count of [2,3]){
  const watcher=count===2,room='th15mp-browser'+count,lobbies=[],members=[];
  for(let seat=0;seat<count+(watcher?1:0);++seat){
   const client='th15_browser_'+count+'_'+seat,member='th15_private_'+count+'_'+seat;members.push({client,member});
   const q=new URLSearchParams({room,lobby:client,member,intent:seat===0?'create':'join',players:String(count),difficulty:'1'});
   const socket=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+q);lobbies.push(socket);await nextJson(socket);
   if(seat<count)await lobbySend(socket,{type:'take-seat',seat,loadout:count===2?seat+2:seat},m=>!!m.room?.seats[seat]);
   else await lobbySend(socket,{type:'spectate',name:'Browser watcher'},m=>m.room?.spectatorCount===1);
  }
  await lobbySend(lobbies[0],{type:'settings',playerCount:count,difficulty:1,chapterThreshold:count===2?500000:5000000},m=>m.room?.chapterThreshold===(count===2?500000:5000000));
  for(let seat=0;seat<count;++seat)await lobbySend(lobbies[seat],{type:'set-ready',ready:true},m=>m.room?.seats[seat]?.ready);
  const started=await lobbySend(lobbies[0],{type:'start',adonisMode:1,inputDelay:2,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
  options=Array.from({length:count+(watcher?1:0)},(_,seat)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member:members[seat].member}),netplayPlayer:seat<count?seat:null,netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:417,netplayLoadouts:Array.from({length:count},(_,s)=>({character:count===2?s+2:s,shot:0})),netplayInputDelay:2,netplayInputDelayAuto:false,netplayAdonisMode:1,netplayPredictionLimit:0,netplayPredictionReserve:2,netplaySpectator:seat===count,netplaySpectatorId:seat===count?members[seat].client:'',netplaySpectatorCount:watcher?1:0,netplayChapterThreshold:count===2?500000:5000000,netplayChallengeMode:false,thpracEnabled:false,renderScale:1}));
  const context=await browser.createBrowserContext(),page=await context.newPage(),errors=[];
  await page.setViewport({width:1280,height:1000});page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser:',m.text());});
  await page.goto(base);await page.evaluate(()=>addRuntime(0));await page.waitForFunction(()=>ready.has(1),{timeout:60000});await page.evaluate(()=>launchRuntime(0));
  const frame=seat=>page.frames().find(f=>f.url().includes('runtimeEpoch='+(seat+1)));
  const status=f=>f.evaluate(()=>{const p=Module._th15_mp_status()>>>2;return Array.from(Module.HEAPU32.subarray(p,p+24));});
  await new Promise(r=>setTimeout(r,1500));assert.equal((await status(frame(0)))[3],0,'Frame zero advanced without the other players');
  for(let seat=1;seat<count;++seat){await page.evaluate(s=>addRuntime(s),seat);await page.waitForFunction(s=>ready.has(s+1),{timeout:60000},seat);await page.evaluate(s=>launchRuntime(s),seat);}
  if(watcher){await page.evaluate(s=>addRuntime(s),count);await page.waitForFunction(s=>ready.has(s+1),{timeout:60000},count);await page.evaluate(s=>launchRuntime(s),count);}
  console.log('Waiting for '+count+' player calibration and frames');
  const observe=setInterval(async()=>{try{console.log(JSON.stringify(await Promise.all(Array.from({length:count},(_,s)=>frame(s).evaluate(()=>{const c=Module._th15_mp_calibration_status()>>>2,p=Module._th15_mp_status()>>>2,t=globalThis.__eaglerPeerTransport;return {status:Array.from(Module.HEAPU32.subarray(p,p+24)),calibration:Array.from(Module.HEAPU32.subarray(c,c+32)),phase:Module._th15_phase(),frames:Module._th15_frame(),route:t?.route,queued:t? t.received.length-t.receivedHead:0,error:document.querySelector('#error').textContent};})))));}catch{}},10000);
  try{await frame(0).waitForFunction(()=>globalThis.__eaglerNetplayLanFrame>=180||document.querySelector('#error').textContent,{timeout:60000});}finally{clearInterval(observe);}
  for(let seat=0;seat<count;++seat){assert.equal(await frame(seat).$eval('#error',e=>e.textContent),'');assert.ok((await status(frame(seat)))[3]>=170);assert.equal(await frame(seat).evaluate(()=>Module._th15_phase()),1);}
  if(watcher){await frame(count).waitForFunction(()=>__eaglerNetplayLanFrame>=170||document.querySelector('#error').textContent,{timeout:60000});assert.equal(await frame(count).$eval('#error',e=>e.textContent),'');assert.equal((await status(frame(count)))[10],1);}
  await page.screenshot({path:resolve(out,count+'p.png'),fullPage:true});
  // Stop one producer. The other peers must wait instead of inventing inputs.
  await frame(count-1).evaluate(()=>Module._th15_loop_stop());await new Promise(r=>setTimeout(r,500));
  const waiting=await status(frame(0));await new Promise(r=>setTimeout(r,350));assert.equal((await status(frame(0)))[3],waiting[3],'Missing peer input advanced the world');
  await frame(count-1).evaluate(()=>Module._th15_loop_start());await frame(0).waitForFunction(f=>__eaglerNetplayLanFrame>=f+30,{timeout:30000},waiting[3]);
  const recorder=count===3?2:0;
  await frame(recorder).evaluate(()=>Module._th15_mp_replay_save());
  const replay=await frame(recorder).evaluate(()=>Array.from(Module.FS.readFile('/save/replay/th15mp_01.rpy')));assert.ok(replay.length>200);
  assert.equal(replay[21],recorder,'Replay metadata belongs to another seat');
  assert.equal(await frame(0).evaluate(bytes=>{const p=Module._malloc(bytes.length);try{Module.HEAPU8.set(bytes,p);return Module._th15_mp_replay_validate(p,bytes.length);}finally{Module._free(p);}},replay),1);
  await writeFile(resolve(out,count+'p.rpy'),new Uint8Array(replay));
  assert.deepEqual(errors,[]);report.push({players:count,frame:(await status(frame(0)))[3],recordedSeat:recorder,frameZeroBarrier:true,missingInputWait:true,replayValidated:true,spectator:watcher});
  for(let seat=0;seat<count+(watcher?1:0);++seat)await frame(seat).evaluate(()=>{Module._th15_loop_stop();Module._th15_mp_close();});await context.close();for(const socket of lobbies)socket.close();
  // Fresh offline viewer: import through the hosted storage contract and use its real menu.
  options=[{replayViewer:true,thpracEnabled:false,renderScale:1}];
  const replayContext=await browser.createBrowserContext(),replayPage=await replayContext.newPage();
  await replayPage.goto(base);await replayPage.evaluate(()=>addRuntime(0));await replayPage.waitForFunction(()=>ready.has(1),{timeout:60000});
  await replayPage.evaluate(bytes=>rpc(0,'write',{path:'replay/th15mp_01.rpy',bytes}),replay);
  await replayPage.evaluate(()=>launchRuntime(0));const viewer=replayPage.frames().find(f=>f.url().includes('runtimeEpoch=1'));
  await viewer.waitForSelector('.mp-replay-menu button');
  assert.equal(await viewer.evaluate(async()=>{
   const manifest=await(await fetch('./manifest.json')).json(),bytes=Module.FS.readFile('/save/replay/th15mp_01.rpy');
   const build=new Uint32Array(Array.from({length:4},(_,i)=>parseInt(manifest.execution.sha256.slice(i*8,i*8+8),16)>>>0));build[0]^=1;
   const p=Module._malloc(bytes.length),b=Module._malloc(16);try{Module.HEAPU8.set(bytes,p);Module.HEAPU32.set(build,b>>>2);return Module._th15_mp_replay_load(p,bytes.length,b);}finally{Module._free(p);Module._free(b);}
  }),0,'Viewer accepted a mismatched executable');
  await viewer.click('.mp-replay-menu button');
  await viewer.waitForFunction(()=>Module._th15_phase()===4||document.querySelector('#error').textContent,{timeout:60000});
  assert.equal(await viewer.$eval('#error',e=>e.textContent),'');
  await replayPage.waitForFunction(()=>events.some(e=>e.event==='exit'),{timeout:10000});
  assert.ok(await replayPage.evaluate(()=>events.some(e=>e.event==='exit')),'Offline replay did not exit through the hosted lifecycle');
  report.at(-1).offlinePlayback=true;await replayContext.close();
 }
 await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}finally{await browser?.close();relay.kill();server.close();await writeFile(resolve(out,'relay.log'),relayLog);}
