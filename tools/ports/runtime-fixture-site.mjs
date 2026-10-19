import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {extname,resolve} from 'node:path';
import {readVerifiedRuntimeFile,verifyRuntimePackage,verifyPrivateData} from './acceptance-contracts.mjs';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const mime=path=>({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript',
  '.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(path)]??'application/octet-stream';

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
export async function createRuntimeFixtureSite({repoRoot,targets,shared={},port=0,renderHost=protocolHostPage,files=new Map()}) {
  const identities=new Map(),sharedInputs=new Map(),httpFailures=[];
  assert.ok(Array.isArray(targets)&&targets.length>0,'Explicit nonempty Runtime target selection required');
  assert.equal(new Set(targets.map(target=>target.id)).size,targets.length,'Duplicate Runtime target identity');
  for(const [name,path] of Object.entries(shared)){
    assert.match(name,/^[A-Za-z0-9_.-]+$/,'Shared resource must use a basename');
    const bytes=await readFile(resolve(path));sharedInputs.set('/shared/'+name,{path:resolve(path),bytes:bytes.length,sha256:sha256(bytes)});
  }
  const server=createServer(async(request,response)=>{
    try {
      const url=new URL(request.url,'http://127.0.0.1');
      if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
      const target=targets.find(target=>url.pathname===`/host/${target.id}`);
      if(target){assert.ok(identities.has(target.id),'Runtime target has not passed preflight');
        const html=renderHost({target,epoch:Number(url.searchParams.get('epoch')??1)});
        response.writeHead(200,{'Content-Type':'text/html','Cache-Control':'no-store'}).end(html);return;}
      for(const target of targets){
        const identity=identities.get(target.id),prefix=`/runtime/${target.id}/`;
        if(url.pathname.startsWith(prefix)){
          assert.ok(identity,'Runtime target has not passed preflight');
          const path=decodeURIComponent(url.pathname.slice(prefix.length));
          const bytes=await readVerifiedRuntimeFile(target,identity.runtime,path);
          response.writeHead(200,{'Content-Type':mime(path),'Content-Length':bytes.length,'Cache-Control':'no-store'}).end(bytes);return;
        }
        if(url.pathname===`/data/${target.id}`){
          assert.ok(identity,'Private DATA has not passed preflight');
          const bytes=await readFile(target.data);
          assert.equal(bytes.length,identity.data.bytes,'Private DATA size changed');
          assert.equal(sha256(bytes),identity.data.sha256,'Private DATA changed');
          response.writeHead(200,{'Content-Type':'application/octet-stream','Content-Length':bytes.length,'Cache-Control':'no-store'}).end(bytes);return;
        }
      }
      const resource=sharedInputs.get(url.pathname);
      if(resource){const bytes=await readFile(resource.path);
        assert.equal(bytes.length,resource.bytes,'Shared resource size changed');assert.equal(sha256(bytes),resource.sha256,'Shared resource changed');
        response.writeHead(200,{'Content-Type':mime(url.pathname),'Content-Length':bytes.length,'Cache-Control':'no-store'}).end(bytes);return;}
      if(files.has(url.pathname)){
        const input=files.get(url.pathname),bytes=typeof input==='string'?await readFile(input):input;
        assert.ok(bytes instanceof Uint8Array,'Fixture asset must be a file path or bytes');
        response.writeHead(200,{'Content-Type':mime(url.pathname),'Content-Length':bytes.length,'Cache-Control':'no-store'}).end(bytes);return;
      }
      response.writeHead(404).end('Unknown Runtime fixture resource');
    }catch(error){httpFailures.push({url:request.url,error:String(error.stack||error)});response.writeHead(500).end(String(error.message||error));}
  });
  await new Promise((accept,reject)=>{server.once('error',reject);server.listen(port,'127.0.0.1',accept);});
  const base=`http://127.0.0.1:${server.address().port}`;
  return {base,httpFailures,sharedInputs:Object.fromEntries(sharedInputs),
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
    close:()=>new Promise((accept,reject)=>server.close(error=>error?reject(error):accept()))};
}
