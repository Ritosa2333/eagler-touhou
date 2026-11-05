#include "regions.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <new>
namespace th20::source::damage {
namespace n=th20::recovered;
void construct_region(Region& region) noexcept {
    scheduler::initialize_link(region.link,reinterpret_cast<scheduler::Node*>(&region));
    // Every remaining 32-bit word is written by the original member constructors.
    std::memset(&region.flags,0,sizeof(Region)-offsetof(Region,flags));
}
void select_context(Region& region,std::int32_t index) noexcept {region.view_index=index;region.context=&game_session::context(index);}
void set_position(Region& region,const sprite::Vec3& position) noexcept {region.motion.position=position;}
void activate(Region& region) noexcept {region.flags|=0x10;}
namespace {
void initialize_damage(Region& region,std::int32_t frames,std::int32_t damage){
    n::timer_set(region.lifetime,frames);region.damage=damage;region.total_damage=0;region.damage_limit=9999999;region.period=1;
    region.hit_callback=0;region.last_target=0;region.cooldown=0;region.damage_group=0;
}
}
void set_rectangle(Region& region,const sprite::Vec3& position,float width,float height,float angle,std::int32_t frames,std::int32_t damage){
    region.flags=(region.flags&~0x5eu)|1;std::memset(&region.motion,0,sizeof(region.motion));set_position(region,position);
    region.size={width,height};region.angle=ecl::math::wrap_angle(angle);region.angle_step=0;initialize_damage(region,frames,damage);
}
void set_circle(Region& region,const sprite::Vec3& position,float radius,float growth,std::int32_t frames,std::int32_t damage){
    region.flags=(region.flags&~0x5eu)|3;std::memset(&region.motion,0,sizeof(region.motion));set_position(region,position);
    region.radius=radius;region.radius_step=growth;initialize_damage(region,frames,damage);
}
std::uint32_t create_rectangle(HitCtrlInf& owner,const sprite::Vec3& position,float width,float height,float angle,std::int32_t frames,std::int32_t damage){auto* region=owner.allocate();if(!region)return 0;select_context(*region,owner.view_index);set_rectangle(*region,position,width,height,angle,frames,damage);return region->handle;}
std::uint32_t create_circle(HitCtrlInf& owner,const sprite::Vec3& position,float radius,float growth,std::int32_t frames,std::int32_t damage){auto* region=owner.allocate();if(!region)return 0;select_context(*region,owner.view_index);set_circle(*region,position,radius,growth,frames,damage);return region->handle;}
void retire(Region& region){
    const auto handle=region.handle;if(!handle)return;
    auto& owner=*static_cast<HitCtrlInf*>(region.context->object_28);owner.detach(region);region.flags&=~1u;region.handle=0;
    if(handle&0x1000000u){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(&region);}
}
void update(Region& region){
    state::update_motion(region.motion,state::clock_scale);region.radius=n::add32(region.radius,region.radius_step);region.angle=ecl::math::wrap_angle(n::add32(region.angle,region.angle_step));
    region.last_target=0;n::timer_add(region.lifetime,-1.f,state::timer_rate);region.cooldown=n::signed_bits(static_cast<std::uint32_t>(region.cooldown)-1);
    if(region.lifetime.current<=0)retire(region);
}
Region* find_handle(std::uint32_t& handle) noexcept {auto* result=controller(0)->find(handle);if(!result)handle=0;return result;}
std::uint32_t activate_handle(std::uint32_t& handle){if(find_handle(handle))activate(*find_handle(handle));return handle;}
void retire_handle(std::uint32_t& handle){if(find_handle(handle))retire(*find_handle(handle));}
void set_handle_position(std::uint32_t& handle,const sprite::Vec3& position){if(find_handle(handle))set_position(*find_handle(handle),position);}
}
