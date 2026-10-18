import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('TH09 sound sample routing, bounded queues, stereo pan, restart and master volume match original',async()=>{
 const m=await oracle(),c=await core(),f=c.sound_fixture(),manager=m.allocate(0x6230),vtable=m.allocate(128),buffers=Array.from({length:54},()=>m.allocate(4)),arg=i=>m.u32(m.reg('ESP')+4+i*4);let events=[],checks=0;
 m.view(manager,0x6230).fill(0);for(let n=0;n<128;++n)m.i32(manager+0x408+n*4,-1);for(let n=0;n<12;++n)m.i32(manager+0x620+n*4,-1);m.u32(manager,1);m.u32(manager+0x610,1);
 for(let n=0;n<54;++n){m.u32(buffers[n],vtable);m.u32(manager+0x208+n*4,buffers[n]);}
 for(const [offset,op,argc] of [[0x48,0,1],[0x34,1,2],[0x40,2,2],[0x3c,3,2],[0x30,4,4]])m.u32(vtable+offset,m.registerImport({dll:'sound-sink',name:'method'+op,argc,handler:()=>{events.push([op,buffers.indexOf(arg(0))+1,op>0&&op<4?arg(1)|0:0]);return 0;}}));
 function compare(label){const p=c.sound_queue(f);for(const [cOffset,nOffset,size] of [[0,0x408,512],[512,0x620,48],[560,0x650,48],[608,0x680,6144]])assert.deepEqual(memory(c,p+cOffset,size),m.bytes(manager+nOffset,size),label+' queue'+cOffset);++checks;}
 try{
  assert.deepEqual(memory(c,c.sound_definitions_data(),54*8),m.bytes(0x4a1f80,54*8));for(let n=0;n<39;++n){const bytes=memory(c,c.sound_sample(n),40);assert.equal(new TextDecoder().decode(bytes.subarray(0,bytes.indexOf(0))),m.string(m.u32(0x4a2130+n*4),40));}
  for(let round=0;round<240;++round){for(let n=0;n<160;++n){const id=(round*7+(n<130?n%6:n))%54;if(n%3===0){const x=Math.fround((n-70.25)*7.4);c.sound_positioned(f,id,x);m.call(0x43e380,{ecx:manager,args:[id,bits(x)]});}else{const pan=(n*317+round*79)%14001-7000;c.sound_enqueue(f,id,pan);m.call(0x43e2f0,{ecx:manager,args:[id,pan]});}if(n%16===0)compare(round+'/'+n);}
   const master=round%101,enabled=round%17!==3,initialized=round%23!==7;events=[];m.u32(manager+0x610,initialized?1:0);m.view(0x4b3537,1)[0]=enabled?1:0;m.i32(0x4e28b4,master);c.sound_process(f,master,initialized,enabled);m.call(0x43f1f0,{ecx:manager});compare('process '+round);assert.deepEqual(Array.from({length:c.sound_call_count(f)},(_,n)=>Array.from(new Int32Array(c.memory.buffer,c.sound_calls(f)+n*12,3))),events,'ordered sink operations '+round);
  }
  report('sound-effects',{checks,requests:38400,logicalEffects:54,samples:39,scope:'All original sample definitions, 12 distinct effects and 128 positions per frame, average pan, queue overflow, enable gates, buffer restart order and master volume. PCM decoding and device mixing are platform boundaries.'});
 }finally{c.sound_delete(f);m.close();}
});
