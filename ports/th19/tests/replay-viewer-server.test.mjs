import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,mkdir,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import http from 'node:http';
import {startReplayViewer} from '../tools/replay/viewer-server.mjs';
import {encodeTh19Replay,th19StreamFrame} from '../web/sdl-runtime/th19-recording.mjs';

test('local viewer serves only token-scoped resources, accepts decoded recordings from prior builds, and never launches a game',async()=>{
 const root=await mkdtemp(resolve(tmpdir(),'th19-viewer-test-'));
 const build='a'.repeat(64),config={protocol:1,build,seed:123,difficulty:2,characters:[0,18],delay:0,rollback:true,rules:[0,1,1,4,4,1,1,0,0]};
 const input={keys:0,direct:0,x:0,y:0};
 const bytes=encodeTh19Replay({kind:'versus',config,build,seed:123,frames:1,startedAt:0},th19StreamFrame(0,input,input,{played:true}));
 let server;
 try{
  await mkdir(resolve(root,'runtime'));await mkdir(resolve(root,'content'));
  await writeFile(resolve(root,'game-resources.json'),JSON.stringify({gameData:{source:'content/th19.dat'},music:{ogg:{files:[],base:'content/music/'}}}));
  await writeFile(resolve(root,'runtime/manifest.json'),JSON.stringify({buildId:build}));
  await writeFile(resolve(root,'runtime/th19.wasm'),new Uint8Array([0,97,115,109]));
  await writeFile(resolve(root,'secret.txt'),'not served');
  const file=resolve(root,'sample.rpy');await writeFile(file,bytes);
  const started=await startReplayViewer({root,file});server=started.server;
  assert.equal(server.address().address,'127.0.0.1');
  const get=path=>fetch(new URL(path,started.url));
  assert.match(await(await get('')).text(),/兽王园/);
  const info=await(await get('session.json')).json();assert.equal(info.build,build);assert.equal(info.hasReplay,true);
  assert.deepEqual(new Uint8Array(await(await get('recording.rpy')).arrayBuffer()),bytes);
  assert.equal((await get('runtime/th19.wasm')).headers.get('content-type'),'application/wasm');
  assert.equal((await get('secret.txt')).status,404);
  assert.equal((await get('/runtime/th19.wasm')).status,404);
  assert.equal((await get('content/%2e%2e%5csecret.txt')).status,404);
  assert.equal((await fetch(started.url,{method:'POST'})).status,405);
  const rejectedHost=await new Promise((ok,no)=>http.get(started.url,{headers:{host:'example.com'}},r=>{r.resume();ok(r.statusCode);}).on('error',no));
  assert.equal(rejectedHost,403);
  await writeFile(resolve(root,'runtime/manifest.json'),JSON.stringify({buildId:'b'.repeat(64)}));
  const compatible=await startReplayViewer({root,file});
  await new Promise(ok=>compatible.server.close(ok));
  await writeFile(file,new Uint8Array([1,2,3]));
  await assert.rejects(startReplayViewer({root,file}));
 }finally{
  if(server)await new Promise(ok=>server.close(ok));
  // root is the exact absolute path returned by mkdtemp, not user input.
  assert.ok(root.startsWith(resolve(tmpdir())+ (process.platform==='win32'?'\\':'/')));
  await rm(root,{recursive:true,force:true});
 }
});
