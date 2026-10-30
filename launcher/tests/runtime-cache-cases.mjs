import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createHash, webcrypto } from "node:crypto";
import vm from "node:vm";
import {resolveRuntimeGenerationWorkerSource} from "../lib/contracts-build.mjs";
import { runtimeGenerationId } from "../lib/runtime-generations.mjs";
import { RUNTIME_MANIFEST_SCHEMA, RUNTIME_PROTOCOL, RUNTIME_GENERATION_SCHEMA,
  runtimeGenerationBase, runtimeGenerationEntry } from "../lib/contracts/runtime-generations.mjs";
const source = (await Promise.all([
  readFile(await resolveRuntimeGenerationWorkerSource(), "utf8"),
  ...["../compat/runtime-generation-reader.js", "../src/runtime-cache-sw.js"]
    .map(path => readFile(new URL(path, import.meta.url), "utf8")),
])).join("\n");
const sha = text => createHash("sha256").update(text).digest("hex");
class MemoryCache {
  entries = new Map();
  key(input) { return typeof input === "string" ? input : input.url; }
  async put(input, response) { this.entries.set(this.key(input), response.clone()); }
  async match(input) { return this.entries.get(this.key(input))?.clone(); }
  async keys() { return [...this.entries.keys()].map(url => new Request(url)); }
  async delete(input) { return this.entries.delete(this.key(input)); }
}
const root="runtime/th08/";
function harness() {
  const stores=new Map(), clients=new Map(), requests=[], cacheModes=[];
  const state={files:{},catalog:null,outage:false,corrupt:false,slow:false,quota:false,allWritesFail:false};
  const scopeUrl=new URL("https://example.test/game/");
  const caches={
    async keys(){return [...stores.keys()]},
    async open(name){
      if(!stores.has(name)){
        const cache=new MemoryCache(),put=cache.put.bind(cache);
        cache.put=async(url,response)=>{
          if(state.allWritesFail || (state.quota && String(url).endsWith("game.wasm")))throw new Error("QuotaExceededError");
          await put(url,response);
        };
        stores.set(name,cache);
      }
      return stores.get(name);
    },
    async delete(name){return stores.delete(name)},
  };
  function publish(version) {
    const files={"game.html":`<script type="module" src="shell.mjs"></script>${version}`,
      "shell.mjs":`shell ${version}`,"glue.mjs":`glue ${version}`,"game.wasm":`wasm ${version}`,"worker.mjs":`worker ${version}`};
    if(version!=="a")files["new-helper.mjs"]=`helper ${version}`;
    const identities=Object.entries(files).map(([path,body])=>({path,bytes:Buffer.byteLength(body),sha256:sha(body)}));
    const descriptor={generation:runtimeGenerationId("game.html",identities),entry:"game.html",files:identities};
    const base=runtimeGenerationBase(root,descriptor.generation);
    for(const [path,body]of Object.entries(files))state.files[base+path]=body;
    state.files[base+"runtime-generation.json"]=JSON.stringify({schema:RUNTIME_GENERATION_SCHEMA,protocol:RUNTIME_PROTOCOL,root,...descriptor});
    const old=state.catalog?.groups[0];
    const previous=(old?[old.current,...old.previous]:[]).filter(item=>item.generation!==descriptor.generation).slice(0,2);
    state.catalog={schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[{root,current:descriptor,previous}]};
    return descriptor;
  }
  const a=publish("a"),first=structuredClone(state.catalog);
  function worker(embedded=first,fetchTimeoutMs=10){
    const context=vm.createContext({self:{clients:{async matchAll(){return [...clients.values()]}}},
      caches,crypto:webcrypto,URL,Request,Response,Headers,Uint8Array,Uint32Array,TextEncoder,
      AbortController,setTimeout,clearTimeout,console,
      fetch:async request=>{
        requests.push(request.url);
        cacheModes.push({url:request.url,cache:request.cache});
        if(state.outage)throw new Error("Offline");
        if(state.slow)return new Promise((_,reject)=>request.signal.addEventListener("abort",()=>reject(new Error("Timeout"))));
        const path=new URL(request.url).pathname.slice(scopeUrl.pathname.length);
        if(state.httpPoison && request.cache==='default' && path.endsWith('game.wasm'))return new Response('damaged HTTP-cache bytes');
        if(path==="runtime-manifest.json")return new Response(JSON.stringify(state.catalog));
        const current=runtimeGenerationBase(root,state.catalog.groups[0].current.generation);
        const body=state.corrupt && path===current+"game.wasm"?"bad wasm":state.files[path];
        if(state.streaming && path.endsWith("game.wasm") && body) {
          const bytes=new TextEncoder().encode(body); let index=0;
          return new Response(new ReadableStream({
            start(controller) { request.signal.addEventListener("abort",()=>controller.error(request.signal.reason),{once:true}); },
            async pull(controller) {
              await new Promise(resolve=>setTimeout(resolve,25));
              if(index<bytes.length) controller.enqueue(bytes.slice(index,index+=1));
              else controller.close();
            },
          }));
        }
        return new Response(body??"missing",{status:body===undefined?404:200,
          ...(state.contentType?{headers:{'Content-Type':state.contentType}}:{})});
      },
    });
    vm.runInContext(source,context);
    return context.createRuntimeCache({scopeUrl,catalog:embedded,fetchTimeoutMs});
  }
  async function launch(sw,id,options={}){
    const selected=await sw.prepareLaunch(root+"game.html",options);
    clients.set(id,{id,url:new URL(selected.entry,scopeUrl).href});
    const response=await resource(sw,selected,"game.html");
    return {...selected,body:await response.text()};
  }
  function resource(sw,selected,name){
    const path=runtimeGenerationBase(root,selected.generation)+name;
    return sw.handle({request:new Request(new URL(path,scopeUrl))});
  }
  return{stores,clients,requests,cacheModes,state,publish,worker,launch,resource,scopeUrl,caches,a};
}
const streaming=harness(); streaming.state.streaming=true;
const streamed=await streaming.launch(streaming.worker(undefined,100),"streaming");
assert.equal(streamed.generation,streaming.a.generation,"cache preparation accepts continuous transfers longer than the idle timeout");
assert.equal(await(await streaming.resource(streaming.worker(),streamed,"game.wasm")).text(),"wasm a");
// Correct bytes can have stale/incorrect MIME headers from an origin or an old
// snapshot. They must remain executable without deleting the user's cache.
const mime=harness();mime.state.contentType='application/octet-stream';
const mimeWorker=mime.worker(),mimeLaunch=await mime.launch(mimeWorker,'mime');
for(const [name,type]of [['game.html','text/html'],['shell.mjs','text/javascript'],['worker.mjs','text/javascript'],['game.wasm','application/wasm']]){
  const response=await mime.resource(mimeWorker,mimeLaunch,name);
  assert.equal(response.headers.get('Content-Type'),type,'verified Runtime MIME: '+name);
  assert.equal(await response.text(),mime.state.files[runtimeGenerationBase(root,mime.a.generation)+name]);
}
const mimeRequests=mime.requests.length;mime.state.outage=true;
const mimeOffline=await mime.launch(mime.worker(),'mime-offline',{localOnly:true});
assert.equal((await mime.resource(mime.worker(),mimeOffline,'shell.mjs')).headers.get('Content-Type'),'text/javascript');
assert.equal(mime.requests.length,mimeRequests,'old binary MIME headers are repaired offline without redownloading bytes');
const uncachedMime=harness();uncachedMime.state.contentType='application/octet-stream';
assert.equal((await uncachedMime.resource(uncachedMime.worker(),{generation:uncachedMime.a.generation},'shell.mjs')).headers.get('Content-Type'),'text/javascript',
  'a verified immutable HTTP fallback also supplies executable module MIME');
