#include "draw.hpp"
#include "dispatch.hpp"
#include "render_state.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
namespace th20::source::sprite::primitive {
namespace n=th20::recovered;namespace m=ecl::math;namespace e=draw_environment;
namespace {
float f(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
#ifdef TH_SDL3
constexpr DevicePrimitive strip=touhou::graphics::Topology::Strip,fan=touhou::graphics::Topology::Fan,
    line_strip=touhou::graphics::Topology::LineStrip,points=touhou::graphics::Topology::Points;
void depth_func_always(Controller& c){flush_textured_quads(c,e::device());e::device().set_depth_compare(touhou::graphics::Compare::Always);}
#else
constexpr DevicePrimitive strip=strip,fan=fan,line_strip=line_strip,points=points;
void depth_func_always(Controller&){dispatch_environment::set_render_state(23,8);}
#endif
bool space(const Controller& c,std::uint32_t bytes){return reinterpret_cast<std::uint32_t>(c.colored_write)+bytes<reinterpret_cast<std::uint32_t>(&c)+0x7d40e80u;}
void transform(Vertex20& out,float x,float y,float sine,float cosine,float px,float py){out.x=n::add32(sub(n::mul32(x,cosine),n::mul32(y,sine)),px);out.y=n::add32(n::add32(n::mul32(x,sine),n::mul32(y,cosine)),py);}
void draw_colored(Controller& c,DevicePrimitive type,std::uint32_t primitive_count,std::uint32_t vertices){
    e::disable_depth_write();c.unknown_cached_e0e=1;select_texture_combine(c,e::device(),2);
#ifdef TH_SDL3
    e::device().vertex_format(touhou::graphics::VertexLayout::ScreenColor);e::device().draw(type,primitive_count,c.colored_write,20);
#else
    e::device().SetFVF(0x44);e::device().DrawPrimitiveUP(type,primitive_count,c.colored_write,20);
#endif
    c.colored_write+=vertices;++c.draw_calls;
}
void write_polar_vertex(Controller& c,Vertex20& v,float x,float y,float angle,float radius,std::uint32_t color){m::polar(v.x,v.y,angle,radius);v.x=n::add32(v.x,x);v.y=n::add32(v.y,y);v.z=0;v.rhw=1;v.color=color;v.x=n::add32(v.x,f(c.fields_c8[2]));v.y=n::add32(v.y,f(c.fields_c8[3]));}
void write_ellipse_vertex(Controller& c,Vertex20& v,float x,float y,float angle,float rotation,float rx,float ry,std::uint32_t color){float vx=n::mul32(m::cosine(angle),rx),vy=n::mul32(m::sine(angle),ry);m::rotate(vx,vy,rotation);v.x=n::add32(vx,x);v.y=n::add32(vy,y);v.z=0;v.rhw=1;v.color=color;v.x=n::add32(v.x,f(c.fields_c8[2]));v.y=n::add32(v.y,f(c.fields_c8[3]));}
void finish_curve(Controller& c,DevicePrimitive type,std::uint32_t count,std::uint32_t vertices,bool depth){
    if(depth){e::disable_depth_write();depth_func_always(c);}
    c.unknown_cached_e0e=1;select_texture_combine(c,e::device(),2);
#ifdef TH_SDL3
    e::device().vertex_format(touhou::graphics::VertexLayout::ScreenColor);e::device().draw(type,count,c.colored_write,20);
#else
    e::device().SetFVF(0x44);e::device().DrawPrimitiveUP(type,count,c.colored_write,20);
#endif
    c.colored_write=reinterpret_cast<Vertex20*>(reinterpret_cast<std::uint32_t>(c.colored_write)+vertices*20);++c.draw_calls;
}
void rounded_vertex(Controller& c,Vertex20& v,float x,float y,float width,float height,float angle,float rotation,float radius,std::uint32_t color,std::int32_t index,std::uint32_t count){
    m::polar(v.x,v.y,angle,radius);v.z=0;v.rhw=1;
    const float half_x=div(width,2.f),half_y=div(height,2.f);
    if(index<static_cast<std::int32_t>(count)/4||static_cast<std::uint32_t>(index)==count){v.x=sub(v.x,half_x);v.y=sub(v.y,half_y);}
    else if(index<static_cast<std::int32_t>(count*2)/4){v.x=n::add32(half_x,v.x);v.y=sub(v.y,half_y);}
    else if(index<static_cast<std::int32_t>(count*3)/4){v.x=n::add32(half_x,v.x);v.y=n::add32(half_y,v.y);}
    else if(index<static_cast<std::int32_t>(count)){v.x=sub(v.x,half_x);v.y=n::add32(half_y,v.y);}
    v.color=color;m::rotate(v.x,v.y,rotation);v.x=n::add32(v.x,x);v.y=n::add32(v.y,y);v.x=n::add32(v.x,f(c.fields_c8[2]));v.y=n::add32(v.y,f(c.fields_c8[3]));
}
}
void p439a00(Controller& c,float x,float y,float width,float height,float angle,std::uint32_t primary,std::uint32_t secondary,std::int32_t ax,std::int32_t ay){
    if(!space(c,80))return;auto* v=c.colored_write;flush_textured_quads(c,e::device());const float sine=m::sine(angle),cosine=m::cosine(angle);
    float x0=0,x1=0,y0=0,y1=0;
    if(ax==0){x0=n::mul32(-width,.5f);x1=n::mul32(width,.5f);}else if(ax==1)x1=width;else if(ax==2)x0=-width;
    if(ay==0){y0=n::mul32(-height,.5f);y1=n::mul32(height,.5f);}else if(ay==1)y1=height;else if(ay==2)y0=-height;
    transform(v[0],x0,y0,sine,cosine,x,y);transform(v[1],x1,y0,sine,cosine,x,y);transform(v[2],x0,y1,sine,cosine,x,y);transform(v[3],x1,y1,sine,cosine,x,y);
    for(unsigned i=0;i<4;++i){v[i].z=0;v[i].x=n::add32(v[i].x,f(c.fields_c8[2]));v[i].y=n::add32(v[i].y,f(c.fields_c8[3]));v[i].rhw=1.f;v[i].color=(i&1)?secondary:primary;}
    draw_colored(c,strip,2,4);
}
std::int32_t p43a1c0(Controller& c,float x,float y,float width,float height,float angle,std::uint32_t primary,std::uint32_t secondary,std::int32_t ax,std::int32_t ay){
    p439a00(c,x,y,n::add32(width,1.f),n::add32(height,1.f),angle,(primary&0xffffffu)|((primary>>25)<<24),(secondary&0xffffffu)|((secondary>>25)<<24),ax,ay);
    p439a00(c,x,y,width,height,angle,primary,secondary,ax,ay);return 0;
}
std::int32_t p43d830(Controller& c,float x,float y,std::uint32_t color){
    if(space(c,0)){auto& v=*c.colored_write;flush_textured_quads(c,e::device());v.x=n::add32(x,f(c.fields_c8[2]));v.y=n::add32(y,f(c.fields_c8[3]));v.z=0;v.rhw=1;v.color=color;draw_colored(c,points,1,1);}return 0;
}
std::int32_t p43b960(Controller& c,float x,float y,float radius,float angle,std::uint32_t raw_count,std::uint32_t primary,std::uint32_t secondary){
    const auto count=static_cast<std::int32_t>(raw_count);if(!space(c,(raw_count+2)*20))return 0;
    auto* vertices=c.colored_write;flush_textured_quads(c,e::device());vertices[0]={n::add32(x,f(c.fields_c8[2])),n::add32(y,f(c.fields_c8[3])),0,1,primary};
    const float tau=n::mul32(3.1415927410125732421875f,2.f);
    const float step=div(tau,static_cast<float>(count));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(raw_count+1);++i){auto& v=vertices[i+1];m::polar(v.x,v.y,angle,radius);v.x=n::add32(v.x,x);v.y=n::add32(v.y,y);v.z=0;v.rhw=1;v.color=secondary;v.x=n::add32(v.x,f(c.fields_c8[2]));v.y=n::add32(v.y,f(c.fields_c8[3]));angle=m::wrap_angle(n::add32(angle,step));}
    e::disable_depth_write();depth_func_always(c);c.unknown_cached_e0e=1;select_texture_combine(c,e::device(),2);
#ifdef TH_SDL3
    e::device().vertex_format(touhou::graphics::VertexLayout::ScreenColor);e::device().draw(fan,raw_count,c.colored_write,20);
#else
    e::device().SetFVF(0x44);e::device().DrawPrimitiveUP(fan,raw_count,c.colored_write,20);
#endif
    c.colored_write=reinterpret_cast<Vertex20*>(reinterpret_cast<std::uint32_t>(c.colored_write)+(raw_count+2)*20);++c.draw_calls;return 0;
}
std::int32_t p43bc60(Controller& c,float x,float y,float radius,float angle,std::uint32_t count,std::uint32_t color){
    if(!space(c,(count+1)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_polar_vertex(c,c.colored_write[i],x,y,angle,radius,color);angle=m::wrap_angle(n::add32(angle,step));}
    finish_curve(c,line_strip,count,count+1,false);return 0;
}
std::int32_t p43be80(Controller& c,float x,float y,float radius,float width,float angle,std::uint32_t count,std::uint32_t inner_color,std::uint32_t outer_color){
    if(!space(c,(count*2+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float half_width=div(width,2.f),step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_polar_vertex(c,c.colored_write[i*2],x,y,angle,sub(radius,half_width),inner_color);write_polar_vertex(c,c.colored_write[i*2+1],x,y,angle,n::add32(half_width,radius),outer_color);angle=m::wrap_angle(n::add32(angle,step));}
    finish_curve(c,strip,count<<1,count*2+2,false);return 0;
}
void p43c180(Controller& c,float x,float y,float rx,float ry,float angle,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return;flush_textured_quads(c,e::device());
    c.colored_write[0]={n::add32(x,f(c.fields_c8[2])),n::add32(y,f(c.fields_c8[3])),0,1,primary};float theta=angle;
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_ellipse_vertex(c,c.colored_write[i+1],x,y,theta,angle,rx,ry,secondary);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,fan,count,count+2,true);
}
void p43c4e0(Controller& c,float x,float y,float rx,float ry,float angle,std::uint32_t count,std::uint32_t color,std::uint32_t /* unused secondary color */){
    // The original reserves count+2 but advances by count+1.
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return;flush_textured_quads(c,e::device());float theta=angle;
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_ellipse_vertex(c,c.colored_write[i],x,y,theta,angle,rx,ry,color);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,line_strip,count,count+1,true);
}
void p43c780(Controller& c,float x,float y,float rx,float ry,float width,float angle,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    // Original gate reserves count+2 vertices although it writes count*2+2.
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return;flush_textured_quads(c,e::device());float theta=angle;
    const float half_width=div(width,2.f),step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_ellipse_vertex(c,c.colored_write[i*2],x,y,theta,angle,sub(rx,half_width),sub(ry,half_width),primary);write_ellipse_vertex(c,c.colored_write[i*2+1],x,y,theta,angle,n::add32(half_width,rx),n::add32(half_width,ry),secondary);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,strip,count<<1,count*2+2,true);
}
std::int32_t p43d9b0(Controller& c,float x,float y,float first_radius,float second_radius,float angle,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    c.colored_write[0]={n::add32(x,f(c.fields_c8[2])),n::add32(y,f(c.fields_c8[3])),0,1,primary};
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_polar_vertex(c,c.colored_write[i+1],x,y,angle,(i&1)?second_radius:first_radius,secondary);angle=m::wrap_angle(n::add32(angle,step));}
    finish_curve(c,fan,count,count+2,true);return 0;
}
std::int32_t p43dce0(Controller& c,float x,float y,float first_radius,float second_radius,float angle,std::uint32_t count,std::uint32_t color){
    if(!space(c,(count+1)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){write_polar_vertex(c,c.colored_write[i],x,y,angle,(i&1)?second_radius:first_radius,color);angle=m::wrap_angle(n::add32(angle,step));}
    finish_curve(c,line_strip,count,count+1,false);return 0;
}
std::int32_t p43df30(Controller& c,float x,float y,float first_radius,float second_radius,float width,float angle,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    if(!space(c,(count*2+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float half_width=div(width,2.f),step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){const float radius=(i&1)?second_radius:first_radius;write_polar_vertex(c,c.colored_write[i*2],x,y,angle,sub(radius,half_width),primary);write_polar_vertex(c,c.colored_write[i*2+1],x,y,angle,n::add32(half_width,radius),secondary);angle=m::wrap_angle(n::add32(angle,step));}
    finish_curve(c,strip,count<<1,count*2+2,false);return 0;
}
void p43cd40(Controller& c,float x,float y,float length,float angle,std::uint32_t primary,std::uint32_t secondary,std::int32_t anchor,std::int32_t /* unused y anchor */){
    if(!space(c,40))return;auto* v=c.colored_write;flush_textured_quads(c,e::device());const float sine=m::sine(angle),cosine=m::cosine(angle);
    float first=0,last=0;if(anchor==0){first=n::mul32(-length,.5f);last=n::mul32(length,.5f);}else if(anchor==1)last=length;else if(anchor==2)first=-length;
    transform(v[0],first,0,sine,cosine,x,y);transform(v[1],last,0,sine,cosine,x,y);
    for(unsigned i=0;i<2;++i){v[i].z=0;v[i].x=n::add32(v[i].x,f(c.fields_c8[2]));v[i].y=n::add32(v[i].y,f(c.fields_c8[3]));v[i].color=i?secondary:primary;}
    // Original 43cfdf..43d005 also sets RHW in three following unused vertices.
    for(unsigned i=0;i<5;++i)v[i].rhw=1;
    draw_colored(c,line_strip,1,2);
}
void p43e2c0(Controller& c,float x,float y,float width,float height,float angle,std::uint32_t primary,std::uint32_t secondary,std::int32_t ax,std::int32_t ay){
    if(!space(c,60))return;auto* v=c.colored_write;flush_textured_quads(c,e::device());const float sine=m::sine(angle),cosine=m::cosine(angle);
    float x0=0,x1=0,y0=0,y1=0,y2=0;
    if(ax==0){x0=div(-width,3.f);x1=div(n::mul32(width,2.f),3.f);}else if(ax==1)x1=width;else if(ax==2)x0=-width;
    if(ay==0){y0=n::mul32(-height,.5f);y2=n::mul32(height,.5f);}else if(ay==1){y1=div(height,2.f);y2=height;}else if(ay==2){y0=-height;y1=div(-height,2.f);}
    transform(v[0],x0,y0,sine,cosine,x,y);transform(v[1],x1,y1,sine,cosine,x,y);transform(v[2],x0,y2,sine,cosine,x,y);
    for(unsigned i=0;i<3;++i){v[i].z=0;v[i].x=n::add32(v[i].x,f(c.fields_c8[2]));v[i].y=n::add32(v[i].y,f(c.fields_c8[3]));v[i].rhw=1;v[i].color=i==1?secondary:primary;}
    draw_colored(c,strip,1,3);
}
void p43b0e0(Controller& c,float x,float y,float width,float height,float angle,std::uint32_t primary,std::uint32_t secondary,std::int32_t ax,std::int32_t ay){
    if(!space(c,100))return;auto* v=c.colored_write;flush_textured_quads(c,e::device());const float sine=m::sine(angle),cosine=m::cosine(angle);
    float x0=0,x1=0,y0=0,y1=0;
    if(ax==0){x0=n::mul32(-width,.5f);x1=n::mul32(width,.5f);}else if(ax==1)x1=width;else if(ax==2)x0=-width;
    if(ay==0){y0=n::mul32(-height,.5f);y1=n::mul32(height,.5f);}else if(ay==1)y1=height;else if(ay==2)y0=-height;
    transform(v[0],x0,y0,sine,cosine,x,y);transform(v[1],x1,y0,sine,cosine,x,y);transform(v[3],x0,y1,sine,cosine,x,y);transform(v[2],x1,y1,sine,cosine,x,y);v[4].x=v[0].x;v[4].y=v[0].y;
    for(unsigned i=0;i<5;++i){v[i].z=0;v[i].x=n::add32(v[i].x,f(c.fields_c8[2]));v[i].y=n::add32(v[i].y,f(c.fields_c8[3]));v[i].rhw=1;v[i].color=i==1||i==2?secondary:primary;}
    draw_colored(c,line_strip,4,5);
}
std::int32_t p43a2b0(Controller& c,float x,float y,float width,float height,float radius,float rotation,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    c.colored_write[0]={n::add32(x,f(c.fields_c8[2])),n::add32(y,f(c.fields_c8[3])),0,1,primary};
    const float inner_width=sub(width,n::mul32(radius,2.f)),inner_height=sub(height,n::mul32(radius,2.f));float theta=-3.1415927410125732421875f;
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){rounded_vertex(c,c.colored_write[i+1],x,y,inner_width,inner_height,theta,rotation,radius,secondary,i,count);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,fan,count,count+2,true);return 0;
}
std::int32_t p43a750(Controller& c,float x,float y,float width,float height,float radius,float rotation,std::uint32_t count,std::uint32_t color){
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float inner_width=sub(width,n::mul32(radius,2.f)),inner_height=sub(height,n::mul32(radius,2.f));float theta=-3.1415927410125732421875f;
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){rounded_vertex(c,c.colored_write[i],x,y,inner_width,inner_height,theta,rotation,radius,color,i,count);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,line_strip,count,count+1,true);return 0;
}
std::int32_t p43ab20(Controller& c,float x,float y,float width,float height,float radius,float thickness,float rotation,std::uint32_t count,std::uint32_t primary,std::uint32_t secondary){
    // Original gate counts count+2 although this function writes twice as many edge vertices.
    if(!space(c,(count+2)*20)||static_cast<std::int32_t>(count)<=1)return 0;flush_textured_quads(c,e::device());
    const float half_width=div(thickness,2.f);float theta=-3.1415927410125732421875f;
    const float step=div(n::mul32(3.1415927410125732421875f,2.f),static_cast<float>(static_cast<std::int32_t>(count)));
    for(std::int32_t i=0;i<static_cast<std::int32_t>(count+1);++i){rounded_vertex(c,c.colored_write[i*2],x,y,width,height,theta,rotation,sub(radius,half_width),primary,i,count);rounded_vertex(c,c.colored_write[i*2+1],x,y,width,height,theta,rotation,n::add32(radius,half_width),secondary,i,count);theta=m::wrap_angle(n::add32(theta,step));}
    finish_curve(c,strip,count<<1,count*2+2,true);return 0;
}
}
