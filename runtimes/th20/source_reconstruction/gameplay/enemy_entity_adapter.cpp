#include "enemy_entity.hpp"
#include "enemy_variables.hpp"
#include "enemy_reads.hpp"
#include "loading_dependencies.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
namespace {
class GameEnemyLifecycleServices final:public EnemyLifecycleServices {
public:
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    void destroy_mesh(sprite::RenderMesh* mesh) override{sprite::destroy_render_mesh(mesh);}
};
sprite::Animation* read_animation(std::uint32_t& handle){return sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);}
EnemyReadEnvironment read_environment(){return {game_session::session,state::random_streams[0],controller->restart_mode,program_entry::graphics_state.field_0b18,replay_selection,read_animation};}
}
EnemyLifecycleServices& enemy_lifecycle_services(){static GameEnemyLifecycleServices services;return services;}
namespace unrecovered {
void destroy_enemy_entity_004a2720(void* entity){retire_enemy(static_cast<Enemy*>(entity));}
void clear_enemy_async_004973c0(void* entity){static_cast<Enemy*>(entity)->clear_async();}
void reset_enemy_script_004972c0(void* entity){static_cast<Enemy*>(entity)->reset();}
void select_enemy_script_00540200(void* entity,const char* name){static_cast<Enemy*>(entity)->select_script(name);}
int tick_enemy_scripts_0053e2b0(void* entity,float delta){auto random=state::stream(state::random_streams[0]);EnemyVmEnvironment environment{&random,state::timer_rate};return static_cast<Enemy*>(entity)->tick(delta,environment);}
std::int32_t read_enemy_integer_0049abc0(Enemy& entity,std::int32_t variable){auto environment=read_environment();return read_enemy_integer(entity,variable,environment);}
float read_enemy_float_004995d0(Enemy& entity,std::int32_t variable){auto environment=read_environment();return read_enemy_float(entity,variable,environment);}
}
}
