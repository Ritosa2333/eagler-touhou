import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,existsSync} from 'node:fs';
import {th19StreamBegin,th19StreamFrame,th19StreamEnd,parseTh19StreamPacket,encodeTh19Replay,decodeTh19Replay,th19InputEdges} from '../web/sdl-runtime/th19-recording.mjs';
import {Netplay} from '../web/sdl-runtime/netplay.mjs';
import {createReplayPlayer,createVersusRecorder,ConfirmedTimeline,VersusReplayRunner} from '../web/sdl-runtime/replay-session.mjs';
const build='a'.repeat(64),config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build,seed:123,difficulty:3,characters:[0,18],delay:0,rollback:true};
const input={keys:3,direct:1,x:1.25,y:-2.5};
test('standalone Runtime and Relay decode the exact same recording protocol',()=>{
 const shared=new URL('../../../common/contracts/th19-recording.mjs',import.meta.url);
 if(existsSync(shared))assert.equal(readFileSync(new URL('../web/sdl-runtime/th19-recording.mjs',import.meta.url),'utf8'),readFileSync(shared,'utf8'));
 assert.deepEqual(parseTh19StreamPacket(th19StreamBegin(config)).config,config);
 assert.deepEqual(parseTh19StreamPacket(th19StreamFrame(0,input,input,{hash:0xffffffff,result:2})),{kind:'frame',frame:0,left:input,right:input,hash:0xffffffff,result:2,played:false});
 assert.equal(parseTh19StreamPacket(th19StreamEnd(1)).frame,1);
 assert.throws(()=>th19StreamFrame(0,{...input,x:NaN},input));
 assert.throws(()=>th19StreamBegin({...config,rules:[0,1,1,3,4,1,1,0,0]}));
});
test('portable versus file validates CRC, sequence, result and metadata',()=>{
 const meta={kind:'versus',config,build,seed:123,frames:1,startedAt:0};
 const bytes=encodeTh19Replay(meta,th19StreamFrame(0,input,input,{played:true}));assert.equal(decodeTh19Replay(bytes).meta.frames,1);
 const corrupt=bytes.slice();corrupt[corrupt.length-10]^=1;assert.throws(()=>decodeTh19Replay(corrupt),/校验/);
 assert.throws(()=>encodeTh19Replay({...meta,frames:2},th19StreamFrame(0,input,input)));
 assert.throws(()=>encodeTh19Replay(meta,th19StreamFrame(1,input,input)),/不连续/);
});
test('new files reject single-player and unplayed sessions',()=>{
 const meta={kind:'normal',files:[],build,seed:123,frames:10,startedAt:0};
 assert.throws(()=>encodeTh19Replay(meta,new Uint8Array(80)),/只保存联机/);
 assert.throws(()=>encodeTh19Replay({...meta,kind:'versus',config,frames:1},th19StreamFrame(0,input,input)),/尚未正式/);
});
test('only contiguous committed frames are saved once, transport failure leaves players connected',async()=>{
 const files=[],recorder=createVersusRecorder({build,write:bytes=>files.push(bytes)}),notices=[];
 const n=new Netplay({}, {onBegin:c=>recorder.begin(c),onConfirmed:b=>recorder.frame(b),onEnd:d=>recorder.end(d),onStatus:s=>notices.push(s)});
 n.connected=true;n.side=0;n.socket={sendSpectator(){throw Error('slow observer');}};
 n.onBegin(config);n.publish(th19StreamBegin(config));n.confirm(0,input,input,{result:1,hash:null,played:true});
 assert.equal(n.connected,true);assert.equal(notices.length,1);
 assert.throws(()=>n.confirm(0,input,input,{result:1}),/不连续/);
 await n.finishRecording(true);await n.finishRecording(true);assert.equal(files.length,1);assert.equal(decodeTh19Replay(files[0]).meta.frames,1);
});
test('spectator never sends input and drains only verified ordered frames',()=>{
 const OldSocket=globalThis.WebSocket,sockets=[];
 class Socket{constructor(){sockets.push(this);}send(){assert.fail('spectator must be receive-only');}close(){}}
 globalThis.WebSocket=Socket;const core={_th19_network_set_rules:()=>1,_th19_network_begin:()=>1,_th19_network_step:()=>1,_th19_network_end(){}};
 const n=new Netplay(core);n.config=config;
 try{
  n.connectSpectator('ws://localhost/',{});const socket=sockets[0],receive=b=>socket.onmessage({data:b.buffer});
  receive(th19StreamBegin(config));receive(th19StreamFrame(0,input,input));receive(th19StreamEnd(1));
  n.budget=1;n.pumpSpectator();assert.equal(n.frame,1);assert.equal(n.completed,true);
 }finally{n.close();globalThis.WebSocket=OldSocket;}
});
test('spectator checks state hashes even when the stream supplies a legacy flag',()=>{
 const OldSocket=globalThis.WebSocket;
 globalThis.WebSocket=class{close(){}};
 const core={_th19_network_end(){},_th19_network_set_rules:()=>1,_th19_network_begin:()=>1,
  _th19_network_step:()=>1,_th19_network_hash:()=>456};
 const n=new Netplay(core);n.config=config;
 try{
  n.connectSpectator('ws://localhost/',{});
  const receive=bytes=>n.socket.onmessage({data:bytes.buffer});
  receive(th19StreamBegin({...config,legacy:true}));
  receive(th19StreamFrame(0,input,input,{played:true,hash:123}));
  assert.throws(()=>n.spectatorRunner.step(),/状态校验失败/);
  assert.equal(n.spectatorRunner.frame,0);
 }finally{n.close();globalThis.WebSocket=OldSocket;}
});
test('spectator presents a frame even if catch-up exhausts its time slice', t=>{
 let now=100;
 t.mock.method(performance,'now',()=>now);
 const presented=[],n=new Netplay({});
 const runner={frame:0,step(present){presented.push(present);++this.frame;now+=9;}};
 Object.assign(n,{connected:true,active:true,lastTick:80,budget:0,receivedFrames:120,
  spectatorRunner:runner,spectatorTarget:null,spectatorPaused:false,spectatorFollowing:true});
 n.pumpSpectator();
 assert.deepEqual(presented,[true]);
 assert.equal(n.frame,1);
});
test('Launcher sync updates one file and leaves the match recorder live',async()=>{
 const files=[],r=createVersusRecorder({build,write:(bytes,path)=>{files.push({bytes,path});return path||'replay/th19_ud0000.rpy';}});
 r.begin(config);r.frame(th19StreamFrame(0,input,input,{played:true}));await r.snapshot();
 r.frame(th19StreamFrame(1,input,input));await r.snapshot();await r.end();
 assert.equal(files.length,3);assert.equal(decodeTh19Replay(files[2].bytes).meta.partial,false);assert.equal(files[0].path,null);assert.equal(files[1].path,'replay/th19_ud0000.rpy');
 assert.equal(decodeTh19Replay(files[1].bytes).meta.frames,2);
});
test('versus initialization and countdown without actual gameplay never save a recording',async()=>{
 const files=[],notices=[],r=createVersusRecorder({build,write:bytes=>files.push(bytes),notice:message=>notices.push(message)});
 r.begin(config);for(let frame=0;frame<120;frame++)r.frame(th19StreamFrame(frame,input,input));
 await r.snapshot();await r.end();assert.equal(files.length,0);assert.equal(notices.length,0);
});

