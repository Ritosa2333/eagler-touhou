#include "death_state.hpp"
#include "../gameplay/enemy.hpp"
#include "../effect_system/effect.hpp"
#include <algorithm>
#include <bit>
namespace th20::source::player_entity {
namespace {
int& field(game_session::Player& p,unsigned offset){return *reinterpret_cast<int*>(reinterpret_cast<std::uint8_t*>(&p)+offset);}
int added(int a,int b){return std::bit_cast<int>(std::uint32_t(a)+std::uint32_t(b));}
}
bool subtract_power(game_session::Player& player,int amount){
    auto& power=field(player,0x30);const int unit=field(player,0x38);if(power<=unit)return false;
    power=std::bit_cast<int>(std::uint32_t(power)-std::uint32_t(amount));if(power<unit)power=unit;
    return added(power,amount)/unit!=power/unit;
}
void add_death_count(game_session::Player& player,int amount) noexcept{auto& value=field(player,0xc8);value=std::clamp(added(value,amount),0,999);}
void add_stone_level(game_session::Player& player,unsigned slot,int amount) noexcept{auto& value=field(player,0x74+slot*4);value=std::clamp(added(value,amount),0,4);}
void add_total_deaths(game_session::PlayerTable& table,int amount) noexcept{auto& value=table.fields_1ec[7];value=std::uint32_t(std::clamp(added(std::bit_cast<int>(value),amount),0,99999));}
void cancel_death(Player& player) noexcept{recovered::timer_set(player.timers_644[0],60);player.state=1;}
std::uint32_t spawn_tracked_effect(effects::Controller& owner,int file,int script,const sprite::Vec3& position,float angle,ShotHitServices& services){
    const auto index=reserve_effect_slot(owner,services.callbacks());if(index==0xffffffffu||recovered::signed_bits(index)>1023)return 0;
    owner.handles[recovered::signed_bits(index)]=services.spawn(*owner.files[file],nullptr,script,position,angle);return index|0x80000000u;
}
void begin_death(Player& player,DeathServices& env){
    auto& movement=env.movement();auto& callbacks=movement.options().callbacks();auto& session=callbacks.firing().session();
    if(session.mode!=2)(void)callbacks.dialogue_active(); //Original discarded dialogue getter precedes effects.
    env.mark_enemies();env.spawn_effect(player);env.add_lives(*player.context->current_player,-1);add_death_count(*player.context->current_player,1);
    auto& global=*session.contexts[0].current_player;for(unsigned slot=0;slot<4;++slot)add_stone_level(global,slot,-3);env.add_meter(global,-100);
    player.state=2;recovered::timer_set(player.timers_644[0],0);recovered::timer_set(player.timers_2050[0],180);movement.bind_script(player,0);
    for(auto& option:player.options){option.state=0;callbacks.interrupt(option.handle_dc,1);callbacks.interrupt(option.handle_e0,1);}player.fields_674[3]=0;
    for(auto& option:player.secondary_options){option.state=0;callbacks.interrupt(option.handle_dc,1);callbacks.interrupt(option.handle_e0,1);}
    env.notify_card(*player.context);auto& enemy=*static_cast<gameplay::EnemyController*>(player.context->objects_04[1]);enemy.data.fields_30[2]+=1u;enemy.data.fields_30[4]=0;
    add_total_deaths(session.player_table,1);
}
}
