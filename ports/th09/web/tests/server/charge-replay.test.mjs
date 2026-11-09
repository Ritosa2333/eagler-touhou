import test from 'node:test';
import assert from 'node:assert/strict';
import {exportReplayName,importReplayName,validateChargeReplay,validateMotionReplay} from '../../sdl-runtime/motion-replay.mjs';

// Envelope validation is JS-owned. Native ChargeReplayTrack tests separately
// decode the compressed rows and validate their game-state semantics.
function extension(magic,game,version,payload){
  const bytes=new Uint8Array(payload.length+24),view=new DataView(bytes.buffer);
  bytes.set(payload);bytes.set(new TextEncoder().encode(magic),payload.length);
  view.setUint32(payload.length+8,game,true);view.setUint32(payload.length+12,version,true);
  view.setUint32(payload.length+16,payload.length,true);
  let hash=2166136261;for(const byte of payload)hash=Math.imul(hash^byte,16777619)>>>0;
  view.setUint32(payload.length+20,hash,true);return bytes;
}
function charge(){const payload=new Uint8Array(8);new DataView(payload.buffer).setUint32(0,40,true);return extension('T9CHARGE',9,1,payload);}
test('charge Replay exports as rpyx and imports through the native rpy path',()=>{
  const bytes=charge();assert.equal(validateChargeReplay(bytes,9),true);
  assert.equal(exportReplayName('th9_01.rpy',bytes,9),'th9_01.rpyx');
  assert.equal(importReplayName('th9_01.rpyx',bytes,9),'th9_01.rpy');
  assert.throws(()=>validateChargeReplay(bytes,8),/不属于这款游戏/);
  const native=new Uint8Array(512);assert.equal(validateChargeReplay(native,9),false);
  assert.equal(exportReplayName('th9_01.rpy',native,9),'th9_01.rpy');
  assert.throws(()=>importReplayName('th9_01.rpyx',native,9),/缺少播放记录/);
});
test('charge and touch extensions coexist and malformed charge envelopes are rejected',()=>{
  const first=charge(),motion=extension('THMOTION',9,2,new Uint8Array(80));
  const both=new Uint8Array(first.length+motion.length);both.set(first);both.set(motion,first.length);
  assert.equal(validateChargeReplay(both,9),true);assert.equal(exportReplayName('th9_01.rpy',both,9),'th9_01.rpyx');
  for(const at of [0,4,first.length-12,first.length-8,first.length-4]){
    const broken=first.slice();broken[at]^=1;assert.throws(()=>validateChargeReplay(broken,9),/录像数据损坏/);
  }
});

test('compact touch combines with charge and rejects malformed entries after a valid checksum',()=>{
  const payload=new Uint8Array(82);payload[0]=2;
  const replay=new Uint8Array([...charge(),...extension('THMOTION',9,3,payload)]);
  assert.equal(validateMotionReplay(replay,9),true);
  assert.equal(validateChargeReplay(replay,9),true);
  assert.equal(importReplayName('th9_01.rpyx',replay,9),'th9_01.rpy');
  for(const control of [5,4]){
    payload[81]=control;
    assert.throws(()=>validateMotionReplay(extension('THMOTION',9,3,payload),9),/录像数据损坏/);
  }
});

test('compact charge version is accepted and unknown versions rejected',()=>{
 const bytes=charge(),view=new DataView(bytes.buffer);view.setUint32(bytes.length-12,2,true);
 assert.equal(validateChargeReplay(bytes,9),true);
 view.setUint32(bytes.length-12,4,true);assert.throws(()=>validateChargeReplay(bytes,9),/录像数据损坏/);
 const payload=Uint8Array.of(40,0,0,0,40,0,0,0,1);
 assert.equal(validateChargeReplay(extension('T9CHARGE',9,3,payload),9),true);
 payload[4]=39;
 assert.throws(()=>validateChargeReplay(extension('T9CHARGE',9,3,payload),9),/录像数据损坏/);
});
