// Purple THPrac Web adapter: native ImGui widgets and common tool authorities.
// Platform/input/draw transport follows ports/th10 and ports/th11 ThpracUi.cpp.
#include "../game/Practice.hpp"
#include "../game/PracticeTools.hpp"
#include "../game/GameState.hpp"
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
#include "../practice/UiCatalog.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#ifdef __EMSCRIPTEN__
#include "imgui_freetype.h"
#include <emscripten.h>
#endif
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <eagler/thprac/PracticePointer.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <random>
#include <string>
#include <vector>
namespace th12::practice::ui {
namespace Mobile=eagler::thprac::mobile;
bool initialized=false,menu_open=false,advanced_open=false,tracker_open=false;
bool practice_open=false,armed=false,menu_ready=false,in_game=false,replaying=false,desktop_pointer=false;
bool text_editing=false;
bool keys[256]{},pressed[256]{};int locale=0,difficulty=1,action=0,warp=0,section_index=0,previous_pad=0;
float mouse_x=-FLT_MAX,mouse_y=-FLT_MAX;bool mouse_down=false,sample_down=false;
int64_t pointer_owner=-1;bool speed_reset=true;Parameters params;
eagler::thprac::PracticePointer pointers;
struct Edge{bool down;float x,y;};std::deque<Edge> edges;
struct DrawCommand{uint32_t count,start,vertex;float clip[4];};
std::vector<ImDrawVert> vertices;std::vector<ImDrawIdx> indices;std::vector<DrawCommand> commands;
uint32_t draw_header[8]{},font_header[3]{};int32_t parameter_words[20]{};
const char* tr(const char* zh,const char* en,const char* ja){return locale==0?zh:locale==2?ja:en;}
const char* label(const char* const* value){return value[locale];}
template<size_t N>const char* const* selection(const char* const (&items)[3][N]){return items[locale];}
template<size_t N>bool combo(const char* name,int* value,const char* const (&items)[3][N]){return ImGui::Combo(name,value,selection(items),int(N));}
void help_marker(const char* const* description){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(description));}
#define key_monitor th12::practice::toolSettings().key_monitor
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter{int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* counter){counter->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* counter){counter->QuadPart=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::random_device{}()));}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include <eagler/thprac/PracticeSpeed.inc>

