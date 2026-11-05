import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {WebSocket} from 'ws';
import {COOPERATIVE_SPECTATOR_MARKERS} from '../src/spectator-stages.mjs';
const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{windowsHide:true,stdio:['ignore','pipe','pipe'],
 env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE:'240',EAGLER_NETPLAY_MAX_ROOMS_PER_IP:String(Object.keys(COOPERATIVE_SPECTATOR_MARKERS).length)}});
let log='',port;child.stdout.on('data',b=>{log+=b;const m=String(b).match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m)port=Number(m[1]);});child.stderr.on('data',b=>log+=b);
const sockets=[];
async function wait(check,label){const end=Date.now()+6000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,5));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),r={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.on('message',(b,binary)=>binary?r.binary.push(Buffer.from(b)):r.json.push(JSON.parse(String(b))));ws.on('close',(code,reason)=>{r.closed=true;r.closeReason=code+' '+reason;});await wait(()=>ws.readyState===1||r.closed,'open');assert.equal(r.closed,false,query+' '+r.closeReason+'\n'+log);return r;}
const send=(s,v)=>s.ws.send(JSON.stringify(v)),room=s=>s.json.findLast(m=>m.room&&typeof m.room==='object')?.room;
function checkpoint(marker,session){const b=Buffer.alloc(44);b.set([69,marker,83,86,2,1,2,0]);b.writeBigUInt64LE(session,8);b.writeUInt32LE(7,16);b.writeUInt32LE(1,24);b.writeUInt32LE(1,28);b.writeUInt32LE(1,32);b.writeUInt32LE(4,36);b.set([4,3,2,1],40);return b;}
function input(marker,session){const b=Buffer.alloc(48);b.set([69,marker,78,80,4,3,2,0]);b.writeBigUInt64LE(session,8);b.writeUInt32LE(7,20);return b;}
const upload=(s,b)=>s.ws.send(Buffer.concat([Buffer.from([0xe8]),b]));
try{await wait(()=>port,'startup');let index=0;
 for(const [product,marker] of Object.entries(COOPERATIVE_SPECTATOR_MARKERS)){
  const id=product+'-'+(7600+index++),prefix=product+'_session_',base=`room=${id}&run=1&players=2`;
  const host=await open(`room=${id}&lobby=${prefix}host&member=${prefix}member_host`),guest=await open(`room=${id}&lobby=${prefix}guest&member=${prefix}member_guest`);
  send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1],'seats');
  send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.ready),'ready');send(host,{type:'start',...(product==='th15mp'?{adonisMode:1}: {})});await wait(()=>host.json.some(m=>m.type==='start'),'start');
  const p0=await open(base+'&player=0&member='+prefix+'member_host'),p1=await open(base+'&player=1&member='+prefix+'member_guest');
  const s0=await open(base+'&player=0&signal=1&member='+prefix+'member_host'),s1=await open(base+'&player=1&signal=1&member='+prefix+'member_guest');
  send(s0,{type:'rtc-failed'});send(s1,{type:'rtc-failed'});await wait(()=>s0.json.some(m=>m.type==='route'),'route');
  let viewer;
  for(let generation=0;generation<3;++generation){
   const session=BigInt(9+generation),descriptor={type:'spectator-session',session:session.toString(16).padStart(16,'0'),seed:417+generation,generation};
   send(p0,descriptor);upload(p0,checkpoint(marker,session));upload(p0,input(marker,session));await wait(()=>room(host)?.spectatorTimeline,'available');
   if(!viewer){const lobby=await open(`room=${id}&lobby=${prefix}viewer&member=${prefix}member_viewer`);send(lobby,{type:'spectate'});await wait(()=>lobby.json.some(m=>m.type==='spectator-start'),'spectate');viewer=await open(base+'&spectator='+prefix+'viewer&member='+prefix+'member_viewer');}
   await wait(()=>viewer.json.some(m=>m.type==='spectator-session'&&m.generation===generation)&&viewer.binary.some(b=>b[2]===78&&b.readBigUInt64LE(8)===session),'new-generation bootstrap');
   assert.equal(viewer.closed,false);
   // The retired generation cannot terminate the new stream over signaling.
   if(generation)send(s0,{type:'spectator-stop',generation:generation-1});
   p0.ws.send(Buffer.from([0xe7,1,generation,42]));await wait(()=>p1.binary.some(b=>b.equals(Buffer.from([generation,42]))),'players continue');assert.equal(viewer.closed,false);
  }
  viewer.ws.terminate();await wait(()=>viewer.closed,'actual receiver socket loss');await new Promise(r=>setTimeout(r,80));
  const recovered=await open(base+'&spectator='+prefix+'viewer&member='+prefix+'member_viewer');await wait(()=>recovered.binary.length===3,'same-admission history replay');
  assert.equal(recovered.json.find(m=>m.type==='spectator-session')?.generation,2);assert.equal(recovered.binary[0].readBigUInt64LE(8),11n);assert.equal(recovered.binary.at(-1)[5],4);
  for(const socket of sockets)socket.close(1000,'scenario complete');await wait(()=>sockets.every(s=>s.readyState===3),'cleanup');sockets.length=0;
 }
 console.log('PASS all cooperative products: three source sessions, current Replay bootstrap, stale STOP isolation, actual spectator reconnect and uninterrupted player traffic');
}finally{for(const s of sockets)s.close();child.kill();}
