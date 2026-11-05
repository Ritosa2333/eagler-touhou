import test from 'node:test';
import assert from 'node:assert/strict';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';
import {SpectatorTape} from '../../sdl-runtime/spectator-tape.mjs';

function packet(length,frame,character=0){
  const bytes=new Uint8Array(length),v=new DataView(bytes.buffer);
  bytes.set(length===102?[84,57,66,71,2,0,2,0]:length===24?[84,57,83,69,1,0,2,0]:[84,57,83,80,length===126?2:1,3,2,0]);
  v.setUint32(8,frame,true);if(length===102)bytes[76]=character;
  return bytes;
}
function viewer(){
  const memory=new ArrayBuffer(4096),calls=[];let played=0,next=0;
  const core={HEAPU8:new Uint8Array(memory),HEAPU32:new Uint32Array(memory),
    _th09_spectator_frame:()=>played,_th09_spectator_playback_info:()=>64,_th09_spectator_begin_buffer:()=>1024,
    _th09_spectator_restart(frame,size,restore){calls.push(['restart',frame,size,restore,core.HEAPU8[1076]]);played=next=frame;return 1;},
    _th09_spectator_feed(frame){assert.equal(frame,next++);calls.push(['input',frame]);return 1;},
    _th09_spectator_charge_buffer:()=>2048,
    _th09_spectator_feed_charge(frame){assert.equal(frame,next++);calls.push(['charge',frame,...core.HEAPU8.slice(2048,2128)]);return 1;},
    _th09_spectator_control(action,target){calls.push(['control',action,target]);return 1;},
    _th09_spectator_finish(frame){assert.equal(frame,next);calls.push(['end',frame]);return 1;}};
  const n=new SharedNetplay(core,{onClose:reason=>assert.fail(reason),onStatus:()=>{}});
  Object.assign(n,{connected:true,active:true,spectator:true,timelineExperiment:true,buildBytes:new Uint8Array(12),spectatorTape:new SpectatorTape(),timeline:{resetMatch(){},refresh(){}}});
  return {n,calls,play:frame=>{played=frame;}};
}
test('real receive path retains played packets, rewinds locally, and continues receiving without a gap',()=>{
  const {n,calls,play}=viewer();n.receive(packet(102,100,3));
  for(let frame=100;frame<500;frame++)n.receive(packet(46,frame));
  play(400);const before=calls.length;n.seekSpectator(50);
  assert.deepEqual(calls.slice(before, before+2),[['restart',100,78,1,3],['control',4,150]]);
  for(let i=0;i<8;i++)n.drainSpectatorHistory();
  assert.deepEqual(calls.slice(before).filter(c=>c[0]==='input').map(c=>c[1]),Array.from({length:400},(_,i)=>100+i));
  n.receive(packet(46,500));assert.equal(n.spectatorFrame,501);assert.equal(n.spectatorTape.frame,501);assert.equal(n.spectatorTape.matches[0].length,401);
  assert.equal(n.spectatorTape.bytes,0);assert.equal(n.connected,true);
});
test('starting the next match releases the completed prefix and keeps current-match rewind',()=>{
  const {n,calls,play}=viewer();
  n.receive(packet(102,100,3));for(let frame=100;frame<102;frame++)n.receive(packet(46,frame));n.receive(packet(24,102));play(102);
  n.receive(packet(102,900,14));for(let frame=900;frame<1100;frame++)n.receive(packet(46,frame));play(1000);
  assert.equal(n.spectatorTape.matches.length,1);assert.equal(n.spectatorTape.locate(1),null);
  const before=calls.length;n.seekSpectator(1);assert.equal(calls.length,before,'completed match is no longer seekable');
  n.seekSpectator(3);assert.deepEqual(calls.filter(c=>c[0]==='restart').at(-1),['restart',900,78,1,14]);
  assert.equal(n.spectatorTape.current.frame,900);assert.equal(n.spectatorTape.timeline(901).position,3);
  n.goLive();assert.deepEqual(calls.filter(c=>c[0]==='restart').at(-1),['restart',900,78,1,14]);
  assert.equal(n.spectatorTape.current.frame,900);assert.equal(n.spectatorTape.matches.length,1);
  assert.deepEqual(calls.filter(c=>c[0]==='control').at(-1),['control',2,1100]);
  assert.equal(n.connected,true);assert.equal(n.spectatorTape.locate(3).match.frame,900,'go-live retains current-match history');
});

test('local charge records survive batching, local rewind and ordered live handoff byte for byte',()=>{
  const {n,calls,play}=viewer();n.receive(packet(102,100,2));
  const batch=new Uint8Array(126*200);
  for(let i=0;i<200;++i){const row=packet(126,100+i);row.fill(i,46);batch.set(row,126*i);}
  for(let at=0;at<batch.length;at+=126*50)n.receive(batch.subarray(at,at+126*50));
  play(260);const before=calls.length;n.seekSpectator(40);
  for(let i=0;i<8;++i)n.drainSpectatorHistory();
  const restored=calls.slice(before).filter(c=>c[0]==='charge');
  assert.equal(restored.length,200);
  restored.forEach((row,i)=>assert.deepEqual(row,['charge',100+i,...new Array(80).fill(i)]));
  n.receive(packet(126,300));assert.equal(n.spectatorFrame,301);assert.equal(n.spectatorTape.matches[0].length,201);
  assert.equal(n.spectatorTape.bytes,0);assert.equal(n.connected,true);
});
