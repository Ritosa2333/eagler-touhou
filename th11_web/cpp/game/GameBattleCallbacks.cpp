#include "GameBattle.hpp"
#include <algorithm>
namespace th11 {
bool GameBattle::spell_result(bool captured,i32 bonus){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled&&captured)for(unsigned i=1;i<mp_options.seat_count;++i)pilots[i]->economy.add_score(bonus);
#endif
    events.push_back({BattleEventKind::SpellResult,captured?0:1,bonus});return hud.notice(captured?0:1,bonus);
}
bool GameBattle::callback_damage(Vec3 p,Vec2 size,i32& out){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled)return mp_damage_position(p,size,out);
#endif
    if(!player)return false;const auto& timer=player->state.state_timer;return player->shots.damage(p,size,timer.previous!=timer.current,economy,out);
}
bool GameBattle::add_score(i32 value){
#ifdef TH11_MULTIPLAYER
    if(mp_enabled){for(unsigned i=0;i<mp_options.seat_count;++i)pilots[i]->economy.add_score(value);return true;}
#endif
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
        // Only stage drops multiply. Player death/F/transfer emissions retain
        // their own rules and never pass through this boundary.
        EnemyDrop drops=enemy.drops;
        if(mp_options.seat_count==3){for(unsigned i:{0u,3u,4u,6u,9u,10u})drops.counts[i]*=2;
            const int type=drops.primary;if(type==1||type==4||type==5||type==7||type==10||type==11){if(!items.drop(drops,enemy.current.position))return false;if(items.spawn(type,{enemy.current.position.x+8,enemy.current.position.y-8,enemy.current.position.z})<0)return false;enemy.drops={};return true;}}
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
#ifdef TH11_MULTIPLAYER
    if(mp_enabled){
        const auto multiply=[](i32 a,i32 b){return signed_bits(u32(a)*u32(b));};
        for(unsigned i=1;i<mp_options.seat_count;++i){auto& e=pilots[i]->economy;const i32 stage=i32(resources.stage_number);e.add_score(multiply(wrapping_add(e.lives,stage),1000000));if(stage!=6&&stage!=7)continue;const i32 points=e.point_value/100;e.add_score(multiply(points-points%10,1000));
            if(stage==7){e.add_score(multiply(e.lives,40000000));e.add_score(multiply(e.power,400000));continue;}
            i32 bonus=0;switch(e.difficulty){case 0:bonus=multiply(wrapping_add(multiply(e.lives,200),e.power),100000);break;case 1:bonus=wrapping_add(multiply(e.lives,25000000),multiply(e.power,150000));break;case 2:bonus=multiply(wrapping_add(multiply(e.lives,175),e.power),200000);break;case 3:bonus=wrapping_add(multiply(e.lives,40000000),multiply(e.power,300000));break;case 4:bonus=multiply(wrapping_add(multiply(e.lives,100),e.power),400000);break;}e.add_score(bonus);
        }
    }
#endif
    return completion.complete();
}
}
