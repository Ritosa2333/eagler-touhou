#include "quad.hpp"
#include "anm_vm.hpp"
#include "binding.hpp"
#include "render_state.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <cstring>
namespace th20::source::sprite {
namespace n=th20::recovered;namespace m=ecl::math;
Vertex28 animation_quad[4]{{0,0,0,0,0xffffffffu,0,0},{0,0,0,0,0xffffffffu,0,0},{0,0,0,0,0xffffffffu,0,0},{0,0,0,0,0xffffffffu,0,0}};
Vec3 animation_position_scratch{};
namespace {
float f(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float inherited_scale(Animation& a,unsigned selector){float parent=1.f;if(a.fields_550[2] && !(a.base.flags[1]&0x1000))parent=inherited_scale(*reinterpret_cast<Animation*>(a.fields_550[2]),selector);const auto own=selector==0?a.base.vector_50.x:(selector==1?a.base.vector_50.y:a.base.vector_58.x);return n::mul32(own,parent);}
std::uint32_t color_multiply(std::uint32_t a,std::uint32_t b,bool normalized){std::uint32_t result=0;for(unsigned shift=0;shift<32;shift+=8){const auto product=((a>>shift)&255)*((b>>shift)&255);auto byte=normalized?product/255:product>>7;if(byte>255)byte=255;result|=byte<<shift;}return result;}
}
float animation_scale_x(Animation& a){return inherited_scale(a,0);}float animation_scale_y(Animation& a){return inherited_scale(a,1);}float animation_aux_scale_x(Animation& a){return inherited_scale(a,2);}
float animation_width(Animation& a){return n::mul32(animation_scale_x(a),a.base.vector_70.x);}float animation_height(Animation& a){return n::mul32(animation_scale_y(a),a.base.vector_70.y);}
float round_sprite_coordinate(float value){std::uint32_t bits;std::memcpy(&bits,&value,4);const auto magnitude=bits&0x7fffffffu;if(magnitude>0x4affffffu)return value;if(magnitude>0x3f7fffffu){const auto shift=0x95u-(magnitude>>23);const auto high=bits>>shift;return f((high+(high&1))<<shift);}return f(magnitude<0x3f000000u?(bits&0x80000000u):((bits&0xbf800000u)|0x3f800000u));}
void calculate_sprite_corners(Animation& a,const SpriteData& sprite,Vec3* p0,Vec3* p1,Vec3* p2,Vec3* p3,bool rotated) {
    constexpr float horizontal[3][4]{{-.5f,.5f,-.5f,.5f},{0,1,0,1},{-1,0,-1,0}};
    constexpr float vertical[3][4]{{-.5f,-.5f,.5f,.5f},{0,0,1,1},{-1,-1,0,0}};
    float sine=0,cosine=1;if(rotated){const float angle=m::wrap_angle(n::add32(inherited_animation_rotation(a).z,f(sprite.fields_24[4])));sine=m::sine(angle);cosine=m::cosine(angle);}
    const auto xanchor=a.base.flags[4],yanchor=a.base.flags[5];
    const float dx=sub(f(sprite.fields_24[0]),a.base.vector_80.x),dy=sub(f(sprite.fields_24[1]),a.base.vector_80.y);
    float sx=n::mul32(a.base.vector_50.x,a.base.vector_58.x),sy=n::mul32(a.base.vector_50.y,a.base.vector_58.y);
    if(a.fields_550[3] && !(a.base.flags[1]&0x1000)){auto& parent=*reinterpret_cast<Animation*>(a.fields_550[3]);sx=n::mul32(n::mul32(parent.base.vector_50.x,parent.base.vector_58.x),sx);sy=n::mul32(n::mul32(parent.base.vector_50.y,parent.base.vector_58.y),sy);}
    const auto mode=a.base.flags[3]&255;
    if(mode==1||mode==5){sx=n::mul32(sx,anm_environment::screen_scale());sy=n::mul32(sy,anm_environment::screen_scale());}
    else if(mode==2||mode==6){sx=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),sx);sy=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),sy);}
    Vec3* points[4]{p0,p1,p2,p3};float x[4],y[4];
    for(unsigned i=0;i<4;++i){x[i]=n::mul32(n::mul32(sub(n::mul32(horizontal[xanchor][i],a.base.vector_70.x),dx),f(sprite.fields_24[2])),sx);y[i]=n::mul32(n::mul32(sub(n::mul32(vertical[yanchor][i],a.base.vector_70.y),dy),f(sprite.fields_24[3])),sy);}
    const auto position=animation_position(a);
    if(!rotated){animation_position_scratch=position;for(unsigned i=0;i<4;++i){points[i]->x=n::add32(x[i],position.x);points[i]->y=n::add32(y[i],position.y);points[i]->z=n::add32(points[i]->z,position.z);}}
    else {for(unsigned i=0;i<4;++i){points[i]->x=n::add32(sub(n::mul32(x[i],cosine),n::mul32(y[i],sine)),position.x);points[i]->y=n::add32(n::add32(n::mul32(x[i],sine),n::mul32(y[i],cosine)),position.y);}}
    const float z=n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z);for(auto* p:points)p->z=z;
}
void calculate_animation_corners(Animation& a,Vec3(&positions)[4]) {
    const auto type=a.base.flags[0]&255;if(type<=3)calculate_sprite_corners(a,current_sprite(draw_environment::controller(),a),positions,positions+1,positions+2,positions+3,type==1);
}
void submit_animation_quad(Controller& c,Animation& a,Vertex28(&v)[4],std::uint32_t flags) {
    const float dx=n::add32(f(c.fields_c8[0]),f(c.fields_c8[2])),dy=n::add32(f(c.fields_c8[1]),f(c.fields_c8[3]));
    for(auto& p:v){p.x=n::add32(p.x,dx);p.y=n::add32(p.y,dy);}
    if(flags&1){v[0].x=sub(round_sprite_coordinate(v[0].x),.5f);v[2].x=v[0].x;v[0].y=sub(round_sprite_coordinate(v[0].y),.5f);v[1].y=v[0].y;v[1].x=sub(round_sprite_coordinate(v[1].x),.5f);v[3].x=v[1].x;v[2].y=sub(round_sprite_coordinate(v[2].y),.5f);v[3].y=v[2].y;}
    const auto* bounds=draw_environment::viewport_bounds();bool left=false,top=false,right=false,bottom=false;
    for(const auto& p:v){left|=bounds[0]<=p.x;top|=bounds[1]<=p.y;right|=p.x<=bounds[2];bottom|=p.y<=bounds[3];}if(!(left&&top&&right&&bottom))return;
    auto& device=draw_environment::device();const auto& sprite=current_sprite(draw_environment::controller(),a);
    if(c.cached_texture!=sprite.texture_id){c.cached_texture=sprite.texture_id;flush_textured_quads(c,device);
#ifdef TH_SDL3
        device.bind_texture(texture(c,c.cached_texture));
#else
        device.SetTexture(0,texture(c,c.cached_texture));
#endif
    }
    if(c.unknown_cached_e0e!=1){flush_textured_quads(c,device);c.unknown_cached_e0e=1;}
    if(!(flags&2)){
        const auto mode=(a.base.flags[2]>>10)&7;auto primary=a.base.field_490,secondary=a.base.field_494;
        auto global=[&](std::uint32_t color){return c.field_7d40e90?color_multiply(color,c.field_7d40e8c,false):color;};
        if(mode==0||mode==1||mode==4){auto color=mode==0?primary:(mode==1?secondary:color_multiply(primary,secondary,true));if((a.base.flags[1]&0x1000000)&&a.fields_550[3])color=color_multiply(color,reinterpret_cast<Animation*>(a.fields_550[3])->field_5cc,false);a.field_5cc=color;color=global(color);for(auto& p:v)p.color=color;}
        else if(mode==2||mode==3){primary=global(primary);secondary=global(secondary);v[0].color=primary;v[3].color=secondary;v[1].color=mode==2?secondary:primary;v[2].color=mode==2?primary:secondary;}
    }
    apply_animation_render_state(c,a,device);select_texture_combine(c,device,((a.base.flags[2]>>4)&3)==3?3:0);
    v[0].u=n::add32(a.base.vectors_378[0].x,f(a.base.field_78));v[1].u=n::add32(n::mul32(a.base.vector_398.x,a.base.vector_68.x),v[0].u);
    v[2].u=n::add32(a.base.vectors_378[2].x,f(a.base.field_78));v[3].u=n::add32(n::mul32(a.base.vector_398.x,a.base.vector_68.x),v[2].u);
    v[0].v=n::add32(a.base.vectors_378[0].y,f(a.base.field_7c));v[2].v=n::add32(n::mul32(a.base.vector_398.y,a.base.vector_68.y),v[0].v);
    v[1].v=n::add32(a.base.vectors_378[1].y,f(a.base.field_7c));v[3].v=n::add32(n::mul32(a.base.vector_398.y,a.base.vector_68.y),v[1].v);
    append_textured_quad(c,v);
}
void draw_axis_aligned_sprite(Controller& c,Animation& a,bool snap){auto& v=animation_quad;calculate_sprite_corners(a,current_sprite(draw_environment::controller(),a),reinterpret_cast<Vec3*>(&v[0]),reinterpret_cast<Vec3*>(&v[1]),reinterpret_cast<Vec3*>(&v[2]),reinterpret_cast<Vec3*>(&v[3]),false);submit_animation_quad(c,a,v,snap?1:0);}
void draw_rotated_sprite(Controller& c,Animation& a){auto& v=animation_quad;calculate_sprite_corners(a,current_sprite(draw_environment::controller(),a),reinterpret_cast<Vec3*>(&v[0]),reinterpret_cast<Vec3*>(&v[1]),reinterpret_cast<Vec3*>(&v[2]),reinterpret_cast<Vec3*>(&v[3]),true);submit_animation_quad(c,a,v,0);}
}
