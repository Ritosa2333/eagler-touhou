#include "ThpracUi.hpp"
#include <eagler/thprac/PracticeSdlKeyboard.hpp>
#include <eagler/thprac/PracticeButton.hpp>
#include "../platform/BrowserRuntime.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "../game/PracticeSections.hpp"
#include "../game/PracticeUiLabels.hpp"
#include "../game/PracticeVersion.hpp"
#include "../game/PracticeLicense.hpp"
#include "../game/GraphicsMath.hpp"
#include "../game/PracticeRsqrtTables.hpp"
#include "Renderer.hpp"
#include "PlatformHost.hpp"
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

namespace th08::ThpracUi {
namespace {
bool initialized=false,frame_open=false,menu_open=false,tracker_open=false,advanced_open=false,practice_was_open=false,practice_keys_armed=false,text_editing=false,desktop_pointer=false;
bool key_down[256]{},key_pressed[256]{};float mouse_x=-FLT_MAX,mouse_y=-FLT_MAX;bool mouse_down=false;
struct PointerEdge {bool down;float x,y;};std::deque<PointerEdge> pointer_edges;bool pointer_sample_down=false;
int locale=0,practice_section_index=0;
int non_extra_difficulty=1,gauge_type=-2;
bool show_license=false,reset_speed_ui=true;
// ImGui runs one frame per fixed 60 Hz tick (update_input). High-refresh
// presentation passes must re-render the cached draw data only: starting a
// new ImGui frame per present consumed edge-triggered input (typed digits)
// several times per press and ran ImGui's clock several times fast.
unsigned input_generation=0,rendered_generation=~0u;bool frame_drawn=false;
struct HotkeyObservation {unsigned key=0,id=0,generation=0;bool value=false;ImVec2 min{},max{};};
std::array<HotkeyObservation,8> hotkey_observations{};unsigned hotkey_observation_count=0;
struct ControlObservation {unsigned semantic=0,id=0,generation=0,activations=0;double value=0;bool visible=false,enabled=true,popup=false;ImVec2 min{},max{};ImRect clip{};};
std::array<ControlObservation,56> control_observations{};
std::array<unsigned,64> control_activations{};unsigned control_count=0,control_overflow=0;
double advanced_scroll[8]{};
std::array<double,40> display_observation{};
void observe_bullet_hitbox(ImDrawList* draw,int index,const BulletState& bullet,float player_width,ImVec2 p1,ImVec2 p2,int vertices){
 auto& o=display_observation;++o[13];o[26]+=vertices;
 if(o[13]==1){o[15]=index;o[16]=bullet.since_fired.current;o[17]=bullet.position.x;o[18]=bullet.position.y;
  o[19]=bullet.sprites.hitbox.x;o[20]=bullet.sprites.hitbox.y;o[21]=player_width;o[22]=p1.x;o[23]=p1.y;o[24]=p2.x;o[25]=p2.y;}
}
void license_text(const char* text){
 auto* draw=ImGui::GetWindowDrawList();const int before=draw->VtxBuffer.Size;ImGui::TextUnformatted(text);
 std::uint32_t hash=2166136261u;unsigned length=0;for(auto* p=reinterpret_cast<const unsigned char*>(text);*p;p++){hash=(hash^*p)*16777619u;++length;}
 display_observation[32]=length;display_observation[33]=hash;display_observation[34]=draw->VtxBuffer.Size-before;
}
void observe_control(unsigned semantic,double value,bool activated=false,bool enabled=true,ImGuiWindow* owner=nullptr){
 if(!semantic||semantic>=control_activations.size()||control_count==control_observations.size()){++control_overflow;return;}
 if(activated)++control_activations[semantic];auto& out=control_observations[control_count++];
 if(!owner)owner=ImGui::GetCurrentWindow();const auto& item=owner->DC.LastItemRect;
 out.semantic=semantic;out.id=owner->DC.LastItemId;out.generation=input_generation;out.activations=control_activations[semantic];
 out.value=value;out.visible=owner->ClipRect.Overlaps(item);out.enabled=enabled;out.min=item.Min;out.max=item.Max;out.clip=owner->ClipRect;out.popup=(owner->Flags&ImGuiWindowFlags_Popup)!=0;
}
bool control_checkbox(unsigned id,const char* text,bool* value){const bool changed=ImGui::Checkbox(text,value);observe_control(id,*value,changed);return changed;}
bool control_button(unsigned id,const char* text,bool enabled=true){const bool clicked=eagler::thprac::button(text,enabled);observe_control(id,clicked,clicked,enabled);return clicked;}
bool control_header(unsigned id,const char* text,ImGuiTreeNodeFlags flags=0){const bool open=ImGui::CollapsingHeader(text,flags);observe_control(id,open,ImGui::IsItemActivated());return open;}
void observe_hotkey(const char* key,bool value){
 if(hotkey_observation_count>=hotkey_observations.size())return;
 auto& out=hotkey_observations[hotkey_observation_count++];out.key=key[0]=='F'?111u+unsigned(key[1]-'0'):unsigned(key[0]);
 out.id=ImGui::GetItemID();out.generation=input_generation;out.value=value;out.min=ImGui::GetItemRectMin();out.max=ImGui::GetItemRectMax();
}

enum Vk {VK_BACK=8,VK_TAB=9,VK_RETURN=13,VK_SHIFT=16,VK_CONTROL=17,VK_MENU=18,VK_ESCAPE=27,VK_SPACE=32,VK_PRIOR=33,VK_NEXT=34,VK_END=35,VK_HOME=36,VK_LEFT=37,VK_UP=38,VK_RIGHT=39,VK_DOWN=40,VK_INSERT=45,VK_DELETE=46,VK_1=49,VK_2=50,VK_3=51,VK_X=88,VK_Z=90,VK_F1=112,VK_F7=118,VK_F12=123};
const char* tr(const char* zh,const char* en,const char* ja){return locale==0?zh:locale==2?ja:en;}
const char* label(const char* const* values){return values[locale];}
eagler::thprac::PracticeKeyMonitor* current_key_monitor=nullptr;
// The generated renderer uses the original native name; bind it per session.
#define key_monitor (*current_key_monitor)
#include <eagler/thprac/PracticeKeyHud.inc>
#undef key_monitor
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::mt19937::result_type(std::time(nullptr))));}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include "PracticeHitbox.inc"
#include <eagler/thprac/PracticeSpeed.inc>
void help_marker(const char* const* values){ImGui::SameLine();ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(values));}
void draw_advanced(BrowserRuntime& runtime){
 auto& state=runtime.app.session.practice;auto& input=state.input;
 ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});
 const auto* child=ImGui::GetCurrentWindow();advanced_scroll[0]=child->InnerClipRect.Min.x;advanced_scroll[1]=child->InnerClipRect.Min.y;
 advanced_scroll[2]=child->InnerClipRect.Max.x;advanced_scroll[3]=child->InnerClipRect.Max.y;advanced_scroll[4]=child->Scroll.y;advanced_scroll[5]=child->ScrollMax.y;advanced_scroll[6]=child->ID;advanced_scroll[7]=ImGui::GetStyle().ScrollbarSize;
 if(control_header(22,label(practice_TH_GAME_SPEED)))if(GameFPSOpt(state.speed,true,&reset_speed_ui))sdl_audio_music_speed(float(state.speed.fps)/60.f);
 if(control_header(23,label(practice_TH_GAMEPLAY),ImGuiTreeNodeFlags_DefaultOpen)){
  control_checkbox(1,label(practice_TH_ADV_DISABLE_X_KEY),&input.disable_xkey);help_marker(practice_TH_ADV_DISABLE_X_KEY_DESC);
  control_checkbox(2,label(practice_TH_ADV_DISABLE_SHIFT_KEY),&input.disable_shiftkey);help_marker(practice_TH_ADV_DISABLE_SHIFT_KEY_DESC);
  control_checkbox(3,label(practice_TH_ADV_DISABLE_Z_KEY),&input.disable_zkey);help_marker(practice_TH_ADV_DISABLE_Z_KEY_DESC);
  control_checkbox(5,label(practice_TH_ADV_FORCE_SHIFT_KEY),&input.force_shiftkey);
  control_checkbox(6,label(practice_THPRAC_FAST_RETRY),&input.enable_fast_retry);help_marker(practice_THPRAC_FAST_RETRY_DESC2);
  control_checkbox(7,label(practice_THPRAC_KB_OPEN),&state.show_keyboard_monitor);
  if(state.show_keyboard_monitor){auto& monitor=state.key_monitor;if(control_button(8,label(monitor.g_record_key_aps?practice_THPRAC_KB_RECORD_STOP:practice_THPRAC_KB_RECORD_START))){if(!monitor.g_record_key_aps)monitor.clear_record();monitor.g_record_key_aps=!monitor.g_record_key_aps;}ImGui::SameLine();if(control_button(9,label(practice_THPRAC_KB_OUTPUT))){const auto csv=monitor.csv();EM_ASM({const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},csv.c_str());}}
  control_checkbox(10,label(practice_THPRAC_INFLIVES_MAP),&state.map_inf_life_to_no_continue);
  if(control_button(11,label(practice_TH_ONE_KEY_DIE),runtime.app.in_game()&&!state.replay)){auto& n=runtime.app.session.numbers;n.lives=n.bombs=0;runtime.app.game.player_state.life.state=2;runtime.app.game.player_state.life.predead_count=0;runtime.app.game.globals.player_state=2;runtime.app.session.values.refresh_integrity();state.assisted=true;}help_marker(practice_TH_ONE_KEY_DIE_DESC);
  control_checkbox(12,label(practice_THPRAC_FIX_RSQRT),&state.use_custom_rsqrt);help_marker(practice_THPRAC_FIX_RSQRT_DESC);
  if(state.use_custom_rsqrt){
   ImGui::SetNextItemWidth(200.f);const char* choices[3]{};const char* text=label(practice_THPRAC_SELECT_CPU_COMBO);
   for(int i=0;i<3;i++){choices[i]=text;text+=std::strlen(text)+1;}
   auto* cpu_owner=ImGui::GetCurrentWindow();const bool cpu_open=ImGui::BeginCombo(label(practice_THPRAC_SELECT_CPU),choices[std::clamp(state.rsqrt_cpu,0,2)]);
   observe_control(13,state.rsqrt_cpu,cpu_open?ImGui::IsWindowAppearing():ImGui::IsItemActivated(),true,cpu_owner);
   if(cpu_open){for(int i=0;i<3;i++){ImGui::PushID(i);const bool selected=ImGui::Selectable(choices[i],state.rsqrt_cpu==i);observe_control(27+i,i,selected);if(selected)state.rsqrt_cpu=i;ImGui::PopID();}ImGui::EndCombo();}
   if(control_button(14,label(practice_THPRAC_EXPORT_CUSTOM))){const auto& data=PracticeRsqrtTables::custom();EM_ASM({const url=URL.createObjectURL(new Blob([HEAPU8.slice($0,$0+$1)]));const a=document.createElement('a');a.href=url;a.download='custom_bin.bin';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);},data.data(),data.size());}help_marker(practice_THPRAC_EXPORT_CUSTOM_DESC);
   ImGui::SameLine();if(control_button(15,label(practice_THPRAC_IMPORT_CUSTOM)))EM_ASM({const input=document.createElement('input');input.type='file';input.onchange=async()=>{const f=input.files?.[0];if(!f)return;if(f.size<8388608){alert('RSQRT: 8 MiB table required');return;}try{FS.writeFile('/thprac-rsqrt-import.bin',new Uint8Array(await f.slice(0,8388608).arrayBuffer()));Module.eaglerRsqrtImportReady=true;}catch(e){alert(String(e));}};input.click();});help_marker(practice_THPRAC_IMPORT_CUSTOM_DESC);
  }
  control_checkbox(16,label(practice_THPRAC_TH08_FORCE_LS),&state.force_last_spell);
  control_checkbox(17,label(practice_TH_ENABLE_LOCK_TIMER),&state.show_lock_timer);
  control_checkbox(18,label(practice_TH_DISABLE_MASTER),&state.disable_master);
  control_checkbox(19,label(practice_THPRAC_SHOW_BULLET_HITBOX),&state.show_bullet_hitbox);
  control_checkbox(20,label(practice_TH_FACTOR_ACB),&state.all_clear_bonus);help_marker(practice_TH_FACTOR_ACB_DESC);
  control_checkbox(21,label(practice_TH08_DOSWNC),&state.doswnc);
 }
 if(control_header(24,label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);else reaction_test.Reset();
 if(control_header(25,label(practice_TH_ABOUT_THPRAC))){auto* draw=ImGui::GetWindowDrawList();const int before=draw->VtxBuffer.Size;ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_source_version);ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");display_observation[27]=4;display_observation[35]=draw->VtxBuffer.Size-before;if(control_button(26,label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;if(show_license)license_text(practice_license);}
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
void toggle_cheat(BrowserRuntime& runtime,int bit){
 auto& p=runtime.app.session.practice;if(p.replay)return;p.cheats^=1u<<bit;if(p.cheats)p.assisted=true;
}
void hotkey_line(const char* key,const char* label,bool& value){
 if(MobileThprac::enabled){MobileThprac::checkbox(key,label,value);observe_hotkey(key,value);return;}
 const auto cursor=ImGui::GetCursorPos();if(value)ImGui::TextColored({0,1,0,1},"[%s: %s]",key,label);else ImGui::Text("%s: %s",key,label);
 ImGui::SetCursorPos(cursor);const ImVec2 size{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()};if(ImGui::InvisibleButton(key,size))value=!value;observe_hotkey(key,value);
}
bool section_has_dialogue(int section){
 switch(section){case 3:case 10:case 18:case 25:case 35:case 47:case 55:case 57:case 66:case 67:case 69:case 83:case 86:return true;default:return false;}
}
// Upstream GuiCombo hides entries whose name is empty for the current
// difficulty (ComboSections skips them, CheckComboItemNew cannot land on
// them); e.g. stage 1's midboss spell exists only on Hard/Lunatic.
std::vector<const PracticeSectionLabel*> matching_sections(const PracticeConfig& p, int difficulty){
 std::vector<const PracticeSectionLabel*> out;for(const auto& s:practice_section_labels){if(s.stage!=p.stage)continue;if(p.warp==2&&s.group!=1)continue;if(p.warp==3&&s.group!=2)continue;if(p.warp==4&&s.spell)continue;if(p.warp==5&&!s.spell)continue;const char* name=s.names[std::clamp(difficulty,0,4)][locale];if(!name||!*name)continue;out.push_back(&s);}return out;
}
void select_current_section(PracticeConfig& p, int difficulty){
 if(p.stage==9)p.warp=5;
 if(p.warp==0||p.warp==6){p.section=0;return;}if(p.warp==1){static constexpr int counts[]{2,4,3,6,6,5,2,2,7};int chapter=p.section>=10000?p.section%100:1;chapter=std::clamp(chapter,1,counts[p.stage]);p.section=10000+(p.stage+1)*100+chapter;return;}
 auto matches=matching_sections(p,difficulty);if(matches.empty()){p.section=0;practice_section_index=0;return;}auto found=std::find_if(matches.begin(),matches.end(),[&](auto* s){return s->id==p.section;});if(found!=matches.end())practice_section_index=int(found-matches.begin());practice_section_index=std::clamp(practice_section_index,0,int(matches.size())-1);p.section=matches[practice_section_index]->id;
}
void draw_practice(BrowserRuntime& runtime){
 auto& state=runtime.app.session.practice;auto& p=state.configured;
 // Extra is its own difficulty. Every other stage needs a non-Extra difficulty,
 // otherwise returning from an Extra run leaves difficulty 4 and the normal
 // stages expose no valid sections (thprac's Extra names are empty for them).
 int& difficulty=runtime.app.title.context.difficulty;
 if(difficulty<4)non_extra_difficulty=difficulty;
 difficulty=p.stage==9?1:(p.stage==8)?4:non_extra_difficulty;
 if(!practice_was_open){
  practice_was_open=true;practice_keys_armed=false;practice_section_index=0;select_current_section(p,difficulty);
  // THGuiPrac::State(1): when the gauge type changes, reset the gauge to the
  // shottype's initial value.
  const int shot=runtime.app.title.context.character;
  const int type=shot==2?-1:shot==3?1:shot==10?2:(shot==4||shot==6||shot==8)?3:(shot==5||shot==7||shot==9||shot==11)?4:0;
  if(gauge_type!=type){gauge_type=type;p.gauge=type==-1?10000:type==1||type==2?-5000:0;}
 }
 const ImVec2 size=locale==0?ImVec2(370,390):locale==1?ImVec2(440,375):ImVec2(380,390);const ImVec2 pos=locale==0?ImVec2(245,75):locale==1?ImVec2(190,75):ImVec2(250,75);
 ImGui::SetNextWindowSize(size,ImGuiCond_Always);ImGui::SetNextWindowPos(pos,ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
 constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
 if(ImGui::Begin("Option###th08-thprac-practice",nullptr,flags)){
  // Reserve the translated label lane, without changing native panel/font size.
  float label_width=0;
  for(const char* text:{tr("人妖槽","Human/Youkai Gauge","人妖ゲージ"),tr("总计蓝点","Point (Total)","得点(合計)"),tr("本关蓝点","Point (Stage)","得点(ステージ)"),tr("累计使魔","Familiars","累計使い魔")})label_width=std::max(label_width,ImGui::CalcTextSize(text).x);
  ImGui::PushItemWidth(-label_width-ImGui::GetStyle().ItemInnerSpacing.x);ImGui::TextUnformatted(tr("练习选项","Option","オプション"));ImGui::Separator();
  const char* modes[]={tr("原版练习","Original","オリジナル"),tr("自定义练习","Custom","カスタム")};int mode=p.mode?1:0;if(ImGui::Combo(tr("模式","Mode","モード"),&mode,modes,2))p.mode=mode;
  const char* stages[]={"1","2","3","4A","4B","5","6A","6B","Extra","Last Word"};if(ImGui::Combo(tr("关卡","Stage","ステージ"),&p.stage,stages,10)){p.section=0;practice_section_index=0;}
  if(p.mode==1){
   if(p.stage==9)p.warp=5;
   const char* warps[]={tr("无","None","なし"),tr("道中","Stage Portion","道中"),tr("道中Boss","Mid Boss","道中ボス"),tr("关底Boss","End Boss","ボス"),tr("非符","Non Spell","通常"),tr("符卡","Spell Card","スペカ"),tr("帧","Frame","フレーム")};
   // Last Word is native Spell Practice, not an ordinary stage warp.
   if(p.stage!=9){
   // Stages 4A/4B have no midboss: warp 2 is unavailable, like upstream.
   if(p.stage==3||p.stage==4){if(p.warp==2)p.warp=0;const char* no_mid[]{warps[0],warps[1],warps[3],warps[4],warps[5],warps[6]};int wi=p.warp<2?p.warp:p.warp-1;if(ImGui::Combo(tr("传送","Warp","ワープ"),&wi,no_mid,6)){p.warp=wi<2?wi:wi+1;p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}}
   else if(ImGui::Combo(tr("传送","Warp","ワープ"),&p.warp,warps,7)){p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}
   }
   if(p.warp==1){static constexpr int setup[9][2]{{1,1},{4,0},{2,1},{4,2},{4,2},{3,2},{2,0},{2,0},{3,4}};const auto& counts=setup[p.stage];int chapter=p.section>=10000?p.section%100:1;
    char portion[64];if(!counts[1])std::snprintf(portion,sizeof(portion),"#%d",chapter);else if(chapter<=counts[0])std::snprintf(portion,sizeof(portion),tr("前半 #%d","First Half #%d","前半 #%d"),chapter);else std::snprintf(portion,sizeof(portion),tr("后半 #%d","Second Half #%d","後半 #%d"),chapter-counts[0]);
    if(ImGui::SliderInt(tr("章节","Chapter","チャプター"),&chapter,1,counts[0]+counts[1],portion))p.section=10000+(p.stage+1)*100+chapter;}
   else if(p.warp>=2&&p.warp<=5){auto matches=matching_sections(p,difficulty);if(!matches.empty()){select_current_section(p,difficulty);std::vector<const char*> names;for(auto* s:matches)names.push_back(s->names[std::clamp(difficulty,0,4)][locale]);if(ImGui::Combo(warps[p.warp],&practice_section_index,names.data(),int(names.size()))){p.section=matches[practice_section_index]->id;p.phase=0;}if(section_has_dialogue(p.section))ImGui::Checkbox(tr("对话","Dialog","会話"),reinterpret_cast<bool*>(&p.dlg));}}
   else if(p.warp==6)ImGui::DragInt(tr("帧","Frame","フレーム"),&p.frame,2,0,0x7fffffff);
   if(p.section==66)ImGui::SliderInt(tr("阶段","Phase","段階"),&p.phase,0,2);else if(p.section==104)ImGui::SliderInt(tr("阶段","Phase","段階"),&p.phase,0,6);else p.phase=0;
   if(p.section==TH08_ST2_BOSS3){bool points=p.sp1_pts!=0;if(ImGui::Checkbox(label(practice_TH08_ST2N2_POINTS),&points)){p.sp1_pts=points;if(points){p.bsX=48;p.bsY=96;p.power=127;}}ImGui::SetNextItemWidth(100);ImGui::DragInt(label(practice_TH_BOSSX),&p.bsX,1,-160,160);ImGui::SameLine();ImGui::SetNextItemWidth(128);ImGui::DragInt(label(practice_TH_BOSSY),&p.bsY,1,48,128);}
   ImGui::SliderInt(tr("残机","Life","残機"),&p.life,0,8);ImGui::SliderInt("Bomb",&p.bomb,0,8);ImGui::SliderInt(tr("火力","Power","霊力"),&p.power,0,128);
   int gaugeMin=-10000,gaugeMax=10000;const int shot=runtime.app.title.context.character;if(shot==3){gaugeMin=-5000;}else if(shot==10){gaugeMin=-5000;gaugeMax=5000;}else if(shot==4||shot==6||shot==8)gaugeMax=2000;else if(shot==5||shot==7||shot==9||shot==11)gaugeMin=-2000;
   // Upstream displays the gauge as a percentage (value / 100).
   float gauge_f=p.gauge/100.f;if(ImGui::SliderFloat(tr("人妖槽","Human/Youkai Gauge","人妖ゲージ"),&gauge_f,gaugeMin/100.f,gaugeMax/100.f,"%3.2f%%"))p.gauge=int(std::lround(gauge_f*100.f));
   const i64 score_min=0,score_max=9999999990ll;ImGui::DragScalar(tr("分数","Score","スコア"),ImGuiDataType_S64,&p.score,20.f,&score_min,&score_max,"%lld");p.score=p.score/10*10;
   ImGui::DragInt(tr("擦弹","Graze","グレイズ"),&p.graze,2,0,0x7fffffff);ImGui::DragInt(tr("总计蓝点","Point (Total)","得点(合計)"),&p.point_total,2,0,9999);ImGui::DragInt(tr("本关蓝点","Point (Stage)","得点(ステージ)"),&p.point_stage,2,0,9999);ImGui::DragInt(tr("刻符","Time Orbs","刻符"),&p.time,2,0,0x7fffffff);ImGui::DragInt(tr("夜点","Point Value","夜点"),&p.value,20,0,9999999);p.value=p.value/10*10;
   char night[16];if(p.night<2)std::snprintf(night,sizeof(night),"11:%s",p.night%2?"30":"00");else std::snprintf(night,sizeof(night),"%02d:%s",(p.night-2)/2,p.night%2?"30":"00");ImGui::SliderInt(tr("夜晚","Night","時刻"),&p.night,0,11,night);
   // Upstream CheckIfBoss(): any midboss/boss section (warp 2-5) shows familiars.
   if(p.warp>=2&&p.warp<=5)ImGui::SliderInt(tr("累计使魔","Familiars","累計使い魔"),&p.familiar,0,2000);const int rankMax=p.rankLock?99:(difficulty>1?12:16);p.rank=std::clamp(p.rank,8,rankMax);ImGui::SliderInt("Rank",&p.rank,8,rankMax);ImGui::Checkbox(tr("锁Rank","Rank Lock","ランク固定"),reinterpret_cast<bool*>(&p.rankLock));
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
 if(practice_keys_armed&&!widget_busy&&(pressed(VK_X)||pressed(VK_ESCAPE))){state.menu=false;runtime.app.title.menus.state.cursor=runtime.app.title.context.character;runtime.app.title.menus.ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);}
}
void draw_overlay(BrowserRuntime& runtime){
 auto& state=runtime.app.session.practice;if(!state.enabled)return;
 RenderBtHitbox(ImGui::GetOverlayDrawList(),runtime);
 if(state.show_lock_timer&&runtime.app.in_game()&&(state.cheats&16)&&MobileThprac::draw_tool(MobileThprac::Indicators,"Tools###th08-mobile-tools",{120,65},locale)){char text[32];std::snprintf(text,sizeof text,"%.2f",float(runtime.app.game.globals.practice_lock_frames)/60.f);const auto size=ImGui::CalcTextSize(text);auto* p=ImGui::GetOverlayDrawList();p->AddRectFilled({32,0},{110,size.y},0xffffffff);const int before=p->VtxBuffer.Size;p->AddText({110-size.x,0},0xff000000,text);display_observation[1]=1;display_observation[2]=runtime.app.game.globals.practice_lock_frames;display_observation[3]=std::strlen(text);for(unsigned i=0;i<std::min(size_t(8),std::strlen(text));i++)display_observation[4+i]=text[i];display_observation[12]=p->VtxBuffer.Size-before;}
 if(state.show_keyboard_monitor&&runtime.app.in_game()&&MobileThprac::draw_tool(MobileThprac::Keyboard,"Keys###th08-mobile-keys",{420,65},locale)){current_key_monitor=&state.key_monitor;KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);KeysHUD(8,{1280,0},{840,0},style,true,false);}
 if(MobileThprac::show(MobileThprac::Cheats,menu_open)){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Cheats, "Mod Menu###th08-thprac-overlay", flags, locale)){static const char* keys[]{"F1","F2","F3","F4","F5","F6"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁Bomb","Inf. Bombs","ボム減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム")};
   const bool cheat_disabled=state.replay||(MobileThprac::enabled&&!runtime.app.in_game());ImGui::BeginDisabled(cheat_disabled);for(int i=0;i<6;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F7",tr("永续BGM","Everlasting BGM","永遠に続くBGM"),value);state.everlasting_bgm=value;bool enemy=state.cheats&64;hotkey_line("U",label(practice_TH_ENEMY_MUTEKI),enemy);if(enemy!=bool(state.cheats&64))toggle_cheat(runtime,6);ImGui::EndDisabled(cheat_disabled);
  }ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Information,tracker_open)&&runtime.app.in_game()){
  static const char* shots[3][12]={{"结界组","咏唱组","红魔组","冥界组","灵梦","紫","魔理沙","爱丽丝","咲夜","蕾米莉亚","妖梦","幽幽子"},{"Border Team","Magic Team","Scarlet Team","Netherworld Team","Reimu","Yukari","Marisa","Alice","Sakuya","Remilia","Youmu","Yuyuko"},{"結界組","詠唱組","紅魔組","冥界組","霊夢","紫","魔理沙","アリス","咲夜","レミリア","妖夢","幽々子"}};
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,220},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(MobileThprac::begin(MobileThprac::Information, "Tracker###th08-thprac-tracker", flags, locale)){const int shot=std::clamp(runtime.app.game.globals.shot,0,11);const char* title=shots[locale][shot];const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);if(ImGui::BeginTable("Tracker table",2)){auto row=[](const char* label,const char* format,int a,int b=0){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(label);ImGui::TableNextColumn();ImGui::Text(format,a,b);};const auto& n=runtime.app.session.numbers;row("Miss","%d (%d)",Scalar::truncate(n.deaths),int(state.tracker_dissolve_count));row("Bomb","%d",Scalar::truncate(n.bombs_used));row(tr("收符卡数","Captured Spells","取得スペル"),"%d",n.captured_spells);row(tr("LSC数","Last Spells","ラストスペル"),"%d",int(state.tracker_last_spell_captures));ImGui::EndTable();}}
  ImGui::End();
 }
 if(MobileThprac::show(MobileThprac::Advanced,advanced_open)){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(MobileThprac::begin(MobileThprac::Advanced, "Advanced Options###th08-thprac-advanced", flags, locale)){draw_advanced(runtime);}ImGui::End();ImGui::PopStyleVar(2);
 }
}
}

