#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace EaglerOptions
{
inline bool TouchEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.touchEnabled; }) != 0;
#else
    return false;
#endif
}

inline bool UnlimitedTouch()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.unlimitedTouch; }) != 0;
#else
    return false;
#endif
}

inline bool TouchBombZoneEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerOptions?.touchBombZoneEnabled !== false; }) != 0;
#else
    return true;
#endif
}

inline bool AlwaysShowHitbox()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.alwaysHitbox; }) != 0;
#else
    return false;
#endif
}

inline bool ShowTh06FocusHitbox()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.th06FocusHitbox; }) != 0;
#else
    return false;
#endif
}

inline bool TouchFireEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerControls?.fireEnabled !== false; }) != 0;
#else
    return false;
#endif
}

inline i32 TouchBombSerial()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerControls?.bombSerial | 0; });
#else
    return 0;
#endif
}
} // namespace EaglerOptions
