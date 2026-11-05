#pragma once
#include <algorithm>
#include <cstdint>
namespace th08::presentation {
// Replay/menu shortcuts can restart the retail calculation chain several
// times before one authored Draw. Endpoints span that entire draw interval,
// not just its final internal calculation. No logical update is skipped.
inline uint64_t calculation_epoch=0;
inline bool calculation_open=false;
struct CalculationScope {
    bool owns=!calculation_open;
    CalculationScope(){if(owns){calculation_open=true;++calculation_epoch;}}
    ~CalculationScope(){if(owns)calculation_open=false;}
    CalculationScope(const CalculationScope&)=delete;
};
struct SnapshotMarker {
    uint64_t last_epoch=~uint64_t(0);
    bool capture(){if(!calculation_open)return true;if(last_epoch==calculation_epoch)return false;last_epoch=calculation_epoch;return true;}
};
inline float alpha=1.0f;
inline float world_alpha=1.0f;
inline bool active=false;
inline bool render_only=false;
#if defined(TH_PRESENTATION_AUDIT)
inline bool audit_hold_interpolation=false;
#endif
inline void begin(float value,bool enabled,bool only,bool world_enabled=true)noexcept{
    alpha=enabled?std::clamp(value,0.0f,1.0f):1.0f;
    world_alpha=enabled&&world_enabled?alpha:1.0f;
    active=enabled;
    render_only=only;
}
inline void end()noexcept{alpha=world_alpha=1.0f;active=false;render_only=false;}
inline float lerp(float previous,float current)noexcept{
#if defined(TH_PRESENTATION_AUDIT)
    if(audit_hold_interpolation)return current;
#endif
    if(alpha>=1)return current;if(alpha<=0)return previous;
    return previous+(current-previous)*alpha;
}
inline float lerp_world(float previous,float current)noexcept{
#if defined(TH_PRESENTATION_AUDIT)
    if(audit_hold_interpolation)return current;
#endif
    if(world_alpha>=1)return current;if(world_alpha<=0)return previous;
    return previous+(current-previous)*world_alpha;
}
}
