import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'../..'),sdk=process.env.EMSDK;
assert(sdk,'Set EMSDK to the pinned Emscripten SDK');
const emcc=['install','upstream'].map(layout=>resolve(sdk,layout,'emscripten/emcc.py')).find(existsSync);
assert(emcc,'Emscripten compiler not found');
const out=resolve(root,'artifacts/issue-153');mkdirSync(out,{recursive:true});
const output=resolve(out,'extrapolation-check.cjs');
execFileSync('python',[emcc,resolve(import.meta.dirname,'extrapolation-check.cpp'),
 '-I'+resolve(root,'common/include'),'-std=c++17','-O1','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',output],
 {cwd:root,env:process.env,stdio:'inherit',windowsHide:true});
execFileSync(process.execPath,[output],{stdio:'inherit',windowsHide:true});

for(const game of ['th08','th08-mp','th10','th10-mp','th11','th11-mp']){
 const executable=resolve(out,game+'-owner-extrapolation.cjs');
 execFileSync('python',[emcc,resolve(import.meta.dirname,'owner-extrapolation-check.cpp'),
  '-I'+resolve(root,'common/include'),'-I'+resolve(root,'ports',game,'web/cpp/game'),
  '-D'+(game.startsWith('th08')?'TH08_OWNER_CHECK':game.startsWith('th10')?'TH10_OWNER_CHECK':'TH11_OWNER_CHECK')+'=1',
  '-std=c++17','-O1','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',executable],
  {cwd:root,env:process.env,stdio:'inherit',windowsHide:true});
 console.log(game+':');execFileSync(process.execPath,[executable],{stdio:'inherit',windowsHide:true});
}
