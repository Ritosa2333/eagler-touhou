#pragma once
#include <algorithm>
namespace th11::high_refresh {
// Presentation-only state for one Draw transaction. Simulation code must never
// read these values; owners publish prev/current endpoints at the fixed tick
// boundary and Draw derives temporary visual values from them.
inline float alpha=1.0f;
inline float world_alpha=1.0f;
inline bool active=false;
inline bool render_only=false;
// High-refresh sessions keep continuous world submissions fractional on
// authoritative tick frames too, so the alpha=1 endpoint is pixel-identical to
// the next reference draw. The 60 Hz kill switch leaves this off.
inline bool smooth_submission=false;
// Maximum position step still treated as continuous motion. Scene/background
// owners lower it so authored world rebases snap instead of blending.
inline float position_limit=128.f;
inline void begin(float value,bool enabled,bool only,bool world_enabled=true) noexcept {
    alpha=enabled?std::clamp(value,0.0f,1.0f):1.0f;
    world_alpha=enabled&&world_enabled?alpha:1.0f;
    active=enabled;
    render_only=only;
}
inline void end() noexcept {alpha=world_alpha=1.0f;active=false;render_only=false;}
inline bool interpolating() noexcept {return active&&render_only;}
inline float lerp(float previous,float current) noexcept {return previous+(current-previous)*alpha;}
inline float lerp_world(float previous,float current) noexcept {return previous+(current-previous)*world_alpha;}
}
