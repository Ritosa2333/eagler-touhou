// Full current entry/session + real DAT provider. DOM/store/play/input are explicit seams.
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {fakeWindow,envelope,flush} from './environment.mjs';
import {mountLauncherSession} from '../../site/runtime/launcher-session.mjs';
import {createPageLaunchContext} from '../../site/runtime/page-launch-context.mjs';
import {createDatOggProviderFromFiles} from '../../site/runtime/dat-ogg-provider.mjs';
import {fixture,origin,rawBytes,sha} from '../dat-ogg/file-fixture.mjs';
const ownerURL=new URL('../../site/runtime/managed-dat-owner.mjs',import.meta.url);
const owner=fs.existsSync(ownerURL)?await import(ownerURL):{};
const source=fs.readFileSync(new URL('../../site/import-entry.mjs',import.meta.url),'utf8');
function element(){return{hidden:false,disabled:true,textContent:'',events:{},addEventListener(type,fn){this.events[type]=fn;},remove(){this.removed=true;}};}
const search='?runtimeEpoch=41&managedData=1&gameGeneration=package-generation-17';
const storeSearch=search+'&managedPackage=1';
async function setup({query=search,referrer=origin+'/app',callback=null,blocked=null,resourceFetch=null,holdRuntime=false,packageFile=null,packageGeneration=null}={}){
 const fromStore=new URLSearchParams(query).has('managedPackage');
 const f=fixture(),provider=fromStore?null:await createDatOggProviderFromFiles(f.descriptorBytes,f.files,{origin}),env=fakeWindow({search:query,referrer});
 env.win.location={origin,href:origin+'/'+query,search:query};
 const main=element(),loader=element(),file=element(),start=element(),status=element(),retry=element(),hint=element(),meter=element(),local={},calls={data:[],packageFiles:[],storeOpen:0,storeReopen:0,deployment:0,imports:0,keys:[],dispose:0,close:0,installed:null};
 const originalDispose=provider?.dispose.bind(provider);
 // The real provider is frozen; count disposal in a forwarding boundary, not by replacing its methods.
 const wrapped=provider?{...provider,dispose(){calls.dispose++;originalDispose();}}:null;
 env.parent.__eaglerPrepareManagedRuntimeDataV1=async request=>{
  calls.data.push(structuredClone(request));
  if(blocked)await blocked;
  if(callback)return callback(f,request);
  const data={buffer:f.files.get('game-data').slice().buffer,bytes:51,fileId:'game-data'};
  if(!fromStore)return data;
  return{...data,managedPackage:{version:'th128-dat-ogg/1',generation:packageGeneration??request.generation,epoch:request.epoch,
   descriptor:f.descriptorBytes.slice().buffer,async readFile(id){
    calls.packageFiles.push(id);
    const bytes=f.files.get(id).slice(),payload={fileId:id,path:f.descriptor.files[id].target,buffer:bytes.buffer,bytes:bytes.length};
    return packageFile?packageFile(id,payload):payload;
   }}};
 };
 loader.querySelector=selector=>selector==='input'?file:selector==='button'?start:selector==='[data-retry]'?retry:selector==='[data-hint]'?hint:selector==='progress'?meter:status;
 const document={referrer,querySelector:()=>main,createElement:()=>loader,body:{prepend(){}}};env.win.document=document;
 class Store{constructor(){calls.storeOpen++;}close(){calls.close++;}}
 let context=null;
 const runtime={keyboard:value=>calls.keys.push(value),clearInput(){},exit(){}};
 const body=source.replace(/^import .*;\r?\n/gm,'').replace("await import('./play.mjs')","await importPlay()");
 const run=new Function('document','globalThis','location','window','addEventListener','DatPackageArchiveStore','installDatPackage','reopenDatPackage','installResourceFetchProvider','readOnlineDeployment','loadOnlineResources','mountLauncherSession','createPageLaunchContext','installPageLaunchContext','importPlay','bindManagedDatProvider','openManagedDatPackage','return(async()=>{'+body+'})();');
 const bind=async(next,options)=>{
  assert.equal(typeof owner.bindManagedDatProvider,'function','Managed DATA caller module is missing');
  return owner.bindManagedDatProvider(next,{...options,fetchImpl:resourceFetch??(async()=>{throw new Error('Unexpected resource fetch');})});
 };
 const pending=run(document,{indexedDB:{open(){}}},env.win.location,env.win,(type,fn)=>local[type]=fn,Store,async()=>wrapped,async()=>{calls.storeReopen++;return wrapped;},p=>{calls.installed=p;},async()=>{calls.deployment++;return null;},async()=>({provider:wrapped,cached:true}),mountLauncherSession,
 options=>{context=createPageLaunchContext(options);return context;},()=>{},async()=>{calls.imports++;if(!holdRuntime){context.bindRuntime(runtime);context.finish();}return{};},bind,owner.openManagedDatPackage);
 return{...env,f,provider,wrapped,calls,main,status,local,pending,send(data){env.send(data,{origin:new URL(referrer).origin});},get context(){return context;},runtime};
}
async function finished(e){await e.pending;await flush();return e;}
const reply=(e,id)=>e.messages.find(row=>row.data.request===id)?.data;
const events=(e,type)=>e.messages.filter(row=>row.data.event===type);
async function answered(e,id){
 const deadline=Date.now()+1000;
 while(!reply(e,id)&&Date.now()<deadline)await new Promise(resolve=>setTimeout(resolve,1));
 return reply(e,id);
}
test('complete Launcher configure selects its mounted Chinese data, full OGG and independent practice locale',async()=>{
 const e=await finished(await setup({query:storeSearch}));
 const options={limitPresentationTo60:false,touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:175,
  touchFocusMode:'two-finger',doubleTapBombEnabled:true,alwaysHitbox:false,oggDecodeMode:'full',thpracEnabled:true,thpracLocale:'ja-JP'};
 const config={music:'ogg',language:'zh',runtimePack:null,options};
 try{
  e.send(envelope('configure','cfg',config));
  const configured=await answered(e,'cfg');assert.equal(configured?.ok,true,configured?.error);
  e.send(envelope('launch','go'));await flush();
  assert.equal(reply(e,'go')?.ok,true);assert.equal(e.calls.imports,1);assert.equal(events(e,'first-frame').length,0);
  assert.deepEqual(e.context.config,{...config,options:{...options,thpracLocale:'ja'}});
 }finally{e.local.pagehide();}
});
test('complete Launcher configure rejects unsupported or unavailable choices before game import',async()=>{
 const e=await finished(await setup({query:storeSearch}));
 try{
  const failures=[{language:'ja'},{runtimePack:{id:'foreign'}},{options:{alwaysHitbox:true}},
   {options:{oggDecodeMode:'stream'}},{options:{thpracLocale:'fr-FR'}}];
  for(let i=0;i<failures.length;i++){
   e.send(envelope('configure','bad-'+i,{music:'ogg',language:'zh',...failures[i]}));
   assert.equal((await answered(e,'bad-'+i))?.ok,false);assert.equal(e.calls.imports,0);
  }
  e.send(envelope('configure','silent',{music:'none',language:'zh',runtimePack:null,options:{thpracLocale:'en-US'}}));
  assert.equal((await answered(e,'silent'))?.ok,true);assert.equal(e.calls.imports,0);
 }finally{e.local.pagehide();}
});
test('Package Store generation alone starts the current entry without opening or reopening a separate ZIP store',async()=>{
 const e=await finished(await setup({query:storeSearch}));
 try{
  assert.equal(e.calls.storeOpen,0);assert.equal(e.calls.storeReopen,0);assert.equal(e.calls.deployment,0);
  assert.deepEqual(e.calls.data,[{game:'th128',generation:'package-generation-17',epoch:41,packageVersion:'th128-dat-ogg/1'}]);
  assert.deepEqual(e.calls.packageFiles,['language-zh','music-00','music-01','music-02','music-03','music-04','music-05','music-06','music-07','music-08','music-09']);
  assert.equal(events(e,'ready').length,1);assert.equal(e.calls.imports,0);
  e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','go'));await flush();
  assert.equal(reply(e,'go')?.ok,true);assert.equal(e.calls.imports,1);assert.equal(events(e,'first-frame').length,0);
  assert.deepEqual(new Uint8Array(await(await e.calls.installed.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
  assert.deepEqual(new Uint8Array(await(await e.calls.installed.fetch(origin+'/assets/music/first.ogg')).arrayBuffer()),e.f.files.get('music-00'));
  e.send(envelope('exit','bye'));await flush();assert.equal(reply(e,'bye')?.ok,true);
  await assert.rejects(e.calls.installed.fetch(origin+'/assets/runtime/front.anm'),{name:'AbortError'});
 }finally{e.local.pagehide();}
});
test('Package Store cancels a pending resource when the Runtime exits before ready',async()=>{
 let release;const gate=new Promise(resolve=>{release=resolve;});
 const e=await setup({query:storeSearch,packageFile:async(_id,payload)=>{await gate;return payload;}});await flush();
 try{
  assert.deepEqual(e.calls.packageFiles,['language-zh']);assert.equal(events(e,'ready').length,0);
  e.send(envelope('exit','bye'));await finished(e);release();await flush();
  assert.equal(reply(e,'bye')?.ok,true);assert.deepEqual(e.calls.packageFiles,['language-zh']);
  assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);
 }finally{release();e.local.pagehide();}
});
test('Package Store rejects a stale generation and changed bytes, with no local fallback',async()=>{
 for(const [options,error] of [[{packageGeneration:'obsolete'},/identity/],[{packageFile(id,payload){if(id==='music-00')new Uint8Array(payload.buffer)[7]^=1;return payload;}},/SHA-256/]]){
  const e=await finished(await setup({query:storeSearch,...options}));
  try{
   assert.equal(e.calls.storeOpen,0);assert.equal(e.calls.storeReopen,0);assert.equal(e.calls.deployment,0);
   assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);assert.match(events(e,'error')[0]?.data.message??'',error);
   if(options.packageGeneration)assert.deepEqual(e.calls.packageFiles,[]);
  }finally{e.local.pagehide();}
 }
});
test('current entry actually requests the exact managed identity before ready and uses the real DAT consumer',async()=>{
 const e=await finished(await setup());
 try{
  assert.deepEqual(e.calls.data,[{game:'th128',generation:'package-generation-17',epoch:41}]);
  assert.equal(events(e,'ready').length,1);assert.equal(e.calls.imports,0);
  assert.equal(events(e,'runtime-info')[0].data.dataMode,'managed-original-DAT-with-verified-package');
  assert.deepEqual(new Uint8Array(await(await e.provider.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
  e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','go'));await flush();
  assert.equal(reply(e,'go').ok,true);assert.equal(e.calls.imports,1);assert.equal(events(e,'first-frame').length,0);
 }finally{e.local.pagehide();}
});
test('parent DATA callback completion gates ready instead of being treated as a game first frame',async()=>{
 let release;const gate=new Promise(resolve=>{release=resolve;});
 const e=await setup({blocked:gate});await flush();
 assert.equal(e.calls.data.length,1);assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);
 release();await finished(e);assert.equal(events(e,'ready').length,1);assert.equal(events(e,'first-frame').length,0);e.local.pagehide();
});
test('exit during a pending DATA callback suppresses late ready and disposes its resource candidate',async()=>{
 let release;const gate=new Promise(resolve=>{release=resolve;});const e=await setup({blocked:gate});await flush();
 e.send(envelope('exit','bye'));await flush();release();await finished(e);
 assert.equal(reply(e,'bye').ok,true);assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);assert(e.calls.dispose>=1);
});
for(const [name,callback]of [
 ['wrong file ID',f=>({buffer:f.files.get('game-data').slice().buffer,bytes:51,fileId:'foreign'})],
 ['incorrect byte count',f=>({buffer:f.files.get('game-data').slice().buffer,bytes:50,fileId:'game-data'})],
 ['altered SHA',f=>{const b=f.files.get('game-data').slice();b[0]^=1;return{buffer:b.buffer,bytes:51,fileId:'game-data'};}],
 ['ZIP renamed DATA',()=>({buffer:new Uint8Array([80,75,3,4,...new Uint8Array(47)]).buffer,bytes:51,fileId:'game-data'})]
])test('current entry refuses '+name+' without ready or game import',async()=>{
 const e=await finished(await setup({callback}));
 assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);assert(e.calls.dispose>=1);assert.match(e.status.textContent,/失败/);e.local.pagehide();
});
for(const [name,query,referrer]of [
 ['another origin',search,'https://other.invalid/app'],
 ['missing generation','?runtimeEpoch=41&managedData=1',origin+'/app'],
 ['duplicate generation',search+'&gameGeneration=other',origin+'/app'],
 ['invalid mode','?runtimeEpoch=41&managedData=2&gameGeneration=one',origin+'/app'],
 ['duplicate mode',search+'&managedData=1',origin+'/app']
])test('managed current entry rejects '+name+' rather than falling back to the saved package',async()=>{
 const e=await finished(await setup({query,referrer}));
 assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);assert.equal(e.calls.data.length,0);e.local.pagehide();
});
test('generation URL change while DATA waits cannot revive the obsolete entry',async()=>{
 let release;const gate=new Promise(resolve=>{release=resolve;});const e=await setup({blocked:gate});await flush();
 e.win.location.search=search.replace('generation-17','generation-18');e.win.location.href=origin+'/'+e.win.location.search;
 release();await finished(e);assert.equal(events(e,'ready').length,0);assert.equal(e.calls.imports,0);assert(e.calls.dispose>=1);e.local.pagehide();
});
test('only matching mounted file receipts accept resource messages, including same-origin Blob URLs',async()=>{
 const f=fixture(),music=f.files.get('music-00'),calls=[];
 const e=await finished(await setup({resourceFetch:async(url,init)=>{calls.push([url,init]);return new Response(music);}}));
 e.send(envelope('resources','rs',{resources:[{url:'blob:'+origin+'/owned-music',path:'/music/th128_00.ogg',bytes:8,sha256:sha(music)}]}));await new Promise(resolve=>setTimeout(resolve,30));
 assert.equal(reply(e,'rs').ok,true);assert.equal(calls.length,1);assert.equal(calls[0][1].redirect,'error');
 assert.deepEqual(new Uint8Array(await(await e.provider.fetch(origin+'/assets/music/first.ogg')).arrayBuffer()),music);e.local.pagehide();
});
test('conflicting resources fail while the same live DAT provider stays readable',async()=>{
 const e=await finished(await setup({resourceFetch:async()=>new Response(new Uint8Array(8))}));
 e.send(envelope('resources','bad',{resources:[{url:origin+'/music.ogg',path:'/music/th128_00.ogg',bytes:8}]}));await new Promise(resolve=>setTimeout(resolve,30));
 assert.equal(reply(e,'bad').ok,false);assert.deepEqual(new Uint8Array(await(await e.provider.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
 e.send(envelope('resources','unknown',{resources:[{url:origin+'/a',path:'/core.mjs',bytes:1}]}));await flush();assert.equal(reply(e,'unknown').ok,false);e.local.pagehide();
});
test('a running managed duplicate-resource check neither blocks keyboard nor emits late success after exit',async()=>{
 let release,entered;const gate=new Promise(resolve=>{release=resolve;}),seen=new Promise(resolve=>{entered=resolve;});
 const e=await finished(await setup({resourceFetch:async()=>{entered();await gate;return new Response(new Uint8Array([79,103,103,83,0,0,0,0]));}}));
 e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','go'));await flush();assert.equal(reply(e,'go').ok,true);
 e.send(envelope('resources','pending',{resources:[{url:origin+'/music.ogg',path:'/music/th128_00.ogg',bytes:8}]}));
 // A missing live resource hook fails directly instead of leaving a test pending.
 await Promise.race([seen,new Promise(resolve=>setTimeout(resolve,30))]);assert.equal(reply(e,'pending'),undefined);
 e.send(envelope('keyboard','key',{code:'KeyZ',down:true}));await flush();assert.equal(reply(e,'key').ok,true);assert.equal(e.calls.keys.length,1);
 e.send(envelope('exit','exit'));await flush();release();await flush();await flush();
 assert.equal(reply(e,'exit').ok,true);assert.equal(e.messages.some(row=>row.data.request==='pending'&&row.data.ok),false);
});
test('ordinary installed-package entry remains independent of a managed callback',async()=>{
 const e=await finished(await setup({query:'?runtimeEpoch=41'}));
 assert.equal(e.calls.data.length,0);assert.equal(events(e,'ready').length,1);e.local.pagehide();
});
