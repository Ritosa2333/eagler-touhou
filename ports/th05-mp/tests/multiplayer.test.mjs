import test from 'node:test';
import assert from 'node:assert/strict';
import {createMultiplayer,multiplayerOptions} from '../web/multiplayer.mjs';
import {runtimeOptions} from '../web/managed.mjs';
import {createManagedTransport} from '../web/managed-transport.mjs';

const options=(count,local)=>({netplayMode:'lan',netplayPlayerCount:count,netplayPlayer:local,netplayUrl:'wss://example.test/relay?room=th05mp-test&run=one&member=seat&runtimeGeneration=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',netplaySeed:42,netplayDifficulty:1,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),netplayInputDelayAuto:true,netplayAdonisMode:1,netplayPredictionLimit:0});

for(const route of ['relay','rtc'])test(`TH05 ${route} crash identifies the failed seat to both teammates`,async()=>{
 const sockets=[],failures=Array.from({length:3},()=>[]),transports=[];
 for(let local=0;local<3;local++){
  const environment={WebSocket:class{
   constructor(url){this.url=new URL(url);this.readyState=1;this.bufferedAmount=0;if(!this.url.searchParams.has('signal'))sockets[local]=this;}
   send(packet){if(typeof packet==='string')return;const bytes=new Uint8Array(packet);assert.equal(bytes[0],0xe7);
    const incoming=bytes.slice();incoming[0]=0xe6;incoming[1]=local;
    queueMicrotask(()=>sockets[bytes[1]].onmessage?.({data:incoming.buffer}));
   }
   close(){this.readyState=3;}
  }};
  const transport=createManagedTransport(options(3,local),['p1','p2','p3'],error=>failures[local].push(error.message),environment);
  transport.connect();
  // Model an established RTC lane with no gameplay callback: the crash
  // notice must still arrive through the authenticated relay.
  if(route==='rtc')for(const peer of environment.__th05PeerTransport.peers.values())peer.rtc={readyState:'open',close(){},send(){throw Error('failure notice entered gameplay input fence');}};
  sockets[local].onmessage({data:JSON.stringify({type:'route',mode:route})});
  transports.push(transport);
 }
 try{
  await transports[0].abort(Error('Mismatching io.KeyCtrl/io.KeyShift vs io.KeyMods'));
  assert.deepEqual(failures[0],[]);
  for(const list of failures.slice(1)){assert.equal(list.length,1);assert.match(list[0],/P1.*TH05.*KeyMods/);}
 }finally{transports.forEach(t=>t.close());}
});
test('native room configuration rejects absent identity and unsupported seats/modes',()=>{
 const good=options(3,2);assert.equal(multiplayerOptions(good),good);assert.throws(()=>runtimeOptions(good),/Unsupported/);assert.equal(runtimeOptions(good,true).netplayPlayer,2);
 for(const patch of [{netplayPlayerCount:4},{netplayPlayer:3},{netplaySeed:-1},{netplayDifficulty:5},{netplayAdonisMode:3},{netplayPredictionLimit:6},{netplaySpectator:true},{netplayLoadouts:[{character:0,shot:1}]},{netplayUrl:'wss://example.test/relay'}])assert.throws(()=>multiplayerOptions({...good,...patch}));
});

test('spectators require a receive-only identity without an owned player seat',()=>{
 const observer={...options(3,null),netplaySpectator:true,netplaySpectatorId:'observer_123456'};
 assert.equal(runtimeOptions(observer,true).netplayPlayer,null);
 assert.equal(runtimeOptions({...observer,netplayPlayer:0},true).netplayPlayer,0);
 for(const patch of [{netplayPlayer:1},{netplaySpectatorId:''},{netplaySpectatorId:'bad/path'}])assert.throws(()=>multiplayerOptions({...observer,...patch}));
});

test('rollback and mixed timing use the common eight-frame ceiling',()=>{
 assert.equal(multiplayerOptions({...options(2,0),netplayAdonisMode:1,netplayPredictionLimit:8}).netplayAdonisMode,1);
 for(const mode of [0,2])assert.equal(multiplayerOptions({...options(2,0),netplayAdonisMode:mode,netplayPredictionLimit:8}).netplayAdonisMode,mode);
 for(const reserve of [0,3,1.5])assert.throws(()=>multiplayerOptions({...options(2,0),netplayPredictionReserve:reserve}));
});

