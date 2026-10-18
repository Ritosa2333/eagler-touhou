import {execFileSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),sdk=resolve(root,'th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin'),out=resolve(root,'artifacts/preload-ownership');mkdirSync(out,{recursive:true});
const sources=['portable/preload-ownership.cpp',...['AnmLibrary','AnmResource','TextureStore','TexturePixels','Arithmetic'].map(n=>'th08_web/cpp/game/'+n+'.cpp')];
execFileSync(resolve(sdk,'clang++.exe'),['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',...sources,resolve(root,'artifacts/numeric/softfloat.o'),'-o',resolve(out,'check.wasm')],{cwd:root,windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{ref:resolve(root,'th08_web/reference')},returnOnExit:true}),{instance}=await WebAssembly.instantiate(readFileSync(resolve(out,'check.wasm')),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Pristine preload regression failed '+code);writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,animations:4,pixelFormats:2,loads:48,checks:['all transition assets fit','cached metadata/scripts/pixels match uncached loader','deferred steps match','mutating active script/sprite/texture leaves template intact','simultaneous instances do not alias textures','static texture handles retained between scenes','clearing cache releases all resident textures']},null,2));
