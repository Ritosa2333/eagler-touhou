import test from'node:test';import assert from'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH17 presented spell clock and replay capture times match original',async()=>{
 const c=await core(),m=await oracle(),spell=m.allocate(0xc0),world=m.allocate(0xb0),replay=m.allocate(0x200),entry=m.allocate(0x158),time=m.allocate(8);
 m.u32(0x4b76a8,0);m.u32(0x4b7690,spell);m.u32(0x4b76b0,world);m.u32(0x4b77d8,replay);m.u32(0x4b59dc,1);m.u32(replay+0x24,entry);m.u32(replay+0x100,entry);m.u32(0x52ae30,1);
 const stub=Buffer.from([0xf2,0x0f,0x10,0x05,0,0,0,0,0xc3]);stub.writeUInt32LE(time,4);m.write(0x462710,stub);
 const f64=(p,v)=>new DataView(m.view(p,8).buffer,m.view(p,8).byteOffset,8).setFloat64(0,v,true),dv=()=>new DataView(c.memory.buffer);let checks=0,cases=0,f=0;
 try{for(let sample=0;sample<160;sample++){
  f=c.spell_capture_create();m.view(spell,0xc0).fill(0);m.view(entry,0x158).fill(0);const playing=sample%3===0;m.i32(world+0xa8,playing?1:0);
  for(let slot=0;slot<8;slot++){
   const encoded=sample%2?0x6ad1584:sample*12345+slot;dv().setInt32(c.spell_capture_field(f,5)+slot*4,encoded,true);m.i32(entry+0x104+slot*4,encoded);
   const began=11.234+sample*1.139+slot*141,elapsed=[.001,.00834,.00835,.0167,.02505,59.999,600.789,1500.123][(sample+slot)%8],frames=(sample+slot)*71;
   for(let phase=0;phase<4;phase++){
    const flags=phase<2?0x3:0x2;const saved=dv().getUint32(c.spell_capture_field(f,0),true)&0x40;dv().setUint32(c.spell_capture_field(f,0),flags|saved,true);m.u32(spell+0x78,flags|(m.u32(spell+0x78)&0x40));m.i32(spell+0x8c,frames);const now=began+(phase>=2?elapsed:0);f64(time,now);m.call(0x461c80);assert.equal(c.spell_capture_present(f,frames,now,playing?1:0),1);
    for(const[kind,off,n]of[[0,0x78,4],[1,0x90,4],[2,0xa8,4],[3,0x88,4],[4,0x98,8]])assert.deepEqual(Buffer.from(memory(c,c.spell_capture_field(f,kind),n)),Buffer.from(m.bytes(spell+off,n)),`${sample}/${slot}/${phase} field ${kind}`);
    assert.deepEqual(Buffer.from(memory(c,c.spell_capture_field(f,5),80)),Buffer.from(m.bytes(entry+0x104,80)));checks+=6;
   }
  }cases++;c.spell_capture_delete(f);f=0;
 }report('spell-capture-clock',{passed:true,cases,checks,originalFunctions:['0x461c80','0x41a570'],scope:'Original presentation-path spell timer, start/end flag 0x40, stopped frame snapshot, 0.0167 rounding, capped seconds and encoded hundredths, live per-stage replay capture-time writes and replay validation/fallback. Host clock is a controlled double; original arithmetic executes in the bounded oracle. Frame-rate telemetry and browser scheduling are separate.'});
 }finally{if(f)c.spell_capture_delete(f);m.close();}
});
