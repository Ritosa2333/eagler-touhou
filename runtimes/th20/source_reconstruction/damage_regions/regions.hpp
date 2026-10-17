#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_state/state.hpp"
#include "../runtime_state/motion.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
namespace th20::source::damage {
struct Region {
    scheduler::Link link;
    std::uint32_t flags;
    float radius,inner_radius,radius_step,angle,angle_step;
    sprite::Vec2 size;
    state::Motion motion;
    recovered::Timer lifetime;
    std::uint32_t handle;
    std::int32_t field_90,damage,total_damage,damage_limit,period;
    std::uint32_t last_target;
    std::int32_t cooldown,damage_group;
    std::uint32_t callback_target;
    std::int32_t hit_callback,polygon_sides,view_index;
    game_session::Context* context;
};
static_assert(offsetof(Region,motion)==0x34 && offsetof(Region,lifetime)==0x7c && offsetof(Region,handle)==0x8c && sizeof(Region)==0xc4);
// Name is identified by the original "initialize HitCtrlInf" diagnostic.
class HitCtrlInf final:public runtime::CallbackOwner {
public:
    Region pool[256];
    std::uint32_t next_handle;
    scheduler::List active,free;
    std::uint32_t visited_regions;
    recovered::Timer age;
    std::int32_t view_index;
    game_session::Context* context;
    HitCtrlInf(); //4bfee0
    ~HitCtrlInf() override; //4c0120
    int initialize(std::int32_t); //4c0b30
    void select_context(std::int32_t) noexcept; //4c2090
    void initialize_pool(std::int32_t); //4c0b30 pool portion
    Region* allocate(); //4c0e60
    Region* find(std::uint32_t handle) noexcept; //4c0d00
    void detach(Region&) noexcept; //4c1eb0
    int update(); //4c02d0
    void advance_handle() noexcept; //4c0c60
};
static_assert(offsetof(HitCtrlInf,pool)==0x10 && offsetof(HitCtrlInf,next_handle)==0xc410);
static_assert(offsetof(HitCtrlInf,age)==0xc448 && sizeof(HitCtrlInf)==0xc460);
HitCtrlInf* controller(std::int32_t index=0) noexcept; //478ec0 ->412710
HitCtrlInf* create_controller(std::int32_t index=0); //4c20d0 ->4bfde0
void construct_region(Region&) noexcept; //4bffb0
void select_context(Region&,std::int32_t) noexcept; //488a20
void set_rectangle(Region&,const sprite::Vec3&,float width,float height,float angle,std::int32_t frames,std::int32_t damage); //4c1c50
void set_circle(Region&,const sprite::Vec3&,float radius,float growth,std::int32_t frames,std::int32_t damage); //4c1d80
std::uint32_t create_rectangle(HitCtrlInf&,const sprite::Vec3&,float,float,float,std::int32_t,std::int32_t); //4c1ae0
std::uint32_t create_circle(HitCtrlInf&,const sprite::Vec3&,float,float,std::int32_t,std::int32_t); //4c1b80
void set_position(Region&,const sprite::Vec3&) noexcept; //4c2030
void activate(Region&) noexcept; //47a4a0
void retire(Region&); //4c1f30
void update(Region&); //4c03d0
bool intersects(const Region&,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius); //4c1030
Region* find_handle(std::uint32_t&) noexcept; //4c0e10, intentionally searches context0
std::uint32_t activate_handle(std::uint32_t&); //4c1c10
void retire_handle(std::uint32_t&); //4c1f00
void set_handle_position(std::uint32_t&,const sprite::Vec3&); //4c1ff0
}
