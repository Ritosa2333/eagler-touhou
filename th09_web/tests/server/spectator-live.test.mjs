import test from 'node:test';
import assert from 'node:assert/strict';
import {SpectatorTape} from '../../sdl-runtime/spectator-tape.mjs';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';

const packet=(length,frame)=>{const p=new Uint8Array(length);new DataView(p.buffer).setUint32(8,frame,true);return p;};
test('explicit live jump discards older queued matches, preserving newest input and receive validation',()=>{
  const tape=new SpectatorTape();tape.frame=301;tape.ended=false;
  for(const [n,f] of [[46,1],[24,2],[74,100],[46,100],[24,101],[74,300],[46,300]])tape.append(packet(n,f));
  assert.equal(tape.advanceToLatestBegin(),true);
  assert.equal(new DataView(tape.peek().buffer).getUint32(8,true),300);
  assert.equal(tape.begins,1);assert.equal(tape.bytes,120);assert.equal(tape.frame,301);
  tape.shift();assert.equal(tape.advanceToLatestBegin(),false);assert.equal(tape.peek().length,46);
  tape.shift();assert.equal(tape.bytes,0);assert.equal(tape.advanceToLatestBegin(),false);
});
test('go-live restarts newest queued match even when paused in an older match',()=>{
  const buffer=new ArrayBuffer(4096),calls=[];
  const core={HEAPU8:new Uint8Array(buffer),HEAPU32:new Uint32Array(buffer),
    _th09_spectator_begin_buffer:()=>1024,
    _th09_spectator_restart:(frame,size)=>{calls.push(['restart',frame,size]);return 1;},
    _th09_spectator_control:(...args)=>{calls.push(['control',...args]);return 1;},
    _th09_spectator_feed:(frame)=>{calls.push(['feed',frame]);return 1;}};
  const n=new SharedNetplay(core,{});n.connected=true;n.spectator=true;n.timelineExperiment=true;
  n.spectatorFrame=10;n.spectatorEnded=true;
  n.spectatorTape=new SpectatorTape();
  n.timeline={resetMatch:frame=>calls.push(['timeline',frame]),refresh:()=>{}};
  for(const [size,frame] of [[74,100],[46,100],[24,101],[74,300],[46,300]])n.spectatorTape.append(packet(size,frame));
  n.goLive();
  assert.deepEqual(calls.filter(c=>c[0]==='restart'),[['restart',300,50]]);
  assert.deepEqual(calls.filter(c=>c[0]==='feed'),[['feed',300]]);
  assert.equal(n.spectatorFrame,301);assert.equal(n.spectatorEnded,false);
  assert.equal(n.spectatorTape.bytes,0);
  n.goLive();assert.equal(calls.filter(c=>c[0]==='restart').length,1);
});
