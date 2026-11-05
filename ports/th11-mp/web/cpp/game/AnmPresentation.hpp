#pragma once
#include "AnmVm.hpp"
#include "HighRefresh.hpp"
#include <cstdint>
namespace th11::presentation {
// Field classes which may be smoothed. Position ownership is separate from the
// authored ANM attribute set: owners such as BulletManager write
// script_position during Draw, and those endpoints are still published at the
// fixed tick because the previous sample is captured before the tick updates.
enum Field : std::uint32_t {
    Position=1,ScriptPosition=2,ChildPosition=4,Rotation=8,Scale=16,
    Rgb=32,Opacity=64,Rgb2=128,Opacity2=256,Uv=512
};
struct Sample {
    std::uint32_t id=0;
    const AnmResource* file=nullptr;
    i16 script=-1,sprite=-1;
    bool visible=false;
    i32 timer=0;
    Vec3 position{},script_position{},child_position{},rotation{};
    Vec2 scale{1,1},uv{};
    std::uint32_t color=0,secondary_color=0,continuous=0;
};
// A new logical tick invalidates the "first sample of this tick" guard.
void begin_tick() noexcept;
// Capture the pre-update endpoint once per VM per tick. Called from
// AnmVm::update so embedded VMs outside AnmManager are covered as well.
void capture(const AnmVm&) noexcept;
// Owners which reposition a VM during Draw (3D stage objects) publish the
// pre-draw endpoint here when the ANM update did not run this tick.
void capture_if_needed(const AnmVm&) noexcept;
// Force the endpoint to the current state (newly spawned / teleported object).
void snap(const AnmVm&) noexcept;
// Draw-only field interpolation into the destination copy. Never writes the
// authoritative current VM.
bool present(AnmVm& destination,const AnmVm& current) noexcept;
// Blended absolute draw position for attached visuals (hitbox markers, text)
// which must follow the subject's final presentation instead of its
// authoritative endpoint.
bool present_position(const AnmVm& current,Vec3& out) noexcept;
void reset() noexcept;
}
