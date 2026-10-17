import {execFileSync} from 'node:child_process';
import {readFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..');
const game=process.argv.includes('--th08')?'th08':'th10';
const sdk=process.env.EMSDK??resolve(root,'tools/emsdk');
const out=resolve(root,game+'_web/artifacts/subpixel');mkdirSync(out,{recursive:true});
const obj=resolve(root,game+'_web/artifacts/sdl3/objects');
// Refuse stale build objects. An explicit renderer override is reserved for
// the before/after negative control; the current build must still be fresh.
const build=JSON.parse(readFileSync(resolve(obj,'../build.json'),'utf8'));
for(const [name,expected] of Object.entries(build.sourceFiles)){
    const actual=createHash('sha256').update(readFileSync(resolve(root,name))).digest('hex');
    if(actual!==expected)throw Error('Rebuild before testing modified source: '+name);
}
const file=resolve(out,'renderer-check.wasm');
execFileSync(process.env.TH_PYTHON??'python',[resolve(sdk,'install/emscripten/emcc.py'),
'-O2','-std=c++17','-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-sDEFAULT_TO_CXX=1',
resolve(root,'portable/subpixel-check.cpp'),
...['AnmRenderer','Arithmetic','GameMath'].map(n=>n==='AnmRenderer'&&process.env.TH_RENDERER_OBJECT?resolve(process.env.TH_RENDERER_OBJECT):resolve(obj,'cpp_game_'+n+'.cpp.o')),
resolve(obj,'softfloat.o'),'-sSTANDALONE_WASM=1','-sSTACK_SIZE=1048576','-o',file],
{env:{...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')},windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const module=await WebAssembly.compile(readFileSync(file));
const instance=await WebAssembly.instantiate(module,{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Renderer regression failed: '+code);
