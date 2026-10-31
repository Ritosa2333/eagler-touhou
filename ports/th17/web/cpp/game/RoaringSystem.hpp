#pragma once
#include "AnimalSpirits.hpp"
#include "DamageSources.hpp"
#include "PlayerLife.hpp"
namespace th17 {
struct RoaringState {
    i32 count=0;std::array<i32,5> inventory{};i32 reserve_count=0;std::array<i32,5> reserve{};
    Timer reward_age{0,0,0,0,0},countdown{0,0,0,0,0};i32 duration=0,kind=0,extension=0;u32 flags=0;
};
static_assert(sizeof(RoaringState)==104);
struct RoaringVisuals {
    u32 hud=0,outer_timer=0,inner_timer=0,mode_effect=0,break_effect=0,animal_effect=0,reward_effect=0;
    i32 damage_source=0;std::array<u32,3> otter_orbs{};i32 hit_delay=0;
};
static_assert(sizeof(RoaringVisuals)==48);
struct RoaringHost {
    virtual ~RoaringHost()=default;
    virtual bool sound(i32,bool queued)=0;
    virtual bool item(i32,const Vec3&,float angle,float speed,i32 delay,i32 sound)=0;
    virtual bool spirit(i32,const Vec3&,float angle)=0;
    virtual bool cancel_bullets(const Vec3&,float radius,i32 reward)=0;
    virtual bool cancel_lasers(const Vec3&,float radius,i32 reward,bool honor)=0;
    virtual bool active_player_shot()const=0;
};
class RoaringSystem {
    AnmManager& animations;Rng& random;DamageSources& damage;PlayerMotion& player;PlayerLifeSession& session;Timer& invulnerability;PlayerSpellStatus& spell;RoaringHost& host;i32 resource;
    bool display_slot(i32);bool activate();bool create_orbs();bool scale_timers();bool update_rewards(float rate);bool finish();
    bool item_reward(i32,Vec3& primary,Vec3& scatter);
    bool interrupt(u32 handle,i32 label,bool immediate=false);
    bool create(u32& handle,i32 script,const Vec3& position={});
public:
    RoaringState state;RoaringVisuals visuals;i32& break_count; i32& break_chain;std::string error;
    RoaringSystem(AnmManager& a,Rng& r,DamageSources& d,PlayerMotion& p,PlayerLifeSession& s,Timer& t,PlayerSpellStatus& sp,RoaringHost& h,i32 id):animations(a),random(r),damage(d),player(p),session(s),invulnerability(t),spell(sp),host(h),resource(id),break_count(s.roaring_breaks),break_chain(s.enemy_chain){}
    ~RoaringSystem();
    bool initialize_hud();
    bool enter_stage();
    bool collect(i32 kind);
    bool break_roaring(bool death);
    bool reward_items();
    i32 otter_hit()noexcept{visuals.hit_delay=20;return -1;}
    bool update(float rate,bool hud_collect,u32 held);
};
}
