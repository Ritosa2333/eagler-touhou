#pragma once
#include "AnmExecutor.hpp"
namespace th09 {
struct EclVm;struct EclInstruction;
struct EnemyAnimationActions {
    virtual ~EnemyAnimationActions()=default;
    virtual bool start_animation(EclVm&,u32 slot,bool character_resource,i32 script)=0;
};
struct EnemyAnimation {
    AnmVm layers[3];
    i16 idle=0,left=-1,right=0,stop_left=0,stop_right=0,death=0;i8 pose=0;
    bool command(EclVm&,const EclInstruction&,EnemyAnimationActions&);
    bool update_pose(EclVm&,EnemyAnimationActions&);
};
}
