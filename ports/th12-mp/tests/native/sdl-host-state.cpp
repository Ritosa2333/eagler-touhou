// Real host/core boundary, without booting SDL, loading assets or running ticks.
#include "Archive.hpp"
#include "Audio.hpp"
#include "Ending.hpp"
#include "ExeTables.hpp"
#include "Profile.hpp"
#include "Renderer.hpp"
#include "Replay.hpp"
#include "Scene.hpp"
#include "TextRaster.hpp"
#include "TitlePresentation.hpp"
#include "TitleScenes.hpp"
#include "PracticeOverlay.hpp"
#include "FrameClock.hpp"
#include <atomic>
#include <deque>
#include <functional>
#include <optional>
#include <thread>
// Dependencies are parsed normally; expose only Game's fields to seed states.
#define private public
#include "Game.hpp"
#undef private
#include "game/GameState.hpp"
#ifdef TH12_MULTIPLAYER
#include "game/Multiplayer.hpp"
#endif
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <filesystem>
#include "game/Practice.hpp"
#include "game/PracticeTools.hpp"
#include "game/StageFlow.hpp"
#include <chrono>
#include <string>
extern "C" const double* th12_host_state_values();
namespace {
using namespace th12host;
void require(bool ok,const char* field){if(!ok){std::fprintf(stderr,"host state mismatch: %s\n",field);std::exit(1);}}
const char* text(double p){return reinterpret_cast<const char*>(uintptr_t(p));}
std::string field(const std::string& json,const char* name){
    std::smatch result;require(std::regex_search(json,result,std::regex("\\\""+std::string(name)+"\\\":(\\\"[^\\\"]*\\\"|[^,}]+)")),name);
    return result[1];
}
void compare(Game& game){
    const auto* ptr=game.host_state_values();std::array<double,HostStateLength> v;
    std::copy_n(ptr,v.size(),v.begin());
    require(v[0]==1&&v[1]==27,"version/length");
    require(uintptr_t(ptr)%alignof(double)==0,"alignment");
    // Collection returns stable object-owned storage, not a temporary.
    require(game.host_state_values()==ptr,"storage");
    const std::string scene=text(v[HostScene]),kind=text(v[HostKind]),track=text(v[HostTrack]);
    const auto json=game.host_state();
    require(field(json,"scene")=="\""+scene+"\"","scene");
    require(field(json,"kind")=="\""+kind+"\"","kind");
    require(field(json,"track")=="\""+track+"\"","track");
    const std::pair<const char*,HostStateField> numbers[]{
        {"fps",HostFps},{"menuSerial",HostMenuSerial},{"menuAge",HostMenuAge},{"selection",HostSelection},
        {"stage",HostStage},{"character",HostCharacter},{"shot",HostShot},{"cursor",HostCursor},
        {"x",HostX},{"y",HostY},{"fast",HostFast},{"slow",HostSlow},{"captureSerial",HostCaptureSerial},
        {"drawCalls",HostDrawCalls},{"vertices",HostVertices},{"practiceVertices",HostPracticeVertices}};
    for(auto [name,index]:numbers){
        const double actual=std::stod(field(json,name));
        const bool floating=index==HostFps||index==HostFast||index==HostSlow;
        require(floating?std::abs(actual-v[index])<=std::max(1.,std::abs(v[index]))*0.000005:actual==v[index],name);
    }
    const std::pair<const char*,HostStateField> booleans[]{{"ready",HostReady},{"menuReady",HostMenuReady},{"paused",HostPaused},{"capturing",HostCapturing},{"seeking",HostSeeking}};
    for(auto [name,index]:booleans)require(field(json,name)==(v[index]?"true":"false"),name);
    require(field(json,"instance")=="\""+std::to_string(int(v[HostStage]))+":"+std::to_string(int(v[HostInstance]))+"\"","instance");
    require(field(json,"gestureScope")=="\""+std::to_string(int(v[HostStage]))+"\"","gestureScope");
}
}
#ifndef TH12_MULTIPLAYER
void practice_replay_flow(){
    using namespace th12host;
    const auto folder=std::filesystem::path("th12-practice-save-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(folder);
    Game game;game.profile.load(folder.string());
    game.menus=std::make_unique<TitleScenes>(game.profile,std::vector<MusicTrackInfo>{},folder.string());
    game.session.emplace();auto& session=*game.session;session.run.mode=ModePractice;session.run.stage=session.stage=4;
    game.menus->run=session.run;
    session.recorder=std::make_unique<ReplayRecorder>(0,0,1,true,std::array<u8,60>{});
    session.recorder->begin_stage(4,123,{});
    session.recorder->tools({int(th12::practice::Invulnerable|th12::practice::InfiniteBombs),0,0x3f000000});session.recorder->push(1,1,0,60);
    th12::fullGameFlow.continueCount=0;
    game.complete_run(true);
    require(game.menus->state.scene==MenuScene::ResultMenu,"Practice clear result menu");
    require(!game.menus->state.choices[1].disabled,"Practice Save Replay enabled");
    for(int i=0;i<11;++i)game.menus->tick(0);
    game.menus->state.selection=1;game.menus->tick(InShot);
    require(game.menus->state.scene==MenuScene::ReplaySave,"Practice replay slots");
    game.menus->tick(0);for(int i=0;i<11;++i)game.menus->tick(0);
    game.menus->tick(InShot);
    require(game.menus->state.scene==MenuScene::Name,"Practice replay name");
    game.menus->tick(0);for(int i=0;i<11;++i)game.menus->tick(0);
    game.menus->tick(InShot);game.actions();
    std::vector<u8> data;Replay replay;std::string why;
    require(host::read_file((folder/"th12_01.rpy").string(),data,1024*1024),"Practice replay file written");
    require(parse_replay(data,replay,why),"Practice replay parses");
    require(replay.info.practice&&replay.stage(4)&&replay.input(4,1)->end(),"Practice identity, stage and end marker");
    require(replay.extension.tool_state(4,0)[0]==int(th12::practice::Invulnerable|th12::practice::InfiniteBombs),"Saved practice retains auxiliary tools");
    const auto previous_name=game.profile.last_name();
    game.menus=std::make_unique<TitleScenes>(game.profile,std::vector<MusicTrackInfo>{},(folder/"th12_01.rpy"/"invalid").string());
    MenuContext result;game.save_replay(1,"FAILED",result);
    require(game.error=="Replay could not be saved"&&game.profile.last_name()==previous_name,"Failed replay write is reported without committing name");
    for(const auto* name:{"th12_01.rpy","th12.cfg","scoreth12.dat"})std::filesystem::remove(folder/name);
    std::filesystem::remove(folder);
}
#endif
void replay_tools(){
    using namespace th12::practice;
    setEnabled(true);reset(true);setTools(Invulnerable|InfiniteBombs);
    toolSettings().force_boss_move_down=true;toolSettings().boss_move_down_range=.5f;
    const auto saved=replayToolState();setReplay(true);
    require(!tool(Invulnerable),"Live tools do not leak into original replays");
    applyReplayToolState({int(InfiniteLives|InfinitePower|TimeLock),3,0x3e800000});
    require(tool(InfiniteLives)&&tool(InfinitePower)&&tool(TimeLock)&&!tool(Invulnerable),"Recorded tools active during replay");
    float center=100,range=80;bossMoveRange(center,range);
    require(center==110&&range==60,"Recorded boss movement range applied during replay");
    require(toolSettings().map_inf_life_to_no_continue,"Recorded infinite-life behavior");
    applyReplayToolState({0,0,0x3f000000});require(!tool(TimeLock),"Mid-replay tool disable");
    setReplay(false);require(replayToolState()==saved,"Live settings restored after replay");
    reset(true);setEnabled(false);
}
void pause_transitions(){
    using namespace th12host;
    Profile profile;profile.load("__pause_no_profile__");TitleScenes menu(profile,{},"__pause_no_replays__");
    MenuContext context;context.run.mode=ModePractice;context.can_save_replay=true;
    menu.open_pause(context);menu.drain();for(int i=0;i<11;++i)menu.tick(0);
    menu.state.selection=1;menu.tick(InShot);menu.drain();for(int i=0;i<30;++i)menu.tick(0);
    menu.state.selection=0;menu.tick(InShot);
    auto actions=menu.drain();require(actions.size()==1&&actions[0].type==MenuAction::Sound&&actions[0].id==7,"Yes confirmation sound retained");
    require(menu.state.scene==MenuScene::PauseConfirm&&menu.state.phase==MenuState::Phase::Out,"Yes animates before title");
    for(int i=0;i<19;++i)menu.tick(0);require(!menu.has_actions(),"Yes waits twenty ticks");
    menu.tick(0);require(menu.state.scene==MenuScene::Title,"Yes returns after twenty ticks");
    menu.open_pause(context);menu.drain();for(int i=0;i<11;++i)menu.tick(0);
    menu.tick(InQuit);actions=menu.drain();require(actions.size()==1&&actions[0].type==MenuAction::Sound&&actions[0].id==7,"Q confirmation sound");
    for(int i=0;i<11;++i)menu.tick(0);require(!menu.has_actions(),"Q waits twelve ticks");
    menu.tick(0);require(menu.state.scene==MenuScene::Title,"Q returns after twelve ticks");
}
int main(){
    replay_tools();pause_transitions();
    static_assert(HostVersion==0&&HostLength==1&&HostScene==2&&HostKind==3&&HostReady==4
        &&HostFps==5&&HostMenuSerial==6&&HostMenuReady==7&&HostMenuAge==8&&HostSelection==9
        &&HostStage==10&&HostCharacter==11&&HostShot==12&&HostCursor==13&&HostInstance==14
        &&HostX==15&&HostY==16&&HostFast==17&&HostSlow==18&&HostPaused==19
        &&HostCaptureSerial==20&&HostDrawCalls==21&&HostVertices==22&&HostPracticeVertices==23&&HostCapturing==24
        &&HostTrack==25&&HostSeeking==26&&HostStateLength==27);
    const auto* closed=th12_host_state_values();require(closed[0]==1&&closed[1]==27,"closed header");
    require(std::strcmp(text(closed[HostScene]),"closed")==0,"closed scene");
    require(std::strcmp(text(closed[HostKind]),"menu")==0,"closed kind");
    require(std::strcmp(text(closed[HostTrack]),"")==0,"closed track");
    {
    Game game;compare(game);
    #ifdef TH12_MULTIPLAYER
    th12::multiplayer::pilot().px=-12345;th12::multiplayer::pilot().py=55296;
#else
    th12::px=-12345;th12::py=55296;
#endif
    th12::phasePlayerCounters[0]=1234567;
    game.gpu.draws=1234567;game.gpu.vertices=7654321;compare(game);
    const auto* v=game.host_state_values();require(v[HostX]==-12345&&v[HostY]==55296,"fixed coordinates");
    require(v[HostInstance]==1234567&&v[HostVertices]==7654321,"integer precision");
    require(field(game.host_state(),"vertices")=="7654321","JSON integer precision");
    game.profile.load("__host_state_no_profile__");
    game.menus=std::make_unique<TitleScenes>(game.profile,std::vector<MusicTrackInfo>{},"__host_state_no_replays__");
    game.phase=Game::Phase::Menu;game.menus->state.age=1234567;game.menus->state.serial=2345678;
    game.menus->state.selection=5;game.menus->state.ready_at=7;game.fps_value=143.125f;
    game.capture_serial=3456789;compare(game);
    v=game.host_state_values();require(v[HostMenuReady]==1&&v[HostSelection]==5&&v[HostFps]==143.125,"menu values");
    game.session.emplace();auto& session=*game.session;
    session.stage=6;session.run.character=2;session.run.shot=1;session.cursor=1234567;
    game.menus->state.scene=MenuScene::Playing;game.game_fps=59.375f;
#ifdef TH12_MULTIPLAYER
    th12::multiplayer::pilot().playerState=1;
#else
    th12::playerState=1;
#endif
    th12::messageVM.active=false;compare(game);
    v=game.host_state_values();require(v[HostReady]==1&&std::strcmp(text(v[HostKind]),"game")==0,"playing");
    require(v[HostStage]==6&&v[HostCharacter]==2&&v[HostShot]==1&&v[HostCursor]==1234567&&v[HostFps]==59.375,"session");
    th12::messageVM.active=true;th12::messageVM.flags=1;compare(game);
    require(std::strcmp(text(game.host_state_values()[HostKind]),"dialogue")==0,"dialogue");
    session.replay=std::make_shared<Replay>();compare(game);
    require(std::strcmp(text(game.host_state_values()[HostKind]),"replay")==0,"replay");
    game.seek_target=9876;game.external_paused=true;compare(game);
    v=game.host_state_values();require(v[HostSeeking]==1&&v[HostPaused]==1,"external pause/seek");
    session.replay.reset();game.external_paused=false;game.seek_target=-1;
    for(const auto scene:{MenuScene::Pause,MenuScene::PauseConfirm}){
        game.menus->state.scene=scene;compare(game);require(game.host_state_values()[HostPaused]==1,"menu pause");
    }
    game.phase=Game::Phase::Loading;compare(game);
    require(std::strcmp(text(game.host_state_values()[HostScene]),"loading")==0,"loading");
#ifdef TH12_MULTIPLAYER
    game.phase=Game::Phase::Menu;game.menus->state.scene=MenuScene::Playing;
    game.mp_playing=true;game.mp_local=1;th12::multiplayer::options.count=2;
    th12::multiplayer::pilots[1].px=-9876;th12::multiplayer::pilots[1].py=43210;
    compare(game);v=game.host_state_values();require(v[HostX]==-9876&&v[HostY]==43210,"local seat coordinates");
    game.mp_replay=true;compare(game);
    require(std::strcmp(text(game.host_state_values()[HostKind]),"replay")==0,"MP replay");
#endif
    }
    // The closed result belongs to Application, not the destroyed Game.
    require(th12_host_state_values()==closed,"closed stable storage");
    require(std::strcmp(text(closed[HostScene]),"closed")==0,"closed lifetime");
    for(size_t i=HostReady;i<HostStateLength;++i)if(i!=HostTrack)require(closed[i]==0,"closed defaults");
    std::puts("host-state ABI v1: all 27 fields, menu/game/replay/dialogue/pause/loading and closed lifetime agree; no game ticks");
}
