import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {request as requestHttp} from 'node:http';
import {lstat,readdir,readFile,stat,writeFile} from 'node:fs/promises';
import {dirname,relative,resolve,sep} from 'node:path';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {createFixtureResponseProbe} from './runtime-fixture-site.mjs';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const allowedHeaders=['cache-control','content-type','etag','last-modified','location','service-worker-allowed','vary'];

function requestBackend(url,method){
  return new Promise((accept,reject)=>{
    const request=requestHttp(url,{method,headers:{'accept-encoding':'identity','connection':'close'}},response=>{
      const chunks=[];
      response.on('data',chunk=>chunks.push(Buffer.from(chunk)));
      response.once('error',reject);
      response.once('end',()=>accept({status:response.statusCode??502,headers:response.headers,body:Buffer.concat(chunks)}));
    });
    request.once('error',reject);request.end();
  });
}

/**
 * Keep serve-static as the sole file server and add a short-lived HTTPS loopback
 * edge so the browser's Service Worker accepts the local site as a secure origin.
 */
export async function createOfflineStaticTlsProxy({backendBase,port=0,responseOverrides=new Map()}={}){
  const backend=new URL(backendBase);
  assert.equal(backend.protocol,'http:','Static backend must use local HTTP');
  assert.equal(backend.hostname,'127.0.0.1','Static backend must bind IPv4 loopback');
  assert.ok(Number(backend.port)>0,'Static backend must use an explicit port');
  return createFixtureServer(async(request,response)=>{
    try{
      assert.ok(request.method==='GET'||request.method==='HEAD',`Unsupported static method ${request.method}`);
      const url=new URL(request.url,backend);
      assert.equal(url.origin,backend.origin,'Static proxy cannot leave the backend loopback origin');
      const controlled=responseOverrides.get(url.pathname+url.search);
      if(controlled){
        assert.equal(controlled.kind,'controlled-resource-response','Static response override needs an explicit control contract');
        assert.equal(sha256(controlled.body),controlled.sha256,'Controlled response bytes changed');
        assert.equal(controlled.body.length,controlled.bytes,'Controlled response size changed');
        response.writeHead(controlled.status,{'content-type':controlled.contentType,'cache-control':'no-store',
          'content-length':String(controlled.bytes)});
        response.end(request.method==='HEAD'?undefined:controlled.body);return;
      }
      const upstream=await requestBackend(url,request.method);
      const headers=Object.fromEntries(allowedHeaders.flatMap(name=>{
        const value=upstream.headers[name];return value===undefined?[]:[[name,value]];
      }));
      if(request.method==='HEAD'&&upstream.headers['content-length']!==undefined)
        headers['content-length']=upstream.headers['content-length'];
      else headers['content-length']=String(upstream.body.length);
      response.writeHead(upstream.status,headers);
      response.end(request.method==='HEAD'?undefined:upstream.body);
    }catch(error){
      if(!response.headersSent)response.writeHead(502,{'content-type':'text/plain; charset=utf-8'});
      response.end(String(error?.message??error));
    }
  },{protocol:'https',port,host:'127.0.0.1'});
}

async function walkSite(root,current,files){
  for(const entry of await readdir(current,{withFileTypes:true})){
    const path=resolve(current,entry.name),info=await lstat(path);
    assert.ok(!info.isSymbolicLink(),`Packaged site must not contain symlinks: ${path}`);
    if(info.isDirectory()){await walkSite(root,path,files);continue;}
    assert.ok(info.isFile(),`Packaged site contains a non-file entry: ${path}`);
    const bytes=await readFile(path),urlPath='/'+relative(root,path).split(sep).join('/');
    files.set(urlPath,{path,bytes:bytes.length,sha256:sha256(bytes)});
  }
}