const observed=harness(); observed.state.streaming=true;
const progressA=[],progressB=[],observedWorker=observed.worker(undefined,100);
await Promise.all([observedWorker.prepareLaunch(root+'game.html',{onProgress:value=>progressA.push(value)}),
  observedWorker.prepareLaunch(root+'game.html',{onProgress:value=>progressB.push(value)})]);
assert.equal(observed.requests.filter(url=>url.endsWith('game.wasm')).length,1,'coalesced preparations share one download');
for(const progress of [progressA,progressB]) {
  const wasm=progress.filter(value=>value.path.endsWith('game.wasm'));
  assert.equal(wasm[0].phase,'requesting');assert.equal(wasm.at(-1).phase,'complete');
  assert.ok(wasm.some(value=>value.phase==='receiving' && value.loaded>0 && value.loaded<value.total),'each observer sees real partial byte progress');
}
const handoff=harness(),handoffWorker=handoff.worker(),handoffBase=runtimeGenerationBase(root,handoff.a.generation);
const importedFiles=handoff.a.files.map(file=>({path:file.path,buffer:new TextEncoder().encode(handoff.state.files[handoffBase+file.path]).buffer}));
handoff.state.outage=true;
const imported=await handoffWorker.importLaunch(root,handoff.a,importedFiles);
assert.equal(imported.generation,handoff.a.generation);assert.equal(handoff.requests.length,0,'verified buffers commit without network');
assert.equal(await(await handoff.resource(handoffWorker,imported,'game.wasm')).text(),'wasm a');
const broken=harness(),brokenWorker=broken.worker();broken.state.outage=true;
const damagedFiles=importedFiles.map(file=>file.path==='game.wasm'?{...file,buffer:new TextEncoder().encode('bad').buffer}:file);
await assert.rejects(brokenWorker.importLaunch(root,broken.a,damagedFiles),/integrity mismatch/);
await assert.rejects(brokenWorker.prepareLaunch(root+'game.html',{localOnly:true}),/No complete local Runtime/,'partial import must not become launchable');
const httpRepair=harness(),repairProgress=[];httpRepair.state.httpPoison=true;
const repairedHttp=await httpRepair.worker().prepareLaunch(root+'game.html',{onProgress:value=>repairProgress.push(value)});
assert.equal(repairedHttp.generation,httpRepair.a.generation);
assert.deepEqual(httpRepair.cacheModes.filter(value=>value.url.endsWith('game.wasm')).map(value=>value.cache),['default','reload']);
assert.ok(repairProgress.some(value=>value.path.endsWith('game.wasm')&&value.phase==='failed'),'invalid HTTP-cache bytes are rejected before a fresh attempt');
const h=harness();let sw=h.worker();
const A=await h.launch(sw,"a");assert.match(A.body,/a$/);
const modulePath=new URL(runtimeGenerationBase(root,A.generation)+'shell.mjs',h.scopeUrl).href;
for(const cache of h.stores.values()) {
  const module=await cache.match(modulePath);
  if(module)await cache.put(modulePath,new Response(await module.arrayBuffer(),{headers:{'Content-Type':'application/octet-stream'}}));
}
h.state.outage=true;
const beforeMimeRepair=h.requests.length;
const cachedModule=await h.resource(sw,A,'shell.mjs');
assert.equal(cachedModule.headers.get('Content-Type'),'text/javascript','cached module MIME is repaired after verified bytes');
assert.equal(await cachedModule.text(),'shell a');
assert.equal((await h.resource(sw,A,'game.wasm')).headers.get('Content-Type'),'application/wasm');
assert.equal((await h.resource(sw,A,'game.html')).headers.get('Content-Type'),'text/html');
assert.equal(h.requests.length,beforeMimeRepair,'MIME repair works offline and does not redownload Runtime bytes');
h.state.outage=false;
const beforeLocal=h.requests.length;
const noEmbeddedCatalog=h.worker({schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[]});
assert.equal((await noEmbeddedCatalog.prepareLaunch(root+'game.html',{localOnly:true})).generation,A.generation);
assert.equal(h.requests.length,beforeLocal,'local selection with no embedded catalog never consults the network');
await assert.rejects(noEmbeddedCatalog.prepareLaunch(root+'game.html',{localOnly:true,exclude:[A.generation]}),/No complete local/);
assert.equal(await(await h.resource(sw,A,"game.wasm")).text(),"wasm a");
const before=h.requests.length;await h.launch(sw,"a2");
assert.equal(h.requests.length,before+1,"every launch revalidates pointer, matching bytes are reused");
const b=h.publish("b"),B=await h.launch(sw,"b");assert.match(B.body,/b$/);
assert.equal(await(await h.resource(sw,A,"glue.mjs")).text(),"glue a");
assert.equal(await(await h.resource(sw,A,"game.wasm")).text(),"wasm a");
sw=h.worker();assert.equal(await(await h.resource(sw,A,"game.wasm")).text(),"wasm a","SW restart needs no client-version routing table");
assert.equal(await(await h.resource(sw,A,"worker.mjs")).text(),"worker a");
h.publish("c");h.state.corrupt=true;
assert.match((await h.launch(sw,"bad-c")).body,/b$/,"bad new Wasm must return a usable complete previous entry");
h.state.corrupt=false;h.state.outage=true;
assert.match((await h.launch(sw,"offline")).body,/b$/);
h.state.outage=false;h.state.slow=true;assert.match((await h.launch(sw,"timeout")).body,/b$/);
h.state.slow=false;h.state.quota=true;assert.match((await h.launch(sw,"quota")).body,/b$/);
h.state.quota=false;
const C=await h.launch(sw,"c");assert.match(C.body,/c$/);
h.state.allWritesFail=true;h.state.outage=true;
assert.match((await h.launch(sw,"read-only-offline")).body,/c$/,"optional selection metadata must not block an already complete offline Runtime");
h.state.allWritesFail=false;
for(const [name,cache]of h.stores)if(name.includes(C.generation))await cache.delete(new URL(runtimeGenerationBase(root,C.generation)+"game.wasm",h.scopeUrl).href);
assert.match((await h.launch(sw,"partial-c")).body,/b$/);
h.state.outage=false;
assert.match((await h.launch(sw,"exclude-c",{exclude:[C.generation]})).body,/b$/,"bootstrap retry excludes the failed entire generation");
assert.equal((await h.resource(sw,A,"glue.mjs")).headers.get("Cache-Control"),"public, max-age=31536000, immutable");
// A server rollback is a fresh pointer observation, not a numerical comparison
// of hashes; a subsequent offline start keeps that successfully chosen version.
h.publish("a");await h.launch(sw,"rollback-a");h.state.outage=true;
assert.match((await h.launch(sw,"rollback-offline")).body,/a$/);

