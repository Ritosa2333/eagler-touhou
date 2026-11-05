#include "ThpracUi.hpp"
#if TH_ENABLE_THPRAC
#include "ApplicationState.hpp"
#include "../game/PracticeCatalog.hpp"
#include "../game/SpellCatalog.hpp"
#include "../game/PracticeLicense.hpp"
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <eagler/thprac/PracticeNavigation.hpp>
#include <eagler/thprac/PracticeSdlKeyboard.hpp>
#include <imgui_freetype.h>
#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <deque>
#include <emscripten.h>

namespace th17::sdl::ThpracUi {
namespace {
namespace mobile=eagler::thprac::mobile;
namespace nav=eagler::thprac::navigation;
bool initialized=false,desktop=false,cheats=false,armed=false,was_open=false,drain_navigation=false;
bool down[256]{},edge[256]{};
int locale=0;
float px=-FLT_MAX,py=-FLT_MAX;
bool pointer_down=false,sampled_down=false;
struct PointerEdge {bool down;float x,y;};
std::deque<PointerEdge> pointer_edges;

const char* tr(const char* zh,const char* en,const char* ja){
    return locale==1?en:locale==2?ja:zh;
}
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
    const auto* label=tr("阶段","Phase","形態");
    if(count==4)combo(label,patch.phase,practice_TH_SPELL_PHASE2);
    if(count==5)combo(label,patch.phase,practice_TH17_SPELL_5PHASE);
    if(patch.section!=TH17_ST6_BOSS11||!patch.phase)return;
    if(ImGui::TreeNode(tr("偶像恶魔循环偏移","Idola Diabolus cycle offsets","イドラディアボルスの周期オフセット"))){
        for(i32 i=0;i<patch.phase;++i){
            char name[24];std::snprintf(name,sizeof name,"P%d (frames)",i+2);
            nav::SliderInt(name,&patch.keiki_offsets[i],0,7080);
        }
        if(ImGui::Button(tr("锁时预设","Time-lock preset","時間停止プリセット")))patch.keiki_offsets={1681,3481,5281};
        ImGui::TreePop();
    }
}
void section_options(PracticePatch& patch,i32 rank){
    static i32 warp=0,previous_stage=0;
    if(previous_stage!=patch.stage){previous_stage=patch.stage;warp=0;}
    if(combo(tr("起点类型","Start point","開始地点"),warp,practice_TH_WARP_SELECT)){
        patch.section=patch.phase=0;
        patch.dialogue=patch.keep_drops=false;
    }
    if(warp==0){patch.section=0;return;}
    if(warp==1){
        i32 wave=patch.section>=10000?patch.section%100:1;
        nav::SliderInt(tr("道中波次","Stage wave","道中の区間"),&wave,1,practice_waves[patch.stage-1]);
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
    nav::Combo(practice_TH_WARP_SELECT[warp][locale],&index,names.data(),i32(names.size()));
    const auto& info=*entries[index];
    if(patch.section!=info.id){
        patch.section=info.id;patch.phase=0;
        patch.dialogue=patch.keep_drops=false;
    }
    if(info.dialogue)nav::Checkbox(tr("保留对话","Keep dialogue","会話あり"),&patch.dialogue);
    if(info.drops)nav::Checkbox(tr("保留符卡掉落","Keep spell drops","スペルのドロップを残す"),&patch.keep_drops);
}
void resources(PracticeSettings& settings){
    nav::SliderInt(tr("残机","Lives","残機"),&settings.lives,0,8);
    nav::SliderInt(tr("残机碎片","Life pieces","残機の欠片"),&settings.life_pieces,0,2);
    nav::SliderInt(tr("炸弹","Bombs","ボム"),&settings.bombs,0,8);
    nav::SliderInt(tr("炸弹碎片","Bomb pieces","ボムの欠片"),&settings.bomb_pieces,0,2);
    char power[24];std::snprintf(power,sizeof power,"%.2f",settings.power/100.);
    nav::SliderInt(tr("火力","Power","霊力"),&settings.power,100,400,power);
    nav::DragInt(tr("最大得点","Point value","最大得点"),&settings.value,10,0,999990);
    settings.value=std::clamp(settings.value,0,999990)/10*10;
    nav::DragInt(tr("擦弹","Graze","グレイズ"),&settings.graze,1,0,999999);
    settings.graze=std::clamp(settings.graze,0,999999);
    const ImS64 min=0,max=9999999990LL;
    ImS64 score=settings.score;
    nav::DragScalar(tr("分数","Score","スコア"),ImGuiDataType_S64,&score,10,&min,&max,"%lld");
    settings.score=std::clamp(score,min,max)/10*10;
    for(i32 i=0;i<5;++i){
        char label[48];
        std::snprintf(label,sizeof label,"%s %d",tr("灵体","Spirit","動物霊"),i+1);
        auto& slots=settings.patch.spirits;
        combo(label,slots[i],practice_TH17_GOAST_SELECT);
        if(!slots[i]){std::fill(slots.begin()+i,slots.end(),0);break;}
    }
}
void setup(ApplicationState& app){
    auto& state=app.practice;
    auto& patch=state.settings.patch;
    const bool entering=!was_open;
    was_open=true;
    if(entering)armed=false;
    const bool spell=state.spell>=0;
    const i32 rank=spell?SpellCatalog::rank(state.spell):patch.stage==7?4:app.selection.difficulty;
    bool start=false,cancel=false;
    ImGui::SetNextWindowPos(spell?ImVec2(220,180):ImVec2(164,40),ImGuiCond_Always);
    ImGui::SetNextWindowSize(spell?ImVec2(382,140):ImVec2(460,402),ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.94f);
    if(ImGui::Begin("thprac###th17-practice",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse)){
        start=ImGui::Button(tr("开始练习","Start practice","練習開始"),{135,28});
        ImGui::SameLine();
        cancel=ImGui::Button(tr("返回","Back","戻る"),{90,28});
        ImGui::Separator();
        if(spell){
            const auto* name=app.localized_spell(state.spell);
            const auto& section=practice_sections[(state.spell==100?TH17_ST7_END_S10:TH17_ST6_BOSS11)-1];
            ImGui::TextUnformatted(name?name->c_str():section.names[rank][locale]);
            phase_options(patch,state.spell);
            nav::focus(tr("阶段","Phase","形態"),entering);
        }else{
            const auto* mode=tr("增强练习","Enhanced practice","拡張練習");
            if(!state.standard_available)ImGui::BeginDisabled();
            nav::Checkbox(mode,&state.purple);
            if(!state.standard_available)ImGui::EndDisabled();
            if(state.purple){
                ImGui::PushItemWidth(232);
                i32 stage=patch.stage-1;
                const char* stages[]={"1","2","3","4","5","6","Extra"};
                if(nav::Combo(tr("关卡","Stage","ステージ"),&stage,stages,7)){
                    patch.stage=stage+1;patch.section=patch.phase=0;
                    patch.dialogue=patch.keep_drops=false;
                }
                section_options(patch,patch.stage==7?4:app.selection.difficulty);
                phase_options(patch,-1);
                resources(state.settings);
                ImGui::PopItemWidth();
            }
            nav::focus(mode,entering);
        }
    }
    ImGui::End();
    if(!armed&&!down[90]&&!down[13]&&!down[88]&&!down[27])armed=true;
    if(armed&&!nav::busy()&&!mobile::captures_advanced(state.advanced)){
        start|=edge[90]||edge[13];
        cancel|=edge[88]||edge[27];
    }
    if(cancel){state.menu=false;drain_navigation=true;}
    else if(start){
        if(spell){
            patch.section=0;patch.dialogue=patch.keep_drops=false;patch.spirits={};
        }
        const i32 selected_rank=spell?rank:patch.stage==7?4:app.selection.difficulty;
        if((!spell&&!state.purple)||patch.valid(selected_rank,state.spell)){
            state.confirmed=true;drain_navigation=true;
            state.menu=false;
        }
    }
}
void advanced(ApplicationState& app){
    auto& state=app.practice;
    ImGui::SetNextWindowPos({160,44},ImGuiCond_Always);
    ImGui::SetNextWindowSize({460,388},ImGuiCond_Always);
    if(mobile::begin(mobile::Advanced,"Advanced###th17-practice-advanced",ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse,locale)){
        if(ImGui::CollapsingHeader(tr("游戏速度","Game speed","ゲーム速度"),ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::SliderInt(tr("逻辑帧率","Simulation FPS","論理 FPS"),&state.speed.fps,15,120);
            ImGui::SliderInt(tr("录像慢放（Shift）","Replay slow (Shift)","リプレイ低速 (Shift)"),&state.speed.fps_replay_slow,15,60);
            ImGui::SliderInt(tr("录像快进（U）","Replay fast (U)","リプレイ早送り (U)"),&state.speed.fps_replay_fast,60,120);
        }
        if(ImGui::CollapsingHeader(tr("游戏输入","Game inputs","操作設定"))){
            auto& input=state.input;
            ImGui::Checkbox(tr("禁用炸弹","Disable bomb","ボムを無効化"),&input.disable_xkey);
            ImGui::Checkbox(tr("同时禁用 C","Also disable C","C も無効化"),&input.disable_Ckey_at_same_time);
            ImGui::Checkbox(tr("禁用射击","Disable shot","ショットを無効化"),&input.disable_zkey);
            ImGui::Checkbox(tr("禁用低速","Disable focus","低速を無効化"),&input.disable_shiftkey);
            ImGui::Checkbox(tr("强制低速","Force focus","低速に固定"),&input.force_shiftkey);
            ImGui::Checkbox(tr("自动射击（A 切换）","Auto shot (toggle with A)","自動ショット (A で切替)"),&input.enable_auto_shoot);
        }
        if(ImGui::CollapsingHeader(tr("关于 thprac","About thprac","thprac について"))){
            ImGui::Text("thprac purple %s",practice_version);
            ImGui::TextUnformatted(practice_license);
        }
    }
    ImGui::End();
}
void information(ApplicationState& app){
    const auto& battle=app.scene()->battle;
    ImGui::SetNextWindowPos({428,265},ImGuiCond_Always);
    ImGui::SetNextWindowSize({200,0},ImGuiCond_Always);
    if(mobile::begin(mobile::Information,"Status###th17-practice-info",ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoNav|ImGuiWindowFlags_NoMouseInputs,locale)){
        ImGui::Text("%s / %d",practice_IGI_DIFF[app.progress.difficulty][locale],app.progress.stage);
        ImGui::Text("%s %d",tr("失误","Misses","ミス"),battle.session.deaths);
        ImGui::Text("%s %d",tr("擦弹","Graze","グレイズ"),battle.graze_total);
        ImGui::Text("%s %d",tr("最大得点","Point value","最大得点"),battle.score.point_value/100);
        const auto& roaring=battle.items->roaring.state;
        ImGui::Text("%s %d/5",tr("灵体","Spirits","動物霊"),roaring.count);
        if(roaring.flags&2)ImGui::Text("%s %.2f s",tr("暴走剩余","Roaring","暴走残り"),roaring.countdown.fractional/60.f);
        if(battle.spell.flags&1)ImGui::Text("%s %.2f s",tr("符卡用时","Spell time","スペル経過"),battle.spell_card->active_frames/60.f);
    }
    ImGui::End();
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
    ImGui::GetStyle().ScrollbarSize=20;
    ImFontConfig config;
    config.RasterizerMultiply=1.25f;
    config.OversampleH=config.OversampleV=5;
    io.FontDefault=io.Fonts->AddFontFromFileTTF("/unifont.otf",16,&config,io.Fonts->GetGlyphRangesChineseFull());
    if(!io.FontDefault||!ImGuiFreeType::BuildFontAtlas(io.Fonts,0)){
        ImGui::DestroyContext();
        return app.fail("TH17 thprac could not load Unifont");
    }
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
    armed=drain_navigation=false;
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
    return initialized&&(app.practice.menu||app.practice.confirmed||drain_navigation||mobile::captures_advanced(app.practice.advanced));
}
bool captures_pointer(float x,float y){
    if(!initialized)return false;
    if(ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel))return true;
    for(auto* window:ImGui::GetCurrentContext()->Windows)
        if(window->Active&&!window->Hidden&&!(window->Flags&ImGuiWindowFlags_NoMouseInputs)&&window->OuterRectClipped.Contains({x,y}))return true;
    return false;
}
bool passive_pointer(float x,float y){return initialized&&mobile::passive_pointer(x,y);}
void update_input(ApplicationState& app,const bool* keys){
    if(!initialized)return;
    const u32 bits=u32(EM_ASM_INT({return Module.eaglerControls?.thpracKeyboardBits|0;}));
    for(i32 i=0;i<256;++i){
        const bool pressed=keys[i]||(i==8&&(bits&1))||(i==9&&(bits&256))||
            (i>=112&&i<=118&&(bits&(1u<<(i-111))))||(i==123&&(bits&512))||(i==85&&(bits&1024));
        edge[i]|=pressed&&!down[i];
        down[i]=pressed;
    }
    if(!down[90]&&!down[13]&&!down[88]&&!down[27])drain_navigation=false;
}
bool render(ApplicationState& app){
    if(!initialized)return true;
    auto& state=app.practice;
    if(edge[8]&&!ImGui::IsAnyItemActive()){cheats=!cheats;publish_menu(cheats);}
    mobile::poll();
    if(edge[9]&&app.scene()&&!ImGui::IsAnyItemActive()){
        if(mobile::enabled)mobile::expanded[mobile::Information]=!mobile::expanded[mobile::Information];
        else state.information=!state.information;
    }
    if(edge[123]){
        if(mobile::enabled)mobile::expanded[mobile::Advanced]=!mobile::expanded[mobile::Advanced];
        else state.advanced=!state.advanced;
    }
    if(edge[27]&&mobile::captures_advanced(state.advanced)&&!state.menu){
        if(mobile::enabled)mobile::expanded[mobile::Advanced]=false;
        else state.advanced=false;
        drain_navigation=true;
    }
    const bool live_practice=app.scene()&&state.active&&!app.progress.practice_record;
    if((cheats||mobile::visible)&&live_practice&&!ImGui::IsAnyItemActive()){
        for(i32 i=0;i<8;++i)if(edge[112+i])state.tools^=1u<<i;
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
    nav::prepare(state.menu&&!mobile::captures_advanced(state.advanced));
    ImGui::NewFrame();
    if(state.menu)setup(app);else was_open=false;
    if(mobile::show(mobile::Cheats,cheats)){
        ImGui::SetNextWindowPos({8,8},ImGuiCond_Always);
        if(mobile::begin(mobile::Cheats,"thprac###th17-practice-tools",ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove,locale)){
            if(!live_practice)ImGui::BeginDisabled();
            const auto option=[&](u32 bit,const char* key,const char* title){
                bool selected=state.tools&bit;
                if(mobile::checkbox(key,title,selected))state.tools^=bit;
            };
            option(PracticeInvincible,"F1",tr("自机无敌","Player invincible","自機無敵"));
            option(PracticeInfiniteLives,"F2",tr("无限残机","Infinite lives","残機無限"));
            option(PracticeInfiniteBombs,"F3",tr("无限炸弹","Infinite bombs","ボム無限"));
            option(PracticeInfinitePower,"F4",tr("火力不减","No power loss","パワー減少なし"));
            option(PracticeTimeLock,"F5",tr("锁定 Boss 计时","Lock boss timer","ボスタイマー停止"));
            option(PracticeAutoBomb,"F6",tr("自动决死","Auto deathbomb","オート喰らいボム"));
            option(PracticeInfiniteRoaring,"F7",tr("无限暴走","Infinite Roaring","暴走無限"));
            option(PracticeNoHyper,"F8",tr("固定普通暴走","Neutral Roaring only","通常暴走のみ"));
            option(PracticeEnemyInvincible,"U",tr("敌机无敌","Enemy invincible","敵無敵"));
            ImGui::Checkbox(tr("持续播放分段 BGM","Everlasting section BGM","区間の BGM を継続"),&state.everlasting_bgm);
            if(!live_practice)ImGui::EndDisabled();
            if(!mobile::enabled){
                ImGui::Separator();
                ImGui::Checkbox(tr("高级设置 [F12]","Advanced settings [F12]","詳細設定 [F12]"),&state.advanced);
                ImGui::Checkbox(tr("高级状态信息 [Tab]","Advanced status [Tab]","詳細情報 [Tab]"),&state.information);
            }
        }
        ImGui::End();
    }
    if(mobile::show(mobile::Advanced,state.advanced))advanced(app);
    if(app.scene()&&mobile::show(mobile::Information,state.information))information(app);
    ImGui::Render();
    std::fill(std::begin(edge),std::end(edge),false);
    app.graphics.backend.render_imgui(ImGui::GetDrawData(),GraphicsDevice::screen);
    const auto* failure=app.graphics.backend.error();
    return !*failure||app.fail(failure);
}
}
#endif
