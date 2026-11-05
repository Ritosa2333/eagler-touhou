#include "gameplay.hpp"
#include <stdexcept>

namespace th20::source::gameplay {
GameController* controller=nullptr;
GameController::GameController(Services& environment)
    :frame_timer{},secondary_timer{},load_stage(0),field_34(0),configuration{},game_flags(0),
     field_ec(0),field_f0(0),field_f4(0),field_f8(0),field_100(0),restart_mode(0),field_10c(0),services(&environment) {
    platform::initialize_configuration(configuration);
}
bool animation_frozen() noexcept {return controller && controller->animation_frozen();}
bool suppress_primary_update() noexcept {return controller && controller->update_suppressed() && controller->animation_frozen();}
GameController::~GameController() {
    auto& env=*services;
    const auto retire=[&](Owner which) {runtime::retire_callback_owner(env.owner(which));};
    const auto disable=[&](Owner which) {env.owner(which)->disable_callbacks();};
    const auto bit=[&](unsigned index) {return (env.session_flags()&(1u<<index))!=0;};
    clear_flag_6();env.commit_progress_if_present();env.reset_clock_scale();env.clear_surface_callbacks();
    // Scene queries are intentionally repeated after dependent calls. The
    // original observes changes those calls make to the shared scene value.
    if(env.scene()==10 || env.scene()==11) {
        env.screen_transition(480.0f,392.0f); // constants 56cda8 and570388
        if(env.scene()==22) env.session_flags()=(env.session_flags()&~2u)|2u;
        env.session_flags()=(env.session_flags()&~1u)|1u;
    } else if(env.scene()==4 || env.scene()==16) env.screen_transition(480.0f,392.0f);
    else if(env.scene()==14) {
        env.screen_transition(480.0f,392.0f);
        if(env.stage_selection_changed()) {env.increment_continue_count();env.session_flags()=(env.session_flags()&~16u)|16u;}
        env.session_flags()=(env.session_flags()&~1u)|1u;
    } else (void)env.scene();
    if(!bit(2)) {
        if(env.scene()!=15 && env.scene()!=16) retire(Owner::global_005c60fc);
        for(auto owner:{Owner::global_005c069c,Owner::global_005c06a0,Owner::global_005c60bc,Owner::global_005c06a4}) retire(owner);
        for(auto operation:{Cleanup::fn_004feff0,Cleanup::fn_00485340,Cleanup::fn_004c47a0,Cleanup::fn_004d5be0,Cleanup::fn_00510830,Cleanup::fn_00513cc0}) env.cleanup(operation);
    } else {
        env.cleanup(Cleanup::fn_004b64a0);retire(Owner::global_005c06a0);
        env.preserve_background_as_secondary();
        disable(Owner::global_005c60bc);disable(Owner::player_primary);disable(Owner::global_005c60fc);
        env.cleanup(Cleanup::fn_004bc220);
    }
    if(!bit(0) && !bit(4)) env.cleanup(Cleanup::fn_004aa940);else env.cleanup(Cleanup::fn_004a80f0);
    env.cleanup(Cleanup::fn_0049d400);env.cleanup(Cleanup::fn_00477fd0);env.cleanup(Cleanup::fn_00488690);
    env.cleanup(Cleanup::fn_00532f40);disable(Owner::session_overlay);disable(Owner::global_005c6120);
    env.remove_callback(update_node);env.remove_callback(draw_node);controller=nullptr;
    if(!(env.session_mode()==2 && bit(0)) && !bit(2) && !bit(5)) {env.stop_music();env.clear_queued_music_name();}
    env.stop_all_effects();env.set_final_clear_color(bit(0)?0u:0xff000000u);
    retire(Owner::global_005c6114);
}
void start_loading(Services& services) {
    std::lock_guard<std::recursive_mutex> outer(runtime::shared_locks().slot(6)); // 4b99f0
    std::lock_guard<std::recursive_mutex> inner(runtime::shared_locks().slot(6)); // 40b1d0
    auto& worker=services.worker();
    {
        std::lock_guard<std::recursive_mutex> detach_lock(runtime::shared_locks().slot(6)); //40bc60
        if(worker.thread.joinable()) worker.thread.detach();
    }
    worker.close_requested.store(false,std::memory_order_seq_cst);
    worker.thread=TH20_WORKER_THREAD("gameplay-load",[&services] {
        // Original 4bcca0 reads global5ba828 when the worker starts.
        if(!controller) throw std::logic_error("Game loading worker has no current GameController");
        services.load(*controller);
    });
}
GameController* create(Services& services,std::int32_t mode) {
    auto* game=new GameController(services);
    services.input_latch()=0;
#ifndef TH_SDL3
    services.device().EvictManagedResources();
#endif
    // TH_SDL3 textures keep CPU stores; there is no managed pool to evict.
    controller=game;game->restart_mode=mode;game->game_flags|=4u;
    services.owner(Owner::session_overlay)->disable_callbacks();
    services.owner(Owner::global_005c6120)->disable_callbacks();
    start_loading(services);return game;
}
void destroy(Services& services,GameController* game) {services.input_latch()=0;runtime::retire_callback_owner(game);}
}
