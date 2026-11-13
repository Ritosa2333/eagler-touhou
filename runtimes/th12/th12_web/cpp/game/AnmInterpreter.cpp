#include "AnmVm.hpp"
#include "AnmOpcodes.hpp"
#include "AnmGeometry.hpp"
#include "AnmTrace.hpp"
#include <cmath>
namespace th11 {
namespace {
Vec3 sample_vector_stack(Vec3Interpolator& interpolation,AnmEnvironment& env){
    const auto value=sample(interpolation,&env.rate);
#if PORT_TITLE == 14
    // Terminal copies skip the temporary. Acceleration's final tangent
    // update overwrites its earlier position value at 409d9d.
    if(interpolation.duration){const float z=interpolation.mode==InterpolationMode::Acceleration?interpolation.final_tangent[2]:value.z;u32 word;std::memcpy(&word,&z,4);env.th14_interpolation_scratch(word);}
#endif
    return value;
}
Rgb sample_rgb_stack(RgbInterpolator& interpolation,AnmEnvironment& env){
    const i32 previous_green=interpolation.start[1];
    const auto value=sample(interpolation,&env.rate);
#if PORT_TITLE == 14
    // 473ef9 stores the previous green component for velocity; 473f47
    // stores its updated value for acceleration. Terminal copies skip both.
    if(interpolation.duration){
        if(interpolation.mode==InterpolationMode::Velocity)env.th14_interpolation_scratch(u32(previous_green));
        else if(interpolation.mode==InterpolationMode::Acceleration)env.th14_interpolation_scratch(u32(interpolation.start[1]));
    }
#endif
    return value;
}
Vec2 sample_vec2_stack(Vec2Interpolator& interpolation,AnmEnvironment& env){
    const auto value=sample(interpolation,&env.rate);
#if PORT_TITLE == 14
    // 474750 -> 4041a0 -> 4a8020 stores the x87 control word at 4a8038.
    // Only two bytes overlap the retained constructor temporary. Device PC24
    // and floating-RNG FNINIT PC64 are already tracked by the ANM environment.
    const i32 mode=i32(interpolation.mode);
    if(interpolation.duration&&mode>=18&&mode<=21)
        env.th14_interpolation_scratch(env.th14_execution_precision()?0x037fu:0x007fu,0xffffu);
#endif
    return value;
}
float add_angle(float a,float b){
    float value=float(double(a)+b);u32 loops=0;
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}
    while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}
    return value;
}
void rgb_start(RgbInterpolator& v,i32 duration,u8 mode,u32 from,u32 to,const float* rate){
    v.duration=duration;v.mode=InterpolationMode(mode);
    for(u32 i=0;i<3;++i){v.start[i]=(from>>(i*8))&255;v.end[i]=(to>>(i*8))&255;v.initial_tangent[i]=v.final_tangent[i]=0;}
    v.timer.set(0,rate);
}
void alpha_start(AlphaInterpolator& v,i32 duration,u8 mode,u8 from,u8 to,const float* rate,bool clear){
    v.duration=duration;v.mode=InterpolationMode(mode);v.start[0]=from;v.end[0]=to;
    if(clear)v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,rate);
}
u32 rgb_bits(Rgb v){return (u32(v.blue)&255)|((u32(v.green)&255)<<8)|((u32(v.red)&255)<<16);}
bool dispatch_interrupt(AnmVm& vm,const float* rate
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    ,const i32* captured_interrupt=nullptr
#endif
){
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    const i32 observed_interrupt=captured_interrupt?*captured_interrupt:i32(vm.pending_interrupt);
#endif
    auto* marker=vm.script_begin;AnmInstruction* fallback=nullptr;
    for(;marker->opcode!=-1;marker=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(marker)+marker->length)){
        if(marker->opcode==64){const i32 id=marker->argument<i32>(0);if(id==
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
            observed_interrupt
#else
            vm.pending_interrupt
#endif
        )break;if(id==-1)fallback=marker;}
    }
    vm.pending_interrupt=0;vm.flags&=~0x1000u;if(marker->opcode!=64)marker=fallback;if(!marker)return false;
    // TH14 47002f calls4752f0: timer assignment keeps the integer frame,
    // resets its fractional clock, and preserves the destination rate.
    if constexpr(kPortTitle==14)vm.saved_timer.set(vm.timer.current,rate);
    else vm.saved_timer=vm.timer;
    vm.saved_instruction=vm.instruction;vm.timer.set(marker->time,rate);
    vm.instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(marker)+marker->length);vm.flags|=1;return true;
}
bool animate(AnmVm& vm,AnmEnvironment& env){
    if constexpr(kPortTitle==16){
        // 462a06 gates the complete 4630f0 velocity consumer with flags2 bit24.
        if(vm.flags2&0x1000000u){
            float* axes[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};const float speeds[]={vm.angular_velocity.x,vm.angular_velocity.y,vm.angular_velocity.z};
            for(u32 i=0;i<3;++i)if(speeds[i]!=0){const float delta=float(env.rate*speeds[i]);*axes[i]=add_angle(*axes[i],delta);vm.flags|=4;}
            if(vm.scale_velocity.y!=0){const float delta=float(env.rate*vm.scale_velocity.y);vm.scale.y=float(delta+vm.scale.y);vm.flags|=8;}
            if(vm.scale_velocity.x!=0){const float delta=float(env.rate*vm.scale_velocity.x);vm.scale.x=float(delta+vm.scale.x);vm.flags|=8;}
            auto scroll=[&](float& pos,float speed){if(speed==0)return;const float delta=float(env.rate*speed);float next=float(delta+pos);if(next>=2)next=float(next-2);else if(next<0)next=float(next+2);pos=next;};
            scroll(vm.uv_offset.x,vm.scroll_velocity.x);scroll(vm.uv_offset.y,vm.scroll_velocity.y);
        }
    }else{
    float* axes[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};const float speeds[]={vm.angular_velocity.x,vm.angular_velocity.y,vm.angular_velocity.z};
    for(u32 i=0;i<3;++i)if(speeds[i]!=0){*axes[i]=add_angle(*axes[i],float(double(env.rate)*speeds[i]));vm.flags|=4;}
    if(vm.scale_velocity.y!=0){vm.scale.y=float(double(env.rate)*vm.scale_velocity.y+vm.scale.y);vm.flags|=8;}
    if(vm.scale_velocity.x!=0){const float delta=float(double(env.rate)*vm.scale_velocity.x);vm.scale.x=float(double(delta)+vm.scale.x);vm.flags|=12;}
    auto scroll=[&](float pos,float speed){
        if constexpr(kPortTitle==14){
            // 473272..47330b skips zero velocity and rounds mulss/addss
            // separately before the single wrap at a period of two.
            if(speed==0)return pos;
            const float delta=float(env.rate*speed);float next=float(delta+pos);
            if(next>=2)next=float(next-2);else if(next<0)next=float(next+2);return next;
        }
        constexpr double period=kPortTitle==13?2:1;float next=float(double(env.rate)*speed+pos);if(next>=period)next=float(double(next)-period);else if(next<0)next=float(double(next)+period);return next;
    };
    vm.uv_offset.x=scroll(vm.uv_offset.x,vm.scroll_velocity.x);vm.uv_offset.y=scroll(vm.uv_offset.y,vm.scroll_velocity.y);
    }
    if((kPortTitle==13||kPortTitle==14)?(vm.th13_native_flags&0x8000):(vm.flags&0x2000)){vm.position.x=float(double(vm.position.x)+env.camera_delta.x);vm.position.y=float(double(vm.position.y)+env.camera_delta.y);vm.position.z=float(double(vm.position.z)+env.camera_delta.z);}
    if(vm.flags2&(kPortTitle==13?0x2000:kPortTitle==14?0x1000:4))if(!env.screen_uv(vm))return false;
    if(vm.position_interpolation.duration)(vm.flags&0x100?vm.child_position:vm.script_position)=sample_vector_stack(vm.position_interpolation,env);
    if(vm.color_interpolation.duration)vm.color=(vm.color&0xff000000)|rgb_bits(sample_rgb_stack(vm.color_interpolation,env));
    if(vm.alpha_interpolation.duration)vm.color=(vm.color&0xffffff)|(u32(sample(vm.alpha_interpolation,&env.rate))<<24);
    if(vm.scale_interpolation.duration){vm.scale=sample_vec2_stack(vm.scale_interpolation,env);vm.flags|=8;}
#if PORT_TITLE == 14
    if(vm.extra_scale_interpolation.duration){vm.extra_scale=sample_vec2_stack(vm.extra_scale_interpolation,env);vm.flags|=8;}
    if(vm.texture_scale_interpolation.duration){vm.texture_scale=sample_vec2_stack(vm.texture_scale_interpolation,env);vm.th13_native_flags|=0x10;}
#endif
    if(vm.rotation_interpolation.duration){vm.rotation=sample_vector_stack(vm.rotation_interpolation,env);vm.flags|=4;}
    if(vm.color2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xff000000)|rgb_bits(sample_rgb_stack(vm.color2_interpolation,env));
    if(vm.alpha2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xffffff)|(u32(sample(vm.alpha2_interpolation,&env.rate))<<24);
    if(vm.scroll_x_interpolation.duration)vm.scroll_velocity.x=sample(vm.scroll_x_interpolation,&env.rate);
    if(vm.scroll_y_interpolation.duration)vm.scroll_velocity.y=sample(vm.scroll_y_interpolation,&env.rate);
#if PORT_TITLE == 13
    if(vm.extra_scale_interpolation.duration){vm.extra_scale=sample_vec2_stack(vm.extra_scale_interpolation,env);vm.flags|=8;}
    if(vm.texture_scale_interpolation.duration){vm.texture_scale=sample_vec2_stack(vm.texture_scale_interpolation,env);vm.th13_native_flags|=0x10;}
#endif
    const u32 mode=vm.drawing_mode();if((mode==9||mode==13||((kPortTitle==13||kPortTitle==14)&&mode==14)||(kPortTitle==14&&(mode==23||mode==24)))&&!env.update_geometry(vm))return false;
    return true;
}
i32 calculate(i32 a,i32 b,u32 op){
    if(op==0)return b;if(op==1)return wrapping_add(a,b);if(op==2)return wrapping_sub(a,b);if(op==3)return signed_bits(u32(a)*u32(b));
    if(b==0||(a==INT32_MIN&&b==-1))__builtin_trap();return op==4?a/b:a%b;
}
double calculate(double a,double b,u32 op){switch(op){case 0:return b;case 1:return a+b;case 2:return a-b;case 3:return a*b;case 4:return a/b;default:return std::fmod(a,b);}}
}
// 0x44b4b0. The instruction set shares older opcodes but has TH11-specific
// scrolling interpolation, screen UVs, child spawning and partial-ring modes.
int AnmVm::update(AnmEnvironment& env){
#if PORT_TITLE == 14
    struct RngContext{AnmEnvironment& environment;const AnmVm& vm;} rng_context{env,*this};
    RngBoundaryScope rng_scope(env.script_rng,env.visual_rng,&rng_context,
        [](void* context,Rng& rng,RngBoundary point){auto& c=*static_cast<RngContext*>(context);return c.environment.th14_rng_boundary(c.vm,rng,point);});
#endif
#if PORT_TITLE == 13
    // 46246d runs the effect's pre-update before stop/pause and rate checks.
    if(th13_effect==5)anm_th13_ribbon_update(*this,env);
#endif
    // TH13 0x46249b returns zero when opcode 2 has stopped the script. Keeping
    // the VM registered preserves its last visible frame (TH12 returns one).
    struct RateScope{decltype(env.rate)& value;float previous;~RateScope(){value=previous;}} guard{env.rate,env.rate};
    if(kPortTitle==16?(flags2&0x200):(kPortTitle==13||kPortTitle==14)?(flags2&0x100):(flags&0x20000000))env.rate=1;
#if PORT_TITLE == 16
    // 45f9ad..45fa55: parent slowdown uses the saved incoming rate, then the
    // pre-update callback runs with that effective rate, including stopped VMs.
    if(const float slowdown=inherited_slowdown();slowdown>0){env.rate=float(guard.previous-float(guard.previous*slowdown));if(env.rate<0)env.rate=0;}
    if(th16_callbacks[0]){const int result=env.th16_pre_update(*this);if(result==AnmEnvironment::th16_callback_unavailable)return -1;if(result)return 1;}
#endif
#if PORT_TITLE == 14
    if(const float slowdown=inherited_slowdown();slowdown>0){env.rate=float(guard.previous-float(guard.previous*slowdown));if(env.rate<0)env.rate=0;}
    // 46ff13 runs pre-update with the effective rate, even after opcode 2.
    if(th13_effect==1&&geometry){const int status=static_cast<TransitionType0Th14*>(geometry)->update(env);if(status)return status;}
    if(th13_effect==8)anm_th14_ribbon_update(*this,env);
    if(th13_effect==7){const int status=anm_th14_particles_update(*this,env);if(status)return status;}
#endif
    if(!instruction)return (kPortTitle==13||kPortTitle==14||kPortTitle==16)?0:1;if(flags&0x20000)return 0;
#if PORT_TITLE == 14
    if(!age.rate)age.set(0,&env.rate);age.tick();
#if defined(TH14_CPP_STAGE_DECODE)
    //46ffa5 loads pending exactly once, before46ffc2 reads Control.
    const i32 pending_on_entry=pending_interrupt;
    if(!pending_on_entry&&(u32(flags2)&0x6000u)==0x2000u&&env.th14_control_vm_blocked())return 0;
#endif
#endif
    auto finish=[&](){
#if PORT_TITLE == 14
        if(rng_scope.failed)return -1;
#endif
        if(!animate(*this,env))return -1;
#if PORT_TITLE == 14
        if(rng_scope.failed)return -1;
#endif
        if(after_update&&after_update(*this))return 1;timer.tick();return 0;};
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(pending_on_entry&&!dispatch_interrupt(*this,&env.rate,&pending_on_entry)){timer.advance(-1);return finish();}
#else
    if(pending_interrupt&&!dispatch_interrupt(*this,&env.rate)){timer.advance(-1);return finish();}
#endif
    for(u32 budget=0;instruction->time<=timer.current;++budget){
        if(budget==100000)return -1;
#if PORT_TITLE == 14
        if(rng_scope.failed||!env.th14_instruction_boundary(*this))return -1;
#endif
        auto* current=instruction;const i32 op=current->opcode;
        push_trace(u32(reinterpret_cast<const u8*>(current)-reinterpret_cast<const u8*>(script_begin)),u16(op),u16(current->length));
        auto integer=[&](u32 n){const auto v=current->argument<i32>(n);return current->references&(1u<<n)?integer_value(v,env):v;};
        auto floating=[&](u32 n)->double{const auto v=current->argument<float>(n);return current->references&(1u<<n)?float_value(v,env):v;};
        auto int_dest=[&](u32 n){auto* p=reinterpret_cast<i32*>(reinterpret_cast<u8*>(current)+8+n*4);return current->references&(1u<<n)?integer_destination(p):p;};
        auto float_dest=[&](u32 n){auto* p=reinterpret_cast<float*>(reinterpret_cast<u8*>(current)+8+n*4);return current->references&(1u<<n)?float_destination(p):p;};
        auto jump=[&](u32 off,i32 time){timer.set(time,&env.rate);instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(script_begin)+off);};
        auto field=[&](u32 mask,u32 shift,u32 value){flags=(flags&~mask)|((value<<shift)&mask);};
        if(op>=6&&op<=27){
            const bool three=op>=18;const u32 operation=three?(op-18)/2+1:(op-6)/2;
            if(!(op&1)){const i32 a=integer(1),b=three?integer(2):0;auto* out=int_dest(0);*out=calculate(three?a:*out,three?b:a,operation);}
            else {double a=0,b;if(three&&operation==5){b=float(floating(2));a=float(floating(1));}else if(three){a=float(floating(1));b=float(floating(2));}else{b=float(floating(1));if(operation==5)a=float(floating(0));}auto* out=float_dest(0);if(!three&&operation!=5)a=*out;*out=float(calculate(a,b,operation));}
        }else if(op>=28&&op<=39){
            const double a=op&1?double(float(floating(0))):integer(0),b=op&1?floating(1):integer(1);bool take=false;
            switch((op-28)/2){case 0:take=a==b;break;case 1:take=a!=b;break;case 2:take=a<b;break;case 3:take=a<=b;break;case 4:take=a>b;break;case 5:take=a>=b;break;}
            if(take){jump(current->argument<u32>(2),current->argument<i32>(3));continue;}
        }else switch(op){
        case -1:case 1:flags&=~1u;instruction=nullptr;return 1;
        case 2:instruction=nullptr;return (kPortTitle==13||kPortTitle==14||kPortTitle==16)?0:1;
        case anm_th14_layer:
            layer=current->argument<u8>(0);
            flags2=(flags2&~0x60000u)|((layer>=3&&layer<=19)?0x20000u:(layer>=20&&layer<=23)?0x40000u:0);
            if((layer>=20&&layer<=29)||layer==34||layer==35)flags2=(flags2&~0x300000u)|0x80000u;
            break;
        case anm_th16_native_layer:
            // 462950 -> 406d80: low-byte layer, two origin categories,
            // and a resolution write only in the specified layer ranges.
            layer=current->argument<u8>(0);
            flags2=(flags2&~0xc0000u)|((layer>=3&&layer<=19)?0x40000u:(layer>=20&&layer<=23)?0x80000u:0);
            if((layer>=20&&layer<=31)||(layer>=36&&layer<=42))flags2=(flags2&~0x600000u)|0x100000u;
            break;
        case anm_th16_native_mode:
            set_drawing_mode(current->argument<u32>(0));
#if PORT_TITLE == 16
            if(drawing_mode()==10&&!env.th16_mode10(*this))return -1;
#endif
            break;
        case anm_th16_native_resolution:
            flags2=(flags2&~0x700000u)|((u32(integer(0))&7u)<<20);break;
        case anm_th16_native_anchor:
            field(0xc0000u,18,current->argument<u16>(0));
            field(0x300000u,20,current->argument<u32>(0)>>16);break;
        case anm_th16_native_origin:
            flags2=(flags2&~0xc0000u)|((u32(current->argument<u8>(0))&3u)<<18);break;
        case anm_th16_native_flag13:
            th13_native_flags=(th13_native_flags&~0x2000u)|((current->argument<u32>(0)&1u)<<13);break;
        case anm_th16_visible:
            field(1,0,current->argument<u32>(0));
            th13_native_flags=(th13_native_flags&~1u)|(current->argument<u32>(0)&1u);break;
        case anm_th16_field2_bit7:case anm_th16_field2_bit8:case anm_th16_field2_bit9:case anm_th16_field2_bit10:case anm_th16_field2_bit11:case anm_th16_field2_bit13:case anm_th16_field2_bit23:case anm_th16_field2_bit25:{
            const u32 shift=op==anm_th16_field2_bit7?7:op==anm_th16_field2_bit8?8:op==anm_th16_field2_bit9?9:op==anm_th16_field2_bit10?10:op==anm_th16_field2_bit11?11:op==anm_th16_field2_bit13?13:op==anm_th16_field2_bit23?23:25;
            const u32 value=(shift==9||shift==13||shift==23)?u32(integer(0)):current->argument<u32>(0);
            flags2=(flags2&~(1u<<shift))|((value&1u)<<shift);break;
        }
        case anm_th16_scale_interpolate:{
            const float y=float(floating(3)),x=float(floating(2));const i32 duration=integer(0);auto& v=scale_interpolation;
            v.duration=duration;v.mode=InterpolationMode(current->argument<u8>(1));std::memcpy(v.start,&scale,8);v.end[0]=x;v.end[1]=y;
            // 447550 preserves both existing tangent vectors.
            v.timer.set(0,&env.rate);flags|=8;break;
        }
        case anm_th16_secondary_rgb_interpolate:{
            const u32 b=u32(integer(4))&255,g=u32(integer(3))&255,r=u32(integer(2))&255;const i32 duration=integer(0);
            rgb_start(color2_interpolation,duration,current->argument<u8>(1),secondary_color,b|(g<<8)|(r<<16),&env.rate);
            th13_native_flags=(th13_native_flags&~0x60000u)|0x20000u;break;
        }
        case anm_th16_secondary_alpha_interpolate:{
            const u8 target=u8(integer(2));const i32 duration=integer(0);
            alpha_start(alpha2_interpolation,duration,current->argument<u8>(1),secondary_color>>24,target,&env.rate,false);
            th13_native_flags=(th13_native_flags&~0x60000u)|0x20000u;break;
        }
        case anm_th14_layer_flag:
            flags2=(flags2&~0x60000u)|((u32(current->argument<u8>(0))&3u)<<17);break;
        case anm_th14_parent_independent:
            flags2=(flags2&~0x8000u)|((u32(integer(0))&1u)<<15);break;
        case anm_th14_blend:
            field(0x70,4,current->argument<u32>(0));
            th13_native_flags=(th13_native_flags&~0x100u)|((current->argument<u32>(0)&8u)<<5);break;
        case anm_th14_visual_rng:
            field(0x40000000,30,current->argument<u8>(0));
            flags2=(flags2&~0x200u)|((u32(current->argument<u8>(0))&1u)<<9);break;
        case anm_th14_rgb:
            field(0x80000000,31,current->argument<u32>(0));
            flags2=(flags2&~0x400u)|((current->argument<u32>(0)&1u)<<10);break;
        case anm_th14_resolution:
            flags2=(flags2&~0x380000u)|((u32(integer(0))&7u)<<19);break;
        case anm_th14_flag22:
            flags2=(flags2&~0x400000u)|((u32(integer(0))&1u)<<22);break;
        case anm_th14_flag24:
            flags2=(flags2&~0x1000000u)|((u32(current->argument<u8>(0))&1u)<<24);break;
        case anm_th16_flag12:
            flags2=(flags2&~0x1000u)|((u32(current->argument<u8>(0))&1u)<<12);break;
        case anm_th16_flag13:
            field(0x2000,13,current->argument<u32>(0));
            flags2=(flags2&~0x2000u)|((u32(current->argument<u8>(0))&1u)<<13);break;
        case anm_th16_blend:
            field(0x1e0,5,current->argument<u32>(0));
            flags2=(flags2&~0x1e0u)|((u32(current->argument<u8>(0))&0xfu)<<5);break;
        case anm_th16_field25: {
            const u32 v=u32(current->argument<u32>(0))&0x1fu;
            field(0x3e000000u,25,v);
            flags2=(flags2&~0x3e000000u)|(v<<25);break; }
        case anm_th16_visual_pair: {
            const u32 v=u32(current->argument<u32>(1))&3u;
            field(0xc0000000u,30,v);
            flags2=(flags2&~0x600u)|(v<<9);break; }
        case anm_th16_layer: {
            const u8 v=current->argument<u8>(0);
            if(v>=2u&&v!=0xffu){
                field(0x40000u,18,1u);
                flags2=(flags2&~0x20000u)|0x20000u;
            }break; }
        case anm_th13_layer:
            layer=current->argument<u8>(0);
            flags2=(flags2&~0x40000u)|((layer>=3&&layer<=18)?0x40000u:0);break;
        case anm_th13_layer_flag:
            flags2=(flags2&~0x40000u)|((u32(current->argument<u8>(0))&1u)<<18);break;
        case anm_th13_flag15:
            th13_native_flags=(th13_native_flags&~0x8000u)|((current->argument<u32>(0)&1u)<<15);break;
        case anm_th13_texture_address:
            flags2=(flags2&~3u)|(u32(integer(0))&3u);
            th13_native_flags=(th13_native_flags&0x3fffffffu)|(u32(integer(1))<<30);break;
        case anm_th13_vertex_color_mode:
            th13_native_flags=(th13_native_flags&~0x60000u)|((u32(current->argument<u8>(0))<<17)&0x60000);break;
        case anm_th13_rotation_order:flags2=(flags2&~0x1cu)|((u32(integer(0))<<2)&0x1c);break;
        case anm_th13_flag6:case anm_th13_flag7:case anm_th13_flag8:case anm_th13_parent_independent:{
            const u32 shift=op==anm_th13_flag6?6:op==anm_th13_flag7?7:op==anm_th13_flag8?8:16,mask=1u<<shift;
            const u32 value=op==anm_th13_flag6||op==anm_th13_flag7?current->argument<u8>(0):u32(integer(0));
            flags2=(flags2&~mask)|((value<<shift)&mask);break;
        }
        case anm_th16_rectangle:case anm_th16_gradient_rectangle:case anm_th16_soft_rectangle:case anm_th16_soft_gradient_rectangle:case anm_th16_rectangle_mode26:case anm_th16_rectangle_mode27:
            set_drawing_mode(op==anm_th16_rectangle?16:op==anm_th16_gradient_rectangle?20:op==anm_th16_soft_rectangle?21:op==anm_th16_soft_gradient_rectangle?22:op==anm_th16_rectangle_mode26?26:27);
            sprite_size={float(floating(0)),float(floating(1))};break;
        case anm_th16_ring:case anm_th16_centered_arc:case anm_th16_arc:
            set_drawing_mode(op==anm_th16_ring?9:op==anm_th16_centered_arc?13:14);
            if(!env.allocate_geometry(*this,u32(integer(0))*56u))return -1;break;
        case anm_th13_rectangle:case anm_th13_gradient_rectangle:case anm_th13_soft_rectangle:case anm_th13_soft_gradient_rectangle:
            set_drawing_mode(op==anm_th13_rectangle?15:op==anm_th13_gradient_rectangle?19:op==anm_th13_soft_rectangle?20:21);
            sprite_size={float(floating(0)),float(floating(1))};break;
        case anm_th13_circle:case anm_th13_circle_outline:
            set_drawing_mode(op==anm_th13_circle?16:17);sprite_size.x=float(floating(0));integers[0]=integer(1);break;
        case anm_th16_circle:case anm_th16_circle_outline:
            set_drawing_mode(op==anm_th16_circle?17:18);sprite_size.x=float(floating(0));integers[0]=integer(1);break;
        case anm_th16_ellipse:
            set_drawing_mode(19);sprite_size={float(floating(0)),float(floating(1))};integers[0]=integer(2);break;
        case anm_th13_arc:
            set_drawing_mode(14);if(!env.allocate_geometry(*this,u32(integer(0))*0x38))return -1;break;
        case anm_th14_cylinder:case anm_th14_world_arc:{
            const i32 count=integer(0);if(count<2||count>0x1000000/48)return -1;
            set_drawing_mode(op==anm_th14_cylinder?23:24);if(!env.allocate_geometry(*this,u32(count)*48))return -1;break;
        }
#if PORT_TITLE == 13
        case anm_th13_callback_effect:if(!anm_th13_install_effect(*this,integer(0),env))return -1;break;
#elif PORT_TITLE == 14
        case anm_th13_callback_effect:if(!anm_th14_install_effect(*this,integer(0),env))return -1;break;
#elif PORT_TITLE == 16
        case anm_th13_callback_effect:if(!anm_th16_install_effect(*this,integer(0),env))return -1;break;
#endif
#if PORT_TITLE == 13 || PORT_TITLE == 14
        case anm_th13_texture_scale:texture_scale={float(floating(0)),float(floating(1))};th13_native_flags|=0x10;break;
        case anm_th13_extra_scale:extra_scale={float(floating(0)),float(floating(1))};flags|=8;break;
        case anm_th13_polar_interpolate:{
            auto& v=position_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<i32>(1));
            std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);
            const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);
            const float radius=float(floating(3)),angle=float(floating(2));
            v.end[0]=float(std::cos(double(angle))*radius);v.end[1]=float(std::sin(double(angle))*radius);v.end[2]=0;
            v.timer.set(0,&env.rate);break;
        }
        case anm_th13_texture_scale_interpolate:case anm_th13_extra_scale_interpolate:{
            const float y=float(floating(3)),x=float(floating(2));const i32 duration=integer(0);
            auto& v=op==anm_th13_texture_scale_interpolate?texture_scale_interpolation:extra_scale_interpolation;
            // 464848 passes primary scale as the start even for extra scale.
            const auto from=op==anm_th13_texture_scale_interpolate?texture_scale:scale;
            v.duration=duration;v.mode=InterpolationMode(current->argument<u8>(1));
            std::memcpy(v.start,&from,8);v.end[0]=x;v.end[1]=y;v.initial_tangent[0]=v.initial_tangent[1]=v.final_tangent[0]=v.final_tangent[1]=0;
            v.timer.set(0,&env.rate);break;
        }
