import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream} from 'node:fs';
import {readFile,writeFile,mkdir,stat} from 'node:fs/promises';
import {resolve,relative,extname} from 'node:path';
import {spawn,execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {createHash,randomUUID,randomInt} from 'node:crypto';
import {launchBrowser} from '../browser/launch.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

const arg=(name,fallback)=>process.argv.find(v=>v.startsWith(`--${name}=`))?.slice(name.length+3)??fallback;
assert.ok(arg('manifest')&&arg('output'),'Required: --manifest=private-runtime-inputs.json --output=results');
const root=resolve(import.meta.dirname,'../..'),output=resolve(arg('output'));
const config=JSON.parse(await readFile(resolve(arg('manifest')),'utf8'));
const {build}=createRequire(resolve(root,'launcher/package.json'))('esbuild');
const optionBundle=await build({entryPoints:[resolve(root,'launcher/src/launcher/multiplayer-runtime-options.mts')],bundle:true,platform:'node',format:'esm',write:false});
const {buildMultiplayerRuntimeOptions}=await import('data:text/javascript;base64,'+Buffer.from(optionBundle.outputFiles[0].text).toString('base64'));
const games=arg('game')?.split(','),players=Number(arg('players','2')),route=arg('route','rtc'),mode=arg('challenge','both');
const duration=Number(arg('frames','1200'));
const waitEnd=process.argv.includes('--wait-end');
const stopStream=process.argv.includes('--stop-stream');
const recover=process.argv.includes('--recover');
const restart=arg('restart','none');
assert.ok(['none','quick','menu','both'].includes(restart));
assert.ok([2,3].includes(players)&&['rtc','relay'].includes(route)&&['normal','challenge','both'].includes(mode));
assert.ok(Number.isSafeInteger(duration)&&duration>=300,'--frames must be at least 300');
const targets=config.targets.filter(t=>t.variant==='multiplayer'&&PRODUCT_GAMES[t.game]?.multiplayer?.gameplay==='cooperative'&&(!games||games.includes(t.game)));
assert.ok(targets.length,'No cooperative multiplayer Runtime selected');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
await mkdir(output,{recursive:true});
const report={schema:'eagler-touhou/mp-browser/1',startedAt:new Date().toISOString(),players,route,frames:duration,
 waitEnd,stopStream,recover,restart,
 scope:'Actual native MP games and independent spectators through a real local relay and the production Runtime protocol',
 commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),harnessSha256:sha(await readFile(new URL(import.meta.url))),cases:[]};
report.relayConfig={freshPerCase:true,rtcTimeoutMs:30000,stunUrls:[]};
await writeFile(resolve(output,'harness.mjs'),await readFile(new URL(import.meta.url)));
const patch=execFileSync('git',['diff','--binary','HEAD'],{cwd:root,maxBuffer:16*1024*1024});
report.patchSha256=sha(patch);await writeFile(resolve(output,'source.patch'),patch);
report.untrackedSources={};
for(const name of execFileSync('git',['ls-files','--others','--exclude-standard','--','common','tools/ports'],{cwd:root,encoding:'utf8'}).trim().split('\n').filter(Boolean)){
 const bytes=await readFile(resolve(root,name));report.untrackedSources[name]={bytes:bytes.length,sha256:sha(bytes)};
 await writeFile(resolve(output,'source-'+name.replaceAll('/','_')),bytes);
}
const save=()=>writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');
const WebSocket=createRequire(resolve(root,'services/relay/package.json'))('ws');
let relayLog='',relay,relayPort;
async function startRelay(){
const wrapper=resolve(output,'relay-behavior-process.mjs');
await writeFile(wrapper,`import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';
const {WebSocketServer}=createRequire(${JSON.stringify(resolve(root,'services/relay/package.json'))})('ws');
const viewers=new Map(),emit=WebSocketServer.prototype.emit;let dropOpening=false;
WebSocketServer.prototype.emit=function(type,...args){if(type==='connection'){const [socket,request]=args,id=new URL(request.url,'http://localhost').searchParams.get('spectator');if(id){viewers.set(id,socket);socket.on('close',()=>{if(viewers.get(id)===socket)viewers.delete(id);});if(dropOpening){dropOpening=false;socket.terminate();}}}return emit.call(this,type,...args);};
process.on('message',message=>{if(message.operation==='drop-opening'){dropOpening=true;process.send({id:message.id,armed:true});return;}if(message.operation!=='drop-spectator')return;const socket=viewers.get(message.spectator);socket?.terminate();process.send({id:message.id,dropped:!!socket});});
await import(pathToFileURL(${JSON.stringify(resolve(root,'services/relay/src/netplay-relay.mjs'))}).href);
`);
const child=spawn(process.execPath,[wrapper],{cwd:root,windowsHide:true,
 env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'30000',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});
child.stderr.on('data',b=>relayLog+=b);let startup='';
const port=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+startup)),15000);
 child.once('error',no);child.stdout.on('data',b=>{relayLog+=b;startup+=b;const m=startup.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m){clearTimeout(timer);yes(Number(m[1]));}});});