/** Pin a site snapshot before the browser run; page response checks use these identities. */
export async function captureOfflineSiteIdentity(siteRoot){
  const root=resolve(siteRoot),rootInfo=await stat(root);
  assert.ok(rootInfo.isDirectory(),'Packaged site root must be a directory');
  const files=new Map();await walkSite(root,root,files);
  assert.ok(files.has('/index.html'),'Packaged site index.html is missing');
  return{root,files,identity:Object.fromEntries([...files].sort(([a],[b])=>a.localeCompare(b))
    .map(([path,item])=>[path,{bytes:item.bytes,sha256:item.sha256}]))};
}

function physicalPathForUrl(snapshot,url){
  let pathname;
  try{pathname=decodeURIComponent(url.pathname);}catch{return null;}
  if(pathname==='/')pathname='/index.html';
  else if(pathname==='/eagler-touhou'||pathname==='/eagler-touhou/')pathname='/index.html';
  else if(pathname==='/eagler-touhou/app-shell-sw.js')pathname='/legacy-mount-retirement-sw.js';
  else if(pathname.startsWith('/eagler-touhou/'))pathname=pathname.slice('/eagler-touhou'.length);
  const record=snapshot.files.get(pathname);
  if(record)return{urlPath:pathname,record};
  const indexPath=`${pathname.endsWith('/')?pathname:`${pathname}/`}index.html`;
  const index=snapshot.files.get(indexPath);
  return index?{urlPath:indexPath,record:index}:null;
}

function assertPlainRecord(value,label){
  assert.ok(value&&typeof value==='object'&&!Array.isArray(value),`${label} must be an object`);
}

function assertExactKeys(value,keys,label){
  assert.deepEqual(Object.keys(value).sort(),[...keys].sort(),`${label} fields differ from the App Shell worker contract`);
}

