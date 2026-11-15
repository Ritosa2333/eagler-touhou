#pragma once
#include "../../cpp/game/EnemyVisualCommands.hpp"
struct CaptureEnemyVisual:EnemyVisualHost {
    std::array<AnmVm,16> instances;std::vector<i32> events;u32 next=0;
    AnmVm* find(u32 handle)override{return handle&&handle<=16?&instances[handle-1]:nullptr;}
    bool create(u32& handle,i32 resource,i32 script,i32 layer)override{handle=next++%16+1;events.insert(events.end(),{1,i32(handle),resource,script,layer});return true;}
    bool retire(u32& handle)override{auto* vm=find(handle);if(vm&&!(vm->visual.render_flags&0x4000000))vm->visual.render_flags=(vm->visual.render_flags&~64u)|32;events.insert(events.end(),{2,i32(handle),0,0,0});handle=0;return true;}
    bool pause(u32 handle)override{auto* vm=find(handle);if(vm)vm->visual.flags&=~2u;events.insert(events.end(),{3,i32(handle),0,0,0});return true;}
    bool interrupt(u32 handle,i32 label)override{auto* vm=find(handle);if(vm)vm->pending_interrupt=label;events.insert(events.end(),{4,i32(handle),label,0,0});return true;}
    bool rebind(u32& handle,i32 script)override{events.insert(events.end(),{5,i32(handle),script,0,0});return true;}
    bool size(u32 handle,Vec2& out)const noexcept override{if(!handle||handle>16)return false;const auto& visual=instances[handle-1].visual;out={float(visual.sprite_size.y*visual.scale.y),float(visual.sprite_size.x*visual.scale.x)};return true;}
    bool change_direction(u32& handle,i32 resource,i32 script,i32 layer)override{return retire(handle)&&create(handle,resource,script,layer);}
};
