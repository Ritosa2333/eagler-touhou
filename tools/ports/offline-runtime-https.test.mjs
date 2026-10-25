import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash,X509Certificate} from 'node:crypto';
import {EventEmitter} from 'node:events';
import {createServer} from 'node:http';
import {request as requestHttps} from 'node:https';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {basename,dirname,join,resolve,sep} from 'node:path';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity,classifyStaticDiscoveryEvents} from './offline-runtime-https.mjs';
import {languageFailureResponse} from './language-network-contract.mjs';

const tempPrefix='eagler-offline-https-test-';
test('static discovery errors require exact verified response identity and unread-body cancellation',()=>{
  const url='https://fixture.test/_eagler/resource-routes?path=%2Fgames%2Fth08%2Fth08.data',response={url,status:404,contentIdentity:'passed',semanticReceipt:{kind:'static-resource-route-discovery',bodyConsumption:'not-read'}};
  const events=[{type:'http',url,status:404},{type:'console',location:{url},text:'Failed to load resource: the server responded with a status of 404 ()'},
    {type:'requestfailed',url,error:'net::ERR_ABORTED'}];
  assert.deepEqual(classifyStaticDiscoveryEvents(events,[response]),{expected:events,blocking:[]});
  for(const mutation of [r=>r.url+='other',r=>r.contentIdentity='failed',r=>r.status=200,r=>r.semanticReceipt.kind='other']){
    const changed=structuredClone(response);mutation(changed);assert.deepEqual(classifyStaticDiscoveryEvents(events,[changed]).blocking,events);
  }
  const read=structuredClone(response);read.semanticReceipt.bodyConsumption='complete';assert.deepEqual(classifyStaticDiscoveryEvents(events,[read]).blocking,[events[2]]);
  assert.equal(classifyStaticDiscoveryEvents([{...events[1],location:{url:'https://other.test/'}}],[response]).blocking.length,1);
});
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
  respond(id,url,body,{status=200,fromServiceWorker=false,headers={'content-type':'text/html'},type='Document'}={}){
    const bytes=Buffer.from(body);this.bodies.set(id,bytes.toString('base64'));
    this.emit('Network.responseReceived',{requestId:id,type,response:{url,status,headers,fromServiceWorker}});
    this.emit('Network.loadingFinished',{requestId:id,encodedDataLength:bytes.length});
  }
}
class ProbePage extends EventEmitter{
  session=new FakeCdpSession();
  context(){return{newCDPSession:async()=>this.session};}
  respond(...args){this.session.respond(...args);}
}

test('TLS control replaces one exact resource response while the static backend remains unchanged',async()=>{
  const backend=createServer((request,response)=>response.writeHead(200).end('original'));
  await new Promise(accept=>backend.listen(0,'127.0.0.1',accept));
  const control=languageFailureResponse(Buffer.alloc(32,5),{kind:'network-failure'});
  const responses=new Map([['/games/new/language/en.zip',control]]);let proxy;
  try{
    proxy=await createOfflineStaticTlsProxy({backendBase:`http://127.0.0.1:${backend.address().port}/`,responseOverrides:responses});
    const failure=await proxy.fetch(new URL('/games/new/language/en.zip',proxy.base));
    assert.equal(failure.status,404);assert.equal(await failure.text(),control.body.toString());
    assert.equal(await (await proxy.fetch(new URL('/games/new/language/en.zip?different=1',proxy.base))).text(),'original');
    responses.clear();assert.equal(await (await proxy.fetch(new URL('/games/new/language/en.zip',proxy.base))).text(),'original');
  }finally{if(proxy)await proxy.close();await new Promise(accept=>backend.close(accept));}
});

test('controlled download holds an unfinished real body and honors request cancellation',async()=>{
  const backend=createServer((request,response)=>response.end('original'));
  await new Promise(accept=>backend.listen(0,'127.0.0.1',accept));
  const body=Buffer.alloc(32768,7),control={kind:'controlled-resource-response',status:200,body,bytes:body.length,
    sha256:createHash('sha256').update(body).digest('hex'),contentType:'application/octet-stream',holdAfterBytes:4096,holdMs:30_000};
  let proxy;
  try{
    proxy=await createOfflineStaticTlsProxy({backendBase:`http://127.0.0.1:${backend.address().port}/`,responseOverrides:new Map([['/slow.bin',control]])});
    const controller=new AbortController(),started=Date.now(),chunks=[];
    const pending=new Promise((accept,reject)=>{
      const request=requestHttps(new URL('/slow.bin',proxy.base),{signal:controller.signal,rejectUnauthorized:false},response=>{
        const certificate=new X509Certificate(response.socket.getPeerCertificate().raw);
        assert.equal(createHash('sha256').update(certificate.publicKey.export({type:'spki',format:'der'})).digest('base64'),proxy.tlsIdentity.spkiSha256);
        assert.equal(response.statusCode,200);assert.equal(response.headers['content-length'],String(body.length));
        response.on('data',chunk=>{chunks.push(chunk);controller.abort();});response.once('error',reject);response.once('end',accept);
      });request.once('error',reject);request.setTimeout(5000,()=>request.destroy(Error('Transfer prefix did not arrive')));request.end();
    });
    await assert.rejects(pending,/abort/i);assert.deepEqual(Buffer.concat(chunks),body.subarray(0,4096));assert.ok(Date.now()-started<5000);
    control.holdMs=10;const response=await proxy.fetch('/slow.bin');assert.equal(response.status,200);
    assert.equal(response.headers.get('content-length'),String(body.length));assert.deepEqual(Buffer.from(await response.arrayBuffer()),body);
    control.holdAfterBytes=body.length;assert.equal((await proxy.fetch('/slow.bin')).status,502);
  }finally{if(proxy)await proxy.close();await new Promise(accept=>backend.close(accept));}
});

