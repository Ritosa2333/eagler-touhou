#include "rounded_panel.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../sprite_renderer/render_state.hpp"
#include "../runtime_state/state.hpp"
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace s=sprite;namespace n=th20::recovered;
namespace {float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
RoundedPanel::RoundedPanel(s::Animation& a):AttachedCallback(a),target_position{},dimensions{},color(0),field_24(0),age{},remaining{}{}
int RoundedPanel::initialize(const Parameters& p,std::int32_t){target_position=p.vector_00;dimensions=p.vector_0c;color=p.value_20;n::timer_set(age,0);s::set_animation_layer(*animation,35);reinterpret_cast<std::uint8_t*>(&animation->base.flags[3])[0]=2;return 0;}
std::int32_t RoundedPanel::update(){if(remaining.current>0){n::timer_add(remaining,-1.f,state::timer_rate);if(remaining.current<=0)return -1;}n::timer_tick(age,state::timer_rate);return 0;}
void RoundedPanel::draw(){auto& a=*animation;const float scale=s::anm_environment::screen_scale();const float width=n::mul32(n::mul32(n::mul32(a.base.vector_50.x,dimensions.x),scale),.5f),height=n::mul32(n::mul32(n::mul32(a.base.vector_50.y,dimensions.y),scale),.5f);
    auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,a,s::draw_environment::device());const auto position=s::animation_position(a);
    s::primitive::p43a2b0(c,position.x,position.y,width,height,8.f,0.f,64,color,color);
    s::primitive::p43ab20(c,position.x,position.y,sub(width,8.f),sub(height,8.f),8.f,6.f,0.f,32,0xff808080u,0xffc0c0c0u); //461286/46128b exact ARGB, not floating NaNs
}
int RoundedPanel::on_interrupt(std::int32_t value){if(value==1){auto& a=*animation;auto& scale=a.base.interpolation_1e0;scale.duration=20;scale.mode=4;scale.start=a.base.vector_50;scale.end={};scale.current=scale.start;n::timer_set(scale.timer,0);
    auto& position=a.base.interpolation_8c;position.duration=20;position.mode=4;position.start=a.base.vector_2c;position.end=target_position;position.current=position.start;n::timer_set(position.timer,0);n::timer_set(remaining,20);
    }return 0;
}
namespace unrecovered {
void __cdecl initialize_13(s::Animation* a,const void* p,std::int32_t index){auto* storage=::operator new(sizeof(RoundedPanel),std::nothrow);if(!storage)throw std::bad_alloc();std::memset(storage,0,sizeof(RoundedPanel));auto* object=new(storage)RoundedPanel(*a);object->initialize(*static_cast<const Parameters*>(p),index);}
}
}