test('team initial stocks follow PR spare-life and Bomb bounds',()=>{
 for(const spares of [0,5])for(const bombs of [0,3])assert.equal(runtimeOptions({...options(2,0),netplayInitialSpares:spares,netplayInitialBombs:bombs},true).netplayInitialSpares,spares);
 for(const patch of [{netplayInitialSpares:-1},{netplayInitialSpares:6},{netplayInitialSpares:1.5},{netplayInitialBombs:4},{netplayInitialBombs:'3'}])assert.throws(()=>runtimeOptions({...options(2,0),...patch},true));
 assert.throws(()=>runtimeOptions({netplayInitialSpares:2}),/Unsupported/);
});

for(const count of [2,3])test(`${count} native seats confirm identical inputs with delayed and reordered packets`,async()=>{
 const endpoints=new Map(),pending=[],errors=[],logs=Array.from({length:count},()=>[]),sessions=[];
 let serial=0;
 function factory(opts,roles){
  const local=opts.netplayPlayer,channels=new Map();
  for(let id=0;id<count;id++)if(id!==local){const channel={readyState:'connecting',send(text){const data=JSON.parse(text);pending.push({from:local,to:id,text,fast:data.type==='input',order:serial++});}};channels.set(id,channel);}
  endpoints.set(local,channels);
  return {channelFor:role=>channels.get(roles.indexOf(role)),connect(){queueMicrotask(()=>{for(const channel of channels.values()){channel.readyState='open';channel.onopen();}});},close(){},finish:async()=>{}};
 }
 function flush(){
  // Control remains reliable and ordered; input packets take different paths.
  const batch=pending.splice(0);batch.sort((a,b)=>a.fast&&b.fast?b.order-a.order:a.order-b.order);
  for(const packet of batch)endpoints.get(packet.to).get(packet.from).onmessage({data:packet.text});
 }
 for(let local=0;local<count;local++){
  let sample=0,inputs=[],frame=0;const ledger=Array.from({length:count},()=>new Map());
  const module={
   _th05_net_reset(delay){for(let f=0;f<delay;f++)for(const lane of ledger)lane.set(f,[0,0,0,0]);return 1;},
   _th05_net_input(seat,f,...input){ledger[seat].set(f,input);return 1;},
   _th05_net_advance(){if(ledger.some(lane=>!lane.has(frame)))return 0;logs[local].push(ledger.map(lane=>lane.get(frame)));++frame;return 1;},
   _th05_net_confirmed:()=>frame-1,_th05_net_stat:()=>0,_th05_net_hash:()=>42,_th05_net_close(){},
   _th05_coop_display:()=>{},_th05_coop_start_stock:()=>1,_th05_coop_input:(seat,...value)=>inputs[seat]=value,_th05_tick(){logs[local].push(inputs.map(value=>[...value]));return 1;},_th05_coop_finished:()=>false};
  sessions.push(createMultiplayer(options(count,local),{module,sample:()=>[(++sample%4)*16+local,(sample+local)*.125,-sample*.25,(sample+local)%4],fail:error=>errors.push(error),transportFactory:factory,setTimer:(fn,ms)=>setTimeout(fn,ms>1000?2000:Math.min(ms,15))}));
 }
 const tasks=sessions.map(s=>s.start());
 const timer=setInterval(flush,1);
 try{
  await Promise.all(tasks);assert.ok(sessions.every(s=>s.delay===sessions[0].delay));
  for(let attempt=0;attempt<10000&&sessions.some(s=>s.frame<500);attempt++){
   for(const session of [...sessions].reverse())if(session.frame<500)session.advance();
   flush();await new Promise(resolve=>setImmediate(resolve));
  }
  assert.deepEqual(errors,[]);assert.ok(sessions.every(s=>s.frame===500));for(const log of logs)assert.deepEqual(log,logs[0]);
  assert.ok(logs[0].some(row=>row[0][1]!==row[1][1]),'seats retain independent touch motion');assert.ok(logs[0].some(row=>row[0][3]>=2),'absolute touch modes survive transport');
  const end=sessions[0].end();assert.ok(end instanceof Promise);assert.equal(sessions[0].end(),end);await end;
 }finally{clearInterval(timer);sessions.forEach(s=>s.close());}
});
