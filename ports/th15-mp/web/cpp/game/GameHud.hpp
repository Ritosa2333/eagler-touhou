#pragma once
#include "AnmManager.hpp"
#include "SessionState.hpp"
#include "BossHud.hpp"
#include "HudFrameData.hpp"
#include "HudDrawData.hpp"
namespace th15 {
struct HudEntryContext {i32 scene_destination=0;};
// The HUD owns logical animation handles. Text/number drawing is a separate
// presentation pass; no device objects or native GUI structure are retained.
class GameHud {
 AnmManager& animations;SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;i32 front,ascii,logo;
 bool check(bool);u32 create(i32 bank,i32 script,u32 ordering=0);u32 widget(i32 script);bool direct_interrupt(u32,i32);
 bool cue(u32,i32,bool immediate=false);bool retire_boss(u32,bool);
public:
 std::array<u32,8> life_icons{},bomb_icons{};std::array<u32,2> boss_icons{};
 u32 root=0,pointdevice=0,background_notice=0,mode_notice=0,chapter_notice=0,difficulty_notice=0,difficulty_label=0,retry_intro=0;
 std::array<u32,10> bonus_digits{};std::array<u32,2> result_banners{};
 u32 flags=0;Timer intro_age{0,0,0,0,0};BossHud boss;
 std::array<HudBossAnimations,3> boss_animations{};std::array<u32,10> boss_stars{};
 u32 boss_banner=0,result_notice=0;Timer frame_age{0,0,0,0,0};HudResultCounter result;i32 last_countdown=-1;
 i32 displayed_score=0,score_increment=0,clear_bonus=0,result_grazes=0,result_deaths=0;
 i32 boss_state=0,spell_state=0,tutorial_state=0,collection_state=0;std::string error;
 GameHud(AnmManager& a,SessionState& p,PlayerLifeSession& s,ItemScoreState& v,i32 front,i32 ascii,i32 logo):animations(a),progress(p),player(s),score(v),front(front),ascii(ascii),logo(logo){frame_age.set(0);}
    bool initialize(const HudEntryContext&);bool life(i32 lives,i32 pieces);bool bombs(i32 bombs,i32 pieces);bool clear_intro();
    bool stage_logo(i32 scene_destination);
    bool name_banner(const std::array<i32,2>&);
    void stage_resource(i32 logo_resource)noexcept{logo=logo_resource;}
    bool update_before_dialogue(const HudFrameContext&,HudFrameServices&);bool update_after_dialogue(const HudFrameContext&);
    void update_score();bool draw(const HudDrawContext&,HudDrawServices&);
    bool stage_clear();bool prepare_spell(bool);bool reset_for_retry();
    void prepare_stage_assets()noexcept{frame_age.set(0);displayed_score=score.score;last_countdown=-1;}
    bool notification(i32 value,i32 kind);
};
}
