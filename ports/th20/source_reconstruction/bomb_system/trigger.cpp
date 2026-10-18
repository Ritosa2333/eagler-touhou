#include "bomb.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::bomb {
namespace ps=gameplay::player_state;
void consume_bomb(game_session::Player& player){
    const auto count=recovered::signed_bits(ps::read<std::uint32_t>(player,0xcc)-1u);
    ps::set_bombs(player,count,gameplay::unrecovered::bomb_observer());
}
int Controller::trigger(){
    retire_bomb(active_bomb);active_bomb=nullptr;
    using Factory=Bomb*(*)();constexpr Factory factories[]{&unrecovered::create_00479140,&unrecovered::create_004783a0};
    active_bomb=factories[ps::read<std::uint32_t>(*context->current_player,8)]();
    active_bomb->start(view_index);active_state=1;recovered::timer_set(timer,0);
    consume_bomb(*context->current_player);add_player_meter(*game_session::context(0).current_player,-200);return 0;
}
}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_00478300(int index){return bomb::create_controller(index);}
}
