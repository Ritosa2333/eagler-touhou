#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;StageEnemy caller;EclContext context;
    Rig(){sim.commands.sim=&sim;sim.current=&caller;}
    i32 exists(i32 id){
        std::vector<u8> code;th13test::emit(code,0,555,1,255,0,{0,u32(id)});
        context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(context,555),0);
        i32 result=-1;CHECK(context.integer(0,0,result));return result;
    }
};
TEST(existence_query_does_not_use_missing_target_lookup_fallback) {
    Rig r;
    for(int id:{2,7}){auto e=std::make_unique<StageEnemy>();e->script_id=id;r.sim.enemies.push_back(std::move(e));}
    CHECK_EQ(r.exists(2),1);CHECK_EQ(r.exists(7),1);CHECK_EQ(r.exists(99),0);CHECK_EQ(r.exists(0),0);
    CHECK(r.sim.find_by_script_id(99)==r.sim.enemies.back().get());
    r.sim.enemies.erase(r.sim.enemies.begin());
    CHECK_EQ(r.exists(2),0);CHECK_EQ(r.exists(7),1);
    r.sim.enemies.back()->dead=true; // original417f10 reads membership until manager removal
    CHECK_EQ(r.exists(7),1);
    r.sim.enemies.clear();CHECK_EQ(r.exists(7),0);
}
TEST(death_effect_instruction_does_not_drop_or_terminate_and_forced_drop_is_separate) {
    Rig r;std::vector<u8> code;
    int sounds=0,effects=0;
    r.caller.death_sound=5;r.caller.death_effect=39;r.caller.drop_main=2;
    r.sim.enemy_sound=[&](i32 id){CHECK_EQ(id,5);++sounds;return true;};
    r.sim.enemy_death_effect=[&](i32,i32 script,const Vec3&,float angle){
        CHECK_EQ(script,39);CHECK_EQ(angle,-1.5707964f);++effects;return true;
    };
    th13test::emit(code,0,561,0,255,0,{});
    r.context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(r.sim.commands.execute(r.context,561),0);
    CHECK(!r.caller.dead);CHECK_EQ(r.sim.random.index(),0u);
    CHECK_EQ(r.caller.drop_main,2);CHECK_EQ(sounds,1);CHECK_EQ(effects,1);
    code.clear();th13test::emit(code,0,562,0,255,0,{});
    r.context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    r.sim.mode_flags=0x20; //562 is unconditional;509 has a mode gate
    CHECK_EQ(r.sim.commands.execute(r.context,562),0);
    CHECK(!r.caller.dead);CHECK_EQ(r.caller.drop_main,0);CHECK_EQ(r.sim.random.index(),2u);
    CHECK_EQ(sounds,1);CHECK_EQ(effects,1);
}
}
int main(){return th13test::run_all("enemy-existence");}