test('native edge tuples preserve press, hold, release for both seats',()=>{
 assert.deepEqual(th19InputEdges(3,6),{held:6,pressed:4,released:1});
 const rows=[3,3,0,4].map((keys,frame)=>th19StreamFrame(frame,{...input,keys},{...input,keys:keys^1},{played:true}));
 const payload=new Uint8Array(rows.length*44);rows.forEach((r,i)=>payload.set(r,i*44));
 const bytes=encodeTh19Replay({kind:'versus',config,build,seed:123,frames:rows.length,startedAt:0},payload);
 assert.equal(bytes[6],2);assert.deepEqual(decodeTh19Replay(bytes).payload,payload);
 const v=new DataView(bytes.buffer),start=16+v.getUint32(8,true);
 assert.deepEqual([0,1,2,3].map(f=>[0,2,4].map(o=>v.getUint16(start+f*44+4+o,true))),[[3,3,0],[3,0,0],[0,0,3],[4,4,0]]);
 // Even a correctly checksummed file cannot supply impossible key edges.
 v.setUint16(start+44+6,3,true);let crc=0xffffffff;
 for(const byte of bytes.subarray(0,bytes.length-4)){crc^=byte;for(let i=0;i<8;i++)crc=(crc>>>1)^((crc&1)?0xedb88320:0);}
 v.setUint32(bytes.length-4,(crc^0xffffffff)>>>0,true);assert.throws(()=>decodeTh19Replay(bytes),/边沿/);
});
test('pending save and rematch never mix headers, paths or inputs',async()=>{
 let release;const gate=new Promise(r=>release=r),files=[];
 const recorder=createVersusRecorder({build,write:async(bytes,path)=>{await gate;files.push({bytes,path});return 'file-'+files.length;}});
 recorder.begin(config);recorder.frame(th19StreamFrame(0,input,input,{played:true}));
 const oldEnd=recorder.end();recorder.begin({...config,seed:456});recorder.frame(th19StreamFrame(0,{...input,keys:0},input,{played:true}));const nextEnd=recorder.end();
 release();await Promise.all([oldEnd,nextEnd]);assert.deepEqual(files.map(f=>decodeTh19Replay(f.bytes).meta.seed),[123,456]);assert.ok(files.every(f=>f.path===null));
});
test('shared runner restarts seed on rewind and stops at first mismatch',()=>{
 const tape=new ConfirmedTimeline(config);for(let i=0;i<3;i++)tape.append(th19StreamFrame(i,input,input,{played:true,hash:i+1}));tape.end();
 let cursor=0,starts=0,bad=false;
 const core={_th19_network_end(){},_th19_network_set_rules:()=>1,_th19_network_begin(seed){assert.equal(seed,123);cursor=0;++starts;return 1;},_th19_network_step(frame){assert.equal(frame,cursor++);return 1;},_th19_network_hash:()=>bad?999:cursor};
 const runner=new VersusReplayRunner(core,tape);runner.reset();runner.step();runner.step();runner.seek(0);runner.step();assert.equal(starts,2);bad=true;assert.throws(()=>runner.step(),/校验失败/);assert.equal(runner.frame,1);assert.throws(()=>runner.step(),/已停止/);
 assert.throws(()=>tape.append(th19StreamFrame(3,input,input)),/不连续/);
});
test('file playback rejects a mismatched build before touching the runtime', async()=>{
 const otherBuild='b'.repeat(64);
 const bytes=encodeTh19Replay({kind:'versus',config:{...config,build:otherBuild},
  build:otherBuild,seed:config.seed,frames:1,startedAt:0},th19StreamFrame(0,input,input,{played:true,hash:123}));
 const player=createReplayPlayer({getCore:()=>assert.fail('replay must not start'),build});
 await assert.rejects(player.open('replay/older.rpy',bytes),/不同游戏构建/);
});

