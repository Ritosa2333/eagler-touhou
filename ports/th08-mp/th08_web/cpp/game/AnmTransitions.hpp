#pragma once
#include "AnmLayout.hpp"
namespace th08 {
inline void begin_animation_transition(AnmVm& vm,u32 channel,i32 duration,u8 mode){vm.interpCurrentTimers[channel].set(0);vm.interpEndTimers[channel].set(duration);vm.interpModes[channel]=mode;}
inline void animation_position_transition(AnmVm& vm,i32 duration,u8 mode,const Vec3& first,const Vec3& last){begin_animation_transition(vm,AnmInterp_Pos,duration,mode);vm.posInitial=first;vm.posFinal=last;}
inline void animation_scale_transition(AnmVm& vm,i32 duration,u8 mode,const Vec2& first,const Vec2& last){begin_animation_transition(vm,AnmInterp_Scale,duration,mode);vm.scaleInitial=first;vm.scaleFinal=last;}
inline void animation_alpha_transition(AnmVm& vm,i32 duration,u8 mode,u8 first,u8 last){begin_animation_transition(vm,AnmInterp_Alpha1,duration,mode);vm.color1Initial.a=first;vm.color1Final.a=last;}
inline void animation_rgb_transition(AnmVm& vm,i32 duration,u8 mode,u32 first,u32 last){begin_animation_transition(vm,AnmInterp_RGB1,duration,mode);vm.color1Initial.r=first>>16;vm.color1Initial.g=first>>8;vm.color1Initial.b=first;vm.color1Final.r=last>>16;vm.color1Final.g=last>>8;vm.color1Final.b=last;}
}
