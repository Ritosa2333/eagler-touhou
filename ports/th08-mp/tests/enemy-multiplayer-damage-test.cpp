#include "EnemyDamage.hpp"
#include "GameConfiguration.hpp"
#include "GameGauge.hpp"
#include <cassert>

using namespace th08;

struct Pilots final:EnemyDamageActions {
    PlayerFrameState frames[3]{};
    i32 hits[3]{},bombs[3]{};
    u8 forms[3]{};
    i32 damage_calls[3]{};
    i16 gauge_values[3]{},previous_gauge_values[3]{};
    GaugeThresholds thresholds[3]{};
    GameGauge gauges[3]{{gauge_values[0],previous_gauge_values[0],thresholds[0]},
                       {gauge_values[1],previous_gauge_values[1],thresholds[1]},
                       {gauge_values[2],previous_gauge_values[2],thresholds[2]}};
    u32 count=2;
    bool available[3]{true,true,true};
    i32 damage(const Vec3&,const Vec3&,i32&,i32&) override {return 0;}
    u32 participant_count()const override{return count;}
    bool participant(u32 seat,EnemyDamageParticipant& out) override {
        out={{float(80+seat*12),300,0},&frames[seat],&gauges[seat],0,u8(bombs[seat]!=0),0,forms[seat]};
        return seat<count&&available[seat];
    }
    i32 participant_damage(u32 seat,const Vec3&,const Vec3&,i32&,i32& bomb_hit) override {
        ++damage_calls[seat];
        bomb_hit=bombs[seat];
        return hits[seat];
    }
};

int main(){
    GameGlobals shared{};
    GameConfiguration config{},display{};
    HighScore high_score{};
    Rng random{};
    GameValues values(shared,config,display,high_score,random);
    Pilots pilots;
    EclVm boss{};
    boss.flags=0x4a; // damageable, vulnerable, boss
    boss.life=100;
    boss.resolved_position={100,100,0};
    boss.hitbox={16,16,0};
    pilots.hits[0]=40;pilots.hits[1]=20;
    i32 bomb_hit=-1;u32 owner=99;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==55&&boss.last_damage==45);
    assert(shared.score==12&&bomb_hit==0&&owner==0);
    assert(pilots.frames[0].boss_target&&pilots.frames[1].boss_target);
    assert(pilots.frames[0].target_reference==&boss&&pilots.frames[1].target_reference==&boss);

    // The native 70-per-enemy-frame cap and TH07 three-player boss scale
    // apply once to combined sources; a tie belongs to the lower seat.
    pilots.count=3;pilots.hits[0]=40;pilots.hits[1]=40;pilots.hits[2]=40;
    boss.life=100;shared.score=0;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==54&&boss.last_damage==46);
    assert(shared.score==14&&owner==0);
    assert(pilots.frames[2].boss_target);

    // A ghost in the middle seat does not truncate the roster iteration.
    pilots.available[1]=false;pilots.hits[0]=20;pilots.hits[2]=40;
    boss.life=100;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==55&&owner==2); // Remaining 2P scale: (20+40)*.75.
    pilots.available[0]=false;boss.life=100;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==60&&owner==2); // Last player returns to native damage.
    pilots.available[0]=pilots.available[1]=true;
    pilots.hits[0]=pilots.hits[1]=0;pilots.hits[2]=60;pilots.bombs[2]=1;
    boss.life=100;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==60); // No extra 3P bomb reduction beyond boss scaling.
    pilots.bombs[2]=0;

    // A Last Spell suppresses bomb damage when its flag disallows bombs,
    // while the ordinary source still contributes before boss balancing.
    pilots.count=2;pilots.hits[0]=20;pilots.hits[1]=30;pilots.bombs[1]=1;
    boss.life=100;shared.score=0;
    assert(damage_enemy_multiplayer(boss,1,0,values,bomb_hit,owner,pilots));
    assert(boss.life==99&&boss.last_damage==1);
    assert(shared.score==10&&bomb_hit==1&&owner==1);

    // Current form alone controls familiar damage for both seats. In
    // particular P1 must not hit in youkai form with a still-human gauge,
    // and P2 must hit in human form with a still-youkai gauge.
    pilots.bombs[0]=pilots.bombs[1]=0;
    pilots.hits[0]=40;pilots.hits[1]=20;
    EclVm parent{};
    for(u8 host=0;host<2;++host)for(u8 guest=0;guest<2;++guest)
    for(i16 host_gauge:{i16(-10000),i16(10000)})for(i16 guest_gauge:{i16(-10000),i16(10000)}){
        pilots.forms[0]=host;pilots.forms[1]=guest;
        pilots.gauges[0].set(host_gauge);pilots.gauges[1].set(guest_gauge);
        pilots.damage_calls[0]=pilots.damage_calls[1]=0;
        EclVm familiar{};familiar.parent=&parent;familiar.flags=0x149u|(u32(host)<<11);familiar.life=100;
        const i32 expected=(host?0:40)+(guest?0:20);
        assert(damage_enemy_multiplayer(familiar,0,0,values,bomb_hit,owner,pilots)==(expected!=0));
        assert(familiar.life==100-expected&&familiar.last_damage==expected);
        assert(pilots.damage_calls[0]==!host&&pilots.damage_calls[1]==!guest);
        if(expected)assert(owner==(host?1u:0u));
    }
    // Ordinary enemies still receive damage from both forms.
    boss.life=100;
    assert(damage_enemy_multiplayer(boss,0,0,values,bomb_hit,owner,pilots));
    assert(boss.life==55);
}
