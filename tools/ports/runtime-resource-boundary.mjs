import assert from 'node:assert/strict';

export async function readInstalledPackageSnapshot(game){
  const current=await window.__offlinePackageStoreObserver.readCurrentPackageGeneration(game);
  if(!current.generation)throw Error('Installed Package Store generation is missing');
  const generation=current.generation;
  const objects={};
  for(const objectId of [...new Set(Object.values(generation.files).map(file=>file.objectId))].sort()){
    const object=await window.__offlinePackageStoreObserver.readPackageObject(objectId);
    if(!object?.blob)throw Error(`Installed Package Store object is missing: ${objectId}`);
    const bytes=await object.blob.arrayBuffer();
    const sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
    objects[objectId]={bytes:bytes.byteLength,sha256};
  }
  return{generationId:generation.id,revision:generation.descriptor.revision,files:generation.files,objects};
}

/** An empty write transaction exercises the real Runtime realm without changing package bytes. */
export async function probeRuntimePackageWrite({database,store}){
  const runtime=document.getElementById('gameFrame').contentWindow;
  return new Promise((accept,reject)=>{
    const request=runtime.indexedDB.open(database);
    request.onerror=()=>reject(request.error);
    request.onsuccess=()=>{
      const db=request.result,transaction=db.transaction([store],'readwrite');
      transaction.oncomplete=()=>{db.close();accept({database,store,mode:'readwrite'});};
      transaction.onabort=transaction.onerror=()=>{db.close();reject(transaction.error);};
    };
  });
}

/** Installed before Runtime scripts. Lease bookkeeping remains a read consumer. */
export function installRuntimePackageWriteObserver({database,stores}){
  if(window===window.top)return;
  if(globalThis.__eaglerRuntimePackageWrites)throw Error('Runtime Package Store observer already installed');
  const writes=[];
  Object.defineProperty(globalThis,'__eaglerRuntimePackageWrites',{value:writes,configurable:false});
  const transaction=IDBDatabase.prototype.transaction;
  IDBDatabase.prototype.transaction=function(names,mode,...rest){
    const selected=typeof names==='string'?[names]:Array.from(names);
    if(this.name===database&&mode&&mode!=='readonly'&&selected.some(name=>stores.includes(name)))
      writes.push({operation:'transaction',database:this.name,stores:selected,mode});
    return transaction.call(this,names,mode,...rest);
  };
  const remove=IDBFactory.prototype.deleteDatabase;
  IDBFactory.prototype.deleteDatabase=function(name,...rest){
    if(name===database)writes.push({operation:'deleteDatabase',database:name});
    return remove.call(this,name,...rest);
  };
  const open=IDBFactory.prototype.open;
  IDBFactory.prototype.open=function(name,...rest){
    const request=open.call(this,name,...rest);
    if(name===database)request.addEventListener('upgradeneeded',event=>{
      writes.push({operation:'upgrade',database:name,oldVersion:event.oldVersion,newVersion:event.newVersion});
    });
    return request;
  };
}

export function assertRuntimeResourceBoundary({before,after,dataRead,writes,expectedData}){
  assert.ok(before?.generationId&&before.revision,'Installed package identity is missing before Runtime launch');
  assert.ok(before.files&&Object.keys(before.files).length,'Installed package object inventory is empty');
  assert.ok(before.objects&&Object.keys(before.objects).length,'Installed package object byte identities are missing');
  assert.deepEqual(after,before,'Runtime changed the installed Package Store generation or its file references');
  assert.ok(dataRead,'Runtime did not consume Launcher-provided managed DATA');
  assert.equal(dataRead.generation,before.generationId,'Runtime consumed a different package generation');
  assert.equal(dataRead.bytes,expectedData.bytes,'Runtime managed DATA byte count differs from installed input');
  assert.equal(dataRead.sha256,expectedData.sha256,'Runtime managed DATA hash differs from installed input');
  assert.ok(Array.isArray(writes),'Runtime Package Store write observation is missing');
  assert.deepEqual(writes,[],'Runtime wrote installation objects, generations or the current installation');
  return{before,after,dataRead,writes,expectedData,scope:'selected Runtime launch and managed DATA acquisition'};
}
