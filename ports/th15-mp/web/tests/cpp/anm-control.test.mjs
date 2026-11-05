import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';
function ins(op,args=[],mask=0,time=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);args.forEach((a,i)=>typeof a==='object'?b.writeFloatLE(a.f,8+i*4):b.writeInt32LE(a|0,8+i*4));return b;}
test('TH15 animation jumps, waits and interrupts preserve original frame timing',async()=>{
 const c=await core(),m=await oracle(),vm=c.anm_vm_create(),rng=c.rng_create(),native=m.allocate(0x608),bank=m.allocate(0x13c),table=m.allocate(4),manager=m.allocate(0x187f600),source=m.allocate(4096),p=c.allocate(4096);let checks=0,frames=0;
 m.u32(0x503c18,manager);m.u32(manager+0x187f4d8,bank);m.u32(bank+0x120,table);m.u32(table,source);m.u32(0x4ca620,0x4e73e8);
 const run=(bytes,rate,interrupts=new Map())=>{memory(c,p,bytes.length).set(bytes);const resource=c.anm_fixture_create(p,bytes.length);assert.equal(c.anm_vm_bind(vm,resource,0),1);m.view(native,0x608).fill(0);m.write(source,bytes);m.call(0x4773e0,{ecx:bank,args:[native,0]});m.f32(0x4e73e8,rate);
 for(let frame=0;frame<40;frame++){if(interrupts.has(frame)){const id=interrupts.get(frame);c.anm_vm_interrupt(vm,id);m.u32(native+0x49c,id);}const result=c.anm_vm_tick(vm,rng,rate);assert.ok(result>=0,string(c,c.anm_vm_error(vm)));m.call(0x477e10,{ecx:native,limit:10000000});const label=JSON.stringify({rate,frame,interrupts:[...interrupts],bytes:bytes.toString('hex')});assert.equal(c.anm_vm_offset(vm),m.i32(native+0x34),label+' instruction');assert.deepEqual(Buffer.from(memory(c,c.anm_vm_timer(vm),20)),Buffer.from(m.bytes(native+0x54c,20)),label+' timer');assert.deepEqual(Buffer.from(memory(c,c.anm_vm_visual(vm),8)),Buffer.from(m.bytes(native+0x18,8)),label+' flags');assert.deepEqual(Buffer.from(memory(c,c.anm_vm_variables(vm),32)),Buffer.from(m.bytes(native+0x4ac,32)),label+' variables');checks+=4;frames++;}c.anm_delete(resource);};
 try{for(const rate of [.5,1,1.5]){
 for(let op=202;op<=213;op++)for(const a of [-7,0,9,NaN])for(const b of [-7,0,9,NaN]){if(!(op&1)&&(Number.isNaN(a)||Number.isNaN(b)))continue;const command=ins(op,[(op&1)?{f:a}:a,(op&1)?{f:b}:b,56,2]);run(Buffer.concat([command,ins(100,[10000,1],1),ins(200,[72,2]),ins(100,[10000,2],1,2),ins(1,[],0,5),ins(-1)]),rate);}
 run(Buffer.concat([ins(100,[10000,3],1),ins(102,[10001,1],1,1),ins(201,[10000,16,1],1,1),ins(6,[1],0,1),ins(3,[],0,3),ins(-1)]),rate);
 for(const wait of [3,4]){
 const bytes=Buffer.concat([ins(310,[1]),ins(wait,[],0,2),ins(5,[10],0,5),ins(102,[10000,1],1,5),ins(7,[],0,7),ins(5,[-1],0,10),ins(102,[10001,1],1,10),ins(7,[],0,12),ins(-1)]);
 run(bytes,rate,new Map([[6,10],[14,10],[22,99],[30,10]]));
 const noFallback=Buffer.concat([ins(310,[1]),ins(wait,[],0,2),ins(5,[10],0,5),ins(102,[10000,1],1,5),ins(7,[],0,7),ins(-1)]);run(noFallback,rate,new Map([[6,99],[14,10],[22,99],[30,10]]));
 }
 }report('anm-control',{passed:true,checks,frames,coveredOpcodes:[1,2,3,4,5,6,7,200,201,...Array.from({length:12},(_,i)=>202+i)],originalFunction:'0x477e10'});}finally{c.anm_vm_delete(vm);c.rng_delete(rng);c.release(p);m.close();}
});
