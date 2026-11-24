import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';import {once} from 'node:events';import {setTimeout as sleep} from 'node:timers/promises';
import {join,resolve} from 'node:path';import {readFile} from 'node:fs/promises';import {createHash} from 'node:crypto';import {pathToFileURL} from 'node:url';
import {makeCoopRules} from '../runtime/coop-rules.mjs';import {prepareCoopStage} from '../runtime/preview-world.mjs';import {createCoopLockstep} from '../runtime/coop-lockstep.mjs';import {openRelayLink} from '../runtime/relay-link.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';import {createTh128SpectatorEncoder,createTh128SpectatorDecoder} from '../runtime/spectator-wire.mjs';import {openTh128SpectatorRelay} from '../runtime/spectator-link.mjs';
const [relayDir,pin,js,binary]=process.argv.slice(2),config=JSON.parse(await readFile(pin));const sha=b=>createHash('sha256').update(b).digest('hex'),read=async p=>new Uint8Array(await readFile(p)),dat=await read(config.dat),wasm=await read(binary);assert.equal(sha(dat),config.datSHA256);
const overrides={bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};const build={coreSHA256:sha(wasm),resourceSHA256:sha(dat)},resources={build,read:k=>k==='wasm'?wasm:k==='dat'?dat:read(overrides[k]??config[k])};
const factory=(await import(pathToFileURL(resolve(js)))).default;const child=spawn(process.execPath,[join(resolve(relayDir),'services/relay/src/netplay-relay.mjs')],{cwd:resolve(relayDir),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''}});
const gone=once(child,'exit');let stdout='',stderr='',errors=[];child.stdout.on('data',x=>stdout=(stdout+x).slice(-30000));child.stderr.on('data',x=>stderr=(stderr+x).slice(-3000));const wait=async(fn,label)=>{const end=performance.now()+10000;do{if(errors.length)throw errors[0];let v=fn();if(v)return v;if(child.exitCode!==null)throw Error('Relay exited: '+stderr);await sleep(2);}while(performance.now()<end);throw Error('Relay timeout '+label+' '+stdout.slice(-1600)+' '+stderr);};
try{const port=Number((await wait(()=>/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(stdout),'listening'))[1]);for(const n of [2,3]){
 const room=`th128mp-${6404+n}`,base=`ws://127.0.0.1:${port}/`,lobbies=[],sig=[],links=[],sessions=[],worlds=[],tracks=[],sockets=[];const members=Array.from({length:n+1},(_,i)=>'coop_member_'+i),names=Array.from({length:n+1},(_,i)=>'coop-net-'+i);let viewerLink,viewerWorld,archiveWorld,closing=false;
 const addr=q=>base+'?'+new URLSearchParams({room,...q});
 const connect=async q=>{const ws=new WebSocket(addr(q));sockets.push(ws);const rows=[];ws.binaryType='arraybuffer';ws.addEventListener('error',()=>{if(!closing)errors.push(Error('socket: '+JSON.stringify(q)));});ws.addEventListener('message',e=>rows.push(typeof e.data==='string'?JSON.parse(e.data):new Uint8Array(e.data)));await wait(()=>ws.readyState===1,'connect');return {rows,send:v=>ws.send(JSON.stringify(v)),room:()=>rows.findLast(r=>r.room)?.room};};
 const lobby=(i,intent)=>connect({lobby:names[i],member:members[i],players:n,intent,visibility:'private',...(intent==='create'?{difficulty:n===2?1:4,challengeMode:n===3?'1':'0'}:{})});
 let receipts;
 try{
  for(let i=0;i<n;i++){lobbies.push(await lobby(i,i?'join':'create'));await wait(()=>lobbies[i].room(),'lobby');lobbies[i].send({type:'take-seat',seat:i,loadout:0});}
  const host=lobbies[0];await wait(()=>host.room()?.playerCount===n&&host.room().seats.slice(0,n).every(Boolean),'all seats');
  lobbies.forEach(l=>l.send({type:'resource-progress',status:'ready',stage:'runtime',percent:100}));await wait(()=>host.room().seats.slice(0,n).every(s=>s?.resource?.status==='ready'),'resources');const runtimeGeneration=build.coreSHA256;
  lobbies.forEach(l=>l.send({type:'set-ready',ready:true,runtimeGeneration}));await wait(()=>host.room().seats.slice(0,n).every(s=>s?.ready),'ready');host.send({type:'start',adonisMode:0,inputDelay:0,predictionLimit:2});const start=await wait(()=>host.rows.find(m=>m.type==='start'),'start');
  assert.equal(start.room.playerCount,n);assert.equal(start.room.difficulty,n===2?1:4);assert.equal(start.room.runtimeGeneration,runtimeGeneration);if(n===3)assert.equal(start.room.challengeMode,true);
  const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:`${room}-${start.serial}`,epoch:1,players:n,route:n===2?'A':'EX',difficulty:start.room.difficulty,challengeMode:start.room.challengeMode===true,inputDelay:0,rollbackWindow:2,seed:Number(room.slice(8))&65535,...build});
  let count=0;const prepared=await prepareCoopStage({rules,resources,factory,onFactory(){},onWorldStart(){count++},onTick(){}});const game=i=>({run:start.serial,player:i,member:members[i],players:n,runtimeGeneration});let encoder,late=null,rolled=false;
  for(let i=0;i<n;i++){
   const owner=await prepared.openCoopStage(rules);worlds.push(owner);const tape=await createConfirmedHistory({rules,allowedMask:763});tracks.push(tape);if(i===0)encoder=await createTh128SpectatorEncoder(tape.header,build);
   const link=openRelayLink({url:addr(game(i)),players:n,localPlayer:i,requireRoute:true,spectatorUpload:i===0,onError:e=>{if(!closing)errors.push(e);},onMessage:(msg,sender)=>{if(i===0&&sender===n-1&&msg.kind==='input'&&msg.frame===40){late=[msg,sender];return;}sessions[i].receive(msg,sender);}});links.push(link);
   sessions.push(await createCoopLockstep({rules,localSeat:i,engine:owner,send:v=>link.send(v),onConfirmed:r=>{tape.append(r);if(i===0)link.sendSpectator(encoder.frame(r));}}));
  }
  await Promise.all(links.map(l=>l.ready));for(let i=0;i<n;i++)sig.push(await connect({...game(i),signal:'1'}));for(const x of sig)x.send({type:'rtc-failed'});
  await wait(()=>sig.every(x=>x.rows.some(r=>r.type==='route'&&r.mode==='relay')),'relay mode');await Promise.all(links.map(l=>l.routeReady));sessions.forEach(s=>s.announce());await wait(()=>sessions.every(s=>s.status().ready),'ready exchange');links[0].sendSpectator(encoder.announce());links[0].sendSpectator(encoder.open());
  const decoder=createTh128SpectatorDecoder({build,matchId:rules.matchId,expectedRules:rules});let cursor=0;
  for(let f=0;f<360;f++){
   sessions.forEach((s,i)=>s.sample(f===40&&i===n-1?128:f<32?([16,64,128][i]):512));for(const s of sessions)assert.equal(s.advance(),true);
   if(f===40){await wait(()=>late,'delayed remote input');continue;}
   if(f===41){sessions[0].receive(...late);late=null;}
   await wait(()=>{sessions.forEach(s=>s.flush());return sessions.every(s=>s.status().committed===f+1);},'confirmed '+f);
   if(f===50){const v=await lobby(n,'join');v.send({type:'spectate'});await wait(()=>v.rows.some(r=>r.type==='spectator-start'),'late spectator');
    viewerLink=openTh128SpectatorRelay({url:addr({run:start.serial,players:n,spectator:names[n],member:members[n],runtimeGeneration}),players:n,decoder,onError:e=>{if(!closing)errors.push(e);}});await viewerLink.ready;await wait(()=>decoder.status().bootstrapped&&decoder.status().nextFrame===f+1,'spectator bootstrap');viewerWorld=await prepared.openCoopStage(rules);
   }
   if(f===110){while(cursor<20)viewerWorld.step(decoder.read(cursor++));assert.equal(viewerWorld.state().frame,20);}
  }
  assert.ok(sessions[0].status().replayed>0);for(const h of tracks)h.seal('completed');links[0].sendSpectator(encoder.finish('completed'));await wait(()=>decoder.status().end==='completed','spectator end');assert.equal(viewerWorld.state().frame,20);
  const saved=await readMultiplayerReplay(tracks[0].replayBytes(),build);archiveWorld=await prepared.openCoopStage(rules);
  for(let f=0;f<360;f++){if(f===cursor)viewerWorld.step(decoder.read(cursor++));archiveWorld.step(saved.read(f));}
  const final=worlds[0].state(),draw=sha(JSON.stringify(worlds[0].draws()));for(const w of [...worlds.slice(1),viewerWorld,archiveWorld]){assert.deepEqual(w.state(),final);assert.equal(sha(JSON.stringify(w.draws())),draw);}
  assert.equal(count,n+2);for(const h of tracks)assert.deepEqual(h.replayBytes(),tracks[0].replayBytes());receipts={players:n,rank:rules.difficulty,challenge:rules.challengeMode,frames:final.frame,realRelay:true,nativeWorlds:count,lateJoinFrame:51,spectatorPausedAt:20,corrected:sessions[0].status().replayed,replayBytes:tracks[0].replayBytes().length,drawSHA256:draw,wasmSHA256:build.coreSHA256};
 }finally{closing=true;viewerLink?.close();viewerWorld?.close();archiveWorld?.close();sessions.forEach(s=>s.close());links.forEach(l=>l.close());for(const ws of sockets)if(ws.readyState<2)ws.close();}
 console.log(JSON.stringify(receipts));
}}
finally{if(child.exitCode===null)child.kill();await gone;}
