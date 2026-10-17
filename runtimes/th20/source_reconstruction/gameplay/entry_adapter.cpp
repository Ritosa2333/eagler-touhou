#include "subsystems.hpp"
#include "loading_dependencies.hpp"
#include "enemy.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include "../game_session/session.hpp"
#include "../stage_background/background.hpp"
#include "../bomb_system/bomb.hpp"
#include "../effect_system/effect.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/laser.hpp"
#include "../item_system/item.hpp"
#include "../player_entity/initialize.hpp"
#include "../special_state/special.hpp"
#include "../hud_system/dialogue.hpp"
#include "../stone_menu/stone.hpp"
#include "../stage_clear/stage_clear.hpp"
#include "../overlay_system/overlay.hpp"
#include <new>
#include <stdexcept>

namespace pe=th20::source::program_entry;
namespace pw=th20::source::platform_window;
namespace th20::source::gameplay {
namespace {
class GameServices final:public Services {
public:
    std::uint32_t& input_latch() override {return pe::window_state.input_latch;}
    Device& device() override {
        if(!pe::graphics_state.device) throw std::logic_error("GameController requires initialized graphics device");
        return *pe::graphics_state.device;
    }
    runtime::Worker& worker() override {return *std::launder(reinterpret_cast<runtime::Worker*>(pe::graphics_state.worker_storage[0]));}
    std::uint32_t& session_flags() override {return game_session::flags();}
    std::int32_t session_mode() override {return game_session::mode();}
    std::int32_t scene() override {return pe::graphics_state.field_0b0c;}
    runtime::CallbackOwner* owner(Owner which) override {
        if(which==Owner::session_overlay) return game_session::overlay_owner();
        if(which==Owner::player_primary) return game_session::primary_owner();
        if(which==Owner::global_005c069c) return background::primary;
        if(which==Owner::global_005c06a0) return background::secondary;
        if(which==Owner::global_005c06a4) return hud::controller;
        if(which==Owner::global_005c6120) return stone_menu::controller;
        if(which==Owner::global_005c6114) return stage_clear::controller();
        return unrecovered::owner(which);
    }
    void preserve_background_as_secondary() override {background::secondary=background::primary;}
    void load(GameController& game) override {load_gameplay(game,loading_services());}
    void commit_progress_if_present() override {unrecovered::commit_progress_if_present();}
    void reset_clock_scale() override {state::set_clock_scale(1.0f);}
    void clear_surface_callbacks() override {pw::surface_callback_first=nullptr;pw::surface_callback_second=nullptr;}
    void screen_transition(float x,float y) override {unrecovered::screen_transition(x,y);}
    bool stage_selection_changed() override {
        const auto previous=player_state::previous_stage(game_session::session.player_table);
        return previous!=player_state::stage(game_session::session.player_table);
    }
    void increment_continue_count() override {game_session::increment_continue_count();}
    void cleanup(Cleanup operation) override {
        if(operation==Cleanup::fn_00513cc0){special_state::release();return;}
        if(operation==Cleanup::fn_004feff0){player_entity::destroy_player();return;}
        if(operation==Cleanup::fn_00485340){bullet::destroy_controller();return;}
        if(operation==Cleanup::fn_004c47a0){auto& owner=game_session::context(0).objects_04[2];runtime::retire_callback_owner(static_cast<item::ItemInf*>(owner));owner=nullptr;return;}
        if(operation==Cleanup::fn_004d5be0){auto& owner=game_session::context(0).objects_04[4];runtime::retire_callback_owner(static_cast<laser::Controller*>(owner));owner=nullptr;return;}
        if(operation==Cleanup::fn_004bc220){item::controller()->initialize_pool();return;}
        if(operation==Cleanup::fn_00477fd0){bomb::destroy_controller();return;}
        if(operation==Cleanup::fn_0049d400){effects::controller(0)->clear();return;}
        if(operation==Cleanup::fn_004aa940){destroy_enemy_controller();return;}
        if(operation==Cleanup::fn_004a80f0){enemy_controller().clear_entities();return;}
        if(operation==Cleanup::fn_004b64a0){hud::release_stage_resources(*hud::controller);return;}
        if(operation==Cleanup::fn_00510830){auto& owner=game_session::context(0).objects_04[6];if(owner){runtime::retire_callback_owner(static_cast<runtime::CallbackOwner*>(owner));owner=nullptr;}return;}
        if(operation==Cleanup::fn_00488690){auto& owner=game_session::context(0).objects_04[3];if(owner){runtime::retire_callback_owner(static_cast<runtime::CallbackOwner*>(owner));owner=nullptr;}return;}
        if(operation==Cleanup::fn_00532f40){overlay::clear(*overlay::controller(0));return;}
        throw std::invalid_argument("Unknown original GameController cleanup operation");
    }
    void remove_callback(scheduler::Node* node) override {scheduler::remove(*pe::function_controller,pe::scheduler_environment,node);}
    void stop_music() override { // 4d9bc0: the literal at571b14 is "dummy"
        pe::thread_registry.enqueue((pe::graphics_state.configuration.flags&16u)?4:3,0,"dummy");
    }
    void clear_queued_music_name() override {pe::thread_registry.queued_track[0]=0;}
    void stop_all_effects() override {pe::thread_registry.requests[0].id=-1;pe::thread_registry.stop_effects(-1);}
    void set_final_clear_color(std::uint32_t color) override {pe::graphics_state.clear_color=color;}
};
GameServices& game_services() {static GameServices services;return services;}
}
}
namespace th20::source::platform_window::unrecovered {
std::uint32_t* current_game_flags(){return gameplay::controller?&gameplay::controller->game_flags:nullptr;}
void restore_stage_selection() {gameplay::restore_stage(game_session::session.player_table);}
bool stage_selection_changed() {
    const auto current=gameplay::player_state::stage(game_session::session.player_table);
    return current!=gameplay::player_state::previous_stage(game_session::session.player_table);
}
void create_game(int mode) {gameplay::create(gameplay::game_services(),mode);}
void destroy_game() {gameplay::destroy(gameplay::game_services(),gameplay::controller);}
int game_restart_mode() {
    if(!gameplay::controller) throw std::logic_error("Scene restart requires current GameController");
    return gameplay::controller->restart();
}
}
namespace th20::source::sprite::anm_environment {
bool gameplay_frozen() {return gameplay::animation_frozen();}
}
namespace th20::source::sprite::controller_environment {
bool suppress_primary_update() {return gameplay::suppress_primary_update();}
}
