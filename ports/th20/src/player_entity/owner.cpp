#include "owner.hpp"
#include <cstring>
namespace th20::source::player_entity {
void* retained_shot_data=nullptr;
void construct_player_fields(Player& player) noexcept{
    //Contiguous constructor calls in4f46a0 write zero to every byte in these
    //scalar/vector/option intervals. The original byte/ANM gaps stay untouched.
    auto* bytes=reinterpret_cast<std::uint8_t*>(&player);
    std::memset(bytes+0x10,0,0x18);sprite::construct_animation(player.animation);
    std::memset(bytes+0x60c,0,0x204d-0x60c);
    std::memset(bytes+0x2050,0,0x2244-0x2050);
    construct_feedback(player.feedback);for(auto& script:player.animation_scripts)script=0;
    construct_shot_controller(player.shots);
    player.handle_1484c=0;player.view_index=0;player.context=nullptr;
}
Player::Player(PlayerServices& host):services(&host){construct_player_fields(*this);}
void clear_shots(ShotController& owner,PlayerServices& host){
    for(scheduler::Iterator iterator(owner.active.sentinel.next);iterator.current;iterator.advance())host.retire_shot(*reinterpret_cast<Shot*>(iterator.current->value));
    recovered::timer_set(owner.timer_12400,-1);recovered::timer_set(owner.timer_12410,-1);recovered::timer_set(owner.timer_12420,0);
    for(auto& count:owner.counters_12468)count=0;
}
Player::~Player(){
    //The diagnostic call40c6b0 in both original lifecycle bodies is a release
    //build no-op. It does not append to the game's log.
    scheduler::remove(services->scheduler(),services->scheduler_environment(),update_node);
    scheduler::remove(services->scheduler(),services->scheduler_environment(),draw_node);
    clear_shots(shots,*services);
    const bool preserve=services->preserve_animation_files();
    if((entity_flags&0x300u)==0){
        services->unload_animation_file(view_index+9,preserve);services->unload_animation_file(view_index+10,preserve);
        if(shot_data){runtime::release_bytes(shot_data);shot_data=nullptr;}retained_shot_data=nullptr;
    }
    services->delete_animation(feedback.handle_4c);services->destroy_animation(animation);
}
}
