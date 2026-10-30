#pragma once
#include "PlayerLife.hpp"
#include "DamageSources.hpp"
#include "BombScorePopup.hpp"
#include <memory>
namespace th17 {
struct BombScreenEffect {i32 kind=0,strength=0,rise=0,hold=0,fall=0;};
struct BombTarget {i32 id=0;Vec3 position{};u32 flags=0;};
struct BombHost {
    virtual ~BombHost()=default;
    virtual bool sound(i32,bool queued)=0;
    virtual bool bomb_hud(i32,i32)=0;
    virtual bool screen_effect(const BombScreenEffect&)=0;
    virtual bool cancel_bullets(const Vec3&,float radius,i32 reward,bool honor_protection)=0;
    virtual bool cancel_lasers(const Vec3&,float radius,i32 reward,bool honor_protection)=0;
    virtual bool cancel_bullets_rectangle(const Vec3&,const Vec2&,float angle,i32 reward)=0;
    virtual bool cancel_lasers_rectangle(const Vec3&,const Vec2&,float angle,i32 reward,bool honor_protection)=0;
    virtual bool nearest_target(const Vec3&,float range,BombTarget&)=0;
};
struct BombContext {
    AnmManager& animations;PlayerMotion& motion;Timer& invulnerability;
    PlayerLifeSession& session;PlayerSpellStatus& spell;DamageSources& damage;Rng& random;
    BombHost& world;u32& roaring_flags;i32& enemy_count;
    i32 player_resource=10;bool hud_available=true,hud_collect=false,roaring_available=true,enemy_available=true;
};
class BombController {
protected:
    BombContext& context;float rate=1;
    virtual bool start()=0;virtual bool frame(bool& finished)=0;
    bool fail(const std::string&);
    bool sound(i32 id,bool queued=false);
    bool interrupt(u32 handle,i32 label=1);
    u32 animation(i32 script,const Vec3&);
    void invalidate_spell()noexcept;
    DamageSource* circle(const Vec3&,float radius,float growth,i32 frames,i32 amount,i32* id=nullptr);
    DamageSource* rectangle(const Vec3&,float angle,const Vec2&,i32 frames,i32 amount);
public:
    Vec3 position{};float angle=0;Timer age;u32 main_handle=0,secondary_handle=0,aura_handle=0;
    i32 failed_spell=0;std::string error;
    BombScorePopup score_popup;
    explicit BombController(BombContext& c):context(c){age.set(0);}
    virtual ~BombController()=default;
    bool allowed()const noexcept;bool begin();bool update(float frame_rate);
    virtual bool stop(){return true;}
    virtual i32 hit(DamageSource&,const DamageQuery&){return -1;}
};
}
