#pragma once
#include "AnmRenderer.hpp"
#include "AnmManager.hpp"
#include "FrameScheduler.hpp"
namespace th17 {
enum class DrawCamera:i32 {Hud=0,Interface=1,Fullscreen=2,Playfield=3};
struct AnmDrawServices {virtual ~AnmDrawServices()=default;virtual bool camera(DrawCamera,bool refresh)=0;};
// Ordered transparent layers interleave with the game's named draw owners.
// These priorities belong to TH17, including its additional animal layers.
class AnmDrawSchedule {
    struct Entry {i32 layer,priority,camera=-1;bool refresh=false,disable_fog=false,disable_depth_write=false,zero_offset=false,restore=false;};
    static const std::array<Entry,39> entries;
    struct Callback {FrameCallback frame;AnmDrawSchedule* owner=nullptr;u32 index=0;};
    std::array<Callback,39> callbacks;FrameScheduler& scheduler;AnmManager& animations;AnmRenderer& renderer;ZunGraphics& graphics;AnmDrawServices& services;
    bool draw(u32);
public:
    std::string error;
    AnmDrawSchedule(FrameScheduler&,AnmManager&,AnmRenderer&,ZunGraphics&,AnmDrawServices&);~AnmDrawSchedule();
    AnmDrawSchedule(const AnmDrawSchedule&)=delete;AnmDrawSchedule& operator=(const AnmDrawSchedule&)=delete;
};
}
