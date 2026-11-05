#include "owner.hpp"
#include "shots.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::player_entity {
namespace pe=program_entry;
namespace {
class GamePlayerServices final:public PlayerServices {
public:
    scheduler::State& scheduler() override{return *pe::function_controller;}
    scheduler::Environment& scheduler_environment() override{return pe::scheduler_environment;}
    bool preserve_animation_files() override{return (game_session::session.flags&1u)!=0;}
    void unload_animation_file(int slot,bool preserve) override{
        if(preserve)sprite::mark_file_animations(*pe::sprite_controller,pe::sprite_controller->files[slot],false);
        else sprite::unload_animation_file(*pe::sprite_controller,slot);
    }
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    void destroy_animation(sprite::Animation& animation) override{sprite::destroy_animation_contents(animation);}
    void retire_shot(Shot& shot) override{player_entity::retire_shot(shot);}
};
}
PlayerServices& player_services(){static GamePlayerServices services;return services;}
}
