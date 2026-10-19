#pragma once
#include "EclVm.hpp"
namespace th09 {
struct EnemySpawn {
    i32 script=0;Vec3 position;i32 life=0,item=0,score=0,mirrored=0,character_program=0;
};
struct EnemyTimelineActions {
    virtual ~EnemyTimelineActions()=default;
    virtual EclVm* spawn(const EnemySpawn&)=0;
    virtual EclVm* boss(i32 index)=0;
    virtual i32* timeline_events()=0;
    virtual void finish_match()=0;
};
class EnemyTimeline {
    EclProgram* program=nullptr;
    u32 cursor=0,end=0;
public:
    Timer time,wait;i32 mirrored=0;bool completed=true,invalid=false;
    bool start(EclProgram&,u32 index,i32 mirror=0);
    i32 instruction_offset() const noexcept{return completed?-1:i32(cursor);}
    // Returns one when the timeline ends, zero while running, minus one on
    // invalid resource/binding input. Game scripts are executed directly.
    i32 step(const FrameTiming&,float frame_step,u8 difficulty_mask,i32 dialogue,Rng&,EnemyTimelineActions&);
};
}
