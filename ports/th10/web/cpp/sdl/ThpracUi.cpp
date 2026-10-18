#include "ThpracUi.hpp"
#include "../platform/Application.hpp"
#include "../platform/GameState.hpp"
#include "../platform/World.hpp"
#include "../platform/Title.hpp"
#include "../game/InputDevices.hpp"
#include "../game/GameEconomy.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeSections.hpp"
#include "../game/PracticeUiLabels.hpp"
#include "../game/PracticeVersion.hpp"
#include "../game/PracticeGameplay.hpp"
#include "../game/StageHints.hpp"
#include "../game/PracticeLicense.hpp"
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include "Renderer.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <eagler/thprac/PracticeMobilePanels.hpp>
namespace MobileThprac = eagler::thprac::mobile;
#include "../../../portable/sdl/third_party/stb_image.h"
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

namespace th10::browser::ThpracUi {
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
std::string clipboard_text;
const char* clipboard_get(void*){return clipboard_text.c_str();}
void clipboard_set(void*,const char* text){clipboard_text=text?text:"";EM_ASM({navigator.clipboard?.writeText(UTF8ToString($0)).catch(()=>{});},clipboard_text.c_str());}
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::mt19937::result_type(std::time(nullptr))));}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include "PracticeHints.inc"
#include <eagler/thprac/PracticeSpeed.inc>
ImTextureID practice_blind_image(Application&,const unsigned char* fallback,size_t length){
 size_t bytes=0;auto* custom=static_cast<unsigned char*>(SDL_LoadFile("/blind.png",&bytes));int w=0,h=0,channels=0;
 auto* image=custom&&bytes<=64*1024*1024?stbi_load_from_memory(custom,int(bytes),&w,&h,&channels,4):nullptr;SDL_free(custom);
 if(!image)image=stbi_load_from_memory(fallback,int(length),&w,&h,&channels,4);
 if(!image)return nullptr;auto* renderer=touhou::sdl::current();const auto texture=renderer?renderer->create_imgui_texture(w,h,image):0;stbi_image_free(image);return reinterpret_cast<ImTextureID>(uintptr_t(texture));
}
#include "PracticeBlind.inc"
void release_blind_image(){if(g_blind_view_opt.blind_texture){if(auto* r=touhou::sdl::current())r->release_imgui_texture(u32(uintptr_t(g_blind_view_opt.blind_texture)));g_blind_view_opt.blind_texture=nullptr;}g_blind_view_opt.is_texture_failed=false;}
void secret_options(Application& app){
 if(!ImGui::CollapsingHeader("Super Secret Settings"))return;
 if(ImGui::Button("ass bullet")){app.state.practice.flip_screen_y=!app.state.practice.flip_screen_y;cancel_pointer();}
 ImGui::Checkbox(label(practice_THPRAC_BLIND),&g_blind_view_opt.blind_view);help_marker(practice_THPRAC_BLIND_DESC);ImGui::SameLine();ImGui::SetNextItemWidth(75);
 ImGui::DragFloat(label(practice_THPRAC_BLIND_SZ),&g_blind_view_opt.blind_size,1,20,600);ImGui::SameLine();
 if(ImGui::Button((std::string(label(practice_THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_RELOAD))+"##blind_reload").c_str()))release_blind_image();
}
void key_monitor_options(Application& runtime){
 ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&runtime.state.practice.show_keyboard_monitor);if(!runtime.state.practice.show_keyboard_monitor)return;
 if(!key_monitor.g_record_key_aps){if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){key_monitor.clear_record();key_monitor.g_record_key_aps=true;}}
 else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
 ImGui::SameLine();if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT))){const auto csv=key_monitor.csv();EM_ASM({const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},csv.c_str());}
 const auto& aps=key_monitor.g_recorded_aps;if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int idx){const auto& a=*static_cast<const std::vector<int>*>(data);return float(a[(a.size()>600?a.size()-600:0)+idx]);},const_cast<std::vector<int>*>(&aps),int(std::min<size_t>(600,aps.size())),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void input_options(Application& runtime){
 auto& input=runtime.state.practice.input;auto check=[](const char* const* name,bool& value,const char* const* description=nullptr){ImGui::Checkbox(label(name),&value);if(description){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(description));}};
 check(practice_TH_ADV_DISABLE_X_KEY,input.disable_xkey,practice_TH_ADV_DISABLE_X_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_SHIFT_KEY,input.disable_shiftkey,practice_TH_ADV_DISABLE_SHIFT_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_Z_KEY,input.disable_zkey,practice_TH_ADV_DISABLE_Z_KEY_DESC);
 check(practice_TH_ADV_DISABLE_C_KEY_SAMETIME,input.disable_Ckey_at_same_time);ImGui::SameLine();check(practice_TH_ADV_FORCE_SHIFT_KEY,input.force_shiftkey);check(practice_THPRAC_FAST_RETRY,input.enable_fast_retry,practice_THPRAC_FAST_RETRY_DESC2);
}
void gameplay_options(Application& runtime){
 auto& p=runtime.state.practice;
 ImGui::Checkbox(label(practice_TH_BOSS_FORCE_MOVE_DOWN),&p.force_boss_move_down);help_marker(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC);
 ImGui::SameLine();ImGui::SetNextItemWidth(180);
 if(ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&p.boss_move_down_range,.002f,0,1))p.boss_move_down_range=std::clamp(p.boss_move_down_range,0.f,1.f);
 if(ImGui::Button(label(practice_TH_ONE_KEY_DIE))&&runtime.world&&runtime.world->actors.player){
  // Purple F12 directly sets state 4 (deathbomb window), not Player::die().
  runtime.state.game.lives=-1;runtime.state.game.power=0;
  runtime.world->actors.player->state=4;p.assisted=true;
 }
 help_marker(practice_TH_ONE_KEY_DIE_DESC);
 // Native TH10 exposes this as explanatory text, not a runtime toggle.
 ImGui::TextUnformatted(label(practice_TH_DISABLE_MASTER_10_DESC));help_marker(practice_TH_DISABLE_MASTER_DESC);
 ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&p.enable_lock_timer);
 if(ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH10_SHOW_POINT),&p.show_point_items))p.tracker_white=p.tracker_yellow=0;
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
bool in_game(Application& runtime){return runtime.world&&runtime.world->actors.session;}
// The overlay draws onto the same GPU backbuffer the game presents.
u32 backbuffer(Application& runtime){return u32(reinterpret_cast<uintptr_t>(runtime.engine.device.back_surface()));}
void toggle_cheat(Application& runtime,int bit){
 auto& p=runtime.state.practice;if(p.replay)return;p.cheats^=1u<<bit;if(p.cheats)p.assisted=true;
}
void hotkey_line(const char* key,const char* label,bool& value){
 if(MobileThprac::enabled){MobileThprac::checkbox(key,label,value);return;}
 const auto cursor=ImGui::GetCursorPos();if(value)ImGui::TextColored({0,1,0,1},"[%s: %s]",key,label);else ImGui::Text("%s: %s",key,label);
 ImGui::SetCursorPos(cursor);const ImVec2 size{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()};if(ImGui::InvisibleButton(key,size))value=!value;
}
// thprac_th10.cpp:302-317.
bool section_has_dialogue(int section){
 switch(section){
 case TH10_ST1_BOSS1:case TH10_ST2_BOSS1:case TH10_ST3_BOSS1:case TH10_ST4_BOSS1:
 case TH10_ST5_BOSS1:case TH10_ST6_BOSS1:case TH10_ST7_END_NS1:case TH10_ST7_MID1:
  return true;
 default:return false;
 }
}
// Upstream GuiCombo hides entries whose name is empty for the current
// difficulty (ComboSections skips them, CheckComboItemNew cannot land on
// them); e.g. stage 1's midboss spell exists only on Hard/Lunatic.
std::vector<const PracticeSectionLabel*> matching_sections(const PracticeConfig& p, int difficulty){
 std::vector<const PracticeSectionLabel*> out;for(const auto& s:practice_section_labels){if(s.stage!=p.stage)continue;if(p.warp==2&&s.group!=1)continue;if(p.warp==3&&s.group!=2)continue;if(p.warp==4&&s.spell)continue;if(p.warp==5&&!s.spell)continue;const char* name=s.names[std::clamp(difficulty,0,4)][locale];if(!name||!*name)continue;out.push_back(&s);}return out;
}
void select_current_section(PracticeConfig& p, int difficulty){
 if(p.warp==0){p.section=0;return;}if(p.warp==1){static constexpr int counts[]{5,5,7,8,6,4,6};int chapter=p.section>=10000?p.section%100:1;chapter=std::clamp(chapter,1,counts[p.stage]);p.section=10000+(p.stage+1)*100+chapter;return;}
 auto matches=matching_sections(p,difficulty);if(matches.empty()){p.section=0;practice_section_index=0;return;}auto found=std::find_if(matches.begin(),matches.end(),[&](auto* s){return s->id==p.section;});if(found!=matches.end())practice_section_index=int(found-matches.begin());practice_section_index=std::clamp(practice_section_index,0,int(matches.size())-1);p.section=matches[practice_section_index]->id;
}
void draw_practice(Application& runtime){
 auto& state=runtime.state.practice;auto& p=state.configured;
 // Extra is its own difficulty. Every other stage needs a non-Extra difficulty,
 // otherwise returning from an Extra run leaves difficulty 4 and the normal
 // stages expose no valid sections (thprac's Extra names are empty for them).
 static int non_extra_difficulty=1;int difficulty=runtime.state.game.difficulty;
 if(difficulty<4)non_extra_difficulty=difficulty;
 difficulty=(p.stage==6)?4:non_extra_difficulty;
 if(!practice_was_open){practice_was_open=true;practice_keys_armed=false;practice_section_index=0;select_current_section(p,difficulty);}
 const ImVec2 size=locale==0?ImVec2(370,390):locale==1?ImVec2(440,375):ImVec2(380,390);const ImVec2 pos=locale==0?ImVec2(245,75):locale==1?ImVec2(190,75):ImVec2(250,75);
 ImGui::SetNextWindowSize(size,ImGuiCond_Always);ImGui::SetNextWindowPos(pos,ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
 constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
 if(ImGui::Begin("Option###th10-thprac-practice",nullptr,flags)){
  ImGui::PushItemWidth(locale==1?-80.f:locale==2?-65.f:-60.f);ImGui::TextUnformatted(tr("练习选项","Option","オプション"));ImGui::Separator();
  const char* modes[]={tr("原版练习","Original","オリジナル"),tr("自定义练习","Custom","カスタム")};int mode=p.mode?1:0;if(ImGui::Combo(tr("模式","Mode","モード"),&mode,modes,2))p.mode=mode;
  const char* stages[]={"1","2","3","4","5","6","Extra"};if(ImGui::Combo(tr("关卡","Stage","ステージ"),&p.stage,stages,7)){p.section=0;practice_section_index=0;}
  if(p.mode==1){
   const char* warps[]={tr("无","None","なし"),tr("道中","Stage Portion","道中"),tr("道中Boss","Mid Boss","道中ボス"),tr("关底Boss","End Boss","ボス"),tr("非符","Non Spell","通常"),tr("符卡","Spell Card","スペカ")};
   // Stage 6 (zero-based 5) has no midboss: warp 2 is unavailable, like upstream.
   if(p.stage==5){if(p.warp==2)p.warp=0;const char* no_mid[]{warps[0],warps[1],warps[3],warps[4],warps[5]};int wi=p.warp<2?p.warp:p.warp-1;if(ImGui::Combo(tr("传送","Warp","ワープ"),&wi,no_mid,5)){p.warp=wi<2?wi:wi+1;p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}}
   else if(ImGui::Combo(tr("传送","Warp","ワープ"),&p.warp,warps,6)){p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}
   if(p.warp==1){static constexpr int setup[7][2]{{3,2},{3,2},{4,3},{4,4},{4,2},{4,0},{4,2}};const auto& counts=setup[p.stage];int chapter=p.section>=10000?p.section%100:1;
    char portion[64];if(!counts[1])std::snprintf(portion,sizeof(portion),"#%d",chapter);else if(chapter<=counts[0])std::snprintf(portion,sizeof(portion),tr("前半 #%d","First Half #%d","前半 #%d"),chapter);else std::snprintf(portion,sizeof(portion),tr("后半 #%d","Second Half #%d","後半 #%d"),chapter-counts[0]);
    if(ImGui::SliderInt(tr("章节","Chapter","チャプター"),&chapter,1,counts[0]+counts[1],portion))p.section=10000+(p.stage+1)*100+chapter;}
   else if(p.warp>=2&&p.warp<=5){auto matches=matching_sections(p,difficulty);if(!matches.empty()){select_current_section(p,difficulty);std::vector<const char*> names;for(auto* s:matches)names.push_back(s->names[std::clamp(difficulty,0,4)][locale]);if(ImGui::Combo(warps[p.warp],&practice_section_index,names.data(),int(names.size()))){p.section=matches[practice_section_index]->id;p.phase=0;}if(section_has_dialogue(p.section))ImGui::Checkbox(tr("对话","Dialog","会話"),reinterpret_cast<bool*>(&p.dlg));}}
   // thprac_th10.cpp:253-264: section-specific widgets.
   if(p.section==TH10_ST4_MID1||p.section==10408){const char* phases[]={tr("正常","Normal","普通"),p.section==TH10_ST4_MID1?tr("无限时间","Infinite Time","時間無限"):tr("无限模式","Infinite Mode","無限モード")};ImGui::Combo(tr("阶段","Phase","段階"),&p.phase,phases,2);}
   else if(p.section==TH10_ST6_BOSS9)ImGui::SliderInt(tr("延迟","Delay","ディレイ"),&p.st6_boss9_spd,0,160);
   else if(p.section==TH10_ST6_BOSS4||p.section==TH10_ST6_BOSS8)ImGui::Checkbox(tr("真实子弹贴图","Real bullet sprites","弾の見た目を変えない"),reinterpret_cast<bool*>(&p.real_bullet_sprite));
   else if(p.section==TH10_ST7_END_S10){const char* phases[]={tr("正常","Normal","普通"),tr("发狂","Rage","発狂")};int phase=std::clamp(p.phase,0,1);if(ImGui::Combo(tr("阶段","Phase","段階"),&phase,phases,2))p.phase=phase;}
   ImGui::SliderInt(tr("残机","Life","残機"),&p.life,0,9);
   // Upstream displays power as a 2-decimal fixed point of (*mPower * 5).
   char power[32];std::snprintf(power,sizeof(power),"%d.%02d",(p.power*5)/100,(p.power*5)%100);ImGui::SliderInt(tr("火力","Power","霊力"),&p.power,0,100,power);
   ImGui::DragInt(tr("信仰","Faith","信仰"),&p.faith,10,0,999990);p.faith=p.faith/10*10;
   ImGui::SliderInt(tr("信仰条","Faith Bar","信仰ゲージ"),&p.faith_bar,0,130);
   const i64 score_min=0,score_max=9999999990ll;ImGui::DragScalar(tr("分数","Score","スコア"),ImGuiDataType_S64,&p.score,10.f,&score_min,&score_max,"%lld");p.score=p.score/10*10;
  }
  ImGui::PopItemWidth();if(!ImGui::IsAnyItemActive()&&!ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))ImGui::SetWindowFocus();
 }
 ImGui::End();ImGui::PopStyleVar(2);
 // The menu can open on the same tick that confirmed character select (the
 // title chain re-executes the callback via EXECUTE_AGAIN), so the opening
 // Z press is still a fresh edge: ignore confirm/cancel until every such
 // key has been released once, matching upstream waiting for real input.
 if(!practice_keys_armed){if(!(key_down[VK_Z]||key_down[VK_RETURN]||key_down[VK_X]||key_down[VK_ESCAPE]))practice_keys_armed=true;}
 // While an imgui item is active (text edit or drag), Enter/Escape/Z belong
 // to the widget, not the menu; use last frame's state so the confirming
 // keystroke itself is also swallowed.
 const bool widget_busy=text_editing||MobileThprac::captures_advanced(advanced_open);text_editing=ImGui::IsAnyItemActive();
 if(practice_keys_armed&&!widget_busy&&(pressed(VK_Z)||pressed(VK_RETURN))){select_current_section(p,difficulty);state.run=p;state.run.warp=0;state.accepted=true;}
 if(practice_keys_armed&&!widget_busy&&(pressed(VK_X)||pressed(VK_ESCAPE))){state.menu=false;if(runtime.title&&runtime.title->value){runtime.title->value->menu.select(runtime.state.game.stage);
#ifdef TH_ENABLE_THPRAC
  runtime.title->value->set_screen(8,&runtime.engine.speed);
#else
  runtime.title->value->set_screen(9,&runtime.engine.speed);
#endif
 }}
}
void draw_overlay(Application& runtime){
 auto& state=runtime.state.practice;if(!state.enabled)return;
 practice_consume_lock_timer(state);
 drag_hints(runtime);
 if(in_game(runtime)&&runtime.world->actors.player){const auto& p=runtime.world->actors.player->position;RenderBlindView(runtime,{p.x,p.y},{192,0},{32,16},1);if(state.flip_screen_y&&!state.replay)state.assisted=true;}
 if(auto* renderer=touhou::sdl::current())renderer->flip_present_y=state.flip_screen_y;
 const bool tool_visible=(!state.force_boss_move_down&&!(state.enable_lock_timer&&(state.cheats&8u)&&in_game(runtime)))||MobileThprac::draw_tool(MobileThprac::Indicators,"Tools###th10-mobile-tools",{120,65},locale);
 if(tool_visible&&state.force_boss_move_down){const char* text=label(practice_TH_BOSS_FORCE_MOVE_DOWN);const auto size=ImGui::CalcTextSize(text);auto* draw=ImGui::GetForegroundDrawList();draw->AddRectFilled({120,0},{120+size.x,size.y},0xffcccccc);draw->AddText({120,0},0xffff0000,text);}
 if(tool_visible&&state.enable_lock_timer&&(state.cheats&8u)&&in_game(runtime)){
  char timer[32];std::snprintf(timer,sizeof timer,"%.2f",state.lock_timer/60.f);
  const auto size=ImGui::CalcTextSize(timer);auto* draw=ImGui::GetForegroundDrawList();
  draw->AddRectFilled({32,0},{110,size.y},IM_COL32(255,255,255,255));
  draw->AddText({110-size.x,0},IM_COL32(0,0,0,255),timer);
 }
 state.record_keys=[](u32 held){key_monitor.record(10,held);};
 if(state.show_keyboard_monitor&&in_game(runtime)&&MobileThprac::draw_tool(MobileThprac::Keyboard,"Keys###th10-mobile-keys",{420,65},locale)){
  KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);
  KeysHUD(10,{1280,0},{840,0},style,true,false);
 }
 if(MobileThprac::show(MobileThprac::Cheats,menu_open)){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Cheats, "Mod Menu###th10-thprac-overlay", flags, locale)){static const char* keys[]{"F1","F2","F3","F4","F5","F6"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム"),tr("不掉信仰","No faith loss","信仰点減少させない")};
   const bool cheat_disabled=MobileThprac::enabled&&(state.replay||!in_game(runtime));ImGui::BeginDisabled(cheat_disabled);for(int i=0;i<6;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F7",tr("永续BGM","Everlasting BGM","永遠に続くBGM"),value);if(!state.replay)state.everlasting_bgm=value;
   ImGui::EndDisabled(cheat_disabled);hotkey_line("Tab",tr("详细信息","In-game information","ゲーム内情報"),tracker_open);
   ImGui::BeginDisabled(cheat_disabled);value=(state.cheats&64u)!=0;hotkey_line("U",tr("敌方无敌","Enemy Invincibility","敵無敵"),value);if(value!=bool(state.cheats&64u))toggle_cheat(runtime,6);ImGui::EndDisabled(cheat_disabled);
  }ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Information,tracker_open)&&in_game(runtime)){
  static const char* shots[3][6]={{"灵梦A","灵梦B","灵梦C","魔理沙A","魔理沙B","魔理沙C"},{"ReimuA","ReimuB","ReimuC","MarisaA","MarisaB","MarisaC"},{"霊夢A","霊夢B","霊夢C","魔理沙A","魔理沙B","魔理沙C"}};
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,150},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Information, "Tracker###th10-thprac-tracker", flags, locale)){
   const int shot=std::clamp(runtime.state.game.character*3+runtime.state.game.shot_type,0,5);
   static const char* difficulties[]{"Easy","Normal","Hard","Lunatic","Extra"};char title[96];
   std::snprintf(title,sizeof title,"%s (%s)",difficulties[std::clamp(runtime.state.game.difficulty,0,4)],shots[locale][shot]);
   const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);
   if(ImGui::BeginTable("Tracker table",2)){
    auto row=[](const char* name,int a){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(name);ImGui::TableNextColumn();ImGui::Text("%8d",a);};
    row(label(practice_THPRAC_INGAMEINFO_MISS_COUNT),int(state.tracker_misses));row(label(practice_THPRAC_INGAMEINFO_BOMB_COUNT),int(state.tracker_bombs));
    if(state.show_point_items){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH10_POINT));ImGui::TableNextColumn();ImGui::TextColored({1,1,1,1},"%5d / ",int(state.tracker_white));ImGui::SameLine(0,0);ImGui::TextColored({1,1,.5f,1},"%d",int(state.tracker_yellow));}
    ImGui::EndTable();
   }
  }
  ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Advanced,advanced_open)){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(MobileThprac::begin(MobileThprac::Advanced, "Advanced Options###th10-thprac-advanced", flags, locale)){
   ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});
   hint_options(runtime);
   if(ImGui::CollapsingHeader(label(practice_TH_GAME_SPEED))){GameFPSOpt(state.speed,true);ImGui::Checkbox(label(practice_TH_GAME_SPEED_PLSPD_KEEP),&state.keep_player_speed);}
   if(ImGui::CollapsingHeader(label(practice_TH_GAMEPLAY))){
    input_options(runtime);key_monitor_options(runtime);
    ImGui::Checkbox(label(practice_THPRAC_INFLIVES_MAP),&state.map_inf_life_to_no_continue);
    gameplay_options(runtime);
    ImGui::Checkbox(label(practice_TH_FACTOR_ACB),&state.all_clear_bonus);help_marker(practice_TH_FACTOR_ACB_DESC);
   }
   secret_options(runtime);
   if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);else reaction_test.Reset();
   if(ImGui::CollapsingHeader(label(practice_TH_ABOUT_THPRAC))){ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_source_version);ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");static bool show_license=false;if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;if(show_license){ImGui::BeginChild("COPYING",{0,384},true);ImGui::TextUnformatted(practice_license);ImGui::EndChild();}}
   ImGui::EndChild();if(!ImGui::IsAnyItemActive()&&!ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))ImGui::SetWindowFocus();
  }ImGui::End();ImGui::PopStyleVar(2);
 }
}
}

