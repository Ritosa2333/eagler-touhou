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
struct NativeStage { i32 score=0; i16 randomSeed=0,pointItemsCollected=0; u8 power=0; i8 livesRemaining=0,bombsRemaining=0; u8 rank=0; i8 powerItemCountForScore=0; };
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
inline std::array<std::vector<u8>,7> recorded;
inline Stage prepared;
inline bool applying=false,applied=false;
template<class C> bool Values(C& c,Stage& s){
    auto& n=s.native;auto& setup=s.setup;
    if(!c.Values(s.number,s.difficulty,s.lifeCount,s.bombCount,n.score,n.randomSeed,n.pointItemsCollected,n.power,n.livesRemaining,n.bombsRemaining,n.rank,n.powerItemCountForScore)||
       !c.Values(setup.playerCount,setup.localPlayer,setup.showStagePlayerNames,setup.challengeMode,
                 setup.showContributionStats))return false;
    for(auto& p:setup.players)if(!c.Values(p.active,p.temporarilyAbsent,p.permanentlyDeparted,p.character,p.shot))return false;
    for(auto& r:s.resources)if(!c.Values(r.lives,r.bombs,r.power,r.challengeDeaths))return false;
    for(auto& r:s.contributions)if(!c.Values(r.enemiesDefeated,r.damageDealt))return false;
    if(!c.Value(s.leadInFrames)||s.leadInFrames>s.leadIn.size())return false;
    for(u32 f=0;f<s.leadInFrames;++f)for(auto& p:s.leadIn[f])
        if(!c.Values(p.buttons,p.analogMode,p.x,p.y,p.unlimited,p.touchUsed,p.touchBomb))return false;
    return true;
}
inline bool Valid(const Stage& s){
    if(s.lifeCount>8||s.bombCount>8||s.number<1||s.number>7||s.difficulty>4||s.setup.playerCount<2||s.setup.playerCount>3||
       s.setup.localPlayer>=s.setup.playerCount)return false;
    for(unsigned i=0;i<s.setup.playerCount;++i)
        if(s.setup.players[i].character>1||s.setup.players[i].shot>1||
           s.resources[i].lives < -1||s.resources[i].lives>8||s.resources[i].bombs < 0||
           s.resources[i].bombs>8||s.resources[i].power<0||s.resources[i].power>128)return false;
    return true;
}
inline void Record(const StageReplayData& native){
    if(applying||!MultiplayerGameplay::IsMultiplayer())return;
    Stage s;s.number=g_GameManager.currentStage;s.difficulty=g_GameManager.difficulty;
    s.lifeCount=g_Supervisor.defaultConfig.lifeCount;s.bombCount=g_Supervisor.defaultConfig.bombCount;
    s.native.score=g_GameManager.score;
    s.native.randomSeed=native.randomSeed;
    s.native.pointItemsCollected=native.pointItemsCollected;
    s.native.power=native.power;
    s.native.livesRemaining=native.livesRemaining;
    s.native.bombsRemaining=native.bombsRemaining;
    s.native.rank=native.rank;
    s.native.powerItemCountForScore=native.powerItemCountForScore;
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
    if(!output||stage<1||stage>7||recorded[stage-1].empty())return false;
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
inline void ApplyResources(){
    for(unsigned i=0;i<prepared.setup.playerCount;++i){
        const auto& r=prepared.resources[i];SetPlayerLives(i,r.lives);SetPlayerBombs(i,r.bombs);SetPlayerPower(i,r.power);
        g_MultiplayerContributionStats[i].challengeDeaths=r.challengeDeaths;
        g_MultiplayerContributionStats[i].enemiesDefeated=prepared.contributions[i].enemiesDefeated;
        g_MultiplayerContributionStats[i].damageDealt=prepared.contributions[i].damageDealt;
    }
}
inline bool Apply(){
    if(!applying||applied||!MultiplayerGameplay::Configure(prepared.setup))return false;
    const auto& n=prepared.native;
    g_Supervisor.defaultConfig.lifeCount=prepared.lifeCount;g_Supervisor.defaultConfig.bombCount=prepared.bombCount;
    g_GameManager.character=prepared.setup.players[0].character;
    g_GameManager.shotType=prepared.setup.players[0].shot;
    g_GameManager.difficulty=static_cast<Difficulty>(prepared.difficulty);
    g_GameManager.pointItemsCollected=n.pointItemsCollected;
    g_GameManager.rank=n.rank;
    g_GameManager.livesRemaining=n.livesRemaining;
    g_GameManager.bombsRemaining=n.bombsRemaining;
    g_GameManager.currentPower=n.power;
    g_GameManager.powerItemCountForScore=n.powerItemCountForScore;
    g_GameManager.score=g_GameManager.guiScore=n.score;
    g_Rng.Initialize(n.randomSeed);
    ApplyResources();
    applied=true;return true;
}
inline bool Finish(){applying=false;return applied;}
}
#endif
