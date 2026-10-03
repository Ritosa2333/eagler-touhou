#!/usr/bin/env python3
"""Execute exact production rule function bodies against a minimal world fixture.

This is a focused behavior test, not a full game, rendering, transport or ECL test.
The fixture stubs rendering/audio/item motion; selected production function bodies
are extracted at run time so tests cannot silently exercise a parallel rules model.
"""
from pathlib import Path
import os
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SIX = 'TH06_MULTI_MAX_PLAYERS' in (ROOT / 'src/Multiplayer.hpp').read_text()
PLAYER = (ROOT / 'src/Player.cpp').read_text()
ITEMS = (ROOT / 'src/ItemManager.cpp').read_text()

def function(text, signature):
    start = text.index(signature)
    brace = text.index('{', start)
    while ';' in text[start:brace]:
        start = text.index(signature, start + 1)
        brace = text.index('{', start)
    depth, end = 1, brace + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end] + '\n'

stub = r'''
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using i32=int; using u8=uint8_t; using i8=int8_t; using u32=uint32_t; using f32=float;
#define TH_ENABLE_MULTIPLAYER_GAMEPLAY
#define TH06_MULTI_MAX_PLAYERS 3
#define TH07_MULTI_MAX_PLAYERS 3
#define ARRAY_SIZE_SIGNED(x) int(sizeof(x)/sizeof((x)[0]))
#define MAX_ITEMS 1100
constexpr int PLAYER_STATE_ALIVE=0, PLAYER_STATE_INVULNERABLE=1, PLAYER_STATE_DEAD=2,
 PLAYER_STATE_SPAWNING=3, PLAYER_STATE_REVIVABLE=4, PLAYER_STATE_SPIRIT=4,
 PLAYER_STATE_ELIMINATED=5, PLAYER_STATE_BORDER=6;
constexpr int ORB_UNFOCUSED=0, OPTION_UNFOCUSED=0, ORB_HIDDEN=1, OPTION_HIDDEN=1,
 BORDER_ACTIVE=1, CHAR_SAKUYA=2, TH_BUTTON_SHOOT=1, TH_BUTTON_BOMB=2,
 ITEM_POWER_BIG=2, ITEM_POWER_SMALL=1, ITEM_LIFE=3, ITEM_FULL_POWER=4, ITEM_BOMB=5,
 SOUND_1UP=0, SOUND_EXTEND=0, SOUND_POWERUP=0, SOUND_ITEM_GET=0, SOUND_DIR_CHANGING=0,
 AnmVmBlendMode_InvSrcAlpha=0, AnmVmBlendMode_One=1, ANM_SCRIPT_PLAYER_IDLE=0;
constexpr unsigned COLOR_WHITE=0xffffffff;
#define COLOR_SET_ALPHA(c,a) ((u32(a)<<24)|0xffffff)
struct ZunVec3 { float x=0,y=0,z=0; };
struct Timer { int n=0; Timer& operator=(int v){n=v;return *this;} operator int()const{return n;}
 void SetCurrent(int v){n=v;} float AsFramesFloat()const{return n;} int AsFrames()const{return n;}
 float AsFloat()const{return n;} int GetCurrent()const{return n;} };
struct Color { unsigned color=0; Color&operator=(unsigned c){color=c;return *this;} };
struct Sprite { float scaleX=1,scaleY=1; ZunVec3 scale; Color color; int blendMode=0;
 struct {int blendMode=0;}flags; };
struct Shooter {int initialRespawnTimer=6,initialBombs=4; float cherryPenaltyMultiplier=.2f;};
Shooter shooter;
struct Player {u8 initParam=0; int playerState=0,orbState=0,optionState=0,
 respawnTimer=6,bulletGracePeriod=0,isFocus=0,lifeGiveTimer=0,lifeGiveTargetToken=0,
 powerGiveTaps=0,powerGiveWindow=0,hasBorder=0;
 float previousHorizontalSpeed=0,previousVerticalSpeed=0,
 verticalMovementSpeedMultiplierDuringBomb=1,horizontalMovementSpeedMultiplierDuringBomb=1;
 Timer invulnerabilityTimer; ZunVec3 pos,prevPos,positionCenter; Sprite playerSprite;
 struct {int isInUse=0;}bombInfo; Shooter*shooterData=&shooter; bool shoot=false,pressed=false;
 void BreakBorder(){hasBorder=0;} };
Player g_Players[3]; bool g_PlayerActive[3]; bool absent[3],departed[3];
int lives[3],bombs[3],power[3],g_powerGiveTaps[3],g_powerGiveWindow[3],g_teamWipeRetryFrames;
bool multiplayer=true;
namespace MultiplayerGameplay {bool IsMultiplayer(){return multiplayer;}
 bool IsPlayerTemporarilyAbsent(u8 p){return absent[p];}
 bool IsPlayerPermanentlyDeparted(u8 p){return departed[p];}
 int GetPlayerCount(){return int(g_PlayerActive[0])+int(g_PlayerActive[1])+int(g_PlayerActive[2]);}
 int GetPlayerCharacter(u8){return 0;} }
bool IsPlayerActive(u8 p){return p<3&&g_PlayerActive[p];}
bool IsPlayerSlotActive(u8 p){return IsPlayerActive(p);}
bool IsPlayerGameplayActive(u8 p){return IsPlayerActive(p)&&!absent[p];}
int GetActivePlayerCount(){return MultiplayerGameplay::GetPlayerCount();}
 bool IsMultiplayerTransferState(int state){return state>=6&&state<=8;}
int GetPlayerLives(u8 p){return lives[p];} int GetPlayerBombs(u8 p){return bombs[p];}
int GetPlayerPower(u8 p){return power[p];}
void SetPlayerLives(u8 p,int v){lives[p]=v;} void AddPlayerLives(u8 p,int v){lives[p]+=v;}
void SetPlayerBombs(u8 p,int v){bombs[p]=v;} void SetPlayerPower(u8 p,int v){power[p]=v;}
void AddPlayerPower(u8 p,int v){power[p]+=v;}
#define WAS_PRESSED_PLAYER(p,b) ((p)->pressed)
#define IS_PRESSED_PLAYER(p,b) ((p)->shoot)
struct Item {bool isInUse=false; int type=0,state=0; ZunVec3 position;};
struct ItemManager {Item items[ITEM_CAPACITY+1]; std::vector<int> emitted;
 bool CanSpawnItems(int count)const;
 SPAWN_ITEM_DECL
 Item* SpawnSingleItem(const ZunVec3* position,int type,int state){
  for(int i=0;i<ITEM_CAPACITY;++i)if(!items[i].isInUse){items[i]={true,type,state,*position};emitted.push_back(type);return items+i;}
  return items+ITEM_CAPACITY; }
 SPAWN_DROP_DECL
 void ActivateAllItems(){} } g_ItemManager;
int GetItemTransferStateForPlayer(u8 p){return p+6;}
int GetLifeTransferSpawnState(u8 p){return p==0?4:p==1?3:5;}
struct {struct {int flag0=0,flag1=0,flag2=0;}flags;
 int lifeDisplayUpdateFrames=0,bombDisplayUpdateFrames=0,powerDisplayUpdateFrames=0,pointDisplayUpdateFrames=0;}g_Gui;
struct {void PlaySoundByIdx(int,int=0){}}g_SoundPlayer;
struct MultiplayerPlayerResources {int livesRemaining=0,bombsRemaining=0,currentPower=0;};
MultiplayerPlayerResources g_MultiplayerPlayerResources[2];
MultiplayerPlayerResources*GetSidecarResources(u8 p){return p>0&&p<3?&g_MultiplayerPlayerResources[p-1]:nullptr;}
struct Config {int lifeCount=2;} config;
struct Globals {int cherryStart=0;} globals;
struct {int powerItemCountForScore=0,currentPower=0,difficulty=0,isInPracticeMode=0,
 extraLives=0,isInRetryMenu=0,livesRemaining=0,cherry=100000,isInReplay=0;
 int&bombsRemaining=bombs[0]; Config*defaultCfg=&config;
 ZunVec3 arcadeRegionSize{384,448,0}; Globals*globals=&::globals;
 void DecreaseSubrank(int){} void CheckGameIntegrityOnDeath(int){} }g_GameManager;
constexpr int SUPERVISOR_STATE_GAMEMANAGER_REINIT=3;
struct {struct {int bombCount=4;}defaultConfig;int curState=0;}g_Supervisor;
struct {unsigned replayEventFlags=0;}replay;
auto*g_ReplayManager=&replay;
struct {struct {int captureScore=123,isCapturing=1,isActive=0;}spellcardInfo;}g_EnemyManager;
struct {unsigned n=0;unsigned GetRandomU16(){return ++n;}}g_Rng;
struct Anm {void SetAndExecuteScriptIdx(Sprite*,int){} void SetAnmIdxAndExecuteScript(Sprite*,int){}}anm;
auto*g_AnmManager=&anm;
int GetPlayerAnmScript(Player*,int v){return v;}
float PlayerSpawnX(Player*){return 192;}
int g_CurFrameInput=0,g_CurFrameRawInput=0;
namespace PracticeRuntime {bool OverlayInfinitePower(){return false;}bool OverlayInfiniteLives(){return false;}
 bool OverlayAutoBomb(){return false;} void RecordTrackerMiss(){} }
int GetMultiplayerRankPenalty(int amount){return amount;}
struct BulletManager {int calls=0;ZunVec3 center;float radius=0;
 void ClearRescueBullets(const ZunVec3* p,float r){++calls;center=*p;radius=r;}
}g_BulletManager;
'''.replace('ITEM_CAPACITY', '512' if SIX else '1100').replace(
    'SPAWN_DROP_DECL', 'void SpawnEnemyDrop(const ZunVec3*,ItemType,int);' if SIX else
    'Item *SpawnEnemyDrop(ZunVec3*,i32,i32);')
