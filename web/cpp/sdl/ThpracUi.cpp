#include "ThpracUi.hpp"
#if TH_ENABLE_THPRAC
#include "ApplicationState.hpp"
#include "../game/PracticeCatalog.hpp"
#include "../game/SpellCatalog.hpp"
#include "../game/PracticeLicense.hpp"
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeNavigation.hpp>
#include <eagler/thprac/PracticeSdlKeyboard.hpp>
#include <imgui_freetype.h>
#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <ctime>
#include <deque>
#include <random>
#include <emscripten.h>

namespace th17::sdl::ThpracUi {
namespace {
namespace mobile=eagler::thprac::mobile;
namespace nav=eagler::thprac::navigation;
bool initialized=false,desktop=false,cheats=false,armed=false,was_open=false,drain_navigation=false;
bool down[256]{},edge[256]{};
// Keyboard shortcuts keep the desktop windows available alongside the
// launcher's optional touch panels. Touch command bits retain mobile folding.
u32 keyboard_hotkeys=0;
bool editing=false,keiki_options=false,show_license=false,reset_speed_ui=false;
bool spirit_menu=false,spirit_pointer=false;
PracticeSpiritSpawn spirit_tool;
int locale=0;
float px=-FLT_MAX,py=-FLT_MAX;
bool pointer_down=false,sampled_down=false;
struct PointerEdge {bool down;float x,y;};
std::deque<PointerEdge> pointer_edges;

const char* tr(const char* zh,const char* en,const char* ja){
    return locale==1?en:locale==2?ja:zh;
}
const char* label(const char* const* value){return value[locale];}
eagler::thprac::PracticeKeyMonitor key_monitor;
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter {int64_t QuadPart=0;};
void practice_counter_frequency(PracticeCounter* c){c->QuadPart=1000000000;}
void practice_counter_now(PracticeCounter* c){c->QuadPart=int64_t(SDL_GetTicksNS());}
std::function<unsigned()> practice_random_generator(unsigned minimum,unsigned maximum){
    return std::bind(std::uniform_int_distribution<unsigned>(minimum,maximum),std::mt19937(std::mt19937::result_type(std::time(nullptr))));
}
#include <eagler/thprac/PracticeReaction.inc>
THGuiTestReactionTest reaction_test;
#include <eagler/thprac/PracticeSpeed.inc>

template<size_t N>
bool combo(const char* label,i32& value,const char* const (&items)[N][3]){
    const char* names[N];
    for(size_t i=0;i<N;++i)names[i]=items[i][locale];
    return nav::Combo(label,&value,names,N);
}
void publish_menu(bool open){
    EM_ASM({
        const open=!!$0;
        if(Module.eaglerThpracMenuOpen===open)return;
        Module.eaglerThpracMenuOpen=open;
        window.dispatchEvent(new CustomEvent('eagler-thprac-menu',{detail:{open}}));
    },int(open));
}
void phase_options(PracticePatch& patch,i32 spell){
    const i32 count=practice_phase_count(patch.section,spell);
    patch.phase=std::clamp(patch.phase,0,count-1);
    if(count==4)combo(label(practice_TH_PHASE),patch.phase,practice_TH_SPELL_PHASE2);
    if(count==5)combo(label(practice_TH_PHASE),patch.phase,practice_TH17_SPELL_5PHASE);
}
void keiki_advanced(PracticePatch& patch){
    if(patch.section!=TH17_ST6_BOSS11||!patch.phase)return;
    ImGui::SameLine();
    if(nav::Checkbox(label(practice_TH17_SHOW_ID_OPT),&keiki_options)&&!keiki_options)patch.keiki_offsets={};
    if(!keiki_options)return;
    const char* offsets[]{label(practice_TH17_ID_P2_OPT),label(practice_TH17_ID_P3_OPT),label(practice_TH17_ID_P4_OPT)};
    for(i32 i=0;i<patch.phase;++i)nav::SliderInt(offsets[i],&patch.keiki_offsets[i],0,7080);
    if(ImGui::Button(label(practice_TH17_ID_TD_CFG)))patch.keiki_offsets={1681,3481,5281};
    if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",label(practice_TH17_ID_TD_CFG_HINT));
}
void section_options(PracticePatch& patch,i32 rank){
    static i32 warp=0,previous_stage=0;
    if(previous_stage!=patch.stage){previous_stage=patch.stage;warp=0;}
    if(combo(label(practice_TH_WARP),warp,practice_TH_WARP_SELECT)){
        patch.section=patch.phase=0;
        patch.dialogue=patch.keep_drops=false;
    }
    if(warp==0){patch.section=0;return;}
    if(warp==1){
        constexpr i32 first_half[]{2,2,3,4,4,2,4};
        const i32 first=first_half[patch.stage-1];
        i32 wave=patch.section>=10000?patch.section%100:1;
        char portion[48];
        std::snprintf(portion,sizeof portion,label(wave<=first?practice_TH_STAGE_PORTION_1:practice_TH_STAGE_PORTION_2),
            wave<=first?wave:wave-first);
        nav::SliderInt(label(practice_TH_CHAPTER),&wave,1,practice_waves[patch.stage-1],portion);
        patch.section=10000+patch.stage*100+wave;
        return;
    }
    std::vector<const PracticeSectionInfo*> entries;
    std::vector<const char*> names;
    i32 index=0;
    for(const auto& info:practice_sections){
        if(info.stage!=patch.stage||!*info.names[rank][locale]||
           (warp==2&&info.group!=1)||(warp==3&&info.group!=2)||(warp==4&&info.spell)||(warp==5&&!info.spell))continue;
        if(info.id==patch.section)index=i32(entries.size());
        entries.push_back(&info);
        names.push_back(info.names[rank][locale]);
    }
    if(entries.empty()){
        patch.section=patch.phase=0;
        patch.dialogue=patch.keep_drops=false;
        return;
    }
    nav::Combo(label(practice_TH_WARP_SELECT[warp]),&index,names.data(),i32(names.size()));
    const auto& info=*entries[index];
    if(patch.section!=info.id){
        patch.section=info.id;patch.phase=0;
        patch.dialogue=patch.keep_drops=false;
    }
    if(info.dialogue)nav::Checkbox(label(practice_TH_DLG),&patch.dialogue);
    phase_options(patch,-1);
    if(info.drops){
        if(info.dialogue)ImGui::SameLine();
        nav::Checkbox(label(practice_TH_DROP_ITEMS),&patch.keep_drops);
    }
    keiki_advanced(patch);
}
void resources(PracticeSettings& settings){
    nav::SliderInt(label(practice_TH_LIFE),&settings.lives,0,8);
    nav::SliderInt(label(practice_TH_LIFE_FRAGMENT),&settings.life_pieces,0,2);
    nav::SliderInt(label(practice_TH_BOMB),&settings.bombs,0,8);
    nav::SliderInt(label(practice_TH_BOMB_FRAGMENT),&settings.bomb_pieces,0,2);
    const char* spirit_labels[]{label(practice_TH17_GOAST_1),label(practice_TH17_GOAST_2),
        label(practice_TH17_GOAST_3),label(practice_TH17_GOAST_4),label(practice_TH17_GOAST_5)};
    auto& slots=settings.patch.spirits;
    for(i32 i=0;i<5;++i){
        combo(spirit_labels[i],slots[i],practice_TH17_GOAST_SELECT);
        if(!slots[i]){std::fill(slots.begin()+i,slots.end(),0);break;}
    }
    char power[24];std::snprintf(power,sizeof power,"%.2f",settings.power/100.);
    nav::SliderInt(label(practice_TH_POWER),&settings.power,100,400,power);
    nav::DragInt(label(practice_TH_VALUE),&settings.value,10,0,999990);
    settings.value=std::clamp(settings.value,0,999990)/10*10;
    nav::DragInt(label(practice_TH_GRAZE),&settings.graze,1,0,999999);
    settings.graze=std::clamp(settings.graze,0,999999);
    const ImS64 min=0,max=9999999990LL;
    ImS64 score=settings.score;
    nav::DragScalar(label(practice_TH_SCORE),ImGuiDataType_S64,&score,10,&min,&max,"%lld");
    settings.score=std::clamp(score,min,max)/10*10;
}

constexpr ImGuiWindowFlags fixed_window=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|
    ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove;
void setup(ApplicationState& app){
    auto& state=app.practice;
    auto& patch=state.settings.patch;
    const bool entering=!was_open;
    was_open=true;
    if(entering)armed=editing=false;
    const bool spell=state.spell>=0;
    const ImVec2 sizes[]{{320,388.8f},{384,379.2f},{358.4f,388.8f}};
    const ImVec2 positions[]{{172.8f,86.4f},{137.6f,86.4f},{147.2f,86.4f}};
    const float phase_height=ImGui::GetTextLineHeightWithSpacing()+ImGui::GetFrameHeightWithSpacing()
        +2*ImGui::GetStyle().WindowPadding.y;
    ImGui::SetNextWindowSize(spell?ImVec2(243.2f,phase_height):sizes[locale],ImGuiCond_Always);
    ImGui::SetNextWindowPos(spell?ImVec2(224,216):positions[locale],ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.8f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
    const auto flags=fixed_window|(spell?ImGuiWindowFlags_NoScrollbar:0);
    if(ImGui::Begin("Option###th17-purple-practice",nullptr,flags)){
        float label_width=spell?48.f:locale==2?67.2f:64.f;
        if(!spell){
            const auto reserve=[&](const char* const* text){
                const float width=ImGui::CalcTextSize(label(text)).x+ImGui::GetStyle().ItemInnerSpacing.x;
                label_width=std::max(label_width,width);
            };
            for(const auto& text:practice_TH_WARP_SELECT)reserve(text);
            reserve(practice_TH_LIFE_FRAGMENT);
            reserve(practice_TH_BOMB_FRAGMENT);
            reserve(practice_TH17_ID_P2_OPT);
            reserve(practice_TH17_GOAST_1);
            reserve(practice_TH_VALUE);
        }
        ImGui::PushItemWidth(-label_width);
        ImGui::TextUnformatted(label(spell?practice_TH_SPELL_PRAC:practice_TH_MENU));
        ImGui::Separator();
        if(spell)phase_options(patch,state.spell);
        else{
            const auto stage_available=[&](i32 stage){
                const i32 loadout=app.selection.character*3+app.selection.animal;
                const i32 difficulty=stage==7?4:app.selection.difficulty;
                return app.records.game.title.stage_available[loadout][difficulty][stage-1]!=0;
            };
            const bool original_available=stage_available(patch.stage);
            i32 mode=state.purple?1:0;
            ImGui::BeginDisabled(!original_available);
            combo(label(practice_TH_MODE),mode,practice_TH_MODE_SELECT);
            ImGui::EndDisabled(!original_available);
            i32 stage=patch.stage-1;
            if(combo(label(practice_TH_STAGE),stage,practice_TH_STAGE_SELECT)){
                patch.stage=stage+1;patch.section=patch.phase=0;
                patch.dialogue=patch.keep_drops=false;
            }
            state.purple=mode!=0||!stage_available(patch.stage);
            if(state.purple){
                section_options(patch,patch.stage==7?4:app.selection.difficulty);
                resources(state.settings);
            }
        }
        ImGui::PopItemWidth();
        nav::focus(label(spell?practice_TH_PHASE:practice_TH_STAGE),entering);
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    if(!armed&&!down[90]&&!down[13]&&!down[88]&&!down[27])armed=true;
    const bool busy=editing||nav::busy();
    editing=nav::busy();
    if(!armed||busy||state.advanced||mobile::captures_advanced(false))return;
    if(edge[88]||edge[27]){
        state.menu=false;
        state.cancelled=!spell;
        drain_navigation=true;
        return;
    }
    if(!edge[90]&&!edge[13])return;
    if(spell){
        patch.section=0;patch.dialogue=patch.keep_drops=false;patch.spirits={};
    }
    const i32 rank=spell?SpellCatalog::rank(state.spell):patch.stage==7?4:app.selection.difficulty;
    if((!spell&&!state.purple)||patch.valid(rank,state.spell)){
        state.confirmed=true;
        state.menu=false;
        drain_navigation=true;
    }
}
void input_options(eagler::thprac::PracticeInput& input){
    const auto check=[](const char* name,bool& value,const char* description){
        ImGui::Checkbox(name,&value);
        ImGui::SameLine();ImGui::TextDisabled("(?)");
        if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",description);
    };
    check(label(practice_TH_ADV_DISABLE_X_KEY),input.disable_xkey,label(practice_TH_ADV_DISABLE_X_KEY_DESC));
    ImGui::SameLine();
    check(label(practice_TH_ADV_DISABLE_SHIFT_KEY),input.disable_shiftkey,label(practice_TH_ADV_DISABLE_SHIFT_KEY_DESC));
    ImGui::SameLine();
    check(label(practice_TH_ADV_DISABLE_Z_KEY),input.disable_zkey,label(practice_TH_ADV_DISABLE_Z_KEY_DESC));
    ImGui::Checkbox(label(practice_TH_ADV_DISABLE_C_KEY_SAMETIME),&input.disable_Ckey_at_same_time);
    ImGui::SameLine();
    ImGui::Checkbox(label(practice_TH_ADV_FORCE_SHIFT_KEY),&input.force_shiftkey);
    ImGui::Checkbox(label(practice_THPRAC_AUTO_SHOOT),&input.enable_auto_shoot);
    if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",tr("按 A 切换","Toggle with A","A で切替"));
}
void help(const char* text){
    ImGui::SameLine();ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",text);
}
void key_monitor_options(bool& enabled){
    ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&enabled);
    if(!enabled)return;
    if(!key_monitor.g_record_key_aps){
        if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){
            key_monitor.clear_record();key_monitor.g_record_key_aps=true;
        }
    }else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
    ImGui::SameLine();
    if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT))){
        const auto csv=key_monitor.csv();
        EM_ASM({
            const blob=new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'});
            const url=URL.createObjectURL(blob);const link=document.createElement('a');
            link.href=url;link.download='APS.csv';link.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
        },csv.c_str());
    }
    const auto& aps=key_monitor.g_recorded_aps;
    if(aps.size()>=2)ImGui::PlotLines("##APS",[](void* data,int idx){
        const auto& values=*static_cast<const std::vector<int>*>(data);
        return float(values[(values.size()>600?values.size()-600:0)+idx]);
    },const_cast<std::vector<int>*>(&aps),int(std::min<size_t>(600,aps.size())),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void keiki_phase_tracker(const ApplicationState& app){
    if(!ImGui::CollapsingHeader(label(practice_TH17_KEIKI_FINAL_PHASETRACK)))return;
    const auto* scene=app.scene();
    const auto& patch=app.practice.active;
    std::array<i32,3> offsets{-1,-1,-1};
    if(scene){
        const auto& frames=scene->battle.enemy_world.keiki_phase_frames;
        const auto first=std::find_if(frames.begin(),frames.end(),[](i32 frame){return frame>=0;});
        if(first!=frames.end()){
            const auto phase=first-frames.begin();
            const i32 start=*first-(phase?patch.keiki_offsets[phase-1]:0);
            for(size_t i=0;i<offsets.size();++i)
                if(patch.keiki_offsets[i]>0)offsets[i]=patch.keiki_offsets[i];
                else if(frames[i+1]>=0)offsets[i]=frames[i+1]-start;
        }
    }
    for(size_t i=0;i<offsets.size();++i)ImGui::Text("P%zu: %d",i+1,offsets[i]);
    ImGui::BeginDisabled(offsets[0]<0);
    if(ImGui::Button(label(practice_TH_COPY_CFG_CODE))){
        char code[25];std::snprintf(code,sizeof code,"%08X%08X%08X",u32(offsets[0]),u32(offsets[1]),u32(offsets[2]));
        EM_ASM({navigator.clipboard?.writeText(UTF8ToString($0)).catch(()=>{});},code);
    }
    ImGui::EndDisabled(offsets[0]<0);
    help(label(offsets[0]<0?practice_TH17_KEIKI_FINAL_PHASETRACK_NONE_HINT:practice_TH_COPY_CFG_CODE_HINT));
}
void gameplay_options(ApplicationState& app){
    auto& state=app.practice;
    input_options(state.input);
    key_monitor_options(state.keyboard_monitor);
    ImGui::Checkbox(label(practice_THPRAC_INFLIVES_MAP),&state.life_until_continue);
    ImGui::Checkbox(label(practice_TH_BOSS_FORCE_MOVE_DOWN),&state.lower_boss);
    help(label(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC));
    ImGui::SameLine();ImGui::SetNextItemWidth(180.f);
    if(ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&state.boss_range,.002f,0.f,1.f))
        state.boss_range=std::clamp(state.boss_range,0.f,1.f);
    ImGui::Checkbox(label(practice_TH17_NO_GOAST),&state.no_spirits);
    ImGui::Checkbox(label(practice_TH18_UNCAP),&state.uncap_score);
    ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&state.show_lock_timer);
    ImGui::Checkbox(label(practice_TH_DISABLE_MASTER),&state.hide_master);
    help(label(practice_TH_DISABLE_MASTER_DESC));
    ImGui::Checkbox(label(practice_TH_FACTOR_ACB),&state.all_clear_bonus);
    help(label(practice_TH_FACTOR_ACB_DESC));
    keiki_phase_tracker(app);
    ImGui::Checkbox(label(practice_TH17_GOAST_REPFIX),&state.repair_roaring_replay);
    help(label(practice_TH17_GOAST_REPFIX_DESC));
    if(ImGui::Button(label(practice_TH17_SPAWN_A_GOAST)))spirit_menu=true;
}
void spirit_options(){
    ImGui::TextUnformatted(label(practice_TH17_SPAWN_A_GOAST));ImGui::Separator();
    if(ImGui::Button(label(practice_TH_BACK))){spirit_menu=false;return;}
    combo(label(practice_TH17_GOAST),spirit_tool.kind,practice_TH17_GOAST_SELECT_TOKENS);
    ImGui::BeginDisabled(!spirit_tool.kind);
    ImGui::BeginDisabled(spirit_tool.random);
    ImGui::SliderFloat(label(practice_TH17_GOAST_ANGLE),&spirit_tool.angle,-3.1415927f,3.1415927f);
    ImGui::Checkbox(label(practice_TH17_FORCE_ANGLE),&spirit_tool.force);
    help(label(practice_TH17_FORCE_ANGLE_HELP));
    ImGui::EndDisabled(spirit_tool.random);
    ImGui::Checkbox(label(practice_TH17_RANDOM_ANGLE),&spirit_tool.random);
    const auto point=ImGui::GetCursorScreenPos();
    ImGui::Dummy({160,120});
    const char* preview=label(practice_TH17_ANGLE_PREVIEW);
    const auto center=ImVec2(point.x+80,point.y+60);
    auto* draw=ImGui::GetWindowDrawList();
    const int begin=draw->VtxBuffer.Size;
    const auto extent=ImGui::CalcTextSize(preview);
    draw->AddText({center.x-extent.x*.5f,center.y-extent.y*.5f},ImGui::GetColorU32(ImGuiCol_Text),preview);
    const float angle=spirit_tool.angle,sine=std::sin(angle),cosine=std::cos(angle);
    for(int i=begin;i<draw->VtxBuffer.Size;++i){
        auto& pos=draw->VtxBuffer[i].pos;
        const float x=pos.x-center.x,y=pos.y-center.y;
        pos={center.x+x*cosine-y*sine,center.y+x*sine+y*cosine};
    }
    ImGui::EndDisabled(!spirit_tool.kind);
}
void advanced(ApplicationState& app){
    auto& state=app.practice;
    ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);
    ImGui::SetNextWindowSize({640,480},ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.8f);
    if(state.advanced?ImGui::Begin("Advanced###th17-purple-advanced",nullptr,fixed_window):mobile::begin(mobile::Advanced,"Advanced###th17-purple-advanced",fixed_window,locale)){
        if(spirit_menu){spirit_options();ImGui::End();return;}
        ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();
        ImGui::BeginChild("Adv. Options",{0,0});
        if(ImGui::CollapsingHeader(label(practice_TH_GAME_SPEED)))GameFPSOpt(state.speed,true,&reset_speed_ui,15);
        if(ImGui::CollapsingHeader(label(practice_TH_GAMEPLAY)))gameplay_options(app);
        if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);
        else reaction_test.Reset();
        if(ImGui::CollapsingHeader(label(practice_TH_ABOUT_THPRAC))){
            ImGui::Text(label(practice_TH_ABOUT_VERSION),practice_version);
            ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));
            ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));
            ImGui::NewLine();ImGui::Text(label(practice_TH_ABOUT_THANKS),"You!");ImGui::NewLine();
            if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))
                show_license=!show_license;
            if(show_license){
                ImGui::BeginChild("COPYING",{0,ImGui::GetIO().DisplaySize.y*.8f},true);
                ImGui::TextWrapped("%s",practice_license);
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
void information(ApplicationState& app){
    const auto& battle=app.scene()->battle;
    ImGui::SetNextWindowPos({440,250},ImGuiCond_Always);
    ImGui::SetNextWindowSize({190,0});
    ImGui::SetNextWindowBgAlpha(.9f);
    constexpr auto flags=fixed_window|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoFocusOnAppearing;
    if(app.practice.information?ImGui::Begin("Info###th17-purple-info",nullptr,flags):mobile::begin(mobile::Information,"Info###th17-purple-info",flags,locale)){
        char heading[128];
        std::snprintf(heading,sizeof heading,"%s (%s)",label(practice_IGI_DIFF[app.progress.difficulty]),
            label(practice_IGI_PL_17[app.progress.character*3+app.progress.animal]));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(heading).x)*.5f);
        ImGui::TextUnformatted(heading);
        ImGui::Columns(2);
        const auto row=[](const char* name,const char* format,auto... values){
            ImGui::TextUnformatted(name);ImGui::NextColumn();
            ImGui::Text(format,values...);ImGui::NextColumn();
        };
        row(label(practice_THPRAC_INGAMEINFO_MISS_COUNT),"%7d",battle.session.deaths);
        row(label(practice_TH_GRAZE),"%7d",battle.graze_total);
        row(label(practice_TH_VALUE),"%7d",battle.score.point_value/100);
        const auto& roaring=battle.items->roaring.state;
        row(tr("灵体","Spirits","動物霊"),"%7d/5",roaring.count);
        if(roaring.flags&2)row(tr("咆哮","Roaring","ロアリング"),"%7.2f s",roaring.countdown.fractional/60.f);
        if(battle.spell.flags&1)row(tr("符卡用时","Spell time","スペル経過"),"%7.2f s",battle.spell_card->active_frames/60.f);
        ImGui::Columns(1);
    }
    ImGui::End();
}
bool hotkey(const char* key,const char* text,bool enabled,bool disabled){
    ImGui::BeginDisabled(disabled);
    bool clicked;
    if(mobile::enabled&&!cheats){
        clicked=mobile::checkbox(key,text,enabled);
    }else{
        char name[192];
        std::snprintf(name,sizeof name,enabled?"[%s: %s]":"%s: %s",key,text);
        if(enabled)ImGui::PushStyleColor(ImGuiCol_Text,{0,1,0,1});
        const auto cursor=ImGui::GetCursorPos();
        ImGui::TextUnformatted(name);
        ImGui::SetCursorPos(cursor);
        clicked=ImGui::InvisibleButton(key,ImGui::CalcTextSize(name));
        if(enabled)ImGui::PopStyleColor();
    }
    ImGui::EndDisabled(disabled);
    return clicked;
}
void cheat_overlay(ApplicationState& app,bool live_practice){
    auto& state=app.practice;
    ImGui::SetNextWindowPos({5,5},ImGuiCond_Always);
    ImGui::SetNextWindowSize({0,0});
    ImGui::SetNextWindowBgAlpha(.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{6,5});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4,2});
    constexpr auto flags=fixed_window|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoNav|ImGuiWindowFlags_NoFocusOnAppearing;
    if(cheats?ImGui::Begin("Mod Menu###th17-purple-overlay",nullptr,flags):mobile::begin(mobile::Cheats,"Mod Menu###th17-purple-overlay",flags,locale)){
        ImGui::SetWindowFontScale(.875f);
        const char* labels[]{label(practice_TH_MUTEKI),label(practice_TH_INFLIVES2),label(practice_TH_INFBOMBS),
            label(practice_TH_INFPOWER),label(practice_TH_TIMELOCK),label(practice_TH_AUTOBOMB),
            label(practice_TH17_INF_ROARING),label(practice_TH17_NO_HYPER)};
        for(i32 i=0;i<8;++i){
            char key[8];std::snprintf(key,sizeof key,"F%d",i+1);
            if(hotkey(key,labels[i],state.tools&(1u<<i),!live_practice))state.tools^=1u<<i;
        }
        if(hotkey("F9",label(practice_TH_EL_BGM),state.everlasting_bgm,!live_practice))
            state.everlasting_bgm=!state.everlasting_bgm;
        auto& tracker=mobile::enabled&&!cheats?mobile::expanded[mobile::Information]:state.information;
        if(hotkey("F10 / Tab",label(practice_THPRAC_INGAMEINFO),tracker,!app.scene()))tracker=!tracker;
        if(hotkey("U",label(practice_TH_ENEMY_MUTEKI),state.tools&PracticeEnemyInvincible,!live_practice))
            state.tools^=PracticeEnemyInvincible;
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}
}

