import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';
const bits=value=>{const b=Buffer.alloc(4);b.writeFloatLE(value);return b.readInt32LE();};
function ins(op,args=[],references=0,cleanup=0,time=0,difficulty=255){const b=Buffer.alloc(16+args.length*4);b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(references,8);b[10]=difficulty;b[11]=args.length;b[12]=cleanup;b[13]=0x34;b[14]=0x56;b[15]=0x78;args.forEach((v,i)=>b.writeInt32LE(v,16+i*4));return b;}
async function fixture(){const c=await core(),m=await oracle();enableNativeMath(m);const context=c.ecl_context_create(),native=m.allocate(0x120c),owner=m.allocate(0x120c),program=m.allocate(0x90),table=m.allocate(8),source=m.allocate(1024),p=c.allocate(1024),rate=m.allocate(4),wrapper=m.allocate(32);
 m.u32(native+0x1018,owner);m.u32(owner+0x11f8,program);m.u32(program+0x8c,table);m.u32(table+4,source);
 const code=Buffer.from([0xf3,0x0f,0x10,0x0d,0,0,0,0,0xe8,0,0,0,0,0xc3]);code.writeUInt32LE(rate,4);code.writeInt32LE(0x48ca80-wrapper-13,9);m.write(wrapper,code);
 const rng=c.rng_create();c.ecl_context_rng(context,rng);m.u32(0x4ca620,0x4e73e8);
 let checks=0;
 function run({bytes,stack,sp=1024,base=8,time=0,difficulty=255,elapsed=1,label,frames=1,interpolationRate=1}){
  memory(c,p,bytes.length).set(bytes);m.write(source+16,bytes);assert.equal(c.ecl_context_code(context,p,bytes.length,difficulty,time),1);c.ecl_context_bind(context,p,sp,base);memory(c,c.ecl_context_stack(context),4096).set(stack);
  m.write(native+12,stack);m.f32(native,time);m.u32(native+4,0);m.u32(native+8,0);m.u32(native+0x100c,sp);m.u32(native+0x1010,base);m.view(native+0x1020,4).fill(0);m.view(native+0x1020,1)[0]=difficulty;m.f32(rate,elapsed);
  for(let slot=0;slot<8;slot++){memory(c,c.ecl_context_interpolation(context,slot),48).fill(0);m.view(native+0x1024+slot*48,48).fill(0);}memory(c,rng,8).fill(0);new DataView(c.memory.buffer).setUint16(rng,23456,true);m.view(0x4e9a48,8).fill(0);m.view(0x4e9a48,2).set([0xa0,0x5b]);c.ecl_context_rate(context,interpolationRate);m.f32(0x4e73e8,interpolationRate);
  for(let frame=0;frame<frames;frame++){
   const expected=m.call(wrapper,{ecx:native})|0,actual=c.ecl_context_step(context,elapsed);const at=label+' frame '+frame;assert.equal(string(c,c.ecl_context_error(context)),'',at+' error');assert.equal(actual,expected,at+' return');assert.deepEqual(Buffer.from(memory(c,c.ecl_context_state(context),12)),Buffer.from(m.bytes(native,12)),at+' cursor/time');assert.equal(c.ecl_context_field(context,0),m.i32(native+0x100c),at+' stack cursor');assert.equal(c.ecl_context_field(context,1),m.i32(native+0x1010),at+' local base');assert.deepEqual(Buffer.from(memory(c,c.ecl_context_stack(context),4096)),Buffer.from(m.bytes(native+12,4096)),at+' stack');assert.deepEqual(Buffer.from(memory(c,p,bytes.length)),Buffer.from(m.bytes(source+16,bytes.length)),at+' bytecode metadata');assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(0x4e9a48,8)),at+' random state');for(let slot=0;slot<8;slot++)assert.deepEqual(Buffer.from(memory(c,c.ecl_context_interpolation(context,slot),48)),Buffer.from(m.bytes(native+0x1024+slot*48,48)),at+' interpolation '+slot);checks+=15;
  }
 }
 return {c,m,run,close:()=>{c.rng_delete(rng);c.release(p);c.ecl_context_delete(context);m.close();},checks:()=>checks};
}
function stackFor(sample,types=0){const b=Buffer.alloc(4096);for(let i=0;i<512;i++){b.writeUInt32LE((0xa6543200|(i&1?0x69:0x66))>>>0,i*8);b.writeInt32LE((sample-17)*12345+i*17,i*8+4);}const left=(sample-16)*1.375,right=(sample%9-4)*.625||1.125;for(const [i,v]of [[126,left],[127,right]]){const floating=(types>>(i-126))&1;b[i*8]=floating?0x66:0x69;floating?b.writeFloatLE(v,i*8+4):b.writeInt32LE(Math.trunc(v)||1,i*8+4);}return b;}
test('TH15 ECL common arithmetic, expression stack and vector commands match original executor',async()=>{
 const f=await fixture(),ops=[0,1,22,23,24,30,31,40,41,42,43,44,45,...Array.from({length:41},(_,i)=>50+i),93];let cases=0;
 try{for(const op of ops)for(let sample=0;sample<32;sample++)for(let types=0;types<4;types++){
  let stack=stackFor(sample,types),args=[],refs=0,sp=1024;const a=(sample-16)*.28125,b=(sample%9-4)*.625,cleanup=sample%2?8:0;
  if((op===56||op===58)&&types&2&&Math.trunc(stack.readFloatLE(1020))===0)stack.writeFloatLE(1.25,1020);
  if(op===23)args=[sample-16];if(op===24)args=[bits(a)];if(op===40)args=[16];if(op===41){stack.writeInt32LE(8,sp-4);}
  if(op===42)args=[sample%2?-1:sample-16],refs=sample%2?1:0;
  if(op===44)args=[bits(sample%2?-1:a)],refs=sample%2?1:0;
  if(op===43)args=[0],refs=1;if(op===45)args=[bits(0)],refs=1;
  if(op===78)args=[0],refs=1;
  if([79,80,88].includes(op)){stack.writeFloatLE(Math.abs(a)+.125,1020);stack[1016]=0x66;}
  if(op===81)args=[bits(0),bits(4),bits(a),bits(b)],refs=3;
  if(op===82){args=[bits(0)];refs=1;stack.writeFloatLE(a,8);}
  if(op===85||op===86)args=[bits(0),bits(a),bits(b)],refs=1;
  if(op===87)args=[bits(0),bits(a),bits(b),bits(a+1.25),bits(b-2.125)],refs=1;
  if(op===89)args=[bits(0),bits(a),bits(b)],refs=1;
  if(op===90)args=[bits(0),bits(4),bits(a),bits(b),bits(a+.875)],refs=3;
  if(op===93)args=[bits(0),bits(4),bits(a),bits(b)],refs=3;
  const bytes=Buffer.concat([ins(op,args,refs,cleanup),ins(0,[],0,0,10000)]),label=JSON.stringify({op,sample,types,cleanup});f.run({bytes,stack,sp,elapsed:[.5,1,1.25][sample%3],label});cases++;
 }report('ecl-interpreter',{passed:true,cases,checks:f.checks(),originalFunction:'0x48ca80',scope:'Common expression, frame, arithmetic and vector bytecode. Calls, threads, enemy commands and full-game replay states remain outside this test.'});
 }finally{f.close();}
});
test('TH15 ECL variable interpolation commands match every native easing mode and slot',async()=>{
 const f=await fixture();let frames=0;try{for(const op of [91,92])for(let mode=0;mode<32;mode++)for(let slot=0;slot<8;slot++)for(const interpolationRate of [.5,1,1.5]){
  const args=[slot,bits(slot*4),13,mode,bits(-19.625+slot),bits(37.875-slot)];if(op===92)args.push(bits(4.125),bits(-1.875));const bytes=Buffer.concat([ins(op,args,2),ins(0,[],0,0,10000)]);f.run({bytes,stack:stackFor(11),frames:32,interpolationRate,label:JSON.stringify({op,mode,slot,interpolationRate})});frames+=32;
 }report('ecl-interpolation',{passed:true,frames,checks:f.checks(),originalFunctions:['0x48ca80','0x41f010','0x41f250'],scope:'ECL scalar interpolation initialization, destination references and frame advancement. No enemy world state is claimed.'});}finally{f.close();}
});
test('TH15 ECL jump timing, conditional pops and difficulty filtering match original executor',async()=>{
 const f=await fixture();try{for(const op of [12,13,14])for(const condition of [-17,-1,0,1,17])for(const floating of [0,1])for(const enabled of [0,1]){
  const stack=stackFor(7);stack[1016]=floating?0x66:0x69;floating?stack.writeFloatLE(condition,1020):stack.writeInt32LE(condition,1020);
  const first=ins(op,[48,7],3,8,0,2),skipped=ins(42,[817]),target=ins(42,[319],0,0,7),future=ins(0,[],0,0,10000);f.run({bytes:Buffer.concat([first,skipped,target,future]),stack,difficulty:enabled?2:1,label:JSON.stringify({op,condition,floating,enabled})});
 }report('ecl-control',{passed:true,checks:f.checks(),originalFunction:'0x48ca80',scope:'Relative jumps, direct jump literals, condition stack types, scheduled instructions and difficulty mask.'});}finally{f.close();}
});
