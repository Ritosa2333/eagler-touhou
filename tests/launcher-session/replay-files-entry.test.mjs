// Real current entry/session/store logic, synthetic transactional IDB and DOM/play boundaries.
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {mountLauncherSession} from '../../site/runtime/launcher-session.mjs';
import {createPageLaunchContext} from '../../site/runtime/page-launch-context.mjs';
import {fakeWindow,envelope,flush} from './environment.mjs';
import {createReplayIDB,replayFile} from './replay-idb.mjs';
const bridgeURL=new URL('../../site/runtime/replay-file-bridge.mjs',import.meta.url),bridge=fs.existsSync(bridgeURL)?await import(bridgeURL):{};
const source=fs.readFileSync(new URL('../../site/import-entry.mjs',import.meta.url),'utf8');
const element=()=>({hidden:false,disabled:false,events:{},textContent:'',addEventListener(t,f){this.events[t]=f;},remove(){}});
async function setup({idb=createReplayIDB(),holdRuntime=false}={}){
 const env=fakeWindow(),calls={imports:0},main=element(),loader=element(),status=element(),file=element(),start=element(),retry=element(),hint=element(),meter=element(),handlers={};
 loader.querySelector=s=>s==='input'?file:s==='button'?start:s==='[data-retry]'?retry:s==='[data-hint]'?hint:s==='progress'?meter:status;
 const document={...env.win.document,querySelector:()=>main,createElement:()=>loader,body:{prepend(){}}},provider={dispose(){},fetch:async()=>new Response(JSON.stringify({variants:{zh:{}}}))};
 let context;const runtime={keyboard(){},clearInput(){},exit(){}};class ResourceStore{close(){}}
 const text=source.replace(/^import .*;\r?\n/gm,'').replace("await import('./play.mjs')","await importPlay()");
 const run=new Function('document','globalThis','location','window','addEventListener','DatPackageArchiveStore','installDatPackage','reopenDatPackage','installResourceFetchProvider','readOnlineDeployment','loadOnlineResources','mountLauncherSession','createPageLaunchContext','installPageLaunchContext','bindManagedDatProvider','createReplayFileBridge','importPlay','return(async()=>{'+text+'})();');
 const makeBridge=options=>{assert.equal(typeof bridge.createReplayFileBridge,'function','Replay current entry bridge missing');return bridge.createReplayFileBridge({...options,indexedDB:idb});};
 await run(document,{indexedDB:idb},env.win.location,env.win,(t,f)=>handlers[t]=f,ResourceStore,async()=>provider,async()=>provider,()=>{},async()=>null,async()=>({provider,cached:true}),mountLauncherSession,
 options=>{context=createPageLaunchContext(options);return context;},()=>{},async()=>null,makeBridge,async()=>{calls.imports++;if(!holdRuntime){context.bindRuntime(runtime);context.finish();}});
 return{...env,idb,calls,handlers,get context(){return context;},runtime};
}
const response=(e,id)=>e.messages.find(row=>row.data.request===id)?.data;
async function command(e,id,command,fields={}){e.send(envelope(command,id,fields));for(let i=0;i<100;i++){await flush();if(response(e,id))return response(e,id);}throw new Error('File command did not settle: '+id);}
test('current list/read uses upstream relative file shape and exact saved original RPY bytes without starting play',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),file=replayFile();
 // Seed through the existing production import consumer; no new mutator is needed for this control.
 const imported=await store.importOriginal(file,{fileName:'seed.rpy'});assert.equal(imported.slot,0);store.close();
 const e=await setup({idb});
 try{
  const listed=await command(e,'list','list');
  assert.equal(listed.ok,true);assert.deepEqual(listed.files,[{path:'replay/th128_00.rpy',size:file.length,sha256:imported.digest}]);
  const read=await command(e,'read','read',{path:'replay/th128_00.rpy'});assert.equal(read.ok,true);assert.deepEqual(read.bytes,[...file]);assert.equal(e.calls.imports,0);
  read.bytes[0]=0;assert.deepEqual((await command(e,'again','read',{path:'replay/th128_00.rpy'})).bytes,[...file]);
 }finally{e.handlers.pagehide();}
});
test('listing an absent Replay DB returns no files and does not create settings, defaults or a replay database',async()=>{
 const e=await setup();try{const listed=await command(e,'list','list');assert.equal(listed.ok,true);assert.deepEqual(listed.files,[]);assert.equal(e.idb.counts.opens,0);assert.equal(e.idb._dbs.size,0);}finally{e.handlers.pagehide();}
});
test('write imports exactly the requested slot, sync observes complete bytes and remove retires only that file',async()=>{
 const e=await setup(),file=replayFile();try{
  const written=await command(e,'write','write',{path:'replay/th128_07.rpy',bytes:[...file]});assert.equal(written.ok,true);assert.equal(written.path,'replay/th128_07.rpy');assert.equal(written.strictFullReadback,true);
  assert.deepEqual((await command(e,'list','list')).files.map(row=>row.path),['replay/th128_07.rpy']);
  const sync=await command(e,'sync','sync');assert.equal(sync.ok,true);assert.equal(sync.verifiedFiles,1);assert.equal(sync.pendingWrites,0);
  assert.equal((await command(e,'remove','remove',{path:'replay/th128_07.rpy'})).ok,true);
  assert.deepEqual((await command(e,'empty','list')).files,[]);assert.equal(e.idb.peek(0),undefined);
 }finally{e.handlers.pagehide();}
});
test('canonical unknown, traversal, score and JSON-config paths fail rather than becoming fabricated files',async()=>{
 const e=await setup();try{
  for(const path of ['/replay/th128_00.rpy','replay/../a.rpy','replay/th128_25.rpy','scoreth128.dat','th128.cfg','replay/slot-00.rpy']){
   const row=await command(e,path,'read',{path});assert.equal(row.ok,false,path);
  }
  assert.equal(e.idb.counts.writes,0);assert.equal(e.calls.imports,0);
 }finally{e.handlers.pagehide();}
});
test('running gameplay permits validated read/sync but refuses external Replay mutation',async()=>{
 const e=await setup();try{
  await command(e,'cfg','configure',{music:'ogg'});await command(e,'launch','launch');
  assert.equal((await command(e,'write','write',{path:'replay/th128_00.rpy',bytes:[...replayFile()]})).ok,false);
  assert.equal((await command(e,'remove','remove',{path:'replay/th128_00.rpy'})).ok,false);
  assert.equal((await command(e,'list','list')).ok,true);assert.equal((await command(e,'sync','sync')).ok,true);
  assert.equal(e.idb.counts.writes,0);
 }finally{e.handlers.pagehide();}
});
test('exit during delayed file validation has no late ACK or write into the retired owner',async()=>{
 let current=true;
 const idb=createReplayIDB({beforeGet(){current=false;}});
 const e=await setup({idb});e.send(envelope('write','write',{path:'replay/th128_02.rpy',bytes:[...replayFile()]}));
 e.send(envelope('exit','exit'));await flush();await flush();
 assert.equal(response(e,'exit').ok,true);assert.equal(e.messages.some(row=>row.data.request==='write'&&row.data.ok),false);assert.equal(e.idb.counts.writes,0);e.handlers.pagehide();
});

