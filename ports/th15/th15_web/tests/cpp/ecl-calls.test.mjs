import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';
import {bits,ins,call,resource} from './ecl-fixtures.mjs';
test('TH15 nested ECL calls preserve argument conversion, local frames, waits and returns',async()=>{
 const c=await core(),m=await oracle(),ctx=c.ecl_context_create(),native=m.allocate(0x11e8),owner=m.allocate(0x120c),program=m.allocate(0x90),defs=m.allocate(3*8),rate=m.allocate(4),wrapper=m.allocate(32);m.u32(native+0x1018,owner);m.u32(owner+12,native);m.u32(owner+0x11f8,program);m.u32(program+8,3);m.u32(program+0x8c,defs);m.view(native+0x1020,1)[0]=255;
 const wrapperCode=Buffer.from([0xf3,0x0f,0x10,0x0d,0,0,0,0,0xe8,0,0,0,0,0xc3]);wrapperCode.writeUInt32LE(rate,4);wrapperCode.writeInt32LE(0x48ca80-wrapper-13,9);m.write(wrapper,wrapperCode);m.f32(rate,1);let checks=0,frames=0;
 try{for(let sample=0;sample<128;sample++)for(const references of [0,1]){
  const pushed=sample-71,floatValue=(sample-69)*1.375;
  const scripts={alpha:Buffer.concat([ins(40,[16]),ins(42,[0],1),ins(42,[4],1),ins(50),ins(43,[8],1),ins(10,[],0,2)]),beta:Buffer.concat([ins(40,[16]),call('alpha',[['f','i',bits(0)],['i','f',4]],6),ins(44,[bits(8)],1),ins(44,[bits(.25)]),ins(51),ins(45,[bits(12)],1),ins(10,[],0,4)]),main:Buffer.concat([ins(40,[16]),ins(42,[pushed]),ins(43,[0],1),ins(44,[bits(floatValue)]),ins(45,[bits(4)],1),ins(42,[sample+3]),call('beta',[['i','i',references?-1:sample+3],['f','f',bits(4)],['i','f',0]],references?14:12),ins(42,[sample+917]),ins(43,[12],1),ins(10,[],0,7)])};
  const {bytes,names}=resource(scripts),pr=c.program_create(),p=c.allocate(bytes.length),nr=m.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.program_attach(pr,p,bytes.length),0);m.write(nr,bytes);
  const table=36;names.forEach((name,i)=>{const text=m.allocate(name.length+1);m.write(text,Buffer.from(name+'\0'));m.u32(defs+i*8,text);m.u32(defs+i*8+4,nr+bytes.readUInt32LE(table+i*4));});assert.equal(c.ecl_context_program(ctx,pr,2),1);c.ecl_context_bind(ctx,c.program_instruction(pr,2),0,0);
  const seed=Buffer.alloc(4096);for(let i=0;i<1024;i++)seed.writeUInt32LE((0xab987600+i)>>>0,i*4);memory(c,c.ecl_context_stack(ctx),4096).set(seed);m.write(native+12,seed);m.f32(native,0);m.u32(native+4,2);m.u32(native+8,0);m.u32(native+0x100c,0);m.u32(native+0x1010,0);
  for(let frame=0;frame<16;frame++){
   const expected=m.call(wrapper,{ecx:native})|0,actual=c.ecl_context_step(ctx,1),label=JSON.stringify({sample,references,frame});assert.equal(string(c,c.ecl_context_error(ctx)),'',label+' error');assert.equal(actual,expected,label+' return');assert.deepEqual(Buffer.from(memory(c,c.ecl_context_state(ctx),12)),Buffer.from(m.bytes(native,12)),label+' state');assert.equal(c.ecl_context_field(ctx,0),m.i32(native+0x100c),label+' stack cursor');assert.equal(c.ecl_context_field(ctx,1),m.i32(native+0x1010),label+' local frame');assert.deepEqual(Buffer.from(memory(c,c.ecl_context_stack(ctx),4096)),Buffer.from(m.bytes(native+12,4096)),label+' stack');checks+=5;frames++;if(actual===-1)break;
  }assert.equal(new DataView(c.memory.buffer).getInt32(c.ecl_context_state(ctx)+8,true),-1);c.program_delete(pr);c.release(p);
 }report('ecl-calls',{passed:true,checks,frames,originalFunctions:['0x48ca80','0x48c800','0x48f640','0x48f690'],scope:'Nested typed calls, expression arguments, suspended call timing and local return state. This is not a full-game replay test.'});
 }finally{c.ecl_context_delete(ctx);m.close();}
});
