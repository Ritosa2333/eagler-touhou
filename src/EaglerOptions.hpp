#pragma once

#include "inttypes.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace EaglerOptions
{
inline bool LimitPresentationTo60()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.limitPresentationTo60; }) != 0;
#else
    return false;
#endif
}

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

inline bool TouchMovementIsJoystick()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return (Module.eaglerOptions?.touchMovementMode || 'touch') === 'joystick'; }) != 0;
#else
    return false;
#endif
}

inline bool TouchMovementIsFreeJoystick()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return (Module.eaglerOptions?.touchMovementMode || 'touch') === 'joystick-free'; }) != 0;
#else
    return false;
#endif
}

inline bool TouchMovementUsesJoystick()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        const mode = Module.eaglerOptions?.touchMovementMode || 'touch';
        return mode === 'joystick' || mode === 'joystick-free';
    }) != 0;
#else
    return false;
#endif
}

inline f32 TouchSensitivity()
{
#ifdef __EMSCRIPTEN__
    return static_cast<f32>(EM_ASM_DOUBLE({
        const value = Number(Module.eaglerOptions?.touchSensitivity);
        if (!Number.isFinite(value)) return 1.0;
        return Math.min(300, Math.max(50, value)) / 100.0;
    }));
#else
    return 1.0f;
#endif
}

inline bool DoubleTapBombEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.doubleTapBombEnabled; }) != 0;
#else
    return false;
#endif
}

inline i32 TouchJoystickX()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerControls?.joystickX | 0; });
#else
    return 0;
#endif
}

inline i32 TouchJoystickY()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerControls?.joystickY | 0; });
#else
    return 0;
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

inline bool TouchFocusUsesTwoFingers()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return (Module.eaglerOptions?.touchFocusMode || 'hold-button') === 'two-finger'; }) != 0;
#else
    return true;
#endif
}

inline bool TouchFocusButtonEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerControls?.focusEnabled; }) != 0;
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

inline i32 TouchEscapeSerial()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerControls?.escapeSerial | 0; });
#else
    return 0;
#endif
}

inline u16 BrowserKeyboardBits()
{
#ifdef __EMSCRIPTEN__
    return static_cast<u16>(EM_ASM_INT({
        if (!Module.eaglerControls) return 0;
        const held = Module.eaglerControls.keyboardBits | 0;
        const pulse = Module.eaglerControls.keyboardPulseBits | 0;
        Module.eaglerControls.keyboardPulseBits = 0;
        return held | pulse;
    }));
#else
    return 0;
#endif
}

inline u16 BrowserGamepadDirectionBits()
{
#ifdef __EMSCRIPTEN__
    return static_cast<u16>(EM_ASM_INT({
        if (!navigator.getGamepads) return 0;
        const pads = navigator.getGamepads();
        let bits = 0;
        for (const pad of pads) {
            if (!pad || !pad.buttons || pad.buttons.length < 16) continue;
            const id = String(pad.id || 0);
            // Real gamepads remain exclusively owned by SDL's selected
            // g_Supervisor.gameController. This browser-side path only repairs
            // Android devices such as NX87 that expose keyboard D-pad keys as
            // a Gamepad API device and therefore never emit KeyboardEvent.
            if (!/keyboard|\bkb\b/i.test(id)) continue;
            if (pad.buttons[12]?.pressed) bits |= 1 << 4;
            if (pad.buttons[13]?.pressed) bits |= 1 << 5;
            if (pad.buttons[14]?.pressed) bits |= 1 << 6;
            if (pad.buttons[15]?.pressed) bits |= 1 << 7;
        }
        return bits;
    }));
#else
    return 0;
#endif
}

inline void ResetBrowserKeyboard()
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        if (Module.eaglerControls) {
            Module.eaglerControls.keyboardBits = 0;
            Module.eaglerControls.keyboardPulseBits = 0;
        }
    });
#endif
}
} // namespace EaglerOptions
