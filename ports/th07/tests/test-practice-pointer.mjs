import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.EMSDK??resolve(root,'../toolchains/emsdk');
const compiler=['upstream','install'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
if(!compiler)throw Error('Set EMSDK to the pinned Emscripten SDK');
const imgui=resolve(process.env.THPRAC_IMGUI_ROOT??resolve(root,'../thprac-reallyportable/thprac/src/3rdParties/ImGui'));
const common=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'third_party/eagler-common'));
const out=resolve(root,'build/practice-pointer-check');mkdirSync(out,{recursive:true});
const output=resolve(out,'check.cjs');
const env={...process.env,EMSDK:sdk,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten'),EMCC_CORES:process.env.EMCC_CORES??'1'};
execFileSync('python',[compiler,'-O1','-std=c++20','-DTH_ENABLE_THPRAC','-DIMGUI_DISABLE_WIN32_FUNCTIONS','-include','cstdint','-Wno-nontrivial-memcall',
  '-I'+imgui,'-I'+resolve(root,'src'),'-I'+resolve(common,'include'),'--use-port=sdl3_ttf','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1',
  resolve(root,'tests/practice-pointer-regression.cpp'),...['imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp'].map(n=>resolve(imgui,n)),'-o',output],{cwd:root,env,windowsHide:true,stdio:'inherit'});
execFileSync(process.execPath,[output],{cwd:root,env,windowsHide:true,stdio:'inherit'});
