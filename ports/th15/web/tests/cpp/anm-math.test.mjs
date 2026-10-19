import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';
function ins(op,args=[],mask=0,time=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);args.forEach((a,i)=>typeof a==='object'?b.writeFloatLE(a.f,8+i*4):b.writeInt32LE(a|0,8+i*4));return b;}
test('TH15 animation trig and random polar instructions match original numeric results',async()=>{
 const c=await core(),m=await oracle(),vm=c.anm_vm_create(),rng=c.rng_create(),native=m.allocate(0x608),bank=m.allocate(0x13c),table=m.allocate(4),manager=m.allocate(0x187f600),source=m.allocate(4096),p=c.allocate(4096);enableNativeMath(m);let checks=0;
 m.u32(0x503c18,manager);m.u32(manager+0x187f4d8,bank);m.u32(bank+0x120,table);m.u32(table,source);m.u32(0x4ca620,0x4e73e8);m.f32(0x4e73e8,1);
 try{for(const op of [124,125,126,127,128,129,130,131])for(let sample=0;sample<1024;sample++){
 const input=op===127?(sample-511)/512:(sample-513)*.0625,args=op===129?[{f:10004}]:op>=130?[{f:10004},{f:10005},{f:input},{f:(sample-413)*2.375}]:[{f:10004},{f:input}],mask=op>=130?3:1;
 const bytes=Buffer.concat([ins(101,[{f:10004},{f:input}],1),ins(op,args,mask),ins(3,[],0,10),ins(-1)]);memory(c,p,bytes.length).set(bytes);const resource=c.anm_fixture_create(p,bytes.length);assert.equal(c.anm_vm_bind(vm,resource,0),1);m.view(native,0x608).fill(0);m.write(source,bytes);m.call(0x4773e0,{ecx:bank,args:[native,0]});const seed=(sample*293+3)&65535;m.u32(0x4e9a40,seed);m.u32(0x4e9a44,0);new DataView(c.memory.buffer,rng,8).setUint32(0,seed,true);new DataView(c.memory.buffer,rng,8).setUint32(4,0,true);
 assert.equal(c.anm_vm_tick(vm,rng,1),0,string(c,c.anm_vm_error(vm)));m.call(0x477e10,{ecx:native,limit:10000000});assert.deepEqual(Buffer.from(memory(c,c.anm_vm_variables(vm),32)),Buffer.from(m.bytes(native+0x4ac,32)),JSON.stringify({op,sample,variables:true}));assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(0x4e9a40,8)),JSON.stringify({op,sample,rng:true}));checks+=2;c.anm_delete(resource);
 }report('anm-math',{passed:true,checks,samples:8192,coveredOpcodes:[124,125,126,127,128,129,130,131],originalFunction:'0x477e10',nativeMath:'Original CRT instructions with equivalent word extraction/insertion compatibility shims; no math function replacement.'});
 }finally{c.anm_vm_delete(vm);c.rng_delete(rng);c.release(p);m.close();}
});
