import {createDatOggProvider} from './dat-ogg-provider.mjs';
const HASH=/^[0-9a-f]{64}$/;
async function sha(bytes){return [...new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes))].map(v=>v.toString(16).padStart(2,'0')).join('');}
function current(signal,isCurrent){if(signal?.aborted||!isCurrent())throw new DOMException('DAT package installation owner was cancelled','AbortError');}
function record(value){
 if(!value||typeof value.revision!=='string'||!value.revision||!HASH.test(value.sha256)||!(value.blob instanceof Blob)
  ||!Number.isSafeInteger(value.bytes)||value.bytes<1||value.bytes>256*1024*1024||value.blob.size!==value.bytes)throw new Error('Invalid DAT archive record');
 return value;
}
export class DatPackageArchiveStore{
 constructor({indexedDB=globalThis.indexedDB,dbName='th128-resource-dat-ogg-v1'}={}){
  if(typeof indexedDB?.open!=='function')throw new Error('Resource archive IndexedDB is unavailable');
  this.indexedDB=indexedDB;this.dbName=dbName;this.db=null;this.opening=null;
 }
 async open(){
  if(this.db)return this.db;
  if(!this.opening)this.opening=new Promise((resolve,reject)=>{
   const request=this.indexedDB.open(this.dbName,1);
   request.onupgradeneeded=()=>{if(!request.result.objectStoreNames.contains('archive'))request.result.createObjectStore('archive',{keyPath:'key'});};
   request.onerror=()=>reject(request.error??new Error('Resource archive database open failed'));
   request.onblocked=()=>reject(new Error('Resource archive database is blocked'));
   request.onsuccess=()=>{const db=request.result;db.onversionchange=()=>{db.close();if(this.db===db)this.db=null;};this.db=db;resolve(db);};
  }).finally(()=>{this.opening=null;});
  return this.opening;
 }
 async transaction(mode,value){
  const db=await this.open();
  return new Promise((resolve,reject)=>{
   const tx=db.transaction(['archive'],mode),store=tx.objectStore('archive');
   let result=null;
   tx.oncomplete=()=>resolve(result);
   tx.onabort=()=>reject(tx.error??new Error('Resource archive transaction aborted'));
   tx.onerror=()=>reject(tx.error??new Error('Resource archive transaction failed'));
   const request=mode==='readonly'?store.get('current'):store.put({...value,key:'current'});
   request.onsuccess=()=>{result=request.result??null;};
   request.onerror=()=>reject(request.error??new Error('Resource archive request failed'));
  });
 }
 async readCurrent(){const result=await this.transaction('readonly');return result?record(result):null;}
 async commit(value){record(value);await this.transaction('readwrite',value);}
 close(){this.db?.close();this.db=null;}
}
export async function installDatPackage(input,{store,origin,signal,isCurrent=()=>true,onProgress=()=>{},expectedRevision}={}){
 if(!store||typeof store.commit!=='function'||typeof store.readCurrent!=='function')throw new Error('Resource archive store is unavailable');
 current(signal,isCurrent);let provider=null;
 try{
  provider=await createDatOggProvider(input,{origin,signal,isCurrent,onProgress});
  if(expectedRevision!==undefined&&provider.revision!==expectedRevision)throw new Error('Downloaded online resource revision mismatch before cache commit');
  for(let index=0;index<provider.targets.length;index++){
   current(signal,isCurrent);
   await provider.fetch(new URL(provider.targets[index],origin),{method:'HEAD',signal});
   if(index%25===0)onProgress({verified:index+1,total:provider.targets.length});
  }
  current(signal,isCurrent);
  const owned=new Uint8Array(input),digest=await sha(owned);current(signal,isCurrent);
  const next={revision:provider.revision,bytes:owned.length,sha256:digest,blob:new Blob([owned],{type:'application/zip'})};
  await store.commit(next);current(signal,isCurrent);
  const saved=record(await store.readCurrent());current(signal,isCurrent);
  if(saved.revision!==next.revision||saved.bytes!==next.bytes||saved.sha256!==digest||await sha(new Uint8Array(await saved.blob.arrayBuffer()))!==digest)
   throw new Error('Resource archive complete readback identity mismatch');
  current(signal,isCurrent);return provider;
 }catch(error){provider?.dispose();throw error;}
}
export async function reopenDatPackage({store,origin,signal,isCurrent=()=>true,onProgress=()=>{}}={}){
 current(signal,isCurrent);const saved=await store.readCurrent();current(signal,isCurrent);if(!saved)return null;
 record(saved);const bytes=new Uint8Array(await saved.blob.arrayBuffer());current(signal,isCurrent);
 if(bytes.length!==saved.bytes||await sha(bytes)!==saved.sha256)throw new Error('Stored archive SHA-256 identity mismatch');
 current(signal,isCurrent);
 const provider=await createDatOggProvider(bytes,{origin,signal,isCurrent,onProgress});
 if(provider.revision!==saved.revision){provider.dispose();throw new Error('Stored archive revision identity mismatch');}
 return provider;
}
