import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,string,report} from './helpers.mjs';
test('TH15 native Pointdevice t15b/v6 header, complete chap container, compression and retry header updates',async()=>{
 const c=await core(),m=await oracle(),f=c.checkpoint_file_create(),other=c.checkpoint_file_create(),p=c.allocate(65536),retry=c.allocate(40),filename=m.allocate(128),popup=m.allocate(0x1100),bomb=m.allocate(64),vtable=m.allocate(64),scratch=m.allocate(1024*1024);let modules=[],writes=[],stamp=0n,checks=0;
 const sp=()=>m.reg('ESP');const put=(at,b)=>{m.write(at,b);return at;};
 m.write(filename,Buffer.from('save2_1.dat\0'));m.u32(0x4e9bd0,popup);m.u32(0x4e9a68,bomb);m.u32(bomb,vtable);
 m.replace(0x490432,'bounded allocation boundary',()=>{const n=m.u32(sp()+4);return n===0x6400000?scratch:m.allocate(n);});
 m.replace(0x490eda,'fixed clock boundary',()=>{const p=m.u32(sp()+4);m.u32(p,Number(stamp&0xffffffffn));m.u32(p+4,Number(stamp>>32n));return Number(stamp&0xffffffffn);});
 for(const [at,id]of [[0x457ec0,1],[0x411920,2],[0x4309a0,3],[0x4128a0,4],[0x412e70,5],[0x413290,6]])m.replace(at,'typed module payload boundary '+id,()=>{const out=m.u32(sp()+4),n=m.u32(sp()+8);m.write(out,modules[id]);m.u32(n,modules[id].length);return out;},2);
 m.u32(vtable+0x20,m.registerImport({dll:'test',name:'bomb module payload boundary',argc:2,handler:()=>{const out=m.u32(sp()+4),n=m.u32(sp()+8);m.write(out,modules[8]);m.u32(n,modules[8].length);return out;}}));
 for(const at of [0x403130,0x4031f0,0x4032b0])m.replace(at,'file open boundary',()=>{m.u32(0x4e0ef0,37);return 0;});
 m.u32(0x4be088,m.registerImport({dll:'test',name:'CloseHandle',argc:1,handler:()=>1}));
 m.u32(0x4be09c,m.registerImport({dll:'test',name:'WriteFile',argc:5,handler:()=>{const b=m.u32(sp()+8),n=m.u32(sp()+12),done=m.u32(sp()+16);writes.push(Buffer.from(m.bytes(b,n)));m.u32(done,n);return 1;}}));
 let supplied;
 m.u32(0x4be08c,m.registerImport({dll:'test',name:'ReadFile',argc:5,handler:()=>{const b=m.u32(sp()+8),n=m.u32(sp()+12),done=m.u32(sp()+16);m.write(b,supplied.subarray(0,n));m.u32(done,Math.min(n,supplied.length));return 1;}}));
 m.replace(0x490f2b,'working directory boundary',()=>0);
 try{
  for(let sample=0;sample<8;sample++){
   modules=Array.from({length:9},(_,id)=>Buffer.alloc(id===0?0x1cc:id===7?0x514:(sample+1)*(id+3)*71));for(let id=0;id<9;id++)for(let n=0;n<modules[id].length;n++)modules[id][n]=(sample*23+id*13+(n%51)*17)&255;
   stamp=0x102030405n+BigInt(sample);const retries=Array.from({length:10},(_,i)=>sample*101+i*77),flags=(sample%6)*4,cid=sample%4,difficulty=sample%5,stage=sample%7+1,chapter=sample*3;
   m.write(0x4e75c8,modules[0]);retries.forEach((v,i)=>{new DataView(c.memory.buffer).setInt32(retry+i*4,v,true);m.i32(0x4e776c+i*4,v);});for(const [at,v]of [[0x4e7404,cid],[0x4e7410,difficulty],[0x4e73f0,stage],[0x4e73f8,chapter],[0x51bbec,flags]])m.i32(at,v);modules[0]=Buffer.from(m.bytes(0x4e75c8,0x1cc));m.u32(popup+0x14,modules[7].readUInt32LE(0));m.write(popup+0xb30,modules[7].subarray(4));
   c.checkpoint_file_header_create(f,Number(stamp&0xffffffffn),Number(stamp>>32n),cid,difficulty,stage,chapter,retry,flags);
   for(let id=0;id<9;id++){memory(c,p,modules[id].length).set(modules[id]);assert.equal(c.checkpoint_file_set_section(f,id,p,modules[id].length),1);}
   assert.equal(c.checkpoint_file_encode(f),1,string(c,c.checkpoint_file_error(f)));const encoded=Buffer.from(memory(c,c.checkpoint_file_output(f),c.checkpoint_file_output_size(f)));
   writes=[];assert.equal(m.call(0x413b00,{args:[filename,0],limit:1000000000}),0);assert.equal(writes.length,2);assert.deepEqual(encoded,Buffer.concat(writes),'native complete file '+sample);checks++;
   memory(c,p,encoded.length).set(encoded);assert.equal(c.checkpoint_file_open(other,p,encoded.length),1,string(c,c.checkpoint_file_error(other)));for(let id=0;id<9;id++){assert.equal(c.checkpoint_file_section_size(other,id),modules[id].length);assert.deepEqual(Buffer.from(memory(c,c.checkpoint_file_section(other,id),modules[id].length)),modules[id]);checks++;}
   supplied=encoded;for(let current=0;current<16;current++)for(const [ch,di]of [[cid,difficulty],[(cid+1)%4,difficulty],[cid,(difficulty+1)%5]]){m.u32(0x51bbec,current*4);const native=m.call(0x413670,{ecx:filename,edx:ch,args:[di]})===0;assert.equal(!!c.checkpoint_file_compatible(other,ch,di,current*4),native);checks++;}
   assert.equal(m.call(0x4137c0,{ecx:filename}),stage);checks++;
   // The retry writer must update metadata without replacing the payload.
   stamp+=13n;m.u32(0x51bbec,flags);m.u32(0x4e7770,0x190000+sample);retries[1]=0x190000+sample;new DataView(c.memory.buffer).setInt32(retry+4,retries[1],true);c.checkpoint_file_header_create(f,Number(stamp&0xffffffffn),Number(stamp>>32n),cid,difficulty,stage,chapter,retry,flags);memory(c,p,encoded.length).set(encoded);assert.equal(c.checkpoint_file_update_header(f,p,encoded.length),1);const updated=Buffer.from(memory(c,p,encoded.length));writes=[];assert.equal(m.call(0x413b00,{args:[filename,1],limit:100000000}),0);assert.equal(writes.length,1);assert.equal(writes[0].length,0x58);assert.deepEqual(updated.subarray(0,0x58),writes[0]);assert.deepEqual(updated.subarray(0x58),encoded.subarray(0x58));checks+=2;
   for(const bad of [encoded.subarray(0,95),Buffer.from(encoded),Buffer.from(encoded),Buffer.from(encoded)]){if(bad.length>=96){const index=checks%3;if(index===0)bad[4]=5;else if(index===1)bad.writeUInt32LE(CheckpointLimit,0x5c);else bad.writeUInt32LE(encoded.length,0x58);}memory(c,p,bad.length).set(bad);assert.equal(c.checkpoint_file_open(other,p,bad.length),0);checks++;}
  }
  report('checkpoint-file',{passed:true,scenarios:8,checks,scope:'Original 413b00 writes and LZSS compression compare every file byte; original 413670 graphics-mode/selection compatibility and 4137c0 stage selection; native retry writer updates exactly 0x58 bytes. Module bodies are isolated supplied payloads at their serialization boundaries. Full module serialization, live Pointdevice file persistence and application restore require separate integration.'});
 }finally{c.checkpoint_file_delete(f);c.checkpoint_file_delete(other);c.release(p);c.release(retry);m.close();}
});
const CheckpointLimit=100*1024*1024+1;
