#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(manager_variables_share_storage_between_enemies_and_both_read_views) {
 StageSim sim;StageEnemy first,second;sim.gvars.sim=&sim;
 for(i32 id=-9926;id<=-9915;++id){
  sim.current=&first;
  if(id<=-9923){auto* p=sim.gvars.integer_destination(id);CHECK(p);if(p)*p=-7;CHECK(sim.gvars.float_destination(id)==nullptr);}
  else{auto* p=sim.gvars.float_destination(id);CHECK(p);if(p)*p=-7.75f;CHECK(sim.gvars.integer_destination(id)==nullptr);}
  sim.current=&second;i32 iv=0;float fv=0;
  CHECK(sim.gvars.integer(id,iv));CHECK_EQ(iv,-7);
  CHECK(sim.gvars.floating(id,fv));CHECK_EQ(fv,id<=-9923?-7.f:-7.75f);
 }
 StageSim next;next.gvars.sim=&next;i32 value=1;float real=1;
 CHECK(next.gvars.integer(-9925,value));CHECK_EQ(value,0);
 CHECK(next.gvars.floating(-9922,real));CHECK_EQ(real,0.f);
}
int main(){return th13test::run_all("shared-ecl-vars");}