return {child,port};
}
const shared=[];
for(const [name,file] of Object.entries(config.shared??{}))shared.push({path:'/'+name,url:'/shared/'+name,bytes:(await stat(file)).size});
const server=createServer(async(req,res)=>{try{
 const url=new URL(req.url,'http://localhost'),t=targets.find(t=>url.pathname===`/host/${t.id}`);
 if(t){res.setHeader('Content-Type','text/html');res.end(`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#222}iframe{width:960px;height:720px;border:0}</style><script>
 window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data/${t.id}')).arrayBuffer();return {buffer,bytes:buffer.byteLength}};
 addEventListener('message',e=>{if(e.source===document.querySelector('iframe')?.contentWindow&&e.origin===location.origin)events.push(e.data)});
 window.rpc=(command,fields={})=>new Promise((yes,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'))},120000);
 function listen(e){if(e.source===document.querySelector('iframe').contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error))}}addEventListener('message',listen);
 document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:${JSON.stringify(t.game)},epoch:1,request,command,...fields},location.origin)});
 </script><iframe src="/runtime/${t.id}/${t.entry}?hosted=1&managedData=1&gameGeneration=mp-behavior&runtimeVariant=multiplayer&runtimeEpoch=1"></iframe>`);return;}
 let file;
 for(const target of targets){if(url.pathname===`/data/${target.id}`)file=target.data;
  const prefix=`/runtime/${target.id}/`;if(url.pathname.startsWith(prefix)){file=resolve(target.runtime,decodeURIComponent(url.pathname.slice(prefix.length)));assert.ok(!relative(resolve(target.runtime),file).startsWith('..'));}}
 if(url.pathname.startsWith('/shared/'))file=config.shared?.[url.pathname.slice(8)];
 if(url.pathname==='/favicon.ico'){res.writeHead(204).end();return;}
 if(!file){res.writeHead(404).end();return;}
 const info=await stat(file);res.writeHead(200,{'Content-Type':({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.css':'text/css','.wasm':'application/wasm'})[extname(file)]??'application/octet-stream','Content-Length':info.size});createReadStream(file).pipe(res);
 }catch(error){res.writeHead(404).end(String(error));}});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
const base=`http://127.0.0.1:${server.address().port}`;
async function relayFault(operation,fields={}){
 const id=randomUUID();return new Promise((yes,no)=>{const timer=setTimeout(()=>{relay.off('message',receive);no(Error('Relay fault control timeout'));},10000);
  const receive=message=>{if(message.id!==id)return;clearTimeout(timer);relay.off('message',receive);yes(message);};
  relay.on('message',receive);relay.send({operation,...fields,id});});
}
async function until(operation,label,timeout=120000){const deadline=Date.now()+timeout;while(true){const result=await operation();if(result)return result;if(Date.now()>deadline)throw Error('Timeout: '+label);await new Promise(r=>setTimeout(r,100));}}
async function lobby(room,client,member,create=false){
 const socket=new WebSocket(`ws://127.0.0.1:${relayPort}/?`+new URLSearchParams({room,lobby:client,member,intent:create?'create':'join',players:String(players),visibility:'private',difficulty:'1'}));
 socket.messages=[];socket.on('message',b=>{try{socket.messages.push(JSON.parse(String(b)));}catch{}});
 await until(()=>{const error=socket.messages.find(m=>m.type==='error');if(error)throw Error(JSON.stringify(error));
  if(socket.readyState===WebSocket.CLOSED)throw Error('Lobby closed before admission');return socket.messages.find(m=>m.type==='state');},'lobby join');return socket;
}
async function send(socket,value,predicate){const start=socket.messages.length;socket.send(JSON.stringify(value));return until(()=>{
 const messages=socket.messages.slice(start),error=messages.find(m=>m.type==='error');if(error)throw Error(JSON.stringify(error));return messages.find(predicate);},value.type);}
try{
 for(const t of targets)for(const challenge of mode==='both'?[false,true]:[mode==='challenge']){
  assert.ok(PRODUCT_GAMES[t.game].multiplayer.playerCounts.includes(players));
  const row={game:t.game,challenge,players,route,steps:[],errors:[],httpErrors:[]};report.cases.push(row);
  const id=`${t.game}-${players}p-${challenge?'challenge':'normal'}-${route}`;
  const sockets=[],contexts=[],endpoints=[];let browser;
  const step=async(name,fn)=>{row.stage=name;row.steps.push({name,at:new Date().toISOString()});await save();return fn();};
  const inspect=async endpoint=>endpoint.page.evaluate(()=>{
   const events=window.events.filter(e=>e?.protocol==='eagler-touhou/1');
   const r=document.querySelector('iframe')?.contentWindow;
   const runtime=r?.[Object.keys(r).find(k=>/^__th\d+Runtime$/.test(k))],module=runtime?.Module??r?.Module,core=runtime?.core??r?.core??module;
   const peer=r?.__eaglerPeerTransport??r?.[Object.keys(r).find(k=>/^__th\d+PeerTransport$/.test(k))];
   let native=null;
   for(const name of ['th11_mp_status','_th15_mp_status'])if(typeof core?.[name]==='function')native=Array.from(new Uint32Array(core.memory?.buffer??module.HEAPU8.buffer,core[name](),24));
   return {playback:[...events].reverse().find(e=>e.event==='runtime-info'&&e.spectatorPlayback)?.spectatorPlayback??null,
    frame:r?.__eaglerNetplayLanFrame??native?.[3]??null,
    failed:!!r?.__eaglerNetplayFailed||native?.[23]===1||peer?.failed===true,
    error:r?.__eaglerNetplayError||(native?.[23]===1||peer?.failed===true?String(peer?.error||'native multiplayer failure'):''),native,
    state:runtime?.status?.()??null,transport:peer?{route:peer.route,failed:peer.failed,error:peer.error,closed:peer.closed,peers:peer.peers?.size}:null,
    spectatorSession:peer?.spectatorSession??null,receiveEpoch:peer?.receiveEpoch??0,reconnectAttempt:peer?.reconnectAttempt??0,
    body:r?.document.body.innerText??'',options:module?.eaglerOptions??null,terminal:events.filter(e=>e.event==='error'||e.event==='exit')};
  });
  const complete=state=>state.playback?.ended&&state.playback.position>=state.playback.received;
  const healthy=state=>{assert.equal(state.failed,false,state.error||state.transport?.error||'Native multiplayer failed');assert.equal(state.error,'');
   for(const event of state.terminal)assert.ok(complete(state)&&event.event==='exit'&&event.status==='success'&&(!event.code||event.code===0),JSON.stringify(event));};
  const progress=async(endpoint,predicate,label)=>until(async()=>{const state=await inspect(endpoint);healthy(state);return state.playback&&predicate(state)?state:null;},label);
  try{
   const localRelay=await startRelay();relay=localRelay.child;relayPort=localRelay.port;
   await step('runtime-identity',async()=>{row.runtime={};const inventoryFile=resolve(t.runtime,'runtime-files.json');let files;
    try{files=JSON.parse(await readFile(inventoryFile)).files;}catch(error){if(error.code!=='ENOENT')throw error;files=Object.fromEntries([t.entry,...t.files].map(name=>[name,null]));}
    for(const [name,expected]of Object.entries(files)){const bytes=await readFile(resolve(t.runtime,name));row.runtime[name]={bytes:bytes.length,sha256:sha(bytes)};if(expected){assert.equal(sha(bytes),expected.sha256,name);assert.equal(bytes.length,expected.bytes,name);}}
   });
   browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});report.browser=browser.version();
   const room=t.game+'mp-'+randomInt(1000,9999),members=[];
   for(let seat=0;seat<players;seat++){const member=randomUUID().replaceAll('-',''),client='mp_'+member;members.push({member,client,seat});
    const socket=await lobby(room,client,member,seat===0);sockets.push(socket);await send(socket,{type:'take-seat',seat,loadout:seat},m=>m.room?.seats?.[seat]?.clientId===client);}
   await send(sockets[0],{type:'settings',playerCount:players,difficulty:1,visibility:'private',challengeMode:challenge},m=>m.room?.challengeMode===challenge);
   for(let seat=0;seat<players;seat++)await send(sockets[seat],{type:'set-ready',ready:true},m=>m.room?.seats?.[seat]?.ready);
   await until(()=>sockets[0].messages.at(-1)?.room?.seats.slice(0,players).every(s=>s?.ready),'all ready');
   const timing=PRODUCT_GAMES[t.game].multiplayer.inputTiming?.measuredStartup
     ? {adonisMode:t.game==='th15'?1:0,inputDelay:t.game==='th15'?2:0,inputDelayAuto:false,predictionReserve:2} : {};
   const started=await send(sockets[0],{type:'start',...timing},m=>m.type==='start');row.room={room,run:started.serial};
   const options={playerCount:players,difficulty:1,seed:417,
    loadouts:PRODUCT_GAMES[t.game].multiplayer.loadouts.slice(0,players).map(l=>({character:l.character,shot:l.shot})),
    ...timing,spectatorCount:1,spectatorTimeline:true,challengeMode:challenge,iceServers:[]};
   const add=async(member,spectator=false,launchNow=true)=>{
    const context=await browser.newContext({viewport:{width:1000,height:760}});contexts.push(context);
    if(route==='relay')await context.addInitScript(()=>Object.defineProperty(window,'RTCPeerConnection',{configurable:true,value:undefined}));
    const page=await context.newPage(),endpoint={page,member,spectator,seat:spectator?null:member.seat};endpoints.push(endpoint);
    page.on('pageerror',error=>row.errors.push(String(error)));page.on('response',r=>{if(r.status()>=400)row.httpErrors.push({url:r.url(),status:r.status()});});
    await page.goto(`${base}/host/${t.id}`);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:120000});
    endpoint.frame=page.frames().find(f=>f.url().includes(`/runtime/${t.id}/`));assert.ok(endpoint.frame);
    const nativeOptions={runtimeVariant:'multiplayer',thpracEnabled:false,touchEnabled:false,renderScale:1,...buildMultiplayerRuntimeOptions({...options,
      player:endpoint.seat??0,spectator,spectatorId:spectator?member.client:'',url:`ws://127.0.0.1:${relayPort}/?`+new URLSearchParams({room,run:String(started.serial),member:member.member})},PRODUCT_GAMES[t.game].multiplayer)};
    await page.evaluate(({options,sharedResources})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],thpracCatalog:[],options,sharedResources}),{options:nativeOptions,sharedResources:shared});
    if(launchNow)await page.evaluate(()=>rpc('launch'));return endpoint;
   };
   const sources=await step('players-start',async()=>{const sources=await Promise.all(members.map(member=>add(member,false,false)));
    await Promise.all(sources.map(e=>e.page.evaluate(()=>rpc('launch'))));
    for(const e of sources)await e.page.evaluate(game=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:1,command:'keyboard',code:'KeyZ',key:'z',keyCode:90,location:0,inputSource:'physical',down:true},location.origin),t.game);
    row.startup=await Promise.all(sources.map(inspect));await save();
    for(const [i,e]of sources.entries())await e.page.screenshot({path:resolve(output,`${id}-startup-${i}.png`)});
    await until(async()=>{const states=await Promise.all(sources.map(inspect));states.forEach(healthy);return states.every(s=>s.frame>=180);},'players frame 180');
    row.playersReady=await Promise.all(sources.map(inspect));
    for(const state of row.playersReady)assert.equal(state.transport?.route,route,'Actual player route must match the requested test path');
    return sources;});
   const join=async(existing)=>{const member={member:randomUUID().replaceAll('-',''),client:'viewer_'+randomUUID().replaceAll('-','')};
    const e=await add(member,true,false);
    // Preparation may outlive the source game. Check before requesting a new
    // admission, just as the launcher checks current room availability.
    if(existing&&(await inspect(existing)).playback?.ended)return null;
    const socket=await lobby(room,member.client,member.member);sockets.push(socket);await send(socket,{type:'spectate'},m=>m.type==='spectator-start');
    await e.page.evaluate(()=>rpc('launch'));await progress(e,s=>s.playback.position>=s.playback.min+120&&!s.playback.seeking,'spectator history playback');return e;};
   if(recover)assert.equal((await relayFault('drop-opening')).armed,true);
   const viewer=await step('join-during-play',join);row.initial=await inspect(viewer);
   if(recover)assert.ok(row.initial.reconnectAttempt>=1,'recover an actual receiver loss before its first session descriptor');
   const control=async(action,frame)=>{const state=await inspect(viewer);assert.ok(state.playback);await viewer.page.evaluate(({run,action,frame})=>rpc('spectator-playback',{run,action,...(frame===undefined?{}:{frame})}),{run:state.playback.run,action,frame});};
   await step('pause-seek-resume',async()=>{
    await control('pause');const paused=await progress(viewer,s=>s.playback.paused,'pause');const before=await Promise.all(sources.map(inspect));
    await viewer.page.waitForTimeout(700);const held=await inspect(viewer);healthy(held);assert.equal(held.playback.position,paused.playback.position);
    const after=await Promise.all(sources.map(inspect));assert.ok(after.every((s,i)=>s.frame>before[i].frame),'viewer pause must not pause players');
    const target=paused.playback.min+20;await control('seek',target);await progress(viewer,s=>!s.playback.seeking&&s.playback.position===target&&s.playback.paused,'backward replay seek');
    await control('play');await progress(viewer,s=>!s.playback.paused&&s.playback.position>target+60,'resume');row.pause={paused:paused.playback,held:held.playback,target};
   });
   await step('upload-backpressure-recovery',async()=>{
    const before=await inspect(viewer),sourceBefore=await Promise.all(sources.map(inspect));
    await sources[0].frame.evaluate(name=>{const s=globalThis[name]??globalThis.__eaglerPeerTransport,socket=s.relay;globalThis.mpUploadWait={reads:0};
     const release=()=>{delete socket.bufferedAmount;mpUploadWait.finished=performance.now();};const unused=setTimeout(release,5000);
     Object.defineProperty(socket,'bufferedAmount',{configurable:true,get(){if(!mpUploadWait.reads++){clearTimeout(unused);mpUploadWait.started=performance.now();setTimeout(release,150);}return 70000;}});},PRODUCT_GAMES[t.game].multiplayer.peerTransportGlobal);
    const after=await progress(viewer,s=>s.playback.received>before.playback.received+120&&s.playback.position>before.playback.position+120,'upload recovery');
    const fault=await sources[0].frame.evaluate(name=>{const s=globalThis[name]??globalThis.__eaglerPeerTransport;return {...mpUploadWait,stopped:!!s.spectatorStopped,failed:!!s.failed};},PRODUCT_GAMES[t.game].multiplayer.peerTransportGlobal);
    assert.ok(fault.reads>0&&fault.finished>fault.started);assert.equal(fault.stopped,false);assert.equal(fault.failed,false);
    const sourceAfter=await Promise.all(sources.map(inspect));sourceAfter.forEach(healthy);assert.ok(sourceAfter.every((s,i)=>s.frame>sourceBefore[i].frame+30));
    row.backpressure={before,after,fault,sourceBefore,sourceAfter};
   });
   await step('sustained-playback',async()=>{const start=(await inspect(viewer)).playback.position;
    row.sustained=await progress(viewer,s=>s.playback.position>=start+duration||complete(s),'sustained native playback');});
   if(recover)await step('spectator-connection-recovery',async()=>{
    const before=await inspect(viewer),sourcesBefore=await Promise.all(sources.map(inspect));
    const {dropped}=await relayFault('drop-spectator',{spectator:viewer.member.client});
    assert.equal(dropped,true,'terminate the actual receiver socket at the real Relay');
    const after=await progress(viewer,s=>s.receiveEpoch>before.receiveEpoch&&s.playback.received>before.playback.received+120&&s.playback.position>=s.playback.min+120&&!s.playback.seeking,'recovered native playback');
    assert.equal(after.spectatorSession.session,before.spectatorSession.session);
    assert.ok(after.playback.revision>before.playback.revision,'same-run UI revision continues across receiver recovery');
    const sourcesAfter=await Promise.all(sources.map(inspect));sourcesAfter.forEach(healthy);
    assert.ok(sourcesAfter.every((s,i)=>s.frame>sourcesBefore[i].frame));
    row.recovery={before,after,sourcesBefore,sourcesAfter};
   });
   if(restart!=='none')for(const method of restart==='both'?['quick','menu']:[restart])await step('restart-'+method,async()=>{
    if(t.game==='th06'&&method==='menu'){(row.inapplicableActions??=[]).push({action:'restart-menu',reason:'The native pause menu has Resume and Quit; Restart uses R.'});return;}
    const before=await inspect(viewer),sourceBefore=await inspect(sources[0]);
    const key=async(code,down,endpoint=sources[0])=>endpoint.page.evaluate(({game,code,down})=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:1,command:'keyboard',code,key:code==='Escape'?'Escape':code==='KeyR'?'r':code==='KeyZ'?'z':'ArrowUp',keyCode:({Escape:27,KeyR:82,KeyZ:90,ArrowUp:38})[code],location:0,inputSource:'physical',down},location.origin),{game:t.game,code,down});
    const press=async code=>{await key(code,true);await sources[0].page.waitForTimeout(150);await key(code,false);await sources[0].page.waitForTimeout(200);};
    // Shared native menus merge seat controls. Release every shoot key so
    // the selecting seat can produce a real Z edge in that merged input.
    for(const e of sources)await key('KeyZ',false,e);
    await press('Escape');await sources[0].page.waitForTimeout(650);
    if(method==='quick')await press('KeyR');
    else{await press('ArrowUp');await press('KeyZ');await sources[0].page.waitForTimeout(700);await press('ArrowUp');await press('KeyZ');}
    const sourceAfter=await until(async()=>{const state=await inspect(sources[0]);healthy(state);return state.spectatorSession?.generation>sourceBefore.spectatorSession?.generation&&state.frame>=180?state:null;},'new source native generation');
    for(const e of sources)await key('KeyZ',true,e);
    const after=await progress(viewer,s=>s.spectatorSession?.session===sourceAfter.spectatorSession.session&&s.playback.run.startsWith('s'+sourceAfter.spectatorSession.session+'_')&&s.receiveEpoch>before.receiveEpoch&&s.playback.position>=s.playback.min+120&&!s.playback.seeking,'viewer resumes after '+method+' restart');
    (row.restarts??=[]).push({method,before,sourceBefore,sourceAfter,after});
   });
   const late=await step('late-join-after-recovery',()=>join(viewer));
   if(late){row.late=await inspect(late);assert.equal(row.late.playback.run,(await inspect(viewer)).playback.run);}
   if((waitEnd&&!challenge)||(await inspect(viewer)).playback.ended)await step('normal-end-buffer-drain',async()=>{
    if(!complete(await inspect(viewer))){
     await control('pause');const paused=await progress(viewer,s=>s.playback.paused,'pause before completion');
     const ended=await progress(viewer,s=>s.playback.ended,'source normal End');
     assert.equal(ended.playback.position,paused.playback.position,'completed upload preserves paused playback');
     row.completedWhilePaused=ended;await control('play');
    }
    row.completed=await progress(viewer,complete,'consume complete replay tail');
   });
   await step('screenshots',async()=>{for(const [i,e]of endpoints.entries())await e.page.screenshot({path:resolve(output,`${id}-${e.spectator?'viewer':'player'}-${i}.png`)});});
   if(stopStream){
    assert.equal((await inspect(viewer)).playback.ended,false,'forced stream failure requires a live source game');
    await step('stream-failure-isolation',async()=>{
     const before=await Promise.all(sources.map(inspect));
     await sources[0].frame.evaluate(name=>{const state=globalThis[name]??globalThis.__eaglerPeerTransport;state.signal.send(JSON.stringify({type:'spectator-stop',generation:state.spectatorSession?.generation}));},PRODUCT_GAMES[t.game].multiplayer.peerTransportGlobal);
     const stopped=[];
     for(const e of [viewer,late].filter(Boolean))stopped.push(await until(async()=>{
      const state=await inspect(e);if(!state.failed||!state.terminal.length)return null;
      assert.ok(state.terminal.some(event=>event.event==='error'||event.event==='exit'&&event.status!=='success'),'native receiver must report abnormal stream termination');return state;
     },'abnormal spectator exit'));
     const after=await until(async()=>{const states=await Promise.all(sources.map(inspect));states.forEach(healthy);return states.every((s,i)=>s.frame>before[i].frame+30)?states:null;},'players survive spectator termination');
     row.streamStopped={before,stopped,after};
    });
   }
   assert.deepEqual(row.errors,[]);assert.deepEqual(row.httpErrors,[]);row.passed=true;
  }catch(error){row.passed=false;row.error=String(error.stack||error);row.failure=[];for(const [i,e]of endpoints.entries())try{row.failure.push(await inspect(e));await e.page.screenshot({path:resolve(output,`${id}-failure-${i}.png`)});}catch{}
  }finally{for(const c of contexts)await c.close().catch(()=>{});await browser?.close();for(const s of sockets)s.close();
   await until(()=>sockets.every(s=>s.readyState===WebSocket.CLOSED),'lobby cleanup',10000);relay?.kill();await save();console.log(`${id} ${row.passed?'PASS':'FAIL'} ${row.stage} ${row.error?.split('\n')[0]??''}`);}
 }
}finally{server.close();relay?.kill();report.finishedAt=new Date().toISOString();await writeFile(resolve(output,'relay.log'),relayLog);await save();}
if(report.cases.some(c=>!c.passed))process.exitCode=1;
