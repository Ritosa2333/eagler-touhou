#include "burst_rings.hpp"
#include "rings.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../sprite_renderer/render_state.hpp"
#include <cmath>
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
s::Vec2 minus(const s::Vec2& a,const s::Vec2& b){return {sub(a.x,b.x),sub(a.y,b.y)};}
void add(s::Vec2& a,const s::Vec2& b){a.x=n::add32(a.x,b.x);a.y=n::add32(a.y,b.y);}
}
void BurstRingGeometry::initialize(std::uint32_t color){for(auto& c:colors)c=color;colors[0]=0;for(auto& r:radii)r=32;}
void BurstRingGeometry::update(float radius){
    radii[0]=0;m::polar(positions[0].x,positions[0].y,0,radii[0]);
    for(unsigned i=1;i<62;++i){auto& r=radii[i];r=n::add32(r,n::mul32(state::signed_unit(state::random_streams[1]),2));const float max=n::add32(radius,4),min=sub(radius,4);if(r>max)r=max;else if(min>r)r=min;}
    float angle=0;const float step=div(n::mul32(3.1415927410125732f,2),59);
    for(unsigned i=1;i<61;++i){m::polar(positions[i].x,positions[i].y,angle,radii[i]);angle=m::wrap_angle(n::add32(angle,step));}positions[60]=positions[1];
}
int draw_thick_polyline(s::Controller& c,int count,const s::Vec3& center,const s::Vec2* p,const std::uint32_t* colors,float width,bool closed){
    const auto end=reinterpret_cast<std::uintptr_t>(c.colored_write)+(static_cast<std::uint32_t>(count)+1u)*20u;if(end>=reinterpret_cast<std::uintptr_t>(&c)+0x7d40e80u||count<=1)return 0;
    auto* vertices=c.colored_write;auto& device=s::draw_environment::device();s::flush_textured_quads(c,device);float offset_x,offset_y;std::memcpy(&offset_x,&c.fields_c8[2],4);std::memcpy(&offset_y,&c.fields_c8[3],4);const float half=div(width,2);
    for(int i=0;i<count;++i){s::Vec2 d;
        if(i==0){d=minus(p[1],p[0]);if(closed)add(d,minus(p[0],p[count-1]));}
        else if(i==count-1){d=minus(p[count-1],p[count-2]);if(closed){d=minus(p[1],p[0]);add(d,minus(p[0],p[count-1]));}}
        else{d=minus(p[i],p[i-1]);add(d,minus(p[i+1],p[i]));}
        m::rotate(d.x,d.y,div(-3.1415927410125732f,2));const float length=m::square_root(n::add32(n::mul32(d.x,d.x),n::mul32(d.y,d.y)));if(std::fabs(length)>=.009999999776482582f){d.x=div(d.x,length);d.y=div(d.y,length);}d.x=n::mul32(d.x,half);d.y=n::mul32(d.y,half);
        const float x=n::add32(p[i].x,center.x),y=n::add32(p[i].y,center.y);auto& a=vertices[i*2];auto& b=vertices[i*2+1];a={n::add32(n::add32(x,d.x),offset_x),n::add32(n::add32(y,d.y),offset_y),0,1,colors[i]};b={n::add32(sub(x,d.x),offset_x),n::add32(sub(y,d.y),offset_y),0,1,colors[i]};
    }
    c.unknown_cached_e0e=1;s::select_texture_combine(c,device,2);
#ifdef TH_SDL3
    device.vertex_format(touhou::graphics::VertexLayout::ScreenColor);device.draw(touhou::graphics::Topology::Strip,count*2-2,vertices,20);
#else
    device.SetFVF(0x44);device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,count*2-2,vertices,20);
#endif
    //Original advances by count despite writing2*count vertices (draw already
    //consumed the data). Preserve its buffer cursor behavior.
    c.colored_write+=count;++c.draw_calls;return 0;
}
BurstRings::BurstRings(s::Animation& a):AttachedCallback(a),rings{},field_b84(0),age{},view_index(0),origin{},follow_animation(1){}
int BurstRings::initialize(const Parameters& p,int view){
    view_index=view;field_b84=p.value_24;origin=p.vector_00;follow_animation=p.enabled;n::timer_set(age,0);rings[0].initialize(0x8040f010);rings[1].initialize(0xff20f000);rings[2].initialize(0x80ff1010);
    animation->vector_5bc=p.vector_00;s::set_animation_layer(*animation,11);auto& alpha=animation->base.interpolation_134;alpha.duration=30;alpha.mode=0;alpha.start=255;alpha.end=0;alpha.current=255;n::timer_set(alpha.timer,0);animation->base.flags[0]=(animation->base.flags[0]&~0xff00u)|0x100;return 0;
}
std::int32_t BurstRings::update(){if(age.current!=age.previous)for(auto& ring:rings)ring.update(animation->base.vector_50.x);n::timer_tick(age,state::timer_rate);return 0;}
void BurstRings::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());const auto p=follow_animation?s::animation_position(*animation):origin;for(auto& ring:rings)draw_thick_polyline(c,60,p,ring.positions+1,ring.colors+1,16,true);draw_triangle_fan(c,61,p,rings[0].positions,rings[0].colors);}
namespace unrecovered {
void __cdecl initialize_9(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(BurstRings),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(BurstRings));auto* value=new(memory)BurstRings(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
