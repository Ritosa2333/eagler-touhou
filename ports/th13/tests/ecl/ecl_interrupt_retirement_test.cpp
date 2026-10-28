#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Items:StageItemHost {
    i32 ordinary=0,spirits=0;
    void spawn_item(float,float,float,i32,float,float)override{++ordinary;}
    void spawn_value_item(i32,float,float,float,float)override{++spirits;}
    void on_et_clear(float,float,float)override{}
};
TEST(interrupt_deletion_does_not_repeat_death_drops_or_spirits) {
    StageSim sim;Items items;sim.items=&items;
    std::vector<u8> finish,root;
    th13test::emit(finish,0,509,0,255,0,{});
    th13test::emit(finish,0,1,0,255,0,{});
    th13test::emit(root,0,23,0,255,0,{600});
    th13test::emit(root,0,10,0,255,0,{});
    auto data=th13test::build_scpt({{"Finish",finish},{"main",root}});
    sim.program.attach(data.data(),u32(data.size()));sim.commands.sim=&sim;sim.gvars.sim=&sim;
    auto* e=sim.spawn_enemy("main",0,100,20,nullptr,false);
    e->invulnerable=0;e->drop_main=1;e->drop_extra[0]=2;
    e->interrupts[0].life=0;e->interrupts[0].health_label="Finish";
    int sounds=0,effects=0,retired=0;
    sim.enemy_sound=[&](i32){++sounds;return true;};
    sim.enemy_death_effect=[&](i32,i32,const Vec3&,float){++effects;return true;};
    sim.retire_sink=[&](u32){++retired;};
    const u32 before=sim.random.index();sim.damage(*e,30);
    CHECK(e->dead);CHECK_EQ(items.ordinary,3);CHECK_EQ(items.spirits,0);
    CHECK_EQ(sounds,0);CHECK_EQ(effects,0);CHECK_EQ(retired,1);
    CHECK_EQ(sim.random.index()-before,10u); // scatter initial + two*(unit,angle)
    sim.damage(*e,30);CHECK_EQ(items.ordinary,3);CHECK_EQ(retired,1);
}
TEST(create_arguments_are_visible_before_constructor_and_preserve_main_drop) {
    StageSim sim;
    std::vector<u8> child,root;
    th13test::emit(child,0,506,0,255,0,{});
    // Constructor reads inherited F0/F1 and I0 immediately.
    th13test::emit(child,0,500,3,255,0,{float_to_bits(-9981.f),float_to_bits(-9980.f)});
    th13test::emit(child,0,1001,1,255,0,{u32(-9985)});
    th13test::emit(child,0,23,0,255,0,{600});
    th13test::emit(root,0,23,0,255,0,{600});
    auto data=th13test::build_scpt({{"Child",child},{"main",root}});
    sim.program.attach(data.data(),u32(data.size()));sim.commands.sim=&sim;sim.gvars.sim=&sim;
    auto* parent=sim.spawn_enemy("main",10,100,100,nullptr,false);
    parent->i_slots[0]=42;parent->f_slots[0]=24;parent->f_slots[1]=36;sim.current=parent;
    std::vector<u8> command;
    th13test::emit(command,0,300,0,255,0,{8,0x6c696843,0x64,float_to_bits(3),float_to_bits(5),3000,20000,1});
    EclContext context;context.instruction=reinterpret_cast<const EclInstruction*>(command.data());
    CHECK_EQ(sim.commands.execute(context,300),0);CHECK_EQ(sim.enemies.size(),size_t(2));
    if(sim.enemies.size()!=2)return;
    const auto& e=*sim.enemies.back();
    CHECK_EQ(e.position[0],13.f);CHECK_EQ(e.position[1],105.f);
    CHECK_EQ(e.life,3000.f);CHECK_EQ(e.spawn_score,20000);CHECK_EQ(e.drop_main,1);
    CHECK_EQ(e.death_spirit_limit,42);CHECK_EQ(e.hurtbox[0],24.f);CHECK_EQ(e.hurtbox[1],36.f);
}
TEST(kill_all_emits_effects_then_runs_death_script_with_cleared_drops) {
 StageSim sim;Items items;sim.items=&items;sim.commands.sim=&sim;sim.gvars.sim=&sim;
 std::vector<u8> root,death;
 th13test::emit(root,0,23,0,255,0,{1000});th13test::emit(root,0,10,0,255,0,{});
 th13test::emit(death,0,516,0,255,0,{9});th13test::emit(death,0,1,0,255,0,{});
 auto data=th13test::build_scpt({{"Death",death},{"main",root}});CHECK_EQ(sim.program.attach(data.data(),u32(data.size())),0);
 auto* e=sim.spawn_enemy("main",0,100,100,nullptr,false);CHECK(e);if(!e)return;
 e->flags=0;e->death_sound=5;e->death_effect=39;e->death_routine="Death";
 e->drop_main=2;e->drop_extra[0]=7;
 auto immune=std::make_unique<StageEnemy>();immune->flags=32;sim.enemies.push_back(std::move(immune));
 std::vector<int> order;
 sim.enemy_sound=[&](i32 id){order.push_back(id);return true;};
 sim.enemy_death_effect=[&](i32,i32 id,const Vec3&,float){order.push_back(id);return true;};
 sim.current=e;EclContext context;std::vector<u8> code;th13test::emit(code,0,525,0,255,0,{});
 context.instruction=reinterpret_cast<const EclInstruction*>(code.data());const auto before=sim.random.index();
 CHECK_EQ(sim.commands.execute(context,525),0);
 CHECK((order==std::vector<int>{5,39,9}));CHECK(e->dead);CHECK(e->flags&0x4000000u);
 CHECK(!sim.enemies.back()->dead);CHECK_EQ(items.ordinary,0);CHECK_EQ(items.spirits,0);
 CHECK_EQ(sim.random.index(),before+2);CHECK(e->death_routine.empty());
 CHECK_EQ(e->drop_area[0],32.f);CHECK_EQ(e->drop_area[1],32.f);
}
}
int main(){return th13test::run_all("interrupt-retirement");}
