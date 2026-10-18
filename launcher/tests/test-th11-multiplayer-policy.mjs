/** L3/module + local relay protocol. No game engine, browser or public-network evidence. */
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
import {PRODUCT_GAMES, multiplayerConfigForProduct, multiplayerInputTimingPolicy,
  isMultiplayerProductId, gameIdForProduct} from '../lib/contracts/product-catalog.mjs';
import {buildMultiplayerRuntimeOptions} from '../.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs';
import {normalizeMultiplayerLobbySnapshot} from '../.cache/build/browser/assets/launcher/multiplayer-lobby-snapshot.mjs';
import {isSpectatorFrameForRoom} from '../../services/relay/src/spectator-frame.mjs';

const game=PRODUCT_GAMES.th11, policy=multiplayerConfigForProduct('th11mp');
assert.equal(isMultiplayerProductId('th11mp'),true);
assert.equal(gameIdForProduct('th11mp'),'th11');
assert.equal(game.multiplayerRuntime,'./runtime/th11/multiplayer/th11.html');
assert.equal(policy.peerTransportGlobal,'__th11PeerTransport');
assert.deepEqual(policy.playerCounts,[2,3]);
assert.deepEqual(policy.difficulties,['Easy','Normal','Hard','Lunatic','Extra']);
assert.deepEqual(policy.loadouts.map(x=>[x.character,x.shot]),[[0,0],[0,1],[0,2],[1,0],[1,1],[1,2]]);
assert.equal(policy.spectator,true);assert.equal(policy.titleRoomEntry,false);assert.equal(policy.gameplay,'cooperative');
assert.equal(Object.hasOwn(policy,'challengeMode'),false,'challenge is a shared cooperative room rule');
assert.deepEqual(policy.inputTiming,{rollbackLimit:12,sendPredictionLimit:12,measuredStartup:true,manualDelayLimit:9});
assert.deepEqual(multiplayerInputTimingPolicy(policy),{measuredStartup:true,rollback:true,manualRollbackDelay:false,manualDelayLimit:9,predictionLimit:12});
for(const id of ['th06mp','th07mp','th08mp','th10mp']) assert.equal(multiplayerConfigForProduct(id).gameplay,'cooperative');
for(const id of ['th08mp','th09mp','th10mp']) assert.equal(multiplayerInputTimingPolicy(multiplayerConfigForProduct(id)).predictionLimit,8,'other title send policy unchanged');
assert.equal(game.replay.prefix,'th11');

const base={url:'ws://localhost/?room=th11mp-1234&run=1&player=0&players=2&member=member_test_1234',
  player:0,playerCount:2,seed:1234,difficulty:4,loadouts:[{character:0,shot:0},{character:1,shot:2}],
  inputDelay:0,inputDelayAuto:true,adonisMode:1,predictionReserve:2,predictionLimit:0,challengeMode:false,
  spectator:false,spectatorId:'',spectatorCount:0,iceServers:[]};
const configured=buildMultiplayerRuntimeOptions(base,policy);
assert.equal(configured.netplayAdonisMode,1);assert.equal(configured.netplayPredictionLimit,0);
assert.equal(configured.netplayPredictionReserve,2);assert.equal(configured.netplayInputDelay,0);
assert.equal(configured.netplayInputDelayAuto,true);assert.equal(configured.netplayDifficulty,4);
assert.deepEqual(configured.netplayLoadouts,base.loadouts);
assert.equal(buildMultiplayerRuntimeOptions({...base,adonisMode:undefined},policy).netplayAdonisMode,1,'unspecified legacy TH11 callers stay pure delay');
for(const challengeMode of [false,true]) for(const d of [0,1,9]){
  const options=buildMultiplayerRuntimeOptions({...base,challengeMode,inputDelayAuto:false,inputDelay:d,predictionLimit:12},policy);
  assert.equal(options.netplayChallengeMode,challengeMode);assert.equal(options.netplayInputDelay,d);
  assert.equal(options.netplayAdonisMode,1);assert.equal(options.netplayPredictionLimit,0,'room cap is not active pure-delay prediction');
}
const direct={...base,adonisMode:0,inputDelayAuto:false,predictionLimit:12};
for(const requestedMode of [0,2])for(const challengeMode of [false,true])for(const suppliedLimit of [undefined,12]){
  const options=buildMultiplayerRuntimeOptions({...direct,adonisMode:requestedMode,challengeMode,predictionLimit:suppliedLimit,inputDelay:requestedMode?9:4},policy);
  assert.equal(options.netplayAdonisMode,0,'Launcher keeps existing Direct/Pure product modes');
  assert.equal(options.netplayPredictionLimit,12);assert.equal(options.netplayInputDelay,0);
  assert.equal(options.netplayInputDelayAuto,false);assert.equal(options.netplayChallengeMode,challengeMode);
}
for(const limit of [0,1,8,11,13,1.5,NaN])assert.throws(()=>buildMultiplayerRuntimeOptions({...direct,predictionLimit:limit},policy),/TH11/);
for(const patch of [{adonisMode:3},{adonisMode:-1},{predictionReserve:0},
  {inputDelay:10,inputDelayAuto:false},{challengeMode:'true'},{difficulty:5},
  {loadouts:[{character:0,shot:0},{character:2,shot:0}]}])
  assert.throws(()=>buildMultiplayerRuntimeOptions({...base,...patch},policy),JSON.stringify(patch));
