#include "Stage.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
namespace {
// See Stage.hpp: counters for the stage-interrupt probe, kept out of StageState so its layout asserts stay true.
u32 interrupt_calls_count=0,interrupt_failure_count=0;
Vec3 vector(const StageInstruction& command,u32 index){return {command.argument<float>(index),command.argument<float>(index+1),command.argument<float>(index+2)};}
void interpolate(Vec3Interpolator& target,Vec3 current,const StageInstruction& command,const float* rate,bool hermite){
    target.duration=command.argument<i32>(0);target.mode=hermite?InterpolationMode::Hermite:command.argument<InterpolationMode>(1);
    std::memcpy(target.start,&current,12);const auto end=vector(command,hermite?5:2);std::memcpy(target.end,&end,12);
    if(hermite){const auto initial=vector(command,2),final=vector(command,8);std::memcpy(target.initial_tangent,&initial,12);std::memcpy(target.final_tangent,&final,12);}
    target.timer.set(0,rate);
}
#if PORT_TITLE == 13
// Retail 408c6b..409067. The target offset is separate from the eye offset.
void camera_effect_th13(StageState& s,const float* rate){
    const u32 kind=s.camera_effect;
    if(!kind||kind>9||kind==5||kind==6)return;
    constexpr double pi=3.1415927410125732421875;
    const i32 period=kind==2?3072:kind==3||kind==7?2048:kind==4?1536:kind==8?1024:512;
    const float phase=kind<5?normalize_angle(float(double(s.effect_timer.fractional)*pi*2/period)):
        float(double(s.effect_timer.fractional)*pi*2/period-pi);
    const float wave=float(std::sin(double(phase)));
    auto& c=s.camera;
    if(kind==1){
        c.eye_offset.x=float(double(wave)*-120);
        c.eye_offset.z=float(double(float(std::sin(double(normalize_angle(float(double(phase)*2))))))*-50);
        c.up.x=float(-double(wave)*.009999999776482582);
        c.target_offset.x=float(-double(c.eye_offset.x)*.5);c.target_offset.z=float(-double(c.eye_offset.z)*.5);
    }else if(kind==2||kind==4){
        const double radius=kind==2?700:550;
        c.eye_offset.x=float(-double(wave)*radius);
        c.eye_offset.y=float(double(float(std::cos(double(phase))))*radius);
        c.target_offset.x=-c.eye_offset.x;c.target_offset.y=-c.eye_offset.y;
    }else if(kind==3){
        c.eye_offset.x=float(double(wave)*50);
        c.eye_offset.z=float(double(float(std::sin(double(normalize_angle(float(double(phase)*2))))))*50);
        c.target_offset.x=-c.eye_offset.x;c.target_offset.z=-c.eye_offset.z;
        c.up.x=float(-double(wave)*.05000000074505806);
    }else if(kind==7){
        c.eye_offset.x=float(double(wave)*70);
        const float doubled=float(double(phase)*2);
        c.eye_offset.z=float(double(float(std::sin(double(normalize_angle(doubled)))))*200);
        c.up.x=float(-double(wave)*.10000000149011612);
    }else{
        c.eye_offset.x=float(double(wave)*(kind==8?-50:-15));
        c.up.x=float(-double(wave)*(kind==8?.10000000149011612:.009999999776482582));
    }
    s.effect_timer.tick();if(s.effect_timer.current>=period)s.effect_timer.set(0,rate);
}
#endif
#if PORT_TITLE == 14
// Retail 40ff7a..4103ce uses scalar SSE operations at every arithmetic step.
void camera_effect_th14(StageState& s,const float* rate){
    const u32 kind=s.camera_effect;if(!kind||kind>9||kind==5||kind==6)return;
    constexpr float pi=3.1415927410125732421875f;
    const i32 period=kind==2?3072:kind==3||kind==7?2048:kind==4?1536:kind==8?1024:512;
    float phase=s.effect_timer.fractional*pi;phase=phase*2;phase=kind==2||kind==4?phase/float(period):phase*(1.f/float(period));
    phase=kind<5?normalize_angle(phase):phase-pi;
    const float wave=float(std::sin(double(phase)));
    auto& c=s.camera;
    if(kind==1){
        c.eye_offset.x=wave*-20;c.eye_offset.z=float(std::sin(double(normalize_angle(phase*2))))*-10;c.up.x=wave*-.01f;
        c.target_offset.x=c.eye_offset.x*-.5f;c.target_offset.z=c.eye_offset.z*-.5f;
    }else if(kind==2||kind==4){
        c.eye_offset.x=wave*(kind==2?-700.f:-550.f);c.eye_offset.y=float(std::cos(double(phase)))*(kind==2?700.f:550.f);
        c.target_offset.x=-c.eye_offset.x;c.target_offset.y=-c.eye_offset.y;
    }else if(kind==3){
        c.eye_offset.x=wave*50;c.eye_offset.z=float(std::sin(double(normalize_angle(phase*2))))*50;c.up.x=wave*-.05f;
        c.target_offset.x=-c.eye_offset.x;c.target_offset.z=-c.eye_offset.z;
    }else if(kind==7){
        c.eye_offset.x=wave*70;c.eye_offset.z=float(std::sin(double(normalize_angle(phase*2))))*200;c.up.x=wave*-.1f;
    }else{c.eye_offset.x=wave*(kind==8?-50.f:-15.f);c.up.x=wave*(kind==8?-.1f:-.01f);}
    s.effect_timer.tick();if(s.effect_timer.current>=period)s.effect_timer.set(0,rate);
}
#endif
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
        // 4049fc. Both arguments are used and the first one really is a jump target:
        //   mov eax,[ebx+0x48]                    ; the flag word guarding one-time initialisation
        //   mov esi,[esi+0xc]                     ; arg index 1, the new wait count
        //   ... (first time only) init +0x40/+0x38/+0x44/+0x48
        //   mov [ebx+0x3c],esi ; dec esi ; mov [ebx+0x38],esi      ; count and count-1
        //   mov edx,[ebx+0x4c] ; mov eax,[edx+8] ; add eax,[ebx+0x1c] ; mov [ebx+0x4c],eax
        //   jmp 0x40521d : mov eax,[edx] ; cmp eax,[ebx+0x3c] ; jle 0x4049e5
        // The loop head at 0x4049e5 fetches through [ebx+0x4c] and the header puts the opcode at +4, the
        // length at +6 and the arguments at +8, so +0xc is argument 1 and +8 is argument 0. The cursor is
        // rebuilt as base(+0x1c) + arg0, so the first argument is a byte offset in the STD stream: this
        // opcode resets the timer and jumps. The port already transcribed both halves and they are right.
        case 1:s.script_timer.set(command.argument<i32>(1),rate);s.instruction_offset=command.argument<u32>(0);continue;
        case 2:{const auto previous=s.camera.position;s.camera.position=vector(command,0);s.camera.animation_delta={float(double(s.camera.position.x)-previous.x),float(double(s.camera.position.y)-previous.y),float(double(s.camera.position.z)-previous.z)};break;}
        case 3:interpolate(s.position_interpolation,s.camera.position,command,rate,false);break;
        case 4:s.camera.direction=vector(command,0);break;
        case 5:interpolate(s.direction_interpolation,s.camera.direction,command,rate,false);break;
        case 6:s.camera.up=vector(command,0);break;
        case 7:s.camera.fov=command.argument<float>(0);break;
        case 8:s.camera.fog.set(command.argument<u32>(0),command.argument<float>(1),command.argument<float>(2));break;
        case 9:{auto& f=s.fog_interpolation;f.duration=command.argument<i32>(0);f.mode=command.argument<InterpolationMode>(1);f.start=s.camera.fog;f.end.set(command.argument<u32>(2),command.argument<float>(3),command.argument<float>(4));f.timer.set(0,rate);break;}
        case 10:interpolate(s.position_interpolation,s.camera.position,command,rate,true);break;
        case 11:interpolate(s.direction_interpolation,s.camera.direction,command,rate,true);break;
        case 12:s.camera_effect=u8(command.argument<u32>(0));if(!s.camera_effect){s.camera.eye_offset={};if constexpr(kPortTitle!=13&&kPortTitle!=14)s.camera.up={0,1,0};}s.effect_timer.set(0,rate);break;
        case 13:world.stage_color(command.argument<u32>(0));break;
        case 14:{const i32 index=command.argument<i32>(0),script=command.argument<i32>(1);if(index<0||index>=8)return false;auto& vm=s.script_animations[index];if(script<0)vm.flags&=~1u;else if(!world.stage_animation(vm,script))return false;
#if PORT_TITLE == 14
            // 40fce9..40fcf2 records the layer even when the script is hidden.
            s.script_animation_layers[index]=command.argument<i32>(2);
#endif
            break;}
        case 17:s.deformation_target=112;s.deformation_radius=192;s.deformation_color=0xffffffff;s.phase_x=s.phase_y=normalize_angle(0);s.deformation_mode=command.argument<i32>(0);if(!world.stage_deformation(s.deformation_mode))return false;break;
#if PORT_TITLE == 13 || PORT_TITLE == 14
        case 18:interpolate(s.up_interpolation,s.camera.up,command,rate,false);break;
#endif
        default:break; // Original skips labels (16) and unassigned opcodes.
        }
        if(stopped)break;s.instruction_offset+=u32(command.length);
    }
    if(!stopped)s.script_timer.tick();
    if(s.direction_interpolation.duration)s.camera.direction=sample(s.direction_interpolation,rate);
    if(s.position_interpolation.duration)s.camera.position=sample(s.position_interpolation,rate);
    if(s.fog_interpolation.duration)s.camera.fog=sample(s.fog_interpolation,rate);
#if PORT_TITLE == 13 || PORT_TITLE == 14
    if(s.up_interpolation.duration)s.camera.up=sample(s.up_interpolation,rate);
#if PORT_TITLE == 13
    camera_effect_th13(s,rate);return true;
#else
    camera_effect_th14(s,rate);return true;
#endif
#endif
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
    ++interrupt_calls_count;
    for(u32 i=0;i<file.instructions.size();++i){const auto& command=file.instructions[i];if(command.time<0)break;
        if(command.opcode==16&&command.argument<i32>(0)==id){if(i+1>=file.instructions.size()){++interrupt_failure_count;return false;}s.instruction_offset=file.instructions[i+1].offset;s.script_timer.set(file.instructions[i+1].time,rate);return true;}
    }++interrupt_failure_count;return true;
}
}

namespace th11 {u32 stage_interrupt_calls(){return interrupt_calls_count;}u32 stage_interrupt_failures(){return interrupt_failure_count;}}
