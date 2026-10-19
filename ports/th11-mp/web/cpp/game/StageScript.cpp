#include "Stage.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
namespace {
Vec3 vector(const StageInstruction& command,u32 index){return {command.argument<float>(index),command.argument<float>(index+1),command.argument<float>(index+2)};}
void interpolate(Vec3Interpolator& target,Vec3 current,const StageInstruction& command,const float* rate,bool hermite){
    target.duration=command.argument<i32>(0);target.mode=hermite?InterpolationMode::Hermite:command.argument<InterpolationMode>(1);
    std::memcpy(target.start,&current,12);const auto end=vector(command,hermite?5:2);std::memcpy(target.end,&end,12);
    if(hermite){const auto initial=vector(command,2),final=vector(command,8);std::memcpy(target.initial_tangent,&initial,12);std::memcpy(target.final_tangent,&final,12);}
    target.timer.set(0,rate);
}
}
// 404470. Stop does not advance the script timer, but active camera/fog
// interpolations continue. Jump targets are byte offsets in the STD stream.
bool stage_script(StageState& s,const StageResource& file,const float* rate,StageScriptWorld& world){
    bool stopped=false;u32 operations=0;
    for(;;){
        const auto* p=file.instruction(s.instruction_offset);if(!p)return false;
        const auto& command=*p;if(command.time<0)return false;if(command.time>s.script_timer.current)break;
        if(++operations>100000)return false;
        switch(command.opcode){
        case 0:stopped=true;break;
        case 1:s.script_timer.set(command.argument<i32>(1),rate);s.instruction_offset=command.argument<u32>(0);continue;
        case 2:{const auto previous=s.camera.position;s.camera.position=vector(command,0);s.camera.animation_delta={float(double(s.camera.position.x)-previous.x),float(double(s.camera.position.y)-previous.y),float(double(s.camera.position.z)-previous.z)};break;}
        case 3:interpolate(s.position_interpolation,s.camera.position,command,rate,false);world.stage_camera_position_interpolated();break;
        case 4:s.camera.direction=vector(command,0);break;
        case 5:interpolate(s.direction_interpolation,s.camera.direction,command,rate,false);break;
        case 6:s.camera.up=vector(command,0);break;
        case 7:s.camera.fov=command.argument<float>(0);break;
        case 8:s.camera.fog.set(command.argument<u32>(0),command.argument<float>(1),command.argument<float>(2));break;
        case 9:{auto& f=s.fog_interpolation;f.duration=command.argument<i32>(0);f.mode=command.argument<InterpolationMode>(1);f.start=s.camera.fog;f.end.set(command.argument<u32>(2),command.argument<float>(3),command.argument<float>(4));f.timer.set(0,rate);break;}
        case 10:interpolate(s.position_interpolation,s.camera.position,command,rate,true);break;
        case 11:interpolate(s.direction_interpolation,s.camera.direction,command,rate,true);break;
        case 12:s.camera_effect=u8(command.argument<u32>(0));if(!s.camera_effect){s.camera.eye_offset={};s.camera.up={0,1,0};}s.effect_timer.set(0,rate);break;
        case 13:world.stage_color(command.argument<u32>(0));break;
        case 14:{const i32 index=command.argument<i32>(0),script=command.argument<i32>(1);if(index<0||index>=8)return false;auto& vm=s.script_animations[index];if(script<0)vm.flags&=~1u;else if(!world.stage_animation(vm,script))return false;break;}
        case 17:s.deformation_target=112;s.deformation_radius=192;s.deformation_color=0xffffffff;s.phase_x=s.phase_y=normalize_angle(0);s.deformation_mode=command.argument<i32>(0);if(!world.stage_deformation(s.deformation_mode))return false;break;
        default:break; // Original skips labels (16) and unassigned opcodes.
        }
        if(stopped)break;s.instruction_offset+=u32(command.length);
    }
    if(!stopped)s.script_timer.tick();
    if(s.direction_interpolation.duration)s.camera.direction=sample(s.direction_interpolation,rate);
    if(s.position_interpolation.duration)s.camera.position=sample(s.position_interpolation,rate);
    if(s.fog_interpolation.duration)s.camera.fog=sample(s.fog_interpolation,rate);
    const u32 kind=s.camera_effect;if(kind!=1&&kind!=5&&kind!=6&&kind!=7)return true;
    constexpr double pi=3.1415927410125732421875;const i32 period=kind==5?2048:kind==6?1024:512;
    const float phase=float(double(s.effect_timer.fractional)*pi*2/period-pi),wave=float(std::sin(double(phase)));
    s.camera.eye_offset.x=float(double(wave)*(kind==5?70:kind==7?-15:-50));
    s.camera.up.x=float(-double(wave)*(kind==7?.009999999776482582:.10000000149011612));
    if(kind==5)s.camera.eye_offset.z=float(double(float(std::sin(double(normalize_angle(float(double(phase)*2))))))*200);
    s.effect_timer.tick();if(s.effect_timer.current>=period)s.effect_timer.set(0,rate);return true;
}
// 404f20. ECL interrupts skip the label and adopt the following time.
bool stage_interrupt(StageState& s,const StageResource& file,i32 id,const float* rate){
    for(u32 i=0;i<file.instructions.size();++i){const auto& command=file.instructions[i];if(command.time<0)break;
        if(command.opcode==16&&command.argument<i32>(0)==id){if(i+1>=file.instructions.size())return false;s.instruction_offset=file.instructions[i+1].offset;s.script_timer.set(file.instructions[i+1].time,rate);return true;}
    }return true;
}
}
