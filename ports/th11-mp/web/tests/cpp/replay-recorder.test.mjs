import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 replay writer produces the original header, compression and encrypted payload',async()=>{
 const c=await core(),m=await oracle(),r=c.recorder_create(),reader=c.replay_create(),out=c.allocate(1000000),state=c.allocate(144),name=c.allocate(16),manager=m.allocate(0x300),fileHeader=m.allocate(36),header=m.allocate(112),clock=m.allocate(0x80),nativeName=m.allocate(16),filename=m.allocate(32),heap=m.heap;
 memory(c,name,9).set(Buffer.from('TEST\0'));m.write(nativeName,Buffer.from('TEST\0'));m.write(filename,Buffer.from('th11_01.rpy\0'));m.u32(0x4a8eb8,manager);m.u32(0x4a8d80,clock);
 const b=Buffer.alloc(8);b.writeDoubleLE(100);m.write(clock+0x24,b);m.write(clock+0x2c,b);
 m.replace(0x460885,'directory boundary',()=>0);
 m.replace(0x46025c,'path formatting boundary',()=>0);
 let nativeOutput;const stop='REPLAY_OUTPUT_READY';
 m.replace(0x4587a0,'capture serialized replay before filesystem write',()=>{const sp=m.reg('ESP'),p=m.u32(sp+0x14),h=Buffer.from(m.bytes(fileHeader,36));nativeOutput=Buffer.concat([h,Buffer.from(m.bytes(p,h.readUInt32LE(28)))]);throw Error(stop);});
 let checks=0;
 try{for(let sample=0;sample<12;++sample){
  m.heap=heap;m.view(manager,0x300).fill(0);m.u32(manager+0x14,fileHeader);m.u32(manager+0x18,header);
  const fh=Buffer.alloc(36);fh.write('t11r');fh.writeUInt16LE(4,4);fh.writeUInt32LE(0x100,16);m.write(fileHeader,fh);
  assert.equal(c.recorder_begin(r,sample%2,sample%3,sample%5,0,1750000000),1);
  const nh=Buffer.from(memory(c,c.recorder_header(r,0),112));nh.writeInt32LE(7,104);nh.writeInt32LE(2,108);m.write(header,nh);m.i32(0x4a56e4,1234567);
  let last=0,total=0;
  for(let st=1;st<=(sample%3)+1;++st){
   const entry=Buffer.alloc(144);entry.writeUInt16LE(st);entry.writeUInt16LE(7321+st,2);entry.writeInt32LE(3000+st,12);entry.writeInt16LE(100,16);entry.writeInt32LE(54321,20);entry.writeInt16LE(2,24);entry.writeInt32LE(100,36);
   memory(c,state,144).set(entry);assert.equal(c.recorder_stage(r,st,state,+(st===1)),1);const nst=m.allocate(144);m.u32(manager+0x1c+st*4,nst);m.write(nst,Buffer.from(memory(c,c.recorder_header(r,st),144)));
   const frames=[1,29,30,31,899,900,901,1789,1800,1801,18,90][sample],keys=[];for(let f=0;f<frames;++f){const held=(f*93)&0x7ff,pressed=held&~((f-1)*93),released=((f-1)*93)&~held;assert.equal(c.recorder_tick(r,held,pressed,released,60),1);const input=Buffer.alloc(6);input.writeUInt16LE(held);input.writeUInt16LE(pressed&65535,2);input.writeUInt16LE(released&65535,4);keys.push(input);}
   const terminal=sample%2&&st===(sample%3)+1;assert.equal(c.recorder_finish(r,+terminal),1);if(terminal)keys.push(Buffer.alloc(6,255));
   const fps=Buffer.alloc(Math.ceil(frames/30),60);let foff=0,ioff=0,prev=0;
   while(ioff<keys.length){const chunk=m.allocate(0x18b0),link=chunk+0x18a4,n=Math.min(900,keys.length-ioff),nf=Math.min(fps.length-foff,Math.ceil(n/30));m.view(chunk,0x18b0).fill(0);m.write(chunk,Buffer.concat(keys.slice(ioff,ioff+n)));m.u32(chunk+0x1518,chunk+n*6);m.write(chunk+0x151c,fps.subarray(foff,foff+nf));m.u32(chunk+0x18a0,chunk+0x151c+nf);m.u32(link,chunk);if(prev)m.u32(prev+4,link);else m.u32(manager+0x40+st*12,link);prev=link;ioff+=n;foff+=nf;}
   last=st;total+=keys.length;
  }
  nativeOutput=null;assert.throws(()=>m.call(0x436420,{ecx:filename,edx:nativeName,args:[0],limit:100000000}),new RegExp(stop));
  const n=c.recorder_save(r,name,1234567,7,2,0,out,1000000);assert.ok(n);assert.deepEqual(Buffer.from(memory(c,out,n)),nativeOutput,`sample${sample} encrypted output`);
  assert.equal(c.replay_open(reader,out,n),1,`sample${sample} reader acceptance`);++checks;
 }report('replay-recorder',{passed:true,checks,scope:'Native 436420 serialization up to first filesystem open, including original LZSS and both encryption passes. Single/multiple stages, 900-entry chunk boundaries and final marker. Optional USER metadata is not compared here.'});
 }finally{for(const p of[out,state,name])c.release(p);c.recorder_delete(r);c.replay_delete(reader);m.close();}
});
