#include "ImGuiOverlay.hpp"
#include "OverlayPanel.hpp"
#include "RoomAudio.hpp"
#define IMGUI_IMPL_OPENGL_ES3
#include "imgui_impl_opengl3.h"

#include <GLES3/gl3.h>
#include <windows.h>
#include <wincrypt.h>
#include "src/webp/decode.h"
#include "../portable/sdl/third_party/stb_image.h"

#include <optional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <limits>
#include <utility>
#include <vector>
#include <unordered_map>

namespace Th09Overlay {
namespace {
using th09::overlay::Overlay;
using Json = nlohmann::json;
SDL_Window* owner = nullptr;
ImGuiContext* context = nullptr;
std::future<std::unique_ptr<ImFontAtlas>> pendingFontAtlas;
std::unique_ptr<ImFontAtlas> fontAtlas;
std::unique_ptr<Overlay> panel;
GLuint cover = 0;
std::unordered_map<std::string, GLuint> avatars;
RoomPhase roomPhase = RoomPhase::idle;
unsigned progressCompleted = 0;
unsigned progressTotal = 0;

bool gameOwnsInput(RoomPhase phase) {
    return phase == RoomPhase::selection || phase == RoomPhase::playing ||
           phase == RoomPhase::spectating || phase == RoomPhase::results;
}
bool returnRequested = false;
bool escapeRequested = false;
bool enabled = false;
bool coverAttempted = false;
bool wasDrawn = false;
bool cursorWasVisible = true;
Uint64 lastFrame = 0;
int viewX = 0, viewY = 0;
float viewScale = 0;
std::optional<Json> handoff;

class Lobby {
public:
    ~Lobby() {
        if (!process) return;
        stopping = true;
        if (worker.joinable()) worker.join();
        if (auto* input = SDL_GetProcessInput(process)) {
            static constexpr char shutdown[] = "{\"type\":\"shutdown\"}\n";
            SDL_WriteIO(input, shutdown, sizeof(shutdown) - 1);
        }
        const auto deadline = SDL_GetTicks() + 300;
        bool exited = SDL_WaitProcess(process, false, nullptr);
        while (!exited && SDL_GetTicks() < deadline) {
            SDL_Delay(5);
            exited = SDL_WaitProcess(process, false, nullptr);
        }
        if (!exited) {
            SDL_KillProcess(process, true);
            SDL_WaitProcess(process, true, nullptr);
        }
        SDL_DestroyProcess(process);
    }

    bool start(const char* site) {
        const char* base = SDL_GetBasePath();
        if (!base) return false;
        const auto executable = std::string(base) + "th09-lobby.exe";
        const char* argv[] = {executable.c_str(), "--stdio", "--site", site, nullptr};
        const auto props = SDL_CreateProperties();
        if (!props) return false;
        SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, argv);
        SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
        SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
        SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
        process = SDL_CreateProcessWithProperties(props);
        SDL_DestroyProperties(props);
        if (!process) return false;
        worker = std::thread([this] { run(); });
        return true;
    }

    void submit(const Json& message) {
        std::scoped_lock lock(mutex);
        if (outbound.size() < 48) outbound.push_back(message);
    }

    Json snapshot() {
        std::scoped_lock lock(mutex);
        return std::exchange(inbound, nullptr);
    }
    bool musicAudible() const { return audible; }

private:
    SDL_Process* process = nullptr;
    std::atomic_bool stopping = false;
    std::atomic_bool audible = false;
    std::thread worker;
    std::mutex mutex;
    Json inbound;
    std::vector<Json> outbound;

