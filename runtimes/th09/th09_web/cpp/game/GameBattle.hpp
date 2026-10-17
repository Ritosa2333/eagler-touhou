#pragma once
#include "GameResources.hpp"
#include "Player.hpp"
#include "EnemyManager.hpp"
#include "BulletVisuals.hpp"
#include "LaserManager.hpp"
#include "EffectManager.hpp"
#include "AttackController.hpp"
#include "MatchRules.hpp"
#include <memory>
namespace th09 {
enum class BattleNotice {begin_charge,end_charge,charge_level,critical_health,begin_survival,survival_time,survival_expired,reward};
enum class BattleGeometry {fan,strip,lines};
enum class BattleSprite {unrotated,rotated,automatic,mirrored};
// Presentation owns backgrounds, HUD, text surfaces and the match scene. These
// callbacks contain no simulated Windows or graphics-device operations.
struct BattlePresentation {
    virtual ~BattlePresentation()=default;
    virtual void sound(i32 id,i32 pan)=0;
    virtual void positioned_sound(i32 side,i32 id,float x)=0;
    virtual void begin_field(i32 side)=0;
    virtual void animation(AnmVm&,BattleSprite mode=BattleSprite::rotated)=0;
    virtual void hitbox(i32,AnmLoaded*,const Vec3&,i32){}
    virtual void colored(const AnmVm*,const AttackColorVertex*,u32,BattleGeometry,bool additive=false)=0;
    virtual void textured(const AnmVm&,const AttackTextureVertex*,u32,BattleGeometry)=0;
    virtual void text(AnmVm&,const char*,u32 color,u32 shadow)=0;
    virtual void score_popup(i32 side,const Vec3&,i32,u32)=0;
    virtual void notice(i32 side,BattleNotice,i32 value=0)=0;
    virtual void damage_flash(i32 side,i32 duration,u32 color)=0;
    virtual void end_round(i32 winner)=0;
    virtual void finish_match()=0;
    virtual void attack_position(i32 side,const Vec3&)=0;
    virtual void boss_indicator(i32 side,i32 slot,i32 state)=0;
    virtual void boss_indicator_position(i32 side,i32 slot,const Vec3&)=0;
    virtual void background_setting(i32 side,i32 value)=0;
    virtual void background_transition(i32 side,i32 state,i32 frames)=0;
    virtual void reset_background(i32 side)=0;
    virtual void boss_background(i32 receiving_side)=0;
    virtual void portrait(i32 side,u32 layer,i32 script)=0;
};
struct BattleState {
    FrameTiming timing;float ecl_step=1;u32 difficulty_mask=255,flags=4;
    // Player limits set by the original playfield initializer (0041a9b8).
    // Using the inner 256-pixel rectangle changes edge movement and CPU dodging.
    PlayfieldLimits limits{{-136,16},{272,416}};
    PlayfieldGeometry geometry[2]{{16,16,{-144,0},288},{336,16,{-144,0},288}};
    i32 scene_phase=0,ending_frames=0,dialogue=-1,extra_damage=0,script_extra_time=0;
    bool rewards_blocked=false,automatic_focus[2]{},hide_players=false;
};
struct BattleField {
    EclPlayfieldState script;EnemyPlayerState enemy_player;
    std::unique_ptr<Player> player;
    std::unique_ptr<BulletManager> bullets;
    std::unique_ptr<BulletVisuals> bullet_visuals;
    std::unique_ptr<LaserManager> lasers;
    std::unique_ptr<EffectManager> effects;
    std::unique_ptr<EnemyManager> enemies;
    std::unique_ptr<AttackController> attacks;
    EffectActor *focus_aura=nullptr,*capture_effect=nullptr,*shield=nullptr;
    EclVm* targeted_enemy=nullptr;
    i32 cpu_level=0,spells=0,bosses=0,counters=0;
};
// A complete pair of live battle fields. Construction is deliberately split at
// field boundaries so scene/background/HUD initialization can keep the original
// RNG order. The enclosing session owns pause, replay input and stage changes.
class GameBattle {
    struct Services;std::unique_ptr<Services> services;
    EclWorldState& world;MatchRules& rules;GameResources& resources;AnmExecutor& animations;BattlePresentation& output;
    bool fail(const char*);
    friend struct Services;
public:
    BattleState state;std::array<BattleField,2> fields;std::array<u8,256> patterns{};
    std::array<AttackBehavior,27> behaviors;
    std::unique_ptr<EffectManager> cross_effects;std::unique_ptr<AttackQueue> attack_queue;
    std::string error;
    GameBattle(EclWorldState&,MatchRules&,GameResources&,AnmExecutor&,BattlePresentation&);
    ~GameBattle();
    GameBattle(const GameBattle&)=delete;GameBattle& operator=(const GameBattle&)=delete;
    void configure(i32 left_character,i32 right_character);
    bool initialize_field(i32 side,i32 input_controller,i32 health);
    bool initialize_shared();
    void sync_players();
    bool update_enemies(i32 side);
    bool update_bullets(i32 side);
    bool update_attacks();
    void update_player(i32 side);
    void update_controller(i32 side);
    void update_effects(i32 side);
    bool begin_attack(i32 receiving_side,i32 variant,i32 level,i32 parameter,const char*);
    void flush_combo(i32 side);
    void clear_hazards(i32 side);
    void reset_player(i32 side,i32 health);
    void reset_field(i32 side,i32 health);
    void reward_enemy(i32 side,i32 reward);
    void draw_enemies(i32 side,bool upper);
    void draw_player(i32 side,bool fading);
    void draw_attacks(bool cross_field);
    void draw_effects(i32 side,i32 layer);
    void draw_bullets(i32 side);
    void draw_controller(i32 side);
};
}
