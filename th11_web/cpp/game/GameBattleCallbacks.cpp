#include "GameBattle.hpp"
#include <algorithm>
namespace th11 {
bool GameBattle::spell_result(bool captured,i32 bonus){
    events.push_back({BattleEventKind::SpellResult,captured?0:1,bonus});return hud.notice(captured?0:1,bonus);
}
bool GameBattle::callback_damage(Vec3 p,Vec2 size,i32& out){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled)return mp_damage_position(p,size,out);
#endif
    if(!player)return false;const auto& timer=player->state.state_timer;return player->shots.damage(p,size,timer.previous!=timer.current,economy,out);
}
bool GameBattle::add_score(i32 value){
    economy.add_score(value);return true;
}
bool GameBattle::rank_delta(i32 value){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled){mp_rank=std::clamp(mp_rank+(value<0?value/i32(mp_options.seat_count):value),-1024,1024);for(unsigned i=0;i<mp_options.seat_count;++i)pilots[i]->economy.rank=mp_rank;return true;}
#endif
    economy.add_rank(value);return true;
}
bool GameBattle::collect(ItemState& item,bool& convert){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled)return mp_collect(item,convert);
#endif
    return item_rewards.collect(item,convert);
}
bool GameBattle::drop_items(EnemyState& enemy){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled){
        // Only eligible stage drops multiply. Shared life fragments retain
        // the original quantity for every roster; ordinary Power stays 2P x1,
        // 3P x2. Player death/F/transfer emissions keep their own boundaries.
        EnemyDrop drops=enemy.drops;
        if(mp_options.seat_count==3){for(unsigned i:{0u,3u,6u,9u,10u})drops.counts[i]*=2;
            const int type=drops.primary;if(type==1||type==4||type==7||type==10||type==11){if(!items.drop(drops,enemy.current.position))return false;if(items.spawn(type,{enemy.current.position.x+8,enemy.current.position.y-8,enemy.current.position.z})<0)return false;enemy.drops={};return true;}}
        const bool ok=items.drop(drops,enemy.current.position);enemy.drops={};return ok;
    }
#endif
    items.player.power=economy.power;items.player.max_power=economy.max_power;return items.drop(enemy.drops,enemy.current.position);
}
void GameBattle::recall_player_options(){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled){for(unsigned i=0;i<mp_options.seat_count;++i)if(pilots[i]->player)pilots[i]->player->motion.recall_options(true);return;}
#endif
    if(player)player->motion.recall_options(true);
}
bool GameBattle::dialogue_stage_complete(){
    return completion.complete();
}
}
