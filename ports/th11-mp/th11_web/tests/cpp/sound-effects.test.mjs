import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';

test('TH11 native sound mapping, bounded queue, pan, stop and mixer volumes',async()=>{
 const m=await oracle(),c=await core(),f=c.sound_fixture(),vtable=m.allocate(128),buffers=Array.from({length:56},()=>m.allocate(4));
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),bits=x=>{const b=Buffer.alloc(4);b.writeFloatLE(x);return b.readUInt32LE();};let events=[],checks=0;
 m.view(0x4c3e80,0x5300).fill(0);m.u32(0x4c3810,0);
 for(let n=0;n<128;++n)m.i32(0x4c4288+n*4,-1);for(let n=0;n<12;++n)m.i32(0x4c44a0+n*4,-1);
 for(let n=0;n<56;++n){m.u32(buffers[n],vtable);m.u32(0x4c4088+n*4,buffers[n]);}
 for(const [offset,op,argc]of [[0x48,0,1],[0x34,1,2],[0x40,2,2],[0x3c,3,2],[0x30,4,4]])m.u32(vtable+offset,m.registerImport({dll:'sound-sink',name:'method'+op,argc,handler:()=>{events.push([op,buffers.indexOf(arg(0))+1,op>0&&op<4?arg(1)|0:0]);return 0;}}));
 function compare(label){const p=c.sound_queue(f);for(const [offset,address,size]of [[0,0x4c4288,512],[512,0x4c44a0,48],[560,0x4c44d0,48],[608,0x4c4500,6144]])assert.deepEqual(memory(c,p+offset,size),m.bytes(address,size),label+' queue '+offset);++checks;}
 try{
  assert.deepEqual(memory(c,c.sound_definitions_data(),56*8),m.bytes(0x4a34f0,56*8));
  for(let n=0;n<46;++n){const b=memory(c,c.sound_sample(n),40);assert.equal(new TextDecoder().decode(b.subarray(0,b.indexOf(0))),m.string(m.u32(0x4a36b0+n*4),40));}
  for(let round=0;round<240;++round){
   for(let n=0;n<180;++n){const id=(round*7+(n<150?n%1:n))%56;
    if(n%3===0){const x=Math.fround((n-70.25)*7.4);c.sound_positioned(f,id,x);m.reg('EDI',id);m.call(0x44a260,{args:[bits(x)]});}
    else{c.sound_enqueue(f,id,0);m.reg('ESI',id);m.call(0x44a1e0);}
    if(n%16===0)compare(round+'/'+n);
   }
   if(round%5===0){c.sound_stop(f,49);m.call(0x44a300);compare('stop');}
   const master=round%101,enabled=round%17!==3,initialized=round%23!==7;
   events=[];m.u32(0x4c4490,initialized?1:0);m.view(0x4c3464,1)[0]=enabled?1:0;m.i32(0x4c916c,master);
   c.sound_process(f,master,initialized,enabled);m.call(0x44a340);compare('process '+round);
   assert.deepEqual(Array.from({length:c.sound_call_count(f)},(_,n)=>Array.from(new Int32Array(c.memory.buffer,c.sound_calls(f)+n*12,3))),events,'sink operations '+round);
  }
  report('sound-effects',{passed:true,checks,requests:43200,logicalEffects:56,samples:46,scope:'Original 44a1e0/44a260 enqueue, 44a300 stop, 44a340 processing; original resource tables, overflow, average pan and all volume settings. Hardware output and decoded PCM are separate checks.'});
 }finally{c.sound_delete(f);m.close();}
});
