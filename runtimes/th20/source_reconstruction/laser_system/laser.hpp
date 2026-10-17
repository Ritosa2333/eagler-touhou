#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../bullet_system/bullet.hpp"
namespace th20::source::laser {
class Controller;
std::uint32_t spawn_laser(Controller&,std::uint32_t kind,const void* parameters); //4d47f0
// The manager's entity contract. Concrete beam classes are still required;
// these pure methods cannot silently substitute for their original behavior.
// Original virtual slot annotations identify evidence, not a native ABI bridge.
#pragma pack(push,4)
class Laser {
public:
    Laser() noexcept; //4c85b0, complete6f8 storage; C++ keeps its own vptr
    virtual ~Laser(); //4c8af0, original slot84; independent C++ destructor
    virtual int update()=0; //1c
    virtual int draw()=0; //20
    virtual int finish()=0; //24
    virtual int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t)=0; //2c
    virtual int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t)=0; //30
    virtual int erase(std::int32_t,std::int32_t)=0; //44, complete-beam erasure
    virtual int cancel_all(std::int32_t)=0; //4c
    std::uint32_t field_04;
    scheduler::Link link; //08
    std::uint32_t flags; //1c
    std::int32_t state;
    std::uint32_t field_24;
    recovered::Timer age; //28
    recovered::Timer timer_38,timer_48;
    sprite::Vec3 position,velocity; //58/64
    float angle,field_74,speed,field_7c;
    float field_80;
    std::uint32_t field_84;
    std::uint32_t field_88,handle;
    bullet::ExtendedCommand commands[24]; //90
    std::uint32_t command_index,field_694;
    std::uint64_t active_commands; //698, used by4d7280 and4d12e0
    std::uint32_t field_6a0,field_6a4,field_6a8;
    recovered::Timer timer_6ac,timer_6bc;
    std::uint32_t field_6cc,field_6d0,field_6d4;
    void* allocated_6d8;
    std::uint32_t field_6dc,field_6e0,field_6e4,field_6e8;
    std::int32_t view_index;
    game_session::Context* context;
    std::uint32_t field_6f4;
    void select_context(std::int32_t) noexcept; //4d7dd0
    virtual void set_position(const sprite::Vec3&) noexcept; //original slot0,4d7d50
    virtual void set_parameter_flag(std::uint32_t) noexcept {} //original slot0c,40ec90 literal no-op for types0/1/2
    void point_at_distance(float,sprite::Vec3&) const; //4d67e0, preserves output.z then adds position.z
    int proximity(const sprite::Vec3&,float) const; //4d46e0
};
#pragma pack(pop)
static_assert(offsetof(Laser,link)==8&&offsetof(Laser,flags)==0x1c&&offsetof(Laser,age)==0x28);
static_assert(offsetof(Laser,commands)==0x90&&offsetof(Laser,allocated_6d8)==0x6d8&&offsetof(Laser,context)==0x6f0&&sizeof(Laser)==0x6f8);
class Controller final:public runtime::CallbackOwner {
public:
    scheduler::List active;
    std::uint32_t count,next_handle;
    sprite::Vec3 cancel_position,cancel_size;
    sprite::AnimationFile* file;
    std::uint32_t field_4c;
    std::int32_t view_index;
    game_session::Context* context;
    Controller(); //4c87c0
    ~Controller() override; //4c8b70
    int initialize(std::int32_t); //4d3ca0
    void select_context(std::int32_t) noexcept; //4d7e10
    void clear(); //4bc480
    void attach(Laser&); //4c9300
    void detach(Laser&); //4cfb80
    int update(); //4cf680
    int draw(); //4d00c0
    int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t); //4c9db0
    int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t); //4caad0
    void erase_all(std::int32_t,std::int32_t); //4c9490, dispatches original slot44
};
static_assert(sizeof(Controller)==0x58&&offsetof(Controller,cancel_position)==0x30&&offsetof(Controller,context)==0x54);
Controller* controller(std::int32_t=0) noexcept; //41ca90
Controller* create_controller(std::int32_t=0); //4d7e40/4c80e0
void destroy_laser(Laser*); //4c7f20/4b9aa0
float project_to_segment(const sprite::Vec3&,float,float,const sprite::Vec3&,sprite::Vec3&); //454d80
}
