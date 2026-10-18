#pragma once
#include "gameplay.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include <memory_resource>
#include <vector>
#include <string>
namespace th20::source::gameplay {
class ScriptLoader;
class EnemyController;
struct EnemyData { //4a2fc0, real scalar/handle/timer subobject atEnemyCtrl+10
    std::uint32_t fields_00[12],fields_30[5];
    std::uint32_t handles_44[16];
    std::uint32_t field_84,field_88;
    recovered::Timer timer_8c;
    std::uint32_t field_9c,field_a0;
};
static_assert(sizeof(EnemyData)==0xa4 && offsetof(EnemyData,timer_8c)==0x8c);
void construct_enemy_data(EnemyData&) noexcept; //4a2fc0/47baa0/49ebe0/422d90
void reset_enemy_counters(EnemyData&) noexcept; //4ab1b0, only first48bytes
extern std::uint32_t previous_enemy_generation,current_enemy_generation; //BSS5c49ec/f0
std::uint32_t advance_enemy_generation(std::int32_t player) noexcept; //4ab140
class EnemyServices {
public:
    virtual ~EnemyServices()=default;
    virtual runtime::Log& log()=0;
    virtual scheduler::State& scheduler_state()=0;
    virtual scheduler::Environment& scheduler_environment()=0;
    virtual sprite::Controller& sprites()=0;
    virtual std::uint32_t& graphics_flags()=0;
    virtual int update_enemy(EnemyController&)=0;              //4a5040's actual enemy update dependencies
    virtual void draw_enemy_overlay()=0;                      //513dd0->512aa0
    virtual void select_layer(int,int)=0;                     //44f3d0
    virtual void retire_entity(void*)=0;                      //4a2720, Enemy has ECL-manager base, not CallbackOwner
    virtual sprite::AnimationFile* existing_animation(EnemyController&,unsigned)=0; //4aae10, slot7 is background's file
};
#pragma pack(push,4)
class EnemyController final:public runtime::CallbackOwner {
public:
    EnemyData data;                                           //+10
    std::pmr::vector<std::pmr::string> loaded_names;            //+b4,4141c0
    std::uint32_t field_c4,field_c8,field_cc;
    recovered::Timer timer_d0;
    std::uint32_t field_e0;
    sprite::AnimationFile* animation_files[8];                 //+e4
    ScriptLoader* loader;                                    //+104
    scheduler::List enemies;                                 //+108
    std::uint32_t field_120,field_124,field_128;
    std::int32_t player_index;                               //+12c
    game_session::Context* context;                          //+130
    EnemyServices* services;                                 //source-only+134
    explicit EnemyController(EnemyServices&);                 //zeroed4a28c0->4a2e80
    ~EnemyController() override;                             //4a3920
    int initialize(int,const char*);                         //4a7080
    int update_callback();                                  //4aa1b0 exact game flags gates
    int draw_callback();                                    //4a5ba0
    void clear_entities();                                  //4a80f0
    void reset_for_stage();                                  //4a7040
};
#pragma pack(pop)
static_assert(offsetof(EnemyController,loaded_names)==0xb4);
static_assert(offsetof(EnemyController,loader)==0x104 && offsetof(EnemyController,enemies)==0x108);
static_assert(offsetof(EnemyController,services)==0x134);
EnemyController* create_enemy_controller(EnemyServices&,int,const char*); //4aba70
EnemyController& enemy_controller(int index=0);               //478060->Context+8
void destroy_enemy_controller(int index=0);                  //4aa940->4bd270
EnemyServices& enemy_services();
namespace unrecovered {
void draw_enemy_overlay_00512aa0();
void destroy_enemy_entity_004a2720(void*);
}
}
