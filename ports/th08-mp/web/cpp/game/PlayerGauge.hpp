#pragma once
#include "GameGauge.hpp"
#include "PlayerForm.hpp"
#include "EffectState.hpp"
namespace th08 {
struct PlayerGaugeState {Timer idle;EffectState* effect=nullptr;};
struct PlayerGaugeActions {virtual ~PlayerGaugeActions()=default;virtual EffectState* gauge_effect(const Vec3& position)=0;};
// Gauge/indicator phase of 0044aec0, after the option and shooting trigger phases.
void update_player_gauge(PlayerGaugeState&,PlayerFormState&,const Timer& shooting,GameGauge&,bool gui_blocked,i32 bomb,const Vec3& position,const FrameTiming&,PlayerGaugeActions&);
}
