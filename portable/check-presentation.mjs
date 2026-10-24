// Build and run the CPU presentation contract with the pinned Emscripten SDK.
import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.EMSDK??resolve(root,'tools/emsdk');
const emcc=[resolve(sdk,'install/emscripten/emcc.py'),resolve(sdk,'upstream/emscripten/emcc.py')].find(existsSync);
assert(emcc,'Set EMSDK explicitly or provide tools/emsdk');
const out=resolve(root,'web/artifacts/presentation');mkdirSync(out,{recursive:true});
const env={...process.env,EMSDK:sdk,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')};
execFileSync('python',[emcc,
  resolve(root,'portable/presentation-check.cpp'),
  resolve(root,'web/cpp/game/AnmPresentation.cpp'),
  '-std=c++17','-O1','-ffp-contract=off','-fno-strict-aliasing',
  '-I'+resolve(root,'web/cpp/game'),
  '-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',resolve(out,'presentation-check.cjs'),
],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync(process.execPath,[resolve(out,'presentation-check.cjs')],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync('python',[emcc,
  resolve(root,'portable/cadence-check.cpp'),
  '-std=c++17','-O1','-ffp-contract=off',
  '-I'+resolve(root,'portable/sdl'),
  '-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',resolve(out,'cadence-check.cjs'),
],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync(process.execPath,[resolve(out,'cadence-check.cjs')],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync('python',[emcc,
  resolve(root,'portable/camera-check.cpp'),
  resolve(root,'web/cpp/game/SceneCamera.cpp'),
  resolve(root,'web/cpp/game/GraphicsMath.cpp'),
  resolve(root,'web/cpp/game/FogInterpolation.cpp'),
  '-std=c++17','-O1','-ffp-contract=off','-fno-strict-aliasing',
  '-I'+resolve(root,'web/cpp/game'),'-I'+resolve(root,'web/cpp'),
  '-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-o',resolve(out,'camera-check.cjs'),
],{env,cwd:root,stdio:'inherit',windowsHide:true});
execFileSync(process.execPath,[resolve(out,'camera-check.cjs')],{env,cwd:root,stdio:'inherit',windowsHide:true});
