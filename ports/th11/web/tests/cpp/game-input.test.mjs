import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 live game keys, auto focus, weapon switch and replay edge records match original',async()=>{
 const c=await core(),m=await oracle(),manager=m.allocate(0x300),chunk=m.allocate(0x18b0),link=m.allocate(12),clock=m.allocate(0x80);m.u32(0x4a8e88,1);m.u32(0x4a8d80,clock);m.f32(clock+0x34,60);m.u32(manager+0x9c,link);m.u32(link,chunk);let checks=0;
 try{for(const auto of[false,true]){const p=c.game_input_create();m.view(0x4c92a8,0x138).fill(0);m.u32(0x4c3480,auto?0x200:0);m.u32(chunk+0x1518,chunk);m.u32(chunk+0x18a0,chunk+0x151c);m.u32(manager+0x1cc,0);
  for(let frame=0;frame<820;++frame){const keys=frame<400?[0,1,9,8,9,1,0,9,1,8][Math.floor(frame/40)]:(frame*14379)&0x7ff;m.u32(0x4c92a8,keys);m.reg('EAX',manager);m.call(0x435fe0);c.game_input_tick(p,keys,+auto);const dv=new DataView(c.memory.buffer);
   for(const[off,np]of[[0,0x4c93c0],[4,0x4c93c4],[8,0x4c93cc],[12,0x4c93d0],[16,0x4c93c8],[20,0x4c93d8],[152,0x4c93bc]])assert.equal(dv.getUint32(p+off,true),m.u32(np),`auto${auto} frame${frame} offset${off}`);
   assert.deepEqual(Buffer.from(memory(c,p+24,128)),Buffer.from(m.bytes(0x4c933c,128)),`frame${frame} repeat counters`);++checks;
  }c.game_input_delete(p);
 }report('game-input',{passed:true,checks,scope:'Native 435fe0 plus 40de10 normal/automatic focus, Marisa-B combined-key switch, all 32 repeat counters, edges and held masks used by original replay recording.'});
 }finally{m.close();}
});
