#ifdef __EMSCRIPTEN__
#include "ReplayManager.hpp"
#include "ReplayExtension.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "MainMenu.hpp"
#include "PracticeRuntime.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include "../third_party/eagler-common/include/eagler/replay/Workspace.hpp"
#include "../third_party/eagler-common/include/eagler/replay/Presentation.hpp"
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
ReplayFile* parse(const u8* bytes,unsigned size){auto* copy=static_cast<ReplayFile*>(std::malloc(size));if(!copy)return nullptr;std::memcpy(copy,bytes,size);return ReplayManager::ValidateReplayData(copy,size);}
unsigned length(const ReplayFile& r,unsigned stage){const auto begin=r.head.stageReplayDataOffsets[stage];if(!begin)return 0;unsigned end=r.head.sizeWithoutHeader+sizeof(ReplayHeader);
    for(unsigned n=0;n<7;n++){if(r.head.stageReplayDataOffsets[n]>begin)end=std::min(end,r.head.stageReplayDataOffsets[n]);if(r.head.stageEndDataOffsets[n]>begin)end=std::min(end,r.head.stageEndDataOffsets[n]);}
    const unsigned header=offsetof(StageReplayData,replayInputs);return end>begin+header+4?(end-begin-header)/4-1:0;
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void* eagler_replay_allocate(unsigned n){return std::malloc(n);}
EMSCRIPTEN_KEEPALIVE void eagler_replay_free(void* p){std::free(p);}
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_inspect(const u8* bytes,unsigned size){
    auto* r=parse(bytes,size);if(!r)return nullptr;const auto& h=r->data;eagler::replay::Description d;
    d.encoded_name(h.name,12);d.text("date",std::string(h.date,6));d.number("character",h.shotType/2);d.number("shot",h.shotType%2);d.number("difficulty",h.difficulty);d.number("score",double(h.score)*10);d.number("slowdown",h.slowdownRate);
    for(unsigned n=0;n<7;n++)if(const auto* st=r->stageReplayData[n]){const auto id=n==6&&h.difficulty>=5?8:n+1;d.stage(id,double(st->score)*10,length(*r,n),n==6?(h.difficulty>=5?"PHANTASM":"EXTRA"):"STAGE "+std::to_string(n+1));}
    ReplayManager::FreeReplay(r);
    description=d.finish();return description.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_error(){return local_error.c_str();}
EMSCRIPTEN_KEEPALIVE void eagler_replay_stop_loop(){g_ReplayWorkspaceDriving=true;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_ready(){return g_ReplayWorkspaceDriving||g_MainMenuForDebug!=nullptr;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_preparing(){return !started;}
EMSCRIPTEN_KEEPALIVE int eagler_replay_open(const char* path,unsigned stage){
    if(!g_AnmManager||stage<1||stage>8)return reject("Game initialization or stage selection is not ready");size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path,&size));if(!bytes)return reject("Selected file could not be opened");
    std::vector<u8> raw(bytes,bytes+size);SDL_free(bytes);auto* r=parse(raw.data(),unsigned(size));if(!r)return 0;
    const auto h=r->data;const unsigned index=std::min(stage-1,6u);if(!r->stageReplayData[index]){ReplayManager::FreeReplay(r);return 0;}
    const char* file=std::strstr(path,"/replay/");if(!file||std::strlen(path)>=sizeof(g_GameManager.replayFilename)){ReplayManager::FreeReplay(r);return 0;}
    for(unsigned n=0;n<7;n++)lengths[n]=length(*r,n);ReplayManager::FreeReplay(r);
    if(g_GameManager.globals)GameManager::CutChain();ReplayManager::SaveReplay(nullptr,nullptr);
    if(g_MainMenuForDebug&&g_MainMenuForDebug->calcChain)g_Chain.Cut(g_MainMenuForDebug->calcChain);
    ReplayExtension::LoadPlayback(raw.data(),raw.size());
    PracticeRuntime::ReplayMenuReset();PracticeRuntime::ReplayMenuCheck(path);PracticeRuntime::ReplayMenuActivate();
    g_GameManager.demo=0;g_GameManager.demoFrames=0;g_GameManager.SetIsReplay(1);std::strcpy(g_GameManager.replayFilename,path);
    g_GameManager.character=h.shotType/2;g_GameManager.shotType=h.shotType%2;g_GameManager.difficulty=h.difficulty;g_GameManager.shotTypeAndCharacter=h.shotType;g_GameManager.replayStage=0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ReplayExtension::MultiplayerReplayConfig config;
    if(ReplayExtension::GetMultiplayerPlaybackConfig(&config)){
        if(config.gameplayAbi!=TH07_MULTI_GAMEPLAY_ABI)return 0;MultiplayerGameplay::SessionState session;session.playerCount=config.playerCount;session.localPlayer=config.localPlayer;session.showStagePlayerNames=config.showStagePlayerNames;session.stage4BossChain=config.stage4BossChain;session.showContributionStats=config.showContributionStats;session.challengeMode=config.challengeMode;
        for(unsigned n=0;n<config.playerCount;n++){session.players[n].active=true;session.players[n].character=config.characters[n];session.players[n].shot=config.shots[n];}
        if(!MultiplayerGameplay::Configure(session))return 0;
    }else MultiplayerGameplay::ResetToSinglePlayer(u8(g_GameManager.character),u8(g_GameManager.shotType));
#endif
    g_GameManager.currentStage=stage-1;g_Supervisor.wantedState=SUPERVISOR_STATE_MAINMENU;g_Supervisor.curState=SUPERVISOR_STATE_GAMEMANAGER;start_stage=stage;started=false;return 1;
}
EMSCRIPTEN_KEEPALIVE const unsigned* eagler_replay_state(){
    static unsigned v[5];v[0]=v[1]=0;const auto stage=unsigned(std::max(1,g_GameManager.currentStage));
    for(unsigned n=std::min(start_stage,7u);n<=7;n++){v[1]+=lengths[n-1];if(n<std::min(stage,7u))v[0]+=lengths[n-1];}
    if(started&&g_ReplayManager)v[0]+=std::max(0,g_ReplayManager->frameId);
    // A partial recording ends at its native input boundary, even while its
    // gameplay stage remains active. Do not consume beyond the saved block.
    v[2]=started&&(!g_ReplayManager||(v[1]&&v[0]>=v[1]));if(v[2])v[0]=v[1];v[3]=started?stage:start_stage;v[4]=start_stage;return v;
}
EMSCRIPTEN_KEEPALIVE void eagler_replay_present(){if(g_Supervisor.gfxDevice)g_Supervisor.gfxDevice->SwapBuffers();}
EMSCRIPTEN_KEEPALIVE int eagler_replay_step(unsigned present){
    const eagler::replay::DeferPresentation presentation(!present);
    g_GameWindow.isAppActive=1;g_GameWindow.accumulator=1./60.;g_LastPerfCounter=SDL_GetPerformanceCounter();
    const auto result=g_GameWindow.Render();
    if(g_ReplayManager&&!g_ReplayManager->data){local_error="Native Replay file load failed: "+std::string(g_GameManager.replayFilename);return 0;}
    if(g_ReplayManager&&unsigned(g_GameManager.currentStage)>=start_stage&&g_Supervisor.wantedState==g_Supervisor.curState)started=true;return result==RENDER_RESULT_KEEP_RUNNING;
}
}
#endif
