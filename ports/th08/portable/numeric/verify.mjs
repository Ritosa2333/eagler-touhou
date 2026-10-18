import {spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {WASI} from 'node:wasi';
const root=resolve(fileURLToPath(new URL('../../',import.meta.url))),out=resolve(root,'artifacts/numeric'),bin=resolve(root,'th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin');mkdirSync(out,{recursive:true});
const flags=['--target=wasm32-wasip1','-O2','-ffp-contract=off','-fno-strict-aliasing'];
function run(exe,args){const r=spawnSync(resolve(bin,exe),args,{cwd:root,encoding:'utf8',windowsHide:true});if(r.error)throw r.error;if(r.status)throw Error(r.stdout+r.stderr);}
run('clang.exe',[...flags,'-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5','-c','th08_web/cpp/third_party/softfloat.c','-o',resolve(out,'softfloat.o')]);
run('clang++.exe',[...flags,'-std=c++17','-fno-exceptions','-fno-rtti','-mexec-model=reactor','-Wl,--no-entry','-Wl,--export-memory','portable/numeric/verify.cpp','th08_web/cpp/game/Arithmetic.cpp','th10_web/cpp/game/Arithmetic.cpp',resolve(out,'softfloat.o'),'-o',resolve(out,'verify.wasm')]);
const wasi=new WASI({version:'preview1',args:[],env:{}}),{instance}=await WebAssembly.instantiate(readFileSync(resolve(out,'verify.wasm')),{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);
const start=performance.now(),failure=instance.exports.verify(50000),checks=instance.exports.checks(),details=Array.from(new Uint32Array(instance.exports.memory.buffer,instance.exports.details(),12));
const result={passed:!failure,operandPairs:600000,checks,milliseconds:performance.now()-start,failure,details};writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));if(failure)process.exitCode=1;
