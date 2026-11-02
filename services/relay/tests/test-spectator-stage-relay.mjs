// Real isolated relay sockets; fake title payloads do not establish native
// world restoration or cross-device acceptance.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';
import {COOPERATIVE_SPECTATOR_MARKERS} from '../src/spectator-stages.mjs';
import {multiplayerConfigForProduct} from '../../../common/contracts/product-catalog.mjs';
const port=await new Promise(resolve=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>resolve(p));});});
const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{windowsHide:true,
  env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',
    EAGLER_NETPLAY_MAX_ROOMS_PER_IP:'24',EAGLER_NETPLAY_ROOM_LOOKUPS_PER_MINUTE:'24',EAGLER_NETPLAY_MAX_CONNECTIONS_PER_IP:'160',EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE:'480',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let log='';child.stdout.on('data',b=>log+=b);child.stderr.on('data',b=>log+=b);const sockets=[];
async function wait(check,label){const end=Date.now()+6000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,5));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),r={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';
  ws.addEventListener('message',e=>typeof e.data==='string'?r.json.push(JSON.parse(e.data)):r.binary.push(Buffer.from(e.data)));
  ws.addEventListener('close',()=>r.closed=true);await wait(()=>ws.readyState===1||r.closed,'connect');assert.equal(ws.readyState,1,query+'\n'+log);return r;}
const send=(r,v)=>r.ws.send(JSON.stringify(v));const room=r=>r.json.findLast(m=>m.room && typeof m.room === 'object')?.room;
function event(marker,kind,frame,stage,serial){const b=Buffer.alloc(kind===1?44:kind===2?40:32);b.set([69,marker,83,86,2,kind,2,0]);b.writeBigUInt64LE(9n,8);b.writeUInt32LE(7,16);b.writeUInt32LE(frame,20);b.writeUInt32LE(stage,24);b.writeUInt32LE(serial,28);
 if(kind===1){b.writeUInt32LE(1,32);b.writeUInt32LE(4,36);b.set([4,3,2,1],40);}if(kind===2){b.writeUInt32LE(1,32);b[36]=2;b[37]=3;}return b;}
