// IDBFS keeps cached database handles even after a browser closes them.
// Recover the handle, never the database contents or the in-memory filesystem.
export function createSaveSync(getModule){
 let pending=Promise.resolve();
 const attempt=(module,populate)=>new Promise((resolve,reject)=>
  module.FS.syncfs(populate,error=>error?reject(error):resolve()));
 return populate=>{
  const current=pending.then(async()=>{
   const module=getModule();
   try{await attempt(module,populate);}
   catch(error){
    if(error?.name!=='InvalidStateError')throw error;
    const idbfs=module.IDBFS;
    let reopened=false;
    for(const [name,database] of Object.entries(idbfs?.dbs??{})){
     try{database.transaction([idbfs.DB_STORE_NAME],'readonly');}
     catch(probeError){
      if(probeError?.name!=='InvalidStateError')continue;
      if(idbfs.dbs[name]!==database)continue;
      delete idbfs.dbs[name];
      database.close();
      reopened=true;
     }
    }
    if(!reopened)throw error;
    // Keep the original direction: a flush must never reload over unsaved data.
    await attempt(module,populate);
   }
  });
  pending=current.catch(()=>{});
  return current;
 };
}
