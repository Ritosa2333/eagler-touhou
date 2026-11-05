import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,root,memory,report} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';
test('Resource ownership and both animation start contracts preserve original states',async()=>{
 const m=await oracle(),c=await core(),f=c.game_resources_fixture(),vm=m.allocate(0x2a4),name=c.allocate(128),heap=m.heap;let checks=0,files=0,textures=0;
 const text=n=>{memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));};
 m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);
 try{for(const filename of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')).sort()){
  const bytes=readFileSync(resolve(root,'reference/assets',filename)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);text(filename);assert.equal(c.game_resources_load(f,name,p,bytes.length),1,filename);const live=c.game_resources_value(f,2),created=c.game_resources_value(f,0);textures+=live;assert.equal(c.game_resources_load(f,name,p,bytes.length),1);assert.equal(c.game_resources_value(f,0),created,'cached resource avoids uploads');
  const part=Array.from({length:5},(_,i)=>c.game_resources_part(f,i));m.heap=heap;const original=installAnm(m,bytes,2);
  for(let script=0;script<original.meta.scripts.length;++script)for(const reset of [0,1]){
   const state=new Uint8Array(0x2a4),sv=new DataView(state.buffer);for(const off of [0,0x208,0x288])for(let axis=0;axis<3;++axis)sv.setFloat32(off+axis*4,(script*7+axis*11)%640-320.25,true);state[0x298]=24;state[0x299]=18;memory(c,part[0],state.length).set(state);m.write(vm,state);memory(c,part[1],8).fill(0);new DataView(c.memory.buffer).setUint16(part[1],0x4321,true);m.u32(0x4ace0c,0x4321);m.u32(0x4ace10,0);
   m.call(reset?0x401560:0x403e00,{ecx:original.file,args:[vm,script]});assert.equal(c.game_resources_start(f,script,reset),1,filename+'/'+script);
   const actual=normalizedAnm(memory(c,part[0],0x2a4),part[2],part[3],part[4]),expected=normalizedAnm(m.bytes(vm,0x2a4),original.file,original.source,original.sprites);assert.deepEqual(actual,expected,filename+'/'+script+'/'+reset);assert.deepEqual(memory(c,part[1],8),m.bytes(0x4ace0c,8));++checks;
  }
  text(filename+'.failed');c.game_resources_fail(f,created+Math.min(1,live-1));assert.equal(c.game_resources_load(f,name,p,bytes.length),0,'injected allocation failure');assert.equal(c.game_resources_value(f,2),live,'failed replacement releases partial upload and keeps previous allocation');assert.equal(c.game_resources_part(f,2),part[2],'stable previous animation');c.game_resources_fail(f,0xffffffff);c.release(p);++files;
 }report('game-resources',{files,textures,checks,scope:'All original ANM files, texture ownership rollback/cache, and both original reset-position/preserve-position start wrappers compared with real ANM execution. Texture pixel decoding and GPU upload remain platform boundaries.'});
 }finally{c.game_resources_delete(f);c.release(name);m.close();}
});
