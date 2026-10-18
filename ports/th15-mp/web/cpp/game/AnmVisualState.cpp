#include "AnmVisualState.hpp"
namespace th15 {
float normalize_angle(float angle)noexcept{
    i32 iterations=0;
    while(!(angle<=3.1415927410125732421875f)){
        angle=float(angle-6.283185482025146484375f);
        if(iterations++>=33)break;
    }
    while(!(-3.1415927410125732421875f<=angle)){
        angle=float(angle+6.283185482025146484375f);
        if(iterations++>=33)break;
    }
    return angle;
}
void AnmVisualState::set_layer(i32 value)noexcept{
    layer=value;
    if(value>=3&&value<=19)render_flags=(render_flags&~0x80000u)|0x40000;
    else if(value>=20&&value<=23)render_flags=(render_flags&~0x40000u)|0x80000;
    else render_flags&=~0xc0000u;
    if((value>=20&&value<31)||(value>=35&&value<42))render_flags=(render_flags&~0x600000u)|0x100000;
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
