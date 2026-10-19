#include "draw.hpp"
#include "binding.hpp"
#include "anm_vm.hpp"
#include "render_state.hpp"
#include <system_error>
#include <cstring>
#ifdef TH_SDL3
#include "platform/GraphicsMath.hpp"
#endif
namespace th20::source::sprite::primitive {
namespace n=th20::recovered;namespace e=draw_environment;
namespace {
float as_float(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
void bind_texture(Controller& c,const SpriteData& sprite,Device& device){if(c.cached_texture!=sprite.texture_id){c.cached_texture=sprite.texture_id;
#ifdef TH_SDL3
    device.bind_texture(texture(c,c.cached_texture));
#else
    device.SetTexture(0,texture(c,c.cached_texture));
#endif
}}
void offset_geometry(Controller& c,Animation& a,float* vertices,std::int32_t count){if(!(a.base.flags[1]&0x200000)){a.base.flags[1]|=0x200000;for(std::int32_t i=0;i<count;++i){vertices[i*7]=n::add32(vertices[i*7],as_float(c.fields_c8[2]));vertices[i*7+1]=n::add32(vertices[i*7+1],as_float(c.fields_c8[3]));}}}
#ifdef TH_SDL3
void rotate_compose(Matrix4& matrix,unsigned axis,float angle){
    float rotation[16];web::math::rotation_axis(rotation,axis,angle);
    web::math::multiply(matrix.elements,matrix.elements,rotation);
}
void select_diffuse_second(Device& device){
    device.set_texture_argument(touhou::graphics::Component::Alpha,touhou::graphics::ArgumentSlot::Second,{touhou::graphics::ArgumentSource::Diffuse});
    device.set_texture_argument(touhou::graphics::Component::Color,touhou::graphics::ArgumentSlot::Second,{touhou::graphics::ArgumentSource::Diffuse});
}
#else
struct MatrixSdk {
    HMODULE module=LoadLibraryW(L"d3dx9_43.dll");
    using Rotation=D3DMATRIX*(WINAPI*)(D3DMATRIX*,float);
    using Multiply=D3DMATRIX*(WINAPI*)(D3DMATRIX*,const D3DMATRIX*,const D3DMATRIX*);
    Rotation rotations[3];Multiply multiply;
    MatrixSdk(){if(!module)throw std::system_error(GetLastError(),std::system_category(),"D3DX9 SDK");rotations[0]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationX"));rotations[1]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationY"));rotations[2]=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationZ"));multiply=reinterpret_cast<Multiply>(GetProcAddress(module,"D3DXMatrixMultiply"));if(!rotations[0]||!rotations[1]||!rotations[2]||!multiply)throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX matrix exports");}
    ~MatrixSdk(){if(module)FreeLibrary(module);}
};
MatrixSdk& matrices(){static MatrixSdk sdk;return sdk;}
void rotate_compose(Matrix4& matrix,unsigned axis,float angle){
    D3DMATRIX rotation;matrices().rotations[axis](&rotation,angle);
    matrices().multiply(reinterpret_cast<D3DMATRIX*>(&matrix),reinterpret_cast<const D3DMATRIX*>(&matrix),&rotation);
}
void select_diffuse_second(Device& device){device.SetTextureStageState(0,D3DTSS_ALPHAARG2,0);device.SetTextureStageState(0,D3DTSS_COLORARG2,0);}
#endif
constexpr Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
}
std::int32_t p445350(Controller& c,Animation& a,float* vertices,std::uint32_t raw_count){
    const auto count=static_cast<std::int32_t>(raw_count);
    if(count<3||!(a.base.flags[0]&0x10000)||!(a.base.flags[1]&1)||!(a.base.field_490&0xff000000))return -1;
    auto& device=e::device();if(c.quad_count)flush_textured_quads(c,device);offset_geometry(c,a,vertices,count);
    bind_texture(c,current_sprite(e::controller(),a),device);
    if(c.unknown_cached_e0e!=3){
#ifdef TH_SDL3
        device.vertex_format(touhou::graphics::VertexLayout::ScreenColorUv);
#else
        device.SetFVF(0x144);
#endif
        select_diffuse_second(device);c.unknown_cached_e0e=3;}
    apply_animation_render_state(c,a,device);select_texture_combine(c,device,((a.base.flags[2]>>4)&3)==3?3:0);
#ifdef TH_SDL3
    device.draw(touhou::graphics::Topology::Strip,raw_count-2,vertices,28);
#else
    device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,raw_count-2,vertices,28);
