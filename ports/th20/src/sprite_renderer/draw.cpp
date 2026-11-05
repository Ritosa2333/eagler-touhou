#include "draw.hpp"
#include "pool.hpp"
#include "anm_vm.hpp"
#include "render_state.hpp"
#include <cstring>
namespace th20::source::sprite {
namespace n=th20::recovered;namespace p=primitive;namespace e=draw_environment;
namespace {
float as_float(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float half(float value){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(value),_mm_set_ss(2.f)));}
}
void draw_animation(Controller& c,Animation& a) {
    if(!(a.base.flags[1]&1))return;
    if(auto* callback=reinterpret_cast<AnimationCallback*>(a.fields_550[6]))callback->draw();
    if(!(a.base.flags[0]&0x10000u)||a.fields_550[8])return;
    e::disable_depth_write();const auto type=a.base.flags[0]&255;const bool visible_alpha=((a.base.field_490|a.base.field_494)&0xff000000u)!=0;
    switch(type){
    case 0:if(visible_alpha)draw_axis_aligned_sprite(c,a,true);return;
    case 1:case 3:if(visible_alpha)draw_rotated_sprite(c,a);return;
    case 2:if(visible_alpha)draw_axis_aligned_sprite(c,a,false);return;
    case 4:if(visible_alpha)p::p440310(c,a);return;
    case 5:if(visible_alpha)p::p4413a0(c,a);return;
    case 6:if(visible_alpha)p::p4408b0(c,a);return;
    case 7:if(visible_alpha)p::p441c00(c,a);return;
    case 8:if(visible_alpha)p::p441f00(c,a);return;
    case 9:case 12:case 13:case 14:p::p445350(c,a,reinterpret_cast<float*>(a.fields_550[5]),a.base.fields_444[0]<<1);return;
    case 11:p::p445130(c,a,reinterpret_cast<float*>(a.fields_550[5]),a.base.fields_444[0]<<1);return;
    case 15:if(visible_alpha){e::enable_fog();p::p441f00(c,a);e::disable_fog();}return;
    case 24:case 25:case 47:p::p443020(c,a,reinterpret_cast<void*>(a.fields_550[5]),a.base.fields_444[0]<<1);return;
    case 48:e::enable_fog();p::p443020(c,a,reinterpret_cast<void*>(a.fields_550[5]),3);e::disable_fog();return;
    case 10:case 23:return; // Original switch has no implementation for these modes.
    default:if(type<16||type>46)return;break;
    }
    float angle=a.base.vector_38.z,width=animation_width(a),height=animation_height(a);
    const auto position=animation_position(a);
    if(a.fields_550[3]&&!(a.base.flags[1]&0x1000))angle=n::add32(reinterpret_cast<Animation*>(a.fields_550[3])->base.vector_38.z,angle);
    apply_animation_render_state(c,a,e::device());
    const auto scaling=a.base.flags[3]&255;
    if(scaling==1||scaling==5){width=n::mul32(width,anm_environment::screen_scale());height=n::mul32(height,anm_environment::screen_scale());}
    else if(scaling==2||scaling==6){width=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),width);height=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),height);}
    const float x=position.x,y=position.y;const auto primary=a.base.field_490,secondary=a.base.field_494,count=a.base.fields_444[0];
    const auto ax=static_cast<std::int32_t>(a.base.flags[4]),ay=static_cast<std::int32_t>(a.base.flags[5]);
    const float u=as_float(a.base.fields_444[4]),v=as_float(a.base.fields_444[5]);
    switch(type){
    case 16:p::p439a00(c,x,y,width,height,angle,primary,primary,ax,ay);break;
    case 17:p::p43b960(c,x,y,width,angle,count,primary,secondary);break;
    case 18:p::p43bc60(c,x,y,width,angle,count,primary);break;
    case 19:p::p43be80(c,x,y,width,height,angle,count,primary,secondary);break;
    case 20:p::p439a00(c,x,y,width,height,angle,primary,secondary,ax,ay);break;
    case 21:p::p43a1c0(c,x,y,width,height,angle,primary,primary,ax,ay);break;
    case 22:p::p43a1c0(c,x,y,width,height,angle,primary,secondary,ax,ay);break;
    case 26:p::p43cd40(c,x,y,width,angle,primary,((a.base.flags[2]>>10)&7)==0?primary:secondary,ax,ay);break;
    case 27:p::p43b0e0(c,x,y,width,height,angle,primary,((a.base.flags[2]>>10)&7)==0?primary:secondary,ax,ay);break;
    case 28:p::p43e2c0(c,x,y,width,height,angle,primary,secondary,ax,ay);break;
    case 29:p::p43be80(c,x,y,n::add32(half(height),width),height,angle,count,primary,secondary);break;
    case 30:p::p43be80(c,x,y,sub(width,half(height)),height,angle,count,primary,secondary);break;
    case 31:p::p43c180(c,x,y,animation_aux_scale_x(a),width,angle,count,primary,secondary);break;
    case 32:p::p43d830(c,x,y,primary);break;
    case 33:p::p43c4e0(c,x,y,animation_aux_scale_x(a),width,angle,count,primary,secondary);break;
    case 34:p::p43c780(c,x,y,animation_aux_scale_x(a),width,height,angle,count,primary,secondary);break;
    case 35:p::p43c780(c,x,y,n::add32(animation_aux_scale_x(a),half(height)),n::add32(half(height),width),height,angle,count,primary,secondary);break;
    case 36:p::p43c780(c,x,y,sub(animation_aux_scale_x(a),half(height)),sub(width,half(height)),height,angle,count,primary,secondary);break;
    case 37:p::p43a2b0(c,x,y,width,height,u,angle,count,primary,secondary);break;
    case 38:p::p43a750(c,x,y,width,height,u,angle,count,primary);break;
    case 39:p::p43ab20(c,x,y,sub(width,n::mul32(u,2.f)),sub(height,n::mul32(u,2.f)),u,v,angle,count,primary,secondary);break;
    case 40:p::p43ab20(c,x,y,sub(width,n::mul32(u,2.f)),sub(height,n::mul32(u,2.f)),n::add32(half(v),u),v,angle,count,primary,secondary);break;
    case 41:p::p43ab20(c,x,y,sub(width,n::mul32(u,2.f)),sub(height,n::mul32(u,2.f)),sub(u,half(v)),v,angle,count,primary,secondary);break;
    case 42:p::p43d9b0(c,x,y,width,animation_aux_scale_x(a),angle,count,primary,secondary);break;
    case 43:p::p43dce0(c,x,y,width,animation_aux_scale_x(a),angle,count,primary);break;
    case 44:p::p43df30(c,x,y,width,animation_aux_scale_x(a),height,angle,count,primary,secondary);break;
    case 45:p::p43df30(c,x,y,n::add32(half(height),width),n::add32(animation_aux_scale_x(a),half(height)),height,angle,count,primary,secondary);break;
    case 46:p::p43df30(c,x,y,sub(width,half(height)),sub(animation_aux_scale_x(a),half(height)),height,angle,count,primary,secondary);break;
    }
}
}
