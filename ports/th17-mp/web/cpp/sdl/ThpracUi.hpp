#pragma once
#include <SDL3/SDL.h>

namespace th17::sdl {
class ApplicationState;
namespace ThpracUi {
#if TH_ENABLE_THPRAC
bool initialize(ApplicationState&);
void shutdown();
void reset_input();
void process_event(const SDL_Event&,float scale);
void update_input(ApplicationState&,const bool* keys);
void record_keys(const ApplicationState&,Uint32 held);
bool render(ApplicationState&);
void mouse(int type,float x,float y);
void cancel_pointer();
bool captures_pointer(float x,float y);
bool passive_pointer(float x,float y);
bool captures_game_input(const ApplicationState&);
#else
inline bool initialize(ApplicationState&){return true;}
inline void shutdown(){}
inline void reset_input(){}
inline void process_event(const SDL_Event&,float){}
inline void update_input(ApplicationState&,const bool*){}
inline void record_keys(const ApplicationState&,Uint32){}
inline bool render(ApplicationState&){return true;}
inline void mouse(int,float,float){}
inline void cancel_pointer(){}
inline bool captures_pointer(float,float){return false;}
inline bool passive_pointer(float,float){return false;}
inline bool captures_game_input(const ApplicationState&){return false;}
#endif
}
}
