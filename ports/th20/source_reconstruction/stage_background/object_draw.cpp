#include "object_draw.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../sprite_renderer/render_state.hpp"
#include "../platform_window/platform_window.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <system_error>
#ifdef TH_SDL3
#include "platform/GraphicsMath.hpp"
#endif
namespace th20::source::background {
namespace n=th20::recovered;namespace s=sprite;namespace pe=program_entry;namespace e=s::draw_environment;
namespace {
float f(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void set_y_rotation(s::Animation& animation,float angle){animation.base.vector_38.y=angle;animation.base.flags[1]|=2;}
#ifndef TH_SDL3
struct RotationLibrary {
    HMODULE module=LoadLibraryW(L"d3dx9_43.dll");using Rotation=D3DMATRIX*(WINAPI*)(D3DMATRIX*,float);using Transform=s::Vec3*(WINAPI*)(s::Vec3*,const s::Vec3*,const D3DMATRIX*);Rotation rotation;Transform transform;
    RotationLibrary(){if(!module)throw std::system_error(GetLastError(),std::system_category(),"D3DX9 SDK");rotation=reinterpret_cast<Rotation>(GetProcAddress(module,"D3DXMatrixRotationY"));transform=reinterpret_cast<Transform>(GetProcAddress(module,"D3DXVec3TransformCoord"));if(!rotation||!transform)throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX rotation exports");}~RotationLibrary(){if(module)FreeLibrary(module);}
};
RotationLibrary& sdk(){static RotationLibrary value;return value;}
#endif
s::SpriteData* optional_sprite(s::Animation& a){const auto& file=*e::controller().files[a.base.fields_10_28[3]];return reinterpret_cast<s::SpriteData*>(reinterpret_cast<std::uintptr_t>(file.sprites)+a.base.fields_10_28[4]*sizeof(s::SpriteData));}
}
void select_background_viewport(pe::GraphicsStatePrefix& graphics,int index){auto& camera=graphics.viewports[index];graphics.current_viewport=&camera;platform_window::update_camera(camera,camera.viewport);update_perspective_camera(camera);
#ifdef TH_SDL3
    {const auto& v=camera.adjusted_viewport;graphics.device->set_viewport(v.X,v.Y,v.Width,v.Height,v.MinZ,v.MaxZ);}
#else
    graphics.device->SetViewport(&camera.adjusted_viewport);
#endif
    auto& c=*pe::sprite_controller;const float x=n::int_float(camera.offset_x),y=n::int_float(camera.offset_y);std::memcpy(c.fields_c8+2,&x,4);std::memcpy(c.fields_c8+3,&y,4);graphics.field_0b04=0;std::memcpy(c.fields_c8,camera.points[2],8);}
void reset_sprite_draw_cache(s::Controller& c) noexcept {c.unknown_cached_e0e=0xff;c.field_e18=0;c.cached_texture=0xffffffffu;c.unknown_cached_e0d=0xff;c.blend_mode=11;c.field_e0f=0xff;c.field_b8=c.field_bc=c.field_c0=c.draw_calls=0;c.unknown_cached_e10=0xff;c.field_7d40e90=0;c.field_7d40e8c=0x80808080u;reinterpret_cast<std::uint8_t*>(&c.field_00)[0]=0xff;c.fields_c8[0]=c.fields_c8[1]=0;}
void draw_embedded_layer(ScriptState& state,int layer){for(unsigned i=0;i<8;++i){auto& a=state.animations[i];if(optional_sprite(a)&&state.fields_3294[i]==static_cast<std::uint32_t>(layer)){platform_window::select_viewport(pe::graphics_state,3);e::disable_fog();s::flush_textured_quads(e::controller(),e::device());e::disable_depth_write();if(optional_sprite(a))s::draw_animation(e::controller(),a);e::enable_depth_write();select_background_viewport(pe::graphics_state,3);}}}
void draw_object_layer(Background& background,int layer){
    auto& c=e::controller();auto& device=e::device();draw_embedded_layer(background.state,layer);s::flush_textured_quads(c,device);e::enable_fog();select_background_viewport(pe::graphics_state,3);c.unknown_cached_e10=1;
    for(auto* instance=background.instances;static_cast<std::int16_t>(instance->object_id)>=0;++instance){auto& object=*background.objects[instance->object_id];if(static_cast<std::int8_t>(object.layer)!=layer)continue;
        if(cull_object(object,instance->position,f(background.state.fields_3294[8]),pe::graphics_state.viewports[3])){++background.rendered[1];instance->unknown&=0xfffe;continue;}
        object.flags|=2;
#ifdef TH_SDL3
        float object_rotation[16];
        if(object.parameters[2]!=0.f)web::math::rotation_y(object_rotation,object.parameters[2]);
#endif
        for(auto* primitive=reinterpret_cast<Primitive*>(&object+1);primitive->type>=0;primitive=reinterpret_cast<Primitive*>(reinterpret_cast<std::uint8_t*>(primitive)+primitive->size)){
            auto& a=background.primitive_animations[primitive->animation_index];if(primitive->type)continue;const float angle=a.base.vector_38.y;
            if((a.base.flags[0]&255)>3){const auto* parameters=reinterpret_cast<const float*>(primitive+1);s::Vec3 position{parameters[0],parameters[1],parameters[2]};if(object.parameters[2]!=0.f){
#ifdef TH_SDL3
                const auto out=web::math::transform_coord(*reinterpret_cast<const web::math::Vec3*>(&position),object_rotation);position={out.x,out.y,out.z};
#else
                D3DMATRIX rotation;sdk().rotation(&rotation,object.parameters[2]);s::Vec3 out;sdk().transform(&out,&position,&rotation);position=out;
#endif
                set_y_rotation(a,ecl::math::wrap_angle(n::add32(angle,object.parameters[2])));}
                a.vector_5bc={n::add32(position.x,instance->position.x),n::add32(position.y,instance->position.y),n::add32(position.z,instance->position.z)};
                if(parameters[3]!=0.f){const auto& sprite=s::current_sprite(c,a);a.base.vector_50.x=div(parameters[3],sprite.extent_4c);a.base.flags[1]|=4;}
                if(parameters[4]!=0.f){const auto& sprite=s::current_sprite(c,a);a.base.vector_50.y=div(parameters[4],sprite.extent_48);a.base.flags[1]|=4;}
            }
            const auto type=a.base.flags[0]&255;if(type==8||type==24)e::enable_fog();else e::disable_fog();if(a.base.flags[1]&0x10)e::disable_depth_write();else e::enable_depth_write();s::draw_animation(c,a);set_y_rotation(a,angle);++background.rendered[2];
        }instance->unknown|=1;++background.rendered[0];
    }e::disable_depth_write();reset_sprite_draw_cache(c);s::flush_textured_quads(c,device);
}
}

