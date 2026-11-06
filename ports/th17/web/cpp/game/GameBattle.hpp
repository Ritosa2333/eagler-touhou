#pragma once
#include "EnemyManager.hpp"
#include "EnemyCombat.hpp"
#include "EnemyDeath.hpp"
#include "EnemyAnmHost.hpp"
#include "EnemyCallbacks.hpp"
#include "ItemSpirits.hpp"
#include "LaserScene.hpp"
#include "FrameScheduler.hpp"
#include "BombReimu.hpp"
#include "BombMarisa.hpp"
#include "BombYoumu.hpp"
#include "ScreenEffects.hpp"
#include "SpellAnmHost.hpp"
#include "Hud.hpp"
#include "Dialogue.hpp"
#include "GameInput.hpp"
namespace th17 {
struct BattleResources {i32 player=10,bullet=7,effect=8,animal=30,text=6;std::array<i32,6> enemy{{7,8,-1,-1,-1,-1}};};
struct CoopPlayerInput {u32 held=0;PlayerTouch touch;};
struct BattleFrame {
    u32 held=0,pressed=0,game_flags=0;float rate=1;Vec3 background_delta{};i32 shoot_frames=0,skip_frames=0;
    bool focus_allowed=true,hud_available=true,hud_collect=false,animation_paused=false,practice=false;
    bool player_enabled=true,bomb_enabled=true,enemies_enabled=true,lasers_enabled=true,bullets_enabled=true,items_enabled=true,spirits_enabled=true;
    std::array<CoopPlayerInput,2> partners{};
};
// Presentation and scene progression have explicit owners. A development
// fixture may capture these boundaries, but an unavailable action is an error.
struct BattleWorldServices {
    virtual ~BattleWorldServices()=default;
    virtual bool rollback_boundary(){return true;}
    virtual bool audio(i32,float,PlayerShots::SoundAction,bool)=0;
    virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
    virtual bool game_over()=0;virtual bool notice(i32)=0;
    virtual bool popup(const Vec3&,i32,u32)=0;virtual bool trophy(i32)=0;
    virtual bool scene_message(i32)=0;virtual bool message_complete(bool&)=0;
    virtual bool dialogue_present()const noexcept=0;
    virtual bool spell_background_visible(bool)=0;virtual bool spell_prepare_hud(bool)=0;
    virtual bool spell_title(AnmVm&,const std::string&)=0;virtual bool spell_result(i32,bool)=0;
    virtual bool chapter(i32)=0;virtual bool boss_segment(i32,i32,float,u32)=0;
    virtual bool boss_segments(i32)=0;virtual bool stage_logo()=0;
    virtual bool background_interrupt(i32)=0;
};
class GameBattle {
    struct Services;std::unique_ptr<Services> services;
    struct Partner;
    std::array<std::unique_ptr<Partner>,2> partners;
    u32 partner_count=0;
    bool primary_retired=false;
    AnmManager& animations;AnmEnvironment& environment;Rng& game_random;Rng& visual_random;BattleWorldServices& presentation;BattleResources resources;
    std::array<FrameCallback,9> callbacks;FrameCallback spell_callback,hud_callback;FrameScheduler schedule;FrameScheduler* active_schedule=&schedule;
    SpellStartContext spell_context;
    HudStartContext hud_context;
    const StageConfiguration* dialogue_stage=nullptr;const MessageResource* message_resource=nullptr;DialogueServices* dialogue_host=nullptr;DialogueResources dialogue_resources;
    BattleFrame input;bool initialized=false,world_running=true;std::vector<PlayerShotTarget> targets;
    bool fail(const std::string&);bool check(bool,const char*);
    bool sound(i32,float=0,PlayerShots::SoundAction=PlayerShots::SoundAction::play,bool queued=false);
    bool spawn_item(i32,const Vec3&,float,float);bool cleanup_enemy(EnemyState&);
    bool death_effect(const EnemyDeathEffect&);bool scene_effect(const EnemyEffectRequest&);
    bool refresh_world();bool refresh_player(PlayerFrameContext&);
    bool refresh_partner(Player&,PlayerFrameContext&);
    BombController* pilot_bomb(u32 seat) const noexcept;
    bool pilot_game_over(u32 seat);
    bool pilot_active(u32 seat) const noexcept;
    Vec3 aim_target(const Vec3& origin) noexcept;
    void protect_teammates(u32 caster,i32 frames) noexcept;
    i32 update_animations(bool);i32 update_player();i32 update_bomb();i32 update_enemies();i32 update_lasers();i32 update_bullets();i32 update_items();i32 update_spirits();i32 update_spell();i32 update_hud();
    bool life_hud(i32,i32,u32 seat=0);bool bomb_hud(i32,i32,u32 seat=0);
    bool scene_message(i32);bool message_complete(bool&);bool update_dialogue();
    ItemSpiritFrame item_frame()const;
public:
    // Taken from TH17 registration sites, not inherited from another game.
    static constexpr std::array<i32,9> update_priorities{{10,23,25,27,28,29,30,31,35}};
    EnemyWorldState enemy_world;PlayerLifeSession session;PlayerSpellStatus spell;ItemScoreState score;
    EnemyAnmHost enemy_visuals;EffectManager effects;ScreenGridViewport grid_viewport;ScreenEffects screen_effects;
    std::unique_ptr<Player> player;std::unique_ptr<ItemSpirits> items;
    std::unique_ptr<BombContext> bomb_context;std::unique_ptr<BombController> bomb;
    std::unique_ptr<SpellAnmHost> spell_visuals;std::unique_ptr<SpellCard> spell_card;
    std::unique_ptr<Hud> hud;
    std::unique_ptr<Dialogue> dialogue;
    std::unique_ptr<BulletScene> bullet_scene;std::unique_ptr<LaserScene> laser_scene;
    std::unique_ptr<EnemyCombat> combat;std::unique_ptr<EnemyDamage> enemy_damage;std::unique_ptr<EnemyDeath> enemy_death;std::unique_ptr<EnemyManager> enemies;
    bool enhanced_practice=false;
    i32 character=0,stage=1,graze_total=0,graze_chapter=0;std::string error;
    bool historical_web_replay=false;
    GameBattle(AnmManager&,AnmEnvironment&,Rng&,Rng&,BattleWorldServices&,EclProgram&,const ShtResource&,i32,BattleResources={});
    ~GameBattle();
    bool initialize(bool initialize_roaring_hud=true);bool spawn(const EnemySpawnRequest&);
    bool replace_stage(class StageAssets&);
    bool configure_spell(SpellRecords&,SpellVisualResources);
    bool configure_hud(const HudStartContext&,HudResources={},bool initialize_now=true);
    bool configure_dialogue(const StageConfiguration&,const MessageResource&,DialogueServices&,DialogueResources);
    bool dialogue_present()const noexcept{return dialogue||presentation.dialogue_present();}
    bool begin_spell(const SpellStartRequest&);bool finish_spell();
    bool prepare_frame(const BattleFrame&);bool step(const BattleFrame&);
    void world_active(bool active)noexcept;
    bool attach(FrameScheduler&);bool detach(FrameScheduler&);
    bool add_coop_player(const ShtResource&,i32 character,i32 animal,i32 bank);
    u32 pilot_count() const noexcept {return partner_count+1;}
    u32 viable_pilots() const noexcept;
    i32 boss_damage(EnemyState&,i32);
    Player& pilot(u32 seat) noexcept;
    i32 life_piece_tier(u32 seat) const noexcept;
    void restore_piece_tier(u32 seat,i32 value) noexcept;
    bool initialize_partner_hud();
    void save_rollback(multiplayer::FrameImage&);
    bool reset_players();
    bool finish_player_options();
    bool stop_bombs();
    bool draw_players(AnmRenderer&);
    const PlayerCollision& player_collision(u32 seat=0)noexcept;
    bool record_graze(const Vec3&,u32 seat=0);
    const BattleFrame& frame_state()const noexcept{return input;}
    BattleFrame& active_frame()noexcept{return input;}
};
}
