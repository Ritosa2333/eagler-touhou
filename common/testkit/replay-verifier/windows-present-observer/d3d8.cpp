#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>

struct IDirect3D8;
struct IDirect3DDevice8;
using D3DDEVTYPE = int;
struct D3DPRESENT_PARAMETERS;

#ifndef TH_GAME
#error TH_GAME must be 6 or 7
#endif

namespace {
constexpr std::uint32_t MAGIC = 0x52565045; // EPVR
#pragma pack(push, 1)
struct Row {
    std::uint32_t magic, game, active, frame, stage, clock;
    std::uint32_t rng, rngBackup, rngCalls, rawInput, input, lastRawInput, lastInput;
    float x, y;
    std::uint32_t playerState;
};
#pragma pack(pop)
static_assert(sizeof(Row) == 64);

using Direct3DCreate8Fn = IDirect3D8 *(WINAPI *)(UINT);
using CreateDeviceFn = HRESULT (STDMETHODCALLTYPE *)(IDirect3D8 *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, IDirect3DDevice8 **);
using PresentFn = HRESULT (STDMETHODCALLTYPE *)(IDirect3DDevice8 *, const RECT *, const RECT *, HWND, const RGNDATA *);
HMODULE realModule;
Direct3DCreate8Fn realCreate;
CreateDeviceFn realCreateDevice;
PresentFn realPresent;
HANDLE output = INVALID_HANDLE_VALUE;

void EnsureOutput() {
    if (output == INVALID_HANDLE_VALUE)
        output = CreateFileW(L"replay-verifier-present.bin", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

template <typename T> void Patch(void **slot, T replacement, T &original) {
    DWORD oldProtect;
    VirtualProtect(slot, sizeof(void *), PAGE_EXECUTE_READWRITE, &oldProtect);
    original = reinterpret_cast<T>(*slot);
    *slot = reinterpret_cast<void *>(replacement);
    VirtualProtect(slot, sizeof(void *), oldProtect, &oldProtect);
}

void Capture() {
    Row row{}; row.magic = MAGIC; row.game = TH_GAME;
#if TH_GAME == 7
    auto replay = *reinterpret_cast<std::uint32_t **>(0x004B9E48);
    row.active = replay && replay[0x44 / 4] == 1;
    if (replay) row.frame = replay[0];
    row.stage = *reinterpret_cast<std::uint32_t *>(0x01347B00 + 0x4C8);
    row.clock = *reinterpret_cast<std::uint32_t *>(0x01347B00 + 0x4C4);
    row.rng = *reinterpret_cast<std::uint16_t *>(0x0049FE20);
    row.rngBackup = *reinterpret_cast<std::uint16_t *>(0x0049FE22);
    row.rngCalls = *reinterpret_cast<std::uint32_t *>(0x0049FE24);
    row.rawInput = *reinterpret_cast<std::uint16_t *>(0x004B9E4C);
    row.input = *reinterpret_cast<std::uint16_t *>(0x004B9E50);
    row.lastRawInput = *reinterpret_cast<std::uint16_t *>(0x004B9E54);
    row.lastInput = *reinterpret_cast<std::uint16_t *>(0x004B9E58);
    row.x = *reinterpret_cast<float *>(0x004BDAD8 + 0x930);
    row.y = *reinterpret_cast<float *>(0x004BDAD8 + 0x934);
#elif TH_GAME == 6
    auto replay = *reinterpret_cast<std::uint32_t **>(0x006D3F18);
    const auto isInReplay = *reinterpret_cast<std::uint32_t *>(0x0069BCA0 + 0x1C);
    row.active = replay && replay[2] == 1 && isInReplay == 1;
    if (replay) row.frame = replay[0];
    row.stage = *reinterpret_cast<std::uint32_t *>(0x0069BCA0 + 0x1A34);
    row.clock = *reinterpret_cast<std::uint32_t *>(0x0069BCA0 + 0x1A30);
    row.rng = *reinterpret_cast<std::uint16_t *>(0x0069D8F8);
    row.rngCalls = *reinterpret_cast<std::uint32_t *>(0x0069D8FC);
    row.rawInput = *reinterpret_cast<std::uint16_t *>(0x0069D904);
    row.input = row.rawInput;
    row.lastRawInput = *reinterpret_cast<std::uint16_t *>(0x0069D908);
    row.lastInput = row.lastRawInput;
    row.x = *reinterpret_cast<float *>(0x006CA628 + 0x440);
    row.y = *reinterpret_cast<float *>(0x006CA628 + 0x444);
    row.playerState = *reinterpret_cast<std::uint8_t *>(0x006CA628 + 0x9E0);
#endif
    EnsureOutput();
    if (output != INVALID_HANDLE_VALUE) { DWORD written; WriteFile(output, &row, sizeof(row), &written, nullptr); }
}

HRESULT STDMETHODCALLTYPE HookPresent(IDirect3DDevice8 *self, const RECT *source, const RECT *dest, HWND window, const RGNDATA *dirty) {
    Capture();
    return realPresent(self, source, dest, window, dirty);
}

HRESULT STDMETHODCALLTYPE HookCreateDevice(IDirect3D8 *self, UINT adapter, D3DDEVTYPE type, HWND window, DWORD behavior, D3DPRESENT_PARAMETERS *parameters, IDirect3DDevice8 **device) {
    const HRESULT result = realCreateDevice(self, adapter, type, window, behavior, parameters, device);
    if (SUCCEEDED(result) && device && *device && !realPresent) Patch((*reinterpret_cast<void ***>(*device)) + 15, HookPresent, realPresent);
    return result;
}
}

extern "C" __declspec(dllexport) IDirect3D8 *WINAPI Direct3DCreate8(UINT sdkVersion) {
    EnsureOutput();
    if (!realCreate) {
        wchar_t path[MAX_PATH]; GetFullPathNameW(L"d3d8to9.dll", MAX_PATH, path, nullptr);
        if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) { GetSystemDirectoryW(path, MAX_PATH); lstrcatW(path, L"\\d3d8.dll"); }
        realModule = LoadLibraryW(path); realCreate = reinterpret_cast<Direct3DCreate8Fn>(GetProcAddress(realModule, "Direct3DCreate8"));
    }
    IDirect3D8 *d3d = realCreate ? realCreate(sdkVersion) : nullptr;
    if (d3d && !realCreateDevice) Patch((*reinterpret_cast<void ***>(d3d)) + 15, HookCreateDevice, realCreateDevice);
    return d3d;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
#if TH_GAME == 6
        auto device = *reinterpret_cast<IDirect3DDevice8 **>(0x006C6D18 + 8);
        if (device && !realPresent) Patch((*reinterpret_cast<void ***>(device)) + 15, HookPresent, realPresent);
#endif
    }
    if (reason == DLL_PROCESS_DETACH && output != INVALID_HANDLE_VALUE) CloseHandle(output);
    return TRUE;
}
