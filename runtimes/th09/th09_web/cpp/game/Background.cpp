#include "Background.hpp"
#include "GraphicsMath.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
namespace {float ease(float t,i32 mode){const float q=1-t;switch(mode){case 1:return 1-q*q;case 2:return 1-q*q*q;case 3:return 1-q*q*q*q;case 4:return t*t;case 5:return t*t*t;case 6:return t*t*t*t;default:return t;}}float mix(float a,float b,float t){return (b-a)*t+a;}float constant(u32 v){float f;std::memcpy(&f,&v,4);return f;}}
bool Background::load(const u8* data,u32 size,i32 background){
    if(!resource.load(data,size))return false;primitives.resize(resource.animation_count);
    for(auto& object:resource.objects)for(const auto& p:object.primitives)services.start_animation(primitives[p.animation],false,p.script);
    script_time.reset();instruction=0;position={};fog_duration=0;boss_parameter=0;boss_count=2;fog={200,500,0xff000000};sway=0;
    for(u32 n=0;n<4;++n){durations[n]=0;interpolation_times[n].reset();}requested_label=0;distance_squared=constant(0x49de7920);
    if(background==5||background==9)distance_squared=constant(0x4a3aeb90);else if(background==11)distance_squared=constant(0x4a804008);else if(background==12||background==15)distance_squared=constant(0x4ab72e88);
    boss_state=0;return true;
}
void Background::jump_label(){
    if(!requested_label)return;instruction=0;
    for(u32 n=0;n<resource.instructions.size();++n){const auto& i=resource.instructions[n];if(i.time==-1)return;if(i.opcode==31&&i.integer(0)==requested_label){instruction=n+1;script_time.reset(i.time);requested_label=0;return;}}
}
float Background::interpolation(u32 n){
    float t;if(interpolation_times[n].current>=durations[n]){interpolation_times[n].reset(durations[n]);t=1;durations[n]=0;}else{interpolation_times[n].tick(services.timing);t=interpolation_times[n].value()/float(durations[n]);}return ease(t,interpolation_modes[n]);
}
void Background::interpolate_camera(){
    for(u32 n=0;n<3;++n)if(durations[n]){float t=interpolation(n);Vec3& out=n==0?camera.position:n==1?camera.direction:camera.up;
        for(u32 axis=0;axis<3;++axis){float *o=&out.x;const float *a=&starts[n].x,*b=&goals[n].x,*c=&end_tangents[n].x,*d=&start_tangents[n].x;o[axis]=interpolation_modes[n]==7?float(hermite(a[axis],b[axis],c[axis],d[axis],t)):mix(a[axis],b[axis],t);}}
    if(durations[3])camera.field_of_view=mix(fov_start,fov_goal,interpolation(3));
    camera.unit_direction=GraphicsMath::normalize(camera.direction);
    if(sway>=1&&sway<=4){const float multiplier=sway<=2?constant(0x3c567750):sway==3?constant(0x3aab92a6):constant(0x3b490fdb);
        const float angle=interpolation_times[4].value()*multiplier-constant(0x40490fdb);const double s=std::sin(double(angle));
        if(sway==1)camera.offset.x=float(s*40.f);else if(sway==2){camera.offset.x=float(70.f*s);camera.up.x=float(s*-.1f);}else if(sway==3){camera.up.x=float(s);camera.up.z=float(std::cos(double(angle)));}else{camera.offset.x=float(s*30.f);camera.offset.z=float(std::cos(double(angle))*30.f);camera.up.x=float(s)*-.1f;}
        interpolation_times[4].tick(services.timing);const i32 period=sway<=2?480:sway==3?4800:2048;if(interpolation_times[4].current>=period)interpolation_times[4].reset();
    }
    if(fog_duration){fog_time.tick(services.timing);float t=std::min(fog_time.value()/float(fog_duration),1.f);fog.color=0;
        for(u32 shift=0;shift<32;shift+=8){const float a=float(u8(fog_start.color>>shift)),b=float(u8(fog_goal.color>>shift));fog.color|=u32(u8(i32(mix(a,b,t))))<<shift;}
        fog.near_plane=mix(fog_start.near_plane,fog_goal.near_plane,t);fog.far_plane=mix(fog_start.far_plane,fog_goal.far_plane,t);if(fog_time.current>=fog_duration)fog_duration=0;
    }
}
void Background::execute_script(){
    u32 budget=0;for(;budget<4096;++budget){if(instruction<0||u32(instruction)>=resource.instructions.size()){invalid=true;break;}const auto& i=resource.instructions[instruction];if(i.time==-1||script_time.current<i.time)break;
        const u32 op=u16(i.opcode);const Vec3 v=i.vector();const i32 a=i.integer(0),b=i.integer(1);
        if(op==0){position=previous_position=v;previous_position_time=i.time;if(u32(instruction+1)>=resource.instructions.size()){invalid=true;break;}const auto& next=resource.instructions[instruction+1];next_position_time=next.time;next_position=next.vector();}
        else if(op==1){fog={i.real(1),i.real(2),i.arguments[0]};fog_goal=fog;}
        else if(op==2){fog_start=fog;fog_duration=a;fog_time.reset();}
        else if(op==3){if(!requested_label)break;requested_label=0;}
        else if(op==4){instruction=a;script_time.reset(b);durations[0]=0;jumped=1;break;}
        else if(op>=5&&op<=10){const u32 n=(op-5)/2;if(op&1){if(op==5)jumped=0;starts[n]=goals[n];goals[n]=v;if(!durations[n])(n==0?camera.position:n==1?camera.direction:camera.up)=v;}else{durations[n]=a;interpolation_times[n].reset();interpolation_modes[n]=b;}}
        else if(op==11){fov_start=fov_goal;fov_goal=i.real(0);if(!durations[3])camera.field_of_view=fov_goal;}
        else if(op==12){durations[3]=a;interpolation_times[3].reset();interpolation_modes[3]=b;}
        else if(op==13)clear_color=i.arguments[0];
        else if(op>=14&&op<=28){const u32 n=(op-14)/5,part=(op-14)%5;switch(part){case 0:starts[n]=v;break;case 1:goals[n]=v;break;case 2:end_tangents[n]=v;break;case 3:start_tangents[n]=v;break;case 4:durations[n]=a;interpolation_times[n].reset();interpolation_modes[n]=7;break;}}
        else if(op==29||op==30||op==34){const u32 n=op==29?0:op==30?1:2;if(a<0)overlays[op==30?0:n].activeSpriteIndex=-1;else services.start_animation(overlays[n],false,a);}
        else if(op==32)camera.offset=v;
        else if(op==33){sway=u8(a);durations[4]=0;interpolation_times[4].reset();interpolation_modes[4]=0;}
        ++instruction;
    }if(budget==4096)invalid=true;interpolate_camera();
}
void Background::update(u32 flags,u32 field_flags){
    if(transition_state){--transition_frames;if(transition_frames<1)transition_state=transition_frames=0;else if(transition_state==1)tint=0x80f00000;else if(transition_state==2)tint=0x80303040;}
    if(flags&0x1800)return;if(field_flags&1){tint=0x40303030;return;}if(resource.instructions.empty())return;
    jump_label();execute_script();if(instruction>=0&&u32(instruction)<resource.instructions.size()&&resource.instructions[instruction].opcode!=3)script_time.tick(services.timing);
    for(auto& o:resource.objects)if(o.flags&1){u32 active=0;for(const auto& p:o.primitives){auto& vm=primitives[p.animation];services.advance_animation(vm);if(vm.currentInstruction)++active;}if(!active)o.flags&=0xfe;}
    if(boss_state>0){if(boss_frames==60)++boss_state;++boss_frames;for(i32 n=0;n<boss_count&&n<32;++n)services.advance_animation(boss_animations[n]);}
    for(u32 n=0;n<3;++n)if(overlays[n].activeSpriteIndex>0){services.advance_animation(overlays[n]);if(n==2)clear_color=u32(overlays[2].color1.d3dColor);}
    ++frame;
}
}
