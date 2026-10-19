import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 communication gauge follows native position bands, graze hold and decay',async()=>{
 const c=await core(),m=await oracle(),f=c.communication_create(),e=c.communication_data(f,0),rate=c.communication_data(f,1),g=c.communication_data(f,2);
 const player=m.allocate(0x900);m.u32(0x4a8eb4,player);
 let checks=0;
 try {
  for(const speed of [1,.5,.99,1.01,1.25])for(const y of [0,63.999,64,79.999,80,127.999,128,400])for(const initial of [0,30,9999,10000,12999,13000]){
   const v=new DataView(c.memory.buffer);v.setFloat32(rate,speed,true);v.setInt32(e+12,initial,true);v.setInt32(g,initial,true);
   m.f32(0x4a7948,speed);m.f32(player+0x880,y);m.i32(0x4a56f4,initial);m.i32(0x4a56f8,initial);
   c.communication_reward(f,500);m.call(0x40baa0,{args:[500]});
   for(let frame=0;frame<24;++frame){
    c.communication_update(f,y);m.reg('EDI',0x4a56e0);m.call(0x420a70);
    const view=new DataView(c.memory.buffer);
    assert.equal(view.getInt32(e+12,true),m.i32(0x4a56f4),`primary ${speed}/${y}/${initial}/${frame}`);
    assert.equal(view.getInt32(g,true),m.i32(0x4a56f8),'secondary');
    assert.deepEqual(Buffer.from(memory(c,g+4,12)),Buffer.from(m.bytes(0x4a56fc,12)),'reward timer');++checks;
   }
  }
  report('communication',{passed:true,checks,scope:'Native 0x40baa0 reward and 0x420a70 position-band growth, decay, caps, secondary gauge and timer; caller scheduling validated separately.'});
 }finally{c.communication_delete(f);m.close();}
});
