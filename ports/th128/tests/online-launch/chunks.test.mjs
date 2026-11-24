import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import os from 'node:os';
import {createHash} from 'node:crypto';
import {readOnlineDeployment,downloadOnlineArchive,loadOnlineResources} from '../../site/runtime/online-resources.mjs';
import {buildOnlineSite,SITE_FILES} from '../../tools/package-online-site.mjs';
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),bytes=Uint8Array.from([4,8,15,16,23,42]),pageURL='https://game.example/';
const parts=Array.from({length:3},(_,i)=>({url:'/games/th128/parts/'+i+'.bin',bytes:2,sha256:sha(bytes.slice(i*2,i*2+2))}));
const deployment={schema:'th128-online-deployment/1',game:'th128',resource:{url:'/games/th128/resource.zip',bytes:6,sha256:sha(bytes),revision:'release1',parts}};
const manifest=value=>new Response(JSON.stringify(value));
const chunkFetch=(calls=[],override=null)=>async(input,init)=>{
 const url=String(input);calls.push({url,init});const index=parts.findIndex(p=>new URL(p.url,pageURL).href===url);
 if(index<0)throw Error('unexpected wholeZIP fetch '+url);
 return override?override(index):new Response(bytes.slice(index*2,index*2+2),{headers:{'Content-Length':'2'}});
};
test('chunk manifest retains immutable ordered same-origin part pins',async()=>{
 const d=await readOnlineDeployment({pageURL,fetchImpl:async()=>manifest(deployment)});
 assert.equal(d.resource.parts.length,3);assert.equal(d.resource.parts[0].url,'https://game.example/games/th128/parts/0.bin');
 assert.equal(d.resource.parts[0].sha256,parts[0].sha256);assert(Object.isFrozen(d.resource.parts));
});
test('chunk manifest rejects empty too-many malformed duplicate cross-origin and wrong-sum parts',async()=>{
 for(const value of [[],Array.from({length:65},()=>parts[0]),[{...parts[0],bytes:0}],parts.slice(0,2),[{...parts[0],url:'https://other.example/a.bin'},...parts.slice(1)],[parts[0],parts[0],parts[2]],[{...parts[0],sha256:'bad'},...parts.slice(1)],[{...parts[0],url:'/a.bin?token=x'},...parts.slice(1)]]){
  await assert.rejects(readOnlineDeployment({pageURL,fetchImpl:async()=>manifest({...deployment,resource:{...deployment.resource,parts:value}})}),/part|chunk|origin|length|bytes|sum|hash/i);
 }
});
test('sequential chunks reconstruct the exact original archive and monotonic overall progress',async()=>{
 const calls=[],progress=[],result=await downloadOnlineArchive(deployment,{pageURL,fetchImpl:chunkFetch(calls),onProgress:p=>progress.push(p)});
 assert.deepEqual(result,bytes);assert.deepEqual(calls.map(x=>x.url),parts.map(p=>new URL(p.url,pageURL).href));
 const downloaded=progress.filter(x=>x.phase==='download').map(x=>x.loaded);assert(downloaded.every((x,i)=>i===0||x>=downloaded[i-1]));assert.equal(downloaded.at(-1),6);assert(progress.every(p=>p.total===6));
 assert(calls.every(x=>x.init.redirect==='error'));
});
test('a corrupt part is rejected even when complete archive identity is pinned',async()=>{
 await assert.rejects(downloadOnlineArchive(deployment,{pageURL,fetchImpl:chunkFetch([],i=>new Response(i===1?Uint8Array.from([0,0]):bytes.slice(i*2,i*2+2)))}),/part.*SHA|part.*hash/i);
});
test('truncated and oversized parts fail before complete installation',async()=>{
 for(const length of [1,3])await assert.rejects(downloadOnlineArchive(deployment,{pageURL,fetchImpl:chunkFetch([],()=>new Response(new Uint8Array(length)))}),/part|length|bytes/i);
});
test('good part hashes do not bypass the complete archive SHA',async()=>{
 const d={...deployment,resource:{...deployment.resource,sha256:'0'.repeat(64)}};
 await assert.rejects(downloadOnlineArchive(d,{pageURL,fetchImpl:chunkFetch()}),/complete.*SHA|complete.*hash/i);
});
test('part redirect or partial HTTP response is rejected',async()=>{
 for(const status of [206,302])await assert.rejects(downloadOnlineArchive(deployment,{pageURL,fetchImpl:chunkFetch([],()=>new Response(bytes.slice(0,2),{status}))}),/status|redirect|206|302/i);
});
test('cancelled part completion cannot start a following part',async()=>{
 let active=true;const calls=[];
 await assert.rejects(downloadOnlineArchive(deployment,{pageURL,isCurrent:()=>active,fetchImpl:chunkFetch(calls,i=>{active=false;return new Response(bytes.slice(i*2,i*2+2));})}),e=>e.name==='AbortError');
 assert.equal(calls.length,1);
});
test('a matching cached archive skips every chunk request',async()=>{
 let writes=0;const saved={bytes:6,sha256:sha(bytes),revision:'release1'},provider={revision:'release1',dispose(){}};
 const result=await loadOnlineResources(deployment,{pageURL,store:{async readCurrent(){return saved;},async commit(){writes++;}},reopen:async()=>provider,fetchImpl:async()=>{throw Error('cache must skip network');}});
 assert.equal(result.source,'cache');assert.equal(writes,0);
});
test('corrupt chunk update preserves prior cache and never calls install',async()=>{
 const saved={bytes:6,sha256:'0'.repeat(64),revision:'old'};let writes=0,installs=0;
 await assert.rejects(loadOnlineResources(deployment,{pageURL,store:{async readCurrent(){return saved;},async commit(){writes++;}},install:async()=>{installs++;},fetchImpl:chunkFetch([],()=>new Response(new Uint8Array(2)))}),/part.*SHA|part.*hash/i);
 assert.equal(writes,0);assert.equal(installs,0);assert.equal(saved.revision,'old');
});
function fixture(){
 const root=fs.mkdtempSync(path.join(os.tmpdir(),'th128-cf-')),site=path.join(root,'site'),core=path.join(root,'core'),zip=path.join(root,'resources.zip'),generation='a'.repeat(64);
 const write=(name,body)=>{fs.mkdirSync(path.dirname(name),{recursive:true});fs.writeFileSync(name,body);};
 for(const name of SITE_FILES)write(path.join(site,name),'fixture '+name);
 write(path.join(site,'runtime/online-resources.mjs'),'online');
 const artifacts={'core.mjs':{bytes:4,sha256:sha('core')},'core.wasm':{bytes:4,sha256:sha('wasm')}};
 write(path.join(core,'site/runtime/build.json'),JSON.stringify({game:'th128',generation,entry:'runtime/builds/'+generation+'/core.mjs',artifacts}));
 write(path.join(core,'site/runtime/builds',generation,'core.mjs'),'core');write(path.join(core,'site/runtime/builds',generation,'core.wasm'),'wasm');fs.writeFileSync(zip,bytes);
 return {root,siteRoot:site,coreRoot:core,coreBuild:generation,resourceZip:zip,outputRoot:path.join(root,'out'),packageManifest:{bytes:6,sha256:sha(bytes),revision:'0123456789abcdef'}};
}
test('static builder writes actual bounded parts and no unhostable full archive',()=>{
 const f=fixture();try{
  const r=buildOnlineSite({...f,resourceChunkBytes:2});assert.equal(r.deployment.resource.parts.length,3);
  assert(!fs.existsSync(path.join(f.outputRoot,'games/th128/th128-dat-ogg-0123456789abcdef.zip')));
  const restored=[];for(const part of r.deployment.resource.parts){const b=fs.readFileSync(path.join(f.outputRoot,part.url.slice(1)));assert.equal(b.length,2);assert.equal(sha(b),part.sha256);restored.push(b);}
  assert.deepEqual(Buffer.concat(restored),Buffer.from(bytes));assert.equal(r.files.filter(x=>x.path.endsWith('.bin')).length,3);
 }finally{fs.rmSync(f.root,{recursive:true,force:true});}
});
test('builder refuses invalid chunk size before writing a deployment',()=>{
 for(const size of [-1,0.5,25*1024*1024+1,'20']){
  const f=fixture();try{assert.throws(()=>buildOnlineSite({...f,resourceChunkBytes:size}),/chunk|part|size|byte/i);assert(!fs.existsSync(f.outputRoot));}finally{fs.rmSync(f.root,{recursive:true,force:true});}
 }
});
