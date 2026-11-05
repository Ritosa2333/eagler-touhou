#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../runtime_state/motion.hpp"
namespace th20::source::gameplay {class EnemyController;}
namespace th20::source::bomb {
// Concrete ownership and virtual slots recovered from vtables56fa78/56fa94.
// A Bomb is a separate polymorphic type; it has no scheduler-owner prefix.
using Motion=state::Motion;
#pragma pack(push,4)
class Bomb {
public:
    virtual ~Bomb();                                          //477a60
    virtual int start(std::int32_t);                           //477d30
    virtual int update();                                    //412540, actual zero-return base slot
    virtual int draw();                                      //412540
    virtual int finish();                                    //412540
    virtual int event(std::uint32_t,std::uint32_t);             //477ce0
    std::uint32_t field_04;
    recovered::Timer timer,secondary_timer;
    Motion motion;
    std::uint32_t handle_70,handle_74,field_78,field_7c;
    sprite::Interpolation<float> interpolation;
    std::uint32_t handle_ac;
    std::int32_t view_index;
    game_session::Context* context;
    Bomb();                                                  //478430
    void select_context(std::int32_t) noexcept;                //478290
};
class Controller final:public runtime::CallbackOwner {
public:
    std::uint32_t field_10;
    Bomb* active_bomb;
    std::int32_t active_state;
    recovered::Timer timer;
    std::uint32_t field_2c,field_30;
    std::int32_t view_index;
    game_session::Context* context;
    Controller();                                            //4779b0
    ~Controller() override;                                  //477ae0
    int initialize(std::int32_t);                             //477dc0
    void select_context(std::int32_t) noexcept;                //4782d0
    int update();                                            //477c20
    int draw();                                              //477cb0
    int event(std::uint32_t,std::uint32_t);                    //477cf0
    int finish();                                            //477f00
    int trigger();                                           //477e30
    bool can_trigger();                                      //4780a0
    bool active() const noexcept {return active_state==1;}    //478130
};
#pragma pack(pop)
static_assert(sizeof(Bomb)==0xb8&&offsetof(Bomb,motion)==0x28&&offsetof(Bomb,interpolation)==0x80);
static_assert(sizeof(Controller)==0x3c&&offsetof(Controller,timer)==0x1c);
Controller* controller(std::int32_t index=0) noexcept;         //478040
Controller* create_controller(std::int32_t index=0);           //478300/477960
void destroy_controller(std::int32_t index=0);                //477fd0/4bd1b0
void retire_bomb(Bomb*);                                     //41f7c0 with Bomb's actual virtual type
void add_enemy_bomb_counter(gameplay::EnemyController&,std::int32_t) noexcept; //477f40
void mark_enemies_for_bomb(gameplay::EnemyController&);        //478190
void set_enemy_bomb_flag(void* entity,std::uint32_t) noexcept; //478260, non-owning view of Enemy+358
void add_player_meter(game_session::Player&,std::int32_t) noexcept; //477f60
std::int32_t bomb_count(game_session::Player&) noexcept;       //477ff0, mutating clamp
void consume_bomb(game_session::Player&);                      //4e1710
void notify_bomb_start(void* secondary_owner);                 //487bb0, Context+10 owner state transition
namespace unrecovered {
Bomb* create_00479140();
Bomb* create_004783a0();
}
}
