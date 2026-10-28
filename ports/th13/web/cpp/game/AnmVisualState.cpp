// 角度归一 see Types.hpp normalize_angle（th13 统一实现，fmod 双精度路径；
// 原版 0x472f30 为 33 次循环减 2π 版，常规角度域内两者等价）。
#include "AnmVisualState.hpp"
namespace th13 {
// op304 层写入（00462450.c:1479-1491）：层号取 u8；3..18 置 0x40000，否则清之。
// th15 的 20-23 → 0x80000 与 0x600000 屏幕层段是 th15 专属，th13 无此分支。
void AnmVisualState::set_layer(i32 value)noexcept{
    layer=value;
    if(value>2&&value<0x13)render_flags|=0x40000;
    else render_flags&=~0x40000u;
}
void AnmVisualState::advance(Vec3& rotation,float rate)noexcept{
    if(!(render_flags&0x1000000))return;
    float* angle[]={&rotation.x,&rotation.y,&rotation.z};const float velocity[]={angular_velocity.x,angular_velocity.y,angular_velocity.z};
    for(u32 i=0;i<3;i++)if(velocity[i]!=0){*angle[i]=normalize_angle(float(float(velocity[i]*rate)+*angle[i]));flags|=4;}
    if(scale_velocity.y!=0){scale.y=float(float(scale_velocity.y*rate)+scale.y);flags|=8;}
    if(scale_velocity.x!=0){scale.x=float(float(scale_velocity.x*rate)+scale.x);flags|=8;}
    float* uv[]={&uv_offset.x,&uv_offset.y};const float speeds[]={uv_velocity.x,uv_velocity.y};
    for(u32 i=0;i<2;i++)if(speeds[i]!=0){float value=float(float(speeds[i]*rate)+*uv[i]);if(value>=2)value=float(value-2);else if(value<0)value=float(value+2);*uv[i]=value;}
}
}