test('resource negatives retain exact byte proof and reject status, hash and warm-worker substitutions',async()=>{
  const root=await tempDirectory();
  try{
    await writeFile(join(root,'index.html'),'launcher');const snapshot=await captureOfflineSiteIdentity(root);
    const path='/games/new/language/en.zip',control=languageFailureResponse(Buffer.alloc(32,5),{kind:'corrupt-archive'});
    const run=async({status=200,fromServiceWorker=false,body=control.body}={})=>{
      const page=new ProbePage(),watcher=await watchOfflineSiteResponses(page,{base:'https://127.0.0.1:9010/',snapshot,
        controlledResponses:new Map([[path,control]])});
      page.respond('zip',`https://127.0.0.1:9010${path}`,body,{status,fromServiceWorker,headers:{'content-type':'application/zip'}});
      return watcher;
    };
    const good=await run();await good.flush();await good.stop();
    assert.equal(good.responses[0].semanticReceipt.purpose,'language-corrupt-archive');
    assert.equal(good.responses[0].actual.sha256,control.sha256);
    for(const options of [{status:404},{fromServiceWorker:true},{body:Buffer.alloc(32,5)}]){
      const bad=await run(options);await assert.rejects(bad.flush(),/altered or unverified/);await bad.stop();
    }
  }finally{await removeTemp(root);}
});

test('static discovery is pinned to the actual serve-static 404 for a published resource',async()=>{
  const root=await tempDirectory();
  try{
    await writeFile(join(root,'index.html'),'launcher');await mkdir(join(root,'games'));
    await writeFile(join(root,'games','data.bin'),'DATA');const snapshot=await captureOfflineSiteIdentity(root);
    const run=async({path='/games/data.bin',status=404,body='Not found',extra=''}={})=>{
      const page=new ProbePage(),watcher=await watchOfflineSiteResponses(page,{base:'https://127.0.0.1:9011/',snapshot,
        resourceRouteDiscovery:true});
      page.respond('routes',`https://127.0.0.1:9011/_eagler/resource-routes?path=${encodeURIComponent(path)}${extra}`,
        body,{status,headers:{'content-type':'text/plain; charset=utf-8','content-length':'9'}});return watcher;
    };
    const good=await run();await good.flush();await good.stop();
    assert.equal(good.responses[0].semanticReceipt.resource,'/games/data.bin');
    const unread=await run({body:''});await unread.flush();await unread.stop();
    assert.equal(unread.responses[0].semanticReceipt.bodyConsumption,'not-read');
    assert.equal(unread.responses[0].actual.bytes,0);
    for(const options of [{path:'/unpublished.bin'},{path:'//another.invalid/file'},{status:200},{body:'other'},{extra:'&unexpected=1'}]){
      const bad=await run(options);await assert.rejects(bad.flush(),/altered or unverified/);await bad.stop();
    }
  }finally{await removeTemp(root);}
});

test('an optional archive 404 cancelled before reading proves status and headers without claiming received body bytes',async()=>{
  const root=await tempDirectory();
  try{
    await writeFile(join(root,'index.html'),'launcher');const snapshot=await captureOfflineSiteIdentity(root);
    const control=languageFailureResponse(Buffer.alloc(32,5),{kind:'network-failure'}),path='/games/new/en.zip?v=known';
    const page=new ProbePage(),watcher=await watchOfflineSiteResponses(page,{base:'https://127.0.0.1:9012/',snapshot,
      controlledResponses:new Map([[path,control]])});
    page.respond('404',`https://127.0.0.1:9012${path}`,'',{status:404,
      headers:{'content-type':control.contentType,'content-length':String(control.bytes),'cache-control':'no-store'}});
    await watcher.flush();await watcher.stop();
    assert.equal(watcher.responses[0].semanticReceipt.bodyConsumption,'not-read');
    assert.equal(watcher.responses[0].actual.bytes,0);
    assert.notEqual(watcher.responses[0].actual.sha256,control.sha256);
  }finally{await removeTemp(root);}
});

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

