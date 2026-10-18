import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 screen shake matches native timing, overlapping callback order and shared RNG',async()=>{
 const c=await core(),m=await oracle();let callbacks=[],checks=0,starts=0;
 m.replace(0x456b70,'register update',()=>{const node=m.reg('ESI');assert.equal(m.reg('EBX'),14);assert.equal(m.u32(node+8),0x448720);callbacks.unshift(m.u32(node+32));return 0;});
 m.replace(0x456c10,'unexpected shake draw callback',()=>{throw Error('type 1 must not register a draw callback');});
 try{
  for(const speed of [1,.5,.99,1.01,1.25,0])for(const args of [[90,8,0],[3,-19,37],[0,8,0],[-5,1,9],[129,2147483647,-2147483648]])for(const overlap of [false,true]){
   const f=c.shake_create(),r=c.shake_data(f,1),offset=c.shake_data(f,2),rate=c.shake_data(f,0);callbacks=[];
   const v=()=>new DataView(c.memory.buffer);v().setFloat32(rate,speed,true);v().setUint16(r,21937,true);m.f32(0x4a7948,speed);m.write(0x4c2f00,Buffer.from(memory(c,r,8)));m.u32(0x4c2ef4,0);m.write(0x4c379c,Buffer.alloc(8));
   const begin=values=>{c.shake_start(f,...values);const p=m.call(0x4489e0,{args:[1,...values,0x43]});assert.equal(callbacks[0],p);assert.equal(m.u32(p+12),0);assert.equal(m.u32(p+16),1);assert.equal(m.u32(p+40),0);++starts;};
   try{
    begin(args);
    for(let frame=0;frame<140;++frame){
     if(overlap&&[0,3,7].includes(frame))begin([frame+9,frame-7,frame+2]);
     const stopped=frame===137;m.u32(0x4c2ef4,stopped?1:0);c.shake_tick(f,stopped?1:0);
     callbacks=callbacks.filter(p=>m.call(0x448720,{ecx:p})!==7);
     assert.equal(c.shake_count(f),callbacks.length,`active ${speed}/${args}/${frame}`);
     assert.deepEqual(Buffer.from(memory(c,r,8)),Buffer.from(m.bytes(0x4c2f00,8)),`RNG ${speed}/${args}/${frame}`);
     assert.deepEqual(Buffer.from(memory(c,offset,8)),Buffer.from(m.bytes(0x4c379c,8)),`offset ${speed}/${args}/${frame}`);
     callbacks.forEach((p,i)=>assert.deepEqual(Buffer.from(memory(c,c.shake_timer(f,i),12)),Buffer.from(m.bytes(p+48,12)),`timer ${i}`));++checks;
    }
   }finally{c.shake_delete(f);}
  }
  report('screen-shake',{passed:true,checks,starts,scope:'Native type-1 constructor, priority-14/newest-first ordering, 448720 timer and exact float/RNG state, overlapping effects and stopped state. Stage-camera/3D composition requires its own tests.'});
 }finally{m.close();}
});
