#include <cstdint>
#include <string>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
using u32=uint32_t;
constexpr u32 PrTime=1;
enum class GameSessionPhase {stage,paused};
struct Spell {u32 flags=0;int elapsed=0,bonus=0;};
static int updates=0,observations=0;
struct Player {Spell spell;int last_error=0;struct{int last_error=0;}motion,shots;struct{int life_state=0;struct{int current=0;}state_timer;}state;bool update(){++updates;return true;}};
struct GameBattle;static void observe(const GameBattle&){++observations;}
struct GameSessionInput{u32 held=0;};
struct GameBattle{Player storage;Player* player=&storage;bool player_live=true;u32 spell_flags=0;int spell_elapsed=0,spell_bonus=0,last_error=0;std::string error;struct{struct{u32 held=0;}movement;}player_input;void(*before_player_update)(const GameBattle&)=observe;void(*practice_key_sample)(u32)=nullptr;void set_input(const GameSessionInput& e){player_input.movement.held=e.held;}bool update(){if(player_live&&player){
        player->spell={spell_flags,spell_elapsed,spell_bonus};
        if(before_player_update)before_player_update(*this);
        if(!player->update()){if(!last_error)last_error=-601;const std::string inner="player update failed: "+std::to_string(player->last_error)+" motion="+std::to_string(player->motion.last_error)+" shots="+std::to_string(player->shots.last_error)+" state="+std::to_string(player->state.life_state)+" timer="+std::to_string(player->state.state_timer.current);error=error.empty()?inner:inner+" ("+error+")";return false;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    };return true;}};
struct Session {bool practice_enabled=true;u32 practice_tools=0;struct{GameSessionPhase phase=GameSessionPhase::stage;u32 stage=1,frame=0;}state;void(*practice_key_sample)(u32)=nullptr;void wire(GameBattle* battle,const GameSessionInput& edges){battle->set_input(edges);}};
static Session s;static Session* game=&s;static eagler::thprac::PracticeKeyMonitor key_monitor;static bool key_monitor_on=true,lock_running=false;static u32 lock_frames=0,last_sample_stage=0,last_sample_frame=0;static GameBattle battle;
void sample_keys(u32 bits){
    if(!game||!game->practice_enabled||game->state.phase!=GameSessionPhase::stage)return;
    if(key_monitor_on)key_monitor.record(14,bits);
    if(game->state.stage!=last_sample_stage||game->state.frame<last_sample_frame)lock_running=false;
    last_sample_stage=game->state.stage;last_sample_frame=game->state.frame;
    if(game->practice_tools&PrTime){if(!lock_running)lock_frames=0;++lock_frames;lock_running=true;}
    else lock_running=false;
}
static void configure_monitor(){}
extern "C"{
void reset(int enabled,int monitor,int recording,int paused,int lock){s={};game=&s;s.practice_enabled=enabled;s.state.phase=paused?GameSessionPhase::paused:GameSessionPhase::stage;s.practice_tools=lock?PrTime:0;key_monitor={};key_monitor.g_record_key_aps=recording;key_monitor_on=monitor;lock_running=false;lock_frames=last_sample_stage=last_sample_frame=0;updates=observations=0;battle.practice_key_sample=nullptr;battle.player=&battle.storage;battle.player_live=true;configure_monitor();}
void step(u32 raw,u32 effective,int sdl,int tick,int alive,int present){if(sdl)sample_keys(raw);if(tick){battle.player_live=alive;battle.player=present?&battle.storage:nullptr;s.wire(&battle,{effective});battle.update();}++s.state.frame;}
u32 rows(){return key_monitor.g_recorded_keys.size();}u32 bits(u32 i){return key_monitor.g_recorded_keys.at(i);}u32 aps(u32 i){return key_monitor.g_recorded_aps.at(i);}u32 locks(){return lock_frames;}u32 observed(){return observations;}u32 players(){return updates;}
}