    void run() {
        th09::desktop::RoomAudio audio;
        auto* input = SDL_GetProcessInput(process);
        auto* output = SDL_GetProcessOutput(process);
        std::string sending, received;
        size_t offset = 0;
        char bytes[8192];
        while (!stopping && input && output) {
            if (auto event = audio.poll(); event.is_object()) {
                event["event"] = event["type"];
                event["type"] = "room-audio";
                submit(event);
            }
            if (offset == sending.size()) {
                std::vector<Json> pending;
                {
                    std::scoped_lock lock(mutex);
                    pending.swap(outbound);
                }
                sending.clear();
                for (const auto& item : pending) sending += item.dump() + '\n';
                offset = 0;
            }
            if (offset < sending.size()) {
                offset += SDL_WriteIO(input, sending.data() + offset, sending.size() - offset);
                const auto status = SDL_GetIOStatus(input);
                if (status != SDL_IO_STATUS_READY && status != SDL_IO_STATUS_NOT_READY) break;
            }
            if (const auto size = SDL_ReadIO(output, bytes, sizeof(bytes))) {
                received.append(bytes, size);
                if (received.size() > 2 * 1024 * 1024) break;
                size_t position = 0;
                for (size_t end; (end = received.find('\n', position)) != std::string::npos; position = end + 1) {
                    auto message = Json::parse(received.begin() + position, received.begin() + end, nullptr, false);
                    if (message.is_object() && message.value("type", "") == "room-audio") {
                        audio.command(message);
                    } else if (message.is_object() && message.value("type", "") == "room-music-audible") {
                        audible = message.value("audible", false);
                    } else if (message.is_object()) {
                        std::scoped_lock lock(mutex);
                        inbound = std::move(message);
                    }
                }
                received.erase(0, position);
            } else {
                const auto status = SDL_GetIOStatus(output);
                if (status != SDL_IO_STATUS_READY && status != SDL_IO_STATUS_NOT_READY) break;
            }
            SDL_Delay(8);
        }
        audible = false;
        std::scoped_lock lock(mutex);
        inbound = {{"status", "联机服务已退出"}, {"scene", "lobby"}};
    }
};
std::unique_ptr<Lobby> lobby;

bool loadCover() {
    const char* base = SDL_GetBasePath();
    if (!base) return false;
    auto* art = SDL_LoadBMP((std::string(base) + "th09-cover.bmp").c_str());
    if (!art) return false;
    auto* rgba = SDL_ConvertSurface(art, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(art);
    if (!rgba) return false;
    GLint oldTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
    glGenTextures(1, &cover);
    glBindTexture(GL_TEXTURE_2D, cover);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgba->w, rgba->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels);
    glBindTexture(GL_TEXTURE_2D, oldTexture);
    SDL_DestroySurface(rgba);
    return true;
}

void dispatch(const Json& value) {
    if (value.value("type", std::string{}) == "return-room") {
        returnRequested = true;
        return;
    }
    if (value.value("type", "") == "open-profile") {
        const auto username = th09::overlay::stringField(value, "username");
        const auto* site = SDL_getenv("EAGLER_DESKTOP_OVERLAY_SITE");
        if (!site || username.empty() || username.size() > 32 || username == "." || username == ".." ||
            username.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789_.-") != std::string::npos) return;
        const std::string page(site);
        const auto separator = page.find("://");
        if (separator == std::string::npos) return;
        const auto origin = page.substr(0, page.find('/', separator + 3));
        if (origin == "https://touhouonline.com" || origin == "https://test.touhouonline.com")
            SDL_OpenURL((origin + "/u/" + username).c_str());
        return;
    }
    if (lobby) lobby->submit(value);
}

GLuint uploadAvatar(const Json& avatar) {
    const auto data = th09::overlay::stringField(avatar, "data");
    const auto type = th09::overlay::stringField(avatar, "type");
    if (data.empty() || data.size() > 10924) return 0;
    std::array<std::uint8_t, 8192> bytes;
    DWORD size = static_cast<DWORD>(bytes.size());
    if (!CryptStringToBinaryA(data.data(), static_cast<DWORD>(data.size()), CRYPT_STRING_BASE64,
                              bytes.data(), &size, nullptr, nullptr)) return 0;
    int width = 0, height = 0;
    const auto validSize = [&] { return width > 0 && width == height && width <= 64; };
    std::array<std::uint8_t, 64 * 64 * 4> rgba;
    if (type == "image/webp") {
        if (!WebPGetInfo(bytes.data(), size, &width, &height) || !validSize() ||
            !WebPDecodeRGBAInto(bytes.data(), size, rgba.data(), rgba.size(), width * 4)) return 0;
    } else if (type == "image/jpeg") {
        int channels = 0;
        if (!stbi_info_from_memory(bytes.data(), size, &width, &height, &channels) || !validSize()) return 0;
        std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels(
            stbi_load_from_memory(bytes.data(), size, &width, &height, &channels, 4), stbi_image_free);
        if (!pixels || !validSize()) return 0;
        std::copy_n(pixels.get(), width * height * 4, rgba.data());
    } else return 0;
    GLint prior = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prior);
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prior));
    return texture;
}

