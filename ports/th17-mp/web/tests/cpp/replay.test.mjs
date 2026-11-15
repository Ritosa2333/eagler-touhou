import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
test('All TH17 original demos decode and play every input frame identically',async()=>{
 const c=await core(),m=await oracle(),manager=m.allocate(0x400),world=m.allocate(0x200),path=m.allocate(256),heap=m.heap;let source,checks=0;const demos=[];
 m.replace(0x48172f,'fixture directory change',()=>0);
 m.replace(0x4446e0,'fixture replay end notification',()=>0);
 m.replace(0x4020d0,'fixture resource read',()=>{const out=m.allocate(source.length);m.write(out,source);if(m.reg('EDX'))m.u32(m.reg('EDX'),source.length);return out;},1);
 m.u32(0x4b59c8,0x40);m.u32(0x4b76b0,world);
 try{
  for(const name of ['demo1.rpy','demo2.rpy','demo3.rpy']){
   source=readFileSync(resolve(root,'reference/assets',name));m.heap=heap;m.view(manager,0x400).fill(0);m.view(0x4b32c4,0x1a0).fill(0);m.write(path,Buffer.from(name+'\0'));
   assert.equal(m.call(0x44eee0,{ecx:manager,args:[path],limit:100000000}),0);
   const replay=c.replay_create(),p=c.allocate(source.length);
   try{
    memory(c,p,source.length).set(source);assert.equal(c.replay_open(replay,p,source.length),1,name+' '+string(c,c.replay_error(replay)));
    const unpacked=source.readUInt32LE(32),nativeData=m.u32(manager+0x208),decoded=Buffer.from(m.bytes(nativeData,unpacked));
    assert.equal(c.replay_size(replay),unpacked);assert.deepEqual(Buffer.from(memory(c,c.replay_data(replay),unpacked)),decoded);
    const number=decoded.readUInt16LE(0xa0),frames=decoded.readUInt32LE(0xa4),slot=manager+number*0x28;
    assert.equal(c.replay_select(replay,number),1);m.u32(manager+0x218,number);m.u32(slot+0xdc,0);m.u32(slot+0xcc,m.u32(slot+0xc8));m.u32(slot+0xd4,m.u32(slot+0xd0));m.u32(manager+0x210,0);
    for(let frame=0;frame<frames+3;frame++){
     const before=c.replay_frame(replay);c.replay_tick(replay,0);assert.equal(c.replay_frame(replay),before);
     m.call(0x44f2f0,{ecx:manager});const got=Buffer.from(memory(c,c.replay_tick(replay,1),8));
     for(const [offset,at] of [[0,0x4b3448],[2,0x4b3454],[4,0x4b3458]])assert.equal(got.readUInt16LE(offset),m.u32(at),name+' input '+offset+' frame '+frame);
     assert.equal(got[6],m.bytes(manager+0x20c,1)[0],name+' fps '+frame);checks+=4;
    }
    demos.push({name,stage:number,frames,character:c.replay_field(replay,1),animal:c.replay_field(replay,2),difficulty:c.replay_field(replay,3)});
   }finally{c.replay_delete(replay);c.deallocate(p);}
  }
  report('replay-input',{passed:true,checks,totalFrames:demos.reduce((s,d)=>s+d.frames,0),demos,scope:'Original decompression and input playback; game simulation comparison remains separate.'});
 }finally{m.close();}
});
