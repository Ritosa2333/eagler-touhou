#include "PlayerLife.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../th08_web/cpp/multiplayer/CooperativeResources.hpp"
#endif
#include <cassert>
#include <cstdio>

using namespace th08;

struct LifeActions final:PlayerLifeActions {
    PlayerLifeContext& context;
    i32 drops[11]{},rectangle_clears=0;
    explicit LifeActions(PlayerLifeContext& context):context(context){}
    void update_integrity()override{}
    void dissolve()override{}
    void effect(i32,const Vec3&,i32,u32)override{}
    EffectState* fixed_effect(i32,const Vec3&,i32,u32)override{return nullptr;}
    void sound(i32,float)override{}
    void cancel_item_homing()override{}
    void cancel_rectangle(const Vec3&,float width,float height,i32 value,i32 lifetime)override{
        assert(width==768&&height==896&&value==-1&&lifetime==0);++rectangle_clears;
    }
    void reset_screen_color()override{}
    void fail_spell()override{}
    void add_deaths(i32)override{}
    void add_time_orbs(i32 value)override{context.time_orbs+=value;}
    void set_power(i32 value)override{context.power=value;}
    void add_power(i32 value)override{context.power+=value;}
    void set_bombs(i32 value)override{context.bombs=value;}
    void add_lives(i32 value)override{context.lives+=value;}
    void subtract_rank(i32)override{}
    void item(i32 type,const Vec3&,i32 mode)override{assert(type>=0&&type<11&&mode==2);++drops[type];}
    void animation(i32)override{}
};

static void ordinary_death(i32 initial_bombs,i32 old_bombs,i32 power,u8 character){
    PlayerLifeState state{};PlayerLifeContext context{};PlayerMovementState movement{};AnmVm animation{};
    context.lives=2;context.power=power;context.bombs=old_bombs;context.character=character;
    LifeActions actions(context);PlayerLife life(state,context,movement,animation,actions);
    ShotProfile profile{};profile.initial_bombs=float(initial_bombs);profile.deathbomb_limit=6;
    state.state=2;state.predead_count=1;
    assert(!life.resolve_death(profile));
    assert(context.power==(power>16?power-16:0));
    assert(actions.drops[2]==1&&actions.drops[0]==5&&actions.drops[4]==0);
    assert(actions.drops[3]==((old_bombs>0&&(character==2||character==8||character==9))?1:0));
    assert(context.lives==2);
    // The animation phase cannot repeat the resource drop.
    assert(!life.resolve_death(profile));assert(actions.drops[2]==1&&actions.drops[0]==5);
    state.timer.set(30);
    assert(life.resolve_death(profile));assert(context.lives==1&&state.state==1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    assert(context.bombs==2);
#else
    assert(context.bombs==initial_bombs);
#endif
    assert((context.hud_flags&15u)==10u);
    state.timer.set(30);life.respawn(profile);assert(state.state==3&&state.timer.current==240&&state.clear_frames==60);
    for(int i=0;i<60;++i)life.update_invincibility({});
    assert(actions.rectangle_clears==60&&state.clear_frames==0&&state.state==3);
    for(int i=0;i<180;++i)life.update_invincibility({});
    assert(state.state==0&&state.timer.current==0);
}

static void final_death(){
    PlayerLifeState state{};PlayerLifeContext context{};PlayerMovementState movement{};AnmVm animation{};
    context.lives=0;context.power=128;context.bombs=7;context.character=2;
    LifeActions actions(context);PlayerLife life(state,context,movement,animation,actions);
    ShotProfile profile{};profile.initial_bombs=3;
    state.state=2;state.predead_count=1;
    assert(!life.resolve_death(profile));
    assert(context.power==0&&context.lives==0);
    assert(actions.drops[4]==5&&actions.drops[0]==0&&actions.drops[2]==0&&actions.drops[3]==0);
    state.timer.set(30);assert(!life.resolve_death(profile));assert(context.game_over==1);
    assert(actions.drops[4]==5);
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
static void initial_and_reset_resources(){
    for(int count:{2,3})for(int lives:{0,2,6})for(int power:{0,64,128}){
        PilotResources banks[3]{};
        for(int seat=0;seat<count;++seat){
            auto& bank=banks[seat];bank.bombs=8;bank.lives=8;bank.power=128;bank.deaths=3;
            multiplayer::begin_base_life(bank,float(lives),float(power));
            assert(bank.bombs==2&&bank.lives==lives&&bank.power==power&&bank.deaths==3);
            multiplayer::begin_base_life(bank,float(lives),0);
            assert(bank.bombs==2&&bank.lives==lives&&bank.power==0&&bank.deaths==3);
            bank.bombs=5;bank.power=float(power);
            multiplayer::begin_next_stage_life(bank,false);
            assert(bank.bombs==5&&bank.lives==lives&&bank.power==power);
            multiplayer::begin_next_stage_life(bank,true);
            assert(bank.bombs==2&&bank.lives==lives&&bank.power==power);
        }
        // A viewer does not allocate or reset a fourth participant resource.
        if(count==2)assert(banks[2].bombs==0&&banks[2].lives==0&&banks[2].power==0);
    }
}
#endif
int main(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    initial_and_reset_resources();
#endif
    for(i32 initial:{2,3,4})for(i32 bombs:{0,1,8})for(i32 power:{0,16,17,128})
        for(u8 character:{u8(0),u8(2),u8(8),u8(9)})ordinary_death(initial,bombs,power,character);
    final_death();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::puts("player-life-rules MP: PASS (144 death cases plus final death)");
#else
    std::puts("player-life-rules SP: PASS (144 unchanged death cases plus final death)");
#endif
}