assert.throws(()=>buildMultiplayerRuntimeOptions({...direct,inputDelayAuto:true},policy),/时序/);
assert.throws(()=>buildMultiplayerRuntimeOptions({...direct,predictionLimit:8},PRODUCT_GAMES.th10.multiplayer),/TH11/,'room URL chooses actual title policy');
for(const options of [base,direct]){
  const watcher=buildMultiplayerRuntimeOptions({...options,challengeMode:true,player:null,spectator:true,spectatorId:'watcher_test_123'},policy);
  assert.equal(watcher.netplayPlayer,null);assert.equal(watcher.netplayPredictionLimit,options.adonisMode===1?0:12);assert.equal(watcher.netplayChallengeMode,true);
}
const timing={phase:'ready',automatic:true,adonisMode:1,inputDelay:3,fullDelay:3,predictionReserve:0,
  rttP95Us:90000,samples:117,lost:5,route:'rtc'};
const room={playerCount:2,difficulty:4,inputDelay:3,inputDelayAuto:true,predictionReserve:2,predictionLimit:12,
  adonisMode:1,challengeMode:false,phase:'running',timing};
const context={localClientId:'member_test_1234',...policy};
assert.deepEqual(normalizeMultiplayerLobbySnapshot(room,context).timing,timing);
assert.equal(normalizeMultiplayerLobbySnapshot(room,context).predictionLimit,12,'TH11 cap survives room normalization');
const directRoom={...room,adonisMode:0,inputDelay:0,inputDelayAuto:false,timing:null};
assert.equal(normalizeMultiplayerLobbySnapshot(directRoom,context).predictionLimit,12);
assert.equal(normalizeMultiplayerLobbySnapshot({...room,timing:{...timing,adonisMode:2,predictionReserve:2,inputDelay:1}},context),null,'measured timing mode must match room');
assert.equal(normalizeMultiplayerLobbySnapshot({...directRoom,inputDelayAuto:true},context),null,'Direct cannot pretend automatic startup');
for(const challengeMode of [false,true])assert.equal(normalizeMultiplayerLobbySnapshot({...directRoom,challengeMode},context).challengeMode,challengeMode);
function timingPacket(value,marker=0x42) {
  const bytes=new Uint8Array(40),view=new DataView(bytes.buffer);
  bytes.set([0x45,marker,0x54,0x4d,1,value.adonisMode,value.inputDelay,value.predictionReserve]);
  for(const [offset,key] of [[8,'fullDelay'],[24,'rttP95Us'],[28,'lost'],[32,'automatic'],[36,'samples']])view.setUint32(offset,Number(value[key]),true);
  return bytes;
}
for(const count of [2,3]){
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket(timing),count),true);
  for(const other of ['th08mp-1234','th10mp-1234','th09mp-1234'])assert.equal(isSpectatorFrameForRoom(other,timingPacket(timing),count),false);
  for(const marker of [0x38,0x41])assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket(timing,marker),count),false);
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket({...timing,adonisMode:2,predictionReserve:2,inputDelay:1}),count),true,'native v6 hybrid measured packet retains existing wire format');
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket({...timing,adonisMode:0}),count),false,'Direct has no fake measured-start packet');
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket({...timing,predictionReserve:1}),count),false,'pure-delay measured result must reserve zero');
  const frame=new Uint8Array(24+count*12);frame.set([0x45,0x42,0x4e,0x50,4,3,count,0]);
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',frame,count),true);
  assert.equal(isSpectatorFrameForRoom('th10mp-1234',frame,count),false);
  assert.equal(isSpectatorFrameForRoom('th11mp-1234',frame,count===2?3:2),false);
  frame[7]=1;assert.equal(isSpectatorFrameForRoom('th11mp-1234',frame,count),false);
}
assert.equal(isSpectatorFrameForRoom('th11mp-1234',timingPacket(timing),1),false);

