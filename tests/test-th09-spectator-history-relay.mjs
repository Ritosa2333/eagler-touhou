// Late join gets only the current match. Existing viewers keep earlier packets
// locally; adding local rewind must not expand the server's bootstrap history.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
const port=await new Promise(resolve=>{const server=net.createServer();server.listen(0,'127.0.0.1',()=>{const port=server.address().port;server.close(()=>resolve(port));});});
// Test an exact deployed/candidate bundle without rebuilding unrelated services.
const child=spawn(process.execPath,[process.env.EAGLER_TEST_RELAY_ENTRY || fileURLToPath(new URL('../server/netplay-relay.mjs',import.meta.url))],{windowsHide:true,
  env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let log='';child.stdout.on('data',bytes=>log+=bytes);child.stderr.on('data',bytes=>log+=bytes);
const sockets=[];
async function wait(test,label){const deadline=Date.now()+5000;while(!test()){if(Date.now()>deadline)throw Error(label+' timeout\n'+log);await new Promise(resolve=>setTimeout(resolve,5));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),row={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';
  ws.addEventListener('message',event=>typeof event.data==='string'?row.json.push(JSON.parse(event.data)):row.binary.push(new Uint8Array(event.data)));
  ws.addEventListener('close',()=>row.closed=true);await wait(()=>ws.readyState===1||row.closed,'connect');assert.equal(ws.readyState,1);return row;}
const send=(row,message)=>row.ws.send(JSON.stringify(message));
const room=row=>row.json.findLast(message=>message.room)?.room;
function packet(kind,frame,version){const length={begin:version===1?74:102,input:46,end:24}[kind],bytes=new Uint8Array(length+1);bytes[0]=0xe8;
  bytes.set(kind==='begin'?[84,57,66,71,version,0,2,0]:kind==='end'?[84,57,83,69,1,0,2,0]:[84,57,83,80,1,3,2,0],1);
  new DataView(bytes.buffer).setUint32(9,frame,true);return bytes;}
try{
  await wait(()=>log.includes('listening'),'relay');
  for(const version of [1,2]){
  const start=version===1?0:137,next=start+100,id=`th09mp-${6196+version}`,base=`room=${id}&run=1&players=2`;
  const host=await open(`room=${id}&lobby=history_host&member=history_member_host`),guest=await open(`room=${id}&lobby=history_guest&member=history_member_guest`),early=await open(`room=${id}&lobby=history_early&member=history_member_early`);
  send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});send(early,{type:'spectate'});
  await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1]&&room(host)?.spectatorCount===1,'seats');
  send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});await wait(()=>room(host)?.seats.slice(0,2).every(seat=>seat?.ready),'ready');send(host,{type:'start'});
  await wait(()=>host.json.some(message=>message.type==='start'),'start');
  const p0=await open(base+'&player=0&member=history_member_host');await open(base+'&player=1&member=history_member_guest');
  const viewer=await open(base+'&spectator=history_early&member=history_member_early');
  const signal0=await open(base+'&player=0&signal=1&member=history_member_host'),signal1=await open(base+'&player=1&signal=1&member=history_member_guest');
  send(signal0,{type:'rtc-failed'});send(signal1,{type:'rtc-failed'});await wait(()=>signal0.json.some(message=>message.type==='route'),'route');
  // Native selection consumes menu frames before the first gameplay initializer.
  for(const [kind,frame] of [['begin',start],['input',start],['end',start+1],['begin',next],['input',next]])p0.ws.send(packet(kind,frame,version));
  await wait(()=>viewer.binary.length===5,'existing viewer receives both matches');
  const late=await open(`room=${id}&lobby=history_late&member=history_member_late`);send(late,{type:'spectate'});
  await wait(()=>room(host)?.spectatorCount===2,'late admission');
  const joined=await open(base+'&spectator=history_late&member=history_member_late');await wait(()=>joined.binary.length===2,'current-match bootstrap');
  assert.deepEqual(joined.binary.map(bytes=>[bytes.length,new DataView(bytes.buffer).getUint32(8,true)]),[[version===1?74:102,next],[46,next]]);
  assert.equal(viewer.binary.length,5);assert.equal(viewer.closed,false);assert.equal(joined.closed,false);
  p0.ws.send(packet('input',next+1,version));await wait(()=>joined.binary.length===3&&viewer.binary.length===6,'continued live input');
  assert.equal(new DataView(joined.binary[2].buffer).getUint32(8,true),next+1);
  for(const socket of sockets)socket.close();await wait(()=>sockets.every(socket=>socket.readyState===3),'scenario cleanup');sockets.length=0;
  }
  console.log('Late spectator receives current match only; existing viewer retains its own earlier input; both continue live PASS');
}finally{for(const socket of sockets)socket.close();child.kill();}
