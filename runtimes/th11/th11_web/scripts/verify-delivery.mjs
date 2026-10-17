// Distribution integrity and HTTP checks, separate from full gameplay acceptance.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {spawn} from 'node:child_process';
import {releaseServer} from '../../th09_web/scripts/release-server.mjs';
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..');
const read=p=>JSON.parse(readFileSync(p,'utf8'));
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const release=resolve(root,'artifacts/sdl-release'),site=resolve(release,'site');
const manifest=read(resolve(site,'manifest.json')),build=read(resolve(release,'build.json'));
assert.equal(manifest.game,'th11');assert.equal(manifest.development,false);
assert.equal(manifest.execution.sha256,sha(resolve(site,'runtime/th11/th11.wasm')));
assert.equal(manifest.execution.loaderSha256,sha(resolve(site,'runtime/th11/th11.mjs')));
assert.equal(build.sha256,manifest.execution.sha256);
for(const [p,h]of Object.entries({...build.sources,...build.headers}))assert.equal(sha(resolve(workspace,p)),h,'source '+p);
for(const [p,h]of Object.entries(read(resolve(root,'config/final-baseline.json')).files))assert.equal(sha(resolve(workspace,p)),h,'baseline '+p);
for(const [url,file]of Object.entries(manifest.files)){
 const p=resolve(site,file.path);assert.equal(readFileSync(p).length,file.bytes,url+' length');assert.equal(sha(p),file.sha256,url+' SHA-256');
}
for(const p of ['th10_web/tools/node.exe','tools/python/python.exe','tools/emsdk/install/bin/clang++.exe',
 'th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe','tools/architecture/typescript/node_modules/typescript/bin/tsc',
 'th10_web/tools/wasi-sdk-34.0-x86_64-windows/share/wasi-sysroot/include/c++/v1',
 'th08_web/node_modules/@alexaltea/unicorn-js/dist/unicorn_x86.js','deploy/server.mjs',
 '启动地灵殿网页版.cmd','重新编译地灵殿.cmd'])assert.ok(existsSync(resolve(workspace,p)),p);
const server=await releaseServer({root:site,port:0});
const deploymentPort=server.server.address().port;
try{
 const remote=await fetch(server.url+'/manifest.json');assert.equal(remote.status,200);assert.equal((await remote.json()).version,manifest.version);
 const html=await fetch(server.url+'/');assert.equal(html.status,200);
 assert.equal(html.headers.get('cross-origin-opener-policy'),'same-origin');assert.equal(html.headers.get('cross-origin-embedder-policy'),'require-corp');
 const wasm=await fetch(server.url+manifest.execution.wasm,{headers:{Range:'bytes=0-7'}});assert.equal(wasm.status,206);assert.equal(Buffer.from(await wasm.arrayBuffer()).subarray(0,4).toString('hex'),'0061736d');
 for(const path of ['/../README-交付说明.md','/th11.exe','/target.json','/cloudflared-token.txt','/scripts/serve.mjs'])assert.equal((await fetch(server.url+path)).status,404,path);
 const result={passed:false,checkedAt:new Date().toISOString(),version:manifest.version,wasm:build.sha256,
  files:Object.keys(manifest.files).length,sourceHashes:true,baselineHashes:true,releaseHashes:true,httpStartup:true,httpRange:true,
  isolatedHeaders:true,privatePaths404:true,scope:'Relocated distribution files, source/build identity and release-server behavior. Not a full gameplay acceptance rerun.'};
 mkdirSync(resolve(workspace,'verification'),{recursive:true});writeFileSync(resolve(workspace,'verification/distribution.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
}finally{server.netplay.close();server.server.closeAllConnections();await new Promise(r=>server.server.close(r));}
// Exercise the actual deployment entry too: it normalizes a file-URL path,
// whereas releaseServer deliberately accepts only a canonical site root.
const child=spawn(process.execPath,[resolve(workspace,'deploy/server.mjs')],{
 cwd:workspace,env:{...process.env,HOST:'127.0.0.1',PORT:String(deploymentPort)},windowsHide:true,stdio:['ignore','pipe','pipe']});
let log='';const exited=new Promise(r=>child.once('exit',r));
try{
 await new Promise((yes,no)=>{
  const timer=setTimeout(()=>no(Error('Deployment startup timeout: '+log)),10000);
  child.once('error',e=>{clearTimeout(timer);no(e);});
  child.once('exit',code=>{clearTimeout(timer);no(Error('Deployment exited '+code+': '+log));});
  child.stderr.on('data',b=>log+=b);
  child.stdout.on('data',b=>{log+=b;if(log.includes('http://127.0.0.1:')){clearTimeout(timer);yes();}});
 });
 const response=await fetch('http://127.0.0.1:'+deploymentPort+'/manifest.json');assert.equal(response.status,200);
 assert.equal((await response.json()).version,manifest.version);
 const p=resolve(workspace,'verification/distribution.json'),result=read(p);result.deploymentEntryStartup=true;result.passed=true;
 writeFileSync(p,JSON.stringify(result,null,2)+'\n');console.log('Deployment entry startup passed.');
}finally{if(child.exitCode===null){child.kill();await exited;}}
