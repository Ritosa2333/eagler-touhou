#include "pool.hpp"
#include "binding.hpp"
#include "anm_vm.hpp"
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::sprite {
namespace n=th20::recovered;
Vec3 detached_animation_position(Animation& a) {
    Vec3 v{n::add32(n::add32(a.vector_5bc.x,a.base.vector_2c.x),a.base.vector_484.x),n::add32(n::add32(a.vector_5bc.y,a.base.vector_2c.y),a.base.vector_484.y),n::add32(n::add32(a.vector_5bc.z,a.base.vector_2c.z),a.base.vector_484.z)};
    if(a.fields_550[2] && !(a.base.flags[1]&0x1000)){
        auto& parent=*reinterpret_cast<Animation*>(a.fields_550[2]);
        if(a.base.flags[1]&0x20)ecl::math::rotate(v.x,v.y,parent.base.vector_38.z);
        const auto p=detached_animation_position(parent);v.x=n::add32(v.x,p.x);v.y=n::add32(v.y,p.y);v.z=n::add32(v.z,p.z);
    }return v;
}
namespace {
std::uint32_t register_spawn(Controller& controller,Animation& a,std::uint32_t owner,std::uint32_t flags) {
    a.fields_550[9]=flags;if(!owner)return 0;
    const auto result=register_animation(controller,reinterpret_cast<AnimationList*>(owner),a,(flags&4)!=0,(flags&2)!=0);
    if(flags&4)a.base.flags[2]&=~3u;return result;
}
}
std::uint32_t spawn_child_animation(Controller& c,AnimationFile& file,Animation& parent,std::int32_t script,std::uint32_t flags) {
    std::lock_guard lock(runtime::shared_locks().slot(9));++file.fields_5c[3];
    auto& a=*allocate_animation(c);const auto owner=parent.field_4e8;
    a.base.fields_10_28[1]=parent.base.fields_10_28[1];a.base.flags[1]|=0x200;
    a.vector_5bc={};a.field_4e8=owner;a.base.flags[1]=(a.base.flags[1]&~0x40000u)|((flags&4)<<16);
    bind_animation_script(file,a,script,&parent);const auto result=register_spawn(c,a,owner,flags);
    scheduler::insert_after(reinterpret_cast<scheduler::Link&>(parent.links[3]),reinterpret_cast<scheduler::Link&>(a.links[2]));return result;
}
std::uint32_t spawn_detached_animation(Controller& c,AnimationFile& file,Animation& parent,std::int32_t script,std::uint32_t flags) {
    std::lock_guard lock(runtime::shared_locks().slot(9));++file.fields_5c[3];auto& a=*allocate_animation(c);const auto owner=parent.field_4e8;
    select_animation_template(file,a,script);a.field_4e8=owner;a.base.fields_10_28[1]=parent.base.fields_10_28[1];a.base.flags[1]|=0x200;
    a.base.flags[1]=(a.base.flags[1]&~0x1000000u)|(parent.base.flags[1]&0x1000000u);
    a.base.flags[1]=(a.base.flags[1]&~0x40000u)|((flags&4)<<16);a.vector_5bc=detached_animation_position(parent);
    a.base.vector_38=parent.base.vector_38;a.base.vector_484=parent.base.vector_2c;execute_animation(a);return register_spawn(c,a,owner,flags);
}
}
