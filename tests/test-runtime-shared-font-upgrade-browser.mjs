// Actual IndexedDB installation upgrade: required fonts only, no DATA/OGG fetch.
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile,readdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import assert from 'node:assert/strict';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';
const project=resolve(import.meta.dirname,'..'),out=resolve(project,'artifacts/validation/runtime-shared-font-upgrade');await mkdir(out,{recursive:true});
const result=await build({stdin:{resolveDir:project,contents:`export {ensureRequiredSharedResources as ensure} from './app/services/game-resources.ts'; export {installPackageFromAcquisition as seed} from './package/package-installer.mjs'; export {readCurrentPackageGeneration as read} from './package/package-store.mjs';`,loader:'ts'},bundle:true,write:false,format:'esm',platform:'browser',plugins:[authoredSourcesPlugin(project)]});
const hash=b=>createHash('sha256').update(b).digest('hex'),requests=[];
const data=new Uint8Array([1,2,3,4]),font=new Uint8Array([4,3,2,1]),ogg=new Uint8Array([8,9,10]);
const descriptor=game=>({schema:'eagler-touhou/package/1',game,revision:'old',runtimeRequirement:{protocol:'eagler-touhou/1',target:game,dataFile:'game-data'},
 files:Object.fromEntries([['game-data','/'+game+'.dat',data],['shared-unifont','/unifont.otf',data],['ogg:track','/music/'+game+'_00.ogg',ogg]].map(([id,target,b])=>[id,{source:id.replace(':','-')+'.bin',target,revision:hash(b).slice(0,16),bytes:b.length,sha256:hash(b)}])),
 base:{files:['game-data','shared-unifont']},components:{ogg:{type:'ogg',files:['ogg:track']}}});
const next=game=>{const d=descriptor(game);d.revision='new';d.files['shared-msgothic']={source:'shared/msgothic.ttc',target:'/msgothic.ttc',revision:hash(font).slice(0,16),bytes:font.length,sha256:hash(font)};d.base.files.push('shared-msgothic');return d;};
const server=createServer(async(req,res)=>{const p=new URL(req.url,'http://localhost').pathname;requests.push(p);res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.json')?'application/json':'text/html');if(p==='/api.mjs')res.end(result.outputFiles[0].contents);else if(p==='/shared/msgothic.ttc')res.end(font);else if(/^\/th(?:11|15)\.package.json$/.test(p))res.end(JSON.stringify(next(p.slice(1,5))));else if(p==='/')res.end('<!doctype html><title>Font upgrade test</title>');else res.writeHead(404).end('Unexpected resource');});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const browserRoot=resolve(process.env.LOCALAPPDATA,'ms-playwright'),installed=(await readdir(browserRoot)).filter(n=>/^chromium-\d+$/.test(n)).sort((a,b)=>Number(b.split('-')[1])-Number(a.split('-')[1]));
const browser=await puppeteer.launch({executablePath:resolve(browserRoot,installed[0],'chrome-win64/chrome.exe'),headless:true,args:['--no-sandbox']});
try{
 const page=await browser.newPage();await page.goto('http://127.0.0.1:'+server.address().port);
 const report=await page.evaluate(async({descriptors,buffers})=>{
  const api=await import('/api.mjs'),report=[];
  for(const descriptor of descriptors){
   const game=descriptor.game,seed=await api.seed({descriptor,source:'local',desiredFileIds:Object.keys(descriptor.files),acquire:async id=>new Uint8Array(buffers[id==='ogg:track'?'ogg':'data']).buffer});
   const catalog={schema:'eagler-touhou/release-catalog/1',games:{[game]:{revision:'new',descriptor:game+'.package.json'}}};
   let metadataReads=0;
   const acquisition={getPublication:async()=>{metadataReads++;return{catalog,catalogUrl:location.origin+'/release-catalog.json'}},fetchImpl:fetch,getPackageFetch:()=>fetch,refreshInstalled:async()=>{}};
   const first=await api.ensure(seed.generation,acquisition),stored=await api.read(game),second=await api.ensure(first,acquisition);
   if(first.id===seed.generation.id||stored.generation.id!==first.id||second.id!==first.id||metadataReads!==1)throw Error('Font upgrade was not committed once');
   for(const id of ['game-data','shared-unifont','ogg:track'])if(first.files[id].objectId!==seed.generation.files[id].objectId)throw Error('Existing content changed: '+id);
   if(!first.files['shared-msgothic']?.objectId||stored.installation.source!=='local')throw Error('Font or local installation ownership lost');
   report.push({game,upgraded:true,secondLaunchMetadataReads:0,dataAndMusicReused:true});
  }return report;
 },{descriptors:['th11','th15'].map(descriptor),buffers:{data:[...data],ogg:[...ogg]}});
 assert.equal(requests.some(p=>/game-data|ogg-track/.test(p)),false);
 await writeFile(out+'/report.json',JSON.stringify({passed:true,report,requests},null,2));console.log(JSON.stringify({passed:true,report,requests}));
}finally{await browser.close();await new Promise(r=>server.close(r));}
