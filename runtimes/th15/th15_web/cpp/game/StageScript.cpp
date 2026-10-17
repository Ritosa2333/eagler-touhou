#include "StageScript.hpp"
#include <cmath>
namespace th15 {
void StageScript::interpolate_fog(i32 duration,i32 mode,const StageFog& target)noexcept{state.fog.start=state.camera.fog;state.fog.end=target;state.fog.duration=duration;state.fog.mode=mode;state.fog.timer.set(0);}
namespace {
std::array<float,3> vector(const StageInstruction& c,u32 i){return {c.argument<float>(i),c.argument<float>(i+1),c.argument<float>(i+2)};}
std::array<float,3> array(Vec3 v){return {v.x,v.y,v.z};}
Vec3 vector(const std::array<float,3>& a){return {a[0],a[1],a[2]};}
void interpolate(Vec3Interpolation& v,Vec3 from,const StageInstruction& c,bool hermite){v.start=array(from);v.end=vector(c,hermite?5:2);v.duration=c.argument<i32>(0);v.mode=hermite?8:c.argument<i32>(1);if(hermite){v.control1=vector(c,2);v.control2=vector(c,8);}v.timer.set(0);}
float sine(float phase){return float(std::sin(double(phase)));}
float cosine(float phase){return float(std::cos(double(phase)));}
}
bool StageScript::step(float rate){
    if(!error.empty())return false;bool stopped=false;u32 operations=0;
    for(;;){const auto* c=file_.instruction(state.instruction_offset);if(!c){error="STD instruction offset outside stream";return false;}if(c->time>state.timer.current)break;if(c->time<0){error="STD script reached terminator without stop or jump";return false;}if(++operations>100000){error="STD instruction loop limit";return false;}
        static constexpr u8 minimum[21]={0,2,3,5,3,5,3,1,3,5,11,11,1,1,3,0,1,1,5,1,1};
        if(c->opcode>=0&&c->opcode<=20&&c->arguments.size()<minimum[c->opcode]){error="truncated STD arguments";return false;}
        const auto& command=*c;auto& s=state;switch(command.opcode){
        case 0:stopped=true;break;
        case 1:s.timer.set(command.argument<i32>(1));s.instruction_offset=command.argument<u32>(0);continue;
        case 2:{const auto prior=s.camera.position;s.camera.position=vector(vector(command,0));s.camera.animation_delta={float(s.camera.position.x-prior.x),float(s.camera.position.y-prior.y),float(s.camera.position.z-prior.z)};break;}
        case 3:interpolate(s.position,s.camera.position,command,false);break;
        case 4:s.camera.direction=vector(vector(command,0));break;
        case 5:interpolate(s.direction,s.camera.direction,command,false);break;
        case 6:s.camera.up=vector(vector(command,0));break;
        case 7:s.camera.fov=command.argument<float>(0);break;
        case 8:s.camera.fog.set(command.argument<u32>(0),command.argument<float>(1),command.argument<float>(2));break;
        case 9:s.fog.start=s.camera.fog;s.fog.end.set(command.argument<u32>(2),command.argument<float>(3),command.argument<float>(4));s.fog.duration=command.argument<i32>(0);s.fog.mode=command.argument<i32>(1);s.fog.timer.set(0);break;
        case 10:interpolate(s.position,s.camera.position,command,true);break;
        case 11:interpolate(s.direction,s.camera.direction,command,true);break;
        case 12:s.camera_effect=u8(command.argument<u32>(0));if(!s.camera_effect)s.camera.eye_offset={};s.effect_timer.set(0);break;
        case 13:if(!world_.clear_color(command.argument<u32>(0))){error="STD clear color unavailable";return false;}break;
        case 14:{const i32 index=command.argument<i32>(0),script=command.argument<i32>(1),layer=command.argument<i32>(2);if(index<0||index>=8){error="STD animation slot out of range";return false;}if(!world_.animation(u32(index),script,layer)){error="STD animation unavailable";return false;}s.animation_layers[u32(index)]=layer;break;}
        case 15:case 16:break;
        case 17:s.deformation_target=112;s.deformation_radius=192;s.deformation_color=0xffffffff;s.phase_x=s.phase_y=0;s.deformation_mode=command.argument<i32>(0);if(!world_.deformation(s.deformation_mode,s.deformation_mode==1?7:17)){error="STD deformation unavailable";return false;}break;
        case 18:interpolate(s.up,s.camera.up,command,false);break;
        case 19:if(!world_.object_interrupt(wrapping_add(command.argument<i32>(0),7))){error="STD object interrupt unavailable";return false;}break;
        case 20:{const auto d=command.argument<float>(0);s.culling_distance_squared=float(d*d);break;}
        default:break; // Original ignores opcodes outside its switch table.
        }
        if(stopped)break;state.instruction_offset+=u32(command.length);
    }
    if(!stopped)state.timer.tick(&rate);
    if(state.direction.duration)state.camera.direction=vector(state.direction.step(rate));
    if(state.position.duration)state.camera.position=vector(state.position.step(rate));
    if(state.fog.duration)state.camera.fog=state.fog.step(rate);
    if(state.up.duration)state.camera.up=vector(state.up.step(rate));
    camera_effect(rate);return true;
}
bool StageScript::interrupt(i32 id){
    for(const auto& command:file_.instructions){if(command.time<0)break;if(command.opcode==16&&command.argument<i32>(0)==id){const auto offset=u32(command.length);const auto* target=file_.instruction(offset);if(!target){error="STD interrupt offset outside stream";return false;}state.instruction_offset=offset;state.timer.set(target->time);return true;}}return true;
}
void StageScript::camera_effect(float rate)noexcept{
    auto& s=state;const u32 kind=s.camera_effect;if(kind==0||kind==5||kind==6||kind>9)return;
    constexpr float pi=3.1415927410125732421875f;float phase=float(float(s.effect_timer.fractional*pi)*2),wave=0; i32 period=512;
    switch(kind){
    case 1:phase=normalize_angle(float(phase*(1.f/512)));wave=sine(phase);s.camera.eye_offset.x=float(wave*-20);s.camera.eye_offset.z=float(sine(normalize_angle(float(phase*2)))*-10);s.camera.up.x=float(wave*-.01f);s.camera.target_offset.x=float(s.camera.eye_offset.x*-.5f);s.camera.target_offset.z=float(s.camera.eye_offset.z*-.5f);break;
    case 2:period=3072;phase=normalize_angle(float(phase/3072));s.camera.eye_offset.x=float(sine(phase)*-700);s.camera.target_offset.x=-s.camera.eye_offset.x;s.camera.eye_offset.y=float(cosine(phase)*700);s.camera.target_offset.y=-s.camera.eye_offset.y;break;
    case 3:period=2048;phase=normalize_angle(float(phase*(1.f/2048)));wave=sine(phase);s.camera.eye_offset.x=float(wave*50);s.camera.eye_offset.z=float(sine(normalize_angle(float(phase*2)))*50);s.camera.target_offset.x=-s.camera.eye_offset.x;s.camera.target_offset.z=-s.camera.eye_offset.z;s.camera.up.x=float(wave*-.05f);break;
    case 4:period=1536;phase=normalize_angle(float(phase/1536));s.camera.eye_offset.x=float(sine(phase)*-550);s.camera.target_offset.x=-s.camera.eye_offset.x;s.camera.eye_offset.y=float(cosine(phase)*550);s.camera.target_offset.y=-s.camera.eye_offset.y;break;
    case 7:period=2048;phase=float(float(phase*(1.f/2048))-pi);wave=sine(phase);s.camera.eye_offset.x=float(wave*70);s.camera.eye_offset.z=float(sine(normalize_angle(float(phase*2)))*200);s.camera.up.x=float(wave*-.1f);break;
    case 8:period=1024;phase=float(float(phase*(1.f/1024))-pi);wave=sine(phase);s.camera.up.x=float(wave*-.1f);s.camera.eye_offset.x=float(wave*-50);break;
    case 9:phase=float(float(phase*(1.f/512))-pi);wave=sine(phase);s.camera.up.x=float(wave*-.01f);s.camera.eye_offset.x=float(wave*-15);break;
    }
    s.effect_timer.tick(&rate);if(s.effect_timer.current>=period)s.effect_timer.set(0);
}
}
