#pragma once
#include "PresentationVisual.hpp"
#include "PlayerSetup.hpp"
#include "PlayerFrame.hpp"
#include "PlayerBombPatterns.hpp"
#include "PlayerCollision.hpp"
#include "PlayerGraze.hpp"
namespace th08 {
// One owner for the state shared by all recovered player phases. In particular,
// shots, spells and the effect system use the same damage/cancellation pools.
struct PlayerSimulationState {
    PlayerMotionState motion;PlayerLifeState life;PlayerLifeContext context;PlayerBombState bomb;PlayerBombContext bomb_input;
    PlayerShotsState shots;PlayerBombObjects bomb_objects;PlayerFrameState frame;PlayerMotionInput input;Vec3 enemy_origin;i32 cancel_item=6;Timer item_gauge_lock;
    // GameManager 0164d2c8, advanced before the player callback. Player's
    // statistics at 0164d318 are advanced afterwards and exclude dialogue.
    i32 stage_play_frames=0;
    PlayerSimulationState(){reset();}
    void reset(){std::memset(this,0,sizeof(*this));}
};
struct PlayerSimulationWorld {
    virtual ~PlayerSimulationWorld()=default;
    virtual bool gui_blocked()=0;
    // Active thprac cheat mask (0 when practice is disabled or replaying).
    virtual u32 practice_cheats()=0;
    virtual i32 hud_state()=0;
    virtual void hud_interrupt(i32)=0;
    virtual void add_score(i32)=0;
    virtual bool boss_present()=0;
    virtual void randomize_integrity()=0;
};
struct PlayerSimulationServices {
    PlayerSetupActions& setup;PlayerMotionActions& motion;PlayerLifeActions& life;PlayerBombActions& bomb;
    PlayerBombPatternActions& patterns;PlayerShotActions& shots;PlayerSimulationWorld& world;
};
class PlayerSimulation:private PlayerFrameActions,private PlayerBombActions,private ShotFiringActions,private PlayerCollisionActions,private PlayerGrazeActions {
    PlayerSimulationState& state;ShotResource (&resources)[2];GameGauge& gauge;GaugeThresholds& thresholds;PlayerSimulationServices services;
    GameGlobals& values;GameRank& rank;
    PlayerLife life;PlayerShots shots;PlayerBombPatterns patterns;PlayerCollision collisions;bool failed=false,initialized=false;
    Vec3 presentation_previous_position{};Vec3 presentation_previous_options[4]{};
    Vec2 presentation_previous_scale{};ZunColor presentation_previous_color{};
    presentation::VisualSample presentation_previous_animation,presentation_previous_option_animation[4];
    presentation::SnapshotMarker presentation_marker;
    i32 presentation_previous_life_state=0,presentation_previous_option_state[4]{};i16 presentation_previous_script=-1,presentation_previous_sprite=-1;bool presentation_valid=false;
    void synchronize_shots();
    void update_bomb()override;
    bool resolve_death()override;
    void respawn()override;
    void update_invincibility()override;
    void update_motion()override;
    void step_animation(AnmVm&)override;
    void update_shots()override;
    void update_shooting()override;
    bool gui_blocked()override{return services.world.gui_blocked();}
    bool invincible()override{return services.world.practice_cheats()&1;}
    i32 hud_state()override{return services.world.hud_state();}
    void hud_interrupt(i32 value)override{services.world.hud_interrupt(value);}
    void add_score(i32 value)override{services.world.add_score(value);}
    void update(PlayerBombKind)override;
    void finish_spell_overlay()override{services.bomb.finish_spell_overlay();}
    void defeat_boss(u32 slot)override{services.bomb.defeat_boss(slot);}
    void last_word_flash()override{services.bomb.last_word_flash();}
    void add_gauge(i16 value,bool force)override{services.bomb.add_gauge(value,force);}
    void sound(i32 index,i32 mode)override{services.bomb.sound(index,mode);}
    void reset_screen_color()override{services.bomb.reset_screen_color();}
    void add_bombs(i32 value)override{services.bomb.add_bombs(value);}
    void set_bombs(i32 value)override{services.bomb.set_bombs(value);}
    void count_bomb(i32 value)override{services.bomb.count_bomb(value);}
    void subtract_rank(i32 value)override{services.bomb.subtract_rank(value);}
    void fail_spell_with_bomb()override{services.bomb.fail_spell_with_bomb();}
    void fire(i32 frame)override;
    // Collision invokes 0044e160 before PlayerLife::die invokes 00406e50.
    void randomize_integrity()override{services.world.randomize_integrity();}
    void graze(const Vec3&,bool laser)override;
    void effect(i32 kind,const Vec3& p,i32 count,u32 color)override{services.life.effect(kind,p,count,color);}
    void sound(i32 index,float x)override{services.shots.sound(index,x);}
    void item(i32 kind,const Vec3& p,i32 mode)override{services.shots.item(kind,p,mode);}
public:
    FrameTiming timing;
    PlayerSimulation(PlayerSimulationState&,ShotResource (&)[2],GameGlobals&,GameGauge&,GaugeThresholds&,GameRank&,Rng&,PlayerSimulationServices);
    bool initialize(const PlayerSetupContext&);
    bool update();
    void die()override;
    PlayerCollision& collision()noexcept{return collisions;}
    PlayerSimulationState& status()noexcept{return state;}
    const ShotProfile& profile(bool focused)const noexcept{return resources[focused].settings();}
    i32 damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit);
    bool draw(const Vec2& screen_offset,bool impacts=false);
    bool invalid()const noexcept{return failed;}
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_bomb_presentation(uintptr_t object,u32 part)const{return patterns.audit_presentation_sample(object,part);}
#endif
};
}
