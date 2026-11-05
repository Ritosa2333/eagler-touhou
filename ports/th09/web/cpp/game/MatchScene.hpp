#pragma once
#include "StageSelection.hpp"
#include "Dialogue.hpp"
namespace th09 {
enum class OverlayResource {front,ascii};
enum class SceneTransition {title,next_stage,ending,game_over,match_complete};
struct ScenePlayerSummary {i32 health=0,best_combo=0,spells=0,bosses=0,counters=0;float rounds_lost=0;Vec3 position;};
struct MatchSceneServices {
    u32 game_flags=0;i32 wins_required=1,wins[2]{},starting_extra_lives=0,opponent_character=0,music_track=-1,cpu_level=0;
    StageRoute route;ScenePlayerSummary players[2];PlayfieldGeometry geometry[2];
    virtual ~MatchSceneServices()=default;
    virtual void start_animation(AnmVm&,OverlayResource,i32 script)=0;
    virtual void set_sprite(AnmVm&,i32 sprite)=0;
    virtual void advance_animation(AnmVm&)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void begin_view(i32 side)=0;
    virtual void outline(const AttackColorVertex*,u32 segments)=0;
    virtual void number(const Vec3&,i32 width,i32 value)=0;
    virtual void clock(const Vec3&,i32 stage,i32 minutes,i32 seconds)=0;
    virtual i32 dialogue_state()const=0;
    virtual void update_dialogue()=0;
    virtual void draw_dialogue()=0;
    virtual void begin_dialogue(i32 script,i32 flip)=0;
    virtual void victory_dialogue(i32 side)=0;
    virtual void play_sound(i32 sound,i32 pan)=0;
    virtual void reset_attack_timer(i32 side)=0;
    virtual void clear_round_hazards(i32 side)=0;
    virtual void flush_combo(i32 side)=0;
    virtual void background_transition(i32 side,i32 state,i32 frames)=0;
    virtual void fade(i32 type,i32 duration,u32 color,i32 side)=0;
    virtual void fade_hud(i32 side)=0;
    virtual void restart_round()=0;
    virtual void transition(SceneTransition)=0;
    virtual void record_defeat(i32 character)=0;
};
class MatchScene {
    MatchRules& rules;MatchSceneServices& services;
    void place_draw(AnmVm&,i32 side);
public:
    // Frame, arrows, music, end-round/result labels, stage banner.
    std::array<AnmVm,19> animations;
    std::array<std::array<AttackColorVertex,5>,2> borders;
    u32 flash_colors[2]{};i32 flash_frames[2]{},displayed_music=0,phase=0,ending_frames=0,winner=0,result[7]{},retry=0,rewards_blocked=0;
    MatchScene(MatchRules& r,MatchSceneServices& s):rules(r),services(s){for(auto& a:animations)std::memset(&a,0,sizeof(a));}
    void initialize();
    void reset_round();
    void end_round(i32 winning_side);
    void show_results();
    void boss_position(i32 side,const Vec3&);
    void update();
    void draw();
};
}