void syncAvatars(const Json& state) {
    const auto found = state.find("avatars");
    for (auto item = avatars.begin(); item != avatars.end();) {
        if (found != state.end() && found->is_object() && found->contains(item->first)) {
            ++item;
        } else {
            glDeleteTextures(1, &item->second);
            item = avatars.erase(item);
        }
    }
    if (found == state.end() || !found->is_object() || found->size() > 2) return;
    for (const auto& asset : found->items())
        if (!avatars.count(asset.key())) avatars.emplace(asset.key(), uploadAvatar(asset.value()));
}

std::unique_ptr<ImFontAtlas> buildFontAtlas(const std::string& path) {
    auto atlas = std::make_unique<ImFontAtlas>();
    ImFontConfig config;
    config.OversampleH = config.OversampleV = 1;
    atlas->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
    size_t fontSize = 0;
    void* fontData = SDL_LoadFile(path.c_str(), &fontSize);
    if (fontData && fontSize > 0 && fontSize <= std::numeric_limits<int>::max()) {
        config.FontDataOwnedByAtlas = false;
        atlas->AddFontFromMemoryTTF(fontData, static_cast<int>(fontSize), 17, &config, atlas->GetGlyphRangesChineseFull());
    }
    SDL_free(fontData);
    unsigned char* pixels = nullptr;
    atlas->GetTexDataAsRGBA32(&pixels, nullptr, nullptr);
    if (!pixels) {
        atlas->Clear();
        atlas->GetTexDataAsRGBA32(&pixels, nullptr, nullptr);
    }
    return atlas;
}

std::string fontPath() {
    const char* base = SDL_GetBasePath();
    return base ? std::string(base) + "ChillRoundGothic_Medium.ttf" : std::string{};
}
}

bool Open(SDL_Window* window) {
    owner = window;
    enabled = SDL_getenv("EAGLER_DESKTOP_OVERLAY_SITE") != nullptr;
    if (enabled) {
        // Build an independent atlas while the title loads. Join before creating the ImGui context.
        try {
            pendingFontAtlas = std::async(std::launch::async, buildFontAtlas, fontPath());
        } catch (const std::system_error&) {
            // If the worker cannot start, build the atlas when the panel is requested.
        }
    }
    return true;
}

void Close() {
    if (panel && panel->visible && !cursorWasVisible) SDL_HideCursor();
    lobby.reset();
    if (context) {
        ImGui::SetCurrentContext(context);
        glDeleteTextures(1, &cover);
        for (const auto& entry : avatars) glDeleteTextures(1, &entry.second);
        avatars.clear();
        ImGui_ImplOpenGL3_Shutdown();
        panel.reset();
        ImGui::DestroyContext(context);
    }
    pendingFontAtlas = {};
    fontAtlas.reset();
    owner = nullptr;
    context = nullptr;
    roomPhase = RoomPhase::idle;
    progressCompleted = progressTotal = 0;
    cursorWasVisible = true;
    cover = 0;
    coverAttempted = false;
    lastFrame = 0;
    returnRequested = false;
    escapeRequested = false;
    handoff.reset();
    wasDrawn = false;
}

bool NeedsPresentation() { return panel && (panel->visible || panel->hasWaitNotice() || wasDrawn); }

bool CapturesKeyboard() { return panel && panel->capturesKeyboard(); }
bool RoomMusicAudible() { return lobby && lobby->musicAudible(); }

void AdoptRoom(const char* options) {
    if (!enabled || !options) return;
    auto config = Json::parse(options, nullptr, false);
    if (!config.is_object() || !config.contains("lobbyClientId")) return;
    handoff = Json{{"type", "adopt-room"}, {"options", std::move(config)}};
    if (lobby) {
        lobby->submit(*handoff);
        handoff.reset();
    }
}

void ReportMatch(std::uint32_t start, std::uint32_t frames, unsigned winner, unsigned first, unsigned second) {
    if (lobby) lobby->submit({{"type", "match-result"}, {"start", start}, {"frames", frames},
        {"winner", winner}, {"characters", {first, second}}});
}

