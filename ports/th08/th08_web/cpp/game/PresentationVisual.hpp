#pragma once
#include "AnmLayout.hpp"
#include "Presentation.hpp"
#include <algorithm>
#include <cmath>

namespace th08::presentation {
// Visual endpoints live beside their owner, never in the original ANM/game
// layouts. Position ownership is opt-in; no script is executed by sampling.
struct VisualSample {
    enum Field : u32 { Position=1,Offset=2,Scale=4,Rotation=8,Uv=16,Rgb=32,Opacity=64,Rgb2=128,Opacity2=256,
                       Attributes=Scale|Rotation|Uv|Rgb|Opacity|Rgb2|Opacity2 };
    Vec3 pos{},pos2{},rotation{};Vec2 scale{1,1},uv{};
    ZunColor color1{},color2{};AnmLoaded* file=nullptr;AnmRawInstr* script_start=nullptr;
    i16 script=-1,sprite=-1;bool visible=false,secondary=false;u32 continuous=0;
    VisualSample()=default;
    explicit VisualSample(const AnmVm& vm){capture(vm);}
    static u32 continuous_fields(const AnmVm& vm){
        u32 mask=0;
        const auto timed=[&](AnmInterp p){return vm.interpEndTimers[p].current>0;};
        if(timed(AnmInterp_Pos))mask|=vm.usePosOffset?Offset:Position;
        if(timed(AnmInterp_Scale)||vm.scaleGrowth.x!=0||vm.scaleGrowth.y!=0)mask|=Scale;
        if(timed(AnmInterp_Rotate)||vm.angleVel.x!=0||vm.angleVel.y!=0||vm.angleVel.z!=0)mask|=Rotation;
        if(vm.uvScrollVel.x!=0||vm.uvScrollVel.y!=0)mask|=Uv;
        if(timed(AnmInterp_RGB1))mask|=Rgb;
        if(timed(AnmInterp_Alpha1))mask|=Opacity;
        if(timed(AnmInterp_RGB2))mask|=Rgb2;
        if(timed(AnmInterp_Alpha2))mask|=Opacity2;
        return mask;
    }
    void capture(const AnmVm& vm){
        pos=vm.pos;pos2=vm.pos2;scale=vm.scale;rotation=vm.rotation;uv=vm.uvScrollPos;
        color1=vm.color1;color2=vm.color2;file=vm.anmFile;script_start=vm.beginningOfScript;script=vm.scriptIndex;sprite=vm.activeSpriteIndex;
        visible=vm.visible;secondary=vm.flag17;continuous=continuous_fields(vm);
    }
    bool matches(const AnmVm& vm)const{return file&&file==vm.anmFile&&script==vm.scriptIndex&&script_start==vm.beginningOfScript&&visible&&vm.visible;}
    // AddU/AddV instructions author a continuous texture translation by
    // accumulating uvScrollPos directly; they intentionally leave
    // uvScrollVel at zero. The owner may smooth a small shortest-path step
    // only while the same ANM/script/sprite lifecycle remains active.
    u32 authored_uv_fields(const AnmVm& current,float max_step=.125f)const{
        if(sprite!=current.activeSpriteIndex||!matches(current))return 0;
        const float du=std::remainder(current.uvScrollPos.x-uv.x,1.0f),dv=std::remainder(current.uvScrollPos.y-uv.y,1.0f);
        return std::fabs(du)<=max_step&&std::fabs(dv)<=max_step?Uv:0;
    }
    static float mix(float a,float b,float t){return a+(b-a)*t;}
    static float cyclic(float a,float b,float t,float period){
        if(t>=1)return b;if(t<=0)return a;
        return a+std::remainder(b-a,period)*t;
    }
    static Vec3 vector(const Vec3& a,const Vec3& b,float t){return {mix(a.x,b.x,t),mix(a.y,b.y,t),mix(a.z,b.z,t)};}
    static bool near(const Vec3& a,const Vec3& b){const float x=b.x-a.x,y=b.y-a.y,z=b.z-a.z;return x*x+y*y+z*z<16384.f;}
    void apply(const AnmVm& current,AnmVm& draw,float t,u32 fields=Attributes,u32 owner_continuous=0)const{
        if(!render_only||!active||t>=1||!matches(current))return;
#if defined(TH_PRESENTATION_AUDIT)
        if(audit_hold_interpolation)return;
#endif
        const u32 mask=fields&(continuous|continuous_fields(current)|owner_continuous);
        if((mask&Position)&&near(pos,current.pos))draw.pos=vector(pos,current.pos,t);
        if((mask&Offset)&&near(pos2,current.pos2))draw.pos2=vector(pos2,current.pos2,t);
        // Reflections are authored discrete changes, not shrink-through-zero.
        if((mask&Scale)&&scale.x*current.scale.x>=0&&scale.y*current.scale.y>=0){draw.scale={mix(scale.x,current.scale.x,t),mix(scale.y,current.scale.y,t)};draw.updateScale=true;}
        if(mask&Rotation){constexpr float tau=6.2831854820251465f;draw.rotation={cyclic(rotation.x,current.rotation.x,t,tau),cyclic(rotation.y,current.rotation.y,t,tau),cyclic(rotation.z,current.rotation.z,t,tau)};draw.updateRotation=true;}
        if(mask&Uv)draw.uvScrollPos={cyclic(uv.x,current.uvScrollPos.x,t,1),cyclic(uv.y,current.uvScrollPos.y,t,1)};
        const auto byte=[&](u8 a,u8 b){return u8(std::clamp(std::lround(mix(a,b,t)),0l,255l));};
        if(mask&Rgb){draw.color1.r=byte(color1.r,current.color1.r);draw.color1.g=byte(color1.g,current.color1.g);draw.color1.b=byte(color1.b,current.color1.b);}
        if(mask&Opacity)draw.color1.a=byte(color1.a,current.color1.a);
        if(mask&Rgb2){draw.color2.r=byte(color2.r,current.color2.r);draw.color2.g=byte(color2.g,current.color2.g);draw.color2.b=byte(color2.b,current.color2.b);}
        if(mask&Opacity2)draw.color2.a=byte(color2.a,current.color2.a);
    }
    // Some owners derive color2 from color1 and switch the submitted channel
    // with flag17 (for example enemy hit/youkai tint).  Keep the tint switch
    // discrete, but preserve an authored alpha fade across the active channel.
    void apply_active_opacity(const AnmVm& current,AnmVm& draw,float t,u32 evidence=Opacity,u32 previous_submitted_factor=128,u32 current_submitted_factor=128)const{
        if(!render_only||!active||t>=1||!matches(current)||!((continuous|continuous_fields(current))&evidence))return;
#if defined(TH_PRESENTATION_AUDIT)
        if(audit_hold_interpolation)return;
#endif
        const u8 from=secondary?color2.a:color1.a,to=current.flag17?current.color2.a:current.color1.a;
        const auto submit=[](u8 value,u32 factor){return std::min(u32(value)*factor/128,255u);};
        const u32 target=u32(std::clamp(std::lround(mix(float(submit(from,previous_submitted_factor)),float(submit(to,current_submitted_factor)),t)),0l,255l));
        u32 value=target;
        if(current_submitted_factor!=128){const u32 base=std::min(255u,(target*128+current_submitted_factor/2)/current_submitted_factor);value=base;u32 error=u32(std::abs(i32(submit(u8(base),current_submitted_factor))-i32(target)));
            for(i32 delta=-1;delta<=1;++delta){const u32 candidate=u32(std::clamp(i32(base)+delta,0,255));const u32 candidate_error=u32(std::abs(i32(submit(u8(candidate),current_submitted_factor))-i32(target)));if(candidate_error<error){value=candidate;error=candidate_error;}}
        }
        if(draw.flag17)draw.color2.a=value;else draw.color1.a=value;
    }
};
}
