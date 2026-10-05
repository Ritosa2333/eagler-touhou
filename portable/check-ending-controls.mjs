import {execFileSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,readdirSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),game=basename(root).replace(/mp$/,'');
const multiplayer=process.argv.includes('--multiplayer'),profile=multiplayer?'multiplayer':game==='th09'?'sdl-release':'sdl3';
const buildRoot=resolve(root,game+'_web/artifacts',profile),obj=resolve(buildRoot,'objects');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
for(const [name,expected] of Object.entries(build.sourceFiles??build.sources)){
  if(createHash('sha256').update(readFileSync(resolve(root,name))).digest('hex')!==expected)throw Error('Rebuild modified source: '+name);
}
const sdk=process.env.EMSDK,out=resolve(root,game+'_web/artifacts/ending-controls',profile);mkdirSync(out,{recursive:true});
if(!sdk)throw Error('Set EMSDK to the build toolchain');
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')};
const objects=readdirSync(obj).filter(n=>n.endsWith('.o')&&(n.includes('cpp_game_')||n==='softfloat.o')).map(n=>resolve(obj,n));
const response=resolve(out,'objects.rsp');writeFileSync(response,objects.map(n=>JSON.stringify(n)).join('\n'));
const library=resolve(out,'game-'+createHash('sha256').update(objects.join('\n')).digest('hex').slice(0,12)+'.a');
execFileSync('python',[resolve(sdk,'upstream/emscripten/emar.py'),'rcs',library,'@'+response],{env,stdio:'inherit',windowsHide:true});
const wasm=resolve(out,'check.wasm');
execFileSync('python',[resolve(sdk,'upstream/emscripten/emcc.py'),'-O2','-std=c++17','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-I'+resolve(root,'portable/sdl'),...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-I'+resolve(process.env.EAGLER_COMMON_ROOT,'include')]:[]),'-fno-exceptions','-fno-rtti','-sDEFAULT_TO_CXX=1',resolve(root,'portable/ending-controls-check.cpp'),library,'-sSTANDALONE_WASM=1','-sSTACK_SIZE=1048576','-o',wasm],{env,stdio:'inherit',windowsHide:true});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
const status=wasi.start(instance);if(status)throw Error('Ending regression failed: '+status);