stub = stub.replace('SPAWN_ITEM_DECL', 'void SpawnItem(const ZunVec3*,int,int);' if SIX else 'Item* SpawnItem(ZunVec3*,int,int);')
# ItemType is an integer in the fixture; production enum conversion is separately compiled.
stub = stub.replace('using i32=int;', 'using ItemType=int; using i32=int;')
constant_names = ['LIFE_GIVE_HOLD_FRAMES','LIFE_GIVE_WAIT_RELEASE_TOKEN','RESOURCE_TRANSFER_DISTANCE_SQ',
 'POWER_GIVE_TAPS_REQUIRED','POWER_GIVE_TAP_WINDOW','POWER_GIVE_AMOUNT','POWER_GIVE_PROMPT_AFTER',
 'REVIVABLE_DRIFT_SPEED','PLAYER_SPIRIT_DRIFT_SPEED','RESCUE_BOMBS','RESCUE_POWER','RESCUE_CLEAR_RADIUS']
constants = ''
for name in constant_names:
    match = re.search(r'(?:constexpr|const)\s+(?:i32|f32)\s+'+name+r'\s*=\s*[^;]+;',PLAYER)
    if match:
        constants += match.group(0)+'\n'
    elif name.startswith('POWER_'):
        header = (ROOT / 'src/Player.hpp').read_text()
        match = re.search(r'(?:constexpr|const)\s+(?:i32|f32)\s+'+name+r'\s*=\s*[^;]+;',header)
        if match: constants += match.group(0)+'\n'

