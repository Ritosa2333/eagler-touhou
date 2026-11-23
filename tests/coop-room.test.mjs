import assert from 'node:assert/strict';
import test from 'node:test';
import {readTh128RoomOptions} from '../runtime/managed-room.mjs';
import {buildMultiplayerGameplayRelayUrl} from '../../../launcher/src/launcher/multiplayer-relay-url.mts';
import {openRelayLink} from '../runtime/relay-link.mjs';
const identity={gameplay:'same-stage-coop',coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64),generation:'c'.repeat(64)};
const room=(n,d,role,extra={})=>({netplayMode:'lan',netplayPlayerCount:n,netplayPlayer:role.player??0,netplaySpectator:'spectator' in role,netplaySpectatorId:role.spectator??'',netplaySeed:4812,netplayDifficulty:d,netplayChallengeMode:true,netplayLoadouts:Array.from({length:n},()=>({character:0,shot:0})),netplayUrl:buildMultiplayerGameplayRelayUrl('ws://127.0.0.1:48001/',{product:'th128mp',roomCode:'4812',runId:8,role,memberId:'member_test',runtimeGeneration:identity.generation}),...extra});
test('identical author room rules across every seat and viewer, preserving all five difficulties and challenge',()=>{
 for(const n of [2,3])for(const d of [0,1,2,3,4]){
  const viewer=readTh128RoomOptions(room(n,d,{spectator:'spectator_12'}),identity);assert.equal(viewer.seat,null);
  for(let seat=0;seat<n;seat++){const p=readTh128RoomOptions(room(n,d,{player:seat}),identity);
   assert.deepEqual(p.rules,viewer.rules);assert.equal(p.rules.players,n);assert.equal(p.rules.difficulty,d);assert.equal(p.rules.stageId,d===4?16:1);assert.equal(p.rules.challengeMode,true);
   assert.equal(p.rules.mode,'same-stage-coop');assert.equal(p.seat,seat);assert.equal(new URL(p.relayUrl).searchParams.get('players'),String(n));
  }
 }
});
test('new gameplay identity refuses loose URL, seat mismatch, practice and invalid challenge/prediction values',()=>{
 const data=room(3,0,{player:2});for(const extra of [{netplayPlayerCount:2},{netplayDifficulty:'4'},{netplayUrl:data.netplayUrl+'&players=2'},{netplayUrl:data.netplayUrl.replace(/&runtimeGeneration=[a-f0-9]+/,'')},{netplayChallengeKeepBullets:true},{netplayChallengeMode:'yes'},{thpracEnabled:true},{netplayLoadouts:[{character:0,shot:0}]}])assert.throws(()=>readTh128RoomOptions({...data,...extra},identity));
});
class Sock extends EventTarget{readyState=1;bufferedAmount=0;constructor(){super();Sock.last=this;}close(){this.readyState=3;}}
test('3P author relay keeps source seats distinct and never infers the sender from the payload',async()=>{
 const seen=[],errors=[];const link=openRelayLink({url:'ws://127.0.0.1:48001/?room=th128mp-4812&run=8&player=2&players=3',players:3,localPlayer:2,WebSocketClass:Sock,onMessage:(msg,who)=>seen.push([msg.seat,who]),onError:e=>errors.push(e)});
 const s=Sock.last;s.dispatchEvent(new Event('open'));const wrap=(i,v)=>new Uint8Array([0xe6,i,...new TextEncoder().encode(JSON.stringify({seat:v}))]).buffer;
 for(const i of [0,1])s.dispatchEvent(new MessageEvent('message',{data:wrap(i,1-i)}));await link.drained();assert.deepEqual(seen,[[1,0],[0,1]]);
 s.dispatchEvent(new MessageEvent('message',{data:wrap(2,0)}));await link.drained();assert.equal(errors.length,1);link.close();
});
