import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {initializeSaveStorage,migrateLegacySaves} from '../th10_web/sdl-runtime/save-storage.mjs';

const root=resolve(fileURLToPath(new URL('../',import.meta.url)));
const shell=readFileSync(resolve(root,'th10_web/sdl-runtime/shell.mjs'),'utf8');
const fileHost=readFileSync(resolve(root,'th10_web/cpp/sdl/FileHost.cpp'),'utf8');
assert.match(shell,/import \{initializeSaveStorage,migrateLegacySaves\} from '\.\/save-storage\.mjs'/);
assert.match(shell,/storage=await initializeSaveStorage\(\{game,runtimeVariant,setCompiledVariant:value=>core\.sdl_files_variant\(value\)/);
assert.match(shell,/await migrateLegacySaves\(storage,\{indexedDB,filesystem:Module\.FS,sync/);
assert.match(shell,/runtimeVariant=query\.get\('runtimeVariant'\)\?\?'normal'/);
assert.match(fileHost,/export_name\("sdl_files_variant"\)/);

function mockFilesystem(){
 const events=[],files=new Map();
 return {
  events,files,
  mkdirTree(path){events.push(['mkdir',path]);},
  mount(_idbfs,_options,path){events.push(['mount',path]);},
  analyzePath(path){events.push(['stat',path]);return {exists:files.has(path)};},
  writeFile(path,bytes){events.push(['write',path]);files.set(path,new Uint8Array(bytes));},
 };
}

async function initialize(runtimeVariant,compiledVariant='normal'){
 const filesystem=mockFilesystem(),events=filesystem.events,identityCalls=[];
 const profile=await initializeSaveStorage({
  game:'th10',runtimeVariant,setCompiledVariant:value=>{identityCalls.push(value);return (value===1)===(compiledVariant==='multiplayer')?1:0;},
  filesystem,idbfs:{kind:'IDBFS'},sync:async populate=>events.push(['sync',populate]),
  beforeMount:()=>events.push(['beforeMount']),
 });
 return {profile,filesystem,identityCalls};
}

// URL selection, C++ compile-identity guard, mount, and restore are one callable
// path used by shell.mjs. A mismatch must fail before even the before-mount hook.
const normal=await initialize(undefined);
assert.equal(normal.profile.variant,'normal');
assert.equal(normal.profile.namespace,'/savesth10');
assert.equal(normal.profile.root('jp'),'/savesth10/jp');
assert.equal(normal.profile.root('chs'),'/savesth10/chs');
assert.deepEqual(normal.identityCalls,[0]);
assert.deepEqual(normal.filesystem.events,[
 ['beforeMount'],['mkdir','/savesth10'],['mount','/savesth10'],['sync',true],
]);
const multiplayer=await initialize('multiplayer','multiplayer');
assert.equal(multiplayer.profile.namespace,'/savesth10-multiplayer');
assert.equal(multiplayer.profile.root('jp'),'/savesth10-multiplayer/jp');
assert.equal(multiplayer.profile.root('chs'),'/savesth10-multiplayer/chs');
assert.deepEqual(multiplayer.identityCalls,[1]);
assert.deepEqual(multiplayer.filesystem.events,[
 ['beforeMount'],['mkdir','/savesth10-multiplayer'],['mount','/savesth10-multiplayer'],['sync',true],
]);

for(const [variant,compiled] of [['preview','normal'],['multiplayer','normal'],['normal','multiplayer']]){
 const filesystem=mockFilesystem();let beforeMountCalls=0,syncCalls=0;
 await assert.rejects(()=>initializeSaveStorage({
  game:'th10',runtimeVariant:variant,setCompiledVariant:value=>(value===(compiled==='multiplayer'?1:0)?1:0),
  filesystem,idbfs:{},sync:async()=>{syncCalls++;},beforeMount:()=>{beforeMountCalls++;},
 }),variant==='preview'?/Unsupported runtimeVariant/:/Runtime variant mismatch/);
 assert.deepEqual(filesystem.events,[],'variant rejection must precede all FS calls');
 assert.equal(beforeMountCalls,0);
 assert.equal(syncCalls,0);
}

assert.equal(normal.profile.relativeSave('scoreth10.dat'),'scoreth10.dat');
assert.equal(normal.profile.relativeSave('/savesth10/chs/replay/TH10_01.RPY'),'replay/th10_01.rpy');
assert.equal(multiplayer.profile.relativeSave('replay/th10_02.rpyx'),'replay/th10_02.rpyx');
assert.equal(multiplayer.profile.relativeSave('/savesth10-multiplayer/jp/scoreth10c.dat'),'scoreth10c.dat');
assert.throws(()=>normal.profile.relativeSave('/savesth10-multiplayer/jp/scoreth10.dat'),/different runtime variant/);
assert.throws(()=>multiplayer.profile.relativeSave('/savesth10/chs/replay/th10_01.rpy'),/different runtime variant/);
assert.throws(()=>normal.profile.relativeSave('../scoreth10.dat'),/Invalid save path/);
assert.throws(()=>multiplayer.profile.root('fr'),/Invalid save language/);

function makeDatabase(rows){
 return {
  objectStoreNames:{contains:name=>name==='files'},
  transaction(){
   const tx={oncomplete:null,onerror:null};
   tx.objectStore=()=>({openCursor(){
    let index=0;const request={onsuccess:null};
    const visit=()=>{
     if(index<rows.length){
      const [key,value]=rows[index++];
      request.onsuccess?.({target:{result:{key,value,continue:()=>queueMicrotask(visit)}}});
     }else queueMicrotask(()=>tx.oncomplete?.());
    };
    queueMicrotask(visit);return request;
   }});
   return tx;
  },
  close(){},
 };
}

function mockIndexedDB(stores){
 const calls={databases:0,opened:[]};
 return {
  calls,
  async databases(){calls.databases++;return [...stores.keys()].map(name=>({name}));},
  open(name){
   calls.opened.push(name);const request={};
   queueMicrotask(()=>{request.result=makeDatabase(stores.get(name)||[]);request.onsuccess?.();});
   return request;
  },
 };
}

// Execute the shell's legacy migration implementation with mock IndexedDB/FS.
normal.filesystem.events.length=0;
const legacy10=mockIndexedDB(new Map([
 ['th10-1.00a-jp',[
  ['scoreth10.dat',new Uint8Array([1,2])],
  ['replay/th10_01.rpy',new Uint8Array([3])],
  ['/savesth10-multiplayer/jp/scoreth10.dat',new Uint8Array([4])],
 ]],
 ['th10-1.00a-chs',[
  ['scoreth10c.dat',new Uint8Array([5,6])],
 ]],
]));
let imported10=0,synced10=[];
assert.equal(await migrateLegacySaves(normal.profile,{
 indexedDB:legacy10,filesystem:normal.filesystem,sync:async populate=>synced10.push(populate),
 importReplayName:path=>{imported10++;return path;},
}),true);
assert.deepEqual(legacy10.calls.opened,['th10-1.00a-jp','th10-1.00a-chs']);
assert.deepEqual([...normal.filesystem.files.get('/savesth10/jp/scoreth10.dat')],[1,2]);
assert.deepEqual([...normal.filesystem.files.get('/savesth10/jp/replay/th10_01.rpy')],[3]);
assert.deepEqual([...normal.filesystem.files.get('/savesth10/chs/scoreth10c.dat')],[5,6]);
assert.equal(normal.filesystem.files.has('/savesth10-multiplayer/jp/scoreth10.dat'),false);
assert.equal(imported10,3);
assert.equal(normal.filesystem.files.has('/savesth10/.migration-v3'),true);
assert.deepEqual(synced10,[false]);
const legacyReadsBeforeRepeat=legacy10.calls.databases;
assert.equal(await migrateLegacySaves(normal.profile,{indexedDB:legacy10,filesystem:normal.filesystem,sync:async()=>{},importReplayName:path=>path}),false);
assert.equal(legacy10.calls.databases,legacyReadsBeforeRepeat,'completed migration is idempotent');

multiplayer.filesystem.events.length=0;
let legacyPropertyReads=0;
const forbiddenIndexedDB=new Proxy({}, {get(){legacyPropertyReads++;throw Error('legacy IndexedDB must not be inspected');}});
assert.equal(await migrateLegacySaves(multiplayer.profile,{indexedDB:forbiddenIndexedDB,filesystem:multiplayer.filesystem,sync:async()=>{throw Error('unexpected sync');},importReplayName:path=>path}),false);
assert.equal(legacyPropertyReads,0);
assert.deepEqual(multiplayer.filesystem.events,[]);

function buildPlan(multiplayer){
 const args=[resolve(root,'portable/build.mjs'),'--print-plan',...(multiplayer?['--multiplayer']:[])];
 const result=spawnSync(process.execPath,args,{cwd:root,encoding:'utf8'});
 assert.equal(result.status,0,result.stderr);return JSON.parse(result.stdout);
}
const normalPlan=buildPlan(false),multiplayerPlan=buildPlan(true);
assert.equal(normalPlan.variant,'normal');assert(!normalPlan.flags.includes('-DTH_ENABLE_NETPLAY=1'));
assert(!normalPlan.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));
assert.equal(multiplayerPlan.variant,'multiplayer');assert(multiplayerPlan.flags.includes('-DTH_ENABLE_NETPLAY=1'));
assert(multiplayerPlan.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));

console.log(JSON.stringify({passed:true,game:'th10',checks:[
 'runtime variant and C++ identity gate run before FS mount/restore',
 'normal and MP relative/absolute paths normalize to their isolated language namespace',
 'cross-variant and malformed save paths are rejected',
 'normal legacy IndexedDB migration copies JP/CHS files and remains idempotent',
 'MP migration does not touch IndexedDB or the filesystem',
 'normal and multiplayer build plans select distinct compile identities',
]}));
