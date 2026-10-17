#pragma once
#include "ScreenViews.hpp"
#include "FrameScheduler.hpp"
#include <functional>
namespace th15 {
struct ScreenSurface {const AnmResource* resource=nullptr;u32 texture=0;};
// Screen captures remain ordinary ANM resources; the backend only receives
// named target/depth/viewport operations and CPU-generated animation vertices.
class ScreenCompositor {
 struct Callback {FrameCallback frame;ScreenCompositor* owner=nullptr;u32 pass=0;};
 FrameScheduler& scheduler;AnmManager& animations;AnmEnvironment& environment;AnmRenderer& renderer;ZunGraphics& graphics;ScreenViews& views;
 std::array<Callback,10> callbacks;
 bool fail(const char*);bool select(const ScreenSurface&);bool compose(u32,bool camera,bool layer);bool run(u32);
public:
 static constexpr std::array<i32,10> priorities{1,14,15,24,25,41,42,54,55,82};
 ScreenSurface main,alternate;std::array<AnmVm*,4> captures{};bool active=false;
 u32 clear_color=0xff000000;Vec2 playfield_size{384,448};std::array<std::function<bool()>,2> replacement;
 std::string error;
 ScreenCompositor(FrameScheduler&,AnmManager&,AnmEnvironment&,AnmRenderer&,ZunGraphics&,ScreenViews&);
 ~ScreenCompositor();
 bool pass(u32 index){return index<callbacks.size()&&run(index);}
};
}
