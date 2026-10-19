#include "pool.hpp"
#include "anm_vm.hpp"
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <cstring>
#include <new>
namespace th20::source::sprite {
namespace {
namespace q=th20::source::scheduler;
q::Link& link(AnimationLink& l) noexcept {return reinterpret_cast<q::Link&>(l);}
q::List& list(AnimationList& l) noexcept {return reinterpret_cast<q::List&>(l);}
void destroy_callback(AnimationCallback* callback) {
    if(!callback)return;
    callback->~AnimationCallback();
    std::lock_guard lock(runtime::shared_locks().slot(1));
    ::operator delete(callback);
}
}
void initialize_animation_list(AnimationList& l) noexcept {q::initialize_list(list(l));}
void initialize_animation_link(AnimationLink& l,Animation* a) noexcept {q::initialize_link(link(l),reinterpret_cast<q::Node*>(a));}
void prepend_animation_link(AnimationList& l,AnimationLink& added) noexcept {
    q::insert_after(link(l.sentinel),link(added));added.owner=&l;
    if(l.tail==&l.sentinel)l.tail=&added;
}
Animation* create_heap_animation() {
    auto* a=static_cast<Animation*>(::operator new(sizeof(Animation)));
    std::memset(a,0,sizeof(*a));construct_animation(*a);return a;
}
Animation* allocate_animation(Controller& controller) {
    auto* free=controller.free_sentinel.next;
    if(!free){auto* a=create_heap_animation();reset_animation_state(*a);clear_animation_suffix(*a);a->index=0xffff;return a;}
    auto& pooled=*reinterpret_cast<PooledAnimation*>(free->value);auto& a=pooled.animation;
    q::unlink(link(pooled.free_link));a.index=pooled.index;pooled.active=1;
    reset_animation_state(a);clear_animation_suffix(a);a.fields_550[2]=a.fields_550[3]=0;
    initialize_animation_link(a.links[0],&a);initialize_animation_link(a.links[2],&a);initialize_animation_link(a.links[3],&a);
    return &a;
}
void release_animation_geometry(Animation& a) noexcept {
    if(a.fields_550[5]){runtime::release_bytes(reinterpret_cast<void*>(a.fields_550[5]));a.fields_550[5]=a.fields_550[7]=0;}
}
void destroy_animation_contents(Animation& a) {
    if(a.fields_550[5])runtime::release_bytes(reinterpret_cast<void*>(a.fields_550[5]));
    a.fields_550[5]=a.fields_550[7]=0;
    destroy_callback(reinterpret_cast<AnimationCallback*>(a.fields_550[6]));a.fields_550[6]=0;
    a.handle=0;a.base.fields_10_28[6]=0xffffffffu;
    // 44c2c3..44c2e2 really loops forever when +550 is nonzero. This is
    // an original invalid-lifetime invariant, not an unrecovered function.
    if(a.fields_550[0])for(;;)*reinterpret_cast<volatile std::uint32_t*>(&a.handle)=0;
}
void destroy_heap_animation(Animation* a) {
    if(!a)return;destroy_animation_contents(*a);
    std::lock_guard lock(runtime::shared_locks().slot(1));::operator delete(a);
}
bool is_pooled_animation(const Controller& c,const Animation* a) noexcept {
    const auto address=reinterpret_cast<std::uintptr_t>(a);
    return address>=reinterpret_cast<std::uintptr_t>(&c.pool[0]) && address<=reinterpret_cast<std::uintptr_t>(&c.pool[0xffff]);
}
std::int32_t retire_animation(Controller& c,Animation& a) {
    if(auto* callback=reinterpret_cast<AnimationCallback*>(a.fields_550[6])){callback->retire();destroy_callback(reinterpret_cast<AnimationCallback*>(a.fields_550[6]));a.fields_550[6]=0;}
    release_animation_geometry(a);q::unlink(link(a.links[0]));q::unlink(link(a.links[4]));
    a.fields_550[2]=a.fields_550[3]=0;
    if(!is_pooled_animation(c,&a))destroy_heap_animation(&a);
    else {auto& pooled=c.pool[a.index];pooled.active=0;q::insert_after(link(c.free_sentinel),link(pooled.free_link));destroy_animation_contents(a);a.handle=0;}
    return 0;
}
std::uint32_t assign_animation_handle(Controller& c,Animation& a) noexcept {
    c.field_7d40e88=(c.field_7d40e88+1)&0xffffu;if(!c.field_7d40e88)++c.field_7d40e88;
    a.handle=(c.field_7d40e88<<16)|(a.index&0xffffu);return a.handle;
}
Animation* find_animation(Controller& c,std::uint32_t handle) noexcept {
    if(!handle)return nullptr;const auto index=handle&0xffffu;
    if(index==0xffff){for(unsigned group=0;group<2;++group)for(auto* l=c.lists[group].sentinel.next;l;l=l->next)if(l->value->handle==handle)return l->value;return nullptr;}
    auto& pooled=c.pool[index];return pooled.active && pooled.animation.handle==handle?&pooled.animation:nullptr;
}
Animation* resolve_animation_handle(Controller& c,std::uint32_t& handle) noexcept {
    auto* a=find_animation(c,handle);if(!a)handle=0;return a;
}
std::uint32_t register_animation(Controller& c,AnimationList* group,Animation& a,bool secondary,bool front) noexcept {
    a.field_4e8=reinterpret_cast<std::uint32_t>(group);auto& destination=group[secondary?1:0];
    if(front)prepend_animation_link(destination,a.links[0]);else q::append(list(destination),link(a.links[0]));
    return assign_animation_handle(c,a);
}
void retire_animation_group(Controller& c,AnimationList* group) {
    for(unsigned i=0;i<2;++i)for(q::Iterator it(reinterpret_cast<q::Link*>(group[i].sentinel.next));it.current;it.advance())retire_animation(c,*reinterpret_cast<Animation*>(it.current->value));
}
void queue_animation_retirement(Animation& a,AnimationList& retired) {
    if(auto* first=a.links[3].next)for(q::Iterator it(reinterpret_cast<q::Link*>(first));it.current;it.advance())queue_animation_retirement(*reinterpret_cast<Animation*>(it.current->value),retired);
    if(a.fields_550[8]!=2){initialize_animation_link(a.links[4],&a);q::append(list(retired),link(a.links[4]));}
    a.fields_550[8]=2;a.fields_550[2]=a.fields_550[3]=0;a.fields_550[1]=a.fields_550[0]=0;
    q::unlink(link(a.links[2]));
}
void update_animation_group(Controller& c,AnimationList* group,bool secondary) {
    AnimationList retired;initialize_animation_list(retired);
    for(q::Iterator it(reinterpret_cast<q::Link*>(group[secondary?1:0].sentinel.next));it.current;it.advance()){
        auto& a=*reinterpret_cast<Animation*>(it.current->value);a.fields_550[1]=a.fields_550[0]=0;
        if(a.fields_550[8]==1 || (a.fields_550[8]==0 && execute_animation(a)!=0))queue_animation_retirement(a,retired);
    }
    for(q::Iterator it(reinterpret_cast<q::Link*>(retired.sentinel.next));it.current;it.advance())retire_animation(c,*reinterpret_cast<Animation*>(it.current->value));
}
std::int32_t update_animations(Controller& c,bool secondary) {
    std::lock_guard lock(runtime::shared_locks().slot(9));if(secondary)c.fields_c8[4]=0;
    update_animation_group(c,c.lists,secondary);return 1;
}
void mark_file_animations(Controller& c,AnimationFile* file,bool preserve_flagged) noexcept {
    if(!file)return;
    for(unsigned i=0;i<2;++i)for(q::Iterator it(reinterpret_cast<q::Link*>(c.lists[i].sentinel.next));it.current;it.advance()){
        auto& a=*reinterpret_cast<Animation*>(it.current->value);
        if((a.base.fields_10_28[2]==file->id || a.base.fields_10_28[3]==file->id) && (!preserve_flagged || !(a.base.flags[6]&2)))a.fields_550[8]=1;
    }
}
void set_animation_color(Animation& a,std::uint32_t color) noexcept {a.base.field_490=color;}
namespace file_environment {
void detach_file_animations(Controller& c,AnimationFile& file,bool preserve_flagged){mark_file_animations(c,&file,preserve_flagged);}
void destroy_animation_contents(Animation& a){sprite::destroy_animation_contents(a);}
}
}
