#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(player_position_aliases_use_the_live_player_and_do_not_consume_random) {
 StageSim sim;sim.gvars.sim=&sim;sim.player_x=-11.7890625f;sim.player_y=287.5390625f;
 const auto before=sim.random.index();float f=0;i32 i=0;
 CHECK(sim.gvars.floating(-9965,f));CHECK_EQ(f,-11.7890625f);
 CHECK(sim.gvars.floating(-9964,f));CHECK_EQ(f,287.5390625f);
 CHECK(sim.gvars.integer(-9965,i));CHECK_EQ(i,-11);
 CHECK(sim.gvars.integer(-9964,i));CHECK_EQ(i,287);CHECK_EQ(sim.random.index(),before);
}
TEST(boss_variable_views_read_primary_boss_not_current_ship_or_secondary_slot) {
 StageSim sim;sim.gvars.sim=&sim;StageEnemy ship;sim.current=&ship;ship.i_slots[0]=99;
 auto boss=std::make_unique<StageEnemy>();boss->script_id=17;
 const i32 ints[]={1,-7,42,16777217};const float floats[]={1.25f,-7.75f,128.f,.125f};
 for(int n=0;n<4;++n){boss->i_slots[n]=ints[n];boss->f_slots[n]=floats[n];}
 sim.enemies.push_back(std::move(boss));sim.boss_slot[0]=17;
 for(int n=0;n<4;++n){float f=0;i32 i=0;
  CHECK(sim.gvars.floating(-9943+n,f));CHECK_EQ(f,float(ints[n]));
  CHECK(sim.gvars.integer(-9943+n,i));CHECK_EQ(i,ints[n]);
  CHECK(sim.gvars.floating(-9939+n,f));CHECK_EQ(f,floats[n]);
  CHECK(sim.gvars.integer(-9939+n,i));CHECK_EQ(i,i32(floats[n]));
 }
 sim.boss_slot[0]=-1;sim.boss_slot[1]=17;
 for(int id=-9943;id<=-9936;++id){float f=3;i32 i=3;CHECK(sim.gvars.floating(id,f));CHECK_EQ(f,0.f);CHECK(sim.gvars.integer(id,i));CHECK_EQ(i,0);}
}
TEST(stage5_height_branch_selects_random_movement_only_below_the_player_threshold) {
 StageSim sim;sim.gvars.sim=&sim;float y=0;
 sim.player_y=287.5390625f;CHECK(sim.gvars.floating(-9964,y));CHECK(!(y<128.f));
 sim.player_y=127.5f;CHECK(sim.gvars.floating(-9964,y));CHECK(y<128.f);
}
int main(){return th13test::run_all("boss global variable views");}