bool initialize(){
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;}))return true;
 if(!initialized){menu_open=tracker_open=advanced_open=practice_was_open=practice_keys_armed=text_editing=desktop_pointer=false;std::fill(std::begin(key_down),std::end(key_down),false);std::fill(std::begin(key_pressed),std::end(key_pressed),false);cancel_pointer();input_generation=0;rendered_generation=~0u;frame_drawn=false;key_monitor.clear_record();key_monitor.g_record_key_aps=false;}
 if(initialized)return true;IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;io.GetClipboardTextFn=clipboard_get;io.SetClipboardTextFn=clipboard_set;
 io.KeyMap[ImGuiKey_Tab]=VK_TAB;io.KeyMap[ImGuiKey_LeftArrow]=VK_LEFT;io.KeyMap[ImGuiKey_RightArrow]=VK_RIGHT;io.KeyMap[ImGuiKey_UpArrow]=VK_UP;io.KeyMap[ImGuiKey_DownArrow]=VK_DOWN;io.KeyMap[ImGuiKey_PageUp]=VK_PRIOR;io.KeyMap[ImGuiKey_PageDown]=VK_NEXT;io.KeyMap[ImGuiKey_Home]=VK_HOME;io.KeyMap[ImGuiKey_End]=VK_END;io.KeyMap[ImGuiKey_Insert]=VK_INSERT;io.KeyMap[ImGuiKey_Delete]=VK_DELETE;io.KeyMap[ImGuiKey_Backspace]=VK_BACK;io.KeyMap[ImGuiKey_Space]=VK_SPACE;io.KeyMap[ImGuiKey_Enter]=VK_RETURN;io.KeyMap[ImGuiKey_Escape]=VK_ESCAPE;io.KeyMap[ImGuiKey_KeyPadEnter]=VK_RETURN;io.KeyMap[ImGuiKey_A]='A';io.KeyMap[ImGuiKey_C]='C';io.KeyMap[ImGuiKey_V]='V';io.KeyMap[ImGuiKey_X]='X';io.KeyMap[ImGuiKey_Y]='Y';io.KeyMap[ImGuiKey_Z]='Z';
 ImGui::StyleColorsDark();locale=EM_ASM_INT({const v=String(Module.eaglerOptions?.thpracLocale||'');return v.startsWith('ja')?2:v.startsWith('en')?1:0;});ImFontConfig config{};config.FontNo=0;config.RasterizerMultiply=1.25f;config.OversampleH=5;config.OversampleV=5;
 ImFontGlyphRangesBuilder glyphs;glyphs.AddRanges(io.Fonts->GetGlyphRangesChineseFull());glyphs.AddText("↑←↓→ΔΣ");static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);
 // Keep the game's own font for the TH10 game renderer, but always render
 // thprac with Unifont. Some spell/option labels contain CJK glyphs missing
 // from the bundled font even when the UI locale itself is Japanese or English.
 // The launcher mounts /unifont.otf whenever thprac is enabled.
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,ranges.Data);
 if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}initialized=true;return true;
}
void shutdown(){if(!initialized)return;if(frame_open){ImGui::EndFrame();frame_open=false;}release_blind_image();g_blind_view_opt={};if(auto* r=touhou::sdl::current())r->flip_present_y=false;move_hints=false;cancel_pointer();publish_menu(false);ImGui::DestroyContext();initialized=false;}
void process_event(const SDL_Event& event){if(!initialized)return;if(event.type==SDL_EVENT_MOUSE_MOTION){if(event.motion.which==SDL_TOUCH_MOUSEID||event.motion.which==SDL_PEN_MOUSEID)return;desktop_pointer=true;mouse(0,event.motion.x,event.motion.y);}else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP){if(event.button.which==SDL_TOUCH_MOUSEID||event.button.which==SDL_PEN_MOUSEID)return;desktop_pointer=true;if(event.button.button==SDL_BUTTON_LEFT)mouse(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN?1:2,event.button.x,event.button.y);}else if(event.type==SDL_EVENT_MOUSE_WHEEL){ImGui::GetIO().MouseWheel+=event.wheel.y;ImGui::GetIO().MouseWheelH+=event.wheel.x;}}
void mouse(int type,float x,float y){if(auto* r=touhou::sdl::current();r&&r->flip_present_y)y=480-y;mouse_x=x;mouse_y=y;if(type==1||type==2){const bool down=type==1;if(down!=mouse_down){if(pointer_edges.size()>=64){cancel_pointer();return;}pointer_edges.push_back({down,x,y});mouse_down=down;}}}
void cancel_pointer(){pointer_edges.clear();mouse_down=pointer_sample_down=false;mouse_x=mouse_y=-FLT_MAX;selected_hint=-1;selected_hint_id=0;}
void update_input(Application& runtime){if(!initialized)return;++input_generation;auto* keys=runtime.input.keyboard_state();const u32 bits=bridge_keys();for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}auto& state=runtime.state.practice;if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;if((pressed(VK_TAB)||pressed(119))&&!ImGui::IsAnyItemActive()&&in_game(runtime))tracker_open=!tracker_open;if(pressed(VK_F12))advanced_open=!advanced_open;if(menu_open&&in_game(runtime)&&!state.replay){for(int i=0;i<6;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F7))state.everlasting_bgm=!state.everlasting_bgm;if(pressed('U'))toggle_cheat(runtime,6);}if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);}
bool captures_game_input(){return MobileThprac::captures_advanced(advanced_open)||practice_was_open;}
bool captures_pointer(float x,float y){
 if(!initialized)return false;
 // Practice blocks the underlying title's keyboard, not the whole touch
 // surface. Outside its actual ImGui windows, taps/swipes must still reach
 // TouchController so Z / Escape can confirm or cancel the practice menu.
 // Keep advanced settings modal and keep ownership of widget drags below.
 if(MobileThprac::captures_advanced(advanced_open))return true;
 if(auto* r=touhou::sdl::current();r&&r->flip_present_y)y=480-y;
 if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
 for(auto* window:ImGui::GetCurrentContext()->Windows)if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
 if(!move_hints)return false;if(selected_hint!=-1&&mouse_down)return true;
 auto* app=static_cast<Application*>(ImGui::GetIO().UserData);if(!app)return false;
 for(int i=0;i<10;++i)if(auto* vm=hint_vm(*app,i);vm&&std::abs(x-vm->position.x)<=8&&std::abs(y-vm->position.y)<=8)return true;
 return false;
}
double simulation_interval(Application& app){return app.state.practice.enabled?app.state.practice.speed.interval(app.state.practice.replay,key_down[VK_CONTROL],key_down[VK_SHIFT],key_down[VK_SPACE]):1./60.;}
void render(Application& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 ImGui::GetIO().UserData=&runtime;
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};if(!pointer_edges.empty()){const auto edge=pointer_edges.front();pointer_edges.pop_front();pointer_sample_down=edge.down;io.MousePos={edge.x,edge.y};}io.MouseDown[0]=pointer_sample_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Desktop thprac numeric fields should be directly editable: ImGui's drag
 // widgets can now switch to TempInputText on a click-release without a drag.
 // Queue numeric characters for the whole practice-menu frame; ImGui clears
 // unused characters at EndFrame, while an active TempInputText consumes them.
 if(runtime.state.practice.menu||advanced_open){for(int vk=48;vk<=57;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-48));for(int vk=96;vk<=105;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-96));if(pressed(189)||pressed(109))io.AddInputCharacter('-');if(pressed(190)||pressed(110))io.AddInputCharacter('.');}
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 if(runtime.state.practice.menu)draw_practice(runtime);else if(!(key_down[VK_X]||key_down[VK_Z]||key_down[VK_ESCAPE]||key_down[VK_RETURN]))practice_was_open=false;draw_overlay(runtime);ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));frame_drawn=true;
}
}
