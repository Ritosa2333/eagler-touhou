#include "Stage.hpp"
#include "ScreenEffect.hpp"
namespace th10 {
// 0x4043d0 / 0x404410 / 0x404420.
void Stage::apply_frame_effect(const Vec3& position,const Vec3& size) noexcept {frame_effect=1;effect_position=position;effect_size=size;}
void Stage::enable_effects() noexcept {effects_enabled=1;}
void Stage::disable_effects(const AnmRegistry& registry) noexcept {effects_enabled=0;registry.request_delete(effect_ids[0]);registry.request_delete(effect_ids[1]);}
static void begin_stage_fade(Stage& stage,float* rate,i32 frames,u32 flag){if(!(stage.fade_timer_flags&1)){stage.fade_timer.rate=rate;stage.fade_timer_flags|=1;}stage.fade_timer.previous=frames-1;stage.fade_timer.current=frames;stage.fade_timer.fractional=Extended::from_int(frames).to_float();stage.draw_flags|=flag;}
// 0x404530. The overlay and stage timers intentionally run in opposite directions.
void Stage::fade_to_black(ScreenEffectEnvironment& env){ScreenEffect::create(ScreenEffectKind::HidePlayfield,30,0,0,0,15,env);begin_stage_fade(*this,env.rate,30,2);}
void Stage::fade_in(float* rate) noexcept {begin_stage_fade(*this,rate,60,4);}
}
