import {execFileSync} from 'node:child_process';
import {mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.EMSDK;
assert(sdk,'Set EMSDK explicitly');
const out=resolve(root,'build/purple-portable');mkdirSync(out,{recursive:true});
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')};
const cc=resolve(sdk,'install/emscripten/emcc.py');
const run=(name,sources,extra=[])=>{
 const target=resolve(out,name+'.js');
 execFileSync('python',[cc,...sources.map(file=>resolve(root,file)),
  '-I'+resolve(root,'portable/include'),'-I'+resolve(root,'portable/generated'),'-I'+resolve(root,'src'),
  '-O1','-std=c++17','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1',
  ...extra,'-o',target],{cwd:root,env,windowsHide:true,stdio:'inherit'});
 execFileSync(process.execPath,[target],{cwd:root,env,windowsHide:true,stdio:'inherit'});
};
run('session',['portable/tests/session_test.cpp','portable/src/session.cpp']);
run('transaction',['portable/tests/ecl_patch_test.cpp','portable/src/ecl_patch.cpp']);
run('stage1',['portable/tests/th07_patch_test.cpp','portable/src/th07/patches.cpp','portable/src/ecl_patch.cpp']);
console.log('TH07 legacy Replay decode, CRC/expected-byte/transaction failure and stage1 patch tests PASS');
if(process.env.TH07_RETAIL_DAT){
 run('retail-ecl',['tests/th07-purple-ecl.cpp','src/pbg4/Lzss.cpp','portable/src/session.cpp','portable/src/ecl_patch.cpp'],
  ['--use-port=sdl3','--embed-file',resolve(process.env.TH07_RETAIL_DAT)+'@/fixture.dat','-sINITIAL_MEMORY=67108864',
   "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['$stringToNewUTF8']"]);
}else console.log('Retail ECL gate NOT RUN: set TH07_RETAIL_DAT to original th07.dat (not localized DATA)');
