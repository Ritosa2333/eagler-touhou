#include "movement.hpp"
#include "shot_controller_frame.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/enemy.hpp"
#include "../effect_system/effect.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#ifdef TH_SDL3
// Direct-touch is a web-port capability; the native reconstruction keeps the
// recovered eight-way path and links no touch owner.
#include "../../../th20_web/cpp/sdl/TouchMotion.hpp"
#endif
namespace th20::source::player_entity {
namespace {
struct GameMovement final:MovementServices {
    OptionFrameServices& options() override{return option_frame_services();}
    StageResetServices& reset() override{return stage_reset_services();}
    int input_slot(int view) override{return shot_controller_services().input_slot(view);}
    std::uint32_t held(int slot,std::uint32_t mask) override{return shot_controller_services().held(slot,mask);}
    bool enemy_ready(game_session::Context& context) override{auto* enemy=static_cast<gameplay::EnemyController*>(context.objects_04[1]);return enemy&&enemy->field_124;}
    float clock_scale() override{return state::clock_scale;}
    void bind_script(Player& player,int script) override{sprite::bind_animation_script(*player.animation_file,player.animation,script,nullptr);}
    sprite::Animation* animation(std::uint32_t& handle) override{return sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);}
    std::uint32_t spawn_focus_effect(Player& player) override{auto& file=*static_cast<effects::Controller*>(player.context->objects_04[7])->files[0];std::uint32_t handle;sprite::spawn_named_animation(*program_entry::sprite_controller,file,handle,"effect",19,nullptr,0,-1,0);return handle;}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
    bool analog(int slot,float& x,float& y) override{
#ifdef TH_SDL3
        (void)slot;return input::analog_motion(x,y);
#else
        (void)slot;(void)x;(void)y;return false;
#endif
    }
};
}
MovementServices& movement_services(){static GameMovement result;return result;}
int update_movement(Player& player){return update_movement(player,movement_services());}
}
