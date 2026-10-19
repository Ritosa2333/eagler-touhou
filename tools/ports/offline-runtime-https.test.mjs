import test from 'node:test';
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import {createServer} from 'node:http';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {basename,dirname,join,resolve,sep} from 'node:path';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity} from './offline-runtime-https.mjs';

const tempPrefix='eagler-offline-https-test-';
async function tempDirectory(){return mkdtemp(join(tmpdir(),tempPrefix));}
async function removeTemp(path){
  const absolute=resolve(path),parent=resolve(tmpdir());
  assert.ok(absolute.toLowerCase().startsWith(`${parent}${sep}`.toLowerCase()));
  assert.ok(basename(absolute).startsWith(tempPrefix));
  await rm(absolute,{recursive:true,force:true});
}
class FakeCdpSession extends EventEmitter{
  bodies=new Map();
  async send(method,{requestId}={}){
    if(method==='Network.getResponseBody')return{body:this.bodies.get(requestId),base64Encoded:true};
    return{};
  }
  async detach(){}
  respond(id,url,body,{status=200,fromServiceWorker=false}={}){
    const bytes=Buffer.from(body);this.bodies.set(id,bytes.toString('base64'));
    this.emit('Network.responseReceived',{requestId:id,type:'Document',response:{url,status,headers:{'content-type':'text/html'},fromServiceWorker}});
    this.emit('Network.loadingFinished',{requestId:id,encodedDataLength:bytes.length});
  }
}
class ProbePage extends EventEmitter{
  session=new FakeCdpSession();
  context(){return{newCDPSession:async()=>this.session};}
  respond(...args){this.session.respond(...args);}
}

test('TLS edge proxies the unchanged local static server and scopes trust to its own certificate',async()=>{
  const backend=createServer((request,response)=>{
    if(request.url==='/'||request.url==='/index.html'){const body='<h1>served</h1>';response.writeHead(200,{'content-type':'text/html','content-length':Buffer.byteLength(body)}).end(body);return;}
    response.writeHead(404).end('Not found');
  });
  await new Promise((accept,reject)=>{backend.once('error',reject);backend.listen(0,'127.0.0.1',accept);});
  let proxy;
  try{
    proxy=await createOfflineStaticTlsProxy({backendBase:`http://127.0.0.1:${backend.address().port}/`});
    assert.equal(proxy.protocol,'https');
    assert.match(proxy.tlsIdentity.spkiSha256,/^[A-Za-z0-9+/]+=*$/);
    const result=await proxy.fetch(new URL('/',proxy.base));
    assert.equal(result.status,200);assert.equal(await result.text(),'<h1>served</h1>');
    const missing=await proxy.fetch(new URL('/missing',proxy.base));assert.equal(missing.status,404);
  }finally{
    if(proxy)await proxy.close();
    await new Promise(accept=>backend.close(accept));
  }
});

test('browser response watcher pins SPA, Runtime, and worker-visible bytes to the frozen site snapshot',async()=>{
  const root=await tempDirectory();
  try{
    await mkdir(join(root,'runtime','th11'),{recursive:true});
    await writeFile(join(root,'index.html'),'<main>launcher</main>');
    await writeFile(join(root,'runtime','th11','game.html'),'<script src="game.js"></script>');
    await writeFile(join(root,'runtime','th11','game.js'),'window.gameReady=true;');
    const snapshot=await captureOfflineSiteIdentity(root),page=new ProbePage();
    const watcher=await watchOfflineSiteResponses(page,{base:'https://127.0.0.1:9000/',snapshot,label:'offline'});
    page.respond('host','https://127.0.0.1:9000/?game=th11','<main>launcher</main>');
    page.respond('runtime-html','https://127.0.0.1:9000/runtime/th11/game.html','<script src="game.js"></script>');
    page.respond('runtime-js','https://127.0.0.1:9000/runtime/th11/game.js','window.gameReady=true;',{fromServiceWorker:true});
    await watcher.flush();await watcher.stop();
    assert.equal(watcher.responses.length,3);
    assert.equal(watcher.responses[2].fromServiceWorker,true);
    const mutated=await verifyOfflineSiteIdentity(snapshot);assert.deepEqual(mutated,snapshot.identity);
  }finally{await removeTemp(root);}
});

test('browser response watcher saves altered site bytes as failure evidence',async()=>{
  const root=await tempDirectory();
  try{
    await writeFile(join(root,'index.html'),'<main>trusted</main>');
    const snapshot=await captureOfflineSiteIdentity(root),page=new ProbePage(),output=join(root,'evidence');
    await mkdir(output);
    const watcher=await watchOfflineSiteResponses(page,{base:'https://127.0.0.1:9001/',snapshot,output,label:'offline'});
    page.respond('mutated','https://127.0.0.1:9001/','<main>injected</main>');
    await assert.rejects(watcher.flush(),/altered or unverified/);await watcher.stop();
    assert.equal(watcher.errors.length,1);
    const evidence=await readFile(join(output,watcher.responses[0].actualBody));assert.equal(evidence.toString(),'<main>injected</main>');
  }finally{await removeTemp(root);}
});
