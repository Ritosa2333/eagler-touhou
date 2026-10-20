import assert from 'node:assert/strict';
import { mkdtemp, writeFile, rm } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import http from 'node:http';
import { start } from './router.mjs';
const root = await mkdtemp(join(tmpdir(), 'eagler-router-'));
for (let i=6;i<14;i++) {
 const name=`th${String(i).padStart(2,'0')}`;
 await writeFile(join(root,`${name}.package.json`),JSON.stringify({files:{'game-data':{source:`games/${name}/${name}.data`,sha256:'a'.repeat(64),bytes:1}}}));
}
await writeFile(join(root,'deployment.json'),JSON.stringify({files:[]}));
const original=globalThis.fetch;
let calls=0,active=0,peak=0;
globalThis.fetch=async(url,options)=>{
 calls++;active++;peak=Math.max(peak,active);
 try {
  await new Promise(r=>setTimeout(r,15));
  if(url.endsWith('runtime-font-inventory.json'))return Response.json({files:[]});
  const match=/\/(th\d+)\.package\.json$/.exec(url);
  if(match)return Response.json({files:{'game-data':{source:`games/${match[1]}/${match[1]}.data`,sha256:'a'.repeat(64),bytes:1}}});
  assert.equal(options.headers.Range,'bytes=0-0');
  return new Response('x',{status:206,headers:{'content-range':'bytes 0-0/1','access-control-allow-origin':'*'}});
 } finally {active--;}
};
let service;
try {
 const pool={root,nodes:[{id:'node',origin:'https://resources.example',weight:1}]};
 service=await start({port:0,sites:{'touhou.vip':pool,'test.touhou.vip':pool,'beta.touhou.vip':pool}});
 const status=await new Promise((resolve,reject)=>http.get(`http://127.0.0.1:${service.server.address().port}/_status`,r=>{let text='';r.on('data',b=>text+=b);r.on('end',()=>resolve(JSON.parse(text)));}).on('error',reject));
 assert.equal(calls,17,'three sites share metadata and wildcard-CORS range probes');
 assert.ok(peak<=4,'all node probes respect the concurrency bound');
 for(const site of Object.values(status)){assert.equal(site.nodes[0].eligibleFiles,8);assert.deepEqual(site.nodes[0].errors,[]);}
 console.log('shared inventory and bounded probes verified');
} finally {globalThis.fetch=original;service?.close();}

const inventoryFile=join(root,'verified-inventory.json');
const inventory={};
for(let i=6;i<14;i++){const name=`th${String(i).padStart(2,'0')}`;inventory[`/games/${name}/${name}.data`]={sha256:(i===13?'b':'a').repeat(64),bytes:1};}
await writeFile(inventoryFile,JSON.stringify(inventory));
globalThis.fetch=()=>{throw new Error('verified publication must not depend on network probes');};
try {
 service=await start({port:0,sites:{'touhou.vip':{root,nodes:[{id:'published',origin:'https://resources.example',weight:1,inventoryFile}]}}});
 const status=await new Promise((resolve,reject)=>http.get(`http://127.0.0.1:${service.server.address().port}/_status`,r=>{let text='';r.on('data',b=>text+=b);r.on('end',()=>resolve(JSON.parse(text)));}).on('error',reject));
 assert.equal(status['touhou.vip'].nodes[0].eligibleFiles,7,'published inventory retains exact content identity gating');
 console.log('verified deployment inventory and mismatch exclusion verified');
} finally {globalThis.fetch=original;service?.close();}

await writeFile(join(root,'th075.package.json'),JSON.stringify({files:{language:{source:'/language-updates/th075/en.zip',sha256:'c'.repeat(64),bytes:4}}}));
const otherInventory=join(root,'other-inventory.json');
const matching={...inventory,'/language-updates/th075/en.zip':{sha256:'c'.repeat(64),bytes:4}};
await writeFile(otherInventory,JSON.stringify(matching));
await writeFile(inventoryFile,JSON.stringify(matching));
try {
 service=await start({port:0,sites:{'play.example':{root,nodes:[
  {id:'a',origin:'https://a.example',weight:1,inventoryFile},
  {id:'b',origin:'https://b.example',weight:1,inventoryFile:otherInventory},
 ]}}});
 const request=path=>new Promise((resolve,reject)=>http.get({hostname:'127.0.0.1',port:service.server.address().port,path,
  headers:{'X-Resource-Site':'play.example','X-Resource-Client':'client'}},response=>{
   let body='';response.on('data',chunk=>body+=chunk);response.on('end',()=>resolve({status:response.statusCode,headers:response.headers,body}));
  }).on('error',reject));
 const regular=await request('/games/th06/th06.data?v=abc');assert.equal(regular.status,307);
 const routes=await request('/_eagler/resource-routes?path='+encodeURIComponent('/games/th06/th06.data?v=abc'));assert.equal(routes.status,200);
 const value=JSON.parse(routes.body);assert.equal(value.sources[0],regular.headers.location);
 assert.equal(value.bytes,1);assert.equal(value.sha256,'a'.repeat(64));assert.equal(value.sources.length,2);
 assert(value.sources.every(url=>url.endsWith('/games/th06/th06.data?v=abc')));
 const language=await request('/_eagler/resource-routes?path='+encodeURIComponent('/language-updates/th075/en.zip'));assert.equal(language.status,200);
 assert.equal(JSON.parse(language.body).sources.length,2);
 assert.equal((await request('/_eagler/resource-routes?path='+encodeURIComponent('/games/missing/file.data'))).status,404);
 assert.equal((await request('/_eagler/resource-routes?path='+encodeURIComponent('/games/%2e%2e/private'))).status,404);
 console.log('ordered mirror discovery, version preservation, language paths and identity gating verified');
} finally {service?.close();await rm(root,{recursive:true,force:true});}
