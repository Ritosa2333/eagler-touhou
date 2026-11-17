// This adapter is specific to the fixed-memory NP21 build pinned by the
// builder. Native CPU, device, allocator, VRAM and SDL state live in HEAPU8.
// File contents are outside that heap: retain metadata + per-write undo pages.
const PAGE=4096,MAX_SNAPSHOTS=13;
const copyBag=object=>Object.fromEntries(Object.entries(object).map(([k,v])=>[k,Array.isArray(v)?v.slice():v]));
function restoreBag(object,bag){
  for(const key of Object.keys(object))if(!Object.hasOwn(bag,key))delete object[key];
  Object.assign(object,bag);
}

export function createNativeSnapshots({module,fs,memfs,tty,host,exports,readBridge,writeBridge}){
  const frames=new Map(),pool=[];
  let active=null,bytes=0;
  const nativeGlobals=['__rollback_g0','__rollback_g1'].map(key=>exports()[key]);
  if(nativeGlobals.some(g=>!(g instanceof WebAssembly.Global)))throw Error('NP21 回滚全局状态接口缺失');
  const memoryBytes=module.HEAPU8.length;
  if(memoryBytes!==328*65536)throw Error('NP21 回滚内存布局变化');

  function journal(buffer,position,length){
    if(!active||!buffer||!length)return;
    let pages=active.writes.get(buffer);
    if(!pages){pages=new Map();active.writes.set(buffer,pages);}
    const end=Math.min(buffer.length,position+length);
    for(let at=Math.floor(position/PAGE)*PAGE;at<end;at+=PAGE){
      if(!pages.has(at))pages.set(at,buffer.slice(at,Math.min(at+PAGE,buffer.length)));
    }
  }
  const write=memfs.stream_ops.write;
  memfs.stream_ops.write=function(stream,buffer,offset,length,position,canOwn){
    journal(stream.node.contents,position,length);
    return write.call(this,stream,buffer,offset,length,position,canOwn);
  };
  // ops_table was initialized during boot and holds the old function reference.
  if(memfs.ops_table)memfs.ops_table.file.stream.write=memfs.stream_ops.write;

  function captureFs(){
    const nodes=new Map();
    function visit(node){
      if(!node||nodes.has(node))return;
      const bag=copyBag(node);
      if(fs.isDir(node.mode)){
        bag.contents={...node.contents};
        for(const child of Object.values(node.contents))visit(child);
      }
      nodes.set(node,bag);
    }
    visit(fs.root);
    const streams=fs.streams.slice(),streamState=new Map(),shared=new Map();
    for(const stream of streams)if(stream){
      visit(stream.node);streamState.set(stream,copyBag(stream));
      if(stream.shared)shared.set(stream.shared,copyBag(stream.shared));
    }
    return {nodes,streams,streamState,shared,nameTable:fs.nameTable.slice(),
      nextInode:fs.nextInode,currentPath:fs.currentPath,
      terminals:Object.values(tty.ttys).filter(Boolean).map(t=>[t,t.input.slice(),t.output.slice()])};
  }
  function restoreFs(saved){
    for(const [node,bag]of saved.nodes)restoreBag(node,bag);
    for(const [stream,bag]of saved.streamState)restoreBag(stream,bag);
    for(const [shared,bag]of saved.shared)restoreBag(shared,bag);
    fs.streams=saved.streams.slice();fs.nameTable=saved.nameTable.slice();
    fs.nextInode=saved.nextInode;fs.currentPath=saved.currentPath;
    for(const [terminal,input,output]of saved.terminals){terminal.input=input.slice();terminal.output=output.slice();}
  }
  function undo(saved){
    for(const [buffer,pages]of saved.writes)for(const [at,data]of pages){
      if(buffer.set)buffer.set(data,at);else for(let i=0;i<data.length;i++)buffer[at+i]=data[i];
    }
  }
  function release(frame){const saved=frames.get(frame);if(saved){pool.push(saved.memory);frames.delete(frame);}}
  return {
    capture(frame){
      if(active||frames.has(frame)||frames.size>=MAX_SNAPSHOTS)throw Error('NP21 回滚快照窗口越界');
      if(module.HEAPU8.length!==memoryBytes)throw Error('回滚期间模拟器内存大小变化');
      const memory=pool.pop()||new Uint8Array(memoryBytes);memory.set(module.HEAPU8);
      const saved={memory,globals:nativeGlobals.map(g=>g.value),files:captureFs(),
        bridge:readBridge(),clock:host.capture(),writes:new Map()};
      frames.set(frame,saved);active=saved;bytes=Math.max(bytes,(frames.size+pool.length)*memoryBytes);
    },
    seal(){active=null;},
    restore(frame){
      active=null;
      const saved=frames.get(frame);if(!saved)throw Error(`回滚快照已过期：${frame}`);
      for(const key of [...frames.keys()].sort((a,b)=>b-a))if(key>=frame)undo(frames.get(key));
      module.HEAPU8.set(saved.memory);saved.globals.forEach((v,i)=>{nativeGlobals[i].value=v;});
      restoreFs(saved.files);writeBridge(saved.bridge);host.restore(saved.clock);
      for(const key of [...frames.keys()])if(key>=frame)release(key);
    },
    confirm(prefix){for(const frame of frames.keys())if(frame<prefix)release(frame);},
    info(){return {snapshotBytes:bytes,snapshots:frames.size};}
  };
}
