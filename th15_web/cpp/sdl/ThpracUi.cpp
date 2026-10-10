#include "ThpracUi.hpp"
#include "ApplicationState.hpp"
#if TH_ENABLE_THPRAC
#include "../game/PracticeSections.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeUiLabels.hpp"
#include "../game/PracticeVersion.hpp"
#include "../game/PracticeLicense.hpp"
#include "../game/PracticeAbScores.hpp"
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <emscripten.h>
#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cmath>
#include <random>
#include <ctime>
#include <deque>
#include "../../../portable/sdl/third_party/stb_image.h"
namespace th15::sdl::ThpracUi {
namespace {
bool initialized=false,down[256]{},edge[256]{},armed=false,was_open=false,desktop=false;
float px=-FLT_MAX,py=-FLT_MAX;bool pointer_down=false;int locale=0,index=0;
struct PointerEdge {bool down;float x,y;};
std::deque<PointerEdge> pointer_edges;
bool pointer_sample_down=false;
unsigned generation=0,draw_generation=~0u;
void publish_menu(bool open){EM_ASM({const v=!!$0;if(Module.eaglerThpracMenuOpen===v)return;Module.eaglerThpracMenuOpen=v;window.dispatchEvent(new CustomEvent('eagler-thprac-menu',{detail:{open:v}}));},int(open));}
void toggle(ApplicationState& app,u32 bit){if(app.practice.replay)return;app.practice.cheats^=bit;app.practice.assisted|=app.practice.cheats!=0;}
const char* label(const char* const* values){return values[locale];}
eagler::thprac::PracticeKeyMonitor key_monitor;
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::mt19937::result_type(std::time(nullptr))));}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include <eagler/thprac/PracticeSpeed.inc>
ImTextureID practice_blind_image(ApplicationState& app,const unsigned char* fallback,size_t length){
 size_t bytes=0;auto* custom=static_cast<unsigned char*>(SDL_LoadFile("/blind.png",&bytes));int w=0,h=0,channels=0;
 auto* image=custom&&bytes<=64*1024*1024?stbi_load_from_memory(custom,int(bytes),&w,&h,&channels,4):nullptr;SDL_free(custom);
 if(!image)image=stbi_load_from_memory(fallback,int(length),&w,&h,&channels,4);
 if(!image)return nullptr;const auto texture=app.graphics.backend.create_imgui_texture(w,h,image);stbi_image_free(image);return reinterpret_cast<ImTextureID>(uintptr_t(texture));
}
#include "PracticeBlind.inc"
void release_blind_image(){if(g_blind_view_opt.blind_texture){if(auto* renderer=touhou::sdl::current())renderer->release_imgui_texture(u32(uintptr_t(g_blind_view_opt.blind_texture)));g_blind_view_opt.blind_texture=nullptr;}g_blind_view_opt.is_texture_failed=false;}
void secret_options(ApplicationState& app){
 if(!ImGui::CollapsingHeader("Super Secret Settings"))return;
 if(ImGui::Button("ass bullet")){app.practice.flip_screen_y=!app.practice.flip_screen_y;if(app.scene()&&!app.practice.replay)app.practice.assisted=true;}
 ImGui::Checkbox(label(practice_THPRAC_BLIND),&g_blind_view_opt.blind_view);ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(practice_THPRAC_BLIND_DESC));ImGui::SameLine();ImGui::SetNextItemWidth(75);
 ImGui::DragFloat(label(practice_THPRAC_BLIND_SZ),&g_blind_view_opt.blind_size,1,20,600);ImGui::SameLine();
 if(ImGui::Button((std::string(label(practice_THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_RELOAD))+"##blind_reload").c_str()))release_blind_image();
}
void key_monitor_options(ApplicationState& app){
 ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&app.practice.show_keyboard_monitor);
 if(!app.practice.show_keyboard_monitor)return;
 if(!key_monitor.g_record_key_aps){if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){key_monitor.clear_record();key_monitor.g_record_key_aps=true;}}
 else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
 ImGui::SameLine();
 if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT))){const auto csv=key_monitor.csv();EM_ASM({const blob=new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'});const url=URL.createObjectURL(blob);const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},csv.c_str());}
 const auto& aps=key_monitor.g_recorded_aps;
 // The upstream lambda casts vector<int> to vector<uint8_t> (undefined
 // behavior). Keep its last-600 sampling, but read the actual element type.
 if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int idx){const auto& values=*static_cast<const std::vector<int>*>(data);return float(values[(values.size()>600?values.size()-600:0)+idx]);},const_cast<std::vector<int>*>(&aps),int(std::min<size_t>(600,aps.size())),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void input_options(ApplicationState& app){
 auto& input=app.practice.input;
 auto check=[&](const char* const* name,bool& value,const char* const* description=nullptr){ImGui::Checkbox(label(name),&value);if(description){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(description));}};
 check(practice_TH_ADV_DISABLE_X_KEY,input.disable_xkey,practice_TH_ADV_DISABLE_X_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_SHIFT_KEY,input.disable_shiftkey,practice_TH_ADV_DISABLE_SHIFT_KEY_DESC);ImGui::SameLine();check(practice_TH_ADV_DISABLE_Z_KEY,input.disable_zkey,practice_TH_ADV_DISABLE_Z_KEY_DESC);
 check(practice_TH_ADV_DISABLE_C_KEY_SAMETIME,input.disable_Ckey_at_same_time);ImGui::SameLine();check(practice_TH_ADV_FORCE_SHIFT_KEY,input.force_shiftkey);
 check(practice_THPRAC_FAST_RETRY,input.enable_fast_retry,practice_THPRAC_FAST_RETRY_DESC2);
 if(input.shoot_key_DIK>=0){ImGui::SameLine();check(practice_THPRAC_AUTO_SHOOT,input.enable_auto_shoot);}
 // This is the SDL/browser keyboard boundary, not a pretend Win32/DInput DLL.
 if(ImGui::IsKeyDown(16)){if(ImGui::IsKeyPressed('F'))input.enable_fast_retry=!input.enable_fast_retry;if(ImGui::IsKeyPressed('D'))input.disable_xkey=!input.disable_xkey;if(ImGui::IsKeyPressed('S'))input.disable_zkey=!input.disable_zkey;if(ImGui::IsKeyPressed('A'))input.disable_shiftkey=!input.disable_shiftkey;}
}
bool dialogue(int section){switch(section){case TH15_ST1_BOSS1:case TH15_ST2_BOSS1:case TH15_ST3_BOSS1:case TH15_ST4_BOSS1:case TH15_ST5_BOSS1:case TH15_ST6_BOSS1:case TH15_ST7_END_NS1:case TH15_ST7_MID1:return true;default:return false;}}
template<size_t N> bool combo(const char* name,int& value,const char* const (&items)[N][3]){const char* names[N];for(size_t i=0;i<N;i++)names[i]=label(items[i]);return ImGui::Combo(name,&value,names,int(N));}
std::vector<const PracticeSectionLabel*> sections(ApplicationState& app){
 auto& s=app.practice;auto& p=s.configured;std::vector<const PracticeSectionLabel*> result;
 const int difficulty=p.stage==6?4:app.progress.difficulty;
 for(const auto& l:practice_section_labels){const auto& info=practice_sections[l.id];if(info.appearance!=p.stage+1||(s.warp==2&&info.group!=1)||(s.warp==3&&info.group!=2)||(s.warp==4&&info.spell)||(s.warp==5&&!info.spell)||!*l.names[difficulty][locale])continue;result.push_back(&l);}return result;
}
void practice_menu(ApplicationState& app){
 auto& s=app.practice;auto& p=s.configured;
 if(!was_open){was_open=true;armed=false;}
 const int difficulty=p.stage==6?4:app.progress.difficulty;
 const ImVec2 size=locale==0?ImVec2(320,388.8f):locale==1?ImVec2(384,360):ImVec2(358.4f,388.8f);
 const ImVec2 position=locale==0?ImVec2(256,67.2f):locale==1?ImVec2(224,79.2f):ImVec2(236.8f,67.2f);
 ImGui::SetNextWindowSize(size,ImGuiCond_Always);ImGui::SetNextWindowPos(position,ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
 if(ImGui::Begin("Option###th15-purple-practice",nullptr,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove)){
  ImGui::PushItemWidth(locale==2?-67.2f:-64.f);ImGui::TextUnformatted(label(practice_TH_MENU));ImGui::Separator();
  combo(label(practice_TH_MODE),p.mode,practice_TH_MODE_SELECT);
  if(combo(label(practice_TH_STAGE),p.stage,practice_TH_STAGE_SELECT))p.section=p.phase=0;
  if(p.mode){
   if(p.stage==5&&s.warp==2){s.warp=0;p.section=p.phase=0;}
   // Purple removes the stage-six Mid Boss entry rather than allowing an
   // impossible section to reach the decoded ECL patcher.
   if(ImGui::BeginCombo(label(practice_TH_WARP),label(practice_TH_WARP_SELECT[s.warp]))){for(int i=0;i<6;i++){if(p.stage==5&&i==2)continue;if(ImGui::Selectable(label(practice_TH_WARP_SELECT[i]),s.warp==i)){s.warp=i;p.section=p.phase=0;}}ImGui::EndCombo();}
   if(!s.warp)p.section=p.phase=0;
   else if(s.warp==1){
    constexpr int setup[7][2]{{2,4},{3,3},{3,3},{3,3},{4,6},{4,0},{5,4}};const auto& counts=setup[p.stage];int chapter=std::clamp(p.section>=10000?p.section%100:1,1,counts[0]+counts[1]);char name[64];std::snprintf(name,sizeof name,label(counts[1]==0?practice_TH_STAGE_PORTION_N:chapter<=counts[0]?practice_TH_STAGE_PORTION_1:practice_TH_STAGE_PORTION_2),chapter<=counts[0]?chapter:chapter-counts[0]);
    if(ImGui::SliderInt(label(practice_TH_CHAPTER),&chapter,1,counts[0]+counts[1],name))p.phase=0;p.section=10000+(p.stage+1)*100+chapter;
   }else{
    auto entries=sections(app);auto found=std::find_if(entries.begin(),entries.end(),[&](auto* l){return l->id==p.section;});index=found==entries.end()?0:int(found-entries.begin());
    if(!entries.empty()){p.section=entries[index]->id;std::vector<const char*> names;for(auto* l:entries)names.push_back(l->names[difficulty][locale]);if(ImGui::Combo(label(practice_TH_WARP_SELECT[s.warp]),&index,names.data(),int(names.size()))){p.section=entries[index]->id;p.phase=0;}}
    else p.section=p.phase=0;
    if(dialogue(p.section))ImGui::Checkbox(label(practice_TH_DLG),&p.dlg);
   }
   const auto* phases=practice_phase_labels(p.section,difficulty);if(phases){const int count=practice_phase_count(p.section,difficulty);p.phase=std::clamp(p.phase,0,count-1);std::vector<const char*> names;for(int i=0;i<count;i++)names.push_back(label(phases[i]));ImGui::Combo(label(practice_TH_PHASE),&p.phase,names.data(),count);}else p.phase=0;
   if(p.section==TH15_ST3_BOSS1&&p.phase>=2)ImGui::SliderFloat(label(practice_TH_BT_PHASE),&p.doremy_normal_1_phase,-3.14159265f,3.14159265f);
   if(p.section==TH15_ST6_BOSS6&&p.phase==1&&difficulty>=2)ImGui::SliderFloat(label(practice_TH_ENHANCEMENT_VALUE),&p.enhanced_para,0,1);
   ImGui::SliderInt(label(practice_TH_LIFE),&p.life,0,8);p.life_fragment=std::min(p.life_fragment,p.stage==6?5:3);ImGui::SliderInt(label(practice_TH_LIFE_FRAGMENT),&p.life_fragment,0,p.stage==6?5:3);
   ImGui::SliderInt(label(practice_TH_BOMB),&p.bomb,0,8);ImGui::SliderInt(label(practice_TH_BOMB_FRAGMENT),&p.bomb_fragment,0,4);
   char power[24];std::snprintf(power,sizeof power,"%.2f",p.power/100.);ImGui::SliderInt(label(practice_TH_POWER),&p.power,0,400,power);
   ImGui::DragInt(label(practice_TH_VALUE),&p.value,10,0,999990);p.value=p.value/10*10;ImGui::DragInt(label(practice_TH_GRAZE),&p.graze,1,0,999999);
   const i64 lo=0,hi=9999999990LL;ImGui::DragScalar(label(practice_TH_SCORE),ImGuiDataType_S64,&p.score,10,&lo,&hi,"%lld");p.score=p.score/10*10;
   if(app.progress.character==3)ImGui::SliderInt(label(practice_TH15_REISEN_SHIELD),&p.reisen_shield,0,3);
  }
  ImGui::PopItemWidth();
 }ImGui::End();ImGui::PopStyleVar(2);
 if(!armed&&!down[90]&&!down[13]&&!down[88]&&!down[27])armed=true;
 const bool busy=ImGui::IsAnyItemActive()||ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel);
 if(armed&&!busy&&!s.advanced_visible&&(edge[90]||edge[13])&&p.valid(p.stage==6?4:app.progress.difficulty)){if(!p.mode){p.section=p.phase=0;p.dlg=false;p.reisen_shield=0;}if(!dialogue(p.section))p.dlg=false;if(app.progress.character!=3)p.reisen_shield=0;s.accepted=true;}
 if(armed&&!busy&&!s.advanced_visible&&(edge[88]||edge[27]))s.cancelled=true;
}
void tracker(ApplicationState& app){
 auto& s=app.practice;auto& b=app.scene()->battle;
 ImGui::SetNextWindowPos({445,255},ImGuiCond_Always);ImGui::SetNextWindowSize({180,0});ImGui::SetNextWindowBgAlpha(.9f);
 const auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
 if(ImGui::Begin("Info###th15-purple-info",nullptr,flags)){
  // The original manager's float globals start at +1c, not the serialized
  // checkpoint's +10. This mapping is covered by enemy-variables.test.mjs.
  if(s.active&&s.run.section==TH15_ST8_AB_TEST){ImGui::Columns(2);for(int i=0;i<5;i++){ImGui::Text("p%d",i+1);ImGui::NextColumn();ImGui::Text("%.2f",b.enemy_world.float_registers[7-i]);ImGui::NextColumn();}ImGui::Columns(1);}
  else {
   char heading[128];std::snprintf(heading,sizeof heading,"%s (%s)",label(practice_IGI_DIFF[std::clamp(app.progress.difficulty,0,4)]),label(practice_IGI_PL_15[std::clamp(app.progress.character,0,3)]));ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(heading).x)*.5f);ImGui::TextUnformatted(heading);
   const bool pointdevice=b.session.mode_flags&0x100;
   ImGui::Columns(2);
   auto rate=[&](){const int total=b.enemy_world.chapter_total,killed=b.enemy_world.chapter_defeated;ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH15_SHOOTING_DOWN_RATE));ImGui::NextColumn();ImGui::Text("%2.1f%%(%d/%d)",total?float(killed)/total*100:0,killed,total);ImGui::NextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH15_PRODUCT));ImGui::NextColumn();ImGui::Text("%2.1f",total?float(killed)/total*b.score.graze_chapter:0);ImGui::NextColumn();};
   if(pointdevice){
    if(s.shooting_down_rate)rate();
    ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_15_RE_TIMES));ImGui::NextColumn();ImGui::Text("%d / %d",app.progress.chapter_deaths,app.progress.stage_deaths[0]);ImGui::NextColumn();
    const int first=app.progress.stage==7?7:1,last=std::min(7,app.progress.stage);
    for(int stage=first;stage<=last;stage++){ImGui::Text(label(practice_THPRAC_INGAMEINFO_15_RE_TIMES_STAGE),stage);ImGui::NextColumn();ImGui::Text("%8d",app.progress.stage_deaths[stage+1]);ImGui::NextColumn();}
   }else{ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_MISS_COUNT));ImGui::NextColumn();ImGui::Text("%8u",s.misses);ImGui::NextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_BOMB_COUNT));ImGui::NextColumn();ImGui::Text("%8u",s.bombs);ImGui::NextColumn();if(s.shooting_down_rate)rate();}
   ImGui::Columns(1);
  }
 }ImGui::End();
}
#include "PracticeAbResult.inc"
}
bool initialize(ApplicationState& app){
 app.practice.record_keys=[&s=app.practice](u32 held){if(s.show_keyboard_monitor)key_monitor.record(15,held);};
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;})){app.practice.enabled=false;shutdown();return true;}
 if(initialized){app.practice.enabled=true;return true;}IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.IniFilename=nullptr;
 const int codes[]{9,37,39,38,40,33,34,36,35,45,46,8,32,13,27,13,65,67,86,88,89,90};for(int i=0;i<ImGuiKey_COUNT;i++)io.KeyMap[i]=codes[i];
 locale=EM_ASM_INT({const l=String(Module.eaglerOptions?.thpracLocale||'');return l.startsWith('ja')?2:l.startsWith('en')?1:0;});ImGui::StyleColorsDark();ImFontConfig config;config.RasterizerMultiply=1.25f;config.OversampleH=config.OversampleV=5;
 ImFontGlyphRangesBuilder glyphs;glyphs.AddRanges(io.Fonts->GetGlyphRangesChineseFull());glyphs.AddText("↑←↓→ΔΣ");static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,ranges.Data);if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}
 initialized=app.practice.enabled=true;return true;
}
void reset_input(){std::fill(std::begin(down),std::end(down),false);std::fill(std::begin(edge),std::end(edge),false);armed=was_open=false;cancel_pointer();draw_generation=~0u;if(initialized){auto& io=ImGui::GetIO();std::fill(std::begin(io.KeysDown),std::end(io.KeysDown),false);std::fill(std::begin(io.MouseDown),std::end(io.MouseDown),false);std::fill(std::begin(io.NavInputs),std::end(io.NavInputs),0.f);io.KeyCtrl=io.KeyShift=io.KeyAlt=io.KeySuper=false;io.MousePos={px,py};io.MouseWheel=io.MouseWheelH=0;io.InputQueueCharacters.clear();}}
void shutdown(){reset_input();release_blind_image();g_blind_view_opt={};if(auto* renderer=touhou::sdl::current())renderer->flip_present_y=false;if(initialized)ImGui::DestroyContext();initialized=false;generation=0;draw_generation=~0u;desktop=false;key_monitor={};reaction_test.Reset();publish_menu(false);}
void mouse(int type,float x,float y){if(auto* renderer=touhou::sdl::current();renderer&&renderer->flip_present_y)y=480-y;px=x;py=y;if(type==1||type==2){const bool pressed=type==1;if(pressed!=pointer_down){if(pointer_edges.size()>=64){cancel_pointer();return;}pointer_edges.push_back({pressed,x,y});pointer_down=pressed;}}}
void cancel_pointer(){pointer_edges.clear();pointer_down=pointer_sample_down=false;px=py=-FLT_MAX;}
void process_event(const SDL_Event& e,float scale){if(!initialized)return;if(e.type==SDL_EVENT_MOUSE_MOTION){if(e.motion.which==SDL_TOUCH_MOUSEID)return;desktop=true;mouse(0,e.motion.x/scale,e.motion.y/scale);}else if(e.type==SDL_EVENT_MOUSE_BUTTON_DOWN||e.type==SDL_EVENT_MOUSE_BUTTON_UP){if(e.button.which==SDL_TOUCH_MOUSEID)return;desktop=true;if(e.button.button==SDL_BUTTON_LEFT)mouse(e.type==SDL_EVENT_MOUSE_BUTTON_DOWN?1:2,e.button.x/scale,e.button.y/scale);}else if(e.type==SDL_EVENT_MOUSE_WHEEL)ImGui::GetIO().MouseWheel+=e.wheel.y;}
bool captures_pointer(float x,float y){if(!initialized)return false;if(auto* renderer=touhou::sdl::current();renderer&&renderer->flip_present_y)y=480-y;if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;for(auto* w:ImGui::GetCurrentContext()->Windows)if(w->Active&&!w->Hidden&&!(w->Flags&ImGuiWindowFlags_NoMouseInputs)&&w->OuterRectClipped.Contains({x,y}))return true;return false;}
bool captures_game_input(){return initialized&&was_open;}
void update_input(ApplicationState& app,const bool* keys){if(!initialized)return;++generation;const u32 bits=u32(EM_ASM_INT({return Module.eaglerControls?.thpracKeyboardBits|0;}));for(int i=0;i<256;i++){bool held=keys[i]||(i==8&&(bits&1))||(i==9&&(bits&256))||(i>=112&&i<=118&&(bits&(1u<<(i-111))))||(i==123&&(bits&512));edge[i]=held&&!down[i];down[i]=held;}
 if(edge[9]&&app.scene()&&!ImGui::IsAnyItemActive())app.practice.tracker_visible=!app.practice.tracker_visible;
 if(edge[8]&&!ImGui::IsAnyItemActive())app.practice.menu_visible=!app.practice.menu_visible;
 if(edge[123])app.practice.advanced_visible=!app.practice.advanced_visible;
 if(edge[27]&&app.practice.advanced_visible)app.practice.advanced_visible=false;
 if(app.practice.menu_visible&&app.scene())for(int i: {0,1,2,3,4,5,6})if(edge[112+i])toggle(app,1u<<i);
 if(app.practice.menu_visible&&app.scene()&&edge[85])toggle(app,PracticeEnemyInvincible);
 publish_menu(app.practice.menu_visible);
}
void render(ApplicationState& app){if(!initialized)return;if(draw_generation==generation){app.graphics.backend.render_imgui(ImGui::GetDrawData(),1);return;}draw_generation=generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60;io.MousePos={px,py};if(!pointer_edges.empty()){const auto event=pointer_edges.front();pointer_edges.pop_front();pointer_sample_down=event.down;io.MousePos={event.x,event.y};}io.MouseDown[0]=pointer_sample_down;io.KeyCtrl=down[17];io.KeyShift=down[16];io.KeyAlt=down[18];io.ConfigDragClickToInputText=desktop;for(int i=0;i<256;i++)io.KeysDown[i]=down[i];
 if(app.practice.menu)for(int i=48;i<=57;i++)if(edge[i])io.AddInputCharacter(ImWchar(i));
 io.NavInputs[ImGuiNavInput_DpadUp]=down[38];io.NavInputs[ImGuiNavInput_DpadDown]=down[40];io.NavInputs[ImGuiNavInput_DpadLeft]=down[37];io.NavInputs[ImGuiNavInput_DpadRight]=down[39];io.NavInputs[ImGuiNavInput_Activate]=down[90]||down[13];io.NavInputs[ImGuiNavInput_Cancel]=down[88]||down[27];if(app.practice.advanced_visible)std::fill(std::begin(io.NavInputs),std::end(io.NavInputs),0.f);ImGui::NewFrame();
 if(app.practice.menu)practice_menu(app);else was_open=false;
 if(app.practice.advanced_visible){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);if(ImGui::Begin("Advanced###th15-purple-advanced",nullptr,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)){
  ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();
  ImGui::BeginChild("Adv. Options",{0,0});
  if(ImGui::CollapsingHeader(label(practice_TH_GAME_SPEED)))GameFPSOpt(app.practice.speed,true);
  if(ImGui::CollapsingHeader(label(practice_TH_GAMEPLAY))){
  input_options(app);key_monitor_options(app);
  ImGui::Checkbox(label(practice_THPRAC_INFLIVES_MAP),&app.practice.map_inf_life_to_no_continue);
  ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH15_SHOW_SHOOTING_DOWN_RATE),&app.practice.shooting_down_rate);
  if(ImGui::Checkbox(label(practice_TH_BOSS_FORCE_MOVE_DOWN),&app.practice.force_boss_move_down)&&app.scene()&&!app.practice.replay)app.practice.assisted=true;
  ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC));
  ImGui::SameLine();ImGui::SetNextItemWidth(90);
  if(ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&app.practice.boss_move_down_range,.002f,0.f,1.f)){app.practice.boss_move_down_range=std::clamp(app.practice.boss_move_down_range,0.f,1.f);if(app.scene()&&app.practice.force_boss_move_down&&!app.practice.replay)app.practice.assisted=true;}
  ImGui::Checkbox(label(practice_TH_DISABLE_MASTER),&app.practice.disable_master_display);
  ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(practice_TH_DISABLE_MASTER_DESC));
  ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&app.practice.show_lock_timer);
  if(ImGui::Checkbox(label(practice_TH_FACTOR_ACB),&app.practice.all_clear_bonus)&&app.scene()&&!app.practice.replay)app.practice.assisted=true;
  ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(practice_TH_FACTOR_ACB_DESC));
  }
  secret_options(app);
  if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);else reaction_test.Reset();
  if(ImGui::CollapsingHeader(label(practice_TH_ABOUT_THPRAC))){
   static bool show_license=false;
   ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_source_version);
   ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));
   ImGui::NewLine();ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");ImGui::NewLine();
   if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;
   if(show_license){ImGui::BeginChild("COPYING",{0,ImGui::GetIO().DisplaySize.y*.8f},true);ImGui::PushTextWrapPos();ImGui::TextUnformatted(practice_license);ImGui::PopTextWrapPos();ImGui::EndChild();}
  }
  ImGui::EndChild();
 }ImGui::End();}
 if(app.practice.menu_visible){ImGui::SetNextWindowPos({5,5},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{6,5});ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4,2});if(ImGui::Begin("Mod Menu###th15-purple-overlay",nullptr,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoNav|ImGuiWindowFlags_NoFocusOnAppearing)){
  ImGui::SetWindowFontScale(.875f);
  const auto hotkey=[](const char* key,const char* text,bool enabled,bool disabled){char name[192];std::snprintf(name,sizeof name,enabled?"[%s: %s]":"%s: %s",key,text);ImGui::BeginDisabled(disabled);if(enabled)ImGui::PushStyleColor(ImGuiCol_Text,{0,1,0,1});const auto cursor=ImGui::GetCursorPos();const auto size=ImGui::CalcTextSize(name);ImGui::TextUnformatted(name);ImGui::SetCursorPos(cursor);const bool clicked=ImGui::InvisibleButton(key,size);if(enabled)ImGui::PopStyleColor();ImGui::EndDisabled(disabled);return clicked;};
  const char* labels[]{label(practice_TH_MUTEKI),label(practice_TH_INFLIVES2),label(practice_TH_INFBOMBS),label(practice_TH_INFPOWER),label(practice_TH_TIMELOCK),label(practice_TH_AUTOBOMB),label(practice_TH_EL_BGM)};
  for(int i:{0,1,2,3,4,5,6}){char key[8];std::snprintf(key,sizeof key,"F%d",i+1);if(hotkey(key,labels[i],app.practice.cheats&(1u<<i),app.practice.replay||!app.scene()))toggle(app,1u<<i);}
  if(hotkey("Tab",label(practice_THPRAC_INGAMEINFO),app.practice.tracker_visible,false))app.practice.tracker_visible=!app.practice.tracker_visible;
  if(hotkey("U",label(practice_TH_ENEMY_MUTEKI),app.practice.cheats&PracticeEnemyInvincible,app.practice.replay||!app.scene()))toggle(app,PracticeEnemyInvincible);
 }ImGui::End();ImGui::PopStyleVar(2);}
 if(app.practice.tracker_visible&&app.scene())tracker(app);
 if(app.practice.show_keyboard_monitor&&app.scene()){
  KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);
  KeysHUD(15,{1280,0},{840,0},style,true,false);
 }
 if(app.scene()&&app.scene()->battle.player){const auto& pos=app.scene()->battle.player->motion.position;RenderBlindView(app,{pos.x,pos.y},{192,0},{32,16},ImGui::GetIO().DisplaySize.x/640.f);}
 app.graphics.backend.flip_present_y=app.practice.flip_screen_y;
 app.practice.lock_timer.draw_tick();
 if(app.scene())app.practice.input.gui_tick();
 if(app.practice.input.enable_auto_shoot&&app.practice.input.is_auto_shooting){auto* p=ImGui::GetOverlayDrawList();const float sz=32*io.DisplaySize.x/1280;const auto text=ImGui::CalcTextSize("A");p->AddRectFilled({0,0},{sz,sz},0xffffdddd);p->PushClipRect({0,0},{sz,sz});p->AddText({sz*.5f-text.x*.5f,sz*.5f-text.y*.5f},0xffcc2222,"A");p->PopClipRect();}
 if(app.practice.show_lock_timer&&app.practice.cheat(PracticeTime)){char text[32];std::snprintf(text,sizeof text,"%.2f",float(app.practice.lock_timer.frames)/60.f);auto size=ImGui::CalcTextSize(text);auto* p=ImGui::GetOverlayDrawList();p->AddRectFilled({32,0},{110,size.y},0xffffffff);p->AddText({110-size.x,0},0xff000000,text);}
 if(app.practice.force_boss_move_down){auto* p=ImGui::GetOverlayDrawList();auto size=ImGui::CalcTextSize(label(practice_TH_BOSS_FORCE_MOVE_DOWN));p->AddRectFilled({120,0},{120+size.x,size.y},0xffcccccc);p->AddText({120,0},0xff0000ff,label(practice_TH_BOSS_FORCE_MOVE_DOWN));}
 if(app.practice.active&&app.scene()&&app.practice.run.section==TH15_ST8_AB_TEST)practice_ab_test_result(app);
 ImGui::Render();app.graphics.backend.render_imgui(ImGui::GetDrawData(),1);
}
}
#else
namespace th15::sdl::ThpracUi {
bool initialize(ApplicationState&){return true;}void shutdown(){}void process_event(const SDL_Event&,float){}void update_input(ApplicationState&,const bool*){}void render(ApplicationState&){}bool captures_game_input(){return false;}bool captures_pointer(float,float){return false;}void mouse(int,float,float){}void cancel_pointer(){}
void reset_input(){}
}
#endif