async function appShellFixture(root){
  await writeFile(join(root,'index.html'),'<main>cached launcher</main>');
  const snapshot=await captureOfflineSiteIdentity(root),base='https://127.0.0.1:9002/';
  const appShell={schema:'eagler-touhou/app-shell/2',buildId:'a'.repeat(20),entries:['./','index.html'],runtimeManifest:null};
  const indexSha=snapshot.files.get('/index.html').sha256;
  const scope=new URL(base);
  const metadata={build:appShell.buildId,createdAt:1000,appliedAt:null,updated:false,integrity:'sha256-v1',
    entries:{[scope.href]:indexSha,[new URL('index.html',scope).href]:indexSha},runtimeTrusted:{},
    install:{reused:0,fetched:2,deferred:0},lastUsedAt:1000};
  return{snapshot,base,appShell,metadata};
}

test('App Shell status response is semantically pinned to its deployment and frozen bytes',async()=>{
  const root=await tempDirectory();
  try{
    const fixture=await appShellFixture(root),page=new ProbePage();
    const watcher=await watchOfflineSiteResponses(page,{...fixture,label:'app-shell'});
    page.respond('status',new URL('__app-shell-update-status__',fixture.base).href,JSON.stringify(fixture.metadata),{
      fromServiceWorker:true,headers:{'content-type':'application/json','cache-control':'no-store'}});
    await watcher.flush();await watcher.stop();
    const record=watcher.responses[0];
    assert.equal(record.contentIdentity,'passed');
    assert.equal(record.actual.sha256,createHash('sha256').update(JSON.stringify(fixture.metadata)).digest('hex'));
    assert.equal(record.metadata.build,fixture.appShell.buildId);
    assert.equal(record.semanticReceipt.kind,'app-shell-update-status');
    assert.equal(record.semanticReceipt.rawBody.sha256,record.actual.sha256);
    assert.deepEqual(record.semanticReceipt.frozenEntries[new URL('./',fixture.base).href],
      {sitePath:'/index.html',sha256:fixture.snapshot.files.get('/index.html').sha256});
  }finally{await removeTemp(root);}
});

test('App Shell status rejects bad identity, bytes, headers, origin path and HTTP/service-worker provenance',async()=>{
  const root=await tempDirectory();
  try{
    const output=join(root,'evidence');await mkdir(output);
    const invalidCases=[
      ['build identity',metadata=>{metadata.build='b'.repeat(20);},{},/build identity differs/],
      ['frozen revision',metadata=>{metadata.entries['https://127.0.0.1:9002/']='f'.repeat(64);},{},/revision differs from frozen site bytes/],
      ['metadata integrity',metadata=>{metadata.integrity='sha1';},{},/declare SHA-256 integrity/],
      ['metadata shape',metadata=>{metadata.unexpected=true;},{},/metadata fields differ/],
      ['timestamp',metadata=>{metadata.lastUsedAt=999;},{},/last-used timestamp precedes cache creation/],
      ['install counts',metadata=>{metadata.install.fetched=1;},{},/counts do not cover every required manifest entry/],
      ['trusted subset',metadata=>{metadata.runtimeTrusted['https://127.0.0.1:9002/runtime/unlisted.js']='f'.repeat(64);},{},/not a deferred manifest entry/],
      ['content type',()=>{}, {headers:{'content-type':'text/plain','cache-control':'no-store'}},/application\/json/],
      ['cache control',()=>{}, {headers:{'content-type':'application/json','cache-control':'max-age=60'}},/Cache-Control: no-store/],
      ['worker provenance',()=>{}, {fromServiceWorker:false},/produced by the Service Worker/],
      ['worker status',()=>{}, {status:503},/must be HTTP 200/],
    ];
    for(const [name,mutate,options,message] of invalidCases){
      const fixture=await appShellFixture(root),page=new ProbePage(),metadata=structuredClone(fixture.metadata);
      mutate(metadata);
      const watcher=await watchOfflineSiteResponses(page,{...fixture,output,label:`bad-${name.replaceAll(' ','-')}`});
      page.respond(name,new URL('__app-shell-update-status__',fixture.base).href,JSON.stringify(metadata),{
        fromServiceWorker:true,headers:{'content-type':'application/json','cache-control':'no-store'},...options});
      await assert.rejects(watcher.flush(),/altered or unverified fixture bytes/);
      await watcher.stop();
      assert.match(watcher.errors[0],message,name);
      assert.equal(watcher.responses[0].contentIdentity,'failed',name);
      assert.ok(watcher.responses[0].actualBody,name);
      const saved=await readFile(join(output,watcher.responses[0].actualBody));
      assert.deepEqual(saved,Buffer.from(JSON.stringify(metadata)),name);
    }

    const fixture=await appShellFixture(root),page=new ProbePage();
    const watcher=await watchOfflineSiteResponses(page,{...fixture,label:'unregistered'});
    page.respond('extra','https://127.0.0.1:9002/unregistered.js','window.extra=true;',{
      fromServiceWorker:true,headers:{'content-type':'text/javascript','cache-control':'no-store'}});
    await assert.rejects(watcher.flush(),/altered or unverified fixture bytes/);
    await watcher.stop();
    assert.match(watcher.errors[0],/outside the frozen site/);
  }finally{await removeTemp(root);}
});
