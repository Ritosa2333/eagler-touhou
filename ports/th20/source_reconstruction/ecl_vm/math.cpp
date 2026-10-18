#include "math.hpp"
#include <cmath>
#include <emmintrin.h>
namespace th20::source::ecl::math {
namespace {
constexpr float pi=3.1415927410125732421875f;
float a(float x,float y){return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float s(float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float m(float x,float y){return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float d(float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float power(float x,unsigned n){float result=x;for(unsigned i=1;i<n;++i)result=m(result,x);return result;}
float shifted_quadratic(float t,float k){
    const auto denominator=m(s(1,k),s(1,k));
    const auto offset=d(m(k,k),denominator);
    return d(s(d(m(s(t,k),s(t,k)),denominator),offset),s(1,offset));
}
}
float sine(float v){return static_cast<float>(std::sin(static_cast<double>(v)));}
float cosine(float v){return static_cast<float>(std::cos(static_cast<double>(v)));}
float square_root(float v){return static_cast<float>(std::sqrt(static_cast<double>(v)));}
float arctangent(float y,float x){return static_cast<float>(std::atan2(static_cast<double>(y),static_cast<double>(x)));}
float wrap_angle(float value){
    if(value>pi){unsigned count=0;do{value=s(value,m(pi,2));if(count>32)break;++count;}while(value>pi);}
    else if(value < -pi){unsigned count=0;do{value=a(m(pi,2),value);if(count>32)break;++count;}while(value < -pi);}
    return value;
}
float angle_difference(float first,float second){
    if(s(first,second)>pi)return s(first,a(m(pi,2),second));
    if(s(second,first)>pi)return s(first,s(second,m(pi,2)));
    return s(first,second);
}
void polar(float& x,float& y,float angle,float length){x=m(cosine(angle),length);y=m(sine(angle),length);}
void rotate(float& x,float& y,float angle){const auto sn=sine(angle),cs=cosine(angle),oldx=x,oldy=y;y=a(m(oldy,cs),m(oldx,sn));x=s(m(oldx,cs),m(oldy,sn));}
float easing(std::int32_t mode,float time,float duration){
    if(duration==0)return 1;auto t=d(time,duration);
    if(mode>=1&&mode<=3)return power(t,static_cast<unsigned>(mode+1));
    if(mode>=4&&mode<=6)return s(1,power(s(1,t),static_cast<unsigned>(mode-2)));
    if(mode>=9&&mode<=11){t=m(t,2);const auto n=static_cast<unsigned>(mode-7);t=t>=1?s(2,power(s(2,t),n)):power(t,n);return d(t,2);}
    if(mode>=12&&mode<=14){t=m(t,2);const auto n=static_cast<unsigned>(mode-10);return t>=1?a(d(power(s(t,1),n),2),.5f):s(.5f,d(power(s(1,t),n),2));}
    switch(mode){
    case 15:return 0;case 16:return 1;
    case 18:return sine(d(m(t,pi),2));
    case 19:return s(1,sine(a(d(m(t,pi),2),d(pi,2))));
    case 20:t=m(t,2);return t>=1?a(d(s(1,sine(d(m(t,pi),2))),2),.5f):d(sine(d(m(t,pi),2)),2);
    case 21:t=m(t,2);return t>=1?a(d(sine(d(m(s(t,1),pi),2)),2),.5f):d(s(1,sine(a(d(m(t,pi),2),d(pi,2)))),2);
    default:break;
    }
    constexpr float offsets[]={.25f,.3f,.35f,.38f,.4f};
    if(mode>=22&&mode<=26)return shifted_quadratic(t,offsets[mode-22]);
    if(mode>=27&&mode<=31)return s(1,shifted_quadratic(s(1,t),offsets[mode-27]));
    return t;
}
float Interpolator::sample(const float* rate){
    if(duration>0){
        th20::recovered::timer_tick(timer,rate);
        if(timer.current>=duration){th20::recovered::timer_set(timer,duration);duration=0;return mode==7||mode==17?start:end;}
    } else if(duration==0)return mode==7||mode==17?start:end;
    if(mode==7){start=a(start,end);current=start;}
    else if(mode==17){start=a(start,tangent_end);tangent_end=a(tangent_end,end);current=start;}
    else if(mode==8){
        const auto t=d(timer.current_f,static_cast<float>(duration));
        const auto term0=m(start,m(m(s(t,1),s(t,1)),a(m(2,t),1)));
        const auto term1=m(end,m(m(t,t),s(3,m(2,t))));
        const auto term2=m(tangent_start,m(m(s(1,t),s(1,t)),t));
        const auto term3=m(tangent_end,m(m(s(t,1),t),t));
        current=a(a(a(term0,term1),term2),term3);
    } else current=a(m(s(end,start),easing(mode,timer.current_f,static_cast<float>(duration))),start);
    return current;
}
}
