import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
const listener=net.createServer();await new Promise(r=>listener.listen(0,'127.0.0.1',r));
const port=listener.address().port;await new Promise(r=>listener.close(r));
const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{
  env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},
  stdio:['ignore','pipe','pipe'],windowsHide:true});
let log='';child.stdout.on('data',b=>log+=b);child.stderr.on('data',b=>log+=b);
const sockets=[];
async function wait(test,label){const end=Date.now()+8000;while(!test()){if(Date.now()>end)throw Error(label+'\n'+log);await new Promise(r=>setTimeout(r,5));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),s={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';
  ws.addEventListener('message',e=>typeof e.data==='string'?s.json.push(JSON.parse(e.data)):s.binary.push(new Uint8Array(e.data)));
  ws.addEventListener('close',()=>s.closed=true);await wait(()=>ws.readyState===1||s.closed,'open');assert.equal(ws.readyState,1);return s;}
const send=(s,m)=>s.ws.send(JSON.stringify(m));
const latest=s=>[...s.json].reverse().find(m=>m.room && typeof m.room==='object')?.room;
const room='th09mp-710001';
function frame(n){const b=Buffer.alloc(47);b.set([0xe8,84,57,83,80,1,3,2,0]);b.writeUInt32LE(n,9);return b;}
function flatten(s){return s.binary.filter(b=>b.length%46===0).flatMap(b=>Array.from({length:b.length/46},(_,i)=>new DataView(b.buffer,b.byteOffset+i*46,46).getUint32(8,true)));}
try{
  await wait(()=>log.includes('listening'),'relay ready');
  const host=await open(`room=${room}&lobby=timeline_host&member=timeline_member_host`),guest=await open(`room=${room}&lobby=timeline_guest&member=timeline_member_guest`);
  send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});
  await wait(()=>latest(host)?.seats?.[0]&&latest(host)?.seats?.[1],'seats');
  async function start(serial){send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});
    await wait(()=>latest(host)?.seats?.slice(0,2).every(s=>s?.ready),'ready');send(host,{type:'start'});
    await wait(()=>host.json.some(m=>m.type==='start'&&m.serial===serial),'start');
    const base=`room=${room}&run=${serial}&players=2`,h=await open(base+'&player=0&member=timeline_member_host'),g=await open(base+'&player=1&member=timeline_member_guest');
    const hs=await open(base+'&player=0&signal=1&member=timeline_member_host'),gs=await open(base+'&player=1&signal=1&member=timeline_member_guest');
    send(hs,{type:'rtc-failed'});send(gs,{type:'rtc-failed'});await wait(()=>hs.json.some(m=>m.type==='route'),'route');return {base,h,g,hs,gs};}
  const first=await start(1);
  const directory=await open('directory=1&member=timeline_directory_member');
  const directoryRoom=()=>[...directory.json].reverse().find(m=>m.type==='directory')?.rooms?.find(r=>r.code==='710001');
  await wait(()=>directoryRoom()?.spectatable===true,'playing room exposes spectator entry');
  assert.equal(directoryRoom().phase,'running');assert.equal(directoryRoom().joinable,false);
  assert.ok(directoryRoom().seats.every(seat => !('name' in seat) && !('machineSign' in seat)),
    'public machine signs are optional; private room names are not a fallback');
  for(let n=0;n<200;n++)first.h.ws.send(frame(n));
  const roundMarker=Buffer.alloc(29);roundMarker.set([0xe8,84,57,82,68,1,0,2,0]);
  roundMarker.writeUInt32LE(200,9);roundMarker.writeUInt32LE(1,25);first.h.ws.send(roundMarker);
  const lobby=await open(`room=${room}&lobby=timeline_viewer&member=timeline_member_viewer`);
  send(lobby,{type:'spectate'});await wait(()=>lobby.json.some(m=>m.type==='spectator-start'&&m.serial===1),'midgame admission');
  send(lobby,{type:'quick-chat',serial:1,phrase:'thanks',seat:0});
  for(const target of [host,guest,lobby])await wait(()=>target.json.some(m=>m.type==='quick-chat'&&m.clientId==='timeline_viewer'&&m.seat===-1&&m.spectator===true),'spectator text broadcast with authoritative role');
  send(host,{type:'th09-match',serial:1,start:0,frames:3600});
  await wait(()=>latest(host)?.seats?.[0]?.stats?.minutes===1,'minute progress');
  send(guest,{type:'th09-match',serial:1,start:0,frames:6000,winner:1});
  send(host,{type:'th09-match',serial:1,start:0,frames:7200,winner:0});
  send(host,{type:'th09-match',serial:1,start:0,frames:7200,winner:0});
  await wait(()=>directoryRoom()?.seats[0]?.stats?.wins===1,'public whole-match score');
  assert.deepEqual(directoryRoom().seats[0].stats,{wins:1,losses:0,minutes:2});
  assert.deepEqual(directoryRoom().seats[1].stats,{wins:0,losses:1,minutes:2});
  send(host,{type:'th09-match',serial:1,start:7300,frames:0});
  await wait(()=>directoryRoom()?.seats[0]?.stats?.minutes===2,'Retry retains cumulative public duration');
  assert.deepEqual(directoryRoom().seats[0].stats,{wins:1,losses:0,minutes:2});
  assert.deepEqual(directoryRoom().seats[1].stats,{wins:0,losses:1,minutes:2});
  send(host,{type:'th09-match',serial:1,start:7300,frames:3600});
  await wait(()=>directoryRoom()?.seats[0]?.stats?.minutes===3,'next match adds to public cumulative duration');
  await wait(()=>latest(host)?.seats[1]?.stats?.minutes===3,'room receives cumulative duration');
  const viewer=await open(first.base+'&spectator=timeline_viewer&member=timeline_member_viewer');
  await wait(()=>flatten(viewer).length===200,'retained prefix');assert.deepEqual(flatten(viewer),Array.from({length:200},(_,i)=>i));
  await wait(()=>viewer.binary.some(b=>b.length===28),'retained round annotation');
  assert.ok(viewer.binary.some(b=>b.length>46),'history uses bounded batches');
  first.h.ws.send(frame(200));await wait(()=>flatten(viewer).length===201,'ordered live handoff');
  const end=Buffer.alloc(25);end.set([0xe8,84,57,83,69,1,0,2,0]);end.writeUInt32LE(201,9);first.h.ws.send(end);
  await wait(()=>viewer.binary.some(b=>b.length===24),'exact terminal marker');
  const begin=Buffer.alloc(75);begin.set([0xe8,84,57,66,71,1,0,2,0]);begin.writeUInt32LE(500,9);
  first.h.ws.send(begin);for(let n=500;n<510;n++)first.h.ws.send(frame(n));
  const lateLobby=await open(`room=${room}&lobby=timeline_late&member=timeline_member_late`);
  send(lateLobby,{type:'spectate'});await wait(()=>lateLobby.json.some(m=>m.type==='spectator-start'),'same-connection second match');
  const late=await open(first.base+'&spectator=timeline_late&member=timeline_member_late');
  await wait(()=>flatten(late).length===10,'latest match history');assert.deepEqual(flatten(late),Array.from({length:10},(_,i)=>500+i));
  assert.equal(new DataView(late.binary.find(b=>b.length===74).buffer).getUint32(8,true),500);
  end.writeUInt32LE(510,9);first.h.ws.send(end);
  await wait(()=>late.binary.some(b=>b.length===24),'second logical end');
  for(const s of [first.h,first.g,first.hs,first.gs])s.ws.close();
  await wait(()=>latest(host)?.phase==='lobby','viewer must not hold next match');assert.equal(viewer.closed,false);
  const second=await start(2);viewer.ws.close();
  for(let n=0;n<5;n++)second.h.ws.send(frame(n));
  const next=await open(second.base+'&spectator=timeline_viewer&member=timeline_member_viewer');
  await wait(()=>flatten(next).length===5,'second match prefix');assert.deepEqual(flatten(next),[0,1,2,3,4]);
  assert.equal(host.closed,false);assert.equal(guest.closed,false);assert.equal(lobby.closed,false);
  second.h.ws.send(Buffer.from([0xe8,0x53,0x54,0x4f,0x50,1]));
  await wait(()=>directoryRoom()?.spectatable===false,'stopped spectator stream withdraws directory entry');
  assert.equal(second.h.closed,false);assert.equal(second.g.closed,false);
  console.log('TH09 experimental spectator: late admission, ordered history/live handoff, same-connection match initializer and history pruning, logical ends, persistent lobby and isolated next-run prefix PASS');
}finally{for(const ws of sockets)ws.close();child.kill();await new Promise(r=>child.once('exit',r));}
