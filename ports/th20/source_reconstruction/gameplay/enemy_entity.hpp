#pragma once
#include "enemy_spawn.hpp"
#include "script_loader.hpp"
#include "../ecl_vm/vm.hpp"
#include <functional>
namespace th20::source::sprite {struct RenderMesh;}
namespace th20::source::gameplay {
class ScriptManager;
struct EnemyVmEnvironment {
    ecl::RandomStream* random;
    const float* clock_rate;
};
struct EnemyRuntimeStack {
    std::pmr::vector<std::uint32_t> words;
    std::int32_t pointer=0,frame_base=0;
    std::uint32_t& absolute(std::int32_t);                   //53e630
    std::uint32_t& local(std::int32_t);                      //53e6a0
    void push(std::uint32_t,char type=0);                   //53f260, four-byte path
    std::uint32_t pop(char type=0);                         //53f0b0, four-byte path
    std::uint32_t peek(std::int32_t,char);                   //540450
    bool enter_frame(std::int32_t);                        //5405b0
    void leave_frame();                                    //540300
};
struct EnemyRuntime {
    float time=0;
    std::int32_t subroutine=0,instruction_offset=0;
    EnemyRuntimeStack stack;
    std::int32_t async_id=0;
    ScriptManager* manager=nullptr;
    std::uint32_t field_2c=0;
    std::uint8_t rank=0,padding_31[3];
    std::pmr::vector<ecl::math::Interpolator> interpolators;
    std::uint32_t flags=0;
    bool active() const noexcept{return subroutine!=-1&&instruction_offset!=-1;}
    ecl::Instruction current();                            //5403d0
    std::int32_t integer_argument(ecl::Instruction,std::int32_t,bool consume=false);
    float float_argument(ecl::Instruction,std::int32_t,bool consume=false);
    std::int32_t integer_value(ecl::Instruction,std::int32_t,std::uint32_t bits); //53ee30, displaced string payload
    float float_value(ecl::Instruction,std::int32_t,float value); //53ea90
    std::uint32_t& integer_destination(ecl::Instruction,std::int32_t);
    std::uint32_t& float_destination(ecl::Instruction,std::int32_t);
    bool call(ecl::Instruction);                           //53f3b0 synchronous
    bool call_into(ecl::Instruction,EnemyRuntime&,std::int32_t skip);
    void tick_interpolators(const float*);                  //53e00c
    std::int32_t tick(float,EnemyVmEnvironment&);             //53b5c0
};
static_assert(sizeof(EnemyRuntimeStack)==0x18&&sizeof(EnemyRuntime)==0x48);
static_assert(offsetof(EnemyRuntime,rank)==0x30&&offsetof(EnemyRuntime,interpolators)==0x34);
class ScriptManager {
public:
    virtual ~ScriptManager();                               //4a3ce0
    virtual int execute_opcode()=0;                         //vtable+4
    virtual std::int32_t read_integer(std::int32_t)=0;         //+8
    virtual std::uint32_t* integer_destination(std::int32_t)=0;//+c
    virtual float read_float(std::int32_t)=0;                //+10
    virtual std::uint32_t* float_destination(std::int32_t)=0; //+14
    std::uint32_t field_04=0,field_08=0;
    EnemyRuntime* current=nullptr;
    EnemyRuntime main;
    ScriptLoader* loader=nullptr;
    scheduler::Link runtimes{};                              //main sentinel holds &main after reset
    void reset();                                           //4972c0
    void clear_async();                                     //4973c0
    void select_script(const char*);                         //540200/540550/540590/540510
    EnemyRuntime& spawn(EnemyRuntime&,ecl::Instruction,std::int32_t id,std::int32_t skip); //53e390
    EnemyRuntime* find_runtime(std::int32_t);                //53e920 returns link in original, exposed value here
    void terminate_async();                                //53e560
    std::int32_t tick(float,EnemyVmEnvironment&);             //53e2b0
};
static_assert(sizeof(ScriptManager)==0x70&&offsetof(ScriptManager,main)==0x10);
struct EnemyMeshOwner {sprite::RenderMesh* mesh;std::uint32_t field_04;float radius,current_radius;std::uint32_t color;float phase_x,phase_y;}; //48b010/48b370,4a4190
static_assert(sizeof(EnemyMeshOwner)==0x1c);
class EnemyLifecycleServices {
public:
    virtual ~EnemyLifecycleServices()=default;
    virtual void delete_animation(std::uint32_t&)=0;          //44fcd0
    virtual void destroy_mesh(sprite::RenderMesh*)=0;         //4a2800
};
EnemyLifecycleServices& enemy_lifecycle_services();
#pragma pack(push,4)
class Enemy final:public ScriptManager {
public:
    std::uint32_t field_70;
    scheduler::Link controller_link;
    EnemyState state;
    SpawnParameters spawn_parameters;
    scheduler::List children;
    scheduler::Link parent_link;
    std::function<void(Enemy*)> callback;                    //5140b0/5139e0/511910 pass Enemy* to capture
    std::int32_t player_index;
    game_session::Context* context;
    Enemy();                                                //4a33c0
    ~Enemy() override;                                      //4a3b10
    int execute_opcode() override;                         //4969e0 ->48c010
    std::int32_t read_integer(std::int32_t) override;         //49abc0
    std::uint32_t* integer_destination(std::int32_t) override;//498600
    float read_float(std::int32_t) override;                 //4995d0
    std::uint32_t* float_destination(std::int32_t) override;  //498210
    void initialize(int,const char*,game_session::Session&); //4a73f0
    void select_context(int,game_session::Session&) noexcept; //4ab970/4ab930
};
#pragma pack(pop)
// Enemy embeds pmr containers and a std::function; MSVC x86 STL layout
// evidence, binding on the Windows build only (TH_SDL3 uses libc++ layouts).
#ifndef TH_SDL3
static_assert(sizeof(Enemy)==0x428&&offsetof(Enemy,state)==0x88&&offsetof(Enemy,spawn_parameters)==0x378);
static_assert(offsetof(Enemy,children)==0x3cc&&offsetof(Enemy,callback)==0x3f8&&offsetof(Enemy,context)==0x424);
#endif
void retire_enemy(Enemy*);                                   //4a2720
namespace unrecovered {
int execute_enemy_opcode_0048c010(EnemyState&);
std::int32_t read_enemy_integer_0049abc0(Enemy&,std::int32_t);
float read_enemy_float_004995d0(Enemy&,std::int32_t);
}
}
