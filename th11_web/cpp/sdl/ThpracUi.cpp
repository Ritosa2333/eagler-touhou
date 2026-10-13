#include "ThpracUi.hpp"
#include "../game/GameSession.hpp"
#include "../game/GameEconomy.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeSections.hpp"
#include "../game/PracticeUiLabels.hpp"
#include "../game/PracticeVersion.hpp"
#include "../game/PracticeLicense.hpp"
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include "../../../portable/sdl/Renderer.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <eagler/thprac/PracticeMobilePanels.hpp>
namespace MobileThprac = eagler::thprac::mobile;
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>
#include <deque>
#include <functional>
#include <random>
#include <ctime>
#include <cmath>

namespace th11::browser::ThpracUi {
namespace {
bool initialized=false,frame_open=false,menu_open=false,tracker_open=false,advanced_open=false,practice_was_open=false,practice_keys_armed=false,text_editing=false,desktop_pointer=false;
bool key_down[256]{},key_pressed[256]{};float mouse_x=-FLT_MAX,mouse_y=-FLT_MAX;bool mouse_down=false;
struct PointerEdge {bool down;float x,y;};std::deque<PointerEdge> pointer_edges;bool pointer_sample_down=false;
int locale=0,practice_section_index=0;
// ImGui runs one frame per fixed 60 Hz tick (update_input). High-refresh
// presentation passes must re-render the cached draw data only: starting a
// new ImGui frame per present consumed edge-triggered input (typed digits)
// several times per press and ran ImGui's clock several times fast.
unsigned input_generation=0,rendered_generation=~0u;bool frame_drawn=false;

enum Vk {VK_BACK=8,VK_TAB=9,VK_RETURN=13,VK_SHIFT=16,VK_CONTROL=17,VK_MENU=18,VK_ESCAPE=27,VK_SPACE=32,VK_PRIOR=33,VK_NEXT=34,VK_END=35,VK_HOME=36,VK_LEFT=37,VK_UP=38,VK_RIGHT=39,VK_DOWN=40,VK_INSERT=45,VK_DELETE=46,VK_1=49,VK_2=50,VK_3=51,VK_X=88,VK_Z=90,VK_F1=112,VK_F7=118,VK_F12=123};
const char* tr(const char* zh,const char* en,const char* ja){return locale==0?zh:locale==2?ja:en;}
const char* label(const char* const* values){return values[locale];}
void help_marker(const char* const* description){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(description));}
eagler::thprac::PracticeKeyMonitor key_monitor;
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::mt19937::result_type(std::time(nullptr))));}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include <eagler/thprac/PracticeSpeed.inc>
void key_monitor_options(GameSession& runtime){
 ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&runtime.practice.show_keyboard_monitor);if(!runtime.practice.show_keyboard_monitor)return;
 if(!key_monitor.g_record_key_aps){if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){key_monitor.clear_record();key_monitor.g_record_key_aps=true;}}
 else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
 ImGui::SameLine();if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT))){const auto csv=key_monitor.csv();EM_ASM({const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},csv.c_str());}
 const auto& aps=key_monitor.g_recorded_aps;if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int idx){const auto& a=*static_cast<const std::vector<int>*>(data);return float(a[(a.size()>600?a.size()-600:0)+idx]);},const_cast<std::vector<int>*>(&aps),int(std::min<size_t>(600,aps.size())),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void input_options(GameSession& runtime){
 auto& input=runtime.practice.input;auto check=[](const char* const* name,bool& value,const char* const* description=nullptr){ImGui::Checkbox(label(name),&value);if(description){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(description));}};
 check(practice_TH_ADV_DISABLE_X_KEY,input.disable_xkey,practice_TH_ADV_DISABLE_X_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_SHIFT_KEY,input.disable_shiftkey,practice_TH_ADV_DISABLE_SHIFT_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_Z_KEY,input.disable_zkey,practice_TH_ADV_DISABLE_Z_KEY_DESC);
 check(practice_TH_ADV_DISABLE_C_KEY_SAMETIME,input.disable_Ckey_at_same_time);ImGui::SameLine();check(practice_TH_ADV_FORCE_SHIFT_KEY,input.force_shiftkey);check(practice_THPRAC_FAST_RETRY,input.enable_fast_retry,practice_THPRAC_FAST_RETRY_DESC2);
}
void draw_advanced(GameSession& runtime){
 auto& state=runtime.practice;ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});
 if(ImGui::CollapsingHeader(label(practice_TH_GAME_SPEED)))GameFPSOpt(state.speed,true);
 if(ImGui::CollapsingHeader(label(practice_TH_GAMEPLAY))){
  ImGui::Checkbox("fix stage 6 replay",&state.fix_stage6_replay);input_options(runtime);key_monitor_options(runtime);
  ImGui::Checkbox(label(practice_THPRAC_INFLIVES_MAP),&state.map_inf_life_to_no_continue);
  bool hint=runtime.config.bytes[0x22]!=0;if(ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH11_SHOW_HINT),&hint))runtime.config.bytes[0x22]=hint;help_marker(practice_THPRAC_INGAMEINFO_ADV_DESC1);help_marker(practice_THPRAC_INGAMEINFO_ADV_DESC2);
  ImGui::Checkbox(label(practice_TH_BOSS_FORCE_MOVE_DOWN),&state.force_boss_move_down);help_marker(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC);ImGui::SameLine();ImGui::SetNextItemWidth(180);ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&state.boss_move_down_range,.002f,0,1);state.boss_move_down_range=std::clamp(state.boss_move_down_range,0.f,1.f);
  ImGui::Checkbox(label(practice_TH_DISABLE_MASTER),&state.disable_master_display);help_marker(practice_TH_DISABLE_MASTER_DESC);
  ImGui::Checkbox(label(practice_TH11_MARISAB_LOCK),&state.lock_marisa_b);help_marker(practice_TH11_MARISAB_LOCK_DESC);if(state.lock_marisa_b){const char* names[5]{};for(int i=0;i<5;i++)names[i]=label(practice_formations[i]);ImGui::SameLine();ImGui::SetNextItemWidth(180);ImGui::Combo(label(practice_TH11_MARISAB_FORMATION_LABEL),&state.locked_formation,names,5);}
  ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&state.show_lock_timer);ImGui::Checkbox(label(practice_TH_FACTOR_ACB),&state.all_clear_bonus);help_marker(practice_TH_FACTOR_ACB_DESC);
 }
 if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);else reaction_test.Reset();
 if(ImGui::CollapsingHeader(label(practice_TH_ABOUT_THPRAC))){ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_source_version);ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));ImGui::NewLine();ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");ImGui::NewLine();static bool show_license=false;if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;if(show_license){ImGui::BeginChild("COPYING",{0,384},true);ImGui::TextUnformatted(practice_license);ImGui::EndChild();}}
 ImGui::EndChild();
}
bool pressed(int vk){return vk>=0&&vk<256&&key_pressed[vk];}
u32 bridge_keys(){return u32(EM_ASM_INT({return (Module.eaglerControls?.thpracKeyboardBits||0)|0;}));}
bool bridge_key_down(int vk,u32 bits){
 if(vk==VK_BACK)return bits&1u;
 if(vk>=VK_F1&&vk<=VK_F7)return bits&(1u<<(vk-VK_F1+1));
 if(vk==VK_TAB)return bits&(1u<<8);
 if(vk==VK_F12)return bits&(1u<<9);
 if(vk=='U')return bits&(1u<<10);
 return false;
}
void publish_menu(bool open){
 EM_ASM({const value=!!$0;if(Module.eaglerThpracMenuOpen===value)return;Module.eaglerThpracMenuOpen=value;window.dispatchEvent(new CustomEvent('eagler-thprac-menu',{detail:{open:value}}));},open?1:0);
}
// In-game test: the gameplay session object exists for the whole run.
bool in_game(GameSession& runtime){return bool(runtime.battle);}
// The overlay draws onto the same GPU backbuffer the game presents.
u32 backbuffer(GameSession&){return 1;}
void toggle_cheat(GameSession& runtime,int bit){
 auto& p=runtime.practice;if(p.replay)return;p.cheats^=1u<<bit;if(p.cheats)p.assisted=true;
}
void hotkey_line(const char* key,const char* label,bool& value){
 if(MobileThprac::enabled){MobileThprac::checkbox(key,label,value);return;}
 const auto cursor=ImGui::GetCursorPos();if(value)ImGui::TextColored({0,1,0,1},"[%s: %s]",key,label);else ImGui::Text("%s: %s",key,label);
 ImGui::SetCursorPos(cursor);const ImVec2 size{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()};if(ImGui::InvisibleButton(key,size))value=!value;
}
// thprac_th11.cpp:302-317.
bool section_has_dialogue(int section){
 switch(section){
 case TH11_ST1_BOSS1:case TH11_ST2_BOSS1:case TH11_ST3_BOSS1:case TH11_ST4_BOSS1:
 case TH11_ST5_BOSS1:case TH11_ST6_BOSS1:case TH11_ST6_MID1:case TH11_ST7_END_NS1:case TH11_ST7_MID1:
  return true;
 default:return false;
 }
}
// Upstream GuiCombo hides entries whose name is empty for the current
// difficulty (ComboSections skips them, CheckComboItemNew cannot land on
// them); e.g. stage 1's midboss spell exists only on Hard/Lunatic.

