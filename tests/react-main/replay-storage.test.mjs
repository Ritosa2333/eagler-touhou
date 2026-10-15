import test,{before,after} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {IDBFactory,IDBKeyRange} from 'fake-indexeddb';
const root=fileURLToPath(new URL('../../',import.meta.url));let work,createReplayStorageReader;
before(async()=>{await mkdir(resolve(root,'.cache'),{recursive:true});work=await mkdtemp(resolve(root,'.cache/replay-storage-'));const out=resolve(work,'reader.mjs');await build({entryPoints:[resolve(root,'app/services/replay-storage.ts')],outfile:out,bundle:true,platform:'node',format:'esm',logLevel:'silent'});({createReplayStorageReader}=await import(pathToFileURL(out).href));});
after(async()=>{if(work)await rm(work,{recursive:true,force:true});});
const match={schema:'eagler-touhou/th09-match-replay/1',names:['甲','乙'],startedAt:1000,frames:600,winner:1};
async function seed(factory,name,entries,version=21){const db=await new Promise((ok,no)=>{const r=factory.open(name,version);r.onupgradeneeded=()=>r.result.createObjectStore('FILE_DATA');r.onsuccess=()=>ok(r.result);r.onerror=()=>no(r.error);});await new Promise((ok,no)=>{const tx=db.transaction('FILE_DATA','readwrite');for(const [path,contents]of entries)tx.objectStore('FILE_DATA').put({contents,mode:0o100666,timestamp:new Date(2000)},name+'/'+path);tx.oncomplete=ok;tx.onabort=()=>no(tx.error);});db.close();}
function signedBytes(bytes){const buffer=new ArrayBuffer(bytes.length+8);new Uint8Array(buffer,3,bytes.length).set(bytes);return new Int8Array(buffer,3,bytes.length);}
function motionReplay(){
  const bytes=new Uint8Array(4+92+24),view=new DataView(bytes.buffer),end=bytes.length-24;
  bytes.set([128,255,84,57]);view.setUint32(4,1,true);view.setUint32(84,31,true);view.setFloat32(88,12.5,true);view.setFloat32(92,-6.25,true);
  bytes.set(new TextEncoder().encode('THMOTION'),end);view.setUint32(end+8,9,true);view.setUint32(end+12,2,true);view.setUint32(end+16,92,true);
  let hash=2166136261;for(let offset=4;offset<end;offset++)hash=Math.imul(hash^bytes[offset],16777619)>>>0;view.setUint32(end+20,hash,true);return bytes;
}
test('readonly IDBFS snapshot isolates roots, exposes file time, keeps matching sidecars and original bytes',async()=>{
  const factory=new IDBFactory(),read=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}));
  await seed(factory,'/savesth09mp',[
    ['replay/A/th9_ud0000.rpy',new Uint8Array([1,2,3])],['replay/A/th9_ud0000.json',new TextEncoder().encode(JSON.stringify(match))],
    ['replay/AB/th9_ud0000.rpy',new Uint8Array([9])],['replay/A/../../score.dat',new Uint8Array([8])],['score.dat',new Uint8Array([7])],
  ]);await seed(factory,'/savesth09',[['replay/th9_01.rpy',new Uint8Array([6])]]);
  const rows=await read.read('th09mp');assert.equal(rows.length,2);assert.deepEqual([...rows.find(r=>r.replayMatch).bytes],[1,2,3]);assert.equal(rows[0].modifiedAt,2000);
  const folder=await read.read('th09mp',{directory:'replay/A'});assert.equal(folder.length,1);assert.deepEqual(folder[0].replayMatch,match);
  const single=await read.read('th09mp',{path:'replay/A/th9_ud0000.rpy'});assert.deepEqual([...single[0].bytes],[1,2,3]);single[0].bytes.fill(0);
  assert.deepEqual([...(await read.read('th09mp',{path:'replay/A/th9_ud0000.rpy'}))[0].bytes],[1,2,3],'download mutation cannot alter persisted bytes');
  assert.equal((await read.read('th09'))[0].path,'replay/th9_01.rpy');
});
test('native signed IDBFS byte views preserve offsets, motion exports and MP sidecars',async()=>{
  const factory=new IDBFactory(),read=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange})),bytes=motionReplay();
  await seed(factory,'/savesth09mp',[
    ['replay/A/th9_ud0000.rpy',signedBytes(bytes)],
    ['replay/A/th9_ud0000.json',signedBytes(new TextEncoder().encode(JSON.stringify(match)))],
  ]);
  await seed(factory,'/savesth09',[['replay/th9_01.rpy',signedBytes(new Uint8Array([128,255,0,127]))]]);
  for(const selection of [{},{directory:'replay/A'},{path:'replay/A/th9_ud0000.rpyx'}]){
    const rows=await read.read('th09mp',selection);assert.equal(rows.length,1);assert.equal(rows[0].path,'replay/A/th9_ud0000.rpyx');
    assert.ok(rows[0].bytes instanceof Uint8Array);assert.deepEqual(rows[0].bytes,bytes);assert.equal(rows[0].size,bytes.length);assert.deepEqual(rows[0].replayMatch,match);
    rows[0].bytes.fill(0);
  }
  assert.deepEqual([...(await read.read('th09'))[0].bytes],[128,255,0,127]);
});
test('non-byte IDBFS contents remain invalid',async()=>{
  for(const contents of [[1,2],new ArrayBuffer(2),new Uint16Array([1,2]),new DataView(new ArrayBuffer(2))]){
    const factory=new IDBFactory(),read=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}));
    await seed(factory,'/savesth09mp',[['replay/th9_01.rpy',contents]]);
    await assert.rejects(read.read('th09mp'),/本地录像内容无效/);
  }
});
test('first use does not create IDBFS; unknown schema and unsafe paths fail without upgrading/writing',async()=>{
  const factory=new IDBFactory(),read=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}));
  assert.deepEqual(await read.read('th09'),[]);assert.deepEqual(await factory.databases(),[]);
  await seed(factory,'/savesth09',[['replay/th9_01.rpy',new Uint8Array([1])]],20);
  await assert.rejects(read.read('th09'),/存储版本/);assert.equal((await factory.databases())[0].version,20);
  for(const path of ['score.dat','replay/../../score.dat','replay/a/b/th9_01.rpy','replay/th6_01.rpy'])await assert.rejects(read.read('th09',{path}));
  await assert.rejects(read.read('th06'));await assert.rejects(read.read('th09',{directory:'replay/../bad'}));
});
