#include "SpectatorReplayStart.hpp"
#include "../sdl/ApplicationState.hpp"
#include "../game/MultiplayerPilot.hpp"
namespace th15::multiplayer {
bool SpectatorReplayStart::Boundary(const sdl::ApplicationState& app){
    return app.scene()&&app.replay&&app.replay->activation_count()>0&&app.session&&app.session->driver();
}
bool SpectatorReplayStart::Capture(sdl::ApplicationState& app,const SessionSetup& setup,const GameInput&,
    u32 frame,std::vector<u8>& out,std::string& error){
    error.clear();if(!Boundary(app)){error="Native Replay stage entry is not ready";return false;}
    const auto* native=app.replay->snapshot(u32(app.progress.stage));
    if(!native){error="Native Replay stage header is missing";return false;}
    SpectatorStageState s;s.frame=frame;s.stage=u32(app.progress.stage);s.seats=setup.player_count;
    if(s.seats<2||s.seats>3)return false;
    for(unsigned i=0;i<s.seats;++i)s.headers[i]=app.replay->seat_entry(i).bytes();
    return EncodeSpectatorStageState(s,out);
}
bool SpectatorReplayStart::Restore(sdl::ApplicationState& app,const SessionSetup& setup,GameInput& menu,
    u32 frame,const std::vector<u8>& bytes,std::string& error){
    error.clear();SpectatorStageState s;
    if(!DecodeSpectatorStageState(bytes,s)||s.frame!=frame||s.seats!=setup.player_count){error="Invalid streaming Replay stage entry";return false;}
    ReplayStageSnapshot first;if(!first.read(s.headers[0].data(),0x238))return false;
    EnemyWorldState enemies;PlayerMotion motion;std::string music;
    ReplayRunState selection{app.progress,app.selection_player,app.selection_score,enemies,motion,music};
    if(!first.restore_progress(selection)||!first.restore_seed(app.environment.game_rng,&app.visual))return false;
    if(app.progress.stage!=i32(s.stage)||app.progress.difficulty!=i32(setup.difficulty))return false;
    app.selected_stage=i32(s.stage);app.progress.new_run=true;app.mp_options.stage=s.stage;
    app.pending_load=0;app.pending_destination=-1;app.current_destination=12;app.exiting=false;app.failure.clear();app.files.read_only=true;
    std::array<ReplayStageSnapshot,3> entries;
    for(unsigned i=0;i<s.seats;++i)if(!entries[i].read(s.headers[i].data(),0x238))return false;
    if(!app.begin_run(&entries)||!app.scene()||!app.session||!app.session->driver()){error=app.failure;return false;}
    // Run the native Replay entrance once, before consuming input frame F.
    if(!app.session->enter_replay_stage()){error=app.session->error;return false;}
    app.run->release_previous();app.display->release_previous();menu={};return true;
}
}
