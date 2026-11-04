#pragma once
#include "AnmManager.hpp"
#include "EnemyState.hpp"
#include "PlayerLife.hpp"
#include "HudDrawData.hpp"
namespace th17 {
struct HudResources {i32 front=5,ascii=2,logo=11;std::array<i32,2> boss_names{{-1,-1}};};
struct HudStartContext {
    u32 mode_flags=0;i32 screen=0,stage=1,character=0,animal=0,difficulty=1,continues=0;
    bool game_active=true,practice_record=false;
};
struct HudBossGauge {
    float displayed=0,target=0;i32 life=0,reserved=0;
    struct Mark {float fraction=0;u32 color=0;};std::array<Mark,4> marks{};
    std::array<u32,7> handles{};i32 active=0,faded=0;
};
static_assert(sizeof(HudBossGauge)==0x54);
class Hud {
    AnmManager& animations;EnemyWorldState& enemies;PlayerLifeSession& session;PlayerMotion& player;PlayerSpellStatus& spell;HudResources resources;
    bool check(bool,const char*);bool root_interrupt(u32,i32,bool=false);bool visible(u32,bool);AnmVm* find(u32&);
    bool stock(std::array<u32,8>&,i32,i32,bool);bool retire_gauge(HudBossGauge&);
    bool update_clock(float);bool update_gauges();bool update_indicator();
public:
    std::array<u32,8> life_handles{},bomb_handles{};std::array<u32,2> clock_handles{};
    std::array<HudBossGauge,3> gauges{};std::array<u32,10> phase_handles{};
    std::array<u32,8> bonus_digits{};std::array<u32,2> bonus_separators{};u32 result_banner=0,pickup_banner=0,result_panel=0;i32 result_active=0;
    u32 stage_clear_notice=0,chapter_result_notice=0;i32 stage_clear_bonus=0,displayed_score=0,score_increment=0;
    struct ChapterResult {i32 grazes=0;float percentage=0;i32 deaths=0,score=0;float initial_percentage=0;i32 target=0,increment=0,duration=0;} chapter_result;
    u32 base=0,indicator=0,difficulty_banner=0,difficulty_label=0,loadout_label=0,boss_name=0,notice_handle=0;
    u32 flags=0;Timer age,notice_age; i32 phase_count=0,current_seconds=-1,previous_seconds=-1;
    bool dialogue=false,boss_hidden=false;u32 game_flags=0;
    std::function<bool(i32)> sound;std::function<bool()> update_dialogue;std::string error;
    Hud(AnmManager& a,EnemyWorldState& e,PlayerLifeSession& s,PlayerMotion& p,PlayerSpellStatus& b,HudResources r={}):animations(a),enemies(e),session(s),player(p),spell(b),resources(r){age.set(0);}
    bool initialize(const HudStartContext&);bool update(float rate);
    void stage_resources(HudResources value,i32 score)noexcept{resources=value;age.set(0);displayed_score=score;current_seconds=previous_seconds=-1;}
    bool lives(i32 stock,i32 pieces){return this->stock(life_handles,stock,pieces,false);}
    bool bombs(i32 stock,i32 pieces){return this->stock(bomb_handles,stock,pieces,true);}
    bool prepare_spell(bool start);
    bool segment(i32 boss,i32 index,float fraction,u32 color);
    bool segments(i32 count);bool stage_logo(const HudStartContext&);
    bool result(i32 amount,i32 kind);
    bool begin_stage_clear(i32 bonus);bool update_notices(float rate);
    void update_score(i32 score,HudScoreState&)noexcept;
    bool draw(const HudDrawContext&,HudDrawServices&);
    bool ensure_boss_name();
};
}
