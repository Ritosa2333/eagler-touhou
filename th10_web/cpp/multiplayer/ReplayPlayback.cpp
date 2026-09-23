#include "ReplayFiles.hpp"
#include "../platform/Application.hpp"

namespace th10::browser {
bool Application::multiplayer_begin_replay(const char* filename,u32 selected_stage){
    if(stopped||multiplayer_replay_scope||state.netplay_runtime.NetworkEnabled()||
       value.screen!=4||!title||!startup||!startup->scores||(world&&world->actors.session))return false;
    std::vector<u8> bytes;multiplayer::ReplayArchive candidate;
    if(!multiplayer::ReadReplayFile(files,filename,bytes)||!candidate.Load(bytes.data(),bytes.size()))return false;
    const auto seek=candidate.SeekFrame(selected_stage);
    if(seek==Netplay::INVALID_FRAME||!ensure_world())return false;
    const bool direct=candidate.SelectCheckpoint(selected_stage);
    auto setup=candidate.Description().setup;
    if(!state.netplay_runtime.BeginPlayback(setup))return false;
    if(!startup->scores->begin_replay()){state.netplay_runtime.Clear();return false;}
    multiplayer_replay_saved_config=state.configuration;multiplayer_replay_scope=true;
    state.configuration=candidate.Description().configuration;
    state.multiplayer_replay=std::move(candidate);multiplayer_replay_seek_target=direct?0:seek;
    multiplayer_replay_seek_stage=selected_stage;
    world->player_count=setup.playerCount;world->local_player=setup.localPlayer;
    for(u32 seat=0;seat<setup.playerCount;++seat){
        auto& game=world->pilots[seat].game;game.character=i32(setup.loadouts[seat].character);
        game.shot_type=i32(setup.loadouts[seat].shot);
    }
    state.game.difficulty=i32(setup.difficulty);
    state.game.stage=direct?i32(selected_stage):setup.difficulty==4?7:1;
    state.game.reserved_040=u32(state.game.stage);state.game.flags=0;
    state.current_stage=menu_data(state.chinese).stages+state.game.stage;
    engine.script_random.seed=engine.visual_random.seed=u16(setup.seed);
    engine.script_random.calls=engine.visual_random.calls=0;
    setup.started=true;state.multiplayer_session=setup;state.input_lanes={};input.player_profiles[0].input={};
    state.multiplayer_cheat_movement_used=false;
    world->audio_events.Reset();world->rollback.Clear();
    multiplayer_frame_open=multiplayer_generation_pending=multiplayer_waiting=false;
    multiplayer_pending_frame=Netplay::INVALID_FRAME;multiplayer_replay_escape=false;
    return true;
}
bool Application::multiplayer_replay_seeking()const{
    // Reconstruct every preceding native tick, including stage loads and run
    // resets. A stage number alone is not a snapshot of all pilots and RNGs.
    // Stop after the first selected tick, so the displayed frame belongs to
    // the chosen stage rather than to its predecessor's transition.
    return multiplayer_replay_scope&&state.multiplayer_session.started&&
           state.multiplayer_replay.Playing()&&!state.multiplayer_replay.Complete()&&
           multiplayer_replay_seek_target>0&&
           state.multiplayer_replay.Cursor()<=multiplayer_replay_seek_target;
}
void Application::multiplayer_finish_replay(){
    if(!multiplayer_replay_scope)return;
    if(startup&&startup->scores)startup->scores->end_replay();
    state.configuration=multiplayer_replay_saved_config;multiplayer_replay_scope=false;
    state.netplay_runtime.Clear();state.multiplayer_session={};state.input_lanes={};
    input.player_profiles[0].input={};multiplayer_replay_seek_target=multiplayer_replay_seek_stage=0;
}
}