async function nativeRecordFixture(){
 const {profileFixture}=await import('../native-score/score-fixture.mjs'),profile=profileFixture();
 return{schema:'th128-owned-native-title-record/1',revision:3,provenance:'ordinary-native-postrun',tainted:false,
 profile,sha256:Buffer.from(await crypto.subtle.digest('SHA-256',profile)).toString('hex'),native:{route:0,rank:0},runId:'explicit-synthetic-score-export-owner'};
}
test('current file list/read exports a real T821 score from the actual validated native store without writing it',async()=>{
 const idb=createReplayIDB(),record=await nativeRecordFixture();idb.seedNativeRecord(record);
 const e=await setup({idb});try{
  const list=await command(e,'scorelist','list'),row=list.files.find(row=>row.path==='scoreth128.dat');
  assert(row,'Native score export path missing from current caller');assert(row.size>24);
  const file=await command(e,'score','read',{path:'scoreth128.dat'});assert.equal(file.ok,true);assert.deepEqual(file.bytes.slice(0,4),[84,56,50,49]);
  const {decodeNativeScoreFile}=await import('../../site/runtime/native-score-file.mjs'),profile=decodeNativeScoreFile(Uint8Array.from(file.bytes));
  assert.equal(profile.length,52232);
  for(const offset of [20,1664,13152+100,14220+200])assert.equal(profile[offset],record.profile[offset]);
  assert.deepEqual(idb._dbs.get('th128-owned-native-title-records-v1').stores.get('records').get('native-profile'),record);
  assert.equal(idb.counts.writes,0);assert.equal(e.calls.imports,0);
 }finally{e.handlers.pagehide();}
});
test('absent native score is errno44 and does not create a default native profile or name DB',async()=>{
 const e=await setup();try{
  const row=await command(e,'missing','read',{path:'scoreth128.dat'});assert.equal(row.ok,false);assert.equal(row.errno,44);
  assert.equal(e.idb._dbs.size,0);assert.equal(e.idb.counts.opens,0);
 }finally{e.handlers.pagehide();}
});
test('external native score writes/removals and fabricated provenance remain refused',async()=>{
 const e=await setup();try{
  assert.equal((await command(e,'wscore','write',{path:'scoreth128.dat',bytes:[84,56,50,49]})).ok,false);
  assert.equal((await command(e,'dscore','remove',{path:'scoreth128.dat'})).ok,false);assert.equal(e.idb.counts.writes,0);
 }finally{e.handlers.pagehide();}
 const idb=createReplayIDB(),bad=await nativeRecordFixture();bad.provenance='file-imported-as-world';idb.seedNativeRecord(bad);
 const x=await setup({idb});try{assert.equal((await command(x,'badscore','read',{path:'scoreth128.dat'})).ok,false);assert.equal(idb.counts.writes,0);}finally{x.handlers.pagehide();}
});
