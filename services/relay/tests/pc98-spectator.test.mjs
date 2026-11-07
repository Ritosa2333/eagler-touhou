import {test} from 'node:test';
import assert from 'node:assert/strict';
import {Pc98SpectatorRelay,isPc98SpectatorRoom} from '../src/pc98-spectator.mjs';
import {createHash} from 'node:crypto';

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

test('a confirmed match initializer replaces late-join history',()=>{
  const relay=new Pc98SpectatorRelay(2,{matchInitializers:true}),checkpoint=(frame,text)=>{
    const data=Buffer.from(text),body=Buffer.alloc(72+data.length);body.writeUInt32LE(data.length,0);body.writeUInt32LE(0,4);
    body.write(createHash('sha256').update(data).digest('hex'),8);data.copy(body,72);
    relay.accept(packet(4,frame,0,body));
  };
  relay.accept(packet(1,0,0));relay.accept(packet(2,0,60));relay.accept(packet(2,60,60));
  checkpoint(0,'base');checkpoint(80,'match');relay.accept(packet(6,120,80,[]));
  assert.deepEqual(relay.history.map(p=>[p[5],p.readUInt32LE(8)]),[[1,0],[4,0],[6,120],[4,80],[2,60]]);
  relay.accept(packet(2,120,1));assert.equal(relay.next,121);
  relay.accept(packet(7,121,121,[]));assert.equal(relay.pendingMatch,121);
  relay.accept(packet(6,121,80,[]));assert.equal(relay.pendingMatch,121,'an older completed initializer must not reopen admission');
  relay.accept(packet(2,121,2));checkpoint(121,'next match');relay.accept(packet(6,123,121,[]));
  assert.equal(relay.pendingMatch,0);
  assert.deepEqual(relay.history.map(p=>[p[5],p.readUInt32LE(8)]),[[1,0],[4,0],[6,123],[4,121],[2,120],[2,121]]);
  assert.throws(()=>relay.accept(packet(6,123,79,[])));
});