std::vector<const PracticeSectionLabel*> matching_sections(GameSession& runtime){
 auto& s=runtime.practice;auto& p=s.configured;int appearance=p.stage+1;
 if(p.stage==3)appearance=s.spell_category?7+s.spell_category:4;
 const int difficulty=p.stage==6?4:runtime.title->selection.difficulty;std::vector<const PracticeSectionLabel*> out;
 for(const auto& l:practice_section_labels){if(l.appearance!=appearance)continue;if(s.warp==2&&l.group!=1)continue;if(s.warp==3&&l.group!=2)continue;if(s.warp==4&&l.spell)continue;if(s.warp==5&&!l.spell)continue;if(!*l.names[difficulty][locale])continue;out.push_back(&l);}return out;
}
void select_current_section(GameSession& runtime){
 auto& s=runtime.practice;auto& p=s.configured;
 if(!s.warp){p.section=p.phase=0;return;}
 if(s.warp==1){constexpr int counts[]{4,4,3,7,6,6,7};p.section=10000+(p.stage+1)*100+std::clamp(p.section>=10000?p.section%100:1,1,counts[p.stage]);return;}
 auto labels=matching_sections(runtime);auto found=std::find_if(labels.begin(),labels.end(),[&](auto* l){return l->id==p.section;});practice_section_index=found==labels.end()?0:int(found-labels.begin());p.section=labels.empty()?0:labels[practice_section_index]->id;
}
void draw_practice(GameSession& runtime){
 auto& s=runtime.practice;auto& p=s.configured;if(!s.menu||!runtime.title)return;
 if(!practice_was_open){practice_was_open=true;practice_keys_armed=false;select_current_section(runtime);}
 const ImVec2 size=locale==0?ImVec2(320,335):locale==1?ImVec2(440,325):ImVec2(340,335),pos=locale==0?ImVec2(150,80):locale==1?ImVec2(100,90):ImVec2(130,80);
 ImGui::SetNextWindowSize(size,ImGuiCond_Always);ImGui::SetNextWindowPos(pos,ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
 const auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
 if(ImGui::Begin("Option###th11-thprac-practice",nullptr,flags)){
  ImGui::PushItemWidth(locale==0?-56.f:locale==1?-58.f:-66.f);ImGui::TextUnformatted(tr("练习选项","Option","オプション"));ImGui::Separator();
  const char* modes[]{tr("原版练习","Original","オリジナル"),tr("自定义练习","Custom","カスタム")};ImGui::Combo(tr("模式","Mode","モード"),&p.mode,modes,2);
  const char* stages[]{"1","2","3","4","5","6","Extra"};if(ImGui::Combo(tr("关卡","Stage","ステージ"),&p.stage,stages,7)){p.section=p.phase=0;if(p.mode&&p.stage==3)s.spell_category=runtime.title->selection.character*3+runtime.title->selection.partner+1;}
  if(p.mode){
   const char* warps[]{tr("无","None","なし"),tr("道中","Stage Portion","道中"),tr("道中Boss","Mid Boss","道中ボス"),tr("关底Boss","End Boss","ボス"),tr("非符","Non Spell","通常"),tr("符卡","Spell Card","スペカ")};
   if(ImGui::Combo(tr("传送","Warp","ワープ"),&s.warp,warps,6))p.section=p.phase=0;
   if(s.warp&&p.stage==3){const char* shots[7]{};for(int i=0;i<7;++i)shots[i]=label(practice_shot_categories[i]);if(ImGui::Combo(label(practice_TH11_SPELL_CATEGORY),&s.spell_category,shots,7))p.section=p.phase=0;}
   select_current_section(runtime);
   if(s.warp==1){constexpr int setup[7][2]{{2,2},{2,2},{2,1},{3,4},{3,3},{3,3},{4,3}};const auto& c=setup[p.stage];int chapter=p.section%100;char label[64];std::snprintf(label,sizeof(label),chapter<=c[0]?tr("前半 #%d","First Half #%d","前半 #%d"):tr("后半 #%d","Second Half #%d","後半 #%d"),chapter<=c[0]?chapter:chapter-c[0]);if(ImGui::SliderInt(tr("章节","Chapter","チャプター"),&chapter,1,c[0]+c[1],label))p.section=10000+(p.stage+1)*100+chapter;}
   else if(s.warp>=2){auto labels=matching_sections(runtime);std::vector<const char*> names;for(auto* l:labels)names.push_back(l->names[p.stage==6?4:runtime.title->selection.difficulty][locale]);if(!names.empty()&&ImGui::Combo(warps[s.warp],&practice_section_index,names.data(),int(names.size()))){p.section=labels[practice_section_index]->id;p.phase=0;}if(section_has_dialogue(p.section))ImGui::Checkbox(tr("对话","Dialog","会話"),&p.dlg);}
   const int phase_count=practice_phase_count(p.section);
   if(phase_count>1){const auto* labels=p.section==TH11_ST6_BOSS9?practice_phase_five:p.section==TH11_ST7_END_S10?practice_phase_four:p.section==10504?practice_phase_infinite:p.section==TH11_ST5_MID3?practice_phase_wave:practice_phase_startset;const char* names[8]{};for(int i=0;i<phase_count;++i)names[i]=labels[i][locale];p.phase=std::clamp(p.phase,0,phase_count-1);ImGui::Combo(tr("阶段","Phase","段階"),&p.phase,names,phase_count);}else p.phase=0;
   if((p.section==TH11_ST4_RA2||p.section==TH11_ST4_RA_BOSS5)&&p.phase){ImGui::DragFloat(practice_TH_BOSSX[locale],&p.boss_x,1,-140,140);ImGui::DragFloat(practice_TH_BOSSY[locale],&p.boss_y,1,80,176);}
   if(p.section==10504)ImGui::SliderInt(practice_TH_11_WAVE_PASSED[locale],&p.wave_passed,0,40);
   ImGui::SliderInt(tr("残机","Life","残機"),&p.life,0,9);ImGui::SliderInt(tr("残机碎片","Life Pieces","残機の欠片"),&p.life_fragment,0,4);
   const bool ma=runtime.title->selection.character==1&&runtime.title->selection.partner==0;char power[32];const int fixed=ma?p.power*100/12:p.power*5;std::snprintf(power,sizeof(power),"%d.%02d",fixed/100,fixed%100);p.power=std::clamp(p.power,0,ma?96:80);ImGui::SliderInt(tr("火力","Power","霊力"),&p.power,0,ma?96:80,power);
   ImGui::DragInt("Graze",&p.graze,1,0,999999);ImGui::DragInt(tr("最大得点","Point Value","最大得点"),&p.value,10,0,999990);p.value=p.value/10*10;
   const i64 lo=0,hi=9999999990LL;ImGui::DragScalar(tr("分数","Score","スコア"),ImGuiDataType_S64,&p.score,10,&lo,&hi,"%lld");p.score=p.score/10*10;
   if(runtime.title->selection.character==1&&runtime.title->selection.partner==1){const char* f[5]{};for(int i=0;i<5;++i)f[i]=label(practice_formations[i]);ImGui::Combo(label(practice_TH11_MARISAB_FORMATION_LABEL),&p.marisa_b_formation,f,5);}
  }
  ImGui::PopItemWidth();if(!ImGui::IsAnyItemActive()&&!ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))ImGui::SetWindowFocus();
 }ImGui::End();ImGui::PopStyleVar(2);
 if(!practice_keys_armed&&!(key_down[VK_Z]||key_down[VK_RETURN]||key_down[VK_X]||key_down[VK_ESCAPE]))practice_keys_armed=true;
 const bool busy=text_editing;text_editing=ImGui::IsAnyItemActive();
 if(practice_keys_armed&&!busy&&(pressed(VK_Z)||pressed(VK_RETURN))&&p.valid())s.accepted=true;
 if(practice_keys_armed&&!busy&&(pressed(VK_X)||pressed(VK_ESCAPE))){s.menu=false;practice_was_open=false;runtime.title->change(TitleScreen::Partner);runtime.title->cursor.pop();}
}
void draw_overlay(GameSession& runtime){
 auto& state=runtime.practice;if(!state.enabled)return;
 if(MobileThprac::show(MobileThprac::Cheats,menu_open)){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Cheats, "Mod Menu###th11-thprac-overlay", flags, locale)){static const char* keys[]{"F1","F2","F3","F4","F5"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム")};
   const bool disabled=state.replay||!in_game(runtime);ImGui::BeginDisabled(disabled);for(int i=0;i<5;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],i==1?label(practice_TH_INFLIVES2):labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F6",label(practice_TH_EL_BGM),value);state.everlasting_bgm=value;bool enemy=state.cheats&32;hotkey_line("U",label(practice_TH_ENEMY_MUTEKI),enemy);if(enemy!=bool(state.cheats&32))toggle_cheat(runtime,5);ImGui::EndDisabled(disabled);hotkey_line("F7 / Tab",label(practice_THPRAC_INGAMEINFO),tracker_open);
  }ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Information,tracker_open)&&in_game(runtime)){
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,175},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Information, "Tracker###th11-thprac-tracker", flags, locale)){const int shot=std::clamp(runtime.state.character*3+runtime.state.subtype,0,5);char title[160];std::snprintf(title,sizeof(title),"%s (%s)",practice_tracker_difficulties[std::clamp(runtime.state.difficulty,0,4)][locale],practice_tracker_shots[shot][locale]);const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);ImGui::Columns(2);ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_MISS_COUNT));ImGui::NextColumn();ImGui::Text("%8d",int(state.tracker_misses));ImGui::NextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_BOMB_COUNT));ImGui::NextColumn();ImGui::Text("%8d",int(state.tracker_bombs));ImGui::Columns(1);}
  ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Advanced,advanced_open)){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(MobileThprac::begin(MobileThprac::Advanced, "Advanced Options###th11-thprac-advanced", flags, locale)){draw_advanced(runtime);ImGui::SetWindowFocus();}ImGui::End();ImGui::PopStyleVar(2);
 }
}
}

