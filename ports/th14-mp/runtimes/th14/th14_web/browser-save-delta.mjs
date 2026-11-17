// Commit only this runtime's changes. IDBFS's full-tree reconcile can otherwise
// delete files that another same-origin runtime imported after our initial load.
export function createTh14SaveDelta({snapshot,openDatabase,storeName='FILE_DATA'}) {
 let baseline=new Map(),loaded=false,queue=Promise.resolve();
 // Historical IDBFS entries may use Int8Array. Compare the underlying bytes,
 // so -128 and128 describe the same stored byte; preserve offset and length.
 const bytes=contents=>{
  if(ArrayBuffer.isView(contents))return new Uint8Array(contents.buffer,contents.byteOffset,contents.byteLength);
  if(contents instanceof ArrayBuffer)return new Uint8Array(contents);
  if(Array.isArray(contents))return Uint8Array.from(contents);
  throw Error('Invalid TH14 save contents');
 };
 const copy=entry=>({...entry,timestamp:new Date(+entry.timestamp),...(entry.contents?{contents:new Uint8Array(bytes(entry.contents))}:{})});
 const equal=(a,b)=>{
  if(!a||!b||a.mode!==b.mode||a.link!==b.link)return false;
  if(!!a.contents!==!!b.contents)return false;
  if(a.contents){const x=bytes(a.contents),y=bytes(b.contents);if(x.length!==y.length)return false;for(let i=0;i<x.length;++i)if(x[i]!==y[i])return false;}
  return true;
 };
 const capture=async()=>{
  const entries=await snapshot(),result=new Map();
  for(const [path,entry] of entries){
   if(!path.startsWith('/savesth14/')||path.includes('/../')||path.includes('/./'))throw Error('Invalid TH14 save path');
   if(!Number.isFinite(+entry.timestamp)||!Number.isInteger(entry.mode))throw Error('Invalid TH14 save entry');
   const old=baseline.get(path);result.set(path,equal(entry,old)?old:copy(entry));
  }
  return result;
 };
 return {
  async loaded(){if(loaded)throw Error('TH14 save baseline already loaded');baseline=await capture();loaded=true;},
  flush(){
   const current=queue.then(async()=>{
    if(!loaded)throw Error('TH14 save baseline not loaded');
    const next=await capture(),writes=[],removes=[];
    for(const [path,entry] of next)if(!equal(entry,baseline.get(path)))writes.push([path,entry]);
    for(const path of baseline.keys())if(!next.has(path))removes.push(path);
    if(!writes.length&&!removes.length){baseline=next;return {writes:0,removes:0};}
    const db=await openDatabase();let committedWrites=0,committedRemoves=0;
    await new Promise((resolve,reject)=>{
     const tx=db.transaction([storeName],'readwrite'),store=tx.objectStore(storeName);let failure;
     const fail=error=>{if(!failure)failure=error;try{tx.abort();}catch{};};
     tx.oncomplete=()=>failure?reject(failure):resolve();
     tx.onabort=()=>reject(failure||tx.error||Error('TH14 save transaction aborted'));
     tx.onerror=event=>{failure||=tx.error||event.target?.error;};
     // Compare the actual remote file before replacing/deleting it. A stale
     // import of the same slot must report a conflict rather than overwrite it.
     for(const [path,entry] of writes){
      const request=store.get(path);
      request.onsuccess=()=>{
       const remote=request.result;
       if(equal(remote,entry))return;
       if(!equal(remote,baseline.get(path))&&(remote||baseline.has(path))){fail(Error('另一个窗口已修改此存档或录像，请重新打开后再试'));return;}
       try{store.put(entry,path);++committedWrites;}catch(error){fail(error);}
      };
      request.onerror=()=>fail(request.error);
     }
     for(const path of removes){
      const request=store.get(path);
      request.onsuccess=()=>{
       const remote=request.result;if(!remote)return;
       if(!equal(remote,baseline.get(path))){fail(Error('另一个窗口已修改此存档或录像，未执行删除'));return;}
       try{store.delete(path);++committedRemoves;}catch(error){fail(error);}
      };
      request.onerror=()=>fail(request.error);
     }
    });
    baseline=next;return {writes:committedWrites,removes:committedRemoves};
   });
   queue=current.catch(()=>{});return current;
  }
 };
}

export function th14IdbfsDeltaAdapter(Module,directory='/savesth14') {
 if(directory!=='/savesth14')throw Error('TH14 save mount required');
 const idb=Module.IDBFS,fs=Module.FS,mount={mountpoint:directory};
 if(!idb?.getLocalSet||!idb?.loadLocalEntry||!idb?.getDB)throw Error('TH14 IDBFS delta support unavailable');
 const invoke=(fn,...args)=>new Promise((resolve,reject)=>fn(...args,(error,value)=>error?reject(error):resolve(value)));
 return createTh14SaveDelta({
  storeName:idb.DB_STORE_NAME,
  openDatabase:()=>invoke(idb.getDB,directory),
  snapshot:async()=>{
   const set=await invoke(idb.getLocalSet,mount),entries=new Map();
   for(const path of Object.keys(set.entries).sort())entries.set(path,await invoke(idb.loadLocalEntry,path));
   return entries;
  }
 });
}
