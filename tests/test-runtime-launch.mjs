import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { prepareRuntimeLaunch } from "../.cache/build/browser/assets/launcher/runtime-launch.mjs";
import { canonicalRuntimePayload, RUNTIME_MANIFEST_SCHEMA, RUNTIME_PROTOCOL,
  RUNTIME_CACHE_PROTOCOL, RUNTIME_CAPABILITIES, RUNTIME_PREPARE, RUNTIME_PROGRESS, RUNTIME_IMPORT,
  validateRuntimeImportRequest } from "../lib/contracts/runtime-generations.mjs";
const hash = text => createHash("sha256").update(text).digest("hex");
function generation(version) {
  const bodies={"game.html":`<script src="game.js"></script>${version}`,"game.js":`// ${version}`,"game.wasm":`wasm ${version}`};
  const files=Object.entries(bodies).map(([path,body])=>({path,bytes:Buffer.byteLength(body),sha256:hash(body)}));
  return {descriptor:{generation:hash(canonicalRuntimePayload("game.html",files)),entry:"game.html",files},bodies};
}
const A=generation("a"),B=generation("b"),root="runtime/th08/",baseUrl="https://example.test/mount/";
const manifest={schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[{root,current:B.descriptor,previous:[A.descriptor]}]};
const supplied=Object.entries(B.bodies).map(([path,body])=>({path,buffer:new TextEncoder().encode(body).buffer}));
const importRequest={type:RUNTIME_IMPORT,root,descriptor:B.descriptor,files:supplied};
assert.equal(validateRuntimeImportRequest(importRequest).files.length,3);
assert.throws(()=>validateRuntimeImportRequest({...importRequest,files:supplied.slice(1)}),/Incomplete/);
assert.throws(()=>validateRuntimeImportRequest({...importRequest,files:[supplied[0],supplied[0],supplied[2]]}),/Invalid Runtime import file/);
assert.throws(()=>validateRuntimeImportRequest({...importRequest,files:[{...supplied[0],buffer:new ArrayBuffer(1)},...supplied.slice(1)]}),/size mismatch/);
const requests=[];
let bad=false,offline=false;
const fetchImpl=async(input,options)=>{
  const url=new URL(input);
  requests.push({url:url.href,...options});
  if(offline)throw new Error("offline");
  if(url.pathname.endsWith("runtime-manifest.json"))return new Response(JSON.stringify(manifest));
  const generation=url.pathname.includes(B.descriptor.generation)?B:A;
  const name=url.pathname.split("/").at(-1),body=bad && generation===B && name==="game.wasm"?"incorrect":generation.bodies[name];
  return new Response(body??"missing",{status:body?200:404});
};
let resolveWorker;
const delayedWorker=new Promise(resolve=>resolveWorker=resolve);
const pending=prepareRuntimeLaunch(root+"game.html?hosted=1&gameGeneration=package-id",{baseUrl,fetchImpl,worker:delayedWorker});
assert.equal(requests.length,0,"local cache authority is consulted before an online manifest");
resolveWorker(null);
const first=await pending;
assert.equal(first.generation,B.descriptor.generation);assert.equal(first.cached,false);
assert.equal(new URL(first.url).searchParams.get("gameGeneration"),"package-id","code generation must not replace game DATA identity");
assert.equal(requests[0].cache,'no-store','the mutable manifest must bypass HTTP caches');
assert.ok(requests.slice(1).every(request=>request.cache==='reload' && request.redirect==='error'),'verified fallback populates the immutable HTTP cache');
assert.ok(requests.slice(1).every(request=>request.url.startsWith(baseUrl+root+B.descriptor.generation+"/")));
bad=true;
const rollback=await prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl,worker:null});
assert.equal(rollback.generation,A.descriptor.generation,"without SW, verify earlier whole server generation before opening it");
bad=false;
const excluded=await prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl,worker:null,exclude:[B.descriptor.generation]});
assert.equal(excluded.generation,A.descriptor.generation);
const messages=[];
const worker={postMessage(data,ports){messages.push(data);ports[0].postMessage(data.type===RUNTIME_CAPABILITIES
  ?{ok:true,protocol:RUNTIME_CACHE_PROTOCOL}
  :{ok:true,protocol:RUNTIME_CACHE_PROTOCOL,cached:true,generation:A.descriptor.generation,entry:root+A.descriptor.generation+"/game.html"});}};
