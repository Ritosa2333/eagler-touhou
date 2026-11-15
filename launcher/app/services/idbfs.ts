export interface IdBfsEntry {mode:number;timestamp:Date|number;contents:unknown}
export function idbfsBytes(contents:unknown):Uint8Array|null {
  if(contents instanceof Uint8Array)return contents;
  if(contents instanceof Int8Array)return new Uint8Array(contents.buffer,contents.byteOffset,contents.byteLength);
  return null;
}
/** Missing databases are never created. Only the shipped IDBFS schema is read. */
export async function openIdbfsDatabase(factory:IDBFactory,name:string):Promise<IDBDatabase|null> {
  return new Promise((resolve,reject)=>{
    let missing=false,settled=false;
    const request=factory.open(name);
    request.onupgradeneeded=event=>{missing=event.oldVersion===0;request.transaction!.abort();};
    request.onerror=()=>{if(settled)return;settled=true;if(missing)resolve(null);else reject(request.error);};
    request.onblocked=()=>{settled=true;reject(new Error('本地存储正被占用，请重试'));};
    request.onsuccess=()=>{
      const database=request.result;
      if(settled){database.close();return;}
      settled=true;
      if(database.version!==21||!database.objectStoreNames.contains('FILE_DATA')){database.close();reject(new Error('无法识别本地存储版本'));return;}
      database.onversionchange=()=>database.close();resolve(database);
    };
  });
}
