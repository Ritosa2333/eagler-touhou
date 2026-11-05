import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report,enableNativeSine} from './helpers.mjs';
test('TH15 easing curves match native SSE results',async()=>{
 const c=await core(),m=await oracle(),args=m.allocate(8),wrapper=m.allocate(64);enableNativeSine(m);let checks=0;
 const code=Buffer.from([0xf3,0x0f,0x10,0x0d,0,0,0,0,0xf3,0x0f,0x10,0x15,0,0,0,0,0xe8,0,0,0,0,0x66,0x0f,0x7e,0xc0,0xc3]);code.writeUInt32LE(args,4);code.writeUInt32LE(args+4,12);code.writeInt32LE(0x404500-wrapper-21,17);m.write(wrapper,code);
 try{for(let mode=-1;mode<=33;mode++)for(const duration of [0,1,7,19,90])for(const elapsed of [-10,-1,0,.125,.5,1,3.75,9,17,30,99]){m.f32(args,elapsed);m.f32(args+4,duration);assert.equal(c.interpolation_weight(mode,elapsed,duration)>>>0,m.call(wrapper,{ecx:mode})>>>0,JSON.stringify({mode,elapsed,duration}));checks++;}report('interpolation-curves',{passed:true,checks,originalFunction:'0x404500',coveredModes:32});}finally{m.close();}
});
test('TH15 scalar, vector2 and vector3 interpolation evolves identically to original',async()=>{
 const c=await core(),m=await oracle(),native=m.allocate(128),out=m.allocate(12);enableNativeSine(m);m.u32(0x4ca620,0x4e73e8);const scalarWrapper=m.allocate(32),store=Buffer.from([0xe8,0,0,0,0,0xf3,0x0f,0x11,0x05,0,0,0,0,0xc3]);store.writeInt32LE(0x41f010-scalarWrapper-5,1);store.writeUInt32LE(out,9);m.write(scalarWrapper,store);let checks=0;
 try{for(const dimension of [1,2,3]){const p=c.interpolation_create(dimension),size=dimension*20+28;
 for(let mode=0;mode<=31;mode++)for(let sample=0;sample<24;sample++){
 const data=Buffer.alloc(size),view=new DataView(data.buffer,data.byteOffset,size);for(let k=0;k<dimension*4;k++)view.setFloat32(k*4,(sample-13)*19.125+(k-2)*33.875,true);
 const timer=dimension*20;view.setInt32(timer,-1,true);view.setUint32(timer+16,1,true);view.setInt32(timer+20,[0,-1,1,7,19,90][sample%6],true);view.setInt32(timer+24,mode,true);memory(c,p,size).set(data);m.write(native,data);
 const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
 for(let frame=0;frame<40;frame++){
 const cp=c.interpolation_step(p,dimension,rate);m.call(dimension===1?scalarWrapper:dimension===2?0x47cb30:0x40a540,{ecx:native,args:dimension===1?[]:[out],limit:1000000});
 assert.deepEqual(Buffer.from(memory(c,cp,dimension*4)),Buffer.from(m.bytes(out,dimension*4)),JSON.stringify({dimension,mode,sample,frame,result:true}));
 assert.deepEqual(Buffer.from(memory(c,p,size)),Buffer.from(m.bytes(native,size)),JSON.stringify({dimension,mode,sample,frame,state:true}));checks+=2;
 }
 }c.interpolation_delete(p,dimension);
 }report('interpolation-state',{passed:true,checks,frames:checks/2,originalFunctions:['0x41f010','0x47cb30','0x40a540'],coveredModes:32,scope:'Animation float interpolation; angular interpolation and integer colors are separate checks.'});}finally{m.close();}
});
test('TH15 angular interpolation preserves wrapping and shortest angle differences',async()=>{
 const c=await core(),m=await oracle(),native=m.allocate(48),out=m.allocate(4),p=c.interpolation_create(4);enableNativeSine(m);m.u32(0x4ca620,0x4e73e8);let checks=0;
 try{for(let mode=0;mode<=31;mode++)for(let sample=0;sample<24;sample++){
 const data=Buffer.alloc(48);for(let i=0;i<4;i++)data.writeFloatLE((sample-13)*19.125+(i-2)*33.875,i*4);data.writeInt32LE(-1,20);data.writeUInt32LE(1,36);data.writeInt32LE([0,-1,1,7,19,90][sample%6],40);data.writeInt32LE(mode,44);memory(c,p,48).set(data);m.write(native,data);const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
 for(let frame=0;frame<40;frame++){const result=c.interpolation_step(p,4,rate);m.call(0x47c720,{ecx:native,args:[out],limit:1000000});assert.deepEqual(Buffer.from(memory(c,result,4)),Buffer.from(m.bytes(out,4)),JSON.stringify({mode,sample,frame,result:true}));assert.deepEqual(Buffer.from(memory(c,p,48)),Buffer.from(m.bytes(native,48)),JSON.stringify({mode,sample,frame,state:true}));checks+=2;}
 }report('interpolation-angle',{passed:true,checks,frames:checks/2,originalFunction:'0x47c720',coveredModes:32});}finally{c.interpolation_delete(p,4);m.close();}
});
test('TH15 alpha and RGB interpolation preserves integer rounding at each step',async()=>{
 const c=await core(),m=await oracle(),native=m.allocate(88),out=m.allocate(12);enableNativeSine(m);m.u32(0x4ca620,0x4e73e8);let checks=0;
 try{for(const dimension of [1,3]){const p=c.integer_interpolation_create(dimension),size=dimension*20+28;
 for(let mode=0;mode<=31;mode++)for(let sample=0;sample<24;sample++){
 const data=Buffer.alloc(size);for(let i=0;i<dimension*4;i++)data.writeInt32LE((sample-13)*1907+(i-2)*3387,i*4);const timer=dimension*20;data.writeInt32LE(-1,timer);data.writeUInt32LE(1,timer+16);data.writeInt32LE([0,-1,1,7,19,90][sample%6],timer+20);data.writeInt32LE(mode,timer+24);memory(c,p,size).set(data);m.write(native,data);const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
 for(let frame=0;frame<40;frame++){
 const result=c.integer_interpolation_step(p,dimension,rate),value=m.call(dimension===1?0x47c530:0x47c170,{ecx:native,args:dimension===1?[]:[out],limit:1000000});if(dimension===1)m.u32(out,value);
 assert.deepEqual(Buffer.from(memory(c,result,dimension*4)),Buffer.from(m.bytes(out,dimension*4)),JSON.stringify({dimension,mode,sample,frame,result:true}));assert.deepEqual(Buffer.from(memory(c,p,size)),Buffer.from(m.bytes(native,size)),JSON.stringify({dimension,mode,sample,frame,state:true}));checks+=2;
 }
 }c.integer_interpolation_delete(p,dimension);
 }report('interpolation-integer',{passed:true,checks,frames:checks/2,originalFunctions:['0x47c530','0x47c170'],coveredModes:32});}finally{m.close();}
});
