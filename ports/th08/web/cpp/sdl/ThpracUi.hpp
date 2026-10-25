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
void cancel_pointer();
bool captures_pointer(float x,float y);
void render(BrowserRuntime&,touhou::sdl::Renderer&);
bool captures_game_input();
// Read-only submitted native panel/widget geometry for shared browser acceptance.
void observation(std::uint32_t* words,std::uint32_t count);
void controls_observation(BrowserRuntime&,double* words,std::uint32_t count);
double simulation_interval(BrowserRuntime&);
}
}
