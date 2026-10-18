#pragma once
#include <SDL3/SDL.h>
namespace th11 {class GameSession;}
namespace touhou::sdl {class Renderer;}
namespace th11::browser::ThpracUi {
bool initialize();void shutdown();void process_event(const SDL_Event&);
void update_input(GameSession&,const bool*);bool captures_game_input();
void apply_input(GameSession&,bool*);double simulation_interval(GameSession&);
bool captures_pointer(float,float);
void render(GameSession&,touhou::sdl::Renderer&);void mouse(int,float,float);void cancel_pointer();
}
