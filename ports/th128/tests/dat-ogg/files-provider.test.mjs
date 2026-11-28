import test from 'node:test';
import assert from 'node:assert/strict';
import * as providerModule from '../../site/runtime/dat-ogg-provider.mjs';
import {fixture,refreshDescriptor,packageZip,origin,rawBytes,denseBytes,sha} from './file-fixture.mjs';
const factory=()=>{
 assert.equal(typeof providerModule.createDatOggProviderFromFiles,'function','Missing file-backed original DAT provider');
 return providerModule.createDatOggProviderFromFiles;
};
const body=async(provider,path,init)=>new Uint8Array(await(await provider.fetch(origin+path,init)).arrayBuffer());

test('file payloads actually supply THA1 raw assets, derived assets and original music bytes',async()=>{
 const create=factory(),f=fixture(),progress=[];
 const provider=await create(f.descriptorBytes,f.files,{origin,onProgress:p=>progress.push(p)});
 try{
  assert.deepEqual([...await body(provider,'/assets/runtime/front.anm')],[11,22,33,44,55,66,77,88]);
  assert.deepEqual(await(await provider.fetch(origin+'/assets/catalog.json')).json(),{variants:{zh:{title:'Synthetic fixture'}}});
  assert.deepEqual([...await body(provider,'/assets/music/first.ogg')],[79,103,103,83,0,0,0,0]);
  assert.deepEqual(provider.targets,['/assets/catalog.json','/assets/dense.zip','/assets/music/first.ogg','/assets/runtime/front.anm']);
  assert.equal(progress.at(-1).completed,12);assert.equal(progress.at(-1).total,12);
 }finally{provider.dispose();}
});
test('all caller buffers and map ownership detach before asynchronous verification',async()=>{
 const create=factory(),f=fixture(),pending=create(f.descriptorBytes,f.files,{origin});
 f.descriptorBytes.fill(0);for(const bytes of f.files.values())bytes.fill(0);f.files.clear();
 const provider=await pending;
 try{assert.deepEqual(await body(provider,'/assets/runtime/front.anm'),rawBytes);assert.deepEqual(await body(provider,'/assets/dense.zip'),denseBytes);}
 finally{provider.dispose();}
});
test('returned response bytes and exported descriptor cannot change later reads',async()=>{
 const create=factory(),f=fixture(),provider=await create(f.descriptorBytes,f.files,{origin});
 try{
  (await body(provider,'/assets/runtime/front.anm')).fill(0);
  provider.descriptor.files['game-data'].sha256='0'.repeat(64);
  assert.deepEqual(await body(provider,'/assets/runtime/front.anm'),rawBytes);
 }finally{provider.dispose();}
});
for(const [name,mutate]of [
 ['missing declared ID',f=>f.files.delete('music-09')],
 ['undeclared ID',f=>f.files.set('injected',new Uint8Array([1]))],
 ['wrong payload type',f=>f.files.set('game-data',new ArrayBuffer(51))],
 ['wrong payload size',f=>f.files.set('game-data',new Uint8Array(50))],
 ['same-length altered SHA',f=>f.files.get('game-data')[0]^=1],
 ['unsafe source',f=>{f.descriptor.files['game-data'].source='../th128.dat';refreshDescriptor(f);}],
 ['duplicate target',f=>{f.descriptor.files['music-01'].target=f.descriptor.files['music-00'].target;refreshDescriptor(f);}],
 ['script carried as a resource',f=>{f.descriptor.files['game-data'].source='games/th128/core.mjs';refreshDescriptor(f);}],
 ['foreign mount layout',f=>{f.descriptor.runtimeRequirement.dataLayout='sha256-'+'f'.repeat(64);refreshDescriptor(f);}],
 ['missing OGG component member',f=>{f.descriptor.components.ogg.files.pop();refreshDescriptor(f);}]
])test('file provider refuses '+name,async()=>{
 const create=factory(),f=fixture();mutate(f);
 await assert.rejects(create(f.descriptorBytes,f.files,{origin}),/package|payload|file|mount|OGG|size|SHA|source|layout|target/i);
});
test('a matching digest never turns a renamed ZIP into a valid original DAT',async()=>{
 const create=factory(),f=fixture(),renamed=new Uint8Array([80,75,3,4,...new Uint8Array(47)]);
 f.files.set('game-data',renamed);f.descriptor.files['game-data'].sha256=sha(renamed);refreshDescriptor(f);
 await assert.rejects(create(f.descriptorBytes,f.files,{origin}),/THA1|magic|archive|table/i);
});
test('file-table container and descriptor bytes require their actual byte and Map types',async()=>{
 const create=factory(),f=fixture();
 await assert.rejects(create(f.descriptor,f.files,{origin}),/descriptor|bytes|Uint8Array/i);
 await assert.rejects(create(f.descriptorBytes,[...f.files],{origin}),/Map|file|payload/i);
});
test('language-index receipts cannot refer to a DAT entry absent from the actual archive',async()=>{
 const create=factory(),f=fixture(index=>{index.raw['absent.anm']={bytes:8,sha256:sha(rawBytes)};});
 await assert.rejects(create(f.descriptorBytes,f.files,{origin}),/unknown DAT resource/);
});
test('construction cancelled during pending SHA cannot return a live provider',async t=>{
 const create=factory(),f=fixture(),controller=new AbortController();
 const real=globalThis.crypto.subtle.digest.bind(globalThis.crypto.subtle);
 let entered,release;
 const enteredPromise=new Promise(resolve=>{entered=resolve;}),gate=new Promise(resolve=>{release=resolve;});
 let first=true;
 t.mock.method(globalThis.crypto.subtle,'digest',async(...args)=>{
  if(first){first=false;entered();await gate;}return real(...args);
 });
 const pending=create(f.descriptorBytes,f.files,{origin,signal:controller.signal});
 await enteredPromise;controller.abort();release();
 await assert.rejects(pending,error=>error.name==='AbortError');
});
test('request cancellation during real deflate output rejects that fetch and permits a fresh request',async()=>{
 const create=factory(),f=fixture(),provider=await create(f.descriptorBytes,f.files,{origin}),controller=new AbortController();
 const Native=globalThis.DecompressionStream;
 let entered,release;const enteredPromise=new Promise(resolve=>{entered=resolve;}),gate=new Promise(resolve=>{release=resolve;});
 globalThis.DecompressionStream=class{
  constructor(format){
   const stream=new Native(format);this.writable=stream.writable;
   this.readable=stream.readable.pipeThrough(new TransformStream({async transform(chunk,output){entered();await gate;output.enqueue(chunk);}}));
  }
 };
 try{
  const pending=provider.fetch(origin+'/assets/dense.zip',{signal:controller.signal});
  await enteredPromise;controller.abort();release();
  await assert.rejects(pending,error=>error.name==='AbortError');
 }finally{globalThis.DecompressionStream=Native;}
 try{assert.deepEqual(await body(provider,'/assets/dense.zip'),denseBytes);}
 finally{provider.dispose();}
});
test('bad candidate does not retire or alter an existing live provider',async()=>{
 const create=factory(),f=fixture(),provider=await create(f.descriptorBytes,f.files,{origin});
 try{
  const invalid=fixture();invalid.files.get('music-00')[4]=99;
  await assert.rejects(create(invalid.descriptorBytes,invalid.files,{origin}),/SHA/);
  assert.deepEqual(await body(provider,'/assets/runtime/front.anm'),rawBytes);
 }finally{provider.dispose();}
});
test('file-source range/HEAD/origin and disposed-owner contracts match existing resource consumers',async()=>{
 const create=factory(),f=fixture(),provider=await create(f.descriptorBytes,f.files,{origin});
 const response=await provider.fetch(origin+'/assets/runtime/front.anm',{headers:{Range:'bytes=2-4'}});
 assert.equal(response.status,206);assert.equal(response.headers.get('Content-Range'),'bytes 2-4/8');
 assert.deepEqual([...new Uint8Array(await response.arrayBuffer())],[33,44,55]);
 const head=await provider.fetch(origin+'/assets/runtime/front.anm',{method:'HEAD'});
 assert.equal(head.headers.get('Content-Length'),'8');assert.equal((await head.arrayBuffer()).byteLength,0);
 await assert.rejects(provider.fetch('https://other.invalid/assets/runtime/front.anm'),/origin/);
 await assert.rejects(provider.fetch(origin+'/assets/runtime/front.anm',{method:'POST'}),/method/);
 provider.dispose();await assert.rejects(provider.fetch(origin+'/assets/runtime/front.anm'),error=>error.name==='AbortError');
});
test('unchanged ZIP entry still uses the same authenticated DAT and asset consumer',async()=>{
 const f=fixture(),provider=await providerModule.createDatOggProvider(packageZip(f),{origin});
 try{assert.deepEqual(await body(provider,'/assets/runtime/front.anm'),rawBytes);assert.deepEqual(await body(provider,'/assets/dense.zip'),denseBytes);}
 finally{provider.dispose();}
});
