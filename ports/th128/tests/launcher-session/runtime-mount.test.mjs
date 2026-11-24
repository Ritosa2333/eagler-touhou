import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync,readFileSync} from 'node:fs';
import {fetchResource,installResourceFetchProvider,resetResourceFetchProvider} from '../../site/resource-fetch.mjs';
import {readOnlineDeployment} from '../../site/runtime/online-resources.mjs';

const root=new URL('../../site/',import.meta.url);
const parent='https://launcher.invalid';
const mount='/runtime/th128/0123456789abcdef/';
const hosted=parent+mount+'th128.html?hosted=1&runtimeEpoch=2';

test('the shipped HTML loads its CSS and bootstrap from the selected Runtime directory',()=>{
 const html=readFileSync(new URL('play.html',root),'utf8');
 const local=[...html.matchAll(/<(?:link\b[^>]*\brel="stylesheet"[^>]*\bhref|script\b[^>]*\bsrc)="([^"]+)"/g)].map(row=>row[1]);
 assert.ok(local.some(value=>new URL(value,hosted).pathname.endsWith('/import-entry.mjs')));
 for(const resource of local){
  const url=new URL(resource,hosted);
  assert.equal(url.origin,parent);
  assert.ok(url.pathname.startsWith(mount),`${url.pathname} escapes ${mount}`);
  assert.ok(existsSync(new URL(url.pathname.slice(mount.length),root)),url.pathname);
 }
});

test('the local manifest follows the Runtime mount while asset requests stay with the package owner',async()=>{
 const originalLocation=globalThis.location,originalFetch=globalThis.fetch;
 const network=[],owned=[];
 globalThis.fetch=async input=>{network.push(String(input));return new Response('local');};
 installResourceFetchProvider({async fetch(input){owned.push(String(input));return new Response('owned');}});
 try{
  for(const page of [parent+'/play.html',hosted]){
   globalThis.location=new URL(page);
   const build=new URL('runtime/build.json',page).href;
   assert.equal(await (await fetchResource(build)).text(),'local');
   assert.equal(await (await fetchResource('/assets/catalog.json')).text(),'owned');
  }
  assert.equal(await (await fetchResource(parent+'/runtime/build.json')).text(),'owned');
  assert.equal(await (await fetchResource(parent+mount+'runtime/build.json?wrong=1')).text(),'owned');
  assert.equal(await (await fetchResource(parent+mount+'runtime/build.json',{method:'POST'})).text(),'owned');
  assert.deepEqual(network,[parent+'/runtime/build.json',parent+mount+'runtime/build.json']);
  assert.deepEqual(owned,['/assets/catalog.json','/assets/catalog.json',parent+'/runtime/build.json',parent+mount+'runtime/build.json?wrong=1',parent+mount+'runtime/build.json']);
 }finally{
  resetResourceFetchProvider();
  globalThis.location=originalLocation;
  globalThis.fetch=originalFetch;
 }
});

test('a nested standalone bootstrap does not consult the host root deployment manifest',async()=>{
 const requests=[];
 const fetchImpl=async url=>{requests.push(url);return new Response(null,{status:404});};
 assert.equal(await readOnlineDeployment({pageURL:hosted,fetchImpl}),null);
 assert.deepEqual(requests,[parent+mount+'deployment.json']);
});
