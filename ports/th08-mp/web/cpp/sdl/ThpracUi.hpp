#pragma once
#include <SDL3/SDL.h>
#include <cstdint>

namespace touhou::sdl { class Renderer; }
namespace th08 {
class BrowserRuntime;
namespace ThpracUi {
bool initialize();
void shutdown();
void process_event(const SDL_Event&);
void update_input(BrowserRuntime&);
void mouse(int type,float x,float y);
void render(BrowserRuntime&,touhou::sdl::Renderer&);
bool captures_game_input();
}
}
