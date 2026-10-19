#include "rings.hpp"
#include "short_line.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../sprite_renderer/render_state.hpp"
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace s=sprite;namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
void RingGeometry::initialize(std::uint32_t color,bool filled){for(auto& c:colors)c=color;if(filled)colors[0]=0x80ffff80;for(auto& radius:radii)radius=32;}
void RingGeometry::update(float radius,bool filled){
    if(filled)for(auto& c:colors)c=(c&0xff00ffffu)|((static_cast<std::uint32_t>(n::truncate32(n::add32(n::mul32(state::signed_unit(state::random_streams[1]),16),224)))&255u)<<16);
    for(auto& r:radii){r=n::add32(r,n::mul32(state::signed_unit(state::random_streams[1]),2));const float max=filled?n::add32(div(radius,4),radius):n::add32(radius,4);const float min=filled?sub(radius,div(radius,4)):sub(radius,4);if(r>max)r=max;else if(min>r)r=min;}
    if(filled)radii[0]=0;float angle=0;const float step=div(n::mul32(3.1415927410125732f,2),filled?58.f:59.f);
    for(unsigned i=0;i<60;++i){m::polar(positions[i].x,positions[i].y,angle,radii[i]);angle=m::wrap_angle(n::add32(angle,step));}positions[59]=positions[filled?1:0];
}
int draw_triangle_fan(s::Controller& c,int count,const s::Vec3& center,const s::Vec2* points,const std::uint32_t* colors){
    const auto end=reinterpret_cast<std::uintptr_t>(c.colored_write)+(static_cast<std::uint32_t>(count)+1u)*20u;if(end>=reinterpret_cast<std::uintptr_t>(&c)+0x7d40e80u||count<=2)return 0;
    auto* vertices=c.colored_write;auto& device=s::draw_environment::device();s::flush_textured_quads(c,device);float offset_x,offset_y;std::memcpy(&offset_x,&c.fields_c8[2],4);std::memcpy(&offset_y,&c.fields_c8[3],4);
    for(int i=0;i<count;++i){auto& v=vertices[i];v.x=n::add32(n::add32(points[i].x,center.x),offset_x);v.y=n::add32(n::add32(points[i].y,center.y),offset_y);v.z=0;v.rhw=1;v.color=colors[i];}
    c.unknown_cached_e0e=1;s::select_texture_combine(c,device,2);
#ifdef TH_SDL3
    device.vertex_format(touhou::graphics::VertexLayout::ScreenColor);device.draw(touhou::graphics::Topology::Fan,count-2,vertices,20);
#else
    device.SetFVF(0x44);device.DrawPrimitiveUP(D3DPT_TRIANGLEFAN,count-2,vertices,20);
#endif
    c.colored_write+=count;++c.draw_calls;return 0;
}
void RingGeometry::draw(const s::Vec3& p,bool filled){auto& c=s::draw_environment::controller();if(filled)draw_triangle_fan(c,60,p,positions,colors);else draw_polyline(c,60,p,positions,colors);}
template<std::size_t N> RingEffect<N>::RingEffect(s::Animation& a):AttachedCallback(a),rings{},field_after_rings(0),age{},view_index(0){}
template<std::size_t N> int RingEffect<N>::initialize(const Parameters& p,int view){
    view_index=view;field_after_rings=p.value_24;n::timer_set(age,0);
    if constexpr(N==1)rings[0].initialize(0x00ff0000,true);else{rings[0].initialize(0xffff4040,false);rings[1].initialize(0xffc040ff,false);rings[2].initialize(0xffffff40,false);}
    animation->vector_5bc=p.vector_00;s::set_animation_layer(*animation,11);auto& alpha=animation->base.interpolation_134;alpha.duration=30;alpha.mode=0;alpha.start=255;alpha.end=0;alpha.current=255;n::timer_set(alpha.timer,0);animation->base.flags[0]=(animation->base.flags[0]&~0xff00u)|0x100;return 0;
}
template<std::size_t N> std::int32_t RingEffect<N>::update(){if(age.current!=age.previous)for(auto& ring:rings)ring.update(animation->base.vector_50.x,N==1);n::timer_tick(age,state::timer_rate);return 0;}
template<std::size_t N> void RingEffect<N>::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());const auto p=s::animation_position(*animation);for(auto& ring:rings)ring.draw(p,N==1);}
template class RingEffect<1>;template class RingEffect<3>;
namespace {
template<class T> void initialize(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(T),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(T));auto* value=new(memory)T(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
namespace unrecovered {
void __cdecl initialize_7(s::Animation* a,const void* p,int view){initialize<TripleRing>(a,p,view);}
void __cdecl initialize_8(s::Animation* a,const void* p,int view){initialize<FilledRing>(a,p,view);}
}
}
