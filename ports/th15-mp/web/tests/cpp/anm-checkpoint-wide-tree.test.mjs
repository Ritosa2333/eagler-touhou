import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root} from './helpers.mjs';

test('Pointdevice animation snapshots round-trip wide native child lists beyond 64 nodes',async()=>{
 const c=await core(),manager=c.anm_manager_create(),source=c.anm_checkpoint_create(),imported=c.anm_checkpoint_create(),file=c.anm_tree_file_create();
 const asset=readFileSync(resolve(root,'reference/assets/front.anm')),p=c.allocate(asset.length);let input=0;
 try{
  memory(c,p,asset.length).set(asset);assert.equal(c.anm_manager_load(manager,3,p,asset.length),1);
  const handle=c.anm_manager_spawn(manager,3,241,24,2),parent=c.anm_manager_find(manager,handle),registry=c.anm_manager_registry(manager);
  for(let i=0;i<96;i++){const child=c.anm_manager_spawn(manager,3,241,24,2);assert.equal(c.anm_registry_attach(registry,c.anm_manager_find(manager,child),parent),1);}
  const saved=c.anm_checkpoint_capture(source,manager,handle)>>>0;assert(saved);
  assert.equal(c.anm_tree_file_write(file,source,saved,manager),1,string(c,c.anm_checkpoint_error(source)));
  const bytes=Buffer.from(memory(c,c.anm_tree_file_bytes(file),c.anm_tree_file_size(file)));input=c.allocate(bytes.length);memory(c,input,bytes.length).set(bytes);
  const restored=c.anm_tree_file_read(file,imported,input,bytes.length,manager)>>>0;
  assert(restored,string(c,c.anm_checkpoint_error(imported)));
  assert.equal(c.anm_tree_file_consumed(file),bytes.length);
  assert.equal(c.anm_checkpoint_count(imported),c.anm_checkpoint_count(source));
  assert.equal(c.anm_tree_file_write(file,imported,restored,manager),1,string(c,c.anm_checkpoint_error(imported)));
  const count=c.anm_checkpoint_count(imported);
  assert.equal(c.anm_tree_file_read(file,imported,input,bytes.length-1,manager),0,'Truncated records still fail');
  assert.equal(c.anm_checkpoint_count(imported),count,'Failed reads roll back their nodes');
  const live=c.anm_checkpoint_restore(imported,manager,restored)>>>0;assert(live,string(c,c.anm_checkpoint_error(imported)));
  c.anm_checkpoint_clear(source);const again=c.anm_checkpoint_capture(source,manager,live)>>>0;assert(again);
  assert.equal(c.anm_tree_file_write(file,source,again,manager),1,string(c,c.anm_checkpoint_error(source)));
 }finally{if(input)c.release(input);c.release(p);c.anm_tree_file_delete(file);c.anm_checkpoint_delete(source);c.anm_checkpoint_delete(imported);c.anm_manager_delete(manager);}
});
