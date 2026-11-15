#include "StageScript.hpp"
#include <cmath>
namespace th17 {
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
        static constexpr u8 minimum[22]={0,2,3,5,3,5,3,1,3,5,11,11,1,1,3,0,1,1,5,1,1,3};
        if(c->opcode>=0&&c->opcode<=21&&c->arguments.size()<minimum[c->opcode]){error="truncated STD arguments";return false;}
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
        case 12:s.camera_effect=u8(command.argument<u32>(0));if(!s.camera_effect)s.camera.eye_offset={};if(s.camera_effect==6)s.effect_fade.set(0);else s.effect_timer.set(0);if(s.camera_effect==2)s.effect_timer.set(512);break;
        case 13:if(!world_.clear_color(command.argument<u32>(0))){error="STD clear color unavailable";return false;}break;
        case 14:{const i32 index=command.argument<i32>(0),script=command.argument<i32>(1),layer=command.argument<i32>(2);if(index<0||index>=8){error="STD animation slot out of range";return false;}if(!world_.animation(u32(index),script,layer)){error="STD animation unavailable";return false;}s.animation_layers[u32(index)]=layer;break;}
        case 15:case 16:break;
        case 17:s.deformation_target=112;s.deformation_radius=192;s.deformation_color=0xffffffff;s.deformation_timer.set(0);s.phase_x=s.phase_y=0;s.deformation_mode=command.argument<i32>(0);if(!world_.deformation(s.deformation_mode,s.deformation_mode==1?7:17)){error="STD deformation unavailable";return false;}break;
        case 18:interpolate(s.up,s.camera.up,command,false);break;
        case 19:if(!world_.object_interrupt(wrapping_add(command.argument<i32>(0),7))){error="STD object interrupt unavailable";return false;}break;
        case 20:{const auto d=command.argument<float>(0);s.culling_distance_squared=float(d*d);break;}
        case 21:s.fov.start={s.camera.fov};s.fov.end={command.argument<float>(2)};s.fov.duration=command.argument<i32>(0);s.fov.mode=command.argument<i32>(1);s.fov.timer.set(0);break;
        default:break; // Original ignores opcodes outside its switch table.
        }
        if(stopped)break;state.instruction_offset+=u32(command.length);
    }
    if(!stopped)state.timer.tick(&rate);
    if(state.direction.duration)state.camera.direction=vector(state.direction.step(rate));
    if(state.position.duration)state.camera.position=vector(state.position.step(rate));
    if(state.fog.duration)state.camera.fog=state.fog.step(rate);
    if(state.up.duration)state.camera.up=vector(state.up.step(rate));
    if(state.fov.duration)state.camera.fov=state.fov.step(rate)[0];
    camera_effect(rate);return true;
}
bool StageScript::interrupt(i32 id){
    for(const auto& command:file_.instructions){if(command.time<0)break;if(command.opcode==16&&command.argument<i32>(0)==id){state.instruction_offset=command.offset;state.timer.set(command.time);return true;}}return true;
}
void StageScript::camera_effect(float rate)noexcept{
    auto& s=state;const u32 kind=s.camera_effect;if(kind==0||kind==7||kind==8||kind>11)return;
    constexpr float pi=3.1415927410125732421875f;
    const auto frame=s.effect_timer.current;const float elapsed=s.effect_timer.fractional;
    const float phase_base=float(float(elapsed*pi)*2);
    const auto integer_phase=[&]{return normalize_angle(float(float(float(float(frame%1024)*pi)*2)*(1.f/1024)));};
    float phase=0,wave=0,amplitude=1;i32 period=2048,wrap=1024;
    switch(kind){
    case 1:amplitude=frame<512?float(elapsed/512):1;phase=normalize_angle(float(phase_base/1024));wave=sine(phase);s.camera.eye_offset.x=float(float(wave*-10)*amplitude);s.camera.eye_offset.z=float(float(sine(normalize_angle(float(phase*2)))*-10)*amplitude);s.camera.up.x=float(float(wave*-.01f)*amplitude);s.camera.target_offset.x=float(s.camera.eye_offset.x*-.5f);s.camera.target_offset.z=float(s.camera.eye_offset.z*-.5f);break;
    case 2:case 4:period=3072;wrap=0;phase=normalize_angle(float(phase_base/3072));s.camera.up.x=-sine(phase);s.camera.up.z=kind==2?cosine(phase):-cosine(phase);break;
    case 3:phase=integer_phase();amplitude=frame<1024?float(float(elapsed*80)/1024):80;s.camera.eye_offset={float(sine(phase)*amplitude),0,0};s.camera.target_offset.x=float(s.camera.eye_offset.x/-10);break;
    case 5:phase=integer_phase();amplitude=frame<512?float(elapsed/512):1;wave=sine(phase);s.camera.eye_offset.x=float(float(wave*-5)*amplitude);s.camera.target_offset.x=float(float(s.camera.eye_offset.x*-.5f)*amplitude);s.camera.up.x=float(float(wave*-.005f)*amplitude);break;
    case 6:phase=integer_phase();amplitude=s.effect_fade.current<512?float(float(1-float(s.effect_fade.fractional/512))*80):0;s.camera.eye_offset={float(sine(phase)*amplitude),0,0};s.camera.target_offset.x=float(s.camera.eye_offset.x/-10);s.effect_fade.tick(&rate);break;
    case 9:period=2048;wrap=0;phase=float(float(phase_base/2048)-pi);wave=sine(phase);s.camera.eye_offset.x=float(wave*70);s.camera.eye_offset.z=float(sine(normalize_angle(float(phase*2)))*200);s.camera.up.x=float(wave*-.1f);break;
    case 10:period=1024;wrap=0;phase=float(float(phase_base/1024)-pi);wave=sine(phase);s.camera.up.x=float(wave*-.1f);s.camera.eye_offset.x=float(wave*-50);break;
    case 11:period=512;wrap=0;phase=float(float(phase_base/512)-pi);wave=sine(phase);s.camera.up.x=float(wave*-.01f);s.camera.eye_offset.x=float(wave*-15);break;
    }
    s.effect_timer.tick(&rate);if(s.effect_timer.current>=period)s.effect_timer.set(wrap);
}
}
