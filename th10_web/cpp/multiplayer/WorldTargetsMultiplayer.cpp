#include "../platform/World.hpp"
#include <limits>
namespace th10::browser {
const Vec3& World::target_player(const Vec3& origin) const {
    static const Vec3 fallback{0,400,0};
    const Vec3* target=&fallback;double best=std::numeric_limits<double>::infinity();
    for(u32 seat=0;seat<player_count;++seat){
        const auto& pilot=pilots[seat];
        if(!pilot.player||cooperation.seats[seat].lifeState!=multiplayer::LifeState::Alive)continue;
        const auto& p=pilot.player->position;
        const double dx=double(p.x)-origin.x,dy=double(p.y)-origin.y,distance=dx*dx+dy*dy;
        if(distance<best){best=distance;target=&p;}
    }
    return *target;
}
void World::award_team_clear_bonus(){
    constexpr i32 lives[]{20000000,25000000,35000000,40000000,40000000},power[]{100000,100000,200000,300000,400000};
    const auto difficulty=u32(state.game.difficulty);if(difficulty>4)return;
    const i32 life_value=state.game.stage==7?40000000:lives[difficulty];
    const i32 power_value=state.game.stage==7?400000:power[difficulty];
    for(u32 seat=0;seat<player_count;++seat){
        const auto& g=pilots[seat].game;
        // Shared faith is awarded once by complete_stage. Each
        // pilot contributes its own resources, with native per-component
        // wrapping, /10 and score saturation in deterministic seat order.
        state.game.add_score(i32(u32(g.lives<0?0:g.lives)*u32(life_value)));
        state.game.add_score(i32(u32(g.power<0?0:g.power)*u32(power_value)));
    }
}
void World::publish_player_targets(EnemyState& enemy){
    const auto absolute=[](Extended n){return n<number(0)?-n:n;};
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.player||cooperation.seats[seat].lifeState!=multiplayer::LifeState::Alive)continue;
        auto& player=*pilot.player;const auto x=number(player.position.x);
        // Keep the native enemy traversal/target_seen rule for each pilot.
        if(!player.target||absolute(number(player.target->state.current.position.x)-x)<absolute(number(enemy.current.position.x)-x)){
            if(!player.target_seen)player.target=reinterpret_cast<Enemy*>(enemy.script_owner);
            player.target_seen=1;
        }
    }
}
}
