#include "named_spawn.hpp"
#include "pool.hpp"
#include "binding.hpp"
#include "anm_vm.hpp"
#include "../runtime_core/runtime_core.hpp"
namespace th20::source::sprite {
namespace n=th20::recovered;
void spawn_named_animation(Controller& c,AnimationFile& file,std::uint32_t& out,
    const char* name,std::int32_t script,const Vec3* position,float rotation_z,
    std::int32_t layer,std::uint32_t flags,Animation** output_animation) {
    std::lock_guard lock(runtime::shared_locks().slot(9));++file.fields_5c[3];
    auto& animation=*allocate_animation(c);if(output_animation)*output_animation=&animation;
    // 0x40c6b0 is the five-byte empty release diagnostic. No error is emitted.
    if(name) (void)(file.stem==name);
    // 0x449490 uses a 0x48-byte group stride (three 0x18-byte lists).
    const auto group_index=((flags&0x10)!=0 || c.field_6c4!=0)?1u:0u;
    auto* group=reinterpret_cast<AnimationList*>(reinterpret_cast<std::uint8_t*>(c.lists)+group_index*0x48u);
    select_animation_template(file,animation,script);
    animation.field_4e8=reinterpret_cast<std::uint32_t>(group);animation.base.flags[1]|=0x200;
    if(layer>=0) {
        animation.base.fields_10_28[1]=static_cast<std::uint32_t>(layer);
        if(layer<24) animation.base.flags[2]=(animation.base.flags[2]&~0x03000000u)|0x01000000u;
    }
    if(!position && !(flags&8)) animation.vector_5bc={};
    else if(position) {
        if(!(flags&1)) animation.vector_5bc=*position;
        else {
            // Original scalar SSE steps: (640-384)/2 + 384/2 + x, y+16.
            constexpr float center=320.f; // All preceding constant operations are exact in binary32.
            animation.vector_5bc={n::add32(center,position->x),n::add32(position->y,16.f),position->z};
        }
    }
    animation.base.vector_38.z=rotation_z;
    animation.base.flags[1]=(animation.base.flags[1]&~0x40000u)|((flags&4)<<16);
    execute_animation(animation);
    // This call updates inherited transform caches even though its result is unused.
    (void)animation_position(animation);
    out=0;animation.fields_550[9]=flags;
    out=register_animation(c,group,animation,(flags&4)!=0,(flags&2)!=0);
    if(flags&4) animation.base.flags[2]&=~3u;
}
std::uint32_t spawn_named_animation(Controller& c,AnimationFile& file,const char* name,
    std::int32_t script,std::int32_t layer,Animation** output_animation) {
    std::uint32_t result;spawn_named_animation(c,file,result,name,script,nullptr,0.f,layer,0,output_animation);return result;
}
}