function appShellStatusPlan(base,appShell,snapshot){
  assert.equal(appShell?.schema,'eagler-touhou/app-shell/2','An App Shell v2 deployment contract is required for status validation');
  assert.match(appShell.buildId,/^[a-f0-9]{20}$/i,'App Shell build id must be a 20-digit hexadecimal identity');
  assert.ok(Array.isArray(appShell.entries)&&appShell.entries.length>0,'App Shell manifest entries are required');
  const scope=new URL(base);
  assert.ok(!scope.search&&!scope.hash,'App Shell scope URL cannot contain a query or fragment');
  if(!scope.pathname.endsWith('/'))scope.pathname+='/';
  const statusUrl=new URL('__app-shell-update-status__',scope).href,entries=new Map();
  for(const value of appShell.entries){
    assert.equal(typeof value,'string','App Shell manifest entry paths must be strings');
    const path=value==='./'?'./':value.replaceAll('\\','/').replace(/^\.\//,'');
    assert.equal(value,path,`App Shell manifest entry path is not canonical: ${value}`);
    assert.ok(path==='./'||(!path.startsWith('/')&&!path.split('/').some(part=>!part||part==='..')&&!/[?#]/.test(path)),
      `Invalid App Shell manifest entry path: ${value}`);
    const entryUrl=new URL(value,scope);
    assert.equal(entryUrl.origin,scope.origin,`App Shell entry leaves its declared origin: ${value}`);
    assert.ok(entryUrl.pathname.startsWith(scope.pathname),`App Shell entry leaves its declared scope: ${value}`);
    assert.ok(!entries.has(entryUrl.href),`Duplicate App Shell entry URL: ${entryUrl.href}`);
    let relativePath;
    try{relativePath=decodeURIComponent(entryUrl.pathname.slice(scope.pathname.length));}
    catch{throw Error(`Invalid encoded App Shell entry path: ${value}`);}
    const sitePath='/'+(relativePath||'index.html');
    const site=snapshot.files.get(sitePath);
    assert.ok(site,`App Shell manifest entry is absent from the frozen site: ${value} -> ${sitePath}`);
    assert.match(site.sha256,/^[a-f0-9]{64}$/i,`Frozen site hash is invalid for ${sitePath}`);
    entries.set(entryUrl.href,{path,sitePath,sha256:site.sha256.toLowerCase(),deferred:path.startsWith('runtime/')});
  }
  return{scopeUrl:scope.href,statusUrl,entries,appShell};
}

function validateAppShellStatus({url,response,record,bytes,plan}){
  assert.equal(url.href,plan.statusUrl,'Unexpected App Shell status endpoint URL');
  assert.equal(response.fromServiceWorker(),true,'App Shell status must be produced by the Service Worker');
  assert.equal(response.status(),200,'App Shell status Service Worker response must be HTTP 200');
  const headers=Object.fromEntries(Object.entries(record.headers).map(([name,value])=>[name.toLowerCase(),String(value).trim()]));
  assert.equal(headers['content-type']?.toLowerCase(),'application/json','App Shell status must use application/json');
  assert.equal(headers['cache-control']?.toLowerCase(),'no-store','App Shell status must use Cache-Control: no-store');
  let metadata;
  try{metadata=JSON.parse(bytes.toString('utf8'));}
  catch(error){throw Error(`App Shell status body is not valid JSON: ${error.message}`);}
  assertPlainRecord(metadata,'App Shell status metadata');
  assertExactKeys(metadata,['build','createdAt','appliedAt','updated','integrity','entries','runtimeTrusted','install','lastUsedAt'],
    'App Shell status metadata');
  assert.equal(metadata.build,plan.appShell.buildId,'App Shell status build identity differs from deployment.json');
  assert.equal(metadata.integrity,'sha256-v1','App Shell status metadata must declare SHA-256 integrity');
  assert.equal(typeof metadata.updated,'boolean','App Shell status updated flag must be boolean');
  for(const name of ['createdAt','lastUsedAt'])
    assert.ok(Number.isSafeInteger(metadata[name])&&metadata[name]>0,`App Shell status ${name} must be a positive millisecond timestamp`);
  assert.ok(metadata.lastUsedAt>=metadata.createdAt,'App Shell last-used timestamp precedes cache creation');
  if(metadata.updated){
    assert.ok(Number.isSafeInteger(metadata.appliedAt)&&metadata.appliedAt>0,'Updated App Shell must have an appliedAt timestamp');
    assert.ok(metadata.appliedAt>=metadata.createdAt,'App Shell appliedAt precedes cache creation');
  }else assert.equal(metadata.appliedAt,null,'Unchanged App Shell must not claim an update application time');

  assertPlainRecord(metadata.entries,'App Shell status entries');
  const expectedEntryUrls=[...plan.entries.keys()].sort();
  assert.deepEqual(Object.keys(metadata.entries).sort(),expectedEntryUrls,'App Shell status entry set differs from deployment.appShell.entries');
  for(const [entryUrl,expected] of plan.entries){
    assert.equal(metadata.entries[entryUrl],expected.sha256,
      `App Shell status revision differs from frozen site bytes: ${entryUrl} (${expected.sitePath})`);
  }

  assertPlainRecord(metadata.install,'App Shell install counts');
  assertExactKeys(metadata.install,['reused','fetched','deferred'],'App Shell install counts');
  for(const name of ['reused','fetched','deferred'])
    assert.ok(Number.isSafeInteger(metadata.install[name])&&metadata.install[name]>=0,`App Shell install.${name} must be a nonnegative integer`);
  const deferredCount=[...plan.entries.values()].filter(entry=>entry.deferred).length;
  assert.equal(metadata.install.deferred,deferredCount,'App Shell deferred count differs from its declared entries');
  assert.equal(metadata.install.reused+metadata.install.fetched,plan.entries.size-deferredCount,
    'App Shell reused/fetched counts do not cover every required manifest entry');

  assertPlainRecord(metadata.runtimeTrusted,'App Shell trusted runtime subset');
  for(const [entryUrl,revision] of Object.entries(metadata.runtimeTrusted)){
    const expected=plan.entries.get(entryUrl);
    assert.ok(expected?.deferred,`App Shell trusted runtime URL is not a deferred manifest entry: ${entryUrl}`);
    assert.equal(revision,expected.sha256,`App Shell trusted runtime revision differs from frozen site bytes: ${entryUrl}`);
  }
  return{
    kind:'app-shell-update-status',validated:true,url:url.href,buildId:metadata.build,
    rawBody:{bytes:bytes.length,sha256:record.actual.sha256},entryCount:plan.entries.size,
    frozenEntries:Object.fromEntries([...plan.entries].map(([entryUrl,entry])=>[entryUrl,{sitePath:entry.sitePath,sha256:entry.sha256}])),
    metadata,
  };
}

/** Verify every browser-visible same-origin site response against frozen site bytes. */
export async function watchOfflineSiteResponses(page,{base,snapshot,appShell,output,label='offline',
  controlledResponses=new Map(),resourceRouteDiscovery=false}={}){
  assert.match(label,/^[A-Za-z0-9_-]+$/,'Response evidence label must be a basename');
  const plan=appShell?appShellStatusPlan(base,appShell,snapshot):null;
  const expectedResponse=async (url,response)=>{
    const control=controlledResponses.get(url.pathname+url.search);
    if(control){
      const {body,...identity}=control;
      assert.equal(control.kind,'controlled-resource-response','Response control must have an explicit contract');
      return{expected:identity,validate:({response,record})=>{
        assert.equal(response.status(),control.status,'Controlled response HTTP status changed');
        assert.equal(response.fromServiceWorker(),false,'Cold failure control unexpectedly came from a Service Worker');
        const notRead=control.status===404&&record.actual.bytes===0;
        if(notRead){
          assert.equal(record.headers['content-length'],String(control.bytes),'Unread control response length header changed');
          assert.equal(record.headers['cache-control'],'no-store','Unread control response must not be cached');
        }else{
          assert.equal(record.actual.bytes,control.bytes,'Controlled response size changed');
          assert.equal(record.actual.sha256,control.sha256,'Controlled response hash changed');
        }
        return{kind:control.kind,purpose:control.purpose,path:url.pathname+url.search,
          bodyConsumption:notRead?'not-read':'complete',...record.actual};
      }};
    }
    if(resourceRouteDiscovery&&url.pathname==='/_eagler/resource-routes'){
      const resource=url.searchParams.get('path');
      assert.deepEqual([...url.searchParams.keys()],['path'],'Unexpected static route-discovery query');
      assert.ok(resource&&resource.startsWith('/')&&!resource.startsWith('//'),'Static route discovery needs an absolute resource path');
      const target=new URL(resource,url);
      assert.equal(target.origin,url.origin,'Static route discovery cannot leave this origin');
      assert.ok(physicalPathForUrl(snapshot,target),'Static route discovery referenced an unpublished resource');
      return{expected:{kind:'static-resource-route-discovery',resource},validate:({response,record,bytes})=>{
        assert.equal(response.status(),404,'Static serve-static route discovery must be unavailable');
        if(bytes.length===0)assert.equal(record.headers['content-length'],'9','Unread static 404 response length differs from serve-static');
        else assert.equal(bytes.toString('utf8'),'Not found','Static route-discovery body differs from serve-static');
        assert.match(record.headers['content-type'],/^text\/plain\b/);
        return{kind:'static-resource-route-discovery',resource,status:404,bodyConsumption:bytes.length?'complete':'not-read',...record.actual};
      }};
    }
    if(plan&&url.href===plan.statusUrl){
      return{kind:'app-shell-update-status',expected:{kind:'app-shell-update-status',buildId:appShell.buildId,
        entryCount:plan.entries.size,semantic:'app-shell-sw.js status metadata and frozen manifest bytes'},
        validate:input=>validateAppShellStatus({...input,plan})};
    }
    const supplied=physicalPathForUrl(snapshot,url);
    assert.ok(supplied,`Browser requested a same-origin path outside the frozen site: ${url.href}`);
    return{bytes:supplied.record.bytes,sha256:supplied.record.sha256,sitePath:supplied.urlPath};
  };
  return createFixtureResponseProbe(page,{base,expectedResponse,output,label});
}

export async function verifyOfflineSiteIdentity(snapshot){
  const current=await captureOfflineSiteIdentity(snapshot.root);
  assert.deepEqual(current.identity,snapshot.identity,'Packaged Launcher site changed during offline acceptance');
  return current.identity;
}
