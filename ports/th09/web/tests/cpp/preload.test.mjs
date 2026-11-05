import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {core,memory,report,root} from './helpers.mjs';
import {resolve} from 'node:path';
test('Incremental resource preparation preserves active sprites and commits without duplicate uploads',async()=>{
 const c=await core(),f=c.game_resources_fixture(),name=c.allocate(128),data=c.allocate(16000000);let checks=0;
 const text=s=>{memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(s));};
 const bytes=n=>{const b=readFileSync(resolve(root,'reference/assets',n));memory(c,data,b.length).set(b);text(n);return b.length;};
 try{
  let n=bytes('front.anm');assert.equal(c.game_resources_load(f,name,data,n),1);const original=c.game_resources_part(f,2),live=c.game_resources_value(f,2),count=c.game_resources_value(f,0),rng=Array.from(memory(c,c.game_resources_part(f,1),8));
  n=bytes('pl06.anm');c.game_resources_preload(f,name,data,n);for(let steps=0;c.game_resources_warm(f);++steps){assert.ok(steps<100);assert.equal(c.game_resources_part(f,2),original);assert.deepEqual(Array.from(memory(c,c.game_resources_part(f,1),8)),rng);++checks;}const uploads=c.game_resources_value(f,0);assert.ok(uploads>count);assert.equal(c.game_resources_load(f,name,data,n),1);assert.equal(c.game_resources_value(f,0),uploads);assert.notEqual(c.game_resources_part(f,2),original);
  const current=c.game_resources_part(f,2),retained=c.game_resources_value(f,2);n=bytes('world06.anm');c.game_resources_preload(f,name,data,n);assert.equal(c.game_resources_warm(f),1);assert.equal(c.game_resources_warm(f),1);c.game_resources_cancel(f);assert.equal(c.game_resources_part(f,2),current);assert.equal(c.game_resources_value(f,2),retained);
  c.game_resources_fail(f,c.game_resources_value(f,0));c.game_resources_preload(f,name,data,n);while(c.game_resources_warm(f)){}assert.equal(c.game_resources_part(f,2),current);assert.equal(c.game_resources_value(f,2),retained);c.game_resources_fail(f,0xffffffff);assert.equal(c.game_resources_load(f,name,data,n),1,'Foreground fallback after failed prewarm');
  report('resource-preload',{checks,commitWithoutUpload:true,rollback:true,rngPreserved:true});
 }finally{c.game_resources_delete(f);}
});
