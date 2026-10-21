#pragma once
#if defined(TH_ENABLE_NETPLAY) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
#include "GameManager.hpp"
#include "Player.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Rng.hpp"
#include "Supervisor.hpp"
#include "multiplayer/GameplaySession.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <array>

// A streaming Replay uses the same stage-entry values as ReplayManager.
// Inputs remain a separate, append-only confirmed stream.
namespace SpectatorReplayStart {
inline constexpr u32 Schema=1;
struct NativeStage { u32 score=0; i32 pointItemsCollectedForExtend=0,cherry=0,cherryMax=0,cherryPlus=0,grazeInTotal=0,extendsFromPointItems=0,nextNeededPointItemsForExtend=0; i16 stageRngSeed=0; u8 currentPower=0,livesRemaining=0,bombsRemaining=0,rank=0,powerItemCountForScore=0,spellCardsCaptured=0; };
struct Stage {
    u32 number=0,difficulty=0;
    u8 lifeCount=0,bombCount=0;
    NativeStage native;
    u32 leadInFrames=0;
    std::array<std::array<Netplay::FrameInput,3>,4> leadIn{};
    MultiplayerGameplay::SessionState setup;
    std::array<ReplayExtension::MultiplayerPlayerResourceSnapshot,3> resources{};
    std::array<ReplayExtension::MultiplayerContributionSnapshot,3> contributions{};
};
inline std::array<std::vector<u8>,8> recorded;
inline Stage prepared;
inline bool applying=false,applied=false;
template<class C> bool Values(C& c,Stage& s){
    auto& n=s.native;auto& setup=s.setup;
    if(!c.Values(s.number,s.difficulty,s.lifeCount,s.bombCount,n.score,n.pointItemsCollectedForExtend,n.cherry,n.cherryMax,n.cherryPlus,n.grazeInTotal,n.extendsFromPointItems,n.nextNeededPointItemsForExtend,n.stageRngSeed,n.currentPower,n.livesRemaining,n.bombsRemaining,n.rank,n.powerItemCountForScore,n.spellCardsCaptured)||
       !c.Values(setup.playerCount,setup.localPlayer,setup.showStagePlayerNames,setup.challengeMode,
                 setup.showContributionStats,setup.stage4BossChain))return false;
    for(auto& p:setup.players)if(!c.Values(p.active,p.temporarilyAbsent,p.permanentlyDeparted,p.character,p.shot))return false;
    for(auto& r:s.resources)if(!c.Values(r.lives,r.bombs,r.power,r.challengeDeaths))return false;
    for(auto& r:s.contributions)if(!c.Values(r.enemiesDefeated,r.damageDealt))return false;
    if(!c.Value(s.leadInFrames)||s.leadInFrames>s.leadIn.size())return false;
    for(u32 f=0;f<s.leadInFrames;++f)for(auto& p:s.leadIn[f])
        if(!c.Values(p.buttons,p.analogMode,p.x,p.y,p.unlimited,p.touchUsed,p.touchBomb))return false;
    return true;
}
inline bool Valid(const Stage& s){
    if(s.lifeCount>8||s.bombCount>8||s.number<1||s.number>8||s.difficulty>5||s.setup.playerCount<2||s.setup.playerCount>3||
       s.setup.localPlayer>=s.setup.playerCount)return false;
    for(unsigned i=0;i<s.setup.playerCount;++i)
        if(s.setup.players[i].character>2||s.setup.players[i].shot>1||
           s.resources[i].lives < -1||s.resources[i].lives>8||s.resources[i].bombs < 0||
           s.resources[i].bombs>8||s.resources[i].power<0||s.resources[i].power>128)return false;
    return true;
}
inline void Record(const StageReplayData& native){
    if(applying||!MultiplayerGameplay::IsMultiplayer())return;
    Stage s;s.number=g_GameManager.currentStage;s.difficulty=g_GameManager.difficulty;
    s.lifeCount=g_GameManager.defaultCfg->lifeCount;s.bombCount=g_GameManager.defaultCfg->bombCount;
    s.native.score=g_GameManager.globals->score;
    s.native.pointItemsCollectedForExtend=native.pointItemsCollectedForExtend;
    s.native.cherry=native.cherry;
    s.native.cherryMax=native.cherryMax;
    s.native.cherryPlus=native.cherryPlus;
    s.native.grazeInTotal=native.grazeInTotal;
    s.native.extendsFromPointItems=native.extendsFromPointItems;
    s.native.nextNeededPointItemsForExtend=native.nextNeededPointItemsForExtend;
    s.native.stageRngSeed=native.stageRngSeed;
    s.native.currentPower=native.currentPower;
    s.native.livesRemaining=native.livesRemaining;
    s.native.bombsRemaining=native.bombsRemaining;
    s.native.rank=native.rank;
    s.native.powerItemCountForScore=native.powerItemCountForScore;
    s.native.spellCardsCaptured=native.spellCardsCaptured;
    s.setup=MultiplayerGameplay::GetState();
    for(unsigned i=0;i<s.setup.playerCount;++i){
        s.resources[i]={GetPlayerLives(i),GetPlayerBombs(i),GetPlayerPower(i),g_MultiplayerContributionStats[i].challengeDeaths};
        s.contributions[i]={g_MultiplayerContributionStats[i].enemiesDefeated,g_MultiplayerContributionStats[i].damageDealt};
    }
    if(!Valid(s))return;
    Netplay::SpectatorScalars::Writer writer;
    if(Values(writer,s)&&writer.Good())recorded[s.number-1]=writer.Bytes();
}
inline bool Encode(std::vector<u8>* output){
    const auto stage=g_GameManager.currentStage;
    if(!output||stage<1||stage>8||recorded[stage-1].empty())return false;
    Stage entry;Netplay::SpectatorScalars::Reader reader(recorded[stage-1]);
    if(!Values(reader,entry)||!reader.Done()||!g_ReplayManager||g_ReplayManager->frameId<0||
       g_ReplayManager->frameId>static_cast<i32>(entry.leadIn.size()))return false;
    entry.leadInFrames=static_cast<u32>(g_ReplayManager->frameId);
    for(u32 f=0;f<entry.leadInFrames;++f)
        if(!ReplayExtension::GetMultiplayerRecordedFrame(stage-1,f,entry.leadIn[f].data(),3))return false;
    Netplay::SpectatorScalars::Writer writer;
    if(!Values(writer,entry)||!writer.Good())return false;
    *output=writer.Bytes();return true;
}
inline bool Prepare(const std::vector<u8>& bytes,u32 stage,u8 seats){
    Stage next;Netplay::SpectatorScalars::Reader reader(bytes);
    if(!Values(reader,next)||!reader.Done()||!Valid(next)||next.number!=stage||next.setup.playerCount!=seats)return false;
    prepared=next;applying=true;applied=false;return true;
}
inline bool Apply(){
    if(!applying||applied||!MultiplayerGameplay::Configure(prepared.setup))return false;
    const auto& n=prepared.native;
    g_GameManager.defaultCfg->lifeCount=prepared.lifeCount;g_GameManager.defaultCfg->bombCount=prepared.bombCount;
    g_GameManager.character=prepared.setup.players[0].character;
    g_GameManager.shotType=prepared.setup.players[0].shot;
    g_GameManager.shotTypeAndCharacter=g_GameManager.character*2+g_GameManager.shotType;
    g_GameManager.difficulty=prepared.difficulty;
    auto& g=*g_GameManager.globals;
    g.pointItemsCollectedForExtend=n.pointItemsCollectedForExtend;
    g_GameManager.rank.rank=n.rank;
    g_GameManager.SetLivesRemaining(n.livesRemaining);
    g_GameManager.SetBombsRemainingAndComputeCsum(n.bombsRemaining);
    g_GameManager.SetCurrentPower(n.currentPower);
    g.grazeInTotal=n.grazeInTotal;
    g_GameManager.powerItemCountForScore=n.powerItemCountForScore;
    g_GameManager.cherry=n.cherry+g.cherryStart;
    g_GameManager.cherryMax=n.cherryMax+g.cherryStart;
    g_GameManager.cherryPlus=n.cherryPlus+g.cherryStart;
    if(g_GameManager.cherryPlus>=g.cherryStart+GetSharedBorderThreshold()){
        g_GameManager.cherryPlus=g.cherryStart+GetSharedBorderThreshold();g_Player.ActivateBorder();
    }
    g.spellCardsCaptured=n.spellCardsCaptured;
    g.extendsFromPointItems=n.extendsFromPointItems;
    g.nextNeededPointItemsForExtend=n.nextNeededPointItemsForExtend;
    g.score=g.guiScore=n.score;
    g_GameManager.RegenerateGameIntegrityCsum();
    for(unsigned i=0;i<prepared.setup.playerCount;++i){
        const auto& r=prepared.resources[i];SetPlayerLives(i,r.lives);SetPlayerBombs(i,r.bombs);SetPlayerPower(i,r.power);
        g_MultiplayerContributionStats[i].challengeDeaths=r.challengeDeaths;
        g_MultiplayerContributionStats[i].enemiesDefeated=prepared.contributions[i].enemiesDefeated;
        g_MultiplayerContributionStats[i].damageDealt=prepared.contributions[i].damageDealt;
    }
    g_Rng.SetSeed(n.stageRngSeed);
    applied=true;return true;
}
inline ChainElem stream_rng;
inline bool stream_rng_attached=false;
inline void EndStream(){if(stream_rng_attached){g_Chain.Cut(&stream_rng);stream_rng_attached=false;}}
inline bool BeginStream(){
    EndStream();stream_rng.arg=nullptr;stream_rng.callback=[](void*)->u32{
        g_Rng.generationCount=0;g_GameManager.isPaused=FALSE;
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    };
    stream_rng_attached=g_Chain.AddToCalcChain(&stream_rng,6)==ZUN_SUCCESS;
    return stream_rng_attached;
}
inline bool Finish(){applying=false;return applied;}
}
#endif
