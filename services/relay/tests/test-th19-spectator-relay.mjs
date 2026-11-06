import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
import {th19StreamBegin,th19StreamFrame,th19StreamEnd} from '../../../common/contracts/th19-recording.mjs';
const port=await new Promise(resolve=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>resolve(p));});});
const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let log='';child.stdout.on('data',b=>log+=b);child.stderr.on('data',b=>log+=b);const sockets=[];
async function wait(check,label){const end=Date.now()+10000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,10));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),row={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';ws.addEventListener('message',e=>typeof e.data==='string'?row.json.push(JSON.parse(e.data)):row.binary.push(new Uint8Array(e.data)));ws.addEventListener('close',()=>row.closed=true);await wait(()=>ws.readyState===1||row.closed,'connect');assert.equal(ws.readyState,1);return row;}
const send=(row,m)=>row.ws.send(JSON.stringify(m)),room=row=>row.json.findLast(m=>m.room)?.room;
const config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build:'a'.repeat(64),seed:1,difficulty:1,characters:[0,1],delay:0,rollback:true},input={keys:0,direct:0,x:0,y:0};
try{
 await wait(()=>log.includes('listening'),'startup');
 for(const [index,route] of ['rtc','relay'].entries()){
  const id=`th19mp-${6710+index}`,base=`room=${id}&run=1&players=2`;
  const host=await open(`room=${id}&lobby=watch_host&member=member_watch_host`),guest=await open(`room=${id}&lobby=watch_guest&member=member_watch_guest`);
  send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});
  await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1],'seats');send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});
  await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.ready),'ready');send(host,{type:'start'});await wait(()=>host.json.some(m=>m.type==='start'),'start');
  const p0=await open(base+'&player=0&member=member_watch_host'),p1=route==='relay'?await open(base+'&player=1&member=member_watch_guest'):null;
  const s0=await open(base+'&player=0&signal=1&member=member_watch_host'),s1=await open(base+'&player=1&signal=1&member=member_watch_guest');
  send(s0,{type:route==='rtc'?'rtc-ready':'rtc-failed'});send(s1,{type:route==='rtc'?'rtc-ready':'rtc-failed'});await wait(()=>s0.json.some(m=>m.type==='route'&&(m.mode||m.route)===route),'route');
  const upload=bytes=>{const b=new Uint8Array(bytes.length+1);b[0]=0xe8;b.set(bytes,1);p0.ws.send(b);};
  upload(th19StreamBegin(config));upload(th19StreamFrame(0,input,input));
  const late=await open(`room=${id}&lobby=watch_late&member=member_watch_late`);send(late,{type:'spectate'});await wait(()=>late.json.some(m=>m.type==='spectator-start'),'late admission');
  const viewer=await open(base+'&spectator=watch_late&member=member_watch_late');await wait(()=>viewer.binary.length===2,'history');assert.deepEqual(viewer.binary[1],th19StreamFrame(0,input,input));
  upload(th19StreamFrame(1,input,input));await wait(()=>viewer.binary.length===3,'live');
  viewer.ws.send(th19StreamFrame(2,input,input));await wait(()=>viewer.closed,'read-only enforcement');assert.equal(p0.closed,false);assert.equal(s0.closed,false);if(p1)assert.equal(p1.closed,false);
  upload(th19StreamEnd(2));
  for(const ws of sockets)ws.close();await wait(()=>sockets.every(ws=>ws.readyState===3),'cleanup');sockets.length=0;
 }
 console.log('TH19 RTC/Relay late-join history, live delivery and receive-only isolation PASS');
}finally{for(const ws of sockets)ws.close();child.kill();}
