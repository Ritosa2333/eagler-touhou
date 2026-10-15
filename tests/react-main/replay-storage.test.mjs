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
test('first use does not create IDBFS; unknown schema and unsafe paths fail without upgrading/writing',async()=>{
  const factory=new IDBFactory(),read=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}));
  assert.deepEqual(await read.read('th09'),[]);assert.deepEqual(await factory.databases(),[]);
  await seed(factory,'/savesth09',[['replay/th9_01.rpy',new Uint8Array([1])]],20);
  await assert.rejects(read.read('th09'),/存储版本/);assert.equal((await factory.databases())[0].version,20);
  for(const path of ['score.dat','replay/../../score.dat','replay/a/b/th9_01.rpy','replay/th6_01.rpy'])await assert.rejects(read.read('th09',{path}));
  await assert.rejects(read.read('th06'));await assert.rejects(read.read('th09',{directory:'replay/../bad'}));
});
