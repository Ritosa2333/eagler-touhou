import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 shipped STD scripts, interrupts, camera waves and fog match original frames',async()=>{
 const c=await core(),m=await oracle(),nativeState=m.allocate(0x3158);let events=[],checks=0,frames=0;
 m.replace(0x45fce4,'stage test allocation',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 m.replace(0x45fd49,'stage test release',()=>0);m.replace(0x402300,'stage test mesh release',()=>0);
 m.replace(0x401fd0,'stage test animation boundary',()=>0);
 m.replace(0x44acd0,'stage script animation event',()=>{events.push(14,(m.reg('EAX')-nativeState-0x180)/0x434,m.reg('EBX'));return 0;});
 m.replace(0x40e530,'stage script deformation event',()=>{events.push(17,m.u32(nativeState+0x3028));return 0;},2);
 const norm=(input,native=false,base=0)=>{const b=Buffer.from(input);for(const offset of [0x1c,0x30,0x44,0x8c,0xd8,0x164])b.writeUInt32LE(0,offset);if(native){b.writeUInt32LE(b.readUInt32LE(0x4c)-base,0x4c);for(let i=0;i<8;++i)b[0x180+i*0x434+0x41c]=b[0x180+i*0x434+0x41d]=0;}return b;};
 try{for(let stage=1;stage<=7;++stage)for(const speed of [1,.375]){
  const data=readFileSync(resolve(root,`reference/assets/stage0${stage}.std`)),raw=c.allocate(data.length),nativeRaw=m.allocate(data.length),f=c.std_create();memory(c,raw,data.length).set(data);m.write(nativeRaw,data);
  assert.equal(c.std_load(f,raw,data.length),1,`stage ${stage} parser`);assert.equal(c.std_count(f,0),data.readUInt16LE(0));assert.equal(c.std_count(f,2),data.readUInt16LE(2));
  const state=c.std_data(f,0),rate=c.std_data(f,1),base=nativeRaw+data.readUInt32LE(8),b=Buffer.alloc(0x3158);
  b.writeFloatLE(-600,0x3048);b.writeFloatLE(300,0x3050);b.writeFloatLE(600,0x3054);b.writeFloatLE(1,0x305c);b.writeFloatLE(Math.fround(Math.PI/6),0x3088);
  memory(c,state,b.length).set(b);m.write(nativeState,b);m.u32(nativeState+0x1c,base);m.u32(nativeState+0x4c,base);m.u32(0x4a8d60,nativeState);m.u32(0x4c3c40,0);
  new DataView(c.memory.buffer).setFloat32(rate,speed,true);m.f32(0x4a7948,speed);c.timer_set(state+0x38,0,rate);m.reg('EAX',nativeState+0x38);m.call(0x406100,{args:[0]});
  const limit=stage>=6?10500:stage===4?7500:stage===3?5000:2500;
  for(let frame=0;frame<Math.ceil(limit/speed);++frame){
   if(stage===3&&[1000,2000,3000].includes(frame)){const id=frame/1000;assert.equal(c.std_interrupt(f,id),1);m.call(0x404f20,{edx:id});}
   events=[];assert.equal(c.std_tick(f),1,`stage ${stage} script frame ${frame}`);m.call(0x404470,{args:[nativeState]});
   assert.equal(firstDifference(norm(memory(c,state,0x3158)),norm(m.bytes(nativeState,0x3158),true,base)),'',`stage=${stage} rate=${speed} frame=${frame}`);
   assert.equal(new DataView(c.memory.buffer).getUint32(c.std_data(f,2),true),m.u32(0x4c3c40));
   assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.std_data(f,3),c.std_count(f,4))),events);checks+=3;++frames;
  }
  c.std_delete(f);c.release(raw);
 }report('stage-script',{passed:true,checks,frames,stages:7,rates:[1,.375],scope:'404470 and 404f20 execute unmodified STD assets and original interpolation; ANM/deformation boundaries recorded separately from their own tested implementations.'});
 }finally{m.close();}
});
