#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
#include <memory>
namespace th20::source::bullet {
struct ExtendedCommand { //47bd90
    recovered::Timer timer{};
    float field_10{},field_14{};
    sprite::Vec3 vector_18{},vector_24{};
    std::uint32_t field_30{},field_34{},field_38{},field_3c{};
};
struct alignas(8) Bullet { //47be00, shared ownership at98 is genuine C++ ownership.
    scheduler::Link link;
    std::uint32_t flags{};
    std::int32_t field_18{};
    sprite::Animation* animation{};
    float field_20{},field_24{};
    std::uint32_t index{},field_2c{},field_30{};
    std::int32_t cancel_script{};
    std::uint32_t field_38{},field_3c{},field_40{},field_44{};
    float scale{};
    std::int16_t field_4c{},field_4e{};
    std::int32_t state{};
    std::uint32_t field_54{},field_58{},animation_handle{},field_60{};
    sprite::Vec3 position{},velocity{};
    sprite::Vec2 size{};
    float angle{};
    std::uint32_t handle{};
    std::uint32_t padding_8c;
    std::uint64_t field_90{};
    std::shared_ptr<void> metadata;
    ExtendedCommand commands[14];
    sprite::Interpolation<sprite::Vec3> interpolation_420{};
    sprite::Interpolation<float> interpolation_474{};
    recovered::Timer timer_4a0{},timer_4b0{},timer_4c0{};
    std::uint32_t field_4d0{},field_4d4{};
    recovered::Timer timer_4d8{},timer_4e8{},timer_4f8{},timer_508{};
    std::uint32_t field_518{};
    std::int32_t view_index{};
    game_session::Context* context{};
    std::uint32_t padding_524;
    Bullet() noexcept;
    ~Bullet()=default; //47c470, metadata is the only nontrivial member.
};
class alignas(8) Controller final:public runtime::CallbackOwner { //BulletInf
public:
    Bullet* next_bullet{};
    std::uint32_t fields_14[13]{};
    float field_48{};
    sprite::Vec2 vector_4c{},vector_54{};
    std::uint32_t padding_5c;
    Bullet pool[2001];
    std::uint32_t handles[2001]{};
    scheduler::List free,active;
    std::uint32_t age{},item_counter{},field_286d84{},cancel_counter{};
    sprite::AnimationFile* file{};
    std::uint32_t field_286d90{},field_286d94{},field_286d98{};
    std::int32_t view_index{};
    game_session::Context* context{};
    std::uint32_t padding_286da4;
    Controller(); //47b8a0
    ~Controller() override; //47c340
    int initialize(std::int32_t); //480fa0
    void select_context(std::int32_t) noexcept; //486530
    int update(); //47d600
    int draw(); //47dc90
};
static_assert(sizeof(ExtendedCommand)==0x40&&sizeof(Bullet)==0x528);
static_assert(offsetof(Bullet,metadata)==0x98&&offsetof(Bullet,commands)==0xa0&&offsetof(Bullet,interpolation_420)==0x420);
static_assert(offsetof(Bullet,timer_4d8)==0x4d8&&offsetof(Bullet,context)==0x520);
static_assert(offsetof(Controller,pool)==0x60&&offsetof(Controller,handles)==0x284e08);
static_assert(offsetof(Controller,free)==0x286d4c&&offsetof(Controller,context)==0x286da0&&sizeof(Controller)==0x286da8);
Controller* controller(std::int32_t=0) noexcept;
Controller* create_controller(std::int32_t=0); //486630/47b5f0
void destroy_controller(std::int32_t=0); //485340
void assign_timer_float(recovered::Timer&,float) noexcept; //4841f0
bool outside_viewport(const sprite::Vec3&,float half_width,float half_height) noexcept; //485390
bool in_circle(const sprite::Vec3&,const sprite::Vec3&,float radius) noexcept; //485a60
int cancel(Bullet&,std::int32_t drop_mode); //47c8f0
int update_bullet(Bullet&); //47d960
void drop_items(Bullet&,const sprite::Vec3&,std::int32_t); //47d3d0
std::int32_t cancel_rectangle(Controller&,const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t drop_mode,std::uint32_t kind); //47cc90
std::int32_t cancel_circle(Controller&,const sprite::Vec3&,float,std::int32_t drop_mode,std::int32_t limit,std::uint32_t kind); //47cf60
namespace unrecovered {
int advance_bullet_00485b60(Bullet&);
int player_hit_test_00484a20(Bullet&,std::int32_t);
void item_spawn_004c45b0(void*,const sprite::Vec3&,std::int32_t,std::int32_t);
void item_spawn_004c3c90(void*,std::int32_t,const sprite::Vec3&,std::int32_t,float,float,std::int32_t,std::int32_t,std::int32_t);
}
}
