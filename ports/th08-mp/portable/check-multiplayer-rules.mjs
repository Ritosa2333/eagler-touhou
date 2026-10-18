import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';

const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const binary=name=>resolve(sdk,'bin',name+(process.platform==='win32'?'.exe':''));
if(!existsSync(binary('clang++')))throw Error('WASI SDK not found: '+binary('clang++'));
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const soft=resolve(out,'rules-softfloat.o');
execFileSync(binary('clang'),['--target=wasm32-wasip1','-O2','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5',
  '-c',resolve(root,'th08_web/cpp/third_party/softfloat.c'),'-o',soft],{stdio:'inherit'});
for(const multiplayer of [false,true]){
  const wasm=resolve(out,`player-life-rules-${multiplayer?'mp':'sp'}.wasm`);
  execFileSync(binary('clang++'),['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
    ...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[]),'-Wl,-z,stack-size=1048576',
    '-I'+resolve(root,'th08_web/cpp/game'),
    ...['Arithmetic','Timer','PlayerLife'].map(name=>resolve(root,`th08_web/cpp/game/${name}.cpp`)),
    resolve(root,'tests/player-life-rules-test.cpp'),soft,'-o',wasm],{stdio:'inherit'});
  const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
  const code=wasi.start(instance);if(code!==0)throw Error('Player life rules failed: '+code);
}
