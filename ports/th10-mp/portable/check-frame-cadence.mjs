import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const executable=process.platform==='win32'?'clang++.exe':'clang++';
const sdk=process.env.WASI_SDK_PATH??[
 resolve(root,'../toolchains/wasi-sdk-34.0-x86_64-windows'),
 resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path=>existsSync(resolve(path,'bin',executable)));
if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
mkdirSync(resolve(root,'artifacts'),{recursive:true});
const path=resolve(root,'artifacts/frame-cadence.wasm');
execFileSync(resolve(sdk,'bin',executable),['--target=wasm32-wasip1','-O2','-std=c++17','-Iportable','portable/check-frame-cadence.cpp','-o',path],{cwd:root,windowsHide:true});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(path),{wasi_snapshot_preview1:wasi.wasiImport});
if(wasi.start(instance))throw Error('Cadence regression');
const result={passed:true,scope:'ordinary single-player cadence, not live MP catch-up',displayHz:[15,20,30,60,90,120,144,165],seconds:60,missedDeadlinePolicy:'skip-expired-single-tick',maxTicksPerCallback:1,pauseClearsDebt:true};
writeFileSync(resolve(root,'artifacts/frame-cadence.json'),JSON.stringify(result,null,2));console.log(result);
