#pragma once
#include "../platform/BrowserRuntime.hpp"
namespace th08 {bool sdl_attach(BrowserRuntime*);void sdl_detach();i32 sdl_graphics(u32,u32,u32,u32);}
#ifdef TH_MULTIPLAYER_FIXTURES
namespace th08 {bool fixture_world_instancing(bool);}
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace th08 {bool sdl_rebind(BrowserRuntime*);}
#endif
