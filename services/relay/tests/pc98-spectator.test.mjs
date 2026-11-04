import {test} from 'node:test';
import assert from 'node:assert/strict';
import {Pc98SpectatorRelay,isPc98SpectatorRoom} from '../src/pc98-spectator.mjs';

function packet(kind,first,count,body=[1]){
  const p=new Uint8Array(16+body.length),v=new DataView(p.buffer);p.set([0x50,0x39,0x38,0x53,1,kind,2,0]);
  v.setUint32(8,first,true);v.setUint32(12,count,true);p.set(body,16);return p;
}
test('PC98 relay retains an ordered confirmed prefix for late viewers',()=>{
  assert(isPc98SpectatorRoom('th02mp-1234'));assert(!isPc98SpectatorRoom('th06mp-1234'));
  const relay=new Pc98SpectatorRelay(2);
  assert.throws(()=>relay.accept(packet(2,0,1)));
  relay.accept(packet(1,0,0));relay.accept(packet(2,0,60));
  assert.throws(()=>relay.accept(packet(2,59,1)));
  relay.accept(packet(2,60,4));relay.accept(packet(3,64,0,[]));
  assert.equal(relay.next,64);assert.equal(relay.history.length,4);assert(relay.ended);
  assert.throws(()=>relay.accept(packet(2,64,1)));
});
