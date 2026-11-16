import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import {fileURLToPath} from 'node:url';
const source=fs.readFileSync(fileURLToPath(new URL('../../site/import-entry.mjs',import.meta.url)),'utf8');
function element(){return{hidden:false,disabled:false,files:null,events:{},textContent:'',addEventListener(t,fn){this.events[t]=fn;},remove(){this.removed=true;}};}
function harness({online=false,saved=null,loadError=null,loadFirstError=null,startError=null,storeAvailable=true,waitOnline=null,discoverFirstError=null,waitDiscover=null}={}){
 const main=element(),loader=element(),file=element(),start=element(),status=element(),retry=element(),note=element(),meter=element(),handlers={};
 loader.querySelector=s=>s==='input'?file:s==='button'?start:s==='[data-retry]'?retry:s==='[data-hint]'?note:s==='progress'?meter:status;
 file.disabled=true;start.disabled=true;retry.hidden=true;meter.hidden=true;
 const document={querySelector:()=>main,createElement:()=>loader,body:{prepend(){}}},calls={store:0,close:0,localInstall:0,online:0,discover:0,provider:0,imports:0,dispose:0};
 class Store{constructor(){calls.store++;}close(){calls.close++;}}
 const provider={dispose(){calls.dispose++;}},manifest=online?{schema:'th128-online-deployment/1',game:'th128',resource:{url:'/games/th128/file.zip',bytes:6,sha256:'f'.repeat(64),revision:'r1'}}:null;
 const body=source.replace(/^import .*;\r?\n/gm,'').replace("await import('./play.mjs')","await importPlay()");
 const fn=new Function('document','globalThis','location','addEventListener','DatPackageArchiveStore','installDatPackage','reopenDatPackage','installResourceFetchProvider','readOnlineDeployment','loadOnlineResources','importPlay','return(async()=>{'+body+'})();');
 const ready=fn(document,{indexedDB:storeAvailable?{open(){}}:null},{href:'https://game.example/',origin:'https://game.example'},(t,fn)=>handlers[t]=fn,Store,
 async()=>{calls.localInstall++;return provider;},async()=>saved,
 p=>{assert.equal(p,provider);calls.provider++;},
 async()=>{calls.discover++;if(waitDiscover)await waitDiscover();if(discoverFirstError&&calls.discover===1)throw discoverFirstError;return manifest;},
 async(_manifest,opts)=>{calls.online++;if(waitOnline)await waitOnline();if(loadFirstError&&calls.online===1)throw loadFirstError;if(loadError)throw loadError;opts.onProgress({phase:'download',loaded:3,total:6});return {provider,source:'download',cached:storeAvailable};},
 async()=>{calls.imports++;if(startError)throw startError;});
 return{ready,main,loader,file,start,status,retry,note,meter,calls,handlers,provider};
}
test('public mode automatically downloads and imports the game without selecting a ZIP',async()=>{
 const h=harness({online:true});await h.ready;
 assert.equal(h.calls.discover,1);assert.equal(h.calls.online,1);assert.equal(h.calls.provider,1);assert.equal(h.calls.imports,1);
 assert.equal(h.file.hidden,true);assert.equal(h.main.hidden,false);assert.equal(h.loader.removed,true);
});
test('returning public visitors use the resource loader cache path without manual start',async()=>{
 const h=harness({online:true,saved:{revision:'r1'}});await h.ready;assert.equal(h.calls.online,1);assert.equal(h.calls.localInstall,0);assert.equal(h.calls.imports,1);
});
test('download failure offers retry and never falls back to manual ZIP selection',async()=>{
 const h=harness({online:true,loadFirstError:new Error('offline')});await h.ready;
 assert.equal(h.calls.imports,0);assert.equal(h.file.hidden,true);assert.match(h.status.textContent,/offline/);assert.equal(h.retry.hidden,false);assert.equal(h.retry.disabled,false);
 await h.retry.events.click();assert.equal(h.calls.online,2);assert.equal(h.calls.imports,1);assert.equal(h.main.hidden,false);
});
test('public launch failure requests reopen and cannot install a duplicate provider',async()=>{
 const h=harness({online:true,startError:new Error('launch stopped')});await h.ready;
 assert.equal(h.calls.provider,1);assert.equal(h.calls.imports,1);assert.match(h.status.textContent,/launch stopped/);
 assert.equal(h.main.hidden,true);assert.equal(h.start.disabled,true);assert.equal(h.retry.hidden,true);
});
test('closing a page while download awaits disposes its late result and never imports the game',async()=>{
 let mark,release;const entered=new Promise(r=>mark=r),held=new Promise(r=>release=r);
 const h=harness({online:true,waitOnline:async()=>{mark();await held;}});assert.equal(await Promise.race([entered.then(()=>true),h.ready.then(()=>false)]),true,'online download entry must be reached');h.handlers.pagehide();release();await h.ready;
 assert.equal(h.calls.imports,0);assert.equal(h.calls.provider,0);assert.equal(h.calls.dispose,1);
});
test('online resources may load for the page even if archive caching is unavailable',async()=>{
 const h=harness({online:true,storeAvailable:false});await h.ready;assert.equal(h.calls.online,1);assert.equal(h.calls.imports,1);assert.equal(h.calls.store,0);
});
test('local builds without deployment manifest retain manual import and start behavior',async()=>{
 const h=harness();await h.ready;assert.equal(h.calls.online,0);assert.equal(h.calls.imports,0);assert.equal(h.file.disabled,false);assert.equal(h.start.disabled,true);
 h.file.files=[{size:4,arrayBuffer:async()=>new ArrayBuffer(4)}];await h.file.events.change();assert.equal(h.calls.localInstall,1);assert.equal(h.start.disabled,false);
 await h.start.events.click();assert.equal(h.calls.imports,1);assert.equal(h.main.hidden,false);
});
test('local saved packages remain available after opening without public deployment',async()=>{
 const h=harness({saved:{}});await h.ready;assert.equal(h.start.disabled,false);assert.equal(h.calls.imports,0);
});
test('oversized local files never reach the archive consumer',async()=>{
 const h=harness();await h.ready;h.file.files=[{size:300*1024*1024,arrayBuffer:async()=>assert.fail('must reject before read')}];await h.file.events.change();
 assert.equal(h.calls.localInstall,0);assert.match(h.status.textContent,/过大/);
});

