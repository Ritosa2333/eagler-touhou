#ifdef __EMSCRIPTEN__
#include "ReplayManager.hpp"
#include "ReplayExtension.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "MainMenu.hpp"
#include "PracticeRuntime.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include <eagler/replay/Workspace.hpp>
#include <eagler/replay/Presentation.hpp>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#include <emscripten.h>
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstring>
#include <vector>
bool g_ReplayWorkspaceDriving=false;
namespace {
std::string description,local_error;
int reject(const char* message){local_error=message;return 0;}unsigned start_stage=1,lengths[7]{};bool started=false;
bool parse(const u8* bytes,unsigned size,std::vector<u8>& out){out.assign(bytes,bytes+size);return ReplayManager::ValidateReplayData(reinterpret_cast<ReplayHeader*>(out.data()),size)==ZUN_SUCCESS;}
unsigned length(const ReplayHeader& h,const std::vector<u8>& bytes,unsigned stage){
    const auto offset=h.stageReplayDataOffsets[stage];if(!offset)return 0;
    const auto* input=reinterpret_cast<const ReplayDataInput*>(bytes.data()+offset+offsetof(StageReplayData,replayInputs));unsigned last=0;
    while(reinterpret_cast<const u8*>(input+1)<=bytes.data()+bytes.size()){if(input->frameNum==9999999)return last;last=input->frameNum;++input;}return 0;
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void* eagler_replay_allocate(unsigned n){return std::malloc(n);}
EMSCRIPTEN_KEEPALIVE void eagler_replay_free(void* p){std::free(p);}
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_inspect(const u8* bytes,unsigned size){
    std::vector<u8> decoded;if(!parse(bytes,size,decoded))return nullptr;const auto& h=*reinterpret_cast<const ReplayHeader*>(decoded.data());eagler::replay::Description d;
    d.encoded_name(h.name,8);d.text("date",std::string(h.date,strnlen(h.date,9)));d.number("character",h.shottypeChara/2);d.number("shot",h.shottypeChara%2);d.number("difficulty",h.difficulty);d.number("score",h.score);d.number("slowdown",h.slowdownRate);
    for(unsigned n=0;n<7;n++)if(h.stageReplayDataOffsets[n]){const auto* st=reinterpret_cast<const StageReplayData*>(decoded.data()+h.stageReplayDataOffsets[n]);d.stage(n+1,st->score,length(h,decoded,n),n==6?"EXTRA":"STAGE "+std::to_string(n+1));}
    description=d.finish();return description.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_error(){return local_error.c_str();}
EMSCRIPTEN_KEEPALIVE void eagler_replay_stop_loop(){g_ReplayWorkspaceDriving=true;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_ready(){return g_ReplayWorkspaceDriving||g_MainMenu.chainCalc!=nullptr;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_preparing(){return !started;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_open(const char* path,unsigned stage){
    if(!g_AnmManager||stage<1||stage>7)return reject("Game initialization or stage selection is not ready");size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path,&size));if(!bytes)return reject("Selected file could not be opened");
    std::vector<u8> raw(bytes,bytes+size),decoded;SDL_free(bytes);if(!parse(raw.data(),unsigned(size),decoded))return reject("Native Replay validation failed");
    const auto& h=*reinterpret_cast<const ReplayHeader*>(decoded.data());if(!h.stageReplayDataOffsets[stage-1])return reject("Selected stage is absent");
    const char* file=std::strstr(path,"/replay/");if(!file||std::strlen(path)>=sizeof(g_GameManager.replayFile))return reject("Selected file path is invalid");
    GameManager::CutChain();ReplayManager::SaveReplay(nullptr,nullptr);
    if(g_MainMenu.chainCalc){g_Chain.Cut(g_MainMenu.chainCalc);g_MainMenu.chainCalc=nullptr;}
    ReplayExtension::LoadPlayback(raw.data(),raw.size());
    PracticeRuntime::ReplayMenuReset();PracticeRuntime::ReplayMenuCheck(path);PracticeRuntime::ReplayMenuActivate();
    g_GameManager.demoMode=0;g_GameManager.demoFrames=0;g_GameManager.isInReplay=1;std::strcpy(reinterpret_cast<char*>(g_GameManager.replayFile),path);
    g_GameManager.character=h.shottypeChara/2;g_GameManager.shotType=h.shottypeChara%2;g_GameManager.difficulty=static_cast<Difficulty>(h.difficulty);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ReplayExtension::MultiplayerReplayConfig config;
    if(ReplayExtension::GetMultiplayerPlaybackConfig(&config)){
        if(config.gameplayAbi!=TH06_MULTI_GAMEPLAY_ABI)return 0;MultiplayerGameplay::SessionState session;session.playerCount=config.playerCount;session.localPlayer=config.localPlayer;session.showStagePlayerNames=true;session.showContributionStats=config.showContributionStats;session.challengeMode=config.challengeMode;
        for(unsigned n=0;n<config.playerCount;n++){session.players[n].active=true;session.players[n].character=config.characters[n];session.players[n].shot=config.shots[n];}
        if(!MultiplayerGameplay::Configure(session))return 0;
    }else MultiplayerGameplay::ResetToSinglePlayer(u8(g_GameManager.character),u8(g_GameManager.shotType));
#endif
    for(unsigned n=0;n<7;n++)lengths[n]=length(h,decoded,n);
    g_GameManager.currentStage=stage-1;g_Supervisor.wantedState=SUPERVISOR_STATE_MAINMENU;g_Supervisor.curState=SUPERVISOR_STATE_GAMEMANAGER;start_stage=stage;started=false;return 1;
}
EMSCRIPTEN_KEEPALIVE const unsigned* eagler_replay_state(){
    static unsigned v[5];v[0]=v[1]=0;const auto stage=unsigned(std::max(1,g_GameManager.currentStage));
    for(unsigned n=start_stage;n<=7;n++){v[1]+=lengths[n-1];if(n<stage)v[0]+=lengths[n-1];}
    if(g_ReplayManager)v[0]+=std::max(0,g_ReplayManager->frameId);
    // The native input terminator can precede stage clear (for example a saved
    // partial MP run). Stop at its recorded boundary instead of holding the
    // final input until the world eventually returns to the menu.
    v[2]=started&&(!g_ReplayManager||(v[1]&&v[0]>=v[1]));if(v[2])v[0]=v[1];v[3]=stage;v[4]=start_stage;return v;
}
EMSCRIPTEN_KEEPALIVE void eagler_replay_present(){if(g_GfxBackend)g_GfxBackend->SwapBuffers();}
EMSCRIPTEN_KEEPALIVE int eagler_replay_step(unsigned present){
    const eagler::replay::DeferPresentation presentation(!present);
    g_GameWindow.isAppActive=1;g_GameWindow.accumulator=1./60.;g_GameWindow.lastPerformanceCounter=SDL_GetPerformanceCounter();
    const auto result=g_GameWindow.Render();if(g_ReplayManager&&(!g_ReplayManager->replayData||!g_ReplayManager->replayInputs))return reject("Native Replay input could not be loaded");if(g_ReplayManager)started=true;return result==RENDER_RESULT_KEEP_RUNNING;
}
}
#endif
