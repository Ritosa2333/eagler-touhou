#include "wavering_trail.hpp"
#include "short_line.hpp"
#include "../ecl_vm/math.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/render_state.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../player_entity/player.hpp"
#include <bit>
#include <cmath>
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace m=ecl::math;
namespace {
constexpr float pi=3.1415927410125732f;
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
s::Vec2 scaled_direction(s::Vec2 v,float scale){ //4678c0: normalize unless length is strictly below .01 (NaN takes division)
    const float length=static_cast<float>(std::sqrt(static_cast<double>(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)))));
    if(!(.009999999776482582f>std::fabs(length))){v.x=div(v.x,length);v.y=div(v.y,length);}
    return {n::mul32(v.x,scale),n::mul32(v.y,scale)};
}
}
WaveringTrail::WaveringTrail(s::Animation& a):AttachedCallback(a),positions{},colors{},angle(0),amplitude(0),age{},view_index(0),extent{},current{}{}
int WaveringTrail::initialize(const Parameters& p,int view){
    view_index=view;positions[0]={};angle=m::wrap_angle(std::bit_cast<float>(p.value_18));n::timer_set(age,1);
    std::uint8_t alpha=255;for(auto& color:colors){alpha-=8;color=(p.value_20&0xffffffu)|(std::uint32_t(alpha)<<24);}for(auto& point:positions)point={};
    auto& a=*animation;a.vector_5bc=p.vector_00;
    extent={n::mul32(n::mul32(s::anm_environment::screen_scale(),p.vector_2c.x),.5f),n::mul32(n::mul32(s::anm_environment::screen_scale(),p.vector_2c.y),.5f)};current={};
    s::set_animation_layer(a,5);auto& fade=a.base.interpolation_134;fade.duration=30;fade.mode=0;fade.start=255;fade.end=0;fade.current=255;n::timer_set(fade.timer,0);
    s::set_animation_layer(a,34);reinterpret_cast<std::uint8_t*>(&a.base.flags[3])[0]=2;reinterpret_cast<std::uint8_t*>(&a.base.flags[0])[1]=1;
    amplitude=n::add32(n::mul32(state::unit(state::random_streams[1]),16),8);angle=m::wrap_angle(n::mul32(state::signed_unit(state::random_streams[1]),pi));return 0;
}
std::int32_t WaveringTrail::update(){
    if(age.current>=60)return 1;
    if(age.current!=age.previous){
        std::memmove(positions+1,positions,29*sizeof(s::Vec2)); //467230 is shift_right, not rotate
        if(age.current<=30){current.x=n::add32(current.x,div(extent.x,30));current.y=n::add32(current.y,div(extent.y,30));}
        float envelope=1;if(age.current<6)envelope=div(age.current_f,6);else if(age.current>30)envelope=0;else if(age.current>=24)envelope=div(n::int_float(30-age.current),6);
        const float sine=static_cast<float>(std::sin(static_cast<double>(angle)));
        const float value=n::mul32(n::mul32(n::mul32(sine,amplitude),envelope),n::mul32(s::anm_environment::screen_scale(),.5f));
        const auto offset=scaled_direction({extent.y,extent.x},value);
        if(age.current<=30)angle=m::wrap_angle(n::add32(angle,div(pi,15)));
        positions[0]={n::add32(current.x,offset.x),n::add32(current.y,offset.y)};
    }
    const auto& player=player_entity::position(game_session::context(0).objects_04[0]);
    if(player.x<-168&&player.y>320)for(auto& color:colors)if((color>>24)>=32)color=(color&0xffffffu)|0x20000000u;
    n::timer_tick(age,state::timer_rate);return 0;
}
void WaveringTrail::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());draw_polyline(c,30,s::animation_position(*animation),positions,colors);}
namespace unrecovered {
void __cdecl initialize_14(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(WaveringTrail),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(WaveringTrail));auto* value=new(memory)WaveringTrail(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
