#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(wait_completion_uses_the_original_integer_countdown_at_fractional_rates) {
    for(float rate:{1.f,.5f,.25f}) {
        StageSim sim;sim.set_time_rate(rate);sim.bullet_animation=[](StageBullet&,bool){return true;};
        StageBullet b;b.x=0;b.y=200;b.vel[0]=2;
        b.ex[0].op=0x80000000u;b.ex[0].args[4]=7;sim.bullets.push_back(b);
        const int completion=rate==1?7:rate==.5f?13:25;
        for(int frame=0;frame<=completion;++frame){
            sim.advance_bullets();const auto& current=sim.bullets[0];
            CHECK_EQ(current.behavior_flags,frame<completion?0x80000000u:0u);
            CHECK_EQ(current.x,float(frame+1)*2*rate);
        }
    }
}
TEST(acceleration_duration_uses_fractional_time_without_skipping_updates) {
    StageSim sim;sim.set_time_rate(.25f);sim.bullet_animation=[](StageBullet&,bool){return true;};
    StageBullet b;b.y=200;b.ex[0].op=4;b.ex[0].args[0]=1;b.ex[0].args[1]=0;b.ex[0].args[4]=2;
    sim.bullets.push_back(b);
    for(int frame=0;frame<8;++frame){sim.advance_bullets();const auto& v=sim.bullets[0];
        CHECK_EQ(v.vel[0],float(frame+1)*.25f);CHECK_EQ(v.ex4_timer,(frame+1)/4);
        CHECK_EQ(v.ex4_time,float(frame+1)*.25f);CHECK_EQ(v.behavior_flags,4u);
    }
    sim.advance_bullets();CHECK_EQ(sim.bullets[0].behavior_flags,0u);
    CHECK_EQ(sim.bullets[0].vel[0],2.f);
}
TEST(delayed_bullet_collision_uses_scaled_manager_age_and_half_displacement) {
    StageSim sim;sim.set_time_rate(.25f);sim.bullet_animation=[](StageBullet&,bool){return true;};
    StageBullet b;b.state=2;b.animation_ready=false;b.collision_ready=false;b.y=100;b.vel[0]=2;
    sim.bullets.push_back(b);
    for(int frame=0;frame<34;++frame){
        sim.advance_bullets();const auto& v=sim.bullets[0];
        CHECK_EQ(v.x,float(frame+1)*.25f);
        CHECK_EQ(v.state_age,(frame+1)/4);CHECK_EQ(v.state_time,float(frame+1)*.25f);
        CHECK_EQ(v.collision_ready,frame>=32);
    }
}
int main(){return th13test::run_all("bullet-rate");}
