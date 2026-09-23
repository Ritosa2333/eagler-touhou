#include "EnemyDamage.hpp"
#include "GameConfiguration.hpp"
#include <cassert>

using namespace th08;

struct Pilots final:EnemyDamageActions {
    PlayerFrameState frames[3]{};
    i32 hits[3]{},bombs[3]{};
    u32 count=2;
    i32 damage(const Vec3&,const Vec3&,i32&,i32&) override {return 0;}
    u32 participant_count()const override{return count;}
    bool participant(u32 seat,EnemyDamageParticipant& out) override {
        out={{float(80+seat*12),300,0},&frames[seat],nullptr,0,u8(bombs[seat]!=0),0};
        return seat<count;
    }
    i32 participant_damage(u32 seat,const Vec3&,const Vec3&,i32&,i32& bomb_hit) override {
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

    // A Last Spell suppresses bomb damage when its flag disallows bombs,
    // while the ordinary source still contributes before boss balancing.
    pilots.count=2;pilots.hits[0]=20;pilots.hits[1]=30;pilots.bombs[1]=1;
    boss.life=100;shared.score=0;
    assert(damage_enemy_multiplayer(boss,1,0,values,bomb_hit,owner,pilots));
    assert(boss.life==99&&boss.last_damage==1);
    assert(shared.score==10&&bomb_hit==1&&owner==1);
}
