import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root} from './helpers.mjs';

test('Reimu bomb checkpoint uses saved aura ownership across unrelated live slots and repeated imports',async()=>{
 const c=await core(),manager=c.anm_manager_create(),pool=c.anm_checkpoint_create();
 const bomb=c.bomb_reimu_create(manager,10),snapshot=c.bomb_checkpoint_create(bomb,pool,0);
 const file=c.player_checkpoint_file_create();let input=0;
 const put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v>>>0,true);
 try{
  let bytes;const asset=resolve(root,'reference/assets/front.anm');
  if(existsSync(asset))bytes=readFileSync(asset);
  else{
   const dat=readFileSync(resolve(root,'artifacts/thprac-private-assets/th15.dat')),raw=c.allocate(dat.length),archive=c.archive_create(),buffer=c.allocate(16*1024*1024);
   try{memory(c,raw,dat.length).set(dat);assert.equal(c.archive_open(archive,raw,dat.length),1);
    for(let i=0;i<c.archive_count(archive);i++)if(string(c,c.archive_name(archive,i))==='front.anm'){
     const n=c.archive_read(archive,i,buffer,16*1024*1024);assert.ok(n>0);bytes=Buffer.from(memory(c,buffer,n));break;
    }
    assert.ok(bytes,'original front.anm must be available');
   }finally{c.release(buffer);c.archive_delete(archive);c.release(raw);}
  }
  const p=c.allocate(bytes.length);
  memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,3,p,bytes.length),1);c.release(p);
  for(let i=0;i<512;i++)assert.ok(c.anm_manager_spawn(manager,3,241,24,2));
  const aura=c.anm_manager_spawn(manager,3,221,24,2);
  put(c.bomb_reimu_field(bomb,14),aura);put(c.bomb_reimu_field(bomb,3)+20,1);
  c.bomb_reimu_orb_available(bomb,1);
  assert.equal(c.bomb_checkpoint_capture(snapshot),1,string(c,c.bomb_checkpoint_error(snapshot)));
  const saved=c.bomb_checkpoint_handle(snapshot,1)>>>0;
  assert.notEqual(saved&0x1fff,aura&0x1fff);
  assert.equal(c.anm_checkpoint_find(pool,aura),0,'legacy live-aura lookup cannot resolve the snapshot tree');
  // Live animation changes after capture must not change the saved aura tree.
  put(c.bomb_reimu_field(bomb,14),c.anm_manager_spawn(manager,3,243,24,2));
  assert.equal(c.bomb_checkpoint_file_write(file,snapshot),1,string(c,c.bomb_checkpoint_error(snapshot)));
  const size=c.player_checkpoint_file_size(file),encoded=Buffer.from(memory(c,c.player_checkpoint_file_bytes(file),size));
  input=c.allocate(size);memory(c,input,size).set(encoded);
  for(let round=0;round<3;round++){
   const liveBefore=new DataView(c.memory.buffer).getUint32(c.bomb_reimu_field(bomb,14),true);
   assert.equal(c.bomb_checkpoint_file_read(file,snapshot,input,size),1,string(c,c.bomb_checkpoint_error(snapshot)));
   assert.equal(new DataView(c.memory.buffer).getUint32(c.bomb_reimu_field(bomb,14),true),liveBefore,'import must not overwrite a live registry handle');
   const imported=c.bomb_checkpoint_handle(snapshot,1)>>>0;
   assert.ok(c.anm_checkpoint_find(pool,imported));
   assert.notEqual(imported,saved,'import remaps saved handles');
   assert.equal(c.bomb_checkpoint_file_write(file,snapshot),1,string(c,c.bomb_checkpoint_error(snapshot)));
   assert.equal(c.player_checkpoint_file_size(file),size);
  }
  assert.equal(c.bomb_checkpoint_restore(snapshot),1,string(c,c.bomb_checkpoint_error(snapshot)));
  assert.ok(c.anm_manager_find(manager,new DataView(c.memory.buffer).getUint32(c.bomb_reimu_field(bomb,14),true)));
 }finally{
  if(input)c.release(input);c.player_checkpoint_file_delete(file);c.bomb_checkpoint_delete(snapshot);
  c.bomb_reimu_delete(bomb);c.anm_checkpoint_delete(pool);c.anm_manager_delete(manager);
 }
});
