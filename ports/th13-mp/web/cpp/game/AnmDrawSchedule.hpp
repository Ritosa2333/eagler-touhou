#pragma once
#include "AnmRenderer.hpp"
#include "AnmManager.hpp"
#include "FrameScheduler.hpp"
namespace th13 {
enum class DrawCamera:i32 {Hud=0,Interface=1,Fullscreen=2,Playfield=3};
struct AnmDrawServices {
    virtual ~AnmDrawServices()=default;
    virtual bool camera(DrawCamera,bool refresh)=0;
    // 原生 0x4679a0 的 UP 批只按材质键（纹理+管线态）冲批；相机/视口切换
    // （0x43b900 SetViewport/SetTransform）不冲批。战斗直绘 pass 入口用此
    // 入口避免同值相机重设把同一材质键的跨 pass 精灵批拆开；默认实现保持
    // 旧语义（先冲批再设置），其他实现者无需改动。
    virtual bool camera_if_changed(DrawCamera index,bool refresh){return camera(index,refresh);}
};
// Register transparent animation layers around the game's object/HUD passes.
// Layer order is deliberately not numeric: playfield overlays temporarily
// switch cameras and then restore the full-screen camera.
class AnmDrawSchedule {
 struct Entry {i32 layer,priority,camera=-1;bool refresh=false,disable_fog=false,disable_depth_write=false,zero_offset=false,restore=false,clear_shake=false;};
 static const std::array<Entry,31> entries;
 struct Callback {FrameCallback callback;AnmDrawSchedule* owner=nullptr;u32 index=0;};
 std::array<Callback,31> callbacks;FrameScheduler& scheduler;AnmManager& animations;AnmRenderer& renderer;ZunGraphics& graphics;AnmDrawServices& services;bool draw(u32);
public:
 static void set_hud_ghost_icons(AnmManager&, const std::array<std::array<u32, 21>, 3>&, u8 ghost);
 std::string error;AnmDrawSchedule(FrameScheduler&,AnmManager&,AnmRenderer&,ZunGraphics&,AnmDrawServices&);~AnmDrawSchedule();
 AnmDrawSchedule(const AnmDrawSchedule&)=delete;AnmDrawSchedule& operator=(const AnmDrawSchedule&)=delete;
};
}
