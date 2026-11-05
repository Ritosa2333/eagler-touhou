#include "death_state.hpp"
#include "shot_hit.hpp"
#include "../bomb_system/bomb.hpp"
#include "../gameplay/enemy.hpp"
#include "../item_system/rewards.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::player_entity {
namespace {
struct GameDeath final:DeathServices {
    MovementServices& movement() override{return movement_services();}
    void mark_enemies() override{bomb::mark_enemies_for_bomb(gameplay::enemy_controller());}
    void spawn_effect(Player& player) override{
        spawn_tracked_effect(*static_cast<effects::Controller*>(player.context->objects_04[7]),0,21,player.position_614,0,shot_hit_services());
    }
    void add_lives(game_session::Player& player,int amount) override{item::add_lives(player,amount,item::reward_environment());}
    void add_meter(game_session::Player& player,int amount) override{bomb::add_player_meter(player,amount);}
    void notify_card(game_session::Context& context) override{bomb::notify_bomb_start(context.objects_04[3]);}
};
}
DeathServices& death_services(){static GameDeath result;return result;}
void begin_death(Player& player){begin_death(player,death_services());}
}
