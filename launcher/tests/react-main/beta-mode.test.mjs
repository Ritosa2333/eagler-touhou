import {test,beforeAll} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
import {IDBFactory,IDBKeyRange} from 'fake-indexeddb';
import {installRuntimeStorageChannel} from '../../lib/runtime-storage-channel.mjs';
import {betaRuntimeHtml,prepareBetaRuntimePublication} from '../../lib/beta-runtime-publication.mjs';
import {mkdtemp,writeFile,readFile,rm,mkdir} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';

let channel,shareBetaResources,createReplayStorageReader;
beforeAll(async()=>{
 const result=await build({stdin:{resolveDir:fileURLToPath(new URL('../../',import.meta.url)),contents:`
   export * from './src/launcher/release-channel.mts';
   export * from './src/launcher/beta-resources.mts';
   export * from './app/services/replay-storage.ts';
 `},bundle:true,write:false,format:'esm',platform:'node'});
 channel=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64'));
 ({shareBetaResources,createReplayStorageReader}=channel);
});
test('mode switch preserves language and clears room and replay navigation',()=>{
 assert.equal(channel.releaseChannelUrl('https://example.test/app/lobby.html?lang=en&room=1234&game=th06mp','https://example.test/app/',true),'https://example.test/app/beta/lobby.html?lang=en');
 assert.equal(channel.releaseChannelUrl('https://example.test/app/beta/index.html?test=1','https://example.test/app/beta/',false),'https://example.test/app/index.html');
});
test('room recovery and membership storage are separate while ordinary preferences share their owner',()=>{
 const entries=new Map(),storage={get length(){return entries.size;},key:i=>[...entries.keys()][i]??null,
  getItem:key=>entries.get(key)??null,setItem:(key,value)=>entries.set(key,value),removeItem:key=>entries.delete(key),clear:()=>entries.clear()};
 const beta=channel.channelSessionStorage(storage,true);
 storage.setItem('room','stable');beta.setItem('room','beta');
 assert.equal(storage.getItem('room'),'stable');assert.equal(beta.getItem('room'),'beta');
 assert.equal(beta.length,1);beta.clear();assert.equal(storage.getItem('room'),'stable');
});
function database(factory,name,version=21){
 return new Promise((ok,no)=>{
  const request=factory.open(name,version);
  request.onupgradeneeded=()=>request.result.createObjectStore('FILE_DATA');
  request.onsuccess=()=>ok(request.result);request.onerror=()=>no(request.error);
 });
}
async function save(factory,name,value){
 const db=await database(factory,name);
 await new Promise((ok,no)=>{
  const tx=db.transaction('FILE_DATA','readwrite');
  tx.objectStore('FILE_DATA').put({contents:new Uint8Array([value]),mode:0o100666,timestamp:new Date()},'/savesth06/replay/th6_01.rpy');
  tx.oncomplete=ok;tx.onabort=()=>no(tx.error);
 });db.close();
}
test('native saves and the offline replay reader share the beta namespace, with common content storage',async()=>{
 const factory=new IDBFactory();
 const betaFactory={open:factory.open.bind(factory),deleteDatabase:factory.deleteDatabase.bind(factory),databases:factory.databases.bind(factory)};
 const beta={location:{href:'https://example.test/beta/runtime/th06/core.html'},indexedDB:betaFactory};
 installRuntimeStorageChannel(beta);
 await save(factory,'/savesth06',1);await save(betaFactory,'/savesth06',2);
 const stableRead=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}));
 const betaRead=createReplayStorageReader(()=>({indexedDB:factory,keyRange:IDBKeyRange}),true);
 assert.deepEqual([...(await stableRead.read('th06'))[0].bytes],[1]);
 assert.deepEqual([...(await betaRead.read('th06'))[0].bytes],[2]);
 const shared=await database(betaFactory,'eagler-touhou-package-store-v1');
 shared.close();
 const names=(await factory.databases()).map(db=>db.name);
 assert.ok(names.includes('eagler-touhou-package-store-v1'));
 assert.ok(!names.includes('beta:eagler-touhou-package-store-v1'));
 assert.deepEqual((await betaFactory.databases()).map(db=>db.name),['/savesth06']);
});
test('equal package identities reuse stable resource URLs and retain beta program entries',()=>{
 const game={runtime:'runtime/th06/beta.html',music:{midi:{files:[]},ogg:{files:['a.ogg'],sha256:['a'],base:'games/th06/music/ogg/'}},gameData:{}};
 const site={shared:{vanillaFont:'shared/font.ttc'},games:{th06:game}};
 const stable={shared:{vanillaFont:'shared/font.ttc'},games:{th06:{...game,runtime:'runtime/th06/stable.html'}}};
 const catalog={games:{th06:{revision:'same',descriptor:'th06.package.json'}}};
 const result=shareBetaResources(site,catalog,stable,catalog,'https://example.test/');
 assert.equal(result.releaseCatalog.games.th06.descriptor,'/th06.package.json');
 assert.equal(result.siteManifest.games.th06.runtime,'runtime/th06/beta.html');
 assert.equal(result.siteManifest.games.th06.music.ogg.base,'https://example.test/games/th06/music/ogg/');
 assert.equal(result.siteManifest.shared.vanillaFont,'https://example.test/shared/font.ttc');
 const changed=shareBetaResources(site,{games:{th06:{revision:'different',descriptor:'beta.package.json'}}},stable,catalog,'https://example.test/');
 assert.equal(changed.releaseCatalog.games.th06.descriptor,'beta.package.json');
});

test('prebuilt beta programs receive isolation before scripts and freeze with updated identities',async()=>{
 const root=await mkdtemp(join(tmpdir(),'beta-publication-'));
 try {
  const source=join(root,'input');await mkdir(source);
  const html='<html><head><script src="core.mjs"></script></head><body></body></html>';
  const wasm=new Uint8Array([0,97,115,109]);
  const input={'th06.html':html,'core.mjs':'export const core = true;','core.wasm':wasm};
  const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
  const files={};
  for(const [path,content] of Object.entries(input)){
   await writeFile(join(source,path),content);
   files[path]={bytes:Buffer.byteLength(content),sha256:hash(content)};
  }
  const published=await prepareBetaRuntimePublication(source,'th06.html',files);
  assert.deepEqual((await prepareBetaRuntimePublication(source,'th06.html'))['th06.html'],published['th06.html']);
  const next=await readFile(join(source,'th06.html'),'utf8');
  assert.ok(next.indexOf('installRuntimeStorageChannel')<next.indexOf('src="core.mjs"'));
  assert.equal(betaRuntimeHtml(next),next,'publication is idempotent');
  assert.notEqual(published['th06.html'].sha256,files['th06.html'].sha256);
  assert.equal(published['th06.html'].sha256,hash(next));
  assert.deepEqual(published['core.wasm'],files['core.wasm']);
  const {writeRuntimeGeneration,verifyRuntimeGenerationDirectory}=await import('../../lib/runtime-generations.mjs');
  const generation=await writeRuntimeGeneration({site:root,root:'runtime/th06/',entry:'th06.html',source,names:Object.keys(files),expected:published});
  await verifyRuntimeGenerationDirectory(root,'runtime/th06/',generation);
 } finally {await rm(root,{recursive:true,force:true});}
});