bool initialize(ApplicationState& app){
    const int options=EM_ASM_INT({
        const o=Module.eaglerOptions;
        if(o?.thpracEnabled!==true)return 0;
        const locale=String(o.thpracLocale||'');
        return locale.startsWith('ja')?3:locale.startsWith('en')?2:1;
    });
    if(!options)return true;
    locale=options-1;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io=ImGui::GetIO();
    io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags|=ImGuiBackendFlags_HasGamepad;
    io.DisplaySize={640,480};
    io.IniFilename=nullptr;
    const int codes[]{9,37,39,38,40,33,34,36,35,45,46,8,32,13,27,13,65,67,86,88,89,90};
    for(int i=0;i<ImGuiKey_COUNT;++i)io.KeyMap[i]=codes[i];
    ImGui::StyleColorsDark();
    ImFontConfig config;
    config.RasterizerMultiply=1.25f;
    config.OversampleH=config.OversampleV=5;
    io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,io.Fonts->GetGlyphRangesChineseFull());
    if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){
        ImGui::DestroyContext();
        return app.fail("TH17 thprac could not load Unifont");
    }
    reset_speed_ui=true;show_license=keiki_options=false;
    initialized=app.practice.enabled=true;
    app.practice.input.shoot_key_DIK=65;
    return true;
}
void cancel_pointer(){
    pointer_edges.clear();
    pointer_down=sampled_down=false;
    px=py=-FLT_MAX;
}
void reset_input(){
    std::fill(std::begin(down),std::end(down),false);
    std::fill(std::begin(edge),std::end(edge),false);
    keyboard_hotkeys=0;
    armed=drain_navigation=editing=false;
    cancel_pointer();
    if(!initialized)return;
    auto& io=ImGui::GetIO();
    std::fill(std::begin(io.KeysDown),std::end(io.KeysDown),false);
    std::fill(std::begin(io.MouseDown),std::end(io.MouseDown),false);
    io.InputQueueCharacters.clear();
    io.MouseWheel=io.MouseWheelH=0;
}
void shutdown(){
    reset_input();
    if(initialized)ImGui::DestroyContext();
    initialized=desktop=cheats=was_open=false;
    key_monitor={};reaction_test.Reset();
    spirit_tool={};spirit_menu=spirit_pointer=false;
    mobile::reset();
    publish_menu(false);
}
void mouse(int type,float x,float y){
    if(!initialized)return;
    px=x;py=y;
    if(type==0)return;
    const bool pressed=type==1;
    if(pressed==pointer_down)return;
    if(pointer_edges.size()>=64)cancel_pointer();
    pointer_edges.push_back({pressed,x,y});
    pointer_down=pressed;
}
void process_event(const SDL_Event& event,float scale){
    if(!initialized)return;
    if(event.type==SDL_EVENT_MOUSE_MOTION&&event.motion.which!=SDL_TOUCH_MOUSEID){
        desktop=true;mouse(0,event.motion.x/scale,event.motion.y/scale);
    }else if((event.type==SDL_EVENT_MOUSE_BUTTON_DOWN||event.type==SDL_EVENT_MOUSE_BUTTON_UP)&&
             event.button.which!=SDL_TOUCH_MOUSEID&&event.button.button==SDL_BUTTON_LEFT){
        desktop=true;
        mouse(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN?1:2,event.button.x/scale,event.button.y/scale);
    }else if(event.type==SDL_EVENT_MOUSE_WHEEL){
        ImGui::GetIO().MouseWheel+=event.wheel.y;
    }
}
bool captures_game_input(const ApplicationState& app){
    return initialized&&(app.practice.menu||app.practice.confirmed||drain_navigation||app.practice.advanced||mobile::captures_advanced(false));
}
bool captures_pointer(float x,float y){
    if(!initialized)return false;
    if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
    for(auto* window:ImGui::GetCurrentContext()->Windows)
        if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
    return false;
}
bool passive_pointer(float x,float y){
    return initialized&&(mobile::passive_pointer(x,y)||(spirit_pointer&&x>32&&x<416&&y>144&&y<464));
}
void update_input(ApplicationState& app,const bool* keys){
    if(!initialized)return;
    const u32 bits=u32(EM_ASM_INT({return Module.eaglerControls?.thpracKeyboardBits|0;}));
    for(i32 i=0;i<256;++i){
        if(keys[i]&&!down[i])keyboard_hotkeys|=i==8?1u:(i==9||i==121)?2u:i==123?4u:0u;
        const bool pressed=keys[i]||(i==8&&(bits&1))||(i==9&&(bits&256))||
            (i>=112&&i<=118&&(bits&(1u<<(i-111))))||(i==123&&(bits&512))||(i==85&&(bits&1024));
        edge[i]|=pressed&&!down[i];
        down[i]=pressed;
    }
    if(!down[90]&&!down[13]&&!down[88]&&!down[27])drain_navigation=false;
}
void record_keys(const ApplicationState& app,Uint32 held){
    if(initialized&&app.practice.keyboard_monitor)key_monitor.record(17,held);
}
bool render(ApplicationState& app){
    if(!initialized)return true;
    auto& state=app.practice;
    mobile::poll();
    if(edge[8]&&!ImGui::IsAnyItemActive()){
        if(!mobile::enabled||(keyboard_hotkeys&1)){cheats=!cheats;publish_menu(cheats);}
        else mobile::expanded[mobile::Cheats]=!mobile::expanded[mobile::Cheats];
    }
    if((edge[9]||((cheats||mobile::visible)&&edge[121]))&&app.scene()&&!ImGui::IsAnyItemActive()){
        if(mobile::enabled&&!(keyboard_hotkeys&2))mobile::expanded[mobile::Information]=!mobile::expanded[mobile::Information];
        else state.information=!state.information;
    }
    if(edge[123]){
        if(mobile::enabled&&!(keyboard_hotkeys&4))mobile::expanded[mobile::Advanced]=!mobile::expanded[mobile::Advanced];
        else state.advanced=!state.advanced;
    }
    if(edge[27]&&(state.advanced||mobile::captures_advanced(false))&&!state.menu){
        state.advanced=false;
        mobile::expanded[mobile::Advanced]=false;
        drain_navigation=true;
    }
    const bool live_practice=app.scene()&&state.active&&!app.progress.practice_record;
    if((cheats||mobile::visible)&&live_practice&&!ImGui::IsAnyItemActive()){
        for(i32 i=0;i<8;++i)if(edge[112+i])state.tools^=1u<<i;
        if(edge[120])state.everlasting_bgm=!state.everlasting_bgm;
        if(edge[85])state.tools^=PracticeEnemyInvincible;
    }
    auto& io=ImGui::GetIO();
    io.DeltaTime=1.f/60;
    io.MousePos={px,py};
    if(!pointer_edges.empty()){
        const auto event=pointer_edges.front();
        pointer_edges.pop_front();
        sampled_down=event.down;
        io.MousePos={event.x,event.y};
    }
    io.MouseDown[0]=sampled_down;
    io.KeyCtrl=down[17];io.KeyShift=down[16];io.KeyAlt=down[18];
    io.ConfigDragClickToInputText=desktop;
    std::copy(std::begin(down),std::end(down),std::begin(io.KeysDown));
    eagler::thprac::sdl_keyboard_frame(io);
    io.NavInputs[ImGuiNavInput_DpadUp]=down[38];io.NavInputs[ImGuiNavInput_DpadDown]=down[40];
    io.NavInputs[ImGuiNavInput_DpadLeft]=down[37];io.NavInputs[ImGuiNavInput_DpadRight]=down[39];
    io.NavInputs[ImGuiNavInput_Activate]=down[90]||down[13];io.NavInputs[ImGuiNavInput_Cancel]=down[88]||down[27];
    const bool advanced_open=state.advanced||mobile::captures_advanced(false);
    if(advanced_open)std::fill(std::begin(io.NavInputs),std::end(io.NavInputs),0.f);
    nav::prepare(state.menu&&!advanced_open);
    ImGui::NewFrame();
    spirit_pointer=live_practice&&spirit_tool.kind&&!advanced_open&&!state.menu&&
        !(app.progress.scene_flags&0x4004)&&!app.scene()->battle.dialogue_present();
    if(spirit_pointer&&app.run_input&&ImGui::IsMouseClicked(ImGuiMouseButton_Left)&&!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
        auto spawn=spirit_tool;spawn.x=io.MousePos.x-224;spawn.y=io.MousePos.y-16;spawn.force&=!spawn.random;
        app.run_input->spawn_spirit(spawn);
    }
    if(state.menu)setup(app);else was_open=false;
    if(state.advanced||mobile::show(mobile::Advanced,false))advanced(app);
    if(cheats||mobile::show(mobile::Cheats,false))cheat_overlay(app,live_practice);
    if(app.scene()&&(state.information||mobile::show(mobile::Information,false)))information(app);
    if(app.scene()&&state.keyboard_monitor&&mobile::draw_tool(mobile::Keyboard,"Keys###th17-mobile-keys",{420,65},locale)){
        KeyRectStyle style;style.text_color_press=style.text_color_release=IM_COL32(32,32,32,255);
        KeysHUD(17,{1280,0},{840,0},style,true,false);
    }
    const bool lock_timer=app.scene()&&app.run_input&&state.show_lock_timer&&
        (app.scene()->battle.session.practice_tools&PracticeTimeLock);
    const bool auto_shoot=app.scene()&&state.input.enable_auto_shoot&&state.input.is_auto_shooting;
    if((lock_timer||auto_shoot)&&mobile::draw_tool(mobile::Indicators,"Tools###th17-mobile-tools",{120,65},locale)){
        auto* draw=ImGui::GetOverlayDrawList();
        if(auto_shoot){
            draw->AddRectFilled({0,0},{16,16},0xffffdddd);
            draw->AddText({4,0},0xffcc2222,"A");
        }
        if(lock_timer){
            char value[32];std::snprintf(value,sizeof value,"%.2f",app.run_input->lock_frames/60.f);
            const auto size=ImGui::CalcTextSize(value);
            draw->AddRectFilled({32,0},{110,size.y},0xffffffff);
            draw->AddText({110-size.x,0},0xff000000,value);
        }
    }
    ImGui::Render();
    std::fill(std::begin(edge),std::end(edge),false);
    keyboard_hotkeys=0;
    app.graphics.backend.render_imgui(ImGui::GetDrawData(),GraphicsDevice::screen);
    const auto* failure=app.graphics.backend.error();
    return !*failure||app.fail(failure);
}
}
#endif
