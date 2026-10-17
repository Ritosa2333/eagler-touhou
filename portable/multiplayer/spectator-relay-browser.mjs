// Real shared relay + production shell. --fixtures substitutes the private
// canonical-state probe, whose native game code is linked from production objects.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {resolve,extname} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),launcher=resolve(process.env.TH15_LAUNCHER_ROOT);
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const runtime=resolve(process.env.TH15_TEST_RUNTIME),fixtures=process.argv.includes('--fixtures');
const buildRoot=resolve(root,'th15_web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer');
const probes=resolve(buildRoot,'spectator-stage');
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const productionBuild=JSON.parse(await readFile(resolve(buildRoot,'build.json'),'utf8'));
const productionManifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
const productionWasm=await readFile(resolve(runtime,'th15.wasm'));
const productionLoader=await readFile(resolve(runtime,'th15.mjs'));
const productionLoaderSha=sha256(await readFile(resolve(buildRoot,'th15-application.mjs')));
assert.equal(productionBuild.features?.multiplayer,true,'TH15_OUTPUT must be a multiplayer build');
assert.equal(sha256(productionWasm),productionBuild.sha256,'packaged Runtime WASM must match TH15_OUTPUT/build.json');
assert.equal(productionManifest.execution?.sha256,productionBuild.sha256,'production manifest must name TH15_OUTPUT WASM');
assert.equal(sha256(productionLoader),productionLoaderSha,'packaged Runtime loader must match TH15_OUTPUT loader');
assert.equal(productionManifest.execution?.loaderSha256,productionLoaderSha,'production manifest must name TH15_OUTPUT loader');
const fixtureBuild=fixtures?JSON.parse(await readFile(resolve(probes,'build.json'),'utf8')):null;
if(fixtures){
 assert.equal(fixtureBuild.productionWasm,productionBuild.sha256,'spectator fixture must link to this TH15_OUTPUT production WASM');
 assert.equal(fixtureBuild.productionLoaderSha256,productionLoaderSha,'spectator fixture must link to this TH15_OUTPUT loader');
 assert.equal(sha256(await readFile(resolve(probes,'th15.wasm'))),fixtureBuild.sha256,'spectator fixture WASM must match its build.json');
 assert.equal(sha256(await readFile(resolve(probes,'th15.mjs'))),fixtureBuild.loaderSha256,'spectator fixture loader must match its build.json');
}
const buildIdentity={productionWasm:productionBuild.sha256,productionLoader:productionLoaderSha,
 productionManifestWasm:productionManifest.execution.sha256,productionManifestLoader:productionManifest.execution.loaderSha256,
 fixtureWasm:fixtureBuild?.sha256??null,fixtureProductionWasm:fixtureBuild?.productionWasm??null};
