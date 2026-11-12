// Real-asset live Session versus exact production spectator adapter.
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/SpectatorJournal.hpp"
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
namespace th13::sdl {std::unique_ptr<ApplicationState> app;std::string last_error;u32 mp_spectator_stage=0;}
using namespace th13::sdl;
#include "production-spectator.inc"
static unsigned checks=0,failures=0;
static void check(bool ok,const char*name){++checks;failures+=!ok;printf("CHECK passed=%d name=%s error=%s\n",ok,name,last_error.c_str());}
static FrameCallback trigger;
static bool transition=true;
static i32 clear_event(void*){if(transition&&app->frames==8)app->run->battle.sim.dialog_host->open_dialog(-2);return 1;}
static multiplayer::SessionSetup setup(unsigned count,unsigned rank){multiplayer::SessionSetup s;s.version=2;s.mode=2;s.player_count=count;s.local_player=0;s.difficulty=rank;s.seed=8719;s.session_id=73;s.build={1,2,3,4};s.input_delay=0;s.prediction_limit=12;return s;}
struct State{u64 world=0,host=0;u32 game_index=0,visual_index=0;int stage=0,sim=0,chapter=0;std::string domains,detail,host_detail;bool operator==(const State& b)const{return world==b.world&&host==b.host&&game_index==b.game_index&&visual_index==b.visual_index&&stage==b.stage&&sim==b.sim&&chapter==b.chapter;}};
static State state(){multiplayer::WorldState world;State s;if(!app->run){check(false,"state requires live world");return s;}const bool captured=world.capture(app->run->battle,app->visual,nullptr,app->run->presentation.get(),app->run.get(),app->run_transition.handle());check(captured,"complete world capture");if(!captured)return s;s.world=world.digest();s.host=app->digest_multiplayer_host();s.game_index=app->game_random.index();s.visual_index=app->visual.index();s.stage=app->progress.stage;s.sim=app->run->battle.sim.frame;s.chapter=app->progress.chapter_frame;char domain[4096]{};world.digest_domains(domain,sizeof domain);s.domains=domain;s.detail=world.diagnostic_detail();multiplayer::HostState host;app->capture_multiplayer_host_state(host);s.host_detail=multiplayer::WorldState::diagnostic_host_detail(host);return s;}
static void compare(const State& actual,const State& expected,const char* name){const bool ok=actual==expected;if(!ok){printf("DIFF name=%s world=%llx/%llx host=%llx/%llx gi=%u/%u vi=%u/%u stage=%d/%d sim=%d/%d chapter=%d/%d\n",name,(unsigned long long)actual.world,(unsigned long long)expected.world,(unsigned long long)actual.host,(unsigned long long)expected.host,actual.game_index,expected.game_index,actual.visual_index,expected.visual_index,actual.stage,expected.stage,actual.sim,expected.sim,actual.chapter,expected.chapter);printf("ACTUAL_DOMAINS %s\nEXPECTED_DOMAINS %s\nACTUAL_DETAIL %s\nEXPECTED_DETAIL %s\nACTUAL_HOST %s\nEXPECTED_HOST %s\n",actual.domains.c_str(),expected.domains.c_str(),actual.detail.c_str(),expected.detail.c_str(),actual.host_detail.c_str(),expected.host_detail.c_str());}check(ok,name);}

