// Materialization only: reuse the exact frozen release engine, do not compile.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), web=resolve(root,'th09_web');
const compiled=resolve(process.env.TH09_OUTPUT||''), assets=resolve(process.env.TH09_RUNTIME_ASSETS||'');
assert.ok(process.env.TH09_OUTPUT&&process.env.TH09_RUNTIME_ASSETS,'explicit frozen engine and asset roots are required');
const sha=b=>createHash('sha256').update(b).digest('hex');
const identity=p=>{const b=readFileSync(p);return {path:p,bytes:b.length,sha256:sha(b)};};
const expectedManaged='830f521ffa13c3be443c8bb277da510734c9625efcb224b1d1ed6ae3e7eee4f4';
const expectedWasm='037c9cd52e70e2c8e8ee516cf15b6aee7f0de5951f88d519b2311cc1c5def373';
const sources=[
 resolve(web,'scripts/build-eagler.mjs'),resolve(web,'tests/multiplayer/managed-touch-materialize.mjs'),
 ...['managed.html','managed.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs','shared-netplay.mjs','motion-replay.mjs','startup-branding.mjs'].map(n=>resolve(web,'sdl-runtime',n)),
 ...['th09.wasm','th09.mjs','build.json'].map(n=>resolve(compiled,n)),
 ...['cp932.bin','blend.bin'].map(n=>resolve(assets,n)),
];
const before=sources.map(identity);
assert.equal(identity(resolve(web,'sdl-runtime/managed.mjs')).sha256,expectedManaged);
assert.equal(identity(resolve(compiled,'th09.wasm')).sha256,expectedWasm);
const output=resolve(root,'docs/multiplayer/rollback-managed-touch-runtime-materialization.json');
const variants=['normal','multiplayer'];
for(const p of [output,...variants.map(v=>resolve(web,v==='normal'?'build-eagler':'build-eagler-multiplayer'))])assert.equal(existsSync(p),false,'refuse to overwrite '+p);
const runs=[];
for(const variant of variants){
 const args=[resolve(web,'scripts/build-eagler.mjs'),...(variant==='multiplayer'?['--multiplayer']:[])];
 const result=spawnSync(process.execPath,args,{cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:60000,maxBuffer:1024*1024});
 process.stdout.write(result.stdout||'');process.stderr.write(result.stderr||'');
 assert.equal(result.error,undefined);assert.equal(result.status,0,variant+' materialization failed');
 const dir=resolve(web,variant==='normal'?'build-eagler':'build-eagler-multiplayer');
 const inventory=JSON.parse(readFileSync(resolve(dir,'runtime-files.json'),'utf8'));
 assert.equal(inventory.schema,'eagler-touhou/runtime-directory/1');
 for(const [name,item] of Object.entries(inventory.files)){
  assert.ok(!name.includes('..')&&!name.startsWith('/')&&!name.includes('\\'),'safe exact inventory key');
  const actual=identity(resolve(dir,name));assert.equal(actual.bytes,item.bytes);assert.equal(actual.sha256,item.sha256);
 }
 assert.equal(inventory.files['shell.mjs'].sha256,expectedManaged);assert.equal(inventory.files['th09.wasm'].sha256,expectedWasm);
 assert.equal(inventory.files['th09.mjs'].sha256,identity(resolve(compiled,'th09.mjs')).sha256);
 runs.push({variant,root:dir,inventory:identity(resolve(dir,'runtime-files.json')),files:inventory.files,
  version:JSON.parse(readFileSync(resolve(dir,'version.json'),'utf8')),args,status:result.status,stdout:result.stdout,stderr:result.stderr});
}
const after=sources.map(identity);assert.deepEqual(after,before);
const result={schema:'th09-managed-touch-runtime-materialization/v1',createdAt:new Date().toISOString(),scope:'Directory Runtime materialization from frozen engine; no compiler, deployment or browser acceptance',
 expectedManaged,expectedWasm,sourceBefore:before,sourceAfter:after,sourceStable:true,runs};
writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({output,identity:identity(output),sourceStable:true,variants:variants.length,wasm:expectedWasm}));