#endif
#if PORT_TITLE == 13 || PORT_TITLE == 14 || PORT_TITLE == 16
        case anm_th13_pivot:pivot={float(floating(0)),float(floating(1))};break;
#endif
#if PORT_TITLE == 16
        case anm_th13_polar_interpolate:{
            auto& v=position_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<i32>(1));
            std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);
            const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);
            const float radius=float(floating(3)),angle=float(floating(2));
            v.end[0]=float(std::cos(double(angle))*radius);v.end[1]=float(std::sin(double(angle))*radius);v.end[2]=0;
            v.timer.set(0,&env.rate);break;
        }
        case anm_th13_texture_scale:
            texture_scale={float(floating(0)),float(floating(1))};th13_native_flags|=0x10;break;
        case anm_th13_texture_scale_interpolate:{
            const float y=float(floating(3)),x=float(floating(2));
            auto& v=texture_scale_interpolation;v.duration=integer(0);
            v.mode=InterpolationMode(current->argument<u8>(1));
            std::memcpy(v.start,&texture_scale,8);v.end[0]=x;v.end[1]=y;
            v.timer.set(0,&env.rate);th13_native_flags|=0x10;break;
        }
#endif
        case anm_th14_polar:{
            const float radius=float(floating(3)),angle=float(floating(2));
            const float x=float(std::cos(double(angle))*radius),y=float(std::sin(double(angle))*radius);
            *float_dest(0)=x;*float_dest(1)=y;break;
        }
        case anm_th13_random_polar:{
#if PORT_TITLE == 14
            env.th14_execution_precision()=true;
#endif
            // TH13 0x463497: both draws use the global script RNG, even when
            // the VM selects the visual RNG. The second draw is signed too.
            const float low=float(floating(2)),high=float(floating(3));
            const float angle=float(double(env.script_rng.signed_unit())*3.1415927410125732421875);
            const float random=env.script_rng.signed_unit();
            const float radius=(kPortTitle==14||kPortTitle==16)?float(float(float(high-low)*random)+low):float((double(high)-low)*random+low);
            const float x=float(std::cos(double(angle))*radius),y=float(std::sin(double(angle))*radius);
            *float_dest(0)=x;*float_dest(1)=y;break;
        }
        case anm_th14_sprite:case anm_th14_random_sprite:{
            // 470f50 selects ASCII sprite 258 for every negative index.
            flags|=1;i32 value=integer(0);
            if(op==anm_th14_random_sprite){const u32 range=u32(integer(1));if(!range)return -1;value=wrapping_add(value,signed_bits(env.script_rng.next32()%range));}
            const i32 bound=sprite_callback?sprite_callback(*this,value):value;
            auto* file=bound<0?env.fallback_sprites:resource;
            if(!file||!bind_sprite_from(*file,bound<0?258:bound))return -1;
            sprite_frame=timer.current;break;
        }
