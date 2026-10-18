#include "projected_draw.hpp"
#include "anm_vm.hpp"
#include "binding.hpp"
#include "render_state.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <system_error>
#ifdef TH_SDL3
#include "platform/GraphicsMath.hpp"
#endif
namespace th20::source::sprite {
namespace n=th20::recovered;namespace m=ecl::math;namespace e=draw_environment;
namespace {
float f(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float length(const Vec3& v){return m::square_root(n::add32(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)),n::mul32(v.z,v.z)));}
struct Vector4 {float x,y,z,w;};
#ifdef TH_SDL3
web::math::Viewport math_viewport(const D3DVIEWPORT9& v){return {v.X,v.Y,v.Width,v.Height,v.MinZ,v.MaxZ};}
const float* floats(const D3DMATRIX& m){return &m._11;}
const float* floats(const Matrix4& m){return m.elements;}
Vec3 project(const Vec3& v,const D3DVIEWPORT9& viewport,const D3DMATRIX& projection,const D3DMATRIX& view,const Matrix4& world){
    const auto out=web::math::project(*reinterpret_cast<const web::math::Vec3*>(&v),math_viewport(viewport),floats(projection),floats(view),floats(world));
    return {out.x,out.y,out.z};
}
Vector4 transform(const Vec3& v,const Matrix4& matrix){
    const auto out=web::math::transform(*reinterpret_cast<const web::math::Vec3*>(&v),floats(matrix));
    return {out.x,out.y,out.z,out.w};
}
void rotate_compose(Matrix4& matrix,unsigned axis,float angle){
    float rotation[16];web::math::rotation_axis(rotation,axis,angle);
    web::math::multiply(matrix.elements,matrix.elements,rotation);
}
#else
struct ProjectSdk {
    HMODULE module=LoadLibraryW(L"d3dx9_43.dll");
    using Rotation=D3DMATRIX*(WINAPI*)(D3DMATRIX*,float);
    using Multiply=D3DMATRIX*(WINAPI*)(D3DMATRIX*,const D3DMATRIX*,const D3DMATRIX*);
    using Project=Vec3*(WINAPI*)(Vec3*,const Vec3*,const D3DVIEWPORT9*,const D3DMATRIX*,const D3DMATRIX*,const D3DMATRIX*);
    using Transform=Vector4*(WINAPI*)(Vector4*,const Vec3*,const D3DMATRIX*);
    Rotation rotations[3];Multiply multiply;Project project;Transform transform;
    ProjectSdk(){if(!module)throw std::system_error(GetLastError(),std::system_category(),"D3DX9 SDK");rotations[0]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationX"));rotations[1]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationY"));rotations[2]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationZ"));multiply=reinterpret_cast<Multiply>(GetProcAddress(module,"D3DXMatrixMultiply"));project=reinterpret_cast<Project>(GetProcAddress(module,"D3DXVec3Project"));transform=reinterpret_cast<Transform>(GetProcAddress(module,"D3DXVec3Transform"));if(!rotations[0]||!rotations[1]||!rotations[2]||!multiply||!project||!transform)throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX projection exports");}
    ~ProjectSdk(){if(module)FreeLibrary(module);}
};
ProjectSdk& sdk(){static ProjectSdk value;return value;}
void rotate_compose(Matrix4& matrix,unsigned axis,float angle){
    D3DMATRIX rotation;sdk().rotations[axis](&rotation,angle);
    sdk().multiply(reinterpret_cast<D3DMATRIX*>(&matrix),reinterpret_cast<const D3DMATRIX*>(&matrix),&rotation);
}
#endif
constexpr Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
void rotate_matrix(Animation& a,const Vec3& rotation,unsigned order){
    constexpr unsigned orders[6][3]{{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};if(order>=6)return;
    const float angles[]{rotation.x,rotation.y,rotation.z};
    for(auto axis:orders[order])if(static_cast<double>(angles[axis])!=0.0)rotate_compose(a.matrix_57c,axis,angles[axis]);
}
std::uint32_t multiply_color(std::uint32_t a,std::uint32_t b,bool normalized=false){std::uint32_t result=0;for(unsigned shift=0;shift<32;shift+=8){auto value=((a>>shift)&255)*((b>>shift)&255);value=normalized?value/255:value>>7;result|=(value>255?255:value)<<shift;}return result;}
std::uint32_t tint(Controller& c,std::uint32_t color){return c.field_7d40e90?multiply_color(color,c.field_7d40e8c):color;}
float fog_value(const program_entry::ViewportState& camera,unsigned index){return f(camera.final_state[index]);}
std::uint32_t fog_color(std::uint32_t color,const program_entry::ViewportState& camera,float t,bool integer_target,bool fade_alpha,bool cubic){
    std::uint32_t output=0;for(unsigned i=0;i<3;++i){const std::uint32_t channel=(color>>(i*8))&255;float delta;
        if(integer_target)delta=n::int_float(n::signed_bits(channel-static_cast<std::uint32_t>(n::truncate32(fog_value(camera,i+2)))));
        else delta=sub(n::int_float(channel),fog_value(camera,i+2));
        const auto v=channel-static_cast<std::uint32_t>(n::truncate32(n::mul32(delta,t)));output|=(v&255)<<(i*8);
    }
    std::uint32_t alpha=color>>24;if(fade_alpha){float amount=t;if(cubic)amount=n::mul32(n::mul32(t,t),t);alpha=n::truncate32(n::mul32(n::int_float(alpha),sub(1.f,amount)));}
    return output|((alpha&255)<<24);
}
void transform_position(Animation& a,Vec3& v){
    const auto mode=a.base.flags[3]&255;if(mode>=1&&mode<=4){float factor=anm_environment::screen_scale();if(mode==2||mode==4)factor=n::mul32(factor,.5f);v.x=n::mul32(v.x,factor);v.y=n::mul32(v.y,factor);v.z=n::mul32(v.z,factor);}
    if(a.fields_550[2]&&!(a.base.flags[1]&0x1000)){auto& parent=*reinterpret_cast<Animation*>(a.fields_550[2]);if(a.base.flags[1]&0x20)m::rotate(v.x,v.y,parent.base.vector_38.z);if(a.base.flags[1]&0x400000){v.x=n::mul32(v.x,parent.base.vector_50.x);v.y=n::mul32(v.y,parent.base.vector_50.y);}const auto pos=animation_position(parent);v.x=n::add32(v.x,pos.x);v.y=n::add32(v.y,pos.y);v.z=n::add32(v.z,pos.z);}
    else {const unsigned preset=(a.base.flags[2]>>24)&3;if(preset){v.x=n::add32(v.x,n::int_float(anm_environment::screen_offset(preset==1?0:1,0)));v.y=n::add32(v.y,n::int_float(anm_environment::screen_offset(preset==1?0:1,1)));}}
}
}
std::int32_t prepare_projected_billboard(Animation& a){
    const float angle=inherited_animation_rotation(a).z,sine=m::sine(angle),cosine=m::cosine(angle);auto& camera=e::current_camera();Matrix4 world=identity;
    world.elements[12]=n::add32(n::add32(a.base.vector_2c.x,a.vector_5bc.x),a.base.vector_484.x);world.elements[13]=n::add32(n::add32(a.base.vector_2c.y,a.vector_5bc.y),a.base.vector_484.y);world.elements[14]=n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z);
    Vec3 origin{},projected,up;
