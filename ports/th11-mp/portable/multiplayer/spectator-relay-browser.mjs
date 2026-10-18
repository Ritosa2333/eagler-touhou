// Real relay, TH11 SDL Runtime and two independent native spectators. The
// diagnostic build is assembled into a fresh artifact directory; no old
// packaged runtime tree is reused or overwritten.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir,copyFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {spawn} from 'node:child_process';
import {resolve,extname,dirname} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {randomUUID,createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..');
const workspace=(()=>{if(process.env.EAGLER_WORKSPACE)return resolve(process.env.EAGLER_WORKSPACE);
 for(let current=root;;current=dirname(current)){if(existsSync(resolve(current,'WORKSPACE.md')))return current;if(dirname(current)===current)break;}
 throw Error('Set EAGLER_WORKSPACE to the workspace containing WORKSPACE.md.');})();
const launcher=resolve(process.env.TH11_LAUNCHER_ROOT||resolve(workspace,'worktrees/launcher-th09-spectator-main'));
const relayRoot=resolve(process.env.TH11_RELAY_ROOT||resolve(launcher,'server'));
const buildRoot=resolve(process.env.TH11_TEST_BUILD||resolve(root,'th11_web/artifacts/multiplayer-diagnostic'));
const archivePath=resolve(process.env.TH11_TEST_ARCHIVE||resolve(workspace,'games/web-content/th11/th11.data'));
const sharedFonts=resolve(process.env.TH11_SHARED_FONT_ROOT||resolve(workspace,'dist/ui-rewrite-fixed-20261010/shared'));
const cp932Path=resolve(process.env.TH11_CP932_FONT||resolve(workspace,'dist/update-ef596d2-20261006/th11-previous-runtime/fonts/cp932.bin'));
const build=JSON.parse(await readFile(resolve(buildRoot,'build.json'),'utf8'));
const fixtures=build.privateSpectatorFixture===true;
const productionBuild=fixtures?JSON.parse(await readFile(resolve(buildRoot,'..','build.json'),'utf8')):build;
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const loadedWasm=await readFile(resolve(buildRoot,'th11-sdl.wasm'));
const loadedLoader=await readFile(resolve(buildRoot,'th11-sdl.mjs'));
assert.equal(sha256(loadedWasm),build.sha256,'TH11 endpoint WASM must match its build.json');
assert.equal(sha256(loadedLoader),build.loaderSha256,'TH11 endpoint loader must match its build.json');
if(fixtures)assert.equal(build.productionWasm,productionBuild.sha256,'spectator fixture must link to the adjacent production build');
const playerCount=Number(process.env.TH11_TEST_PLAYER_COUNT||2);
const stageOneOnly=process.env.TH11_TEST_STAGE_ONE_ONLY==='1';
const lifecycleOnly=process.argv.includes('--lifecycle-only')||process.env.TH11_TEST_LIFECYCLE_ONLY==='1';
const softwareRenderer=process.env.TH11_TEST_SOFTWARE_RENDERER==='1';
assert.ok(playerCount===2||playerCount===3);
const seats=Array.from({length:playerCount},(_,i)=>i),viewerSeat=playerCount,lateSeat=playerCount+1;
assert.equal(build.game,'th11');assert.equal(build.variant,'multiplayer');assert.equal(build.features?.multiplayer,true);
assert.ok(build.diagnostic===true,'This browser exercise expects the separately built diagnostic MP Runtime.');
for(const path of [archivePath,resolve(sharedFonts,'msgothic.ttc'),resolve(sharedFonts,'unifont.otf'),cp932Path,
  resolve(buildRoot,'th11-sdl.mjs'),resolve(buildRoot,'th11-sdl.wasm')])assert.ok(existsSync(path),'Missing test input '+path);

const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
const out=resolve(root,'artifacts/multiplayer-spectator/relay-'+stamp+'-'+randomUUID().slice(0,8));
const runtime=resolve(out,'runtime');await mkdir(runtime,{recursive:true});
const shellRoot=resolve(root,'th11_web/sdl-runtime');
const copy=async(source,name)=>copyFile(source,resolve(runtime,name));
for(const name of ['startup-branding.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs','eagler-host.mjs','multiplayer.mjs','multiplayer.css'])
  await copy(resolve(shellRoot,name),name);
for(const [source,name] of [
  [resolve(root,'portable/browser/motion-replay.mjs'),'motion-replay.mjs'],
  [resolve(root,'portable/browser/multiplayer-replay.mjs'),'multiplayer-replay.mjs'],
  [resolve(root,'third_party/eagler-common/browser/direct-input-bridge.mjs'),'direct-input-bridge.mjs'],
  [resolve(root,'third_party/eagler-common/browser/adonis-calibration.mjs'),'adonis-calibration.mjs'],
  [resolve(buildRoot,'th11-sdl.mjs'),'th11-sdl.mjs'],[resolve(buildRoot,'th11-sdl.wasm'),'th11-sdl.wasm'],
  [resolve(sharedFonts,'msgothic.ttc'),'msgothic.ttc'],[resolve(sharedFonts,'unifont.otf'),'unifont.otf'],
  [cp932Path,'fonts/cp932.bin']]){
  const target=resolve(runtime,name);await mkdir(resolve(target,'..'),{recursive:true});await copy(source,name);
}
let options=[];
const shell=await readFile(resolve(shellRoot,'shell.mjs'),'utf8');
const buildInfo=/\/\*TH11_BUILD_INFO\*\/\{[^;]*\}/;
const factory=/\/\*TH11_MULTIPLAYER_FACTORY\*\/[^;\r\n]*;/g;
assert.match(shell,buildInfo);assert.equal([...shell.matchAll(factory)].length,1);
await writeFile(resolve(runtime,'shell.mjs'),shell.replace(buildInfo,'/*TH11_BUILD_INFO*/'+JSON.stringify({version:build.version,completeGame:true,multiplayer:true}))
  .replace(factory,value=>value.replace('/*TH11_MULTIPLAYER_FACTORY*/','')));
await copy(resolve(shellRoot,'managed.html'),'th11.html');
const cp932Bytes=await readFile(cp932Path);
await writeFile(resolve(runtime,'resources.json'),JSON.stringify({schema:'eagler-sdl-resources/1',game:'th11',
  resources:[{path:'/fonts/cp932.bin',url:'./fonts/cp932.bin',bytes:cp932Bytes.length}]},null,2)+'\n');
await writeFile(resolve(runtime,'manifest.json'),JSON.stringify({game:'th11',protocol:'eagler-touhou/1',adapter:'sdl3-eagler',
  profile:'multiplayer',product:'th11mp',variant:'multiplayer',builtAt:build.builtAt,version:build.version,
  features:{thprac:false,languages:true,focusHitbox:false,multiplayer:true},music:['ogg-stream','ogg-full','none'],touchReplay:false,
  execution:{kind:build.kind,sha256:build.sha256,loaderSha256:build.loaderSha256,architecture:build.architecture}},null,2)+'\n');
const runtimeManifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
assert.equal(runtimeManifest.execution.sha256,build.sha256,'TH11 runtime manifest must name the selected build');
assert.equal(runtimeManifest.execution.loaderSha256,build.loaderSha256,'TH11 runtime manifest must name the selected loader');
assert.equal(sha256(await readFile(resolve(runtime,'th11-sdl.wasm'))),build.sha256,'assembled TH11 Runtime WASM identity');
assert.equal(sha256(await readFile(resolve(runtime,'th11-sdl.mjs'))),build.loaderSha256,'assembled TH11 Runtime loader identity');
await mkdir(out,{recursive:true});

const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
let relayLog='',browser,collectFailure;
const relay=spawn(process.execPath,[resolve(relayRoot,'netplay-relay.mjs')],{cwd:relayRoot,windowsHide:true,
  env:{...process.env,EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
relay.stderr.on('data',b=>relayLog+=b);
const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+relayLog)),15000);
  relay.once('error',error=>{clearTimeout(timer);no(Error('Relay spawn failed '+error+' '+relayLog));});
  relay.stdout.on('data',b=>{relayLog+=b;const m=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m){clearTimeout(timer);yes(Number(m[1]));}});
  relay.once('exit',code=>{clearTimeout(timer);no(Error('Relay exited '+code+' '+relayLog));});});
