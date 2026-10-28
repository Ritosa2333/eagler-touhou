// STD 舞台背景场景：图元 VM 池（st01wl.anm 脚本）+ 相机脚本驱动 + 实例
// （FACE）遍历绘制（裁剪/雾/深度写）。数据通路对齐 th15 StageScene 的 th13
// 同族裁剪：包含 STD 内嵌动画槽，不包含 th15 专属形变服务。
#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "StageResource.hpp"
#include "StageScript.hpp"
#include "Timer.hpp"
#include <vector>
namespace th13 {
class StageScenery {
public:
    StageResource file;
    StageScript script{file};
    std::string error;
    u32 frames = 0;
    bool ready() const noexcept { return initialized; }
    // 0x404c lifecycle flags: bit1=geometry, bit2=fade-out, bit4=fade-in,
    // bit8=retired. These are per STD instance, not global scene flags.
    void begin_exit() noexcept;
    void begin_enter() noexcept;
    bool retired() const noexcept { return (lifecycle_flags & 8u) != 0; }
    bool draw_camera_enabled() const noexcept;
    bool draw_geometry_enabled() const noexcept { return !retired() && (lifecycle_flags & 1u) != 0; }
    u32 draw_clear_color() const noexcept;
    bool finish_draw_pass(i32 pass) noexcept;
    u32 take_draw_tint() noexcept;
    bool fade_in_requested() const noexcept { return fade_in_pending; }
    void clear_fade_in_request() noexcept { fade_in_pending = false; }
    const StageCamera& camera() const noexcept { return script.state.camera; }
    const Vec3& camera_direction() const noexcept { return view_direction; }
    bool initialize(AnmManager& animations, i32 bank);
    bool update(AnmManager& animations, AnmEnvironment& environment, float rate = 1);
    // layer_first..layer_last 的对象组绘制（背景 0-7 / 前景 8-11 两段调用）；
    // cull_extent = 裁剪矩形尺寸（原点取视口左上）。
    bool draw(AnmManager& animations, AnmRenderer& renderer, ZunGraphics& graphics,
              i32 layer_first, i32 layer_last, const GraphicsViewport& viewport, Vec2 cull_extent,
              Vec2 screen_shake = {});
    bool interrupt_objects(i32 label);
private:
    std::vector<AnmVm> primitives;
    std::array<AnmVm, 8> background_slots{};
    std::vector<u8> object_flags;
    std::vector<i16> instance_flags;
    bool initialized = false;
    u32 lifecycle_flags = 1; // normal scene: geometry enabled
    Timer lifecycle_timer{0, 0, 0, 0, 0};
    bool fade_in_pending = false;
    u32 transition_color = 0x00808080u;
    float rate = 1;
    Vec3 view_direction{};
    bool bind(AnmVm&, AnmManager&, i32 bank, i32 script_index);
    bool fail(const std::string& message) {
        if (error.empty()) error = message;
        return false;
    }
};
}  // namespace th13
