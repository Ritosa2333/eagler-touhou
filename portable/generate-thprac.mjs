// Source extraction, following TH08/TH10/TH11's portable boundary.
// The purple checkout is mandatory; generated game logic is not handwritten.
import {readFileSync,existsSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const repository=resolve(import.meta.dirname,'..');
const argument=process.argv.slice(2).find(v=>!v.startsWith('--'));
if(!argument)throw Error('Usage: node portable/generate-thprac.mjs <thprac_purple> [--check]');
const upstream=resolve(argument);
const read=p=>readFileSync(resolve(upstream,p),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n').replaceAll('\r','');
const source=read('thprac/src/thprac/thprac_th15.cpp');
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const abtest=read('thprac/src/thprac/thprac_th15_abtest.h');
const sharedSource=read('thprac/src/thprac/thprac_games.h');
const versionHeader=read('thprac/src/thprac/thprac_version.h');
const version=Array.from({length:4},(_,i)=>{const match=versionHeader.match(new RegExp('#define THPRAC_VERSION_'+i+' (\\d+)'));if(!match)throw Error('Purple version boundary changed');return match[1];}).join('.');
if(!source.includes('doremy_normal_1_phase')||!source.includes('TH15_ST8_AB_TEST')||!source.includes('g_blind_view'))throw Error('TH15 purple source required');
const digest=createHash('sha256').update(source).digest('hex');
const start=source.indexOf('    void ECLJump('),end=source.indexOf('    __declspec(noinline) void THSectionPatch()');
if(start<0||end<=start)throw Error('Purple TH15 extraction boundary changed');
let body=source.slice(start,end).replaceAll('__declspec(noinline) ','').replaceAll('THPrac::TH15::','').replaceAll('th_sections_t','int').replaceAll('(BYTE)','(uint8_t)');
// Namespace constant becomes an adapter-class constant, not per-run storage.
body=body.replace('    constexpr unsigned int st7Start', '    static constexpr unsigned int st7Start');
// Replace precisely the one injected-process hook in the resource region.
// Its live meaning (next chapter bonus sets enemy-total to 1) remains separate.
if(body.split('th15_st7boss1_chapter_bonus.Enable();').length!==2)throw Error('Chapter bonus hook drift');
body=body.replace('th15_st7boss1_chapter_bonus.Enable();','effects.extra_boss_chapter_bonus = true;');
// Observe explicit jumps for source-boundary validation; instruction words,
// offsets, ordering and all switch branches remain upstream-owned.
body=body.replace('ecl.SetPos(start);','ecl.Jump(start, dest);\n        ecl.SetPos(start);');
if(/GetMem|GetPtr|__declspec|\.Enable\(|\*\([^\n]*\*\)/.test(body))throw Error('Unmapped purple platform dependency');
const glossary=Object.assign({},...Object.values(definitions).map(g=>g.glossary||{}));
const groups=Object.assign({},...Object.values(definitions).map(g=>g.groups||{}));
const entries=Object.entries(definitions.th15.sections);
const sections=entries.map(([key,v],i)=>({id:i+1,key,appearance:v.appearance,spell:!!v.spell,bgm:v.bgm,names:Array.from({length:5},(_,d)=>{
 const label=Object.entries(v).find(([k])=>k.startsWith('!')&&k.includes('ENHLX'[d]))?.[1];
 if(label===undefined)return ['','',''];
 if(typeof label==='string'){if(!glossary[label])throw Error('Unknown section glossary '+label);return glossary[label];}return label;
})}));
const phaseStart=source.indexOf('        const th_glossary_t* SpellPhase()'),phaseEnd=source.indexOf('        void PracticeMenu()',phaseStart);
if(phaseStart<0||phaseEnd<=phaseStart)throw Error('Purple phase boundary changed');
let phase=source.slice(phaseStart,phaseEnd).replace('const th_glossary_t* SpellPhase()','inline int practice_phase_count(int section, int difficulty)').replace('            auto section = CalcSection();\n','').replaceAll('mDiffculty','difficulty').replaceAll('return nullptr;','return 1;');
const usedGroups=[];
phase=phase.replace(/return (TH\w+);/g,(_,key)=>{if(!groups[key])throw Error('Unknown phase group '+key);usedGroups.push(key);return 'return '+groups[key].length+';';});
const tuple=a=>'{'+a.map(v=>JSON.stringify(v)).join(',')+'}';
const uiGroups=[...new Set([...usedGroups,'TH_MODE_SELECT','TH_STAGE_SELECT','TH_WARP_SELECT','IGI_DIFF','IGI_PL_15'])];
const sharedTools=['thprac_games.cpp','thprac_launcher_tools.cpp','thprac_games_SSS.cpp'].map(p=>read('thprac/src/thprac/'+p)).join('\n');
const uiKeys=[...new Set([...source.match(/\bTH[A-Z0-9_]+\b/g),...sharedTools.match(/\bTH[A-Z0-9_]+\b/g),'THPRAC_INFLIVES_MAP','TH_FACTOR_ACB','TH_FACTOR_ACB_DESC'])].filter(key=>glossary[key]);
// The portable completion owner depends on this precise asymmetric topology:
// native practice falls through, stage six and non-final stages return to
// practice, while Extra is deliberately NOT patched at its award branch.
const acbStart=source.indexOf('    PATCH_ST(th15_all_clear_bonus_1,'),acbEnd=source.indexOf('    HOOKSET_DEFINE(th15_master_disable)',acbStart);
const acb=source.slice(acbStart,acbEnd);
if(!acb.includes('0x43d99d, "eb0b909090"')||!acb.includes('0x43daac, 7')||!acb.includes('0x43dcb5, 7')||
 (acb.match(/GetMemContent\(0x4e7794\) & 0x10/g)||[]).length!==2||
 (acb.match(/GetMemAddr\(0x4e9a8c, 0x160\)/g)||[]).length!==2||
 (acb.match(/pCtx->Eip = 0x43d9a2;/g)||[]).length!==2)throw Error('Purple all-clear completion topology drift');
const phaseLabels=source.slice(phaseStart,phaseEnd).replace('const th_glossary_t* SpellPhase()','inline const char* const (*practice_phase_labels(int section, int difficulty))[3]').replace('            auto section = CalcSection();\n','').replaceAll('mDiffculty','difficulty').replace(/return (TH\w+);/g,(_,key)=>'return practice_'+key+';');
const resultStart=source.indexOf('    void ABTestRender()'),resultEnd=source.indexOf('    void MakeReisenShieldANM(',resultStart);
if(resultStart<0||resultEnd<=resultStart)throw Error('Purple AB result boundary changed');
let abResult=source.slice(resultStart,resultEnd);
const scoreStart=abResult.indexOf('                float scores[6]'),scoreEnd=abResult.indexOf('                float avg_score',scoreStart);
if(scoreStart<0||scoreEnd<=scoreStart)throw Error('Purple AB scoring boundary changed');
const scoreBody=abResult.slice(scoreStart,scoreEnd);
const scoresHelper=scoreBody.replace('float scores[6] = {','std::array<float,6> scores = {');
const replaceOnce=(a,b)=>{if(abResult.split(a).length!==2)throw Error('AB result adapter boundary changed: '+a);abResult=abResult.replace(a,b);};
replaceOnce('void ABTestRender()','void practice_ab_test_result(ApplicationState& app)');
replaceOnce('static int t = 0;','auto& t = app.practice.ab_result_frames;');
replaceOnce('DWORD ecl_glob = *(DWORD*)(0x4E9A80);','const auto& globals = app.scene()->battle.enemy_world;');
replaceOnce('DWORD m9923 = ecl_glob ? *(DWORD*)(ecl_glob + 0x18) : 0;','const int m9923 = globals.integer_registers[3];');
for(let i=0;i<5;i++)replaceOnce(`*(float*)(ecl_glob + 0x${(0x38-i*4).toString(16).toUpperCase()})`,`globals.float_registers[${7-i}]`);
replaceOnce('float result_origs[5] = {','std::array<float,5> result_origs = {');
replaceOnce(scoreBody,'                auto scores = practice_ab_scores(result_origs);\n');
// Platform-independent numeric constants and printf preserve the source
// algorithm/drawing order on the shared C++17/ImGui backend.
abResult=abResult.replaceAll('std::numbers::pi_v<float>','3.1415927410125732421875f').replace(/S\((TH\w+)\)/g,(_,key)=>'label(practice_'+key+')');
replaceOnce('std::string text = std::format("{}:{}({:>3.1f})", chars[i], rank, scores[i] * 100.0f).c_str();','char formatted[256]; std::snprintf(formatted, sizeof formatted, "%s:%s(%3.1f)", chars[i], rank, scores[i] * 100.0f);\n                        std::string text = formatted;');
// Keep the native radar and font; move side labels inside the clipped panel.
replaceOnce('pos.x -= sz_text.x * 0.5f;', `pos.x -= sz_text.x * 0.5f;
                        if (i != 0 && i != 3) {
                            const float inset = ImGui::GetStyle().FramePadding.x * 2.0f;
                            const float left = p0.x + inset;
                            const float right = std::max(left, p1.x - inset - sz_text.x);
                            pos.x = std::clamp(pos.x, left, right);
                        }`);
if(/DWORD|GetMem|ecl_glob|std::format|std::numbers/.test(abResult))throw Error('Unmapped AB result platform dependency');
// Preserve the shared state machine and native call-site classification.
const musicStart=sharedSource.indexOf('    static bool mElStatus { false };',sharedSource.indexOf('static bool ElBgmTest('));
const musicEnd=sharedSource.indexOf('\n}\n',musicStart);
if(musicStart<0||musicEnd<musicStart)throw Error('Purple everlasting boundary changed');
let music=sharedSource.slice(musicStart,musicEnd).replace('    static bool mElStatus { false };\n    static int mLockBgmId { -1 };\n','');
if(music.includes('static ')||!music.includes('return mElStatus;'))throw Error('Purple everlasting state drift');
const musicSites=source.match(/result = ElBgmTest<([^>]+)>\(/)?.[1].split(',').map(v=>v.trim());
if(!musicSites||musicSites.length!==5||musicSites.some(v=>!/^0x[0-9a-f]+$/i.test(v))||!source.includes('is_practice = (*((int32_t*)0x4e7794) & 0x1);'))throw Error('Purple TH15 music classification drift');
const range=source.match(/float y_max = \(\*y_pos\)[\s\S]*?\*y_range = \(y_max - y_min2\);/)?.[0];
const rangeDefault=sharedSource.match(/#define BOSS_MOVE_DOWN_RANGE_INIT ([0-9.]+)/)?.[1];
if(!range||!rangeDefault)throw Error('Purple movement hook drift');
const starsStart=source.indexOf('EHOOK_ST(th15_stars_bgm_sync,'),starsEnd=source.indexOf('    void ECLSetChapter',starsStart);
const stars=source.slice(starsStart,starsEnd);
const starsOffset=stars.match(/\*\(uint32_t\*\)\(pCtx->Esp \+ 0x10\) = (0x[0-9a-f]+);/i)?.[1];
if(!starsOffset||!stars.includes('thPracParam.mode == 1 && thPracParam.section == TH15_ST6_STARS')||!stars.includes('call_addr == 0x48B4EB')||!stars.includes('self->Disable();'))throw Error('Purple Stars music hook drift');
const files={
 'th15_web/cpp/game/PracticeNativeHooks.hpp':`// Generated purple ElBgmTest and TH15 boss-range arithmetic (MIT).\n// TH15 LF source sha256 ${digest}; shared LF sha256 ${createHash('sha256').update(sharedSource).digest('hex')}.\n#pragma once\n#include "Types.hpp"\nnamespace th15 {\ninline constexpr u32 practice_stars_bgm_byte_offset=${starsOffset};\ninline constexpr float practice_boss_range_default=${rangeDefault}f;\ninline void practice_boss_range(float* y_pos,float* y_range,float g_bossMoveDownRange){\n${range}\n}\nstruct PracticeMusic {\n static constexpr u32 play_addr=${musicSites[0]},stop_addr=${musicSites[1]},pause_addr=${musicSites[2]},resume_addr=${musicSites[3]},caller_addr=${musicSites[4]};\n bool mElStatus=false;int mLockBgmId=-1;\n bool suppress(bool hotkey_status,bool practice_status,u32 retn_addr,int bgm_param,u32 caller=caller_addr){\n${music}\n }\n};\n}\n`,
 'th15_web/cpp/game/PracticeVersion.hpp':`// Generated from purple thprac_version.h (MIT).\n#pragma once\nnamespace th15 {inline constexpr const char* practice_source_version="${version}";}\n`,
 'th15_web/cpp/sdl/PracticeAbResult.inc':`// Generated purple ABTestRender (MIT), LF source sha256 ${digest}.\n// Typed globals, per-run counter, C++17 formatting and side-label bounds are adapted.\n${abResult}`,
 'th15_web/cpp/game/PracticeAbScores.hpp':`// Generated verbatim purple ABTestRender scoring (MIT).\n#pragma once\n#include <array>\n#include <cmath>\nnamespace th15 {\ninline std::array<float,6> practice_ab_scores(std::array<float,5> result_origs){\n${scoresHelper} return scores;\n}\n}\n`,
 'th15_web/cpp/game/PracticePatches.inc':`// Generated from thprac_purple TH15 (MIT), LF sha256 ${digest}.\n// Only platform-hook ownership and bounds observations are adapted.\n${body}`,
 'th15_web/cpp/game/PracticeAbTest.hpp':`// Generated verbatim from purple thprac_th15_abtest.h (MIT).\n#pragma once\nnamespace th15 {\n${abtest}\n}\n`,
 'th15_web/cpp/game/PracticeSections.hpp':`// Generated from purple thprac_games_def.json (MIT). Enum order is authoritative.\n#pragma once\nnamespace th15 {\nenum PracticeSection {PracticeNone=0,\n${entries.map(([k],i)=>` ${k}=${i+1},`).join('\n')}\n};\nstruct PracticeSectionInfo {int appearance,group,bgm;bool spell;};\ninline constexpr PracticeSectionInfo practice_sections[]{{0,0,0,false},\n${sections.map(s=>` {${s.appearance[0]},${s.appearance[1]},${s.bgm},${s.spell}},`).join('\n')}\n};\n${phase}\n}\n`,
 'th15_web/cpp/game/PracticeSectionCatalog.hpp':`// Generated purple section names; locale order zh-CN, en-US, ja-JP.\n#pragma once\nnamespace th15 {\nstruct PracticeSectionLabel {int id;const char* names[5][3];};\ninline constexpr PracticeSectionLabel practice_section_labels[]{\n${sections.map(s=>` {${s.id},{${s.names.map(tuple).join(',')}}},`).join('\n')}\n};\n}\n`,
 'th15_web/cpp/game/PracticeUiLabels.hpp':`// Generated purple glossary and phase groups (MIT).\n#pragma once\nnamespace th15 {\n${uiKeys.map(key=>`inline constexpr const char* practice_${key}[3]${tuple(glossary[key])};`).join('\n')}\n${uiGroups.map(key=>`inline constexpr const char* practice_${key}[][3]{${groups[key].map(label=>{if(!glossary[label])throw Error('Unknown glossary '+label);return tuple(glossary[label]);}).join(',')}};`).join('\n')}\n${phaseLabels}\n}\n`,
 'th15_web/sdl-runtime/practice-sections.mjs':`// Generated from thprac_purple (MIT), LF source sha256 ${digest}.\nexport const sections=${JSON.stringify(sections,null,2)};\n`,
 'th15_web/cpp/game/THPRAC-LICENSE.txt':read('LICENCE'),
};
if(process.argv.includes('--write')){
 for(const [path,value] of Object.entries(files))writeFileSync(resolve(repository,path),value);
 console.log(JSON.stringify({generated:Object.keys(files),source:digest,sections:sections.length}));
}else if(process.argv.includes('--check')){
 for(const [path,value] of Object.entries(files))if(!existsSync(resolve(repository,path))||readFileSync(resolve(repository,path),'utf8').replaceAll('\r\n','\n').trimEnd()!==value.trimEnd())throw Error('Stale purple extraction: '+path);
 console.log(JSON.stringify({passed:true,source:digest,sections:sections.length,abtestBytes:(abtest.match(/0x[0-9a-f]{2}/gi)||[]).length}));
}else console.log('*** Begin Patch\n'+Object.entries(files).map(([path,value])=>'*** Add File: '+resolve(repository,path).replaceAll('\\','/')+'\n'+value.trimEnd().split('\n').map(line=>'+'+line).join('\n')).join('\n')+'\n*** End Patch');
