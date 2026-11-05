#pragma once
#include "EnemyManager.hpp"
namespace th09 {
struct EclEffectRequest {i32 type=0;Vec3 position,velocity;i32 count=0;u32 color=0;bool explicit_velocity=false;};
struct EclSceneActions {
    virtual ~EclSceneActions()=default;
    virtual void play_positioned_sound(i32,float x)=0;
    virtual void effect(const EclEffectRequest&)=0;
    virtual void boss_indicator(i32 slot,i16 state)=0;
    virtual void boss_indicator_position(i32 slot,const Vec3&)=0;
    virtual void release_attached_effects(EclVm&)=0;
    virtual void score_popup(const Vec3&,i32 amount,u32 color)=0;
    virtual void scene_setting(i32)=0;
    virtual void add_script_extra_time(i32 frames)=0;
    virtual void end_attack(u32 attack_kind)=0;
};
class EclGameOperations:public EclGameCommands {
    EnemyManager& manager;EclSceneActions& scene;
    EclVm* boss(i32 id){return id>=0&&u32(id)<manager.bosses.size()?manager.bosses[id]:nullptr;}
public:
    EclGameOperations(EnemyManager& m,EclSceneActions& s):manager(m),scene(s){}
    bool execute(EclVm&,EclInstruction&) override;
    i32 cancel_enemies(i32 maximum_score,i32 accumulated=0);
};
}
