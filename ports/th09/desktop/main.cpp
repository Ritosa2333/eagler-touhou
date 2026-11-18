#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace th09::sdl {
int run_desktop();
}

namespace {
void configure_graphics_library(const char* hint, const char* filename) {
    if (SDL_GetHint(hint)) return;
    const auto* base = SDL_GetBasePath();
    if (!base) return;
    const std::string path = std::string(base) + filename;
    SDL_PathInfo info{};
    if (SDL_GetPathInfo(path.c_str(), &info) && info.type == SDL_PATHTYPE_FILE) {
        SDL_SetHint(hint, path.c_str());
    }
}
}

int main(int, char**) {
    configure_graphics_library(SDL_HINT_OPENGL_ES_DRIVER, "libGLESv2.dll");
    configure_graphics_library(SDL_HINT_EGL_LIBRARY, "libEGL.dll");
    if (const auto* directory = _wgetenv(L"EAGLER_DESKTOP_GAME_HOME")) {
        std::error_code error;
        std::filesystem::current_path(directory, error);
        if (error) {
            std::fprintf(stderr, "Cannot open the game directory: %s\n", error.message().c_str());
            return 1;
        }
    }
    return th09::sdl::run_desktop();
}
