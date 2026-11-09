import test from 'node:test';
import assert from 'node:assert/strict';

for(const game of ['02','03','04'])test(`TH${game} restores heap-backed files after native memory`,async()=>{
  const {createDosboxSnapshots}=await import(`../../ports/th${game}/web/dosbox-snapshots.js`);
  const heap=new Uint8Array(8192),file={mode:1,contents:heap.subarray(4096,4104)},root={mode:2,contents:{file}},marked=[];
  file.contents.fill(7);
  const globals=Object.fromEntries(Array.from({length:4},(_,i)=>['__rollback_g'+i,new WebAssembly.Global({value:'i32',mutable:true},0)]));
  const diff={bytes:0,begin(){},changes:()=>new Uint32Array(),pack:()=>null,restoreBlocks(){file.contents.fill(7);},sync:page=>marked.push(page),release(){}};
  const fs={root,streams:[],nameTable:[],nextInode:1,currentPath:'/',isDir:mode=>mode===2};
  const module={HEAPU8:heap,FS:fs,memoryDiff:diff,nativeExports:()=>globals,captureContinuation:()=>null,restoreContinuation(){},
    thDirtyMark:(at,size)=>marked.push([at,size]),MEMFS:{stream_ops:{write(stream,bytes,offset,length,position){stream.node.contents.set(bytes.subarray(offset,offset+length),position);}},ops_table:{file:{stream:{}}}}};
  const snapshots=createDosboxSnapshots({module,tick:0,keys:new Set(),ci:{keyMatrix:{}}});
  snapshots.capture(0);module.MEMFS.stream_ops.write({node:file},new Uint8Array([1]),0,1,0);
  snapshots.restore(0);
  assert.deepEqual([...file.contents],Array(8).fill(7));
  assert.deepEqual(marked,game==='03'?[1]:[[4096,1]],'heap-backed writes remain synchronized with the checkpoint');
});

for(const game of ['02','04'])test(`TH${game} restores the checkpoint before native and file writes on one heap page`,async()=>{
  const {createDosboxSnapshots}=await import(`../../ports/th${game}/web/dosbox-snapshots.js`);
  const heap=new Uint8Array(8192),file={mode:1,contents:heap.subarray(4096,4104)},root={mode:2,contents:{file}},marked=[];
  file.contents.fill(7);let preimage;
  const globals=Object.fromEntries(Array.from({length:4},(_,i)=>['__rollback_g'+i,new WebAssembly.Global({value:'i32',mutable:true},0)]));
  const diff={bytes:0,begin(){},changes:()=>new Uint32Array([1]),pack(){return preimage;},restoreBlocks(blocks){heap.set(blocks.at(-1),4096);},sync(){},release(){}};
  const fs={root,streams:[],nameTable:[],nextInode:1,currentPath:'/',isDir:mode=>mode===2};
  const module={HEAPU8:heap,FS:fs,memoryDiff:diff,nativeExports:()=>globals,captureContinuation:()=>null,restoreContinuation(){},
    thDirtyMark:(at,size)=>marked.push([at,size]),MEMFS:{stream_ops:{write(stream,bytes,offset,length,position){stream.node.contents.set(bytes.subarray(offset,offset+length),position);}},ops_table:{file:{stream:{}}}}};
  const snapshots=createDosboxSnapshots({module,tick:0,keys:new Set(),ci:{keyMatrix:{}}});
  snapshots.capture(0);preimage=heap.slice(4096,8192);file.contents[1]=9;
  module.MEMFS.stream_ops.write({node:file},new Uint8Array([1]),0,1,0);snapshots.restore(0);
  assert.deepEqual([...file.contents],Array(8).fill(7));
  assert.deepEqual(marked,[[4096,1]],'heap-backed JS file writes mark native pages');
});
