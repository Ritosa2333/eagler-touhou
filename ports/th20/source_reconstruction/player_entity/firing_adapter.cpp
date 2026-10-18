#include "firing.hpp"
#include "shots.hpp"
#include "../damage_regions/regions.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include <bit>
#include <cstring>
namespace th20::source::player_entity {
namespace {
class GameFiringServices final:public FiringServices {
public:
    game_session::Session& session() override{return game_session::session;}
    float clock_rate() override{return *state::timer_rate;}
    float signed_random() override{return state::signed_unit(state::random_streams[0]);}
    Shot* create_heap_shot() override{auto* result=static_cast<Shot*>(runtime::allocate_bytes(sizeof(Shot)));if(result){std::memset(result,0,sizeof(Shot));construct_shot(*result);}return result;}
    ShotCallbacks callbacks(const ShotRecord& row) override{return unrecovered::shot_callbacks(row);}
    std::uint32_t spawn_animation(sprite::AnimationFile& file,int script) override{return sprite::spawn_named_animation(*program_entry::sprite_controller,file,nullptr,script,-1);}
    sprite::Animation& animation(std::uint32_t& handle) override{return *sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);}
    std::uint32_t create_damage(game_session::Context& context,const Shot& shot) override{return damage::create_rectangle(*static_cast<damage::HitCtrlInf*>(context.object_28),shot.motion.position,shot.vector_b0.x,shot.vector_b0.y,shot.motion.angle_1c,9999999,std::bit_cast<std::int32_t>(shot.fields_98[5]));}
    damage::Region* damage(std::uint32_t& handle) override{return damage::find_handle(handle);}
    void retire(Shot& shot) override{retire_shot(shot);}
    void sound_at(int id,float x) override{program_entry::thread_registry.request_effect_at(id,x);}
};
}
FiringServices& firing_services(){static GameFiringServices result;return result;}
}
