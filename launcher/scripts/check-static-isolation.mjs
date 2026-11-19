// Exercise the actual static server over HTTP, without browser or game assets.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {once} from 'node:events';
import {mkdtemp,mkdir,writeFile,readFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const script=resolve(process.argv[2]),report=process.argv[3];
const root=await mkdtemp(resolve(tmpdir(),'th14-isolation-'));
const runtime='/runtime/th14/'+'0'.repeat(64)+'/th14.html';
await mkdir(resolve(root,'runtime/th14','0'.repeat(64)),{recursive:true});
await mkdir(resolve(root,'sub'));
await writeFile(resolve(root,'index.html'),'TH14 isolation fixture');
await writeFile(resolve(root,'sub/index.html'),'redirect fixture');
await writeFile(resolve(root,'.'+runtime),'TH14 runtime fixture');
const rows=[];
async function freePort(){const s=createServer();s.listen(0,'127.0.0.1');await once(s,'listening');const p=s.address().port;await new Promise(r=>s.close(r));return p;}
for(const enabled of [false,true]){
 const port=await freePort(),child=spawn(process.execPath,[script,root,String(port)],{env:{...process.env,EAGLER_TOUHOU_HOST:'127.0.0.1',EAGLER_TOUHOU_CROSS_ORIGIN_ISOLATION:enabled?'1':'0'},stdio:['ignore','pipe','pipe']});
 let output='';child.stdout.on('data',x=>output+=x);child.stderr.on('data',x=>output+=x);
 const exit=once(child,'exit');
 try{
  const deadline=Date.now()+10000;
  while(!output.includes('Serving immutable site directory:')){assert.ok(Date.now()<deadline,output);assert.equal(child.exitCode,null,output);await new Promise(r=>setTimeout(r,10));}
  let etag;
  for(const [name,path,options,status] of [
   ['root','/',{},200],['runtime-head',runtime,{method:'HEAD'},200],
   ['runtime-304',runtime,{headers:{'If-None-Match':()=>etag}},304],
   ['missing','/missing',{},404],['legacy-redirect','/eagler-touhou',{},302],['directory-redirect','/sub',{},308]]){
   if(name==='runtime-304')options.headers['If-None-Match']=etag;
   const response=await fetch(`http://127.0.0.1:${port}${path}`,{...options,redirect:'manual',signal:AbortSignal.timeout(5000)});
   assert.equal(response.status,status,name);const h=response.headers;
   if(name==='runtime-head'){etag=h.get('etag');assert.ok(etag);assert.ok(h.get('cache-control').includes('no-transform'));}
   for(const [key,value]of [['cross-origin-opener-policy','same-origin'],['cross-origin-embedder-policy','require-corp'],['cross-origin-resource-policy','same-origin']])assert.equal(h.get(key),enabled?value:null,`${enabled} ${name} ${key}`);
   rows.push({enabled,name,status,passed:true});await response.arrayBuffer();
  }
 }finally{child.kill();await exit;}
}
const q={passed:true,cases:rows.length,rows,actualHttpServerExecuted:true,sourceSha256:createHash('sha256').update(await readFile(script)).digest('hex'),fixtureRoot:root,silent:true,gameAssetsUsed:false,completeGame:false};
if(report)await writeFile(report,JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify({passed:true,cases:rows.length,sourceSha256:q.sourceSha256}));
