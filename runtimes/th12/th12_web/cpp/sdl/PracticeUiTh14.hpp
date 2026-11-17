#pragma once
#include "../game/PortTitle.hpp"
#if PORT_TITLE == 14 && defined(TH14_ENABLE_THPRAC)
#include "../game/Types.hpp"
namespace touhou::sdl {class Renderer;}
namespace th11 {
class GameSession;
namespace sdl {class GraphicsDevice;}
namespace sdl::PracticeUiTh14 {
void configure(GameSession&,bool enabled,int locale);
void mobile(bool enabled,bool visible);
bool key(unsigned vk,bool down);
bool pointer(unsigned type,int id,float x,float y,float* restart_x=nullptr,float* restart_y=nullptr);
void cancel_pointer();
double speed_scale() noexcept;
void sample_keys(u32 held);
void mouse_motion(float x,float y);
bool wheel(float delta);
bool keep_bgm() noexcept;
bool captures() noexcept;
void clear_input();
void close();
void sync_setup();
bool draw(GameSession&,GraphicsDevice&);
}
}
#endif
