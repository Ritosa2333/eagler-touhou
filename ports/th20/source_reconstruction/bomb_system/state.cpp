#include "bomb.hpp"
#include "../gameplay/enemy.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::bomb {
namespace ps=gameplay::player_state;
void add_enemy_bomb_counter(gameplay::EnemyController& enemy,std::int32_t amount) noexcept {
    // EnemyData+3c == EnemyController+4c. ADD uses modulo32-bit arithmetic.
    enemy.data.fields_30[3]+=static_cast<std::uint32_t>(amount);
}
void set_enemy_bomb_flag(void* entity,std::uint32_t value) noexcept {
    auto* destination=static_cast<std::uint8_t*>(entity)+0x358;std::uint32_t flags;std::memcpy(&flags,destination,4);
    flags=(flags&~32u)|((value&1u)<<5);std::memcpy(destination,&flags,4);
}
void mark_enemies_for_bomb(gameplay::EnemyController& enemy){
    scheduler::Iterator iterator(enemy.enemies.sentinel.next);
    while(iterator.current){set_enemy_bomb_flag(iterator.current->value,1);iterator.advance();}
}
void add_player_meter(game_session::Player& player,std::int32_t amount) noexcept {
    const auto bits=ps::read<std::uint32_t>(player,0x5c)+static_cast<std::uint32_t>(amount);
    ps::write(player,0x5c,std::clamp(recovered::signed_bits(bits),0,10000));
}
std::int32_t bomb_count(game_session::Player& player) noexcept {
    const auto count=std::clamp(ps::read<std::int32_t>(player,0xcc),0,10);ps::write(player,0xcc,count);return count;
}
void notify_bomb_start(void* secondary_owner){
    auto* bytes=static_cast<std::uint8_t*>(secondary_owner);std::uint32_t flags;std::memcpy(&flags,bytes+0x78,4);
    if(!(flags&1u))return;
    std::int32_t elapsed;std::memcpy(&elapsed,bytes+0x28,4); // Timer at+24, current at+28
    if(elapsed>=60){const std::uint32_t zero=0;std::memcpy(bytes+0x7c,&zero,4);flags&=~34u;std::memcpy(bytes+0x78,&flags,4);}
    else{game_session::Context* context;std::memcpy(&context,bytes+0xc0,4);
        if(static_cast<Controller*>(context->objects_04[5])->active()){flags|=32u;std::memcpy(bytes+0x78,&flags,4);}}
}
}
