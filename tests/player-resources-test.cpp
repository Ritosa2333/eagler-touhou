#include "PlayerResources.hpp"
#include "GameGauge.hpp"
#include <cassert>

using namespace th08;

int main(){
    GameGlobals shared{};
    HighScore high_score{};
    PilotResources pilot0{},pilot1{},pilot2{};
    PlayerResourceView view0(shared,pilot0),view1(shared,pilot1),view2(shared,pilot2);
    PlayerValues values0(view0,high_score),values1(view1,high_score),values2(view2,high_score);
    GaugeThresholds thresholds0{},thresholds1{},thresholds2{};
    thresholds0.configure(3);
    thresholds1.configure(10);
    thresholds2.configure(4);
    GameGauge gauge0(view0.gauge,view0.gauge_copy,thresholds0);
    GameGauge gauge1(view1.gauge,view1.gauge_copy,thresholds1);
    GameGauge gauge2(view2.gauge,view2.gauge_copy,thresholds2);

    // Shared values are aliases to the same native owner from every seat.
    shared.score=500;
    shared.time_orbs=4;
    shared.total_time_orbs=4;
    shared.last_spell_requirement=12;
    shared.clock_time=6;
    values0.add_score(120);
    values1.add_time_orbs(3);
    values2.add_clock(2);
    assert(view0.score==512&&view1.score==512&&view2.score==512);
    assert(view0.time_orbs==7&&view1.time_orbs==7&&view2.time_orbs==7);
    assert(view0.total_time_orbs==7&&view2.last_spell_requirement==12);
    assert(view0.clock_time==8&&view1.clock_time==8&&view2.clock_time==8);
    assert(shared.point_value==20&&high_score.time_orbs==3);

    // Personal resources and counters remain isolated while statistics that
    // belong to the native shared record still accumulate in that record.
    values0.set_lives(2);
    values1.set_lives(5);
    values2.set_lives(8);
    values0.set_bombs(1);
    values1.set_bombs(3);
    values2.set_bombs(6);
    values0.set_power(16);
    values1.set_power(64);
    values2.set_power(128);
    values0.add_lives(1);
    values1.add_bombs(2);
    values2.add_power(-8);
    assert(pilot0.lives==3&&pilot1.lives==5&&pilot2.lives==8);
    assert(pilot0.bombs==1&&pilot1.bombs==5&&pilot2.bombs==6);
    assert(pilot0.power==16&&pilot1.power==64&&pilot2.power==120);
    assert(pilot0.gauge==0&&pilot1.gauge==0&&pilot2.gauge==0);
    assert(shared.lives==0&&shared.bombs==0&&shared.power==0&&shared.gauge==0);

    pilot0.deaths=1;
    pilot1.deaths=4;
    pilot0.deaths_stage=1;
    values0.add_deaths(2);
    values1.add_deaths(1);
    pilot1.bombs_used=3;
    pilot1.bombs_used_stage=1;
    values1.count_bombs(2);
    assert(pilot0.deaths==3&&pilot0.deaths_stage==3);
    assert(pilot1.deaths==5&&pilot1.deaths_stage==1&&pilot2.deaths==0);
    assert(pilot1.bombs_used==5&&pilot1.bombs_used_stage==3);
    assert(pilot0.bombs_used==0&&pilot2.bombs_used==0);
    assert(high_score.deaths==2);

    // Each pilot owns both its gauge and fighter-specific thresholds.
    gauge0.add(12000,false,false);
    gauge1.add(12000,false,false);
    gauge2.add(12000,false,false);
    assert(gauge0.value()==10000&&pilot0.gauge_copy==10000&&gauge0.youkai_bonus());
    assert(gauge1.value()==5000&&pilot1.gauge_copy==5000&&gauge1.youkai_bonus());
    assert(gauge2.value()==2000&&pilot2.gauge_copy==2000&&!gauge2.youkai_bonus()&&!gauge2.youkai());
    gauge0.set(-3500);gauge1.set(-3500);gauge2.set(-3500);
    assert(gauge0.human_bonus()&&gauge1.human_bonus()&&!gauge2.human_bonus());
    const i16 before0=gauge0.value(),before2=gauge2.value();
    const i16 previous0=pilot0.gauge_copy,previous2=pilot2.gauge_copy;
    gauge0.add(500,true,false);
    assert(gauge0.value()==before0&&pilot0.gauge_copy==previous0);
    gauge1.add(500,true,true);
    assert(gauge1.value()==-3000&&pilot1.gauge_copy==-3000);
    assert(gauge2.value()==before2&&pilot2.gauge_copy==previous2);

    // Restore the owner in place; long-lived references and its value adapter
    // must remain attached to the same pilot storage.
    const PilotResources checkpoint=pilot1;
    float* const lives_address=&pilot1.lives;
    pilot1.reset();
    assert(&view1.lives==lives_address&&view1.lives==0);
    pilot1.restore(checkpoint);
    assert(&view1.lives==lives_address&&view1.lives==5);
    values1.add_lives(2);
    assert(pilot1.lives==7&&pilot0.lives==3&&pilot2.lives==8);

    // The TH08 negative-orb underflow path clamps only the current pool.
    shared.time_orbs=2;
    shared.total_time_orbs=17;
    high_score.time_orbs=9;
    values2.add_time_orbs(-3);
    assert(shared.time_orbs==0&&shared.total_time_orbs==17&&high_score.time_orbs==9);
    // The shared MP score owner does not run the original byte-checksum RNG
    // protocol against a different, per-pilot economy layout.
    GameConfiguration config{},display{};Rng random{};
    GameValues shared_values(shared,config,display,high_score,random);
    const auto random_before=random;
    shared_values.initialize_integrity();shared_values.update_integrity();
    shared_values.randomize_integrity();shared_values.refresh_integrity();
    assert(shared_values.checksum()==0&&!shared_values.tampered());
    assert(std::memcmp(&random,&random_before,sizeof(random))==0);
}
