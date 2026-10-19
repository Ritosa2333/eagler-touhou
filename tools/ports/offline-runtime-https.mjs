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
export async function createOfflineStaticTlsProxy({backendBase,port=0}={}){
  const backend=new URL(backendBase);
  assert.equal(backend.protocol,'http:','Static backend must use local HTTP');
  assert.equal(backend.hostname,'127.0.0.1','Static backend must bind IPv4 loopback');
  assert.ok(Number(backend.port)>0,'Static backend must use an explicit port');
  return createFixtureServer(async(request,response)=>{
    try{
      assert.ok(request.method==='GET'||request.method==='HEAD',`Unsupported static method ${request.method}`);
      const url=new URL(request.url,backend);
      assert.equal(url.origin,backend.origin,'Static proxy cannot leave the backend loopback origin');
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

/** Verify every browser-visible same-origin site response against frozen site bytes. */
export async function watchOfflineSiteResponses(page,{base,snapshot,output,label='offline'}={}){
  assert.match(label,/^[A-Za-z0-9_-]+$/,'Response evidence label must be a basename');
  const expectedResponse=async url=>{
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
