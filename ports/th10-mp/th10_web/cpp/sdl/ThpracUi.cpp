#include "ThpracUi.hpp"
#include "../platform/Application.hpp"
#include "../platform/GameState.hpp"
#include "../platform/World.hpp"
#include "../platform/Title.hpp"
#include "../game/InputDevices.hpp"
#include "../game/GameEconomy.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeSections.hpp"
#include "Renderer.hpp"
#include "imgui.h"
#include "imgui_freetype.h"
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th10::browser::ThpracUi {
namespace {
bool initialized=false,frame_open=false,menu_open=false,tracker_open=false,advanced_open=false,practice_was_open=false,practice_keys_armed=false,text_editing=false,desktop_pointer=false;
bool key_down[256]{},key_pressed[256]{};float mouse_x=-FLT_MAX,mouse_y=-FLT_MAX;bool mouse_down=false;
int locale=0,practice_section_index=0;
// ImGui runs one frame per fixed 60 Hz tick (update_input). High-refresh
// presentation passes must re-render the cached draw data only: starting a
// new ImGui frame per present consumed edge-triggered input (typed digits)
// several times per press and ran ImGui's clock several times fast.
unsigned input_generation=0,rendered_generation=~0u;bool frame_drawn=false;

enum Vk {VK_BACK=8,VK_TAB=9,VK_RETURN=13,VK_SHIFT=16,VK_CONTROL=17,VK_MENU=18,VK_ESCAPE=27,VK_SPACE=32,VK_PRIOR=33,VK_NEXT=34,VK_END=35,VK_HOME=36,VK_LEFT=37,VK_UP=38,VK_RIGHT=39,VK_DOWN=40,VK_INSERT=45,VK_DELETE=46,VK_1=49,VK_2=50,VK_3=51,VK_X=88,VK_Z=90,VK_F1=112,VK_F7=118,VK_F12=123};
const char* tr(const char* zh,const char* en,const char* ja){return locale==0?zh:locale==2?ja:en;}
bool pressed(int vk){return vk>=0&&vk<256&&key_pressed[vk];}
u32 bridge_keys(){return u32(EM_ASM_INT({return (Module.eaglerControls?.thpracKeyboardBits||0)|0;}));}
bool bridge_key_down(int vk,u32 bits){
 if(vk==VK_BACK)return bits&1u;
 if(vk>=VK_F1&&vk<=VK_F7)return bits&(1u<<(vk-VK_F1+1));
 if(vk==VK_TAB)return bits&(1u<<8);
 if(vk==VK_F12)return bits&(1u<<9);
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
   if(p.section==TH10_ST6_BOSS9)ImGui::SliderInt(tr("延迟","Delay","ディレイ"),&p.st6_boss9_spd,0,160);
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
 const bool widget_busy=text_editing;text_editing=ImGui::IsAnyItemActive();
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
 if(menu_open){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Mod Menu###th10-thprac-overlay",nullptr,flags)){static const char* keys[]{"F1","F2","F3","F4","F5","F6"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム"),tr("不掉信仰","No faith loss","信仰点減少させない")};
   for(int i=0;i<6;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F7",tr("永续BGM","Everlasting BGM","永遠に続くBGM"),value);state.everlasting_bgm=value;
  }ImGui::End();
 }
 if(tracker_open&&in_game(runtime)){
  static const char* shots[3][6]={{"灵梦A","灵梦B","灵梦C","魔理沙A","魔理沙B","魔理沙C"},{"ReimuA","ReimuB","ReimuC","MarisaA","MarisaB","MarisaC"},{"霊夢A","霊夢B","霊夢C","魔理沙A","魔理沙B","魔理沙C"}};
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,150},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Tracker###th10-thprac-tracker",nullptr,flags)){const int shot=std::clamp(runtime.state.game.character*3+runtime.state.game.shot_type,0,5);const char* title=shots[locale][shot];const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);if(ImGui::BeginTable("Tracker table",2)){auto row=[](const char* label,const char* format,int a){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(label);ImGui::TableNextColumn();ImGui::Text(format,a);};row("Miss","%d",int(state.tracker_misses));row("Bomb","%d",int(state.tracker_bombs));ImGui::EndTable();}}
  ImGui::End();
 }
 if(advanced_open){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(ImGui::Begin("Advanced Options###th10-thprac-advanced",nullptr,flags)){ImGui::TextUnformatted(tr("高级选项","Advanced Options","拡張オプション"));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});if(ImGui::CollapsingHeader(tr("游戏速度","Game Speed","ゲームの速度"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::BeginDisabled();int fps=60;ImGui::SliderInt("FPS",&fps,60,6000);ImGui::EndDisabled();}if(ImGui::CollapsingHeader(tr("游戏进行","Gameplay","ゲームプレイ"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::Checkbox(tr("总是计入All Clear Bonus","Always factor in the \"All Clear Bonus\"","プラクティスでもオールクリアボーナスを加算する"),&state.all_clear_bonus);}if(ImGui::CollapsingHeader(tr("关于","About","バージョン情報"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::TextUnformatted("thprac v2.3.0.3");ImGui::TextUnformatted("github.com/touhouworldcup/thprac");ImGui::TextUnformatted("Thanks: You!");}ImGui::EndChild();ImGui::SetWindowFocus();}ImGui::End();ImGui::PopStyleVar(2);
 }
}
}

bool initialize(){
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;}))return true;
 if(initialized)return true;IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;
 io.KeyMap[ImGuiKey_Tab]=VK_TAB;io.KeyMap[ImGuiKey_LeftArrow]=VK_LEFT;io.KeyMap[ImGuiKey_RightArrow]=VK_RIGHT;io.KeyMap[ImGuiKey_UpArrow]=VK_UP;io.KeyMap[ImGuiKey_DownArrow]=VK_DOWN;io.KeyMap[ImGuiKey_PageUp]=VK_PRIOR;io.KeyMap[ImGuiKey_PageDown]=VK_NEXT;io.KeyMap[ImGuiKey_Home]=VK_HOME;io.KeyMap[ImGuiKey_End]=VK_END;io.KeyMap[ImGuiKey_Insert]=VK_INSERT;io.KeyMap[ImGuiKey_Delete]=VK_DELETE;io.KeyMap[ImGuiKey_Backspace]=VK_BACK;io.KeyMap[ImGuiKey_Space]=VK_SPACE;io.KeyMap[ImGuiKey_Enter]=VK_RETURN;io.KeyMap[ImGuiKey_Escape]=VK_ESCAPE;io.KeyMap[ImGuiKey_KeyPadEnter]=VK_RETURN;io.KeyMap[ImGuiKey_A]='A';io.KeyMap[ImGuiKey_C]='C';io.KeyMap[ImGuiKey_V]='V';io.KeyMap[ImGuiKey_X]='X';io.KeyMap[ImGuiKey_Y]='Y';io.KeyMap[ImGuiKey_Z]='Z';
 ImGui::StyleColorsDark();locale=EM_ASM_INT({const v=String(Module.eaglerOptions?.thpracLocale||'');return v.startsWith('ja')?2:v.startsWith('en')?1:0;});ImFontConfig config{};config.FontNo=0;config.RasterizerMultiply=1.25f;config.OversampleH=5;config.OversampleV=5;const ImWchar* range=locale==0?io.Fonts->GetGlyphRangesChineseFull():locale==2?io.Fonts->GetGlyphRangesJapanese():io.Fonts->GetGlyphRangesDefault();
 // Keep the game's own font for the TH10 game renderer, but always render
 // thprac with Unifont. Some spell/option labels contain CJK glyphs missing
 // from the bundled font even when the UI locale itself is Japanese or English.
 // The launcher mounts /unifont.otf whenever thprac is enabled.
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,range);
 if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}initialized=true;return true;
}
void shutdown(){if(!initialized)return;if(frame_open){ImGui::EndFrame();frame_open=false;}publish_menu(false);ImGui::DestroyContext();initialized=false;}
void process_event(const SDL_Event& event){if(!initialized)return;if(event.type==SDL_EVENT_MOUSE_MOTION){if(event.motion.which!=SDL_TOUCH_MOUSEID&&event.motion.which!=SDL_PEN_MOUSEID)desktop_pointer=true;mouse_x=event.motion.x;mouse_y=event.motion.y;}else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP){if(event.button.which!=SDL_TOUCH_MOUSEID&&event.button.which!=SDL_PEN_MOUSEID)desktop_pointer=true;mouse_x=event.button.x;mouse_y=event.button.y;if(event.button.button==SDL_BUTTON_LEFT)mouse_down=event.type==SDL_EVENT_MOUSE_BUTTON_DOWN;}else if(event.type==SDL_EVENT_MOUSE_WHEEL){ImGui::GetIO().MouseWheel+=event.wheel.y;ImGui::GetIO().MouseWheelH+=event.wheel.x;}}
void mouse(int type,float x,float y){mouse_x=x;mouse_y=y;if(type==1)mouse_down=true;else if(type==2)mouse_down=false;}
void update_input(Application& runtime){if(!initialized)return;++input_generation;auto* keys=runtime.input.keyboard_state();const u32 bits=bridge_keys();for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}auto& state=runtime.state.practice;if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;if(pressed(VK_TAB)&&!ImGui::IsAnyItemActive()&&in_game(runtime))tracker_open=!tracker_open;if(pressed(VK_F12))advanced_open=!advanced_open;if(menu_open&&in_game(runtime)&&!state.replay){for(int i=0;i<6;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F7))state.everlasting_bgm=!state.everlasting_bgm;}if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);}
bool captures_game_input(){return advanced_open||practice_was_open;}
void render(Application& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};io.MouseDown[0]=mouse_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Desktop thprac numeric fields should be directly editable: ImGui's drag
 // widgets can now switch to TempInputText on a click-release without a drag.
 // Queue numeric characters for the whole practice-menu frame; ImGui clears
 // unused characters at EndFrame, while an active TempInputText consumes them.
 if(runtime.state.practice.menu){for(int vk=48;vk<=57;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-48));for(int vk=96;vk<=105;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-96));if(pressed(189)||pressed(109))io.AddInputCharacter('-');if(pressed(190)||pressed(110))io.AddInputCharacter('.');}
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 if(runtime.state.practice.menu)draw_practice(runtime);else if(!(key_down[VK_X]||key_down[VK_Z]||key_down[VK_ESCAPE]||key_down[VK_RETURN]))practice_was_open=false;draw_overlay(runtime);ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));frame_drawn=true;
}
}
