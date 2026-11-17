#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;StageEnemy enemy;EclContext context;
    Rig(){sim.commands.sim=&sim;sim.current=&enemy;sim.interactive=true;sim.player_x=sim.player_y=0;}
    int op(u16 code,std::initializer_list<u32> args){
        std::vector<u8> bytes;th13test::emit(bytes,0,code,0,255,0,args);
        context.instruction=reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context,code);
    }
};
TEST(protection_checks_final_spawn_and_preserves_boundary_contact) {
    // Native40e840 gate; stop before ANM creation, observe accepted/rejected.
    const float cases[][5]={{40,39,0,0,0},{40,40,0,0,1},{40,41,0,0,1},
        {-40,39,0,0,0},{0,0,0,0,1},{40,0,0,40,1},{40,0,0,39,0},
        {40,24,32,0,1},{40,24,31,0,0}};
    for(const auto& row:cases){
        Rig r;CHECK_EQ(r.op(600,{0}),0);CHECK_EQ(r.op(526,{float_to_bits(row[0])}),0);
        CHECK_EQ(r.enemy.bullet_protect_radius2,row[0]*row[0]);
        r.enemy.position[0]=row[1];r.enemy.position[1]=row[2];
        r.enemy.et[0].spin=row[3];r.enemy.et[0].aim=1;
        CHECK_EQ(r.op(601,{0}),0);
        CHECK_EQ(r.sim.bullets.size(),size_t(row[4]));
        CHECK_EQ(r.sim.active_bullet_protect_radius2,0.f);
    }
}
TEST(protection_does_not_skip_random_sampling_or_leak_to_another_enemy) {
    Rig r;CHECK_EQ(r.op(600,{0}),0);CHECK_EQ(r.op(526,{float_to_bits(40)}),0);
    r.enemy.et[0].aim=8;r.enemy.et[0].count=3;r.enemy.et[0].ways=2;
    const auto before=r.sim.random.index();CHECK_EQ(r.op(601,{0}),0);
    CHECK(r.sim.bullets.empty());CHECK_EQ(r.sim.random.index()-before,24u);
    StageEnemy other;other.et[0].reset();r.sim.current=&other;
    CHECK_EQ(r.op(601,{0}),0);CHECK_EQ(r.sim.bullets.size(),1u);
}
}
int main(){return th13test::run_all("ecl-bullet-protection");}
