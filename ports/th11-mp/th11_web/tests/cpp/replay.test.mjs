import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
test('TH11 original demos decrypt, index stages and reproduce every recorded input',async()=>{
 const c=await core(),m=await oracle(),replay=c.replay_create(),out=c.allocate(36),manager=m.allocate(0x2dc),name=m.allocate(32),heap=m.heap;
 let archive=0,markers=0,frames=0;const files=[];
 m.replace(0x458400,'archive replay bytes',()=>archive,2);
 m.replace(0x42c760,'replay terminal marker',()=>{++markers;return 0;});
 m.u32(0x4a5758,0x20);m.u32(0x4a8e88,1);
 const str=p=>{let n=0;while(memory(c,p+n,1)[0])++n;return Buffer.from(memory(c,p,n)).toString();};
 try{for(let index=0;index<4;++index){
  const raw=readFileSync(resolve(root,`reference/assets/demo${index}.rpy`)),data=c.allocate(raw.length);memory(c,data,raw.length).set(raw);
  assert.equal(c.replay_open(replay,data,raw.length),1,str(c.replay_error(replay)));c.release(data);
  m.heap=heap;m.view(manager,0x2dc).fill(0);archive=m.allocate(raw.length);m.write(archive,raw);m.write(name,Buffer.from(`demo${index}.rpy\0`));
  m.call(0x436b60,{args:[manager,name]});const decoded=m.u32(manager+0x18),length=c.replay_size(replay);
  assert.equal(length,raw.readUInt32LE(0x20));assert.deepEqual(Buffer.from(memory(c,c.replay_data(replay),length)),Buffer.from(m.bytes(decoded,length)));
  const count=m.u32(decoded+0x58);let total=0;
  for(let st=1;st<=7;++st){const cp=c.replay_stage(replay,st),np=m.u32(manager+0xb4+st*0x24);if(!np){assert.equal(cp,0);continue;}
   assert.ok(cp);assert.equal(c.replay_select(replay,st),1);const ticks=m.u32(np+4);m.i32(manager+0x1d4,st);m.u32(manager+0x1cc,0);
   m.u32(manager+0xa8+st*0x24,m.u32(manager+0xa4+st*0x24));m.u32(manager+0xb0+st*0x24,m.u32(manager+0xac+st*0x24));m.u32(manager+0xb8+st*0x24,0);m.u32(0x4c93c0,0);
   for(let frame=0;frame<ticks+4;++frame){c.replay_tick(replay,out,1);m.reg('ESI',manager);m.call(0x436190);const dv=new DataView(c.memory.buffer);
    for(const [off,address] of [[0,0x4c93c0],[4,0x4c93c4],[8,0x4c93cc],[12,0x4c93d0]])assert.equal(dv.getUint32(out+off,true),m.u32(address),`demo${index} stage${st} frame${frame} +${off}`);
    const fpsOffset=frame?1+Math.floor((frame-1)/30):0,fpsSize=m.u32(np+8)-ticks*6;
    if(frame<ticks&&fpsOffset<fpsSize)assert.equal(dv.getUint32(out+16,true),m.bytes(manager+0x1c8,1)[0]);
    assert.equal(memory(c,out+20,1)[0],frame<ticks&&m.u32(0x4c93c0)===65535?1:0);++frames;
   }total+=ticks;
  }files.push({file:`demo${index}.rpy`,stages:count,frames:total});
  const stageHeader=n=>{const p=c.replay_stage(replay,n);return p?c.replay_data(replay)+new DataView(c.memory.buffer).getUint32(p+4,true):0;};
  const metadata=Buffer.from(memory(c,c.replay_data(replay),112)),headers=Array.from({length:7},(_,i)=>{const p=stageHeader(i+1);return p?Buffer.from(memory(c,p,144)):null;});
  c.replay_metadata(replay);assert.equal(c.replay_size(replay),112+count*144,'catalog retains only headers');assert.deepEqual(Buffer.from(memory(c,c.replay_data(replay),112)),metadata);
  for(let i=0;i<7;++i)if(headers[i]){assert.deepEqual(Buffer.from(memory(c,stageHeader(i+1),144)),headers[i]);assert.equal(c.replay_select(replay,i+1),0,'metadata cannot be mistaken for playable inputs');}
 }
 report('replay',{passed:true,files,frames,markers,scope:'Native 436b60 decrypt/decompress/stage indexing and 436190 per-tick held/pressed/released inputs, end markers and bounded FPS display. Full game replay determinism remains separate.'});
 }finally{c.replay_delete(replay);c.release(out);m.close();}
});
