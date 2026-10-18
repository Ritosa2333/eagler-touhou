import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 score defaults, compressed storage and reload match original routines',async()=>{
 const c=await core(),m=await oracle(),f=c.score_file_create(),loaded=c.score_file_create(),out=c.allocate(1000000),rng=c.allocate(8),manager=m.allocate(0x2e21c),header=m.allocate(24),heap=m.heap;let checks=0,captured;
 const stop='SCORE_OUTPUT_READY';m.u32(0x4a8ebc,manager);
 m.replace(0x4587a0,'capture score file before opening filesystem',()=>{const h=Buffer.from(m.bytes(header,24));captured=Buffer.concat([h,Buffer.from(m.bytes(m.reg('EDI'),h.readUInt32LE(16)))]);throw Error(stop);});
 try{for(let sample=0;sample<8;++sample){
  m.heap=heap;m.view(manager,0x2e21c).fill(0);m.u32(manager,header);const h=Buffer.alloc(24);h.write('TH11');h.writeUInt16LE(4,8);h.writeUInt32LE(256,12);m.write(header,h);
  const seed=Buffer.alloc(8);seed.writeUInt16LE(sample*7927);memory(c,rng,8).set(seed);m.write(0x4c2f00,seed);c.score_file_initialize(f,rng);
  m.reg('EAX',manager+0x2ddd4);m.call(0x437660);assert.deepEqual(Buffer.from(memory(c,c.score_file_data(f,7),0x448)),Buffer.from(m.bytes(manager+0x2ddd4,0x448)),'settings initialization');assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(0x4c2f00,8)));
  for(let selection=0;selection<7;++selection){m.reg('EAX',manager+8+selection*0x68d4);m.call(0x437550);assert.deepEqual(Buffer.from(memory(c,c.score_file_data(f,selection),0x68d4)),Buffer.from(m.bytes(manager+8+selection*0x68d4,0x68d4)),`defaults ${selection}`);
   // Exercise nonempty high scores, clear counts and spell records while
   // preserving native structural fields and all unknown record bytes.
   const data=Buffer.from(memory(c,c.score_file_data(f,selection),0x68d4));data.writeUInt32LE(12345678+selection+sample,16);data.writeUInt32LE(sample+1,0x590+selection%5*4);data.write('native oracle',0x664+(sample*19)*0x90);data.writeUInt32LE(sample*13,0x6e4+sample*19*0x90);data.writeUInt32LE(sample*17,0x6e8+sample*19*0x90);memory(c,c.score_file_data(f,selection),data.length).set(data);m.write(manager+8+selection*0x68d4,data);
  }
  captured=null;assert.throws(()=>m.call(0x437a70,{limit:800000000}),new RegExp(stop));const size=c.score_file_save(f,out,1000000);assert.ok(size);assert.deepEqual(Buffer.from(memory(c,out,size)),captured,'serialized header and encrypted payload');
  assert.equal(c.score_file_open(loaded,out,size),1);for(let selection=0;selection<8;++selection){const n=selection<7?0x68d4:0x448,np=selection<7?manager+8+selection*0x68d4:manager+0x2ddd4;assert.deepEqual(Buffer.from(memory(c,c.score_file_data(loaded,selection),n)),Buffer.from(m.bytes(np,n)),`reload ${selection}`);}
  ++checks;
 }report('score-file',{passed:true,checks,scope:'Original 437550/437660 defaults including 512 RNG samples, 437a70 checksums, LZSS, encryption and complete serialized file. Nonempty records and C++ reload preserve bytes; menu/result insertion and browser persistence are separate integration gates.'});
 }finally{for(const p of[out,rng])c.release(p);c.score_file_delete(f);c.score_file_delete(loaded);m.close();}
});
