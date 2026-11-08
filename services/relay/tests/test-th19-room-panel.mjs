import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
const listener=net.createServer();await new Promise(r=>listener.listen(0,'127.0.0.1',r));
const port=listener.address().port;await new Promise(r=>listener.close(r));
const external=process.env.TH19_SCORE_TEST_URL;
const child=external?null:spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{
  env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},
  stdio:['ignore','pipe','pipe'],windowsHide:true});
let log='';child?.stdout.on('data',b=>log+=b);child?.stderr.on('data',b=>log+=b);
const sockets=[];
async function wait(test,label){const end=Date.now()+8000;while(!test()){if(Date.now()>end)throw Error(label+'\n'+log);await new Promise(r=>setTimeout(r,5));}}
async function open(query){const ws=new WebSocket(`${external||`ws://127.0.0.1:${port}/`}?${query}`),s={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';
  ws.addEventListener('message',e=>typeof e.data==='string'?s.json.push(JSON.parse(e.data)):s.binary.push(new Uint8Array(e.data)));
  ws.addEventListener('close',()=>s.closed=true);await wait(()=>ws.readyState===1||s.closed,'open');assert.equal(ws.readyState,1);return s;}
const send=(s,m)=>s.ws.send(JSON.stringify(m));
const latest=s=>[...s.json].reverse().find(m=>m.room && typeof m.room==='object')?.room;
const room='th19mp-7101';
function frame(n){const b=Buffer.alloc(47);b.set([0xe8,84,57,83,80,1,3,2,0]);b.writeUInt32LE(n,9);return b;}
function flatten(s){return s.binary.filter(b=>b.length%46===0).flatMap(b=>Array.from({length:b.length/46},(_,i)=>new DataView(b.buffer,b.byteOffset+i*46,46).getUint32(8,true)));}
try{
  if(!external)await wait(()=>log.includes('listening'),'relay ready');
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
  const directoryRoom=()=>[...directory.json].reverse().find(m=>m.type==='directory')?.rooms?.find(r=>r.code==='7101');
  await wait(()=>directoryRoom()?.spectatable===true,'playing room exposes spectator entry');
  assert.equal(directoryRoom().phase,'running');assert.equal(directoryRoom().joinable,false);
  assert.ok(directoryRoom().seats.every(seat => !('name' in seat) && !('machineSign' in seat)),
    'public machine signs are optional; private room names are not a fallback');
  send(host,{type:'th09-match',serial:1,start:0,frames:3600});
  send(guest,{type:'th09-match',serial:1,start:0,frames:3600});
  await wait(()=>directoryRoom()?.seats[0]?.stats?.minutes===1,'small round progress published');
  assert.deepEqual(directoryRoom().seats.slice(0,2).map(s=>s.stats.wins),[0,0]);
  assert.deepEqual(latest(host).seats.slice(0,2).map(s=>s.stats.wins),[0,0]);
  send(host,{type:'th09-match',serial:1,start:0,frames:7200,winner:0});
  send(host,{type:'th09-match',serial:1,start:0,frames:7200,winner:0});
  send(guest,{type:'th09-match',serial:1,start:0,frames:7200});
  await wait(()=>directoryRoom()?.seats[0]?.stats?.minutes===2,'confirmed progress published');
  assert.deepEqual(directoryRoom().seats.slice(0,2).map(s=>s.stats.wins),[0,0]);
  for(const s of [first.h,first.g,first.hs,first.gs])s.ws.close();
  await wait(()=>latest(host)?.phase==='lobby','return to lobby');
  send(guest,{type:'th09-match',serial:1,start:0,frames:7200,winner:0});
  await wait(()=>[host,guest].every(s=>latest(s)?.seats[0]?.stats?.wins===1),'late final confirms the room score');
  assert.deepEqual(latest(host).seats.slice(0,2).map(s=>s.stats.wins),[1,0]);
  const second=await start(2);
  send(guest,{type:'th09-match',serial:2,start:0,frames:3600,winner:1});
  send(host,{type:'th09-match',serial:2,start:0,frames:3600,winner:1});
  await wait(()=>directoryRoom()?.seats[1]?.stats?.wins===1,'next whole match public score');
  assert.deepEqual(directoryRoom().seats.slice(0,2).map(s=>s.stats.wins),[1,1]);
  await wait(()=>latest(host)?.seats[1]?.stats?.wins===1,'next whole match host room score');
  assert.deepEqual(latest(host).seats.slice(0,2).map(s=>s.stats.wins),[1,1]);
  console.log('TH19 real relay: both seats confirm progress, late final settles once, next run 1:1; room and directory PASS');
}finally{for(const ws of sockets)ws.close();if(child){child.kill();await new Promise(r=>child.once('exit',r));}}
