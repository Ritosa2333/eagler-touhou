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

for(const game of ['02','04'])test(`TH${game} restores filesystem node and stream metadata from independent snapshots`,async()=>{
  const {createDosboxSnapshots}=await import(`../../ports/th${game}/web/dosbox-snapshots.js`);
  const file={mode:1,contents:new Uint8Array([1,2]),name:'original',labels:['a']};
  const root={mode:2,contents:{file}},shared={flags:['read']},stream={node:file,position:2,shared};
  const fs={root,streams:[stream],nameTable:[file],nextInode:3,currentPath:'/',isDir:mode=>mode===2};
  const heap=new Uint8Array(8192),globals=Object.fromEntries(Array.from({length:4},(_,i)=>['__rollback_g'+i,new WebAssembly.Global({value:'i32',mutable:true},0)]));
  const module={HEAPU8:heap,FS:fs,nativeExports:()=>globals,captureContinuation:()=>null,restoreContinuation(){},MEMFS:{stream_ops:{write(s,b,o,n,p){s.node.contents.set(b.subarray(o,o+n),p);}},ops_table:{file:{stream:{}}}}};
  const snapshots=createDosboxSnapshots({module,tick:0,keys:new Set(),ci:{keyMatrix:{}}});snapshots.capture(0);
  file.name='changed';file.labels.push('b');root.contents.extra={mode:1};stream.position=9;shared.flags.push('write');fs.nameTable.push(root);fs.nextInode=8;fs.currentPath='/new';
  module.MEMFS.stream_ops.write(stream,new Uint8Array([8]),0,1,0);snapshots.restore(0);
  assert.equal(file.name,'original');assert.deepEqual(file.labels,['a']);assert.deepEqual([...file.contents],[1,2]);
  assert.deepEqual(Object.keys(root.contents),['file']);assert.equal(stream.position,2);assert.deepEqual(shared.flags,['read']);
  assert.deepEqual(fs.nameTable,[file]);assert.equal(fs.nextInode,3);assert.equal(fs.currentPath,'/');
});
