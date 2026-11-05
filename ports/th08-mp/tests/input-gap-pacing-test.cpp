#include "../web/cpp/multiplayer/InputGapPacing.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <initializer_list>

using th08::multiplayer::InputGapPacing;
static void equal(double actual,double expected){assert(std::abs(actual-expected)<1.e-12);}

static void stable_prediction_is_free(){
    InputGapPacing pacing;
    // A stable necessary prediction window never earns a slowdown, however
    // many times the pump observes it or how long the match has run.
    for(std::uint32_t missing=0;missing<=4;++missing){
        pacing.Reset(0);
        for(std::uint64_t now:{1ull,25ull,50ull,150ull,10'000ull}){
            pacing.Update(now,missing,true);equal(pacing.Scale(),1);
        }
    }
}
static void shortage_targets_and_bounds(){
    struct Case{std::uint32_t missing;double expected;};
    const Case cases[]{{5,4./3},{6,5./3},{7,2},{8,2},
        {std::numeric_limits<std::uint32_t>::max(),2}};
    for(const auto& test:cases){
        InputGapPacing pacing;pacing.Reset(100);
        pacing.Update(150,test.missing,true);equal(pacing.Scale(),test.expected);
        pacing.Update(10'000,test.missing,true);equal(pacing.Scale(),test.expected);
        pacing.Update(20'000,0,true);equal(pacing.Scale(),1);
    }
}
static void wall_time_ramp_and_release(){
    InputGapPacing pacing;pacing.Reset(0);
    pacing.Update(25,7,true);equal(pacing.Scale(),1.5);
    for(unsigned repeat=0;repeat<20;++repeat){
        pacing.Update(25,7,true);equal(pacing.Scale(),1.5);
        pacing.Update(25,0,true);equal(pacing.Scale(),1.5);
    }
    pacing.Update(10,7,true);equal(pacing.Scale(),1.5);
    pacing.Update(40,7,true);equal(pacing.Scale(),1.8);
    pacing.Update(50,7,true);equal(pacing.Scale(),2);
    // Recovery takes 150 ms for the full 2 -> 1 interval change; it must
    // not instantly turn a released backlog into a full-speed burst.
    pacing.Update(75,2,true);equal(pacing.Scale(),11./6);
    pacing.Update(150,3,true);equal(pacing.Scale(),4./3);
    pacing.Update(200,4,true);equal(pacing.Scale(),1);
    pacing.Update(250,8,true);equal(pacing.Scale(),2);
    pacing.Update(300,6,true);equal(pacing.Scale(),5./3);
    pacing.Update(350,5,true);equal(pacing.Scale(),4./3);
}
static void disabled_and_new_session_reset(){
    InputGapPacing pacing;pacing.Reset(1'000);
    pacing.Update(1'050,8,true);equal(pacing.Scale(),2);
    pacing.Update(1'050,8,false);equal(pacing.Scale(),1);
    pacing.Update(2'000,8,false);equal(pacing.Scale(),1);
    pacing.Update(2'000,8,true);equal(pacing.Scale(),1);
    pacing.Update(2'025,8,true);equal(pacing.Scale(),1.5);
    pacing.Reset(0);equal(pacing.Scale(),1);
    pacing.Update(0,8,true);equal(pacing.Scale(),1);
    pacing.Update(25,8,true);equal(pacing.Scale(),1.5);
}
int main(){
    stable_prediction_is_free();shortage_targets_and_bounds();
    wall_time_ramp_and_release();disabled_and_new_session_reset();
    std::puts("TH08 H8 input-gap pacing targets, wall-time ramps and lifecycle reset: PASS");
}
