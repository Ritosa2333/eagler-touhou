import test from 'node:test';
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';
import {setTimeout as delay} from 'node:timers/promises';
import {installOfflinePackageStoreObserver} from './offline-package-observer.mjs';

const {IDBFactory}=createRequire(new URL('../../launcher/package.json',import.meta.url))('fake-indexeddb');
// Independent storage fixture: these keys and bytes are not produced by the observer.
const schema={name:'installed-packages',version:3,installations:'installations',generations:'generations',objects:'objects'};
const requestResult=request=>new Promise((resolve,reject)=>{
  request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
});
async function seed(factory,version=3){
  const request=factory.open(schema.name,version);
  request.onupgradeneeded=()=>{
    for(const store of ['installations','generations','objects'])request.result.createObjectStore(store);
  };
  const db=await requestResult(request),tx=db.transaction(['installations','generations','objects'],'readwrite');
  tx.objectStore('installations').put({currentGeneration:'revision-a'},'th08');
  tx.objectStore('generations').put({id:'revision-a',game:'th08',files:{data:{objectId:'data-object'}}},['th08','revision-a']);
  tx.objectStore('objects').put({data:Uint8Array.of(0,17,255).buffer,type:'fixture/data'},'data-object');
  tx.objectStore('objects').put({blob:new Blob([Uint8Array.of(9,8,7)])},'blob-object');
  await new Promise((resolve,reject)=>{tx.oncomplete=resolve;tx.onabort=()=>reject(tx.error);});db.close();
}
function install(indexedDB,options=schema){
  const context={indexedDB,Blob,ArrayBuffer,setTimeout,clearTimeout};
  // Match Playwright's serialized init-script boundary, with no module closure available.
  const realm={...context,options};runInNewContext(`(${installOfflinePackageStoreObserver.toString()})(options)`,realm);
  return realm.__offlinePackageStoreObserver;
}

test('serialized offline observer reads actual generation keys and Blob/ArrayBuffer bytes using readonly transactions',async()=>{
  const factory=new IDBFactory();await seed(factory);
  const modes=[],closed=[];
  const observer=install({open(name){
    const request=factory.open(name);
    request.addEventListener('success',()=>{
      const db=request.result,transaction=db.transaction.bind(db),close=db.close.bind(db);
      db.transaction=(stores,mode)=>{modes.push(mode);return transaction(stores,mode);};
      db.close=()=>{closed.push(true);close();};
    });return request;
  }});
  const current=await observer.readCurrentPackageGeneration('th08');
  assert.equal(current.installation.currentGeneration,'revision-a');
  assert.equal(current.generation.files.data.objectId,'data-object');
  assert.equal((await observer.readPackageGeneration('th08','revision-a')).id,'revision-a');
  assert.equal(await observer.readPackageGeneration('th08','missing'),null);
  for(const [id,expected] of [['data-object',[0,17,255]],['blob-object',[9,8,7]]]){
    const value=await observer.readPackageObject(id);
    assert.deepEqual([...new Uint8Array(await value.blob.arrayBuffer())],expected);
  }
  assert.equal(await observer.readPackageObject('missing-object'),null);
  const missing=await observer.readCurrentPackageGeneration('th09');
  assert.equal(missing.installation,null);assert.equal(missing.generation,null);
  assert.deepEqual(modes,Array(7).fill('readonly'));assert.equal(closed.length,7);
  // Reads retain both schema and the fixture's original records.
  const db=await requestResult(factory.open(schema.name)),tx=db.transaction('objects','readonly');
  assert.deepEqual([...new Uint8Array((await requestResult(tx.objectStore('objects').get('data-object'))).data)],[0,17,255]);
  db.close();
});

test('offline observer rejects a missing or mismatched database without creating or upgrading it',async()=>{
  const missing=new IDBFactory();
  await assert.rejects(install(missing).readCurrentPackageGeneration('th08'),/absent/);
  assert.deepEqual(await missing.databases(),[]);
  for(const version of [2,4]){
    const factory=new IDBFactory();await seed(factory,version);
    await assert.rejects(install(factory).readCurrentPackageGeneration('th08'),/version .* differs/);
    assert.deepEqual(await factory.databases(),[{name:schema.name,version}]);
  }
});

test('offline observer bounds a stalled open and closes a late connection',async()=>{
  const request={};let closed=0;
  const pending=install({open:()=>request},{...schema,timeoutMs:10}).readCurrentPackageGeneration('th08');
  await assert.rejects(pending,/timed out/);
  request.result={close(){closed++;}};request.onsuccess();
  assert.equal(closed,1);
});

test('offline observer bounds a stalled transaction, aborts it and closes the database',async()=>{
  const request={};let aborted=0,closed=0;
  const transaction={objectStore:()=>({get:()=>({})}),abort(){aborted++;}};
  const pending=install({open:()=>request},{...schema,timeoutMs:10}).readCurrentPackageGeneration('th08');
  request.result={version:3,transaction:()=>transaction,close(){closed++;}};request.onsuccess();
  await assert.rejects(pending,/timed out/);
  assert.equal(aborted,1);assert.equal(closed,1);
  await delay(5);assert.equal(closed,1);
});
