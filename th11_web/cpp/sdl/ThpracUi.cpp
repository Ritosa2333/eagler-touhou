#include "ThpracUi.hpp"
#include "../game/GameSession.hpp"
#include "../game/GameEconomy.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeSections.hpp"
#include "../game/PracticeUiLabels.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th11::browser::ThpracUi {
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
bool in_game(GameSession& runtime){return bool(runtime.battle);}
// The overlay draws onto the same GPU backbuffer the game presents.
u32 backbuffer(GameSession&){return 1;}
void toggle_cheat(GameSession& runtime,int bit){
 auto& p=runtime.practice;if(p.replay)return;p.cheats^=1u<<bit;if(p.cheats)p.assisted=true;
}
void hotkey_line(const char* key,const char* label,bool& value){
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
 if(p.stage==3)appearance=8+(s.spell_category?s.spell_category-1:runtime.title->selection.character*3+runtime.title->selection.partner);
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
  const char* stages[]{"1","2","3","4","5","6","Extra"};if(ImGui::Combo(tr("关卡","Stage","ステージ"),&p.stage,stages,7))p.section=p.phase=0;
  if(p.mode){
   const char* warps[]{tr("无","None","なし"),tr("道中","Stage Portion","道中"),tr("道中Boss","Mid Boss","道中ボス"),tr("关底Boss","End Boss","ボス"),tr("非符","Non Spell","通常"),tr("符卡","Spell Card","スペカ")};
   if(ImGui::Combo(tr("传送","Warp","ワープ"),&s.warp,warps,6))p.section=p.phase=0;
   if(s.warp&&p.stage==3){const char* shots[]{tr("当前机体","Selected Shot Type","選択中の機体"),"Reimu A","Reimu B","Reimu C","Marisa A","Marisa B","Marisa C"};if(ImGui::Combo(tr("符卡分类","Spell Category","スペカ分類"),&s.spell_category,shots,7))p.section=p.phase=0;}
   select_current_section(runtime);
   if(s.warp==1){constexpr int setup[7][2]{{2,2},{2,2},{2,1},{3,4},{3,3},{3,3},{4,3}};const auto& c=setup[p.stage];int chapter=p.section%100;char label[64];std::snprintf(label,sizeof(label),chapter<=c[0]?tr("前半 #%d","First Half #%d","前半 #%d"):tr("后半 #%d","Second Half #%d","後半 #%d"),chapter<=c[0]?chapter:chapter-c[0]);if(ImGui::SliderInt(tr("章节","Chapter","チャプター"),&chapter,1,c[0]+c[1],label))p.section=10000+(p.stage+1)*100+chapter;}
   else if(s.warp>=2){auto labels=matching_sections(runtime);std::vector<const char*> names;for(auto* l:labels)names.push_back(l->names[p.stage==6?4:runtime.title->selection.difficulty][locale]);if(!names.empty()&&ImGui::Combo(warps[s.warp],&practice_section_index,names.data(),int(names.size()))){p.section=labels[practice_section_index]->id;p.phase=0;}if(section_has_dialogue(p.section))ImGui::Checkbox(tr("对话","Dialog","会話"),&p.dlg);}
   const int phase_count=p.section==TH11_ST6_BOSS9?5:p.section==TH11_ST7_END_S9?3:p.section==TH11_ST7_END_S10?4:0;
   if(phase_count){const auto* labels=p.section==TH11_ST6_BOSS9?practice_phase_five:p.section==TH11_ST7_END_S9?practice_phase_timeout:practice_phase_four;const char* names[5]{};for(int i=0;i<phase_count;++i)names[i]=labels[i][locale];ImGui::Combo(tr("阶段","Phase","段階"),&p.phase,names,phase_count);}else p.phase=0;
   ImGui::SliderInt(tr("残机","Life","残機"),&p.life,0,9);ImGui::SliderInt(tr("残机碎片","Life Pieces","残機の欠片"),&p.life_fragment,0,4);
   const bool ma=runtime.title->selection.character==1&&runtime.title->selection.partner==0;char power[32];const int fixed=ma?p.power*100/12:p.power*5;std::snprintf(power,sizeof(power),"%d.%02d",fixed/100,fixed%100);p.power=std::clamp(p.power,0,ma?96:80);ImGui::SliderInt(tr("火力","Power","霊力"),&p.power,0,ma?96:80,power);
   ImGui::DragInt("Graze",&p.graze,1,0,999999);ImGui::DragInt(tr("最大得点","Point Value","最大得点"),&p.value,10,0,999990);p.value=p.value/10*10;
   const i64 lo=0,hi=9999999990LL;ImGui::DragScalar(tr("分数","Score","スコア"),ImGuiDataType_S64,&p.score,10,&lo,&hi,"%lld");p.score=p.score/10*10;
   if(runtime.title->selection.character==1&&runtime.title->selection.partner==1){const char* f[]{tr("火","Fire","火"),tr("水","Water","水"),tr("木","Wood","木"),tr("金","Metal","金"),tr("土","Earth","土")};ImGui::Combo(tr("配置","Formation","オプション"),&p.marisa_b_formation,f,5);}
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
 if(menu_open){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Mod Menu###th11-thprac-overlay",nullptr,flags)){static const char* keys[]{"F1","F2","F3","F4","F5"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム")};
   for(int i=0;i<5;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F6",tr("永续BGM","Everlasting BGM","永遠に続くBGM"),value);state.everlasting_bgm=value;
  }ImGui::End();
 }
 if(tracker_open&&in_game(runtime)){
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,175},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Tracker###th11-thprac-tracker",nullptr,flags)){const int shot=std::clamp(runtime.state.character*3+runtime.state.subtype,0,5);const char* title=practice_tracker_shots[shot][locale];const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);if(ImGui::BeginTable("Tracker table",2)){auto row=[](const char* label,const char* format,int a){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(label);ImGui::TableNextColumn();ImGui::Text(format,a);};row("Miss","%d",int(state.tracker_misses));row("Bomb","%d",int(state.tracker_bombs));ImGui::EndTable();}}
  ImGui::End();
 }
 if(advanced_open){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(ImGui::Begin("Advanced Options###th11-thprac-advanced",nullptr,flags)){ImGui::TextUnformatted(tr("高级选项","Advanced Options","拡張オプション"));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});if(ImGui::CollapsingHeader(tr("游戏速度","Game Speed","ゲームの速度"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::BeginDisabled();int fps=60;ImGui::SliderInt("FPS",&fps,60,6000);ImGui::EndDisabled();}if(ImGui::CollapsingHeader(tr("游戏进行","Gameplay","ゲームプレイ"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::Checkbox(tr("总是计入All Clear Bonus","Always factor in the \"All Clear Bonus\"","プラクティスでもオールクリアボーナスを加算する"),&state.all_clear_bonus);}if(ImGui::CollapsingHeader(tr("关于","About","バージョン情報"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::TextUnformatted("thprac v2.3.0.3");ImGui::TextUnformatted("github.com/touhouworldcup/thprac");ImGui::TextUnformatted("Thanks: You!");}ImGui::EndChild();ImGui::SetWindowFocus();}ImGui::End();ImGui::PopStyleVar(2);
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
void cancel_pointer(){mouse_down=false;mouse_x=mouse_y=-FLT_MAX;}
bool captures_pointer(float x,float y){
 if(!initialized)return false;
 // ImGui owns taps on its windows/popups. Do not synthesize a second Z
 // activation from the game's menu gesture when selecting a combo item.
 if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
 for(auto* window:ImGui::GetCurrentContext()->Windows)if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
 return false;
}
void update_input(GameSession& runtime,const bool* keys){if(!initialized)return;++input_generation;const u32 bits=bridge_keys();for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}auto& state=runtime.practice;if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;if(pressed(VK_TAB)&&!ImGui::IsAnyItemActive()&&in_game(runtime))tracker_open=!tracker_open;if(pressed(VK_F12))advanced_open=!advanced_open;if(menu_open&&in_game(runtime)&&!state.replay){for(int i=0;i<5;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F1+5))state.everlasting_bgm=!state.everlasting_bgm;}if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);}
bool captures_game_input(){return advanced_open||practice_was_open;}
void render(GameSession& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};io.MouseDown[0]=mouse_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Desktop thprac numeric fields should be directly editable: ImGui's drag
 // widgets can now switch to TempInputText on a click-release without a drag.
 // Queue numeric characters for the whole practice-menu frame; ImGui clears
 // unused characters at EndFrame, while an active TempInputText consumes them.
 if(runtime.practice.menu){for(int vk=48;vk<=57;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-48));for(int vk=96;vk<=105;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-96));if(pressed(189)||pressed(109))io.AddInputCharacter('-');if(pressed(190)||pressed(110))io.AddInputCharacter('.');}
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 // Persistent touch fire must not keep the closed Practice menu capturing
 // gameplay inputs (including the launcher's Escape serial pulse).
 if(runtime.practice.menu)draw_practice(runtime);else practice_was_open=false;draw_overlay(runtime);ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),backbuffer(runtime));frame_drawn=true;
}
}
