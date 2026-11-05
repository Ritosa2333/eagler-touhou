#pragma once
#include "EnemyManager.hpp"
#include "EnemyCombat.hpp"
#include "EnemyDeath.hpp"
#include "EnemyAnmHost.hpp"
#include "ItemManager.hpp"
#include "LaserScene.hpp"
#include "BombReimu.hpp"
#include "BombMarisa.hpp"
#include "BombSanae.hpp"
#include "BombReisen.hpp"
#include "FrameScheduler.hpp"
#include "SpellAnmHost.hpp"
#include "SessionState.hpp"
namespace th15 {
struct BattleSceneBindings;
class RecordStore;
struct BattleResources {i32 player=10,bullet=7,effect=8;};
struct BattleFrame {
    u32 held=0,pressed=0,game_flags=0;float rate=1;Vec3 background_delta{};
    bool focus_allowed=false,hud_available=true,hud_collect=false,animation_paused=false;
    bool player_enabled=true,bomb_enabled=true,bullets_enabled=true,items_enabled=true;
    bool enemies_enabled=true,lasers_enabled=true,spell_enabled=true;
};
// Stage/session, GUI and presentation services remain explicit until those
// original systems are reconstructed. No missing action silently succeeds.
struct BattleWorldServices:SpellCardServices {
    virtual ~BattleWorldServices()=default;
    virtual bool audio(i32,float pan,PlayerShots::SoundAction,bool queued)=0;
    virtual bool life_hud(i32 lives,i32 pieces)=0;virtual bool bomb_hud(i32 bombs,i32 pieces)=0;
    virtual bool game_over()=0;virtual bool notice(i32)=0;
    virtual bool popup(const Vec3&,i32,u32)=0;
    virtual bool cancellation_effect(i32,const Vec3&,const Vec3&)=0;
    virtual bool graze_spark(const Vec3&)=0;virtual bool graze_flash()=0;
    virtual bool graze_resonance(float)=0;
    virtual bool shake(const ScreenShakeSpec&)=0;virtual bool nudge(const ScreenNudgeSpec&)=0;
    virtual bool bomb_damage(const DamageQuery&,i32& amount)=0;
    virtual int enemy_callback(EnemyRuntime&,float rate)=0;
    virtual bool enemy_additional_damage(EnemyState&,i32,i32&)=0;
    virtual bool enemy_contact(EnemyState&,AnmVm*,i32&,bool& handled)=0;
    virtual bool enemy_distortion(EnemyState&,float rate)=0;
    virtual bool enemy_death_callback(EnemyRuntime&)=0;
    virtual bool background_fog(i32 duration,i32 mode,const StageFog&){return false;}
    virtual bool background_interrupt(i32){return false;}
    virtual bool scene_message(i32){return false;}
    virtual bool message_complete(bool&){return false;}
    virtual bool dialogue_present()const noexcept{return false;}
    virtual bool begin_enemy_distortion(EnemyState&){return false;}
    virtual bool retire_enemy_distortion(EnemyState&){return false;}
    virtual bool boss_segment(i32,i32,float,u32){return false;}
    virtual bool boss_segments(i32){return false;}
    virtual bool stage_logo(){return false;}
};
class GameBattle {
    struct Services;std::unique_ptr<Services> services;
    AnmManager& animations;AnmEnvironment& environment;Rng& game_random;Rng& visual_random;
    BattleWorldServices& presentation;EclProgram& program;BattleResources resources;i32 character;
    RecordStore* records=nullptr;
    bool initialized=false;BattleFrame input;SessionState* progress=nullptr;BattleSceneBindings* scene=nullptr;
    // Callbacks outlive the scheduler that links them.
    std::array<FrameCallback,9> callbacks;FrameScheduler schedule;FrameScheduler* active_schedule=&schedule;
    bool refresh_world();bool refresh_player(PlayerFrameContext&);
    bool spawn_item(i32,const Vec3&,float,float);bool death_effect(const EnemyDeathEffect&);
    bool cleanup_enemy(EnemyState&);bool sound(i32,float,PlayerShots::SoundAction,bool=false);
    bool check(bool,const char*);bool fail(const std::string&);
    bool life_hud(i32,i32);bool bomb_hud(i32,i32);bool popup(const Vec3&,i32,u32);bool dialogue_present()const noexcept;
    i32 update_animations(bool alternate);i32 update_player();i32 update_bomb();i32 update_enemies();
    i32 update_lasers();i32 update_bullets();i32 update_items();
    i32 update_spell();
public:
    static constexpr std::array<i32,9> update_priorities{9,23,25,26,27,28,31,29,34};
    EnemyWorldState enemy_world;PlayerLifeSession session;PlayerSpellStatus spell;ItemScoreState score;BombEnemyState bomb_enemies;
    i32 stage=1;SpellVisualResources spell_visuals;
    SpellAnmHost spell_animations;SpellCard spell_card;
    EnemyAnmHost enemy_visuals;EffectManager effects;
    std::unique_ptr<Player> player;std::unique_ptr<ItemCollection> collection;std::unique_ptr<ItemManager> items;
    std::unique_ptr<BulletScene> bullet_scene;std::unique_ptr<LaserScene> laser_scene;
    std::unique_ptr<EnemyCombat> combat;std::unique_ptr<EnemyDamage> enemy_damage;std::unique_ptr<EnemyDeath> enemy_death;
    std::unique_ptr<EnemyManager> enemies;std::unique_ptr<BombContext> bomb_context;std::unique_ptr<BombController> bomb;
    std::string error;
    std::function<bool()> game_over_begin;
    GameBattle(AnmManager&,AnmEnvironment&,Rng& game,Rng& visual,BattleWorldServices&,EclProgram&,const ShtResource&,i32 character,BattleResources);
    ~GameBattle();
    bool record_graze(const Vec3&);void graze_flash()noexcept{player->motion.barrier_timer.set(10);}void graze_resonance(float value)noexcept{items->motion_scale=value;}
    bool initialize(bool configure_player=true);bool prepare_frame(const BattleFrame&);bool step(const BattleFrame&);bool spawn(const EnemySpawnRequest&);
    bool attach(FrameScheduler&);BattleFrame& active_frame()noexcept{return input;}
    bool detach(FrameScheduler&);bool replace_stage(EclProgram&,const std::array<i32,6>&,const SpellVisualResources&);
    bool release_stage_enemies();bool retire_bomb_visuals();bool retire_spell_visuals();
    void disable_bullet_callback()noexcept{callbacks[5].enabled=false;}
    void bind_progress(SessionState&,i32* requested_chapter=nullptr)noexcept;
    void bind_records(RecordStore* owner)noexcept{records=owner;}
    void bind_scene(BattleSceneBindings& owner)noexcept{scene=&owner;}
    void unbind_scene(BattleSceneBindings& owner)noexcept{if(scene==&owner)scene=nullptr;}
    const PlayerCollision& player_collision()noexcept;
    const BattleFrame& frame_state()const noexcept{return input;}
    i32 selected_character()const noexcept{return character;}
};
}