test('paused observer saves received confirmed input once, independent of its cursor',async()=>{
 const OldSocket=globalThis.WebSocket;let socket;globalThis.WebSocket=class{constructor(){socket=this;}close(){}};
 const files=[],recorder=createVersusRecorder({build,write:bytes=>files.push(bytes)});
 const core={_th19_network_end(){},_th19_network_set_rules:()=>1,_th19_network_begin:()=>1,_th19_network_step:()=>1};
 const n=new Netplay(core,{onBegin:c=>recorder.begin(c),onConfirmed:b=>recorder.frame(b),onEnd:d=>recorder.end(d)});n.config=config;
 try{
  n.connectSpectator('ws://localhost/?run=1',{});const receive=b=>socket.onmessage({data:b.buffer});receive(th19StreamBegin(config));
  n.spectatorControl({run:'1',action:'pause'});
  for(let i=0;i<3;i++)receive(th19StreamFrame(i,input,input,{played:true}));
  receive(th19StreamEnd(3));await n.replayFlush;assert.equal(n.frame,0);assert.equal(n.confirmedFrames,3);assert.equal(files.length,1);assert.equal(decodeTh19Replay(files[0]).meta.frames,3);
  n.spectatorControl({run:'1',action:'live'});n.budget=8;n.pumpSpectator();await n.replayFlush;assert.equal(n.frame,3);assert.equal(files.length,1);
 }finally{n.close();globalThis.WebSocket=OldSocket;}
});
test('disconnect retains observer history and finalizes a partial recording',async()=>{
 const OldSocket=globalThis.WebSocket;let socket;globalThis.WebSocket=class{constructor(){socket=this;}close(){}};
 const ended=[],core={_th19_network_end(){},_th19_network_set_rules:()=>1,_th19_network_begin:()=>1};
 const n=new Netplay(core,{onEnd:d=>ended.push(d)});n.config=config;
 try{n.connectSpectator('ws://localhost/?run=1',{});socket.onmessage({data:th19StreamBegin(config).buffer});socket.onmessage({data:th19StreamFrame(0,input,input,{played:true}).buffer});socket.onclose();await n.replayFlush;assert.equal(n.connected,true);assert.equal(n.spectatorTape.length,1);assert.equal(ended[0].partial,true);}
 finally{n.close();globalThis.WebSocket=OldSocket;}
});
test('timeline rejects duplicate, missing and post-result input without advancing',()=>{
 const tape=new ConfirmedTimeline(config);assert.throws(()=>tape.append(th19StreamFrame(1,input,input)),/不连续/);assert.equal(tape.length,0);
 tape.append(th19StreamFrame(0,input,input,{played:true,result:2}));assert.throws(()=>tape.append(th19StreamFrame(1,input,input)),/不连续/);assert.equal(tape.length,1);tape.end(1);
});

 test('spectator accepts committed in-game characters/cards but rejects changed build and rules',()=>{
 const old=globalThis.WebSocket;let socket;globalThis.WebSocket=class{constructor(){socket=this;}close(){}};
 const selected={...config,characters:[15,7],rules:[...config.rules.slice(0,7),3,5]};
 try{for(const kind of ['selected','build','difficulty','lives']){
  let begun;const core={_th19_network_set_rules:()=>1,_th19_network_begin:(...args)=>{begun=args;return 1;},_th19_network_end(){}};
  const n=new Netplay(core);n.config=structuredClone(config);n.connectSpectator('ws://localhost/',{});
  const c=structuredClone(selected);if(kind==='build')c.build='b'.repeat(64);if(kind==='difficulty')c.difficulty=0;if(kind==='lives')c.rules[3]=c.rules[4]=2;
  socket.onmessage({data:th19StreamBegin(c).buffer});
  assert.equal(n.prepared,kind==='selected');if(kind==='selected'){assert.deepEqual(begun,[config.seed,config.difficulty,15,7]);assert.deepEqual(n.config.rules,c.rules);}
  n.close();
 }}finally{globalThis.WebSocket=old;}
 });