#endif
    return 0;
}
std::int32_t p445130(Controller& c,Animation& a,float* vertices,std::uint32_t raw_count){
    const auto count=static_cast<std::int32_t>(raw_count);if(count<3)return -1;
    auto& device=e::device();if(c.quad_count)flush_textured_quads(c,device);offset_geometry(c,a,vertices,count);
    if(c.unknown_cached_e0e!=3){
#ifdef TH_SDL3
        device.vertex_format(touhou::graphics::VertexLayout::ScreenColorUv);
#else
        device.SetFVF(0x144);
#endif
        c.unknown_cached_e0e=3;}
    apply_animation_render_state(c,a,device);bind_texture(c,current_sprite(e::controller(),a),device);e::disable_depth_write();
    if(c.unknown_cached_e0e!=1){select_diffuse_second(device);c.unknown_cached_e0e=1;}
#ifdef TH_SDL3
    device.draw(touhou::graphics::Topology::Fan,raw_count-2,vertices,28);
#else
    device.DrawPrimitiveUP(D3DPT_TRIANGLEFAN,raw_count-2,vertices,28);
#endif
    return 0;
}
void p443020(Controller& c,Animation& a,void* vertices,std::uint32_t raw_count){
    if(!(a.base.flags[0]&0x10000)||!(a.base.flags[1]&1)||static_cast<std::int32_t>(raw_count)<3)return;
    auto& device=e::device();if(c.quad_count)flush_textured_quads(c,device);
    if(a.base.flags[1]&0x10)e::disable_depth_write();else e::enable_depth_write();
    a.base.matrix_3b8=identity;a.matrix_57c=a.base.matrix_3b8;
    a.matrix_57c.elements[0]=n::mul32(n::mul32(a.base.vector_50.x,a.base.vector_58.x),a.matrix_57c.elements[0]);
    a.matrix_57c.elements[5]=n::mul32(n::mul32(a.base.vector_50.y,a.base.vector_58.y),a.matrix_57c.elements[5]);a.base.flags[1]&=~4u;
    const float angles[]{a.base.vector_38.x,a.base.vector_38.y,a.base.vector_38.z};
    for(unsigned axis=0;axis<3;++axis)if(static_cast<double>(angles[axis])!=0.0)rotate_compose(a.matrix_57c,axis,angles[axis]);
    a.base.flags[1]&=~2u;Matrix4 world=a.matrix_57c;
    world.elements[12]=n::add32(n::add32(a.base.vector_2c.x,a.vector_5bc.x),a.base.vector_484.x);
    world.elements[13]=n::add32(n::add32(a.base.vector_2c.y,a.vector_5bc.y),a.base.vector_484.y);
    world.elements[14]=n::add32(n::add32(a.base.vector_2c.z,a.vector_5bc.z),a.base.vector_484.z);
    if(((a.base.flags[2]>>24)&3)==0&&!a.fields_550[3]){world.elements[12]=n::add32(world.elements[12],static_cast<float>(anm_environment::screen_offset(0,0)));world.elements[13]=n::add32(world.elements[13],static_cast<float>(anm_environment::screen_offset(0,1)));}
#ifdef TH_SDL3
    device.transform(touhou::graphics::MatrixKind::World,world.elements);
#else
    device.SetTransform(D3DTS_WORLD,reinterpret_cast<D3DMATRIX*>(&world));
#endif
    apply_animation_render_state(c,a,device);auto& sprite=current_sprite(e::controller(),a);bind_texture(c,sprite,device);
    if(c.field_e18!=reinterpret_cast<std::uint32_t>(&sprite)||as_float(a.base.field_78)!=0.f||as_float(a.base.field_7c)!=0.f||a.base.vector_68.x!=1.f||a.base.vector_68.y!=1.f){
        c.field_e18=reinterpret_cast<std::uint32_t>(&sprite);Matrix4 uv=a.base.matrix_3f8;
        uv.elements[8]=n::add32(a.base.vectors_378[0].x,as_float(a.base.field_78));uv.elements[9]=n::add32(a.base.vectors_378[0].y,as_float(a.base.field_7c));
        uv.elements[0]=n::mul32(uv.elements[0],a.base.vector_68.x);uv.elements[5]=n::mul32(uv.elements[5],a.base.vector_68.y);
#ifdef TH_SDL3
        device.transform(touhou::graphics::MatrixKind::Texture,uv.elements);
#else
        device.SetTransform(D3DTS_TEXTURE0,reinterpret_cast<D3DMATRIX*>(&uv));
#endif
    }
    select_texture_combine(c,device,1);
    if(c.unknown_cached_e0e!=5){
#ifdef TH_SDL3
        device.vertex_format(touhou::graphics::VertexLayout::WorldColorUv);
#else
        device.SetFVF(0x142);
#endif
        select_diffuse_second(device);c.unknown_cached_e0e=5;}
#ifdef TH_SDL3
    device.draw(touhou::graphics::Topology::Strip,raw_count-2,vertices,24);
#else
    device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,raw_count-2,vertices,24);
#endif
}
}
