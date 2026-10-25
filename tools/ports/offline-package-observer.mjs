/** Serialized into the browser: inspect the installed store without creating or updating it. */
export function installOfflinePackageStoreObserver(options) {
  const {name,version,installations,generations,objects,timeoutMs=10000}=options;
  if(!name||!Number.isInteger(version)||!installations||!generations||!objects||!(timeoutMs>0))
    throw Error('Invalid offline Package Store observer schema');

  function read(stores,inspect) {
    return new Promise((resolve,reject)=>{
      let db,transaction,settled=false,result;
      const finish=error=>{
        if(settled)return;
        settled=true;clearTimeout(timer);
        if(error){try{transaction?.abort();}catch{}}
        db?.close();
        if(error)reject(error);else resolve(result);
      };
      const timer=setTimeout(()=>finish(Error('Offline Package Store read timed out')),timeoutMs);
      try{
        // Omitting a version opens an existing DB as-is. Abort creation before its upgrade commits.
        const request=globalThis.indexedDB.open(name);
        request.onupgradeneeded=event=>{
          db=event.target.result;
          event.target.transaction.abort();
          finish(Error('Offline Package Store is absent; observation cannot create it'));
        };
        request.onerror=()=>finish(request.error||Error('Offline Package Store open failed'));
        request.onblocked=()=>finish(Error('Offline Package Store open blocked'));
        request.onsuccess=()=>{
          db=request.result;
          if(settled){db.close();return;}
          try{
            if(db.version!==version)throw Error(`Offline Package Store version ${db.version} differs from ${version}`);
            db.onversionchange=()=>{db.close();finish(Error('Offline Package Store version changed during observation'));};
            transaction=db.transaction(stores,'readonly');
            transaction.oncomplete=()=>finish();
            transaction.onabort=transaction.onerror=()=>finish(transaction.error||Error('Offline Package Store read aborted'));
            inspect(transaction,value=>{result=value;});
          }catch(error){finish(error);}
        };
      }catch(error){finish(error);}
    });
  }

  Object.defineProperty(globalThis,'__offlinePackageStoreObserver',{configurable:true,value:Object.freeze({
    readCurrentPackageGeneration(game){
      return read([installations,generations],(transaction,done)=>{
        const request=transaction.objectStore(installations).get(game);
        request.onsuccess=()=>{
          const installation=request.result||null;
          if(!installation?.currentGeneration){done({installation,generation:null});return;}
          const generation=transaction.objectStore(generations).get([game,installation.currentGeneration]);
          generation.onsuccess=()=>done({installation,generation:generation.result||null});
        };
      });
    },
    readPackageGeneration(game,id){
      if(typeof game!=='string'||!game||typeof id!=='string'||!id)throw Error('Generation observation needs game and pinned id');
      return read([generations],(transaction,done)=>{
        const request=transaction.objectStore(generations).get([game,id]);request.onsuccess=()=>done(request.result||null);
      });
    },
    async readPackageObject(id){
      const value=await read([objects],(transaction,done)=>{
        const request=transaction.objectStore(objects).get(id);
        request.onsuccess=()=>done(request.result||null);
      });
      if(value?.blob instanceof Blob)return value;
      if(value?.data instanceof ArrayBuffer)return{...value,blob:new Blob([value.data],{type:value.type||'application/octet-stream'})};
      return null;
    },
  })});
}
