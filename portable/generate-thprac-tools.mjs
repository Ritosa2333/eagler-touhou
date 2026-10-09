import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..'),upstream=resolve(process.argv[2]);
const read=p=>readFileSync(resolve(upstream,'thprac/src/thprac',p),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const key=read('thprac_igi_key_render.cpp'),kh=read('thprac_igi_key_render.h'),tools=read('thprac_launcher_tools.cpp'),th=read('thprac_launcher_tools.h'),sss=read('thprac_games_SSS.cpp');
const hooks=read('thprac_games_hooks.cpp');
const games=read('thprac_games.cpp');
const title=read('thprac_th15.cpp'),gamesHeader=read('thprac_games.h');
const f12=region(title,'        void ContentUpdate()','            ImGui::EndChild();');
for(const call of ['GameFPSOpt(mOptCtx)','DisableKeyOpt()','KeyHUDOpt()','InfLifeOpt()','GameplayOpt(mOptCtx)','SSS::SSS_UI(15)','InGameReactionTestOpt()','AboutOpt()'])if(!f12.includes(call))throw Error('TH15 F12 membership drift: '+call);
if(!gamesHeader.includes('bool GameFPSOpt(adv_opt_ctx& ctx, bool replay = true);'))throw Error('TH15 F12 replay-speed default drift');
function region(s,a,b){const i=s.indexOf(a),j=s.indexOf(b,i);if(i<0||j<=i)throw Error('Purple tool extraction drift: '+a);return s.slice(i,j);}
const keyEnum=region(key,'enum THKey','const std::string THKeyNames');
let record=region(key,'void RecordKey(','void KeysHUD(').replace('void RecordKey(int ver, uint32_t cur_key)','void record(int ver, uint32_t cur_key)').replace('    static std::deque<uint32_t> keys_per_sec; // 60 f','');
let hud=region(key,'void static KeysRect','void RecordKey(')+region(key,'void KeysHUD(','void SaveKeyRecorded()');
hud=hud.replace('    ImGuiIO& io = ImGui::GetIO();','    const auto& g_keys_down = key_monitor.g_keys_down;\n    const auto& g_key_mask = key_monitor.g_key_mask;\n    const auto g_aps_cur = key_monitor.g_aps_cur;\n    ImGuiIO& io = ImGui::GetIO();');
hud=hud.replace('std::string aps_text = std::format("aps: {:>3}/(60frame)", g_aps_cur);','char aps_buffer[64]; std::snprintf(aps_buffer,sizeof aps_buffer,"aps: %3d/(60frame)",g_aps_cur);\n        std::string aps_text = aps_buffer;');
const style=region(kh,'struct KeyRectStyle','void RecordKey');
const reactionClass=th.match(/class THGuiTestReactionTest[\s\S]*?\n};/)?.[0];
if(!reactionClass)throw Error('Purple reaction class drift');
let reaction=region(tools,'THGuiTestReactionTest::THGuiTestReactionTest()','class THGuiRollPlayer');
reaction=reaction.replaceAll('LARGE_INTEGER','PracticeCounter').replaceAll('QueryPerformanceFrequency','practice_counter_frequency').replaceAll('QueryPerformanceCounter','practice_counter_now').replaceAll('GetRndGenerator','practice_random_generator').replaceAll('DWORD','unsigned').replaceAll('std::fabsf','std::fabs');
reaction=reaction.replace(/    Gui::KeyboardInputUpdate\([^\n]+\);\n/g,'');
reaction=reaction.replace(/\n        \|\| \(GetAsyncKeyState\(VK_DOWN\)[^;]+;/,';').replaceAll('VK_SHIFT','16');
reaction=reaction.replaceAll('GuiCenteredText(S(THPRAC_TOOLS_REACTION_TEST));','ImGui::TextUnformatted(S(THPRAC_TOOLS_REACTION_TEST));');
reaction=reaction.replaceAll('std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str()','(std::string(S(THPRAC_TOOLS_REACTION_TEST_RESULT))+"(framecount)").c_str()');
reaction=reaction.replace(/S\((TH\w+)\)/g,(_,k)=>'label(practice_'+k+')');
if(/GetAsyncKeyState|\bGui::|VK_|std::format|QueryPerformance|GetRndGenerator/.test(reaction.replace(/\/\/[^\n]*/g,'')))throw Error('Unmapped reaction platform dependency');
const preamble='// Generated from purple shared tools (MIT). Algorithm and drawing order remain source-owned.\n';
const blindOptions=region(sss,'    struct BlindViewOpt','    bool g_th20_change_stone');
let blind=region(sss,'    void RenderBlindView(','// EHOOK_ST(SSS_th15_ass');
blind=blind.replace('int dx_ver, DWORD device,','ApplicationState& app,').replace('ReadImage(dx_ver, device, "blind.png", (char*)blind_file, sizeof(blind_file))','practice_blind_image(app, blind_file, sizeof(blind_file))');
if(/ReadImage|DWORD|dx_ver/.test(blind))throw Error('Unmapped blind image boundary');
let input=region(hooks,'    if (g_input_opt.enable_auto_shoot) {','    if (g_input_opt.disable_f10_11_13) {');
const scans={Z:90,X:88,C:67,D:68,ESCAPE:27,R:82,Q:81,LSHIFT:160,RSHIFT:161};
input=input.replaceAll('g_input_opt.','').replaceAll('((BYTE*)state)','state').replace(/DIK_([A-Z]+)/g,(_,k)=>{if(!(k in scans))throw Error('Unmapped input scan '+k);return scans[k];}).replaceAll('IS_KEY_DOWN','is_key_down');
if(/DIK_|BYTE|g_input_opt/.test(input))throw Error('Unmapped input platform boundary');
let speed=region(games,'bool GameFPSOpt(','void DisableKeyOpt(').replace('adv_opt_ctx& ctx','PracticeSpeed& ctx');
speed=speed.replaceAll('GetRelWidth(0.23f)','ImGui::GetIO().DisplaySize.x*.23f').replaceAll('HelpMarker("Blah");','ImGui::TextDisabled("(?)");');
// Exclude native launcher sound preferences, which are not TH15 F12 settings.
speed=speed.replace('ChangeBGMSpeed(ctx.fps / 60.0f, -1.0f);','').replace('BGMPitchChanger();','');
speed=speed.replace(/S\((TH\w+)\)/g,(_,k)=>'label(practice_'+k+')');
if(/adv_opt_ctx|ChangeBGMSpeed|BGMPitchChanger|\bS\(/.test(speed))throw Error('Unmapped speed platform dependency');
const files={
 'th15_web/cpp/game/PracticeLicense.hpp':preamble+'#pragma once\nnamespace th15 {inline constexpr const char* practice_license=R"THPRAC('+readFileSync(resolve(root,'th15_web/cpp/game/THPRAC-LICENSE.txt'),'utf8').replaceAll('\r\n','\n')+')THPRAC";}\n',
 'th15_web/cpp/sdl/PracticeSpeed.inc':preamble+speed,
 'th15_web/cpp/game/PracticeInput.hpp':`${preamble}#pragma once\n#include "Types.hpp"\nnamespace th15 {\nstruct PracticeInput {\n bool enable_auto_shoot=false;int shoot_key_DIK=-1;bool last_is_auto_shoot_key_down=false,is_auto_shooting=false,is_th128=false;\n bool disable_xkey=false,disable_Ckey_at_same_time=true,disable_shiftkey=false,force_shiftkey=false,disable_zkey=false;\n bool enable_fast_retry=false;int fast_retry_count_down=0;static constexpr int fast_retry_cout_down_max=15;\n static bool is_key_down(u8 b){return (b&0x80)!=0;}\n void apply(u8* state){\n${input}\n if(fast_retry_count_down){if(fast_retry_count_down<=fast_retry_cout_down_max)state[27]=0x80;if(fast_retry_count_down<=1)state[82]=0x80;}\n }\n void begin_retry(int mode){if(mode&&enable_fast_retry)fast_retry_count_down=fast_retry_cout_down_max;}\n void gui_tick(){if(fast_retry_count_down)fast_retry_count_down--;}\n void reset(){last_is_auto_shoot_key_down=is_auto_shooting=false;fast_retry_count_down=0;}\n};\n}\n`,
 'th15_web/cpp/game/PracticeKeyMonitor.hpp':`${preamble}#pragma once\n#include <cstdint>\n#include <deque>\n#include <vector>\n#include <string>\nnamespace th15 {\n${keyEnum}\nstruct PracticeKeyMonitor {\n std::vector<int> g_recorded_aps;\n std::vector<uint16_t> g_recorded_keys;\n bool g_keys_down[END]{};uint32_t g_key_mask[END]{};int g_aps_cur=0;bool g_record_key_aps=false;std::deque<uint32_t> keys_per_sec;\n${record}\n void clear_record(){g_recorded_aps.clear();g_recorded_keys.clear();}\n std::string csv()const{std::string out="frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\\n";for(size_t j=0;j<g_recorded_aps.size();j++){out+=std::to_string(j+1)+","+std::to_string(g_recorded_aps[j]);for(int i=0;i<END;i++)out+=(g_recorded_keys[j]&(1<<i))?",O":",-";out+="\\n";}return out;}\n};\n}\n`,
 'th15_web/cpp/sdl/PracticeKeyHud.inc':preamble+style+hud,
 'th15_web/cpp/sdl/PracticeReaction.inc':preamble+reactionClass.replaceAll('LARGE_INTEGER','PracticeCounter')+'\n'+reaction,
 'th15_web/cpp/sdl/PracticeBlind.inc':preamble+blindOptions+blind,
};
if(process.argv.includes('--write')){for(const [p,v]of Object.entries(files))writeFileSync(resolve(root,p),v);console.log('Generated purple shared key monitor and reaction test');}
else if(process.argv.includes('--check')){for(const[p,v]of Object.entries(files))if(!existsSync(resolve(root,p))||readFileSync(resolve(root,p),'utf8').replaceAll('\r\n','\n').trimEnd()!==v.trimEnd())throw Error('Stale shared tools: '+p);console.log('Purple shared tool extraction verified');}
else throw Error('Use --write or --check');
