#pragma once
#include "PlayerLife.hpp"
#include "PlayerBounds.hpp"
#include "ScreenShake.hpp"
namespace th15 {
struct BombEnemyState {i32 uses=0,chain=0;bool available=false;};
struct BombHost {
    virtual ~BombHost()=default;
    virtual bool sound(i32,bool queued)=0;virtual bool bomb_hud(i32,i32)=0;
    virtual bool cancel_bullets(const Vec3&,float radius,i32 reward)=0;virtual bool cancel_lasers(const Vec3&,float radius,i32 reward,bool)=0;
    virtual bool shake(const ScreenShakeSpec&)=0;
    virtual bool nudge(const ScreenNudgeSpec&)=0;
    virtual bool cancel_bullets_rectangle(const Vec3&,const Vec2&,float angle,i32 reward)=0;
    virtual bool cancel_lasers_rectangle(const Vec3&,const Vec2&,float angle,i32 reward,bool)=0;
};
struct BombContext {AnmManager& animations;PlayerMotion& motion;PlayerLife& life;PlayerBounds& bounds;ShtHeader& header;PlayerLifeSession& session;PlayerSpellStatus& spell;BombEnemyState& enemies;BombHost& world;i32 player_resource=0;bool hud_available=true,hud_collect=false;};
class BombController {
    friend class BombCheckpoint;
protected:
    BombContext& context;float frame_rate=1;
    virtual bool start()=0;virtual bool frame(bool& finished)=0;
    virtual bool stop_active(){return true;}
    void invalidate_spell()noexcept;
public:
    Vec3 position{};float angle=0;Timer age,secondary_age;bool effective_against_spell=false;std::string error;
    explicit BombController(BombContext& c):context(c){age.set(0);}
    virtual ~BombController()=default;
    bool allowed()const noexcept;bool begin();bool update(float rate);bool stop(){return context.session.bomb_state==0||stop_active();}
};
}