const pageHtml=`<!doctype html><meta charset="utf-8"><style>body{margin:0}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.runtimeEpochs={};window.addRuntime=(seat,epoch=seat+1)=>{const f=document.createElement('iframe');f.id='p'+seat;runtimeEpochs[seat]=epoch;f.src='/runtime/th11.html?managedData=1&hosted=1&runtimeEpoch='+epoch;document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+seat),request=crypto.randomUUID();
 const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},60000);
 function listen(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error));}}
 addEventListener('message',listen);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th11',epoch:runtimeEpochs[seat]??seat+1,request,command,...fields},location.origin);});
window.launchRuntime=async seat=>{const o=await(await fetch('/options')).json();await rpc(seat,'configure',{music:'none',options:o[seat],sharedResources:[
 {path:'/msgothic.ttc',url:'/shared/msgothic.ttc',bytes:${(await readFile(resolve(sharedFonts,'msgothic.ttc'))).length}},
 {path:'/unifont.otf',url:'/shared/unifont.otf',bytes:${(await readFile(resolve(sharedFonts,'unifont.otf'))).length}}]});await rpc(seat,'launch');};
</script>`;
const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(pageHtml);return;}
 if(path==='/data'){res.end(await readFile(archivePath));return;}
 if(path==='/options'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(options));return;}
 let file;
 if(path.startsWith('/runtime/')&&!path.includes('..'))file=resolve(runtime,path.slice(9));
 else if(path==='/shared/msgothic.ttc')file=resolve(sharedFonts,'msgothic.ttc');
 else if(path==='/shared/unifont.otf')file=resolve(sharedFonts,'unifont.otf');
 else {res.writeHead(404).end();return;}
 if(!file.startsWith(runtime+'\\')&&!file.startsWith(runtime+'/')&&!file.startsWith(sharedFonts+'\\')&&!file.startsWith(sharedFonts+'/'))throw Error('Unsafe asset path');
 res.setHeader('Content-Type',({'.mjs':'application/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html','.css':'text/css','.ttc':'font/collection','.otf':'font/otf'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(error){res.writeHead(404).end(String(error));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));
const base='http://127.0.0.1:'+server.address().port,sockets=[],report={passed:false,build:{sha256:build.sha256,loaderSha256:build.loaderSha256,diagnostic:build.diagnostic,
  productionSha256:productionBuild.sha256,fixtureProductionWasm:build.productionWasm||null,endpointWasmMatchesBuild:true,endpointLoaderMatchesBuild:true,
  runtimeManifestWasm:runtimeManifest.execution.sha256,runtimeManifestLoader:runtimeManifest.execution.loaderSha256},endpoints:[],
  data:{sha256:createHash('sha256').update(await readFile(archivePath)).digest('hex')},players:playerCount,relayPort,output:out,phases:[]};
 const wait=async(check,label,ms=15000)=>{const end=Date.now()+ms;while(Date.now()<end){const value=await check();if(value)return value;await new Promise(r=>setTimeout(r,25));}throw Error(label+' timeout');};
async function open(query){const ws=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+query),r={ws,messages:[]};sockets.push(ws);
 ws.addEventListener('message',e=>{if(typeof e.data==='string')r.messages.push(JSON.parse(e.data));});await wait(()=>ws.readyState===1,'lobby connect');assert.equal(ws.readyState,1);return r;}
const latest=r=>r.messages.findLast(m=>m.room)?.room;
async function send(r,message,predicate){const at=r.messages.length;r.ws.send(JSON.stringify(message));await wait(()=>r.messages.slice(at).some(m=>m.type==='error'||predicate(m)),'lobby '+message.type);
 const error=r.messages.slice(at).find(m=>m.type==='error');if(error)throw Error(error.error);return r.messages.slice(at).find(predicate);}
const instances=[];
const phase=label=>{report.phases.push(label);console.log('TH11 '+playerCount+'P '+label);};
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required',
  ...(softwareRenderer?['--use-angle=swiftshader','--enable-unsafe-swiftshader']:['--enable-gpu','--use-gl=angle','--use-angle=d3d11']),
  '--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const roomCode=String(7400+Math.floor(Math.random()*90)),room='th11mp-'+roomCode,members=[];
 const lobbies=[];
 for(let seat=0;seat<playerCount;seat++){
  const client='spectator_lab_'+roomCode+'_'+seat,member='spectator_member_'+roomCode+'_'+seat;members.push({client,member});
  const lobby=await open(new URLSearchParams({room,lobby:client,member,intent:seat?'join':'create',players:String(playerCount),difficulty:'0'}));lobbies.push(lobby);
  await send(lobby,{type:'take-seat',seat,loadout:seat},m=>m.room?.seats?.[seat]?.clientId===client);
 }
 const viewerId='spectator_wait_'+roomCode,viewerMember='spectator_viewer_'+roomCode;
 const observerLobby=await open(new URLSearchParams({room,lobby:viewerId,member:viewerMember,intent:'join'}));lobbies.push(observerLobby);
 await send(observerLobby,{type:'spectate'},m=>m.room?.spectatorCount===1);
 for(let seat=0;seat<playerCount;seat++)await send(lobbies[seat],{type:'set-ready',ready:true},m=>m.room?.seats?.[seat]?.ready);
 await wait(()=>latest(lobbies[0])?.seats?.slice(0,playerCount).every(s=>s?.ready),'all ready');
 const started=await send(lobbies[0],{type:'start',adonisMode:1,inputDelay:2,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
 options=Array.from({length:playerCount+1},(_,seat)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member:seat<playerCount?members[seat].member:viewerMember}),
  netplayPlayer:seat<playerCount?seat:0,netplayPlayerCount:playerCount,netplayDifficulty:0,netplaySeed:417,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0},{character:0,shot:2}].slice(0,playerCount),
  netplayInputDelay:2,netplayInputDelayAuto:false,netplayAdonisMode:1,netplayPredictionLimit:0,netplayPredictionReserve:2,netplaySpectator:seat===viewerSeat,
  netplaySpectatorId:seat===viewerSeat?viewerId:'',netplaySpectatorCount:1,netplayChallengeMode:false,thpracEnabled:false,renderScale:1}));
 const contexts=[],pages=new Map(),epochs=new Map(),errors=[];
 const frame=seat=>pages.get(seat).frames().find(f=>f.url().includes('runtimeEpoch='+epochs.get(seat)));
 const readGame=f=>f.evaluate(()=>{const p=core.th11_mp_game_status();return Array.from(new Uint32Array(core.memory.buffer,p,56));});
 const readRollback=f=>f.evaluate(()=>{const p=core.th11_mp_rollback_status();return Array.from(new Uint32Array(core.memory.buffer,p,24));});
 const readSpectatorDiagnostics=f=>f.evaluate(()=>{const p=core.th11_mp_spectator_diagnostics();const a=Array.from(new Uint32Array(core.memory.buffer,p,34));return {
  version:a[0],publisherOwner:!!a[1],publisherNext:a[2],publishFrame:a[3],publishFailed:!!a[4],
  transportHasSpectators:!!a[5],transportStatePlus1:a[6],transportFailed:!!a[7],transportDisconnected:!!a[8],
  timingSent:!!a[9],timingReady:!!a[10],receiveFrame:a[11],initialFrameQueue:a[12],playbackReady:!!a[13],
  receivedFrontier:a[14],receivedSegments:a[15],playbackPosition:a[16],playbackTarget:a[17],
  seeking:!!a[18],playbackFailed:!!a[19],publisherStarted:!!a[20],captureAttempts:a[21],captureFailures:a[22],
  captureFrame:a[23],captureStage:a[24],captureFailureReason:a[25],captureBytes:a[26],sessionFrame:a[27],
  stage:a[28],phase:a[29],stageActive:!!a[30],transitioning:!!a[31],outgoingStage:!!a[32],stageLoaded:!!a[33]
 };});
 const readNativeError=f=>f.evaluate(()=>{const p=core.th11_mp_error?.();if(!p)return '';const h=new Uint8Array(core.memory.buffer);let e=p;while(e<h.length&&h[e])++e;return new TextDecoder().decode(h.subarray(p,e));});
 const readPlayback=f=>f.evaluate(()=>{const p=core.th11_mp_spectator_state();if(!p)return null;const h=new Uint8Array(core.memory.buffer);let e=p;while(e<h.length&&h[e])++e;return JSON.parse(new TextDecoder().decode(h.subarray(p,e)));});
 const recordGraphics=async seat=>{const graphics=await frame(seat).evaluate(()=>{const canvas=Module.canvas;if(!canvas)return null;
   const gl=canvas.getContext('webgl2')||canvas.getContext('webgl');if(!gl)return null;
   const debug=gl.getExtension('WEBGL_debug_renderer_info');return {vendor:gl.getParameter(debug?debug.UNMASKED_VENDOR_WEBGL:gl.VENDOR),renderer:gl.getParameter(debug?debug.UNMASKED_RENDERER_WEBGL:gl.RENDERER)};});
  (report.graphics??=[]).push({seat,requested:softwareRenderer?'swiftshader':'d3d11',...graphics});};
 const health=f=>f.evaluate(()=>{const p=core.th11_mp_error?.();let nativeError='';if(p){const h=new Uint8Array(core.memory.buffer);let e=p;while(e<h.length&&h[e])++e;nativeError=new TextDecoder().decode(h.subarray(p,e));}
  return {error:document.querySelector('#error')?.textContent||'',failed:!!globalThis.__eaglerNetplayFailed,
   reason:globalThis.__eaglerNetplayError||'',nativeError};});
 collectFailure=async()=>{const all=[];for(const [seat] of pages){const f=frame(seat);if(f)all.push({seat,health:await health(f),game:await readGame(f),spectator:await readSpectatorDiagnostics(f),playback:await readPlayback(f)});}return all;};
 const add=async (seat,epoch=seat+1)=>{
  epochs.set(seat,epoch);
  const context=await browser.createBrowserContext();contexts.push(context);const page=await context.newPage();pages.set(seat,page);
  const endpoint={seat,runtimeEpoch:epoch,expectedWasm:build.sha256,expectedLoader:build.loaderSha256,productionWasm:productionBuild.sha256};report.endpoints.push(endpoint);
  page.on('response',response=>{const path=new URL(response.url()).pathname;if(path==='/runtime/th11-sdl.wasm')void response.buffer().then(bytes=>{endpoint.wasm=sha256(bytes);}).catch(error=>{endpoint.responseError=String(error);});
   if(path==='/runtime/th11-sdl.mjs')void response.buffer().then(bytes=>{endpoint.loader=sha256(bytes);}).catch(error=>{endpoint.responseError=String(error);});});
  page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser:',m.text());});await page.goto(base,{waitUntil:'domcontentloaded',timeout:120000});
  await page.evaluate(({seat,epoch})=>addRuntime(seat,epoch),{seat,epoch});await page.waitForFunction(e=>ready.has(e),{timeout:120000},epoch);
  const f=frame(seat);instances.push(f);
  const endpointManifest=await f.evaluate(async()=>{const r=await fetch('./manifest.json');return r.ok?await r.json():null;});
  assert.equal(endpointManifest?.execution?.sha256,build.sha256,'seat '+seat+' served Runtime manifest WASM identity');
  assert.equal(endpointManifest?.execution?.loaderSha256,build.loaderSha256,'seat '+seat+' served Runtime manifest loader identity');
  await f.evaluate(fixtures=>{globalThis.spectatorLabHashes=new Map();if(fixtures){globalThis.spectatorLabCheckpoints=new Map();Module.onSpectatorLabCheckpoint=(frame,hash)=>{const p=core.th11_spectator_lab_words(),length=new Uint32Array(core.memory.buffer,p,1)[0];spectatorLabCheckpoints.set(frame,{frame,hash:hash>>>0,words:Array.from(new Uint32Array(core.memory.buffer,p+4,length))});};Module.onSpectatorLabFrame=(frame,hash)=>{spectatorLabHashes.set(frame,[0,hash>>>0]);if(frame===globalThis.spectatorLabNext&&!core.th11_spectator_lab_next())throw Error('Native stage transition fixture failed');};return;}globalThis.spectatorLabPoll=setInterval(()=>{
    try{const p=core?.th11_mp_game_status?.();if(!p)return;const a=Array.from(new Uint32Array(core.memory.buffer,p,56));if(a[0]===1){spectatorLabHashes.set(a[4],[a[2],a[1]]);while(spectatorLabHashes.size>60000)spectatorLabHashes.delete(spectatorLabHashes.keys().next().value);}}
    catch{}},2);},fixtures);
  await page.evaluate(s=>launchRuntime(s),seat);await wait(()=>endpoint.wasm&&endpoint.loader,'seat '+seat+' runtime binary identity');
  assert.equal(endpoint.wasm,endpoint.expectedWasm,'seat '+seat+' loaded expected WASM');assert.equal(endpoint.loader,endpoint.expectedLoader,'seat '+seat+' loaded expected loader');
  endpoint.runtimeManifestMatched=true;return f;
 };
 const sfxStarts=async f=>f.evaluate(()=>{const p=core.th11_sfx_statistics();return new Uint32Array(core.memory.buffer,p,120)[2];});
 let playbackControlCount=0,controlsAtSourceTransition=null;
 const control=async(seat,action,target)=>{playbackControlCount++;const f=frame(seat),state=await readPlayback(f);assert.ok(state,'native stage playback report absent');
  await pages.get(seat).evaluate(({seat,state,action,target})=>rpc(seat,'spectator-playback',{run:state.run,action,...(target===undefined?{}:{frame:target})}),{seat,state,action,target});};
 const waitPosition=async(seat,predicate,label,ms=180000)=>{try{return await wait(async()=>{const f=frame(seat),h=await health(f);if(h.failed||h.error||h.nativeError)throw Error(label+' native viewer failed '+JSON.stringify(h));const state=await readPlayback(f);return state&&predicate(state);},label,ms);}
  catch(error){const f=frame(seat),host=frame(0);report.lastWaitSnapshot={label,viewer:{health:await health(f),playback:await readPlayback(f),game:await readGame(f),rollback:await readRollback(f),spectator:await readSpectatorDiagnostics(f)},host:{health:await health(host),game:await readGame(host),rollback:await readRollback(host),spectator:await readSpectatorDiagnostics(host)},relayTail:relayLog.slice(-6000),lobbyTail:lobbies[0].messages.slice(-12)};
   if(fixtures){const expected=await host.evaluate(()=>Array.from(globalThis.spectatorLabCheckpoints.values()).at(-1)),actual=await f.evaluate(()=>{const p=core.th11_spectator_lab_words(),n=new Uint32Array(core.memory.buffer,p,1)[0];return Array.from(new Uint32Array(core.memory.buffer,p+4,n));});report.canonicalDifference={expected,actual,first:expected?.words.findIndex((value,index)=>value!==actual[index])};}
   throw error;}};
 phase('lobby-and-players');report.playerCount=playerCount;
 for(let seat=0;seat<playerCount;seat++)await add(seat);
 for(let seat=0;seat<playerCount;seat++)await pages.get(seat).evaluate(({seat})=>rpc(seat,'keyboard',{code:'KeyZ',down:true}),{seat});
 await wait(async()=>{for(const seat of seats){const status=await health(frame(seat));if(status.failed||status.error||status.nativeError)throw Error('source startup failed '+JSON.stringify({seat,...status}));}
  const values=await Promise.all(seats.map(s=>readGame(frame(s))));return values.every(v=>v[4]>=300&&v[3]===1);},'players reached live stage frames',120000);
 for(const seat of seats)await recordGraphics(seat);
 report.playersAtFirstViewerAdmission=await Promise.all(seats.map(s=>readGame(frame(s))));report.hostRollbackAtFirstViewerAdmission=await readRollback(frame(0));
 report.hostSpectatorAtFirstViewerAdmission=await readSpectatorDiagnostics(frame(0));
 phase('first-late-viewer');
 const firstViewer=await add(viewerSeat);
 await waitPosition(viewerSeat,s=>s.position>=180&&!s.paused&&!s.seeking,'first independent native restore',Number(process.env.TH11_FIRST_VIEWER_TIMEOUT_MS||120000));
 await recordGraphics(viewerSeat);
 const firstState=await readPlayback(firstViewer);assert.ok(firstState.min>=0&&firstState.position>=180,firstState);
 if(lifecycleOnly){
  assert.equal(fixtures,false,'focused lifecycle requires the final production Runtime, not the fixture build');
  const delay=ms=>new Promise(resolve=>setTimeout(resolve,ms));
  const status=async seat=>frame(seat).evaluate(()=>Array.from(new Uint32Array(core.memory.buffer,core.th11_mp_status(),24)));
  const healthy=async seat=>{const f=frame(seat),h=await health(f),s=await status(seat);assert.equal(h.failed,false,'seat '+seat+' host reports a network failure');assert.equal(h.error,'','seat '+seat+' shell error');assert.equal(h.nativeError,'','seat '+seat+' native error');assert.equal(s[19],1,'seat '+seat+' native transport remains open');assert.equal(s[23],0,'seat '+seat+' native status has no error');return s;};
  const sampleOneX=async(label)=>{
   const before=await readPlayback(frame(viewerSeat)),sourceBefore=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4])),startedAt=Date.now();
   assert.ok(before&&!before.paused&&!before.seeking,label+' must begin in normal playback');await delay(1200);
   const elapsed=(Date.now()-startedAt)/1000,after=await readPlayback(frame(viewerSeat)),sourceAfter=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4]));
   const sourceDelta=Math.max(...sourceAfter.map((value,index)=>value-sourceBefore[index])),viewerDelta=after.position-before.position;
   assert.ok(after&&!after.paused&&!after.seeking,label+' must remain normal playback');assert.ok(sourceDelta>=20,label+' source did not advance enough to measure pacing');
   assert.ok(viewerDelta>0,label+' native viewer did not advance');assert.ok(viewerDelta<=sourceDelta+Math.max(10,Math.ceil(sourceDelta*.3)),label+' advanced faster than source: '+JSON.stringify({viewerDelta,sourceDelta,elapsed}));
   await Promise.all(seats.map(seat=>healthy(seat)));await healthy(viewerSeat);
   return {elapsedSeconds:elapsed,sourceFrames:sourceDelta,viewerFrames:viewerDelta,viewerToSourceRatio:Number((viewerDelta/sourceDelta).toFixed(3)),initialPosition:before.position,finalPosition:after.position,
    initialEdge:before.edge,finalEdge:after.edge,initialTarget:before.target,finalTarget:after.target,following:after.following,paused:after.paused,seeking:after.seeking};
  };
  const initialOneX=await sampleOneX('initial history playback');
   await control(viewerSeat,'live');const liveRequested=await readPlayback(frame(viewerSeat));assert.ok(liveRequested,'Live request did not produce playback state');
   const fixedLiveTarget=liveRequested.target;await waitPosition(viewerSeat,s=>s.following&&!s.paused&&!s.seeking&&s.position>=fixedLiveTarget,'explicit Stage1 Live target completes',30000);
  const liveCompleted=await readPlayback(frame(viewerSeat)),liveOneX=await sampleOneX('passive playback after explicit Live');
  assert.equal(liveOneX.initialTarget,liveCompleted.target,'new source frames moved the fixed Live target');
  const sourceBeforePause=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4]));
  await control(viewerSeat,'pause');const localPaused=await readPlayback(frame(viewerSeat));await delay(17050);
  const localPausedAfter=await readPlayback(frame(viewerSeat));assert.equal(localPausedAfter.position,localPaused.position,'local pause advanced the native cursor');
  assert.ok(localPausedAfter.paused&&!localPausedAfter.seeking,'local pause state changed during the 17-second hold');
  const sourceDuringLocalPause=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4]));
  assert.ok(sourceDuringLocalPause.some((value,index)=>value>sourceBeforePause[index]),'source did not keep publishing during the local pause');
  await Promise.all(seats.map(seat=>healthy(seat)));await healthy(viewerSeat);
  report.localPause={durationMs:17050,positionBefore:localPaused.position,positionAfter:localPausedAfter.position,
   receivedBefore:localPaused.received,receivedAfter:localPausedAfter.received,sourceAdvanced:true,healthy:true};
  await control(viewerSeat,'play');
  await waitPosition(viewerSeat,s=>!s.paused&&!s.seeking&&s.position>localPausedAfter.position+120,'resume after local pause',120000);
  const readyToSeek=await readPlayback(frame(viewerSeat));assert.ok(readyToSeek.position>readyToSeek.min+620,'not enough retained history to observe an in-flight native seek');
  const seekTarget=readyToSeek.min+300;await control(viewerSeat,'seek',seekTarget);
  const seekStart=await wait(async()=>{const s=await readPlayback(frame(viewerSeat));return s?.seeking&&s.position<seekTarget?s:null;},'native backward seek becomes active',10000);
  const seekProgress=await wait(async()=>{const s=await readPlayback(frame(viewerSeat));return s?.seeking&&s.position>seekStart.position?s:null;},'native seek advances before Pause',10000);
  await control(viewerSeat,'pause');
  await waitPosition(viewerSeat,s=>s.paused&&!s.seeking,'Pause cancels active native seek',10000);
  const seekPaused=await readPlayback(frame(viewerSeat));await delay(450);
  const seekPausedAfter=await readPlayback(frame(viewerSeat));assert.equal(seekPausedAfter.position,seekPaused.position,'cursor moved after Pause canceled seek');
  assert.ok(seekPaused.position>=readyToSeek.min&&seekPaused.position<=readyToSeek.position,'Pause did not retain the reached native seek position');
  report.seekPause={target:seekTarget,positionBeforeSeek:seekStart.position,positionAtPauseRequest:seekProgress.position,
   positionAfterPause:seekPaused.position,seekingBeforePause:true,seekingAfterPause:seekPaused.seeking,pausedAfterPause:seekPaused.paused};
  await Promise.all(seats.map(seat=>healthy(seat)));await healthy(viewerSeat);
  await control(viewerSeat,'live');const returnLiveRequested=await readPlayback(frame(viewerSeat));assert.ok(returnLiveRequested,'return-to-Live request did not produce playback state');
  const returnLiveTarget=returnLiveRequested.target;await waitPosition(viewerSeat,s=>s.following&&!s.seeking&&!s.paused&&s.position>=returnLiveTarget,'return to live before source idle',30000);
   const beforeReentry=await readPlayback(frame(viewerSeat));assert.ok(beforeReentry?.run,'active viewer has no native run identity');
   const identity={clientId:viewerId,memberId:viewerMember,run:String(beforeReentry.run),lobbySerial:String(started.serial)};
  const reentries=[];let currentViewer=frame(viewerSeat),currentEpoch=epochs.get(viewerSeat);
  for(let cycle=1;cycle<=2;cycle++){
   const previousEpoch=currentEpoch;
   const before=await readPlayback(currentViewer);assert.equal(String(before.run),identity.run,'active receiver run changed before re-entry');
   await currentViewer.evaluate(()=>{clearInterval(globalThis.spectatorLabPoll);core.th11_loop_stop();core.th11_mp_stop();});
    const oldContext=pages.get(viewerSeat).browserContext();await oldContext.close();const oldContextIndex=contexts.indexOf(oldContext);if(oldContextIndex>=0)contexts.splice(oldContextIndex,1);
   await send(observerLobby,{type:'leave-spectator'},m=>m.room?.spectatorCount===0);
   const afterLeave=latest(observerLobby);assert.equal(afterLeave?.spectatorCount,0,'leave-spectator did not release the existing lobby claim');
   await send(observerLobby,{type:'spectate'},m=>m.room?.spectatorCount===1);
   const epoch=1000+cycle;currentEpoch=epoch;currentViewer=await add(viewerSeat,epoch);
   await waitPosition(viewerSeat,s=>String(s.run)===identity.run&&!s.paused&&!s.seeking&&s.position>=180,'same-identity native receiver re-entry '+cycle,120000);
   const restored=await readPlayback(currentViewer);assert.equal(String(restored.run),identity.run);assert.ok(restored.position>=restored.min);
   const pacing=await sampleOneX('same-identity re-entry '+cycle);
    reentries.push({cycle,clientId:viewerId,memberId:viewerMember,run:identity.run,previousEpoch,
     runtimeEpoch:epoch,position:restored.position,min:restored.min,following:restored.following,pacing,
     leaveSpectatorCount:0,spectatorCount:latest(observerLobby)?.spectatorCount});
   }
   for(const seat of seats)await frame(seat).evaluate(()=>core.th11_loop_stop());
   const sourceIdleStart=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4]));
   await delay(17050);
   const sourceIdleEnd=await Promise.all(seats.map(async seat=>(await readGame(frame(seat)))[4]));
   assert.deepEqual(sourceIdleEnd,sourceIdleStart,'TH11 test-scheduler hold should stop source gameplay frames');
   for(const seat of seats)await healthy(seat);await healthy(viewerSeat);
   const idleViewer=await readPlayback(frame(viewerSeat));assert.ok(idleViewer&&!idleViewer.paused&&!idleViewer.seeking,'source idle caused spectator playback failure');
   report.sourceIdle={durationMs:17050,method:'native RAF stop test-scheduler hold; netplay instance/socket retained; no pause-key or external-network claim',returnLiveTarget,
    sourceFramesBefore:sourceIdleStart,sourceFramesDuring:sourceIdleEnd,sourceStayedFixed:true,viewerPosition:idleViewer.position,
    viewerTransportOpen:(await status(viewerSeat))[19]===1,allSourcesHealthy:true};
  report.lifecycleOnly=true;report.passed=true;report.playerCount=playerCount;report.identity=identity;report.firstRuntimeEpoch=viewerSeat+1;
  report.initialHistoryOneX=initialOneX;report.explicitLive={target:liveCompleted.target,position:liveCompleted.position,edge:liveCompleted.edge,following:liveCompleted.following,passiveOneX:liveOneX};
  report.reentryCycles=reentries;report.reentryCount=reentries.length;report.testSchedulerHold=true;
  await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:true,lifecycleOnly:true,output:out,identity,reentries:reentries.length}));
 }else{
 if(fixtures&&!stageOneOnly){await control(viewerSeat,'live');await waitPosition(viewerSeat,s=>s.following&&!s.paused&&!s.seeking&&s.position>=s.edge-4,'Stage1 Live setup before source transition',30000);report.stageOneLiveSetup={following:true};}
 let firstTarget=Math.max(firstState.min+5,firstState.position-40);
 const verifyHash=async(seat,viewer,target,label)=>{
  const got=await frame(seat).evaluate(frame=>globalThis.spectatorLabHashes.get(frame)||null,target);
  const actual=await readGame(viewer);assert.ok(got,label+' lacks stored player hash at frame '+target);
  assert.equal(actual[4],target,label+' viewer stopped at a different frame');assert.equal(actual[1],got[1],label+' canonical gameplay hash mismatch');
  (report.canonicalComparisons??=[]).push({label,frame:target,viewerHash:actual[1],sourceHash:got[1],sources:playerCount});
  for(const sourceSeat of seats){const peer=await frame(sourceSeat).evaluate(frame=>globalThis.spectatorLabHashes.get(frame)||null,target);assert.ok(peer,label+' lacks peer source hash');assert.equal(peer[1],got[1],label+' source players differ');}
 };
 const verifyRange=async(viewer,start,end,label)=>{
  assert.ok(fixtures,'per-tick range proof requires private native hooks');
  const take=async f=>f.evaluate(({start,end})=>Array.from({length:end-start},(_,i)=>spectatorLabHashes.get(start+i+1)?.[1]??null),{start,end});
  const actual=await take(viewer);for(const sourceSeat of seats){const expected=await take(frame(sourceSeat));
   for(let n=0;n<actual.length;n++){assert.notEqual(actual[n],null,label+' missing viewer frame '+(start+n+1));assert.notEqual(expected[n],null,label+' missing source frame '+(start+n+1));assert.equal(actual[n],expected[n],label+' source '+sourceSeat+' diverged at '+(start+n+1));}}
  (report.frameRangeComparisons??=[]).push({label,start:start+1,end,frames:end-start,sources:playerCount,matched:true});
 };
 assert.ok(await sfxStarts(frame(0))>0,'active source never started an SFX');
 const runStageOneControls=async()=>{
  await control(viewerSeat,'pause');const paused=await readPlayback(firstViewer),pausedSfx=await sfxStarts(firstViewer);await new Promise(r=>setTimeout(r,400));
  const pausedAgain=await readPlayback(firstViewer);assert.equal(pausedAgain.position,paused.position);assert.ok(pausedAgain.received>paused.received);assert.equal(await sfxStarts(firstViewer),pausedSfx,'paused spectator started a sound');
  if(!fixtures){firstTarget=Math.max(firstState.min+5,paused.position-40);const all=await Promise.all(seats.map(seat=>frame(seat).evaluate(()=>Array.from(spectatorLabHashes.keys()))));const shared=all[0].filter(value=>value>=firstState.min+5&&value<=firstTarget&&all.every(list=>list.includes(value)));assert.ok(shared.length,'no shared production hash sample');firstTarget=Math.max(...shared);}
  await control(viewerSeat,'seek',firstTarget);
  await waitPosition(viewerSeat,s=>s.position===firstTarget&&!s.seeking,'first backwards seek');await verifyHash(0,firstViewer,firstTarget,'stage-one seek');assert.equal(await sfxStarts(firstViewer),pausedSfx,'historical seek started a sound');report.silentPauseAndSeek=true;
  await control(viewerSeat,'play');await waitPosition(viewerSeat,s=>s.position>firstTarget,'play after seek');
  await wait(async()=>await sfxStarts(firstViewer)>pausedSfx,'audible playback resumes after seek',30000);report.audioResumesAfterSeek=true;
  await control(viewerSeat,'live');await waitPosition(viewerSeat,s=>s.following&&!s.seeking&&s.position>=s.edge-4,'first follow-live',30000);
 };
 if(stageOneOnly||!fixtures)await runStageOneControls();
 let secondViewer=firstViewer,node;
 if(!stageOneOnly){
 phase('stage-two-publication');
 if(fixtures){const values=await Promise.all(seats.map(s=>readGame(frame(s)))),next=Math.max(...values.map(v=>v[4]))+180;controlsAtSourceTransition=playbackControlCount;for(const seat of seats)await frame(seat).evaluate(value=>globalThis.spectatorLabNext=value,next);report.nativeTransitionFixture={frame:next,method:'StageCompletion NextStage through GameSession update; no direct stage increment',driverSeats:seats,viewerTransitionInput:false,viewerControlCountAtTrigger:controlsAtSourceTransition};}
 const stageTwoTimeout=Number(process.env.TH11_STAGE_TWO_TIMEOUT_MS||900000);
 await wait(async()=>{
  for(const seat of seats){const h=await health(frame(seat));if(h.failed||h.error||h.nativeError)throw Error('TH11 player failed '+JSON.stringify(h));}
  const h=await health(firstViewer);if(h.failed)throw Error('stage publication viewer stopped '+JSON.stringify(h));const state=await readPlayback(firstViewer);return state?.nodes.some(n=>n.kind==='stage'&&n.stage===2);
 },'stage two checkpoint published',stageTwoTimeout);
 const viewerState=await readPlayback(firstViewer);node=viewerState.nodes.find(n=>n.kind==='stage'&&n.stage===2);report.stageTwoNode=node;report.hostStageTwoCheckpoint=await readSpectatorDiagnostics(frame(0));phase('stage-two-history-and-controls');
 if(fixtures){await waitPosition(viewerSeat,s=>{const segment=s.nodes.find(n=>n.kind==='stage'&&n.stage===2);if(!segment)return false;assert.ok(s.following,'observer lost Following across automatic Stage2 checkpoint');assert.equal(s.paused,false,'observer paused before the first Stage2 user control');assert.equal(s.seeking,false,'observer sought before automatic Stage2 crossing completed');return segment.id===node.id&&s.position>node.frame;},'automatic Stage2 segment follows while continuously Following',stageTwoTimeout);
  const automatic=await readPlayback(firstViewer);await wait(async()=>{for(const seat of seats){const has=await frame(seat).evaluate(target=>spectatorLabHashes.has(target),automatic.position);if(!has)return false;}return true;},'source hashes available at automatic Stage2 viewer frame',30000);
  await verifyHash(0,firstViewer,automatic.position,'automatic Stage2 segment same-frame canonical');const controlsSinceSourceTransition=playbackControlCount-controlsAtSourceTransition;assert.equal(controlsSinceSourceTransition,0,'viewer controls must remain untouched from source Stage2 trigger through automatic canonical comparison');report.automaticStageTwo={nodeFrame:node.frame,viewerFrame:automatic.position,following:automatic.following,paused:automatic.paused,seeking:automatic.seeking,sourceCount:playerCount,canonicalMatched:true,controlsSinceSourceTransition};
  await runStageOneControls();
 }
 await waitPosition(viewerSeat,s=>s.edge>node.frame+620,'confirmed stage-two history',180000);
 await control(viewerSeat,'pause');await control(viewerSeat,'seek',node.frame);await waitPosition(viewerSeat,s=>s.position===node.frame&&!s.seeking,'stage-two checkpoint restore');
 await verifyHash(0,firstViewer,node.frame,'stage-two constructor checkpoint');
 await control(viewerSeat,'seek',node.frame+600);await waitPosition(viewerSeat,s=>s.position===node.frame+600&&!s.seeking,'stage-two forward reconstruction');
 await verifyHash(0,firstViewer,node.frame+600,'stage-two reconstructed state');if(fixtures)await verifyRange(firstViewer,node.frame,node.frame+600,'first viewer Stage2 every tick');
 await control(viewerSeat,'seek',firstTarget);await waitPosition(viewerSeat,s=>s.position===firstTarget&&!s.seeking,'cross-stage backward seek');
 await verifyHash(0,firstViewer,firstTarget,'backward seek across stage boundary');await control(viewerSeat,'live');
 phase('second-independent-stage-two-viewer');
 const lateId='spectator_late_'+roomCode,lateMember='spectator_late_member_'+roomCode;
 const lateLobby=await open(new URLSearchParams({room,lobby:lateId,member:lateMember,intent:'join'}));lobbies.push(lateLobby);
 await send(lateLobby,{type:'spectate'},m=>m.room?.spectatorCount===2);
 options.push({...options[viewerSeat],netplaySpectatorId:lateId,netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member:lateMember})});
 secondViewer=await add(lateSeat);
 await waitPosition(lateSeat,s=>s.min===node.frame&&s.position>node.frame,'late viewer starts at stage-two checkpoint',120000);
 await recordGraphics(lateSeat);
 await control(lateSeat,'pause');await control(lateSeat,'seek',node.frame);await waitPosition(lateSeat,s=>s.position===node.frame&&!s.seeking,'late stage-two exact restore');
 await verifyHash(0,secondViewer,node.frame,'independent late stage-two constructor');
 await control(lateSeat,'seek',node.frame+600);await waitPosition(lateSeat,s=>s.position===node.frame+600&&!s.seeking,'late stage-two replay');
 await verifyHash(0,secondViewer,node.frame+600,'independent late stage-two replay');if(fixtures)await verifyRange(secondViewer,node.frame,node.frame+600,'late viewer Stage2 every tick');
 await control(lateSeat,'play');await waitPosition(lateSeat,s=>s.position>node.frame+600,'late viewer play');await control(lateSeat,'live');
 await waitPosition(lateSeat,s=>s.following&&!s.seeking&&s.position>=s.edge-4,'late viewer live follow',30000);
 }
 const followStart=(await readGame(frame(0)))[4];let maxLag=0,samples=0;
 await wait(async()=>{const state=await readPlayback(secondViewer);assert.ok(state.following&&!state.seeking);maxLag=Math.max(maxLag,state.edge-state.position);samples++;return (await readGame(frame(0)))[4]>=followStart+120;},'sustained live after 120 source frames',180000);
 assert.ok(maxLag<=4,'sustained live lag '+maxLag);report.sustainedLive={sourceFrames:120,maxLag,samples};
 assert.deepEqual(errors,[]);report.passed=true;report.stageTwoNode=node;report.firstViewer=await readPlayback(firstViewer);report.secondViewer=await readPlayback(secondViewer);
 report.independentBrowserContexts=contexts.length;report.privateSpectatorFixture=fixtures;report.privateSpectatorKeepAlive=!!build.privateSpectatorKeepAlive;report.privateSpectatorMirroredTransition=!!build.privateSpectatorMirroredTransition;report.privateSpectatorSourceOnlyTransition=!!build.privateSpectatorSourceOnlyTransition;report.stageOneOnly=stageOneOnly;report.fullStageBoundaryAcceptance=fixtures&&!stageOneOnly;await pages.get(stageOneOnly?viewerSeat:lateSeat).screenshot({path:resolve(out,'spectator-stage-two.png'),fullPage:true});
 await pages.get(0).screenshot({path:resolve(out,'source-current.png'),fullPage:true});report.sourceScreenshotFrame=(await readGame(frame(0)))[4];
 await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({passed:true,output:out,stageTwoNode:node,wasm:build.sha256}));
 for(const f of instances)try{await f.evaluate(()=>{clearInterval(globalThis.spectatorLabPoll);core.th11_loop_stop();core.th11_mp_stop();});}catch{}
 for(const context of contexts)await context.close();
 }
}catch(error){try{report.failureInstances=await collectFailure?.();}catch{}report.error=String(error.stack||error);await writeFile(resolve(out,'failure.txt'),report.error);await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');throw error;
}finally{for(const socket of sockets)socket.close();await browser?.close();server.close();relay.kill();await writeFile(resolve(out,'relay.log'),relayLog);}
