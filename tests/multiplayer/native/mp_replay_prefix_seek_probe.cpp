// Standalone fixture for tests/multiplayer/replay-prefix-seek.py.
// The generated include contains exact production BrowserRuntime functions.
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/ReplayArchive.hpp"
#include "multiplayer/StageEntry.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/HostState.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
using namespace th13;
namespace th13::sdl {
std::unique_ptr<ApplicationState> app;
std::unique_ptr<multiplayer::ReplayArchive> mp_replay_loaded;
std::string last_error;
bool mp_playback_pending=true,mp_started=true,mp_playback_paused=false,mp_recording=false;
u32 mp_playback_frame=0,mp_playback_chapter=0,mp_playback_seek_target=Netplay::INVALID_FRAME;
unsigned mp_playback_rate=1;
}
using namespace th13::sdl;
#define EMSCRIPTEN_KEEPALIVE
#include "production-playback.inc"
static FrameCallback trigger;
static unsigned clear_calls=0,checks=0,failures=0;
static bool terminal_mode=false;
static void check(bool ok,const char* name){++checks;failures+=!ok;printf("CHECK passed=%d name=%s frame=%u chapter=%u target=%u error=%s\n",ok,name,mp_playback_frame,mp_playback_chapter,mp_playback_seek_target,last_error.c_str());}
static i32 clear_event(void*){
    // A bounded API fixture triggers the production clear path, identically
    // on continuous playback and every prefix reconstruction. Not a clear proof.
    if(app->frames==8){++clear_calls;app->run->battle.sim.dialog_host->open_dialog(-2);}
    if(terminal_mode&&app->frames==200)app->request_destination(0xf);
    return 1;
}
static multiplayer::SessionSetup setup(unsigned count){multiplayer::SessionSetup s;s.version=2;s.mode=2;s.player_count=count;s.local_player=0;s.difficulty=count==3?3:1;s.seed=8719;s.session_id=73;s.build={1,2,3,4};s.input_delay=0;s.prediction_limit=12;s.selections={0,0,0};return s;}
static bool boot(unsigned count){
    multiplayer::session.Stop();const auto s=setup(count);const auto words=multiplayer::EncodeSessionSetup(s);if(!multiplayer::session.Configure(words.data(),words.size()))return false;
    app=std::make_unique<ApplicationState>();setenv("TH13_SAVE_PATH",("prefix-seek-"+std::to_string(count)+".bin").c_str(),1);
    const char* assets=std::getenv("TH13_NATIVE_ASSET_ROOT");const char* root=std::getenv("TH13_NATIVE_REPO_ROOT");if(!assets||!root)return false;
    if(!app->audio_device.initialize_for_test(assets,std::string(root)+"/games/th13/thbgmogg.dat")||!app->initialize(false)||!app->configure_multiplayer(count,{0,0,0},0))return false;
    app->progress.character=0;app->progress.difficulty=s.difficulty;
    if(!app->start_game(5))return false;app->pending_destination=-1;app->progress.stage=app->progress.starting_stage=app->selected_stage=5;app->frames=0;
    app->game_random.reseed_restore(8719);app->visual.reseed_restore(8719);if(!app->begin_run()||!app->complete_loading())return false;
    multiplayer::StageEntry root_entry;root_entry.Capture(app->run->battle,app->progress,app->run->score_view(),app->game_random.seed(),app->game_random.index(),app->visual.seed(),app->visual.index());
    multiplayer::ReplayArchive archive;if(!archive.Begin(s,42)||!archive.AppendStageEntry(5,root_entry.Encode()))return false;
    for(unsigned f=0;f<220;++f){std::array<Netplay::FrameInput,3> input{};for(unsigned seat=0;seat<count;++seat){const u32 held=f<165?1u:((f/7+seat)%2?0x41:0x81);if(!multiplayer::InputLanes::Capture(held,(f==175||f==190)&&seat==count-1,input[seat]))return false;}if(!archive.Append(f,f<9?5:6,input.data(),count))return false;}
    std::vector<u8> wire;if(!archive.Encode(wire,"BOUNDARY",0,false))return false;mp_replay_loaded=std::make_unique<multiplayer::ReplayArchive>();if(!mp_replay_loaded->Load(wire.data(),wire.size()))return false;
    mp_playback_pending=mp_started=true;mp_playback_frame=mp_playback_chapter=0;mp_playback_seek_target=Netplay::INVALID_FRAME;mp_playback_paused=false;mp_playback_rate=1;
    trigger.enabled=true;trigger.run=clear_event;return app->scheduler.add(trigger,FramePass::Update,19)>=0;
}
struct State{u64 world=0,host=0;u32 game_index=0,visual_index=0;int stage=0,sim=0,chapter=0;std::string domains,detail,host_detail;bool operator==(const State& b)const{return world==b.world&&host==b.host&&game_index==b.game_index&&visual_index==b.visual_index&&stage==b.stage&&sim==b.sim&&chapter==b.chapter;}};
static State state(){multiplayer::WorldState world;State s;if(!app->run){check(false,"state requires live world");return s;}const bool captured=world.capture(app->run->battle,app->visual,nullptr,app->run->presentation.get(),app->run.get(),app->run_transition.handle());check(captured,"complete world capture");if(!captured)return s;s.world=world.digest();s.host=app->digest_multiplayer_host();s.game_index=app->game_random.index();s.visual_index=app->visual.index();s.stage=app->progress.stage;s.sim=app->run->battle.sim.frame;s.chapter=app->progress.chapter_frame;char domain[4096]{};world.digest_domains(domain,sizeof domain);s.domains=domain;s.detail=world.diagnostic_detail();multiplayer::HostState host;app->capture_multiplayer_host_state(host);s.host_detail=multiplayer::WorldState::diagnostic_host_detail(host);return s;}
static bool tick(){if(app->loading_waiting()&&!app->complete_loading())return false;unsigned simulated=0;return mp_tick_playback(60.f,simulated);}
static bool drain(unsigned target){unsigned calls=0;while(mp_playback_seek_target!=Netplay::INVALID_FRAME&&calls++<400){if(!tick())return false;}return mp_playback_frame==target&&mp_playback_seek_target==Netplay::INVALID_FRAME;}
static void compare(const State& actual,const State& expected,const char* name){const bool ok=actual==expected;if(!ok){printf("DIFF name=%s world=%llx/%llx host=%llx/%llx gi=%u/%u vi=%u/%u stage=%d/%d sim=%d/%d chapter=%d/%d\n",name,(unsigned long long)actual.world,(unsigned long long)expected.world,(unsigned long long)actual.host,(unsigned long long)expected.host,actual.game_index,expected.game_index,actual.visual_index,expected.visual_index,actual.stage,expected.stage,actual.sim,expected.sim,actual.chapter,expected.chapter);printf("ACTUAL_DOMAINS %s\nEXPECTED_DOMAINS %s\nACTUAL_DETAIL %s\nEXPECTED_DETAIL %s\nACTUAL_HOST %s\nEXPECTED_HOST %s\n",actual.domains.c_str(),expected.domains.c_str(),actual.detail.c_str(),expected.detail.c_str(),actual.host_detail.c_str(),expected.host_detail.c_str());}check(ok,name);}
// Record an independent live Session before invoking any replay function.
// Its pre-input root and eight resulting worlds are the expected authority.
static void compare_recorded_root(unsigned count,unsigned rank,unsigned stage,unsigned character){
    auto& session=multiplayer::session;
    session.Stop();last_error.clear();auto s=setup(count);s.difficulty=rank;for(unsigned seat=0;seat<count;++seat)s.selections[seat]=character;
    const auto words=multiplayer::EncodeSessionSetup(s);
    check(session.Configure(words.data(),words.size()),"record configure");
    app=std::make_unique<ApplicationState>();
    const auto save="root-record-"+std::to_string(count)+"-"+std::to_string(rank)+"-"+std::to_string(character)+".bin";
    setenv("TH13_SAVE_PATH",save.c_str(),1);
    const char* assets=std::getenv("TH13_NATIVE_ASSET_ROOT");const char* root=std::getenv("TH13_NATIVE_REPO_ROOT");
    check(assets&&root&&app->audio_device.initialize_for_test(assets,std::string(root)+"/games/th13/thbgmogg.dat")&&app->initialize(false),"record assets initialize");
    check(app->configure_multiplayer(count,{i32(character),i32(character),i32(character)},0),"record configure seats");
    app->progress.run_mode_flags=0;app->progress.practice_mode=0;app->progress.selected_spell_id=-1;
    app->progress.character=character;app->progress.difficulty=rank;
    check(app->start_game(stage),"record start game");app->pending_destination=-1;
    app->progress.stage=app->progress.starting_stage=app->selected_stage=stage;app->frames=0;
    app->game_random.reseed_restore(s.seed);app->visual.reseed_restore(s.seed);
    check(app->begin_run()&&app->complete_loading()&&session.BeginLocalSimulation(),"record real world begin");
    multiplayer::ReplayArchive recorded;check(recorded.Begin(s,42),"record archive begin");
    std::array<State,9> expected{};unsigned captured=0,confirmed=0;
    session.stage_source=[stage]{return stage;};
    session.spectator_sink=[&](u32 f,const std::array<Netplay::FrameInput,3>&){
        if(f==0){expected[0]=state();multiplayer::StageEntry entry;
            entry.Capture(app->run->battle,app->progress,app->run->score_view(),app->game_random.seed(),app->game_random.index(),app->visual.seed(),app->visual.index());
            check(recorded.AppendStageEntry(stage,entry.Encode()),"record exact preinput root");++captured;
        }return true;
    };
    session.confirmed_sink=[&](u32 f,u32 label,const std::array<Netplay::FrameInput,3>& inputs){check(recorded.Append(f,label,inputs.data(),count),"record exact committed inputs");++confirmed;};
    for(unsigned f=0;f<8;++f){
        std::array<u32,3> masks{};for(unsigned seat=0;seat<count;++seat)masks[seat]=((f+seat)%3==0?0x41u:0x81u)|1u;
        for(unsigned seat=1;seat<count;++seat){Netplay::FrameInput input{};check(multiplayer::InputLanes::Capture(masks[seat],false,input),"record encode remote");const auto result=session.SubmitRemote(seat,f,input);check(result!=Netplay::RemoteInputResult::InvalidPlayer&&result!=Netplay::RemoteInputResult::ConflictingConfirmedInput&&result!=Netplay::RemoteInputResult::TooOld,"record submit remote");}
        check(session.Pump(f*1000/60)&&session.CaptureLocal(masks[0],f*1000/60),"record capture local");session.AdvanceCapturePhase();
        check(session.CanSimulate()&&app->update(session.MenuInput(),false)&&app->commit(60.f),"record live update");session.OnFrameSimulated();check(app->present(),"record live present");expected[f+1]=state();
    }
    check(captured==1&&confirmed==8&&recorded.FrameCount()==8&&session.Committed()==8,"record contiguous actual Session archive");
    std::vector<u8> wire;check(recorded.Encode(wire,"ROOT",0,false),"record archive encode");
    // replay_load uses Configure to retire the live Session before reconstruction.
    check(session.Configure(words.data(),words.size()),"replay configure replaces live Session");
    mp_replay_loaded=std::make_unique<multiplayer::ReplayArchive>();check(mp_replay_loaded->Load(wire.data(),wire.size()),"record archive reload");
    mp_playback_pending=mp_started=true;mp_playback_frame=mp_playback_chapter=0;mp_playback_seek_target=Netplay::INVALID_FRAME;mp_playback_paused=false;mp_playback_rate=1;
    check(mp_start_playback(),"recorded world playback start");
    printf("RECORD_COMPARE seats=%u rank=%u stage=%u character=%u frame=0\n",count,rank,stage,character);
    compare(state(),expected[0],"replay root equals independent live preinput world");
    for(unsigned f=0;f<8;++f){check(tick(),"recorded world playback tick");printf("RECORD_COMPARE seats=%u rank=%u stage=%u character=%u frame=%u\n",count,rank,stage,character,f+1);compare(state(),expected[f+1],"replay frame equals independent recorded Session world");}
    app.reset();session.Stop();
}
int main(){setvbuf(stdout,nullptr,_IOLBF,0);
    compare_recorded_root(2,1,1,0);compare_recorded_root(3,3,1,0);compare_recorded_root(2,4,7,0);
    for(unsigned character:{1u,2u,3u})compare_recorded_root(2,1,1,character);
    for(unsigned count:{2u,3u}){
        check(boot(count),"boot real assets");if(!app)return 2;
        const auto recorded_root=state();check(mp_start_playback(),"root start");compare(state(),recorded_root,"saved root rebuild equals live initialized world");std::vector<State> expected(221);expected[0]=state();multiplayer::StageEntry wrong;
        for(unsigned f=0;f<220;++f){check(tick(),"continuous frame");expected[f+1]=state();if(f+1==9)wrong.Capture(app->run->battle,app->progress,app->run->score_view(),app->game_random.seed(),app->game_random.index(),app->visual.seed(),app->visual.index());}
        check(expected[9].stage==6&&expected[9].sim==9,"chapter label precedes world activation");check(expected[165].stage==6&&expected[165].sim<20,"real transition activates new world");
        // Preserve the actual production wrong carry in the saved archive.
        multiplayer::ReplayArchive saved;check(saved.Begin(setup(count),42),"save begin");check(saved.AppendStageEntry(5,*mp_replay_loaded->StageEntryAt(5)),"save root entry");check(saved.AppendStageEntry(6,wrong.Encode()),"save real intermediate carry");
        for(unsigned f=0;f<220;++f)check(saved.Append(f,f<9?5:6,mp_replay_loaded->FrameAt(f)->data(),count),"save input");std::vector<u8> wire;check(saved.Encode(wire,"BOUNDARY",0,false),"save encode");check(mp_replay_loaded->Load(wire.data(),wire.size()),"saved archive reload");
        for(unsigned target:{9u,0u,165u,177u,220u,100u,9u}){
            mp_playback_paused=true;mp_playback_rate=8;check(th13_mp_replay_seek(target)==1,"timeline seek accepted");check(mp_playback_paused&&mp_playback_rate==8,"seek retains pause and rate");check(target==0?mp_playback_seek_target==Netplay::INVALID_FRAME:mp_playback_seek_target==target,"seek exposes real pending target");check(drain(target),"bounded seek reaches target");compare(state(),expected[target],"prefix seek all authoritative state");const auto before=mp_playback_frame;check(tick()&&mp_playback_frame==before,"paused seek remains paused");
        }
        mp_playback_paused=true;mp_playback_rate=7;check(th13_mp_replay_seek_stage(6)==1,"chapter seek accepted");check(mp_playback_chapter==6&&mp_playback_frame==0&&mp_playback_seek_target==9,"chapter request retained while root replay pending");check(drain(9),"chapter seek drains saved prefix");compare(state(),expected[9],"chapter seek real state");
        check(th13_mp_replay_seek(176)==1&&drain(176)&&app->pause->active(),"seek reaches actual paused game");check(th13_mp_replay_seek(203)==1&&drain(203)&&!app->pause->active(),"seek reconstructs pause resume edges after real animation gates");
        const unsigned frame_before_invalid=mp_playback_frame;check(!th13_mp_replay_seek(221)&&!th13_mp_replay_seek_stage(7)&&mp_playback_frame==frame_before_invalid,"invalid seek leaves cursor intact");
        // Rate batches must apply the clear destination at the exact frame.
        check(th13_mp_replay_seek(0)==1,"rate replay restart");mp_playback_paused=false;mp_playback_rate=8;unsigned guard=0;while(mp_playback_frame<220&&guard++<100){check(tick(),"rate8 frame batch");compare(state(),expected[mp_playback_frame],"rate8 identical at batch frontier");}
        multiplayer::ReplayArchive terminal;check(terminal.Begin(setup(count),42),"terminal fixture begin");check(terminal.AppendStageEntry(5,*mp_replay_loaded->StageEntryAt(5))&&terminal.AppendStageEntry(6,wrong.Encode()),"terminal fixture entries");for(unsigned f=0;f<201;++f)check(terminal.Append(f,f<9?5:6,mp_replay_loaded->FrameAt(f)->data(),count),"terminal fixture inputs");std::vector<u8> end_wire;check(terminal.Encode(end_wire,"TERMINAL",0,true)&&mp_replay_loaded->Load(end_wire.data(),end_wire.size()),"terminal fixture saved and loaded");terminal_mode=true;
        mp_playback_paused=true;check(th13_mp_replay_seek(201)==1&&drain(201),"seek end includes final handoff input");check(tick()&&!app->run&&bool(app->ending)&&mp_playback_frame==201,"end tick completes real loading and releases world");check(tick()&&mp_playback_frame==201,"end tick never dereferences absent run");check(th13_mp_replay_seek(0)==1&&app->run&&!app->ending&&mp_playback_frame==0,"backward seek from Ending builds root");terminal_mode=false;
        // Old direct stage+carry path is a semantic negative, not malformed data.
        check(app->begin_title(0)&&app->configure_multiplayer(count,{0,0,0},0)&&app->start_game(6),"negative direct stage construct");app->pending_destination=-1;app->progress.stage=app->progress.starting_stage=app->selected_stage=6;app->progress.difficulty=setup(count).difficulty;app->frames=0;app->game_random.reseed_restore(8719);app->visual.reseed_restore(8719);check(app->begin_run(),"negative direct stage begin");u16 gs=0,vs=0;u32 gi=0,vi=0;wrong.Restore(app->run->battle,app->progress,app->run->score_view(),gs,gi,vs,vi);app->game_random.reseed_restore(gs);app->game_random.set_index(gi);app->visual.reseed_restore(vs);app->visual.set_index(vi);check(!(state()==expected[9]),"negative old direct carry restoration diverges");
        app->scheduler.remove(trigger);app.reset();multiplayer::session.Stop();
    }
    printf("SUMMARY checks=%u failures=%u scope=synthetic-clear-trigger-real-transition-and-archive-prefix-replay natural_clear=0\n",checks,failures);return failures?1:0;
}
