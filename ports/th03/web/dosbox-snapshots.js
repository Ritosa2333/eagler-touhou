// Suspend at an Asyncify boundary; capture native memory/globals, the saved
// continuation, and the host filesystem. Audio reaches output on confirmation.
const copyBag=o=>Object.fromEntries(Object.entries(o).map(([k,v])=>[k,Array.isArray(v)?v.slice():v]));
function restoreBag(o,bag){for(const k of Object.keys(o))if(!Object.hasOwn(bag,k))delete o[k];Object.assign(o,bag);}
export function createDosboxSnapshots(emulator){
  const m=emulator.module,fs=m.FS,frames=new Map(),pool=[],diff=m.memoryDiff;
  const globals=Object.entries(m.nativeExports()).filter(([name,value])=>name.startsWith('__rollback_g')&&value instanceof WebAssembly.Global).map(([,value])=>value);
  if(globals.length!==4)throw Error('DOSBox-X 原生全局快照接口不匹配');
  let active=null,tracking=null,total=0;
  function syncMemory(){if(!diff)return;diff.begin();const pages=diff.changes();
    if(tracking){const block=diff.pack(pages);if(block)tracking.undo.push(block);}
    else for(const page of pages)diff.sync(page);
    total=Math.max(total,diff.bytes+[...frames.values()].reduce((n,s)=>n+s.undo.reduce((bytes,block)=>bytes+(block.buffer?.length||0),0),0));}
  function release(saved){if(saved.memory)pool.push(saved.memory);else for(const block of saved.undo)diff.release(block);}
  const write=m.MEMFS.stream_ops.write;
  m.MEMFS.stream_ops.write=function(stream,buffer,offset,length,position,canOwn){
    const contents=stream.node.contents;if(tracking&&contents&&length){let pages=tracking.writes.get(contents);if(!pages){pages=new Map();tracking.writes.set(contents,pages);}for(let at=Math.floor(position/4096)*4096;at<Math.min(contents.length,position+length);at+=4096)if(!pages.has(at))pages.set(at,contents.slice(at,at+4096));}
    return write.call(this,stream,buffer,offset,length,position,canOwn);
  };
  if(m.MEMFS.ops_table)m.MEMFS.ops_table.file.stream.write=m.MEMFS.stream_ops.write;
  function captureFs(){const nodes=new Map(),streams=fs.streams.slice(),streamState=new Map(),shared=new Map();
    function visit(node){if(!node||nodes.has(node))return;const bag=copyBag(node);if(fs.isDir(node.mode)){bag.contents={...node.contents};for(const child of Object.values(node.contents))visit(child);}nodes.set(node,bag);}visit(fs.root);
    for(const stream of streams)if(stream){visit(stream.node);streamState.set(stream,copyBag(stream));if(stream.shared)shared.set(stream.shared,copyBag(stream.shared));}
    return {nodes,streams,streamState,shared,nameTable:fs.nameTable.slice(),nextInode:fs.nextInode,currentPath:fs.currentPath};
  }
  return {capture(frame){if(active||frames.has(frame)||frames.size>=13)throw Error('DOSBox-X 回滚窗口越界');syncMemory();const continuation=m.captureContinuation(),memory=diff?null:(pool.pop()||new Uint8Array(m.HEAPU8.length));if(memory){if(memory.length!==m.HEAPU8.length)throw Error('DOSBox-X 回滚窗口内内存增长');memory.set(m.HEAPU8);}
      const saved={memory,undo:[],globals:globals.map(g=>g.value),continuation,files:captureFs(),writes:new Map(),tick:emulator.tick,keys:[...emulator.keys],keyMatrix:{...emulator.ci.keyMatrix},presentation:m.framePresenter?.capture()};frames.set(frame,saved);active=tracking=saved;if(memory)total=Math.max(total,(frames.size+pool.length)*memory.length);},
    // Native execution stays suspended between steps. The next capture or
    // restore records the preceding frame's writes, avoiding a second full
    // heap scan at every boundary. Filesystem writes remain tracked meanwhile.
    seal(){active=null;},
    restore(frame){active=null;const s=frames.get(frame);if(!s)throw Error('DOSBox-X 快照已过期');
      syncMemory();const restoring=[...frames.keys()].filter(key=>key>=frame).sort((a,b)=>b-a).map(key=>frames.get(key));
      // Newest-first blocks describe the original undo order. The resident
      // backend validates the whole chain and restores each page only once.
      if(diff){const blocks=restoring.flatMap(saved=>saved.undo.slice().reverse());if(diff.restoreBlocks)diff.restoreBlocks(blocks);else for(const block of blocks)diff.restoreBlock(block);}
      for(const saved of restoring)for(const [buffer,pages]of saved.writes)for(const [at,data]of pages)buffer.set(data,at);
      if(s.memory)m.HEAPU8.set(s.memory);globals.forEach((g,i)=>{g.value=s.globals[i];});m.restoreContinuation(s.continuation);emulator.tick=s.tick;emulator.keys=new Set(s.keys);emulator.ci.keyMatrix={...s.keyMatrix};
      // Restore the framebuffer pointer/size alongside the native allocation.
      // The GPU stays unchanged until the room finishes catching up.
      m.framePresenter?.restore(s.presentation);
      for(const [node,bag]of s.files.nodes)restoreBag(node,bag);for(const [stream,bag]of s.files.streamState)restoreBag(stream,bag);for(const [shared,bag]of s.files.shared)restoreBag(shared,bag);fs.streams=s.files.streams.slice();fs.nameTable=s.files.nameTable.slice();fs.nextInode=s.files.nextInode;fs.currentPath=s.files.currentPath;
      for(const key of [...frames.keys()])if(key>=frame){release(frames.get(key));frames.delete(key);}tracking=[...frames.values()].at(-1)||null;},
    // A grouped checkpoint can precede the confirmed prefix by one tick.
    // Retain it until its second tick is confirmed, so an odd-tick correction
    // can reconstruct that boundary without committing effects a second time.
    confirm(retainFrom){for(const key of [...frames.keys()])if(key<retainFrom){release(frames.get(key));frames.delete(key);}tracking=[...frames.values()].at(-1)||null;},
    info(){return {snapshots:frames.size,snapshotBytes:total,core:'DOSBox-X Asyncify',storage:diff?(diff.restoreBlocks?'resident page undo + merged restore':'page undo'):'full copy'};}};
}
