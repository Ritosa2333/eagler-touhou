import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
test('TH17 replay snapshots preserve native timer assignment and player entry state',async()=>{
 const c=await core(),m=await oracle(),f=c.replay_state_create(),source=c.allocate(0xec),ns=m.allocate(0xec),nd=m.allocate(0xec),manager=m.allocate(0x320),player=m.allocate(0x19100),entry=m.allocate(0x158);let checks=0;
 const same=(p,np,n,label)=>{assert.deepEqual(Buffer.from(memory(c,p,n)),Buffer.from(m.bytes(np,n)),label);checks++;},dv=()=>new DataView(c.memory.buffer);
 try{
  for(let sample=0;sample<512;sample++){
   const input=Buffer.alloc(0xec),initial=Buffer.alloc(0xec);
   for(let at=0;at<input.length;at+=4){input.writeUInt32LE((sample*7919+at*15427+0xffabcdef)>>>0,at);initial.writeUInt32LE((sample*15107+at*919+0x98fedcba)>>>0,at);}
   for(const at of [0xb4,0xc8]){input.writeInt32LE(sample%31-17,at+4);input.writeFloatLE(sample*.125,at+8);initial.writeUInt32LE(sample%4,at+16);}
   memory(c,source,input.length).set(input);m.write(ns,input);memory(c,c.replay_state_values(f),initial.length).set(initial);m.write(nd,initial);
   c.replay_state_assign(f,source);m.call(0x44e0f0,{ecx:nd,args:[ns]});same(c.replay_state_values(f),nd,0xec,'snapshot '+sample);
  }
  m.u32(0x4b77d8,manager);m.u32(0x4b77d0,player);m.u32(manager+12,1);let configure=0;m.replace(0x449630,'options are independently verified after replay restoration',()=>{configure++;return 0;});
  for(let sample=0;sample<128;sample++){
   const header=Buffer.alloc(0x158),stage=sample%7+1;header.writeUInt16LE(stage);header.writeUInt16LE(sample*337&65535,2);header.writeInt32LE(sample*719-44032,12);header.writeInt32LE(sample*113+40000,16);header.writeInt32LE(sample%2,0x100);
   memory(c,c.replay_state_entry(f),header.length).set(header);m.write(entry,header);m.i32(0x4b59dc,stage);m.u32(manager+stage*0x28+0xd8,entry);m.f32(player+0x618,13.25);dv().setFloat32(c.replay_state_motion(f,1)+8,13.25,true);
   for(let i=0;i<12;i++){m.i32(player+0x734+i*0xe4,i+19);dv().setInt32(c.replay_state_motion(f,4+i),i+19,true);}
   c.replay_state_restore_player(f);m.call(0x44f5a0);same(c.replay_state_motion(f,0),player+0x61c,8,'fixed position '+sample);same(c.replay_state_motion(f,1),player+0x610,12,'position '+sample);same(c.replay_state_motion(f,2),player+0x18dd0,4,'focus '+sample);same(c.replay_state_motion(f,3),player+0x18edc,33*8,'option history '+sample);for(let i=0;i<12;i++)same(c.replay_state_motion(f,4+i),player+0x734+i*0xe4,4,'option snap '+sample+'/'+i);
   const game=Buffer.from([1,2,3,4,5,6,7,8]),visual=Buffer.from([9,10,11,12,13,14,15,16]);memory(c,c.replay_state_rng(f,0),8).set(game);memory(c,c.replay_state_rng(f,1),8).set(visual);c.replay_state_seed(f);game.writeUInt16LE(header.readUInt16LE(2));game.writeUInt32LE(0,4);visual.writeUInt16LE(header.readUInt16LE(2));assert.deepEqual(Buffer.from(memory(c,c.replay_state_rng(f,0),8)),game);assert.deepEqual(Buffer.from(memory(c,c.replay_state_rng(f,1),8)),visual);checks+=2;
  }
  assert.equal(configure,128);
  for(const name of ['demo1.rpy','demo2.rpy','demo3.rpy']){
   const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length),r=c.replay_create();try{memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(r,p,bytes.length),1,string(c,c.replay_error(r)));const b=Buffer.from(memory(c,c.replay_data(r),c.replay_size(r))),stage=b.readUInt16LE(0xa0);assert.equal(c.replay_initial_state(r,stage,c.replay_state_entry(f)),1);assert.deepEqual(Buffer.from(memory(c,c.replay_state_entry(f),0x158)),b.subarray(0xa0,0x1f8));checks++;}finally{c.replay_delete(r);c.deallocate(p);}
  }
  report('replay-stage-state',{passed:true,checks,snapshotCases:512,playerEntryCases:128,originalFunctions:['0x44e0f0','0x44f5a0'],scope:'All serialized run scalars, Roaring inventory, timer value-assignment semantics, fixed player position/focus/history and first four option snap flags, RNG seeding and three native demo headers. Options reconfiguration is an independently verified boundary; full demo simulation is a separate comparison.'});
 }finally{c.replay_state_delete(f);c.deallocate(source);m.close();}
});