signatures = (['bool IsTerminalPlayerState(', 'bool IsLivingTransferPlayer(', 'bool InTransferRange(',
 'Player *SelectPowerTransferReceiver(', 'Player *SelectLifeTransferReceiver(', 'void ResetTransferInputState(',
 'void RevivePlayerFromTeammate(', 'void UpdatePowerTransfer(', 'void UpdateLifeTransfer('] if SIX else
 ['bool IsPlayerActiveForProximity(', 'bool IsPlayerActiveForLifeTransfer(',
 'Player *SelectPowerTransferReceiver(', 'bool IsPowerTransferArmed(', 'void UpdatePowerTransfer(',
 'Player *SelectLifeTransferReceiver(', 'void UpdateLifeTransfer('])
functions = '' if SIX else function((ROOT / 'src/MultiplayerResources.cpp').read_text(), 'i32 GetPlayerInitialBombs(')
functions += ''.join(function(PLAYER,s) for s in signatures)
functions += function((ROOT / 'src/MultiplayerResources.cpp').read_text(), 'void ResetMultiplayerPlayerResources(')
functions += function(PLAYER, 'i32 GetBossParticipantCount(')
functions += function(PLAYER, 'void PrepareMultiplayerStageRevival(')
functions += function(ITEMS, 'bool ItemManager::CanSpawnItems(')
functions += function(ITEMS, 'void ItemManager::SpawnItem(' if SIX else 'Item *ItemManager::SpawnItem(')
functions += function(ITEMS, 'void ItemManager::SpawnEnemyDrop(' if SIX else 'Item *ItemManager::SpawnEnemyDrop(')
if SIX:
    a = PLAYER.index('    if (p->playerState == PLAYER_STATE_DEAD)')
    b = PLAYER.index('    if (p->bulletGracePeriod != 0)',a)
    functions += '''void DeathTick(Player*p) { bool multiplayer=::multiplayer;
      int livesRemaining=GetPlayerLives(p->initParam); float scaleFactor1,scaleFactor2;\n'''+PLAYER[a:b]+'}\n'
