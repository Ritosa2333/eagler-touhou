import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {pathToFileURL,fileURLToPath} from 'node:url';
import path from 'node:path';
const root=fileURLToPath(new URL('../../',import.meta.url));
const target=root+'/site/runtime/online-resources.mjs';
const product=fs.existsSync(target)?await import(pathToFileURL(target)): {};
function need(name){assert.equal(typeof product[name],'function',name+' automatic website entry is missing');return product[name];}
const bytes=Uint8Array.from([4,8,15,16,23,42]),digest=Buffer.from(await crypto.subtle.digest('SHA-256',bytes)).toString('hex'),pageURL='https://game.example/play.html';
const deployment={schema:'th128-online-deployment/1',game:'th128',resource:{url:'/games/th128/resource.zip',revision:'release1',bytes:6,sha256:digest}};
const manifest=(v=deployment,status=200)=>new Response(JSON.stringify(v),{status,headers:{'Content-Type':'application/json'}});
test('no deployment file preserves the local ZIP mode',async()=>{
 const read=need('readOnlineDeployment');const calls=[];const d=await read({pageURL,fetchImpl:async(url,init)=>{calls.push([String(url),init]);return new Response(null,{status:404});}});
 assert.equal(d,null);assert.equal(calls[0][0],'https://game.example/deployment.json');assert.equal(calls[0][1].cache,'no-store');
});
test('a configured website pins the same-origin resource URL and exact version hash',async()=>{
 const d=await need('readOnlineDeployment')({pageURL,fetchImpl:async()=>manifest()});
 assert.equal(d.resource.url,'https://game.example/games/th128/resource.zip');assert.equal(d.resource.sha256,digest);assert.equal(d.resource.bytes,6);assert.equal(d.resource.revision,'release1');
});
test('invalid deployment cannot send visitors to another origin or unbounded downloads',async()=>{
 const read=need('readOnlineDeployment');
 for(const resource of [{...deployment.resource,url:'https://other.invalid/a.zip'},{...deployment.resource,bytes:0},{...deployment.resource,bytes:268435457},{...deployment.resource,sha256:'x'},{...deployment.resource,revision:''}])
  await assert.rejects(read({pageURL,fetchImpl:async()=>manifest({...deployment,resource})}),/deployment|resource|origin|hash|bytes|revision/i);
});
test('server errors and oversized deployment text are explicit errors',async()=>{
 const read=need('readOnlineDeployment');
 await assert.rejects(read({pageURL,fetchImpl:async()=>new Response('',{status:503})}),/503|deployment/i);
 await assert.rejects(read({pageURL,fetchImpl:async()=>new Response('x'.repeat(9000))}),/budget|large|limit|deployment/i);
});
test('streamed archive reports actual byte progress and verifies the complete SHA',async()=>{
 const download=need('downloadOnlineArchive'),progress=[],options=[];
 const stream=new ReadableStream({start(c){c.enqueue(bytes.slice(0,2));c.enqueue(bytes.slice(2));c.close();}});
 const result=await download(deployment,{pageURL,onProgress:p=>progress.push(p),fetchImpl:async(u,i)=>{options.push([String(u),i]);return new Response(stream,{headers:{'Content-Length':'6'}});}});
 assert.deepEqual(result,bytes);assert.equal(options[0][1].redirect,'error');assert(progress.some(p=>p.phase==='download'&&p.loaded===2&&p.total===6));assert.equal(progress.at(-1).loaded,6);
});
test('partial truncated oversized and hash-changed resources never reach installation',async()=>{
 const download=need('downloadOnlineArchive');
 for(const [body,headers,status] of [[bytes.slice(0,3),{},200],[new Uint8Array(7),{},200],[new Uint8Array(6),{},200],[bytes,{'Content-Length':'7'},200],[bytes,{},206]])
  await assert.rejects(download(deployment,{pageURL,fetchImpl:async()=>new Response(body,{headers,status})}),/length|bytes|SHA|hash|status|206|download/i);
});
test('owner cancellation after fetch is enforced before reading or installing',async()=>{
 const download=need('downloadOnlineArchive');let current=true;
 await assert.rejects(download(deployment,{pageURL,isCurrent:()=>current,fetchImpl:async()=>{current=false;return new Response(bytes);}}),e=>e.name==='AbortError');
});
function store(initial=null,failWrite=false){let value=initial;return{writes:0,async readCurrent(){return value;},async commit(next){this.writes++;if(failWrite)throw new DOMException('quota','QuotaExceededError');value=next;},value:()=>value};}
const record=(changes={})=>({revision:'release1',bytes:6,sha256:digest,blob:new Blob([bytes]),...changes});
const provider=()=>({revision:'release1',fetch:async()=>new Response(),disposed:false,dispose(){this.disposed=true;}});
function operations(){
 const calls={install:0,reopen:0};return{calls,
 async install(input,{store}){calls.install++;assert.deepEqual(input,bytes);await store.commit(record());const saved=await store.readCurrent();assert.equal(saved.sha256,digest);return provider();},
 async reopen({store}){calls.reopen++;assert.equal((await store.readCurrent()).sha256,digest);return provider();}};
}
test('an exact installed archive opens with zero game-file downloads or writes',async()=>{
 const load=need('loadOnlineResources'),s=store(record()),op=operations();let network=0;
 const result=await load(deployment,{pageURL,store:s,...op,fetchImpl:async()=>{network++;throw Error('must use installedbytes');}});
 assert.equal(result.source,'cache');assert.equal(result.cached,true);assert.equal(network,0);assert.equal(s.writes,0);assert.equal(op.calls.reopen,1);
});
test('first visit downloads then installs using the existing archive consumer',async()=>{
 const s=store(),op=operations(),result=await need('loadOnlineResources')(deployment,{pageURL,store:s,...op,fetchImpl:async()=>new Response(bytes)});
 assert.equal(result.source,'download');assert.equal(result.cached,true);assert.equal(s.writes,1);assert.equal(op.calls.install,1);assert.equal(op.calls.reopen,0);
});
test('a stale archive is preserved on corrupt update with no install or start',async()=>{
 const old=record({revision:'old',sha256:'0'.repeat(64)}),s=store(old),op=operations();
 await assert.rejects(need('loadOnlineResources')(deployment,{pageURL,store:s,...op,fetchImpl:async()=>new Response(new Uint8Array(6))}),/SHA|hash/i);
 assert.equal(s.value(),old);assert.equal(s.writes,0);assert.equal(op.calls.install,0);
});
test('a denied cache uses explicit session resources so visitors can still launch',async()=>{
 const s=store(null,true),op=operations(),result=await need('loadOnlineResources')(deployment,{pageURL,store:s,...op,fetchImpl:async()=>new Response(bytes)});
 assert.equal(result.source,'download');assert.equal(result.cached,false);assert.equal(result.provider.revision,'release1');assert.equal(s.value(),null);assert(result.cacheError);
});
test('no IndexedDB still allows a verified in-memory download',async()=>{
 const op=operations(),result=await need('loadOnlineResources')(deployment,{pageURL,store:null,...op,fetchImpl:async()=>new Response(bytes)});
 assert.equal(result.cached,false);assert.equal(result.source,'download');assert.equal(result.provider.revision,'release1');
});
test('an unreadable stored blob can be replaced only after a verified fresh download',async()=>{
 const s=store(record()),op=operations();op.reopen=async()=>{throw Error('stored SHA mismatch');};
 const result=await need('loadOnlineResources')(deployment,{pageURL,store:s,...op,fetchImpl:async()=>new Response(bytes)});
 assert.equal(result.source,'download');assert.equal(s.writes,1);assert.equal(op.calls.install,1);
});
test('obsolete download completion cannot install into the next page owner',async()=>{
 const s=store(),op=operations();let current=true;
 await assert.rejects(need('loadOnlineResources')(deployment,{pageURL,store:s,...op,isCurrent:()=>current,fetchImpl:async()=>{current=false;return new Response(bytes);}}),e=>e.name==='AbortError');
 assert.equal(s.writes,0);assert.equal(op.calls.install,0);
});
