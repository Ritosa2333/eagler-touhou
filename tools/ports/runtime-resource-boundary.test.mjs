import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {webcrypto} from 'node:crypto';
import {assertRuntimeResourceBoundary,installRuntimePackageWriteObserver,readInstalledPackageSnapshot} from './runtime-resource-boundary.mjs';

const before={generationId:'installed-original',revision:'package-revision',files:{data:{objectId:'data-1'}},
  objects:{'data-1':{bytes:4,sha256:'a'.repeat(64)}}};
const dataRead={generation:before.generationId,bytes:4,sha256:'a'.repeat(64)};
const input={before,after:structuredClone(before),dataRead,writes:[],expectedData:{bytes:4,sha256:'a'.repeat(64)}};
test('resource boundary rejects a replaced generation, changed object, wrong managed bytes and write attempt',()=>{
  assertRuntimeResourceBoundary(input);
  for(const after of [{...before,generationId:'new'}, {...before,files:{data:{objectId:'new'}}},
    {...before,objects:{'data-1':{bytes:4,sha256:'b'.repeat(64)}}}])
    assert.throws(()=>assertRuntimeResourceBoundary({...input,after}),/changed the installed/);
  assert.throws(()=>assertRuntimeResourceBoundary({...input,dataRead:{...dataRead,sha256:'b'.repeat(64)}}),/hash differs/);
  assert.throws(()=>assertRuntimeResourceBoundary({...input,dataRead:{...dataRead,generation:'other'}}),/different package/);
  assert.throws(()=>assertRuntimeResourceBoundary({...input,writes:[{operation:'transaction'}]}),/Runtime wrote/);
  assert.throws(()=>assertRuntimeResourceBoundary({...input,writes:null}),/observation is missing/);
});

test('browser observer records actual authoritative write operations and leaves forwarding and lease writes intact',()=>{
  const calls=[],listeners=[];
  class Database{constructor(name){this.name=name;}transaction(...args){calls.push(args);return args;}}
  class Factory{deleteDatabase(name){return name;}open(name){return{addEventListener(type,callback){listeners.push({name,type,callback});}};}}
  const context={window:{top:{}},IDBDatabase:Database,IDBFactory:Factory};
  runInNewContext(`(${installRuntimePackageWriteObserver.toString()})({database:'packages',stores:['objects','generations','installations']})`,context);
  const db=new Database('packages');
  assert.deepEqual(db.transaction(['objects'],'readonly'),[['objects'],'readonly']);
  db.transaction('leases','readwrite');new Database('game-saves').transaction('objects','readwrite');
  assert.equal(context.__eaglerRuntimePackageWrites.length,0);
  for(const name of ['objects','generations','installations'])db.transaction(name,'readwrite');
  const factory=new Factory();factory.open('packages');listeners[0].callback({oldVersion:3,newVersion:4});
  assert.equal(factory.deleteDatabase('packages'),'packages');
  const writes=JSON.parse(JSON.stringify(context.__eaglerRuntimePackageWrites));
  assert.equal(writes.length,5);
  assert.deepEqual(writes.slice(0,3).map(item=>item.stores[0]),['objects','generations','installations']);
  assert.deepEqual(writes.slice(3).map(item=>item.operation),['upgrade','deleteDatabase']);
  assert.equal(calls.length,6,'Observation must forward every transaction unchanged');
});

test('serialized Package Store observer hashes blob bytes independently of stored hash metadata',async()=>{
  const reads=[],files={data:{objectId:'same-object'},alias:{objectId:'same-object'}};
  const observer={
    async readCurrentPackageGeneration(){return{generation:{id:'real',descriptor:{revision:'revision'},files}};},
    async readPackageObject(id){reads.push(id);return{sha256:'f'.repeat(64),blob:new Blob(['abc'])};},
  };
  const source=`(${readInstalledPackageSnapshot.toString()})('th08')`;
  const value=await runInNewContext(source,{window:{__offlinePackageStoreObserver:observer},crypto:webcrypto});
  assert.deepEqual(JSON.parse(JSON.stringify(value)),{generationId:'real',revision:'revision',files,
    objects:{'same-object':{bytes:3,sha256:'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad'}}});
  assert.deepEqual(reads,['same-object'],'Aliased files reuse one actual object read');
  observer.readPackageObject=async()=>null;
  await assert.rejects(runInNewContext(source,{window:{__offlinePackageStoreObserver:observer},crypto:webcrypto}),/object is missing/);
});
