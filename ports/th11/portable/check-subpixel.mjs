import { execFileSync } from 'node:child_process';
import { mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.EMSDK;assert(sdk,'Set EMSDK explicitly');
const out=resolve(root,'th11_web/artifacts/subpixel');mkdirSync(out,{recursive:true});
const temporary=resolve(out,'tmp');mkdirSync(temporary,{recursive:true});
const env={...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMCC_TEMP_DIR:process.env.EMCC_TEMP_DIR??temporary};
const sources=['portable/subpixel-check.cpp',...['AnmGeometry','AnmRenderer','AnmProjection','GraphicsMath'].map(n=>`th11_web/cpp/game/${n}.cpp`)];
execFileSync('python',[resolve(sdk,'install/emscripten/emcc.py'),...sources.map(p=>resolve(root,p)),
  '-std=c++17','-O2','-ffp-contract=off','-fno-strict-aliasing','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',resolve(out,'check.js')],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync(process.execPath,[resolve(out,'check.js')],{env,cwd:root,stdio:'inherit',windowsHide:true});
assert(readFileSync(resolve(root,'th11_web/cpp/game/BulletManager.cpp'),'utf8').includes('renderer.draw(b->vm,true)'));
console.log('TH11 production bullet draw selects continuous geometry PASS');
