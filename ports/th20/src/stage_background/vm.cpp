#include "vm.hpp"
#include "fog.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include <bit>
#include <cstring>
namespace th20::source::background {
namespace n=th20::recovered;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
sprite::Vec3 vector(const float* v){return {v[0],v[1],v[2]};}
FogState fog(const ScriptState& s){FogState result;std::memcpy(&result,s.camera.final_state,sizeof(result));return result;}
template<class T>void configure(sprite::Interpolation<T>& p,int duration,int mode,T start,T end){p.duration=duration;p.mode=mode;p.start=start;p.end=end;n::timer_set(p.timer,0);}
void interpolate(ScriptState& s,const float* rate){
    if(s.direction_interpolation.duration)sprite::sample_animation_interpolation(&s.direction_interpolation,3,false,false,s.camera.vectors[1],rate);
    if(s.position_interpolation.duration)sprite::sample_animation_interpolation(&s.position_interpolation,3,false,false,s.camera.vectors[0],rate);
    if(s.fog_interpolation.duration){const auto result=sample_fog(s.fog_interpolation,rate);std::memcpy(s.camera.final_state,&result,sizeof(result));}
    if(s.up_interpolation.duration)sprite::sample_animation_interpolation(&s.up_interpolation,3,false,false,s.camera.vectors[2],rate);
    if(s.fov_interpolation.duration)sprite::sample_animation_interpolation(&s.fov_interpolation,1,false,false,&s.camera.field_of_view,rate);
    update_camera_motion(s,rate);
}
}
void execute_script(ScriptState& s,const float* rate){
    for(;;){
        const auto& instruction=*reinterpret_cast<const Instruction*>(reinterpret_cast<const std::uint8_t*>(s.owner->instructions)+s.instruction_offset);
        if(s.timer.current<instruction.time){n::timer_tick(s.timer,rate);interpolate(s,rate);return;}
        const auto* args=reinterpret_cast<const std::uint32_t*>(&instruction+1);
        auto integer=[&](unsigned i){return std::bit_cast<std::int32_t>(args[i]);};
        auto real=[&](unsigned i){return std::bit_cast<float>(args[i]);};
        auto vec=[&](unsigned i){return sprite::Vec3{real(i),real(i+1),real(i+2)};};
        switch(instruction.opcode){
        case 0:interpolate(s,rate);return;
        case 1:n::timer_set(s.timer,integer(1));s.instruction_offset=args[0];continue;
        case 2:
            for(unsigned i=0;i<3;++i){const float old=s.camera.vectors[0][i];s.camera.vectors[0][i]=real(i);s.camera.final_vector[i]=sub(real(i),old);}break;
        case 3:configure(s.position_interpolation,integer(0),integer(1),vector(s.camera.vectors[0]),vec(2));break;
        case 4:for(unsigned i=0;i<3;++i)s.camera.vectors[1][i]=real(i);break;
        case 5:configure(s.direction_interpolation,integer(0),integer(1),vector(s.camera.vectors[1]),vec(2));break;
        case 6:for(unsigned i=0;i<3;++i)s.camera.vectors[2][i]=real(i);break;
        case 7:s.camera.field_of_view=real(0);break;
        case 8:{const auto result=make_fog(real(1),real(2),float(args[0]&255),float((args[0]>>8)&255),float((args[0]>>16)&255),float(args[0]>>24));std::memcpy(s.camera.final_state,&result,sizeof(result));break;}
        case 9:configure(s.fog_interpolation,integer(0),integer(1),fog(s),make_fog(real(3),real(4),float(args[2]&255),float((args[2]>>8)&255),float((args[2]>>16)&255),float(args[2]>>24)));break;
        case 10:case 11:{auto& p=instruction.opcode==10?s.position_interpolation:s.direction_interpolation;
            p.duration=integer(0);p.mode=8;p.start=vector(s.camera.vectors[instruction.opcode==10?0:1]);p.tangent_start=vec(2);p.end=vec(5);p.tangent_end=vec(8);n::timer_set(p.timer,0);break;}
        case 12:s.camera_motion=static_cast<std::uint8_t>(args[0]);if(!s.camera_motion)for(float& value:s.camera.vectors[5])value=0.f;
            n::timer_set(s.camera_motion==6?s.secondary_motion_timer:s.motion_timer,0);if(s.camera_motion==2)n::timer_set(s.motion_timer,512);break;
        case 13:vm_environment::set_clear_color(args[0]);break;
        case 14:{auto& animation=s.animations[integer(0)];if(integer(1)>=0)vm_environment::assign_animation(s,integer(0),integer(1));
            else if(integer(1)==-2)animation.base.flags[0]&=~0x10000u;
            else if(integer(1)==-1){animation.base.fields_10_28[6]=0xffffffff;animation.base.flags[0]&=~0x10000u;}
            s.fields_3294[integer(0)]=args[2];break;}
        case 17:s.mesh_mode=args[0];vm_environment::reset_meshes(s);break;
        case 18:configure(s.up_interpolation,integer(0),integer(1),vector(s.camera.vectors[2]),vec(2));break;
        case 19:vm_environment::interrupt_animations(*s.owner,args[0]+7u);break;
        case 20:s.fields_3294[8]=std::bit_cast<std::uint32_t>(n::mul32(real(0),real(0)));break;
        case 21:configure(s.fov_interpolation,integer(0),integer(1),s.camera.field_of_view,real(2));break;
        case 22:vm_environment::set_primary_rotation_flag(args[0]);break;
        }
        s.instruction_offset+=static_cast<std::uint32_t>(static_cast<std::int32_t>(instruction.size));
    }
}
}
