// Execute the production state transitions, with rendering/audio represented
// by counters. --before verifies the same fixture rejects the pre-fix source.
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..'),before=process.argv.includes('--before');
const read=name=>before?execFileSync('git',['show','dc78e4cf:ports/th07/src/'+name],{cwd:root,encoding:'utf8'}):readFileSync(resolve(root,'src',name),'utf8');
const player=read('Player.cpp'),gui=read('Gui.cpp');
function method(name){const start=player.indexOf(name);if(start<0)throw Error(name);let at=player.indexOf('{',start),depth=1,end=at+1;for(;depth;end++){if(player[end]==='{')depth++;if(player[end]==='}')depth--;}return player.slice(start,end);}
const run=gui.slice(gui.indexOf('ZunResult GuiImpl::RunMsg()'));
const settlement=run.slice(run.indexOf('#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY'),run.indexOf('    if (g_Player.playerState != PLAYER_STATE_DEAD)'));
const source=`
#include <cassert>
#include <cstdio>
#include <cstdint>
using u8=uint8_t;using i32=int;using u32=uint32_t;
constexpr int TH07_MULTI_MAX_PLAYERS=3;
enum {BORDER_NONE,BORDER_READY,BORDER_ACTIVE};
enum {PLAYER_STATE_ALIVE,PLAYER_STATE_INVULNERABLE,PLAYER_STATE_BORDER,PLAYER_STATE_SPAWNING,PLAYER_STATE_DEAD,PLAYER_STATE_ELIMINATED,PLAYER_STATE_SPIRIT};
enum {SOUND_BORDER_BREAK,SOUND_BORDER_ACTIVATE,SOUND_BORDER_ACTIVATE2};
struct Vec {float x=0,y=0,z=0;};
struct Timer {int value=0;Timer& operator=(int n){value=n;return *this;}int GetCurrent()const{return value;}};
struct Effect {int inUseFlag=1;struct {int interpStartTimes[5]{},interpEndTimes[5]{},easeModes[5]{},intVars1[1]{};Vec scaleInterpInitial,scaleInterpFinal,angleVel;} vm;};
struct Player {int hasBorder=0,playerState=0,initParam=0,respawnTimer=0,borderInvulnerabilityTime=0;Timer invulnerabilityTimer,borderTimer;struct {bool isInUse=false;}bombInfo;struct {Vec scale;struct {u32 color=0;}color;int blendMode=0;}playerSprite;struct Shot {int initialRespawnTimer=0;} shot,*shooterData=&shot;Vec pos;Effect *borderEffect=nullptr,*effect=nullptr;void ActivateBorder();void BreakBorderNaturally();void BreakBorder(){hasBorder=BORDER_NONE;}};
Player g_Players[3];Player& g_Player=g_Players[0];bool g_PlayerActive[3]{};bool g_SharedBorderTransition=false;
int count=2;int g_cherryMaxBreakGrowth[3]{};
namespace MultiplayerGameplay {bool IsMultiplayer(){return true;}}
int GetActivePlayerCount(){return count;}bool IsPlayerSlotActive(int i){return g_PlayerActive[i];}
bool IsSharedBorderParticipant(Player* p){return p->playerState!=PLAYER_STATE_ELIMINATED&&p->playerState!=PLAYER_STATE_SPIRIT;}
int GetPlayerEffectSlot(Player*,int slot){return slot;}
struct Gui {bool dialogue=false;int awards=0,starts=0;bool HasCurrentMsgIdx(){return dialogue;}void ShowStatusPopup(int,int type){if(type==4)awards++;if(type==2)starts++;}}g_Gui;
struct Game {struct Globals {int cherryStart=0;} globalsValue,*globals=&globalsValue;int cherry=100000,cherryMax=200000,cherryPlus=50000;long long score=0;void IncreaseCherryMax(int n){cherryMax+=n;}void IncreaseCherry(int n){cherry+=n;}void AddScore(int n){score+=n;}}g_GameManager;
struct {Effect pool[2048];int used=0;Effect* SpawnSpecialEffect(int,Vec*,int,int,u32){assert(used<2048);return &pool[used++];}}g_EffectManager;
struct {void PlaySoundByIdx(int,int){}}g_SoundPlayer;
struct Replay {int replayEventFlags=0;}replay;Replay* g_ReplayManager=&replay;
void ActivateSharedBorder();bool IsSharedBorderActive();
${method('void ClearSharedBorderState(Player *player)')}
${method('bool IsSharedBorderActive()')}
${method('void ActivateSharedBorder()')}
${method('void Player::ActivateBorder()')}
${method('void Player::BreakBorderNaturally()')}
void dialogueTick(){
${settlement}
}
void reset(int n){count=n;g_Gui={};g_GameManager.score=0;g_GameManager.cherry=100000;g_GameManager.cherryMax=200000;g_GameManager.cherryPlus=n==3?75000:50000;g_EffectManager.used=0;for(int i=0;i<3;i++){g_Players[i]=Player{};g_Players[i].shooterData=&g_Players[i].shot;g_Players[i].initParam=i;g_PlayerActive[i]=i<n;}}
void pendingTick(){for(int i=0;i<count;i++)if(g_Players[i].hasBorder==BORDER_READY)g_Players[i].ActivateBorder();}
int main(){for(int n:{2,3}){
 reset(n);g_Gui.dialogue=true;ActivateSharedBorder();
 for(int f=0;f<600;f++){pendingTick();dialogueTick();for(int i=0;i<n;i++)assert(g_Players[i].hasBorder==BORDER_READY);assert(g_Gui.awards==0&&g_GameManager.score==0&&g_EffectManager.used==0);}
 g_Gui.dialogue=false;pendingTick();assert(IsSharedBorderActive());assert(g_EffectManager.used==n);
 g_Gui.dialogue=true;dialogueTick();const auto score=g_GameManager.score;assert(g_Gui.awards==1&&score>0);
 for(int f=0;f<600;f++){pendingTick();dialogueTick();assert(g_Gui.awards==1&&g_GameManager.score==score);}
 for(int i=0;i<n;i++)assert(g_Players[i].hasBorder==BORDER_NONE);
 reset(n);g_Players[n-1].bombInfo.isInUse=true;ActivateSharedBorder();assert(g_Players[n-1].hasBorder==BORDER_READY);
 g_Gui.dialogue=true;dialogueTick();assert(g_Gui.awards==1);assert(g_Players[n-1].hasBorder==BORDER_NONE);assert(g_Players[n-1].playerState==PLAYER_STATE_ALIVE);
 g_Gui.dialogue=false;g_Players[n-1].bombInfo.isInUse=false;pendingTick();assert(!IsSharedBorderActive());
 reset(n);g_Players[0].hasBorder=BORDER_READY;g_Players[0].BreakBorderNaturally();assert(g_Gui.awards==0);
 }std::puts("PASS TH07 shared border: 2P/3P, dialogue overlap, 600-frame wait, one-shot award, pending bomb participant, stale READY rejection");}
`;
const out=resolve(root,'build/tests/shared-border-dialogue');mkdirSync(out,{recursive:true});
const cpp=resolve(out,before?'before.cpp':'fixed.cpp'),wasm=cpp.replace('.cpp','.wasm');writeFileSync(cpp,'#include <initializer_list>\n'+source);
const sdk=process.env.WASI_SDK_PATH;if(!sdk)throw Error('Set WASI_SDK_PATH');
execFileSync(resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++'),['-std=c++17','-O2','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',cpp,'-o',wasm],{stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
let code;try{code=wasi.start(instance);}catch(error){if(!before||!(error instanceof WebAssembly.RuntimeError))throw error;code=1;}
if(before){if(code===0)throw Error('Old source did not reproduce');console.log('PASS negative reproduction: old source rejected');}else if(code!==0)throw Error('Border regression failed');
