import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,readdirSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
test('TH17 all nine SHT variants preserve original twenty-group shot definitions and callback identities',async()=>{
 const c=await core(),m=await oracle(),player=m.allocate(0x19100),heap=m.heap,resources=[];let shots=0,checks=0;
 try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.sht')).sort()){
  m.heap=heap;const source=readFileSync(resolve(root,'reference/assets',name)),base=m.allocate(source.length),p=c.allocate(source.length),s=c.sht_create();memory(c,p,source.length).set(source);m.write(base,source);m.u32(player+0x1903c,base);
  // Execute the unchanged original loader's relocation block. Asset I/O,
  // player allocation and unrelated constructor behavior are outside this test.
  m.reg('EDI',player);m.reg('EDX',base);m.reg('ESP',m.stack);m.cpu.emu_start(0x446467,0x4464ee,0,1000000);assert.equal(m.reg('EIP'),0x4464ee);
  assert.equal(c.sht_open(s,p,source.length),1,name);assert.deepEqual(Buffer.from(memory(c,c.sht_header(s),0xe0)),Buffer.from(m.bytes(base,0xe0)),name+' header');const counts=[];checks++;
  for(let group=0;group<20;group++){
   let native=m.u32(base+0xe0+group*4),count=0;
   while(m.view(native,1)[0]<128){const got=Buffer.from(memory(c,c.sht_shot(s,group,count),0x58)),original=Buffer.from(m.bytes(native,0x58));assert.deepEqual(got.subarray(0,0x28),original.subarray(0,0x28),name+'/'+group+'/'+count+' values');assert.deepEqual(got.subarray(0x38),original.subarray(0x38),name+'/'+group+'/'+count+' behavior parameters');for(let k=0;k<4;k++){const table=[0x4a0fc0,0x4a0fa0,0x4b77d4,0x4a0f80][k];assert.equal(m.u32(table+got.readUInt32LE(0x28+k*4)*4),original.readUInt32LE(0x28+k*4),name+'/'+group+'/'+count+' callback '+k);}native+=0x58;count++;shots++;checks+=6;}
   assert.equal(c.sht_count(s,group),count,name+' group '+group);counts.push(count);checks++;
  }resources.push({name,groups:counts});c.sht_delete(s);c.deallocate(p);
 }report('sht-resource',{passed:true,completeGame:false,files:resources.length,shots,checks,originalFunction:'0x4463f0',originalInstructionRange:['0x446467','0x4464ee'],scope:'Original SHT v5 resource header, twenty groups and all shot fields; callback addresses are checked against the original tables while the portable game retains typed callback identifiers. This excludes player constructor overrides, execution of shot callbacks, roaring selection and gameplay.',resources});
 }finally{m.close();}
});
