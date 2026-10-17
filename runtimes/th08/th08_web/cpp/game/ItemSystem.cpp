#include "ItemSystem.hpp"
namespace th08 {
ItemSystem::ItemSystem(PlayerSimulation& p,GameGlobals& g,GameValues& v,GameGauge& gauge,GameRank& r,HighScore& h,Rng& random,AnmLibrary& a,AnmExecutor& e,AnmRenderer& graphics,ItemSystemActions& services)
    :player(p),globals(g),animations(a),executor(e),renderer(graphics),actions(services),rank(r),pool(*state,random,*this),rewards(reward_input,g,v,gauge,r,h,pool,*this),updater(*state,pool,input,p.profile(false),p.profile(true),*this){state->reset();pool.snapshot();}
void ItemSystem::synchronize(){
    auto& c=player.status().context;c.power=Scalar::truncate(globals.power);c.lives=Scalar::truncate(globals.lives);c.bombs=Scalar::truncate(globals.bombs);c.time_orbs=globals.time_orbs;c.last_spell_requirement=globals.last_spell_requirement;c.gauge=globals.gauge;
    input.power=c.power;
}
ItemState* ItemSystem::spawn(const Vec3& position,i32 type,i32 mode){executor.timing=player.timing;return pool.spawn(position,type,mode,Scalar::truncate(globals.power),player.status().life.state);}
void ItemSystem::collect(ItemState& item){
    read_hud();
    auto& p=player.status();p.context.replay_flags=input.replay_flags;reward_input.hud_flags=p.context.hud_flags;reward_input.power_flag=p.context.miss_control;reward_input.gauge_lock=input.gauge_lock;
    rewards.collect(item);failed|=rewards.failed;input.replay_flags=p.context.replay_flags;p.context.hud_flags=reward_input.hud_flags;p.context.miss_control=reward_input.power_flag;write_hud();synchronize();
}
bool ItemSystem::update(){
    if(failed)return false;pool.snapshot();auto& p=player.status();synchronize();read_hud();executor.timing=updater.timing=player.timing;
    input.player=p.motion.movement.position;input.height=p.context.extent.y;input.focused=p.motion.form.focused;input.character=p.context.character;input.player_state=p.life.state;
    input.shooting=p.shots.shooting_timer;input.gauge_lock=p.item_gauge_lock;input.replay_flags=p.context.replay_flags;
    reward_input.collect_line=player.profile(false).item_collect_line;reward_input.difficulty=difficulty;reward_input.bomb_triggered=p.bomb.triggered;reward_input.bomb_active=p.bomb.active;reward_input.focused=p.motion.form.focused;reward_input.time_spell=p.context.time_spell;
    updater.update();p.item_gauge_lock=input.gauge_lock;p.context.replay_flags=input.replay_flags;synchronize();return !failed;
}
bool ItemSystem::draw(const Vec2& offset){if(failed)return false;pool.draw(offset);return !failed;}
void ItemSystem::time_orb(){
    read_hud();
    auto& p=player.status();reward_input.hud_flags=p.context.hud_flags;reward_input.bomb_triggered=p.bomb.triggered;reward_input.bomb_active=p.bomb.active;reward_input.focused=p.motion.form.focused;reward_input.gauge_lock=p.item_gauge_lock;
    rewards.time_orb(nullptr);failed|=rewards.failed;p.context.hud_flags=reward_input.hud_flags;write_hud();synchronize();
}
}
