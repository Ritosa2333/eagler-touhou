#include "short_line.hpp"
#include "../ecl_vm/math.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/render_state.hpp"
#include "../sprite_renderer/quad.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::effects {
namespace n=th20::recovered;namespace s=sprite;namespace m=ecl::math;
namespace {constexpr float pi=3.1415927410125732421875f;float div32(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
template<std::size_t N> ShortLine<N>::ShortLine(s::Animation& a):AttachedCallback(a),positions{},colors{},angle(0),age{}{}
template<std::size_t N> int ShortLine<N>::initialize(const Parameters& p,std::int32_t){
    positions[0]={};angle=n::mul32(state::signed_unit(state::random_streams[1]),pi);n::timer_set(age,1);
    for(unsigned i=0;i<N;++i){colors[i]=p.value_20;if(i>=N/2)colors[i]=(colors[i]&0xffffffu)|((255u-(i-N/2)*(N==20?24u:8u))<<24);}
    animation->vector_5bc=p.vector_00;s::set_animation_layer(*animation,N==20?19:5);reinterpret_cast<std::uint8_t*>(&animation->base.flags[0])[1]=0;return 0;
}
template<std::size_t N> std::int32_t ShortLine<N>::update(){
    if(age.current>=static_cast<std::int32_t>(N))return 1;
    if(age.current!=age.previous){
        // Original indexes the arrays with this counter. A negative counter is
        // outside its valid lifetime and would write before the positions array.
        if(age.current<0)throw std::out_of_range("ShortLine negative sample index");
        const auto current=static_cast<unsigned>(age.current);
        for(unsigned i=0;i<current;++i){const auto alpha=colors[i]>>24;colors[i]=(colors[i]&0xffffffu)|((alpha<16?0u:alpha-16u)<<24);}
        const auto length=n::add32(n::mul32(state::unit(state::random_streams[1]),N==64?5.f:8.f),N==64?4.f:1.f);
        m::polar(positions[current].x,positions[current].y,angle,length);
        if(current){positions[current].x=n::add32(positions[current].x,positions[current-1].x);positions[current].y=n::add32(positions[current].y,positions[current-1].y);}
        const auto delta=div32(n::mul32(state::signed_unit(state::random_streams[1]),pi),N==20?1.f:N==64?5.f:8.f);
        angle=m::wrap_angle(n::add32(angle,delta));
    }
    n::timer_tick(age,state::timer_rate);return 0;
}
template<std::size_t N> void ShortLine<N>::draw(){auto& c=s::draw_environment::controller();s::apply_animation_render_state(c,*animation,s::draw_environment::device());draw_polyline(c,age.current,s::animation_position(*animation),positions,colors);}
int draw_polyline(s::Controller& c,std::int32_t count,const s::Vec3& center,const s::Vec2* positions,const std::uint32_t* colors){
    const auto end=reinterpret_cast<std::uint32_t>(c.colored_write)+(static_cast<std::uint32_t>(count)+1u)*20u;
    if(end>=reinterpret_cast<std::uint32_t>(&c)+0x7d40e80u||count<=1)return 0;
    auto* vertices=c.colored_write;auto& device=s::draw_environment::device();s::flush_textured_quads(c,device);
    float offset_x,offset_y;std::memcpy(&offset_x,&c.fields_c8[2],4);std::memcpy(&offset_y,&c.fields_c8[3],4);
    for(int i=0;i<count;++i){auto& v=vertices[i];v.x=n::add32(positions[i].x,center.x);v.y=n::add32(positions[i].y,center.y);v.z=0;v.rhw=1;v.color=colors[i];v.x=n::add32(v.x,offset_x);v.y=n::add32(v.y,offset_y);}
    c.unknown_cached_e0e=1;s::select_texture_combine(c,device,2);
#ifdef TH_SDL3
    device.vertex_format(touhou::graphics::VertexLayout::ScreenColor);device.draw(touhou::graphics::Topology::LineStrip,static_cast<std::uint32_t>(count-1),vertices,20);
#else
    device.SetFVF(0x44);device.DrawPrimitiveUP(D3DPT_LINESTRIP,static_cast<UINT>(count-1),vertices,20);
#endif
    c.colored_write+=count;++c.draw_calls;return 0;
}
template class ShortLine<20>;template class ShortLine<30>;template class ShortLine<64>;
namespace {
void initialize_long_line(LongLine& line,const Parameters& p,bool orange){
    line.positions[0]={};line.angle=n::mul32(state::signed_unit(state::random_streams[1]),pi);n::timer_set(line.age,1);
    for(unsigned i=0;i<64;++i){auto color=orange?0xff0080ffu:0xff505050u;
        if(i<8){const auto channel=255u-(i<<5);color=orange?(color&0xff00ffffu)|(channel<<16):(color&0xffffff00u)|channel;}
        if(i>=32)color=(color&0xffffffu)|((255u-(i-32)*16u)<<24); // uint8 alpha wraps after entry47, as in original
        line.colors[i]=color;
    }
    line.animation->vector_5bc=p.vector_00;s::set_animation_layer(*line.animation,orange?15:19);reinterpret_cast<std::uint8_t*>(&line.animation->base.flags[0])[1]=orange?1:0;
}
}
int LongLine::initialize_gray(const Parameters& p,std::int32_t){initialize_long_line(*this,p,false);return 0;}
int LongLine::initialize(const Parameters& p,std::int32_t view){view_index=view;initialize_long_line(*this,p,true);
    auto& alpha=animation->base.interpolation_134;alpha.duration=64;alpha.mode=0;alpha.start=255;alpha.end=0;alpha.current=255;n::timer_set(alpha.timer,0);return 0;
}
namespace unrecovered {
template<class T> void create_short_line(s::Animation* a,const void* p,int view){auto* storage=::operator new(sizeof(T),std::nothrow);if(!storage)throw std::bad_alloc();std::memset(storage,0,sizeof(T));auto* object=new(storage)T(*a);object->initialize(*static_cast<const Parameters*>(p),view);}
void __cdecl initialize_4(s::Animation* a,const void* p,int view){create_short_line<ShortLine20>(a,p,view);}
void __cdecl initialize_10(s::Animation* a,const void* p,int view){create_short_line<ShortLine30>(a,p,view);}
void __cdecl initialize_2(s::Animation* a,const void* p,int view){create_short_line<LongLine>(a,p,view);}
void __cdecl initialize_3(s::Animation* a,const void* p,int view){auto* storage=::operator new(sizeof(LongLine),std::nothrow);if(!storage)throw std::bad_alloc();std::memset(storage,0,sizeof(LongLine));auto* object=new(storage)LongLine(*a);object->initialize_gray(*static_cast<const Parameters*>(p),view);}
}
}
