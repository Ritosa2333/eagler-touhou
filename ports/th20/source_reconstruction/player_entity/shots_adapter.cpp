#include "shots.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../damage_regions/regions.hpp"
namespace th20::source::player_entity {
namespace {
class GameShotServices final:public ShotServices {
public:
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    void retire_damage(std::uint32_t& handle) override{if(auto* region=damage::find_handle(handle))damage::retire(*region);}
    void release(Shot* shot) override{runtime::release_bytes(shot);}
};
}
ShotServices& shot_services(){static GameShotServices services;return services;}
}
