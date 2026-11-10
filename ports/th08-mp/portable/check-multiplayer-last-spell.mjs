import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const binary=name=>resolve(sdk,'bin',name+(process.platform==='win32'?'.exe':''));
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const soft=resolve(out,'last-spell-softfloat.o');
const run=(cmd,args)=>execFileSync(cmd,args,{windowsHide:true,stdio:'inherit'});
run(binary('clang'),['--target=wasm32-wasip1','-O2','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5','-c',resolve(root,'web/cpp/third_party/softfloat.c'),'-o',soft]);
for(const multiplayer of [false,true]){
  const wasm=resolve(out,`last-spell-${multiplayer?'mp':'sp'}.wasm`);
  run(binary('clang++'),['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
    ...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[]),'-Wl,-z,stack-size=1048576',
    '-I'+resolve(root,'web/cpp/game'),...['Arithmetic','Timer','PlayerLife','PlayerBomb'].map(n=>resolve(root,`web/cpp/game/${n}.cpp`)),
    resolve(root,'tests/last-spell-termination-test.cpp'),soft,'-o',wasm]);
  const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
  if(wasi.start(instance)!==0)throw Error('Last Spell termination failed');
}
