import {execFileSync} from 'node:child_process';
import {readFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.EMSDK??resolve(root,'tools/emsdk');
const imgui=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'../../common'),'third_party/imgui'),out=resolve(root,'web/artifacts/purple-check');mkdirSync(out,{recursive:true});
const output=resolve(out,'menu.js'),env={...process.env,EMSDK:sdk,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')};
execFileSync(process.env.TH_PYTHON??'python',[resolve(sdk,'install/emscripten/emcc.py'),resolve(root,'portable/check-th08-purple-menu.cpp'),
 ...['imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp'].map(n=>resolve(imgui,n)),
 '-I'+imgui,'-DIMGUI_DISABLE_WIN32_FUNCTIONS','--use-port=sdl3_ttf','-O1','-std=c++17','-sDEFAULT_TO_CXX=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sENVIRONMENT=node','-o',output],
 {cwd:root,env,windowsHide:true,stdio:'inherit'});
if(process.argv.includes('--reproduce-old-pop')){
 execFileSync(process.execPath,[output,'--reproduce-old-pop'],{cwd:root,env,windowsHide:true,stdio:'inherit'});
 throw Error('Expected the unfixed legacy stack-pop assertion');
}
const ui=readFileSync(resolve(root,'web/cpp/sdl/ThpracUi.cpp'),'utf8');
const overlay=ui.slice(ui.indexOf('if(MobileThprac::show(MobileThprac::Cheats,menu_open))'),ui.indexOf('if(MobileThprac::show(MobileThprac::Information,tracker_open)'));
assert(overlay.includes('ImGui::BeginDisabled(cheat_disabled)')&&overlay.includes('ImGui::EndDisabled(cheat_disabled)'),
 'The shipped legacy ImGui requires the same disabled predicate at both ends');
execFileSync(process.execPath,[output],{cwd:root,env,windowsHide:true,stdio:'inherit'});
console.log('TH08 purple menu: PASS (cold-open, gameplay, replay, reopen; actual ImGui stacks)');
