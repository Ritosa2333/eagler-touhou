#include "vm.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
namespace th20::source::background {
namespace n=th20::recovered;namespace m=th20::source::ecl::math;
namespace {
constexpr float pi=std::bit_cast<float>(0x40490fdbu);
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float neg(float a){return std::bit_cast<float>(std::bit_cast<std::uint32_t>(a)^0x80000000u);}
float angle(float time,float period){return div(n::mul32(n::mul32(time,pi),2.f),period);}
}
void update_camera_motion(ScriptState& s,const float* rate){
    auto& timer=s.motion_timer;auto& secondary=s.secondary_motion_timer;
    auto& offset=s.camera.vectors[5];auto& target=s.camera.vectors[6];auto& up=s.camera.vectors[2];
    auto tick=[&](int limit,int reset){n::timer_tick(timer,rate);if(timer.current>=limit)n::timer_set(timer,reset);};
    switch(s.camera_motion){
    case 1:{
        const float amplitude=timer.current>=512?1.f:div(timer.current_f,512.f);
        const float a=m::wrap_angle(angle(timer.current_f,1024.f));
        offset[0]=n::mul32(n::mul32(m::sine(a),-10.f),amplitude);
        offset[2]=n::mul32(n::mul32(m::sine(m::wrap_angle(n::mul32(a,2.f))),-10.f),amplitude);
        up[0]=n::mul32(n::mul32(neg(m::sine(a)),.01f),amplitude);
        target[0]=div(neg(offset[0]),2.f);target[2]=div(neg(offset[2]),2.f);tick(2048,1024);break;
    }
    case 2:case 4:{
        const float a=m::wrap_angle(angle(timer.current_f,n::mul32(512.f,6.f)));
        up[0]=n::mul32(neg(m::sine(a)),1.f);
        up[2]=n::mul32(s.camera_motion==4?neg(m::cosine(a)):m::cosine(a),1.f);tick(3072,0);break;
    }
    case 3:case 6:{
        const float a=m::wrap_angle(angle(n::int_float(timer.current%1024),1024.f));
        const float amplitude=s.camera_motion==3?
            (timer.current>=1024?80.f:div(n::mul32(timer.current_f,80.f),1024.f)):
            (secondary.current>=512?0.f:n::mul32(sub(1.f,div(secondary.current_f,512.f)),80.f));
        offset[0]=n::mul32(m::sine(a),amplitude);offset[1]=0.f;offset[2]=0.f;target[0]=div(neg(offset[0]),10.f);
        n::timer_tick(timer,rate);if(s.camera_motion==6)n::timer_tick(secondary,rate);if(timer.current>=2048)n::timer_set(timer,1024);break;
    }
    case 5:{
        const float a=m::wrap_angle(angle(n::int_float(timer.current%1024),1024.f));
        const float amplitude=timer.current<512?div(timer.current_f,512.f):1.f;
        offset[2]=n::mul32(n::mul32(m::sine(a),-50.f),amplitude);tick(2048,1024);break;
    }
    case 7:{
        const float a=m::wrap_angle(angle(timer.current_f,4800.f));
        offset[0]=n::mul32(m::sine(a),1200.f);offset[1]=n::mul32(neg(m::cosine(a)),1200.f);offset[2]=n::mul32(m::cosine(a),-40.f);
        target[0]=n::mul32(m::sine(a),-512.f);target[1]=n::mul32(neg(m::cosine(a)),-512.f);
        target[2]=n::add32(n::mul32(m::cosine(m::wrap_angle(n::mul32(a,2.f))),110.f),100.f);tick(4800,0);break;
    }
    case 8:{
        const float a=m::wrap_angle(sub(angle(timer.current_f,800.f),pi));
        offset[0]=n::mul32(m::sine(a),-30.f);offset[1]=n::mul32(m::sine(a),-15.f);offset[2]=n::mul32(m::sine(a),-20.f);
        up[0]=n::mul32(neg(m::sine(a)),.1f);tick(800,0);break;
    }
    case 9:{
        float a=m::wrap_angle(angle(timer.current_f,2000.f));if(a>=pi)a=m::wrap_angle(sub(a,n::mul32(pi,2.f)));
        offset[0]=n::mul32(m::sine(a),5.f);offset[1]=n::mul32(m::sine(a),-15.f);
        up[0]=n::mul32(neg(m::sine(a)),1.f);up[1]=n::mul32(m::cosine(a),1.f);tick(2000,0);break;
    }
    case 11:{
        const float a=sub(angle(timer.current_f,2048.f),pi);
        offset[0]=n::mul32(m::sine(a),70.f);offset[2]=n::mul32(m::sine(m::wrap_angle(n::mul32(a,2.f))),200.f);
        up[0]=n::mul32(neg(m::sine(a)),.1f);tick(2048,0);break;
    }
    case 12:case 13:{
        const bool last=s.camera_motion==13;const float a=sub(angle(timer.current_f,last?512.f:1024.f),pi);
        offset[0]=n::mul32(m::sine(a),last?-15.f:-50.f);up[0]=n::mul32(neg(m::sine(a)),last?.01f:.1f);tick(last?512:1024,0);break;
    }
    // Mode 10, zero and values outside this switch preserve camera and timers.
    }
}
}
