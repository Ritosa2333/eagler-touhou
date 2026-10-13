// Shared input/key/speed/reaction authorities live in eagler-common.
// Same shared-tool extraction boundary as the purple TH15 adapter (MIT).
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {commonRoot} from './common-root.mjs';
const root=resolve(import.meta.dirname,'..'),upstream=resolve(process.argv[2]);
const shared=spawnSync(process.execPath,[resolve(commonRoot,'tools/generate-purple-thprac.mjs'),upstream,'--check'],{stdio:'inherit',windowsHide:true});
if(shared.error)throw shared.error;if(shared.status!==0)throw Error('Common purple source verification failed');
const read=p=>readFileSync(resolve(upstream,'thprac/src/thprac',p),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
function region(s,a,b){const i=s.indexOf(a),j=s.indexOf(b,i);if(i<0||j<=i)throw Error('Purple tool extraction drift: '+a);return s.slice(i,j);}
const key=read('thprac_igi_key_render.cpp'),kh=read('thprac_igi_key_render.h'),tools=read('thprac_launcher_tools.cpp'),th=read('thprac_launcher_tools.h'),hooks=read('thprac_games_hooks.cpp'),games=read('thprac_games.cpp');
const f12=region(read('thprac_th10.cpp'),'        void ContentUpdate()','            ImGui::EndChild();');
for(const call of ['GameFPSOpt(mOptCtx)','DisableKeyOpt()','KeyHUDOpt()','InfLifeOpt()','GameplayOpt(mOptCtx)','InGameReactionTestOpt()','AboutOpt()'])if(!f12.includes(call))throw Error('TH10 F12 membership drift: '+call);
const preamble='// Generated from purple shared tools (MIT), following TH15.\n';
const sss=read('thprac_games_SSS.cpp');
if(!f12.includes('SSS::SSS_UI(10)'))throw Error('TH10 SSS membership drift');
const blindOptions=region(sss,'    struct BlindViewOpt','    bool g_th20_change_stone');
let blind=region(sss,'    void RenderBlindView(','// EHOOK_ST(SSS_th15_ass');
blind=blind.replace('int dx_ver, DWORD device,','Application& app,').replace('ReadImage(dx_ver, device, "blind.png", (char*)blind_file, sizeof(blind_file))','practice_blind_image(app, blind_file, sizeof(blind_file))');
if(/ReadImage|DWORD|dx_ver/.test(blind))throw Error('Unmapped blind image boundary');
const files={
 'th10_web/cpp/game/PracticeLicense.hpp':`${preamble}#pragma once\nnamespace th10 {inline constexpr const char* practice_license=${JSON.stringify(readFileSync(resolve(upstream,'LICENCE'),'utf8'))};}\n`,
 'th10_web/cpp/sdl/PracticeBlind.inc':preamble+blindOptions+blind,
};
if(process.argv.includes('--write')){for(const[p,v]of Object.entries(files))writeFileSync(resolve(root,p),v);console.log('Generated TH10 purple shared tools');}
else if(process.argv.includes('--check')){for(const[p,v]of Object.entries(files))if(!existsSync(resolve(root,p))||readFileSync(resolve(root,p),'utf8').replaceAll('\r\n','\n').trimEnd()!==v.trimEnd())throw Error('Stale shared tools: '+p);console.log('Purple shared tool extraction verified');}
else throw Error('Use --write or --check');
