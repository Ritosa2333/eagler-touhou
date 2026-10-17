#pragma once
#include <SDL3/SDL.h>
namespace th15::sdl {class ApplicationState;
namespace ThpracUi {
bool initialize(ApplicationState&);void shutdown();void process_event(const SDL_Event&,float scale);
void reset_input();
void update_input(ApplicationState&,const bool*);void render(ApplicationState&);
bool captures_game_input();bool captures_pointer(float,float);
void mouse(int,float,float);void cancel_pointer();
}}
