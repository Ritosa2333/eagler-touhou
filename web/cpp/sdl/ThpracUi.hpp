#pragma once
#include <SDL3/SDL.h>
#include <cstdint>

namespace touhou::sdl { class Renderer; }
namespace th10::browser {
struct Application;
namespace ThpracUi {
bool initialize();
void shutdown();
void process_event(const SDL_Event&);
void update_input(Application&);
void mouse(int type,float x,float y);
void render(Application&,touhou::sdl::Renderer&);
bool captures_game_input();
}
}
