#pragma once
#include "PlayerBombObjects.hpp"
#include "ScreenEffects.hpp"
#include "Rng.hpp"
#include "PlayerOptions.hpp"
#include "PresentationVisual.hpp"
namespace th08 {
struct PlayerBombPatternContext {FrameTiming timing;Vec3 homing_target{-999,-999,0};PlayerOption* options=nullptr;Timer shooting_timer;AnmVm* main_animation=nullptr;};
struct PlayerBombPatternActions:PlayerBombStartActions {
    virtual void animation(AnmVm&,i32 script,bool effect_bank)=0;
    virtual bool step_animation(AnmVm&)=0;
    virtual EffectState* spawn_effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual EffectState* parameter_effect(i32 kind,const Vec3& position,const Vec3& parameters,i32 slot,u32 color)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void panned_sound(i32 index,float x)=0;
    virtual void screen(ScreenEffectType type,i32 duration,i32 a,i32 b,i32 c,i32 priority)=0;
    // Reset renderer mix, compose the stage tint and enable that tint this frame.
    virtual void background_color(u32 color)=0;
    // Draw-side renderer mix reset; also runs in render-only presentation passes.
    virtual void reset_screen_color()=0;
    virtual void draw(AnmVm&,bool rotated)=0;
    virtual void rectangle(float left,float top,float right,float bottom,u32 color)=0;
};
class PlayerBombPatterns {
    PlayerBombObjects& objects;PlayerBombState& bomb;PlayerLifeState& life;PlayerLifeContext& context;
    PlayerMovementState& movement;PlayerBombContext& input;DamageRegions& regions;Rng& rng;PlayerBombPatternActions& actions;
    struct PresentationSample {Vec3 position{};float angle=0;i32 state=0,age=0;i16 script=-1;presentation::VisualSample visual;};
    PresentationSample presentation_previous[128]{};
    // Only object zero uses multiple ANM parts (Master Spark / Yukari cut-in).
    presentation::VisualSample presentation_additional[7]{};
    i32 presentation_additional_age[7]{};
    presentation::SnapshotMarker presentation_marker;
    void snapshot_presentation();
    void presentation_visual(u32 index,u32 part,AnmVm& draw)const;
    Vec3 presentation_position(u32 index)const;
    float presentation_angle(u32 index)const;
    void begin(PlayerBombKind kind,i32 sprite,i32 duration,i32 invincibility,i32 variant);
    void step(AnmVm*,u32 count);
    void marisa(bool last);void yukari(bool last);void last_word();void reimu(bool last);void alice(bool last);void draw_alice();void remilia(bool last);
    void reimu_begin(bool last);void reimu_explode(PlayerBombObject&,bool last);void draw_reimu(bool last,const Vec2&);
    void sakuya(bool last);void draw_sakuya(bool last,const Vec2&);
    void yuyuko(bool last);void draw_yuyuko(bool last,const Vec2&);
    void youmu(bool last);void draw_youmu(bool last);
    bool draw_marisa(const Vec2&);void draw_yukari(const Vec2&);void tint(u32 color);
public:
    PlayerBombPatternContext frame;
    PlayerBombPatterns(PlayerBombObjects& objects,PlayerBombState& bomb,PlayerLifeState& life,PlayerLifeContext& context,PlayerMovementState& movement,PlayerBombContext& input,DamageRegions& regions,Rng& rng,PlayerBombPatternActions& actions)
        :objects(objects),bomb(bomb),life(life),context(context),movement(movement),input(input),regions(regions),rng(rng),actions(actions){}
    // False identifies a callback that has not yet been recovered or invalid data.
    bool update(PlayerBombKind);
    bool draw(PlayerBombKind,const Vec2& offset);
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_presentation_sample(uintptr_t object,u32 part)const;
#endif
};
}