// Keep one room per independently owned relay process. Sequential scenarios
// must not retain old rooms on one IP and trip the production room quota.
async function verifyRelayCase(count,requestedMode,automatic,code){
  const port=await new Promise((resolve,reject)=>{
    const server=net.createServer();server.once('error',reject);server.listen(0,'127.0.0.1',()=>{
      const port=server.address().port;server.close(error=>error?reject(error):resolve(port));
    });
  });
  const root=fileURLToPath(new URL('..',import.meta.url));
  const relay=spawn(process.execPath,[fileURLToPath(new URL('../../services/relay/src/netplay-relay.mjs',import.meta.url))],{
    cwd:root,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},
    stdio:['ignore','pipe','pipe'],
  });
  const relayClosed=new Promise(resolve=>relay.once('close',resolve));
  let relayLog='';
  relay.stdout.on('data',chunk=>{relayLog+=String(chunk);});
  relay.stderr.on('data',chunk=>{relayLog+=String(chunk);});
  const sockets=[];
  function nextJson(socket,predicate=()=>true){
    return new Promise((resolve,reject)=>{
      const timer=setTimeout(()=>{cleanup();reject(Error('relay response timeout\n'+relayLog));},5000);
      const onMessage=event=>{try{const message=JSON.parse(String(event.data));if(predicate(message)){cleanup();resolve(message);}}catch(error){cleanup();reject(error);}};
      const onClose=()=>{cleanup();reject(Error('relay closed before response\n'+relayLog));};
      function cleanup(){clearTimeout(timer);socket.removeEventListener('message',onMessage);socket.removeEventListener('close',onClose);}
      socket.addEventListener('message',onMessage);socket.addEventListener('close',onClose);
    });
  }
  async function connect(index,challenge){
    const query=new URLSearchParams({room:'th11mp-'+code,lobby:'th11_client_'+code+'_'+index,member:'private_th11_'+code+'_'+index,
      intent:index===0?'create':'join',players:String(count),difficulty:'4',...(challenge===undefined?{}:{challengeMode:challenge?'1':'0'})});
    const socket=new WebSocket('ws://127.0.0.1:'+port+'/?'+query);sockets.push(socket);
    const first=await nextJson(socket);
    assert.equal(first.type,'state',`room ${code} player ${index}: ${JSON.stringify(first)}`);
    assert.equal(first.room.adonisMode,0);assert.equal(first.room.predictionLimit,12);
    assert.equal(first.room.challengeMode,challenge??false);
    return socket;
  }
  async function send(socket,message,predicate){const result=nextJson(socket,predicate);socket.send(JSON.stringify(message));return result;}
  try{
    await new Promise((resolve,reject)=>{
      const cleanup=()=>{clearTimeout(timer);relay.stdout.off('data',receive);relay.off('exit',exited);relay.off('error',failed);};
      const finish=error=>{cleanup();error?reject(error):resolve();};
      const receive=()=>{if(relayLog.includes('listening'))finish();};
      const exited=exit=>finish(Error('relay exited '+exit+'\n'+relayLog));
      const failed=error=>finish(error);
      const timer=setTimeout(()=>finish(Error('relay listen timeout\n'+relayLog)),5000);
      relay.stdout.on('data',receive);relay.once('exit',exited);relay.once('error',failed);
      receive();
    });
    const peers=[];
    for(let index=0;index<count;index++){
      const peer=await connect(index,count===3?true:undefined);peers.push(peer);
      await send(peer,{type:'take-seat',seat:index,loadout:index===0?3:5},reply=>!!reply.room?.seats[index]);
    }
    if(count===3)await send(peers[0],{type:'settings',playerCount:count,difficulty:4,challengeMode:false},reply=>reply.room?.challengeMode===false);
    for(let index=0;index<count;index++)await send(peers[index],{type:'set-ready',ready:true},reply=>reply.room?.seats[index]?.ready===true);
    const changed=await send(peers[0],{type:'settings',playerCount:count,difficulty:4,challengeMode:true},reply=>reply.room?.challengeMode===true);
    assert.equal(changed.room.seats.slice(0,count).every(seat=>seat?.ready===false),true,'challenge invalidates every readiness');
    assert.equal(normalizeMultiplayerLobbySnapshot(changed.room,context).challengeMode,true);
    for(let index=0;index<count;index++)await send(peers[index],{type:'set-ready',ready:true},reply=>reply.room?.seats[index]?.ready===true);
    for(const patch of [{adonisMode:3},{predictionLimit:0},{predictionLimit:13},{predictionReserve:0},
      {inputDelay:10,inputDelayAuto:false},{adonisMode:0,inputDelayAuto:true}]){
      await send(peers[0],{type:'start',adonisMode:1,inputDelay:0,inputDelayAuto:true,predictionReserve:2,predictionLimit:12,...patch},reply=>reply.type==='error'&&/timing/.test(reply.error));
    }
    const started=await send(peers[0],{type:'start',adonisMode:requestedMode,inputDelay:automatic?0:3,inputDelayAuto:automatic,predictionReserve:2,predictionLimit:12},reply=>reply.type==='start');
    const expectedMode=requestedMode===1?1:0;
    assert.equal(started.room.adonisMode,expectedMode);assert.equal(started.room.predictionLimit,12);assert.equal(started.room.challengeMode,true);
    assert.equal(started.room.inputDelay,expectedMode===0?0:automatic?0:3);
    assert.equal(started.room.inputDelayAuto,expectedMode===0?false:automatic);
    const running=normalizeMultiplayerLobbySnapshot(started.room,context);assert.ok(running);
    assert.equal(running.predictionLimit,12,'real relay P12 survives typed lobby normalizer');
    const runtime=buildMultiplayerRuntimeOptions({...base,playerCount:count,challengeMode:running.challengeMode,
      adonisMode:running.adonisMode,inputDelay:running.inputDelay,inputDelayAuto:running.inputDelayAuto,predictionLimit:running.predictionLimit,
      loadouts:started.room.seats.slice(0,count).map(seat=>policy.loadouts[seat.loadout])},policy);
    assert.equal(runtime.netplayChallengeMode,true);assert.equal(runtime.netplayPredictionLimit,expectedMode===0?12:0);
    assert.equal(runtime.netplayAdonisMode,expectedMode);
    const result={...timing,automatic,route:'relay'};
    await send(peers[0],{type:'timing-result',serial:started.serial,timing:{...result,adonisMode:2,inputDelay:1,predictionReserve:2}},reply=>reply.type==='error');
    if(expectedMode===1){
      const confirmed=await send(peers[0],{type:'timing-result',serial:started.serial,timing:result},reply=>!!reply.room?.timing);
      assert.equal(confirmed.room.timing.predictionReserve,0);assert.equal(confirmed.room.timing.adonisMode,1);
      assert.equal(confirmed.room.inputDelay,3);assert.equal(confirmed.room.playerCount,count);assert.equal(confirmed.room.challengeMode,true);
    }else{
      await send(peers[0],{type:'timing-result',serial:started.serial,timing:result},reply=>reply.type==='error');
      assert.equal(runtime.netplayInputDelay,0);assert.equal(runtime.netplayInputDelayAuto,false);
    }
    console.log(`TH11 local relay ${count}P requestMode=${requestedMode} automatic=${automatic} room=${code} PASS`);
  }finally{
    for(const socket of sockets)socket.close();
    if(relay.exitCode===null&&relay.signalCode===null)relay.kill();
    await relayClosed;
  }
}
for(const fixture of [[2,0,false,'9101'],[3,0,false,'9102'],[2,1,true,'9111'],[3,1,false,'9112'],[2,2,false,'9121']])
  await verifyRelayCase(...fixture);
console.log('TH11 MP D0/P12 Direct, legacy pure delay, typed Runtime options, EBTM/EBNP and real local 2P/3P relay policy PASS');
