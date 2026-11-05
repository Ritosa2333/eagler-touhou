import test from 'node:test';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
import {releaseServer} from '../../scripts/release-server.mjs';
test('Public release serves only allowlisted files with ranges, hashes and private development paths excluded',async()=>{
 const run=await releaseServer({root:fileURLToPath(new URL('../../artifacts/sdl-release/site',import.meta.url)),port:0});
 try{
  const get=path=>fetch(run.url+path);const root=await get('/');assert.equal(root.status,200);assert.match(await root.text(),/东方花映塚/);assert.match(root.headers.get('content-security-policy'),/wasm-unsafe-eval/);
  for(const path of ['/th09.exe','/score.dat','/save/score.dat','/package.json','/scripts/serve.mjs','/probe.mjs','/artifacts/cpp/analysis/jp/functions/00401000.c','/cloudflared-token.txt','/%2e%2e/%2e%2e/chatgpt2api/cloudflared-token.txt'])assert.equal((await get(path)).status,404,path);
  assert.equal((await fetch(run.url+'/manifest.json',{method:'POST',body:'x'})).status,405);
  const manifest=await(await get('/manifest.json')).json();assert.equal(manifest.game,'th09');assert.equal(manifest.execution.kind,'cpp-sdl3');
  const wasm=await get(manifest.execution.wasm),tag=wasm.headers.get('etag'),bytes=await wasm.arrayBuffer();assert.equal(wasm.headers.get('content-type'),'application/wasm');assert.match(wasm.headers.get('cache-control'),/immutable/);assert.ok(bytes.byteLength>2000000);const module=new WebAssembly.Module(bytes);assert.equal(WebAssembly.Module.exports(module).some(e=>e.name.startsWith('th09_probe_')),false);
  const unchanged=await fetch(run.url+manifest.execution.wasm,{headers:{'if-none-match':tag}});assert.equal(unchanged.status,304);
  const range=await fetch(run.url+manifest.execution.wasm,{headers:{range:'bytes=0-7'}});assert.equal(range.status,206);assert.equal((await range.arrayBuffer()).byteLength,8);
  const invalid=await fetch(run.url+manifest.execution.wasm,{headers:{range:'bytes=999999999-'}});assert.equal(invalid.status,416);
  const head=await fetch(run.url+'/th09.dat',{method:'HEAD'});assert.equal(head.status,200);assert.equal(Number(head.headers.get('content-length')),88180006);assert.equal((await head.arrayBuffer()).byteLength,0);
  const worker=await get('/app-shell-sw.js');assert.equal(worker.headers.get('service-worker-allowed'),'/');assert.match(await worker.text(),/clients.claim/);
 }finally{run.netplay.close();await new Promise(r=>run.server.close(r));}
});