static bool tick(){if(app->loading_waiting()&&!app->complete_loading())return false;unsigned simulated=0;return mp_tick_spectator(60.f,simulated);}
static Netplay::SpectatorStageEvent event(const multiplayer::SessionSetup&s,u32 frame,u32 stage,u32 serial,const std::vector<u8>&bytes){Netplay::SpectatorStageEvent e;e.game=multiplayer::SpectatorJournal::kGame;e.playerCount=s.player_count;e.sessionId=s.session_id;e.gameplayAbi=multiplayer::GameplayAbi(s);e.frame=frame;e.stage=stage;e.serial=serial;e.schema=multiplayer::SpectatorStageSchema;e.state=bytes;return e;}
struct Tape{std::vector<State> expected;std::array<std::vector<u8>,2> entries;std::vector<Netplay::SpectatorFramePacket> inputs;};
static Tape record(unsigned count,unsigned rank){
    auto s=setup(count,rank);auto words=multiplayer::EncodeSessionSetup(s);auto&session=multiplayer::session;session.Stop();check(session.Configure(words.data(),words.size()),"configure actual recording Session");
    app=std::make_unique<ApplicationState>();setenv("TH13_SAVE_PATH",("spectator-"+std::to_string(count)+"-"+std::to_string(rank)+".bin").c_str(),1);
    const char*assets=getenv("TH13_NATIVE_ASSET_ROOT");const char*root=getenv("TH13_NATIVE_REPO_ROOT");check(assets&&root&&app->audio_device.initialize_for_test(assets,std::string(root)+"/games/th13/thbgmogg.dat")&&app->initialize(false)&&app->configure_multiplayer(count,{0,0,0},0),"initialize actual recording assets");
    const unsigned stage=rank==4?7:1;app->progress.character=0;app->progress.difficulty=rank;check(app->start_game(stage),"record legal initial stage");app->pending_destination=-1;app->progress.stage=app->progress.starting_stage=app->selected_stage=stage;app->frames=0;app->game_random.reseed_restore(s.seed);app->visual.reseed_restore(s.seed);check(app->begin_run()&&app->complete_loading()&&session.BeginLocalSimulation(),"record actual world begin");
    transition=rank!=4;trigger.enabled=true;trigger.run=clear_event;check(app->scheduler.add(trigger,FramePass::Update,19)>=0,"attach bounded API clear fixture");
    Tape tape;tape.expected.resize(321);tape.inputs.resize(320);unsigned preinputs=0;
    session.spectator_sink=[&](u32 f,const std::array<Netplay::FrameInput,3>&inputs){++preinputs;if(f==0||f==9){multiplayer::StageEntry entry;entry.Capture(app->run->battle,app->progress,app->run->score_view(),app->game_random.seed(),app->game_random.index(),app->visual.seed(),app->visual.index());tape.entries[f==0?0:1]=entry.Encode();}auto&p=tape.inputs[f];p.sessionId=s.session_id;p.gameplayAbi=multiplayer::GameplayAbi(s);p.playerCount=count;p.frame=f;p.inputs=inputs;return true;};
    tape.expected[0]=state();
    for(unsigned f=0;f<320;++f){
        for(unsigned seat=1;seat<count;++seat){Netplay::FrameInput in{};check(multiplayer::InputLanes::Capture(f<165?1u:((f/7+seat)%2?0x41u:0x81u),(f==175||f==190)&&seat==count-1,in),"capture recorded remote input");auto r=session.SubmitRemote(seat,f,in);check(r!=Netplay::RemoteInputResult::InvalidPlayer&&r!=Netplay::RemoteInputResult::ConflictingConfirmedInput&&r!=Netplay::RemoteInputResult::TooOld,"submit actual remote input");}
        const u32 local=f<165?1u:((f/7)%2?0x41u:0x81u);check(session.Pump(f*1000/60)&&session.CaptureLocal(local,f*1000/60),"sample actual local input");session.AdvanceCapturePhase();check(session.CanSimulate()&&app->update(session.MenuInput(),false)&&app->commit(60.f),"execute actual Session frame");session.OnFrameSimulated();check(app->present(),"present actual Session frame");tape.expected[f+1]=state();
    }
    check(preinputs==320&&session.Committed()==320,"recorded history continuous");return tape;
}
static void receive(const multiplayer::SessionSetup&s,const Tape&tape,unsigned frames=320){auto&playback=multiplayer::session.Runtime().StagePlayback();auto root=event(s,0,s.difficulty==4?7:1,1,tape.entries[0]);check(playback.Receive(root),"receive complete run root");for(unsigned f=0;f<frames;++f){if(f==9&&s.difficulty!=4)check(playback.Receive(event(s,9,2,2,tape.entries[1])),"receive intermediate carry as bookmark");check(playback.Receive(tape.inputs[f]),"receive exact saved frame");}auto boot=event(s,frames,s.difficulty==4?7:2,s.difficulty==4?1:2,{});boot.kind=Netplay::SpectatorEventKind::Bootstrap;boot.schema=0;check(playback.Receive(boot),"late join bootstrap full prefix");}
static void drain(const Tape&tape,unsigned target){auto&p=multiplayer::session.Runtime().StagePlayback();unsigned guard=0;while((!p.Initialized()||p.Seeking()||p.Position()<target)&&guard++<500){check(tick(),"production spectator tick");if(p.Initialized())compare(state(),tape.expected[p.Position()],"spectator frontier equals recorded live Session");if(p.Failed())break;}check(p.Position()==target&&!p.Seeking(),"spectator reaches real target");}
int main(){setvbuf(stdout,nullptr,_IOLBF,0);
 for(auto config:{std::array<unsigned,2>{2,1},std::array<unsigned,2>{3,3},std::array<unsigned,2>{2,4}}){const auto count=config[0],rank=config[1];auto s=setup(count,rank);auto tape=record(count,rank);auto words=multiplayer::EncodeSessionSetup(s);
  check(multiplayer::session.Configure(words.data(),words.size()),"configure continuous spectator");
  auto&continuous=multiplayer::session.Runtime().StagePlayback();auto root=event(s,0,rank==4?7:1,1,tape.entries[0]);check(continuous.Receive(root),"continuous viewer receives root");auto boot=root;boot.kind=Netplay::SpectatorEventKind::Bootstrap;boot.schema=0;boot.state.clear();check(continuous.Receive(boot)&&tick(),"continuous viewer installs root before any input");compare(state(),tape.expected[0],"continuous spectator exact preinput0");
  for(unsigned f=0;f<320;++f){if(f==9&&rank!=4)check(continuous.Receive(event(s,9,2,2,tape.entries[1])),"continuous intermediate bookmark");check(continuous.Receive(tape.inputs[f]),"continuous viewer receives saved input");}
  for(unsigned f=1;f<=230;++f){check(!continuous.FastForwarding()&&tick()&&continuous.Position()==f,"continuous spectator consumes exactly one saved input");compare(state(),tape.expected[f],"continuous spectator every frame equals recorded live Session");}
  check(multiplayer::session.Configure(words.data(),words.size()),"configure spectator from actual recorded identity");receive(s,tape);auto&p=multiplayer::session.Runtime().StagePlayback();
  drain(tape,rank==4?0:9);check(p.Control(Netplay::SpectatorStagePlayback::Action::Pause),"pause viewer");const auto held=p.Position();check(tick()&&p.Position()==held,"paused viewer never consumes input");
  for(unsigned target:{165u,177u,220u,0u,9u,100u,203u}){check(p.Control(Netplay::SpectatorStagePlayback::Action::Seek,target),"seek complete recorded prefix");drain(tape,target);check(p.Paused(),"seek preserves viewer pause");}
  check(p.Control(Netplay::SpectatorStagePlayback::Action::Seek,0),"reset before direct cross-transition batch");drain(tape,0);
  check(p.Control(Netplay::SpectatorStagePlayback::Action::Seek,220),"direct seek crosses pending destination and load");drain(tape,220);
  check(p.Control(Netplay::SpectatorStagePlayback::Action::Live),"return viewer to live");drain(tape,230);check(p.Following(),"viewer follows actual buffered live edge");
  check(p.Control(Netplay::SpectatorStagePlayback::Action::Pause)&&p.Control(Netplay::SpectatorStagePlayback::Action::Seek,0),"seek root after gameplay pause and transition");drain(tape,0);
  app->scheduler.remove(trigger);app.reset();multiplayer::session.Stop();
 }
 // Complete, valid wire events with an illegal root must never build a world.
 auto s=setup(2,1);auto tape=record(2,1);auto words=multiplayer::EncodeSessionSetup(s);
 for(unsigned mutation=0;mutation<5;++mutation){check(multiplayer::session.Configure(words.data(),words.size()),"configure malformed root negative");auto&p=multiplayer::session.Runtime().StagePlayback();auto root=event(s,0,1,1,tape.entries[0]);if(mutation==0)root.frame=9;if(mutation==1)root.stage=2;if(mutation==2)root.sessionId++;if(mutation==3){multiplayer::StageEntry e;multiplayer::StageEntry::Decode(root.state.data(),root.state.size(),e);e.seat_count=3;root.state=e.Encode();}if(mutation==4){multiplayer::StageEntry e;multiplayer::StageEntry::Decode(root.state.data(),root.state.size(),e);e.chapter_frame=1;root.state=e.Encode();}check(p.Receive(root),"negative root wire remains valid");auto boot=root;boot.kind=Netplay::SpectatorEventKind::Bootstrap;boot.schema=0;boot.state.clear();check(p.Receive(boot),"negative bootstrap accepted by generic wire controller");check(!tick()&&p.Failed(),"illegal TH13 root rejected at production world boundary");}
 app->scheduler.remove(trigger);app.reset();multiplayer::session.Stop();printf("SUMMARY checks=%u failures=%u scope=recorded-live-session-vs-spectator-full-prefix natural_clear=0 frames=320\n",checks,failures);return failures?1:0;
}
