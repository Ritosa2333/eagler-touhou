import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {graphicsOracle} from './graphics-oracle.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 scene camera initialization and both original matrix callbacks match',async()=>{
 const c=await core(),m=await oracle();graphicsOracle(m);const p=c.allocate(0x118),q=m.allocate(0x118),f=c.comp_create(),device=m.allocate(4),table=m.allocate(0x180);
 m.u32(0x4c3268,0);m.u32(0x4c3288,device);m.u32(device,table);m.u32(table+0xb0,m.registerImport({dll:'camera-test',name:'matrix',argc:3,handler:()=>0}));
 let state=17483,checks=0;const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 try{
  for(const restricted of[0,1]){m.view(0x4c3484,0x348).fill(0);m.u32(0x4c3810,restricted?0x4000:0);m.call(0x42aae0,{ecx:0x4c3280});c.comp_cameras_reset(f,restricted);
   for(let i=0;i<3;++i){assert.equal(firstDifference(Buffer.from(memory(c,c.comp_camera(f,i),0x118)),Buffer.from(m.bytes(0x4c3484+i*0x118,0x118))),'',`initial camera ${i} restricted ${restricted}`);++checks;}
  }
  for(let screen=0;screen<2;++screen)for(let sample=0;sample<1024;++sample){
   const b=Buffer.alloc(0x118);for(let i=0;i<0x4c;i+=4)b.writeFloatLE((random()-.5)*500,i);b.writeFloatLE(.1+random()*2,0x48);
   b.writeUInt32LE(sample%32,0xcc);b.writeUInt32LE(sample%16,0xd0);b.writeUInt32LE(1+Math.floor(random()*2000),0xd4);b.writeUInt32LE(1+Math.floor(random()*2000),0xd8);b.writeFloatLE(1,0xe0);
   memory(c,p,0x118).set(b);m.write(q,b);m.reg('EDI',q);m.call(screen?0x42a810:0x42a970);c.scene_camera_update(p,screen);
   assert.equal(firstDifference(Buffer.from(memory(c,p,0x118)),Buffer.from(m.bytes(q,0x118))),'',`screen=${screen} sample=${sample}`);++checks;
  }
  report('scene-camera',{passed:true,checks,scope:'Native 42aae0 camera initialization in both viewport configurations, 42a810 screen and 42a970 perspective callbacks with actual original D3DX matrix math; complete 0x118-byte camera state comparison.'});
 }finally{c.release(p);c.comp_delete(f);m.close();}
});
