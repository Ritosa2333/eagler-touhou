// Browser-owned pristine bytes: usable in room pages, iframes and workers,
// including LAN HTTP where Cache Storage / SubtleCrypto may be unavailable.
import {sha256} from './sha256.js';
const pending=new Map(),memory=new Map();let memoryBytes=0,database;
const LIMIT=24*1024*1024;
function openCache(){
  return database??=new Promise(resolve=>{
    if(!globalThis.indexedDB){resolve(null);return;}
    try{
      const request=indexedDB.open('touhou-room-assets-v1',1);
      request.onupgradeneeded=()=>request.result.createObjectStore('assets',{keyPath:'key'}).createIndex('identity','identity');
      request.onerror=request.onblocked=()=>resolve(null);
      request.onsuccess=()=>{const db=request.result;db.onversionchange=()=>{db.close();database=null;};resolve(db);};
    }catch{resolve(null);}
  });
}
async function stored(key,record){
  try{
    const db=await openCache();if(!db)return null;
    return await new Promise(resolve=>{
      const tx=db.transaction('assets',record?'readwrite':'readonly'),store=tx.objectStore('assets');
      const request=record?store.put(record):store.get(key);
      let value=null;request.onsuccess=()=>{value=record?null:request.result?.bytes??null;};
      tx.oncomplete=()=>resolve(value);tx.onerror=tx.onabort=()=>resolve(null);
      if(record){
        const cursor=store.index('identity').openKeyCursor(IDBKeyRange.only(record.identity));
        cursor.onsuccess=()=>{const item=cursor.result;if(!item)return;if(item.primaryKey!==key)store.delete(item.primaryKey);item.continue();};
      }
    });
  }catch{return null;}
}
async function valid(bytes,hash,size){return (size===undefined||bytes.byteLength===size)&&(!hash||await sha256(bytes)===hash);}
export async function assetBytes(url,{hash,size,gzip=false,signal}={}){
  const address=new URL(url,import.meta.url).href,identity=address+'|'+gzip,key=identity+'|'+(hash||'')+'|'+(size??'');
  if(signal?.aborted)throw signal.reason;
  let task=pending.get(key);
  if(!task){
    task=(async()=>{
      const saved=memory.get(key);if(saved)return saved;
      let bytes=await stored(key);
      if(!(bytes instanceof Uint8Array)||!await valid(bytes,hash,size)){
        const response=await fetch(address,{cache:'no-cache',signal:AbortSignal.timeout(120000)});
        if(!response.ok)throw Error('资源下载失败：'+new URL(address).pathname+' ('+response.status+')');
        const body=gzip?new Response(response.body.pipeThrough(new DecompressionStream('gzip'))):response;
        bytes=new Uint8Array(await body.arrayBuffer());
        if(!await valid(bytes,hash,size))throw Error('资源校验失败：'+new URL(address).pathname);
        await stored(key,{key,identity,bytes});
      }
      if(bytes.byteLength<=LIMIT){
        while(memoryBytes+bytes.byteLength>LIMIT&&memory.size){const [old,value]=memory.entries().next().value;memory.delete(old);memoryBytes-=value.byteLength;}
        memory.set(key,bytes);memoryBytes+=bytes.byteLength;
      }
      return bytes;
    })();
    pending.set(key,task);task.then(()=>pending.delete(key),()=>pending.delete(key));
  }
  const bytes=await task;if(signal?.aborted)throw signal.reason;
  // Transferring or patching a caller's disk cannot contaminate another game.
  return bytes.slice();
}
export async function freshJSON(url){
  const response=await fetch(new URL(url,import.meta.url),{cache:'no-store',signal:AbortSignal.timeout(30000)});
  if(!response.ok)throw Error('资源清单读取失败：'+url);return response.json();
}
export async function parallelAssets(items,load,concurrency=4){
  let next=0;await Promise.all(Array.from({length:Math.min(concurrency,items.length)},async()=>{while(next<items.length)await load(items[next++]);}));
}
