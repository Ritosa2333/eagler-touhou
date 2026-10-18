#pragma once
#include "EnemyTimeline.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct AttackPlayerState {i32 levels[2]{},spell_count=0,boss_count=0,boss_counters=0,portrait_script=0;};
struct AttackControllerServices {
    FrameTiming timing;PlayfieldGeometry geometry[2];u32 game_flags=0;i32 reward_time=0;
    AttackPlayerState players[2];
    virtual ~AttackControllerServices()=default;
    virtual EclVm* boss(i32 side)=0;
    virtual void spawn_attack_enemy(i32 side,i32 script,i32 life,i32 score)=0;
    virtual void start_animation(AnmVm&,i32 script)=0;
    virtual void set_sprite(AnmVm&,bool ascii_resource,i32 sprite)=0;
    virtual bool advance_animation(AnmVm&)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void draw_text(AnmVm&,const char*,u32 color,u32 shadow)=0;
    virtual void play_sound(i32 id,i32 pan)=0;
    virtual void background_transition(i32 side,i32 state,i32 frames)=0;
    virtual void reset_background(i32 side)=0;
    virtual void boss_background(i32 side)=0;
    virtual void portrait(i32 side,u32 layer,i32 script)=0;
    virtual void begin_draw(i32 side)=0;
};
// Controls attacks arriving on one field. Character ECL supplies the attack's
// pattern; this manager owns its lifetime, level changes, banner and freeze.
class AttackController {
    AttackControllerServices& services;
    void show_name();
public:
    i32 side=0;std::array<char,128> name{};Timer time{0,0,0};i32 notices[2]{};
    std::array<AnmVm,7> animations;
    i32 parameters[2][3]{};i32 active_variants[2]{-1,-1};
    AttackController(i32 field,AttackControllerServices& s):services(s),side(field){for(auto& a:animations)std::memset(&a,0,sizeof(a));}
    bool begin(i32 variant,i32 level,i32 parameter,const char* name);
    void notify_pattern(u32 kind);
    void update();
    void draw();
    void clear()noexcept{active_variants[0]=active_variants[1]=-1;}
};
}