static bool InitPanel() {
    if (panel) return true;
    if (!fontAtlas) fontAtlas = pendingFontAtlas.valid() ? pendingFontAtlas.get() : buildFontAtlas(fontPath());
    context = ImGui::CreateContext(fontAtlas.get());
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    th09::overlay::setupKeys(io);
    th09::overlay::theme();
    if (!ImGui_ImplOpenGL3_Init("#version 300 es")) {
        ImGui::DestroyContext(context);
        context = nullptr;
        return false;
    }
    panel = std::make_unique<Overlay>();
    panel->dispatch = dispatch;
    panel->avatarFor = [](const std::string& asset) -> ImTextureID {
        const auto found = avatars.find(asset);
        return found == avatars.end() ? nullptr : reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(found->second));
    };
    panel->visible = false;
    panel->setGameplay(gameOwnsInput(roomPhase));
    panel->update({{"scene", "lobby"}, {"status", "正在连接"}});
    lobby = std::make_unique<Lobby>();
    if (!lobby->start(SDL_getenv("EAGLER_DESKTOP_OVERLAY_SITE")))
        panel->update({{"scene", "lobby"}, {"status", "无法启动联机服务"}});
    else if (handoff) {
        lobby->submit(*handoff);
        handoff.reset();
    }
    return true;
}

bool ShowLobby() {
    if (!owner || !enabled || !InitPanel()) return false;
    if (!panel->visible) {
        cursorWasVisible = SDL_CursorVisible();
        SDL_ShowCursor();
        panel->visible = true;
    }
    return true;
}

void SetRoomPhase(RoomPhase phase, unsigned completed, unsigned total) {
    if (phase == roomPhase && completed == progressCompleted && total == progressTotal) return;
    const bool wasInGame = gameOwnsInput(roomPhase);
    roomPhase = phase;
    progressCompleted = completed;
    progressTotal = total;
    if (phase == RoomPhase::connecting || phase == RoomPhase::loadingSpectator) ShowLobby();
    if (!panel) return;
    const bool inGame = gameOwnsInput(phase);
    panel->setGameplay(inGame);
    if (inGame && !wasInGame && panel->visible) {
        panel->hide();
        if (!cursorWasVisible) SDL_HideCursor();
        SDL_StopTextInput(owner);
    }
    const char* label = phase == RoomPhase::connecting ? "正在连接其他玩家…" :
        phase == RoomPhase::synchronizing ? (total ? "正在测量延迟并同步对局…" : "正在同步对局…") :
        phase == RoomPhase::loadingSpectator ? "正在等待旁观数据…" : "";
    panel->setRuntimeProgress(label, completed, total);
}

void SetTransportRecovering(bool recovering) {
    if (panel) panel->reconnecting = recovering;
}

void SetEscapeAvailable(bool available) {
    if (!panel || panel->escapeAvailable == available) return;
    panel->escapeAvailable = available;
    if (available && roomPhase != RoomPhase::idle) SDL_Log("Peer unresponsive: Esc can return to title");
}

bool TakeEscapeRequest() { return std::exchange(escapeRequested, false); }

void FinishRoom(const char* error, bool showRoom) {
    SetTransportRecovering(false);
    SetRoomPhase(RoomPhase::idle);
    if (showRoom) {
        if (!ShowLobby()) return;
        if (error && *error) panel->setRoomIssue(error);
        SetEscapeAvailable(error && *error);
    } else if (panel) {
        panel->hide();
        if (!cursorWasVisible) SDL_HideCursor();
        SDL_StopTextInput(owner);
        SetEscapeAvailable(false);
    }
    if (lobby) {
        if (showRoom) lobby->submit({{"type", "runtime-return"}, {"message", error ? error : ""}});
        else lobby->submit({{"type", "leave"}});
    }
}

bool HandleEvent(const SDL_Event& event) {
    if (!owner || !enabled) return false;
    if (panel && panel->escapeAvailable && event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
        if (!event.key.repeat) escapeRequested = true;
        return true;
    }
    if (!panel && (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)) {
        const bool f2 = event.key.scancode == SDL_SCANCODE_F2;
        const bool chat = gameOwnsInput(roomPhase) && event.key.scancode == SDL_SCANCODE_LALT;
        if (!f2 && !chat) return false;
        const bool start = !event.key.repeat && ((f2 && event.type == SDL_EVENT_KEY_DOWN) ||
            (chat && event.type == SDL_EVENT_KEY_UP && !(event.key.mod & (SDL_KMOD_CTRL | SDL_KMOD_SHIFT | SDL_KMOD_GUI))));
        if (!start || !InitPanel()) return true;
    }
    if (!panel) return false;
    auto translated = event;
    int windowW, windowH, pixelW, pixelH;
    const bool pointing = event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_WHEEL;
    if (pointing && panel->visible && viewScale > 0 && SDL_GetWindowSize(owner, &windowW, &windowH) &&
        SDL_GetWindowSizeInPixels(owner, &pixelW, &pixelH) && windowW > 0 && windowH > 0) {
        const auto translate = [&](float& x, float& y) {
            x = (x * pixelW / windowW - viewX) / viewScale;
            y = (y * pixelH / windowH - viewY) / viewScale;
        };
        if (event.type == SDL_EVENT_MOUSE_MOTION) translate(translated.motion.x, translated.motion.y);
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            translate(translated.button.x, translated.button.y);
        if (event.type == SDL_EVENT_MOUSE_WHEEL) translate(translated.wheel.mouse_x, translated.wheel.mouse_y);
    }
    const bool wasVisible = panel->visible;
    const bool consumed = panel->event(translated, owner);
    if (panel->visible != wasVisible) {
        if (panel->visible) {
            cursorWasVisible = SDL_CursorVisible();
            SDL_ShowCursor();
        } else {
            if (!cursorWasVisible) SDL_HideCursor();
            SDL_StopTextInput(owner);
        }
    }
    return consumed;
}

