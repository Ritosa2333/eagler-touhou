import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 enemy damage, health and timeout interrupts match original phase transitions',async()=>{
 const c=await core(),m=await oracle(),f=c.enemy_fixture_create(),p=c.enemy_fixture_data(f,0),s=p+0x103c,w=c.enemy_fixture_data(f,2),q=m.allocate(0x2678),hud=m.allocate(0x4450),spell=m.allocate(0x900),special=m.allocate(0x50),manager=m.allocate(0x100),phase=c.allocate(40),globals=c.allocate(24);let damageChecks=0,phaseChecks=0,seed=7883;
 const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};const put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v>>>0,true);
 const normalize=b=>{b=Buffer.from(b);b.writeUInt32LE(0,0x119c);return b;};
 try{
  m.u32(0x4a8d84,hud);m.u32(0x4a8d6c,spell);m.u32(0x4a8d64,special);m.u32(0x4a8d7c,manager);
  const fields=[w+8,globals,globals+4,globals+8,globals+12,globals+16,globals+20,0,0,0];fields.forEach((x,i)=>put(phase+i*4,x));
  for(let i=0;i<12000;++i){const data=Buffer.alloc(24);for(let j=0;j<6;++j)data.writeUInt32LE(random(),j*4);memory(c,p+0x252c,24).set(data);m.write(q+0x252c,data);const amount=random()|0;m.reg('EAX',amount);assert.equal(c.enemy_damage(s,amount),m.call(0x4100e0,{ecx:q+0x252c})|0);assert.deepEqual(Buffer.from(memory(c,p+0x252c,24)),Buffer.from(m.bytes(q+0x252c,24)));++damageChecks;}
  for(let example=0;example<6000;++example){const b=Buffer.alloc(0x2678),g=Buffer.alloc(24);for(let i=0;i<6;++i)g.writeUInt32LE(random(),i*4);const health=random()%20000,frame=[0,59,60,119,5999,6000,6001,10000][example%8],active=example%3,character=example%2,subtype=example%3;
   b.writeInt32LE(health,0x252c);b.writeInt32LE(frame,0x1194);b.writeFloatLE(frame+.375,0x1198);b.writeUInt32LE(example%2,0x11a0);b.writeUInt32LE(random(),0x25bc);b.writeUInt32LE(w+8,0x119c);
   for(let i=0;i<8;++i){const off=0x25d0+i*16;b.writeInt32LE((example+i)%4===0?-1:random()%15000,off);b.writeInt32LE((example+i)%5===0?-1:(random()%11000)+1,off+4);b.writeUInt32LE(0x1000+i*16,off+8);b.writeUInt32LE(0x1008+i*16,off+12);}
   // Force ordered health and timeout boundary cases in half the fixtures.
   if(example%2===0){for(let i=0;i<8;++i)b.writeInt32LE(-1,0x25d0+i*16);const slot=(example>>1)%8,off=0x25d0+slot*16;b.writeInt32LE(health+(example%6===0?0:-1),off);b.writeInt32LE(frame+(example%4?1:-1),off+4);}
   g.writeUInt32LE(example%64,8);g.writeInt32LE([0,59,60,61,200][example%5],12);
   memory(c,p,b.length).set(b);b.writeUInt32LE(0x4a7948,0x119c);m.write(q,b);memory(c,globals,24).set(g);m.write(hud+0x4440,g.subarray(0,8));m.write(spell+0x8e0,g.subarray(8,12));m.write(spell+0x88c,g.subarray(12,16));m.write(spell+0x8e4,g.subarray(16,20));m.write(manager+0x18,g.subarray(20,24));put(phase+28,active);put(phase+32,character);put(phase+36,subtype);m.i32(special+0x3c,active);m.i32(0x4a5710,character);m.i32(0x4a5714,subtype);
   const native=m.call(0x417b40,{ecx:q}),actual=c.enemy_interrupt(s,phase);assert.equal(actual>>>0,native,`phase ${example} return`);assert.deepEqual(normalize(memory(c,p,0x2678)),normalize(m.bytes(q,0x2678)),`phase ${example} state`);
   const expected=Buffer.concat([Buffer.from(m.bytes(hud+0x4440,8)),Buffer.from(m.bytes(spell+0x8e0,4)),Buffer.from(m.bytes(spell+0x88c,4)),Buffer.from(m.bytes(spell+0x8e4,4)),Buffer.from(m.bytes(manager+0x18,4))]);assert.deepEqual(Buffer.from(memory(c,globals,24)),expected,`phase ${example} globals`);++phaseChecks;
  }
  report('enemy-phase',{passed:true,damageChecks,phaseChecks,scope:'sevenths damage accumulation, signed overflow, ordered health interrupts, timeout countdown and spell state changes'});
 }finally{c.release(phase);c.release(globals);c.enemy_fixture_delete(f);m.close();}
});