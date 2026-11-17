#pragma once
#include <algorithm>
namespace th10::high_refresh {
// Weight 1 is current; live high-refresh draws use 1 + display phase.
// Values below 1 remain available to explicit interpolation diagnostics.
inline float alpha=1.0f;
inline float world_alpha=1.0f;
inline bool active=false;
inline bool render_only=false;
inline void begin(float value,bool enabled,bool only,bool world_enabled=true)noexcept{
    alpha=enabled?std::clamp(value,0.0f,2.0f):1.0f;
    world_alpha=enabled&&world_enabled?alpha:1.0f;
    active=enabled;
    render_only=only;
}
inline void end()noexcept{alpha=world_alpha=1.0f;active=false;render_only=false;}
inline float lerp(float previous,float current)noexcept{return previous+(current-previous)*alpha;}
inline float lerp_world(float previous,float current)noexcept{return previous+(current-previous)*world_alpha;}
}
