import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {installPlayerArt} from '../../ports/th02/web/player-art.js';
import {installProjectileArt} from '../../ports/th04/web/projectile-art.js';

function fixture(){
  const heap=new Uint8Array(10*1024*1024),marks=[];
  let next=0x10000;
  const api={malloc:size=>{const p=next;next+=size+16;return p;},thArtBind(){},thArtState(){return 0;},thLayerSetView(){}};
  return {heap,marks,mod:{HEAPU8:heap,nativeExports:()=>api,thDirtyMark:(p,n)=>marks.push([p,n,heap.slice(p,p+n)])}};
}
function exercise(f,art){
  const v=new DataView(f.heap.buffer),tags=art.tags,coverage=v.getUint32(art.control+20,true),records=v.getUint32(art.control+16,true);
  for(const offset of [0,262143,524287]){f.heap[coverage+offset]=7;f.heap[tags+offset*2]=123;}
  f.heap[records]=91;f.marks.length=0;
  art.clearLayers();
  assert.deepEqual(f.marks.map(([p,n])=>[p,n]),[[tags,1048576],[coverage,524288]]);
  assert.equal(f.heap.subarray(tags,tags+1048576).some(Boolean),false);
  assert.equal(f.heap.subarray(coverage,coverage+524288).some(Boolean),false);
  assert.equal(f.heap[records],91,'records are masked by cleared coverage');
  for(const [p,,before] of f.marks)f.heap.set(before,p);
  for(const offset of [0,262143,524287]){assert.equal(f.heap[coverage+offset],7);assert.equal(f.heap[tags+offset*2],123);}
  art.clearLayers();
  assert.equal(f.heap[coverage],0,'replayed reset clears restored layers');
}

{
  const f=fixture(),signature=new TextEncoder().encode('TH02COOPv0002!!\0'),mailbox=0x900000;
  f.heap.set(signature,mailbox);f.heap[mailbox+22]=1;
  const art=installPlayerArt(f.mod,{entries:{mailbox:0x2000,art_context:0x3000}},new Uint8Array([1]));
  art.bind(mailbox);exercise(f,art);
  // A rollback can restore the control block independently of the JS closure.
  const v=new DataView(f.heap.buffer);v.setUint32(art.control,0,true);v.setUint32(art.control+12,0,true);
  art.bind(mailbox);assert.equal(v.getUint32(art.control+12,true),mailbox+22);
}
{
  const f=fixture(),bytes=new Uint8Array([4,5,6]),fetch=globalThis.fetch;
  globalThis.fetch=async()=>new Response(bytes);
  try{
    const art=await installProjectileArt(f.mod,{mailbox_cs_offset:0x2000,projectileArt:{file:'fixture',bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),context_cs_offset:0x3000}});
    art.bind(0x900000);exercise(f,art);
  }finally{globalThis.fetch=fetch;}
}
console.log('TH02/TH04 layer reset and rollback passed');
