// Shared Launcher gamepad action boundary. Generated copies are synchronized
// by tools/sync-gamepad-input.mjs. No device button indices cross this boundary.
#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
namespace eagler { namespace input {
inline bool BrowserGamepadOwned() {
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return globalThis.__eaglerGamepadInputBridge?.owned() ? 1 : 0; }) != 0;
#else
    return false;
#endif
}
// -1 keeps the original native gamepad owner. 0 is a valid neutral Launcher
// snapshot, including blur, disconnect, configuration and spectator states.
inline int BrowserGamepadState(unsigned seat=0) {
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return globalThis.__eaglerGamepadInputBridge?.sample($0) ?? -1; }, seat);
#else
    (void)seat; return -1;
#endif
}
template<class Keys> inline void ApplyGamepadVirtualKeys(Keys& keys,int bits) {
    if(bits<0)return;
    constexpr unsigned vk[]={90,88,16,27,38,40,37,39,17,13,67,82};
    for(unsigned n=0;n<12;++n)if(bits&(1<<n))keys[vk[n]]=128;
    if(bits&4)keys[160]=128;
    if(bits&256)keys[162]=128;
}
template<class Keys> inline void ApplyGamepadScanKeys(Keys& keys,int bits) {
    if(bits<0)return;
    constexpr unsigned scans[]={44,45,42,1,200,208,203,205,29,28,46,19};
    for(unsigned n=0;n<12;++n)if(bits&(1<<n))keys[scans[n]]=128;
}
}}
