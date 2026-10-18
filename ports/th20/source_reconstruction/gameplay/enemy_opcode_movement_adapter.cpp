#include "enemy_opcode_movement.hpp"
#include "enemy_variables.hpp"
#include "../runtime_state/state.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../player_entity/player.hpp"
namespace th20::source::gameplay {
namespace {
struct Host final:EnemyMovementOpcodeServices {
    float clock_scale() override{return th20::source::state::clock_scale;}
    float random_angle() override{return recovered::mul32(th20::source::state::signed_unit(th20::source::state::random_streams[0]),3.1415927f);}
    unsigned random_integer() override{auto& random=th20::source::state::random_streams[0];std::lock_guard<std::recursive_mutex> guard(runtime::shared_locks().slot(10));random.last=recovered::lcg_next(random.state);return random.last&0xffffu;} //497470 omits modulus division
    sprite::Vec3 player_position(game_session::Context& c) override{return player_entity::position(c.objects_04[0]);}
    Enemy* selected(EnemyController& c,unsigned index) override{return static_cast<Enemy*>(selected_enemy(&c,index));}
    Enemy* find(EnemyController& c,unsigned identifier) override{return static_cast<Enemy*>(find_enemy_in_list(c.enemies,identifier));}
};
}
EnemyMovementOpcodeServices& enemy_movement_opcode_services(){static Host host;return host;}
EnemyOpcodeResult execute_enemy_movement_opcode(EnemyOpcodeReader& r){return execute_enemy_movement_opcode(r,enemy_movement_opcode_services());}
}
