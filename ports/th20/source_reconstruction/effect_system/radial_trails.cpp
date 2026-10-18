#include "radial_trails.hpp"
#include "short_line.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../sprite_renderer/render_state.hpp"
#include <bit>
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace n=recovered;namespace m=ecl::math;namespace s=sprite;
void RadialTrail::initialize(float angle,float angular_step,float length,std::uint32_t color){
    const float step=_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(length),_mm_set_ss(30)));std::uint8_t alpha=0;
    for(unsigned i=0;i<30;++i){colors[i]=color;if(i<8){colors[i]=(colors[i]&0xffffff)|(std::uint32_t(alpha)<<24);alpha=static_cast<std::uint8_t>(alpha+32);}else if(i>13){alpha=static_cast<std::uint8_t>(alpha-8);colors[i]=(colors[i]&0xffffff)|(std::uint32_t(alpha)<<24);}}
    for(auto& p:positions){m::polar(p.x,p.y,angle,length);angle=n::add32(angle,angular_step);length=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(length),_mm_set_ss(step)));}
}
void RadialTrail::draw(const s::Vec3& center,int start,int count){draw_polyline(s::draw_environment::controller(),count,center,positions+start,colors+start);}
RadialTrails::RadialTrails(s::Animation& a):AttachedCallback(a),trails{},age{}{}
int RadialTrails::initialize(const Parameters& p,int){
    n::timer_set(age,1);float angle=m::wrap_angle(std::bit_cast<float>(p.value_18)),angular_step=m::wrap_angle(std::bit_cast<float>(p.value_1c));
    const float step=_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(n::mul32(3.1415927410125732f,2)),_mm_set_ss(32)));
    for(auto& trail:trails){trail.initialize(angle,angular_step,std::bit_cast<float>(p.value_24),p.value_20);angle=m::wrap_angle(n::add32(angle,step));angular_step=m::wrap_angle(n::mul32(angular_step,-1));}
    animation->vector_5bc=p.vector_00;s::set_animation_layer(*animation,5);animation->base.flags[0]&=~0xff00u;return 0;
}
std::int32_t RadialTrails::update(){if(age.current>=60)return 1;n::timer_tick(age,state::timer_rate);return 0;}
void RadialTrails::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());const auto p=s::animation_position(*animation);
    const int count=age.current<30?age.current:n::signed_bits(60u-static_cast<std::uint32_t>(age.current));const int start=age.current<30?0:n::signed_bits(static_cast<std::uint32_t>(age.current)-30u);for(auto& trail:trails)trail.draw(p,start,count);
}
namespace unrecovered {
void __cdecl initialize_11(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(RadialTrails),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(RadialTrails));auto* value=new(memory)RadialTrails(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
