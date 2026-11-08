#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/AnmVm.hpp"
using namespace th13;
namespace {
struct Rig {
 StageSim sim;AnmVm vm;u32 serial=0;
 Rig(float x,float y,u32 flags){std::vector<u8>s;th13test::emit(s,0,23,0,255,0,{1000});th13test::emit(s,0,10,0,255,0,{});auto b=th13test::build_scpt({{"main",s}},{});sim.program.attach(b.data(),u32(b.size()));sim.commands.sim=&sim;sim.gvars.sim=&sim;
 vm.visual.sprite_size={32,48};vm.visual.scale={1,-1};sim.enemy_animation=[&](StageEnemy&,i32){return &vm;};auto*e=sim.spawn_enemy("main",x,y,10,nullptr,false);serial=e->serial;e->flags=flags;e->tick_gate=false;}
 bool tick(){CHECK(sim.step_frame());return !sim.enemies.empty();}
};
TEST(enemy_cull_uses_absolute_anm_extent_and_inclusive_edges) {
 for(float x:{-216.f,-193.f,193.f,216.f}){Rig r(x,100,0x20000);CHECK(r.tick());CHECK_EQ(r.sim.enemies[0]->anm_size_a[0],48.f);}
 for(float x:{-217.f,217.f}){Rig r(x,100,0x20000);CHECK(!r.tick());}
 for(float y:{-16.f,464.f}){Rig r(0,y,0x20000);CHECK(r.tick());}
 for(float y:{-17.f,465.f}){Rig r(0,y,0x20000);CHECK(!r.tick());}
}
TEST(enemy_cull_observes_live_scale_and_preserves_last_extent_without_vm) {
 Rig r(-210,100,0x20000);CHECK(r.tick());r.vm.visual.scale.y=.5f;CHECK(!r.tick());
 Rig missing(-210,100,0x20000);CHECK(missing.tick());missing.sim.enemy_animation=[](StageEnemy&,i32)->AnmVm*{return nullptr;};CHECK(missing.tick());
}
TEST(enemy_cull_retention_bits_and_first_entry_gate_match_original) {
 {Rig r(-300,-100,0);CHECK(r.tick());}
 {Rig r(-300,-100,0x20004);CHECK(r.tick());} // X branch returns without checking Y
 {Rig r(-300,-100,0x20008);CHECK(!r.tick());}
 {Rig r(0,-100,0x20008);CHECK(r.tick());}
}
}
int main(){return th13test::run_all("enemy-culling");}
