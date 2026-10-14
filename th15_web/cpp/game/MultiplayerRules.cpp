#if TH15_MULTIPLAYER
#include "GameBattle.hpp"
#include "MultiplayerPilot.hpp"
#include "MultiplayerEconomy.hpp"
#include <algorithm>
namespace th15 {
namespace {
double distance2(const Vec3& a,const Vec3& b){const double x=double(a.x)-b.x,y=double(a.y)-b.y;return x*x+y*y;}
}
// TH11 MP: one shared fragment event, including ghosts banking extra lives.
// TH15 retains its own three-piece Normal / native Extra threshold and cap.
bool GameBattle::mp_reward_fragment(){
    std::array<PlayerLifeSession*,3> stocks{};for(unsigned s=0;s<mp_options.seat_count;++s)stocks[s]=&pilots[s]->session;
    const auto added=multiplayer_fragment(mp_fragments,score.life_piece_tier,score.difficulty,stocks,mp_options.seat_count);
    for(unsigned s=0;s<mp_options.seat_count;++s){auto& p=*pilots[s];
        p.session.life_pieces=mp_fragments;p.owned_life_piece_tier=score.life_piece_tier;
        if(added[s]&&(!p.sound(17,true)||!p.notice(4)))return false;
        if(!p.life_hud(p.session.extra_lives,p.session.life_pieces))return false;
    }
    return true;
}
bool MultiplayerPilot::revive(bool rescued){
    const auto position=player->motion.position;
    if(bomb&&!bomb->stop())return world.fail(bomb->error);
    session.extra_lives=std::max(session.extra_lives,0);session.power=score.max_power/2;session.bombs=player->life.base_bombs;
    if(!player->visuals.pose(0)||!player->reset_for_stage())return world.fail(player->error);
    player->motion.position=position;player->motion.position_fixed={truncate_int(position.x*128),truncate_int(position.y*128)};player->motion.snap_options();
    player->life.age.set(60);player->life.invulnerability.set(rescued?280:120);
    ghost=false;ghost_clock=0;ghost_dx=ghost_dy=0;life_hold=power_taps=power_gap=0;life_latched=false;keys={};input={};spell_startup_bomb=false;
    return life_hud(session.extra_lives,session.life_pieces)&&bomb_hud(session.bombs,session.bomb_pieces);
}
bool GameBattle::mp_spawn_transfer(i32 kind,const Vec3& position,unsigned target){
    const auto id=items->spawn(kind,position,0,0);if(!id)return false;
    mp_gift_targets[id-1]=int(target);return true;
}
int GameBattle::mp_life_receiver(unsigned giver)const{
    if(giver>=mp_options.seat_count)return -1;const auto& p=*pilots[giver];int receiver=-1;
    if(!p.alive()||(p.keys.held&9)!=8||p.life_latched||p.session.extra_lives<=0)return -1;
    for(unsigned j=0;j<mp_options.seat_count;++j){if(giver==j)continue;const auto& q=*pilots[j];
        if((!q.alive()&&!q.ghost)||(!q.ghost&&q.session.extra_lives>=8)||distance2(p.player->motion.position,q.player->motion.position)>400)continue;
        if(receiver<0||q.ghost>pilots[unsigned(receiver)]->ghost||(q.ghost==pilots[unsigned(receiver)]->ghost&&q.session.extra_lives<pilots[unsigned(receiver)]->session.extra_lives))receiver=int(j);
    }return receiver;
}
// Same TH11 confirmed-frame actions: Shift without Z for 90 ticks within
// 20px gives a life/rescues; five Z edges with <=24-tick gaps give one Power.
bool GameBattle::mp_update_rules(){
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];const auto held=p.keys.held;
        if(!(held&8)){p.life_hold=0;p.life_latched=false;}
        if(p.power_gap<25)++p.power_gap;if(p.power_gap>24)p.power_taps=0;
        if(!p.alive()||(input.game_flags&(4|0x70|0x4000|0x10000))){p.life_hold=0;continue;}
        int receiver=mp_life_receiver(i);
        if(receiver>=0){if(++p.life_hold>=90){auto& q=*pilots[unsigned(receiver)];bool transferred=false;
            if(q.ghost){--p.session.extra_lives;transferred=q.revive(true);if(!transferred)return false;if(!p.sound(17,true))return false;}
            else if(mp_spawn_transfer(5,p.player->motion.position,unsigned(receiver))){--p.session.extra_lives;transferred=true;}
            if(transferred){p.life_latched=true;p.life_hold=0;if(!p.life_hud(p.session.extra_lives,p.session.life_pieces))return false;}
        }}else p.life_hold=0;
        if(p.keys.pressed&1){if(p.power_gap>24)p.power_taps=0;p.power_gap=0;
            if(++p.power_taps>=5){p.power_taps=0;receiver=-1;
                if(multiplayer_can_give_power(p.session.power,p.session.power_step))for(unsigned j=0;j<mp_options.seat_count;++j){if(i==j)continue;const auto& q=*pilots[j];
                    if(!q.alive()||q.session.power>=score.max_power||distance2(p.player->motion.position,q.player->motion.position)>400)continue;
                    if(receiver<0||i64(q.session.power)*pilots[unsigned(receiver)]->session.power_step<i64(pilots[unsigned(receiver)]->session.power)*q.session.power_step)receiver=int(j);
                }
                if(receiver>=0&&mp_spawn_transfer(3,p.player->motion.position,unsigned(receiver))){p.session.power-=p.session.power_step;if(!p.options_changed())return false;}
            }
        }
    }return true;
}
}
#endif
