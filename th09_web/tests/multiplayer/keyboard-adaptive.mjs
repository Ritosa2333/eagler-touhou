import {spawnSync} from 'node:child_process';
import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=fileURLToPath(new URL('../../',import.meta.url)),common=resolve(root,'../third_party/eagler-common');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const compiler=resolve(process.env.WASI_SDK_BIN||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin','clang++.exe');
const wasm=resolve(out,'keyboard-adaptive.wasm');
const sources=[resolve(root,'tests/multiplayer/keyboard-adaptive.cpp'),...['NetplayCore','NetplayProtocol'].map(n=>resolve(common,'src/netplay',n+'.cpp'))];
const compile=spawnSync(compiler,['-std=c++17','-O2','-fno-exceptions','-fno-rtti','-I'+resolve(common,'include'),'-I'+resolve(root,'cpp/multiplayer'),'-Wl,-z,stack-size=1048576',...sources,'-o',wasm],{windowsHide:true,encoding:'utf8'});
if(compile.status!==0)throw Error(compile.stderr||String(compile.error));
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const bytes=[];const original=wasi.wasiImport.fd_write;
// Collect the WASI program's JSON without replacing the prediction logic.
// Other file descriptors keep normal WASI behavior.
let runner;
const imports={...wasi.wasiImport,fd_write:(fd,iovs,count,written)=>{
 if(fd!==1)return original(fd,iovs,count,written);
 const memory=new Uint8Array(runner.exports.memory.buffer),view=new DataView(memory.buffer);let size=0;
 for(let i=0;i<count;++i){const p=view.getUint32(iovs+i*8,true),n=view.getUint32(iovs+i*8+4,true);bytes.push(memory.slice(p,p+n));size+=n;}
 view.setUint32(written,size,true);return 0;
}};
({instance:runner}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:imports}));
if(wasi.start(runner)!==0)throw Error('prediction probe failed');
const rows=JSON.parse(Buffer.concat(bytes).toString());
assert.equal(rows.length,12);
for(const row of rows){assert.equal(row.tested+row.stalls,2400);assert.equal(row.mismatches,row.falseStop+row.overHoldAfterRelease+row.wrongDirection);}
const sha=b=>createHash('sha256').update(b).digest('hex');
const report={passed:true,scope:'Candidate stable-hold selector over two actual common-core predictors; synthetic discrete keyboard traces; no production policy, gameplay, timing or human-trace claim',wasmSha256:sha(readFileSync(wasm)),sources:Object.fromEntries(sources.map(p=>[p,sha(readFileSync(p))])),rows};
writeFileSync(resolve(out,'keyboard-adaptive-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report.rows));
