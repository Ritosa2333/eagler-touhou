#include "PracticeUiTh14.hpp"
#if PORT_TITLE == 14 && defined(TH14_ENABLE_THPRAC)
#include "../game/GameSession.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <eagler/thprac/PracticePointer.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#include <functional>
#include <random>
#include "imgui_freetype.h"
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <cfloat>
#include <cstdio>
#include <cmath>
#include <deque>
#include <vector>

namespace th11::sdl::PracticeUiTh14 {
namespace {
namespace Mobile=eagler::thprac::mobile;
using Labels=const char* const[3];
struct Item {int id,stage,variant,phases,group;bool dialogue,drops,angle,timeout,spell;const char* names[5][3];};
#include "PracticeLabelsTh14.inc"
constexpr const char* shortcuts[]={"F1","F2","F3","F4","F5","F6"};
constexpr Labels browser_text[]={
    {"开始练习","Start practice","練習開始"},
    {"网页使用浏览器输入接口。","The web port uses browser input.","Web版はブラウザーの入力を使用します。"},
    {"已选择录像","Replay selected","リプレイ選択済み"},
};
constexpr int keys[]={9,37,39,38,40,33,34,36,35,45,46,8,32,13,27,13,65,67,86,88,89,90};
constexpr ImGuiWindowFlags panel_flags=ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar;
GameSession* game=nullptr;
ImGuiContext* context=nullptr;
PracticeTh14 params;
int locale=2,rank=1,shot=0;
bool setup=false,cheats=false,advanced=false,tracker=false,armed=false;
bool everlasting_bgm=false,setup_popup=false;
unsigned announced=0;
std::array<bool,256> held{},pressed{};
struct PointerEdge{bool down;float x,y;};
std::deque<PointerEdge> edges;
eagler::thprac::PracticePointer touch_routes;
int active_pointer=0;
bool owned_pointer=false,mouse_down=false;
float mouse_x=-FLT_MAX,mouse_y=-FLT_MAX,scroll_advanced=0,mouse_wheel=0;
const char* label(Labels& text){return text[locale];}
#include "PracticeAdvancedLabelsTh14.inc"
eagler::thprac::PracticeSpeed speed;
eagler::thprac::PracticeKeyMonitor key_monitor;
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){
    return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::random_device{}()));
}
#include <eagler/thprac/PracticeReaction.inc>
#include <eagler/thprac/PracticeSpeed.inc>
THGuiTestReactionTest reaction_test;
bool key_monitor_on=false,show_lock_timer=false,show_items=false,show_bonus=false,show_hitboxes=false;
bool lock_running=false,reset_speed_ui=false;
u32 lock_frames=0,last_sample_stage=0,last_sample_frame=0;
void publish(){
    const unsigned state=unsigned(captures())|(unsigned(setup)<<1)|(unsigned(Mobile::visible)<<2);
    if(announced==state)return;
    announced=state;
    EM_ASM({window.dispatchEvent(new CustomEvent('eagler-thprac-menu',{
        detail:{open:!!($0&1),practice:!!($0&2),mobile:!!($0&4)}}));},state);
}
bool on_title(){return game->state.phase==GameSessionPhase::title&&game->title;}
bool on_stage(){return game->state.phase==GameSessionPhase::stage||game->state.phase==GameSessionPhase::paused;}
bool editable(){return on_stage()&&!game->state.replay&&!game->state.demo;}
bool visible(){return setup||cheats||advanced||tracker||Mobile::visible;}
bool modal(){return setup||advanced||(Mobile::visible&&Mobile::expanded[Mobile::Advanced]);}
bool pointer_hit(float x,float y){
    if(modal())return true;
    if(!context||!visible())return false;
    for(const auto* window:context->Windows)
        if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&
           window->OuterRectClipped.Contains({x,y}))return true;
    return false;
}
bool awaiting_setup(){
    return on_title()&&game->title->screen==TitleScreen::Practice&&game->title->substate==2&&
        game->title->th14_practice_overlay_enabled&&game->title->th14_practice_stage_choice==0;
}
#include "PracticeMenuPurpleTh14.inc"
void cheat_options(){
    ImGui::SetNextWindowPos(Mobile::visible?ImVec2{8,8}:ImVec2{5,5},ImGuiCond_Always);
    ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);
    if(Mobile::begin(Mobile::Cheats,"Mod Menu###th14-purple-overlay",panel_flags|ImGuiWindowFlags_AlwaysAutoResize,locale)){
        const Labels* mods[]={&practice_TH_MUTEKI,&practice_TH_INFLIVES2,&practice_TH_INFBOMBS,&practice_TH_INFPOWER,&practice_TH_TIMELOCK,&practice_TH_AUTOBOMB};
        const auto toggle=[](const char* key,const char* text,bool& on,bool disabled){
            ImGui::BeginDisabled(disabled);
            bool changed=false;
            if(Mobile::visible&&!cheats)changed=Mobile::checkbox(key,text,on);
            else{
                char caption[160];std::snprintf(caption,sizeof caption,on?"[%s: %s]":"%s: %s",key,text);
                if(on)ImGui::PushStyleColor(ImGuiCol_Text,{0,1,0,1});
                const auto cursor=ImGui::GetCursorPos();ImGui::TextUnformatted(caption);ImGui::SetCursorPos(cursor);
                changed=ImGui::InvisibleButton(key,ImGui::CalcTextSize(caption));
                if(on)ImGui::PopStyleColor();
                if(changed)on=!on;
            }
            ImGui::EndDisabled(disabled);return changed;
        };
        for(int i=0;i<6;++i){
            const u32 mask=1u<<i;bool value=!!(game->practice_tools&mask);
            if(toggle(shortcuts[i],label(*mods[i]),value,!editable()))game->practice_tools^=mask;
        }
        toggle("F7",label(practice_TH_EL_BGM),everlasting_bgm,!editable());
        bool& info=Mobile::visible&&!cheats?Mobile::expanded[Mobile::Information]:tracker;
        if(toggle("Tab",label(practice_THPRAC_INGAMEINFO),info,!on_stage())&&Mobile::visible&&!cheats)tracker=false;
        bool enemy=(game->practice_tools&PrEnemy)!=0;
        if(toggle("U",label(practice_TH_ENEMY_MUTEKI),enemy,!editable()))game->practice_tools^=PrEnemy;
    }
    ImGui::End();
}
void export_keys(){
    const auto csv=key_monitor.csv();
    EM_ASM({
        const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));
        const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
    },csv.c_str());
}
#include "PracticeAdvancedPurpleTh14.inc"
#include "PracticeInfoPurpleTh14.inc"
bool ensure_context(){
    if(context)return true;
    IMGUI_CHECKVERSION();context=ImGui::CreateContext();
    auto& io=ImGui::GetIO();io.DisplaySize={640,480};io.DisplayFramebufferScale={2,2};io.IniFilename=nullptr;
    io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;
    static_assert(std::size(keys)==ImGuiKey_COUNT);
    for(int i=0;i<ImGuiKey_COUNT;++i)io.KeyMap[i]=keys[i];
    ImGui::StyleColorsDark();
    ImFontGlyphRangesBuilder glyphs;
    glyphs.AddRanges(io.Fonts->GetGlyphRangesDefault());
    for(const auto* row:purple_glyphs)glyphs.AddText(label(*row));
    for(const auto& row:browser_text)glyphs.AddText(label(row));
    for(const auto& item:practice_labels)for(const auto& row:item.names)for(const auto* text:row)glyphs.AddText(text);
    for(const auto* text:{"作弊菜单高级设置高级状态信息","チートメニュー詳細設定詳細情報"})glyphs.AddText(text);
    static ImVector<ImWchar> ranges;glyphs.BuildRanges(&ranges);
    ImFontConfig cfg;cfg.RasterizerMultiply=1.25f;
    if(SDL_GetPathInfo("/unifont.otf",nullptr))io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&cfg,ranges.Data);
    if(!io.FontDefault)io.FontDefault=io.Fonts->AddFontDefault();
    if(ImGuiFreeType::BuildFontAtlas(io.Fonts,0))return true;
    ImGui::DestroyContext(context);context=nullptr;return false;
}
}
void configure(GameSession& s,bool enabled,int language){
    close();game=&s;s.practice_enabled=enabled;s.practice_tools=0;
    locale=std::clamp(language,0,2);
    params={};params.stage=1;rank=1;shot=0;
    speed={};reset_speed_ui=true;reaction_test.Reset();everlasting_bgm=false;
    key_monitor={};key_monitor_on=show_lock_timer=show_items=show_bonus=show_hitboxes=show_drop_bar=show_license=false;
    s.purple_show_history=s.purple_fast_retry=false;s.purple_laser={};boss_range=.5f;
    for(bool& open:opt_open)open=true;
    lock_running=false;lock_frames=last_sample_stage=last_sample_frame=0;
}
void mobile(bool enabled,bool open){
    if(!game||!game->practice_enabled)return;
    open=enabled&&open&&!setup;
    Mobile::update(enabled,open);
    publish();
}
double speed_scale() noexcept{
    return game&&game->practice_enabled&&game->state.phase==GameSessionPhase::stage&&
        !game->state.demo&&!modal()?
        std::min(8.,(1./60.)/speed.interval(game->state.replay,held[17],held[16],held[32])):1.;
}
void sample_keys(u32 bits){
    if(!game||!game->practice_enabled||game->state.phase!=GameSessionPhase::stage)return;
    if(key_monitor_on)key_monitor.record(14,bits);
    if(game->state.stage!=last_sample_stage||game->state.frame<last_sample_frame)lock_running=false;
    last_sample_stage=game->state.stage;last_sample_frame=game->state.frame;
    if(game->practice_tools&PrTime){if(!lock_running)lock_frames=0;++lock_frames;lock_running=true;}
    else lock_running=false;
}
bool captures() noexcept {return game&&game->practice_enabled&&modal();}
bool key(unsigned vk,bool down){
    if(!game||!game->practice_enabled||vk>=held.size())return false;
    const bool was_captured=captures();
    const bool edge=down&&!held[vk];held[vk]=down;pressed[vk]|=edge;
    if(edge&&(vk==90||vk==13)&&!setup&&selected_spell()>=0){
        spell_setup=selected_spell();params={};setup=true;armed=false;pressed[vk]=false;publish();return true;
    }
    if(setup&&vk==88&&!down)held[27]=false;
    const bool special=vk==8||vk==9||vk==123||vk==85||(vk>=112&&vk<=118);
    if(edge&&held[16]&&(advanced||editable())){
        if(vk==70)game->purple_fast_retry=!game->purple_fast_retry;
        else if(vk==68)game->practice_tools^=PrNoBomb;
        else if(vk==83)game->practice_tools^=PrNoShoot;
        else if(vk==65)game->practice_tools^=PrNoFocus;
    }
    if(edge){
        if(vk==123&&!setup&&(on_title()||on_stage())){
            const bool open=advanced||Mobile::expanded[Mobile::Advanced];
            advanced=!open;
            if(open)Mobile::expanded[Mobile::Advanced]=false;
            publish();
        }
        else if(vk==8&&!setup&&on_stage()){cheats=!cheats;publish();}
        else if(vk==9&&on_stage()){
            tracker=!tracker;
            if(Mobile::enabled)Mobile::expanded[Mobile::Information]=tracker;
        }
        else if(setup&&(vk==27||vk==88)){
            if(!setup_popup)finish_setup(false);
            else if(vk==88)held[27]=true;
        }
        else if(vk==27&&(advanced||Mobile::expanded[Mobile::Advanced])){
            advanced=false;Mobile::expanded[Mobile::Advanced]=false;publish();
        }
        else if(editable()&&vk>=112&&vk<=117)game->practice_tools^=1u<<(vk-112);
        else if(editable()&&vk==85)game->practice_tools^=PrEnemy;
        else if(editable()&&vk==118)everlasting_bgm=!everlasting_bgm;
    }
    return special||was_captured||captures();
}
bool pointer(unsigned type,int id,float x,float y,float* restart_x,float* restart_y){
    if(!game||!game->practice_enabled||type>2)return false;
    const float px=x*640,py=y*480;
    if(type==0&&owned_pointer&&id==active_pointer)cancel_pointer();
    using Route=eagler::thprac::PracticePointer::TouchRoute;
    const bool hit=type==0&&pointer_hit(px,py);
    // TH14 receives mobile state through its host export; do not poll the
    // other ports' Module.eaglerControls transport and clear that state.
    const auto* info=type==0&&restart_x&&context&&Mobile::visible&&!modal()&&Mobile::information_window?
        ImGui::FindWindowByID(Mobile::information_window):nullptr;
    const bool passive=info&&info->Active&&!info->Hidden&&info->OuterRectClipped.Contains({px,py})&&
        !ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel);
    const auto decision=touch_routes.route_touch(type,id,hit,passive,px,py);
    if(decision.route==Route::Game)return false;
    if(decision.route==Route::BeginGame){*restart_x=decision.x/640;*restart_y=decision.y/480;return false;}
    if(decision.route==Route::Pending)return true;
    if(decision.route==Route::Tap){
        if(!owned_pointer&&edges.size()<31){edges.push_back({true,decision.x,decision.y});edges.push_back({false,px,py});}
        return true;
    }
    if(type==0){if(owned_pointer)return true;owned_pointer=true;active_pointer=id;}
    if(!owned_pointer||active_pointer!=id)return true;
    if(type==1&&modal()&&!setup&&context&&mouse_down&&!ImGui::IsAnyItemActive()&&
       !ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))scroll_advanced+=py-mouse_y;
    mouse_x=px;mouse_y=py;
    if(type!=1&&edges.size()<32)edges.push_back({type==0,px,py});
    if(type==2)owned_pointer=false;
    return true;
}
void cancel_pointer(){touch_routes.reset();edges.clear();owned_pointer=mouse_down=false;mouse_x=mouse_y=-FLT_MAX;}
void clear_input(){held.fill(false);pressed.fill(false);cancel_pointer();}
bool wheel(float delta){if(!visible())return false;mouse_wheel=std::clamp(mouse_wheel+delta,-10.f,10.f);return true;}
void mouse_motion(float x,float y){if(visible()&&!owned_pointer){mouse_x=x*640;mouse_y=y*480;}}
bool keep_bgm() noexcept{return game&&game->practice_enabled&&everlasting_bgm&&game->practice_run.section&&!game->state.replay&&!game->state.demo;}
void close(){setup=setup_popup=cheats=advanced=tracker=false;Mobile::update(Mobile::enabled,false);clear_input();publish();}
void sync_setup(){
    if(!game||!game->practice_enabled)return;
    const bool active=awaiting_setup()||(setup&&spell_setup>=0&&selected_spell()>=0);
    if(active==setup)return;
    if(active){
        const auto& menu=*game->title;
        spell_setup=-1;
        params={};params.stage=std::clamp(menu.cursor.selected+1,1,6);
        rank=menu.selection.difficulty;shot=menu.selection.partner;mode=1;warp=0;chapter=1;
        Mobile::update(Mobile::enabled,false);cheats=advanced=false;
        edges.clear();owned_pointer=mouse_down=false;pressed.fill(false);
        armed=false;
    }else spell_setup=-1;
    setup=active;setup_popup=false;publish();
}
void draw(GameSession& s,touhou::sdl::Renderer& renderer){
    sync_setup();
    const bool hud=s.practice_enabled&&on_stage()&&(key_monitor_on||show_drop_bar||show_hitboxes||(show_lock_timer&&lock_running));
    if(!s.practice_enabled||(!visible()&&!hud)||!ensure_context())return;
    auto& io=ImGui::GetIO();io.DeltaTime=1.f/60;io.MousePos={mouse_x,mouse_y};
    if(!edges.empty()){auto e=edges.front();edges.pop_front();mouse_down=e.down;io.MousePos={e.x,e.y};}
    io.MouseWheel=mouse_wheel;mouse_wheel=0;
    io.MouseDown[0]=mouse_down;io.KeyCtrl=held[17];io.KeyShift=held[16];io.KeyAlt=held[18];
    for(unsigned i=0;i<held.size();++i)io.KeysDown[i]=held[i];
    ImGui::NewFrame();
    if(setup)practice_options();
    else if(modal())advanced_options();
    else{
        if(cheats||Mobile::visible)cheat_options();
        if(advanced||Mobile::visible)advanced_options();
        if(on_stage()&&key_monitor_on&&(!Mobile::enabled||Mobile::visible)){
            KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);
            if(!Mobile::enabled||Mobile::expanded[Mobile::Keyboard])KeysHUD(14,{1280,0},{840,0},style,true,false);
            if(Mobile::visible){
                ImGui::SetNextWindowPos({420,65},ImGuiCond_Always);ImGui::SetNextWindowSize({0,0});
                Mobile::begin(Mobile::Keyboard,"Keys###th14-mobile-keys",panel_flags|ImGuiWindowFlags_AlwaysAutoResize,locale);ImGui::End();
            }
        }
        if(on_stage()&&show_lock_timer&&lock_running){
            auto* draw=ImGui::GetOverlayDrawList();char seconds[32];
            std::snprintf(seconds,sizeof seconds,"%.2f s",lock_frames/60.);
            draw->AddRectFilled({31,0},{110,20},IM_COL32(255,255,255,255));
            draw->AddText({36,1},IM_COL32(0,0,0,255),seconds);
        }
        if(on_stage()&&show_drop_bar&&game->battle&&game->battle->player)drop_bar();
        if(on_stage()&&show_hitboxes&&game->battle){
            auto* draw=ImGui::GetOverlayDrawList();draw->PushClipRect({32,16},{416,464},true);
            for(auto* b=game->battle->bullets.first_th14();b;b=b->th14_active_next)
                if((b->state==1||b->state==2)&&(b->flags&2)&&!b->collision_delay&&b->hitbox.x>0){
                    const float scale=b->flags&0x40?b->th14_size:1.f;
                    const ImVec2 center{224+b->position.x,16+b->position.y};
                    const u32 color=IM_COL32(255,80,80,255);
                    if(b->flags&0x10)draw->AddCircle(center,b->hitbox.x*scale,color,12);
                    else{
                        const float x=b->hitbox.x*scale*.5f,y=b->hitbox.y*scale*.5f;
                        draw->AddRect({center.x-x,center.y-y},{center.x+x,center.y+y},color);
                    }
                }
            draw->PopClipRect();
        }
    }
    if(on_stage()&&!modal()&&(tracker||Mobile::visible))information();
    publish();
    ImGui::Render();
    renderer.render_imgui(ImGui::GetDrawData(),1);
    pressed.fill(false);
}
}
#endif