test('deployment discovery network failure offers retry without enabling manual import',async()=>{
 const h=harness({online:true,discoverFirstError:new Error('manifest offline')});await h.ready;
 assert.match(h.status.textContent,/manifest offline/);assert.equal(h.retry.hidden,false);assert.equal(h.retry.disabled,false);
 assert.equal(h.file.disabled,true);assert.equal(h.calls.imports,0);
 await h.retry.events.click();assert.equal(h.calls.discover,2);assert.equal(h.calls.online,1);assert.equal(h.calls.imports,1);
 assert.equal(h.file.hidden,true);assert.equal(h.main.hidden,false);
});
test('deployment discovery retry keeps one active owner while network is pending',async()=>{
 let mark,release;const entered=new Promise(r=>mark=r),held=new Promise(r=>release=r);
 let attempts=0;
 const h=harness({online:true,discoverFirstError:new Error('offline'),waitDiscover:async()=>{attempts++;if(attempts===2){mark();await held;}}});
 await h.ready;const retrying=h.retry.events.click();
 assert.equal(await Promise.race([entered.then(()=>true),retrying.then(()=>false)]),true);
 await h.retry.events.click();assert.equal(h.calls.discover,2);release();await retrying;
 assert.equal(h.calls.online,1);assert.equal(h.calls.imports,1);
});
test('closing during deployment discovery cannot offer retry or start the game',async()=>{
 let mark,release;const entered=new Promise(r=>mark=r),held=new Promise(r=>release=r);
 const h=harness({online:true,waitDiscover:async()=>{mark();await held;}});await entered;
 h.handlers.pagehide();release();await h.ready;
 assert.equal(h.calls.imports,0);assert.equal(h.calls.online,0);assert.equal(h.file.disabled,true);
});
