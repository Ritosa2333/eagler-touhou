import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();};
test('TH11 ECL stack, argument conversion and references match original routines',async()=>{
 const c=await core(),m=await oracle(),cp=c.allocate(0x1024),np=m.allocate(0x1024),ci=c.allocate(160),ni=m.allocate(160),g=c.ecl_globals_create(),ints=c.ecl_globals_data(g,0),floats=c.ecl_globals_data(g,1),ng=m.allocate(512),owner=m.allocate(32),vtable=m.allocate(32),input=m.allocate(4),out=m.allocate(4);
 const put=(p,o,v)=>new DataView(c.memory.buffer).setUint32(p+o,v>>>0,true),get=(p,o)=>new DataView(c.memory.buffer).getUint32(p+o,true);let stackChecks=0,argumentChecks=0,referenceChecks=0;
 const fixture=Buffer.alloc(0x1024);for(let i=0;i<fixture.length;++i)fixture[i]=(i*17+31)&255;
 const reset=(top=0,base=0)=>{memory(c,cp,fixture.length).set(fixture);m.write(np,fixture);put(cp,4,ci);m.u32(np+4,ni);put(cp,0x1008,top);m.u32(np+0x1008,top);put(cp,0x100c,base);m.u32(np+0x100c,base);m.u32(np+0x1014,owner);};
 const compare=label=>assert.deepEqual(Buffer.from(memory(c,cp+8,0x1008)),Buffer.from(m.bytes(np+8,0x1008)),label);
 const imp=(name,fn)=>m.registerImport({dll:'ecl-globals-test',name,argc:1,handler:()=>fn((-m.i32(m.reg('ESP')+4))>>>0&63)});
 m.u32(owner,vtable);m.u32(vtable+4,imp('int',i=>m.u32(ng+i*4)));m.u32(vtable+8,imp('int-ref',i=>ng+i*4));m.u32(vtable+16,imp('float-ref',i=>ng+256+i*4));
 const floatGetter=m.allocate(32),fg=Buffer.from([0x8b,0x44,0x24,4,0xf7,0xd8,0x83,0xe0,63,0xd9,0x04,0x85,0,0,0,0,0xc2,4,0]);fg.writeUInt32LE(ng+256,12);m.write(floatGetter,fg);m.u32(vtable+12,floatGetter);
 for(let i=0;i<64;++i){const x=(i*9876543-23456789)|0,y=Math.fround(i*0.375-13.625);put(ints,i*4,x);m.i32(ng+i*4,x);put(floats,i*4,bits(y));m.f32(ng+256+i*4,y);}
 const wrap=(fn,argument)=>{const p=m.allocate(48),b=Buffer.alloc(48,0x90);let n=0;if(argument){b[n++]=0xff;b[n++]=0x35;b.writeUInt32LE(input,n);n+=4;}b[n++]=0xbb;b.writeUInt32LE(fn,n);n+=4;b[n++]=0xff;b[n++]=0xd3;b[n++]=0xd9;b[n++]=0x1d;b.writeUInt32LE(out,n);n+=4;b[n++]=0xc3;m.write(p,b);return p;};
 const floatArg=wrap(0x45d550,false),resolveFloat=wrap(0x45d670,true);
 try{
  for(const top of [0,4,8,12,64,1024,2048,4080,4084,4088,4092,4096])for(const type of [0,102,105,120])for(const value of [0,0x12345678,0xffffffff,0x80000000]){reset(top,12);m.u32(input,value);m.reg('EAX',np+8);const native=m.call(0x45dc30,{edx:type,args:[input]})|0;assert.equal(c.ecl_stack_push(cp+8,type,value),native);compare(`push ${top}/${type}/${value}`);++stackChecks;}
  for(let top=0;top<=4096;top+=4)for(const count of [0,4,24,128]){reset(top,Math.max(0,top-16));m.reg('EAX',np+8);const native=m.call(0x45dce0,{ecx:count})|0;assert.equal(c.ecl_stack_frame(cp+8,count,0),native);compare(`frame ${top}/${count}`);++stackChecks;if(native===0&&top+count+4<4096){m.reg('EAX',np+8);m.call(0x45dd20);c.ecl_stack_frame(cp+8,0,1);compare('leave frame');++stackChecks;}}
  const run=(kind,index,value,referenced,stackValue=null)=>{reset(stackValue?8:0,64);const instruction=Buffer.alloc(160);instruction.writeUInt16LE(referenced?(1<<(index&31))&65535:0,8);instruction.writeUInt32LE(value>>>0,16+index*4);memory(c,ci,160).set(instruction);m.write(ni,instruction);if(stackValue){put(cp,8,stackValue[0]);m.u32(np+8,stackValue[0]);put(cp,12,stackValue[1]);m.u32(np+12,stackValue[1]);}
   let native;m.resetThreadFPU();m.reg('EAX',np);
   if(kind===0)native=m.call(0x45d4b0,{edx:np,args:[index]});
   else if(kind===1){m.call(floatArg,{ecx:index});native=m.u32(out);}
   else if(kind===2)native=m.call(0x45d600,{ecx:np,args:[value]});
   else if(kind===3){m.u32(input,value);m.call(resolveFloat);native=m.u32(out);}
   else if(kind===4)native=m.call(0x45d710);
   else native=m.call(0x45d740,{edx:np,ecx:index});
   let actual=c.ecl_argument(cp,g,kind,index,value)>>>0;
   if(kind>=4){const normalize=(p,native)=>!p?0:p>=(native?ng:ints)&&p<(native?ng+512:floats+256)?p-(native?ng:ints)+0x100000:p-(native?np:cp);assert.equal(normalize(actual,false),normalize(native,true),`ref ${kind}/${index}/${value}`);++referenceChecks;}
   else{assert.equal(actual,native,`arg ${kind}/${index}/${value}/${referenced}/${stackValue}`);++argumentChecks;}
   compare('argument stack');
  };
  for(let index=0;index<16;++index){
   for(const v of [0,1,0xffffffff,0x80000000,0x7fffffff,0x3fc00000,0x80000001]){run(0,index,v,false);run(1,index,v,false);}
   for(let offset=0;offset<256;offset+=4){run(0,index,offset,true);run(1,index,bits(offset),true);run(2,index,offset,true);run(3,index,bits(offset),true);run(5,index,bits(offset),true);if(index===0)run(4,index,offset,true);}
   for(const id of [-2,-12,-64,-100,-9999,-10000000]){run(0,index,id,true);run(1,index,bits(id),true);run(2,index,id,true);run(3,index,bits(id),true);run(5,index,bits(id),true);if(index===0)run(4,index,id,true);}
   for(const type of [102,105,120])for(const v of type===102?[bits(0),bits(-0),bits(1.75),bits(-234.875),bits(16777216),bits(-16777218)]:[0,1,0xffffffff,0x80000000,0x7fffffff,0x01000001])for(const kind of [0,1,2,3])run(kind,index,kind%2?bits(-1):0xffffffff,true,[type,v]);
   for(const kind of [0,1,2,3])run(kind,index,kind%2?bits(-1):0xffffffff,true);
   run(5,index,bits(12),false);if(index===0)run(4,index,12,false);
  }
  report('ecl-context',{passed:true,stackChecks,argumentChecks,referenceChecks,scope:'original stack operations, numeric argument readers, local/global references and conversions; execution control flow is separate'});
 }finally{c.release(cp);c.release(ci);c.ecl_globals_delete(g);m.close();}
});
