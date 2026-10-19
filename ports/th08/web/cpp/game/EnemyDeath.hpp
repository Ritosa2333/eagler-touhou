#pragma once
#include "EnemyFamiliars.hpp"
namespace th08 {
struct EnemyDeathActions {
    virtual ~EnemyDeathActions()=default;
    virtual i32 cancel_projectiles(i32 maximum,bool reward)=0;
    virtual i32 cancel_enemies(i32 maximum,i32 score)=0;
    virtual void popup_bonus(i32 score)=0;
};
// Death phase of 0042c660 (0042d551..0042db0b). Death mode is separate
// from retiring an ECL object: several modes keep its script alive.
class EnemyDeath {
    EclExecutor& executor;EnemyFamiliars& familiars;EnemyFamiliarContext& input;
    EnemyDropSequence& drops;Rng& random;GameGauge& gauge;GameValues& values;
    EnemyFamiliarActions& visuals;EnemyDeathActions& actions;u16& replay;
public:
    EnemyDeath(EclExecutor& e,EnemyFamiliars& f,EnemyFamiliarContext& i,EnemyDropSequence& d,Rng& r,GameGauge& g,GameValues& v,EnemyFamiliarActions& p,EnemyDeathActions& a,u16& replay_flags)
      :executor(e),familiars(f),input(i),drops(d),random(r),gauge(g),values(v),visuals(p),actions(a),replay(replay_flags){}
    bool run(EclVm&,i32 slot,bool focused,i32 bomb_hit);
};
}
