import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {TH05_INITIAL_STOCK,validTh05InitialSpares,validTh05InitialBombs} from '../../common/contracts/th05-initial-stock.mjs';
import {buildMultiplayerRuntimeOptions} from '../.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs';
import {normalizeMultiplayerLobbySnapshot} from '../.cache/build/browser/assets/launcher/multiplayer-lobby-snapshot.mjs';
import {multiplayerConfigForProduct} from '../../common/contracts/product-catalog.mjs';

const config=multiplayerConfigForProduct('th05mp');
const input={url:'ws://localhost/?room=th05mp-1234&run=1',player:0,playerCount:2,seed:1,difficulty:1,adonisMode:0,predictionLimit:8,spectator:false,spectatorId:'',spectatorCount:0,iceServers:[],loadouts:[{character:0,shot:0},{character:1,shot:0}]};
assert.deepEqual(TH05_INITIAL_STOCK,{spares:2,bombs:3});
for(let spares=0;spares<=5;spares++)for(let bombs=0;bombs<=3;bombs++){
 assert(validTh05InitialSpares(spares)&&validTh05InitialBombs(bombs));
 const options=buildMultiplayerRuntimeOptions({...input,initialSpares:spares,initialBombs:bombs},config);
 assert.equal(options.netplayInitialSpares,spares);assert.equal(options.netplayInitialBombs,bombs);
}
for(const [key,value] of [['initialSpares',-1],['initialSpares',6],['initialSpares',1.5],['initialBombs',4],['initialBombs','3']])assert.throws(()=>buildMultiplayerRuntimeOptions({...input,[key]:value},config));
assert.throws(()=>buildMultiplayerRuntimeOptions({...input,url:'ws://localhost/?room=th11mp-1234&run=1',initialSpares:2},config));
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:new URL('../../',import.meta.url),windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0'}});
let logs='';const sockets=[];relay.stderr.on('data',b=>logs+=b);
function next(socket,predicate=()=>true){return new Promise((yes,no)=>{const timer=setTimeout(()=>{clean();no(Error('Relay reply timeout '+logs));},6000);const receive=event=>{const value=JSON.parse(event.data);if(predicate(value)){clean();yes(value);}};const clean=()=>{clearTimeout(timer);socket.removeEventListener('message',receive);};socket.addEventListener('message',receive);});}
function send(socket,message,predicate){const reply=next(socket,predicate);socket.send(JSON.stringify(message));return reply;}
try{
 const port=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error(logs)),10000);relay.stdout.on('data',b=>{logs+=b;const m=logs.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m){clearTimeout(timer);yes(Number(m[1]));}});});
 async function connect(product,id,seat){const socket=new WebSocket(`ws://127.0.0.1:${port}/?room=${product}-stocktest&lobby=stock_client_${id}&member=stock_member_${id}&intent=${seat===0?'create':'join'}&players=2`);sockets.push(socket);assert.equal((await next(socket)).type,'state');await send(socket,{type:'take-seat',seat,loadout:seat},m=>m.room?.seats?.[seat]?.clientId===`stock_client_${id}`);return socket;}
 const host=await connect('th05mp','host',0),guest=await connect('th05mp','guest',1);
 const settings={type:'settings',playerCount:2,difficulty:1,initialSpares:5,initialBombs:0};
 await send(host,{type:'set-ready',ready:true},m=>m.room?.seats?.[0]?.ready===true);await send(guest,{type:'set-ready',ready:true},m=>m.room?.seats?.[1]?.ready===true);
 const peer=next(guest,m=>m.room?.initialSpares===5&&m.room?.initialBombs===0);
 const state=await send(host,settings,m=>m.room?.initialSpares===5&&m.room?.initialBombs===0);await peer;
 assert(state.room.seats.every(s=>!s||!s.ready),'Team stock changes invalidate all ready seats');
 const normalized=normalizeMultiplayerLobbySnapshot(state.room,{localClientId:'stock_client_host',...config});assert.equal(normalized.initialSpares,5);assert.equal(normalized.initialBombs,0);
 assert.equal(normalizeMultiplayerLobbySnapshot({...state.room,initialBombs:4},{localClientId:'stock_client_host',...config}),null);
 assert.equal((await send(guest,{...settings,initialSpares:0},m=>m.type==='error')).type,'error');
 for(const patch of [{initialSpares:-1},{initialSpares:6},{initialBombs:4},{initialBombs:'3'}])assert.equal((await send(host,{...settings,...patch},m=>m.type==='error')).type,'error');
 await send(host,{type:'set-ready',ready:true},m=>m.room?.seats?.[0]?.ready===true);await send(guest,{type:'set-ready',ready:true},m=>m.room?.seats?.[1]?.ready===true);
 await send(host,{type:'start',adonisMode:0,inputDelay:0,predictionLimit:8},m=>m.type==='start');
 assert.equal((await send(host,{...settings,initialSpares:0},m=>m.type==='error')).error,'本局已经开始');
 const other=await connect('th11mp','other',0);assert.equal((await send(other,settings,m=>m.type==='error')).type,'error');
 console.log('PASS TH05 stock bounds, Runtime mapping, snapshot validation, host authority, peer sync, ready reset and run lock');
}finally{for(const socket of sockets)socket.close();relay.kill();}
