import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {fakeWindow,envelope,flush} from './environment.mjs';
import {mountLauncherSession} from '../../site/runtime/launcher-session.mjs';
const url=new URL('../../site/runtime/page-launch-context.mjs',import.meta.url),contextModule=fs.existsSync(url)?await import(url):{};
const source=fs.readFileSync(new URL('../../site/import-entry.mjs',import.meta.url),'utf8');
function element(){return{hidden:false,disabled:true,textContent:'',events:{},files:null,addEventListener(type,fn){this.events[type]=fn;},remove(){this.removed=true;}};}
async function setup({heldRuntime=false,importError=null,stopError=null}={}){
 assert.equal(typeof contextModule.createPageLaunchContext,'function','Hosted current entry lifecycle missing');
 const env=fakeWindow(),main=element(),loader=element(),file=element(),start=element(),status=element(),retry=element(),hint=element(),progress=element(),localHandlers={},calls={imports:0,provider:0,dispose:0,stops:0,close:0,keys:[],ready:0};
 retry.hidden=true;loader.querySelector=s=>s==='input'?file:s==='button'?start:s==='[data-retry]'?retry:s==='[data-hint]'?hint:s==='progress'?progress:status;
 const document={...env.win.document,querySelector:()=>main,createElement:()=>loader,body:{prepend(){}}};
 let activeContext=null;const make=options=>{activeContext=contextModule.createPageLaunchContext(options);return activeContext;};
 const runtime={keyboard:value=>calls.keys.push(value),clearInput(){},exit(){calls.stops++;if(stopError)throw stopError;},applyOptions(){}};
 const provider={revision:'release',fetch:async(url,init)=>{assert.equal(url,'https://game.example/assets/catalog.json');assert(init.signal instanceof AbortSignal);return new Response(JSON.stringify({variants:{zh:{},ja:{}}}));},dispose(){calls.dispose++;}};
 class Store{close(){calls.close++;}}
 const body=source.replace(/^import .*;\r?\n/gm,'').replace("await import('./play.mjs')","await importPlay()");
 const fn=new Function('document','globalThis','location','window','addEventListener','DatPackageArchiveStore','installDatPackage','reopenDatPackage','installResourceFetchProvider','readOnlineDeployment','loadOnlineResources','mountLauncherSession','createPageLaunchContext','installPageLaunchContext','importPlay','return(async()=>{'+body+'})();');
 await fn(document,{indexedDB:{open(){}}},env.win.location,env.win,(t,f)=>localHandlers[t]=f,Store,async()=>provider,async()=>provider,()=>{calls.provider++;},async()=>null,async()=>({provider,cached:true}),mountLauncherSession,make,()=>{},
 async()=>{calls.imports++;if(importError)throw importError;if(!heldRuntime){activeContext.bindRuntime(runtime);activeContext.finish();}return {};});
 return{...env,calls,main,file,start,status,retry,loader,localHandlers,runtime,get context(){return activeContext;}};
}
test('hosted entry prepares a saved authenticated package but does not start before configure and launch',async()=>{
 const e=await setup();assert.equal(e.calls.imports,0);assert.equal(e.messages.filter(x=>x.data.event==='ready').length,1);
 e.send(envelope('configure','cfg',{music:'ogg',language:'zh'}));await flush();e.send(envelope('launch','launch'));await flush();
 assert.equal(e.calls.imports,1);assert.equal(e.calls.provider,1);assert.equal(e.main.hidden,false);assert.equal(e.messages.find(x=>x.data.request==='launch').data.ok,true);
});
test('unsupported raw resource maps and unsupported music do not boot the game or return hollow success',async()=>{
 const e=await setup();e.send(envelope('configure','bad',{music:'midi',language:'zh'}));await flush();
 assert.equal(e.messages.find(x=>x.data.request==='bad').data.ok,false);e.send(envelope('resources','raw',{resources:[{path:'/th128.dat',url:'blob:ignored'}]}));await flush();
 assert.equal(e.messages.find(x=>x.data.request==='raw').data.ok,false);assert.equal(e.calls.imports,0);
});
test('launch ACK waits for actual page readiness and first-frame is not emitted by import',async()=>{
 const e=await setup({heldRuntime:true});e.send(envelope('configure','cfg',{music:'ogg',language:'zh'}));await flush();e.send(envelope('launch','launch',{launchSerial:9}));await flush();
 assert.equal(e.calls.imports,1);assert.equal(e.messages.some(x=>x.data.request==='launch'),false);e.context.bindRuntime(e.runtime);e.context.finish();await flush();
 assert.equal(e.messages.find(x=>x.data.request==='launch').data.ok,true);assert.equal(e.messages.some(x=>x.data.event==='first-frame'),false);e.context.notePresented(true);
 assert.equal(e.messages.find(x=>x.data.event==='first-frame').data.launchSerial,9);
});
test('parent exit during startup cancels the installed owner and suppresses late game readiness',async()=>{
 const e=await setup({heldRuntime:true});e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','launch'));await flush();e.send(envelope('exit','exit'));await flush();
 assert.equal(e.context.signal.aborted,true);e.context.bindRuntime(e.runtime);e.context.finish();await flush();assert.equal(e.calls.stops,1);assert.equal(e.main.hidden,true);
 assert.equal(e.messages.some(x=>x.data.request==='launch'&&x.data.ok),false);assert(e.calls.dispose>=1);
});

test('retry after failed module initialization never ACKs a game that was never ready',async()=>{
 const e=await setup({importError:Error('module failed')});e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','first'));await flush();
 assert.equal(e.messages.find(x=>x.data.request==='first').data.ok,false);e.send(envelope('launch','retry'));await flush();
 assert.equal(e.messages.find(x=>x.data.request==='retry').data.ok,false);assert.equal(e.calls.imports,1);assert.equal(e.main.hidden,true);
});
test('a throwing runtime cleanup still closes the resource owner and store and returns a failed exit ACK',async()=>{
 const e=await setup({stopError:Error('native cleanup failed')});e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','first'));await flush();
 const closedBefore=e.calls.close;e.send(envelope('exit','exit'));await flush();assert.equal(e.messages.find(x=>x.data.request==='exit').data.ok,false);
 assert(e.calls.dispose>=1);assert(e.calls.close>closedBefore);
});
test('pagehide cleanup preserves provider and store cleanup even if the runtime exit hook throws',async()=>{
 const e=await setup({stopError:Error('runtime cleanup failure')});e.send(envelope('configure','cfg',{music:'ogg'}));await flush();e.send(envelope('launch','first'));await flush();
 const closedBefore=e.calls.close;try{e.localHandlers.pagehide();}catch{}
 await flush();assert(e.calls.dispose>=1);assert(e.calls.close>closedBefore);
});
test('current entry accepts no BGM mode and passes it to the real launch context without early ready ACK',async()=>{
 const e=await setup({heldRuntime:true});
 try{
  e.send(envelope('configure','silent',{music:'none',language:'zh'}));await flush();assert.equal(e.messages.find(x=>x.data.request==='silent')?.data.ok,true,'music:none was rejected by current caller');
  e.send(envelope('launch','silent-launch'));await flush();assert.equal(e.context.config.music,'none');assert.equal(e.messages.some(x=>x.data.request==='silent-launch'),false);
  e.context.bindRuntime(e.runtime);e.context.finish();await flush();assert.equal(e.messages.find(x=>x.data.request==='silent-launch')?.data.ok,true);assert.equal(e.messages.some(x=>x.data.event==='first-frame'),false);
 }finally{e.localHandlers.pagehide();}
});
