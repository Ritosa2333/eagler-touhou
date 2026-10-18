#include "spiral_trail.hpp"
#include "short_line.hpp"
#include "../ecl_vm/math.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/render_state.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../player_entity/player.hpp"
#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace m=ecl::math;
namespace {
constexpr float pi=3.1415927410125732f;
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
std::uint8_t random_byte(){auto& r=state::random_streams[1];std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(10));r.last=n::lcg_next(r.state);return static_cast<std::uint8_t>(r.last);} //449d20 uses raw step, no modulus
float random_angle(){return n::mul32(state::signed_unit(state::random_streams[1]),pi);} //4297d0
void set_alpha(std::uint32_t& color,std::uint32_t value){color=(color&0xffffffu)|((value&255u)<<24);}
void finalize_animation(s::Animation& a,const Parameters& p){a.vector_5bc=p.vector_00;s::set_animation_layer(a,5);auto& alpha=a.base.interpolation_134;alpha.duration=80;alpha.mode=0;alpha.start=255;alpha.end=0;alpha.current=255;n::timer_set(alpha.timer,0);}
void track_player(s::Animation& a){auto* player=game_session::context(0).objects_04[0];if(player)a.vector_5bc=player_entity::position(player);}
}
SpiralTrail::SpiralTrail(s::Animation& a):AttachedCallback(a),positions{},colors{},angle(0),turn(0),age{},view_index(0){}
ReverseSpiralTrail::ReverseSpiralTrail(s::Animation& a):SpiralTrail(a),origin{},follow_player(1){}
int SpiralTrail::initialize(const Parameters& p,int view){
    view_index=view;positions[0]={};turn=std::bit_cast<float>(p.value_1c);angle=std::bit_cast<float>(p.value_18);n::timer_set(age,1);
    for(std::uint32_t i=0;i<80;++i){const auto channel=((80-i)*3)&255u;colors[i]=0xff0000u|channel|(std::uint32_t(random_byte())<<8)|(channel<<24);if(i>39)set_alpha(colors[i],255-(i-40)*32);if(i<8)set_alpha(colors[i],i*32);}
    finalize_animation(*animation,p);return 0;
}
std::int32_t SpiralTrail::update(){
    if(age.current>=80)return 1;if(age.current!=age.previous){const auto i=age.current;if(i<0)throw std::out_of_range("SpiralTrail negative sample index");
        if(i>49)for(auto& color:colors){const float alpha=n::int_float(color>>24);const auto reduction=n::truncate32(div(alpha,n::add32(n::mul32(state::unit(state::random_streams[1]),4),8)));set_alpha(color,(color>>24)-static_cast<std::uint32_t>(reduction));}
        if(i<16){m::polar(positions[i].x,positions[i].y,angle,8);angle=m::wrap_angle(n::add32(angle,sub(turn,div(n::mul32(n::int_float(i),turn),100))));}
        else{const float length=n::add32(n::mul32(state::unit(state::random_streams[1]),1),4);m::polar(positions[i].x,positions[i].y,angle,length);const float change=sub(turn,n::mul32(random_angle(),.10000000149011612f));angle=m::wrap_angle(n::add32(angle,change));}
        if(i>0){positions[i].x=n::add32(positions[i].x,positions[i-1].x);positions[i].y=n::add32(positions[i].y,positions[i-1].y);}track_player(*animation);
    }
    n::timer_tick(age,state::timer_rate);return 0;
}
void SpiralTrail::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());draw_polyline(c,age.current,s::animation_position(*animation),positions,colors);}
int ReverseSpiralTrail::initialize(const Parameters& p,int view){
    view_index=view;positions[0]={};turn=std::bit_cast<float>(p.value_1c);origin=p.vector_00;follow_player=p.enabled;angle=std::bit_cast<float>(p.value_18);n::timer_set(age,1);std::uint32_t fade=0;
    for(std::uint32_t i=0;i<80;++i){const auto j=79-i,channel=((80-i)*3)&255u;colors[j]=0xff0000u|channel|(std::uint32_t(random_byte())<<8)|(channel<<24);if(j<41)set_alpha(colors[j],255-fade++*32);if(j<8)set_alpha(colors[j],i*32);
        if(j<64){const float length=n::add32(n::mul32(state::unit(state::random_streams[1]),1),4);m::polar(positions[j].x,positions[j].y,angle,length);const float change=sub(turn,n::mul32(random_angle(),.10000000149011612f));angle=m::wrap_angle(n::add32(angle,change));}
        else{m::polar(positions[j].x,positions[j].y,angle,8);angle=m::wrap_angle(n::add32(angle,turn));}
        if(j<79){positions[j].x=n::add32(positions[j].x,positions[j+1].x);positions[j].y=n::add32(positions[j].y,positions[j+1].y);}
    }
    finalize_animation(*animation,p);return 0;
}
std::int32_t ReverseSpiralTrail::update(){
    if(age.current>=80)return 1;if(age.current!=age.previous){if(age.current>69)for(auto& color:colors){const float alpha=n::int_float(color>>24);const auto reduction=n::truncate32(div(alpha,n::add32(n::mul32(state::unit(state::random_streams[1]),2),2)));set_alpha(color,(color>>24)-static_cast<std::uint32_t>(reduction));}if(follow_player)track_player(*animation);}
    n::timer_tick(age,state::timer_rate);return 0;
}
namespace {
template<class T> void initialize(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(T),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(T));auto* value=new(memory)T(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
namespace unrecovered {
void __cdecl initialize_5(s::Animation* a,const void* p,int view){initialize<SpiralTrail>(a,p,view);}
void __cdecl initialize_6(s::Animation* a,const void* p,int view){initialize<ReverseSpiralTrail>(a,p,view);}
}
}
