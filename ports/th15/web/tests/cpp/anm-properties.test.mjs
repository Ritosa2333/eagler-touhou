import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';
function ins(op,args=[],mask=0,time=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);args.forEach((a,i)=>typeof a==='object'?b.writeFloatLE(a.f,8+i*4):b.writeInt32LE(a|0,8+i*4));return b;}
test('TH15 animation properties, aliasing and per-frame velocity match original',async()=>{
 const c=await core(),m=await oracle(),vm=c.anm_vm_create(),rng=c.rng_create(),native=m.allocate(0x608),bank=m.allocate(0x13c),table=m.allocate(4),manager=m.allocate(0x187f600),source=m.allocate(4096),p=c.allocate(4096);let checks=0;
 const fields=[[0,0x18,8],[8,0x24,4],[16,0x4ec,12],[28,0x50,12],[40,0x5c,8],[48,0x64,8],[56,0x6c,8],[64,0x74,8],[72,0x84,8],[80,0x3d4,8],[88,0x8c,8],[96,0x538,8],[136,0x7c,8]];
 const ops=[302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,400,401,402,403,404,405,406,415,416,421,422,423,424,425,426,429,431,432,434,436,437,438,603,604,605,606,607,608,611];
 m.u32(0x503c18,manager);m.u32(manager+0x187f4d8,bank);m.u32(bank+0x120,table);m.u32(table,source);m.u32(0x4ca620,0x4e73e8);
 try{for(const op of ops)for(let sample=0;sample<32;sample++){
 const f={f:(sample-13)*.375},g={f:(sample-17)*.625},h={f:(sample-11)*.875};let args;
 if([603,606,607,608].includes(op))args=[f,g];else if([604,605].includes(op))args=[f,sample*71-83];else if(op===611)args=[f,g,sample*101-23];else if([400,401,415].includes(op))args=[f,g,h];else if([402,416,429,434,436].includes(op))args=[f,g];else if([425,426].includes(op))args=[f];else if([404,406].includes(op))args=[sample*59-127,sample*71-83,sample*101-23];else if(op===312)args=[sample,sample>>1];else if(op===421)args=[(sample&3)|((sample>>2&3)<<16)];else if([308,309,316,317,422].includes(op))args=[];else args=[op===302?0:op===304?(sample*5)%44:sample*73-519];
 const reference=sample&1;let mask=0;
 if(reference&&![302,303,304,305,306,307,310,311,315,421,423,424,431,438].includes(op)){
 args=args.map((a,i)=>{mask|=1<<i;return typeof a==='object'?{f:10004+i}:10000+i;});
 }
 const prefix=[ins(100,[10000,sample*59-127],1),ins(100,[10001,sample*71-83],1),ins(100,[10002,sample*101-23],1),ins(101,[{f:10004},f],1),ins(101,[{f:10005},g],1),ins(101,[{f:10006},h],1)];
 const bytes=Buffer.concat([...prefix,ins(op,args,mask),ins(3,[],0,3),ins(-1)]);memory(c,p,bytes.length).set(bytes);const resource=c.anm_fixture_create(p,bytes.length);new DataView(c.memory.buffer,c.anm_vm_visual(vm)+8,4).setInt32(0,0,true);assert.equal(c.anm_vm_bind(vm,resource,0),1);m.view(native,0x608).fill(0);m.write(source,bytes);m.call(0x4773e0,{ecx:bank,args:[native,0]});
 if(sample&2){m.u32(native+0x18,m.u32(native+0x18)|0x400);new DataView(c.memory.buffer,c.anm_vm_visual(vm),4).setUint32(0,6|0x400,true);}
 const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
 for(let frame=0;frame<12;frame++){
 assert.equal(c.anm_vm_tick(vm,rng,rate),0,string(c,c.anm_vm_error(vm)));m.call(0x477e10,{ecx:native,limit:10000000});const vp=c.anm_vm_visual(vm),vars=c.anm_vm_variables(vm);
 for(const [co,no,n]of fields){assert.deepEqual(Buffer.from(memory(c,vp+co,n)),Buffer.from(m.bytes(native+no,n)),JSON.stringify({op,sample,frame,field:no.toString(16)}));checks++;}
 for(const [co,no,n]of [[0,0x4ac,32],[64,0x38,12],[76,0x44,12]]){assert.deepEqual(Buffer.from(memory(c,vars+co,n)),Buffer.from(m.bytes(native+no,n)),JSON.stringify({op,sample,frame,variable:no.toString(16)}));checks++;}
 assert.equal(c.anm_vm_offset(vm),m.i32(native+0x34));assert.deepEqual(Buffer.from(memory(c,c.anm_vm_timer(vm),20)),Buffer.from(m.bytes(native+0x54c,20)),JSON.stringify({op,sample,frame,timer:true}));checks+=2;
 }c.anm_delete(resource);
 }report('anm-properties',{passed:true,checks,coveredOpcodes:ops,frames:ops.length*32*12,originalFunctions:['0x477e10','0x409a30','0x47b410'],scope:'Game-side animation attributes and frame evolution; rasterization and child spawning are not covered.'});
 }finally{c.anm_vm_delete(vm);c.rng_delete(rng);c.release(p);m.close();}
});
