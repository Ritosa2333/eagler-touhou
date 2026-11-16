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
async function setup({query=search,referrer=origin+'/app',callback=null,blocked=null,resourceFetch=null,holdRuntime=false}={}){
 const f=fixture(),provider=await createDatOggProviderFromFiles(f.descriptorBytes,f.files,{origin}),env=fakeWindow({search:query,referrer});
 env.win.location={origin,href:origin+'/'+query,search:query};
 const main=element(),loader=element(),file=element(),start=element(),status=element(),retry=element(),hint=element(),meter=element(),local={},calls={data:[],imports:0,keys:[],dispose:0,close:0,installed:null};
 const originalDispose=provider.dispose.bind(provider);
 // The real provider is frozen; count disposal in a forwarding boundary, not by replacing its methods.
 const wrapped={...provider,dispose(){calls.dispose++;originalDispose();}};
 env.parent.__eaglerPrepareManagedRuntimeDataV1=async request=>{
  calls.data.push(structuredClone(request));
  if(blocked)await blocked;
  return callback?callback(f,request):{buffer:f.files.get('game-data').slice().buffer,bytes:51,fileId:'game-data'};
 };
 loader.querySelector=selector=>selector==='input'?file:selector==='button'?start:selector==='[data-retry]'?retry:selector==='[data-hint]'?hint:selector==='progress'?meter:status;
 const document={referrer,querySelector:()=>main,createElement:()=>loader,body:{prepend(){}}};env.win.document=document;
 class Store{close(){calls.close++;}}
 let context=null;
 const runtime={keyboard:value=>calls.keys.push(value),clearInput(){},exit(){}};
 const body=source.replace(/^import .*;\r?\n/gm,'').replace("await import('./play.mjs')","await importPlay()");
 const run=new Function('document','globalThis','location','window','addEventListener','DatPackageArchiveStore','installDatPackage','reopenDatPackage','installResourceFetchProvider','readOnlineDeployment','loadOnlineResources','mountLauncherSession','createPageLaunchContext','installPageLaunchContext','importPlay','bindManagedDatProvider','return(async()=>{'+body+'})();');
 const bind=async(next,options)=>{
  assert.equal(typeof owner.bindManagedDatProvider,'function','Managed DATA caller module is missing');
  return owner.bindManagedDatProvider(next,{...options,fetchImpl:resourceFetch??(async()=>{throw new Error('Unexpected resource fetch');})});
 };
 const pending=run(document,{indexedDB:{open(){}}},env.win.location,env.win,(type,fn)=>local[type]=fn,Store,async()=>wrapped,async()=>wrapped,p=>{calls.installed=p;},async()=>null,async()=>({provider:wrapped,cached:true}),mountLauncherSession,
 options=>{context=createPageLaunchContext(options);return context;},()=>{},async()=>{calls.imports++;if(!holdRuntime){context.bindRuntime(runtime);context.finish();}return{};},bind);
 return{...env,f,provider,wrapped,calls,main,status,local,pending,send(data){env.send(data,{origin:new URL(referrer).origin});},get context(){return context;},runtime};
}
async function finished(e){await e.pending;await flush();return e;}
const reply=(e,id)=>e.messages.find(row=>row.data.request===id)?.data;
const events=(e,type)=>e.messages.filter(row=>row.data.event===type);
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