offline=true;
const cached=await prepareRuntimeLaunch(root+B.descriptor.generation+"/game.html",{baseUrl,fetchImpl,worker});
assert.equal(cached.generation,A.descriptor.generation);assert.equal(cached.cached,true);
assert.equal(messages[1].type,RUNTIME_PREPARE);assert.equal(messages[1].catalog,undefined,"failed pointer does not inject a fake current catalog");
const progressEvents = [];
const progressPath = root+A.descriptor.generation+'/game.wasm';
const progressWorker = {postMessage(data, ports) {
  if(data.type===RUNTIME_CAPABILITIES) {ports[0].postMessage({ok:true,protocol:RUNTIME_CACHE_PROTOCOL});return;}
  assert.equal(data.progress,true);
  for(const phase of ['requesting','receiving']) ports[0].postMessage({type:RUNTIME_PROGRESS,protocol:RUNTIME_CACHE_PROTOCOL,path:progressPath,phase,loaded:phase==='receiving'?2:0,total:6});
  ports[0].postMessage({type:RUNTIME_PROGRESS,protocol:RUNTIME_CACHE_PROTOCOL,path:progressPath,phase:'receiving',loaded:-1,total:6});
  ports[0].postMessage({ok:true,protocol:RUNTIME_CACHE_PROTOCOL,cached:true,generation:A.descriptor.generation,entry:root+A.descriptor.generation+'/game.html'});
}};
const progressed=await prepareRuntimeLaunch(root+'game.html',{baseUrl,fetchImpl,worker:progressWorker,onProgress:value=>progressEvents.push(value)});
assert.equal(progressed.generation,A.descriptor.generation,'intermediate progress is not a final preparation response');
assert.deepEqual(progressEvents.map(value=>value.phase),['requesting','receiving','failed'],'invalid progress is ignored and a final response releases unfinished tasks');
await assert.rejects(prepareRuntimeLaunch("https://other.test/runtime/a.html",{baseUrl,fetchImpl,worker:null}),/outside/);
await assert.rejects(prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl,worker:null}),/unavailable/);
offline=false;
let stall=false, wasmAttempts=0;
const slowFetch=async(input,options)=>{
  const response=await fetchImpl(input,options);
  if (!String(input).includes(B.descriptor.generation) || !String(input).endsWith("game.wasm")) return response;
  wasmAttempts++;
  const bytes=new TextEncoder().encode(B.bodies["game.wasm"]);
  let index=0;
  return new Response(new ReadableStream({
    start(controller) {
      options.signal.addEventListener("abort",()=>controller.error(options.signal.reason),{once:true});
    },
    async pull(controller) {
      if(stall) return new Promise(()=>{});
      await new Promise(resolve=>setTimeout(resolve,25));
      if(index<bytes.length) controller.enqueue(bytes.slice(index,index+=1));
      else controller.close();
    },
  }));
};
const slow=await prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl:slowFetch,worker:null,timeoutMs:100});
assert.equal(slow.generation,B.descriptor.generation,"continuous transfer may take longer than the inactivity timeout");
assert.equal(wasmAttempts,1,"healthy slow transfer must not restart");
stall=true; wasmAttempts=0;
const stalled=await prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl:slowFetch,worker:null,timeoutMs:100});
assert.equal(stalled.generation,A.descriptor.generation,"stalled transfer still permits whole-generation fallback");
assert.equal(wasmAttempts,3,"stalled requests retain bounded retries");

// A cached launch cannot be held by a stalled network or a partially downloaded
// successor. The check handle runs only after its caller starts the game.
offline=false;requests.length=0;
let holdUpdate,updateDone=false;
const offlineFirstWorker={postMessage(data,ports){
  if(data.type===RUNTIME_CAPABILITIES){ports[0].postMessage({ok:true,protocol:RUNTIME_CACHE_PROTOCOL,localOnly:true});return;}
  if(data.localOnly){ports[0].postMessage({ok:true,protocol:RUNTIME_CACHE_PROTOCOL,cached:true,generation:A.descriptor.generation,entry:root+A.descriptor.generation+'/game.html'});return;}
  holdUpdate=()=>ports[0].postMessage({ok:true,protocol:RUNTIME_CACHE_PROTOCOL,cached:true,generation:B.descriptor.generation,entry:root+B.descriptor.generation+'/game.html'});
}};
const local=await prepareRuntimeLaunch(root+B.descriptor.generation+'/game.html',{baseUrl,fetchImpl,worker:offlineFirstWorker});
assert.equal(local.generation,A.descriptor.generation);assert.equal(requests.length,0,'complete local startup makes no fetch');
const update=local.checkForUpdate().then(value=>{updateDone=true;return value;});
while(!holdUpdate)await new Promise(resolve=>setTimeout(resolve,0));
assert.equal(updateDone,false,'download in progress cannot announce readiness');
assert.equal(local.generation,A.descriptor.generation,'background update never switches the selected entry');
holdUpdate();assert.equal((await update).generation,B.descriptor.generation);
assert.equal(await local.checkForUpdate(),await update,'one launch coalesces its update download');
const sameFetch=async()=>new Response(JSON.stringify({...manifest,groups:[{root,current:A.descriptor,previous:[]}]}));
const same=await prepareRuntimeLaunch(root+'game.html',{baseUrl,fetchImpl:sameFetch,worker:offlineFirstWorker});
assert.equal(await same.checkForUpdate(),null,'identical manifests do not announce an update');
const offlineCopy=await prepareRuntimeLaunch(root+'game.html',{baseUrl,fetchImpl:async()=>{throw new Error('Offline')},worker:offlineFirstWorker});
assert.equal(await offlineCopy.checkForUpdate(),null,'failed background check stays quiet');

const fontBodies = {"game.html":"font fixture", "fonts/blend4444.bin":"font bytes"};
const fontFiles = Object.entries(fontBodies).map(([path,body])=>({path,bytes:Buffer.byteLength(body),sha256:hash(body)}));
const fontDescriptor = {entry:"game.html",files:fontFiles,generation:hash(canonicalRuntimePayload("game.html",fontFiles))};
const fontManifest = {schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[{root,current:fontDescriptor,previous:[]}]};
const fontModes = [];
let corruptFont = false;
const fontFetch = async (input,options) => {
  const path = new URL(input).pathname;
  if(path.endsWith("runtime-manifest.json")){assert.equal(options.redirect,"error");return new Response(JSON.stringify(fontManifest));}
  const name = path.includes("/fonts/") ? "fonts/blend4444.bin" : "game.html";
  fontModes.push([name,options.redirect]);
  return new Response(corruptFont && name.startsWith("fonts/") ? "wrong" : fontBodies[name]);
};
await prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl:fontFetch,worker:null});
assert.deepEqual(fontModes,[["game.html","error"],["fonts/blend4444.bin","follow"]]);
corruptFont=true;
await assert.rejects(prepareRuntimeLaunch(root+"game.html",{baseUrl,fetchImpl:fontFetch,worker:null}),/integrity mismatch/);
console.log("Launcher offline-first, verified background updates, whole-generation fallback, bounded downloads and DATA identity separation: PASS");
