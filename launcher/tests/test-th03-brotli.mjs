import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,readFile,rm,utimes} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';
import {brotliDecompressSync} from 'node:zlib';
import {spawn} from 'node:child_process';
import {createServer,request} from 'node:http';
import {prepareTh03Brotli,th03BrotliSidecar,acceptsBrotli} from '../lib/th03-brotli.mjs';
const root=await mkdtemp(resolve(tmpdir(),'th03-brotli-'));
try{
 const paths=[`runtime/th03/multiplayer/${'a'.repeat(64)}/native-classic.wasm`,'games/th03/th03.data'];
 const raw=Buffer.from('TH03 native test '.repeat(400));
 for(const path of paths){const file=resolve(root,path);await mkdir(dirname(file),{recursive:true});await writeFile(file,raw);}
 assert.equal(await prepareTh03Brotli(root),2);
 for(const path of paths){const file=resolve(root,path),encoded=await readFile(file+'.br');assert.deepEqual(brotliDecompressSync(encoded),raw);assert(encoded.length<raw.length);assert(await th03BrotliSidecar(file,path,'gzip, br'));assert.equal(await th03BrotliSidecar(file,path,'gzip, br;q=0'),null);await utimes(file,new Date(Date.now()+2000),new Date(Date.now()+2000));assert.equal(await th03BrotliSidecar(file,path,'br'),null,'stale compressed content is ignored');}
 const file=resolve(root,paths[0]);await writeFile(file,raw);await prepareTh03Brotli(root);
 const probe=createServer();await new Promise(r=>probe.listen(0,'127.0.0.1',r));const port=probe.address().port;await new Promise(r=>probe.close(r));
 const server=spawn(process.execPath,['scripts/serve-static.mjs',root,String(port)],{cwd:resolve(import.meta.dirname,'..'),windowsHide:true,stdio:['ignore','pipe','pipe']});
 try{await new Promise((yes,no)=>{server.stdout.once('data',yes);server.once('exit',no);});const get=encoding=>new Promise((yes,no)=>{request({host:'127.0.0.1',port,path:'/'+paths[0],headers:{'Accept-Encoding':encoding}},r=>{const chunks=[];r.on('data',b=>chunks.push(b));r.on('end',()=>yes({headers:r.headers,body:Buffer.concat(chunks)}));}).on('error',no).end();});const response=await get('br, gzip');assert.equal(response.headers['content-encoding'],'br');assert.deepEqual(brotliDecompressSync(response.body),raw);assert.equal(+response.headers['content-length'],response.body.length);const plain=await get('br;q=0');assert.equal(plain.headers['content-encoding'],undefined);assert.deepEqual(plain.body,raw);}finally{server.kill();}
 assert(!acceptsBrotli('br;q=0, gzip'));assert(acceptsBrotli('gzip, br;q=0.5'));
 console.log('TH03 Brotli generation, round trip and stale-file fallback passed');
}finally{await rm(root,{recursive:true,force:true});}