void cancel_pointer(){pointers.reset();pointer_owner=-1;edges.clear();mouse_down=sample_down=false;mouse_x=mouse_y=-FLT_MAX;if(initialized){ImGui::GetIO().MouseDown[0]=false;ImGui::ClearActiveID();}}
bool hit(float x,float y){
  if(!initialized||!enabled())return false;
  if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
  for(auto* window:ImGui::GetCurrentContext()->Windows)
    if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
  return false;
}
void hotkey_line(const char* key,const char* name,bool& value){
  if(Mobile::enabled){Mobile::checkbox(key,name,value);return;}
  const auto cursor=ImGui::GetCursorPos();
  if(value)ImGui::TextColored({0,1,0,1},"[%s: %s]",key,name);else ImGui::Text("%s: %s",key,name);
  ImGui::SetCursorPos(cursor);
  if(ImGui::InvisibleButton(key,{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()}))value=!value;
}
void toggle(int bit){if(!replaying&&in_game)setTools(tools()^uint32_t(bit));}
void input_options(){
  auto& input=toolSettings().input;
  auto check=[](const char* const* name,bool& value,const char* const* description=nullptr){ImGui::Checkbox(label(name),&value);if(description)help_marker(description);};
  check(practice_TH_ADV_DISABLE_X_KEY,input.disable_xkey,practice_TH_ADV_DISABLE_X_KEY_DESC);ImGui::SameLine();
  check(practice_TH_ADV_DISABLE_SHIFT_KEY,input.disable_shiftkey,practice_TH_ADV_DISABLE_SHIFT_KEY_DESC);ImGui::SameLine();
  check(practice_TH_ADV_DISABLE_Z_KEY,input.disable_zkey,practice_TH_ADV_DISABLE_Z_KEY_DESC);
  check(practice_TH_ADV_DISABLE_C_KEY_SAMETIME,input.disable_Ckey_at_same_time);ImGui::SameLine();
  check(practice_TH_ADV_FORCE_SHIFT_KEY,input.force_shiftkey);
  check(practice_THPRAC_FAST_RETRY,input.enable_fast_retry,practice_THPRAC_FAST_RETRY_DESC2);
}
void key_monitor_options(){
  ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&toolSettings().show_keyboard_monitor);
  if(!toolSettings().show_keyboard_monitor)return;
  if(!key_monitor.g_record_key_aps){if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){key_monitor.clear_record();key_monitor.g_record_key_aps=true;}}
  else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
  ImGui::SameLine();if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT))){
#ifdef __EMSCRIPTEN__
    const auto csv=key_monitor.csv();EM_ASM({const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},csv.c_str());
#endif
  }
  const auto& aps=key_monitor.g_recorded_aps;
  if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int index){const auto& values=*static_cast<const std::vector<int>*>(data);return float(values[(values.size()>600?values.size()-600:0)+index]);},const_cast<std::vector<int>*>(&aps),int(std::min<size_t>(600,aps.size())),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void draw_advanced(){
  auto& state=toolSettings();ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});
  if(ImGui::CollapsingHeader(label(practice_TH_GAME_SPEED)))GameFPSOpt(speedSettings(),true,&speed_reset);
  if(ImGui::CollapsingHeader(label(practice_TH_GAMEPLAY))){
    input_options();key_monitor_options();
    ImGui::Checkbox(label(practice_THPRAC_INFLIVES_MAP),&state.map_inf_life_to_no_continue);
    ImGui::BeginDisabled(replaying);
    ImGui::Checkbox(label(practice_TH_BOSS_FORCE_MOVE_DOWN),&state.force_boss_move_down);help_marker(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC);
    ImGui::SameLine();ImGui::SetNextItemWidth(180);ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&state.boss_move_down_range,.002f,0,1);
    state.boss_move_down_range=std::clamp(state.boss_move_down_range,0.f,1.f);
    ImGui::EndDisabled(replaying);
    ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_12_UFO_SUPER_ULTRA_CHROMATIC_INFO_PRO),&state.chromatic_ufo);
    ImGui::Checkbox(label(practice_TH_DISABLE_MASTER),&state.disable_master_display);help_marker(practice_TH_DISABLE_MASTER_DESC);
    ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&state.show_lock_timer);
    ImGui::BeginDisabled(replaying);bool bonus=tools()&AllClearBonus;
    if(ImGui::Checkbox(label(practice_TH_FACTOR_ACB),&bonus))setTools(bonus?tools()|AllClearBonus:tools()&~AllClearBonus);
    help_marker(practice_TH_FACTOR_ACB_DESC);ImGui::EndDisabled(replaying);
  }
  if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);else reaction_test.Reset();
  if(ImGui::CollapsingHeader(label(practice_TH_ABOUT_THPRAC))){
    ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_source_version);ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));ImGui::NewLine();ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");ImGui::NewLine();
    static bool show_license=false;if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;
    if(show_license){ImGui::BeginChild("COPYING",{0,384},true);ImGui::TextUnformatted(practice_license);ImGui::EndChild();}
  }
  ImGui::EndChild();
}
std::vector<const SectionLabel*> section_choices(){
  std::vector<const SectionLabel*> out;const int diff=params.stage==6?4:difficulty;
  for(const auto& section:section_labels){if(section.stage!=params.stage+1||!*section.names[diff][locale])continue;
    if(warp==2&&section.group!=1||warp==3&&section.group!=2||warp==4&&section.spell||warp==5&&!section.spell)continue;
    out.push_back(&section);}
  return out;
}
const SectionLabel* normalize_section(){
  params.stage=std::clamp(params.stage,0,6);
  if(!warp){params.section=params.phase=0;params.dlg=false;return nullptr;}
  if(warp==1){params.section=10000+(params.stage+1)*100+std::clamp(params.section>=10000?params.section%100:1,1,chapter_counts[params.stage][0]+chapter_counts[params.stage][1]);params.phase=0;params.dlg=false;return nullptr;}
  const auto choices=section_choices();auto found=std::find_if(choices.begin(),choices.end(),[](auto* value){return value->id==params.section;});
  section_index=found==choices.end()?0:int(found-choices.begin());const auto* selected=choices.empty()?nullptr:choices[section_index];params.section=selected?selected->id:0;
  params.phase=std::clamp(params.phase,0,(selected?selected->phases:1)-1);if(!selected||!selected->dialogue)params.dlg=false;return selected;
}
void draw_practice(){
  const bool english=locale==1;
  ImGui::SetNextWindowSize({english?425.f:350.f,410},ImGuiCond_Always);ImGui::SetNextWindowPos({english?185.f:260.f,50},ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
  constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(ImGui::Begin("Option###th12-thprac-practice",nullptr,flags)){
    ImGui::PushItemWidth(-65);ImGui::TextUnformatted(label(practice_TH_MENU));ImGui::Separator();ImGui::BeginDisabled(!menu_ready);
    combo(label(practice_TH_MODE),&params.mode,practice_group_TH_MODE_SELECT);
    if(combo(label(practice_TH_STAGE),&params.stage,practice_group_TH_STAGE_SELECT))params.section=params.phase=0;
    if(params.mode){
      const auto* warps=selection(practice_group_TH_WARP_SELECT);
      if(combo(label(practice_TH_WARP),&warp,practice_group_TH_WARP_SELECT))params.section=params.phase=0;
      const auto* section=normalize_section();
      if(warp==1){int chapter=params.section%100;char name[64];const int first=chapter_counts[params.stage][0];
        std::snprintf(name,sizeof name,params.stage==5?label(practice_TH_STAGE_PORTION_N):label(chapter<=first?practice_TH_STAGE_PORTION_1:practice_TH_STAGE_PORTION_2),chapter<=first||params.stage==5?chapter:chapter-first);
        if(ImGui::SliderInt(label(practice_TH_CHAPTER),&chapter,1,first+chapter_counts[params.stage][1],name))params.section=10000+(params.stage+1)*100+chapter;
      }else if(warp>=2){const auto choices=section_choices();std::vector<const char*> names;for(auto* value:choices)names.push_back(value->names[params.stage==6?4:difficulty][locale]);
        if(!names.empty()&&ImGui::Combo(warps[warp],&section_index,names.data(),int(names.size()))){params.section=choices[section_index]->id;params.phase=0;section=normalize_section();}
        if(section&&section->phases==2)combo(label(practice_TH_PHASE),&params.phase,practice_group_TH_SPELL_PHASE1);
        if(section&&section->phases==5)combo(label(practice_TH_PHASE),&params.phase,practice_group_TH_SPELL_PHASE3);
        if(section&&section->dialogue)ImGui::Checkbox(label(practice_TH_DLG),&params.dlg);
      }
      ImGui::SliderInt(label(practice_TH_LIFE),&params.life,0,9);ImGui::SliderInt(label(practice_TH_LIFE_FRAGMENT),&params.life_fragment,0,3);
      ImGui::SliderInt(label(practice_TH_BOMB),&params.bomb,0,9);ImGui::SliderInt(label(practice_TH_BOMB_FRAGMENT),&params.bomb_fragment,0,2);
      char power[24];std::snprintf(power,sizeof power,"%d.%02d",params.power/100,params.power%100);ImGui::SliderInt(label(practice_TH_POWER),&params.power,0,400,power);
      combo(label(practice_TH12_UFO_SIDE),&params.ufo_side,practice_group_TH_SIDE_SELECT);
      combo(label(practice_TH12_VENTRA_1),&params.ventra_1,practice_group_TH12_VENTRA_SELECT);
      if(params.ventra_1)combo(label(practice_TH12_VENTRA_2),&params.ventra_2,practice_group_TH12_VENTRA_SELECT);else params.ventra_2=params.ventra_3=0;
      if(params.ventra_2)combo(label(practice_TH12_VENTRA_3),&params.ventra_3,practice_group_TH12_VENTRA_SELECT);else params.ventra_3=0;
      ImGui::DragInt(label(practice_TH_VALUE),&params.value,10,0,999990);ImGui::DragInt(label(practice_TH_GRAZE),&params.graze,1,0,999999);
      const int64_t minimum=0,maximum=9999999990LL;ImGui::DragScalar(label(practice_TH_SCORE),ImGuiDataType_S64,&params.score,10,&minimum,&maximum,"%lld");
      params.life=std::clamp(params.life,0,9);params.life_fragment=std::clamp(params.life_fragment,0,3);params.bomb=std::clamp(params.bomb,0,9);params.bomb_fragment=std::clamp(params.bomb_fragment,0,2);params.power=std::clamp(params.power,0,400);
      params.value=std::clamp(params.value,0,999990)/10*10;params.graze=std::clamp(params.graze,0,999999);params.score=std::clamp(params.score,minimum,maximum)/10*10;
    }
    ImGui::EndDisabled(!menu_ready);ImGui::PopItemWidth();
    const bool busy=text_editing||ImGui::IsAnyItemActive()||ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel);
    text_editing=ImGui::IsAnyItemActive()||ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel);
    if(!busy)ImGui::SetWindowFocus();
    if(!armed&&!(keys[90]||keys[13]||keys[88]||keys[27]||pressed[90]||pressed[13]||pressed[88]||pressed[27]||(previous_pad&259)))armed=true;
    if(menu_ready&&armed&&!busy){if(pressed[90]||pressed[13])action=1;if(pressed[88]||pressed[27])action=2;}
  }ImGui::End();ImGui::PopStyleVar(2);
}
void draw_overlay(){
  if(Mobile::show(Mobile::Cheats,menu_open)){
    ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);
    constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
    if(Mobile::begin(Mobile::Cheats,"Mod Menu###th12-thprac-overlay",flags,locale)){
      const char* names[]{label(practice_TH_MUTEKI),label(practice_TH_INFLIVES),label(practice_TH_INFBOMBS),label(practice_TH_INFPOWER),label(practice_TH_TIMELOCK),label(practice_TH_AUTOBOMB),label(practice_TH_EL_BGM)};
      const char* hotkeys[]{"F1","F2","F3","F4","F5","F6","F7"};ImGui::BeginDisabled(replaying||!in_game);
      for(int i=0;i<7;i++){bool value=tools()&(1u<<i);hotkey_line(hotkeys[i],names[i],value);if(value!=bool(tools()&(1u<<i)))toggle(1<<i);}ImGui::EndDisabled(replaying||!in_game);
      hotkey_line("Tab",label(practice_THPRAC_INGAMEINFO),tracker_open);ImGui::BeginDisabled(replaying||!in_game);
      bool enemy=tools()&EnemyInvulnerable;hotkey_line("U",label(practice_TH_ENEMY_MUTEKI),enemy);if(enemy!=bool(tools()&EnemyInvulnerable))toggle(EnemyInvulnerable);ImGui::EndDisabled(replaying||!in_game);
    }ImGui::End();
  }
  if(Mobile::show(Mobile::Information,tracker_open)&&in_game){
    ImGui::SetNextWindowPos({425,338},ImGuiCond_Always);ImGui::SetNextWindowSize({210,0},ImGuiCond_Always);
    constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
    if(Mobile::begin(Mobile::Information,"Tracker###th12-thprac-tracker",flags,locale)){
      const auto& tracker=trackerInfo();const auto* diffs=selection(practice_group_IGI_DIFF);const auto* players=selection(practice_group_IGI_PL_12);
      char title[128];std::snprintf(title,sizeof title,"%s (%s)",diffs[std::clamp(th12::difficulty,0,4)],players[std::clamp(th12::character*2+th12::shot,0,5)]);
      ImGui::SetCursorPosX((ImGui::GetWindowWidth()-ImGui::CalcTextSize(title).x)*.5f);ImGui::TextUnformatted(title);ImGui::Columns(2);
      auto counter=[](const char* const* name,int value){ImGui::TextUnformatted(label(name));ImGui::NextColumn();ImGui::Text("%8d",value);ImGui::NextColumn();};
      counter(practice_THPRAC_INGAMEINFO_MISS_COUNT,tracker.misses);counter(practice_THPRAC_INGAMEINFO_BOMB_COUNT,tracker.bombs);
      counter(practice_THPRAC_INGAMEINFO_12_UFO_COUNT3,tracker.dropped[0]+tracker.dropped[1]+tracker.dropped[2]+tracker.dropped[3]);
      counter(practice_THPRAC_INGAMEINFO_12_UFO_COUNT2,tracker.collected[0]+tracker.collected[1]+tracker.collected[2]);
      ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_12_UFO_COUNT1));ImGui::NextColumn();
      for(int i=0;i<4;i++){if(i){ImGui::SameLine(0,0);ImGui::TextUnformatted("/");ImGui::SameLine(0,0);}ImVec4 color=i==0?ImVec4{1,.5f,.5f,1}:i==1?ImVec4{.5f,1,.5f,1}:i==2?ImVec4{.5f,.75f,1,1}:ImVec4{1,1,1,1};
        if(i==3&&toolSettings().chromatic_ufo)ImGui::ColorConvertHSVtoRGB(float(ImGui::GetTime()*.96),.5f,1,color.x,color.y,color.z);ImGui::TextColored(color,"%d",tracker.summoned[i]);}
      ImGui::Columns(1);
    }ImGui::End();
  }
  if(Mobile::show(Mobile::Advanced,advanced_open)){
    ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
    constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
    if(Mobile::begin(Mobile::Advanced,"Advanced Options###th12-thprac-advanced",flags,locale)){draw_advanced();ImGui::SetWindowFocus();}ImGui::End();ImGui::PopStyleVar(2);
  }
  if(toolSettings().show_keyboard_monitor&&in_game&&Mobile::draw_tool(Mobile::Keyboard,"Keys###th12-mobile-keys",{420,65},locale)){
    KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);KeysHUD(12,{1280,0},{840,0},style,true,false);
  }
  if(toolSettings().show_lock_timer&&tool(TimeLock)&&in_game&&Mobile::draw_tool(Mobile::Indicators,"Tools###th12-mobile-tools",{120,65},locale)){
    char text[32];std::snprintf(text,sizeof text,"%.2f",toolSettings().lock_frames/60.f);auto* draw=ImGui::GetOverlayDrawList();draw->AddRectFilled({32,0},{110,ImGui::GetFontSize()},0xffffffff);draw->AddText({110-ImGui::CalcTextSize(text).x,0},0xff000000,text);
  }
}
void pack_draw(){
  vertices.clear();indices.clear();commands.clear();const auto* draw=ImGui::GetDrawData();
  for(int list=0;list<draw->CmdListsCount;list++){
    const auto* data=draw->CmdLists[list];const uint32_t vertex_base=uint32_t(vertices.size()),index_base=uint32_t(indices.size());
    vertices.insert(vertices.end(),data->VtxBuffer.begin(),data->VtxBuffer.end());indices.insert(indices.end(),data->IdxBuffer.begin(),data->IdxBuffer.end());
    for(const auto& command:data->CmdBuffer){if(command.UserCallback)continue;commands.push_back({command.ElemCount,index_base+command.IdxOffset,vertex_base+command.VtxOffset,{command.ClipRect.x,command.ClipRect.y,command.ClipRect.z,command.ClipRect.w}});}
  }
  draw_header[0]=uint32_t(uintptr_t(vertices.data()));draw_header[1]=uint32_t(uintptr_t(indices.data()));draw_header[2]=uint32_t(uintptr_t(commands.data()));
  draw_header[3]=uint32_t(vertices.size());draw_header[4]=uint32_t(indices.size());draw_header[5]=uint32_t(commands.size());draw_header[6]=sizeof(ImDrawVert);draw_header[7]=sizeof(ImDrawIdx);
}
} // namespace th12::practice::ui
using namespace th12::practice;
using namespace th12::practice::ui;
extern "C" {
int th12_practice_ui_init(const unsigned char* font,int length,int language){
  if(initialized)return 1;
  IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;locale=std::clamp(language,0,2);
  const int keymap[]{9,37,39,38,40,33,34,36,35,45,46,8,32,13,27,13,65,67,86,88,89,90};
  for(int i=0;i<ImGuiKey_COUNT;i++)io.KeyMap[i]=keymap[i];ImGui::StyleColorsDark();
  if(font&&length>0){ImFontConfig config;config.FontDataOwnedByAtlas=false;config.RasterizerMultiply=1.25f;config.OversampleH=config.OversampleV=5;
    ImFontGlyphRangesBuilder glyphs;glyphs.AddRanges(io.Fonts->GetGlyphRangesChineseFull());glyphs.AddText("↑←↓→ΔΣ");static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);
    io.FontDefault=io.Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(font),length,16,&config,ranges.Data);
  }else io.Fonts->AddFontDefault();
