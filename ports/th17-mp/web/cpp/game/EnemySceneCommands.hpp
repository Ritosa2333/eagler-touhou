#pragma once
#include "EnemyAttributeCommands.hpp"
namespace th17 {
struct SpellStartRequest {i32 id=0,time=0,mode=0;std::string name;};
struct EnemyEffectRequest {i32 resource=0,script=0;Vec3 position{};u32 ordering=2;};
struct EnemySceneHost {
    virtual ~EnemySceneHost()=default;
    virtual bool nudge(i32 duration,i32 first,i32 last){return false;}
    virtual bool scene_message(i32){return false;}
    virtual bool message_complete(bool&){return false;}
    virtual bool cancel_all_bullets(i32){return false;}
    virtual bool cancel_all_lasers(i32,bool honor){return false;}
    virtual bool cancel_bullets_circle(const Vec3&,float radius,i32 reward,bool honor){return false;}
    virtual bool cancel_lasers_circle(const Vec3&,float radius,i32 reward,bool honor){return false;}
    virtual bool clear_lasers_with_rewards(){return false;}
    virtual bool begin_spell(const SpellStartRequest&){return false;}
    virtual bool finish_spell(){return false;}
    virtual bool chapter(i32){return false;}
    virtual bool boss_segment(i32 boss,i32 index,float fraction,u32 color){return false;}
    virtual bool boss_segments(i32){return false;}
    virtual bool stage_logo(){return false;}
    virtual bool tracked_effect(i32 script,const Vec3&){return false;}
    virtual bool effect(const EnemyEffectRequest&){return false;}
    virtual bool immediate_update(EnemyState&,EnemyUpdateRule){return false;}
    virtual bool retire_distortion(EnemyState&){return false;}
    virtual bool begin_distortion(EnemyState&){return false;}
    virtual bool drop_items(EnemyState&){return false;}
    virtual bool background_interrupt(i32){return false;}
    virtual bool bullet_bounds(const Vec2&){return false;}
};
class EnemySceneCommands final:public EclCommands {
    EnemyState& enemy;EnemyWorldState& world;
public:
    EnemySceneHost* host=nullptr;EnemySpawnHost* spawning=nullptr;
    EnemySceneCommands(EnemyState& e,EnemyWorldState& w):enemy(e),world(w){}
    static bool handles(u16 op)noexcept;
    int execute(EclContext&,u16)override;
};
}
