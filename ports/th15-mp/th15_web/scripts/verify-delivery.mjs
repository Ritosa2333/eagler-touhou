// Delivery integrity and actual server startup; separate from full gameplay acceptance.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {spawn} from 'node:child_process';
import {releaseServer} from '../../th09_web/scripts/release-server.mjs';
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..');
const read=p=>JSON.parse(readFileSync(p,'utf8')),sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const release=resolve(root,process.env.TH15_VERIFY_RELEASE||'artifacts/sdl-release'),site=resolve(release,'site');
const manifest=read(resolve(site,'manifest.json')),build=read(resolve(release,'build.json')),web=read(resolve(root,'reference/web-release.json'));
assert.equal(manifest.game,'th15');assert.equal(manifest.development,false);assert.equal(manifest.completeGame,true);
assert.equal(manifest.webVersion,web.version);assert.equal(manifest.releaseDate,web.date);
assert.equal(manifest.execution.sha256,sha(resolve(site,'runtime/th15/th15.wasm')));
assert.equal(manifest.execution.loaderSha256,sha(resolve(site,'runtime/th15/th15.mjs')));
assert.equal(build.sha256,manifest.execution.sha256);
for(const [p,h] of Object.entries({...build.sources,...build.headers}))assert.equal(sha(resolve(workspace,p)),h,'source '+p);
const extensions=new Map(read(resolve(root,'reference/shared-architecture-extensions.json')).changes.map(e=>[e.path,e]));
for(const e of read(resolve(root,'reference/architecture-baseline.json')).sharedFiles){const update=extensions.get(e.path);if(update)assert.equal(update.baselineSha256,e.sha256);assert.equal(sha(resolve(workspace,e.path)),update?.currentSha256||e.sha256,'shared '+e.path);}
for(const [p,e] of extensions)assert.equal(sha(resolve(workspace,p)),e.currentSha256,'shared extension '+p);
for(const [url,file] of Object.entries(manifest.files)){const p=resolve(site,file.path);assert.equal(readFileSync(p).length,file.bytes,url+' bytes');assert.equal(sha(p),file.sha256,url+' hash');}
const exports=WebAssembly.Module.exports(new WebAssembly.Module(readFileSync(resolve(site,'runtime/th15/th15.wasm')))).map(e=>e.name);
assert.ok(!exports.some(n=>/application_|th15_probe|test_protection/.test(n)),'Production contains developer exports');
assert.ok(readFileSync(resolve(site,'index.html'),'utf8').includes('brand-version'));
assert.ok(readFileSync(resolve(site,'CHANGELOG.txt'),'utf8').startsWith('['+web.date+'] 绀珠传 WEB '+web.version+'\n'));
for(const p of ['th10_web/tools/node.exe','tools/python/python.exe','tools/emsdk/install/bin/clang++.exe','th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe','th10_web/tools/wasi-sdk-34.0-x86_64-windows/share/wasi-sysroot/include/c++/v1','tools/architecture/typescript/node_modules/typescript/bin/tsc','th08_web/node_modules/@alexaltea/unicorn-js/dist/unicorn_x86.js','th10_web/node_modules/playwright/index.mjs','deploy/server.mjs','开发环境.cmd','启动绀珠传网页版.cmd','重新编译绀珠传.cmd'])assert.ok(existsSync(resolve(workspace,p)),p);
const server=await releaseServer({root:site,port:0});let port;
const result={passed:false,checkedAt:new Date().toISOString(),webVersion:web.version,manifestVersion:manifest.version,wasm:build.sha256,sourceHashes:true,sharedArchitecture:true,releaseFiles:Object.keys(manifest.files).length,productionExports:true,scope:'Relocated delivery source/toolchain/release identity, all assets and HTTP/deployment startup. Full gameplay proofs are retained separately.'};
try{
 port=server.server.address().port;
 const response=await fetch(server.url+'/manifest.json');assert.equal(response.status,200);assert.equal((await response.json()).version,manifest.version);
 const html=await fetch(server.url+'/');assert.equal(html.status,200);assert.equal(html.headers.get('cross-origin-opener-policy'),'same-origin');assert.equal(html.headers.get('cross-origin-embedder-policy'),'require-corp');
 const wasm=await fetch(server.url+manifest.execution.wasm,{headers:{Range:'bytes=0-7'}});assert.equal(wasm.status,206);assert.equal(Buffer.from(await wasm.arrayBuffer()).subarray(0,4).toString('hex'),'0061736d');
 for(const p of ['/README-交付说明.md','/th15.exe','/target.json','/cloudflared-token.txt','/scripts/serve.mjs'])assert.equal((await fetch(server.url+p)).status,404,p);
 result.httpStartup=true;result.httpRange=true;result.isolatedHeaders=true;result.privatePaths404=true;
}finally{server.netplay.close();server.server.closeAllConnections();await new Promise(r=>server.server.close(r));}
// The real deployment entry is tested only against the bundled canonical release.
const child=spawn(process.execPath,[resolve(workspace,'deploy/server.mjs')],{cwd:workspace,env:{...process.env,HOST:'127.0.0.1',PORT:String(port)},windowsHide:true,stdio:['ignore','pipe','pipe']});
let log='';const exited=new Promise(r=>child.once('exit',r));
try{
 await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Deployment startup timeout: '+log)),15000);child.once('error',e=>{clearTimeout(timer);no(e);});child.once('exit',code=>{clearTimeout(timer);no(Error('Deployment exited '+code+': '+log));});child.stderr.on('data',b=>log+=b);child.stdout.on('data',b=>{log+=b;if(log.includes('http://127.0.0.1:')){clearTimeout(timer);yes();}});});
 const response=await fetch('http://127.0.0.1:'+port+'/manifest.json');assert.equal(response.status,200);assert.equal((await response.json()).webVersion,web.version);result.deploymentEntryStartup=true;result.passed=true;
 const report=resolve(workspace,process.env.TH15_DELIVERY_REPORT||'verification/distribution.json');mkdirSync(resolve(report,'..'),{recursive:true});writeFileSync(report,JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
}finally{if(child.exitCode===null){child.kill();await exited;}}