#ifdef TH_SDL3
    projected=project(origin,camera.viewport,camera.projection,camera.view,world);
#else
    sdk().project(&projected,&origin,&camera.viewport,&camera.projection,&camera.view,reinterpret_cast<D3DMATRIX*>(&world));
#endif
    // COMISS/JA and JBE admit unordered projected Z, as the original does.
    if(projected.z<0.f||projected.z>1.f)return -1;
#ifdef TH_SDL3
    up=project(*reinterpret_cast<Vec3*>(&camera.vectors[4]),camera.viewport,camera.projection,camera.view,world);
#else
    sdk().project(&up,reinterpret_cast<Vec3*>(&camera.vectors[4]),&camera.viewport,&camera.projection,&camera.view,reinterpret_cast<D3DMATRIX*>(&world));
#endif
    const float distance=length({sub(up.x,projected.x),sub(up.y,projected.y),sub(up.z,projected.z)});
    const float sx=n::mul32(n::mul32(n::mul32(n::mul32(distance,.5f),a.base.vector_70.x),a.base.vector_50.x),a.base.vector_58.x),sy=n::mul32(n::mul32(n::mul32(n::mul32(distance,.5f),a.base.vector_70.y),a.base.vector_50.y),a.base.vector_58.y);
    constexpr float horizontal[3][4]{{-.5f,.5f,-.5f,.5f},{0,1,0,1},{-1,0,-1,0}},vertical[3][4]{{-.5f,-.5f,.5f,.5f},{0,0,1,1},{-1,-1,0,0}};
    for(unsigned i=0;i<4;++i){auto& v=animation_quad[i];const float x=n::mul32(horizontal[a.base.flags[4]][i],sx),y=n::mul32(vertical[a.base.flags[5]][i],sy);v.x=n::add32(sub(n::mul32(x,cosine),n::mul32(y,sine)),projected.x);v.y=n::add32(n::add32(n::mul32(x,sine),n::mul32(y,cosine)),projected.y);v.z=projected.z;}
    return 0;
}
std::int32_t prepare_projected_matrix(Controller& c,Animation& a){
    if(!(a.base.flags[1]&0x4000000)){a.matrix_57c=a.base.matrix_3b8;a.matrix_57c.elements[0]=n::mul32(n::mul32(a.base.vector_50.x,a.base.vector_58.x),a.matrix_57c.elements[0]);a.matrix_57c.elements[5]=n::mul32(n::mul32(a.base.vector_50.y,a.base.vector_58.y),a.matrix_57c.elements[5]);a.base.flags[1]&=~4u;rotate_matrix(a,inherited_animation_rotation(a),0);a.base.flags[1]&=~2u;}
    Matrix4 world=a.matrix_57c;
    world.elements[12]=n::add32(n::add32(n::add32(a.vector_5bc.x,a.base.vector_2c.x),a.base.vector_484.x),world.elements[12]);
    if(((a.base.flags[2]>>24)&3)&&!a.fields_550[3]){world.elements[12]=n::add32(div(n::int_float(e::scaled_dimension(0)),2.f),world.elements[12]);world.elements[13]=n::add32(div(sub(n::int_float(e::scaled_dimension(1)),448.f),2.f),world.elements[13]);}
    world.elements[13]=n::add32(n::add32(n::add32(a.vector_5bc.y,a.base.vector_2c.y),a.base.vector_484.y),world.elements[13]);world.elements[14]=n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z);
    if(a.fields_550[3]&&!(a.base.flags[1]&0x1000)){const auto& p=*reinterpret_cast<Animation*>(a.fields_550[3]);world.elements[12]=n::add32(n::add32(n::add32(p.vector_5bc.x,p.base.vector_2c.x),p.base.vector_484.x),world.elements[12]);world.elements[13]=n::add32(n::add32(n::add32(p.vector_5bc.y,p.base.vector_2c.y),p.base.vector_484.y),world.elements[13]);world.elements[14]=n::add32(n::add32(n::add32(p.vector_5bc.z,p.base.vector_2c.z),p.base.vector_484.z),world.elements[14]);}
    c.matrix_60007d8=world;
    // The original constructs anchor corners on its stack, then discards them.
    // It does not update Controller.corners or animation_quad here.
    return 0;
}
namespace primitive {
void p440310(Controller& c,Animation& a){if(!prepare_projected_billboard(a))submit_animation_quad(c,a,animation_quad,0);}
void p4413a0(Controller& c,Animation& a){prepare_projected_matrix(c,a);submit_animation_quad(c,a,animation_quad,0);for(auto& v:animation_quad)v.rhw=1;}
void p4408b0(Controller& c,Animation& a){
    if(prepare_projected_billboard(a))return;auto& camera=e::current_camera();const float fog_start=fog_value(camera,0),fog_end=fog_value(camera,1),span=sub(fog_start,fog_end);
    Vec3 delta{sub(n::add32(n::add32(a.base.vector_2c.x,a.vector_5bc.x),a.base.vector_484.x),camera.vectors[0][0]),sub(n::add32(n::add32(a.base.vector_2c.y,a.vector_5bc.y),a.base.vector_484.y),camera.vectors[0][1]),sub(n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z),camera.vectors[0][2])};
    if(((a.base.flags[2]>>24)&3)&&!a.fields_550[3]){delta.x=n::add32(div(n::int_float(e::scaled_dimension(0)),2.f),delta.x);delta.y=n::add32(div(sub(n::int_float(e::scaled_dimension(1)),448.f),2.f),delta.y);}
    const float distance=length(delta);const auto mode=(a.base.flags[2]>>10)&7;
    if(mode<=4){
        auto primary=tint(c,mode==1?a.base.field_494:(mode==4?multiply_color(a.base.field_490,a.base.field_494,true):a.base.field_490));auto secondary=tint(c,a.base.field_494);
        if(!(distance<=fog_start)){const float t=div(sub(fog_start,distance),span);if(t>=1.f)return;primary=fog_color(primary,camera,t,true,true,mode==0||mode==1||mode==4);if(mode==2||mode==3)secondary=fog_color(secondary,camera,t,true,true,false);}
        if((mode==0||mode==1)&&(a.base.flags[1]&0x100000)){const float start=f(a.base.field_4b8),end=f(a.base.field_4bc);if(distance<=end)return;if(distance<start){const float t=div(sub(start,distance),sub(start,end));primary=(primary&0xffffffu)|((static_cast<std::uint32_t>(n::truncate32(n::mul32(n::int_float(primary>>24),sub(1.f,t))))&255)<<24);}}
        if(mode==2||mode==3){animation_quad[0].color=primary;animation_quad[3].color=secondary;animation_quad[1].color=mode==2?secondary:primary;animation_quad[2].color=mode==2?primary:secondary;}
        else for(auto& v:animation_quad)v.color=primary;
    }
    submit_animation_quad(c,a,animation_quad,2);
}
void p441c00(Controller& c,Animation& a){
    prepare_projected_matrix(c,a);auto& camera=e::current_camera();const float fog_start=fog_value(camera,0),span=sub(fog_start,fog_value(camera,1));const auto color=((a.base.flags[2]>>10)&7)==0?a.base.field_490:a.base.field_494;
    for(unsigned i=0;i<4;++i){Vector4 transformed{};
#ifdef TH_SDL3
        transformed=transform(*reinterpret_cast<Vec3*>(&c.corners[i]),c.matrix_60007d8);
#else
        sdk().transform(&transformed,reinterpret_cast<Vec3*>(&c.corners[i]),reinterpret_cast<D3DMATRIX*>(&c.matrix_60007d8));
#endif
        const float distance=length({sub(transformed.x,camera.vectors[0][0]),sub(transformed.y,camera.vectors[0][1]),sub(transformed.z,camera.vectors[0][2])});
        if(distance<=fog_start)animation_quad[i].color=color;else {const float t=div(sub(fog_start,distance),span);animation_quad[i].color=t<1.f?fog_color(color,camera,t,false,false,false):((camera.final_state[6]&0xffffffu)|(color&0xff000000u));}
    }
    submit_animation_quad(c,a,animation_quad,2);for(auto& v:animation_quad)v.rhw=1;
}
void p441f00(Controller& c,Animation& a){
    if(!(a.base.flags[0]&0x10000)||!(a.base.flags[1]&1)||!(a.base.field_490&0xff000000u))return;auto& device=e::device();if(c.quad_count)flush_textured_quads(c,device);if(a.base.flags[1]&0x10)e::disable_depth_write();else e::enable_depth_write();
    if(!(a.base.flags[1]&0x4000000)){a.matrix_57c=a.base.matrix_3b8;a.matrix_57c.elements[0]=n::mul32(n::mul32(a.base.vector_50.x,a.base.vector_58.x),a.matrix_57c.elements[0]);a.matrix_57c.elements[5]=n::mul32(n::mul32(a.base.vector_50.y,a.base.vector_58.y),a.matrix_57c.elements[5]);a.base.flags[1]&=~4u;
        const auto mode=a.base.flags[3]&255;if(mode==1||mode==5){a.matrix_57c.elements[0]=n::mul32(a.matrix_57c.elements[0],anm_environment::screen_scale());a.matrix_57c.elements[5]=n::mul32(a.matrix_57c.elements[5],anm_environment::screen_scale());}else if(mode==2||mode==6){a.matrix_57c.elements[0]=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),a.matrix_57c.elements[0]);a.matrix_57c.elements[5]=n::mul32(n::mul32(anm_environment::screen_scale(),.5f),a.matrix_57c.elements[5]);}
        rotate_matrix(a,inherited_animation_rotation(a),(a.base.flags[2]>>18)&7);a.base.flags[1]&=~2u;
    }
    Matrix4 world=a.matrix_57c;Vec3 pos{sub(n::add32(n::add32(a.base.vector_2c.x,a.vector_5bc.x),a.base.vector_484.x),n::mul32(n::mul32(a.base.vector_80.x,a.base.vector_50.x),a.base.vector_58.x)),sub(n::add32(n::add32(a.base.vector_2c.y,a.vector_5bc.y),a.base.vector_484.y),n::mul32(n::mul32(a.base.vector_80.y,a.base.vector_50.y),a.base.vector_58.y)),world.elements[14]};transform_position(a,pos);world.elements[12]=pos.x;world.elements[13]=pos.y;
    apply_animation_render_state(c,a,device);const auto color=tint(c,((a.base.flags[2]>>10)&7)==0?a.base.field_490:a.base.field_494);if(c.field_e04!=color){flush_textured_quads(c,device);c.field_e04=color;
#ifdef TH_SDL3
        device.set_texture_factor(color);
#else
        device.SetRenderState(D3DRS_TEXTUREFACTOR,color);
#endif
    }
    world.elements[14]=n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z);
