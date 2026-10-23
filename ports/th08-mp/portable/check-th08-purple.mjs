import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {commonInclude} from './common-root.mjs';

const root=resolve(import.meta.dirname,'..');
const sdk=process.env.EMSDK??resolve(root,'tools/emsdk');
const emcc=resolve(sdk,'install/emscripten/emcc.py');
const out=resolve(root,'web/artifacts/purple-check');
mkdirSync(out,{recursive:true});
const temporary=resolve(out,'temp');mkdirSync(temporary,{recursive:true});
const env={...process.env,TEMP:temporary,TMP:temporary,EMSDK:sdk,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')};
const cases={
 config:['web/cpp/game/PracticeConfig.cpp'],
 input:[],history:[],hitbox:[],rsqrt:[],speed:[],
 score:['web/cpp/game/SpellHistory.cpp'],
 ecl:['web/cpp/game/EclProgram.cpp'],
 graphics:['web/artifacts/sdl3/objects/cpp_game_GraphicsMath.cpp.o','web/artifacts/sdl3/objects/cpp_game_GameMath.cpp.o','web/artifacts/sdl3/objects/cpp_game_Arithmetic.cpp.o','web/artifacts/sdl3/objects/softfloat.o'],
};
for(const [name,dependencies] of Object.entries(cases)){
 for(const path of dependencies)if(!existsSync(resolve(root,path)))throw Error(`Build SDL3 first: missing ${path}`);
 const output=resolve(out,`${name}.js`);
 const source=name==='config'?'check-th08-thprac.cpp':`check-th08-purple-${name}.cpp`;
 const args=[emcc,resolve(root,`portable/${source}`),...dependencies.map(p=>resolve(root,p)),
  '-O2','-std=c++17','-I'+commonInclude,'-fno-strict-aliasing','-ffp-contract=off','-sDEFAULT_TO_CXX=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sENVIRONMENT=node','-o',output];
 execFileSync(process.env.TH_PYTHON??'python',args,{cwd:root,env,windowsHide:true,stdio:'inherit'});
 execFileSync(process.execPath,[output],{cwd:root,env,windowsHide:true,stdio:'inherit'});
 console.log(`TH08 purple ${name}: PASS`);
}