function input(marker,frame){const b=Buffer.alloc(48);b.set([69,marker,78,80,4,3,2,0]);b.writeBigUInt64LE(9n,8);b.writeUInt32LE(frame,16);b.writeUInt32LE(7,20);return b;}
function upload(p,b){p.ws.send(Buffer.concat([Buffer.from([0xe8]),b]));}
try{
 await wait(()=>log.includes('listening'),'relay');let n=0;
 for(const ending of ['failure','upload','signal'])for(const [product,marker] of Object.entries(COOPERATIVE_SPECTATOR_MARKERS)){
  const id=`${product}-${6300+n++}`,base=`room=${id}&run=1&players=2`,prefix=product+'_';
  const host=await open(`room=${id}&lobby=${prefix}host&member=${prefix}member_host`),guest=await open(`room=${id}&lobby=${prefix}guest&member=${prefix}member_guest`);
  send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});
  await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1],'seats');
  send(guest,{type:'spectate'});
  await wait(()=>!room(host)?.seats[1]&&room(host)?.spectators.some(s=>s.clientId===prefix+'guest'),'seated guest voluntarily moves to spectator');
  send(guest,{type:'take-seat',seat:1,loadout:1});
  await wait(()=>room(host)?.seats[1]?.clientId===prefix+'guest','spectator returns to player seat');
  if (multiplayerConfigForProduct(product).inputTiming) {
  send(host,{type:'timing-settings',rollbackEnabled:false,timingChoice:4});
  await wait(()=>room(guest)?.timingChoice===4&&room(guest)?.adonisMode===1,'timing choice broadcast to guest');
  const denied=guest.json.length;send(guest,{type:'timing-settings',rollbackEnabled:false,timingChoice:7});
  await wait(()=>guest.json.slice(denied).some(m=>m.type==='error'),'guest cannot edit timing');
  assert.equal(room(host).timingChoice,4);
  send(host,{type:'timing-settings',rollbackEnabled:false,timingChoice:'auto'});
  await wait(()=>room(guest)?.timingChoice==='auto','host restores automatic timing');
  }
  const waiting=await open(`room=${id}&lobby=${prefix}waiting&member=${prefix}member_waiting`);send(waiting,{type:'spectate'});
  await wait(()=>room(host)?.spectatorCount===1,'full lobby admits an observer before start');
  assert.deepEqual(room(host).seats.slice(0,2).map(s=>s.clientId),[prefix+'host',prefix+'guest']);
  send(waiting,{type:'quick-chat',phrase:'1',serial:0});
  await wait(()=>host.json.some(m=>m.type==='quick-chat'&&m.clientId===prefix+'waiting'&&m.spectator&&m.seat===-1),'lobby spectators can use quick chat');
  send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});
  await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.ready),'ready');
  for(const client of [host,guest]) send(client,{type:'resource-progress',status:'ready',stage:'runtime',percent:100});
  await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.resource?.status==='ready'),'native preload complete');
  send(guest,{type:'resources-changed'});
  await wait(()=>room(host)?.seats.slice(0,2).every(s=>s&&!s.ready),'resource switch cancels every ready acknowledgement');
  const before=guest.json.length;send(guest,{type:'set-ready',ready:true});
  await wait(()=>guest.json.length>before,'unprepared ready rejected');
  assert.equal(room(guest).seats[1].ready,false);
  send(guest,{type:'resource-progress',status:'ready',stage:'runtime',percent:100});
  await wait(()=>room(host)?.seats[1]?.resource?.status==='ready','replacement preload complete');
  send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});
  await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.ready),'replacement ready');
  send(host,{type:'start',...(product==='th15mp'?{adonisMode:1}: {})});await wait(()=>host.json.some(m=>m.type==='start'),'start');
  const p0=await open(base+`&player=0&member=${prefix}member_host`),p1=await open(base+`&player=1&member=${prefix}member_guest`);
  const s0=await open(base+`&player=0&signal=1&member=${prefix}member_host`),s1=await open(base+`&player=1&signal=1&member=${prefix}member_guest`);
  send(s0,{type:'rtc-failed'});send(s1,{type:'rtc-failed'});await wait(()=>s0.json.some(m=>m.type==='route'),'route');
  const early=await open(base+`&spectator=${prefix}waiting&member=${prefix}member_waiting`);
  const length=product==='th06mp'?3072:1,next=600+length;
  let initial=event(marker,1,600,4,1);
  if(product==='th15mp'){
    const expanded=Buffer.alloc(40+16+3*0x238);expanded.set(initial);expanded.writeUInt32LE(expanded.length-40,36);initial=expanded;
  }
  upload(p0,initial);for(let frame=600;frame<next;++frame){
    upload(p0,input(marker,frame));
    // This is a history fixture, not a flood test. Respect the real player
    // upload token bucket while preparing a multi-burst bootstrap backlog.
    if(length>1&&(frame-599)%256===0)await new Promise(resolve=>setTimeout(resolve,300));
  }upload(p0,event(marker,2,next,4,2));
  await wait(()=>early.binary.length===length+3,'pre-start observer receives native bootstrap exactly once');
  assert.equal(early.binary[0].length,initial.length,'portable initializer survives the relay packet budget');
  assert.deepEqual(early.binary.slice(0,3).map(b=>b[5]),[1,4,3]);assert.equal(early.binary.at(-1)[5],2);
  await wait(()=>room(host)?.spectatorTimeline===true,'native initializer advertises timeline');
  const late=await open(`room=${id}&lobby=${prefix}late&member=${prefix}member_late`);send(late,{type:'spectate'});await wait(()=>room(host)?.spectatorCount===2,'late admission');
  const viewer=await open(base+`&spectator=${prefix}late&member=${prefix}member_late`);
  if(length===1){await wait(()=>viewer.binary.length===4,'stage prefix and bootstrap frontier');
    assert.deepEqual(viewer.binary.map(b=>[b[2],b[5],b.readUInt32LE(b[2]===83?20:16)]),[[83,1,600],[78,3,600],[83,2,next],[83,4,next]]);
  }else{await wait(()=>viewer.binary.length>2,'downloading a long stage');assert.equal(viewer.binary.some(b=>b[2]===83&&b[5]===4),false);}
  upload(p0,event(marker,1,next,5,3));upload(p0,input(marker,next));await wait(()=>viewer.binary.length===length+5,'old viewer continues into next stage');
  assert.deepEqual(viewer.binary.filter(b=>b[2]===78).map(b=>b.readUInt32LE(16)),Array.from({length:length+1},(_,i)=>600+i));
  assert.equal(viewer.binary.filter(b=>b[2]===83&&b[5]===4).length,1,'one fixed entry frontier even if a stage changes during bootstrap');
  const late2=await open(`room=${id}&lobby=${prefix}later&member=${prefix}member_later`);send(late2,{type:'spectate'});await wait(()=>room(host)?.spectatorCount===3,'second admission');
  const current=await open(base+`&spectator=${prefix}later&member=${prefix}member_later`);await wait(()=>current.binary.length===3,'latest stage only');
  assert.deepEqual(current.binary.map(b=>b[2]===83?b.readUInt32LE(20):b.readUInt32LE(16)),[next,next,next+1]);
  let returning=current;
  for(let attempt=0;attempt<2;++attempt){
    const admissions=late2.json.filter(m=>m.type==='spectator-start').length;
    send(late2,{type:'leave-spectator'});send(late2,{type:'spectate'});
    await wait(()=>late2.json.filter(m=>m.type==='spectator-start').length>admissions,'same-run spectator readmission');
    const reopened=await open(base+`&spectator=${prefix}later&member=${prefix}member_later`);
    await wait(()=>returning.closed,'leaving the spectator seat releases its old receiver');
    await wait(()=>reopened.binary.length===3,'returning observer receives a fresh current-stage bootstrap');
    assert.deepEqual(reopened.binary.map(b=>b[2]===83?b.readUInt32LE(20):b.readUInt32LE(16)),[next,next,next+1]);
    assert.equal(reopened.closed,false);assert.equal(p0.closed,false);assert.equal(p1.closed,false);
    returning=reopened;
  }
  if(ending==='failure'){
    // A gap kills only the optional stream. Both players retain their route.
    upload(p0,input(marker,900));await wait(()=>early.closed&&viewer.closed&&returning.closed,'bad stream closes viewers');
  }else{
    upload(p0,event(marker,3,next+1,5,4));
    await wait(()=>[early,viewer,returning].every(s=>s.binary.at(-1)?.[2]===83&&s.binary.at(-1)?.[5]===3),'continuous replay reaches End');
    const lengths=[early,viewer,returning].map(s=>s.binary.length);
    const stop=Buffer.from([0xe8,0x53,0x54,0x4f,0x50,1]);
    if(ending==='upload')p0.ws.send(stop);else send(s0,{type:'spectator-stop'});
    send(s0,{type:'signal',to:1,candidate:{candidate:'after-completion'}});
    await wait(()=>s1.json.some(m=>m.type==='signal'&&m.candidate?.candidate==='after-completion'),'stop signaling barrier');
    p0.ws.send(Buffer.from([0xe7,1,4,5,6]));
    await wait(()=>p1.binary.some(b=>b.equals(Buffer.from([4,5,6]))),'stop upload barrier');
    assert.ok([early,viewer,returning].every(s=>!s.closed),'completed replay stays available to buffered viewers');
    assert.deepEqual([early,viewer,returning].map(s=>s.binary.length),lengths,'stop adds no replay packet');
  }
  assert.equal(p0.closed,false);assert.equal(p1.closed,false);p0.ws.send(new Uint8Array([0xe7,1,9,8,7]));
  await wait(()=>p1.binary.some(b=>b.equals(Buffer.from([9,8,7]))),'player traffic survives');
  for(const s of sockets)s.close();await wait(()=>sockets.every(s=>s.readyState===3),'cleanup');sockets.length=0;
 }
 console.log('PASS six cooperative products: native-gated admission, current-stage prefix, Boss dependency, explicit entry frontier, stage rollover, repeated same-run spectator return, completed replay retention after upload/signal stop and player failure isolation');
}finally{for(const s of sockets)s.close();child.kill();}
