// Actual Chrome, two documents, production preparation/acquisition and IndexedDB.
// Package bytes and Runtime ports are synthetic; this does not execute a game.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {execFileSync} from 'node:child_process';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';

const project=resolve(import.meta.dirname,'..');
const bundle=await build({stdin:{resolveDir:project,loader:'ts',contents:`
 export {startPreparedGameBackground} from './app/services/game-preparation-background.ts';
 export {ensureOggStartup} from './app/services/game-resources.ts';
 export {installPackageFromAcquisition} from './package/package-installer.mjs';
 export {installPublishedPackage} from './package/package-launcher.mjs';
 export * from './package/package-store.mjs';
`},bundle:true,write:false,format:'esm',platform:'browser',plugins:[authoredSourcesPlugin(project)]});
const server=createServer((req,res)=>{
  if(req.url==='/api.mjs'){res.setHeader('Content-Type','text/javascript');res.end(bundle.outputFiles[0].contents);}
  else {res.setHeader('Content-Type','text/html');res.end('<!doctype html><title>Package generation conditions</title>');}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const executablePath=process.env.CHROME_PATH || execFileSync(process.env.PYTHON||'python',[
 '-c','from playwright.sync_api import sync_playwright\nwith sync_playwright() as p: print(p.chromium.executable_path)'
],{encoding:'utf8'}).trim();
let browser;
const report=[];
async function waitFor(page,predicate) {
 const deadline=Date.now()+5000;
 while(Date.now()<deadline) {
  if(await page.evaluate(predicate)) return;
  await new Promise(resolve=>setTimeout(resolve,20));
 }
 throw new Error('Browser condition did not settle: '+predicate);
}
try {
 browser=await puppeteer.launch({executablePath,headless:true});
 for(const mode of ['progressive-replaced','progressive-in-flight','startup-in-flight','staging-without-locks']) {
  console.log('Checking',mode);
  const context=await browser.createBrowserContext(), pages=await Promise.all([context.newPage(),context.newPage()]);
  try {
   for(const page of pages) {
    await page.goto(`http://127.0.0.1:${server.address().port}`);
    await page.evaluate(async(mode)=>{
     if(mode==='staging-without-locks') Object.defineProperty(navigator,'locks',{configurable:true,value:undefined});
     window.api=await import('/api.mjs');
     window.descriptor=revision=>({schema:'eagler-touhou/package/1',game:'th06',revision,
      runtimeRequirement:{protocol:'eagler-touhou/1',target:'th06',dataFile:'data'},
      files:{data:{source:'data',target:'/th06.data',revision,bytes:1},
       ...Object.fromEntries([1,2,3].map(n=>['track-'+n,{source:'track-'+n,target:'/bgm/track-'+n+'.ogg',revision:'ogg-r1',bytes:1}]))},
      base:{files:['data']},components:{ogg:{type:'ogg',files:['track-1','track-2','track-3']}}});
     window.install=(revision,value,files=['data'])=>api.installPackageFromAcquisition({descriptor:descriptor(revision),
      source:'local',reuseCurrent:false,desiredFileIds:files,acquire:async()=>new Uint8Array([value])});
    },mode);
   }
   const [oldPage,newPage]=pages;
   await oldPage.evaluate(async mode=>{
    const files=mode.startsWith('progressive')?['data','track-1','track-2']:['data'];
    window.loaded=(await install('r1',1,files)).generation;
    window.fetchStarted=false;window.fetches=[];window.attached=[];window.errors=[];
    const gate=new Promise(resolve=>{window.releaseFetch=resolve;});
    window.acquisition={
     getMetadata:()=>({siteManifest:{shared:{resourceMode:'hosted'}}}),
     getPublication:async()=>({catalog:{schema:'eagler-touhou/release-catalog/1',games:{th06:{revision:'r1',descriptor:'package.json'}}},catalogUrl:location.href}),
     readCurrent:()=>api.readCurrentPackageGeneration('th06'),refreshInstalled:async()=>{},
     getPackageFetch:()=>async url=>{
      fetches.push(String(url));
      if(new URL(url,location.href).pathname.endsWith('package.json')) {window.fetchStarted=true;await gate;return new Response(JSON.stringify(descriptor('r1')));}
      return new Response(new Uint8Array([1]));
     },
     installPublished:(game,options)=>api.installPublishedPackage(game,options),
    };
    window.run=()=>mode==='startup-in-flight'
     ? api.ensureOggStartup({generation:loaded,music:'ogg-stream',acquisition,translate:key=>key})
     : api.startPreparedGameBackground({input:{acquisition,runtime:{getSnapshot:()=>({epoch:1,launched:true,game:'th06',music:'ogg'}),extendOggResources:async(_epoch,_generation,ids)=>attached.push(...ids)},translate:key=>key,settings:{gameVersion:'original'},onBackgroundError:error=>errors.push(error.message)},
       prepared:{music:'ogg-stream',plan:{game:'th06',generation:loaded,localOgg:{fileIds:['track-1','track-2']}}},epoch:1,deferredUpdate:null});
   },mode);
   if(mode==='progressive-replaced') {
    await newPage.evaluate(()=>install('r3',3));
    await oldPage.evaluate(()=>{releaseFetch();return run();});
    assert.deepEqual(await oldPage.evaluate(()=>fetches),[]);
   } else if(mode==='staging-without-locks') {
    await newPage.evaluate(()=>{
     window.importStarted=false;
     const gate=new Promise(resolve=>{window.releaseImport=resolve;});
     window.pendingImport=api.installPackageFromAcquisition({descriptor:descriptor('r3'),source:'local',reuseCurrent:false,
      desiredFileIds:['data'],acquire:async()=>{window.importStarted=true;await gate;return new Uint8Array([3]);}});
    });
    await waitFor(newPage,()=>window.importStarted);
    await oldPage.evaluate(()=>{
     window.installationReads=0;
     const original=IDBObjectStore.prototype.get;
     IDBObjectStore.prototype.get=function(key){const request=original.call(this,key);if(this.name===api.PACKAGE_INSTALLATIONS)request.addEventListener('success',()=>{window.installationReads++;});return request;};
     window.pending=api.installPublishedPackage('th06',{catalog:{schema:'eagler-touhou/release-catalog/1',games:{th06:{revision:'r2',descriptor:'package.json'}}},
      catalogUrl:location.href,expectedGenerationId:loaded.id,
      fetchImpl:async url=>new URL(url,location.href).pathname.endsWith('package.json')?new Response(JSON.stringify(descriptor('r2'))):new Response(new Uint8Array([2]))})
      .then(()=>({error:null}),error=>({error:error.message})).then(outcome=>{window.pendingOutcome=outcome;return outcome;});
    });
    await waitFor(oldPage,()=>window.installationReads>=2||window.pendingOutcome);
    assert.equal(await oldPage.evaluate(()=>window.pendingOutcome),undefined,
     JSON.stringify(await oldPage.evaluate(()=>({reads:installationReads,outcome:window.pendingOutcome}))));
    await newPage.evaluate(async()=>{releaseImport();await pendingImport;});
    const outcome=await oldPage.evaluate(()=>pending);
    assert.equal(outcome.error,'Package changed before background update');
   } else {
    await oldPage.evaluate(()=>{window.pending=run().then(result=>{window.runDone=true;return result;});});
    await waitFor(oldPage,()=>window.fetchStarted||window.runDone);
    assert.equal(await oldPage.evaluate(()=>fetchStarted),true,JSON.stringify(await oldPage.evaluate(()=>({errors,fetches}))));
    await newPage.evaluate(()=>install('r3',3));
    const outcome=await oldPage.evaluate(async()=>{releaseFetch();return await pending;});
    if(mode==='startup-in-flight') assert.equal(outcome.music,'midi');
   }
   const current=await oldPage.evaluate(async()=>{
    const {generation,installation}=await api.readCurrentPackageGeneration('th06');
    const object=await api.readPackageObject(generation.files.data.objectId);
    return {revision:generation.descriptor.revision,data:[...new Uint8Array(await object.blob.arrayBuffer())],pending:installation.pendingGeneration,attached};
   });
   assert.equal(current.revision,'r3',mode);assert.deepEqual(current.data,[3],mode);assert.equal(current.pending,null,mode);assert.deepEqual(current.attached,[],mode);
   report.push({mode,pass:true,...current});
  } finally {await context.close();}
 }
 const out=resolve(project,'artifacts/validation/web-audit-fixes-20261018');await mkdir(out,{recursive:true});
 await writeFile(resolve(out,'package-browser.json'),JSON.stringify({browser:await browser.version(),cases:report},null,2)+'\n');
 console.log(JSON.stringify({pass:true,cases:report}));
} finally {await browser?.close();await new Promise(resolve=>server.close(resolve));}