#ifdef __EMSCRIPTEN__
  if(!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return 0;}
#else
  io.Fonts->Build();
#endif
  unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);io.Fonts->TexID=reinterpret_cast<ImTextureID>(1);
  font_header[0]=uint32_t(uintptr_t(pixels));font_header[1]=width;font_header[2]=height;initialized=true;return 1;
}
const uint32_t* th12_practice_ui_font(){return font_header;}
void th12_practice_ui_key(int code,int down){if(code<0||code>=256)return;if(down&&!keys[code])pressed[code]=true;keys[code]=down!=0;}
void th12_practice_ui_mobile(int mode,int visible){
#ifdef __EMSCRIPTEN__
  EM_ASM({Module.eaglerControls??={};Module.eaglerControls.thpracMobilePanelsEnabled=!!$0;Module.eaglerControls.thpracMobilePanelsVisible=!!$1;},mode,visible);
#endif
  th12::practice::ui::Mobile::update(mode!=0,visible!=0);
}
void th12_practice_ui_context(int scene,int ready,int replay,int diff,int language){
  const bool entering=scene==1&&!practice_open;practice_open=scene==1;in_game=scene==2;menu_ready=ready!=0;replaying=replay!=0;difficulty=std::clamp(diff,0,4);locale=std::clamp(language,0,2);
  if(entering){armed=false;action=0;cancel_pointer();}
}
void th12_practice_ui_params(const int32_t* words,int count){
  if(!words||count!=20)return;params.mode=words[1];params.stage=words[2];params.section=words[3];params.phase=words[4];params.dlg=words[5]!=0;
  params.score=int64_t(uint32_t(words[6]))+(int64_t(words[7])<<32);params.life=words[8];params.life_fragment=words[9];params.bomb=words[10];params.bomb_fragment=words[11];params.power=words[12];params.value=words[13];params.graze=words[14];params.ufo_side=words[15];params.ventra_1=words[16];params.ventra_2=words[17];params.ventra_3=words[18];
  warp=params.section>=10000?1:0;for(const auto& section:section_labels)if(section.id==params.section)warp=section.group==1?2:3;
}
const int32_t* th12_practice_ui_parameters(){
  const auto& p=params;const int32_t values[]{1,p.mode,p.stage,p.section,p.phase,int(p.dlg),int32_t(uint64_t(p.score)),int32_t(uint64_t(p.score)>>32),p.life,p.life_fragment,p.bomb,p.bomb_fragment,p.power,p.value,p.graze,p.ufo_side,p.ventra_1,p.ventra_2,p.ventra_3,0};std::copy(std::begin(values),std::end(values),parameter_words);return parameter_words;
}
int th12_practice_ui_capture(){return initialized&&enabled()&&(practice_open||th12::practice::ui::Mobile::captures_advanced(advanced_open));}
int th12_practice_ui_pointer(int type,int id,float x,float y,int desktop){
  if(!initialized||!enabled())return 0;desktop_pointer|=desktop!=0;
  if(type==3){const bool owned=pointers.route(2,id,false);if(pointer_owner==id){pointer_owner=-1;edges.clear();mouse_down=sample_down=false;ImGui::GetIO().MouseDown[0]=false;ImGui::ClearActiveID();}return owned;}
  const bool owned=pointers.route(unsigned(type),id,type==0&&hit(x,y));
  if(desktop&&type==1&&pointer_owner<0){mouse_x=x;mouse_y=y;}
  if(!owned)return 0;
  if(type==0&&pointer_owner<0)pointer_owner=id;
  if(pointer_owner==id){mouse_x=x;mouse_y=y;if(type!=1){const bool down=type==0;if(down!=mouse_down){if(edges.size()>=64){cancel_pointer();return 1;}edges.push_back({down,x,y});mouse_down=down;}if(type==2)pointer_owner=-1;}}
  return 1;
}
void th12_practice_ui_cancel(){if(initialized)cancel_pointer();std::fill(std::begin(keys),std::end(keys),false);std::fill(std::begin(pressed),std::end(pressed),false);resetToolInput();}
void th12_practice_ui_clear_keys(){std::fill(std::begin(keys),std::end(keys),false);std::fill(std::begin(pressed),std::end(pressed),false);previous_pad=0;resetToolInput();}
int th12_practice_ui_wheel(float x,float y,float horizontal,float vertical){if(!hit(x,y))return 0;auto& io=ImGui::GetIO();io.MousePos={x,y};mouse_x=x;mouse_y=y;io.MouseWheelH+=horizontal;io.MouseWheel+=vertical;return 1;}
int th12_practice_ui_behavior(){return enabled()?(int(toolSettings().disable_master_display)|(int(toolSettings().input.enable_fast_retry)<<1)):0;}
void th12_practice_ui_tick(int pad){
  if(!initialized)return;
  if(!enabled()){menu_open=advanced_open=tracker_open=false;commands.clear();vertices.clear();indices.clear();std::fill(std::begin(pressed),std::end(pressed),false);return;}
  auto& io=ImGui::GetIO();if(pressed[8]&&!ImGui::IsAnyItemActive())menu_open=!menu_open;if(pressed[9]&&!ImGui::IsAnyItemActive()&&in_game)tracker_open=!tracker_open;
  if(pressed[123])advanced_open=!advanced_open;
  if(menu_open&&in_game&&!replaying){for(int i=0;i<7;i++)if(pressed[112+i])toggle(1<<i);if(pressed[85])toggle(EnemyInvulnerable);}
  if(in_game&&!replaying&&!th12_practice_ui_capture()&&toolSettings().input.enable_fast_retry&&pressed[82])action=3;
  if(pressed[27]&&advanced_open)advanced_open=false;
  io.DeltaTime=1.f/60;io.MousePos={mouse_x,mouse_y};if(!edges.empty()){const auto edge=edges.front();edges.pop_front();sample_down=edge.down;io.MousePos={edge.x,edge.y};}io.MouseDown[0]=sample_down;
  const auto down=[](int code){return keys[code]||pressed[code];};
  io.KeyCtrl=down(17)||down(162)||down(163);io.KeyShift=down(16)||down(160)||down(161);io.KeyAlt=down(18);io.ConfigDragClickToInputText=desktop_pointer;
  for(int i=0;i<256;i++)io.KeysDown[i]=keys[i]||pressed[i];
  for(int code=48;code<=57;code++)if(pressed[code])io.AddInputCharacter(ImWchar(code));
  for(int code=96;code<=105;code++)if(pressed[code])io.AddInputCharacter(ImWchar('0'+code-96));
  if(pressed[189]||pressed[109])io.AddInputCharacter('-');if(pressed[190]||pressed[110])io.AddInputCharacter('.');
  io.NavInputs[ImGuiNavInput_DpadUp]=keys[38]||pressed[38]||bool(pad&16);io.NavInputs[ImGuiNavInput_DpadDown]=keys[40]||pressed[40]||bool(pad&32);io.NavInputs[ImGuiNavInput_DpadLeft]=keys[37]||pressed[37]||bool(pad&64);io.NavInputs[ImGuiNavInput_DpadRight]=keys[39]||pressed[39]||bool(pad&128);
  io.NavInputs[ImGuiNavInput_Activate]=keys[90]||keys[13]||pressed[90]||pressed[13]||bool(pad&1);io.NavInputs[ImGuiNavInput_Cancel]=keys[88]||keys[27]||pressed[88]||pressed[27]||bool(pad&258);
  if(practice_open){if((pad&1)&&!(previous_pad&1))pressed[90]=true;if((pad&258)&&!(previous_pad&258))pressed[88]=true;}
  previous_pad=pad;
  ImGui::NewFrame();if(practice_open)draw_practice();draw_overlay();ImGui::Render();pack_draw();std::fill(std::begin(pressed),std::end(pressed),false);
}
const uint32_t* th12_practice_ui_draw(){return draw_header;}
int th12_practice_ui_action(){const int value=action;action=0;return value;}
int th12_practice_ui_flags(){return int(menu_open)|(int(advanced_open)<<1)|(int(tracker_open)<<2)|(int(th12::practice::ui::Mobile::expanded[th12::practice::ui::Mobile::Advanced])<<3);}
void th12_practice_ui_shutdown(){if(!initialized)return;cancel_pointer();ImGui::DestroyContext();initialized=false;th12::practice::ui::Mobile::reset();}
}
#endif
