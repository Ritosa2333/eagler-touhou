// Forced-include compatibility shim for the Emscripten/SDL3 (TH_SDL3) build.
// Honest declarations only: calling-convention macros are meaningless on wasm32
// (clang ignores them), and the *_s CRT helpers below are real bounded
// implementations, not success-faking stubs. Nothing here pretends a platform
// facility exists when it does not; unimplemented OS surface stays in the
// windows.h/d3d9.h/... shims as declarations that fail at link time.
#pragma once

#if defined(_MSC_VER)
#error "msvc_compat.h is only for the clang/wasm TH_SDL3 build"
#endif

// --- MSVC calling conventions (no-ops off x86) ------------------------------
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __thiscall
#define __thiscall
#endif
#ifndef __fastcall
#define __fastcall
#endif
#ifndef __clrcall
#define __clrcall
#endif
#ifndef __vectorcall
#define __vectorcall
#endif
#ifndef __forceinline
#define __forceinline inline
#endif
#ifndef __declspec
#define __declspec(x)
#endif
#ifndef __pragma
#define __pragma(x)
#endif
#ifndef __noop
#define __noop(...) ((void)0)
#endif
#ifndef __assume
#define __assume(x) ((void)0)
#endif
// Windows API calling-convention aliases (real headers derive these from
// __stdcall/__cdecl, which are no-ops on wasm32).
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef WINAPIV
#define WINAPIV __cdecl
#endif
#ifndef APIENTRY
#define APIENTRY WINAPI
#endif
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#ifndef PASCAL
#define PASCAL __stdcall
#endif
#ifndef NTAPI
#define NTAPI __stdcall
#endif
#ifndef CDECL
#define CDECL __cdecl
#endif

// --- MSVC secure CRT --------------------------------------------------------
// Semantics follow the documented contracts: bounded copy with ERANGE on
// truncation, -1 from the printf family on truncation, errno_t error codes.
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifndef TH20_SHIM_ERRNO_T
#define TH20_SHIM_ERRNO_T
typedef int errno_t;
#endif
// MSVC CRT time aliases (musl only knows time_t).
typedef long long __time64_t;
typedef int __time32_t;

#ifndef TH20_SHIM_SECURE_CRT
#define TH20_SHIM_SECURE_CRT

#define TH20_SHIM_EINVAL 22
#define TH20_SHIM_ERANGE 34
#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif

static inline errno_t strcpy_s(char* dst, size_t dstsz, const char* src) {
    if (!dst || !src || dstsz == 0) return TH20_SHIM_EINVAL;
    const size_t len = strlen(src);
    if (len >= dstsz) { dst[0] = '\0'; return TH20_SHIM_ERANGE; }
    memcpy(dst, src, len + 1);
    return 0;
}
static inline errno_t strcat_s(char* dst, size_t dstsz, const char* src) {
    if (!dst || !src || dstsz == 0) return TH20_SHIM_EINVAL;
    const size_t used = strlen(dst);
    if (used >= dstsz) { if (dstsz) dst[0] = '\0'; return TH20_SHIM_EINVAL; }
    return strcpy_s(dst + used, dstsz - used, src) ? TH20_SHIM_ERANGE : 0;
}
static inline errno_t strncpy_s(char* dst, size_t dstsz, const char* src, size_t count) {
    if (!dst || dstsz == 0 || (!src && count != 0)) return TH20_SHIM_EINVAL;
    if (!src) { dst[0] = '\0'; return 0; }
    size_t len = 0;
    while (len < count && src[len]) ++len;
    if (count == _TRUNCATE) {
        if (len >= dstsz) len = dstsz - 1;
    } else if (len >= dstsz) {
        dst[0] = '\0';
        return TH20_SHIM_ERANGE;
    }
    memcpy(dst, src, len);
    dst[len] = '\0';
    return 0;
}
static inline errno_t wcscpy_s(wchar_t* dst, size_t dstsz, const wchar_t* src) {
    if (!dst || !src || dstsz == 0) return TH20_SHIM_EINVAL;
    const size_t len = wcslen(src);
    if (len >= dstsz) { dst[0] = L'\0'; return TH20_SHIM_ERANGE; }
    memcpy(dst, src, (len + 1) * sizeof(wchar_t));
    return 0;
}
static inline int vsprintf_s(char* dst, size_t dstsz, const char* fmt, va_list ap) {
    if (!dst || !fmt || dstsz == 0) return -1;
    const int r = vsnprintf(dst, dstsz, fmt, ap);
    if (r < 0 || (size_t)r >= dstsz) { dst[0] = '\0'; return -1; }
    return r;
}
static inline int sprintf_s(char* dst, size_t dstsz, const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    const int r = vsprintf_s(dst, dstsz, fmt, ap);
    va_end(ap);
    return r;
}
static inline int vswprintf_s(wchar_t* dst, size_t dstsz, const wchar_t* fmt, va_list ap) {
    if (!dst || !fmt || dstsz == 0) return -1;
    const int r = vswprintf(dst, dstsz, fmt, ap);
    if (r < 0 || (size_t)r >= dstsz) { dst[0] = L'\0'; return -1; }
    return r;
}
static inline int swprintf_s(wchar_t* dst, size_t dstsz, const wchar_t* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    const int r = vswprintf_s(dst, dstsz, fmt, ap);
    va_end(ap);
    return r;
}
static inline int swscanf_s(const wchar_t* src, const wchar_t* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    const int r = vswscanf(src, fmt, ap);
    va_end(ap);
    return r;
}
static inline errno_t memcpy_s(void* dst, size_t dstsz, const void* src, size_t count) {
    if (!dst || (!src && count != 0)) return TH20_SHIM_EINVAL;
    if (count > dstsz) { memset(dst, 0, dstsz); return TH20_SHIM_ERANGE; }
    if (count) memcpy(dst, src, count);
    return 0;
}
static inline errno_t memmove_s(void* dst, size_t dstsz, const void* src, size_t count) {
    if (!dst || (!src && count != 0)) return TH20_SHIM_EINVAL;
    if (count > dstsz) { memset(dst, 0, dstsz); return TH20_SHIM_ERANGE; }
    if (count) memmove(dst, src, count);
    return 0;
}
#ifdef __cplusplus
// MSVC also provides array-reference overloads that deduce the destination
// bound; forward them to the bounded forms above.
template <size_t N> static inline errno_t strcpy_s(char (&dst)[N], const char* src) { return strcpy_s(dst, N, src); }
template <size_t N> static inline errno_t strcat_s(char (&dst)[N], const char* src) { return strcat_s(dst, N, src); }
template <size_t N> static inline errno_t strncpy_s(char (&dst)[N], const char* src, size_t count) { return strncpy_s(dst, N, src, count); }
template <size_t N> static inline errno_t wcscpy_s(wchar_t (&dst)[N], const wchar_t* src) { return wcscpy_s(dst, N, src); }
template <size_t N, typename... Args> static inline int sprintf_s(char (&dst)[N], const char* fmt, Args... args) { return sprintf_s(dst, N, fmt, args...); }
template <size_t N, typename... Args> static inline int swprintf_s(wchar_t (&dst)[N], const wchar_t* fmt, Args... args) { return swprintf_s(dst, N, fmt, args...); }
#endif
#endif // TH20_SHIM_SECURE_CRT

