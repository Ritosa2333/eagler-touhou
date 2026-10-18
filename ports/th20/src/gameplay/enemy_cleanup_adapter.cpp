#include "enemy_cleanup.hpp"
#include "enemy_damage.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
namespace {
class Source final:public EnemyCleanupServices {
    void defeat(Enemy& enemy) override{unrecovered::defeat_enemy_004a5640(&enemy);}
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    const float* timer_rate() override{return state::timer_rate;}
} source;
}
EnemyCleanupServices& enemy_cleanup_services(){return source;}
}
