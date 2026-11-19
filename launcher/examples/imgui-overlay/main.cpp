#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <eagler/thprac/PracticePointer.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "OverlayPanel.hpp"

namespace {
using namespace th09::overlay;
class Renderer {
public:
    explicit Renderer(SDL_Renderer* renderer) : backend(renderer) {}
    ~Renderer() { SDL_DestroyTexture(atlas); }
    bool upload(ImGuiIO& io) {
        unsigned char* pixels = nullptr;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        if (!pixels || width <= 0 || height <= 0) return false;
        atlas = SDL_CreateTexture(backend, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
        if (!atlas || !SDL_UpdateTexture(atlas, nullptr, pixels, width * 4)) return false;
        SDL_SetTextureBlendMode(atlas, SDL_BLENDMODE_BLEND);
        io.Fonts->SetTexID(atlas);
        return true;
    }
    bool draw(ImDrawData* data) {
        for (int i = 0; i < data->CmdListsCount; ++i) {
            const auto& list = *data->CmdLists[i];
            colors.resize(list.VtxBuffer.Size);
            for (int v = 0; v < list.VtxBuffer.Size; ++v) {
                const auto color = list.VtxBuffer[v].col;
                colors[v] = {channel(color, IM_COL32_R_SHIFT), channel(color, IM_COL32_G_SHIFT),
                             channel(color, IM_COL32_B_SHIFT), channel(color, IM_COL32_A_SHIFT)};
            }
            for (const auto& command : list.CmdBuffer) {
                if (command.UserCallback) {
                    if (command.UserCallback != ImDrawCallback_ResetRenderState) command.UserCallback(&list, &command);
                    continue;
                }
                const auto clip = command.ClipRect;
                const SDL_Rect rect{static_cast<int>(clip.x), static_cast<int>(clip.y),
                    static_cast<int>(clip.z - clip.x), static_cast<int>(clip.w - clip.y)};
                if (rect.w <= 0 || rect.h <= 0 || !command.ElemCount) continue;
                SDL_SetRenderClipRect(backend, &rect);
                const auto* vertices = list.VtxBuffer.Data + command.VtxOffset;
                if (!SDL_RenderGeometryRaw(backend, static_cast<SDL_Texture*>(command.TextureId),
                    &vertices->pos.x, sizeof(ImDrawVert), colors.data() + command.VtxOffset, sizeof(SDL_FColor),
                    &vertices->uv.x, sizeof(ImDrawVert), list.VtxBuffer.Size - command.VtxOffset,
                    list.IdxBuffer.Data + command.IdxOffset, command.ElemCount, sizeof(ImDrawIdx))) return false;
            }
        }
        return SDL_SetRenderClipRect(backend, nullptr);
    }
private:
    SDL_Renderer* backend;
    SDL_Texture* atlas = nullptr;
    std::vector<SDL_FColor> colors;
    static float channel(ImU32 color, int shift) { return float((color >> shift) & 255) / 255; }
};

struct Pipe {
    std::mutex mutex;
    Json pending;
};

void paintBackground(SDL_Renderer* backend, SDL_Texture* image, float w, float h, bool game) {
    SDL_SetRenderDrawColor(backend, 22, 20, 28, 255);
    SDL_RenderClear(backend);
    if (!image) return;
    float iw, ih;
    SDL_GetTextureSize(image, &iw, &ih);
    const float scale = std::max(w / iw, h / ih);
    const SDL_FRect rect{(w - iw * scale) / 2, (h - ih * scale) / 2, iw * scale, ih * scale};
    const auto intensity = game ? 210 : 143;
    SDL_SetTextureColorMod(image, intensity, intensity, intensity);
    SDL_RenderTexture(backend, image, nullptr, &rect);
    SDL_SetTextureColorMod(image, 255, 255, 255);
}

int run(int argc, char** argv) {
    const bool interactive = argc == 2 && std::string(argv[1]) == "--interactive";
    if (!interactive && argc < 3) {
        std::fprintf(stderr, "Usage: th09-imgui-overlay.exe --interactive | <scene.json> <out.bmp> [width] [height]\n");
        return 1;
    }
    const auto base = std::filesystem::path(SDL_GetBasePath());
    const int initialWidth = argc >= 4 ? std::stoi(argv[3]) : 960;
    const int initialHeight = argc >= 5 ? std::stoi(argv[4]) : 600;
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
    if (interactive) SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition");
    if (!SDL_Init(interactive ? SDL_INIT_VIDEO : 0)) return 2;
    auto* window = interactive ? SDL_CreateWindow("th09 | ImGui Overlay", initialWidth, initialHeight, SDL_WINDOW_RESIZABLE) : nullptr;
    auto* surface = interactive ? nullptr : SDL_CreateSurface(initialWidth, initialHeight, SDL_PIXELFORMAT_RGBA32);
    auto* backend = window ? SDL_CreateRenderer(window, nullptr) : surface ? SDL_CreateSoftwareRenderer(surface) : nullptr;
    if (!backend) return 3;
    if (window) {
        SDL_SetWindowMinimumSize(window, 410, 320);
        SDL_SetRenderVSync(backend, 1);
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    setupKeys(io);
    theme();
    const auto* windir = SDL_getenv("WINDIR");
    auto font = base / "ChillRoundGothic_Medium.ttf";
    if (!std::filesystem::exists(font))
        font = std::filesystem::path(windir ? windir : "C:/Windows") / "Fonts" / "msyh.ttc";
    if (!std::filesystem::exists(font)) return 4;
    ImFontConfig config;
    config.OversampleH = 1;
    config.OversampleV = 1;
    if (!io.Fonts->AddFontFromFileTTF(font.string().c_str(), 17.0f, &config, io.Fonts->GetGlyphRangesChineseFull()))
        return 4;
    const auto loadTexture = [backend, &base](const char* file) -> SDL_Texture* {
        const auto path = (base / file).string();
        if (!std::filesystem::exists(path)) return nullptr;
        auto* pixels = SDL_LoadBMP(path.c_str());
        if (!pixels) return nullptr;
        auto* texture = SDL_CreateTextureFromSurface(backend, pixels);
        SDL_DestroySurface(pixels);
        return texture;
    };
    auto* cover = loadTexture("th09-cover.bmp");
    if (!cover) return 5;
    auto* gameplay = loadTexture("game-preview.bmp");
    Overlay overlay;
    overlay.cover = cover;
    overlay.dispatch = [](const Json& value) { std::cout << value.dump() << std::endl; };
    auto pipe = std::make_shared<Pipe>();
    if (interactive) {
        overlay.update({{"scene", "lobby"}, {"status", "正在连接"}});
        std::thread([pipe] {
            for (std::string line; std::getline(std::cin, line);) {
                auto next = Json::parse(line, nullptr, false);
                if (!next.is_object()) continue;
                std::scoped_lock lock(pipe->mutex);
                pipe->pending = std::move(next);
            }
        }).detach();
        SDL_StartTextInput(window);
        SDL_StopTextInput(window);
    } else {
        std::ifstream fixture(argv[1]);
        if (!fixture) return 6;
        overlay.update(Json::parse(fixture), true);
    }
    int status = 0;
    {
        Renderer renderer(backend);
        if (!renderer.upload(io)) return 7;
        bool active = true;
        int previewFrames = 2;
        Uint64 prior = SDL_GetTicksNS();
        while (active) {
            SDL_Event e;
            if (interactive) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_EVENT_QUIT) active = false;
                    overlay.event(e, window);
                }
                std::scoped_lock lock(pipe->mutex);
                if (pipe->pending.is_object()) {
                    overlay.update(std::move(pipe->pending));
                    pipe->pending = nullptr;
                }
            }
            int w = initialWidth, h = initialHeight;
            if (window) SDL_GetCurrentRenderOutputSize(backend, &w, &h);
            io.DisplaySize = {static_cast<float>(w), static_cast<float>(h)};
            const auto now = SDL_GetTicksNS();
            io.DeltaTime = std::clamp(float(now - prior) / 1.0e9f, .001f, .1f);
            prior = now;
            paintBackground(backend, gameplay ? gameplay : cover, io.DisplaySize.x, io.DisplaySize.y,
                            stringField(overlay.state, "scene") == "game");
            overlay.setGameplay(stringField(overlay.state, "scene") == "game");
            overlay.beginFrame();
            overlay.render(io.DisplaySize.x, io.DisplaySize.y);
            ImGui::Render();
            if (!renderer.draw(ImGui::GetDrawData())) { status = 8; break; }
            if (interactive) {
                if (overlay.editing() && (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)) {
                    const auto rect = overlay.textArea();
                    if (!SDL_TextInputActive(window)) SDL_StartTextInput(window);
                    if (rect.w > 0) {
                        const auto caret = ImGui::GetCurrentContext()->PlatformImePos;
                        SDL_SetTextInputArea(window, &rect, std::clamp(static_cast<int>(caret.x) - rect.x, 0, rect.w));
                    }
                } else if (SDL_TextInputActive(window)) SDL_StopTextInput(window);
                SDL_RenderPresent(backend);
                SDL_Delay(2);
            } else if (--previewFrames == 0) {
                SDL_RenderPresent(backend);
                if (!SDL_SaveBMP(surface, argv[2])) status = 9;
                active = false;
            }
        }
    }
    SDL_DestroyTexture(gameplay);
    SDL_DestroyTexture(cover);
    ImGui::DestroyContext();
    SDL_DestroyRenderer(backend);
    if (surface) SDL_DestroySurface(surface);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return status;
}
}

int main(int argc, char** argv) {
    SDL_SetMainReady();
    try {
        const int status = run(argc, argv);
        if (status) std::fprintf(stderr, "Prototype error %d: %s\n", status, SDL_GetError());
        return status;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 10;
    }
}
