#include "ThpracUi.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../game/PracticeSectionCatalog.hpp"
#include "Renderer.hpp"
#include "imgui.h"
#include "imgui_freetype.h"
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th08::ThpracUi {
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
void toggle_cheat(BrowserRuntime& runtime,int bit){
 auto& p=runtime.app.session.practice;if(p.replay)return;p.cheats^=1u<<bit;if(p.cheats)p.assisted=true;
}
void hotkey_line(const char* key,const char* label,bool& value){
 const auto cursor=ImGui::GetCursorPos();if(value)ImGui::TextColored({0,1,0,1},"[%s: %s]",key,label);else ImGui::Text("%s: %s",key,label);
 ImGui::SetCursorPos(cursor);const ImVec2 size{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()};if(ImGui::InvisibleButton(key,size))value=!value;
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
 if(p.warp==0||p.warp==6){p.section=0;return;}if(p.warp==1){static constexpr int counts[]{2,4,3,6,6,5,2,2,7};int chapter=p.section>=10000?p.section%100:1;chapter=std::clamp(chapter,1,counts[p.stage]);p.section=10000+(p.stage+1)*100+chapter;return;}
 auto matches=matching_sections(p,difficulty);if(matches.empty()){p.section=0;practice_section_index=0;return;}auto found=std::find_if(matches.begin(),matches.end(),[&](auto* s){return s->id==p.section;});if(found!=matches.end())practice_section_index=int(found-matches.begin());practice_section_index=std::clamp(practice_section_index,0,int(matches.size())-1);p.section=matches[practice_section_index]->id;
}
void draw_practice(BrowserRuntime& runtime){
 auto& state=runtime.app.session.practice;auto& p=state.configured;
 // Extra is its own difficulty. Every other stage needs a non-Extra difficulty,
 // otherwise returning from an Extra run leaves difficulty 4 and the normal
 // stages expose no valid sections (thprac's Extra names are empty for them).
 static int non_extra_difficulty=1;int& difficulty=runtime.app.title.context.difficulty;
 if(difficulty<4)non_extra_difficulty=difficulty;
 difficulty=(p.stage==8)?4:non_extra_difficulty;
 if(!practice_was_open){
  practice_was_open=true;practice_keys_armed=false;practice_section_index=0;select_current_section(p,difficulty);
  // THGuiPrac::State(1): when the gauge type changes, reset the gauge to the
  // shottype's initial value.
  const int shot=runtime.app.title.context.character;
  const int type=shot==2?-1:shot==3?1:shot==10?2:(shot==4||shot==6||shot==8)?3:(shot==5||shot==7||shot==9||shot==11)?4:0;
  static int gauge_type=-2;if(gauge_type!=type){gauge_type=type;p.gauge=type==-1?10000:type==1||type==2?-5000:0;}
 }
 const ImVec2 size=locale==0?ImVec2(370,390):locale==1?ImVec2(440,375):ImVec2(380,390);const ImVec2 pos=locale==0?ImVec2(245,75):locale==1?ImVec2(190,75):ImVec2(250,75);
 ImGui::SetNextWindowSize(size,ImGuiCond_Always);ImGui::SetNextWindowPos(pos,ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
 constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
 if(ImGui::Begin("Option###th08-thprac-practice",nullptr,flags)){
  ImGui::PushItemWidth(locale==1?-80.f:locale==2?-65.f:-60.f);ImGui::TextUnformatted(tr("练习选项","Option","オプション"));ImGui::Separator();
  const char* modes[]={tr("原版练习","Original","オリジナル"),tr("自定义练习","Custom","カスタム")};int mode=p.mode?1:0;if(ImGui::Combo(tr("模式","Mode","モード"),&mode,modes,2))p.mode=mode;
  const char* stages[]={"1","2","3","4A","4B","5","6A","6B","Extra"};if(ImGui::Combo(tr("关卡","Stage","ステージ"),&p.stage,stages,9)){p.section=0;practice_section_index=0;}
  if(p.mode==1){
   const char* warps[]={tr("无","None","なし"),tr("道中","Stage Portion","道中"),tr("道中Boss","Mid Boss","道中ボス"),tr("关底Boss","End Boss","ボス"),tr("非符","Non Spell","通常"),tr("符卡","Spell Card","スペカ"),tr("帧","Frame","フレーム")};
   // Stages 4A/4B have no midboss: warp 2 is unavailable, like upstream.
   if(p.stage==3||p.stage==4){if(p.warp==2)p.warp=0;const char* no_mid[]{warps[0],warps[1],warps[3],warps[4],warps[5],warps[6]};int wi=p.warp<2?p.warp:p.warp-1;if(ImGui::Combo(tr("传送","Warp","ワープ"),&wi,no_mid,6)){p.warp=wi<2?wi:wi+1;p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}}
   else if(ImGui::Combo(tr("传送","Warp","ワープ"),&p.warp,warps,7)){p.section=0;p.phase=0;p.frame=0;practice_section_index=0;select_current_section(p,difficulty);}
   if(p.warp==1){static constexpr int setup[9][2]{{1,1},{4,0},{2,1},{4,2},{4,2},{3,2},{2,0},{2,0},{3,4}};const auto& counts=setup[p.stage];int chapter=p.section>=10000?p.section%100:1;
    char portion[64];if(!counts[1])std::snprintf(portion,sizeof(portion),"#%d",chapter);else if(chapter<=counts[0])std::snprintf(portion,sizeof(portion),tr("前半 #%d","First Half #%d","前半 #%d"),chapter);else std::snprintf(portion,sizeof(portion),tr("后半 #%d","Second Half #%d","後半 #%d"),chapter-counts[0]);
    if(ImGui::SliderInt(tr("章节","Chapter","チャプター"),&chapter,1,counts[0]+counts[1],portion))p.section=10000+(p.stage+1)*100+chapter;}
   else if(p.warp>=2&&p.warp<=5){auto matches=matching_sections(p,difficulty);if(!matches.empty()){select_current_section(p,difficulty);std::vector<const char*> names;for(auto* s:matches)names.push_back(s->names[std::clamp(difficulty,0,4)][locale]);if(ImGui::Combo(warps[p.warp],&practice_section_index,names.data(),int(names.size()))){p.section=matches[practice_section_index]->id;p.phase=0;}if(section_has_dialogue(p.section))ImGui::Checkbox(tr("对话","Dialog","会話"),reinterpret_cast<bool*>(&p.dlg));}}
   else if(p.warp==6)ImGui::DragInt(tr("帧","Frame","フレーム"),&p.frame,2,0,0x7fffffff);
   if(p.section==66)ImGui::SliderInt(tr("阶段","Phase","段階"),&p.phase,0,2);else if(p.section==104)ImGui::SliderInt(tr("阶段","Phase","段階"),&p.phase,0,6);else p.phase=0;
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
 const bool widget_busy=text_editing;text_editing=ImGui::IsAnyItemActive();
 if(practice_keys_armed&&!widget_busy&&(pressed(VK_Z)||pressed(VK_RETURN))){select_current_section(p,difficulty);state.run=p;state.run.warp=0;state.accepted=true;}
 if(practice_keys_armed&&!widget_busy&&(pressed(VK_X)||pressed(VK_ESCAPE))){state.menu=false;runtime.app.title.menus.state.cursor=runtime.app.title.context.character;runtime.app.title.menus.ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);}
}
void draw_overlay(BrowserRuntime& runtime){
 auto& state=runtime.app.session.practice;if(!state.enabled)return;
 if(menu_open){ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);constexpr auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Mod Menu###th08-thprac-overlay",nullptr,flags)){static const char* keys[]{"F1","F2","F3","F4","F5","F6"};const char* labels[]{tr("无敌","Invincibility","無敵"),tr("锁残","Inf. Lives","残機減らない"),tr("锁Bomb","Inf. Bombs","ボム減らない"),tr("锁火力","Inf. Power","霊力減らない"),tr("锁时","Time Lock","残り時間減らない"),tr("自动B","Auto Bomb","自動喰らいボム")};
   for(int i=0;i<6;i++){bool value=state.cheats&(1u<<i);hotkey_line(keys[i],labels[i],value);if(value!=bool(state.cheats&(1u<<i)))toggle_cheat(runtime,i);}bool value=state.everlasting_bgm;hotkey_line("F7",tr("永续BGM","Everlasting BGM","永遠に続くBGM"),value);state.everlasting_bgm=value;
  }ImGui::End();
 }
 if(tracker_open&&runtime.app.in_game()){
  static const char* shots[3][12]={{"结界组","咏唱组","红魔组","冥界组","灵梦","紫","魔理沙","爱丽丝","咲夜","蕾米莉亚","妖梦","幽幽子"},{"Border Team","Magic Team","Scarlet Team","Netherworld Team","Reimu","Yukari","Marisa","Alice","Sakuya","Remilia","Youmu","Yuyuko"},{"結界組","詠唱組","紅魔組","冥界組","霊夢","紫","魔理沙","アリス","咲夜","レミリア","妖夢","幽々子"}};
  ImGui::SetNextWindowSize({170,0},ImGuiCond_Always);ImGui::SetNextWindowPos({450,220},ImGuiCond_Always);constexpr auto flags=ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
  if(ImGui::Begin("Tracker###th08-thprac-tracker",nullptr,flags)){const int shot=std::clamp(runtime.app.game.globals.shot,0,11);const char* title=shots[locale][shot];const auto size=ImGui::CalcTextSize(title);ImGui::SetCursorPosX(ImGui::GetWindowSize().x*.5f-size.x*.5f);ImGui::TextUnformatted(title);if(ImGui::BeginTable("Tracker table",2)){auto row=[](const char* label,const char* format,int a,int b=0){ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextUnformatted(label);ImGui::TableNextColumn();ImGui::Text(format,a,b);};const auto& n=runtime.app.session.numbers;row("Miss","%d (%d)",Scalar::truncate(n.deaths),int(state.tracker_dissolve_count));row("Bomb","%d",Scalar::truncate(n.bombs_used));row(tr("收符卡数","Captured Spells","取得スペル"),"%d",n.captured_spells);row(tr("LSC数","Last Spells","ラストスペル"),"%d",int(state.tracker_last_spell_captures));ImGui::EndTable();}}
  ImGui::End();
 }
 if(advanced_open){ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);ImGui::SetNextWindowBgAlpha(.8f);ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);constexpr auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
  if(ImGui::Begin("Advanced Options###th08-thprac-advanced",nullptr,flags)){ImGui::TextUnformatted(tr("高级选项","Advanced Options","拡張オプション"));ImGui::Separator();ImGui::BeginChild("Adv. Options",{0,0});if(ImGui::CollapsingHeader(tr("游戏速度","Game Speed","ゲームの速度"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::BeginDisabled();int fps=60;ImGui::SliderInt("FPS",&fps,60,6000);ImGui::EndDisabled();}if(ImGui::CollapsingHeader(tr("游戏进行","Gameplay","ゲームプレイ"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::Checkbox(tr("全通奖励","All Clear Bonus","オールクリアボーナス"),&state.all_clear_bonus);ImGui::Checkbox(tr("符卡名不一致时不重置符卡历史","Don't overwrite scores when spell name changes","言語切替時にスペルカード履歴を上書きしない"),&state.doswnc);}if(ImGui::CollapsingHeader(tr("关于","About","バージョン情報"),ImGuiTreeNodeFlags_DefaultOpen)){ImGui::TextUnformatted("thprac v2.3.0.3");ImGui::TextUnformatted("github.com/touhouworldcup/thprac");ImGui::TextUnformatted("Thanks: You!");}ImGui::EndChild();ImGui::SetWindowFocus();}ImGui::End();ImGui::PopStyleVar(2);
 }
}
}

bool initialize(){
 if(!EM_ASM_INT({return Module.eaglerOptions?.thpracEnabled?1:0;}))return true;
 if(initialized)return true;IMGUI_CHECKVERSION();ImGui::CreateContext();auto& io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;io.DisplaySize={640,480};io.DisplayFramebufferScale={1,1};io.IniFilename=nullptr;
 io.KeyMap[ImGuiKey_Tab]=VK_TAB;io.KeyMap[ImGuiKey_LeftArrow]=VK_LEFT;io.KeyMap[ImGuiKey_RightArrow]=VK_RIGHT;io.KeyMap[ImGuiKey_UpArrow]=VK_UP;io.KeyMap[ImGuiKey_DownArrow]=VK_DOWN;io.KeyMap[ImGuiKey_PageUp]=VK_PRIOR;io.KeyMap[ImGuiKey_PageDown]=VK_NEXT;io.KeyMap[ImGuiKey_Home]=VK_HOME;io.KeyMap[ImGuiKey_End]=VK_END;io.KeyMap[ImGuiKey_Insert]=VK_INSERT;io.KeyMap[ImGuiKey_Delete]=VK_DELETE;io.KeyMap[ImGuiKey_Backspace]=VK_BACK;io.KeyMap[ImGuiKey_Space]=VK_SPACE;io.KeyMap[ImGuiKey_Enter]=VK_RETURN;io.KeyMap[ImGuiKey_Escape]=VK_ESCAPE;io.KeyMap[ImGuiKey_KeyPadEnter]=VK_RETURN;io.KeyMap[ImGuiKey_A]='A';io.KeyMap[ImGuiKey_C]='C';io.KeyMap[ImGuiKey_V]='V';io.KeyMap[ImGuiKey_X]='X';io.KeyMap[ImGuiKey_Y]='Y';io.KeyMap[ImGuiKey_Z]='Z';
 ImGui::StyleColorsDark();locale=EM_ASM_INT({const v=String(Module.eaglerOptions?.thpracLocale||'');return v.startsWith('ja')?2:v.startsWith('en')?1:0;});ImFontConfig config{};config.FontNo=0;config.RasterizerMultiply=1.25f;config.OversampleH=5;config.OversampleV=5;const ImWchar* range=locale==0?io.Fonts->GetGlyphRangesChineseFull():locale==2?io.Fonts->GetGlyphRangesJapanese():io.Fonts->GetGlyphRangesDefault();
 // Keep MS Gothic for the TH08 game renderer, but always render thprac with
 // Unifont. Some spell/option labels contain CJK glyphs missing from the
 // bundled MS Gothic even when the UI locale itself is Japanese or English.
 // The launcher mounts /unifont.otf whenever thprac is enabled.
 io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,range);
 if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){ImGui::DestroyContext();return false;}initialized=true;return true;
}
void shutdown(){if(!initialized)return;if(frame_open){ImGui::EndFrame();frame_open=false;}publish_menu(false);ImGui::DestroyContext();initialized=false;}
void process_event(const SDL_Event& event){if(!initialized)return;if(event.type==SDL_EVENT_MOUSE_MOTION){if(event.motion.which!=SDL_TOUCH_MOUSEID&&event.motion.which!=SDL_PEN_MOUSEID)desktop_pointer=true;mouse_x=event.motion.x;mouse_y=event.motion.y;}else if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP){if(event.button.which!=SDL_TOUCH_MOUSEID&&event.button.which!=SDL_PEN_MOUSEID)desktop_pointer=true;mouse_x=event.button.x;mouse_y=event.button.y;if(event.button.button==SDL_BUTTON_LEFT)mouse_down=event.type==SDL_EVENT_MOUSE_BUTTON_DOWN;}else if(event.type==SDL_EVENT_MOUSE_WHEEL){ImGui::GetIO().MouseWheel+=event.wheel.y;ImGui::GetIO().MouseWheelH+=event.wheel.x;}}
void mouse(int type,float x,float y){mouse_x=x;mouse_y=y;if(type==1)mouse_down=true;else if(type==2)mouse_down=false;}
void update_input(BrowserRuntime& runtime){if(!initialized)return;++input_generation;auto* keys=runtime.keyboard_state();const u32 bits=bridge_keys();for(int i=0;i<256;i++){const bool down=keys[i]!=0||bridge_key_down(i,bits);key_pressed[i]=down&&!key_down[i];key_down[i]=down;}auto& state=runtime.app.session.practice;if(!state.enabled){menu_open=tracker_open=advanced_open=false;publish_menu(false);return;}if(pressed(VK_BACK)&&!ImGui::IsAnyItemActive())menu_open=!menu_open;if(pressed(VK_TAB)&&!ImGui::IsAnyItemActive()&&runtime.app.in_game())tracker_open=!tracker_open;if(pressed(VK_F12))advanced_open=!advanced_open;if(menu_open&&runtime.app.in_game()&&!state.replay){for(int i=0;i<6;i++)if(pressed(VK_F1+i))toggle_cheat(runtime,i);if(pressed(VK_F7))state.everlasting_bgm=!state.everlasting_bgm;}if(pressed(VK_ESCAPE)&&advanced_open)advanced_open=false;publish_menu(menu_open);}
bool captures_game_input(){return advanced_open||practice_was_open;}
void render(BrowserRuntime& runtime,touhou::sdl::Renderer& renderer){if(!initialized)return;
 if(rendered_generation==input_generation){if(frame_drawn)renderer.render_imgui(ImGui::GetDrawData(),runtime.backbuffer());return;}
 rendered_generation=input_generation;auto& io=ImGui::GetIO();io.DeltaTime=1.f/60.f;io.DisplaySize={640,480};io.MousePos={mouse_x,mouse_y};io.MouseDown[0]=mouse_down;io.KeyCtrl=key_down[VK_CONTROL];io.KeyShift=key_down[VK_SHIFT];io.KeyAlt=key_down[VK_MENU];io.ConfigDragClickToInputText=desktop_pointer;for(int i=0;i<256;i++)io.KeysDown[i]=key_down[i];
 // Desktop thprac numeric fields should be directly editable: ImGui's drag
 // widgets can now switch to TempInputText on a click-release without a drag.
 // Queue numeric characters for the whole practice-menu frame; ImGui clears
 // unused characters at EndFrame, while an active TempInputText consumes them.
 if(runtime.app.session.practice.menu){for(int vk=48;vk<=57;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-48));for(int vk=96;vk<=105;vk++)if(pressed(vk))io.AddInputCharacter(ImWchar('0'+vk-96));if(pressed(189)||pressed(109))io.AddInputCharacter('-');if(pressed(190)||pressed(110))io.AddInputCharacter('.');}
 io.NavInputs[ImGuiNavInput_DpadUp]=key_down[VK_UP];io.NavInputs[ImGuiNavInput_DpadDown]=key_down[VK_DOWN];io.NavInputs[ImGuiNavInput_DpadLeft]=key_down[VK_LEFT];io.NavInputs[ImGuiNavInput_DpadRight]=key_down[VK_RIGHT];io.NavInputs[ImGuiNavInput_Activate]=key_down[VK_Z]||key_down[VK_RETURN];io.NavInputs[ImGuiNavInput_Cancel]=key_down[VK_X]||key_down[VK_ESCAPE];ImGui::NewFrame();frame_open=true;
 if(runtime.app.session.practice.menu)draw_practice(runtime);else if(!(key_down[VK_X]||key_down[VK_Z]||key_down[VK_ESCAPE]||key_down[VK_RETURN]))practice_was_open=false;draw_overlay(runtime);ImGui::Render();frame_open=false;renderer.render_imgui(ImGui::GetDrawData(),runtime.backbuffer());frame_drawn=true;
}
}
