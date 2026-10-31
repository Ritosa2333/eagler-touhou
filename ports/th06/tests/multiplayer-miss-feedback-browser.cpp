// Private browser fixture: linked by miss-feedback-browser.cmake, outside production builds.
#include "MainMenu.hpp"
#include "Gui.hpp"
#include "GameManager.hpp"
#include "Player.hpp"
#include "Supervisor.hpp"
#include "EffectManager.hpp"
#include "AsciiManager.hpp"
#include "SoundPlayer.hpp"
#include "AnmManager.hpp"
#include "Rng.hpp"
#include "multiplayer/GameplaySession.hpp"
#include "netplay/NetplaySideEffects.hpp"
#include "netplay/Th06RollbackState.hpp"
#include <eagler/netplay/NetplayInput.hpp>
#include <emscripten.h>
#include <cstdio>
#include <cstring>
static unsigned labCount=2,labStage=6;
static bool labChallenge=true;
static ChainCallbackResult dispatch(void*){
 MultiplayerGameplay::SessionState state;state.playerCount=labCount;state.challengeMode=labChallenge;
 for(unsigned i=0;i<labCount;i++){state.players[i].active=true;state.players[i].character=i%2;state.players[i].shot=i==2;}
 if(!MultiplayerGameplay::Configure(state))return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
 g_GameManager.difficulty=NORMAL;g_GameManager.character=0;g_GameManager.shotType=0;g_GameManager.currentStage=labStage-1;
 g_GameManager.livesRemaining=2;g_GameManager.bombsRemaining=2;g_GameManager.isInPracticeMode=0;g_GameManager.isInReplay=0;g_GameManager.demoMode=0;
 g_Supervisor.framerateMultiplier=g_Supervisor.effectiveFramerateMultiplier=1;
 g_Supervisor.curState=SUPERVISOR_STATE_GAMEMANAGER;
 return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
}
extern "C" EMSCRIPTEN_KEEPALIVE int feedback_lab_begin(unsigned count,unsigned stage,int challenge){
 if(!g_MainMenu.chainCalc)return 0;labCount=count;labStage=stage;labChallenge=challenge;g_MainMenu.chainCalc->callback=dispatch;return 1;
}
extern "C" EMSCRIPTEN_KEEPALIVE int feedback_lab_ready(){return g_GameManager.currentStage==int(labStage)&&g_Gui.impl&&GetActivePlayerCount()==int(labCount)&&g_Players[0].chainCalc;}
#define CHECK(ok,why) do{if(!(ok))return why;}while(0)
static unsigned deathParticles(){unsigned n=0;for(const auto& e:g_EffectManager.effects)if(e.inUseFlag&&e.missConfirmation.deferred)n++;return n;}
extern "C" EMSCRIPTEN_KEEPALIVE const char* feedback_lab_test(){
 using namespace Netplay;using namespace Netplay::SideEffects;
 CHECK(feedback_lab_ready(),"stage not ready");g_EffectManager.Reset();ResetPlayerContributionStats();g_GameManager.rank=32;g_GameManager.minRank=0;g_GameManager.maxRank=32;g_GameManager.subRank=50;
 g_GameManager.isInGameMenu=g_GameManager.isInRetryMenu=0;
 std::array<FrameInput,3> lanes{};Input::SetReplayOverride(0);Input::SetPlayerInputOverrides(lanes.data(),labCount);
 for(unsigned i=0;i<labCount;i++){auto& p=g_Players[i];p.playerState=PLAYER_STATE_ALIVE;p.positionCenter={80.f+60*i,300.f,.2f};p.bombInfo.isInUse=0;p.respawnTimer=0;p.bulletGracePeriod=0;p.teamBombProtectionTimer.SetCurrent(0);SetPlayerLives(i,0);SetPlayerPower(i,80);}
 CHECK(Th06Rollback::Reset(),"rollback setup");
 auto step=[&](unsigned f,bool resim,bool hit){if(!Th06Rollback::BeginFrame(f))return false;{SimulationFrameScope scope(901,f,resim);for(unsigned i=0;i<labCount;i++){if(hit)g_Players[i].Die();else Player::OnUpdate(&g_Players[i]);}}return Th06Rollback::EndFrame();};
 CHECK(step(0,false,true),"predicted native collision");CHECK(deathParticles()==0,"collision emitted death particles before deathbomb window");
 for(unsigned f=1;f<=12;f++)CHECK(step(f,false,false),"predicted native death frames");
 for(unsigned i=0;i<labCount;i++)CHECK(g_MultiplayerContributionStats[i].challengeDeaths==1,"native challenge Miss not committed at deathbomb expiry");
 CHECK(deathParticles()==17*labCount,"native death particles count");
 CHECK(g_GameManager.rank*100+g_GameManager.subRank==3250-(1600/int(labCount))*int(labCount),"simultaneous native Miss Rank penalty not divided by players");
 for(const auto&e:g_EffectManager.effects)if(e.inUseFlag&&e.missConfirmation.deferred)CHECK(!MissFrameVisible(e.missConfirmation),"unconfirmed death particles became visible");
 CHECK(Th06Rollback::RestoreTo(0),"restore speculative death");CHECK(deathParticles()==0,"rollback retained false death particles");
 for(unsigned f=0;f<=12;f++)CHECK(step(f,true,false),"corrected native alive frames");
 CHECK(ConsumeConfirmedMisses(901,12)==0,"corrected-away native death sounded");
 for(unsigned i=0;i<labCount;i++)CHECK(g_MultiplayerContributionStats[i].challengeDeaths==0,"rollback retained false challenge Miss");
 CHECK(step(13,false,true),"actual native hit");for(unsigned f=14;f<=25;f++)CHECK(step(f,false,false),"actual native death frames");
 CHECK(ConsumeConfirmedMisses(901,24)==0,"actual native death sounded early");CHECK(ConsumeConfirmedMisses(901,25)==((1u<<labCount)-1),"confirmed native Miss sound mask");CHECK(ConsumeConfirmedMisses(901,25)==0,"confirmed native Miss sounded twice");
 for(const auto&e:g_EffectManager.effects)if(e.inUseFlag&&e.missConfirmation.deferred)CHECK(MissFrameVisible(e.missConfirmation),"confirmed death particles remain hidden");
 for(unsigned i=0;i<labCount;i++){CHECK(GetPlayerLives(i)==0,"challenge death consumed hidden stock");g_MultiplayerContributionStats[i].challengeDeaths=23+i;}
 g_AsciiManager.numStrings=0;g_AsciiManager.scale={0,0};g_AsciiManager.color=0;g_AsciiManager.isGui=1;g_AsciiManager.isSelected=true;g_Gui.DrawGameScene();unsigned rows=0;
 for(unsigned i=0;i<labCount;i++){char text[16];std::snprintf(text,sizeof text,"%u",23+i);bool found=false;for(int n=0;n<g_AsciiManager.numStrings;n++){const auto& s=g_AsciiManager.strings[n];if(std::strcmp(s.text,text)||s.position.x!=540||s.position.y!=112+48*i)continue;CHECK(s.position.z==0&&s.scale.x==.7f&&s.scale.y==.7f&&s.color==COLOR_WHITE&&!s.isGui&&!s.isSelected,"Miss HUD inherited invalid render state");found=true;}CHECK(found,"native numeric Miss row missing");rows++;}
 g_Gui.ShowSpellcard(0,34,"Conjuring - Eternal Meek");auto& portrait=g_Gui.impl->enemySpellcardPortrait;CHECK(portrait.sprite&&portrait.currentInstruction&&portrait.flags.isVisible,"stage 6 Sakuya spell portrait unavailable");
 Input::ClearPlayerButtonOverrides();Input::ClearReplayOverride();Th06Rollback::Clear();
 static char result[256];std::snprintf(result,sizeof result,"PASS %uP: native speculative Miss revoked, confirmed Miss once, %u death particles, Rank -%d, %u independent visible Miss rows, stage 6 portrait sprite %d",labCount,17*labCount,(1600/int(labCount))*int(labCount),rows,int(portrait.activeSpriteIndex));return result;
}