bool TakeReturnRequest() { return std::exchange(returnRequested, false); }

void Composite(int x, int y, int width, int height) {
    wasDrawn = panel && (panel->visible || panel->hasWaitNotice()) && width > 0 && height > 0;
    if (!wasDrawn) return;
    viewX = x;
    viewY = y;
    viewScale = std::max(1.f, std::min(width / 640.f, height / 480.f));
    if (lobby) {
        auto next = lobby->snapshot();
        if (next.is_object()) {
            syncAvatars(next);
            next.erase("avatars");
            panel->update(std::move(next));
        }
    }
    auto& io = ImGui::GetIO();
    io.DisplaySize = {width / viewScale, height / viewScale};
    io.DisplayFramebufferScale = {viewScale, viewScale};
    const auto now = SDL_GetTicksNS();
    io.DeltaTime = lastFrame ? std::clamp(float(now - lastFrame) / 1e9f, .001f, .1f) : 1 / 60.f;
    lastFrame = now;
    if (!coverAttempted) {
        coverAttempted = true;
        loadCover();
    }
    panel->cover = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(cover));
    ImGui_ImplOpenGL3_NewFrame();
    panel->beginFrame();
    panel->render(io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Render();
    if (!panel->visible && !cursorWasVisible) SDL_HideCursor();

    // Shift the projection and scissor together for a letterboxed game surface.
    auto* drawData = ImGui::GetDrawData();
    int pixelW = 0, pixelH = 0;
    if (SDL_GetWindowSizeInPixels(owner, &pixelW, &pixelH)) {
        const auto scale = io.DisplayFramebufferScale;
        drawData->DisplayPos = {-x / scale.x, -y / scale.y};
        drawData->DisplaySize = {pixelW / scale.x, pixelH / scale.y};
    }
    GLboolean colorMask[4];
    glGetBooleanv(GL_COLOR_WRITEMASK, colorMask);
    const auto stencilEnabled = glIsEnabled(GL_STENCIL_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (stencilEnabled) glDisable(GL_STENCIL_TEST);
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
    if (stencilEnabled) glEnable(GL_STENCIL_TEST);
    glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
    if (panel->editing() && (SDL_GetWindowFlags(owner) & SDL_WINDOW_INPUT_FOCUS)) {
        if (!SDL_TextInputActive(owner)) SDL_StartTextInput(owner);
        auto rect = panel->textArea();
        int windowW, windowH;
        if (rect.w && SDL_GetWindowSize(owner, &windowW, &windowH) &&
            SDL_GetWindowSizeInPixels(owner, &pixelW, &pixelH) && pixelW > 0 && pixelH > 0) {
            const auto caret = ImGui::GetCurrentContext()->PlatformImePos;
            const int cursor = std::clamp(static_cast<int>(caret.x) - rect.x, 0, rect.w);
            const auto toWindowX = [&](float px) { return static_cast<int>(px * windowW / pixelW); };
            const auto toWindowY = [&](float px) { return static_cast<int>(px * windowH / pixelH); };
            rect.x = toWindowX(x + rect.x * viewScale);
            rect.y = toWindowY(y + rect.y * viewScale);
            rect.w = std::max(1, toWindowX(rect.w * viewScale));
            rect.h = std::max(1, toWindowY(rect.h * viewScale));
            SDL_SetTextInputArea(owner, &rect, toWindowX(cursor * viewScale));
        }
    } else if (SDL_TextInputActive(owner)) {
        SDL_StopTextInput(owner);
    }
}
}
