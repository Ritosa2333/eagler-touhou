import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.EMSDK??resolve(root,'../th08/tools/emsdk');
const out=resolve(root,'th10_web/artifacts/thcrap-layout');mkdirSync(out,{recursive:true});
const file=resolve(out,'check.wasm');
const th11=process.argv.includes('--th11-proof');
if(th11&&!process.env.EAGLER_WORKSPACE)throw Error('TH11 proof requires EAGLER_WORKSPACE');
execFileSync(process.env.TH_PYTHON??'python',[resolve(sdk,'install/emscripten/emcc.py'),
    '-O2','-std=c++17','-sDEFAULT_TO_CXX=1','-sSTANDALONE_WASM=1',
    ...(th11?['-DTH11_LAYOUT_PROOF=1','-I'+resolve(process.env.EAGLER_WORKSPACE,'th11/th11_web/cpp/sdl')]:[]),
    resolve(root,'portable/thcrap-layout-check.cpp'),'-o',file],
    {env:{...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')},windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(file)),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('THCRAP layout regression failed: '+code);
if(th11)console.log('TH11 existing layout port: same ending regression cases PASS (no TH11 source changes)');
