#pragma once
#include "GameResources.hpp"
#include "Combo.hpp"
#include "MatchRules.hpp"
#include "SpriteGeometry.hpp"
namespace th09 {
struct HudFrame {
    FrameTiming timing;PlayfieldGeometry geometry;Vec3 player;
    ComboState combo;ScoreCounter score;
    i32 health=10,spell_level=1,boss_level=1,wins=0;float charge=0,available=0;
    bool versus=false;
};
struct HudPresentation {
    virtual ~HudPresentation()=default;
    virtual void begin_hud(i32 side)=0;
    virtual void hud_animation(AnmVm&,bool rotated)=0;
    virtual void hud_triangle(const AttackColorVertex*)=0;
};
class HeadsUpDisplay {
    GameResources& resources;HudPresentation& output;i32 side;
    void start(u32 slot,i32 script){resources.start(AnimationFile::front,animations[slot],script);}
    void sprite(u32 slot,i32 id){resources.sprite(AnimationFile::front,animations[slot],id);}
    void advance(u32 first,u32 count);
    void draw_range(const HudFrame&,u32 first,u32 count,bool rotated,bool relative=false,bool zero_z=false);
public:
    std::array<AnmVm,63> animations;std::array<AnmVm,2> portraits;
    i32 charge_active=0,wipe_state=0;Timer wipe{0,0,0},blink{0,0,0};
    HeadsUpDisplay(GameResources& r,HudPresentation& p,i32 s):resources(r),output(p),side(s){std::memset(animations.data(),0,sizeof(animations));std::memset(portraits.data(),0,sizeof(portraits));}
    void initialize(bool versus);
    void ready(){start(34,67);}
    void begin_charge();void end_charge();void charge_level(i32);
    void health_notice(i32 kind);void survival_expired(){start(44,78);}
    void begin_survival(i32 frames);void survival_time(i32 frames);
    void portrait(u32 layer,i32 script,AnimationFile resource);
    void close(){wipe_state=1;wipe.reset();}
    void open(){blink.reset();if(wipe_state){wipe_state=2;wipe.reset(30);}}
    void update(const HudFrame&);void draw(const HudFrame&);
};
}
