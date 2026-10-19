// Real Package Store import/update/reload and production Runtime configure.
// Private historical ZIP required; the test never publishes its contents.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {openAsBlob,createReadStream} from 'node:fs';
import {resolve,extname} from 'node:path';
import {createServer} from 'node:http';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {parseStoredZip,readStoredZipEntry} from '../package/stored-zip.mjs';
import {authoredSourcesPlugin} from '../lib/authored-source-resolution.mjs';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const project=resolve(import.meta.dirname,'..'),output=resolve(project,'artifacts/th075-language-upgrade');await mkdir(output,{recursive:true});
const oldZip=await openAsBlob(resolve(process.env.TH075_OLD_IMPORT_ZIP)),oldEntries=await parseStoredZip(oldZip);
const old=JSON.parse(new TextDecoder().decode(await readStoredZipEntry(oldZip,oldEntries.get('package.json'))));
const site=resolve(process.env.TH075_TEST_SITE),runtime=resolve(process.env.TH075_RUNTIME_DIRECTORY||resolve(project,'../ports/th075/build-eagler'));
const current=JSON.parse(await readFile(resolve(site,'th075.package.json'),'utf8'));
let dataDownloads=0;
const entry=`
import {installParsedPackageZip,installPackageFromRemote} from './package/package-installer.mjs';
import {readCurrentPackageGeneration,readPackageObject} from './package/package-store.mjs';
import {unzipSync} from 'fflate';
import {validateStaticLanguagePackEntries} from './src/launcher/language-pack-validation.mts';
const read=async id=>{const object=await readPackageObject(id);return object.blob?object.blob.arrayBuffer():object.data;};
const current=async()=> (await readCurrentPackageGeneration('th075')).generation;
window.test={
 async importOld(){const descriptor=await(await fetch('/old.json')).json(),files=new Map();
  for(const id of [...descriptor.base.files,'language:lang_zh-hans']){const declaration=descriptor.files[id],blob=await(await fetch('/old/'+encodeURIComponent(id))).blob();files.set(id,{blob,declaration});}
  return (await installParsedPackageZip({descriptor,files})).generation.descriptor.revision;
 },
 async update(){const previous=await current(),descriptor=await(await fetch('/current.json')).json();
  const installed=await installPackageFromRemote(descriptor,{descriptorUrl:location.origin+'/current.json',desiredFileIds:[...descriptor.base.files,'language:lang_zh-hans'],source:'local',expectedGenerationId:previous.id});
  return {revision:installed.generation.descriptor.revision,dataReused:installed.generation.files['game-data'].objectId===previous.files['game-data'].objectId};
 },
 async launch(){const generation=await current();window.events=[];
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await read(generation.files['game-data'].objectId)});
  const frame=document.createElement('iframe');frame.id='runtime';frame.width=800;frame.height=600;
  const ready=new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Runtime ready timeout')),60000);const receive=e=>{if(e.data?.event==='ready'){clearTimeout(timer);window.removeEventListener('message',receive);yes();}if(e.data?.event==='error')no(Error(e.data.error));};window.addEventListener('message',receive);});
  frame.src='/runtime/th075.html?runtimeEpoch=1&managedData=1&gameGeneration='+generation.id;document.body.append(frame);await ready;
  const entries=unzipSync(new Uint8Array(await read(generation.files['language:lang_zh-hans'].objectId)));
  const runtimePack={...validateStaticLanguagePackEntries(entries,{game:'th075',language:'lang_zh-hans'}),language:'lang_zh-hans'};
  await test.rpc('configure',{music:'none',language:'lang_zh-hans',runtimePack,options:{renderScale:1}});await test.rpc('launch');
  return generation.descriptor.revision;
 },
 rpc(command,fields={}){return new Promise((yes,no)=>{const request=crypto.randomUUID(),frame=document.querySelector('#runtime');const timer=setTimeout(()=>no(Error('RPC timeout '+command)),60000);
  function receive(e){if(e.source!==frame.contentWindow||e.data?.request!==request)return;clearTimeout(timer);window.removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}
  window.addEventListener('message',receive);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th075',epoch:1,command,request,...fields},location.origin);
 });}
};window.addEventListener('message',e=>window.events?.push(e.data));`;
const built=await build({stdin:{contents:entry,resolveDir:project,sourcefile:'th075-upgrade-entry.mjs'},bundle:true,write:false,format:'esm',platform:'browser',plugins:[authoredSourcesPlugin(project)]});
const server=createServer(async(req,res)=>{try{
 const path=new URL(req.url,'http://test').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<script type="module" src="/test.mjs"></script>');return;}
 if(path==='/test.mjs'){res.setHeader('Content-Type','text/javascript');res.end(built.outputFiles[0].contents);return;}
 if(path==='/old.json'||path==='/current.json'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(path==='/old.json'?old:current));return;}
 if(path.startsWith('/old/')){const id=decodeURIComponent(path.slice(5)),file=old.files[id];if(!file||!oldEntries.has(file.source))throw Error('Missing historical file '+id);res.end(await readStoredZipEntry(oldZip,oldEntries.get(file.source)));return;}
 if(path.startsWith('/runtime/')){const name=path.slice(9);if(name.includes('/')||name.includes('..'))throw Error('Invalid Runtime path');const file=resolve(runtime,name);res.setHeader('Content-Type',extname(file)==='.mjs'?'text/javascript':extname(file)==='.css'?'text/css':extname(file)==='.html'?'text/html':'application/wasm');createReadStream(file).pipe(res);return;}
 if(path==='/'+current.files['game-data'].source){dataDownloads++;res.writeHead(500).end('Existing DATA must be reused');return;}
 const language=current.files['language:lang_zh-hans'];if(path==='/'+language.source){res.end(await readFile(resolve(site,language.source)));return;}
 res.writeHead(404).end();
 }catch(error){res.writeHead(500).end(String(error));}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
 const page=await browser.newPage();page.setDefaultTimeout(120000);await page.setViewport({width:900,height:700});await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>!!window.test);
 assert.equal(await page.evaluate(()=>test.importOld()),old.revision);console.log('Historical import PASS');
 assert.equal(await page.evaluate(()=>test.launch()),old.revision);console.log('Historical language launch PASS');
 const update=await page.evaluate(()=>test.update());assert.equal(update.revision,current.revision);assert.equal(update.dataReused,true);assert.equal(dataDownloads,0);
 await page.reload();await page.waitForFunction(()=>!!window.test);assert.equal(await page.evaluate(()=>test.launch()),current.revision);
 const frame=page.frames().find(f=>f.url().includes('/runtime/th075.html'));await frame.waitForFunction(()=>Module._th075_frame()>60);
 assert.equal(await frame.evaluate(()=>Module.th075Language),'lang_zh-hans');assert.equal(await frame.evaluate(()=>FS.analyzePath('/thcrap/th075/images/base/data/system/replay.dat.bin').exists),true);
 const metadata=await frame.evaluate(()=>JSON.parse(FS.readFile('/thcrap/th075/localization/options.json',{encoding:'utf8'})));assert.equal(metadata.storyDialogue.scripts.length,11);assert(metadata.storyDialogue.quotes>1700);
 // Drive the production controller into Story and let its first dialogue appear.
 await frame.evaluate(()=>Module.ccall('th075_return_title','number',[],[],{async:true}));
 async function frames(n){const start=await frame.evaluate(()=>Module._th075_frame());await frame.waitForFunction(v=>Module._th075_frame()>=v,{},start+n);}
 async function tap(code='KeyZ'){await page.evaluate(code=>test.rpc('keyboard',{code,down:true}),code);await frames(2);await page.evaluate(code=>test.rpc('keyboard',{code,down:false}),code);await frames(5);}
 await frames(60);await tap();await frame.waitForFunction(()=>Module._th075_scene()===3);await frames(50);
 for(let i=0;i<15;i++){if(await frame.evaluate(()=>Module._th075_scene()===0))break;await tap();await frames(15);}
 await frame.waitForFunction(()=>Module._th075_scene()===0);await frames(500);
 assert.equal(await frame.evaluate(()=>Module.UTF8ToString(Module._th075_error())),'');await page.screenshot({path:resolve(output,'updated-chinese-dialogue.png')});
 await page.evaluate(()=>test.rpc('sync'));await page.reload();await page.waitForFunction(()=>!!window.test);assert.equal(await page.evaluate(()=>test.launch()),current.revision);
 assert.equal(dataDownloads,0);const report={historicalRevision:old.revision,currentRevision:current.revision,importedBaseReused:true,newDataDownloads:dataDownloads,productionConfigure:true,refreshLaunch:true,secondRefreshLaunch:true,chineseQuotes:metadata.storyDialogue.quotes};await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(report);
}finally{await browser?.close();await new Promise(r=>server.close(r));}
