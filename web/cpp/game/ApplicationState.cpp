#include "ApplicationState.hpp"
#include "GameSession.hpp"
namespace th10 {
// 0x4203f0. Read each global immediately before its destruction: shutting down
// a game or title may already release the replay owned by that screen.
void ApplicationState::shutdown_screens(ApplicationEnvironment& env){if(*env.current_game)env.destroy_game(*env.current_game);if(*env.current_title)env.destroy_title(*env.current_title);if(*env.current_startup)env.destroy_startup(*env.current_startup);if(*env.current_ending)env.destroy_ending(*env.current_ending);if(*env.current_replay)env.destroy_replay(*env.current_replay);}
// Lock counters are changed after the platform call, including callbacks that
// change the counter while entering or leaving the critical section.
void ApplicationState::lock(u32 index,ApplicationEnvironment& env){env.enter_lock(*this,index);++lock_depth[index];}
void ApplicationState::unlock(u32 index,ApplicationEnvironment& env){env.leave_lock(*this,index);--lock_depth[index];}
// 0x420ea0 / 0x420f20. Loading replacement joins the previous worker while
// holding the loader lock, then starts an unsuspended worker.
i32 ApplicationState::begin_loading(CallbackToken callback,void* argument,ApplicationEnvironment& env){lock(6,env);resource_loader.start(callback,argument,false,env);unlock(6,env);return 0;}
void ApplicationState::stop_loading(ApplicationEnvironment& env){resource_loader.stop(env);}
// 0x420f30. The three loading animations are only created from the idle state.
void ApplicationState::show_loading(const Vec3& position,ApplicationEnvironment& env){if(loading_state)return;for(i32 i=0;i<3;++i)env.loading_ids[i]=env.create_loading_animation(*loading_animations,i);loading_state=1;for(u32 i=0;i<3;++i)env.registry->set_position(env.loading_ids[i],position,false);}
// 0x421070 / 0x421300. Failure retains state 2, successful completion returns
// to idle, and either outcome releases the application loading pause.
void ApplicationState::finish_loading(bool success,ApplicationEnvironment& env){if(loading_state==1){for(u32 i=0;i<3;++i)env.registry->interrupt(env.loading_ids[i],success?1:2);env.loading_ids[0]=env.loading_ids[1]=env.loading_ids[2]=0;loading_state=success?0:2;}if(*env.loading_pause)*env.loading_pause=0;}
// 0x4218d0. These are the original application transitions, including replay
// mode propagation when advancing a stage and the two distinct title returns.
i32 ApplicationState::transition(ApplicationEnvironment& env){
    if(screen==pending_screen)return 1;lock(5,env);const i32 previous=screen;previous_screen=previous;background_color=0xff000000;
    switch(pending_screen){
    case 0:pending_screen=1;startup=env.create_startup_screen(*this);if(startup)break;pending_screen=3;[[fallthrough]];
    case 3:env.destroy_screens(*this);unlock(5,env);return 4;
    case 4:
        if(previous==7)env.destroy_game(*env.current_game);else if(previous==14)env.destroy_ending(*env.current_ending);else if(previous!=1&&previous!=2)break;
        env.create_title();break;
    case 7:if(previous==4)env.destroy_title(*env.current_title);new_game=1;env.create_game(0);break;
    case 10:env.destroy_game(*env.current_game);new_game=1;pending_screen=7;env.game->stage=env.game->reserved_040;*env.current_stage=env.stages+env.game->stage;env.create_game(0);break;
    case 11:{const i32 mode=(*env.current_game)->replay_mode;new_game=0;if(previous==7)env.destroy_game(*env.current_game);pending_screen=7;env.create_game(mode);break;}
    case 12:if(previous==4)env.destroy_title(*env.current_title);pending_screen=7;new_game=1;env.create_game(1);break;
    case 13:env.destroy_game(*env.current_game);new_game=1;pending_screen=7;env.create_game(0);break;
    case 14:if(previous==7)env.destroy_game(*env.current_game);env.create_ending();break;
    case 15:
        if(previous==7)env.destroy_game(*env.current_game);else if(previous==14)env.destroy_ending(*env.current_ending);else if(previous!=2)break;
        pending_screen=4;*env.return_menu=3;env.create_title();break;
    default:break;
    }
    screen=pending_screen;unlock(5,env);return 1;
}
}