else:
    functions += function(PLAYER,'i32 UpdateMultiplayerDeath(')
    functions += 'void DeathTick(Player*p){UpdateMultiplayerDeath(p);}\n'

main = r'''
void reset(int count=3){
 shooter.initialBombs=4;g_ItemManager=ItemManager{};g_BulletManager=BulletManager{};g_Rng.n=0;g_teamWipeRetryFrames=0;
 for(int i=0;i<3;++i){g_Players[i]=Player{};g_Players[i].initParam=i;g_PlayerActive[i]=i<count;
 absent[i]=departed[i]=false;lives[i]=2;bombs[i]=4;power[i]=100;g_powerGiveTaps[i]=g_powerGiveWindow[i]=0;}
}
void hold(Player*p,int n){p->isFocus=1;for(int i=0;i<n;++i)UpdateLifeTransfer(p);}
void tap(Player*p,int n){for(int i=0;i<n;++i){p->pressed=true;UpdatePowerTransfer(p);p->pressed=false;UpdatePowerTransfer(p);}}
void test(){
 for(int count: {2,3}){
  reset(count);ZunVec3 origin;g_ItemManager.SpawnItem(&origin,ITEM_POWER_SMALL,0);
  for(int copy=1;copy<count;++copy)assert(g_ItemManager.items[copy].position.x!=g_ItemManager.items[copy-1].position.x);
  reset(count);g_ItemManager.SpawnItem(&origin,ITEM_LIFE,GetItemTransferStateForPlayer(1));
  assert(g_ItemManager.emitted.size()==1); // Gifts are never multiplied.
 }

 reset();g_Supervisor.curState=3;g_Players[1].playerState=PLAYER_STATE_SPIRIT;
 PrepareMultiplayerStageRevival(&g_Players[0]);PrepareMultiplayerStageRevival(&g_Players[1]);
 assert(bombs[0]==4&&bombs[1]==INITIAL_BOMBS);g_Supervisor.curState=0;
 reset();assert(g_ItemManager.CanSpawnItems(0));assert(g_ItemManager.CanSpawnItems(6));
 for(auto&i:g_ItemManager.items)i.isInUse=true;
 assert(!g_ItemManager.CanSpawnItems(1));
 for(int i=0;i<5;++i)g_ItemManager.items[i].isInUse=false;
 assert(!g_ItemManager.CanSpawnItems(6));tap(&g_Players[0],5);assert(power[0]==100);
 g_ItemManager.items[5].isInUse=false;assert(g_ItemManager.CanSpawnItems(6));
 tap(&g_Players[0],5);assert(power[0]==80);assert(g_ItemManager.emitted.size()==6);
 assert(std::count(g_ItemManager.emitted.begin(),g_ItemManager.emitted.end(),ITEM_POWER_BIG)==2);
 tap(&g_Players[0],5);assert(power[0]==80); // Full pool never debits a second time.
 reset();power[1]=power[2]=128;tap(&g_Players[0],5);assert(power[0]==100);
 reset();power[0]=19;tap(&g_Players[0],5);assert(power[0]==19);
 reset();lives[1]=lives[2]=8;hold(&g_Players[0],180);assert(lives[0]==2);
 reset(2);g_Players[2].playerState=PLAYER_STATE_SPIRIT;lives[2]=0;
 assert(SelectLifeTransferReceiver(&g_Players[0])==&g_Players[1]); // Inactive spectator never selected.
 for(int count: {2,3}){
  reset(count);g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=0;power[1]=0;bombs[1]=0;
  hold(&g_Players[0],89);assert(lives[0]==2);assert(g_Players[1].playerState==PLAYER_STATE_SPIRIT);
  hold(&g_Players[0],1);assert(lives[0]==1);assert(lives[1]==0);
  assert(bombs[0]==4&&bombs[1]==INITIAL_BOMBS&&power[1]==64);
  assert(g_Players[1].bulletGracePeriod==0&&g_BulletManager.calls==0);
  assert(g_Players[1].playerState==PLAYER_STATE_INVULNERABLE);
  assert(g_Players[1].invulnerabilityTimer==240);
  hold(&g_Players[0],180);assert(lives[0]==1); // Same hold/retransmitted level is one donation.
  g_Players[0].isFocus=0;UpdateLifeTransfer(&g_Players[0]);
  hold(&g_Players[0],90);assert(lives[0]==0); // Explicit release and repress permits a new paid gift.
 }
 reset();g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=3;
 hold(&g_Players[0],90);assert(lives[1]==0); // Rescue discards banked shared extends.
 reset(2);g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=8;
 hold(&g_Players[0],90);assert(lives[1]==0&&g_Players[1].playerState==PLAYER_STATE_INVULNERABLE);
 reset();lives[0]=0;g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=0;
 hold(&g_Players[0],180);assert(g_Players[1].playerState==PLAYER_STATE_SPIRIT);
 for(int stock: {0,1,3}){
  reset();Player*p=&g_Players[0];lives[0]=stock;power[0]=80;p->playerState=PLAYER_STATE_DEAD;
  p->respawnTimer=1;DeathTick(p);
  assert(g_ItemManager.emitted.size()==(stock?18u:5u));
  assert(power[0]==(stock?64:0));
  if(!stock)assert(std::count(g_ItemManager.emitted.begin(),g_ItemManager.emitted.end(),ITEM_FULL_POWER)==5);
  p->invulnerabilityTimer=30;DeathTick(p);
  assert(lives[0]==(stock?stock-1:0));assert(lives[1]==2&&lives[2]==2);
  assert(bombs[0]==(stock?INITIAL_BOMBS:TERMINAL_BOMBS));
 }
 reset();ZunVec3 position;
 for(int item: {ITEM_LIFE,ITEM_BOMB,ITEM_POWER_SMALL,ITEM_POWER_BIG})g_ItemManager.SpawnEnemyDrop(&position,item,0);
 assert(g_ItemManager.emitted.size()==10); // 3P doubles stage LIFE/BOMB, triples ordinary P.
 for(int count: {2,3}){
  reset(count);ZunVec3 origin;for(int kind: {ITEM_POWER_SMALL,ITEM_POWER_BIG,ITEM_FULL_POWER,ITEM_LIFE,ITEM_BOMB})g_ItemManager.SpawnEnemyDrop(&origin,kind,0);
  assert(g_ItemManager.emitted.size()==unsigned(2*count+(count==3?5:3)));
  g_Players[1].playerState=PLAYER_STATE_SPIRIT;
  assert(GetBossParticipantCount()==count-1); // Ghost does not lower the fixed drop multiplier.
  g_ItemManager.SpawnEnemyDrop(&origin,ITEM_POWER_SMALL,0);
  assert(g_ItemManager.emitted.size()==unsigned(3*count+(count==3?5:3)));
 }
 reset();assert(SelectLifeTransferReceiver(&g_Players[0])==&g_Players[2]);
 assert(SelectPowerTransferReceiver(&g_Players[0])==&g_Players[1]);
 assert(GetBossParticipantCount()==3);g_Players[1].playerState=PLAYER_STATE_DEAD;assert(GetBossParticipantCount()==3);
 g_Players[1].playerState=PLAYER_STATE_SPIRIT;assert(GetBossParticipantCount()==2);
 absent[2]=true;assert(GetBossParticipantCount()==1);absent[2]=false;departed[2]=true;assert(GetBossParticipantCount()==1);
 departed[2]=false;g_Players[1].playerState=PLAYER_STATE_ALIVE;assert(GetBossParticipantCount()==3);
 for(int count: {2,3})for(int donorBombs: {0,1,4}){
  reset(count);bombs[0]=donorBombs;g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=0;bombs[1]=3;
  hold(&g_Players[0],90);assert(bombs[0]==donorBombs&&bombs[1]==INITIAL_BOMBS);AddPlayerLives(1,1);
  Player*p=&g_Players[1];p->playerState=PLAYER_STATE_DEAD;p->respawnTimer=1;DeathTick(p);
  p->invulnerabilityTimer=30;DeathTick(p);
  assert(bombs[1]==INITIAL_BOMBS&&lives[1]==0&&bombs[0]==donorBombs&&lives[0]==1);
 }
 // Exact deterministic re-execution of production rescue from a copied pre-frame fixture.
 reset();g_Players[1].playerState=PLAYER_STATE_SPIRIT;lives[1]=0;
 hold(&g_Players[0],89);auto before0=g_Players[0],before1=g_Players[1];
 int beforeLives[3],beforeBombs[3],beforePower[3];std::copy(lives,lives+3,beforeLives);std::copy(bombs,bombs+3,beforeBombs);std::copy(power,power+3,beforePower);
 hold(&g_Players[0],1);auto expected0=g_Players[0],expected1=g_Players[1];int expectedLives=lives[0];
 g_Players[0]=before0;g_Players[1]=before1;std::copy(beforeLives,beforeLives+3,lives);std::copy(beforeBombs,beforeBombs+3,bombs);std::copy(beforePower,beforePower+3,power);
 hold(&g_Players[0],1);assert(lives[0]==expectedLives);
 assert(std::memcmp(&expected0,&g_Players[0],sizeof(Player))==0);
 assert(std::memcmp(&expected1,&g_Players[1],sizeof(Player))==0);
}
int main(){test();std::puts("production cooperative transfer/death/drop rule fixtures: PASS");}
'''
startup = r"""
 reset();g_GameManager.isInReplay=0;bombs[0]=4;
 START_RESET
 assert(g_MultiplayerPlayerResources[0].bombsRemaining==INITIAL_BOMBS);
 assert(g_MultiplayerPlayerResources[1].bombsRemaining==INITIAL_BOMBS);
 START_EXTRA
""".replace('START_RESET', 'ResetMultiplayerPlayerResources();' if SIX else
            'ResetMultiplayerPlayerResources(1);ResetMultiplayerPlayerResources(2);').replace(
 'START_EXTRA', 'assert(bombs[0]==2);g_GameManager.isInReplay=1;bombs[0]=5;ResetMultiplayerPlayerResources();assert(bombs[0]==5);' if SIX else
 'assert(g_MultiplayerPlayerResources[0].livesRemaining==2);assert(g_MultiplayerPlayerResources[0].currentPower==0);')
main = main.replace('INITIAL_BOMBS', '2' if SIX else '3')
main = main.replace('void test(){', 'void test(){'+startup).replace('TERMINAL_BOMBS', '4' if SIX else '3')
main = main.replace('INITIAL_BOMBS', '2' if SIX else '3')
with tempfile.TemporaryDirectory(prefix='coop-rules-') as temp:
    source=Path(temp)/'rules.cpp'
    source.write_text(stub+constants+functions+main)
    subprocess.run([os.environ.get('CXX','c++'),'-std=c++20','-O0','-g',str(source),'-o',str(Path(temp)/'rules')],check=True)
    subprocess.run([str(Path(temp)/'rules')],check=True)