#ifdef TH_SDL3
    device.transform(touhou::graphics::MatrixKind::World,world.elements);
#else
    device.SetTransform(D3DTS_WORLD,reinterpret_cast<D3DMATRIX*>(&world));
#endif
    auto& sprite=current_sprite(e::controller(),a);if(c.cached_texture!=sprite.texture_id){c.cached_texture=sprite.texture_id;flush_textured_quads(c,device);
#ifdef TH_SDL3
        device.bind_texture(texture(c,c.cached_texture));
#else
        device.SetTexture(0,texture(c,c.cached_texture));
#endif
    }
    if(c.field_e18!=reinterpret_cast<std::uint32_t>(&sprite)||f(a.base.field_78)!=0.f||f(a.base.field_7c)!=0.f||a.base.vector_68.x!=1.f||a.base.vector_68.y!=1.f){c.field_e18=reinterpret_cast<std::uint32_t>(&sprite);Matrix4 uv=a.base.matrix_3f8;uv.elements[8]=n::add32(a.base.vectors_378[0].x,f(a.base.field_78));uv.elements[9]=n::add32(a.base.vectors_378[0].y,f(a.base.field_7c));uv.elements[0]=n::mul32(uv.elements[0],a.base.vector_68.x);uv.elements[5]=n::mul32(uv.elements[5],a.base.vector_68.y);
#ifdef TH_SDL3
        device.transform(touhou::graphics::MatrixKind::Texture,uv.elements);
#else
        device.SetTransform(D3DTS_TEXTURE0,reinterpret_cast<D3DMATRIX*>(&uv));
#endif
    }
    if(c.unknown_cached_e0e!=2){
#ifdef TH_SDL3
        device.vertex_format(touhou::graphics::VertexLayout::WorldUv);
#else
        device.SetStreamSource(0,c.corner_buffer,0,20);device.SetFVF(0x102);
#endif
        c.unknown_cached_e0e=2;}select_texture_combine(c,device,1);
#ifdef TH_SDL3
    device.draw_corner_strip((a.base.flags[5]*3+a.base.flags[4])*4,2);
#else
    device.DrawPrimitive(D3DPT_TRIANGLESTRIP,(a.base.flags[5]*3+a.base.flags[4])*4,2);
#endif
}
}
}
