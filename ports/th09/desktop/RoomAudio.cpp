#include "RoomAudio.hpp"
#include <SDL3/SDL_timer.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>

namespace th09::desktop {
namespace {
using Microsoft::WRL::ComPtr;
using Json = nlohmann::json;

class Notifications final : public IMFMediaEngineNotify {
public:
    STDMETHODIMP QueryInterface(REFIID iid, void** result) override {
        if (!result) return E_POINTER;
        *result = nullptr;
        if (iid != IID_IUnknown && iid != __uuidof(IMFMediaEngineNotify)) return E_NOINTERFACE;
        *result = static_cast<IMFMediaEngineNotify*>(this);
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++references_; }
    STDMETHODIMP_(ULONG) Release() override {
        const auto remaining = --references_;
        if (!remaining) delete this;
        return remaining;
    }
    STDMETHODIMP EventNotify(DWORD event, DWORD_PTR, DWORD) override {
        switch (event) {
        case MF_MEDIA_ENGINE_EVENT_CANPLAY: ready = true; break;
        case MF_MEDIA_ENGINE_EVENT_ERROR: failed = true; break;
        case MF_MEDIA_ENGINE_EVENT_ENDED: ended = true; break;
        case MF_MEDIA_ENGINE_EVENT_PLAYING: playing = true; break;
        }
        return S_OK;
    }
    std::atomic_bool ready{}, failed{}, ended{}, playing{};

private:
    std::atomic<ULONG> references_{1};
};
}

struct RoomAudio::Player {
    bool initialized = false;
    bool mediaStarted = false;
    HMODULE mediaLibrary = nullptr;
    decltype(&MFShutdown) shutdownMedia = nullptr;
    decltype(&MFCreateAttributes) createAttributes = nullptr;
    ComPtr<IMFMediaEngine> engine;
    ComPtr<Notifications> notice;
    unsigned generation = 0;
    Uint64 nextPosition = 0;
    bool failed = false;
    bool metadata = false;
    double volume = .5;

    ~Player() {
        clear();
        if (mediaStarted) shutdownMedia();
        if (mediaLibrary) FreeLibrary(mediaLibrary);
        if (initialized) CoUninitialize();
    }

    void clear() {
        if (engine) engine->Shutdown();
        engine.Reset();
        notice.Reset();
        failed = false;
        metadata = false;
        nextPosition = 0;
    }

    bool load(std::string url) {
        if (url.size() > 4096 ||
            (url.rfind("https://", 0) != 0 && url.rfind("http://127.0.0.1:", 0) != 0)) return false;
        if (!initialized) initialized = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
        if (!mediaLibrary && initialized) {
            mediaLibrary = LoadLibraryExW(L"mfplat.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (mediaLibrary) {
                const auto startup = reinterpret_cast<decltype(&MFStartup)>(GetProcAddress(mediaLibrary, "MFStartup"));
                shutdownMedia = reinterpret_cast<decltype(&MFShutdown)>(GetProcAddress(mediaLibrary, "MFShutdown"));
                createAttributes = reinterpret_cast<decltype(&MFCreateAttributes)>(GetProcAddress(mediaLibrary, "MFCreateAttributes"));
                if (startup && shutdownMedia && createAttributes) mediaStarted = SUCCEEDED(startup(MF_VERSION, MFSTARTUP_FULL));
            }
        }
        if (!mediaStarted) return false;
        ComPtr<IMFMediaEngineClassFactory> factory;
        ComPtr<IMFAttributes> attributes;
        notice.Attach(new Notifications);
        if (FAILED(CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))) ||
            FAILED(createAttributes(&attributes, 1)) ||
            FAILED(attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, notice.Get())) ||
            FAILED(factory->CreateInstance(MF_MEDIA_ENGINE_AUDIOONLY, attributes.Get(), &engine))) return false;
        engine->SetVolume(volume);
        const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, url.data(), static_cast<int>(url.size()), nullptr, 0);
        if (!length) return false;
        BSTR source = SysAllocStringLen(nullptr, length);
        if (!source) return false;
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, url.data(), static_cast<int>(url.size()), source, length);
        const auto result = engine->SetSource(source);
        SysFreeString(source);
        return SUCCEEDED(result) && SUCCEEDED(engine->Load());
    }

    Json state(const char* type) const {
        Json result{{"type", type}, {"generation", generation}};
        if (engine) {
            const double position = engine->GetCurrentTime();
            if (std::isfinite(position)) result["position"] = position;
        }
        return result;
    }
};

RoomAudio::RoomAudio() = default;
RoomAudio::~RoomAudio() = default;

void RoomAudio::command(const Json& message) {
    const auto action = message.value("action", "");
    const auto generation = message.value("generation", 0u);
    if (!player_) player_ = std::make_unique<Player>();
    auto& player = *player_;
    if (action == "load" || action == "clear") {
        if (generation < player.generation) return;
        player.clear();
        player.generation = generation;
        if (action == "load") player.failed = !player.load(message.value("value", ""));
        return;
    }
    if (generation != player.generation || !player.engine) return;
    if (action == "volume") {
        player.volume = std::clamp(message.value("value", .5), 0., 1.);
        player.engine->SetVolume(player.volume);
    } else if (action == "seek") {
        player.engine->SetCurrentTime(std::max(0., message.value("value", 0.)));
    } else if (action == "pause") {
        player.engine->Pause();
    } else if (action == "play") {
        player.failed = FAILED(player.engine->Play());
    }
}

Json RoomAudio::poll() {
    if (!player_) return nullptr;
    auto& player = *player_;
    if (player.failed || (player.notice && player.notice->failed.exchange(false))) {
        player.clear();
        return player.state("error");
    }
    if (!player.engine) return nullptr;
    if (player.notice->ready.exchange(false) && !player.metadata) {
        const double length = player.engine->GetDuration();
        if (!player.engine->HasAudio() || !std::isfinite(length) || length <= 0) {
            player.clear();
            return player.state("error");
        }
        auto result = player.state("loadedmetadata");
        result["duration"] = length;
        player.metadata = true;
        return result;
    }
    if (player.notice->ended.exchange(false)) return player.state("ended");
    if (player.notice->playing.exchange(false)) return player.state("playing");
    const auto now = SDL_GetTicks();
    if (!player.engine->IsPaused() && now >= player.nextPosition) {
        player.nextPosition = now + 250;
        return player.state("position");
    }
    return nullptr;
}
}