bool initialize(){
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;}))return true;
 if(initialized)return true;IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;
 io.KeyMap[ImGuiKey_Tab]=VK_TAB;io.KeyMap[ImGuiKey_LeftArrow]=VK_LEFT;io.KeyMap[ImGuiKey_RightArrow]=VK_RIGHT;io.KeyMap[ImGuiKey_UpArrow]=VK_UP;io.KeyMap[ImGuiKey_DownArrow]=VK_DOWN;io.KeyMap[ImGuiKey_PageUp]=VK_PRIOR;io.KeyMap[ImGuiKey_PageDown]=VK_NEXT;io.KeyMap[ImGuiKey_Home]=VK_HOME;io.KeyMap[ImGuiKey_End]=VK_END;io.KeyMap[ImGuiKey_Insert]=VK_INSERT;io.KeyMap[ImGuiKey_Delete]=VK_DELETE;io.KeyMap[ImGuiKey_Backspace]=VK_BACK;io.KeyMap[ImGuiKey_Space]=VK_SPACE;io.KeyMap[ImGuiKey_Enter]=VK_RETURN;io.KeyMap[ImGuiKey_Escape]=VK_ESCAPE;io.KeyMap[ImGuiKey_KeyPadEnter]=VK_RETURN;io.KeyMap[ImGuiKey_A]='A';io.KeyMap[ImGuiKey_C]='C';io.KeyMap[ImGuiKey_V]='V';io.KeyMap[ImGuiKey_X]='X';io.KeyMap[ImGuiKey_Y]='Y';io.KeyMap[ImGuiKey_Z]='Z';
 ImGui::StyleColorsDark();locale=EM_ASM_INT({const v=String(Module.eaglerOptions?.thpracLocale||'');return v.startsWith('ja')?2:v.startsWith('en')?1:0;});ImFontConfig config{};config.FontNo=0;config.RasterizerMultiply=1.25f;config.OversampleH=5;config.OversampleV=5;ImFontGlyphRangesBuilder glyphs;glyphs.AddRanges(io.Fonts->GetGlyphRangesChineseFull());glyphs.AddText("↑←↓→ΔΣ");static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);const ImWchar* range=ranges.Data;
 // Keep the game's own font for the TH10 game renderer, but always render
 // thprac with Unifont. Some spell/option labels contain CJK glyphs missing
 // from the bundled font even when the UI locale itself is Japanese or English.
 // The launcher mounts /unifont.otf whenever thprac is enabled.
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,range);
 if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}initialized=true;return true;
}
void shutdown(){if(!initialized)return;if(frame_open){ImGui::EndFrame();frame_open=false;}publish_menu(false);ImGui::DestroyContext();initialized=false;}
void process_event(const SDL_Event& event){if(!initialized)return;if(event.type==SDL_EVENT_MOUSE_MOTION){if(event.motion.which==SDL_TOUCH_MOUSEID)return;desktop_pointer=true;mouse(0,event.motion.x,event.motion.y);}else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP){if(event.button.which==SDL_TOUCH_MOUSEID)return;desktop_pointer=true;if(event.button.button==SDL_BUTTON_LEFT)mouse(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN?1:2,event.button.x,event.button.y);}else if(event.type==SDL_EVENT_MOUSE_WHEEL){ImGui::GetIO().MouseWheel+=event.wheel.y;ImGui::GetIO().MouseWheelH+=event.wheel.x;}}
void mouse(int type,float x,float y){mouse_x=x;mouse_y=y;if(type==1||type==2){const bool down=type==1;if(down!=mouse_down){if(pointer_edges.size()>=64){cancel_pointer();return;}pointer_edges.push_back({down,x,y});mouse_down=down;}}}
void cancel_pointer(){pointer_edges.clear();mouse_down=pointer_sample_down=false;mouse_x=mouse_y=-FLT_MAX;}
bool captures_pointer(float x,float y){
 if(!initialized)return false;
 // ImGui owns taps on its windows/popups. Do not synthesize a second Z
 // activation from the game's menu gesture when selecting a combo item.
 if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
 for(auto* window:ImGui::GetCurrentContext()->Windows)if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
 return false;
}
void update_input(GameSession& runtime,const bool* keys){
 if(!initialized)return;++input_generation;const u32 bits=bridge_keys();for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}
 auto& state=runtime.practice;state.record_keys=[&state](u32 held){if(state.show_keyboard_monitor)key_monitor.record(11,held);};
 if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}
 if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;
 if((pressed(VK_TAB)||pressed(VK_F7))&&!ImGui::IsAnyItemActive()&&in_game(runtime))tracker_open=!tracker_open;
 if(pressed(VK_F12))advanced_open=!advanced_open;
 if(menu_open&&in_game(runtime)&&!state.replay){for(int i=0;i<5;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F1+5))state.everlasting_bgm=!state.everlasting_bgm;if(pressed('U'))toggle_cheat(runtime,5);}
 if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);
}
void apply_input(GameSession& runtime,bool* keys){
 auto& state=runtime.practice;if(!state.enabled||!runtime.battle||state.replay||captures_game_input())return;
 u8 vk[256]{};for(int i=0;i<256;i++)vk[i]=keys[i]?128:0;
 vk[160]|=vk[16];state.input.apply(vk);vk[16]=vk[160]|vk[161];for(int i=0;i<256;i++)keys[i]=vk[i]!=0;state.input.gui_tick();
 if(state.input.disable_xkey||state.input.disable_zkey||state.input.disable_shiftkey||state.input.force_shiftkey||state.input.enable_fast_retry)state.assisted=true;
}
double simulation_interval(GameSession& runtime){return runtime.practice.enabled?runtime.practice.speed.interval(runtime.state.replay,key_down[VK_CONTROL],key_down[VK_SHIFT],key_down[VK_SPACE]):1./60.;}
bool captures_game_input(){return MobileThprac::captures_advanced(advanced_open)||practice_was_open;}
void render(GameSession& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};if(!pointer_edges.empty()){const auto edge=pointer_edges.front();pointer_edges.pop_front();pointer_sample_down=edge.down;io.MousePos={edge.x,edge.y};}io.MouseDown[0]=pointer_sample_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Desktop thprac numeric fields should be directly editable: ImGui's drag
 // widgets can now switch to TempInputText on a click-release without a drag.
 // Queue numeric characters for the whole practice-menu frame; ImGui clears
 // unused characters at EndFrame, while an active TempInputText consumes them.
 if(runtime.practice.menu){for(int vk=48;vk<=57;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-48));for(int vk=96;vk<=105;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-96));if(pressed(189)||pressed(109))io.AddInputCharacter('-');if(pressed(190)||pressed(110))io.AddInputCharacter('.');}
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 // Persistent touch fire must not keep the closed Practice menu capturing
 // gameplay inputs (including the launcher's Escape serial pulse).
 if(runtime.practice.menu)draw_practice(runtime);else practice_was_open=false;draw_overlay(runtime);
 if(runtime.practice.show_keyboard_monitor&&in_game(runtime)&&MobileThprac::draw_tool(MobileThprac::Keyboard,"Keys###th11-mobile-keys",{420,65},locale)){
  KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);
  KeysHUD(11,{1280,0},{840,0},style,true,false);
 }
 const bool tool_visible=(!runtime.practice.force_boss_move_down&&!(runtime.practice.show_lock_timer&&in_game(runtime)&&(runtime.practice.cheats&8)))||MobileThprac::draw_tool(MobileThprac::Indicators,"Tools###th11-mobile-tools",{120,65},locale);
 if(tool_visible&&runtime.practice.force_boss_move_down){auto* p=ImGui::GetOverlayDrawList();const auto size=ImGui::CalcTextSize(label(practice_TH_BOSS_FORCE_MOVE_DOWN));p->AddRectFilled({120,0},{120+size.x,size.y},0xffcccccc);p->AddText({120,0},0xffff0000,label(practice_TH_BOSS_FORCE_MOVE_DOWN));}
 if(tool_visible&&runtime.practice.show_lock_timer&&in_game(runtime)&&(runtime.practice.cheats&8)){char text[32];std::snprintf(text,sizeof text,"%.2f",float(runtime.practice.lock_frames)/60.f);const auto size=ImGui::CalcTextSize(text);auto* p=ImGui::GetOverlayDrawList();p->AddRectFilled({32,0},{110,size.y},0xffffffff);p->AddText({110-size.x,0},0xff000000,text);}
 ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));frame_drawn=true;
}
}
