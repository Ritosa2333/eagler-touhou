#include "special.hpp"
#include "../ecl_vm/math.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
#include <cstring>
#include <emmintrin.h>
namespace th20::source::special_state {
namespace n=recovered;namespace m=ecl::math;namespace ps=gameplay::player_state;
namespace {
float sub(float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float div(float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float cubic(float p0,float p1,float p2,float p3,float t,bool byte_order){
    const auto add=n::add32,mul=n::mul32;
    if(byte_order){
        const float a=mul(mul(mul(p0,sub(t,1)),sub(t,1)),add(mul(2,t),1));
        const float b=mul(mul(mul(p1,t),t),sub(3,mul(2,t)));
        const float c=mul(mul(mul(p2,sub(1,t)),sub(1,t)),t);
        const float d=mul(mul(mul(p3,sub(t,1)),t),t);
        return add(add(add(a,b),c),d);
    }
    const float a=mul(p0,mul(mul(sub(t,1),sub(t,1)),add(mul(2,t),1)));
    const float b=mul(p1,mul(mul(t,t),sub(3,mul(2,t))));
    const float c=mul(p2,mul(mul(sub(1,t),sub(1,t)),t));
    const float d=mul(p3,mul(mul(sub(t,1),t),t));
    return add(add(add(a,b),c),d);
}
int clamp(game_session::Player& p,unsigned offset,int lo,int hi){const int value=std::clamp(ps::read<int>(p,offset),lo,hi);ps::write(p,offset,value);return value;}
}
void construct(ByteInterpolation& p) noexcept{p.start=p.end=p.tangent_start=p.tangent_end=p.current=0;p.timer={};p.duration=p.mode=0;}
void construct(Entry& p) noexcept{p.link={};p.link.value=reinterpret_cast<scheduler::Node*>(&p);p.enemy_handle=p.animation_handle=0;p.age={};}
void construct(Controller& p) noexcept{scheduler::initialize_list(p.entries);p.age={};p.charge_age={};p.text_expansion={};construct(p.text_alpha);p.meter_remaining={};p.active=0;}
void start(ByteInterpolation& p,int duration,int mode,std::uint8_t from,std::uint8_t to){p.duration=duration;p.mode=mode;p.start=from;p.end=to;p.current=from;n::timer_set(p.timer,0);}
std::uint8_t evaluate(ByteInterpolation& p){
    if(p.mode==7){p.start=std::uint8_t(p.start+p.end);p.current=p.start;}
    else if(p.mode==17){p.start=std::uint8_t(p.start+p.tangent_end);p.tangent_end=std::uint8_t(p.tangent_end+p.end);p.current=p.start;}
    else if(p.mode==8){const float t=div(p.timer.current_f,n::int_float(p.duration));p.current=std::uint8_t(n::truncate32(cubic(float(p.start),float(p.end),float(p.tangent_start),float(p.tangent_end),t,false)));}
    else p.current=std::uint8_t(n::truncate32(n::add32(n::mul32(n::int_float(int(p.end)-int(p.start)),m::easing(p.mode,p.timer.current_f,n::int_float(p.duration))),float(p.start))));
    return p.current;
}
std::uint8_t sample(ByteInterpolation& p,const float* rate){
    if(p.duration>0){n::timer_tick(p.timer,rate);if(p.timer.current>=p.duration){n::timer_set(p.timer,p.duration);p.duration=0;return p.mode==7||p.mode==17?p.start:p.end;}}
    else if(p.duration==0)return p.mode==7||p.mode==17?p.start:p.end;
    return evaluate(p);
}
float evaluate(FloatInterpolation& p){
    if(p.mode==7){p.start=n::add32(p.start,p.end);p.current=p.start;}
    else if(p.mode==17){p.start=n::add32(p.start,p.tangent_end);p.tangent_end=n::add32(p.tangent_end,p.end);p.current=p.start;}
    else if(p.mode==8)p.current=cubic(p.start,p.end,p.tangent_start,p.tangent_end,div(p.timer.current_f,n::int_float(p.duration)),false);
    else p.current=n::add32(n::mul32(sub(p.end,p.start),m::easing(p.mode,p.timer.current_f,n::int_float(p.duration))),p.start);
    return p.current;
}
void start(FloatInterpolation& p,int duration,int mode,float from,float to){p.duration=duration;p.mode=mode;p.start=from;p.end=to;p.current=from;n::timer_set(p.timer,0);}
float sample(FloatInterpolation& p,const float* rate){
    if(p.duration>0){n::timer_tick(p.timer,rate);if(p.timer.current>=p.duration){n::timer_set(p.timer,p.duration);p.duration=0;return p.mode==7||p.mode==17?p.start:p.end;}}
    else if(p.duration==0)return p.mode==7||p.mode==17?p.start:p.end;
    return evaluate(p);
}
unsigned color(Controller& p,game_session::Player& player){
    if(p.active&&p.entries.sentinel.next)return static_cast<unsigned>(reinterpret_cast<Entry*>(p.entries.sentinel.next->value)->color);
    int best=-1;unsigned selected=0;
    for(unsigned i=0;i<4;++i)if(clamp(player,0x64+i*4,0,1000)>best){if(i!=3)best=clamp(player,0x64+i*4,0,1000);selected=i;}
    return selected;
}
}