#if PORT_TITLE == 14
        case anm_th14_copy_variables:
            if(parent){
                std::memcpy(integers,parent->integers,sizeof(integers));std::memcpy(floats,parent->floats,sizeof(floats));
                std::memcpy(extra_integers,parent->extra_integers,sizeof(extra_integers));extra_floats=parent->extra_floats;
                random_scale=parent->random_scale;random_bound=parent->random_bound;
            }break;
#endif
#if PORT_TITLE == 16
        case anm_th16_copy_variables:
            if(parent){
                std::memcpy(integers,parent->integers,sizeof integers);std::memcpy(floats,parent->floats,sizeof floats);
                std::memcpy(extra_integers,parent->extra_integers,sizeof extra_integers);std::memcpy(th16_variable_tail,parent->th16_variable_tail,sizeof th16_variable_tail);
            }break;
#endif
        case 3:{
            // TH12's title ANM opens scripts 79 and 83 with Sprite(-1): a sentinel
            // meaning "leave this VM without a sprite". bind_sprite() rejects
            // negatives, so the script aborted and title creation failed with
            // "ANM opcode 3". The renderer already treats a null sprite as nothing
            // to draw (AnmRenderer::draw returns -2), so unbinding is safe.
            flags|=1;const i32 value=integer(0);
            const i32 bound=sprite_callback?sprite_callback(*this,value):value;
            if(bound<0){sprite=nullptr;sprite_index=-1;sprite_size.x=sprite_size.y=0;}
            else if(!bind_sprite(bound))return -1;
            sprite_frame=timer.current;break;
        }
        case 4:jump(current->argument<u32>(0),current->argument<i32>(1));continue;
        case 5:{auto* counter=int_dest(0);*counter=wrapping_add(*counter,-1);if(integer(0)>0){jump(current->argument<u32>(1),current->argument<i32>(2));continue;}break;}
        case 40:{const u32 bound=u32(integer(1));auto& rng=(kPortTitle==14||kPortTitle==16)?env.script_rng:env.random(*this);const i32 v=bound?signed_bits(rng.next32()%bound):0;*int_dest(0)=v;break;}
        case 41:{const float max=float(floating(1));
#if PORT_TITLE == 14
            env.th14_execution_precision()=true;
#endif
            auto& rng=(kPortTitle==14||kPortTitle==16)?env.script_rng:env.random(*this);const float value=float(double(rng.unit())*max);*float_dest(0)=value;break;}
        case 42:case 43:case 44:case 45:case 46:{const double v=float(floating(1));double result=op==42?std::sin(v):op==43?std::cos(v):op==44?std::tan(v):op==45?std::acos(v):std::atan(v);*float_dest(0)=float(result);break;}
        case 47:{const float v=float(floating(0));*float_dest(0)=add_angle(v,0);break;}
        case 48:{const float z=float(floating(2)),y=float(floating(1)),x=float(floating(0));(flags&0x100?child_position:script_position)={x,y,z};break;}
        case 49:rotation.x=float(floating(0));rotation.y=float(floating(1));rotation.z=float(floating(2));flags|=4;break;
        case 50:scale.x=float(floating(0));scale.y=float(floating(1));flags|=8;break;
        case 51:color=(color&0xffffff)|(u32(integer(0))<<24);break;
        case 52:case 76:{u32& c=op==52?color:secondary_color;c=(c&~0xff0000u)|((u32(integer(0))&255)<<16);c=(c&~0xff00u)|((u32(integer(1))&255)<<8);c=(c&~255u)|(u32(integer(2))&255);break;}
        case anm_th16_angular_velocity:{
            const float z=float(floating(2)),y=float(floating(1)),x=float(floating(0));
            angular_velocity={x,y,z};flags2|=0x1000000u;break;
        }
        case anm_th16_scale_velocity:{
            const float y=float(floating(1)),x=float(floating(0));
            scale_velocity={x,y};flags2|=0x1000000u;break;
        }
        case anm_th16_scroll_x_velocity:scroll_velocity.x=float(floating(0));flags2|=0x1000000u;break;
        case anm_th16_scroll_y_velocity:scroll_velocity.y=float(floating(0));flags2|=0x1000000u;break;
        case 53:
            if constexpr(kPortTitle==14){
                // Retail47163b resolves Z, Y, X before storing X, Y, Z.
                // Random references consume their own draws in that order.
                const float z=float(floating(2)),y=float(floating(1)),x=float(floating(0));
                angular_velocity={x,y,z};flags2|=0x800000u;
            }else{angular_velocity.x=float(floating(0));angular_velocity.y=float(floating(1));angular_velocity.z=float(floating(2));}
            flags|=4;break;
        case 54:
            if constexpr(kPortTitle==14){
                // Retail4716bf resolves Y before X and enables velocity updates.
                const float y=float(floating(1)),x=float(floating(0));
                scale_velocity={x,y};flags2|=0x800000u;
            }else{scale_velocity.x=float(floating(0));scale_velocity.y=float(floating(1));}
            break;
        case 55:{const i32 duration=integer(1);alpha_start(alpha_interpolation,duration,0,color>>24,current->argument<u8>(0),&env.rate,true);break;}
        case 56:{auto& v=position_interpolation;v.duration=integer(0);std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);v.mode=InterpolationMode(current->argument<i32>(1));const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);const float z=float(floating(4)),y=float(floating(3)),x=float(floating(2));v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);break;}
        // Retail475000/475194 also select secondary vertex-color mode.
        case 57:case 78:{const u32 from=op==57?color:secondary_color;const u32 b=u32(integer(4))&255,g=u32(integer(3))&255,r=u32(integer(2))&255;const i32 duration=integer(0);rgb_start(op==57?color_interpolation:color2_interpolation,duration,current->argument<u8>(1),from,b|(g<<8)|(r<<16),&env.rate);if constexpr(kPortTitle==14)if(op==78)th13_native_flags=(th13_native_flags&~0x60000u)|0x20000u;break;}
        case 58:case 79:{const u8 to=u8(integer(2));const i32 duration=integer(0);alpha_start(op==58?alpha_interpolation:alpha2_interpolation,duration,current->argument<u8>(1),(op==58?color:secondary_color)>>24,to,&env.rate,op==58);if constexpr(kPortTitle==14)if(op==79)th13_native_flags=(th13_native_flags&~0x60000u)|0x20000u;break;}
        case 59:{const float z=float(floating(4)),y=float(floating(3)),x=float(floating(2));auto& v=rotation_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<i32>(1));std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);std::memcpy(v.start,&rotation,12);v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);flags|=4;break;}
        case 60:{const float y=float(floating(3)),x=float(floating(2));auto& v=scale_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<u8>(1));std::memcpy(v.start,&scale,8);v.end[0]=x;v.end[1]=y;v.timer.set(0,&env.rate);flags|=8;break;}
        case 61:flags=(flags^0x200)|8;scale.x=-scale.x;break;
        case 62:flags=(flags^0x400)|8;scale.y=-scale.y;break;
        case 63:case 69:if(op==69)flags&=~1u;if(pending_interrupt){if(dispatch_interrupt(*this,&env.rate))continue;}else flags|=0x1000;timer.advance(-1);return finish();
        case 65:{const u32 v=current->argument<u32>(0);field(0xc0000,18,v&65535);field(0x300000,20,v>>16);break;}
        case 66:field(0x70,4,current->argument<u32>(0));break;
        case 67:set_drawing_mode(current->argument<u32>(0));if(drawing_mode()==10&&!env.change_draw_mode(*this))return -1;break;
        case 68:layer=current->argument<u8>(0);break;
        case 70:scroll_velocity.x=float(floating(0));if constexpr(kPortTitle==14)flags2|=0x800000u;break;
        case 71:scroll_velocity.y=float(floating(0));if constexpr(kPortTitle==14)flags2|=0x800000u;break;
        case 72:field(1,0,current->argument<u32>(0));break;
        case 73:field(0x800,11,current->argument<u32>(0));break;
        case 74:field(0x2000,13,current->argument<u32>(0));break;
        case 75:
            if constexpr(kPortTitle==14){
                // Original opcode6 at4704a7..4704d2 subtracts the requested
                // frame count directly; global slowdown does not scale it.
                const i32 amount=integer(0);timer.previous=timer.current;
                timer.fractional=float(timer.fractional-float(amount));timer.current=truncate_int(timer.fractional);
            }else timer.advance(float(wrapping_sub(0,integer(0))));break;
        case 77:secondary_color=(secondary_color&0xffffff)|(u32(integer(0))<<24);break;
        case 80:field(0x8000,15,current->argument<u8>(0));break;
        case 81:
            // TH14 470132 uses the same integer-frame timer assignment.
            if constexpr(kPortTitle==14)timer.set(saved_timer.current,&env.rate);
            else timer=saved_timer;
            instruction=saved_instruction;continue;
        case 82:field(0x8000000,27,current->argument<u8>(0));break;
        case 83:script_position=position;position={};break;
        case 84:case 101:flags=op==84?(flags&0xfe7fffff)|0x2400000:(flags&0xff7fffff)|0x3400000;if constexpr(kPortTitle==13||kPortTitle==14)set_drawing_mode(op==84?9:13);if(!env.allocate_geometry(*this,u32(integer(0))*0x38))return -1;break;
        case 85:field(0x10000000,28,current->argument<u8>(0));break;
        case 86:field(0x20000000,29,integer(0));break;
        case 87:field(0x40000000,30,current->argument<u8>(0));break;
        case 89:field(0x80000000,31,current->argument<u32>(0));break;
        case 88:case 90:case 91:case 92:case 95:case 96:case 97:{
            auto* v=env.spawn(*this,i16(integer(0)),op);if(!v)return -1;
            if(op!=95&&op!=97){
                // TH13 46fa6a preserves children spawned by the child's first
                // update when the parent's list was empty.
                if((kPortTitle!=13&&kPortTitle!=14&&kPortTitle!=16)||child.next)v->child.next=child.next;
                if(child.next)child.next->previous=&v->child;child.next=&v->child;v->child.previous=&child;
            }
            if constexpr(kPortTitle==13||kPortTitle==14||kPortTitle==16){
                if(op!=95&&op!=97)v->parent=this;
                if(op==96||op==97){v->child_position.x=float(floating(1));v->child_position.y=float(floating(2));}
            }else{
                if(op==96||op==97){v->child_position.x=float(floating(1));v->child_position.y=float(floating(2));}else v->child_position=script_position;v->position=position;
            }break;
        }
        case 93:case 94:{const float to=float(floating(2));const i32 duration=integer(0);auto& v=op==93?scroll_x_interpolation:scroll_y_interpolation;v.duration=duration;v.mode=InterpolationMode(current->argument<i32>(1));v.start[0]=op==93?scroll_velocity.x:scroll_velocity.y;v.end[0]=to;v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,&env.rate);break;}
        case 98:if(!env.screen_uv(*this))return -1;break;
        case 99:{constexpr u32 shift=kPortTitle==13?13:2,mask=1u<<shift;flags2=(flags2&~mask)|((u32(integer(0))<<shift)&mask);break;}
        case 100:{const float ax=float(floating(1)),ay=float(floating(2)),az=float(floating(3)),bx=float(floating(7)),by=float(floating(8)),bz=float(floating(9));auto& v=position_interpolation;v.duration=integer(0);v.mode=InterpolationMode::Hermite;v.initial_tangent[0]=ax;v.initial_tangent[1]=ay;v.initial_tangent[2]=az;v.final_tangent[0]=bx;v.final_tangent[1]=by;v.final_tangent[2]=bz;const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);const float z=float(floating(6)),y=float(floating(5)),x=float(floating(4));v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);break;}
        case 102:{flags|=1;const i32 first=integer(0);const u32 range=u32(integer(1));if(!range)return -1;const i32 value=wrapping_add(first,signed_bits(env.script_rng.next32()%range));if(!bind_sprite(sprite_callback?sprite_callback(*this,value):value))return -1;sprite_frame=timer.current;break;}
        default:break;
        }
        instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(current)+current->length);
    }
    return finish();
}
}