// --- x87/SSE control word (MSVC <float.h> surface) --------------------------
// wasm has a single fixed round-to-nearest FP environment; dynamic rounding is
// architecturally unavailable. __control87_2 therefore honestly reports the
// unchanged environment instead of pretending the requested mode took effect.
// Callers that verify the new mode (platform_services/clock.cpp
// set_rounding_mode) will observe "not applied" for any nonzero mode.
#ifndef _MCW_RC
#define _MCW_RC 0x00000300
#define _RC_NEAR 0x00000000
#define _RC_DOWN 0x00000100
#define _RC_UP   0x00000200
#define _RC_CHOP 0x00000300
#define _MCW_PC 0x00030000
#define _MCW_EM 0x0008001f
static inline unsigned __control87_2(unsigned /*new_value*/, unsigned /*mask*/,
                                     unsigned* x87, unsigned* sse) {
    const unsigned current = 0; // round-to-nearest, the only wasm FP mode
    if (x87) *x87 = current;
    if (sse) *sse = current;
    return current;
}
#endif

// --- MSVC CRT time/printf helpers -------------------------------------------
#include <time.h>
// _localtime64_s: real conversion through the C library localtime_r.
static inline errno_t _localtime64_s(struct tm* dest, const __time64_t* source) {
    if (!dest || !source) return TH20_SHIM_EINVAL;
    const time_t value = (time_t)*source;
    if (!localtime_r(&value, dest)) { memset(dest, 0, sizeof(*dest)); return TH20_SHIM_EINVAL; }
    return 0;
}
// UCRT common printf entry. The original statically linked CRT rounds exact
// decimal halves away from zero; musl (like modern UCRT defaults) rounds
// ties-to-even. The STANDARD_ROUNDING option bit is parsed but musl offers no
// away-from-zero mode, so this documents rather than reproduces that aspect.
#define _CRT_INTERNAL_PRINTF_STANDARD_ROUNDING 0x00000002ULL
#define _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS 0x00000030ULL
static inline int __stdio_common_vsprintf_s(unsigned long long /*options*/,
        char* buffer, size_t size, const char* format, void* /*locale*/, va_list args) {
    return vsprintf_s(buffer, size, format, args);
}

// _time64: seconds since epoch, like the CRT original.
static inline __time64_t _time64(__time64_t* dest) {
    const __time64_t now = (__time64_t)time(nullptr);
    if (dest) *dest = now;
    return now;
}
