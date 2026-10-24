import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import {extname,resolve} from 'node:path';
import {readVerifiedRuntimeFile,verifyRuntimePackage,verifyPrivateData} from './acceptance-contracts.mjs';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {createChromiumResponseSource} from '../browser/response-body-capture.mjs';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const mime=path=>({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript',
  '.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(path)]??'application/octet-stream';

/**
 * Compare browser-visible bytes with the verified fixture. expectedResponse may
 * return a semantic descriptor with validate({url,response,record,bytes}); the
 * body is still captured and hashed before that validator runs.
 */
export function watchFixtureResponses(page,{base,expectedResponses,expectedResponse,output,label='runtime'}){
  assert.match(label,/^[A-Za-z0-9_-]+$/,'Response evidence label must be a basename');
  const origin=new URL(base).origin,responses=[],errors=[],pending=new Set();
  const receive=response=>{
    const url=new URL(response.url());if(!['http:','https:'].includes(url.protocol)||url.origin!==origin)return;
    if(url.pathname==='/favicon.ico'&&response.status()===204)return;
    const operation=(async()=>{
      const expected=(expectedResponse?await expectedResponse(url,response):null)??expectedResponses.get(url.pathname+url.search);
      assert.ok(expected,`Browser response was not supplied by the verified fixture: ${url.href}`);
      const semantic=typeof expected.validate==='function';
      const record={url:url.href,status:response.status(),expected:semantic?(expected.expected??{kind:expected.kind??'semantic'}):expected,
        headers:await response.allHeaders(),
        fromServiceWorker:response.fromServiceWorker(),requestId:response.requestId,resourceType:response.resourceType};
      responses.push(record);
      let bytes;
      try{bytes=await response.body();}catch(error){record.bodyError=String(error.stack||error);
        record.captureFailure=error.responseCapture;
        if(error.bufferedBody){record.bufferedAfterFailure={bytes:error.bufferedBody.length,sha256:sha256(error.bufferedBody)};
          if(output){const name=`${label}-${responses.length}-${sha256(url.href).slice(0,16)}.partial.response`;
            await writeFile(resolve(output,name),error.bufferedBody);record.bufferedBody=name;}}
        throw Error(`Cannot capture browser response body: ${url.href} (${response.status()}): ${error.message??error}`,{cause:error});}
      record.actual={bytes:bytes.length,sha256:sha256(bytes)};
      record.capture=response.captureDetails?.();
      record.transport=record.capture?.terminal;
      record.contentIdentity='failed';
      if(semantic){
        try{
          const receipt=await expected.validate({url,response,record,bytes});
          record.semanticReceipt=receipt;
          if(receipt&&typeof receipt==='object'&&Object.hasOwn(receipt,'metadata'))record.metadata=receipt.metadata;
          record.contentIdentity='passed';
          return;
        }catch(error){
          if(output){const name=`${label}-${responses.length}-${sha256(url.href).slice(0,16)}.response`;
            await writeFile(resolve(output,name),bytes);record.actualBody=name;}
          throw error;
        }
      }
      if(response.status()!==200||bytes.length!==expected.bytes||record.actual.sha256!==expected.sha256){
        if(output){const name=`${label}-${responses.length}-${sha256(url.href).slice(0,16)}.response`;
          await writeFile(resolve(output,name),bytes);record.actualBody=name;}
        throw Error(`Browser response identity mismatch: ${url.href} expected ${expected.bytes}/${expected.sha256}, received ${bytes.length}/${record.actual.sha256}`);
      }
      record.contentIdentity='passed';
    })().catch(error=>errors.push(String(error.stack||error)));
    pending.add(operation);void operation.finally(()=>pending.delete(operation));
  };
  page.on('response',receive);
  return {responses,errors,async flush(){
    while(pending.size)await Promise.all([...pending]);
    assert.deepEqual(errors,[],'Browser received altered or unverified fixture bytes');
  },stop(){page.off('response',receive);}};
}

export async function createFixtureResponseProbe(page,options){
  const origin=new URL(options.base).origin;
  const source=await createChromiumResponseSource(page,{...options.captureBudgets,
    acceptResponse:response=>{const url=new URL(response.url);return ['http:','https:'].includes(url.protocol)&&url.origin===origin;}});
  const probe=watchFixtureResponses(source,options);
  return{responses:probe.responses,errors:probe.errors,capture:source.capture,flush:probe.flush,
    async stop(){probe.stop();await source.close();}};
}

export function protocolHostPage({target,epoch=1,scripts=[]}) {
  assert.ok(Number.isSafeInteger(epoch)&&epoch>0,'Runtime epoch must be a positive integer');
  const runtime=`/runtime/${target.id}/${target.entry}?hosted=1&managedData=1&gameGeneration=runtime-fixture&runtimeEpoch=${epoch}`;
  return `<!doctype html><meta charset="utf-8"><title>${target.id}</title>
  <style>html,body{margin:0;background:#222}iframe{width:960px;height:720px;border:0}</style>
  ${scripts.map(src=>`<script src=${JSON.stringify(src)}></script>`).join('')}
  <script>
  window.events=[];
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const response=await fetch(${JSON.stringify('/data/'+target.id)});if(!response.ok)throw Error('Private DATA unavailable');const buffer=await response.arrayBuffer();return{buffer,bytes:buffer.byteLength}};
  window.send=(command,fields={})=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:${JSON.stringify(target.game)},epoch:${epoch},command,...fields},location.origin);
  const belongs=event=>event.origin===location.origin&&event.source===document.querySelector('iframe')?.contentWindow&&event.data?.protocol==='eagler-touhou/1'&&event.data?.game===${JSON.stringify(target.game)}&&event.data?.epoch===${epoch};
  addEventListener('message',event=>{if(belongs(event))events.push(event.data)});
  window.rpc=(command,fields={})=>new Promise((accept,reject)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout: '+command))},120000);function receive(event){if(!belongs(event)||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?accept(event.data):reject(Error(event.data.error||'Command rejected'))}addEventListener('message',receive);send(command,{...fields,request})});
  </script><iframe src=${JSON.stringify(runtime)}></iframe>`;
}

/** A local fixture serves only identity-verified packages and immutable DATA. */
export async function createRuntimeFixtureSite({repoRoot,targets,shared={},port=0,protocol='http',renderHost=protocolHostPage,files=new Map()}) {
  const identities=new Map(),sharedInputs=new Map(),httpFailures=[],expectedResponses=new Map();
  assert.ok(Array.isArray(targets)&&targets.length>0,'Explicit nonempty Runtime target selection required');
  assert.equal(new Set(targets.map(target=>target.id)).size,targets.length,'Duplicate Runtime target identity');
  for(const [name,path] of Object.entries(shared)){
    assert.match(name,/^[A-Za-z0-9_.-]+$/,'Shared resource must use a basename');
    const bytes=await readFile(resolve(path));sharedInputs.set('/shared/'+name,{path:resolve(path),bytes:bytes.length,sha256:sha256(bytes)});
  }
  const fixture=await createFixtureServer(async(request,response)=>{
    try {
      const url=new URL(request.url,'http://127.0.0.1');
      const reply=(bytes,type)=>{
        const body=typeof bytes==='string'?Buffer.from(bytes):bytes;
        expectedResponses.set(url.pathname+url.search,{bytes:body.length,sha256:sha256(body)});
        response.writeHead(200,{'Content-Type':type,'Content-Length':body.length,'Cache-Control':'no-store'}).end(body);
      };
      if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
      const target=targets.find(target=>url.pathname===`/host/${target.id}`);
      if(target){assert.ok(identities.has(target.id),'Runtime target has not passed preflight');
        const html=renderHost({target,epoch:Number(url.searchParams.get('epoch')??1)});
        reply(html,'text/html');return;}
      for(const target of targets){
        const identity=identities.get(target.id),prefix=`/runtime/${target.id}/`;
        if(url.pathname.startsWith(prefix)){
          assert.ok(identity,'Runtime target has not passed preflight');
          const path=decodeURIComponent(url.pathname.slice(prefix.length));
          const bytes=await readVerifiedRuntimeFile(target,identity.runtime,path);
          reply(bytes,mime(path));return;
        }
        if(url.pathname===`/data/${target.id}`){
          assert.ok(identity,'Private DATA has not passed preflight');
          const bytes=await readFile(target.data);
          assert.equal(bytes.length,identity.data.bytes,'Private DATA size changed');
          assert.equal(sha256(bytes),identity.data.sha256,'Private DATA changed');
          reply(bytes,'application/octet-stream');return;
        }
      }
      const resource=sharedInputs.get(url.pathname);
      if(resource){const bytes=await readFile(resource.path);
        assert.equal(bytes.length,resource.bytes,'Shared resource size changed');assert.equal(sha256(bytes),resource.sha256,'Shared resource changed');
        reply(bytes,mime(url.pathname));return;}
      if(files.has(url.pathname)){
        const input=files.get(url.pathname),bytes=typeof input==='string'?await readFile(input):input;
        assert.ok(bytes instanceof Uint8Array,'Fixture asset must be a file path or bytes');
        reply(bytes,mime(url.pathname));return;
      }
      response.writeHead(404).end('Unknown Runtime fixture resource');
    }catch(error){httpFailures.push({url:request.url,error:String(error.stack||error)});response.writeHead(500).end(String(error.message||error));}
  },{port,protocol});
  const {base}=fixture;
  return {base,protocol:fixture.protocol,tlsIdentity:fixture.tlsIdentity,fetch:fixture.fetch,httpFailures,sharedInputs:Object.fromEntries(sharedInputs),
    watchPage:(page,options={})=>createFixtureResponseProbe(page,{...options,base,expectedResponses}),
    hostUrl:(target,epoch=1)=>`${base}/host/${target.id}?epoch=${epoch}`,
    async verifyShared(){
      for(const input of sharedInputs.values()){
        const bytes=await readFile(input.path);
        assert.equal(bytes.length,input.bytes,'Shared resource size changed');
        assert.equal(sha256(bytes),input.sha256,'Shared resource changed');
      }
      return Object.fromEntries(sharedInputs);
    },
    async verify(target){
      assert.ok(targets.includes(target),'Runtime target is outside the selected set');
      const identity={runtime:await verifyRuntimePackage(target,{repoRoot}),data:await verifyPrivateData(target.data)};
      if(target.dataSha256)assert.equal(identity.data.sha256,target.dataSha256,'Private DATA identity mismatch');
      const previous=identities.get(target.id);if(previous)assert.deepEqual(identity,previous,'Runtime source, receipts, package or DATA changed during acceptance');
      identities.set(target.id,identity);return identity;
    },
    close:fixture.close};
}
