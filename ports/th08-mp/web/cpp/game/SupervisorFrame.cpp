// Original Supervisor::OnUpdate frame order and scene transitions.
#include "SupervisorFrame.hpp"
namespace th08 {
JobResult SupervisorFrame::title(bool replay_finished){state.target=i32(Scene::Title);actions.discard_graphics();return actions.title(replay_finished)?JobResult::Continue:JobResult::Exit;}
JobResult SupervisorFrame::transition(){
    state.previous=state.active;const auto active=Scene(state.active),target=Scene(state.target);
    switch(active){
    case Scene::Init:return title();
    case Scene::Title:
        switch(target){case Scene::Exit:return JobResult::Exit;case Scene::Error:return JobResult::Error;
        case Scene::Game:if(!actions.game())return JobResult::Exit;break;
        case Scene::Results:if(!actions.results(false))return JobResult::Exit;break;
        case Scene::Music:if(!actions.music())return JobResult::Exit;break;
        case Scene::Ending:actions.cut_game();if(!actions.ending())return JobResult::Exit;break;
        default:break;}break;
    case Scene::Game:
        switch(target){
        case Scene::Exit:return JobResult::Exit;
        case Scene::Title:actions.cut_game();state.target=i32(Scene::Init);actions.save_replay();return title();
        case Scene::GameResults:actions.cut_game();if(!actions.results(true))return JobResult::Exit;break;
        case Scene::Restart:
            actions.cut_game();if(!state.practice&&state.difficulty<4)state.stage=0;actions.save_replay();if(!actions.game())return JobResult::Exit;state.target=i32(Scene::Game);break;
        case Scene::SpellRestart:
            state.target=i32(Scene::Reinitialize);state.keep_resources=true;actions.cut_game();if(!actions.game())return JobResult::Exit;state.target=i32(Scene::Game);break;
        case Scene::NextStage:
            state.target=i32(Scene::Reinitialize);actions.cut_game();actions.advance_stage();if(!actions.game())return JobResult::Exit;state.target=i32(Scene::Game);break;
        case Scene::Reinitialize:
            actions.cut_game();if(!actions.game())return JobResult::Exit;state.target=i32(Scene::Game);break;
        case Scene::FinishReplay:
            actions.cut_game();state.target=i32(Scene::Init);actions.save_replay();return title(true);
        case Scene::Ending:actions.cut_game();if(!actions.ending())return JobResult::Exit;break;
        default:break;}break;
    case Scene::GameResults:
        if(target==Scene::Exit){actions.save_replay();return JobResult::Exit;}
        if(target==Scene::Title){state.target=i32(Scene::Init);actions.save_replay();return title();}break;
    case Scene::Results:case Scene::Music:
        if(target==Scene::Exit)return JobResult::Exit;
        if(target==Scene::Title){state.target=i32(Scene::Init);return title();}break;
    case Scene::Ending:
        if(target==Scene::Exit)return JobResult::Exit;
        if(target==Scene::Title){state.target=i32(Scene::Init);return title();}
        if(target==Scene::GameResults&&!actions.results(true))return JobResult::Exit;break;
    default:break;
    }
    return JobResult::Continue;
}
JobResult SupervisorFrame::update(){
    if(state.close_requested&&!state.background_running)return JobResult::Exit;
    renderer.current_shader=0xff;renderer.current_sprite=nullptr;renderer.current_texture=0;renderer.current_color_op=0xff;renderer.current_blend=3;renderer.disable_z_write=0xff;
    renderer.state_changes=renderer.flushes=0;renderer.camera_mode=0xff;renderer.mix_color=0x80808080;renderer.mix_enabled=false;renderer.shake={};
    actions.animate_loading();if(!actions.service_animations())return JobResult::Exit;
    if(state.background_status){if(state.background_status==2)return JobResult::Exit;if(!state.background_wait)state.background_status=0;else return JobResult::Continue;}
    state.fog=0xff;actions.update_audio_fades();input.update(actions.poll_input(),state.sticky_input);
    if(state.active!=state.target){const auto result=transition();if(result!=JobResult::Continue)return result;input.current=input.previous=input.scrolling=0;}
    state.active=state.target;state.calculations=wrapping_add(state.calculations,1);
    if(state.calculations%4000==3999&&!actions.version_valid())return JobResult::Exit;
    if(state.screen_effect_counter)state.screen_effect_counter=wrapping_sub(state.screen_effect_counter,1);
    return JobResult::Continue;
}
}
