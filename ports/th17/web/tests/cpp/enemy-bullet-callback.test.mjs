import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report,enableNativeMath} from './helpers.mjs';

test('TH17 named enemy callback redirects original marked bullet ranges with the same RNG',async()=>{
 const c=await core(),m=await oracle();enableNativeMath(m);
 const enemy=c.enemy_variables_create(),bullets=c.bullet_manager_create(),native=m.allocate(0x71b54c),entity=m.allocate(0x4560),shot=c.allocate(896),data=Buffer.alloc(896),nodes=Array.from({length:64},()=>m.allocate(12)),actors=Array.from({length:64},()=>m.allocate(0xe88));
 m.u32(0x4b768c,native);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);
 data.writeInt16LE(64,868);data.writeInt16LE(1,870);data.writeInt32LE(-1,880);data.writeInt32LE(-1,884);
 let cases=0,checks=0,redirected=0;
 try{for(let sample=0;sample<256;sample++){
  c.bullet_manager_reset(bullets);memory(c,shot,896).set(data);c.enemy_shooter_read(enemy,0,shot);if(sample%17)assert.equal(c.bullet_manager_emit(bullets,enemy,0),1);
  const slots=[];for(let slot=c.bullet_manager_list(bullets,1,0)>>>0;slot!==0xffffffff;slot=c.bullet_manager_list(bullets,2,slot)>>>0)slots.push(slot);
  m.u32(native+0xc0,slots.length?nodes[0]:0);m.u32(native+0x71b540,0);m.u32(native+0x71b544,0);
  for(let index=0;index<slots.length;index++){
   const view=c.bullet_manager_bullet(bullets,slots[index]),np=actors[index],transform=Buffer.alloc(44),mark=(index*37+sample*73)%800-100;
   transform.writeFloatLE(.25,8);transform.writeFloatLE(.5,12);transform.writeInt32LE(17,24);transform.writeInt32LE(23,28);transform.writeUInt32LE(0x4000000,32);transform.writeInt32LE(3,36);
   memory(c,shot,44).set(transform);for(let t=0;t<18;t++){c.bullet_transform_read(view,t,shot);m.write(np+0x68c+t*44,transform);}
   new DataView(c.memory.buffer).setInt32(c.bullet_state_field(view,21,0),mark,true);m.i32(np+0x658,mark);new DataView(c.memory.buffer).setInt32(c.bullet_state_field(view,16,0),3,true);m.i32(np+0x66c,3);
   m.u32(nodes[index],np);m.u32(nodes[index]+4,index+1<slots.length?nodes[index+1]:0);
  }
  const rng=Buffer.alloc(8);rng.writeUInt16LE((sample*737+997)&65535);rng.writeUInt32LE(sample*733,4);memory(c,c.enemy_variables_rng(enemy),8).set(rng);m.write(0x4b7668,rng);
  for(let i=0;i<4;i++){
   const integer=[31+sample,sample%10-1,7,11][i],value=[sample*.125+1,sample*.25-33,128+sample*.0625,(sample%37-18)*.125][i],dv=new DataView(c.memory.buffer);
   dv.setInt32(c.enemy_fixture_field(enemy,0,6,i),integer,true);m.i32(entity+0x28c+i*4,integer);dv.setFloat32(c.enemy_fixture_field(enemy,0,7,i),value,true);m.f32(entity+0x29c+i*4,value);
  }
  m.call(0x428f60,{ecx:entity});assert.equal(c.enemy_callback_run(enemy,bullets,1),1);
  assert.deepEqual(Buffer.from(memory(c,c.enemy_variables_rng(enemy),8)),Buffer.from(m.bytes(0x4b7668,8)),'RNG '+sample);
  for(let index=0;index<slots.length;index++){
   const view=c.bullet_manager_bullet(bullets,slots[index]),np=actors[index];
   assert.deepEqual(Buffer.from(memory(c,c.bullet_callback_transform_bytes(view),18*44)),Buffer.from(m.bytes(np+0x68c,18*44)),'transforms '+sample+'/'+index);
   assert.equal(new DataView(c.memory.buffer).getInt32(c.bullet_state_field(view,16,0),true),m.i32(np+0x66c),'index '+sample+'/'+index);if(m.i32(np+0x66c)===8)redirected++;checks+=2;
  }cases++;checks++;
 }
 report('enemy-redirect-bullets',{passed:true,cases,checks,redirected,originalFunctions:['0x428f60'],scope:'Original native callback over marked active bullet ranges, complete transform entries including retained fields, target geometry, traversal/angle order and RNG even for an empty pool. Bullet activation and motion have separate native comparisons.'});
 }finally{c.enemy_variables_delete(enemy);c.bullet_manager_delete(bullets);c.deallocate(shot);m.close();}
});
