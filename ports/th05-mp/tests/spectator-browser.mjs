import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {WebSocket} from 'ws';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const repo=resolve(import.meta.dirname,'../../..'),out=resolve(process.env.TH05_TEST_OUTPUT??resolve(repo,'ports/th05-mp/artifacts/multiplayer-browser'));await mkdir(out,{recursive:true});
const count=Number(process.env.TH05_MP_PLAYERS??2),route=process.env.TH05_MP_ROUTE??'rtc',mode=Number(process.env.TH05_MP_MODE??1),jitter=process.env.TH05_MP_JITTER==='1';
let log='',port;const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'30000'}});
relay.stdout.on('data',b=>{log+=b;const m=log.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m)port=Number(m[1]);});relay.stderr.on('data',b=>log+=b);
const wait=async(check,label)=>{const end=Date.now()+15000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,10));}};
const roomId='th05mp-native-'+Date.now().toString(36),generation='a'.repeat(64),members=Array.from({length:count},(_,i)=>'native_member_'+i+'_'+Date.now().toString(36));
async function lobby(seat){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,lobby:'native_client_'+seat,member:members[seat],players:String(count),difficulty:'1'})}`),messages=[];sockets.push(ws);ws.on('message',b=>messages.push(JSON.parse(String(b))));await wait(()=>messages.some(m=>m.type==='state'),'lobby');return {messages,send:m=>ws.send(JSON.stringify(m)),room:()=>messages.findLast(m=>m.room)?.room};}
let browser,site;const report={count,route,mode,jitter};
try{
 await wait(()=>port,'relay startup');const clients=[];
 for(let seat=0;seat<count;seat++){const client=await lobby(seat);clients.push(client);client.send({type:'take-seat',seat,loadout:seat,name:'Native '+seat});await wait(()=>client.room()?.seats[seat],'seat');if(seat===0){client.send({type:'settings',playerCount:count,difficulty:1});await wait(()=>client.room()?.playerCount===count,'capacity');}}
 for(const client of clients)client.send({type:'set-ready',ready:true,runtimeGeneration:generation,movementMode:'touch'});
 await wait(()=>clients[0].room()?.seats.slice(0,count).every(s=>s?.ready),'ready');clients[0].send({type:'start',adonisMode:mode,inputDelay:0,inputDelayAuto:mode!==0,predictionLimit:8});
 await wait(()=>clients[0].messages.some(m=>m.type==='start'),'run');const serial=clients[0].messages.find(m=>m.type==='start').serial;
 site=await multiplayerFixture(count);browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1320,height:1020}}),errors=[];
 if(route==='relay')await page.addInitScript(()=>{delete window.RTCPeerConnection;});
 page.on('pageerror',e=>errors.push(e.message));await page.goto(site.url);await page.waitForFunction(count=>events.filter(e=>e.event==='ready').length===count,count,{timeout:30000});
 const configure=async seat=>{const opts={netplayMode:'lan',netplayUrl:`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,run:String(serial),member:members[seat],runtimeGeneration:generation})}`,netplayPlayer:seat,netplayPlayerCount:count,netplaySeed:739,netplayDifficulty:1,netplayInputDelay:0,netplayInputDelayAuto:mode!==0,netplayAdonisMode:mode,netplayPredictionLimit:8,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),touchEnabled:true,touchMovementMode:'touch-unlimited'};
  const result=await page.evaluate(({seat,opts})=>command(seat,'configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:opts}),{seat,opts});assert.equal(result.ok,true,JSON.stringify(result));
  const preview=await page.evaluate(seat=>command(seat,'prepare-room'),seat);assert.equal(preview.presented,true,JSON.stringify(preview));};
 for(let seat=0;seat<count;seat++)await configure(seat);
 const launch=await page.evaluate(count=>Promise.all(Array.from({length:count},(_,seat)=>command(seat,'launch',{launchSerial:1}))),count);assert.ok(launch.every(r=>r.ok),JSON.stringify(launch));
 await page.waitForFunction(count=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.__eaglerNetplayLanFrame>250),count,{timeout:30000});
 report.running=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>({frame:f.contentWindow.__eaglerNetplayLanFrame,route:f.contentWindow.__eaglerNetplayTransport,inputDelay:f.contentWindow.__eaglerNetplayInputDelay,saveRoot:f.contentWindow.Module.FS.readlink('/savesth05')})));

 const viewerId='observer_'+Date.now().toString(36),viewerMember='observer_member_'+Date.now().toString(36),messages=[];
 const lobbyViewer=new WebSocket(`ws://127.0.0.1:${port}/?`+new URLSearchParams({room:roomId,lobby:viewerId,member:viewerMember}));sockets.push(lobbyViewer);lobbyViewer.on('message',b=>messages.push(JSON.parse(String(b))));await wait(()=>messages.some(m=>m.type==='state'),'viewer lobby');lobbyViewer.send(JSON.stringify({type:'spectate'}));await wait(()=>messages.some(m=>m.type==='spectator-start'),'spectator admission');
 await page.evaluate(count=>{const iframe=document.createElement('iframe');iframe.width=640;iframe.height=480;iframe.src='/runtime/th05.html?runtimeEpoch='+(7+count)+'&managedData=1&multiplayer=1';document.body.append(iframe);},count);await page.waitForFunction(count=>events.filter(e=>e.event==='ready').length===count+1,count);
 const opts={netplayMode:'lan',netplayUrl:`ws://127.0.0.1:${port}/?`+new URLSearchParams({room:roomId,run:String(serial),member:viewerMember,runtimeGeneration:generation}),netplayPlayer:0,netplayPlayerCount:count,netplaySpectator:true,netplaySpectatorId:viewerId,netplaySpectatorTimeline:true,netplaySeed:739,netplayDifficulty:1,netplayAdonisMode:mode,netplayPredictionLimit:8,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),touchEnabled:true,touchMovementMode:'touch-unlimited'};
 const config=await page.evaluate(({count,opts})=>command(count,'configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:opts}),{count,opts});assert.equal(config.ok,true,JSON.stringify(config));
 const started=await page.evaluate(count=>command(count,'launch',{launchSerial:1}),count);assert.equal(started.ok,true,JSON.stringify(started));
 await page.waitForFunction(count=>{const windows=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow),viewer=windows[count];return viewer.__eaglerNetplayFrame>360&&viewer.__eaglerNetplayFrame>=windows[0].__eaglerNetplayFrame-16;},count,{timeout:30000});
 report.spectator=await page.evaluate(count=>{const windows=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow),viewer=windows[count],f=Math.floor((viewer.__eaglerNetplayFrame-8)/120)*120;return {receiveSocketOpen:viewer.__th05PeerTransport.relay.readyState===1,receiveOnly:viewer.__th05PeerTransport.peers.size===0,frame:viewer.__eaglerNetplayFrame,hostFrame:windows[0].__eaglerNetplayFrame,checkedFrame:f,hostHash:windows[0].Module._th05_net_hash(f),viewerHash:viewer.__th05SpectatorHashes.get(f),stock:Array.from({length:count},(_,s)=>viewer.Module._th05_coop_value(s,0))};},count);assert.ok(report.spectator.receiveSocketOpen);assert.ok(report.spectator.receiveOnly);assert.ok(report.spectator.hostHash);assert.equal(report.spectator.viewerHash,report.spectator.hostHash,JSON.stringify(report.spectator));
 const viewerReport=()=>page.evaluate(count=>events.findLast(e=>e.epoch===7+count&&e.spectatorPlayback)?.spectatorPlayback,count);
 const control=async(action,frame)=>{const state=await viewerReport();const result=await page.evaluate(({count,run,action,frame})=>command(count,'spectator-playback',{run,action,frame}),{count,run:state.run,action,frame});assert.equal(result.ok,true,JSON.stringify(result));};
 await control('pause');const paused=await viewerReport();await page.waitForTimeout(200);const still=await viewerReport();assert.equal(still.position,paused.position);assert.ok(still.received>paused.received);
 // Read-only input is checked while paused, where any mutation is visible.
 const checksum=()=>page.evaluate(count=>document.querySelectorAll('iframe')[count].contentWindow.Module._th05_coop_checksum(),count);
 const beforeInput=await checksum();await page.evaluate(count=>command(count,'keyboard',{code:'ArrowRight',down:true}),count);await page.waitForTimeout(100);await page.evaluate(count=>command(count,'keyboard',{code:'ArrowRight',down:false}),count);assert.equal(await checksum(),beforeInput);
 await control('seek',240);await page.waitForFunction(count=>{const s=events.findLast(e=>e.epoch===7+count&&e.spectatorPlayback)?.spectatorPlayback;return s?.position===240&&!s.seeking;},count);
 assert.equal(await checksum(),report.spectator.hostHash,'Local seek reconstructs the confirmed native world');
 await control('seek',0);await page.waitForFunction(count=>{const s=events.findLast(e=>e.epoch===7+count&&e.spectatorPlayback)?.spectatorPlayback;return s?.position===0&&!s.seeking;},count);
 await control('play');await page.waitForFunction(count=>events.findLast(e=>e.epoch===7+count&&e.spectatorPlayback)?.spectatorPlayback.position>=20,count);
 await control('live');await page.waitForFunction(count=>{const s=events.findLast(e=>e.epoch===7+count&&e.spectatorPlayback)?.spectatorPlayback;return s?.following&&!s.seeking&&s.received-s.position<16;},count,{timeout:30000});
 report.timeline={paused:paused.position,receivedWhilePaused:still.received,seek:240,restart:0,live:await viewerReport(),readOnly:true};
 // Stop owned loops before exercising native stage boundaries independently.
 await page.evaluate(count=>command(count,'return-room'),count);
 for(let seat=0;seat<count;seat++)await page.evaluate(seat=>command(seat,'return-room'),seat);
 report.boundaries=await page.evaluate(count=>{
  const windows=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow),host=windows[0].Module,viewer=windows[count].Module,result=[];
  if(!host._th05_coop_start_stock(count,0,1,739,0,1,2,0,0,4,3))throw Error('Native checkpoint start failed');
  function load(){const size=host._th05_spectator_size(),ptr=viewer._malloc(size);try{viewer.HEAPU8.set(host.HEAPU8.subarray(host._th05_spectator_state(),host._th05_spectator_state()+size),ptr);if(!viewer._th05_spectator_load(ptr,size))throw Error('Native checkpoint load failed');}finally{viewer._free(ptr);}}
  function check(){if(host._th05_coop_checksum()!==viewer._th05_coop_checksum())throw Error('Stage checkpoint checksum differs');}
  load();check();
  for(let stage=0;stage<3;stage++){
   for(let tick=0;tick<180;tick++){for(let seat=0;seat<count;seat++){const key=32|(tick%60<30?4:8);host._th05_coop_input(seat,key,0,0,0);viewer._th05_coop_input(seat,key,0,0,0);}host._th05_tick(0);viewer._th05_tick(0);check();}
   const progress=(host._th05_native_coop_probe(38,0,0)+14)%32;
   for(let item=0;item<7;item++){
    host._th05_native_coop_probe(2,item%count,2);
    viewer._th05_native_coop_probe(2,item%count,2);
   }
   if(host._th05_native_coop_probe(38,0,0)!==progress)throw Error('Dream items did not share Rank progress');
   check();
   const before=host._th05_spectator_serial();host._th05_native_coop_probe(12,0,0);
   let ticks=0;while(host._th05_spectator_serial()===before&&ticks++<240)host._th05_tick(0);
   if(host._th05_spectator_serial()===before)throw Error('Stage did not transition');load();check();
   if(viewer._th05_native_coop_probe(38,0,0)!==progress)throw Error('Stage checkpoint lost Rank progress');
   result.push({stage:stage+1,transitionTicks:ticks,bytes:host._th05_spectator_size(),checksum:host._th05_coop_checksum()});
  }
  const size=host._th05_spectator_size();
  const legacy=host.HEAPU8.slice(host._th05_spectator_state(),host._th05_spectator_state()+size);
  const header=new DataView(legacy.buffer);
  if(header.getUint32(0,true)!==0x35535634)throw Error('Current combat rules were not recorded');
  const ptr=viewer._malloc(size);
  try{
   for(const revision of [0x35535633,0x35535632,0x35535631]){
    header.setUint32(0,revision,true);
    viewer.HEAPU8.set(legacy.subarray(0,size-4),ptr);
    if(!viewer._th05_spectator_load(ptr,size-4))throw Error('Legacy stage checkpoint rejected');
    const magic=new DataView(viewer.HEAPU8.buffer).getUint32(viewer._th05_spectator_state(),true);
    if(magic!==revision)throw Error('Legacy combat rules were not retained');
   }
  }finally{viewer._free(ptr);}
  return result;
 },count);
 report.errors=errors;report.runtimeErrors=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(errors,[]);assert.deepEqual(report.runtimeErrors,[]);report.passed=true;await writeFile(resolve(out,'spectator.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report,null,2));
}catch(error){console.error(report,log);throw error;}finally{await browser?.close();await site?.close();for(const ws of sockets)ws.terminate();relay.kill();}