const legacy=harness(),old=await legacy.caches.open("eagler-touhou-runtime-%2Fgame%2F-legacy");
const entries=legacy.a.files.map(file=>({url:root+file.path,revision:file.sha256}));
for(const file of legacy.a.files)await old.put(new URL(root+file.path,legacy.scopeUrl).href,
 new Response(legacy.state.files[runtimeGenerationBase(root,legacy.a.generation)+file.path]));
await old.put(new URL("__runtime-cache__/complete",legacy.scopeUrl).href,new Response(JSON.stringify({complete:true,group:{root,entries},createdAt:1})));
legacy.publish("b");
const legacyLocal=await legacy.launch(legacy.worker(),"legacy-local",{localOnly:true});
assert.equal(legacyLocal.generation,legacy.a.generation,'a complete legacy program starts before downloading its successor');
assert.equal(legacy.requests.length,0,'legacy cache migration is entirely local');
await assert.rejects(legacy.worker().prepareLaunch(root+'game.html',{localOnly:true,exclude:[legacy.a.generation]}),/No complete local/);
assert.equal(legacy.requests.length,0,'excluded legacy programs cannot trigger a foreground download');
legacy.state.outage=true;
assert.match((await legacy.launch(legacy.worker(),"legacy-offline",{localOnly:true})).body,/a$/);
for(const failure of ['missing','corrupt']) {
 const invalid=harness(),copy=await invalid.caches.open('eagler-touhou-runtime-%2Fgame%2F-invalid');
 for(const request of await old.keys())await copy.put(request,await old.match(request));
 const wasm=new URL(root+'game.wasm',invalid.scopeUrl).href;
 if(failure==='missing')await copy.delete(wasm);else await copy.put(wasm,new Response('damaged wasm'));
 await assert.rejects(invalid.worker().prepareLaunch(root+'game.html',{localOnly:true}),/No complete local/);
 assert.equal(invalid.requests.length,0,failure+' legacy caches stay out of local startup');
}
const damaged=harness(),poison=await damaged.caches.open("eagler-touhou-app-shell-poisoned");
await poison.put(new URL(root+"glue.mjs",damaged.scopeUrl).href,new Response("glue a"));
const newB=damaged.publish("b");
await poison.put(new URL(root+"game.wasm",damaged.scopeUrl).href,new Response("wasm b"));
const repaired=await damaged.launch(damaged.worker(),"repaired");assert.equal(repaired.generation,newB.generation);
assert.equal(await(await damaged.resource(damaged.worker(),repaired,"glue.mjs")).text(),"glue b");
// Missing local state does not make an already fixed immutable URL become
// latest. An ordinary same-origin request can fetch its retained server files.
const retained=harness(),oldA=retained.a;retained.publish("b");
assert.equal(await(await retained.resource(retained.worker(structuredClone(retained.state.catalog)),{generation:oldA.generation},"game.wasm")).text(),"wasm a");
const alias=await retained.worker().handle({request:{url:new URL(root+"game.html?runtimeEpoch=2",retained.scopeUrl).href,method:"GET",mode:"navigate"}});
assert.equal(alias.status,302);assert.ok(alias.headers.get("Location").endsWith("/game.html?runtimeEpoch=2"));
console.log("Immutable Runtime latest-first, coherent rollback, legacy migration, read-only cache, restart and retained URL recovery: PASS");
