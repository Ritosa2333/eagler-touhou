import test from 'node:test';
import assert from 'node:assert/strict';
import {createMultiplayer,multiplayerOptions} from '../web/multiplayer.mjs';
import {runtimeOptions} from '../web/managed.mjs';

const options=(count,local)=>({netplayMode:'lan',netplayPlayerCount:count,netplayPlayer:local,netplayUrl:'wss://example.test/relay?room=th05mp-test&run=one&member=seat&runtimeGeneration=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',netplaySeed:42,netplayDifficulty:1,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),netplayInputDelayAuto:true,netplayAdonisMode:1,netplayPredictionLimit:0});
test('native room configuration rejects absent identity and unsupported seats/modes',()=>{
 const good=options(3,2);assert.equal(multiplayerOptions(good),good);assert.throws(()=>runtimeOptions(good),/Unsupported/);assert.equal(runtimeOptions(good,true).netplayPlayer,2);
 for(const patch of [{netplayPlayerCount:4},{netplayPlayer:3},{netplaySeed:-1},{netplayDifficulty:5},{netplayAdonisMode:0},{netplayPredictionLimit:6},{netplaySpectator:true},{netplayLoadouts:[{character:0,shot:1}]},{netplayUrl:'wss://example.test/relay'}])assert.throws(()=>multiplayerOptions({...good,...patch}));
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
  let sample=0,inputs=[];
  const module={_th05_coop_display:()=>{},_th05_coop_start:()=>1,_th05_coop_input:(seat,...value)=>inputs[seat]=value,_th05_tick(){logs[local].push(inputs.map(value=>[...value]));return 1;},_th05_coop_finished:()=>false};
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
 }finally{clearInterval(timer);sessions.forEach(s=>s.close());}
});
