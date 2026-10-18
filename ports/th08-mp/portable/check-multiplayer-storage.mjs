import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {initializeSaveStorage,migrateLegacySaves} from '../th08_web/sdl-runtime/save-storage.mjs';

const root=resolve(fileURLToPath(new URL('../',import.meta.url)));
const shell=readFileSync(resolve(root,'th08_web/sdl-runtime/shell.mjs'),'utf8');
const fileHost=readFileSync(resolve(root,'th08_web/cpp/sdl/FileHost.cpp'),'utf8');
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

async function initialize(runtimeVariant,compiledVariant='normal',game='th08'){
 const filesystem=mockFilesystem(),events=filesystem.events,identityCalls=[];
 const profile=await initializeSaveStorage({
  game,runtimeVariant,setCompiledVariant:value=>{identityCalls.push(value);return (value===1)===(compiledVariant==='multiplayer')?1:0;},
  filesystem,idbfs:{kind:'IDBFS'},sync:async populate=>events.push(['sync',populate]),
  beforeMount:()=>events.push(['beforeMount']),
 });
 return {profile,filesystem,identityCalls};
}

// URL selection, C++ compile-identity guard, mount, and restore are one callable
// path used by shell.mjs. A mismatch must fail before even the before-mount hook.
const normal=await initialize(undefined);
assert.equal(normal.profile.variant,'normal');
assert.equal(normal.profile.namespace,'/savesth08');
assert.equal(normal.profile.root(),'/savesth08');
assert.deepEqual(normal.identityCalls,[0]);
assert.deepEqual(normal.filesystem.events,[
 ['beforeMount'],['mkdir','/savesth08'],['mount','/savesth08'],['sync',true],
]);
const multiplayer=await initialize('multiplayer','multiplayer');
assert.equal(multiplayer.profile.namespace,'/savesth08-multiplayer');
assert.deepEqual(multiplayer.identityCalls,[1]);
assert.deepEqual(multiplayer.filesystem.events,[
 ['beforeMount'],['mkdir','/savesth08-multiplayer'],['mount','/savesth08-multiplayer'],['sync',true],
]);

for(const [variant,compiled] of [['preview','normal'],['multiplayer','normal'],['normal','multiplayer']]){
 const filesystem=mockFilesystem();let beforeMountCalls=0,syncCalls=0;
 await assert.rejects(()=>initializeSaveStorage({
  game:'th08',runtimeVariant:variant,setCompiledVariant:value=>(value===(compiled==='multiplayer'?1:0)?1:0),
  filesystem,idbfs:{},sync:async()=>{syncCalls++;},beforeMount:()=>{beforeMountCalls++;},
 }),variant==='preview'?/Unsupported runtimeVariant/:/Runtime variant mismatch/);
 assert.deepEqual(filesystem.events,[],'variant rejection must precede all FS calls');
 assert.equal(beforeMountCalls,0);
 assert.equal(syncCalls,0);
}

assert.equal(normal.profile.relativeSave('score.dat'),'score.dat');
assert.equal(normal.profile.relativeSave('/savesth08/replay/TH8_01.RPY'),'replay/th8_01.rpy');
assert.equal(multiplayer.profile.relativeSave('replay/th8_02.rpyx'),'replay/th8_02.rpyx');
assert.equal(multiplayer.profile.relativeSave('/savesth08-multiplayer/score.dat'),'score.dat');
assert.throws(()=>normal.profile.relativeSave('/savesth08-multiplayer/score.dat'),/different runtime variant/);
assert.throws(()=>multiplayer.profile.relativeSave('/savesth08/replay/th8_01.rpy'),/different runtime variant/);
assert.throws(()=>normal.profile.relativeSave('../score.dat'),/Invalid save path/);

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

// Run the same legacy migration function as the shell with mock IDB/FS. Normal
// migration still copies valid records, preserves existing files, and marks once.
normal.filesystem.events.length=0;
normal.filesystem.files.set('/savesth08/th08.cfg',new Uint8Array([9]));
const legacy08=mockIndexedDB(new Map([['th08-native-1.00d',[
 ['score.dat',new Uint8Array([1,2])],
 ['th08.cfg',new Uint8Array([3])],
 ['replay/th8_01.rpy',new Uint8Array([4])],
 ['/savesth08-multiplayer/score.dat',new Uint8Array([5])],
]] ]));
let imported08=0,synced08=[];
assert.equal(await migrateLegacySaves(normal.profile,{
 indexedDB:legacy08,filesystem:normal.filesystem,sync:async populate=>synced08.push(populate),
 importReplayName:path=>{imported08++;return path;},
}),true);
assert.deepEqual(legacy08.calls.opened,['th08-native-1.00d']);
assert.deepEqual([...normal.filesystem.files.get('/savesth08/score.dat')],[1,2]);
assert.deepEqual([...normal.filesystem.files.get('/savesth08/th08.cfg')],[9]);
assert.deepEqual([...normal.filesystem.files.get('/savesth08/replay/th8_01.rpy')],[4]);
assert.equal(normal.filesystem.files.has('/savesth08-multiplayer/score.dat'),false);
assert.equal(imported08,3);
assert.equal(normal.filesystem.files.has('/savesth08/.migration-v3'),true);
assert.deepEqual(synced08,[false]);
const legacyReadsBeforeRepeat=legacy08.calls.databases;
assert.equal(await migrateLegacySaves(normal.profile,{indexedDB:legacy08,filesystem:normal.filesystem,sync:async()=>{},importReplayName:path=>path}),false);
assert.equal(legacy08.calls.databases,legacyReadsBeforeRepeat,'completed migration is idempotent');

// MP must exit before touching either IndexedDB or the mounted filesystem.
multiplayer.filesystem.events.length=0;
let legacyPropertyReads=0;
const forbiddenIndexedDB=new Proxy({}, {get(){legacyPropertyReads++;throw Error('legacy IndexedDB must not be inspected');}});
assert.equal(await migrateLegacySaves(multiplayer.profile,{indexedDB:forbiddenIndexedDB,filesystem:multiplayer.filesystem,sync:async()=>{throw Error('unexpected sync');},importReplayName:path=>path}),false);
assert.equal(legacyPropertyReads,0);
assert.deepEqual(multiplayer.filesystem.events,[]);

function buildPlan(multiplayer){
 const args=[resolve(root,'portable/build.mjs'),'--th08','--print-plan',...(multiplayer?['--multiplayer']:[])];
 const result=spawnSync(process.execPath,args,{cwd:root,encoding:'utf8'});
 assert.equal(result.status,0,result.stderr);return JSON.parse(result.stdout);
}
const normalPlan=buildPlan(false),multiplayerPlan=buildPlan(true);
assert.equal(normalPlan.variant,'normal');assert(!normalPlan.flags.includes('-DTH_ENABLE_NETPLAY=1'));
assert(!normalPlan.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));
assert.equal(multiplayerPlan.variant,'multiplayer');assert(multiplayerPlan.flags.includes('-DTH_ENABLE_NETPLAY=1'));
assert(multiplayerPlan.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));

console.log(JSON.stringify({passed:true,game:'th08',checks:[
 'runtime variant and C++ identity gate run before FS mount/restore',
 'normal and MP relative/absolute paths normalize to their isolated namespace',
 'cross-variant and malformed save paths are rejected',
 'normal legacy IndexedDB migration copies only valid files and remains idempotent',
 'MP migration does not touch IndexedDB or the filesystem',
 'normal and multiplayer build plans select distinct compile identities',
]}));
