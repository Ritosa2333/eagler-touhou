#include "environment.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
namespace th20::source::special_state {
namespace n=recovered;namespace ps=gameplay::player_state;
namespace {
int clamp(game_session::Player& p,unsigned offset,int lo,int hi){const auto result=std::clamp(ps::read<int>(p,offset),lo,hi);ps::write(p,offset,result);return result;}
void set(game_session::Player& p,unsigned offset,int value,int hi){ps::write(p,offset,std::clamp(value,0,hi));}
void add(game_session::Player& p,unsigned offset,int value,int hi){set(p,offset,n::signed_bits(ps::read<unsigned>(p,offset)+static_cast<unsigned>(value)),hi);}
int meter(game_session::Player& p){return clamp(p,0x5c,0,10000);}
int maximum(game_session::Player& p){return clamp(p,0x60,0,5000);}
}
int update_entry(Entry& p,Environment& host){
    if(!host.enemy_exists(p.enemy_handle))return 1;
    if(p.age.current>60){
        effects::Parameters effect;effects::construct_parameters(effect);
        constexpr unsigned colors[]{0xffff4040,0xff4040ff,0xfff0f040,0xff40f040};
        unsigned color=colors[p.color];auto* bytes=reinterpret_cast<unsigned char*>(&color);
        bytes[2]=std::uint8_t(bytes[2]-state::next(state::random_streams[0])%64);
        bytes[1]=std::uint8_t(bytes[1]-state::next(state::random_streams[0])%64);
        bytes[0]=std::uint8_t(bytes[0]-state::next(state::random_streams[0])%64);
        effect.value_20=color;effect.vector_00=host.enemy_position(p.enemy_handle);host.effect(10,effect);
    }
    n::timer_tick(p.age,state::timer_rate);return 0;
}
void update(Controller& p,Environment& host){
    auto& player=host.player();
    if(host.session_mode()==2)set(player,0x5c,0,10000);
    if(host.enemies_present()&&!p.active&&!host.boss_collecting()){
        if(p.charge_age.current%3<2)add(player,0x5c,1,10000);
        n::timer_tick(p.charge_age,state::timer_rate);
    }
    if(!p.active){
        const int current=meter(player);
        if(maximum(player)<=current&&!host.boss_collecting()&&!host.special_blocked()){
            gameplay::SpawnParameters spawn;gameplay::construct_spawn_parameters(spawn);
            int selected=0,best=-1,level=0;
            const int forced=clamp(player,0x98,-1,4);
            if(forced<0){for(int i=0;i<4;++i)if(clamp(player,0x64+unsigned(i)*4,0,1000)>best){best=clamp(player,0x64+unsigned(i)*4,0,1000);level=clamp(player,0x74+unsigned(i)*4,0,4);selected=i;}}
            else{selected=clamp(player,0x98,-1,4);level=clamp(player,0x74+unsigned(selected>=1&&selected<=3?selected:0)*4,0,4);ps::write(player,0x98,-1);}
            const float random=state::signed_unit(state::random_streams[0]);spawn.position={n::mul32(random,144.f),120.f,0};
            constexpr int health[]{3200,3400,3700,3900,4000,3200,3400,3700,3900,4000,3200,3400,3700,3900,4000,3200,3400,3700,3900,4000};
            constexpr const char* names[]{"StoneR","StoneB","StoneY","StoneG"};
            spawn.health=health[selected*5+level];p.active=1;host.spawn_enemy(names[selected],spawn);
            if(selected>=0&&selected<4)set(player,0x64+unsigned(selected)*4,0,1000);
            add(player,0x5c,-(maximum(player)/3),10000);
            n::timer_set(p.age,0);start(p.text_expansion,60,4,1.f,0.f);start(p.text_alpha,120,4,255,0);start(p.meter_remaining,1320,5,1.f,0.f);
        }
    }
    int count=0;
    for(scheduler::Iterator it(p.entries.sentinel.next);it.current;it.advance()){
        ++count;auto* value=reinterpret_cast<Entry*>(it.current->value);
        if(update_entry(*value,host)!=0){retire(*reinterpret_cast<Entry*>(it.current->value),host);p.active=0;}
    }
    if(p.active){
        if(p.age.current>10&&count==0)p.active=0;
        const float limit=n::int_float(maximum(player)),factor=evaluate(p.meter_remaining);set(player,0x5c,n::truncate32(n::mul32(limit,factor)),10000);
        n::timer_tick(p.age,state::timer_rate);sample(p.text_expansion,state::timer_rate);sample(p.text_alpha,state::timer_rate);sample(p.meter_remaining,state::timer_rate);
        if(!(p.meter_remaining.timer.current<p.meter_remaining.duration)||p.age.current>=1800){
            p.active=0;n::timer_set(p.age,0);
            for(scheduler::Iterator it(p.entries.sentinel.next);it.current;it.advance())retire(*reinterpret_cast<Entry*>(it.current->value),host);
        }
    }
}
int collect(Entry& p,void* enemy,Environment& host){
    host.clear_bullets(host.position_of(enemy));host.clear_lasers(host.position_of(enemy));auto& player=host.player();
    if(p.color>=0&&p.color<4)add(player,0x74+unsigned(p.color)*4,1,4);
    const int current=meter(player);if(maximum(player)/2<=current)set(player,0x5c,maximum(player)/2,10000);
    add(player,0x60,150,5000);return 0;
}
}
