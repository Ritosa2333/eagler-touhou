#include "GameState.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define STATE_EXPORT(name) extern "C" __attribute__((export_name(name)))
STATE_EXPORT("game_state_create") browser::GameState* game_state_create(browser::Input* input,u32 chinese){auto* bytes=std::malloc(sizeof(browser::GameState));return bytes?new(bytes)browser::GameState(*input,chinese!=0):nullptr;}
STATE_EXPORT("game_state_destroy") void game_state_destroy(browser::GameState* state){if(state){state->~GameState();std::free(state);}}
STATE_EXPORT("game_state_economy") GameEconomy* game_state_economy(browser::GameState* state){return &state->game;}
STATE_EXPORT("game_state_configuration") ApplicationConfig* game_state_configuration(browser::GameState* state){return &state->configuration;}
STATE_EXPORT("game_state_pending") i32 game_state_pending(browser::GameState* state){return state->pending_screen;}
