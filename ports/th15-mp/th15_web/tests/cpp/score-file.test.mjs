import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,string,report,sha}from './helpers.mjs';
test('TH15 original score defaults, RNG, compressed files, edited records and selective import match native saved-file boundaries',async()=>{
 const c=await core(),m=await oracle(),manager=m.allocate(0x33b54),f=c.score_file_create(),N=0x33b4c,blocks=0xa4a0*5;let writes=[],checks=0;const results=[];
 m.view(0x503d86,1)[0]=0;m.replace(0x490f2b,'directory boundary',()=>0);m.replace(0x402db0,'score load boundary',()=>0,1);m.replace(0x403130,'score creation boundary',()=>{m.u32(0x4e0ef0,777);return 0;});
 const writeAt=m.u32(0x4be09c),entry=m.importMap.get(writeAt);m.importMap.set(writeAt,{...entry,argc:5,handler:()=>{const sp=m.reg('ESP'),n=m.u32(sp+12);assert.equal(m.u32(sp+4),777);writes.push(Buffer.from(m.bytes(m.u32(sp+8),n)));m.u32(m.u32(sp+16),n);return 1;}});const closeAt=m.u32(0x4be088),close=m.importMap.get(closeAt);m.importMap.set(closeAt,{...close,argc:1,handler:()=>1});
 const state=()=>Buffer.concat([Buffer.from(memory(c,c.score_file_field(f,0),blocks)),Buffer.from(memory(c,c.score_file_field(f,1),0x42c))]);
 const nativeReset=(seed,calls)=>{m.u32(0x4e9a48,seed);m.u32(0x4e9a4c,calls);m.call(0x45dbd0,{ecx:manager});m.u32(0x4e9bc8,manager);};
 const pack=data=>{const p=c.allocate(data.length);try{memory(c,p,data.length).set(data);assert.equal(c.score_file_test_pack(f,p,data.length),1);return Buffer.from(memory(c,c.score_file_data(f),c.score_file_size(f)));}finally{c.release(p);}};
 const reopen=(bytes,native=true)=>{const p=c.allocate(bytes.length);try{memory(c,p,bytes.length).set(bytes);assert.equal(c.score_file_open(f,p,bytes.length),1,string(c,c.score_file_error(f)));if(native){const ptr=m.allocate(bytes.length);m.write(ptr,bytes);m.u32(manager,ptr);m.u32(manager+4,0);assert.equal(m.call(0x45ddd0,{ecx:manager,limit:1000000000}),0);assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,N)));checks++;}}finally{c.release(p);}};
 const sum=(p)=>{let s=0;for(let n=8;n<p.length;n++)s=(s+p[n])>>>0;p.writeUInt32LE(s,4);};
 try{
  for(const [seed,calls]of [[0,0],[0xabcd1234,5432],[0xdeadffff,0xfffffff0],[0x80009630,1]]){
   nativeReset(seed,calls);c.score_file_reset(f,seed,calls);assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,N)));assert.deepEqual(Buffer.from(memory(c,c.score_file_field(f,2),8)),Buffer.from(m.bytes(0x4e9a48,8)));checks+=2;
   for(let variant=0;variant<3;variant++){
    if(variant){const data=state();for(let i=16;i<data.length;i++){if((i*17+variant*97)%109!==0||i%0xa4a0<16)continue;data[i]=(i*23+variant*59)&255;}memory(c,c.score_file_field(f,0),blocks).set(data.subarray(0,blocks));memory(c,c.score_file_field(f,1),0x42c).set(data.subarray(blocks));m.write(manager+8,data);}
    writes=[];assert.equal(m.call(0x45df50,{limit:1000000000}),0);assert.equal(writes.length,2);const expected=Buffer.concat(writes);assert.equal(c.score_file_save(f),1,string(c,c.score_file_error(f)));const file=Buffer.from(memory(c,c.score_file_data(f),c.score_file_size(f)));assert.deepEqual(file,expected,'compressed file seed '+seed+' variant '+variant);assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,N)));checks+=2;reopen(file);results.push({seed,calls,variant,bytes:file.length,sha256:sha(file)});
   }
  }
  nativeReset(1234,7);c.score_file_reset(f,1234,7);assert.equal(c.score_file_save(f),1);writes=[];m.call(0x45df50,{limit:1000000000});const saved=state();
  for(let variant=0;variant<8;variant++){
   nativeReset(1234,7);c.score_file_reset(f,1234,7);const payload=Buffer.from(saved);
   if(variant===0){payload[16+6]='A'.charCodeAt(0);sum(payload.subarray(0,0xa4a0));}
   if(variant===1)payload[16+6]^=0xff; // invalid CR checksum: retain default character block
   if(variant===2)payload.writeUInt16LE(2,2); // unsupported record version skipped
   if(variant===3)payload.writeUInt16LE(0x5158,0xa4a0); // unknown record stops later imports
   if(variant===4){payload.writeUInt32LE(3,12);sum(payload.subarray(0,0xa4a0));} // duplicate IDs: later original block wins
   if(variant===5)payload[blocks+12]='Z'.charCodeAt(0); // invalid ST checksum
   if(variant===6){payload[blocks+20]=0xc7;sum(payload.subarray(blocks));}
   if(variant===7)payload.writeUInt16LE(2,blocks+2);
   reopen(pack(payload));checks++;
  }
  for(const scenario of ['badMagic','badVersion','shortHeader','oversize','shortBlock','zeroBlock','badId','truncatedPayload']){
   c.score_file_reset(f,1234,7);let file;
   if(['shortBlock','zeroBlock','badId'].includes(scenario)){let raw=Buffer.from(saved);if(scenario==='shortBlock')raw=raw.subarray(0,11);if(scenario==='zeroBlock')raw.writeUInt32LE(0,8);if(scenario==='badId'){raw.writeUInt32LE(5,12);sum(raw.subarray(0,0xa4a0));}file=pack(raw);}
   else{file=pack(saved);if(scenario==='badMagic')file[0]^=1;if(scenario==='badVersion')file.writeUInt16LE(4,8);if(scenario==='shortHeader')file=file.subarray(0,23);if(scenario==='oversize')file.writeUInt32LE(0xffffffff,20);if(scenario==='truncatedPayload')file=file.subarray(0,file.length-20);}
   const before=state(),p=c.allocate(file.length);try{memory(c,p,file.length).set(file);assert.equal(c.score_file_open(f,p,file.length),0,scenario);assert.deepEqual(state(),before,scenario+' atomic failure');assert.ok(string(c,c.score_file_error(f)));checks+=3;}finally{c.release(p);}
  }
  report('score-file',{passed:true,checks,results,scope:'Complete native default blocks, 494 RNG words, exact compressed/encrypted exports for four RNG starts and three edited states each, native selective import of valid/bad checksums/version/unknown and duplicate blocks. Unsafe/truncated original file cases use bounded atomic validation, not native OOB behavior. Runtime record access and app persistence remain separate.'});
 }finally{c.score_file_delete(f);m.close();}
});
