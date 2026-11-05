import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
function bits(v){const b=Buffer.alloc(4);b.writeFloatLE(v);return b.readUInt32LE();}
test('TH11 timers, floating RNG and all interpolation modes match original routines',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(128),out=c.allocate(16),rate=c.allocate(4),q=m.allocate(128),nativeOut=m.allocate(16),wrappers=new Map();let checks=0;
 // Small test-only wrapper stores ST(0) before returning to the oracle.
 function fpuCall(address,registers={},args=[]){let stub=wrappers.get(address);if(!stub){stub=m.allocate(32);const code=Buffer.alloc(20,0x90);code[0]=0xe8;code.writeInt32LE(address-stub-5,1);code[5]=0xd9;code[6]=0x1d;code.writeUInt32LE(nativeOut,7);code[11]=0xc3;m.write(stub,code);wrappers.set(address,stub);}for(const [r,v] of Object.entries(registers))m.reg(r,v);m.call(stub,{args});return m.u32(nativeOut);}
 function normalized(b,offset){const a=Buffer.from(b);a.writeUInt32LE(0,offset);return a;}
 try{
  for(const speed of [0,0.125,0.5,0.98999995,0.99,0.9900001,1,1.0099999,1.01,1.5]){
   new DataView(c.memory.buffer).setFloat32(rate,speed,true);m.f32(0x4a7948,speed);
   for(const initial of [-99999,-1,0,1,12345,16777215]){
    memory(c,p,20).fill(0);m.view(q,20).fill(0);c.timer_set(p,initial,rate);m.reg('EAX',q);m.call(0x406100,{args:[initial]});
    for(let frame=0;frame<64;++frame){
     if(frame%3){c.timer_tick(p);m.reg('ESI',q);m.call(0x459270);}
     else {const delta=[-1,0.3,3.75][frame%9/3];c.timer_advance(p,delta);m.reg('ESI',q);m.call(0x459210,{args:[bits(delta)]});}
     assert.deepEqual(normalized(memory(c,p,20),12),normalized(m.bytes(q,20),12),`timer rate=${speed} initial=${initial} frame=${frame}`);++checks;
    }
   }
  }
  for(const mode of [0,1])for(const seed of [0,1,0xffff,0x9630]){
   const state=Buffer.alloc(8);state.writeUInt16LE(seed);memory(c,p,8).set(state);m.write(q,state);
   for(let i=0;i<512;++i){const expected=fpuCall(mode?0x458dd0:0x458da0,{ESI:q});assert.equal(bits(c.rng_float(p,mode)),expected);assert.deepEqual(Buffer.from(memory(c,p,8)),Buffer.from(m.bytes(q,8)));++checks;}
  }
  for(let kind=0;kind<5;++kind)for(let mode=0;mode<=17;++mode)for(const speed of [1,0.375])for(const duration of [1,17,60]){
   const n=[2,3,3,1,1][kind],float=kind===0||kind===1||kind===4,timer=n*16,size=timer+28,b=Buffer.alloc(size);
   for(let j=0;j<n*4;++j){const value=float?Math.fround((j%2?-1:1)*(j+1)*3.14159):((j%2?-1:1)*(j+1)*31);float?b.writeFloatLE(value,j*4):b.writeInt32LE(value,j*4);}
   b.writeInt32LE(-1,timer);b.writeUInt32LE(1,timer+16);b.writeInt32LE(duration,timer+20);b.writeInt32LE(mode,timer+24);
   b.writeUInt32LE(rate,timer+12);memory(c,p,size).set(b);b.writeUInt32LE(0x4a7948,timer+12);m.write(q,b);new DataView(c.memory.buffer).setFloat32(rate,speed,true);m.f32(0x4a7948,speed);
   const limit=Math.ceil(duration/speed);
   for(let frame=0;frame<limit;++frame){
    c.interpolation_sample(p,out,kind,rate);m.reg('EDI',q);m.reg('EBX',nativeOut);m.reg('EAX',q);
    if(kind===4)fpuCall(0x44ebd0,{EDI:q});else {const result=m.call([0x44e910,0x405240,0x44e4b0,0x44e7c0][kind]);if(kind===3)m.u32(nativeOut,result);}
    assert.deepEqual(Buffer.from(memory(c,out,n*4)),Buffer.from(m.bytes(nativeOut,n*4)),`interpolation kind=${kind} mode=${mode} rate=${speed} duration=${duration} frame=${frame}`);
    assert.deepEqual(normalized(memory(c,p,size),timer+12),normalized(m.bytes(q,size),timer+12),`interpolation state kind=${kind} mode=${mode} frame=${frame}`);++checks;
   }
  }
  report('interpolation',{passed:true,checks,modes:18});
 }finally{m.close();c.release(p);c.release(out);c.release(rate);}
});
