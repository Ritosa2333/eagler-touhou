import {execFileSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),sdk=resolve(root,'tools/emsdk'),out=resolve(root,'artifacts/shared-platform-validation');mkdirSync(out,{recursive:true});
execFileSync('python',[resolve(sdk,'install/emscripten/emcc.py'),'-O2','-std=c++17',resolve(root,'portable/numeric/truncate-check.cpp'),resolve(root,'th08_web/artifacts/sdl3/objects/softfloat.o'),'-sSTANDALONE_WASM=1','-o',resolve(out,'truncate-check.wasm')],{env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten')},windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true}),module=await WebAssembly.compile(readFileSync(resolve(out,'truncate-check.wasm'))),instance=await WebAssembly.instantiate(module,{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Numeric comparison failed: '+code);writeFileSync(resolve(out,'numeric.json'),JSON.stringify({passed:true,randomCases:1000000,reference:'SoftFloat extF80M_to_i64, truncate toward zero, low 32 bits'},null,2));
