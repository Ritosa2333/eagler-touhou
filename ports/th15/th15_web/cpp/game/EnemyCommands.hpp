#pragma once
#include "EnemyState.hpp"
#include "BulletCommands.hpp"
#include "EclContext.hpp"
#include "EnemySpawn.hpp"
#include "EnemyVisualCommands.hpp"
#include "LaserCommands.hpp"
#include "StageFog.hpp"
#include "SpellCard.hpp"
#include "ScreenShake.hpp"
namespace th15 {
struct EnemyCommandHost {
    struct EffectRequest {i32 resource=0,script=0;Vec3 position{};float rotation=0;u32 ordering=2;};
    virtual ~EnemyCommandHost()=default;
    virtual bool pause_animation(u32 handle,bool paused)=0;
    virtual bool sound(i32 id,const Vec3& position)=0;
    virtual bool cancel_bullets_circle(const Vec3&,float,i32,bool){return false;}
    virtual bool cancel_lasers_circle(const Vec3&,float,i32,bool){return false;}
    virtual bool cancel_bullets_rectangle(const Vec3&,const Vec2&,float,i32){return false;}
    virtual bool background_fog(i32 duration,i32 mode,const StageFog&){return false;}
    virtual bool background_interrupt(i32){return false;}
    virtual bool cancel_all_bullets(i32){return false;}
    virtual bool cancel_all_lasers(i32,bool){return false;}
    virtual bool clear_lasers_with_rewards(){return false;}
    virtual bool begin_spell(const SpellStartRequest&){return false;}
    virtual bool finish_spell(){return false;}
    virtual bool scene_message(i32){return false;}
    virtual bool message_complete(bool&){return false;}
    virtual bool nudge(const ScreenNudgeSpec&){return false;}
    virtual bool boss_segment(i32 boss,i32 index,float fraction,u32 color){return false;}
    virtual bool boss_segments(i32){return false;}
    virtual bool stage_logo(){return false;}
    virtual bool drop_items(EnemyState&){return false;}
    virtual bool effect(const EffectRequest&){return false;}
    virtual bool retire_distortion(EnemyState&){return false;}
    virtual bool begin_distortion(EnemyState&){return false;}
};
class EnemyCommands final:public EclCommands {
    EnemyState& enemy;EnemyWorldState& world;
public:
    BulletCommands bullets;
    LaserCommands lasers;
    EnemyVisualCommands visuals;
    EnemyCommandHost* host=nullptr;
    EnemySpawnHost* spawning=nullptr;
    EnemyCommands(EnemyState& enemy,EnemyWorldState& world):enemy(enemy),world(world),bullets(enemy,world),lasers(enemy),visuals(enemy){}
    int execute(EclContext& context,u16 opcode)override;
};
}
