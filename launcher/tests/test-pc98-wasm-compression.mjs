import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer,request} from 'node:http';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,resolve} from 'node:path';
import {gunzipSync} from 'node:zlib';
import {acceptsGzip,pc98CompressedPath,preparePc98WasmGzip} from '../lib/pc98-wasm-compression.mjs';

const root=await mkdtemp(resolve(tmpdir(),'pc98-wasm-'));
const path=`runtime/th04/multiplayer/${'a'.repeat(64)}/vendor/dosbox-x/wdosbox-x-rollback.wasm`;
const raw=await readFile(new URL('../../ports/th04/web/vendor/dosbox-x/wdosbox-x-rollback.wasm',import.meta.url));
let server;
try{
  for(const game of ['th02','th03','th04'])for(const name of ['wdosbox-x-rollback.wasm','wdosbox-x-rollback-tracked.wasm']){
    const dest=`runtime/${game}/multiplayer/${'a'.repeat(64)}/vendor/dosbox-x/${name}`,source=await readFile(new URL(`../../ports/${game}/web/vendor/dosbox-x/${name}`,import.meta.url));
    await mkdir(dirname(resolve(root,dest)),{recursive:true});await writeFile(resolve(root,dest),source);
  }
  assert.equal(await preparePc98WasmGzip(root),6);
  const compressed=await readFile(pc98CompressedPath(root,path));assert.deepEqual(gunzipSync(compressed),raw);assert(compressed.length<raw.length);
  assert(!acceptsGzip('gzip;q=0, br'));assert(acceptsGzip('br, gzip;q=0.5'));
  const probe=createServer();await new Promise(r=>probe.listen(0,'127.0.0.1',r));const port=probe.address().port;await new Promise(r=>probe.close(r));
  server=spawn(process.execPath,['scripts/serve-static.mjs',root,String(port)],{cwd:resolve(import.meta.dirname,'..'),windowsHide:true,stdio:['ignore','pipe','pipe']});
  await new Promise((yes,no)=>{server.stdout.on('data',()=>yes());server.once('exit',code=>no(Error('server exited '+code)));});
  const get=encoding=>new Promise((yes,no)=>{request({host:'127.0.0.1',port,path:'/'+path,headers:{'Accept-Encoding':encoding}},r=>{const chunks=[];r.on('data',b=>chunks.push(b));r.on('end',()=>yes({headers:r.headers,body:Buffer.concat(chunks),status:r.statusCode}));}).on('error',no).end();});
  const zipped=await get('gzip');assert.equal(zipped.status,200);assert.equal(zipped.headers['content-encoding'],'gzip');assert.equal(zipped.headers['content-type'],'application/wasm');assert.equal(Number(zipped.headers['content-length']),compressed.length);assert.deepEqual(gunzipSync(zipped.body),raw);
  const response=await fetch(`http://127.0.0.1:${port}/${path}`,{headers:{'Accept-Encoding':'gzip'}});assert.equal(response.headers.get('content-encoding'),'gzip');await WebAssembly.compileStreaming(response);
  const plain=await get('gzip;q=0');assert.equal(plain.headers['content-encoding'],undefined);assert.deepEqual(plain.body,raw);
  console.log(`PC-98 WASM: ${raw.length} -> ${compressed.length} bytes; HTTP gzip and uncompressed fallback passed`);
}finally{server?.kill();await rm(root,{recursive:true,force:true});}
