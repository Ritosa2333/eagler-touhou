import test from'node:test';import assert from'node:assert/strict';import{core,oracle,memory,string,report,sha}from'./helpers.mjs';
test('TH17 original score defaults, file bytes and record unlock match native',async()=>{
 const c=await core(),m=await oracle(),manager=m.allocate(0x2d5f8),f=c.score_file_create(),records=c.record_store_create(),blocks=0x4820*10,settings=0x4b0;let writes=[],checks=0;const exports=[];
 m.view(0x509b8e,1)[0]=0;m.replace(0x48172f,'directory selection outside file codec',()=>0);m.replace(0x4020d0,'no existing file',()=>0,1);m.replace(0x402450,'save output file',()=>{m.u32(0x4b3908,777);return 0;});
 const writeAddress=m.u32(0x49a0a0),write=m.importMap.get(writeAddress);m.importMap.set(writeAddress,{...write,argc:5,handler:()=>{const sp=m.reg('ESP'),size=m.u32(sp+12);writes.push(Buffer.from(m.bytes(m.u32(sp+8),size)));m.u32(m.u32(sp+16),size);return 1;}});
 const closeAddress=m.u32(0x49a08c),close=m.importMap.get(closeAddress);m.importMap.set(closeAddress,{...close,argc:1,handler:()=>1});
 const state=()=>Buffer.concat([Buffer.from(memory(c,c.score_file_field(f,0),blocks)),Buffer.from(memory(c,c.score_file_field(f,1),settings))]);
 const nativeReset=(seed,calls)=>{m.u32(0x4b7668,seed);m.u32(0x4b766c,calls);m.call(0x44fad0,{ecx:manager});m.u32(0x4b77dc,manager);};
 try{
  for(const [seed,calls]of[[0,0],[0xabcd1234,5432],[0x80009630,1]]){
   nativeReset(seed,calls);c.score_file_reset(f,seed,calls);assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,blocks+settings)),'Native default blocks');assert.deepEqual(Buffer.from(memory(c,c.score_file_field(f,2),8)),Buffer.from(m.bytes(0x4b7668,8)));checks+=2;
   for(let variant=0;variant<3;variant++){
    if(variant){const data=state();for(let i=16;i<data.length;i++)if((i*17+variant*97)%109===0&&i%0x4820>=16)data[i]=(i*23+variant*59)&255;memory(c,c.score_file_field(f,0),blocks).set(data.subarray(0,blocks));memory(c,c.score_file_field(f,1),settings).set(data.subarray(blocks));m.write(manager+8,data);}
    writes=[];assert.equal(m.call(0x4500b0,{limit:1000000000}),0);assert.equal(writes.length,2);assert.equal(c.score_file_save(f),1,string(c,c.score_file_error(f)));const file=Buffer.from(memory(c,c.score_file_data(f),c.score_file_size(f)));assert.deepEqual(file,Buffer.concat(writes),'Exact native encoded file');assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,blocks+settings)));checks+=2;
    const pointer=c.allocate(file.length),native=m.allocate(file.length);memory(c,pointer,file.length).set(file);m.write(native,file);m.u32(manager,native);m.u32(manager+4,0);assert.equal(m.call(0x44ff30,{ecx:manager,limit:1000000000}),0);assert.equal(c.score_file_open(f,pointer,file.length),1,string(c,c.score_file_error(f)));assert.deepEqual(state(),Buffer.from(m.bytes(manager+8,blocks+settings)));c.deallocate(pointer);checks++;
    exports.push({seed,calls,variant,bytes:file.length,sha256:sha(file)});
   }
  }
  nativeReset(42,5);c.score_file_reset(f,42,5);assert.equal(c.record_store_import(records,c.score_file_field(f,0),c.score_file_field(f,1)),1);
  assert.deepEqual(Buffer.from(memory(c,c.record_store_export(records,0),blocks)),Buffer.from(m.bytes(manager+8,blocks)));assert.deepEqual(Buffer.from(memory(c,c.record_store_export(records,1),settings)),Buffer.from(m.bytes(manager+0x2d148,settings)));checks+=2;
  assert.equal(c.record_store_unlock(records),1);m.call(0x451030);assert.deepEqual(Buffer.from(memory(c,c.record_store_export(records,0),blocks)),Buffer.from(m.bytes(manager+8,blocks)),'Complete native cheat changes');assert.deepEqual(Buffer.from(memory(c,c.record_store_export(records,1),settings)),Buffer.from(m.bytes(manager+0x2d148,settings)));checks+=2;
  report('score-file',{passed:true,checks,exports,scope:'All ten TH17 native character blocks, settings version 2, 494 RNG words, original v1 score container, byte-exact compressed/encrypted writes and reads, typed record roundtrip and all original cheat unlock mutations. File IO captured; browser persistence and menu integration remain separate.'});
 }finally{c.record_store_delete(records);c.score_file_delete(f);m.close();}
});
