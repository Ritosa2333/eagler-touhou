import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI SDK not found: '+compiler);
const common=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'third_party/eagler-common'));
const out=resolve(root,'artifacts/network-cadence-tests');mkdirSync(out,{recursive:true});
const result={passed:false,scope:'WASI correctness of scheduling and input-gap pacing; not a frame-time benchmark',cases:[]};
for(const name of ['network-frame-schedule','input-gap-pacing']){
  const source=resolve(root,'tests',name+'-test.cpp'),wasm=resolve(out,name+'.wasm');
  execFileSync(compiler,['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
    '-I'+resolve(common,'include'),source,'-o',wasm],{windowsHide:true,stdio:'inherit'});
  const bytes=readFileSync(wasm);
  const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});
  const status=wasi.start(instance);
  if(status!==0)throw Error(name+' failed: '+status);
  result.cases.push({name,passed:true,wasmSha256:createHash('sha256').update(bytes).digest('hex')});
}
result.passed=true;
writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify(result));
