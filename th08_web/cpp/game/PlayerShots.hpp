#pragma once
#include "PresentationVisual.hpp"
#include "ShotResource.hpp"
#include "AnmLayout.hpp"
#include "DamageRegions.hpp"
#include <array>
namespace th08 {
struct PlayerShot {
    AnmVm animation;Vec3 position,history[32],size,velocity;float reserved448=0,speed=0,angle=0;
    Timer timer;i16 damage=0,state=0,kind=0,laser_slot=0,option=0,interval=0;
    u8 focused=0,reserved46d=0;i16 animation_index=0;u8 gauge_bonus=0,reserved471[3]{};
    ShotUpdate update=ShotUpdate::None;ShotDraw draw=ShotDraw::None;ShotHit hit=ShotHit::None;const ShotDefinition* definition=nullptr;
};
static_assert(sizeof(PlayerShot)==0x484&&offsetof(PlayerShot,timer)==0x454&&offsetof(PlayerShot,definition)==0x480);
struct PlayerLaserSlot {Timer timer;PlayerShot* shot=nullptr;};
struct PlayerShotsState {
    PlayerShot shots[128];PlayerLaserSlot lasers[3];const ShotDefinition* laser_definitions[3]{};
    Vec3 position,options[4],enemy_target,target,homing_target;
    float option_angle=0,orbit_angle=0;i32 bomb=0;u32 game_flags=0;
    u8 focused=0,enemy_available=0,youkai_bonus=0,character=0;
    i8 player_state=0;u8 gui_blocked=0,option_active=0,padding=0;Timer shooting_timer;
    DamageRegions regions;Timer collision_timer;i32 time_item_threshold=1;
    u8 time_spell=0,human_bonus=0,effect_counter=0,reserved=0;
};
struct PlayerShotActions {
    virtual ~PlayerShotActions()=default;
    virtual void animation(AnmVm&,i32 script)=0;
    virtual void sound(i32 index,float x)=0;
    virtual bool step_animation(AnmVm&)=0;
    virtual void damage_region(const Vec3& position,const Vec2& size,i32 damage,i32 kind,bool laser)=0;
    virtual void effect(i32 type,const Vec3& position)=0;
    virtual void item(i32 type,const Vec3& position,i32 mode)=0;
    virtual void draw(AnmVm&,bool impact)=0;
};
class PlayerShots {
public:
    PlayerShots(PlayerShotsState& state,Rng& rng):state(state),rng(rng){}
    PlayerShotActions* actions=nullptr;
    FrameTiming timing;
    enum class Failure:u32 {None,InvalidOption,InvalidLaserSlot,MissingActions,InvalidInterval,MissingSprite,MissingDefinition};
    Failure failure=Failure::None;
    bool initialize(PlayerShot& shot,const ShotDefinition& definition);
    bool create(PlayerShot& shot,i32 frame,const ShotDefinition& definition);
    u32 emit(const ShotStream& stream,i32 frame);
    bool update_callback(PlayerShot& shot,ShotUpdate kind);
    bool update();
    bool hit_callback(PlayerShot& shot,const Vec3& position);
    i32 damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit);
    void draw(bool impact,const Vec2& screen_offset);
    void draw_trail(PlayerShot& shot,const Vec2& screen_offset);
private:
    PlayerShotsState& state;Rng& rng;
    struct PresentationSample {Vec3 position{},history[32]{};float angle=0;i32 age=0;i16 state=0,kind=0;bool active=false;presentation::VisualSample visual;};
    std::array<PresentationSample,128> presentation_previous{};
    presentation::SnapshotMarker presentation_marker;
    // A laser tail deliberately leaves its tint in the original VM. Retain
    // the main sprite's authored draw color without changing that game state.
    struct DrawColor {ZunColor color{};i32 age=0;i16 state=0,kind=0,script=-1;bool valid=false;};
    std::array<DrawColor,128> authored_colors{};
    void snapshot_presentation();
    void direction(PlayerShot& shot,float angle,float speed);
};
}
