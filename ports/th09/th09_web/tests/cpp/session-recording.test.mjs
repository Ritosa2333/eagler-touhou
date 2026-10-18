import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {core,memory,report,root} from './helpers.mjs';
import {normalizedAnm} from './anm-oracle.mjs';
test('Fresh C++ runs saved as native replays reproduce their original complete match state',async()=>{
 const c=await core(),decoded=c.replay_create(),f=c.session_create(),name=c.allocate(128),data=c.allocate(16000000),out=c.allocate(16000000);let checks=0,frames=0;const scenarios=[];
 const source=n=>readFileSync(resolve(root,'reference/assets',n));
 for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|sht|ecl|msg|std|bmp|png|jpg)$/.test(n))){const b=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);}
 const fields=(fn,count)=>Array.from({length:count()},(_,i)=>Array.from({length:3},(_,j)=>new DataView(c.memory.buffer).getUint32(fn()+i*12+j*4,true)));
 const stateFields=[[0,fields(c.motion_fields,c.motion_field_count)],[1,fields(c.shot_control_fields,c.shot_control_field_count)],[17,fields(c.match_scene_fields,c.match_scene_field_count)]];
 const error=()=>{const bytes=memory(c,c.session_error(f),256);return new TextDecoder().decode(bytes.subarray(0,bytes.indexOf(0)));};
 const snapshot=()=>{const h=createHash('sha256');h.update(memory(c,c.session_part(f,0,6),2));h.update(memory(c,c.session_part(f,0,20),40));for(let side=0;side<2;++side){for(const [part,list] of stateFields)for(const [,offset,size] of list)h.update(memory(c,c.session_part(f,side,part)+offset,size));h.update(memory(c,c.session_part(f,side,5),60));const slot=5+side,resource=[0,1,2].map(n=>c.session_resource(f,slot,n));h.update(JSON.stringify(normalizedAnm(memory(c,c.session_part(f,side,2),676),...resource)));}return h.digest('hex');};
 try{
  for(const [character,mode,difficulty,seed] of [[0,0,1,0x7351],[6,0,2,0x8291],[13,1,4,0x1234],[4,2,0,0x11a3],[10,2,2,0xf821],[15,2,3,0xcafe]]){
   assert.equal(c.session_begin(f,character,mode,difficulty,seed),1,error());c.session_result(f,out);assert.equal(new TextDecoder().decode(memory(c,out+33,8)),'26/09/20');const states=[];
   for(let frame=0;frame<2400&&c.session_value(f,0)===1;++frame){const keys=(frame%8<4?1:0)|(frame<240?256:0)|(frame%640<160?64:frame%640<480?128:64)|(frame%71<19?4:0)|(frame%401===400?2:0);assert.equal(c.session_step(f,keys,0,keys),1,error());states.push(snapshot());++frames;}
   memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode('TEST'));const size=c.session_save(f,name,out,16000000);assert.ok(size,error());const bytes=Uint8Array.from(memory(c,out,size));memory(c,data,size).set(bytes);assert.equal(c.replay_decode(decoded,data,size),1);assert.equal(new TextDecoder().decode(memory(c,c.replay_data(decoded)+0xc4,8)),'26/09/20');assert.equal(c.session_play(f,data,size,mode===2?9:0),1,error());
   for(let frame=0;frame<states.length;++frame){assert.equal(c.session_step(f,0,0,0),1,error());assert.equal(snapshot(),states[frame],`Replay ${character}/${mode}/${difficulty} frame ${frame}`);++checks;}
   scenarios.push({character,mode,difficulty,frames:states.length,bytes:size});
  }
  report('session-recording',{checks,frames,scenarios,scope:'Unforced recorded gameplay and reloaded native replay state: RNG seed, both scores/players/shot controls/combo, scene state and body animations. These are finite match windows, not full campaign proof.'});
 }finally{c.session_delete(f);c.replay_delete(decoded);}
});