bool initialize(){
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;}))return true;
 if(initialized)return true;IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;
 io.KeyMap[ImGuiKey_Tab]=VK_TAB;io.KeyMap[ImGuiKey_LeftArrow]=VK_LEFT;io.KeyMap[ImGuiKey_RightArrow]=VK_RIGHT;io.KeyMap[ImGuiKey_UpArrow]=VK_UP;io.KeyMap[ImGuiKey_DownArrow]=VK_DOWN;io.KeyMap[ImGuiKey_PageUp]=VK_PRIOR;io.KeyMap[ImGuiKey_PageDown]=VK_NEXT;io.KeyMap[ImGuiKey_Home]=VK_HOME;io.KeyMap[ImGuiKey_End]=VK_END;io.KeyMap[ImGuiKey_Insert]=VK_INSERT;io.KeyMap[ImGuiKey_Delete]=VK_DELETE;io.KeyMap[ImGuiKey_Backspace]=VK_BACK;io.KeyMap[ImGuiKey_Space]=VK_SPACE;io.KeyMap[ImGuiKey_Enter]=VK_RETURN;io.KeyMap[ImGuiKey_Escape]=VK_ESCAPE;io.KeyMap[ImGuiKey_KeyPadEnter]=VK_RETURN;io.KeyMap[ImGuiKey_A]='A';io.KeyMap[ImGuiKey_C]='C';io.KeyMap[ImGuiKey_V]='V';io.KeyMap[ImGuiKey_X]='X';io.KeyMap[ImGuiKey_Y]='Y';io.KeyMap[ImGuiKey_Z]='Z';
 ImGui::StyleColorsDark();locale=EM_ASM_INT({const v=String(Module.eaglerOptions?.thpracLocale||'');return v.startsWith('ja')?2:v.startsWith('en')?1:0;});ImFontConfig config{};config.FontNo=0;config.RasterizerMultiply=1.25f;config.OversampleH=5;config.OversampleV=5;ImFontGlyphRangesBuilder glyphs;glyphs.AddRanges(io.Fonts->GetGlyphRangesChineseFull());glyphs.AddText("↑←↓→ΔΣ");static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);const ImWchar* range=ranges.Data;
 // Keep MS Gothic for the TH08 game renderer, but always render thprac with
 // Unifont. Some spell/option labels contain CJK glyphs missing from the
 // bundled MS Gothic even when the UI locale itself is Japanese or English.
 // The launcher mounts /unifont.otf whenever thprac is enabled.
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,range);
 if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}initialized=true;return true;
}
void shutdown(){
 GraphicsMath::practice_rsqrt(false,'i');
 if(initialized){if(frame_open)ImGui::EndFrame();publish_menu(false);ImGui::DestroyContext();}
 initialized=frame_open=menu_open=tracker_open=advanced_open=practice_was_open=practice_keys_armed=text_editing=desktop_pointer=frame_drawn=false;
 std::fill(std::begin(key_down),std::end(key_down),false);std::fill(std::begin(key_pressed),std::end(key_pressed),false);
 cancel_pointer();input_generation=0;rendered_generation=~0u;current_key_monitor=nullptr;
 hotkey_observation_count=0;hotkey_observations={};
 control_count=control_overflow=0;control_observations={};control_activations={};std::fill(std::begin(advanced_scroll),std::end(advanced_scroll),0.);
 practice_section_index=0;non_extra_difficulty=1;gauge_type=-2;show_license=false;reset_speed_ui=true;reaction_test=THGuiTestReactionTest{};
}
void process_event(const SDL_Event& event){if(!initialized)return;if(event.type==SDL_EVENT_MOUSE_MOTION){if(event.motion.which==SDL_TOUCH_MOUSEID||event.motion.which==SDL_PEN_MOUSEID)return;desktop_pointer=true;mouse(0,event.motion.x,event.motion.y);}else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP){if(event.button.which==SDL_TOUCH_MOUSEID||event.button.which==SDL_PEN_MOUSEID)return;desktop_pointer=true;if(event.button.button==SDL_BUTTON_LEFT)mouse(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN?1:2,event.button.x,event.button.y);}else if(event.type==SDL_EVENT_MOUSE_WHEEL){ImGui::GetIO().MouseWheel+=event.wheel.y;ImGui::GetIO().MouseWheelH+=event.wheel.x;}}
void mouse(int type,float x,float y){mouse_x=x;mouse_y=y;if(type==1||type==2){const bool down=type==1;if(down!=mouse_down){if(pointer_edges.size()>=64){cancel_pointer();return;}pointer_edges.push_back({down,x,y});mouse_down=down;}}}
void cancel_pointer(){pointer_edges.clear();mouse_down=pointer_sample_down=false;mouse_x=mouse_y=-FLT_MAX;}
bool captures_pointer(float x,float y){
 if(!initialized)return false;
 if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
 for(auto* window:ImGui::GetCurrentContext()->Windows)if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
 return false;
}
void update_input(BrowserRuntime& runtime){
 if(!initialized)return;++input_generation;auto* keys=runtime.keyboard_state();const u32 bits=bridge_keys();
 for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}
 auto& state=runtime.app.session.practice;if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}
 if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;
 if((pressed(VK_TAB)||pressed(119))&&!ImGui::IsAnyItemActive()&&runtime.app.in_game())tracker_open=!tracker_open;
 if(pressed(VK_F12))advanced_open=!advanced_open;
 if(menu_open&&runtime.app.in_game()&&!state.replay){for(int i=0;i<6;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F7))state.everlasting_bgm=!state.everlasting_bgm;if(pressed('U'))toggle_cheat(runtime,6);}
 if(!ImGui::IsAnyItemActive()&&key_down[VK_SHIFT]&&pressed('F'))state.input.enable_fast_retry=!state.input.enable_fast_retry;
 if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);
}
bool captures_game_input(){return MobileThprac::captures_advanced(advanced_open)||practice_was_open;}
void observation(std::uint32_t* out,std::uint32_t count){
 if(!out||count<128)return;std::fill(out,out+count,0u);out[0]=1;
 if(!initialized||!ImGui::GetCurrentContext())return;
 auto bits=[](float value){std::uint32_t word=0;std::memcpy(&word,&value,sizeof word);return word;};
 out[1]=1;out[2]=input_generation;out[3]=rendered_generation;out[4]=menu_open;out[5]=tracker_open;out[6]=advanced_open;
 out[7]=MobileThprac::enabled;out[8]=MobileThprac::visible;out[9]=frame_drawn;out[10]=mouse_down||pointer_sample_down;out[11]=unsigned(pointer_edges.size());
 const char* names[]{"Mod Menu###th08-thprac-overlay","Advanced Options###th08-thprac-advanced","Tracker###th08-thprac-tracker"};
 const int panels[]{MobileThprac::Cheats,MobileThprac::Advanced,MobileThprac::Information};
 for(unsigned i=0;i<3;i++){const auto* window=ImGui::FindWindowByName(names[i]);auto* p=out+12+i*8;
  if(!window)continue;p[0]=1u|(window->Active?2u:0u)|(window->Hidden?4u:0u);p[1]=bits(window->Pos.x);p[2]=bits(window->Pos.y);
  p[3]=bits(window->Size.x);p[4]=bits(window->Size.y);p[5]=unsigned(window->DrawList->VtxBuffer.Size);p[6]=MobileThprac::expanded[panels[i]];p[7]=window->ID;
 }
 out[36]=unsigned(locale);out[37]=hotkey_observation_count;out[38]=text_editing;out[39]=practice_was_open;
 for(unsigned i=0;i<hotkey_observation_count;i++){const auto& item=hotkey_observations[i];auto* p=out+40+i*8;
  p[0]=item.key;p[1]=item.value;p[2]=bits(item.min.x);p[3]=bits(item.min.y);p[4]=bits(item.max.x);p[5]=bits(item.max.y);p[6]=item.generation;p[7]=item.id;
 }
 // PUK1 extends the reserved tail with actual UI input, preserving the prefix.
 out[104]=practice_keys_armed;out[105]=text_editing||MobileThprac::captures_advanced(advanced_open);
 out[106]=key_down[VK_Z]||key_down[VK_RETURN];out[107]=key_down[VK_X]||key_down[VK_ESCAPE];
 out[110]=0x50554b31u;out[111]=1;
 for(unsigned key=0;key<256;key++){if(key_down[key])out[112+key/32]|=1u<<(key%32);if(key_pressed[key])out[120+key/32]|=1u<<(key%32);}
}
void controls_observation(BrowserRuntime& runtime,double* out,std::uint32_t count){
 if(!out||count<1024)return;std::fill(out,out+count,0.);out[0]=2;if(!initialized)return;
 const auto& a=runtime.app;const auto& g=a.game;const auto& p=a.session.practice;const auto& n=a.session.numbers;
 out[1]=1;out[2]=input_generation;out[3]=rendered_generation;out[4]=advanced_open;out[5]=locale;out[6]=control_count;out[7]=control_overflow;
 std::copy(std::begin(advanced_scroll),std::end(advanced_scroll),out+8);
 out[16]=a.in_game();out[17]=g.control.state.frames;const auto& input=g.player.consumed_input();out[18]=input.sequence;out[19]=input.current;out[20]=input.previous;
 out[21]=g.player_state.life.state;out[22]=n.bombs;out[23]=n.power;out[24]=n.lives;out[25]=n.score;out[26]=n.deaths;out[27]=n.bombs_used;out[28]=g.globals.game_flags;out[29]=g.player_state.context.game_over;
 out[32]=p.input.disable_xkey;out[33]=p.input.disable_shiftkey;out[34]=p.input.disable_zkey;out[35]=p.input.force_shiftkey;out[36]=p.input.enable_fast_retry;
 out[37]=p.show_keyboard_monitor;out[38]=p.key_monitor.g_record_key_aps;out[39]=p.key_monitor.g_recorded_keys.size();out[40]=p.key_monitor.g_recorded_aps.size();
 out[41]=p.key_monitor.g_recorded_keys.empty()?0:p.key_monitor.g_recorded_keys.back();out[42]=p.key_monitor.g_aps_cur;
 out[43]=p.map_inf_life_to_no_continue;out[44]=p.use_custom_rsqrt;out[45]=p.rsqrt_cpu;out[46]=p.force_last_spell;out[47]=p.show_lock_timer;
 out[48]=p.disable_master;out[49]=p.show_bullet_hitbox;out[50]=p.all_clear_bonus;out[51]=p.doswnc;out[52]=p.input.fast_retry_count_down;
 out[53]=p.speed.fps;out[54]=p.speed.fps_status;out[55]=p.speed.fps_replay_slow;out[56]=p.speed.fps_replay_fast;out[57]=p.speed.fps_debug_acc;
 out[58]=g.globals.practice_force_last_spell;out[59]=g.globals.practice_enemy_invincible;out[60]=g.globals.practice_lock_frames;out[61]=g.gui_context.practice_all_clear_bonus;out[62]=g.spell_drawing.disable_master;out[63]=show_license;
 GraphicsMath::practice_normalization_observation(out+64);
 std::copy(display_observation.begin(),display_observation.end(),out+76);
 for(unsigned i=0;i<control_count;i++){const auto& c=control_observations[i];auto* row=out+128+i*16;
  row[0]=c.semantic;row[1]=c.id;row[2]=c.min.x;row[3]=c.min.y;row[4]=c.max.x-c.min.x;row[5]=c.max.y-c.min.y;
  row[6]=c.value;row[7]=c.visible;row[8]=c.enabled;row[9]=c.generation;row[10]=c.activations;row[11]=c.popup;
  row[12]=c.clip.Min.x;row[13]=c.clip.Min.y;row[14]=c.clip.Max.x;row[15]=c.clip.Max.y;
 }
}
double simulation_interval(BrowserRuntime& runtime){auto& state=runtime.app.session.practice;return state.enabled?state.speed.interval(state.replay,key_down[VK_CONTROL],key_down[VK_SHIFT],key_down[VK_SPACE]):1./60.;}
void render(BrowserRuntime& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 if(EM_ASM_INT({const ready=!!Module.eaglerRsqrtImportReady;Module.eaglerRsqrtImportReady=false;return ready?1:0;})){if(auto* file=std::fopen("/thprac-rsqrt-import.bin","rb")){std::vector<char> data(PracticeRsqrtTables::custom_size);if(std::fread(data.data(),1,data.size(),file)==data.size())PracticeRsqrtTables::import_custom(data.data(),data.size());std::fclose(file);std::remove("/thprac-rsqrt-import.bin");}}
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),runtime.backbuffer());return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};if(!pointer_edges.empty()){const auto edge=pointer_edges.front();pointer_edges.pop_front();pointer_sample_down=edge.down;io.MousePos={edge.x,edge.y};}io.MouseDown[0]=pointer_sample_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Numeric editing also applies to advanced tools outside the practice menu.
 eagler::thprac::sdl_keyboard_frame(io);
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 display_observation={};display_observation[0]=input_generation;hotkey_observation_count=control_count=control_overflow=0;std::fill(std::begin(advanced_scroll),std::end(advanced_scroll),0.);if(runtime.app.session.practice.menu)draw_practice(runtime);else practice_was_open=false;draw_overlay(runtime);auto& practice=runtime.app.session.practice;GraphicsMath::practice_rsqrt(practice.enabled&&practice.use_custom_rsqrt,"iac"[std::clamp(practice.rsqrt_cpu,0,2)]);ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),runtime.backbuffer());frame_drawn=true;
}
}
