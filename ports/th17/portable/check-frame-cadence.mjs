import {readFile,mkdtemp,rm} from 'node:fs/promises';
import {resolve,join} from 'node:path';
import {tmpdir} from 'node:os';
import {execFileSync} from 'node:child_process';
import {WASI} from 'node:wasi';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
const input=resolve(import.meta.dirname,'check-frame-cadence.cpp');
if(process.env.TH17_WASI_CXX){
 const directory=await mkdtemp(join(tmpdir(),'th17-cadence-'));
 try{
  const path=resolve(directory,'clock.wasm');
  execFileSync(process.env.TH17_WASI_CXX,['--target=wasm32-wasip1','-O2','-std=c++17',input,'-o',path],{windowsHide:true});
  const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(await readFile(path),{wasi_snapshot_preview1:wasi.wasiImport});
  if(wasi.start(instance)!==0)throw Error('TH17 cadence verification failed');
 }finally{await rm(directory,{recursive:true,force:true});}
}else await runNativeCpp(await readFile(input,'utf8'),{includes:[import.meta.dirname]});
console.log(JSON.stringify({passed:true,game:'th17',logicHz:60,seconds:60,expectedTicks:3600,refreshHz:[15,20,30,50,59,60,75,90,120,144,165],jitter:true,pauseClearsDebt:true}));
