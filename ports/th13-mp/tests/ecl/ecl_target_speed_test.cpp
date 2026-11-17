#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th13;

namespace {
u32 bits(float value){u32 out;std::memcpy(&out,&value,4);return out;}
void check(float direction,const std::array<u32,6>& first,const std::array<u32,6>& last) {
    StageSim sim;sim.player_x=30;sim.player_y=40;
    StageBullet bullet;bullet.alive=true;bullet.state=1;bullet.speed=.2f;
    bullet.vel[0]=.2f;bullet.angle=0;bullet.behavior_flags=0;
    bullet.ex[0].op=0x200000;bullet.ex[0].args[0]=3;
    bullet.ex[0].args[1]=direction;bullet.ex[0].args[4]=60;
    sim.process_bullet_queue(bullet,false);
    CHECK(sim.error().empty());
    CHECK_EQ(bits(bullet.target_speed_step),1027548556u);
    sim.bullets.push_back(bullet);
    for(int frame=0;frame<61;++frame) {
        sim.advance_bullets();CHECK(sim.error().empty());CHECK_EQ(sim.bullets.size(),1u);
        if(sim.bullets.empty())return;
        const auto& b=sim.bullets[0];
        const std::array<u32,6> state={bits(b.speed),bits(b.angle),bits(b.vel[0]),bits(b.vel[1]),b.behavior_flags,u32(b.target_speed_time)};
        if(frame==0)for(size_t i=0;i<6;++i)CHECK_EQ(state[i],first[i]);
        if(frame==59)for(size_t i=0;i<6;++i)CHECK_EQ(state[i],last[i]);
        if(frame==60){CHECK_EQ(b.behavior_flags,0u);CHECK_EQ(b.target_speed_time,60);}
        if(std::getenv("TH13_TARGET_SPEED_TRACE")){
            std::printf("T %.9g %d",double(direction),frame);
            for(auto value:state)std::printf(" %u",value);std::printf("\n");
        }
    }
}
// Original 0040f030 setup + 00410400 updates, synthetic .2-speed bullet;
// target3 over60 ticks, keep heading / explicit heading / aimed heading.
TEST(target_speed_keep_heading) {
    check(-999999,{1048352304,0,1048352304,0,2097152,1},
                   {1077936121,0,1077936121,0,2097152,60});
}
TEST(target_speed_turn_keeps_scalar_speed_independent_of_velocity_length) {
    check(1.5707964f,{1048352304,1047182278,1045220557,1027548556,2097152,1},
                    {1077936121,1069543232,1045220557,1077097261,2097152,60});
}
TEST(target_speed_aims_once_at_queue_consumption) {
    check(999,{1048352304,1042690712,1047099605,1025043158,2097152,1},
              {1077936121,1063215053,1072735190,1074748458,2097152,60});
}
}
int main(){return th13test::run_all("ecl-target-speed");}