const out=resolve(probes,process.env.TH15_TEST_EVIDENCE_NAME||(fixtures?'relay-fixture':'relay-production'));await mkdir(out,{recursive:true});
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);let options=[],relayLog='',browser;
const sharedFonts=resolve(process.env.TH15_TEST_SHARED_FONTS||resolve(root,'../dist/ui-rewrite-fixed-20261010/shared'));
const relay=spawn(process.execPath,[resolve(launcher,'server/netplay-relay.mjs')],{cwd:launcher,windowsHide:true,
 env:{...process.env,EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
relay.stderr.on('data',b=>relayLog+=b);
const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+relayLog)),15000);relay.stdout.on('data',b=>{relayLog+=b;const m=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m){clearTimeout(timer);yes(Number(m[1]));}});relay.once('exit',code=>{clearTimeout(timer);no(Error('Relay exited '+code+' '+relayLog));});});
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=seat=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th15.html?managedData=1&hosted=1&runtimeEpoch='+(seat+1);document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+seat),request=crypto.randomUUID();const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},60000);function listen(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',listen);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:seat+1,request,command,...fields},location.origin);});
window.launchRuntime=async seat=>{const o=await(await fetch('/options')).json();await rpc(seat,'configure',{music:'none',options:o[seat],sharedResources:['msgothic.ttc','unifont.otf'].map(name=>({path:'/'+name,url:'/shared/'+name}))});await rpc(seat,'launch');};
</script>`;
const httpFailures=[];
const server=createServer(async(req,res)=>{let path='/';try{path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(path==='/data'){res.end(archive);return;}if(path==='/options'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(options));return;}
 if(path==='/shared/msgothic.ttc'||path==='/shared/unifont.otf'){res.end(await readFile(resolve(sharedFonts,path.slice(8))));return;}
 if(!path.startsWith('/runtime/')||path.includes('..')){httpFailures.push({path,status:404,reason:'not-a-runtime-route'});res.writeHead(404).end();return;}
 const name=path.slice(9),assetName=name==='th15-application.wasm'?'th15.wasm':name;
 const fixtureAsset=fixtures&&(/^th15\.(?:mjs|wasm)$/.test(name)||name==='th15-application.wasm');
 const file=resolve(fixtureAsset?probes:runtime,assetName);
 if(!file.startsWith(runtime+'\\')&&!file.startsWith(probes+'\\')&&!file.startsWith(runtime+'/')&&!file.startsWith(probes+'/'))throw Error('Unsafe path');
 res.setHeader('Content-Type',({'.mjs':'application/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(e){httpFailures.push({path,status:404,reason:String(e)});res.writeHead(404).end(String(e));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));const base='http://127.0.0.1:'+server.address().port,sockets=[],reports=[],endpointIdentity=[];
async function wait(check,label,ms=15000){const until=Date.now()+ms;for(;;){if(await check())return;if(Date.now()>until)throw Error(label+' timeout '+relayLog);await new Promise(r=>setTimeout(r,20));}}
async function open(query){const ws=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+query),r={ws,messages:[],closed:false};sockets.push(ws);ws.addEventListener('message',e=>{if(typeof e.data==='string')r.messages.push(JSON.parse(e.data));});ws.addEventListener('close',()=>r.closed=true);await wait(()=>ws.readyState===1||r.closed,'lobby connect');assert.equal(ws.readyState,1);return r;}
const latest=r=>r.messages.findLast(m=>m.room)?.room;
async function send(r,message,predicate){const at=r.messages.length;r.ws.send(JSON.stringify(message));await wait(()=>r.messages.slice(at).some(m=>m.type==='error'||predicate(m)),'lobby '+message.type);const error=r.messages.slice(at).find(m=>m.type==='error');if(error)throw Error(error.error);return r.messages.slice(at).find(predicate);}
try{
 const rendererArgs=process.env.TH15_TEST_SOFTWARE_RENDERER==='1'?['--use-angle=swiftshader','--enable-unsafe-swiftshader']:['--enable-gpu','--use-gl=angle','--use-angle=d3d11'];
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required',...rendererArgs,'--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 for(const count of (process.env.TH15_TEST_PLAYER_COUNTS||'2,3').split(',').map(Number)){
  assert.ok(count===2||count===3);
  const room='th15mp-'+(7400+count),members=[],lobbies=[];
  for(let seat=0;seat<count;seat++){
   const client='spectator_lab_'+count+'_'+seat,member='spectator_member_'+count+'_'+seat;members.push({client,member});
   const lobby=await open(new URLSearchParams({room,lobby:client,member,intent:seat?'join':'create',players:String(count),difficulty:'1'}));lobbies.push(lobby);
   await send(lobby,{type:'take-seat',seat,loadout:seat},m=>m.room?.seats?.[seat]?.clientId===client);
  }
  const directory=await open('directory=1&member=stage_directory_'+count);await wait(()=>directory.messages.some(m=>m.type==='directory'),'directory');
  const snapshot=directory.messages.findLast(m=>m.type==='directory');assert.ok(snapshot.rooms.some(r=>r.code===String(7400+count)&&r.players===count&&r.phase==='lobby'&&!r.joinable),'full waiting room remains discoverable for observer admission');
  // The waiting-room observer occupies no seat; its Runtime joins only later.
  const viewerId='stage_waiting_'+count,viewerMember='stage_waiting_member_'+count;
  const observer=await open(new URLSearchParams({room,lobby:viewerId,member:viewerMember,intent:'join'}));lobbies.push(observer);
  await send(observer,{type:'spectate'},m=>m.room?.spectatorCount===1);assert.deepEqual(latest(observer).seats.slice(0,count).map(s=>s.clientId),members.map(m=>m.client));
  for(let seat=0;seat<count;seat++)await send(lobbies[seat],{type:'set-ready',ready:true},m=>m.room?.seats?.[seat]?.ready);
  await wait(()=>latest(lobbies[0]).seats.slice(0,count).every(s=>s?.ready),'all ready');
  const started=await send(lobbies[0],{type:'start',adonisMode:1,inputDelay:2,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
  options=Array.from({length:count+1},(_,seat)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member:seat<count?members[seat].member:viewerMember}),netplayPlayer:seat<count?seat:null,netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:417,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),netplayInputDelay:2,netplayInputDelayAuto:false,netplayAdonisMode:1,netplayPredictionLimit:0,netplayPredictionReserve:2,netplaySpectator:seat===count,netplaySpectatorId:seat===count?viewerId:'',netplaySpectatorCount:1,netplayChallengeMode:false,thpracEnabled:false,renderScale:1}));
  // Each participant has its own browser context and renderer. Loading a new
  // WASM/font set must not block the existing players' or viewer's event loop.
  const contexts=[],pages=new Map(),errors=[],endpoints=[];
  const frame=seat=>pages.get(seat).frames().find(f=>f.url().includes('runtimeEpoch='+(seat+1)));
  const add=async seat=>{console.log('TH15 '+count+'p loading seat '+seat);const context=await browser.createBrowserContext();contexts.push(context);const page=await context.newPage();pages.set(seat,page);
   const endpoint={count,seat,productionManifestWasm:productionBuild.sha256,productionManifestLoader:productionLoaderSha,
    expectedWasm:fixtures?fixtureBuild.sha256:productionBuild.sha256,expectedLoader:fixtures?fixtureBuild.loaderSha256:productionLoaderSha};endpoints.push(endpoint);endpointIdentity.push(endpoint);
   page.on('response',response=>{const path=new URL(response.url()).pathname;
    if(path==='/runtime/th15-application.wasm'||path==='/runtime/th15.wasm'){
     endpoint.wasmRequest=path;endpoint.wasmStatus=response.status();
     void response.buffer().then(bytes=>{endpoint.wasmBytes=bytes.length;endpoint.wasm=sha256(bytes);}).catch(error=>{endpoint.responseError=String(error);});
    }
    if(path==='/runtime/th15.mjs'){endpoint.loaderStatus=response.status();void response.buffer().then(bytes=>{endpoint.loaderBytes=bytes.length;endpoint.loader=sha256(bytes);}).catch(error=>{endpoint.responseError=String(error);});}
   });
   page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser:',m.text());});await page.goto(base,{waitUntil:'domcontentloaded',timeout:120000});await page.evaluate(s=>addRuntime(s),seat);await page.waitForFunction(s=>ready.has(s+1),{timeout:120000},seat);
   const f=frame(seat),endpointManifest=await f.evaluate(async()=>{const r=await fetch('./manifest.json');return r.ok?await r.json():null;});
   assert.equal(endpointManifest?.execution?.sha256,productionBuild.sha256,'seat '+seat+' served production manifest identity');
   assert.equal(endpointManifest?.execution?.loaderSha256,productionLoaderSha,'seat '+seat+' served production loader identity');
   if(fixtures)await frame(seat).evaluate(()=>{Module.spectatorLabStates=new Map();Module.spectatorLabTrace=frame=>{const p=Module._th15_spectator_lab_network_state(frame)>>>2;Module.spectatorLabStates.set(frame,Array.from(Module.HEAPU32.subarray(p,p+16)));if(frame===Module.spectatorLabNext&&!Module._th15_spectator_lab_next())throw Error('Native next stage failed');};});
   await page.evaluate(s=>launchRuntime(s),seat);await wait(()=>endpoint.wasm&&endpoint.loader,'seat '+seat+' runtime binary identity');
   assert.equal(endpoint.wasmStatus,200,'seat '+seat+' WASM response must be HTTP 200 at '+endpoint.wasmRequest);
   assert.equal(endpoint.loaderStatus,200,'seat '+seat+' loader response must be HTTP 200');
   assert.equal(endpoint.wasm,endpoint.expectedWasm,'seat '+seat+' loaded expected WASM');assert.equal(endpoint.loader,endpoint.expectedLoader,'seat '+seat+' loaded expected loader');
   endpoint.productionManifestMatched=true;endpoint.loadedWasm=endpoint.wasm;endpoint.loadedLoader=endpoint.loader;
  };
  for(let seat=0;seat<count;seat++)await add(seat);
  console.log('TH15 '+count+'p players launched');
  const observing=setInterval(async()=>{try{console.log(JSON.stringify(await Promise.all(Array.from({length:count},(_,seat)=>frame(seat).evaluate(()=>{const p=Module._th15_mp_status()>>>2,c=Module._th15_mp_calibration_status()>>>2;return {status:Array.from(Module.HEAPU32.subarray(p,p+24)),calibration:Array.from(Module.HEAPU32.subarray(c,c+32)),phase:Module._th15_phase(),error:document.querySelector('#error').textContent,transport:globalThis.__eaglerPeerTransport?.route};})))));}catch{}},10000);
  try{await frame(0).waitForFunction(()=>{const p=Module._th15_mp_status()>>>2;return Module.HEAPU32[p+3]>=300||Module.HEAPU32[p+22]||globalThis.__eaglerNetplayFailed||document.querySelector('#error').textContent;},{timeout:60000});}finally{clearInterval(observing);}
  assert.equal(await frame(0).evaluate(()=>{const p=Module._th15_mp_status()>>>2;return Module.HEAPU32[p+22];}),0,await frame(0).evaluate(()=>{const p=Module._th15_spectator_lab_publication?.();return p?new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p))):'P1 optional spectator publication failed';}));
  for(let seat=0;seat<count;seat++)assert.equal(await frame(seat).$eval('#error',e=>e.textContent),'');
  await add(count);const viewer=frame(count);
  const read=f=>f.evaluate(()=>{const error=document.querySelector('#error').textContent;if(error)throw Error('Native spectator: '+error);const p=Module._th15_mp_spectator_state();return p?{...JSON.parse(new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p)))),...(Module.spectatorLabPlayback?{probe:Module.spectatorLabPlayback}:{})}:null;});
  await wait(async()=>{const s=await read(viewer);return s&&s.position>=150;},'late native restore',60000);
  const initialViewerState=await read(viewer);
  let playbackControlCount=0,controlsAtSourceTransition=null;
  const control=async(action,target)=>{playbackControlCount++;const state=await read(viewer);return pages.get(count).evaluate(({count,state,action,target})=>rpc(count,'spectator-playback',{run:state.run,action,...(target===undefined?{}:{frame:target})}),{count,state,action,target});};
  assert.ok(initialViewerState&&!initialViewerState.paused&&!initialViewerState.seeking,'late viewer must start in playable history state');
  await new Promise(r=>setTimeout(r,350));const entryProgress=await read(viewer);
  assert.equal(entryProgress.paused,false,'late viewer history playback remains unpaused');assert.equal(entryProgress.seeking,false,'late viewer history playback is not seeking');
  assert.ok(entryProgress.received>initialViewerState.received,'late viewer keeps receiving confirmed history');
  assert.ok(entryProgress.position>initialViewerState.position,'late viewer advances through its initial history target');
  await control('live');await wait(async()=>{const s=await read(viewer);return s.following&&!s.paused&&!s.seeking&&s.position>=s.edge-4;},'Stage1 Live setup before controls or source transition',30000);
  const stageOneLiveSetup={initialFollowing:initialViewerState.following,initialTarget:initialViewerState.target,initialPosition:initialViewerState.position,
   initialReceived:initialViewerState.received,historyAdvancedTo:entryProgress.position,followingAfterLive:true};
  const compare=async(f,target)=>{const sample=await f.evaluate(()=>{const p=Module._th15_mp_spectator_state(),state=JSON.parse(new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p)))),at=state.position,q=Module._th15_spectator_lab_network_state(at)>>>2;return {frame:at,state,actual:Array.from(Module.HEAPU32.subarray(q,q+16))};});
   if(target!==undefined)assert.equal(sample.frame,target,'canonical snapshot position');
   assert.equal(sample.actual[3],sample.frame,'canonical native frame must match captured playback position');
   await wait(async()=>{for(let seat=0;seat<count;seat++)if(!await frame(seat).evaluate(frame=>Module.spectatorLabStates.has(frame),sample.frame))return false;return true;},'source hashes at canonical snapshot frame '+sample.frame,30000);
   for(let seat=0;seat<count;seat++)assert.deepEqual(sample.actual,await frame(seat).evaluate(frame=>Module.spectatorLabStates.get(frame),sample.frame),'canonical equality seat '+seat+' frame '+sample.frame);return sample;};
  let followSamples=0,maxFollowLag=0,target=initialViewerState.min+Math.min(30,initialViewerState.edge-initialViewerState.min),automaticStageTwo=null;
  const runControls=async(stageOneTarget)=>{
   await control('pause');const paused=await read(viewer);await new Promise(r=>setTimeout(r,350));const pausedAfter=await read(viewer);assert.equal(pausedAfter.position,paused.position);assert.ok(pausedAfter.received>paused.received);
   if(stageOneTarget===undefined)target=paused.min+Math.min(30,paused.edge-paused.min);
   await control('seek',target);await wait(async()=>{const s=await read(viewer);return s.position===target&&!s.seeking;},'backward seek');if(fixtures)await compare(viewer,target);
   await control('play');await wait(async()=> (await read(viewer)).position>target,'play');await control('live');await wait(async()=>{const s=await read(viewer);return s.following&&!s.seeking&&s.position>=s.edge-4;},'follow live',30000);
   const followStart=await frame(0).evaluate(()=>Module.HEAPU32[(Module._th15_mp_status()>>>2)+3]);
   await wait(async()=>{const s=await read(viewer);assert.ok(s.following&&!s.seeking&&!s.paused);++followSamples;maxFollowLag=Math.max(maxFollowLag,s.edge-s.position);const source=await frame(0).evaluate(()=>Module.HEAPU32[(Module._th15_mp_status()>>>2)+3]);return source>=followStart+120&&s.position>=s.edge-4;},'sustained live follow',60000);
   assert.ok(maxFollowLag<=16,'live playback accumulated '+maxFollowLag+' frames of lag');
  };
  if(!fixtures)await runControls();
  if(fixtures){
   const scheduled=await frame(0).evaluate(()=>Module.HEAPU32[(Module._th15_mp_status()>>>2)+3]+180);
   controlsAtSourceTransition=playbackControlCount;
   for(let seat=0;seat<count;seat++)await frame(seat).evaluate(value=>Module.spectatorLabNext=value,scheduled);
   await wait(async()=> (await read(viewer))?.nodes.some(n=>n.kind==='stage'&&n.stage===2),'native stage two publication',60000);
   const node=(await read(viewer)).nodes.find(n=>n.kind==='stage'&&n.stage===2);
   await wait(async()=>{const s=await read(viewer),segment=s?.nodes.find(n=>n.kind==='stage'&&n.stage===2);if(!segment)return false;assert.ok(s.following,'observer lost Following across automatic Stage2 checkpoint');assert.equal(s.paused,false,'observer paused before first user control');return segment.id===node.id&&s.position>node.frame&&!s.seeking;},'automatic Stage2 segment follows without user controls',180000);
   const automatic=await compare(viewer);
   const controlsSinceSourceTransition=playbackControlCount-controlsAtSourceTransition;assert.equal(controlsSinceSourceTransition,0,'viewer controls must remain untouched from source Stage2 trigger through automatic canonical comparison');
   automaticStageTwo={nodeFrame:node.frame,viewerFrame:automatic.frame,following:automatic.state.following,paused:automatic.state.paused,seeking:automatic.state.seeking,sourceCount:count,canonicalMatched:true,controlsSinceSourceTransition,stageOneLiveSetup:true};
   await runControls(target);
   await wait(async()=> (await read(viewer)).edge>=node.frame+620,'stage two 600-frame confirmed tail',360000);
   await control('pause');await control('seek',node.frame+30);await wait(async()=>{const s=await read(viewer);return s.position===node.frame+30&&!s.seeking;},'stage two seek');
   await compare(viewer,node.frame+30);
   await control('seek',node.frame+600);await wait(async()=>{const s=await read(viewer);return s.position===node.frame+600&&!s.seeking;},'stage two 600-frame future seek',360000);
   await compare(viewer,node.frame+600);
   await control('seek',target);await wait(async()=>{const s=await read(viewer);return s.position===target&&!s.seeking;},'return to previous stage');await compare(viewer,target);await control('live');
   const id=viewerId+'_late',member=viewerMember+'_late',late=await open(new URLSearchParams({room,lobby:id,member,intent:'join'}));lobbies.push(late);await send(late,{type:'spectate'},m=>m.room?.spectatorCount===2);
   const seat=count+1;options.push({...options[count],netplaySpectatorId:id,netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(started.serial),member})});await add(seat);const fresh=frame(seat);
   await wait(async()=>{const s=await read(fresh);return s?.min===node.frame&&s.position>node.frame;},'independent late stage two viewer',60000);
   const state=await read(fresh);await pages.get(seat).evaluate(({seat,state})=>rpc(seat,'spectator-playback',{run:state.run,action:'pause'}),{seat,state});await pages.get(seat).evaluate(({seat,state,target})=>rpc(seat,'spectator-playback',{run:state.run,action:'seek',frame:target}),{seat,state,target:node.frame+30});await wait(async()=>{const s=await read(fresh);return s.position===node.frame+30&&!s.seeking;},'late stage two exact position');await compare(fresh,node.frame+30);
   await pages.get(seat).evaluate(({seat,state,target})=>rpc(seat,'spectator-playback',{run:state.run,action:'seek',frame:target}),{seat,state:await read(fresh),target:node.frame+600});await wait(async()=>{const s=await read(fresh);return s.position===node.frame+600&&!s.seeking;},'late stage two 600-frame future seek',360000);await compare(fresh,node.frame+600);
  }
  assert.deepEqual(errors,[]);const row={count,fixtures,fullWaitingRoomObserver:true,lateJoin:true,pause:true,backwardSeek:true,stageTwoFuture600:fixtures,lateStageTwoFuture600:fixtures,automaticStageTwo,stageOneLiveSetup,play:true,live:true,followSamples,maxFollowLag,canonicalEquality:fixtures,endpoints,state:await read(viewer)};
  row.independentBrowserContexts=contexts.length;await pages.get(count).screenshot({path:resolve(out,count+'p.png'),fullPage:true});reports.push(row);console.log(JSON.stringify(row));
  for(let seat=0;seat<options.length;seat++)await frame(seat).evaluate(()=>{Module._th15_loop_stop();Module._th15_mp_close();});for(const context of contexts)await context.close();for(const socket of sockets.splice(0))socket.close();
 }
 await writeFile(resolve(out,'report.json'),JSON.stringify({status:'PASS',buildIdentity,reports,httpFailures},null,2)+'\n');
}catch(e){
 const snapshots=[];for(const page of browser?await browser.pages():[])for(const frame of page.frames()){
  try{snapshots.push({url:frame.url(),state:await frame.evaluate(()=>{if(!globalThis.Module)return null;const status=Module._th15_mp_status?.()>>>2,p=Module._th15_mp_spectator_state?.();return {status:Array.from(Module.HEAPU32.subarray(status,status+24)),state:p?JSON.parse(new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p)))):null,probe:Module.spectatorLabPlayback,error:document.querySelector('#error')?.textContent};})});}catch(error){snapshots.push({url:frame.url(),error:String(error)});}
 }
 await writeFile(resolve(out,'failure.json'),JSON.stringify({buildIdentity,endpointIdentity,httpFailures,error:String(e.stack||e),snapshots},null,2)+'\n');await writeFile(resolve(out,'failure.txt'),String(e.stack||e));throw e;
}
finally{await browser?.close();for(const socket of sockets)socket.close();relay.kill();server.close();await writeFile(resolve(out,'relay.log'),relayLog);}
